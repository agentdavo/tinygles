#include "zgl.hpp"
#include "zmath.hpp"
#include "specbuf.hpp"
#include "light.hpp"

namespace fp
{

    // Fast inverse square root implementation
    inline tGLfixed fast_inverse_sqrt(tGLfixed number)
    {
        tGLfixed x2 = number * 0.5F;
        tGLfixed y = number;
        long i = *(long *)&y;      // evil floating point bit-level hacking
        i = 0x5f3759df - (i >> 1); // what the heck?
        y = *(tGLfixed *)&i;
        y = y * (1.5F - (x2 * y * y)); // 1st iteration of Newton's method
        return y;
    }

    void glMaterialx(GLenum face, GLenum pname, tGLfixed param)
    {
        tGLfixed v[4] = {param, 0, 0, 0};
        glMaterialxv(face, pname, v);
    }

    void glMaterialxv(GLenum face, GLenum pname, const tGLfixed *v)
    {
        GLContext *c = gl_get_context();
        GLMaterial *m = (face == GL_FRONT) ? &c->material.materials[0] : &c->material.materials[1];

        if (face == GL_FRONT_AND_BACK)
        {
            glMaterialxv(GL_FRONT, pname, v);
            face = GL_BACK;
        }

        switch (pname)
        {
        case GL_EMISSION:
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
            memcpy(&m->emission, v, sizeof(tGLfixed) * 4);
            break;
        case GL_SHININESS:
            m->shininess = v[0];
            m->shininess_i = (v[0] / 128.0f) * SPECULAR_BUFFER_RESOLUTION;
            break;
        case GL_AMBIENT_AND_DIFFUSE:
            memcpy(&m->ambient, v, sizeof(tGLfixed) * 4);
            memcpy(&m->diffuse, v, sizeof(tGLfixed) * 4);
            break;
        default:
            assert(0);
        }
    }

    void glColorMaterial(GLenum face, GLenum mode)
    {
        GLContext *c = gl_get_context();
        c->material.color.current_mode = face;
        c->material.color.current_type = mode;
    }

    void glLightx(GLenum light, GLenum pname, tGLfixed param)
    {
        tGLfixed v[4] = {param, 0, 0, 0};
        glLightxv(light, pname, v);
    }

    void glLightxv(GLenum light, GLenum pname, const tGLfixed *param)
    {
        GLContext *c = gl_get_context();
        GLLight *l = &c->light.lights[light - GL_LIGHT0];

        switch (pname)
        {
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        {
            V4 v = *(V4 *)param;
            memcpy(&l->ambient, &v, sizeof(V4)); // Use V4 for these light parameters
            break;
        }
        case GL_POSITION:
        {
            V4 v = *(V4 *)param;
            gl_M4_MulV4(&l->position, c->matrix.stack_ptr[0], &v); // Handle position as a V4
            if (l->position.W == 0)
            {
                // If W is 0, it's a directional light (light at infinity)
                V4_to_V3(l->norm_position, l->position);
                gl_V3_Norm(&l->norm_position);  // Normalize the direction (treating it as V3)
            }
            break;
        }
        case GL_SPOT_DIRECTION:
        {
            V3 v = *(V3 *)param;                        // Handle spot direction as a V3
            memcpy(&l->spot_direction, &v, sizeof(V3)); // Copy V3 into spot_direction
            l->norm_spot_direction = l->spot_direction; // Normalize the spot direction
            gl_V3_Norm(&l->norm_spot_direction);
            break;
        }
        case GL_SPOT_EXPONENT:
            l->spot_exponent = param[0];
            break;
        case GL_SPOT_CUTOFF:
        {
            tGLfixed angle = param[0];
            assert(angle == 180 || (angle >= 0 && angle <= 90)); // Spot cutoff must be in range
            l->spot_cutoff = angle;
            if (angle != 180)
            {
                l->cos_spot_cutoff = cos(angle * GLfixed_PI / 180.0); // Precompute cosine of the cutoff
            }
            break;
        }
        case GL_CONSTANT_ATTENUATION:
            l->attenuation[0] = param[0];
            break;
        case GL_LINEAR_ATTENUATION:
            l->attenuation[1] = param[0];
            break;
        case GL_QUADRATIC_ATTENUATION:
            l->attenuation[2] = param[0];
            break;
        default:
            assert(0); // Unsupported pname
        }
    }

    void glLightModeli(GLenum pname, GLint param)
    {
        tGLfixed v[4] = {param, 0, 0, 0};
        glLightModelxv(pname, v);
    }

    void glLightModelxv(GLenum pname, const tGLfixed *param)
    {
        GLContext *c = gl_get_context();
        V4 v = *(V4 *)param;

        switch (pname)
        {
        case GL_LIGHT_MODEL_AMBIENT:
            memcpy(&c->light.model.ambient, &v, sizeof(V4));
            break;
        case GL_LIGHT_MODEL_TWO_SIDE:
            c->light.model.two_side = (int)v.X;
            break;
        default:
            fprintf(stderr, "glLightModel: illegal pname: 0x%x\n", pname);
            break;
        }
    }

    // Optimized clamp
    static inline tGLfixed clampx(tGLfixed a, tGLfixed min, tGLfixed max)
    {
        return (a < min) ? min : ((a > max) ? max : a);
    }

    /* Optimized lighting model with fast inverse square root */
    void gl_shade_vertex(GLContext *c, GLVertex *v)
    {
        tGLfixed R, G, B, A;
        GLMaterial *m = &c->material.materials[0];
        GLLight *l;
        V3 n = v->normal, d;
        tGLfixed dist_sq, inv_dist, dot, att;
        int twoside = c->light.model.two_side;

        // Initialize with emission and ambient light
        R = m->emission.X + m->ambient.X * c->light.model.ambient.X;
        G = m->emission.Y + m->ambient.Y * c->light.model.ambient.Y;
        B = m->emission.Z + m->ambient.Z * c->light.model.ambient.Z;
        A = clampx(m->diffuse.W, 0, 1);

        // Loop over all lights
        for (l = c->light.first; l != NULL; l = l->next)
        {
            tGLfixed lR, lG, lB;

            // Ambient contribution
            lR = l->ambient.X * m->ambient.X;
            lG = l->ambient.Y * m->ambient.Y;
            lB = l->ambient.Z * m->ambient.Z;

            // Compute the direction to the light
            if (l->position.W == 0)
            {
                // Directional light
                d = l->norm_position;
                att = 1.0f;
            }
            else
            {
                // Positional light with attenuation
                d.X = l->position.X - v->ec.X;
                d.Y = l->position.Y - v->ec.Y;
                d.Z = l->position.Z - v->ec.Z;

                dist_sq = d.X * d.X + d.Y * d.Y + d.Z * d.Z;
                inv_dist = fast_inverse_sqrt(dist_sq);
                d.X *= inv_dist;
                d.Y *= inv_dist;
                d.Z *= inv_dist;

                // Attenuation based on distance
                att = 1.0f / (l->attenuation[0] + inv_dist * (l->attenuation[1] + inv_dist * l->attenuation[2]));
            }

            // Calculate the dot product between the light direction and the normal
            dot = d.X * n.X + d.Y * n.Y + d.Z * n.Z;
            if (twoside && dot < 0)
                dot = -dot;

            if (dot > 0)
            {
                // Add diffuse contribution
                lR += dot * l->diffuse.X * m->diffuse.X;
                lG += dot * l->diffuse.Y * m->diffuse.Y;
                lB += dot * l->diffuse.Z * m->diffuse.Z;

                // Calculate specular contribution if needed
                // Specular code omitted for brevity
            }

            // Accumulate the light's contribution
            R += att * lR;
            G += att * lG;
            B += att * lB;
        }

        // Final color clamping
        v->color.X = clampx(R, 0, 1);
        v->color.Y = clampx(G, 0, 1);
        v->color.Z = clampx(B, 0, 1);
        v->color.W = A;
    }

} // namespace fp

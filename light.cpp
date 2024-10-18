#include "zgl.hpp"
#include "zmath.hpp"
#include "specbuf.hpp"
#include "light.hpp"

namespace fp
{

    void glMaterialx(GLenum face, GLenum pname, tGLfixed param)
    {
        tGLfixed v[4] = {param, 0, 0, 0};
        glMaterialxv(face, pname, v);
    }

    void glMaterialxv(GLenum face, GLenum pname, const tGLfixed *v)
{
    GLContext *c = gl_get_context();
    int i;
    GLMaterial *m;

    if (face == GL_FRONT_AND_BACK)
    {
        glMaterialxv(GL_FRONT, pname, v);
        face = GL_BACK;
    }
    if (face == GL_FRONT)
        m = &c->material.materials[0];
    else
        m = &c->material.materials[1];

    switch (pname)
    {
    case GL_EMISSION:
        m->emission.X = v[0];
        m->emission.Y = v[1];
        m->emission.Z = v[2];
        m->emission.W = v[3];
        break;
    case GL_AMBIENT:
        m->ambient.X = v[0];
        m->ambient.Y = v[1];
        m->ambient.Z = v[2];
        m->ambient.W = v[3];
        break;
    case GL_DIFFUSE:
        m->diffuse.X = v[0];
        m->diffuse.Y = v[1];
        m->diffuse.Z = v[2];
        m->diffuse.W = v[3];
        break;
    case GL_SPECULAR:
        m->specular.X = v[0];
        m->specular.Y = v[1];
        m->specular.Z = v[2];
        m->specular.W = v[3];
        break;
    case GL_SHININESS:
        m->shininess = v[0];
        m->shininess_i = (v[0] / 128.0f) * SPECULAR_BUFFER_RESOLUTION;
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        m->diffuse.X = v[0];
        m->diffuse.Y = v[1];
        m->diffuse.Z = v[2];
        m->diffuse.W = v[3];
        m->ambient.X = v[0];
        m->ambient.Y = v[1];
        m->ambient.Z = v[2];
        m->ambient.W = v[3];
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
        tGLfixed v[4] = {pname, 0, 0, 0};
        glLightxv(light, pname, v);
    }

    void glLightxv(GLenum light, GLenum pname, const tGLfixed *param)
{
    GLContext *c = gl_get_context();
    GLLight *l;
    int i;

    assert(light >= GL_LIGHT0 && light < GL_LIGHT0 + MAX_LIGHTS);

    V4 v = *(V4 *)param;
    l = &c->light.lights[light - GL_LIGHT0];

    switch (pname)
    {
    case GL_AMBIENT:
        l->ambient = v;
        break;
    case GL_DIFFUSE:
        l->diffuse = v;
        break;
    case GL_SPECULAR:
        l->specular = v;
        break;
    case GL_POSITION:
    {
        V4 pos;
        gl_M4_MulV4(&pos, c->matrix.stack_ptr[0], &v);

        l->position = pos;

        if (l->position.W == 0)
        {
            l->norm_position.X = pos.X;
            l->norm_position.Y = pos.Y;
            l->norm_position.Z = pos.Z;

            gl_V3_Norm(&l->norm_position);
        }
    }
    break;
    case GL_SPOT_DIRECTION:
        l->spot_direction.X = v.X;
        l->spot_direction.Y = v.Y;
        l->spot_direction.Z = v.Z;

        l->norm_spot_direction.X = v.X;
        l->norm_spot_direction.Y = v.Y;
        l->norm_spot_direction.Z = v.Z;
        fp::gl_V3_Norm(&l->norm_spot_direction);
        break;
    case GL_SPOT_EXPONENT:
        l->spot_exponent = v.X;
        break;
    case GL_SPOT_CUTOFF:
    {
        tGLfixed a = v.X;
        assert(a == 180 || (a >= 0 && a <= 90));
        l->spot_cutoff = a;
        if (a != 180)
            l->cos_spot_cutoff = cos(a * GLfixed_PI / 180.0);
    }
    break;
    case GL_CONSTANT_ATTENUATION:
        l->attenuation[0] = v.X;
        break;
    case GL_LINEAR_ATTENUATION:
        l->attenuation[1] = v.X;
        break;
    case GL_QUADRATIC_ATTENUATION:
        l->attenuation[2] = v.X;
        break;
    default:
        assert(0);
    }
}

    void glLightModeli(GLenum pname, GLint param)
    {
        tGLfixed v[4] = {pname, 0, 0, 0};
        glLightModelxv(pname, v);
    }

    void glLightModelxv(GLenum pname, const tGLfixed *param)
    {
        GLContext *c = gl_get_context();
        V4 v = *(V4 *)param;

        switch (pname)
        {
        case GL_LIGHT_MODEL_AMBIENT:
            c->light.model.ambient = v;
            break;
            // todo(alpi): enum is not defined in gles/gl.h
            //        case GL_LIGHT_MODEL_LOCAL_VIEWER:
            //            c->light.model.local = (int)v.X;
            //            break;
        case GL_LIGHT_MODEL_TWO_SIDE:
            c->light.model.two_side = (int)v.X;
            break;
        default:
            fprintf(stderr, "glLightModel: illegal pname: 0x%x\n", pname);
            break;
        }
    }

    static inline tGLfixed clampx(tGLfixed a, tGLfixed min, tGLfixed max)
    {
        if (a < min)
            return min;
        else if (a > max)
            return max;
        else
            return a;
    }

    /* non optimized lighting model */
    void gl_shade_vertex(GLContext *c, GLVertex *v)
{
    tGLfixed R, G, B, A;
    GLMaterial *m;
    GLLight *l;
    V3 n, s, d;
    tGLfixed dist, tmp, att;
    int twoside = c->light.model.two_side;

    m = &c->material.materials[0];

    n.X = v->normal.X;
    n.Y = v->normal.Y;
    n.Z = v->normal.Z;

    R = m->emission.X + m->ambient.X * c->light.model.ambient.X;
    G = m->emission.Y + m->ambient.Y * c->light.model.ambient.Y;
    B = m->emission.Z + m->ambient.Z * c->light.model.ambient.Z;
    A = clampx(m->diffuse.W, 0, 1);

    for (l = c->light.first; l != NULL; l = l->next)
    {
        tGLfixed lR, lB, lG;

        /* ambient */
        lR = l->ambient.X * m->ambient.X;
        lG = l->ambient.Y * m->ambient.Y;
        lB = l->ambient.Z * m->ambient.Z;

        if (l->position.W == 0)
        {
            /* light at infinity */
            d.X = l->position.X;
            d.Y = l->position.Y;
            d.Z = l->position.Z;
            att = 1;
        }
        else
        {
            /* distance attenuation */
            d.X = l->position.X - v->ec.X;
            d.Y = l->position.Y - v->ec.Y;
            d.Z = l->position.Z - v->ec.Z;
            dist = sqrt(d.X * d.X + d.Y * d.Y + d.Z * d.Z);
            if (dist > 1E-3)
            {
                tmp = 1 / dist;
                d.X = fp::multiply(d.X, tmp);
                d.Y = fp::multiply(d.Y, tmp);
                d.Z = fp::multiply(d.Z, tmp);
            }
            att = 1.0f / (l->attenuation[0] + dist * (l->attenuation[1] + dist * l->attenuation[2]));
        }
        tGLfixed dot = d.X * n.X + d.Y * n.Y + d.Z * n.Z;
        if (twoside && dot < 0)
            dot = -dot;
        if (dot > 0)
        {
            /* diffuse light */
            lR += dot * l->diffuse.X * m->diffuse.X;
            lG += dot * l->diffuse.Y * m->diffuse.Y;
            lB += dot * l->diffuse.Z * m->diffuse.Z;

            /* specular light handling... */
        }

        R += att * lR;
        G += att * lG;
        B += att * lB;
    }

    v->color.X = clampx(R, 0, 1);
    v->color.Y = clampx(G, 0, 1);
    v->color.Z = clampx(B, 0, 1);
    v->color.W = A;
}

} // namespace fp

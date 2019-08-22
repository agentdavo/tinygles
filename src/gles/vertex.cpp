#include "vertex.hpp"
#include "matrix.hpp" // to print matrix
#include "light.hpp"

namespace fp {

void glNormal3x(tGLfixed x, tGLfixed y, tGLfixed z) {
    GLContext *c = gl_get_context();
    c->current.normal.X = x;
    c->current.normal.Y = y;
    c->current.normal.Z = z;
    c->current.normal.W = 0;
}

void glTexCoord4x(tGLfixed s, tGLfixed t, tGLfixed r, tGLfixed q) {
    GLContext *c = gl_get_context();
    c->current.tex_coord.X = s;
    c->current.tex_coord.Y = t;
    c->current.tex_coord.Z = r;
    c->current.tex_coord.W = q;
}

void glEdgeFlag(GLboolean flag) {
    GLContext *c = gl_get_context();
    c->current.edge_flag = flag;
}

void glColor4x(tGLfixed r, tGLfixed g, tGLfixed b, tGLfixed a) {
    GLContext *c = gl_get_context();
    c->current.color.X = r;
    c->current.color.Y = g;
    c->current.color.Z = b;
    c->current.color.W = a;

    c->current.longcolor[0] = (unsigned int) (r * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) + ZB_POINT_RED_MIN);
    c->current.longcolor[1] = (unsigned int) (g * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) + ZB_POINT_GREEN_MIN);
    c->current.longcolor[2] = (unsigned int) (b * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) + ZB_POINT_BLUE_MIN);

    if (c->material.color.enabled) {
        tGLfixed color[4] = {0.5, 0.5, 0.5, 0.5};
        glMaterialxv(c->material.color.current_mode, c->material.color.current_type, color);
    }
}


void gl_eval_viewport(GLContext * c) {
    GLViewport *v;
    tGLfixed zsize = (1 << (ZB_Z_BITS + ZB_POINT_Z_FRAC_BITS));

    v = &c->viewport;

    v->trans.X = ((v->xsize - 0.5) / 2.0) + v->xmin;
    v->trans.Y = ((v->ysize - 0.5) / 2.0) + v->ymin;
    v->trans.Z = ((zsize - 0.5) / 2.0) + ((1 << ZB_POINT_Z_FRAC_BITS)) / 2;

    v->scale.X = (v->xsize - 0.5) / 2.0;
    v->scale.Y = -(v->ysize - 0.5) / 2.0;
    v->scale.Z = -((zsize - 0.5) / 2.0);
}

void glBegin(GLenum type) {
    GLContext *c = gl_get_context();
    M4 tmp;

    assert(c->in_begin == 0);

    c->begin_type = type;
    c->in_begin = 1;
    c->vertex_n = 0;
    c->vertex_cnt = 0;

    if (c->matrix.model_projection_updated) {

        if (c->light.enabled) {
            /* precompute inverse modelview */
            gl_M4_Inv(&tmp, c->matrix.stack_ptr[0]);
            gl_M4_Transpose(&c->matrix.model_view_inv, &tmp);
        } else {
            tGLfixed *m = &c->matrix.model_projection.m[0][0];
            /* precompute projection matrix */
#ifdef DEBUG_GLBEGIN_MATRIX
            std::cout<<"inside glBegin, c->matrix.stack_ptr[1]="<<std::endl;
            gl_print_matrix4(c->matrix.stack_ptr[1]);
            std::cout<<"inside glBegin, c->matrix.stack_ptr[0]="<<std::endl;
            gl_print_matrix4(c->matrix.stack_ptr[0]);
#endif // DEBUG_GLBEGIN_MATRIX
            gl_M4_Mul(&c->matrix.model_projection, c->matrix.stack_ptr[1], c->matrix.stack_ptr[0]);
#ifdef DEBUG_GLBEGIN_MATRIX
            std::cout<<"inside glBegin, c->matrix.model_projection="<<std::endl;
            gl_print_matrix4(&c->matrix.model_projection);
#endif // DEBUG_GLBEGIN_MATRIX
            /* test to accelerate computation */
            c->matrix.model_projection_no_w_transform = 0;
            if (m[12] == 0.0 && m[13] == 0.0 && m[14] == 0.0) {
                c->matrix.model_projection_no_w_transform = 1;
            }
        }

        /* test if the texture matrix is not Identity */
        c->matrix.apply_texture = !gl_M4_IsId(c->matrix.stack_ptr[2]);

        c->matrix.model_projection_updated = 0;
    }
    /*  viewport */
    if (c->viewport.updated) {
        gl_eval_viewport(c);
        c->viewport.updated = 0;
    }
    /* triangle drawing functions */
    switch (c->polygon_mode_front) {
        case GL_POINTS:
            c->draw_triangle_front = gl_draw_triangle_point;
            break;
        case GL_LINES:
            c->draw_triangle_front = gl_draw_triangle_line;
            break;
        default:
            c->draw_triangle_front = gl_draw_triangle_fill;
            break;
    }

    switch (c->polygon_mode_back) {
        case GL_POINTS:
            c->draw_triangle_back = gl_draw_triangle_point;
            break;
        case GL_LINES:
            c->draw_triangle_back = gl_draw_triangle_line;
            break;
        default:
            c->draw_triangle_back = gl_draw_triangle_fill;
            break;
    }
}

/* coords, tranformation , clip code and projection */
/* TODO : handle all cases */
static inline void gl_vertex_transform(GLContext *c, GLVertex *v) {
    tGLfixed *m;
    V4 *n;
#ifdef DEBUG_VERTEX_TRANSFORM
    std::cout<<"vertex not transformed x, y, z, w = "<<v->pc.X<<", "<<v->pc.Y<<", "<<v->pc.Z<<", "<<v->pc.W<<std::endl;
#endif // DEBUG_VERTEX_TRANSFORM
    if (c->light.enabled) {
        /* eye coordinates needed for lighting */

        m = &c->matrix.stack_ptr[0]->m[0][0];
        v->ec.X = (v->coord.X * m[0]  + v->coord.Y * m[1]  + v->coord.Z * m[2]  + m[3]);
        v->ec.Y = (v->coord.X * m[4]  + v->coord.Y * m[5]  + v->coord.Z * m[6]  + m[7]);
        v->ec.Z = (v->coord.X * m[8]  + v->coord.Y * m[9]  + v->coord.Z * m[10] + m[11]);
        v->ec.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);

        /* projection coordinates */
        m = &c->matrix.stack_ptr[1]->m[0][0];
        v->pc.X = (v->ec.X * m[0]  + v->ec.Y * m[1]  + v->ec.Z * m[2]  + v->ec.W * m[3]);
        v->pc.Y = (v->ec.X * m[4]  + v->ec.Y * m[5]  + v->ec.Z * m[6]  + v->ec.W * m[7]);
        v->pc.Z = (v->ec.X * m[8]  + v->ec.Y * m[9]  + v->ec.Z * m[10] + v->ec.W * m[11]);
        v->pc.W = (v->ec.X * m[12] + v->ec.Y * m[13] + v->ec.Z * m[14] + v->ec.W * m[15]);

        m = &c->matrix.model_view_inv.m[0][0];
        n = &c->current.normal;

        v->normal.X = (n->X * m[0] + n->Y * m[1] + n->Z * m[2]);
        v->normal.Y = (n->X * m[4] + n->Y * m[5] + n->Z * m[6]);
        v->normal.Z = (n->X * m[8] + n->Y * m[9] + n->Z * m[10]);

        if (c->normalize_enabled) {
            gl_V3_Norm(&v->normal);
        }
    } else {
        /* no eye coordinates needed, no normal */
        /* NOTE: W = 1 is assumed */
        m = &c->matrix.model_projection.m[0][0];
#ifdef DEBUG_VERTEX_TRANSFORM
//        std::cout<<"inside gl_vertex_transform, c->matrix.model_projection="<<std::endl;
//        gl_print_matrix4(&c->matrix.model_projection);
#endif // DEBUG_VERTEX_TRANSFORM

        v->pc.X = (v->coord.X * m[0] + v->coord.Y * m[1] + v->coord.Z * m[2] + m[3]);
        v->pc.Y = (v->coord.X * m[4] + v->coord.Y * m[5] + v->coord.Z * m[6] + m[7]);
        v->pc.Z = (v->coord.X * m[8] + v->coord.Y * m[9] + v->coord.Z * m[10] + m[11]);
        if (c->matrix.model_projection_no_w_transform) {
            v->pc.W = m[15];
        } else {
            v->pc.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);
        }
#ifdef DEBUG_VERTEX_TRANSFORM
        std::cout<<"vertex transformed x, y, z, w = "<<v->pc.X<<", "<<v->pc.Y<<", "<<v->pc.Z<<", "<<v->pc.W<<std::endl;
#endif // DEBUG_VERTEX_TRANSFORM
    }

    v->clip_code = gl_clipcode(v->pc.X, v->pc.Y, v->pc.Z, v->pc.W);
}

void glVertex4x(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w) {
    GLContext *c = gl_get_context();
    GLVertex *v;
    int n, i, cnt;

    assert(c->in_begin != 0);

    n = c->vertex_n;
    cnt = c->vertex_cnt;
    cnt++;
    c->vertex_cnt = cnt;

    /* quick fix to avoid crashes on large polygons */
    if (n >= c->vertex_max) {
        GLVertex* newarray;
        c->vertex_max <<= 1;	/* just double size */
        newarray = (GLVertex*)malloc(sizeof(GLVertex) * c->vertex_max);
        if (!newarray) {
            fprintf(stderr, "unable to allocate GLVertex array.\n");
        }
        memcpy(newarray, c->vertex, n * sizeof(GLVertex));
        free(c->vertex);
        c->vertex = newarray;
    }
    /* new vertex entry */
    v = &c->vertex[n];
    n++;

    v->coord.X = x;
    v->coord.Y = y;
    v->coord.Z = z;
    v->coord.W = w;

    gl_vertex_transform(c, v);

    /* color */

    if (c->light.enabled) {
        gl_shade_vertex(c, v);
    } else {
        v->color = c->current.color;
    }

    /* tex coords */
    if (c->texture.enabled_2d) {
        if (c->matrix.apply_texture) {
            gl_M4_MulV4(&v->tex_coord, c->matrix.stack_ptr[2], &c->current.tex_coord);
        } else {
            v->tex_coord = c->current.tex_coord;
        }
    }

    /* precompute the mapping to the viewport */
    if (v->clip_code == 0)
        gl_transform_to_viewport(c, v);

    /* edge flag */

    v->edge_flag = c->current.edge_flag;

    switch (c->begin_type) {
        case GL_POINTS:
            gl_draw_point(c, &c->vertex[0]);
            n = 0;
            break;
        case GL_LINES:
            if (n == 2) {
                gl_draw_line(c, &c->vertex[0], &c->vertex[1]);
                n = 0;
            }
            break;
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
            if (n == 1) {
                c->vertex[2] = c->vertex[0];
            } else if (n == 2) {
                gl_draw_line(c, &c->vertex[0], &c->vertex[1]);
                c->vertex[0] = c->vertex[1];
                n = 1;
            }
            break;
        case GL_TRIANGLES:
            if (n == 3) {
                gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
                n = 0;
            }
            break;
        case GL_TRIANGLE_STRIP:
            if (cnt >= 3) {
                if (n == 3)
                    n = 0;
                /* needed to respect triangle orientation */
                switch(cnt & 1) {
                    case 0:
                        gl_draw_triangle(c, &c->vertex[2], &c->vertex[1], &c->vertex[0]);
                        break;
                    default:
                    case 1:
                        gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
                        break;
                }
            }
            break;
        case GL_TRIANGLE_FAN:
            if (n == 3) {
                gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
                c->vertex[1] = c->vertex[2];
                n = 2;
            }
            break;
//            // todo(alpi): is GL_QUADS deprecated?  (use GL_TRIANGLES instead)
//        case GL_QUADS:
//            if (n == 4) {
//                c->vertex[2].edge_flag = 0;
//                gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
//                c->vertex[2].edge_flag = 1;
//                c->vertex[0].edge_flag = 0;
//                gl_draw_triangle(c, &c->vertex[0], &c->vertex[2], &c->vertex[3]);
//                n = 0;
//            }
//            break;
//         //                todo(alpi): is GL_QUAD_STRIP deprecated? (use GL_TRIANGLES instead)
//        case GL_QUAD_STRIP:
//            if (n == 4) {
//                gl_draw_triangle(c, &c->vertex[0], &c->vertex[1], &c->vertex[2]);
//                gl_draw_triangle(c, &c->vertex[1], &c->vertex[3], &c->vertex[2]);
//                for (i = 0; i < 2; i++)
//                    c->vertex[i] = c->vertex[i + 2];
//                n = 2;
//            }
//            break;
//           //              todo(alpi): is GL_POLYGON deprecated?
//    case GL_POLYGON:
//        break;
        default:
            fprintf(stderr, "glBegin: type %x not handled\n", c->begin_type);
    }

    c->vertex_n = n;
}

void glEnd() {
    GLContext *c = gl_get_context();
    assert(c->in_begin == 1);

    if (c->begin_type == GL_LINE_LOOP) {
        if (c->vertex_cnt >= 3) {
            gl_draw_line(c, &c->vertex[0], &c->vertex[2]);
        }
                             // tod(alpi)o: is GL_POLYGON deprecated?
    }/* else if (c->begin_type == GL_POLYGON) {
        int i = c->vertex_cnt;
        while (i >= 3) {
            i--;
            gl_draw_triangle(c, &c->vertex[i], &c->vertex[0], &c->vertex[i - 1]);
        }
    }*/
    c->in_begin = 0;
}

} // namespce fp

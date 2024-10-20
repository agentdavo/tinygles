#include "zgl.hpp"

/* fill triangle profile */
/* #define PROFILE */

#define CLIP_XMIN (1<<0)
#define CLIP_XMAX (1<<1)
#define CLIP_YMIN (1<<2)
#define CLIP_YMAX (1<<3)
#define CLIP_ZMIN (1<<4)
#define CLIP_ZMAX (1<<5)

namespace fp {

void gl_transform_to_viewport(GLContext *c, GLVertex *v) {
    tGLfixed winv;

    /* coordinates */
    winv = 1.0 / v->pc.W;
    v->zp.x = (int)(v->pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
    v->zp.y = (int)(v->pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
    v->zp.z = (int)(v->pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
    /* color */
    if (c->light.enabled) {
        v->zp.r = (int)(v->color.X * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) + ZB_POINT_RED_MIN);
        v->zp.g = (int)(v->color.Y * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) + ZB_POINT_GREEN_MIN);
        v->zp.b = (int)(v->color.Z * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) + ZB_POINT_BLUE_MIN);
    } else {
        /* no need to convert to integer if no lighting : take current color */
        v->zp.r = c->current.longcolor[0];
        v->zp.g = c->current.longcolor[1];
        v->zp.b = c->current.longcolor[2];
    }

    /* texture */

    if (c->texture.enabled_2d) {
        v->zp.s = (int)(v->tex_coord.X * (ZB_POINT_S_MAX - ZB_POINT_S_MIN) + ZB_POINT_S_MIN);
        v->zp.t = (int)(v->tex_coord.Y * (ZB_POINT_T_MAX - ZB_POINT_T_MIN) + ZB_POINT_T_MIN);
    }
}

/* point */

void gl_draw_point(GLContext *c, GLVertex *p0) {
    if (p0->clip_code == 0) {
        ZB_plot(c->zb, &p0->zp);
    }
}

/* line */

static inline void interpolate(GLVertex *q, GLVertex *p0, GLVertex *p1, tGLfixed t) {
    q->pc.X=p0->pc.X+(p1->pc.X-p0->pc.X)*t;
    q->pc.Y=p0->pc.Y+(p1->pc.Y-p0->pc.Y)*t;
    q->pc.Z=p0->pc.Z+(p1->pc.Z-p0->pc.Z)*t;
    q->pc.W=p0->pc.W+(p1->pc.W-p0->pc.W)*t;

    q->color.X = p0->color.X + (p1->color.X - p0->color.X) * t;
    q->color.Y = p0->color.Y + (p1->color.Y - p0->color.Y) * t;
    q->color.Z = p0->color.Z + (p1->color.Z - p0->color.Z) * t;
}

/*
 * Line Clipping
 */

/* Line Clipping algorithm from 'Computer Graphics', Principles and
   Practice */
static inline int ClipLine1(tGLfixed denom, tGLfixed num, tGLfixed *tmin, tGLfixed *tmax) {
    tGLfixed t;

    if (denom>0) {
        t = num/denom;
        if (t>*tmax) return 0;
        if (t>*tmin) *tmin = t;
    } else if (denom<0) {
        t = num/denom;
        if (t<*tmin) return 0;
        if (t<*tmax) *tmax = t;
    } else if (num>0) return 0;
    return 1;
}

void gl_draw_line(GLContext *c, GLVertex *p1, GLVertex *p2) {
    tGLfixed dx, dy, dz, dw, x1, y1, z1, w1;
    tGLfixed tmin, tmax;
    GLVertex q1, q2;
    int cc1, cc2;

    cc1=p1->clip_code;
    cc2=p2->clip_code;

    if ((cc1 | cc2) == 0) {
        if (c->depth_test)
            ZB_line_z(c->zb, &p1->zp, &p2->zp);
        else
            ZB_line(c->zb, &p1->zp, &p2->zp);
    } else if ((cc1&cc2) != 0) {
        return;
    } else {
        dx = p2->pc.X-p1->pc.X;
        dy = p2->pc.Y-p1->pc.Y;
        dz = p2->pc.Z-p1->pc.Z;
        dw = p2->pc.W-p1->pc.W;
        x1 = p1->pc.X;
        y1 = p1->pc.Y;
        z1 = p1->pc.Z;
        w1 = p1->pc.W;

        tmin = 0;
        tmax = 1;
        if (ClipLine1(dx+dw, -x1-w1, &tmin, &tmax) &&
                ClipLine1(-dx+dw, x1-w1, &tmin, &tmax) &&
                ClipLine1(dy+dw, -y1-w1, &tmin, &tmax) &&
                ClipLine1(-dy+dw, y1-w1, &tmin, &tmax) &&
                ClipLine1(dz+dw, -z1-w1, &tmin, &tmax) &&
                ClipLine1(-dz+dw, z1-w1, &tmin, &tmax)) {

            interpolate(&q1, p1, p2, tmin);
            interpolate(&q2, p1, p2, tmax);
            fp::gl_transform_to_viewport(c, &q1);
            fp::gl_transform_to_viewport(c, &q2);

            if (c->depth_test)
                ZB_line_z(c->zb, &q1.zp, &q2.zp);
            else
                ZB_line(c->zb, &q1.zp, &q2.zp);
        }
    }
}


/* triangle */

/*
 * Clipping
 */

/* We clip the segment [a,b] against the 6 planes of the normal volume.
 * We compute the point 'c' of intersection and the value of the parameter 't'
 * of the intersection if x=a+t(b-a).
 */

#define clip_func(name, sign, dir, dir1, dir2) \
static tGLfixed name(V4 *c, V4 *a, V4 *b) {\
    tGLfixed t, dX, dY, dZ, dW, den;\
    dX = (b->X - a->X);\
    dY = (b->Y - a->Y);\
    dZ = (b->Z - a->Z);\
    dW = (b->W - a->W);\
    den = -(sign d ## dir) + dW;\
    if (den == 0) t = 0;\
    else t = ( sign a->dir - a->W) / den;\
    c->dir1 = a->dir1 + t * d ## dir1;\
    c->dir2 = a->dir2 + t * d ## dir2;\
    c->W = a->W + t * dW;\
    c->dir = sign c->W;\
    return t;\
}


clip_func(clip_xmin, -, X, Y, Z)

clip_func(clip_xmax, +, X, Y, Z)

clip_func(clip_ymin, -, Y, X, Z)

clip_func(clip_ymax, +, Y, X, Z)

clip_func(clip_zmin, -, Z, X, Y)

clip_func(clip_zmax, +, Z, X, Y)


tGLfixed (*clip_proc[6])(V4 *, V4 *, V4 *)=  {
    clip_xmin, clip_xmax,
    clip_ymin, clip_ymax,
    clip_zmin, clip_zmax
};

static inline void updateTmp(GLContext *c, GLVertex *q, GLVertex *p0, GLVertex *p1, tGLfixed t) {
    if (c->current_shade_model == GL_SMOOTH) {
        q->color.X = p0->color.X + (p1->color.X - p0->color.X) * t;
        q->color.Y = p0->color.Y + (p1->color.Y - p0->color.Y) * t;
        q->color.Z = p0->color.Z + (p1->color.Z - p0->color.Z) * t;
    } else {
        q->color.X = p0->color.X;
        q->color.Y = p0->color.Y;
        q->color.Z = p0->color.Z;
    }

    if (c->texture.enabled_2d) {
        q->tex_coord.X = p0->tex_coord.X + (p1->tex_coord.X - p0->tex_coord.X) * t;
        q->tex_coord.Y = p0->tex_coord.Y + (p1->tex_coord.Y - p0->tex_coord.Y) * t;
    }

    q->clip_code = gl_clipcode(q->pc.X, q->pc.Y, q->pc.Z, q->pc.W);
    if (q->clip_code == 0)
        fp::gl_transform_to_viewport(c, q);
}

static void gl_draw_triangle_clip(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2, int clip_bit);

void gl_draw_triangle(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2) {
    int co, c_and, cc[3], front;
    tGLfixed norm;

    cc[0] = p0->clip_code;
    cc[1] = p1->clip_code;
    cc[2] = p2->clip_code;

    co = cc[0] | cc[1] | cc[2];

    /* we handle the non clipped case here to go faster */
    if (co == 0) {

        norm = (tGLfixed)(p1->zp.x - p0->zp.x) * (tGLfixed)(p2->zp.y - p0->zp.y) - (tGLfixed)(p2->zp.x - p0->zp.x) * (tGLfixed)(p1->zp.y - p0->zp.y);

        if (norm == 0) return;

        front = norm < 0.0;
        front = front ^ c->current_front_face;

        /* back face culling */
        if (c->cull_face_enabled) {
            /* most used case first */
            if (c->current_cull_face == GL_BACK) {
                if (front == 0) return;
                c->draw_triangle_front(c, p0, p1, p2);
            } else if (c->current_cull_face == GL_FRONT) {
                if (front != 0) return;
                c->draw_triangle_back(c, p0, p1, p2);
            } else {
                return;
            }
        } else {
            /* no culling */
            if (front) {
                c->draw_triangle_front(c, p0, p1, p2);
            } else {
                c->draw_triangle_back(c, p0, p1, p2);
            }
        }
    } else {
        c_and = cc[0] & cc[1] & cc[2];
        if (c_and == 0) {
            gl_draw_triangle_clip(c, p0, p1, p2, 0);
        }
    }
}

static void gl_draw_triangle_clip(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2, int clip_bit) {
    int co, c_and, co1, cc[3], edge_flag_tmp, clip_mask;
    GLVertex tmp1, tmp2, *q[3];
    tGLfixed tt;

    cc[0] = p0->clip_code;
    cc[1] = p1->clip_code;
    cc[2] = p2->clip_code;

    co = cc[0] | cc[1] | cc[2];
    if (co == 0) {
        fp::gl_draw_triangle(c, p0, p1, p2);
    } else {
        c_and = cc[0] & cc[1] & cc[2];
        /* the triangle is completely outside */
        if (c_and != 0) return;

        /* find the next direction to clip */
        while (clip_bit < 6 && (co & (1 << clip_bit)) == 0)  {
            clip_bit++;
        }

        /* this test can be true only in case of rounding errors */
        if (clip_bit == 6) {
#if 0
            printf("Error:\n");
            printf("%f %f %f %f\n", p0->pc.X, p0->pc.Y,p0->pc.Z, p0->pc.W);
            printf("%f %f %f %f\n", p1->pc.X, p1->pc.Y, p1->pc.Z, p1->pc.W);
            printf("%f %f %f %f\n", p2->pc.X, p2->pc.Y, p2->pc.Z, p2->pc.W);
#endif
            return;
        }

        clip_mask = 1 << clip_bit;
        co1 = (cc[0] ^ cc[1] ^ cc[2]) & clip_mask;

        if (co1)  {
            /* one point outside */

            if (cc[0] & clip_mask) { q[0] = p0; q[1] = p1; q[2] = p2; }
            else if (cc[1] & clip_mask) { q[0] = p1; q[1] = p2; q[2] = p0; }
            else { q[0] = p2; q[1] = p0; q[2] = p1; }

            tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
            updateTmp(c, &tmp1, q[0], q[1], tt);

            tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
            updateTmp(c, &tmp2, q[0], q[2], tt);

            tmp1.edge_flag = q[0]->edge_flag;
            edge_flag_tmp = q[2]->edge_flag;
            q[2]->edge_flag = 0;
            gl_draw_triangle_clip(c, &tmp1, q[1], q[2], clip_bit+1);

            tmp2.edge_flag = 1;
            tmp1.edge_flag = 0;
            q[2]->edge_flag = edge_flag_tmp;
            gl_draw_triangle_clip(c, &tmp2, &tmp1, q[2], clip_bit+1);
        } else {
            /* two points outside */

            if ((cc[0] & clip_mask) == 0) { q[0] = p0; q[1] = p1; q[2] = p2; }
            else if ((cc[1] & clip_mask) == 0) { q[0] = p1; q[1] = p2; q[2] = p0; }
            else { q[0] = p2; q[1] = p0; q[2] = p1; }

            tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
            updateTmp(c, &tmp1, q[0], q[1], tt);

            tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
            updateTmp(c, &tmp2, q[0], q[2], tt);

            tmp1.edge_flag = 1;
            tmp2.edge_flag = q[2]->edge_flag;
            gl_draw_triangle_clip(c, q[0], &tmp1, &tmp2, clip_bit+1);
        }
    }
}

#ifdef PROFILE
int count_triangles, count_triangles_textured, count_pixels;
#endif

void gl_draw_triangle_fill(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2) {
#ifdef PROFILE
    {
        int norm;
        assert(p0->zp.x >= 0 && p0->zp.x < c->zb->xsize);
        assert(p0->zp.y >= 0 && p0->zp.y < c->zb->ysize);
        assert(p1->zp.x >= 0 && p1->zp.x < c->zb->xsize);
        assert(p1->zp.y >= 0 && p1->zp.y < c->zb->ysize);
        assert(p2->zp.x >= 0 && p2->zp.x < c->zb->xsize);
        assert(p2->zp.y >= 0 && p2->zp.y < c->zb->ysize);

        norm = (p1->zp.x - p0->zp.x) * (p2->zp.y - p0->zp.y) - (p2->zp.x - p0->zp.x) * (p1->zp.y - p0->zp.y);
        count_pixels += abs(norm)/2;
        count_triangles++;
    }
#endif

    if (c->texture.enabled_2d) {
#ifdef PROFILE
        count_triangles_textured++;
#endif
        ZB_setTexture(c->zb, (PIXEL*)c->texture.current->images[0].pixmap);
        ZB_fillTriangleMappingPerspective(c->zb, &p0->zp, &p1->zp, &p2->zp);
    } else if (c->current_shade_model == GL_SMOOTH) {
        ZB_fillTriangleSmooth(c->zb, &p0->zp, &p1->zp, &p2->zp);
    } else {
        ZB_fillTriangleFlat(c->zb, &p0->zp, &p1->zp, &p2->zp);
    }
}

/* Render a clipped triangle in line mode */

void gl_draw_triangle_line(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2) {
    if (c->depth_test) {
        if (p0->edge_flag) ZB_line_z(c->zb, &p0->zp, &p1->zp);
        if (p1->edge_flag) ZB_line_z(c->zb, &p1->zp, &p2->zp);
        if (p2->edge_flag) ZB_line_z(c->zb, &p2->zp, &p0->zp);
    } else {
        if (p0->edge_flag) ZB_line(c->zb, &p0->zp, &p1->zp);
        if (p1->edge_flag) ZB_line(c->zb, &p1->zp, &p2->zp);
        if (p2->edge_flag) ZB_line(c->zb, &p2->zp, &p0->zp);
    }
}



/* Render a clipped triangle in point mode */
void gl_draw_triangle_point(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2) {
  if (p0->edge_flag) ZB_plot(c->zb, &p0->zp);
  if (p1->edge_flag) ZB_plot(c->zb, &p1->zp);
  if (p2->edge_flag) ZB_plot(c->zb, &p2->zp);
}

} // namespace fp

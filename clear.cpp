#include "clear.hpp"

namespace fp {

void glClearColor4xv(const tGLfixed* v) {
    GLContext *c = gl_get_context();
    c->clear.color.v[0] = v[0];
    c->clear.color.v[1] = v[1];
    c->clear.color.v[2] = v[2];
    c->clear.color.v[3] = v[3];
}

void glClearColor4x(const tGLfixed& r, const tGLfixed& g, const tGLfixed& b, const tGLfixed& a) {
    GLContext *c = gl_get_context();
    c->clear.color.v[0] = r;
    c->clear.color.v[1] = g;
    c->clear.color.v[2] = b;
    c->clear.color.v[3] = a;
}

void glClearDepth(double depth) {
    GLContext *c = gl_get_context();
    c->clear.depth = depth;
}

void glClear(GLbitfield mask) {
    GLContext *c = gl_get_context();
    int z = 0;
    int r = (int)(c->clear.color.v[0] * 65535);
    int g = (int)(c->clear.color.v[1] * 65535);
    int b = (int)(c->clear.color.v[2] * 65535);

    /* TODO : correct value of Z */

    ZB_clear(c->zb, mask & GL_DEPTH_BUFFER_BIT, z,
             mask & GL_COLOR_BUFFER_BIT, r, g, b);
}

} // namespace fp

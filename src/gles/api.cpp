#include "api.hpp"
#include "vertex.hpp"

namespace fp {

/* glVertex */
void glVertex2x(tGLfixed x, tGLfixed y) {
    glVertex4x(x,y,0,1);
}

void glVertex3x(tGLfixed x, tGLfixed y, tGLfixed z) {
    glVertex4x(x,y,z,1);
}

void glVertex3xv(const tGLfixed *v) {
    glVertex4x(v[0], v[1], v[2], 1);
}

/* glNormal */
void glNormal3xv(const tGLfixed *v) {
    glNormal3x(v[0], v[1], v[2]);
}

/* glColor */
void glColor4xv(const tGLfixed *v) {
    glColor4x(v[0], v[1], v[2], v[3]);
}

void glColor3x(tGLfixed x, tGLfixed y, tGLfixed z) {
    glColor4x(x, y, z, 1);
}

void glColor3xv(const tGLfixed *v) {
    glColor4x(v[0], v[1], v[2], 1);
}

/* TexCoord */
void glTexCoord2x(tGLfixed s, tGLfixed t) {
    glTexCoord4x(s, t, 0, 1);
}

void glTexCoord2xv(const tGLfixed *v) {
    glTexCoord4x(v[0], v[1], 0, 1);
}

/* Special Functions */
void glFlush() {
    /* nothing to do */
}

/* Non standard functions */
void glDebug(int mode) {
    GLContext *c = gl_get_context();
    c->print_flag = mode;
}

} // namespace fp

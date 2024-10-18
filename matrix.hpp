#include "zgl.hpp"

namespace fp {

void gl_print_matrix(const tGLfixed *m);
void gl_print_matrix2(const tGLfixed *m);
void gl_print_matrix4(M4* m);

static inline void gl_matrix_update(GLContext *c);

void glMatrixMode(GLenum mode);

void glLoadMatrixx(const tGLfixed *matrix);

void glLoadIdentity();

void glMultMatrixx(const tGLfixed *matrix);


void glPushMatrix();

void glPopMatrix();

void glRotatex(tGLfixed angle, tGLfixed x, tGLfixed y, tGLfixed z);

void glScalex(tGLfixed x, tGLfixed y, tGLfixed z);

void glTranslatex(tGLfixed x, tGLfixed y, tGLfixed z);

void glFrustumx(tGLfixed left, tGLfixed right, tGLfixed bottom, tGLfixed top, tGLfixed near, tGLfixed far);

void glOrthox(tGLfixed left, tGLfixed right, tGLfixed bottom, tGLfixed top, tGLfixed near, tGLfixed far);

} // namespace fp

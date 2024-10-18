#include "zgl.hpp"

namespace fp {

void glMaterialx(GLenum face, GLenum pname, tGLfixed param);

void glMaterialxv(GLenum face, GLenum pname, const tGLfixed *v);

void glColorMaterial(GLenum face, GLenum mode);
void glLightx(GLenum light, GLenum pname, tGLfixed param);

void glLightxv(GLenum light, GLenum pname, const tGLfixed *param);

void glLightModeli(GLenum pname, GLint param);
void glLightModelxv(GLenum pname, const tGLfixed *param);


static inline tGLfixed clampx(tGLfixed a, tGLfixed min, tGLfixed max);

/* non optimized lighting model */
void gl_shade_vertex(GLContext *c, GLVertex *v);

} // namespace fp

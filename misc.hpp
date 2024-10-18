#include "zgl.hpp"

namespace fp {

void glViewport(GLint x, GLint y, GLint width, GLint height);


void glShadeModel(GLenum mode);
void glCullFace(GLenum mode);

void glFrontFace(GLenum mode);

void glPolygonMode(GLenum face, GLenum mode);

void glHint(GLenum target, GLenum mode);

void glPolygonOffset(tGLfixed factor, tGLfixed units);

} // namespace fp

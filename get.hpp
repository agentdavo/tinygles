#include "zgl.hpp"

namespace fp {

// TODO: do something with this
const GLubyte *glGetString(GLenum name);

void glGetIntegerx(GLenum pname, GLint *params);
void glGetFloatv(GLenum pname, tGLfixed *v);

void glGetBooleanv(GLenum pname, GLboolean *v);

} // namepsace fp

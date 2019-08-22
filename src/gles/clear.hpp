#include "zgl.hpp"

namespace fp {

void glClearColor4xv(const tGLfixed* v);
void glClearColor4x(const tGLfixed& r, const tGLfixed& g, const tGLfixed& b, const tGLfixed& a);
void glClearDepth(double depth) ;
void glClear(GLbitfield mask);

} // namespace fp

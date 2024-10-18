#include <stdbool.h>

#include "zgl.hpp"

namespace fp {

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    fprintf(stderr, "STUB: glScissor()\n");
}

void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
    fprintf(stderr, "STUB: glColorMask()\n");
}

void glDepthFunc(GLenum func) {
    fprintf(stderr, "STUB: glDepthFunc()\n");
}

void glClearDepthx(GLclampx depth) {
    fprintf(stderr, "STUB: glClearDepthf()\n");
}

void glDepthMask(GLboolean flag) {
    fprintf(stderr, "STUB: glDepthMask()\n");
}

// todo:
//void glLineWidth(tGLfixed width) {
//    fprintf(stderr, "STUB: glLineWidth\n");
//}

//void glPointSize(tGLfixed size) {
//    fprintf(stderr, "STUB: glPointSize\n");
//}

void glFinish() {
    fprintf(stderr, "STUB: glFinish()\n");
}

} // namespace fp

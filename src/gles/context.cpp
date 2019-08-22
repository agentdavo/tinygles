#include "zgl.hpp"

namespace fp {

GLContext *gl_get_context() {
    assert(gl_ctx);
    return gl_ctx;
}

} // namespace fp



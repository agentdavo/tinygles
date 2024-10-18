#pragma once

#include "zgl.hpp"

/* Max # of specular light pow buffers */
#define MAX_SPECULAR_BUFFERS 8
/* # of entries in specular buffer */
#define SPECULAR_BUFFER_SIZE 1024
/* specular buffer granularity */
#define SPECULAR_BUFFER_RESOLUTION 1024

namespace fp {

GLSpecBuf *specbuf_get_buffer(GLContext *c, const int shininess_i, const tGLfixed shininess);
void specbuf_cleanup(GLContext *c); /* free all memory used */

} // namespace fp

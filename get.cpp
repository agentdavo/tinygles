#include "get.hpp"

namespace fp {

// TODO: do something with this
const GLubyte *glGetString(GLenum name) {
    return (GLubyte *)"";
}

void glGetIntegerx(GLenum pname, GLint *params) {
    GLContext *c = gl_get_context();

    switch(pname) {
        case GL_VIEWPORT:
            params[0] = c->viewport.xmin;
            params[1] = c->viewport.ymin;
            params[2] = c->viewport.xsize;
            params[3] = c->viewport.ysize;
            break;
        case GL_MAX_MODELVIEW_STACK_DEPTH:
            *params = MAX_MODELVIEW_STACK_DEPTH;
            break;
        case GL_MAX_PROJECTION_STACK_DEPTH:
            *params = MAX_PROJECTION_STACK_DEPTH;
            break;
        case GL_MAX_LIGHTS:
            *params = MAX_LIGHTS;
            break;
        case GL_MAX_TEXTURE_SIZE:
            *params = 256; /* not completely true, but... */
            break;
        case GL_MAX_TEXTURE_STACK_DEPTH:
            *params = MAX_TEXTURE_STACK_DEPTH;
            break;
        default:
            *params = 0;
            fprintf(stderr, "glGet: option not implemented: %x\n", pname);
            break;
    }
}

void glGetFloatv(GLenum pname, tGLfixed *v) {
    int i;
    int mnr = 0; /* just a trick to return the correct matrix */
    GLContext *c = gl_get_context();

    switch (pname) {
        case GL_CURRENT_COLOR: {
            // Access the color components directly
            *v++ = c->current.color.X;
            *v++ = c->current.color.Y;
            *v++ = c->current.color.Z;
            *v++ = c->current.color.W;
            break;
        }
        case GL_CURRENT_NORMAL: {
            // Access the normal components directly
            *v++ = c->current.normal.X;
            *v++ = c->current.normal.Y;
            *v++ = c->current.normal.Z;
            break;
        }
        case GL_CURRENT_TEXTURE_COORDS: {
            // Access the texture coordinate components directly
            *v++ = c->current.tex_coord.X;
            *v++ = c->current.tex_coord.Y;
            *v++ = c->current.tex_coord.Z;
            *v++ = c->current.tex_coord.W;
            break;
        }
        case GL_TEXTURE_MATRIX:
            mnr++;
        case GL_PROJECTION_MATRIX:
            mnr++;
            std::cout<<"fixme: the function glGetFloatv(GL_PROJECTION_MATRIX) swap matrix index [3][2] and [2][3], see tests"<<std::endl;
        case GL_MODELVIEW_MATRIX: {
            tGLfixed *p = &c->matrix.stack_ptr[mnr]->m[0][0];
            for (i = 0; i < 4; i++) {
                *v++ = p[0];
                *v++ = p[4];
                *v++ = p[8];
                *v++ = p[12];
                p++;
            }
            break;
        }
        case GL_LINE_WIDTH:
            *v = 1.0f;
            break;
        case GL_POINT_SIZE:
            *v = 1.0f;
            break;
        default:
            fprintf(stderr, "warning: unknown pname in glGetFloatv(): %x\n", pname);
            break;
    }
}


void glGetBooleanv(GLenum pname, GLboolean *v) {
    fprintf(stderr, "warning: unknown pname in glGetBooleanv(): %x\n", pname);
}

} // namepsace fp

#pragma once

#include "zgl.hpp"

#define VERTEX_ARRAY   0x0001
#define COLOR_ARRAY    0x0002
#define NORMAL_ARRAY   0x0004
#define TEXCOORD_ARRAY 0x0008

namespace fp {

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glArrayElement(GLint idx);

void glEnableClientState(GLenum array);

void glDisableClientState(GLenum array);

// TODO: support other types? or do we want to assume glshim is helping us?
void glVertexPointer(GLint size, GLenum type, GLsizei stride,
                     const GLvoid *pointer);

void glColorPointer(GLint size, GLenum type, GLsizei stride,
                    const GLvoid *pointer);

void glNormalPointer(GLenum type, GLsizei stride,
                const GLvoid *pointer);
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride,
                       const GLvoid *pointer);
} // namespace fp

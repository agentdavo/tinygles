#pragma once

#include "zgl.hpp"

namespace fp {

void glNormal3x(tGLfixed x, tGLfixed y, tGLfixed z);

void glTexCoord4x(tGLfixed s, tGLfixed t, tGLfixed r, tGLfixed q);

void glEdgeFlag(GLboolean flag);

void glColor4x(tGLfixed r, tGLfixed g, tGLfixed b, tGLfixed a);

void gl_eval_viewport(GLContext * c);

void glBegin(GLenum type);

/* coords, tranformation , clip code and projection */
/* TODO : handle all cases */
static inline void gl_vertex_transform(GLContext *c, GLVertex *v);

void glVertex4x(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w);

void glEnd();

} // namespace fp

#pragma once

#include "zgl.hpp"

#include "vertex.hpp"
#include "matrix.hpp"
#include "light.hpp"
#include "misc.hpp"
#include "clear.hpp"
#include "get.hpp"

namespace fp {

/* glVertex */
void glVertex2x(tGLfixed x, tGLfixed y);

void glVertex3x(tGLfixed x, tGLfixed y, tGLfixed z);

void glVertex3xv(const tGLfixed *v);
/* glNormal */
void glNormal3xv(const tGLfixed *v);

/* glColor */
void glColor4xv(const tGLfixed *v);
void glColor3x(tGLfixed x, tGLfixed y, tGLfixed z);
void glColor3xv(const tGLfixed *v);

/* TexCoord */
void glTexCoord2x(tGLfixed s, tGLfixed t);
void glTexCoord2xv(const tGLfixed *v);

/* Special Functions */
void glFlush();

/* Non standard functions */
void glDebug(int mode);

} // namespace fp

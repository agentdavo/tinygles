#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "glx.hpp"
#include "mygl.h"

#define KEY_UP     0xe000
#define KEY_DOWN   0xe001
#define KEY_LEFT   0xe002
#define KEY_RIGHT  0xe003
#define KEY_ESCAPE 0xe004

namespace fp {

static Display *dpy;
static Window win;

void draw( void );
void idle( void );
GLenum key(int k, GLenum mask);
void reshape( int width, int height );
void init( void );

void tkSwapBuffers();
int ui_loop(int argc, char **argv, const char *name);

} // namespace fp

#ifndef __X11__X11__
#define __X11__X11__

#if defined (__cplusplus)
extern "C" {
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GL/glx.h>
#include <GL/gl.h>
#include "ui.h"

Display *dpy;
Window win;

void tkSwapBuffers();
int ui_loop(int argc, char **argv, const char *name);
int test_clink();

#if defined (__cplusplus)
}
#endif

#endif __X11__X11__

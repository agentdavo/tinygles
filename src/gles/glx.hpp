/* simple glx driver for TinyGL */
#include <GL/glx.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "zgl.hpp"

namespace fp {

typedef struct {
    GLContext *gl_context;
    Display *display;
    XVisualInfo visual_info;
    int xsize, ysize;
    XImage *ximage;
    GC gc;
    Colormap cmap;
    Drawable drawable;
    int do_convert; /* true if must do convertion to X11 format */
    /* shared memory */
    int shm_use;
    XShmSegmentInfo *shm_info;
    int CompletionType;
} TinyGLXContext;

Bool glXQueryExtension(Display *dpy, int *errorb, int *event);


XVisualInfo* glXChooseVisual(Display *dpy, int screen, int *attribList);



GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);


void glXDestroyContext(Display *dpy, GLXContext ctx1);


static int glxXErrorFlag = 0;
static int glxHandleXError(Display *dpy, XErrorEvent *event);

static int bits_per_pixel(Display *dpy, XVisualInfo *visinfo);
static int create_ximage(TinyGLXContext *ctx, int xsize, int ysize, int depth);

static void free_ximage(TinyGLXContext *ctx);

/* resize the glx viewport : we try to use the xsize and ysize
   given. We return the effective size which is guaranted to be smaller */

int glX_resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr);

/* we assume here that drawable is a window */
Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx1);

static Bool WaitForShmCompletion(Display *dpy, XEvent *event, char *arg) ;
void glXSwapBuffers(Display *dpy, GLXDrawable drawable) ;

void glXWaitGL();

void glXWaitX();

} // naemspace fp

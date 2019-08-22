#pragma once

#include "fixed_point_type.hpp"
#include "fixed_point_operations.hpp"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "mygl.h"

#include "zbuffer.hpp"
#include "zmath.hpp"

#ifndef MIN
#define MIN(a, b) (((a) < (b) ? (a) : (b)))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b) ? (a) : (b)))
#endif

/* initially # of allocated GLVertexes (will grow when necessary) */
#define POLYGON_MAX_VERTEX 16

/* Max # of specular light pow buffers */
#define MAX_SPECULAR_BUFFERS 8
/* # of entries in specular buffer */
#define SPECULAR_BUFFER_SIZE 1024
/* specular buffer granularity */
#define SPECULAR_BUFFER_RESOLUTION 1024


#define MAX_MODELVIEW_STACK_DEPTH  64
#define MAX_PROJECTION_STACK_DEPTH 32
#define MAX_TEXTURE_STACK_DEPTH    32

#define MAX_NAME_STACK_DEPTH       64
#define MAX_TEXTURE_LEVELS         11
#define MAX_LIGHTS                 16

#define VERTEX_HASH_SIZE 1031

#define OP_BUFFER_MAX_SIZE 512

#define TGL_OFFSET_FILL    0x1
#define TGL_OFFSET_LINE    0x2
#define TGL_OFFSET_POINT   0x4

#define TGL_PIXEL_ENUM GL_UNSIGNED_BYTE
#define TGL_PIXEL_TYPE GLuint

namespace fp {

typedef struct GLSpecBuf {
    int shininess_i;
    int last_used;
    tGLfixed buf[SPECULAR_BUFFER_SIZE+1];
    struct GLSpecBuf *next;
} GLSpecBuf;

typedef struct GLLight {
    V4 ambient;
    V4 diffuse;
    V4 specular;
    V4 position;
    V3 spot_direction;
    tGLfixed spot_exponent;
    tGLfixed spot_cutoff;
    tGLfixed attenuation[3];
    /* precomputed values */
    tGLfixed cos_spot_cutoff;
    V3 norm_spot_direction;
    V3 norm_position;
    /* we use a linked list to know which are the enabled lights */
    int enabled;
    struct GLLight *next,*prev;
} GLLight;

typedef struct GLMaterial {
    V4 emission;
    V4 ambient;
    V4 diffuse;
    V4 specular;
    tGLfixed shininess;

    /* computed values */
    int shininess_i;
    int do_specular;
} GLMaterial;

typedef struct GLViewport {
    int xmin, ymin, xsize, ysize;
    V3 scale;
    V3 trans;
    int updated;
} GLViewport;

typedef union {
    int op;
    tGLfixed f;
    int i;
    unsigned int ui;
    void *p;
} GLParam;

typedef struct GLParamBuffer {
    GLParam ops[OP_BUFFER_MAX_SIZE];
    struct GLParamBuffer *next;
} GLParamBuffer;

typedef struct GLVertex {
    int edge_flag;
    V3 normal;
    V4 coord;
    V4 tex_coord;
    V4 color;

    /* computed values */
    V4 ec;                /* eye coordinates */
    V4 pc;                /* coordinates in the normalized volume */
    int clip_code;        /* clip code */
    ZBufferPoint zp;      /* integer coordinates for the rasterization */
} GLVertex;

typedef struct GLImage {
    void *pixmap;
    int xsize, ysize;
} GLImage;

/* textures */

#define TEXTURE_HASH_TABLE_SIZE 256

typedef struct GLTexture {
    GLImage images[MAX_TEXTURE_LEVELS];
    int handle;
    struct GLTexture *next,*prev;
} GLTexture;


/* shared state */

typedef struct GLSharedState {
    GLTexture **texture_hash_table;
} GLSharedState;

typedef struct {
    tGLfixed x, y, z;
} GLRasterPos;

typedef struct {
    tGLfixed *p;
    int size;
    int stride;
} GLArray;

struct GLContext;

typedef void (*gl_draw_triangle_func)(struct GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);

/* display context */
typedef struct GLContext {
    /* Z buffer */
    ZBuffer *zb;

    /* shared state */
    GLSharedState shared_state;

    /* viewport */
    GLViewport viewport;

    /* lights */
    struct {
        GLLight lights[MAX_LIGHTS];
        GLLight *first;
        struct {
            V4 ambient;
            int local;
            int two_side;
        } model;
        int enabled;
    } light;

    /* materials */
    struct {
        GLMaterial materials[2];
        struct {
            int enabled;
            int current_mode;
            int current_type;
        } color;
    } material;

    /* textures */
    struct {
        GLTexture *current;
        int enabled_2d;
    } texture;

    /* current list */
    GLParamBuffer *current_op_buffer;
    int current_op_buffer_index;
    int exec_flag, compile_flag, print_flag;

    /* matrix */
    struct {
        int mode;
        M4 *stack[3];
        M4 *stack_ptr[3];
        int stack_depth_max[3];

        M4 model_view_inv;
        M4 model_projection;
        int model_projection_updated;
        int model_projection_no_w_transform;
        int apply_texture;
    } matrix;

    /* current state */
    int polygon_mode_back;
    int polygon_mode_front;

    int current_front_face;
    int current_shade_model;
    int current_cull_face;
    int cull_face_enabled;
    int normalize_enabled;
    gl_draw_triangle_func draw_triangle_front, draw_triangle_back;

    /* clear */
    struct {
        tGLfixed depth;
        V4 color;
    } clear;

    /* glBegin / glEnd */
    int in_begin;
    int begin_type;
    int vertex_n, vertex_cnt;
    int vertex_max;
    GLVertex *vertex;

    /* current vertex state */
    struct {
        V4 color;
        unsigned int longcolor[3]; /* precomputed integer color */
        V4 normal;
        V4 tex_coord;
        int edge_flag;
    } current;

    /* opengl 1.1 arrays  */
    struct {
        GLArray vertex,
                normal,
                color,
                tex_coord;
    } array;
    int client_states;

    /* opengl 1.1 polygon offset */
    struct {
        tGLfixed factor;
        tGLfixed units;
        int states;
    } offset;

    /* specular buffer. could probably be shared between contexts,
       but that wouldn't be 100% thread safe */
    GLSpecBuf *specbuf_first;
    int specbuf_used_counter;
    int specbuf_num_buffers;

    /* opaque structure for user's use */
    void *opaque;

    /* resize viewport function */
    int (*gl_resize_viewport)(struct GLContext *c, int *xsize, int *ysize);

    /* depth test */
    int depth_test;

    struct {
        int dfactor;
        int sfactor;
        int enabled;
    } blend;

    struct {
        int func;
        int ref;
    } alpha;

    struct {
        int op;
    } logic;

    // TODO: glPushAttrib
    GLRasterPos raster_pos;
} GLContext;

extern GLContext *gl_ctx;

/* clip.c */
void gl_transform_to_viewport(GLContext *c, GLVertex *v);
void gl_draw_triangle(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
void gl_draw_line(GLContext *c, GLVertex *p0, GLVertex *p1);
void gl_draw_point(GLContext *c, GLVertex *p0);

void gl_draw_triangle_point(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
void gl_draw_triangle_line(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
void gl_draw_triangle_fill(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);

///* matrix.c */
//void gl_print_matrix(const tGLfixed *m);

/* light.c */
void gl_shade_vertex(GLContext *c, GLVertex *v);

void glInitTextures(GLContext *c);
void glEndTextures(GLContext *c);
GLTexture *alloc_texture(GLContext *c, int h);

/* image_util.c */
//void gl_convertRGB_to_5R6G5B(unsigned short *pixmap, unsigned char *rgb, int xsize, int ysize);
//void gl_convertRGB_to_8A8R8G8B(unsigned int *pixmap, unsigned char *rgb, int xsize, int ysize);
//void gl_resizeImage(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);
//void gl_resizeImageNoInterpolate(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);

GLContext *gl_get_context(void);

void glClose(void);
void glInit(void *zbuffer1);

/* specular buffer "api" */
//GLSpecBuf *specbuf_get_buffer(GLContext *c, const int shininess_i, const tGLfixed shininess);

/* this clip epsilon is needed to avoid some rounding errors after
   several clipping stages */

#define CLIP_EPSILON (1E-5)

static inline int gl_clipcode(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w1) {
    tGLfixed w;

    w = w1 * (1.0 + CLIP_EPSILON);
    return (x < -w)     |
        ((x > w)  << 1) |
        ((y < -w) << 2) |
        ((y > w)  << 3) |
        ((z < -w) << 4) |
        ((z > w)  << 5);
}

} // namespace fp

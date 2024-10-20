// zgl.hpp

#pragma once

#include "fixed_point_type.hpp"
#include "fixed_point_operations.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "mygl.h"
#include "zbuffer.hpp"
#include "zmath.hpp"
#include "texture.hpp" // Include texture definitions

#ifndef MIN
#define MIN(a, b) (((a) < (b) ? (a) : (b)))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b) ? (a) : (b)))
#endif

// Define constants and macros
#define POLYGON_MAX_VERTEX 16
#define MAX_SPECULAR_BUFFERS 8
#define SPECULAR_BUFFER_SIZE 1024
#define SPECULAR_BUFFER_RESOLUTION 1024

#define MAX_MODELVIEW_STACK_DEPTH 64
#define MAX_PROJECTION_STACK_DEPTH 32
#define MAX_TEXTURE_STACK_DEPTH 32

#define MAX_NAME_STACK_DEPTH 64
#define MAX_LIGHTS 16

#define VERTEX_HASH_SIZE 1031

#define OP_BUFFER_MAX_SIZE 512

#define TGL_OFFSET_FILL 0x1
#define TGL_OFFSET_LINE 0x2
#define TGL_OFFSET_POINT 0x4

#define TGL_PIXEL_ENUM GL_UNSIGNED_BYTE
#define TGL_PIXEL_TYPE GLuint

namespace fp
{

    typedef struct GLSpecBuf
    {
        int shininess_i;
        int last_used;
        tGLfixed buf[SPECULAR_BUFFER_SIZE + 1];
        struct GLSpecBuf *next;

        GLSpecBuf()
            : shininess_i(0), last_used(0), next(nullptr)
        {
            // Initialize buf to zero
            memset(buf, 0, sizeof(buf));
        }
    } GLSpecBuf;

    struct GLLight
    {
        V4 ambient;
        V4 diffuse;
        V4 specular;
        V4 position;
        V3 spot_direction;
        tGLfixed spot_exponent;
        tGLfixed spot_cutoff;
        tGLfixed attenuation[3];

        // Precomputed values
        tGLfixed cos_spot_cutoff;
        V3 norm_spot_direction;
        V3 norm_position;

        // Linked list for enabled lights
        int enabled;
        GLLight *next;
        GLLight *prev;

        GLLight()
            : ambient(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              diffuse(tGLfixed(1), tGLfixed(1), tGLfixed(1), tGLfixed(1)),
              specular(tGLfixed(1), tGLfixed(1), tGLfixed(1), tGLfixed(1)),
              position(tGLfixed(0), tGLfixed(0), tGLfixed(1), tGLfixed(0)),
              spot_direction(tGLfixed(0), tGLfixed(0), tGLfixed(-1)),
              spot_exponent(tGLfixed(0)),
              spot_cutoff(tGLfixed(180)),
              cos_spot_cutoff(tGLfixed(-1)),
              norm_spot_direction(tGLfixed(0), tGLfixed(0), tGLfixed(-1)),
              norm_position(tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              enabled(0),
              next(nullptr),
              prev(nullptr)
        {
            attenuation[0] = tGLfixed(1);
            attenuation[1] = tGLfixed(0);
            attenuation[2] = tGLfixed(0);
        }
    };

    struct GLLightModel
    {
        V4 ambient;
        int local;
        int two_side;

        GLLightModel()
            : ambient(tGLfixed(0.2f), tGLfixed(0.2f), tGLfixed(0.2f), tGLfixed(1.0f)),
              local(0),
              two_side(0)
        {
        }
    };

    struct GLMaterial
    {
        V4 emission;
        V4 ambient;
        V4 diffuse;
        V4 specular;
        tGLfixed shininess;

        // Computed values
        int shininess_i;
        int do_specular;

        GLMaterial()
            : emission(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              ambient(tGLfixed(0.2f), tGLfixed(0.2f), tGLfixed(0.2f), tGLfixed(1)),
              diffuse(tGLfixed(0.8f), tGLfixed(0.8f), tGLfixed(0.8f), tGLfixed(1)),
              specular(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              shininess(tGLfixed(0)),
              shininess_i(0),
              do_specular(0)
        {
        }
    };

    struct GLMaterialColor
    {
        int enabled;
        int current_mode;
        int current_type;

        GLMaterialColor()
            : enabled(0),
              current_mode(GL_FRONT_AND_BACK),
              current_type(GL_AMBIENT_AND_DIFFUSE)
        {
        }
    };

    struct GLMatrixState
    {
        int mode;
        M4 *stack[3];
        M4 *stack_ptr[3];
        int stack_depth_max[3];

        M4 model_view_inv;
        M4 model_projection;
        int model_projection_updated;
        int model_projection_no_w_transform;
        int apply_texture;

        GLMatrixState()
            : mode(GL_MODELVIEW),
              model_projection_updated(0),
              model_projection_no_w_transform(0),
              apply_texture(0)
        {
            for (int i = 0; i < 3; ++i)
            {
                stack[i] = nullptr;
                stack_ptr[i] = nullptr;
                stack_depth_max[i] = 0;
            }
        }
    };

    struct GLCurrentState
    {
        V4 color;
        unsigned int longcolor[3]; // Precomputed integer color
        V4 normal;
        V4 tex_coord;
        int edge_flag;

        GLCurrentState()
            : color(tGLfixed(1), tGLfixed(1), tGLfixed(1), tGLfixed(1)),
              normal(tGLfixed(1), tGLfixed(0), tGLfixed(0), tGLfixed(0)),
              tex_coord(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              edge_flag(1)
        {
            longcolor[0] = 65535;
            longcolor[1] = 65535;
            longcolor[2] = 65535;
        }
    };

    struct GLViewport
    {
        int xmin, ymin, xsize, ysize;
        V3 scale;
        V3 trans;
        int updated;

        GLViewport()
            : xmin(0), ymin(0), xsize(0), ysize(0),
              scale(tGLfixed(1.0f), tGLfixed(1.0f), tGLfixed(1.0f)),
              trans(tGLfixed(0.0f), tGLfixed(0.0f), tGLfixed(0.0f)),
              updated(0)
        {
        }
    };

    union GLParam
    {
        int op;
        float f;
        int i;
        unsigned int ui;
        void *p;

        GLParam() : op(0) {} // Default constructor
    };

    struct GLParamBuffer
    {
        GLParam ops[OP_BUFFER_MAX_SIZE];
        struct GLParamBuffer *next;

        GLParamBuffer()
            : next(nullptr)
        {
            // Initialize ops to zero
            memset(ops, 0, sizeof(ops));
        }
    };

    struct GLVertex
    {
        int edge_flag;
        V3 normal;
        V4 coord;
        V4 tex_coord;
        V4 color;

        // Computed values
        V4 ec;           // Eye coordinates
        V4 pc;           // Coordinates in the normalized volume
        int clip_code;   // Clip code
        ZBufferPoint zp; // Integer coordinates for rasterization

        GLVertex()
            : edge_flag(1),
              normal(tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              coord(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              tex_coord(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              color(tGLfixed(1), tGLfixed(1), tGLfixed(1), tGLfixed(1)),
              ec(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              pc(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(1)),
              clip_code(0),
              zp()
        {
        }
    };

    struct GLRasterPos
    {
        tGLfixed x, y, z;

        GLRasterPos()
            : x(tGLfixed(0)),
              y(tGLfixed(0)),
              z(tGLfixed(0))
        {
        }
    };

    struct GLArray
    {
        tGLfixed *p;
        int size;
        int stride;

        GLArray()
            : p(nullptr),
              size(0),
              stride(0)
        {
        }
    };

    typedef void (*gl_draw_triangle_func)(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);

    /* Display context */
    struct GLContext
    {
        /* Z buffer */
        ZBuffer *zb;

        /* Shared state */
        GLSharedState shared_state;

        /* Viewport */
        GLViewport viewport;

        /* Lights */
        struct LightContext
        {
            GLLight lights[MAX_LIGHTS];
            GLLight *first;
            GLLightModel model;
            int enabled;

            LightContext()
                : first(nullptr), enabled(0)
            {
                // Lights are initialized individually by their constructors
            }
        } light;

        /* Materials */
        struct MaterialContext
        {
            GLMaterial materials[2];
            GLMaterialColor color;

            MaterialContext()
            {
                // Materials are initialized individually by their constructors
            }
        } material;

        /* Texture state */
        GLTextureState texture;

        /* Error handling */
        GLenum error; // Last error code

        /* Pixel storage modes */
        GLint unpack_alignment; // Pixel storage unpack alignment

        /* Current list */
        GLParamBuffer *current_op_buffer;
        int current_op_buffer_index;
        int exec_flag, compile_flag, print_flag;

        /* Matrix */
        GLMatrixState matrix;

        /* Current vertex state */
        GLCurrentState current;

        /* Current state */
        int polygon_mode_back;
        int polygon_mode_front;

        int current_front_face;
        int current_shade_model;
        int current_cull_face;
        int cull_face_enabled;
        int normalize_enabled;
        gl_draw_triangle_func draw_triangle_front;
        gl_draw_triangle_func draw_triangle_back;

        /* Clear */
        struct ClearState
        {
            tGLfixed depth;
            V4 color;

            ClearState()
                : depth(tGLfixed(1.0f)),
                  color(tGLfixed(0), tGLfixed(0), tGLfixed(0), tGLfixed(0))
            {
            }
        } clear;

        /* glBegin / glEnd */
        int in_begin;
        int begin_type;
        int vertex_n, vertex_cnt;
        int vertex_max;
        GLVertex *vertex;

        /* OpenGL 1.1 arrays */
        struct ArrayState
        {
            GLArray vertex;
            GLArray normal;
            GLArray color;
            GLArray tex_coord;

            ArrayState()
            {
                // Arrays are initialized by their constructors
            }
        } array;
        int client_states;

        /* OpenGL 1.1 polygon offset */
        struct GLOffset
        {
            tGLfixed factor;
            tGLfixed units;
            int states;

            GLOffset()
                : factor(tGLfixed(0)),
                  units(tGLfixed(0)),
                  states(0)
            {
            }
        } offset;

        /* Specular buffer */
        GLSpecBuf *specbuf_first;
        int specbuf_used_counter;
        int specbuf_num_buffers;

        /* Opaque structure for user's use */
        void *opaque;

        /* Resize viewport function */
        int (*gl_resize_viewport)(GLContext *c, int *xsize, int *ysize);

        /* Depth test */
        int depth_test;

        /* Blending */
        struct GLBlend
        {
            int dfactor;
            int sfactor;
            int enabled;

            GLBlend()
                : dfactor(GL_ONE_MINUS_SRC_ALPHA),
                  sfactor(GL_SRC_ALPHA),
                  enabled(0)
            {
            }
        } blend;

        /* Alpha test */
        struct GLAlphaTest
        {
            int func;
            int ref;

            GLAlphaTest()
                : func(GL_ALWAYS),
                  ref(0)
            {
            }
        } alpha;

        /* Logic operation */
        struct GLLogicOp
        {
            int op;

            GLLogicOp()
                : op(GL_COPY)
            {
            }
        } logic;

        /* Raster position */
        GLRasterPos raster_pos;

        GLint textsize;

        /* Constructor */
        GLContext()
            : zb(nullptr),
              shared_state(),
              viewport(),
              light(),
              material(),
              texture(),
              error(GL_NO_ERROR),
              unpack_alignment(4),
              current_op_buffer(nullptr),
              current_op_buffer_index(0),
              exec_flag(1),
              compile_flag(0),
              print_flag(0),
              matrix(),
              current(),
              polygon_mode_back(GL_FILL),
              polygon_mode_front(GL_FILL),
              current_front_face(GL_CCW),
              current_shade_model(GL_SMOOTH),
              current_cull_face(GL_BACK),
              cull_face_enabled(0),
              normalize_enabled(0),
              draw_triangle_front(nullptr),
              draw_triangle_back(nullptr),
              clear(),
              in_begin(0),
              begin_type(0),
              vertex_n(0),
              vertex_cnt(0),
              vertex_max(0),
              vertex(nullptr),
              array(),
              client_states(0),
              offset(),
              specbuf_first(nullptr),
              specbuf_used_counter(0),
              specbuf_num_buffers(0),
              opaque(nullptr),
              gl_resize_viewport(nullptr),
              depth_test(0),
              blend(),
              alpha(),
              logic(),
              raster_pos(),
              textsize(1)
        {
            // Initialize other members if necessary
        }
    };

    extern GLContext *gl_ctx;

    /* Function declarations */

    /* clip.c */
    void gl_transform_to_viewport(GLContext *c, GLVertex *v);
    void gl_draw_triangle(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
    void gl_draw_line(GLContext *c, GLVertex *p0, GLVertex *p1);
    void gl_draw_point(GLContext *c, GLVertex *p0);

    void gl_draw_triangle_point(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
    void gl_draw_triangle_line(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);
    void gl_draw_triangle_fill(GLContext *c, GLVertex *p0, GLVertex *p1, GLVertex *p2);

    /* matrix.c */
    void gl_print_matrix(const tGLfixed *m);

    /* light.c */
    void gl_shade_vertex(GLContext *c, GLVertex *v);

    /* texture functions are declared in texture.hpp */

    /* image_util.c */
    void gl_convertRGB_to_5R6G5B(unsigned short *pixmap, unsigned char *rgb, int xsize, int ysize);
    void gl_convertRGB_to_8A8R8G8B(unsigned int *pixmap, unsigned char *rgb, int xsize, int ysize);
    void gl_resizeImage(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);
    void gl_resizeImageNoInterpolate(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);

    /* Context management */
    GLContext *gl_get_context(void);
    void glClose(void);
    void glInit(void *zbuffer1);

    /* Specular buffer API */
    GLSpecBuf *specbuf_get_buffer(GLContext *c, int shininess_i, tGLfixed shininess);

    /* Clip epsilon */
#define CLIP_EPSILON (1E-5)

    static inline int gl_clipcode(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w1)
    {
        tGLfixed w = w1 * (tGLfixed(1.0) + tGLfixed(CLIP_EPSILON));
        return (x < -w) |
               ((x > w) << 1) |
               ((y < -w) << 2) |
               ((y > w) << 3) |
               ((z < -w) << 4) |
               ((z > w) << 5);
    }

} // namespace fp

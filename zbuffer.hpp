#pragma once

#include "fixed_point_type.hpp"

/*
 * Z buffer
 */

#define ZB_Z_BITS 16

#define ZB_POINT_Z_FRAC_BITS 14

#define ZB_POINT_S_MIN ( (1<<13) )
#define ZB_POINT_S_MAX ( (1<<22)-(1<<13) )
#define ZB_POINT_T_MIN ( (1<<21) )
#define ZB_POINT_T_MAX ( (1<<30)-(1<<21) )

#define ZB_POINT_RED_MIN ( (1<<10) )
#define ZB_POINT_RED_MAX ( (1<<16)-(1<<10) )
#define ZB_POINT_GREEN_MIN ( (1<<9) )
#define ZB_POINT_GREEN_MAX ( (1<<16)-(1<<9) )
#define ZB_POINT_BLUE_MIN ( (1<<10) )
#define ZB_POINT_BLUE_MAX ( (1<<16)-(1<<10) )

/* display modes */
#define ZB_MODE_5R6G5B  1  /* true color 16 bits */
#define ZB_MODE_INDEX   2  /* color index 8 bits */
#define ZB_MODE_RGBA    3  /* 32 bit rgba mode */
#define ZB_MODE_RGB24   4  /* 24 bit rgb mode */
#define ZB_NB_COLORS    225 /* number of colors for 8 bit display */

#define RGB_TO_PIXEL(r, g, b) \
  ((((r) << 8) & 0xff0000) | ((g) & 0xff00) | ((b) >> 8))

#define RGB32_TO_RGB16(v) \
  (((v >> 8) & 0xf800) | (((v) >> 5) & 0x07e0) | (((v) & 0xff) >> 3))

typedef uint32_t PIXEL;

#define PSZB 4
#define PSZSH 5

namespace fp {

typedef struct {
    int xsize, ysize;
    int linesize; /* line size, in bytes */
    int mode;

    unsigned short *zbuf;
    PIXEL *pbuf;
    int frame_buffer_allocated;

    int nb_colors;
    unsigned char *dctable;
    int *ctable;
    PIXEL *current_texture;
} ZBuffer;

typedef struct {
    int x, y, z;     /* integer coordinates in the zbuffer */
    int s, t;       /* coordinates for the mapping */
    int r, g, b;     /* color indexes */

    tGLfixed sz, tz;   /* temporary coordinates for mapping */
} ZBufferPoint;

/* zbuffer.c */

ZBuffer *ZB_open(int xsize, int ysize, int mode,
                 int nb_colors,
                 unsigned char *color_indexes,
                 int *color_table,
                 void *frame_buffer);


void ZB_close(ZBuffer *zb);

void ZB_resize(ZBuffer *zb, void *frame_buffer, int xsize, int ysize);
void ZB_clear(ZBuffer *zb, int clear_z, int z, int clear_color, int r, int g, int b);

/* linesize is in BYTES */
void ZB_copyFrameBuffer(ZBuffer *zb, void *buf, int linesize);

/* zdither.c */

void ZB_initDither(ZBuffer *zb, int nb_colors, unsigned char *color_indexes, int *color_table);
void ZB_closeDither(ZBuffer *zb);
void ZB_ditherFrameBuffer(ZBuffer *zb, unsigned char *dest, int linesize);

/* zline.c */

void ZB_plot(ZBuffer *zb, ZBufferPoint *p);
void ZB_line(ZBuffer *zb, ZBufferPoint *p1, ZBufferPoint *p2);
void ZB_line_z(ZBuffer *zb, ZBufferPoint *p1, ZBufferPoint *p2);

/* ztriangle.c */

void ZB_setTexture(ZBuffer *zb, PIXEL *texture);

void ZB_fillTriangleFlat(ZBuffer *zb, ZBufferPoint *p1, ZBufferPoint *p2, ZBufferPoint *p3);

void ZB_fillTriangleSmooth(ZBuffer *zb, ZBufferPoint *p1, ZBufferPoint *p2, ZBufferPoint *p3);

void ZB_fillTriangleMapping(ZBuffer *zb, ZBufferPoint *p1, ZBufferPoint *p2, ZBufferPoint *p3);

void ZB_fillTriangleMappingPerspective(ZBuffer *zb, ZBufferPoint *p0, ZBufferPoint *p1, ZBufferPoint *p2);


typedef void (*ZB_fillTriangleFunc)(ZBuffer *, ZBufferPoint *, ZBufferPoint *, ZBufferPoint *);

} // namespace fp

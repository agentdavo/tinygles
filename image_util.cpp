#include "zgl.hpp"

/*
 * image conversion
 */

namespace fp {

void gl_convertRGB_to_5R6G5B(unsigned short *pixmap, unsigned char *rgb, int xsize, int ysize) {
    int i, n;
    unsigned char *p;

    p = rgb;
    n = xsize * ysize;
    for (i = 0;i < n; i++) {
        pixmap[i] = ((p[0] & 0xF8) << 8) | (( p[1] & 0xFC) << 3) | ((p[2] & 0xF8) >> 3);
        p+=3;
    }
}

void gl_convertRGB_to_8A8R8G8B(unsigned int *pixmap, unsigned char *rgb, int xsize, int ysize) {
    int i, n;
    unsigned char *p;

    p = rgb;
    n = xsize * ysize;
    for (i = 0; i < n; i++) {
        pixmap[i] = (((unsigned int)p[0]) << 16) |
                    (((unsigned int)p[1]) << 8) |
                    (((unsigned int)p[2]));
        p+=3;
    }
}

/*
 * linear interpolation with xf, yf normalized to 2^16
 */

#define INTERP_NORM_BITS  16
#define INTERP_NORM       (1 << INTERP_NORM_BITS)

static inline int interpolate(int v00, int v01, int v10, int xf, int yf) {
    return v00 + (((v01 - v00)*xf + (v10 - v00) * yf) >> INTERP_NORM_BITS);
}

///* resizing with no interlating nor nearest pixel */
void gl_resizeImageNoInterpolate(unsigned char *dst, int dst_width, int dst_height,
                                 const unsigned char *src, int src_width, int src_height)
{
    int x_ratio = (src_width << 16) / dst_width + 1;
    int y_ratio = (src_height << 16) / dst_height + 1;
    int x, y;

    for (int i = 0; i < dst_height; i++) {
        for (int j = 0; j < dst_width; j++) {
            x = (j * x_ratio) >> 16;
            y = (i * y_ratio) >> 16;
            dst[(i * dst_width + j) * 3 + 0] = src[(y * src_width + x) * 3 + 0];
            dst[(i * dst_width + j) * 3 + 1] = src[(y * src_width + x) * 3 + 1];
            dst[(i * dst_width + j) * 3 + 2] = src[(y * src_width + x) * 3 + 2];
        }
    }
}


} // namespace fp

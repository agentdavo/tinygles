#pragma once

namespace fp {
	
void gl_convertRGB_to_5R6G5B(unsigned short *pixmap, unsigned char *rgb, int xsize, int ysize);
void gl_convertRGB_to_8A8R8G8B(unsigned int *pixmap, unsigned char *rgb, int xsize, int ysize);
// todo(alpi): temprorary commented
//void gl_resizeImage(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);
//void gl_resizeImageNoInterpolate(unsigned char *dest, int xsize_dest, int ysize_dest, unsigned char *src, int xsize_src, int ysize_src);

} // namespace fp

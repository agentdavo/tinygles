#pragma once

#include "mygl.h"  // OpenGL ES 1.1 headers
#include <vector>  // For std::vector

namespace fp
{
    // Constants for the font texture atlas
    const int ATLAS_WIDTH = 128;
    const int ATLAS_HEIGHT = 64;  // Adjusted to fit 128 characters in 8 rows
    const int GLYPH_WIDTH = 8;
    const int GLYPH_HEIGHT = 8;
    const int GLYPHS_PER_ROW = 16;
    const int GLYPHS_PER_COLUMN = 8;  // 128 characters / 16 columns

    // Initializes the font texture atlas.
    void initFontTexture();

    // Sets the size of the text to be drawn (scaling factor).
    void glTextSize(GLint mode);

    // Draws a string of text at the specified position with the given color.
    void glDrawText(const char* text, GLint x, GLint y, GLuint color);

    // Declare createFontTextureAtlas
    std::vector<GLubyte> createFontTextureAtlas();
} // namespace fp

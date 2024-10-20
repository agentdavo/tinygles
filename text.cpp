#include "zgl.hpp"
#include "text.hpp"
#include "font8x8_basic.h"
#include <vector>
#include <cstdint>

namespace fp
{

    // Declare createFontTextureAtlas
    std::vector<GLubyte> createFontTextureAtlas();

    GLuint fontTextureID = 0;
    static GLint textSize = 1;  // Text size scaling factor, initially set to 1

    void initFontTexture() {
        std::vector<GLubyte> atlasData = createFontTextureAtlas();

        glGenTextures(1, &fontTextureID);
        glBindTexture(GL_TEXTURE_2D, fontTextureID);

        // Set texture parameters
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Upload texture data using GL_RGB and GL_UNSIGNED_BYTE
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, atlasData.data());
    }

    std::vector<GLubyte> createFontTextureAtlas() {
        // Create an RGB texture atlas initialized with black pixels
        std::vector<GLubyte> atlasData(ATLAS_WIDTH * ATLAS_HEIGHT * 3, 0);  // Initialize with zeros (black)

        for (int c = 0; c < 128; ++c) {  // For each ASCII character
            int glyphX = (c % GLYPHS_PER_ROW) * GLYPH_WIDTH;
            int glyphY = (c / GLYPHS_PER_ROW) * GLYPH_HEIGHT;

            for (int y = 0; y < GLYPH_HEIGHT; ++y) {
                for (int x = 0; x < GLYPH_WIDTH; ++x) {
                    if (font8x8_basic[c][y] & (1 << x)) {
                        int texX = glyphX + x;
                        int texY = glyphY + y;
                        int index = (texY * ATLAS_WIDTH + texX) * 3;  // Multiply by 3 for RGB
                        atlasData[index] = 0xFF;     // Red channel
                        atlasData[index + 1] = 0xFF; // Green channel
                        atlasData[index + 2] = 0xFF; // Blue channel
                    }
                }
            }
        }

        return atlasData;
    }

    // Sets the size of the text to be drawn (scaling factor)
    void glTextSize(GLint mode)
    {
        if (mode >= 1 && mode <= 16)  // Ensure mode is within a valid range
        {
            textSize = mode;
        }
    }

    // Draws a string of text on the screen using OpenGL ES 1.1
    void glDrawText(const char* text, GLint x, GLint y, GLuint color) {
        GLint xOff = 0, yOff = 0;
        int i = 0;

        // Enable necessary client states
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // Set up blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, fontTextureID);

        // Set the color for the text (only RGB components are used)
        glColor4ub(
            (color >> 24) & 0xFF,  // Red
            (color >> 16) & 0xFF,  // Green
            (color >> 8) & 0xFF,   // Blue
            0xFF                   // Alpha set to 255 (opaque)
        );

        std::vector<GLfixed> vertices;
        std::vector<GLfixed> texCoords;

        while (text[i] != '\0') {
            if (text[i] == '\n') {
                xOff = 0;
                yOff += GLYPH_HEIGHT * textSize;
            } else {
                // Calculate vertex positions using GLint
                GLint fx = x + xOff;
                GLint fy = y + yOff;
                GLint fw = GLYPH_WIDTH * textSize;
                GLint fh = GLYPH_HEIGHT * textSize;

                // Convert positions to GLfixed by shifting left 16 bits
                GLfixed fx_fixed = fx << 16;
                GLfixed fy_fixed = fy << 16;
                GLfixed fw_fixed = fw << 16;
                GLfixed fh_fixed = fh << 16;

                // Append vertex positions (two triangles per quad)
                vertices.insert(vertices.end(), {
                    // First triangle
                    fx_fixed, fy_fixed,
                    fx_fixed + fw_fixed, fy_fixed,
                    fx_fixed, fy_fixed + fh_fixed,

                    // Second triangle
                    fx_fixed + fw_fixed, fy_fixed,
                    fx_fixed + fw_fixed, fy_fixed + fh_fixed,
                    fx_fixed, fy_fixed + fh_fixed
                });

                // Calculate texture coordinates
                unsigned char c = text[i];
                int glyphX = (c % GLYPHS_PER_ROW) * GLYPH_WIDTH;
                int glyphY = (c / GLYPHS_PER_ROW) * GLYPH_HEIGHT;

                // Convert texture coordinates to GLfixed in [0, 1<<16] range
                GLfixed s0 = (glyphX << 16) / ATLAS_WIDTH;
                GLfixed t0 = (glyphY << 16) / ATLAS_HEIGHT;
                GLfixed s1 = ((glyphX + GLYPH_WIDTH) << 16) / ATLAS_WIDTH;
                GLfixed t1 = ((glyphY + GLYPH_HEIGHT) << 16) / ATLAS_HEIGHT;

                // Invert t-coordinates because OpenGL's texture origin is at the bottom-left
                t0 = (1 << 16) - t0;
                t1 = (1 << 16) - t1;

                // Append texture coordinates
                texCoords.insert(texCoords.end(), {
                    // First triangle
                    s0, t1,
                    s1, t1,
                    s0, t0,

                    // Second triangle
                    s1, t1,
                    s1, t0,
                    s0, t0
                });

                xOff += GLYPH_WIDTH * textSize;
            }
            i++;
        }

        // Set up vertex and texture coordinate pointers
        glVertexPointer(2, GL_FIXED, 0, vertices.data());
        glTexCoordPointer(2, GL_FIXED, 0, texCoords.data());

        // Draw all character quads in one call
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 2));

        // Disable client states
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

}  // namespace fp

// texture.hpp

#pragma once

#include "mygl.h"     // Include OpenGL ES 1.1 headers for types like GLenum, GLuint, etc.
#include <cstddef>    // For size_t

namespace fp {

// Define constants
#define TEXTURE_HASH_TABLE_SIZE 256
#define MAX_TEXTURE_LEVELS 10

// Forward declaration of GLContext
struct GLContext;

// Structure to hold texture image data
struct GLImage {
    GLsizei xsize;
    GLsizei ysize;
    GLint internal_format;
    GLenum format;
    GLenum type;
    unsigned char *pixmap;  // Pointer to the pixel data

    GLImage() : xsize(0), ysize(0), internal_format(0), format(0), type(0), pixmap(nullptr) {}
};

// Texture object structure
struct GLTexture {
    GLuint handle;                // Texture name/ID
    GLint wrap_s;                 // Texture wrapping mode for S coordinate
    GLint wrap_t;                 // Texture wrapping mode for T coordinate
    GLint min_filter;             // Minification filter
    GLint mag_filter;             // Magnification filter
    bool is_complete;             // Indicates if the texture is complete (level 0 defined)

    GLTexture *next;              // Next texture in the hash table linked list
    GLTexture *prev;              // Previous texture in the hash table linked list

    GLImage images[MAX_TEXTURE_LEVELS];  // Array of mipmap levels

    GLTexture() : handle(0), wrap_s(GL_REPEAT), wrap_t(GL_REPEAT),
                  min_filter(GL_NEAREST_MIPMAP_LINEAR), mag_filter(GL_LINEAR),
                  is_complete(false), next(nullptr), prev(nullptr) {}
};

struct GLSharedState {
    GLTexture *texture_hash_table[TEXTURE_HASH_TABLE_SIZE];

    GLSharedState() {
        for (int i = 0; i < TEXTURE_HASH_TABLE_SIZE; ++i) {
            texture_hash_table[i] = nullptr;
        }
    }
};

// Texture state within a context
struct GLTextureState {
    GLTexture *current;       // Currently bound texture
    GLenum env_mode;          // Texture environment mode (e.g., GL_MODULATE)
    int enabled_2d;

    GLTextureState() : current(nullptr), env_mode(GL_MODULATE) {}
};

// Function declarations

void glGenTextures(GLsizei n, GLuint *textures);
void glDeleteTextures(GLsizei n, const GLuint *textures);
void glBindTexture(GLenum target, GLuint texture);
GLboolean glIsTexture(GLuint texture);

void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                  GLsizei width, GLsizei height, GLint border,
                  GLenum format, GLenum type, const GLvoid *pixels);

void glTexEnvx(GLenum target, GLenum pname, GLenum param);
void glTexEnvi(GLenum target, GLenum pname, GLint param);

void glTexParameteri(GLenum target, GLenum pname, GLint param);
void glPixelStorei(GLenum pname, GLint param);

// Internal helper functions (if needed)
// These may not need to be exposed publicly, but included here for completeness
GLTexture *alloc_texture(GLContext *c, GLuint h);
GLTexture *find_texture(GLContext *c, GLuint h);
void free_texture(GLContext *c, GLuint h);
void gl_error(GLenum error_code, const char *message);

}  // namespace fp

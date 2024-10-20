// texture.cpp

#include "texture.hpp"
#include "zgl.hpp" // Include OpenGL context and helper functions
#include <cstdlib> // For calloc, free, malloc
#include <cstring> // For memcpy
#include <cstdio>  // For fprintf
#include <cassert> // For assert (used sparingly)
#include <cmath>   // For floorf, fmodf, fabsf, etc.
#include "mygl.h"  // Include OpenGL context and helper functions

// Define maximum number of texture levels (e.g., for mipmapping)
#define MAX_TEXTURE_LEVELS 10

namespace fp
{

    GLTexture *find_texture(GLContext *c, GLuint h)
    {
        if (h == 0)
            return NULL; // Texture name 0 refers to the default texture object

        GLTexture *t = c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];
        while (t != NULL)
        {
            if (t->handle == h)
                return t;
            t = t->next;
        }
        return NULL;
    }

    void free_texture(GLContext *c, GLuint h)
    {
        GLTexture *t = find_texture(c, h);
        if (!t)
            return; // Exit early if texture not found
        GLTexture **ht = &c->shared_state.texture_hash_table[t->handle % TEXTURE_HASH_TABLE_SIZE];

        if (t->prev == NULL)
        {
            *ht = t->next;
        }
        else
        {
            t->prev->next = t->next;
        }
        if (t->next != NULL)
        {
            t->next->prev = t->prev;
        }

        for (int i = 0; i < MAX_TEXTURE_LEVELS; i++)
        {
            if (t->images[i].pixmap != NULL)
            {
                free(t->images[i].pixmap);
                t->images[i].pixmap = NULL;
            }
        }

        free(t);
    }

    GLTexture *alloc_texture(GLContext *c, GLuint h)
    {
        GLTexture *t = (GLTexture *)calloc(1, sizeof(GLTexture));
        if (t == NULL)
        {
            // Handle memory allocation failure
            gl_error(GL_OUT_OF_MEMORY, "alloc_texture: failed to allocate memory for texture");
            return NULL;
        }

        GLTexture **ht = &c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];

        t->next = *ht;
        if (*ht != NULL)
        {
            (*ht)->prev = t;
        }
        *ht = t;
        t->handle = h;

        // Initialize texture parameters to default values
        t->wrap_s = GL_REPEAT;
        t->wrap_t = GL_REPEAT;
        t->min_filter = GL_NEAREST;
        t->mag_filter = GL_NEAREST;

        return t;
    }

    // Global variable for generating unique texture handles
    static GLuint next_texture_handle = 1; // Start from 1, since 0 is reserved

    void glGenTextures(GLsizei n, GLuint *textures)
    {
        if (n < 0 || textures == NULL)
        {
            gl_error(GL_INVALID_VALUE, "glGenTextures: n < 0 or textures is NULL");
            return;
        }

        for (GLsizei i = 0; i < n; i++)
        {
            textures[i] = next_texture_handle++;
        }
    }

    void glDeleteTextures(GLsizei n, const GLuint *textures)
    {
        if (n < 0 || textures == NULL)
        {
            gl_error(GL_INVALID_VALUE, "glDeleteTextures: n < 0 or textures is NULL");
            return;
        }

        GLContext *c = gl_get_context();
        for (GLsizei i = 0; i < n; i++)
        {
            GLuint texture = textures[i];
            if (texture == 0)
            {
                continue; // Texture name 0 is the default texture and cannot be deleted
            }

            GLTexture *t = find_texture(c, texture);
            if (t != NULL)
            {
                if (t == c->texture.current)
                {
                    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture if it's currently bound
                }
                free_texture(c, texture);
            }
        }
    }

    void glBindTexture(GLenum target, GLuint texture)
    {
        GLContext *c = gl_get_context();

        if (target != GL_TEXTURE_2D)
        {
            gl_error(GL_INVALID_ENUM, "glBindTexture: target must be GL_TEXTURE_2D");
            return;
        }

        GLTexture *t = NULL;
        if (texture != 0)
        {
            t = find_texture(c, texture);
            if (t == NULL)
            {
                // Create a new texture object but mark it as incomplete until glTexImage2D is called
                t = alloc_texture(c, texture);
                if (t == NULL)
                {
                    // Allocation failed, error already reported
                    return;
                }
                t->is_complete = false;
            }
        }

        c->texture.current = t; // Can be NULL if texture == 0
    }

    GLboolean glIsTexture(GLuint texture)
    {
        if (texture == 0)
            return GL_FALSE;
        return find_texture(gl_get_context(), texture) != NULL ? GL_TRUE : GL_FALSE;
    }

    void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                      GLsizei width, GLsizei height, GLint border,
                      GLenum format, GLenum type, const GLvoid *pixels)
    {
        GLContext *c = gl_get_context();

        if (target != GL_TEXTURE_2D)
        {
            gl_error(GL_INVALID_ENUM, "glTexImage2D: target must be GL_TEXTURE_2D");
            return;
        }
        if (border != 0)
        {
            gl_error(GL_INVALID_VALUE, "glTexImage2D: border must be 0");
            return;
        }
        if (level != 0)
        {
            gl_error(GL_INVALID_VALUE, "glTexImage2D: only level 0 is supported");
            return;
        }
        if (width != 256 || height != 256)
        {
            gl_error(GL_INVALID_VALUE, "glTexImage2D: only 256x256 textures are supported");
            return;
        }

        // Supported internal formats
        if (internalFormat != GL_RGB)
        {
            gl_error(GL_INVALID_VALUE, "glTexImage2D: only GL_RGB internal format is supported");
            return;
        }

        // Supported formats
        if (format != GL_RGB)
        {
            gl_error(GL_INVALID_ENUM, "glTexImage2D: only GL_RGB format is supported");
            return;
        }

        if (type != GL_UNSIGNED_BYTE)
        {
            gl_error(GL_INVALID_ENUM, "glTexImage2D: only GL_UNSIGNED_BYTE type is supported");
            return;
        }

        // Supported types
        if (type != GL_UNSIGNED_BYTE && type != GL_UNSIGNED_SHORT_5_6_5 &&
            type != GL_UNSIGNED_SHORT_4_4_4_4 && type != GL_UNSIGNED_SHORT_5_5_5_1)
        {
            gl_error(GL_INVALID_ENUM, "glTexImage2D: unsupported type");
            return;
        }

        GLTexture *texture = c->texture.current;
        if (texture == NULL)
        {
            gl_error(GL_INVALID_OPERATION, "glTexImage2D: no texture bound");
            return;
        }

        GLImage *image = &texture->images[level];

        image->xsize = width;
        image->ysize = height;
        image->internal_format = internalFormat;
        image->format = format;
        image->type = type;

        int bytesPerPixel;
        if (type == GL_UNSIGNED_BYTE)
        {
            if (format == GL_RGBA)
            {
                bytesPerPixel = 4;
            }
            else if (format == GL_RGB)
            {
                bytesPerPixel = 3;
            }
            else if (format == GL_ALPHA)
            {
                bytesPerPixel = 1;
            }
            else
            {
                gl_error(GL_INVALID_ENUM, "glTexImage2D: invalid format");
                return;
            }
        }
        else if (type == GL_UNSIGNED_SHORT_5_6_5 ||
                 type == GL_UNSIGNED_SHORT_4_4_4_4 ||
                 type == GL_UNSIGNED_SHORT_5_5_5_1)
        {
            bytesPerPixel = 2;
        }
        else
        {
            gl_error(GL_INVALID_ENUM, "glTexImage2D: invalid type");
            return;
        }

        if (image->pixmap != NULL)
        {
            free(image->pixmap); // Free the existing texture if necessary
            image->pixmap = NULL;
        }

        // Allocate memory for the texture image data
        size_t dataSize = width * height * bytesPerPixel;
        if (pixels != NULL)
        {
            image->pixmap = (unsigned char *)malloc(dataSize);
            if (image->pixmap == NULL)
            {
                gl_error(GL_OUT_OF_MEMORY, "glTexImage2D: failed to allocate texture memory");
                return;
            }

            // Copy pixel data to the allocated memory
            memcpy(image->pixmap, pixels, dataSize);
        }
        else
        {
            // If pixels is NULL, the texture image is undefined
            image->pixmap = NULL;
        }

        // Mark texture as complete if level 0 is defined
        if (level == 0)
        {
            texture->is_complete = true;
        }
    }

    void glTexEnvx(GLenum target, GLenum pname, GLint param)
    {
        GLContext *c = gl_get_context();

        if (target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
        {
            gl_error(GL_INVALID_ENUM, "glTexEnvx: unsupported target or pname");
            return;
        }

        switch (param)
        {
        case GL_MODULATE:
        case GL_DECAL:
        case GL_BLEND:
        case GL_REPLACE:
            c->texture.env_mode = param;
            break;
        default:
            gl_error(GL_INVALID_ENUM, "glTexEnvx: unsupported texture environment mode");
            break;
        }
    }

    void glTexEnvi(GLenum target, GLenum pname, GLint param)
    {
        GLContext *c = gl_get_context();

        if (target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
        {
            gl_error(GL_INVALID_ENUM, "glTexEnvi: unsupported target or pname");
            return;
        }

        if (param == GL_DECAL)
        {
            c->texture.env_mode = param;
        }
        else
        {
            gl_error(GL_INVALID_ENUM, "glTexEnvi: only GL_DECAL is supported");
        }
    }

    void glTexParameteri(GLenum target, GLenum pname, GLint param)
    {
        GLContext *c = gl_get_context();
        GLTexture *t = c->texture.current;

        if (target != GL_TEXTURE_2D)
        {
            gl_error(GL_INVALID_ENUM, "glTexParameteri: unsupported target");
            return;
        }
        if (t == NULL)
        {
            gl_error(GL_INVALID_OPERATION, "glTexParameteri: no texture bound");
            return;
        }

        switch (pname)
        {
        case GL_TEXTURE_WRAP_S:
            if (param == GL_CLAMP_TO_EDGE || param == GL_REPEAT)
            {
                t->wrap_s = param;
            }
            else
            {
                gl_error(GL_INVALID_ENUM, "glTexParameteri: invalid GL_TEXTURE_WRAP_S mode");
            }
            break;
        case GL_TEXTURE_WRAP_T:
            if (param == GL_CLAMP_TO_EDGE || param == GL_REPEAT)
            {
                t->wrap_t = param;
            }
            else
            {
                gl_error(GL_INVALID_ENUM, "glTexParameteri: invalid GL_TEXTURE_WRAP_T mode");
            }
            break;
        case GL_TEXTURE_MIN_FILTER:
            if (param == GL_NEAREST || param == GL_LINEAR ||
                param == GL_NEAREST_MIPMAP_NEAREST || param == GL_LINEAR_MIPMAP_NEAREST ||
                param == GL_NEAREST_MIPMAP_LINEAR || param == GL_LINEAR_MIPMAP_LINEAR)
            {
                t->min_filter = param;
            }
            else
            {
                gl_error(GL_INVALID_ENUM, "glTexParameteri: invalid GL_TEXTURE_MIN_FILTER mode");
            }
            break;
        case GL_TEXTURE_MAG_FILTER:
            if (param == GL_NEAREST || param == GL_LINEAR)
            {
                t->mag_filter = param;
            }
            else
            {
                gl_error(GL_INVALID_ENUM, "glTexParameteri: invalid GL_TEXTURE_MAG_FILTER mode");
            }
            break;
        default:
            gl_error(GL_INVALID_ENUM, "glTexParameteri: invalid pname");
            break;
        }
    }

    void glPixelStorei(GLenum pname, GLint param)
    {
        GLContext *c = gl_get_context();

        if (pname == GL_UNPACK_ALIGNMENT)
        {
            if (param == 1 || param == 2 || param == 4 || param == 8)
            {
                c->unpack_alignment = param;
            }
            else
            {
                gl_error(GL_INVALID_VALUE, "glPixelStorei: invalid unpack alignment");
            }
        }
        else
        {
            gl_error(GL_INVALID_ENUM, "glPixelStorei: invalid pname");
        }
    }

    // Implement the gl_error function to handle OpenGL errors
    void gl_error(GLenum error_code, const char *message)
    {
        GLContext *c = gl_get_context();
        if (c->error == GL_NO_ERROR)
        {
            c->error = error_code;
        }
        // Optionally log the error message
        fprintf(stderr, "OpenGL Error [%d]: %s\n", error_code, message);
    }

} // namespace fp

#include "specbuf.hpp"
#include <math.h>
#include <stdlib.h>

namespace fp
{

    static void calc_buf(GLSpecBuf *buf, const tGLfixed shininess)
    {
        int i;
        tGLfixed val, inc;
        val = 0.0f;
        inc = 1.0f / SPECULAR_BUFFER_SIZE;
        for (i = 0; i <= SPECULAR_BUFFER_SIZE; i++)
        {
            buf->buf[i] = std::pow(static_cast<double>(val), static_cast<double>(shininess));
            val += inc;
        }
    }

    GLSpecBuf *specbuf_get_buffer(GLContext *c, const int shininess_i, const tGLfixed shininess)
    {
        GLSpecBuf *found, *oldest;
        found = oldest = c->specbuf_first;
        while (found && found->shininess_i != shininess_i)
        {
            if (found->last_used < oldest->last_used)
            {
                oldest = found;
            }
            found = found->next;
        }
        if (found)
        { /* hey, found one! */
            found->last_used = c->specbuf_used_counter++;
            return found;
        }
        if (oldest == NULL || c->specbuf_num_buffers < MAX_SPECULAR_BUFFERS)
        {
            /* create new buffer */
            GLSpecBuf *buf = (GLSpecBuf *)malloc(sizeof(GLSpecBuf));
            if (!buf)
                fprintf(stderr, "could not allocate specular buffer");
            c->specbuf_num_buffers++;
            buf->next = c->specbuf_first;
            c->specbuf_first = buf;
            buf->last_used = c->specbuf_used_counter++;
            buf->shininess_i = shininess_i;
            calc_buf(buf, shininess);
            return buf;
        }
        /* overwrite the lru buffer */
        /*tgl_trace("overwriting spec buffer :(\n");*/
        oldest->shininess_i = shininess_i;
        oldest->last_used = c->specbuf_used_counter++;
        calc_buf(oldest, shininess);
        return oldest;
    }

} // namespace fp

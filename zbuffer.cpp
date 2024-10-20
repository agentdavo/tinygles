/*
 * Z buffer: 16 bits Z / 16 bits color
 *
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>  // For sysconf to get the number of cores
#include "zbuffer.hpp"

namespace fp {

ZBuffer *ZB_open(int xsize, int ysize, int mode,
                 int nb_colors,
                 unsigned char *color_indexes,
                 int *color_table,
                 void *frame_buffer) {
    ZBuffer *zb;
    int size;

    zb = (ZBuffer*)malloc(sizeof(ZBuffer));
    if (zb == NULL)
        return NULL;

    zb->xsize = xsize;
    zb->ysize = ysize;
    zb->mode = mode;
    zb->linesize = (xsize * PSZB + 3) & ~3;
    switch (mode) {
        case ZB_MODE_INDEX:
            ZB_initDither(zb, nb_colors, color_indexes, color_table);
            break;
        case ZB_MODE_RGBA:
        case ZB_MODE_5R6G5B:
            zb->nb_colors = 0;
            break;
        default:
            goto error;
    }

    size = zb->xsize * zb->ysize * sizeof(unsigned short);

    zb->zbuf = (short unsigned int*)malloc(size);
    if (zb->zbuf == NULL)
        goto error;

    if (frame_buffer == NULL) {
        zb->pbuf = (PIXEL*)malloc(zb->ysize * zb->linesize);
        if (zb->pbuf == NULL) {
            free(zb->zbuf);
            goto error;
        }
        zb->frame_buffer_allocated = 1;
    } else {
        zb->frame_buffer_allocated = 0;
        zb->pbuf = (PIXEL*)frame_buffer;
    }

    zb->current_texture = NULL;
    return zb;
error:
    free(zb);
    return NULL;
}

void ZB_close(ZBuffer * zb) {
    if (zb->mode == ZB_MODE_INDEX)
        ZB_closeDither(zb);

    if (zb->frame_buffer_allocated)
        free(zb->pbuf);

    free(zb->zbuf);
    free(zb);
}

void ZB_resize(ZBuffer * zb, void *frame_buffer, int xsize, int ysize) {
    int size;

    /* xsize must be a multiple of 4 */
    xsize = xsize & ~3;

    zb->xsize = xsize;
    zb->ysize = ysize;
    zb->linesize = (xsize * PSZB + 3) & ~3;
    size = zb->xsize * zb->ysize * sizeof(unsigned short);

    free(zb->zbuf);
    zb->zbuf = (short unsigned int*)malloc(size);

    if (zb->frame_buffer_allocated)
        free(zb->pbuf);

    if (frame_buffer == NULL) {
        zb->pbuf = (PIXEL*)malloc(zb->ysize * zb->linesize);
        zb->frame_buffer_allocated = 1;
    } else {
        zb->pbuf = (PIXEL*)frame_buffer;
        zb->frame_buffer_allocated = 0;
    }
}

// Structure to pass arguments to each thread
typedef struct {
    ZBuffer *zb;
    uint8_t *buf;  // Use uint8_t for byte-level access
    int linesize;
    int startY;  // Starting line for this thread
    int endY;    // Ending line for this thread
} ThreadData;

// Thread function for copying RGBA framebuffer
static void* ZB_copy_FrameBufferRGBA_thread(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    ZBuffer *zb = data->zb;
    uint8_t *p1 = data->buf + data->startY * data->linesize;  // Buffer for each thread
    PIXEL *q = (PIXEL*)((uint8_t*)zb->pbuf + data->startY * zb->linesize); // Pointer to the zbuffer's pixel buffer

    int n = zb->xsize * PSZB;  // Number of bytes per scanline

    for (int y = data->startY; y < data->endY; y++) {
        memcpy(p1, q, n);  // Copy each scanline
        p1 += data->linesize;  // Move to the next line in the destination buffer
        q = (PIXEL*)((uint8_t*)q + zb->linesize);  // Move to the next line in the source buffer
    }

    return NULL;
}

void ZB_copy_FrameBufferRGBA(ZBuffer *zb, void *buf, int linesize) {
    int numCores = sysconf(_SC_NPROCESSORS_ONLN);  // Get number of CPU cores
    int numThreads = numCores - 1;  // Leave 1 core for the main thread

    if (numThreads <= 0) {
        numThreads = 1;  // Use at least 1 thread
    }

    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int linesPerThread = zb->ysize / numThreads;  // Divide the work among threads
    int remainingLines = zb->ysize % numThreads;  // Handle any remaining lines

    // Launch threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].zb = zb;
        threadData[i].buf = (uint8_t*)buf;
        threadData[i].linesize = linesize;
        threadData[i].startY = i * linesPerThread;
        threadData[i].endY = threadData[i].startY + linesPerThread;

        if (i == numThreads - 1) {
            threadData[i].endY += remainingLines;  // Last thread handles remaining lines
        }

        pthread_create(&threads[i], NULL, ZB_copy_FrameBufferRGBA_thread, &threadData[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}

// Thread function for 5R6G5B framebuffer copy
static void* copy_FrameBuffer5R6G5B_thread(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    ZBuffer *zb = data->zb;
    uint16_t *p1 = (uint16_t*)(data->buf + data->startY * data->linesize);  // Use uint16_t for 16-bit RGB buffer
    PIXEL *q = (PIXEL*)((uint8_t*)zb->pbuf + data->startY * zb->linesize);  // Pointer to ZBuffer's pixel data

    for (int y = data->startY; y < data->endY; y++) {
        for (int x = 0; x < zb->xsize; x++) {
            p1[x] = RGB32_TO_RGB16(q[x]);  // Convert from RGB32 to RGB16
        }
        p1 = (uint16_t*)((uint8_t*)p1 + data->linesize);  // Move to the next line in the destination buffer
        q += zb->xsize;  // Move to the next line in the source buffer
    }

    return NULL;
}

void ZB_copy_FrameBuffer5R6G5B(ZBuffer *zb, void *buf, int linesize) {
    int numCores = sysconf(_SC_NPROCESSORS_ONLN);  // Get the number of CPU cores
    int numThreads = numCores - 1;  // Leave 1 core for the main thread

    if (numThreads <= 0) {
        numThreads = 1;  // Use at least 1 thread
    }

    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int linesPerThread = zb->ysize / numThreads;  // Distribute lines evenly across threads
    int remainingLines = zb->ysize % numThreads;  // Handle remaining lines

    // Launch threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].zb = zb;
        threadData[i].buf = (uint8_t*)buf;
        threadData[i].linesize = linesize;
        threadData[i].startY = i * linesPerThread;
        threadData[i].endY = threadData[i].startY + linesPerThread;

        if (i == numThreads - 1) {
            threadData[i].endY += remainingLines;  // Last thread handles extra lines
        }

        pthread_create(&threads[i], NULL, copy_FrameBuffer5R6G5B_thread, &threadData[i]);
    }

    // Join threads
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}

void ZB_copyFrameBuffer(ZBuffer * zb, void *buf, int linesize) {
    switch (zb->mode) {
        case ZB_MODE_5R6G5B:
            ZB_copy_FrameBuffer5R6G5B(zb, buf, linesize);
            break;
        case ZB_MODE_RGBA:
            ZB_copy_FrameBufferRGBA(zb, buf, linesize);
            break;
        default:
            assert(0);  // Invalid mode
    }
}

/*
 * adr must be aligned on an 'int'
 */
static void memset_short(void *adr, int val, int count) {
    int i, n, v;
    unsigned int *p;
    unsigned short *q;

    p = (unsigned int*)adr;
    v = val | (val << 16);

    n = count >> 3;
    for (i = 0; i < n; i++) {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
    }

    q = (unsigned short *) p;
    n = count & 7;
    for (i = 0; i < n; i++)
        *q++ = val;
}

static void memset_long(void *adr, int val, int count) {
    int i, n, v;
    unsigned int *p;

    p = (unsigned int*)adr;
    v = val;
    n = count >> 2;
    for (i = 0; i < n; i++) {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
    }

    n = count & 3;
    for (i = 0; i < n; i++)
        *p++ = val;
}

/* count must be a multiple of 4 and >= 4 */
void memset_RGB24(void *adr, int r, int v, int b, long count) {
    long i, n;
    long v1, v2, v3,*pt = (long *)(adr);
    unsigned char *p, R = (unsigned char)r, V = (unsigned char)v , B = (unsigned char)b;

    p=(unsigned char *)adr;
    *p++ = R;
    *p++ = V;
    *p++ = B;
    *p++ = R;
    *p++ = V;
    *p++ = B;
    *p++ = R;
    *p++ = V;
    *p++ = B;
    *p++ = R;
    *p++ = V;
    *p++ = B;
    v1 = *pt++;
    v2 = *pt++;
    v3 = *pt++;
    n = count >> 2;
    for(int i = 1; i < n; i++) {
        *pt++ = v1;
        *pt++ = v2;
        *pt++ = v3;
    }
}

void ZB_clear(ZBuffer * zb, int clear_z, int z, int clear_color, int r, int g, int b) {
    int color;
    int y;
    PIXEL *pp;

    if (clear_z) {
        memset_short(zb->zbuf, z, zb->xsize * zb->ysize);
    }
    if (clear_color) {
        pp = zb->pbuf;
        for (y = 0; y < zb->ysize; y++) {
            color = RGB_TO_PIXEL(r, g, b);
            memset_long(pp, color, zb->xsize);
            pp = (PIXEL *) ((char *)pp + zb->linesize);
        }
    }
}

} // namespace fp

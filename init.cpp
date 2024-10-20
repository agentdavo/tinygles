#include "zgl.hpp"
#include "matrix.hpp"
#include <memory.h>

namespace fp {

GLContext *gl_ctx;

void initSharedState(GLContext *c) {
    GLSharedState *s = &c->shared_state;
    // s->texture_hash_table = (GLTexture**)calloc(1, sizeof(GLTexture *) * TEXTURE_HASH_TABLE_SIZE);

    alloc_texture(c, 0); // Allocate default texture (ID 0)
}

void endSharedState(GLContext *c) {
    GLSharedState *s = &c->shared_state;
    free(s->texture_hash_table);
}

void initLights(GLContext *c) {
    for (int i = 0; i < MAX_LIGHTS; i++) {
        GLLight *l = &c->light.lights[i];
        l->ambient = gl_V4_New(0, 0, 0, 1);
        l->diffuse = gl_V4_New(1, 1, 1, 1);
        l->specular = gl_V4_New(1, 1, 1, 1);
        l->position = gl_V4_New(0, 0, 1, 0);
        l->norm_position = gl_V3_New(0, 0, 1);
        l->spot_direction = gl_V3_New(0, 0, -1);
        l->norm_spot_direction = gl_V3_New(0, 0, -1);
        l->spot_exponent = 0;
        l->spot_cutoff = 180;
        l->attenuation[0] = 1;
        l->attenuation[1] = 0;
        l->attenuation[2] = 0;
        l->enabled = 0;
    }
    c->light.first = NULL;
    c->light.model.ambient = gl_V4_New(0.2, 0.2, 0.2, 1);
    c->light.model.local = 0;
    c->light.model.two_side = 0;
    c->light.enabled = 0;
}

void initMaterials(GLContext *c) {
    for (int i = 0; i < 2; i++) {
        GLMaterial *m = &c->material.materials[i];
        m->emission = gl_V4_New(0, 0, 0, 1);
        m->ambient = gl_V4_New(0.2, 0.2, 0.2, 1);
        m->diffuse = gl_V4_New(0.8, 0.8, 0.8, 1);
        m->specular = gl_V4_New(0, 0, 0, 1);
        m->shininess = 0;
    }
    c->material.color.current_mode = GL_FRONT_AND_BACK;
    c->material.color.current_type = GL_AMBIENT_AND_DIFFUSE;
    c->material.color.enabled = 0;
}

void initMatrices(GLContext *c, ZBuffer *zbuffer) {
    GLViewport *v = &c->viewport;
    v->xmin = 0;
    v->ymin = 0;
    v->xsize = zbuffer->xsize;
    v->ysize = zbuffer->ysize;
    v->updated = 1;

    // Initialize matrix stack depths
    c->matrix.stack_depth_max[0] = MAX_MODELVIEW_STACK_DEPTH;
    c->matrix.stack_depth_max[1] = MAX_PROJECTION_STACK_DEPTH;
    c->matrix.stack_depth_max[2] = MAX_TEXTURE_STACK_DEPTH;

    // Allocate matrix stacks
    for (int i = 0; i < 3; i++) {
        c->matrix.stack[i] = (M4*)calloc(1, c->matrix.stack_depth_max[i] * sizeof(M4));
        c->matrix.stack_ptr[i] = c->matrix.stack[i];
    }

    // Initialize identity matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    c->matrix.model_projection_updated = 1;
}

void initState(GLContext *c) {
    c->current.color = {1.0, 1.0, 1.0, 1.0};
    c->current.longcolor[0] = 65535;
    c->current.longcolor[1] = 65535;
    c->current.longcolor[2] = 65535;

    c->current.normal = {1.0, 0.0, 0.0, 0.0};
    c->current.edge_flag = 1;
    c->current.tex_coord = {0, 0, 0, 1};

    c->polygon_mode_front = GL_FILL;
    c->polygon_mode_back = GL_FILL;
    c->current_front_face = 0;
    c->current_cull_face = GL_BACK;
    c->current_shade_model = GL_SMOOTH;
    c->cull_face_enabled = 0;

    c->clear.color = {0, 0, 0, 0};
    c->clear.depth = 0;
}

void glInit(void *zbuffer1) {

    ZBuffer *zbuffer = static_cast<ZBuffer *>(zbuffer1);
    GLContext *c = new GLContext();
    if (c == NULL) {
        fprintf(stderr, "Failed to allocate GLContext\n");
        exit(1);  // Handle error appropriately
    }
    gl_ctx = c;

    c->zb = zbuffer;
    c->vertex_max = POLYGON_MAX_VERTEX;
    c->vertex = new GLVertex[POLYGON_MAX_VERTEX];
    if (c->vertex == NULL) {
        fprintf(stderr, "Failed to allocate vertex array\n");
        free(c);
        exit(1);  // Handle error appropriately
    }

    // Initialize shared state, lights, materials, and matrices
    initSharedState(c);
    initLights(c);
    initMaterials(c);
    initMatrices(c, zbuffer);
    initState(c);

    // Initialize OpenGL 1.1 settings
    c->client_states = 0;
    c->offset.states = 0;
    c->gl_resize_viewport = NULL;
    c->specbuf_first = NULL;
    c->specbuf_used_counter = 0;
    c->specbuf_num_buffers = 0;
    c->depth_test = 0;
    c->textsize = 1;
}

void glClose() {
    GLContext *c = gl_get_context();
    if (c == NULL) return;

    // Clean up shared state
    endSharedState(c);

    // Delete vertex array
    delete[] c->vertex;
    c->vertex = nullptr;

    // Delete GLContext
    delete c;
    gl_ctx = nullptr;
}

} // namespace fp

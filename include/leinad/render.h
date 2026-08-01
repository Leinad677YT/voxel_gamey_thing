#pragma once

#include <SDL3/SDL_gpu.h>
#include "data/tags.h"
#include "data/types.h"

#define LEINAD_TEXTURE_RADIUS 16

#define LEINAD_RENDER_DEFAULT_FOV 100.0f
#define LEINAD_RENDER_DEFAULT_NEARPLANE 0.125f
#define LEINAD_RENDER_DEFAULT_FARPLANE 600.0f

struct render_data {
    float fov;
    float near_plane;
    float far_plane;
} RENDER = {
    .fov = LEINAD_RENDER_DEFAULT_FOV,
    .near_plane = LEINAD_RENDER_DEFAULT_NEARPLANE,
    .far_plane = LEINAD_RENDER_DEFAULT_FARPLANE
};

struct render_atlas {
    SDL_GPUTexture* texture;
    SDL_GPUSampler* sampler;
    int width;
    int height;
};

static struct render_atlas block_atlas= {
    .texture = NULL,
    .sampler = NULL,
    .width = 0,
    .height = 0
};


LEINAD_FCALL int leinad_render_main(struct leinad_position pos, struct leinad_rotation rot);
LEINAD_FCALL int leinad_render_world(SDL_GPUCommandBuffer* restrict cmdbuf, SDL_GPUTexture* restrict swapchain, struct leinad_position pos, struct leinad_rotation rot);

LEINAD_FCALL int leinad_render_init();
LEINAD_FCALL void leinad_render_end();

LEINAD_FCALL int leinad_render_update_textures();
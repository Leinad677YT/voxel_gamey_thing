#pragma once

#include <SDL3/SDL_gpu.h>
#include "../data/tags.h"

#define LEINAD_TEXTURE_RADIUS 16

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


LEINAD_FCALL int leinad_render_world();

LEINAD_FCALL int leinad_render_init();
LEINAD_FCALL void leinad_render_end();

LEINAD_FCALL int leinad_render_update_textures();
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

struct render_mesh {
    SDL_GPUBuffer* vertex;
    SDL_GPUBuffer* index;
    Uint32 vert_o_count,vert_t_count;
    Uint32 ind_x,ind_x_, ind_y,ind_y_, ind_z,ind_z_, ind_unspecified;
};

LEINAD_FCALL int leinad_render_world();

LEINAD_FCALL int leinad_render_init();
LEINAD_FCALL void leinad_render_end();

LEINAD_FCALL int leinad_render_update_textures();
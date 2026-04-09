#pragma once

#include <SDL3/SDL.h>

#include "data.h"

#define LEINAD_TEXTURE_WIDTH 16
#define LEINAD_TEXTURE_HEIGHT 16
#define LEINAD_TEXTURE_9CROP_W 6
#define LEINAD_TEXTURE_9CROP_H 6

SDL_Texture* leinad_load_texture(SDL_Renderer* render, const char* texture_name);

bool leinad_render_9crop(const leinad_welem_instance_t* welem, SDL_Renderer* render);
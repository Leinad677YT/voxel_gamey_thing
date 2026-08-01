#pragma once

#include <SDL3/SDL.h>

#include <leinad/render.h>

SDL_Surface* leinad_load_png(const char* texture_name);

LEINAD_AUX int _update_atlas(struct render_atlas *atlas, char* root_path, Uint32 tx_amount, int tx_w, int tx_h);


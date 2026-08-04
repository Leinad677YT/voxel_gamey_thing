#pragma once

#include <SDL3/SDL.h>

static struct app_data {
    SDL_Window* window;                  // Window
    SDL_GPUDevice *device;               // GPU to use
    Uint64 current_ns;
    Uint64 previous_ns;
} APP = {
    .window = NULL,
    .device = NULL,
    .current_ns = 0,
    .previous_ns = 0
};
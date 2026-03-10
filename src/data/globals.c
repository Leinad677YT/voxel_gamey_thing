#include <SDL3/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Uint64 current_ns = 0;
Uint64 previous_ns = 0;


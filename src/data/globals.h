#ifndef LEINAD_GLOBALS
    #define LEINAD_GLOBALS 1

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

extern SDL_Window* window;                  // Window
extern SDL_Renderer* renderer;              // Renderer

extern Uint64 current_ns;
extern Uint64 previous_ns;

#endif
#include <SDL3/SDL.h>

#include "../data/app.h"
#include "../data/globals.h"


SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {
    
  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }

//   { // set window background
//     SDL_SetRenderDrawColorFloat(renderer, (float) (0.5 + 0.5 * SDL_sin(SDL_GetTicks() / 1000.0)), 0, 0, SDL_ALPHA_OPAQUE_FLOAT);
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);
//   }  

    previous_ns = current_ns;
    return SDL_APP_CONTINUE;
}

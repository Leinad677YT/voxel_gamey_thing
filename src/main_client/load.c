#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

// #include "../data/types.h"
#include "../data/globals.h"
#include "../data/app.h"


SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {

  { // initialize SDL subsystem

    SDL_SetAppMetadata(
        LEINAD_APP_NAME,
        LEINAD_APP_VERSION,
        LEINAD_APP_IDENTIFIER
    );

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

  }
  
  { // initialize window and renderer

    bool created = SDL_CreateWindowAndRenderer(
        LEINAD_WINDOW_TITLE,
        LEINAD_WINDOW_WIDTH,
        LEINAD_WINDOW_HEIGHT,
        0,
        &window,
        &renderer
    );
    
    if (!created) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetWindowTitle(window,LEINAD_WINDOW_TITLE);
    SDL_SetWindowSize(window,LEINAD_WINDOW_WIDTH,LEINAD_WINDOW_HEIGHT);

  }

  { // set window background
    SDL_SetRenderDrawColor(renderer, 0xFF,  0xFF,  0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }  

    current_ns = SDL_GetTicksNS();
    previous_ns = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;
}

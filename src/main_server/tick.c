#include <SDL3/SDL.h>

#include <leinad/data/app.h>
#include <leinad/world/loading.h>

#include <leinad/app.h>
#include <leinad/player.h>

SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {

  { // check the time since last tick
    APP.current_ns = SDL_GetTicksNS();

    if (APP.current_ns - APP.previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
  
    return SDL_APP_SUCCESS;

    leinad_update_entities();

    leinad_update_players();


    APP.previous_ns = APP.current_ns;

    return SDL_APP_CONTINUE;
}

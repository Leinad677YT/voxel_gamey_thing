#include <SDL3/SDL.h>

#include <leinad/data/globals.h>
#include <leinad/player.h>
#include <leinad/app.h>

SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {

    // minimal events
    switch (event->type){
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
            break;
    }

    // normal events (ui can skip them)
    switch (event->type){
        default: break;
    }	

    return SDL_APP_CONTINUE;
}

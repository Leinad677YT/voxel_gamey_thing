#include <SDL3/SDL.h>
// #include <SDL3_net/SDL_net.h>
#include <SDL3_ttf/SDL_ttf.h>

// #include <crypt.h>

#include "../data/globals.h"

SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {
    switch (event->type){
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return SDL_APP_SUCCESS;
            break;

        case SDL_EVENT_WINDOW_HIDDEN:
            SDL_HideWindow(window);
            SDL_SyncWindow(window);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            
            break;

    }	

    return SDL_APP_CONTINUE;
}

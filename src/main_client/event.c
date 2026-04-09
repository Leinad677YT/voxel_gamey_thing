#include <SDL3/SDL.h>
// #include <SDL3_net/SDL_net.h>
#include <SDL3_ttf/SDL_ttf.h>

// #include <crypt.h>

#include "../data/globals.h"
#include "../ui/data.h"

SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {

    // minimal events
    switch (event->type){
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return SDL_APP_SUCCESS;
            break;

        case SDL_EVENT_WINDOW_HIDDEN:
            SDL_HideWindow(window);
            SDL_SyncWindow(window);
            break;

    }	


    // UI interactions
    switch (manage_ui_interaction(appstate,event)){
        case LEINAD_UI_STATUS_CONTINUE:
            break;
        case LEINAD_UI_STATUS_SUCCESS:
            return SDL_APP_SUCCESS;
        case LEINAD_UI_STATUS_FAILURE:
            return SDL_APP_SUCCESS;
        case LEINAD_UI_STATUS_INTERCEPTED:
            return SDL_APP_CONTINUE;
            
    }

    // normal events (ui can skip them)
    switch (event->type){

        case SDL_EVENT_KEY_DOWN:
            SDL_Log("x= %lf, y= %lf, z= %lf",x,y,z);
            if (event->key.repeat) break;
            switch(event->key.key) {
                case SDLK_A:
                    toadd_x--; break;
                case SDLK_D:
                    toadd_x++; break;
                case SDLK_W:
                    toadd_z--; break;
                case SDLK_S:
                    toadd_z++; break;
                case SDLK_LCTRL:
                    toadd_y--; break;
                case SDLK_SPACE:
                    toadd_y++; break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            if (event->key.repeat) break;
            switch(event->key.key) {
                case SDLK_A:
                    toadd_x++; break;
                case SDLK_D:
                    toadd_x--; break;
                case SDLK_W:
                    toadd_z++; break;
                case SDLK_S:
                    toadd_z--; break;
                case SDLK_LCTRL:
                    toadd_y++; break;
                case SDLK_SPACE:
                    toadd_y--; break;
            }

        default: break;
    }	

    return SDL_APP_CONTINUE;
}

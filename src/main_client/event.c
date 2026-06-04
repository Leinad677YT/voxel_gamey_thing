#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
// #include <SDL3_ttf/SDL_ttf.h>

// #include <crypt.h>

#include <leinad/data/globals.h>

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

    // normal events (ui can skip them)
    switch (event->type){

        case SDL_EVENT_KEY_DOWN:
            // SDL_Log("x= %lf, y= %lf, z= %lf",pos_x,pos_y,pos_z);
            if (event->key.repeat) break;
            switch(event->key.key) {
                case SDLK_A:
                    toadd_x--; 
                    NET_WriteToStreamSocket(client_sock, "left", 5);
                    break;
                case SDLK_D:
                    toadd_x++; 
                    NET_WriteToStreamSocket(client_sock, "right", 6);
                    break;
                case SDLK_W:
                    toadd_z--; 
                    NET_WriteToStreamSocket(client_sock, "forward", 8);
                    break;
                case SDLK_S:
                    toadd_z++; 
                    NET_WriteToStreamSocket(client_sock, "down", 5);
                    break;
                case SDLK_LCTRL:
                    toadd_y--; 
                    NET_WriteToStreamSocket(client_sock, "crouch", 7);
                    break;
                case SDLK_SPACE:
                    toadd_y++; 
                    NET_WriteToStreamSocket(client_sock, "jump", 5);
                    break;
                case SDLK_ESCAPE:
                    NET_WriteToStreamSocket(client_sock, "close", 6);
                    return SDL_APP_SUCCESS;
            }
            break;

        case SDL_EVENT_KEY_UP:
            if (event->key.repeat) break;
            switch(event->key.key) {
                case SDLK_A:
                    toadd_x++; 
                    NET_WriteToStreamSocket(client_sock, "_left", 6);
                    break;
                case SDLK_D:
                    toadd_x--; 
                    NET_WriteToStreamSocket(client_sock, "_right", 7);
                    break;
                case SDLK_W:
                    toadd_z++; 
                    NET_WriteToStreamSocket(client_sock, "_forward", 8);
                    break;
                case SDLK_S:
                    toadd_z--; 
                    NET_WriteToStreamSocket(client_sock, "_down", 6);
                    break;
                case SDLK_LCTRL:
                    toadd_y++; 
                    NET_WriteToStreamSocket(client_sock, "_crouch", 8);
                    break;
                case SDLK_SPACE:
                    toadd_y--; 
                    NET_WriteToStreamSocket(client_sock, "_jump", 6);
                    break;
            }

        default: break;
    }	

    return SDL_APP_CONTINUE;
}

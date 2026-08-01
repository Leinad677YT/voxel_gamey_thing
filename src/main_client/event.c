#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
// #include <SDL3_ttf/SDL_ttf.h>

// #include <crypt.h>

#include <leinad/math/trigonometry.h>
#include <leinad/data/globals.h>
#include <leinad/player.h>

SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {

    static bool ignore_mouse = false;

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

        case SDL_EVENT_MOUSE_MOTION:
        
            if (ignore_mouse) {
                ignore_mouse = false;
                break;
            }
            client->generic.rotation.yaw += 1.0f * event->motion.xrel;
            if (client->generic.rotation.yaw >= 180.0f) client->generic.rotation.yaw -= 360.0f;
            if (client->generic.rotation.yaw < -180.0f) client->generic.rotation.yaw += 360.0f;

            client->generic.rotation.pitch += 1.0f * event->motion.yrel;
            if (client->generic.rotation.pitch > 89.99f) client->generic.rotation.pitch = 89.99f;
            else if (client->generic.rotation.pitch < -89.99f) client->generic.rotation.pitch = -89.99f;

            SDL_WarpMouseInWindow(window, SceneWidth * 0.5f, SceneHeight * 0.5f);
            ignore_mouse = true;

            break;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.repeat) break;
            switch(event->key.key) {
                case SDLK_A:
                    client->generic.motion.x -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "left", 5);
                    break;
                case SDLK_D:
                    client->generic.motion.x += client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "right", 6);
                    break;
                case SDLK_W:
                    client->generic.motion.z -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "forward", 8);
                    break;
                case SDLK_S:
                    client->generic.motion.z += client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "down", 5);
                    break;
                case SDLK_LCTRL:
                    client->generic.motion.y -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "crouch", 7);
                    break;
                case SDLK_SPACE:
                    client->generic.motion.y += client->fly_speed;
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
                    client->generic.motion.x += client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_left", 6);
                    break;
                case SDLK_D:
                    client->generic.motion.x -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_right", 7);
                    break;
                case SDLK_W:
                    client->generic.motion.z += client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_forward", 8);
                    break;
                case SDLK_S:
                    client->generic.motion.z -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_down", 6);
                    break;
                case SDLK_LCTRL:
                    client->generic.motion.y += client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_crouch", 8);
                    break;
                case SDLK_SPACE:
                    client->generic.motion.y -= client->fly_speed;
                    NET_WriteToStreamSocket(client_sock, "_jump", 6);
                    break;
            }

        default: break;
    }	

    return SDL_APP_CONTINUE;
}

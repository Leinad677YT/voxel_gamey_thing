#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../src/libs/bit_manipulation.h"

SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {
    SDL_assert(pow2(0) == 1);
    SDL_assert(pow2(1) == 2);
    SDL_assert(pow2(3) == 8);
    SDL_assert(pow2(8) == 256);


    SDL_assert(next_pow2(0) == 1);
    SDL_assert(next_pow2(1) == 1);
    SDL_assert(next_pow2(3) == 4);
    SDL_assert(next_pow2(5) == 8);
    SDL_assert(next_pow2(17) == 32);
    SDL_assert(next_pow2(99) == 128);

    return SDL_APP_SUCCESS;
}



SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    return;
}
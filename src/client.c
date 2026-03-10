#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


// AUX

    // globals
    #include "data/globals.c"

//


// Init
#include "main_client/load.c"

// Render loop
#include "main_client/tick.c"

// Event processing
#include "main_client/event.c"

// Close
#include "main_client/close.c"

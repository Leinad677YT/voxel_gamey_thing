#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


// LIBRARIES
    #include "libs/queue.c"
    #include "libs/stack.c"
    #include "libs/sorted_list.c"
//

// AUX

    // entity types
    #include "entity/player.c"

    // render
    #include "render/shaders.c"
    #include "render/textures.c"

    // math
    #include "math/matrix.c"

    // world
    #include "world/region.c"
    #include "world/loading.c"
    #include "world/block.c"

//


// Init
#include "main_client/load.c"

// Render loop
#include "main_client/tick.c"

// Event processing
#include "main_client/event.c"

// Close
#include "main_client/close.c"

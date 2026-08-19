#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


// LIBRARIES
    #include "libs/queue.c"
    #include "libs/stack.c"
    #include "libs/sorted_list.c"
//

// AUX

    // datapacks :tada:
    #include "libs/namespacing.c"

    // math
    #include "math/matrix.c"

    // world
    #include "world/region.c"
    #include "world/loading.c"
    #include "world/block.c"
    #include "world/entity.c"

    // entity types
    #include "entity/player.c"

//


// Init
#include "main_server/load.c"

// Render loop
#include "main_server/tick.c"

// Event processing
#include "main_server/event.c"

// Close
#include "main_server/close.c"


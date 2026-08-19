#include <SDL3/SDL.h>

#include <leinad/world.h>
#include <leinad/app.h>

#include <leinad/data/app.h>
#include <leinad/data/control_shortcuts.h>

#include <leinad/data/namespacing.h>
#include <leinad/datatype/dimension.h>


SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {

  { // initialize SDL subsystem

    SDL_SetAppMetadata(
        LEINAD_APP_NAME,
        LEINAD_APP_VERSION,
        LEINAD_APP_IDENTIFIER
    );

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

  }

  { // LOAD DATAPACKS
    switch (NAMESPACE.load_all()) {
        case LEINAD_RETURN_FAILURE:
            SDL_Log("Could not load datapacks correctly, this probably comes from an OOM error, consider asigning more memory to the instance.");
            return SDL_APP_FAILURE;
        default:
            break;
    };
  }

  { // load_dimensions
    WORLD.dimensions = SDL_malloc(sizeof(struct dimension*) * 3);
    WORLD.dimension_amount = 3;

    WORLD.dimensions[0] = SDL_malloc(sizeof(struct dimension));
    WORLD.dimensions[0]->generator = SDL_malloc(sizeof(struct LEINAD_DIMENSION_GENERATOR_DEBUG));
    WORLD.dimensions[0]->generator->generic.type = LEINAD_DIMENSION_GENERATOR_DEBUG;

    WORLD.dimensions[1] = NULL;
    WORLD.dimensions[2] = NULL;
  }

    APP.current_ns = SDL_GetTicksNS();
    APP.previous_ns = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;
}

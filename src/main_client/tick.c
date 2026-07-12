#include <SDL3/SDL.h>

#include <leinad/data/app.h>
#include <leinad/data/globals.h>

#include <leinad/world/loading.h>

#include <leinad/player.h>

#include "../world/render.c"

extern int leinad_render2_ui();


SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {
    
  { // render world
    ENFORCE(leinad_render_world(client->generic.pos,(vec3){-client->generic.pos.x,-client->generic.pos.y,-client->generic.pos.z}));
  }

  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
    client->generic.pos.x += client->generic.motion.x * (current_ns - previous_ns) / 100000000;
    client->generic.pos.y += client->generic.motion.y * (current_ns - previous_ns) / 100000000;
    client->generic.pos.z += client->generic.motion.z * (current_ns - previous_ns) / 100000000;

    static int i = 0;
    i++;

    if (i > /*1s*/ 1000000000 / LEINAD_TICK_RANGE_NS) {
        // leinad_render_update_textures();
        load_around_player(client);
        if (loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH) / 2] == NULL) SDL_Log("outisde center chunk");
        i = 0;
    }

previous_ns = current_ns;
return SDL_APP_CONTINUE;
}

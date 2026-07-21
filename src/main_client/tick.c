#include <SDL3/SDL.h>

#include <leinad/data/app.h>
#include <leinad/data/globals.h>

#include <leinad/world/loading.h>

#include <leinad/player.h>

#include "../world/render.c"

extern int leinad_render2_ui();


SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {

    static int frames = 0;
    static Uint64 last_ns = 0;
    
  { // render world
    ENFORCE(leinad_render_world(client->generic.pos,(vec3){-client->generic.pos.x,-client->generic.pos.y,-client->generic.pos.z}));
    frames++;
  }

  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
    client->generic.pos.x += client->generic.motion.x * (current_ns - previous_ns) / 100000000;
    client->generic.pos.y += client->generic.motion.y * (current_ns - previous_ns) / 100000000;
    client->generic.pos.z += client->generic.motion.z * (current_ns - previous_ns) / 100000000;

    if (current_ns - last_ns > 1000000000) {
        double fps = (double)(frames) / ((double)(current_ns - last_ns)/1000000000);
        SDL_Log(
            """"""
            "> fps: %lf\n"
            "> player: %4.4lf, %4.4lf, %4.4lf"
            "> center: %4.4lf, %4.4lf, %4.4lf"
            ,fps
            ,client->generic.pos.x,client->generic.pos.y,client->generic.pos.z
            ,loaded_chunks.center_pos[0],loaded_chunks.center_pos[1],loaded_chunks.center_pos[2]
        );
        load_around_player(client);
        // leinad_render_update_textures();
        // if (loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH) / 2] == NULL) SDL_Log("outisde center chunk");
        frames = 0;
        last_ns = current_ns; 
    }

previous_ns = current_ns;
return SDL_APP_CONTINUE;
}

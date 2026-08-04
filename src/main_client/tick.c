#include <SDL3/SDL.h>

#include <leinad/data/app.h>
#include <leinad/world/loading.h>

#include <leinad/app.h>
#include <leinad/player.h>

#include "../render/render_main.c"

SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {

    static int frames = 0;
    static Uint64 last_ns = 0;
    
  { // render world
    ENFORCE(leinad_render_main(client->generic.pos,client->generic.rotation));
    frames++;
  }

  { // check the time since last tick
    APP.current_ns = SDL_GetTicksNS();

    if (APP.current_ns - APP.previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
  
    leinad_update_entities();

    leinad_update_players();


    if (APP.current_ns - last_ns > 1000000000) {
        double fps = (double)(frames) / ((double)(APP.current_ns - last_ns)/1000000000);
        SDL_Log(
            """"""
            "> fps: %lf\n"
            "> player: %4.4lf, %4.4lf, %4.4lf\n"
            "> center: %4.4lf, %4.4lf, %4.4lf\n"
            "> rotation: %4.2f, %4.2f"
            ,fps
            ,client->generic.pos.x,client->generic.pos.y,client->generic.pos.z
            ,loaded_chunks.center_pos[0],loaded_chunks.center_pos[1],loaded_chunks.center_pos[2]
            ,client->generic.rotation.yaw, client->generic.rotation.pitch
        );
        load_around_player(client);
        // leinad_render_update_textures();
        // if (loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH) / 2] == NULL) SDL_Log("outisde center chunk");
        frames = 0;
        last_ns = APP.current_ns; 
    }

    APP.previous_ns = APP.current_ns;
    return SDL_APP_CONTINUE;
}

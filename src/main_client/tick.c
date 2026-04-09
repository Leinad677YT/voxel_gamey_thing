#include <SDL3/SDL.h>

#include "../data/app.h"
#include "../data/globals.h"

#include "../world/render.c"

extern int leinad_render2_ui();

SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {
    
    
    //   { // set render background
    //     SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT_FLOAT);
//     SDL_RenderClear(renderer);
//   } 


{ // render world
    switch (leinad_render_world()){
        case SDL_APP_CONTINUE: break;
        case SDL_APP_FAILURE: return SDL_APP_FAILURE; break;
        case SDL_APP_SUCCESS: return SDL_APP_SUCCESS; break;
    }
    
}


//   { // leinad_update_ui(ui_active->ui);
//     leinad_render_ui();

//     SDL_RenderPresent(renderer);
//   }  

  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
    x+= toadd_x / 16.0 ;
    y+= toadd_y / 16.0;
    z+= toadd_z / 16.0;



previous_ns = current_ns;
return SDL_APP_CONTINUE;
}

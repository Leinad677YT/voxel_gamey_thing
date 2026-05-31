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
    ENFORCE(leinad_render_world((struct leinad_position){pos_x,pos_y,pos_z},(vec3){-pos_x,-pos_y,-pos_z}));
  }


//   { // leinad_update_ui(ui_active->ui);
//     leinad_render_ui();

//     SDL_RenderPresent(renderer);
//   }  

  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
    pos_x+= toadd_x / 32.0 ;
    pos_y+= toadd_y / 32.0;
    pos_z+= toadd_z / 32.0;



previous_ns = current_ns;
return SDL_APP_CONTINUE;
}

#include <SDL3/SDL.h>

#include <leinad/data/app.h>
#include <leinad/data/globals.h>

#include "../world/render.c"

extern int leinad_render2_ui();


SDL_AppResult SDL_AppIterate( __attribute__ ((unused)) void *appstate) {
    
// static struct test {double a;int b;} times[20] = {0};
// static int i = {0};
// static int frames = 0;
    
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
//   frames++;
  { // check the time since last tick
    current_ns = SDL_GetTicksNS();

    if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) return SDL_APP_CONTINUE;
  }
    pos_x+= toadd_x * (current_ns - previous_ns) / 100000000;
    pos_y+= toadd_y * (current_ns - previous_ns) / 100000000;
    pos_z+= toadd_z * (current_ns - previous_ns) / 100000000;

    // if (i < 20)
    //     times[i] = (struct test) {current_ns - previous_ns,frames};
    // else {
    //     long long aux;
    //     double _aux;

    //     for (int j = 0; j < 20; j++) {
    //         _aux+=times[j].a;
    //         aux+=times[j].b;
    //     }
    //     SDL_Log("fps: %lf",aux / (_aux) / 1000000);
    //     i = -1;
    // }
    // frames = 0;
    // i++;

previous_ns = current_ns;
return SDL_APP_CONTINUE;
}

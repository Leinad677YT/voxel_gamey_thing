#include <SDL3/SDL.h>

#include "../data/app.h"
#include "../data/globals.h"

static SDL_Texture *texture = NULL;
static int texture_width = 0;
static int texture_height = 0;

int leinad_load_ui(){
    
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_asprintf(&png_path, "%sresources/ui/textures/empty-button.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */

    texture_width = surface->w;
    texture_height = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

    return SDL_APP_CONTINUE;  /* carry on with the program! */

    return true;
}


int leinad_render2_ui(){
    SDL_FRect dst_rect;
    const Uint64 now = SDL_GetTicks();

    // /* we'll have some textures move around over a few seconds. */
    // const float direction = ((now % 2000) >= 1000) ? 1.0f : -1.0f;
    // const float scale = ((float) (((int) (now % 1000)) - 500) / 500.0f) * direction;

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF,0xFF, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    // SDL_RenderClear(renderer);  /* start with a blank canvas. */

    /* Just draw the static texture a few times. You can think of it like a
       stamp, there isn't a limit to the number of times you can draw with it. */

    /* top left */
    dst_rect.x = (0.0f);
    dst_rect.y = 0.0f;
    dst_rect.w = (float) texture_width;
    dst_rect.h = (float) texture_height;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    /* center this one. */
    dst_rect.x = ((float) (LEINAD_WINDOW_WIDTH - texture_width)) / 2.0f;
    dst_rect.y = ((float) (LEINAD_WINDOW_HEIGHT - texture_height)) / 2.0f;
    dst_rect.w = (float) texture_width;
    dst_rect.h = (float) texture_height;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    /* bottom right. */
    dst_rect.x = ((float) (LEINAD_WINDOW_WIDTH - texture_width)) - (0.0f);
    dst_rect.y = (float) (LEINAD_WINDOW_HEIGHT - texture_height);
    dst_rect.w = (float) texture_width + 0;
    dst_rect.h = (float) texture_height;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    SDL_RenderPresent(renderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
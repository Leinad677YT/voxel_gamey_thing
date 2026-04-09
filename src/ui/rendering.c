#include <SDL3/SDL.h>

#include "rendering.h"
#include "data.h"


SDL_Texture* leinad_load_texture(SDL_Renderer* render, const char* texture_name){
    SDL_Texture* texture = NULL;
    SDL_Surface *surface = NULL;
    char *png_path = NULL;


    /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
       engines refer to these as "sprites." We'll do a static texture (upload once, draw many
       times) with data from a png file. */

    /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
       Load a .png into a surface, move it to a texture from there. */
    SDL_asprintf(&png_path, "%sresources/ui/textures/%s.png", SDL_GetBasePath(), texture_name);  /* allocate a string of the full file path */
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return NULL;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */

    texture = SDL_CreateTextureFromSurface(render, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return NULL;
    }

    SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

    return texture;  /* carry on with the program! */
}


#define RENDER(renderer) if (!SDL_RenderTexture(renderer,welem->texture,&src_rect,&dst_rect)){ SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Could not render a button"); ret = false;}

bool leinad_render_9crop(const leinad_welem_instance_t* welem, SDL_Renderer* render){
    SDL_FRect src_rect, dst_rect;
    bool ret = true;

    leinad_welem_t* data = leinad_get_welem_data(welem->id);


    { // TOP LEFT
        src_rect.x = 0;
        src_rect.y = 0;
        src_rect.h = LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x;
        dst_rect.y = welem->y;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    { // MIDDLE UP
        src_rect.x = LEINAD_TEXTURE_9CROP_W;
        src_rect.y = 0;
        src_rect.h = LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_WIDTH - 2*LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = data->width - 2*LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    { // TOP RIGHT
        src_rect.x = LEINAD_TEXTURE_WIDTH - LEINAD_TEXTURE_9CROP_W;
        src_rect.y = 0;
        src_rect.h = LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + data->width - LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    { // MIDDLE LEFT
        src_rect.x = 0;
        src_rect.y = LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_HEIGHT - 2*LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x;
        dst_rect.y = welem->y + LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = data->height - 2*LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);

    }

    { // MIDDLE
        src_rect.x = LEINAD_TEXTURE_9CROP_W;
        src_rect.y = LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_HEIGHT - 2*LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_WIDTH - 2*LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y + LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = data->height - 2*LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = data->width - 2*LEINAD_TEXTURE_9CROP_W;

        RENDER(render);

    }

    { // MIDDLE RIGHT
        src_rect.x = LEINAD_TEXTURE_WIDTH - LEINAD_TEXTURE_9CROP_W;
        src_rect.y = LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_HEIGHT - 2*LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + data->width - LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y + LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = data->height - 2*LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);

    }

    { // BOTTOM LEFT
        src_rect.x = 0;
        src_rect.y = LEINAD_TEXTURE_HEIGHT - LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x;
        dst_rect.y = welem->y + data->height - LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    { // BOTTOM MIDDLE
        src_rect.x = LEINAD_TEXTURE_9CROP_W;
        src_rect.y = LEINAD_TEXTURE_HEIGHT - LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_9CROP_H;
        src_rect.w = LEINAD_TEXTURE_WIDTH - 2*LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y + data->height - LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = data->width - 2*LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    { // BOTTOM RIGHT
        src_rect.x = LEINAD_TEXTURE_WIDTH - LEINAD_TEXTURE_9CROP_W;
        src_rect.y = LEINAD_TEXTURE_HEIGHT - LEINAD_TEXTURE_9CROP_H;
        src_rect.h = LEINAD_TEXTURE_HEIGHT;
        src_rect.w = LEINAD_TEXTURE_9CROP_W;

        dst_rect.x = welem->x + data->width - LEINAD_TEXTURE_9CROP_W;
        dst_rect.y = welem->y + data->height - LEINAD_TEXTURE_9CROP_H;
        dst_rect.h = LEINAD_TEXTURE_9CROP_H;
        dst_rect.w = LEINAD_TEXTURE_9CROP_W;

        RENDER(render);
    }

    return ret;
}

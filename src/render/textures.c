#include "textures.h"



#include <SDL3/SDL.h>

SDL_Surface* leinad_load_png(const char* texture_name) {
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_asprintf(&png_path, "%sresources/textures/%s.png", SDL_GetBasePath(), texture_name);

    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return NULL;
    }

    int a = surface->w;

    SDL_free(png_path);

    return surface;
}

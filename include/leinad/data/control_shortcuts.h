#pragma once

// propagates the returned status of `foo` if it's `SDL_APP_SUCCESS` or `SDL_APP_FAILURE`
#define ENFORCE(foo) switch (foo) {case SDL_APP_SUCCESS: return SDL_APP_SUCCESS; break; case SDL_APP_FAILURE: return SDL_APP_FAILURE; break; default: break;}

// looks cooler B)
#define loop() for(;;)

typedef enum leinad_return {
    LEINAD_RETURN_CONTINUE = SDL_APP_CONTINUE,
    LEINAD_RETURN_SUCCESS = SDL_APP_SUCCESS,
    LEINAD_RETURN_FAILURE = SDL_APP_FAILURE,
} leinad_return_t;


// reallocs the memory on 1.5 geometric series until size fits in, assumes dir != NULL and size > 0
leinad_return_t ensure_capacity(void** restrict dir, const size_t size, size_t* restrict current_max) {
    void* new_dir = NULL;
    bool req = false;

    while(size >= *current_max) {
        *current_max = (size_t)(*current_max * 1.5);
        req = true;
    }

    if (!req) return LEINAD_RETURN_CONTINUE;

    new_dir = SDL_realloc(*dir, *current_max);

    if (new_dir == NULL) return LEINAD_RETURN_FAILURE;

    *dir = new_dir;
    return LEINAD_RETURN_CONTINUE;

}


#pragma once

// propagates the returned status of `foo` if it's `SDL_APP_SUCCESS` or `SDL_APP_FAILURE`
#define ENFORCE(foo) switch (foo) {case SDL_APP_SUCCESS: return SDL_APP_SUCCESS; break; case SDL_APP_FAILURE: return SDL_APP_FAILURE; break; default: break;}

// looks cooler B)
#define loop() for(;;)
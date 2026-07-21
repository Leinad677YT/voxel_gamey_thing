#pragma once

// All app-relevant info will be dumped here

#include <SDL3/SDL.h>

#define LEINAD_APP_NAME "VoxelsAndStuff"
#define LEINAD_APP_VERSION "0.1"
#define LEINAD_APP_IDENTIFIER "leinad.voxel_game"

#define LEINAD_WINDOW_TITLE "winwow"
#define LEINAD_WINDOW_WIDTH 1080
#define LEINAD_WINDOW_HEIGHT 720
#define LEINAD_WINDOW_FLAGS 0 // | SDL_WINDOW_FULLSCREEN // | SDL_WINDOW_SURFACE_VSYNC_DISABLED
#define LEINAD_TICK_RANGE_NS 50000000

#define LEINAD_SERVER_ADDR "127.0.0.1"
#define LEINAD_SERVER_PORT 6666

#define LEINAD_CLIENT_ADDR "127.0.0.1"
#define LEINAD_CLIENT_PORT 7777

#define LEINAD_MAX_PATH_LENGTH 256


struct appdata {
    
};

int a = 1;
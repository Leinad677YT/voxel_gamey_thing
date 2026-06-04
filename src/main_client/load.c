#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#include <leinad/data/globals.h>
#include <leinad/data/app.h>

#include "../render/shaders.h"

#include "../render/init.c"

void AUX_PRINT(void* IGNORED){

    SDL_Log("YIPPIEE");

    return;
}


SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {

    SDL_GPUShader *vertex_shader;
    SDL_GPUShader *fragment_shader;
    SDL_GPUTransferBuffer *transfer_buffer;
    SDL_GPUCommandBuffer *cmd_buf;

  { // initialize SDL subsystem

    SDL_SetAppMetadata(
        LEINAD_APP_NAME,
        LEINAD_APP_VERSION,
        LEINAD_APP_IDENTIFIER
    );

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!NET_Init()) {
        SDL_Log("Couldn't initialize SDL_net: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
  }
  
  { // initialize gpu

    device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true,
        NULL
    );

    if (!device) {
        SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
  }

  { // initialize window and renderer

    window = SDL_CreateWindow (
        LEINAD_WINDOW_TITLE,
        LEINAD_WINDOW_WIDTH,
        LEINAD_WINDOW_HEIGHT,
        0 | LEINAD_WINDOW_FLAGS
    );

    if (window == NULL) {
    SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    renderer = SDL_CreateGPURenderer(
        device,
        window
    );

    if (renderer == NULL) {
    SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
  }

    { // assign window to gpu

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_Log("Couldn't claim window for GPU device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
  }

  if (INIT_render()) {
        SDL_Log("Couldn't init rendering shaders\n > SDL_ERRROR [%s]", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Uint32 drawablew, drawableh;
    SDL_GetWindowSizeInPixels(window, (int *)&drawablew, (int *)&drawableh);
    depth_texture = CreateDepthTexture(drawablew, drawableh);


    { // init server and client

    NET_Address** local_addrs;
    int addr_amount, i;

    client_addr = NET_ResolveHostname(LEINAD_CLIENT_ADDR);
    server_addr = NET_ResolveHostname(LEINAD_SERVER_ADDR);

    NET_WaitUntilResolved(client_addr, -1);
    NET_WaitUntilResolved(server_addr, -1);

    server = NET_CreateServer(server_addr, LEINAD_SERVER_PORT);
    client_sock = NET_CreateClient(client_addr, LEINAD_SERVER_PORT);

    NET_WaitUntilConnected(client_sock, -1);

  }

    current_ns = SDL_GetTicksNS();
    previous_ns = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;
}

#include <SDL3/SDL.h>

#include "../data/globals.h"
#include "../data/app.h"

#include "../ui/data.h"
#include "../ui/screens.h"
#include "../ui/elements.h"
#include "../ui/elements/button.h"

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
        0
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

  { // create render stack
    leinad_create_stack(&ui_render_stack, 30);
  }


  { // load yippiee

    leinad_create_button(
        LEINAD_WELEMD_YIPIEE_BUTTON,
        80,60,
        2,2,
        AUX_PRINT
    );

    leinad_ui_t* ui = uiStart_load();

    leinad_ui_instance_t* ui_instance = leinad_instanciate_ui(ui,0,0,2);

    if (
        ui_instance == NULL
        || !leinad_push_ui(ui_instance)
        || !leinad_set_active_ui(ui_instance)
    ) {
        leinad_destroy_ui_instance(ui_instance);
        leinad_destroy_ui(ui);
        return SDL_APP_FAILURE;
    }

  }

    Uint32 drawablew, drawableh;
    SDL_GetWindowSizeInPixels(window, (int *)&drawablew, (int *)&drawableh);
    depth_texture = CreateDepthTexture(drawablew, drawableh);


    current_ns = SDL_GetTicksNS();
    previous_ns = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;
}

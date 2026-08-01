#include <leinad/data/control_shortcuts.h>
#include <leinad/data/globals.h>
#include <leinad/data/types.h>
#include <leinad/data/tags.h>

#include "render_world.c"

LEINAD_FCALL int leinad_render_main(struct leinad_position pos, struct leinad_rotation rot) {
    SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
    if (cmdbuf == NULL) {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUTexture* swapchain;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchain, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (swapchain != NULL) {

        ENFORCE(leinad_render_world(cmdbuf, swapchain, pos, rot));

    }

    if (!SDL_SubmitGPUCommandBuffer(cmdbuf)) SDL_Log("Could not submit the command buffer:\n> %s",SDL_GetError());


    return SDL_APP_CONTINUE;
}
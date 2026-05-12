#include <SDL3/SDL.h>

#include "../data/globals.h"
#include "../world/render.h"

void SDL_AppQuit(
    __attribute__ ((unused)) void *appstate,
    __attribute__ ((unused)) SDL_AppResult result
) {

    // free gpu mine
    SDL_ReleaseGPUBuffer(device, vertex_buffer);
    SDL_ReleaseGPUSampler(device,sampler);
    SDL_ReleaseGPUTexture(device, gpu_texture);
    SDL_ReleaseGPUTexture(device, depth_texture);
    SDL_ReleaseGPUGraphicsPipeline(device,pipeline);
    SDL_DestroyRenderer(renderer);

    leinad_render_end();

    // free gpu example
	SDL_ReleaseGPUGraphicsPipeline(device, ScenePipeline);
	SDL_ReleaseGPUTexture(device, SceneColorTexture);
	SDL_ReleaseGPUTexture(device, SceneDepthTexture);
	SDL_ReleaseGPUBuffer(device, SceneVertexBuffer);
	SDL_ReleaseGPUBuffer(device, SceneIndexBuffer);

	SDL_ReleaseGPUGraphicsPipeline(device, EffectPipeline);
	SDL_ReleaseGPUBuffer(device, EffectVertexBuffer);
	SDL_ReleaseGPUBuffer(device, EffectIndexBuffer);
	SDL_ReleaseGPUSampler(device, EffectSampler);

    SDL_ReleaseWindowFromGPUDevice(device, window);

    // free window
    SDL_DestroyWindow(window);
    SDL_DestroyGPUDevice(device);

}

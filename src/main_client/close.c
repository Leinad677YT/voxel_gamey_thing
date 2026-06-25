#include <SDL3/SDL.h>

#include <leinad/data/globals.h>
#include <leinad/render.h>

void SDL_AppQuit(
    __attribute__ ((unused)) void *appstate,
    __attribute__ ((unused)) SDL_AppResult result
) {

    // free gpu mine
    if (depth_texture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, depth_texture);
    if (SkyPipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device,SkyPipeline);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);

    leinad_render_end();

    // free gpu example
    if (ScenePipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device, ScenePipeline);
    if (TransparencyPipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device, TransparencyPipeline);
    if (AuxTransparencyPipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device, AuxTransparencyPipeline);
    if (FrontPipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device, FrontPipeline);
    if (SceneColorTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, SceneColorTexture);
    if (SceneTransparencyTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, SceneTransparencyTexture);
    if (AuxTransparencyTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, AuxTransparencyTexture);
    if (FrontTransparencyTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, FrontTransparencyTexture);
    if (FrontBGTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, FrontBGTexture);
    if (SceneDepthTexture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, SceneDepthTexture);

    if (EffectPipeline != NULL && device != NULL) SDL_ReleaseGPUGraphicsPipeline(device, EffectPipeline);
    if (EffectVertexBuffer != NULL && device != NULL) SDL_ReleaseGPUBuffer(device, EffectVertexBuffer);
    if (EffectIndexBuffer != NULL && device != NULL) SDL_ReleaseGPUBuffer(device, EffectIndexBuffer);
    if (EffectSampler != NULL && device != NULL) SDL_ReleaseGPUSampler(device, EffectSampler);
    if (AuxiliarySampler != NULL && device != NULL) SDL_ReleaseGPUSampler(device, AuxiliarySampler);
    if (Auxiliary2Sampler != NULL && device != NULL) SDL_ReleaseGPUSampler(device, Auxiliary2Sampler);

    if (window != NULL && device != NULL) SDL_ReleaseWindowFromGPUDevice(device, window);

    // free window
    if (window != NULL) SDL_DestroyWindow(window);
    if (device != NULL) SDL_DestroyGPUDevice(device);


    // finish network connections
    if (client_sock != NULL) NET_DestroyStreamSocket(client_sock);
    
    NET_Quit();
    SDL_Quit();

}

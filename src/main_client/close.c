#include <SDL3/SDL.h>

#include <leinad/data/globals.h>
#include <leinad/render.h>
#include <leinad/app.h>

void SDL_AppQuit(
    __attribute__ ((unused)) void *appstate,
    __attribute__ ((unused)) SDL_AppResult result
) {

    // free gpu mine
    if (depth_texture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, depth_texture);
    if (SkyPipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device,SkyPipeline);

    leinad_render_end();

    // free gpu example
    if (ScenePipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device, ScenePipeline);
    if (TransparencyPipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device, TransparencyPipeline);
    if (AuxTransparencyPipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device, AuxTransparencyPipeline);
    if (FrontPipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device, FrontPipeline);
    if (SceneColorTexture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, SceneColorTexture);
    if (SceneTransparencyTexture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, SceneTransparencyTexture);
    if (AuxTransparencyTexture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, AuxTransparencyTexture);
    if (FrontTransparencyTexture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, FrontTransparencyTexture);
    if (FrontBGTexture != NULL && APP.device != NULL) SDL_ReleaseGPUTexture(APP.device, FrontBGTexture);

    if (EffectPipeline != NULL && APP.device != NULL) SDL_ReleaseGPUGraphicsPipeline(APP.device, EffectPipeline);
    if (EffectVertexBuffer != NULL && APP.device != NULL) SDL_ReleaseGPUBuffer(APP.device, EffectVertexBuffer);
    if (EffectIndexBuffer != NULL && APP.device != NULL) SDL_ReleaseGPUBuffer(APP.device, EffectIndexBuffer);
    if (EffectSampler != NULL && APP.device != NULL) SDL_ReleaseGPUSampler(APP.device, EffectSampler);
    if (AuxiliarySampler != NULL && APP.device != NULL) SDL_ReleaseGPUSampler(APP.device, AuxiliarySampler);
    if (Auxiliary2Sampler != NULL && APP.device != NULL) SDL_ReleaseGPUSampler(APP.device, Auxiliary2Sampler);

    if (APP.window != NULL && APP.device != NULL) SDL_ReleaseWindowFromGPUDevice(APP.device, APP.window);

    // free window
    if (APP.window != NULL) SDL_DestroyWindow(APP.window);
    if (APP.device != NULL) SDL_DestroyGPUDevice(APP.device);


    // finish network connections
    if (client_sock != NULL) NET_DestroyStreamSocket(client_sock);
    
    NET_Quit();
    SDL_Quit();

}

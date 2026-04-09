#include "../data/globals.h"
#include "../math/matrix.h"
#include "../math/trigonometry.h"

static Uint64 current_ms = 0;
static Uint64 previous_ms = 0;


int leinad_render_world(){
    SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
    if (cmdbuf == NULL) {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return -1;
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

    if (swapchainTexture != NULL) {
        // Render the 3D Scene (Color and Depth pass)
        float nearPlane = 2.0f;
        float farPlane = 600.0f;

        matrix4x4 proj = Matrix4x4_CreatePerspectiveFieldOfView(
            80.0f * SDL_PI_F / 180.0f,
            SceneWidth / (float)SceneHeight,
            nearPlane,
            farPlane
        );
        matrix4x4 view = Matrix4x4_CreateLookAt( // si CAM - LOOKAT = vec(0,whatever,0) hay div0
            (vec3) { x, y, z },
            (vec3) { 0, 0, 0 },
            (vec3) { 0, 1, 0 }
        );

        matrix4x4 viewproj = Matrix4x4_Multiply(view, proj);

        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = SceneColorTexture;
        colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 1.0f, 1.0f, 0.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = { 0 };
        depthStencilTargetInfo.texture = SceneDepthTexture;
        depthStencilTargetInfo.cycle = true;
        depthStencilTargetInfo.clear_depth = 1;
        depthStencilTargetInfo.clear_stencil = 0;
        depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
        depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;

        SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
        SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { nearPlane, farPlane }, 8);

        if (TEXTURE == NULL || TEXTURESampler == NULL) {
            SDL_Log("woops"); return 1;
        }

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, &depthStencilTargetInfo);
        SDL_BindGPUGraphicsPipeline(renderPass, ScenePipeline);
        SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){.buffer = SceneVertexBuffer, .offset = 0 }, 1);
        SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = SceneIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
            { .texture = TEXTURE, .sampler = TEXTURESampler }
        }, 1);
        SDL_DrawGPUIndexedPrimitives(renderPass, 36, 1, 0, 0, 0);
        SDL_EndGPURenderPass(renderPass);

        // Render the Outline Effect that samples from the Color/Depth textures
        SDL_GPUColorTargetInfo swapchainTargetInfo = { 0 };
        swapchainTargetInfo.texture = swapchainTexture;
        swapchainTargetInfo.clear_color = (SDL_FColor){ 0.2f, 0.5f, 0.4f, 1.0f };
        swapchainTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        swapchainTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(cmdbuf, &swapchainTargetInfo, 1, NULL);
        SDL_BindGPUGraphicsPipeline(renderPass, EffectPipeline);
        SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
        SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
        	{ .texture = SceneColorTexture, .sampler = EffectSampler },
        	{ .texture = SceneDepthTexture, .sampler = EffectSampler }
        }, 2);
        SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);

    return SDL_APP_CONTINUE;
}

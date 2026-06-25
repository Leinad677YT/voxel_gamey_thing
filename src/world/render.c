#include <leinad/render.h>

#include <leinad/data/types.h>
#include <leinad/data/app.h>
#include <leinad/data/globals.h>
#include <leinad/data/control_shortcuts.h>

#include "../libs/io.h"
#include "../libs/bit_manipulation.h"

#include <leinad/type/vec3.h>
#include <leinad/type/matrix4x4.h>

#include "../render/textures.h"
#include "data.h"

LEINAD_FCALL int leinad_render_world(struct leinad_position pos, vec3 view_vec){
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

        SDL_GPURenderPass* renderPass;
        matrix4x4 viewproj;

        // Render the 3D Scene (Color and Depth pass)
        float nearPlane = 0.125f;
        float farPlane = 600.0f;

        struct _chunkrenderdata chunk_renderdata = {
            .pos = pos,
            .viewvec = view_vec
        };


      { // create viewproj matrix
            matrix4x4 proj = Matrix4x4_CreatePerspectiveFieldOfView(
                100.0f * SDL_PI_F / 180.0f,
                SceneWidth / (float)SceneHeight,
                nearPlane,
                farPlane
            );
            matrix4x4 view = Matrix4x4_CreateLookAt( // si CAM - LOOKAT = vec(0,whatever,0) hay div0
                (vec3) { pos.x, pos.y, pos.z },
                (vec3) { pos.x + view_vec.x,pos.y + view_vec.y,pos.z + view_vec.z },
                (vec3) { 0, 1, 0 }
            );

            viewproj = Matrix4x4_Multiply(view, proj);
      }

    //   SDL_Log("pre_sky\n");
      { // draw sky @todo hacerlo bonito
        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = SceneColorTexture; // RGBA
        colorTargetInfo.clear_color = (SDL_FColor){ 0.2f, 0.7f, 1.0f, 0.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
        SDL_BindGPUGraphicsPipeline(renderPass, SkyPipeline);
        SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
        SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
        SDL_EndGPURenderPass(renderPass);

      }

    //   SDL_Log("pre_opaque\n");
      { // draw opaques
        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = SceneColorTexture; // RGBA
        colorTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = { 0 };
        depthStencilTargetInfo.texture = SceneDepthTexture;
        depthStencilTargetInfo.cycle = true;
        depthStencilTargetInfo.clear_depth = 1;
        depthStencilTargetInfo.clear_stencil = 0;
        depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
        SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { nearPlane, farPlane }, 2*sizeof(float));

        { // render all opaque block parts

        renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, &depthStencilTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, ScenePipeline);
        chunk_renderdata.renderpass = renderPass;
        loaded_chunks_forall_decreasing(leinad_chunk_render_opaque, &chunk_renderdata);
        SDL_EndGPURenderPass(renderPass);

        }

        { // @todo render entities

        }

      }

    //   SDL_Log("pre_transparency\n");
      { // @todo draw transparency
        SDL_GPUColorTargetInfo colorTargetInfo[2] = { 0 };
        colorTargetInfo[0].texture = SceneTransparencyTexture; // RGBA
        colorTargetInfo[0].clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f };
        colorTargetInfo[0].load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo[0].store_op = SDL_GPU_STOREOP_STORE;

        colorTargetInfo[1].texture = AuxTransparencyTexture; // RGBA
        colorTargetInfo[1].clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f };
        colorTargetInfo[1].load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo[1].store_op = SDL_GPU_STOREOP_STORE;

        
        SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = { 0 };
        depthStencilTargetInfo.texture = SceneDepthTexture;
        depthStencilTargetInfo.cycle = false;
        // depthStencilTargetInfo.clear_depth = 1;
        depthStencilTargetInfo.clear_stencil = 0;
        depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
        depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
        SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { nearPlane, farPlane }, 2*sizeof(float));

        { // render all translucent block parts

        renderPass = SDL_BeginGPURenderPass(cmdbuf, colorTargetInfo, 2,&depthStencilTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, TransparencyPipeline);
        chunk_renderdata.renderpass = renderPass;
        loaded_chunks_forall_decreasing(leinad_chunk_render_transparent, &chunk_renderdata);
        SDL_EndGPURenderPass(renderPass);

        }

        { // @todo render entities

        }

      }




    //   SDL_Log("pre_front\n");
      { // @todo draw front transparency
        SDL_GPUColorTargetInfo colorTargetInfo[2] = { 0 };
        colorTargetInfo[0].texture = FrontTransparencyTexture; // RGBA
        colorTargetInfo[0].clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f };
        colorTargetInfo[0].load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo[0].store_op = SDL_GPU_STOREOP_STORE;

        colorTargetInfo[1].texture = FrontBGTexture; // RGBA
        colorTargetInfo[1].clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f };
        colorTargetInfo[1].load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo[1].store_op = SDL_GPU_STOREOP_STORE;

        
        SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = { 0 };
        depthStencilTargetInfo.texture = SceneDepthTexture;
        depthStencilTargetInfo.cycle = false;
        // depthStencilTargetInfo.clear_depth = 1;
        depthStencilTargetInfo.clear_stencil = 0;
        depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
        depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
        SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { nearPlane, farPlane }, 2*sizeof(float));

        { // render all translucent block parts

        renderPass = SDL_BeginGPURenderPass(cmdbuf, colorTargetInfo, 2,&depthStencilTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, FrontPipeline);
        chunk_renderdata.renderpass = renderPass;
        loaded_chunks_forall_decreasing(leinad_chunk_render_front, &chunk_renderdata);
        SDL_EndGPURenderPass(renderPass);

        }

        { // @todo render entities

        }

      }


    //   SDL_Log("pre_merge\n");
      { // mix transparents and opaques

        SDL_GPUColorTargetInfo textureTargetInfo = { 0 };
        textureTargetInfo.texture = SceneColorTexture; // RGBA
        textureTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
        textureTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(cmdbuf, &textureTargetInfo, 1, NULL);
        SDL_BindGPUGraphicsPipeline(renderPass, AuxTransparencyPipeline);
        SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
        SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
            { .texture = SceneTransparencyTexture, .sampler = EffectSampler },
            { .texture = AuxTransparencyTexture, .sampler = AuxiliarySampler },
            { .texture = FrontTransparencyTexture, .sampler = Auxiliary2Sampler }
        }, 3);
        SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
        SDL_EndGPURenderPass(renderPass);
      }

    //   SDL_Log("pre_outline\n");
      { // draw outline

        SDL_GPUColorTargetInfo swapchainTargetInfo = { 0 };
        swapchainTargetInfo.texture = swapchainTexture; // RGBA
        swapchainTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f };
        swapchainTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        swapchainTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(cmdbuf, &swapchainTargetInfo, 1, NULL);
        SDL_BindGPUGraphicsPipeline(renderPass, EffectPipeline);
        SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
        SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
            { .texture = SceneColorTexture, .sampler = EffectSampler },
            { .texture = SceneDepthTexture, .sampler = AuxiliarySampler },
            { .texture = FrontBGTexture, .sampler = Auxiliary2Sampler }
        }, 3);
        SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
        SDL_EndGPURenderPass(renderPass);
      }

    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);

    return SDL_APP_CONTINUE;
}










LEINAD_AUX static int _update_atlas(struct render_atlas *atlas, char* root_path, Uint32 tx_amount, int tx_w, int tx_h);

LEINAD_FCALL int leinad_render_update_textures(){
    
    // UPDATE THE ATLASES
    ENFORCE(_update_atlas(&block_atlas, "block", LEINAD_BLOCKTX_amount,LEINAD_TEXTURE_RADIUS,LEINAD_TEXTURE_RADIUS));
    
    return SDL_APP_CONTINUE;
} 


LEINAD_FCALL int leinad_render_init() {

    ENFORCE(leinad_render_update_textures());

    return SDL_APP_CONTINUE;
}


LEINAD_FCALL void leinad_render_end() {
    if (block_atlas.sampler != NULL && device != NULL) SDL_ReleaseGPUSampler(device, block_atlas.sampler);
    if (block_atlas.texture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, block_atlas.texture);

    if (chunk_test == NULL) return;

    if (chunk_test->mesh[0]->vertex != NULL && device != NULL) SDL_ReleaseGPUBuffer(device, chunk_test->mesh[0]->vertex);
    if (chunk_test->mesh[0]->index != NULL && device != NULL) SDL_ReleaseGPUBuffer(device, chunk_test->mesh[0]->index);
}

LEINAD_AUX static int _update_atlas(struct render_atlas *atlas, char* root_path, Uint32 tx_amount, int tx_w, int tx_h) {
    Uint32 i;
    char path[LEINAD_MAX_PATH_LENGTH];
    char current_tx[LEINAD_MAX_LINE_LENGTH];
    SDL_IOStream *atlas_io;
    SDL_Surface *tx_surface, *atlas_surface;
    SDL_Rect target = {0,0,tx_w,tx_h};

    if (atlas->texture != NULL) SDL_ReleaseGPUTexture(device,atlas->texture);
    if (atlas->sampler != NULL) SDL_ReleaseGPUSampler(device,atlas->sampler);

    // create the atlas surface
    atlas->width = pow2(next_pow4(tx_amount));
    atlas->height = pow2(next_pow4(tx_amount));
    atlas_surface = SDL_CreateSurface(atlas->width * tx_w,atlas->height * tx_h, SDL_PIXELFORMAT_ARGB8888);
    
    // open the atlas redirection table
    SDL_snprintf(path,LEINAD_MAX_PATH_LENGTH, "%s/resources/atlas/%s.txt", SDL_GetBasePath(), root_path);
    atlas_io = SDL_IOFromFile(path, "r");

    for (i = 0; i < tx_amount; i++) {
        // you shall start reading NOW!
        if (
            leinad_io_readline(atlas_io, current_tx) <= 0
        ) continue;
        tx_surface = NULL;
        SDL_snprintf(path,LEINAD_MAX_PATH_LENGTH, "%s/%s", root_path, current_tx);

        tx_surface = leinad_load_png(path);

        // blit texture on the atlas
        target.x = (i % (atlas->width)) * tx_w;
        target.y = (i / (atlas->width)) * tx_h;
        SDL_BlitSurface(tx_surface, NULL, atlas_surface, &target);

        SDL_DestroySurface(tx_surface);
    }

    const SDL_GPUTextureCreateInfo texture_info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
        .width = atlas->width * tx_w,
        .height = atlas->height * tx_h,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    };
    atlas->texture = SDL_CreateGPUTexture(device, &texture_info);

    const SDL_GPUSamplerCreateInfo sampler_info = {
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    };
    atlas->sampler = SDL_CreateGPUSampler(device, &sampler_info);

    if (atlas->texture == NULL) {
        SDL_Log("Couldn't reload the %s atlas: %s",root_path, SDL_GetError());
        SDL_CloseIO(atlas_io);
        return SDL_APP_FAILURE;
    }

        SDL_GPUTransferBufferCreateInfo transfer_info_texture = {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = (Uint32)(atlas->width * tx_w * atlas->height * tx_h * 4) // 4B / pixel
        };

        SDL_GPUTransferBuffer* textureTransferBuffer = SDL_CreateGPUTransferBuffer(
            device,
            &transfer_info_texture
        );

        Uint8* textureTransferPtr = (Uint8*)SDL_MapGPUTransferBuffer(
            device,
            textureTransferBuffer,
            false
        );
        SDL_memcpy(textureTransferPtr, atlas_surface->pixels, atlas->width * tx_w * atlas->height * tx_h * 4);
        SDL_UnmapGPUTransferBuffer(device, textureTransferBuffer);

        // copy pass
        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);



        SDL_UploadToGPUTexture(
            copyPass,
            &(SDL_GPUTextureTransferInfo) {
                .transfer_buffer = textureTransferBuffer,
                .offset = 0
            },
            &(SDL_GPUTextureRegion) {
                .texture = atlas->texture,
                .w = atlas->width * tx_w,
                .h = atlas->height * tx_h,
                .d = 1
            },
            false
        );
    
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuf);


        SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
        
                
    SDL_DestroySurface(atlas_surface);
    SDL_CloseIO(atlas_io);
    return SDL_APP_CONTINUE;
}

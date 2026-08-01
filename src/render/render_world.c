#include <SDL3/SDL.h>

#include <leinad/data/control_shortcuts.h>
#include <leinad/data/globals.h>
#include <leinad/data/types.h>
#include <leinad/data/tags.h>
#include <leinad/data/app.h>
#include <leinad/render.h>

#include "textures.h"

#include <leinad/world/loading.h>
#include <leinad/world/region.h>

#include <leinad/math/trigonometry.h>

LEINAD_FCALL int leinad_render_world(SDL_GPUCommandBuffer* restrict cmdbuf, SDL_GPUTexture* restrict swapchain, struct leinad_position pos, struct leinad_rotation rot){

    SDL_GPURenderPass* renderPass;

    vec3 view_vec = angles_to_vec3(degree_to_radian(rot.yaw),degree_to_radian(-rot.pitch));
    matrix4x4 viewproj;

    struct _chunkrenderdata chunk_renderdata = {
        .pos = pos,
        .rotation = rot,
        .fov_planes = {0},
        .viewvec = view_vec,
        .command_buffer = cmdbuf
    };


  { // create viewproj matrix
        matrix4x4 proj = Matrix4x4_CreatePerspectiveFieldOfView(
            degree_to_radian(RENDER.fov),
            SceneWidth / (float)SceneHeight,
            RENDER.near_plane,
            RENDER.far_plane
        );

        matrix4x4 view = Matrix4x4_CreateLookAt( // si CAM - LOOKAT = vec(0,whatever,0) hay div0
            // (vec3) { 1,130,1 },
            // (vec3) {0,0,0},
            (vec3) { pos.x, pos.y, pos.z },
            (vec3) {pos.x + view_vec.x, pos.y + view_vec.y, pos.z + view_vec.z},
            (vec3) { 0, 1, 0 }
        );

        viewproj = Matrix4x4_Multiply(view, proj);
  }

  { // get fov planes
    const vec3 resized_viewvec = {
        view_vec.x * (1 + sin(degree_to_radian(RENDER.fov / 2 + 2))),
        view_vec.y * (1 + sin(degree_to_radian(RENDER.fov / 2 + 2))),
        view_vec.z * (1 + sin(degree_to_radian(RENDER.fov / 2 + 2)))
    };
    vec3 aux;

    // LEFT

    // get normal
    aux = angles_to_vec3(degree_to_radian(rot.yaw - RENDER.fov/2 -2),degree_to_radian(-rot.pitch));
    aux = (vec3) {resized_viewvec.x - aux.x,resized_viewvec.y - aux.y,resized_viewvec.z - aux.z};
    aux = Vec3_Normalize(aux);

    // get plane constant
    chunk_renderdata.fov_planes[0].x = aux.x; chunk_renderdata.fov_planes[0].y = aux.y; chunk_renderdata.fov_planes[0].z = aux.z;
    chunk_renderdata.fov_planes[0].w = vec3_dot(aux,(vec3){pos.x - aux.x *LEINAD_MESH_RADIUS * 1.7,pos.y - aux.y *LEINAD_MESH_RADIUS * 1.7,pos.z - aux.z *LEINAD_MESH_RADIUS * 1.7});

    // RIGHT

    // get normal
    aux = angles_to_vec3(degree_to_radian(rot.yaw + RENDER.fov/2 +2),degree_to_radian(-rot.pitch));
    aux = (vec3) {resized_viewvec.x - aux.x,resized_viewvec.y - aux.y,resized_viewvec.z - aux.z};
    aux = Vec3_Normalize(aux);

    // get plane constant
    chunk_renderdata.fov_planes[1].x = aux.x; chunk_renderdata.fov_planes[1].y = aux.y; chunk_renderdata.fov_planes[1].z = aux.z;
    chunk_renderdata.fov_planes[1].w = vec3_dot(aux,(vec3){pos.x - aux.x *LEINAD_MESH_RADIUS * 1.7,pos.y - aux.y *LEINAD_MESH_RADIUS * 1.7,pos.z - aux.z *LEINAD_MESH_RADIUS * 1.7});

    // UP

    // get normal
    aux = angles_to_vec3(degree_to_radian(rot.yaw),degree_to_radian(-rot.pitch - SDL_atan(SceneWidth / (float)SceneHeight)));
    aux = (vec3) {resized_viewvec.x - aux.x,resized_viewvec.y - aux.y,resized_viewvec.z - aux.z};
    aux = Vec3_Normalize(aux);

    // get plane constant
    chunk_renderdata.fov_planes[2].x = aux.x; chunk_renderdata.fov_planes[2].y = aux.y; chunk_renderdata.fov_planes[2].z = aux.z;
    chunk_renderdata.fov_planes[2].w = vec3_dot(aux,(vec3){pos.x - aux.x *LEINAD_MESH_RADIUS * 1.7,pos.y - aux.y *LEINAD_MESH_RADIUS * 1.7,pos.z - aux.z *LEINAD_MESH_RADIUS * 1.7});

    // DOWN

    // get normal
    aux = angles_to_vec3(degree_to_radian(rot.yaw),degree_to_radian(-rot.pitch + SDL_atan(SceneWidth / (float)SceneHeight)));
    aux = (vec3) {resized_viewvec.x - aux.x,resized_viewvec.y - aux.y,resized_viewvec.z - aux.z};
    aux = Vec3_Normalize(aux);

    // get plane constant
    chunk_renderdata.fov_planes[3].x = aux.x; chunk_renderdata.fov_planes[3].y = aux.y; chunk_renderdata.fov_planes[3].z = aux.z;
    chunk_renderdata.fov_planes[3].w = vec3_dot(aux,(vec3){pos.x - aux.x *LEINAD_MESH_RADIUS * 1.7,pos.y - aux.y *LEINAD_MESH_RADIUS * 1.7,pos.z - aux.z *LEINAD_MESH_RADIUS * 1.7});


  }
    

  { // draw sky @todo hacerlo bonito
    SDL_GPUColorTargetInfo colorTargetInfo = {
        .texture = swapchain,
        .load_op = SDL_GPU_LOADOP_DONT_CARE,
        .store_op = SDL_GPU_STOREOP_STORE
    };

    renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
    SDL_BindGPUGraphicsPipeline(renderPass, SkyPipeline);

    SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
    SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);

    SDL_EndGPURenderPass(renderPass);

  }

  { // draw opaques
    SDL_GPUColorTargetInfo colorTargetInfo[2] = {
        { // swapchain
            .texture = swapchain,
            .load_op = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        },
        { // outline color
            .texture = FrontBGTexture,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .clear_color = {0,0,0,0},
            .store_op = SDL_GPU_STOREOP_STORE
        }
    };

    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {
        .texture = depth_texture,
        .cycle = true,
        
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .clear_depth = 1.0f,
        .store_op = SDL_GPU_STOREOP_STORE,
        
        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE
    };

    SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
    SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { RENDER.near_plane, RENDER.far_plane }, 2*sizeof(float));


    { // render all opaque block parts

        renderPass = SDL_BeginGPURenderPass(cmdbuf, colorTargetInfo, 2, &depthStencilTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, ScenePipeline);
        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
            { .texture = block_atlas.texture, .sampler = block_atlas.sampler }
        }, 1);

        chunk_renderdata.renderpass = renderPass;
        loaded_chunks_forall_increasing(leinad_chunk_render_opaque, &chunk_renderdata);

    }

    { // @todo render entities

    }

    SDL_EndGPURenderPass(renderPass);

  }

  { // draw transparency
    SDL_GPUColorTargetInfo colorTargetInfo[2] = {
        { // averaged written value translucency
            .texture = SceneTransparencyTexture,
            .cycle = false,

            .load_op = SDL_GPU_LOADOP_CLEAR,
            .clear_color = {0,0,0,0},
            .store_op = SDL_GPU_STOREOP_STORE
        },
        { // amount of times written translucency
            .texture = AuxTransparencyTexture,
            .cycle = false,

            .load_op = SDL_GPU_LOADOP_CLEAR,
            .clear_color = {0,0,0,0},
            .store_op = SDL_GPU_STOREOP_STORE
        }
    };

    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {
        .texture = depth_texture,
        .cycle = false,

        .load_op = SDL_GPU_LOADOP_LOAD,
        .store_op = SDL_GPU_STOREOP_STORE,

        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE
    };

    SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
    SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { RENDER.near_plane, RENDER.far_plane }, 2*sizeof(float));

    { // render all translucent block parts

    renderPass = SDL_BeginGPURenderPass(cmdbuf, colorTargetInfo, 2,&depthStencilTargetInfo);

    SDL_BindGPUGraphicsPipeline(renderPass, TransparencyPipeline);
    SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
        { .texture = block_atlas.texture, .sampler = block_atlas.sampler }
    }, 1);


    chunk_renderdata.renderpass = renderPass;
    loaded_chunks_forall_increasing(leinad_chunk_render_transparent, &chunk_renderdata);

    }

    { // @todo render entities

    }

    SDL_EndGPURenderPass(renderPass);

  }

  { // draw front transparency
    SDL_GPUColorTargetInfo colorTargetInfo[2] = {
        { // swapchain
            .texture = FrontTransparencyTexture,
            .cycle = true,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE
        },
        { // outline color
            .texture = FrontBGTexture,
            .cycle = false,
            .load_op = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        }
    };
    
    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {
        .texture = depth_texture,
        .cycle = false,
        
        .load_op = SDL_GPU_LOADOP_LOAD,
        .store_op = SDL_GPU_STOREOP_STORE,

        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
    };


    SDL_PushGPUVertexUniformData(cmdbuf, 0, &viewproj, sizeof(viewproj));
    SDL_PushGPUFragmentUniformData(cmdbuf, 0, (float[]) { RENDER.near_plane, RENDER.far_plane }, 2*sizeof(float));

    { // render all translucent block parts

        renderPass = SDL_BeginGPURenderPass(cmdbuf, colorTargetInfo, 2,&depthStencilTargetInfo);

        SDL_BindGPUFragmentSamplers(renderPass, 0, (SDL_GPUTextureSamplerBinding[]){
            { .texture = block_atlas.texture, .sampler = block_atlas.sampler },
            { .texture = SceneTransparencyTexture, .sampler = AuxiliarySampler },
            { .texture = AuxTransparencyTexture, .sampler = Auxiliary2Sampler }
        }, 3);


        SDL_BindGPUGraphicsPipeline(renderPass, FrontPipeline);
        chunk_renderdata.renderpass = renderPass;
        loaded_chunks_forall_increasing(leinad_chunk_render_front, &chunk_renderdata);

    }

    { // @todo render entities

    }

    SDL_EndGPURenderPass(renderPass);

  }

  { // draw outline

    SDL_GPUColorTargetInfo colorTargetInfo = {
        .texture = swapchain,
        .cycle = false,
        .load_op = SDL_GPU_LOADOP_LOAD,
        .store_op = SDL_GPU_STOREOP_STORE
    };

    renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
    SDL_BindGPUGraphicsPipeline(renderPass, AuxTransparencyPipeline);

    SDL_BindGPUFragmentSamplers(
        renderPass, 0,
        (SDL_GPUTextureSamplerBinding[]){
            { .texture = depth_texture, .sampler = EffectSampler },
            { .texture = FrontBGTexture, .sampler = AuxiliarySampler },
            { .texture = FrontTransparencyTexture, .sampler = Auxiliary2Sampler }
        }, 3
    );

    SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = EffectVertexBuffer, .offset = 0 }, 1);
    SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = EffectIndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);

    SDL_EndGPURenderPass(renderPass);
  }

    return SDL_APP_CONTINUE;
}





LEINAD_FCALL int leinad_render_update_textures(){
    
    // UPDATE THE ATLASES
    ENFORCE(_update_atlas(&block_atlas, "block", LEINAD_BLOCKTX_amount,LEINAD_TEXTURE_RADIUS,LEINAD_TEXTURE_RADIUS));
    
    return SDL_APP_CONTINUE;
} 


LEINAD_FCALL int leinad_render_init() {

    ENFORCE(leinad_render_update_textures());

    return SDL_APP_CONTINUE;
}

LEINAD_AUX static void free_chunks(leinad_chunk_t* chunk, __attribute__((unused)) void* _) {
    leinad_chunk_free(chunk);
}


LEINAD_FCALL void leinad_render_end() {
    if (block_atlas.sampler != NULL && device != NULL) SDL_ReleaseGPUSampler(device, block_atlas.sampler);
    if (block_atlas.texture != NULL && device != NULL) SDL_ReleaseGPUTexture(device, block_atlas.texture);

    loaded_chunks_forall_increasing(free_chunks, NULL);
}
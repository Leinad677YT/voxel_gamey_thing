#include <SDL3/SDL.h>

#include <leinad/data/globals.h>
#include <leinad/data/types.h>
#include <leinad/data/control_shortcuts.h>
#include "../render/shaders.h"

#include <leinad/render.h>
#include <leinad/world/block.h>
#include <leinad/world/blockdata.h>
#include <leinad/world/loading.h>

#include <leinad/world.h>
#include <leinad/app.h>

LEINAD_FCALL int INIT_render() {

  { // Initialize sub-renderers
    
    // world
    ENFORCE(leinad_render_init());
  }

  { // Creates the Shaders & Pipelines

    bool failed = false;

    SDL_GPUShader* skyVertexShader = NULL;
    SDL_GPUShader* skyFragmentShader = NULL;

    SDL_GPUShader* opaqueBlockVertexShader = NULL;
    SDL_GPUShader* opaqueBlockFragmentShader = NULL;

    SDL_GPUShader* transparentBlockVertexShader = NULL;
    SDL_GPUShader* transparentBlockFragmentShader = NULL;

    SDL_GPUShader* frontBlockVertexShader = NULL;
    SDL_GPUShader* frontBlockFragmentShader = NULL;

    SDL_GPUShader* outlineVertexShader = NULL;
    SDL_GPUShader* outlineFragmentShader = NULL;

  { // get shaders

        // sky_vert
        skyVertexShader = LoadShader(APP.device, "Sky.vert", 0, 0, 0, 0);
        if (skyVertexShader == NULL) {
            SDL_Log("Failed to create 'Sky' vertex shader!");
            failed = true;
            goto end_shaders;
        }

        // sky_frag
        skyFragmentShader = LoadShader(APP.device, "Sky.frag", 0, 0, 0, 0);
        if (skyFragmentShader == NULL) {
            SDL_Log("Failed to create 'Sky' fragment shader!");
            failed = true;
            goto end_shaders;
        }

        // texture_vert
        opaqueBlockVertexShader = LoadShader(APP.device, "OpaqueBlock.vert", 0, 2, 0, 0);
        if (opaqueBlockVertexShader == NULL) {
            SDL_Log("Failed to create 'OpaqueBlock' vertex shader!");
            failed = true;
            goto end_shaders;
        }

        // texture_frag
        opaqueBlockFragmentShader = LoadShader(APP.device, "OpaqueBlock.frag", 1, 1, 0, 0);
        if (opaqueBlockFragmentShader == NULL) {
            SDL_Log("Failed to create 'OpaqueBlock' fragment shader!");
            failed = true;
            goto end_shaders;
        }

        // transparent_texture_vert
        transparentBlockVertexShader = LoadShader(APP.device, "TransparentBlock.vert", 0, 2, 0, 0);
        if (transparentBlockVertexShader == NULL) {
            SDL_Log("Failed to create 'TransparentBlock' vertex shader!");
            failed = true;
            goto end_shaders;
        }

        // transparent_texture_frag
        transparentBlockFragmentShader = LoadShader(APP.device, "TransparentBlock.frag", 1, 1, 0, 0);
        if (transparentBlockFragmentShader == NULL) {
            SDL_Log("Failed to create 'TransparentBlock' fragment shader!");
            failed = true;
            goto end_shaders;
        }

        // front_texture_vert
        frontBlockVertexShader = LoadShader(APP.device, "FrontTransparentBlock.vert", 0, 2, 0, 0);
        if (frontBlockVertexShader == NULL) {
            SDL_Log("Failed to create 'FrontTransparentBlock' vertex shader!");
            failed = true;
            goto end_shaders;
        }

        // front_texture_frag
        frontBlockFragmentShader = LoadShader(APP.device, "FrontTransparentBlock.frag", 3, 1, 0, 0);
        if (frontBlockFragmentShader == NULL) {
            SDL_Log("Failed to create 'FrontTransparentBlock' fragment shader!");
            failed = true;
            goto end_shaders;
        }

        // outline_vert
        outlineVertexShader = LoadShader(APP.device, "DepthOutline.vert", 0, 1, 0, 0);
        if (outlineVertexShader == NULL) {
            SDL_Log("Failed to create 'DepthOutline' vertex shader!");
            failed = true;
            goto end_shaders;
        }

        // outline_frag
        outlineFragmentShader = LoadShader(APP.device, "DepthOutline.frag", 3, 0, 0, 0);
        if (outlineFragmentShader == NULL) {
            SDL_Log("Failed to create 'DepthOutline' fragment shader!");
            failed = true;
            goto end_shaders;
        }
  }

  { // sky pipeline
    SDL_Log("sky_pipeline_starts_load");
        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                { // swapchain
                    .format = SDL_GetGPUSwapchainTextureFormat(APP.device, APP.window)
                }
            },
            .has_depth_stencil_target = false
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(PositionTextureVertex)
            }},
            .num_vertex_attributes = 2,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .location = 0,
                .offset = 0
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .location = 1,
                .offset = sizeof(float) * 3
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = skyVertexShader,
        .fragment_shader = skyFragmentShader
    };

    SkyPipeline = SDL_CreateGPUGraphicsPipeline(APP.device, &pipelineCreateInfo);
    if (SkyPipeline == NULL) {
        SDL_Log("Failed to create Sky pipeline!");
        failed = true;
        goto end_pipelines;
    }
  }

  { // opaque-textured-3d pipeline
    SDL_Log("opaque_pipeline_starts_load");
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 2,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                { // swapchain
                    .format = SDL_GetGPUSwapchainTextureFormat(APP.device, APP.window)
                },
                { // outline color
                    .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM
                }
            },
            .has_depth_stencil_target = true,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM
        },
        .depth_stencil_state = (SDL_GPUDepthStencilState){
            .enable_depth_test = true,
            .enable_depth_write = true,
            .enable_stencil_test = false,
            .compare_op = SDL_GPU_COMPAREOP_LESS
        },
        .rasterizer_state = (SDL_GPURasterizerState){
            .cull_mode = SDL_GPU_CULLMODE_FRONT,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(PositionTextureColorVertex)
            }},
            .num_vertex_attributes = 3,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .location = 0,
                .offset = 0
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .location = 1,
                .offset = sizeof(float) * 3
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .location = 2,
                .offset = sizeof(float) * 5
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = opaqueBlockVertexShader,
        .fragment_shader = opaqueBlockFragmentShader
    };

    ScenePipeline = SDL_CreateGPUGraphicsPipeline(APP.device, &pipelineCreateInfo);
    if (ScenePipeline == NULL) {
        SDL_Log("Failed to create Scene pipeline!");
        failed = true;
        goto end_pipelines;
    }

  }

  { // transparent-textured-3d pipeline (translucency)
    SDL_Log("translucent_pipeline_starts_load");
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 2,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                { // averaged written value translucency
                    .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                    .blend_state = (struct SDL_GPUColorTargetBlendState){
                        .enable_blend = true,
                        .enable_color_write_mask = false,
                        
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        
                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE
                    }
                },
                { // amount of times written translucency
                    .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                    .blend_state = (struct SDL_GPUColorTargetBlendState){
                        .enable_blend = true,
                        .enable_color_write_mask = false,
                        
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        
                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE
                    }
                }
            },
            .has_depth_stencil_target = true,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM
        },
        .depth_stencil_state = (SDL_GPUDepthStencilState){
            .enable_depth_test = true,
            .enable_depth_write = false,
            .enable_stencil_test = false,
            .compare_op = SDL_GPU_COMPAREOP_LESS
        },
        .rasterizer_state = (SDL_GPURasterizerState){
            .cull_mode = SDL_GPU_CULLMODE_FRONT,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(PositionTextureColorVertex)
            }},
            .num_vertex_attributes = 3,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .location = 0,
                .offset = 0
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .location = 1,
                .offset = sizeof(float) * 3
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .location = 2,
                .offset = sizeof(float) * 5
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = transparentBlockVertexShader,
        .fragment_shader = transparentBlockFragmentShader
    };

    TransparencyPipeline = SDL_CreateGPUGraphicsPipeline(APP.device, &pipelineCreateInfo);
    if (TransparencyPipeline == NULL) {
        SDL_Log("Failed to create Transparency pipeline!");
        failed = true;
        goto end_pipelines;
    }

  }

  { // front transparency pipeline
    SDL_Log("front_translucency_pipeline_starts_load");
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 2,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                { // transparency texture
                    .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM
                },
                { // outline color
                    .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM
                }
            },
            .has_depth_stencil_target = true,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM
        },
        .depth_stencil_state = (SDL_GPUDepthStencilState){
            .enable_depth_test = true,
            .enable_depth_write = true,
            .enable_stencil_test = false,
            .compare_op = SDL_GPU_COMPAREOP_LESS
        },
        .rasterizer_state = (SDL_GPURasterizerState){
            .cull_mode = SDL_GPU_CULLMODE_FRONT,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(PositionTextureColorVertex)
            }},
            .num_vertex_attributes = 3,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .location = 0,
                .offset = 0
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .location = 1,
                .offset = sizeof(float) * 3
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .location = 2,
                .offset = sizeof(float) * 5
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = frontBlockVertexShader,
        .fragment_shader = frontBlockFragmentShader
    };

    FrontPipeline = SDL_CreateGPUGraphicsPipeline(APP.device, &pipelineCreateInfo);
    if (FrontPipeline == NULL) {
        SDL_Log("Failed to create Transparency pipeline!");
        failed = true;
        goto end_pipelines;
    }

  }

  { // outline pipeline
    SDL_Log("outline_pipeline_starts_load");
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                { // swapchain
                    .format = SDL_GetGPUSwapchainTextureFormat(APP.device, APP.window),
                    .blend_state = {
                        .enable_blend = true,
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,

                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
                        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                    }
                }
            },
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(PositionTextureVertex)
            }},
            .num_vertex_attributes = 2,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .location = 0,
                .offset = 0
            }, {
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .location = 1,
                .offset = sizeof(float) * 3
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = outlineVertexShader,
        .fragment_shader = outlineFragmentShader,
    };

    AuxTransparencyPipeline = SDL_CreateGPUGraphicsPipeline(APP.device, &pipelineCreateInfo);
    if (AuxTransparencyPipeline == NULL) {
        SDL_Log("Failed to create AuxTransparency pipeline!");
        failed = true;
        goto end_pipelines;
    }

  }
    
    end_pipelines:
    end_shaders:

    if (skyVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, skyVertexShader);
    if (skyVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, skyFragmentShader);
    
    if (opaqueBlockVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, opaqueBlockVertexShader);
    if (opaqueBlockFragmentShader != NULL) SDL_ReleaseGPUShader(APP.device, opaqueBlockFragmentShader);

    if (transparentBlockVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, transparentBlockVertexShader);
    if (transparentBlockFragmentShader != NULL) SDL_ReleaseGPUShader(APP.device, transparentBlockFragmentShader);
    
    if (frontBlockVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, frontBlockVertexShader);
    if (frontBlockFragmentShader != NULL) SDL_ReleaseGPUShader(APP.device, frontBlockFragmentShader);
    
    if (outlineVertexShader != NULL) SDL_ReleaseGPUShader(APP.device, outlineVertexShader);
    if (outlineFragmentShader != NULL) SDL_ReleaseGPUShader(APP.device, outlineFragmentShader);

    if (failed) return SDL_APP_FAILURE;

  }

  { // Create the Scene Textures

        SDL_GetWindowSizeInPixels(APP.window, &SceneWidth, &SceneHeight);

        SceneTransparencyTexture = SDL_CreateGPUTexture(
            APP.device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                // .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );

        AuxTransparencyTexture = SDL_CreateGPUTexture(
            APP.device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                // .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );
        
        FrontTransparencyTexture = SDL_CreateGPUTexture(
            APP.device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                // .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );

        FrontBGTexture = SDL_CreateGPUTexture(
            APP.device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );


        depth_texture = CreateDepthTexture(SceneWidth,SceneHeight);

    }

    // Create Outline Effect Sampler
    EffectSampler = SDL_CreateGPUSampler(APP.device, &(SDL_GPUSamplerCreateInfo){
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });

    AuxiliarySampler = SDL_CreateGPUSampler(APP.device, &(SDL_GPUSamplerCreateInfo){
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });

    Auxiliary2Sampler = SDL_CreateGPUSampler(APP.device, &(SDL_GPUSamplerCreateInfo){
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });

    
  { // Create & Upload Outline Effect Vertex and Index buffers
        EffectVertexBuffer = SDL_CreateGPUBuffer(
            APP.device,
            &(SDL_GPUBufferCreateInfo) {
                .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                .size = sizeof(PositionTextureVertex) * 4
            }
        );

        EffectIndexBuffer = SDL_CreateGPUBuffer(
            APP.device,
            &(SDL_GPUBufferCreateInfo) {
                .usage = SDL_GPU_BUFFERUSAGE_INDEX,
                .size = sizeof(Uint16) * 6
            }
        );

        SDL_GPUTransferBuffer* bufferTransferBuffer = SDL_CreateGPUTransferBuffer(
            APP.device,
            &(SDL_GPUTransferBufferCreateInfo) {
                .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                .size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6)
            }
        );

        PositionTextureVertex* transferData = SDL_MapGPUTransferBuffer(
            APP.device,
            bufferTransferBuffer,
            false
        );

        transferData[0] = (PositionTextureVertex) { -1,  1, 0, 0, 0 };
        transferData[1] = (PositionTextureVertex) {  1,  1, 0, 1, 0 };
        transferData[2] = (PositionTextureVertex) {  1, -1, 0, 1, 1 };
        transferData[3] = (PositionTextureVertex) { -1, -1, 0, 0, 1 };

        Uint16* indexData = (Uint16*) &transferData[4];
        indexData[0] = 0;
        indexData[1] = 1;
        indexData[2] = 2;
        indexData[3] = 0;
        indexData[4] = 2;
        indexData[5] = 3;

        SDL_UnmapGPUTransferBuffer(APP.device, bufferTransferBuffer);

        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(APP.device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        SDL_UploadToGPUBuffer(
            copyPass,
            &(SDL_GPUTransferBufferLocation) {
                .transfer_buffer = bufferTransferBuffer,
                .offset = 0
            },
            &(SDL_GPUBufferRegion) {
                .buffer = EffectVertexBuffer,
                .offset = 0,
                .size = sizeof(PositionTextureVertex) * 4
            },
            false
        );

        SDL_UploadToGPUBuffer(
            copyPass,
            &(SDL_GPUTransferBufferLocation) {
                .transfer_buffer = bufferTransferBuffer,
                .offset = sizeof(PositionTextureVertex) * 4
            },
            &(SDL_GPUBufferRegion) {
                .buffer = EffectIndexBuffer,
                .offset = 0,
                .size = sizeof(Uint16) * 6
            },
            false
        );

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
        SDL_ReleaseGPUTransferBuffer(APP.device, bufferTransferBuffer);
  }


  { // load test chunk
        loaded_chunks.center_pos[0] = 0; loaded_chunks.center_pos[1] = 0; loaded_chunks.center_pos[2] = 0;
        for (int y = 0; y < LOADED_CHUNKS_LENGTH;y++)
         for (int z = 0; z < LOADED_CHUNKS_LENGTH;z++)
          for (int x = 0; x < LOADED_CHUNKS_LENGTH;x++){
            leinad_chunk_load(&loaded_chunks.chunk[y*raise2(LOADED_CHUNKS_LENGTH) + z*LOADED_CHUNKS_LENGTH + x],(x-LOADED_CHUNKS_RADIUS) * 128,(y-LOADED_CHUNKS_RADIUS) * 128,(z-LOADED_CHUNKS_RADIUS) * 128,WORLD.dimensions[0]);
        }

        #define _chunk_test (loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH)/2])

        // _chunk_test->block[leinad_get_chunk_index(11, 0, 0)] = (struct blockdata) {
        //     .id = LEINAD_BLOCK_STONE,
        //     .rotation_n_subpos = 0,
        //     .custom_data = 0
        // };

        // _chunk_test->block[leinad_get_chunk_index(11, 1, 0)] = (struct blockdata) {
        //     .id = LEINAD_BLOCK_STONE,
        //     .rotation_n_subpos = 0,
        //     .custom_data = 0
        // };

        // _chunk_test->block[leinad_get_chunk_index(10, 0, 0)] = (struct blockdata) {
        //     .id = LEINAD_BLOCK_STONE,
        //     .rotation_n_subpos = 0,
        //     .custom_data = 0
        // };

        // _chunk_test->block[leinad_get_chunk_index(11, 0, 1)] = (struct blockdata) {
        //     .id = LEINAD_BLOCK_GLASS,
        //     .rotation_n_subpos = 0,
        //     .custom_data = 0
        // };

        // for (int i = 0; i<20;i++) for (int j = 0; j < 10; j++) for (int glass = LEINAD_BLOCK_WHITE_STAINED_GLASS; glass <= LEINAD_BLOCK_PURPLE_STAINED_GLASS; glass++) {
        //     _chunk_test->block[leinad_get_chunk_index(i, j, glass)] = (struct blockdata) {
        //         .id = glass,
        //         .rotation_n_subpos = 0,
        //         .custom_data = 0
        //     };
            
        // }

        // // for(int i = 0; i<32;i++) for(int j = 0; j < 32; j++) {
        // //     _chunk_test->block[leinad_get_chunk_index(i, 0, j)] = (struct blockdata) {
        // //         .id = LEINAD_BLOCK_STONE,
        // //         .rotation_n_subpos = 0,
        // //         .custom_data = 0
        // //     };
            
        // // }

        // for(int i = 0; i<LEINAD_REGION_RADIUS;i++) for(int j = 0; j < LEINAD_REGION_RADIUS; j++) {
        //     _chunk_test->block[leinad_get_chunk_index(i, LEINAD_REGION_RADIUS-1, j)] = (struct blockdata) {
        //         .id = LEINAD_BLOCK_GLASS,
        //         .rotation_n_subpos = 0,
        //         .custom_data = 0
        //     };
            
        // }

        for (int a = 0; a < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; a++)
         for (int b = 0; b < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; b++)
          for (int c = 0; c < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; c++)
            leinad_chunk_create_mesh(_chunk_test, a, b, c);


        #undef _chunk_test
    }
    SDL_Log("[OK] Render initialized");

    return SDL_APP_CONTINUE;
}
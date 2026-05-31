#include <SDL3/SDL.h>

#include "../data/globals.h"
#include "../data/types.h"
#include "../data/control_shortcuts.h"
#include "../render/shaders.h"

#include "../world/render.h"
#include "../world/block.h"
#include "../world/blockdata.h"

#include "../world/data.h"

LEINAD_FCALL int INIT_render() {

  { // Initialize sub-renderers
    
    // world
    ENFORCE(leinad_render_init());
  }

  { // Creates the Shaders & Pipelines

    bool failed = false;

    SDL_GPUShader* skyVertexShader = NULL;
    SDL_GPUShader* skyFragmentShader = NULL;

    SDL_GPUShader* textureVertexShader = NULL;
    SDL_GPUShader* textureFragmentShader = NULL;

    SDL_GPUShader* transparentTextureVertexShader = NULL;
    SDL_GPUShader* transparentTextureFragmentShader = NULL;

    SDL_GPUShader* auxTransparentVertexShader = NULL;
    SDL_GPUShader* auxTransparentFragmentShader = NULL;

    SDL_GPUShader* effectVertexShader = NULL;
    SDL_GPUShader* effectFragmentShader = NULL;

  { // get shaders

        // sky_vert
        skyVertexShader = LoadShader(device, "Sky.vert", 0, 0, 0, 0);
        if (skyVertexShader == NULL) {
            SDL_Log("Failed to create 'Sky' vertex shader!");
            failed = true; goto end_pipelines;
        }

        // sky_frag
        skyFragmentShader = LoadShader(device, "Sky.frag", 0, 0, 0, 0);
        if (skyFragmentShader == NULL) {
            SDL_Log("Failed to create 'Sky' fragment shader!");
            failed = true; goto end_pipelines;
        }
        
        // texture_vert
        textureVertexShader = LoadShader(device, "TexturedQuad.vert", 0, 1, 0, 0);
        if (textureVertexShader == NULL) {
            SDL_Log("Failed to create 'TexturedQuad' vertex shader!");
            failed = true; goto end_pipelines;
        }

        // texture_frag
        textureFragmentShader = LoadShader(device, "TexturedQuad.frag", 1, 1, 0, 0);
        if (textureFragmentShader == NULL) {
            SDL_Log("Failed to create 'TexturedQuad' fragment shader!");
            failed = true; goto end_pipelines;
        }

        // transparent_texture_vert
        transparentTextureVertexShader = LoadShader(device, "TransparentTexturedQuad.vert", 0, 1, 0, 0);
        if (transparentTextureVertexShader == NULL) {
            SDL_Log("Failed to create 'TransparentTexturedQuad' vertex shader!");
            failed = true; goto end_pipelines;
        }

        // transparent_texture_frag
        transparentTextureFragmentShader = LoadShader(device, "TransparentTexturedQuad.frag", 1, 1, 0, 0);
        if (transparentTextureFragmentShader == NULL) {
            SDL_Log("Failed to create 'TransparentTexturedQuad' fragment shader!");
            failed = true; goto end_pipelines;
        }

        // aux_transparent_texture_vert
        auxTransparentVertexShader = LoadShader(device, "MergeTransparency.vert", 0, 1, 0, 0);
        if (auxTransparentVertexShader == NULL) {
            SDL_Log("Failed to create 'MergeTransparency' vertex shader!");
            failed = true; goto end_pipelines;
        }

        // aux_transparent_texture_frag
        auxTransparentFragmentShader = LoadShader(device, "MergeTransparency.frag", 2, 0, 0, 0);
        if (auxTransparentFragmentShader == NULL) {
            SDL_Log("Failed to create 'MergeTransparency' fragment shader!");
            failed = true; goto end_pipelines;
        }

        // border_vert
        effectVertexShader = LoadShader(device, "DepthOutline.vert", 0, 1, 0, 0);
        if (effectVertexShader == NULL) {
            SDL_Log("Failed to create 'TexturedQuad' vertex shader!");
            failed = true; goto end_pipelines;
        }

        // border_frag
        effectFragmentShader = LoadShader(device, "DepthOutline.frag", 2, 0, 0, 0);
        if (effectFragmentShader == NULL) {
            SDL_Log("Failed to create 'DepthOutline' fragment shader!");
            failed = true; goto end_pipelines;
        }

  }

  { // sky pipeline
        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                .format = SDL_GetGPUSwapchainTextureFormat(device, window)
            }},
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

    SkyPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (SkyPipeline == NULL) {
        SDL_Log("Failed to create Sky pipeline!");
        failed = true; goto end_pipelines;
    }
  }

  { // opaque-textured-3d pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                .format = SDL_GetGPUSwapchainTextureFormat(device, window)
            }},
            .has_depth_stencil_target = true,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM
        },
        .depth_stencil_state = (SDL_GPUDepthStencilState){
            .enable_depth_test = true,
            .enable_depth_write = true,
            .enable_stencil_test = false,
            .compare_op = SDL_GPU_COMPAREOP_LESS,
            .write_mask = 0xFF
        },
        .rasterizer_state = (SDL_GPURasterizerState){
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
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
        .vertex_shader = textureVertexShader,
        .fragment_shader = textureFragmentShader
    };

    ScenePipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (ScenePipeline == NULL) {
        SDL_Log("Failed to create Scene pipeline!");
        failed = true; goto end_pipelines;
    }

  }

  { // transparent-textured-3d pipeline (translucency)
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
        .target_info = {
            .num_color_targets = 2,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                (SDL_GPUColorTargetDescription){
                    .format = SDL_GPU_TEXTUREFORMAT_R10G10B10A2_UNORM,
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
                (SDL_GPUColorTargetDescription){
                    .format = SDL_GPU_TEXTUREFORMAT_R10G10B10A2_UNORM,
                    .blend_state = (struct SDL_GPUColorTargetBlendState){
                        .enable_blend = true,
                        .enable_color_write_mask = false,
                        
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_CONSTANT_COLOR,
                        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_CONSTANT_COLOR,
                        
                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_CONSTANT_COLOR,
                        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_CONSTANT_COLOR
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
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
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
        .vertex_shader = transparentTextureVertexShader,
        .fragment_shader = transparentTextureFragmentShader
    };

    TransparencyPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (TransparencyPipeline == NULL) {
        SDL_Log("Failed to create Transparency pipeline!");
        failed = true; goto end_pipelines;
    }

  }

  { // aux pipeline that puts together transparency and opaques
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo) {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .blend_state = (SDL_GPUColorTargetBlendState) {
                    .enable_blend = true,
                    .color_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,

                    .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
                    .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                }
            }},
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
        .vertex_shader = auxTransparentVertexShader,
        .fragment_shader = auxTransparentFragmentShader,
    };

    AuxTransparencyPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (AuxTransparencyPipeline == NULL) {
        SDL_Log("Failed to create AuxTransparency pipeline!");
        failed = true; goto end_pipelines;
    }

  }

  { // post-effect pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .blend_state = (SDL_GPUColorTargetBlendState) {
                    .enable_blend = true,
                    .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                    .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .color_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                    .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                }
            }},
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
        .vertex_shader = effectVertexShader,
        .fragment_shader = effectFragmentShader,
    };

    EffectPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (EffectPipeline == NULL) {
        SDL_Log("Failed to create Outline Effect pipeline!");
        failed = true; goto end_pipelines;
    }

  }
    
end_pipelines:

    if (skyVertexShader != NULL) SDL_ReleaseGPUShader(device, skyVertexShader);
    if (skyVertexShader != NULL) SDL_ReleaseGPUShader(device, skyFragmentShader);
    
    if (textureVertexShader != NULL) SDL_ReleaseGPUShader(device, textureVertexShader);
    if (textureFragmentShader != NULL) SDL_ReleaseGPUShader(device, textureFragmentShader);

    if (transparentTextureVertexShader != NULL) SDL_ReleaseGPUShader(device, transparentTextureVertexShader);
    if (transparentTextureFragmentShader != NULL) SDL_ReleaseGPUShader(device, transparentTextureFragmentShader);
    
    if (auxTransparentVertexShader != NULL) SDL_ReleaseGPUShader(device, auxTransparentVertexShader);
    if (auxTransparentFragmentShader != NULL) SDL_ReleaseGPUShader(device, auxTransparentFragmentShader);
    
    if (effectVertexShader != NULL) SDL_ReleaseGPUShader(device, effectVertexShader);
    if (effectFragmentShader != NULL) SDL_ReleaseGPUShader(device, effectFragmentShader);

    if (failed) return -1;

  }

  { // Create the Scene Textures

        int w, h;
        SDL_GetWindowSizeInPixels(window, &w, &h);
        SceneWidth = w;
        SceneHeight = h;

        SceneColorTexture = SDL_CreateGPUTexture(
            device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );

        SceneTransparencyTexture = SDL_CreateGPUTexture(
            device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );

        AuxTransparencyTexture = SDL_CreateGPUTexture(
            device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET
            }
        );


        SceneDepthTexture = SDL_CreateGPUTexture(
            device,
            &(SDL_GPUTextureCreateInfo) {
                .type = SDL_GPU_TEXTURETYPE_2D,
                .width = SceneWidth,
                .height = SceneHeight,
                .layer_count_or_depth = 1,
                .num_levels = 1,
                .sample_count = SDL_GPU_SAMPLECOUNT_1,
                .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
                .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
            }
        );
  }

    // Create Outline Effect Sampler
    EffectSampler = SDL_CreateGPUSampler(device, &(SDL_GPUSamplerCreateInfo){
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });

    AuxiliarySampler = SDL_CreateGPUSampler(device, &(SDL_GPUSamplerCreateInfo){
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });

    
  { // Create & Upload Outline Effect Vertex and Index buffers
        EffectVertexBuffer = SDL_CreateGPUBuffer(
            device,
            &(SDL_GPUBufferCreateInfo) {
                .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                .size = sizeof(PositionTextureVertex) * 4
            }
        );

        EffectIndexBuffer = SDL_CreateGPUBuffer(
            device,
            &(SDL_GPUBufferCreateInfo) {
                .usage = SDL_GPU_BUFFERUSAGE_INDEX,
                .size = sizeof(Uint16) * 6
            }
        );

        SDL_GPUTransferBuffer* bufferTransferBuffer = SDL_CreateGPUTransferBuffer(
            device,
            &(SDL_GPUTransferBufferCreateInfo) {
                .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                .size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6)
            }
        );

        PositionTextureVertex* transferData = SDL_MapGPUTransferBuffer(
            device,
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

        SDL_UnmapGPUTransferBuffer(device, bufferTransferBuffer);

        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
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
        SDL_ReleaseGPUTransferBuffer(device, bufferTransferBuffer);
  }


  { // load test chunk
        leinad_region_t* region_test = leinad_region_create_empty();
        chunk_test = leinad_chunk_create();
        
        leinad_chunk_setfromregion(region_test,chunk_test);

        chunk_test->block[leinad_get_chunk_index(11, 0, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(11, 1, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(10, 0, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(11, 0, 1)] = (struct blockdata) {
            .id = LEINAD_BLOCK_GLASS,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        for(int i = 0; i<20;i++) for(int j = 0; j < 10; j++) for (int glass = LEINAD_BLOCK_WHITE_STAINED_GLASS; glass <= LEINAD_BLOCK_PURPLE_STAINED_GLASS; glass++) {
            chunk_test->block[leinad_get_chunk_index(i, j, glass)] = (struct blockdata) {
                .id = glass,
                .rotation_n_subpos = 0,
                .custom_data = 0
            };
            
        }

        for(int i = 0; i<32;i++) for(int j = 0; j < 32; j++) {
            chunk_test->block[leinad_get_chunk_index(i, 0, j)] = (struct blockdata) {
                .id = LEINAD_BLOCK_STONE,
                .rotation_n_subpos = 0,
                .custom_data = 0
            };
            
        }


        leinad_chunk_create_mesh(chunk_test, 0, 0, 0);

        loaded_chunks.chunk[LOADED_CHUNKS_RADIUS] = chunk_test;

    }

    return SDL_APP_CONTINUE;
}
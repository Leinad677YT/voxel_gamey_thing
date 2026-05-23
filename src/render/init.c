#include <SDL3/SDL.h>

#include "../data/globals.h"
#include "../data/types.h"
#include "../data/control_shortcuts.h"
#include "../render/shaders.h"

#include "../world/render.h"
#include "../world/block.h"
#include "../world/blockdata.h"



LEINAD_FCALL int INIT_render() {

  { // Initialize sub-renderers
    
    // world
    ENFORCE(leinad_render_init());
  }

  { // Creates the Shaders & Pipelines
    SDL_GPUShader* skyVertexShader;
    SDL_GPUShader* skyFragmentShader;
    SDL_GPUShader* textureVertexShader;
    SDL_GPUShader* textureFragmentShader;
    SDL_GPUShader* effectVertexShader;
    SDL_GPUShader* effectFragmentShader;

    { // get shaders

        // sky_vert
        skyVertexShader = LoadShader(device, "Sky.vert", 0, 0, 0, 0);
        if (skyVertexShader == NULL)
        {
            SDL_Log("Failed to create 'Sky' vertex shader!");
            return -1;
        }

        // sky_frag
        skyFragmentShader = LoadShader(device, "Sky.frag", 0, 0, 0, 0);
        if (skyFragmentShader == NULL)
        {
            SDL_Log("Failed to create 'Sky' fragment shader!");
            return -1;
        }
        // texture_vert
        textureVertexShader = LoadShader(device, "TexturedQuad.vert", 0, 1, 0, 0);
        if (textureVertexShader == NULL)
        {
            SDL_Log("Failed to create 'TexturedQuad' vertex shader!");
            return -1;
        }

        // texture_frag
        textureFragmentShader = LoadShader(device, "TexturedQuad.frag", 1, 1, 0, 0);
        if (textureFragmentShader == NULL)
        {
            SDL_Log("Failed to create 'TexturedQuad' fragment shader!");
            return -1;
        }

        // border_vert
        effectVertexShader = LoadShader(device, "DepthOutline.vert", 0, 1, 0, 0);
        if (effectVertexShader == NULL)
        {
            SDL_Log("Failed to create 'TexturedQuad' vertex shader!");
            return -1;
        }

        // border_frag
        effectFragmentShader = LoadShader(device, "DepthOutline.frag", 2, 1, 0, 0);
        if (effectFragmentShader == NULL)
        {
            SDL_Log("Failed to create 'DepthOutline' fragment shader!");
            return -1;
        }

    }

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

    pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
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
    if (ScenePipeline == NULL)
    {
        SDL_Log("Failed to create Scene pipeline!");
        return -1;
    }

    pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
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
        .fragment_shader = effectFragmentShader
    };

    EffectPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (EffectPipeline == NULL)
    {
        SDL_Log("Failed to create Outline Effect pipeline!");
        return -1;
    }

    SDL_ReleaseGPUShader(device, effectVertexShader);
    SDL_ReleaseGPUShader(device, effectFragmentShader);

    SDL_ReleaseGPUShader(device, textureVertexShader);
    SDL_ReleaseGPUShader(device, textureFragmentShader);

    SDL_ReleaseGPUShader(device, skyVertexShader);
    SDL_ReleaseGPUShader(device, skyFragmentShader);
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

        chunk_test->block[leinad_get_chunk_index(0, 0, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(0, 1, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(10, 0, 0)] = (struct blockdata) {
            .id = LEINAD_BLOCK_STONE,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        chunk_test->block[leinad_get_chunk_index(0, 0, 1)] = (struct blockdata) {
            .id = LEINAD_BLOCK_GLASS,
            .rotation_n_subpos = 0,
            .custom_data = 0
        };

        leinad_chunk_create_mesh(chunk_test, 0, 0, 0);


    }

    return SDL_APP_CONTINUE;
}
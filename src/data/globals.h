#pragma once

#include <SDL3/SDL.h>

// #include "types.h"

#include "../libs/stack.h"
#include "../ui/data.h"

#include "../world/block.h"

extern SDL_Window* window;              // Window
extern SDL_Renderer* renderer;          // Renderer

extern SDL_GPUDevice *device;               // GPU to use
extern SDL_GPUGraphicsPipeline *pipeline;   // Shaders pipeline
extern SDL_GPUSampler *sampler;             // Pixel sampler
extern SDL_GPUTexture *gpu_texture;         // Texture to render
extern SDL_GPUBuffer *vertex_buffer;        // [Buffer] Vertex
extern SDL_GPUTexture *depth_texture;       // Depth texture

extern Uint64 current_ns;
extern Uint64 previous_ns;

leinad_stack_t ui_render_stack;         // UI layers to render
leinad_ui_instance_t* ui_active;        // active UI

extern leinad_welem_t welem_data[];     // array with the welem data


extern SDL_GPUGraphicsPipeline* ScenePipeline;
extern SDL_GPUBuffer* SceneVertexBuffer;
extern SDL_GPUBuffer* SceneIndexBuffer;
extern SDL_GPUTexture* SceneColorTexture;
extern SDL_GPUTexture* SceneDepthTexture;

extern SDL_GPUGraphicsPipeline* EffectPipeline;
extern SDL_GPUBuffer* EffectVertexBuffer;
extern SDL_GPUBuffer* EffectIndexBuffer;
extern SDL_GPUSampler* EffectSampler;

extern int SceneWidth, SceneHeight;
extern double pos_x,pos_y,pos_z;
double toadd_x, toadd_y, toadd_z;

extern leinad_chunk_t* chunk_test;
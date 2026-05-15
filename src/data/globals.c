#include <SDL3/SDL.h>

// #include "types.h"

#include "../libs/stack.h"
#include "../ui/data.h"

#include "../world/block.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Uint64 current_ns = 0;
Uint64 previous_ns = 0;

leinad_stack_t ui_render_stack  = NULL;
leinad_ui_instance_t* ui_active = NULL;

SDL_GPUDevice *device = NULL;
SDL_GPUGraphicsPipeline *pipeline = NULL;
SDL_GPUSampler *sampler = NULL;
SDL_GPUTexture *gpu_texture = NULL;
SDL_GPUBuffer *vertex_buffer = NULL;
SDL_GPUTexture *depth_texture = NULL;



SDL_GPUGraphicsPipeline* ScenePipeline;
SDL_GPUBuffer* SceneVertexBuffer;
SDL_GPUBuffer* SceneIndexBuffer;
SDL_GPUTexture* SceneColorTexture;
SDL_GPUTexture* SceneDepthTexture;

SDL_GPUGraphicsPipeline* EffectPipeline;
SDL_GPUBuffer* EffectVertexBuffer;
SDL_GPUBuffer* EffectIndexBuffer;
SDL_GPUSampler* EffectSampler;

int SceneWidth, SceneHeight;
double pos_x = 0,pos_y = 0,pos_z = 0;
double toadd_x = 0, toadd_y = 0, toadd_z = 0;


leinad_chunk_t* chunk_test = NULL;
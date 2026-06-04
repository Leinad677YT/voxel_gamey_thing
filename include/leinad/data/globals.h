#pragma once

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

// #include "types.h"

#include "../../../src/world/region.h"

extern SDL_Window* window;              // Window
extern SDL_Renderer* renderer;          // Renderer

extern SDL_GPUDevice *device;               // GPU to use
extern SDL_GPUGraphicsPipeline *pipeline;   // Shaders pipeline
extern SDL_GPUTexture *depth_texture;       // Depth texture

extern Uint64 current_ns;
extern Uint64 previous_ns;

extern SDL_GPUGraphicsPipeline* SkyPipeline;
extern SDL_GPUGraphicsPipeline* FrontPipeline;

extern SDL_GPUGraphicsPipeline* ScenePipeline;
extern SDL_GPUGraphicsPipeline* TransparencyPipeline;
extern SDL_GPUGraphicsPipeline* AuxTransparencyPipeline;
extern SDL_GPUTexture* SceneColorTexture;
extern SDL_GPUTexture* SceneDepthTexture;
extern SDL_GPUTexture* SceneTransparencyTexture;
extern SDL_GPUTexture* AuxTransparencyTexture;
extern SDL_GPUTexture* FrontTransparencyTexture;
extern SDL_GPUTexture* FrontBGTexture;
extern SDL_GPUGraphicsPipeline* EffectPipeline;
extern SDL_GPUBuffer* EffectVertexBuffer;
extern SDL_GPUBuffer* EffectIndexBuffer;
extern SDL_GPUSampler* EffectSampler;
extern SDL_GPUSampler* AuxiliarySampler;
extern SDL_GPUSampler* Auxiliary2Sampler;
extern int SceneWidth, SceneHeight;
extern double pos_x,pos_y,pos_z;
double toadd_x, toadd_y, toadd_z;

extern leinad_chunk_t* chunk_test;


extern NET_Server* server;
extern NET_Address* server_addr;

extern NET_StreamSocket* client_sock;
extern NET_Address* client_addr;

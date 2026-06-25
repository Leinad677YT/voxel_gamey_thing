#pragma once

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

// #include "types.h"

#include "../world/region.h"

static SDL_Window* window = NULL;       // Window
static SDL_Renderer* renderer = NULL;   // Renderer

static SDL_GPUDevice *device = NULL;               // GPU to use
static SDL_GPUGraphicsPipeline *pipeline = NULL;   // Shaders pipeline
static SDL_GPUTexture *depth_texture = NULL;       // Depth texture

static Uint64 current_ns = 0;
static Uint64 previous_ns = 0;

static SDL_GPUGraphicsPipeline* SkyPipeline = NULL;
static SDL_GPUGraphicsPipeline* FrontPipeline = NULL;

static SDL_GPUGraphicsPipeline* ScenePipeline = NULL;
static SDL_GPUGraphicsPipeline* TransparencyPipeline = NULL;
static SDL_GPUGraphicsPipeline* AuxTransparencyPipeline = NULL;
static SDL_GPUTexture* SceneColorTexture = NULL;
static SDL_GPUTexture* SceneDepthTexture = NULL;
static SDL_GPUTexture* SceneTransparencyTexture = NULL;
static SDL_GPUTexture* AuxTransparencyTexture = NULL;
static SDL_GPUTexture* FrontTransparencyTexture = NULL;
static SDL_GPUTexture* FrontBGTexture = NULL;
static SDL_GPUGraphicsPipeline* EffectPipeline = NULL;
static SDL_GPUBuffer* EffectVertexBuffer = NULL;
static SDL_GPUBuffer* EffectIndexBuffer = NULL;
static SDL_GPUSampler* EffectSampler = NULL;
static SDL_GPUSampler* AuxiliarySampler = NULL;
static SDL_GPUSampler* Auxiliary2Sampler = NULL;
static int SceneWidth = 0, SceneHeight = 0;
static double pos_x = 0, pos_y = 0, pos_z = 0;
static double toadd_x = 0, toadd_y = 0, toadd_z = 0;

static leinad_chunk_t* chunk_test = NULL;


static NET_Server* server = NULL;
static NET_Address* server_addr = NULL;

static NET_StreamSocket* client_sock = NULL;
static NET_Address* client_addr = NULL;

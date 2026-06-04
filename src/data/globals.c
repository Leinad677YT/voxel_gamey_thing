
#include <leinad/data/globals.h>

#include "../world/region.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Uint64 current_ns = 0;
Uint64 previous_ns = 0;

SDL_GPUDevice *device = NULL;
SDL_GPUTexture *depth_texture = NULL;

SDL_GPUGraphicsPipeline* SkyPipeline = NULL;
SDL_GPUGraphicsPipeline* FrontPipeline = NULL;

SDL_GPUGraphicsPipeline* ScenePipeline = NULL;
SDL_GPUGraphicsPipeline* TransparencyPipeline = NULL;
SDL_GPUGraphicsPipeline* AuxTransparencyPipeline = NULL;
SDL_GPUTexture* SceneColorTexture = NULL;
SDL_GPUTexture* SceneTransparencyTexture = NULL;
SDL_GPUTexture* AuxTransparencyTexture = NULL;
SDL_GPUTexture* FrontTransparencyTexture = NULL;
SDL_GPUTexture* FrontBGTexture = NULL;
SDL_GPUTexture* SceneDepthTexture = NULL;

SDL_GPUGraphicsPipeline* EffectPipeline = NULL;
SDL_GPUBuffer* EffectVertexBuffer = NULL;
SDL_GPUBuffer* EffectIndexBuffer = NULL;
SDL_GPUSampler* EffectSampler = NULL;
SDL_GPUSampler* AuxiliarySampler = NULL;
SDL_GPUSampler* Auxiliary2Sampler = NULL;

int SceneWidth, SceneHeight;
double pos_x = 0,pos_y = 0,pos_z = 0;
double toadd_x = 0, toadd_y = 0, toadd_z = 0;


leinad_chunk_t* chunk_test = NULL;

NET_Server* server = NULL;
NET_Address* server_addr = NULL;

NET_StreamSocket* client_sock = NULL;
NET_Address* client_addr = NULL;

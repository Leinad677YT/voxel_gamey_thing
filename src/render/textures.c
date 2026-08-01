#include "textures.h"

#include <leinad/data/app.h>
#include <leinad/render.h>
#include <leinad/data/globals.h>

#include "../libs/io.h"
#include "../libs/bit_manipulation.h"

SDL_Surface* leinad_load_png(const char* texture_name) {
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_asprintf(&png_path, "%sresources/textures/%s.png", SDL_GetBasePath(), texture_name);

    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return NULL;
    }

    SDL_free(png_path);

    return surface;
}

LEINAD_AUX int _update_atlas(struct render_atlas *atlas, char* root_path, Uint32 tx_amount, int tx_w, int tx_h) {
    Uint32 i;
    char path[LEINAD_MAX_PATH_LENGTH];
    char current_tx[LEINAD_MAX_LINE_LENGTH];
    struct SDL_IOStream *atlas_io;
    struct SDL_Surface *tx_surface, *atlas_surface;
    struct SDL_Rect target = {0,0,tx_w,tx_h};

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
        ) {
            current_tx[0] = 'n'; current_tx[1] = 'u'; current_tx[2] = 'l'; current_tx[3] = 'l'; current_tx[4] = '\0';
            tx_surface = NULL;
            tx_surface = leinad_load_png(current_tx);            
        } else {
            tx_surface = NULL;
            SDL_snprintf(path,LEINAD_MAX_PATH_LENGTH, "%s/%s", root_path, current_tx);
            tx_surface = leinad_load_png(path);

            if (tx_surface == NULL || current_tx[0] == '\0') {
                current_tx[0] = 'n'; current_tx[1] = 'u'; current_tx[2] = 'l'; current_tx[3] = 'l'; current_tx[4] = '\0';
                tx_surface = leinad_load_png(current_tx);            
            }
        }

        target.x = (i % (atlas->width)) * tx_w;
        target.y = (i / (atlas->width)) * tx_h;

        // blit texture on the atlas
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

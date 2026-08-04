#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <time.h>
#include <stdio.h>

#include "../src/world/block.c"
#include "../src/world/region.c"
#include "../src/world/entity.c"
#include "../src/world/loading.c"
#include "../src/render/render_main.c"
#include "../src/math/matrix.c"
#include "../src/render/textures.c"

SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    APP.device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true,
        NULL
    );

    if (!APP.device) {
        SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    leinad_render_init();

    block_atlas.height = 120;
    block_atlas.width = 120;

    // full region
    leinad_region_t region_full = {
        .amounts_perLODlevel = 0 & mask_amount_64x,
        .ctrl_data = is_full_region,
        .redirection_flags = {0}
    };

    ((Uint16*)(region_full.region_data))[0*8+0] = (Uint16) LEINAD_BLOCK_AIR;
    ((Uint16*)(region_full.region_data))[0*8+1] = (Uint16) 0;
    ((Uint16*)(region_full.region_data))[0*8+2] = (Uint16) 0;
    ((Uint16*)(region_full.region_data))[0*8+3] = (Uint16) 0;


    leinad_chunk_t *chunk = NULL;
    chunk = leinad_chunk_create(0,0,0);
    leinad_chunk_setfromregion(&region_full, chunk);
    if (chunk == NULL) {printf("chunk not properly initialized"); return SDL_APP_FAILURE;}


chunk->block[leinad_get_chunk_index(0, 0, 0)].id = LEINAD_BLOCK_STONE;
chunk->block[leinad_get_chunk_index(0, 0, 0)].rotation_n_subpos = 0;
chunk->block[leinad_get_chunk_index(0, 0, 0)].custom_data = 0;

chunk->block[8*128*128 + 0*128 + 0].id = LEINAD_BLOCK_STONE;
chunk->block[8*128*128 + 0*128 + 0].rotation_n_subpos = 0;
chunk->block[8*128*128 + 0*128 + 0].custom_data = 0;

chunk->block[8*128*128 + 0*128 + 1].id = LEINAD_BLOCK_STONE;
chunk->block[8*128*128 + 0*128 + 1].rotation_n_subpos = 0;
chunk->block[8*128*128 + 0*128 + 1].custom_data = 0;

chunk->block[8*128*128 + 1*128 + 0].id = LEINAD_BLOCK_STONE;
chunk->block[8*128*128 + 1*128 + 0].rotation_n_subpos = 0;
chunk->block[8*128*128 + 1*128 + 0].custom_data = 0;

chunk->block[8*128*128 + 1*128 + 1].id = LEINAD_BLOCK_STONE;
chunk->block[8*128*128 + 1*128 + 1].rotation_n_subpos = 0;
chunk->block[8*128*128 + 1*128 + 1].custom_data = 0;

chunk->block[9*128*128 + 0*128 + 0].id = LEINAD_BLOCK_STONE;
chunk->block[9*128*128 + 0*128 + 0].rotation_n_subpos = 0;
chunk->block[9*128*128 + 0*128 + 0].custom_data = 0;

chunk->block[9*128*128 + 0*128 + 1].id = LEINAD_BLOCK_STONE;
chunk->block[9*128*128 + 0*128 + 1].rotation_n_subpos = 0;
chunk->block[9*128*128 + 0*128 + 1].custom_data = 0;

chunk->block[9*128*128 + 1*128 + 0].id = LEINAD_BLOCK_STONE;
chunk->block[9*128*128 + 1*128 + 0].rotation_n_subpos = 0;
chunk->block[9*128*128 + 1*128 + 0].custom_data = 0;

chunk->block[9*128*128 + 1*128 + 1].id = LEINAD_BLOCK_STONE;
chunk->block[9*128*128 + 1*128 + 1].rotation_n_subpos = 0;
chunk->block[9*128*128 + 1*128 + 1].custom_data = 0;


    leinad_region_t* region3 = NULL;
    region3 = leinad_region_create_from_chunk(chunk);

    if (region3 == NULL) {printf("region3 not properly initialized"); return SDL_APP_FAILURE;}

    SDL_free(region3);

    // end of manual checks

    // start of auto test
    SDL_Log("- - AUTOTEST START - -");
    
    SDL_srand(time(NULL));
    for (Uint32 i = 0; i < 400000; i++) {
        Uint32 j = SDL_rand(128*128*128);
        leinad_blockdata_clone(
            (struct blockdata){.id= SDL_rand(LEINAD_BLOCK_amount),0,0},
            &chunk->block[j]
        );
    }

    Uint64 test_time = SDL_GetTicksNS();

    region3 = leinad_region_create_from_chunk(chunk);

    test_time = SDL_GetTicksNS() - test_time;
    

    SDL_Log("- - REGION CREATED - -");
    SDL_Log("time taken: %lu ns",test_time);
    Uint64 failcount = 0;

    test_time = SDL_GetTicksNS();

    for (int i =0; i<LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS;i++)
        leinad_chunk_create_mesh(chunk,i,0,0);

    // for(Uint32 y = 0; y < LEINAD_REGION_RADIUS;y++) for(Uint32 z = 0; z < LEINAD_REGION_RADIUS;z++) for(Uint32 x = 0; x < LEINAD_REGION_RADIUS;x++){
    //     struct blockdata potato = leinad_region_getblock(x,y,z,region3);
    //     if (
    //         leinad_blockdata_comparator(
    //             &chunk->block[leinad_get_chunk_index(x,y,z)],
    //             &potato
    //         )
    //     ) {
    //         failcount++;
    //         printf(
    //             """"""
    //             "block <%d,%d,%d> failed by comparing:\n"
    //             "CHUNK: %u %u %u\n"
    //             "REGION: %u %u %u\n"
    //             """""",
    //             x,y,z,
    //             chunk->block[leinad_get_chunk_index(x,y,z)].id,
    //              chunk->block[leinad_get_chunk_index(x,y,z)].rotation_n_subpos,
    //              chunk->block[leinad_get_chunk_index(x,y,z)].custom_data,
    //             potato.id, potato.rotation_n_subpos, potato.custom_data
    //         );
    //         leinad_region_getblock(x,y,z,region3);
    //     }
    // }
    // SDL_Log("%lu fails out of %u ",failcount,128*128*128);

    test_time = SDL_GetTicksNS() - test_time;
    SDL_Log("time taken: %lu ns",test_time);

    SDL_free(chunk);


    return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent(
    __attribute__ ((unused)) void *appstate,
    SDL_Event *event
) {
    return 1;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return 1;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    return;
}
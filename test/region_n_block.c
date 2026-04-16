
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <time.h>

#include "../src/world/block.c"

SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {

    // full region
    leinad_region_t region_full = {
        .amounts_perLODlevel = 0 & mask_amount_64x,
        .ctrl_data = is_full_region,
        .redirection_flags = {0}
    };

    ((Uint16*)(region_full.region_data))[0*8+0] = (Uint16) LEINAD_BLOCK_NULL;
    ((Uint16*)(region_full.region_data))[0*8+1] = (Uint16) 0;
    ((Uint16*)(region_full.region_data))[0*8+2] = (Uint16) 0;
    ((Uint16*)(region_full.region_data))[0*8+3] = (Uint16) 0;


    leinad_chunk_t *chunk = NULL;
    chunk = leinad_chunk_create();
    leinad_chunk_setfromregion(&region_full, chunk);
    if (chunk == NULL) {printf("chunk not properly initialized"); return SDL_APP_FAILURE;}


    chunk->block[0*128*128 + 01*128 + 01].id = 0x55;
    chunk->block[0*128*128 + 01*128 + 01].rotation_n_subpos = 0;
    chunk->block[0*128*128 + 01*128 + 01].custom_data = 0;

chunk->block[8*128*128 + 0*128 + 0].id = 0x10;
chunk->block[8*128*128 + 0*128 + 0].rotation_n_subpos = 0;
chunk->block[8*128*128 + 0*128 + 0].custom_data = 0;

chunk->block[8*128*128 + 0*128 + 1].id = 0x11;
chunk->block[8*128*128 + 0*128 + 1].rotation_n_subpos = 0;
chunk->block[8*128*128 + 0*128 + 1].custom_data = 0;

chunk->block[8*128*128 + 1*128 + 0].id = 0x12;
chunk->block[8*128*128 + 1*128 + 0].rotation_n_subpos = 0;
chunk->block[8*128*128 + 1*128 + 0].custom_data = 0;

chunk->block[8*128*128 + 1*128 + 1].id = 0x13;
chunk->block[8*128*128 + 1*128 + 1].rotation_n_subpos = 0;
chunk->block[8*128*128 + 1*128 + 1].custom_data = 0;

        chunk->block[19*128*128 + 0*128 + 0].id = 0x14;
        chunk->block[19*128*128 + 0*128 + 0].rotation_n_subpos = 0;
        chunk->block[19*128*128 + 0*128 + 0].custom_data = 0;

    chunk->block[9*128*128 + 40*128 + 01].id = 0x15;
    chunk->block[9*128*128 + 40*128 + 01].rotation_n_subpos = 0;
    chunk->block[9*128*128 + 40*128 + 01].custom_data = 0;

chunk->block[9*128*128 + 1*128 + 1].id = 0x17;
chunk->block[9*128*128 + 1*128 + 1].rotation_n_subpos = 0;
chunk->block[9*128*128 + 1*128 + 1].custom_data = 0;


chunk->block[10*128*128 + 2*128 + 70].id = 0x11;
chunk->block[10*128*128 + 2*128 + 70].rotation_n_subpos = 0;
chunk->block[10*128*128 + 2*128 + 70].custom_data = 0;

chunk->block[10*128*128 + 2*128 + 71].id = 0x12;
chunk->block[10*128*128 + 2*128 + 71].rotation_n_subpos = 0;
chunk->block[10*128*128 + 2*128 + 71].custom_data = 0;

chunk->block[10*128*128 + 3*128 + 70].id = 0x13;
chunk->block[10*128*128 + 3*128 + 70].rotation_n_subpos = 0;
chunk->block[10*128*128 + 3*128 + 70].custom_data = 0;

chunk->block[10*128*128 + 3*128 + 71].id = 0x14;
chunk->block[10*128*128 + 3*128 + 71].rotation_n_subpos = 0;
chunk->block[10*128*128 + 3*128 + 71].custom_data = 0;

chunk->block[11*128*128 + 2*128 + 70].id = 0x15;
chunk->block[11*128*128 + 2*128 + 70].rotation_n_subpos = 0;
chunk->block[11*128*128 + 2*128 + 70].custom_data = 0;

chunk->block[11*128*128 + 2*128 + 71].id = 0x16;
chunk->block[11*128*128 + 2*128 + 71].rotation_n_subpos = 0;
chunk->block[11*128*128 + 2*128 + 71].custom_data = 0;

chunk->block[11*128*128 + 3*128 + 70].id = 0x17;
chunk->block[11*128*128 + 3*128 + 70].rotation_n_subpos = 0;
chunk->block[11*128*128 + 3*128 + 70].custom_data = 0;

chunk->block[11*128*128 + 3*128 + 71].id = 0x18;
chunk->block[11*128*128 + 3*128 + 71].rotation_n_subpos = 0;
chunk->block[11*128*128 + 3*128 + 71].custom_data = 0;

    // chunk->block[128*128*  4 + 128*  3 +  2].id = 3;
    // chunk->block[128*128* 50 + 128*  2 + 28].id = 4;
    // chunk->block[128*128*  9 + 128* 33 +121].id = 5;
    // chunk->block[128*128* 73 + 128* 99 +  4].id = 6;

    leinad_region_t* region3 = NULL;
    region3 = leinad_region_create_from_chunk(chunk);

    if (region3 == NULL) {printf("region3 not properly initialized"); return SDL_APP_FAILURE;}

    SDL_free(region3);

    // end of manual checks

    // start of auto test
    SDL_Log("- - AUTOTEST START - -");
    
    SDL_srand(time(NULL));
    for (Uint32 i = 0; i < 4000; i++) {
        Uint32 j = SDL_rand(128*128*128);
        leinad_blockdata_clone(
            (struct blockdata){.id= SDL_rand(0xFE),0,0},
            &chunk->block[j]
        );
    }

    Uint64 test_time = SDL_GetTicksNS();

    region3 = leinad_region_create_from_chunk(chunk);

    test_time = SDL_GetTicksNS() - test_time;
    

    SDL_Log("- - REGION CREATED - -");
    SDL_Log("time taken: %lu ns",test_time);
    Uint64 failcount = 0;

    for(Uint32 y = 0; y < LEINAD_REGION_RADIUS;y++) for(Uint32 z = 0; z < LEINAD_REGION_RADIUS;z++) for(Uint32 x = 0; x < LEINAD_REGION_RADIUS;x++){
        struct blockdata potato = leinad_region_getblock(x,y,z,region3);
        if (
            leinad_blockdata_comparator(
                &chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x],
                &potato
            )
        ) {
            failcount++;
            printf(
                """"""
                "block <%d,%d,%d> failed by comparing:\n"
                "CHUNK: %u %u %u\n"
                "REGION: %u %u %u\n"
                """""",
                x,y,z,
                chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].id,
                 chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].rotation_n_subpos,
                 chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].custom_data,
                potato.id, potato.rotation_n_subpos, potato.custom_data
            );
            leinad_region_getblock(x,y,z,region3);
        }
    }
    SDL_Log("%lu fails out of %u ",failcount,128*128*128);

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
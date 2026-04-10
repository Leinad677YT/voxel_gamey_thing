
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
    ((Uint16*)(region_full.region_data))[0*8+1] = (Uint16) 0b0110000011110000;
    ((Uint16*)(region_full.region_data))[0*8+2] = (Uint16) 0b0100000100000110;
    ((Uint16*)(region_full.region_data))[0*8+3] = (Uint16) 0b0011000000001000;


    struct blockdata aux = leinad_region_getblock(0,1,8,&region_full);

    SDL_Log(
        ""
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux.id, aux.rotation_n_subpos, aux.rotation_n_subpos
    );

    SDL_Log("block should be: %d\n",LEINAD_BLOCK_NULL);


    leinad_chunk_t *chunk = NULL;

    chunk = leinad_chunk_create();
    leinad_chunk_setfromregion(&region_full, chunk);

    leinad_region_t* region2 = NULL;
    region2 = leinad_region_create_from_chunk(chunk); 

    if (region2 == NULL) {printf("region2 not properly initialized"); return SDL_APP_FAILURE;}

    struct blockdata aux2 = leinad_region_getblock(0,1,8,region2);

    SDL_Log(
        ""
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux2.id, aux2.rotation_n_subpos, aux2.rotation_n_subpos
    );

    SDL_Log("block should be: %d\n",LEINAD_BLOCK_NULL);

    SDL_free(region2);

    chunk->block[0].id = 0x55;
    chunk->block[0].rotation_n_subpos = 0x1234;
    chunk->block[0].custom_data = 0x12345678;
    // chunk->block[128*128*  4 + 128*  3 +  2].id = 3;
    // chunk->block[128*128* 50 + 128*  2 + 28].id = 4;
    // chunk->block[128*128*  9 + 128* 33 +121].id = 5;
    // chunk->block[128*128* 73 + 128* 99 +  4].id = 6;

    leinad_region_t* region3 = NULL;
    region3 = leinad_region_create_from_chunk(chunk);

    if (region3 == NULL) {printf("region2 not properly initialized"); return SDL_APP_FAILURE;}

    aux2 = leinad_region_getblock(0,0,0,region3);
    SDL_Log(
        """\n"
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux2.id, aux2.rotation_n_subpos, aux2.rotation_n_subpos
    ); SDL_Log("block should be: %d\n",0x55);

    aux2 = leinad_region_getblock(1,1,1,region3);
    SDL_Log(
        """\n"
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux2.id, aux2.rotation_n_subpos, aux2.rotation_n_subpos
    ); SDL_Log("block should be: %d\n",1);

    aux2 = leinad_region_getblock(2,2,2,region3);
    SDL_Log(
        """\n"
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux2.id, aux2.rotation_n_subpos, aux2.rotation_n_subpos
    ); SDL_Log("block should be: %d\n",1);

    aux2 = leinad_region_getblock(6,9,4,region3);
    SDL_Log(
        """\n"
        "block: %d\n"
        "custom_placement: %x\n"
        "custom_data: %x\n"
        , aux2.id, aux2.rotation_n_subpos, aux2.rotation_n_subpos
    ); SDL_Log("block should be: %d\n",1);

    SDL_free(region3);

    // end of manual checks

    // start of auto test
    SDL_Log("- - AUTOTEST START - -");
    
    SDL_srand(time(NULL));
    for (Uint32 i = 0; i < 40; i++) {
        leinad_blockdata_clone(
            (struct blockdata){.id=SDL_rand(0xFFFE),.rotation_n_subpos=SDL_rand(0xFFFF),.custom_data=SDL_rand(0xFFFFFFFF)},
            &chunk->block[SDL_rand(128*128*128)]
        );
    }

    region3 = leinad_region_create_from_chunk(chunk);

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
            // printf(
            //     """"""
            //     "block <%d,%d,%d> failed by comparing:\n"
            //     "A: %x %x %x\n"
            //     "B: %x %x %x\n"
            //     """""",
            //     x,y,z,
            //     chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].id,
            //      chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].rotation_n_subpos,
            //      chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x].custom_data,
            //     potato.id, potato.rotation_n_subpos, potato.custom_data
            // );
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
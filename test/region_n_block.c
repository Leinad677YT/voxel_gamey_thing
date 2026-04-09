
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

#include "../src/world/block.c"

SDL_AppResult SDL_AppInit(
    __attribute__ ((unused)) void **appstate,
    __attribute__ ((unused)) int argc,
    __attribute__ ((unused)) char *argv[]
) {
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

    chunk->block[0].id = LEINAD_BLOCK_STONE;
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
    ); SDL_Log("block should be: %d\n",0);

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


    SDL_free(chunk);
    SDL_free(region3);

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
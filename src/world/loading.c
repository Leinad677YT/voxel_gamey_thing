#include <leinad/data/types.h>

#include <leinad/world/region.h>
#include <leinad/world/loading.h>

#include <leinad/player.h>

struct _loaded_chunks loaded_chunks = {
    .chunk = {0},
    .region = {0}
};

/**
 * Iterate over the loaded_chunks from furthest to nearest
 * @todo nearest to furthest
 */
void loaded_chunks_forall_decreasing(void(*fun)(leinad_chunk_t*,void*),void* arg) {

    #define invert(x) (LOADED_CHUNKS_LENGTH -x -1)
    
    // volume to check
    for(int volume = 0; volume < 8; volume++)
     // position to check
     for(int k = 0; k < LOADED_CHUNKS_RADIUS; k++)
      for(int j = 0; j < LOADED_CHUNKS_RADIUS; j++)
       for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (volume){
            case 0: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 3: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 4: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 5: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 6: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 7: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
        }
    }

    for(int strip = 0; strip < 12; strip++)
     // position to check
     for(int j = 0; j < LOADED_CHUNKS_RADIUS; j++)
      for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (strip){
            case 0:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 3:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            
            case 4:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 5:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 6:  fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 7:  fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;

            case 8:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 9:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 10: fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 11: fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
        }
    }

    for(int row = 0; row < 12; row++)
     // position to check
     for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (row){
            case 0:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 3:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 4:  fun(loaded_chunks.chunk[i        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 5:  fun(loaded_chunks.chunk[invert(i)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
        }
    }

    fun(loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH)/2],arg);

    #undef invert
}




LEINAD_FINITIALIZER int leinad_chunk_load(leinad_chunk_t** restrict chunk, float x, float y, float z) {

    

    success:
        return SDL_APP_CONTINUE;
    failure:
        return SDL_APP_FAILURE;
}



void load_around_player(struct entity_player* player) {
    struct leinad_position player_pos = leinad_entity_getpos(&player->generic);
    struct leinad_position center_pos = {0};

    int tries = 0;


  { // move already loaded chunks if center does not match
    
    start_move:

    center_pos = (struct leinad_position){    
        loaded_chunks.center_pos[0],
        loaded_chunks.center_pos[1],
        loaded_chunks.center_pos[2]
    };
    
    // player.y below center.y
    #define POS i
    if (player_pos.y < center_pos.y) {

        // all loaded to Y+1
        for (int i = raise3(LOADED_CHUNKS_LENGTH) -raise2(LOADED_CHUNKS_LENGTH) -1; i >= 0 ; i--)
            loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)] = loaded_chunks.chunk[POS];
        for (int i = 0; i < raise2(LOADED_CHUNKS_LENGTH); i++) 
            loaded_chunks.chunk[POS] = NULL;

        loaded_chunks.center_pos[1] -= LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.y - SDL_fmod(player_pos.y, LEINAD_REGION_RADIUS) > center_pos.y + LEINAD_REGION_RADIUS) {

        // all loaded to Y-1
        for(int i = 0; i < raise3(LOADED_CHUNKS_LENGTH) - raise2(LOADED_CHUNKS_LENGTH); i++)
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)];
        for (int i = raise3(LOADED_CHUNKS_LENGTH) -raise2(LOADED_CHUNKS_LENGTH); i < raise3(LOADED_CHUNKS_LENGTH); i++) 
            loaded_chunks.chunk[POS] = NULL;

        loaded_chunks.center_pos[1] += LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS

    // player.z below center.z
    #define POS (i*raise2(LOADED_CHUNKS_LENGTH) + j)
    if (player_pos.z < center_pos.z) {

        // all loaded to Z+1
        for (int i = LOADED_CHUNKS_LENGTH -1; i >= 0; i--)
         for (int j = raise2(LOADED_CHUNKS_LENGTH) - LOADED_CHUNKS_LENGTH -1; j >= 0; j--)
            loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH] = loaded_chunks.chunk[POS];

        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH; j++)
            loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) +j] = NULL;

        loaded_chunks.center_pos[2] -= LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.z - SDL_fmod(player_pos.z, LEINAD_REGION_RADIUS) > center_pos.z + LEINAD_REGION_RADIUS) {

        // all loaded to Z-1
        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < raise2(LOADED_CHUNKS_LENGTH) - LOADED_CHUNKS_LENGTH; j++)
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH];

        for (int i = 1; i <= LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH; j++)
            loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) - j] = NULL;

        loaded_chunks.center_pos[2] += LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS

    // player.x below center.x
    #define POS (i*LOADED_CHUNKS_LENGTH + j)
    if (player_pos.x < center_pos.x) {

        // all loaded to X+1
        for (int i = raise2(LOADED_CHUNKS_LENGTH) -1; i >= 0; i--)
         for (int j = LOADED_CHUNKS_LENGTH - 1; j >= 0; j--)
            loaded_chunks.chunk[POS + 1] = loaded_chunks.chunk[POS];

        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH; j++)
            loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) + j*LOADED_CHUNKS_LENGTH] = NULL;

        loaded_chunks.center_pos[0] -= LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.x - SDL_fmod(player_pos.x, LEINAD_REGION_RADIUS) > center_pos.x + LEINAD_REGION_RADIUS) {

        // all loaded to X-1
        for (int i = 0; i < raise2(LOADED_CHUNKS_LENGTH); i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH - 1; j++)
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + 1];

        for (int i = 1; i <= raise2(LOADED_CHUNKS_LENGTH); i++)
            loaded_chunks.chunk[i*LOADED_CHUNKS_LENGTH -1] = NULL;

        loaded_chunks.center_pos[0] += LEINAD_REGION_RADIUS;
        tries++; if (tries < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS
  }
    end_move:

    if (tries == LOADED_CHUNKS_LENGTH) {
        loaded_chunks.center_pos[0] = player_pos.x - SDL_fmod(player_pos.x,LEINAD_REGION_RADIUS);
        loaded_chunks.center_pos[1] = player_pos.y - SDL_fmod(player_pos.y,LEINAD_REGION_RADIUS);
        loaded_chunks.center_pos[2] = player_pos.z - SDL_fmod(player_pos.z,LEINAD_REGION_RADIUS);
    }

    center_pos.x = loaded_chunks.center_pos[0];
    center_pos.y = loaded_chunks.center_pos[1];
    center_pos.z = loaded_chunks.center_pos[2];

    SDL_Log("end");

}

/** 
 * @todo convert to regions and save to disk depending on direction
 */ 
void unload_chunk(leinad_chunk_t** chunk, enum load_direction dir) {
    switch(dir) {
        case LOAD_DIR_pX:
        case LOAD_DIR_nX:
        case LOAD_DIR_pY:
        case LOAD_DIR_nY:
        case LOAD_DIR_pZ:
        case LOAD_DIR_nZ:
        case LOAD_DIR_none:
            break;
    }
    leinad_chunk_free(*chunk);
    *chunk = NULL; 
}
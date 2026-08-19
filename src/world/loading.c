#include <leinad/data/types.h>

#include <leinad/world/region.h>
#include <leinad/world/loading.h>

#include <leinad/player.h>
#include <leinad/datatype/dimension.h>

struct _loaded_chunks loaded_chunks = {
    .chunk = {0},
    .region = {0}
};

/**
 * Iterate over the loaded_chunks from furthest to nearest (NOT EXHAUSTIVE!)
 * This function only ensures that for 2 chunks: A and B; A will run the
 *  function before B iff d(A) > d(B)
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

    for(int row = 0; row < 6; row++)
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

/**
 * Iterate over the loaded_chunks from nearest to furthest (NOT EXHAUSTIVE!)
 * This function only ensures that for 2 chunks: A and B; A will run the
 *  function before B iff d(A) < d(B)
 */
void loaded_chunks_forall_increasing(void(*fun)(leinad_chunk_t*,void*),void* arg) {

    #define invert(x) (LOADED_CHUNKS_LENGTH -x -1)

    fun(loaded_chunks.chunk[raise3(LOADED_CHUNKS_LENGTH)/2],arg);
    
    for(int row = 5; row >= 0; row--)
     // position to check
     for(int i = LOADED_CHUNKS_RADIUS -1; i >= 0; i--) {
        switch (row){
            case 0:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 3:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 4:  fun(loaded_chunks.chunk[i        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 5:  fun(loaded_chunks.chunk[invert(i)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
        }
    }

    for(int strip = 11; strip >= 0; strip--)
     // position to check
     for(int j = LOADED_CHUNKS_RADIUS -1; j >= 0; j--)
      for(int i = LOADED_CHUNKS_RADIUS -1; i >= 0 ; i--) {
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

    // volume to check
    for(int volume = 7; volume >= 0; volume--)
     // position to check
     for(int k = LOADED_CHUNKS_RADIUS -1; k >= 0; k--)
      for(int j = LOADED_CHUNKS_RADIUS -1; j >= 0; j--)
       for(int i = LOADED_CHUNKS_RADIUS -1; i >= 0; i--) {
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


    #undef invert
}


#define MAX_HEIGHT 128
#define WATER_LEVEL 64

LEINAD_FINITIALIZER void leinad_chunk_generate(leinad_chunk_t* restrict chunk){

    switch (chunk->dimension->generator->generic.type) {
        default:
        case LEINAD_DIMENSION_GENERATOR_DEBUG:
            if (chunk->pos[1] != 0 || chunk->pos[0] < 0 || chunk->pos[2] < 0) break;
            
            const int y = 0;

            for (int z = chunk->pos[2]; z < SDL_min(chunk->pos[2] + LEINAD_REGION_RADIUS,3*(int)SDL_ceil(SDL_sqrt(LEINAD_BLOCK_amount))); z+=3)
             for (int x = chunk->pos[0]; x < SDL_min(chunk->pos[0] + LEINAD_REGION_RADIUS,3*(int)SDL_ceil(SDL_sqrt(LEINAD_BLOCK_amount))); x+=3) {
                if ((z / 3) * (int)SDL_ceil(SDL_sqrt(LEINAD_BLOCK_amount)) + x / 3 > LEINAD_BLOCK_amount) break;
                chunk->block[leinad_get_chunk_index(x - (int)chunk->pos[0],y,z - (int)chunk->pos[2])].id = (z / 3) * (int)SDL_ceil(SDL_sqrt(LEINAD_BLOCK_amount)) + x / 3;
                chunk->block[leinad_get_chunk_index(x - (int)chunk->pos[0],y,z - (int)chunk->pos[2])].light = (LEINAD_BLOCK_MAX_LIGHT_LEVEL) | (LEINAD_BLOCK_MAX_LIGHT_LEVEL << 4);
                SDL_Log("x: %d z: %d, id- %d",x,z,(z / 3) * (int)SDL_ceil(SDL_sqrt(LEINAD_BLOCK_amount)) + x / 3);
            }
            break;
        case LEINAD_DIMENSION_GENERATOR_FLAT:
            break;
        case LEINAD_DIMENSION_GENERATOR_NOISE:
            break;
    }
}


LEINAD_FINITIALIZER int leinad_chunk_load(leinad_chunk_t** restrict chunk, float x, float y, float z, struct dimension* dimension) {

    #define loaded false

    if (loaded) {
        // idk lol, just grab it
    }
    else {
        *chunk = leinad_chunk_create(x,y,z,dimension);
        if (*chunk == NULL) goto failure;

        leinad_chunk_generate(*chunk);

        for (int c = 0; c < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; c++)
         for (int b = 0; b < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; b++)
          for (int a = 0; a < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; a++)
            leinad_chunk_create_mesh(*chunk, c, a, b);

    }

    success:
        return SDL_APP_CONTINUE;
    failure:
        return SDL_APP_FAILURE;

    #undef loaded
}



void load_around_player(struct entity_player* player) {
    struct leinad_position player_pos = leinad_entity_getpos(&player->generic);
    struct leinad_position center_pos = {0};

    int tries_x, tries_y, tries_z = tries_y = tries_x = 0;


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

        SDL_Log("# -y");
        // all loaded to Y+1
        for (int i = raise3(LOADED_CHUNKS_LENGTH) -raise2(LOADED_CHUNKS_LENGTH) -1; i >= 0 ; i--){
            if (i >= raise3(LOADED_CHUNKS_LENGTH) -2*raise2(LOADED_CHUNKS_LENGTH))
                unload_chunk(&loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)],LOAD_DIR_nY);
            loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)] = loaded_chunks.chunk[POS];
        }
        for (int i = 0; i < raise2(LOADED_CHUNKS_LENGTH); i++) 
            leinad_chunk_load(
            &(loaded_chunks.chunk[POS]),
            loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)]->pos[0],
            loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)]->pos[1] -LEINAD_REGION_RADIUS,
            loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)]->pos[2],
            leinad_dimension_getfromid(player->dimension)
        );

        loaded_chunks.center_pos[1] -= LEINAD_REGION_RADIUS;
        tries_y++; if (tries_y < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.y > center_pos.y + LEINAD_REGION_RADIUS) {

        SDL_Log("# +y");
        // all loaded to Y-1
        for(int i = 0; i < raise3(LOADED_CHUNKS_LENGTH) - raise2(LOADED_CHUNKS_LENGTH); i++) {
            if (i < raise2(LOADED_CHUNKS_LENGTH))
                unload_chunk(&loaded_chunks.chunk[POS],LOAD_DIR_pY);
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + raise2(LOADED_CHUNKS_LENGTH)];
        }
        for (int i = raise3(LOADED_CHUNKS_LENGTH) -raise2(LOADED_CHUNKS_LENGTH); i < raise3(LOADED_CHUNKS_LENGTH); i++) 
            leinad_chunk_load(
                &loaded_chunks.chunk[POS],
                loaded_chunks.chunk[POS - raise2(LOADED_CHUNKS_LENGTH)]->pos[0],
                loaded_chunks.chunk[POS - raise2(LOADED_CHUNKS_LENGTH)]->pos[1] +LEINAD_REGION_RADIUS,
                loaded_chunks.chunk[POS - raise2(LOADED_CHUNKS_LENGTH)]->pos[2],
            leinad_dimension_getfromid(player->dimension)
            );

        loaded_chunks.center_pos[1] += LEINAD_REGION_RADIUS;
        tries_y++; if (tries_y < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS

    // player.z below center.z
    #define POS (i*raise2(LOADED_CHUNKS_LENGTH) + j)
    if (player_pos.z < center_pos.z) {

    /*
        0  1  2    3  4  5    6  7  8

        9 10 11   12 13 14   15 16 17
    
       18 19 20   21 22 23   24 25 26
    */
        SDL_Log("# -z");
        // all loaded to Z+1
        for (int i = LOADED_CHUNKS_LENGTH -1; i >= 0; i--)
         for (int j = raise2(LOADED_CHUNKS_LENGTH) - LOADED_CHUNKS_LENGTH -1; j >= 0; j--) {
            if (j >  raise2(LOADED_CHUNKS_LENGTH) - 2*LOADED_CHUNKS_LENGTH -1) {
                unload_chunk(&loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH],LOAD_DIR_nZ);
            }
            loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH] = loaded_chunks.chunk[POS];
        }

        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH; j++) {
            leinad_chunk_load(
                &loaded_chunks.chunk[POS],
                loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH]->pos[0],
                loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH]->pos[1],
                loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH]->pos[2] -LEINAD_REGION_RADIUS,
            leinad_dimension_getfromid(player->dimension)
            );
        }
        loaded_chunks.center_pos[2] -= LEINAD_REGION_RADIUS;
        tries_z++; if (tries_z < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.z > center_pos.z + LEINAD_REGION_RADIUS) {

        SDL_Log("# +z");
        // all loaded to Z-1
        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < raise2(LOADED_CHUNKS_LENGTH) - LOADED_CHUNKS_LENGTH; j++) {
            if (j <  LOADED_CHUNKS_LENGTH){
                unload_chunk(&loaded_chunks.chunk[POS],LOAD_DIR_pZ);
            }
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + LOADED_CHUNKS_LENGTH];
        }
        for (int i = 1; i <= LOADED_CHUNKS_LENGTH; i++)
         for (int j = 1; j <= LOADED_CHUNKS_LENGTH; j++) {
            leinad_chunk_load(
                &loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) - j],
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) - j - LOADED_CHUNKS_LENGTH]->pos[0],
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) - j - LOADED_CHUNKS_LENGTH]->pos[1],
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) - j - LOADED_CHUNKS_LENGTH]->pos[2] +LEINAD_REGION_RADIUS,
            leinad_dimension_getfromid(player->dimension)
            );
        }
        loaded_chunks.center_pos[2] += LEINAD_REGION_RADIUS;
        tries_z++; if (tries_z < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS

    // player.x below center.x
    #define POS (i*LOADED_CHUNKS_LENGTH + j)
    if (player_pos.x < center_pos.x) {

        SDL_Log("# -x");
        // all loaded to X+1
        for (int i = raise2(LOADED_CHUNKS_LENGTH) -1; i >= 0; i--)
         for (int j = LOADED_CHUNKS_LENGTH - 1; j >= 1; j--) {
            if (j == LOADED_CHUNKS_LENGTH -1)
                unload_chunk(&loaded_chunks.chunk[POS],LOAD_DIR_nX);
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS-1];
        }
        for (int i = 0; i < LOADED_CHUNKS_LENGTH; i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH; j++)
            leinad_chunk_load(
                &loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) + j*LOADED_CHUNKS_LENGTH],
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) + j*LOADED_CHUNKS_LENGTH +1]->pos[0] -LEINAD_REGION_RADIUS,
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) + j*LOADED_CHUNKS_LENGTH +1]->pos[1],
                loaded_chunks.chunk[i*raise2(LOADED_CHUNKS_LENGTH) + j*LOADED_CHUNKS_LENGTH +1]->pos[2],
            leinad_dimension_getfromid(player->dimension)
            );

        loaded_chunks.center_pos[0] -= LEINAD_REGION_RADIUS;
        tries_x++; if (tries_x < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    } else if (player_pos.x > center_pos.x + LEINAD_REGION_RADIUS) {

        SDL_Log("# +x");
        // all loaded to X-1
        for (int i = 0; i < raise2(LOADED_CHUNKS_LENGTH); i++)
         for (int j = 0; j < LOADED_CHUNKS_LENGTH - 1; j++) {
            if (j == 0)
                unload_chunk(&loaded_chunks.chunk[POS],LOAD_DIR_pX);
            loaded_chunks.chunk[POS] = loaded_chunks.chunk[POS + 1];
        }
        for (int i = 1; i <= raise2(LOADED_CHUNKS_LENGTH); i++){
            leinad_chunk_load(
                &loaded_chunks.chunk[i*LOADED_CHUNKS_LENGTH -1],
                loaded_chunks.chunk[i*LOADED_CHUNKS_LENGTH -2]->pos[0] + LEINAD_REGION_RADIUS,
                loaded_chunks.chunk[i*LOADED_CHUNKS_LENGTH -2]->pos[1],
                loaded_chunks.chunk[i*LOADED_CHUNKS_LENGTH -2]->pos[2],
            leinad_dimension_getfromid(player->dimension)
            );
        }
        loaded_chunks.center_pos[0] += LEINAD_REGION_RADIUS;
        tries_x++; if (tries_x < LOADED_CHUNKS_LENGTH) goto start_move; else goto end_move;

    }
    #undef POS
  }
    end_move:

    if (tries_y == LOADED_CHUNKS_LENGTH || tries_z == LOADED_CHUNKS_LENGTH || tries_x == LOADED_CHUNKS_LENGTH) {
        loaded_chunks.center_pos[0] = player_pos.x - SDL_fmod(SDL_fmod(player_pos.x,LEINAD_REGION_RADIUS) + LEINAD_REGION_RADIUS,LEINAD_REGION_RADIUS);
        loaded_chunks.center_pos[1] = player_pos.y - SDL_fmod(SDL_fmod(player_pos.y,LEINAD_REGION_RADIUS) + LEINAD_REGION_RADIUS,LEINAD_REGION_RADIUS);
        loaded_chunks.center_pos[2] = player_pos.z - SDL_fmod(SDL_fmod(player_pos.z,LEINAD_REGION_RADIUS) + LEINAD_REGION_RADIUS,LEINAD_REGION_RADIUS);
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
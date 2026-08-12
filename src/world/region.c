#include <leinad/math/arithmetic.h>
#include "../libs/bit_manipulation.h"

#include <leinad/world/region.h>
#include <leinad/render.h>
#include <leinad/app.h>
#include <leinad/world/block.h>

#include "meshing_new.c"

LEINAD_AUX // fills masks used to get data depending on subregion-offset [0,7]
           // if both umask and pmask are the same variable, result = umask
#define GETMASKS(umask,pmask,value) \
    switch (value){ \
        case 7: pmask = 0b11111110; umask = 0b00000001; break; \
        case 6: pmask = 0b11111100; umask = 0b00000010; break; \
        case 5: pmask = 0b11111000; umask = 0b00000100; break; \
        case 4: pmask = 0b11110000; umask = 0b00001000; break; \
        case 3: pmask = 0b11100000; umask = 0b00010000; break; \
        case 2: pmask = 0b11000000; umask = 0b00100000; break; \
        case 1: pmask = 0b10000000; umask = 0b01000000; break; \
        case 0: pmask = 0b00000000; umask = 0b10000000; break; \
    }


// x,y,z are already modulo'd 128
LEINAD_FGET struct blockdata leinad_region_getblock(int x, int y, int z, leinad_region_t* region) {
    struct blockdata data = {0};
    Uint16 offset, offset_i;
    Uint8 umask, pmask, check;
    Uint64 index, aux_index, aux_index2;

    const Uint8* flags[7] = {
        &region->redirection_flags[0b1001001001001],
        &region->redirection_flags[0b1001001001001],
        &region->redirection_flags[0b1001001001],
        &region->redirection_flags[0b1001001],
        &region->redirection_flags[0b1001],
        &region->redirection_flags[0b1],
        &region->redirection_flags[0]
    };


    // FULLxFULLxFULL
    if (region->ctrl_data & is_full_region) {
        // (only exists 1 entry)
        index = 0;
    }
    else {
        short lvl;
        offset = 0; 
        check = 0;

        // gets last available level
        for (lvl = 6; lvl > 0; lvl--){
            offset = (offset << 3) | check;
            check = (((y >> lvl) &1) << 2) | (((z >> lvl) &1) << 1) | (((x >> lvl) &1) << 0);
            GETMASKS(umask, pmask, check);
            if (flags[lvl][offset] & umask) break;
        }

        // sets base index accordingly
        index = 0;
        switch(lvl) { 
            case 0:
                index+=((region->amounts_perLODlevel & mask_amount_02x) >> 45);

            __attribute__((fallthrough)); 
            case 1:
                index+=((region->amounts_perLODlevel & mask_amount_04x) >> 30);

            __attribute__((fallthrough)); 
            case 2:
                index+=((region->amounts_perLODlevel & mask_amount_08x) >> 18);

            __attribute__((fallthrough)); 
            case 3:
                index+=((region->amounts_perLODlevel & mask_amount_16x) >> 9);

            __attribute__((fallthrough)); 
            case 4:
                index+=((region->amounts_perLODlevel & mask_amount_32x) >> 3);

            __attribute__((fallthrough)); 
            case 5:
                index+= (region->amounts_perLODlevel & mask_amount_64x);

            __attribute__((fallthrough)); 
            case 6:
                index += (lvl) ? count_set_bits(flags[lvl][offset] & pmask) 
                               : (count_set_bits((~flags[0][offset]) & pmask) << 3);
        }

        // adds inbetween values

        if (lvl) { // if not special case (1x1x1)
            for (offset_i = 0 ; offset_i < offset; offset_i++) {
                index += count_set_bits(flags[lvl][offset_i]);                
            }
        }
        else { // 1x1x1 needs special counting for its same level
            aux_index = 0;

            // counts all non-greater-than 2x2x2 subregions
            for (offset_i = 0 ; offset_i <offset; offset_i++) {
                aux_index += count_set_bits((~flags[0][offset_i]) & 0xFF);                
            }

            
          { // remove the false positives
            offset =0; check = 0;
            for (lvl = 6; lvl > 1; lvl--){
                offset = (offset << 3) | check;
                check = (((y >> lvl) &1) << 2) | (((z >> lvl) &1) << 1) | (((x >> lvl) &1) << 0);

                GETMASKS(umask, pmask, check);
                aux_index2 = count_set_bits(flags[lvl][offset] & pmask);
    
                for (offset_i = 0 ; offset_i < offset; offset_i++) {
                    aux_index2 += count_set_bits(flags[lvl][offset_i]);                
                }

                aux_index -= aux_index2 << (3*(lvl-1));
            }
          }

          // add resulting index offset to the calculation
          index += (aux_index << 3) + (((y &1) << 2) | ((z &1) << 1) | ((x &1) << 0));
        }
    }

    // fill in the data and return
    leinad_blockdata_clone(region->region_data[index], &data);
    return data;
}

LEINAD_FBUILDER leinad_chunk_t* leinad_chunk_create(float x, float y, float z) {
    leinad_chunk_t* chunk;
    Uint32 i;

    chunk = SDL_malloc(sizeof(leinad_chunk_t));

    if (chunk == NULL) return NULL;
    
    for(i = 0; i < raise3(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS); i++){
        chunk->mesh[i] = SDL_malloc(sizeof(struct chunk_mesh));
        SDL_memset(chunk->mesh[i],0,sizeof(struct chunk_mesh));
    }

    for(i = 0; i< raise3(LEINAD_REGION_RADIUS); i++) {
        chunk->block[i] = (struct blockdata){.id = LEINAD_BLOCK_default, .rotation_n_subpos = 0, .custom_data = 0};
    }

    chunk->pos[0] = x; chunk->pos[1] = y; chunk->pos[2] = z;

    return chunk;
}

LEINAD_FCLEANER void leinad_chunk_free(leinad_chunk_t* chunk) {
    if (chunk == NULL) return;
    
    for (int i = 0; i < raise3(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS); i++) {
        
        if (chunk->mesh[i] != NULL) {
            // if (chunk->mesh[i]->index != NULL) SDL_Log("released  IDX: %20p\t\tVTX: %20p",chunk->mesh[i]->index,chunk->mesh[i]->vertex);
            if (chunk->mesh[i]->index_opaque != NULL)  SDL_ReleaseGPUBuffer(APP.device,chunk->mesh[i]->index_opaque );
            if (chunk->mesh[i]->vertex_opaque != NULL) SDL_ReleaseGPUBuffer(APP.device,chunk->mesh[i]->vertex_opaque);
            if (chunk->mesh[i]->index_translucent != NULL)  SDL_ReleaseGPUBuffer(APP.device,chunk->mesh[i]->index_translucent );
            if (chunk->mesh[i]->vertex_translucent != NULL) SDL_ReleaseGPUBuffer(APP.device,chunk->mesh[i]->vertex_translucent);
        }

        SDL_free(chunk->mesh[i]);
    }

    SDL_free(chunk);
}


/**
 * @todo
 * attention citizens! THIS CODE CAN BE VERY HEAVILY OPTIMIZED, LIKE, CRAZY
 *  OPTIMIZED, IF I HAD A NICKEL FOR EVERY OPTIMIZATION THIS CODE COULD
 *  HAVE, I WOULD HAVE MORE THAN, IDK, 4 NICKELS, WHICH ARE A LOT
 *  PLEASE DONT FORGET THIS CODE PLEASE I BEG OF YOU :sob:
 *
 * does not override the region's pos, useful for cloning regions or saving data
 */
LEINAD_FINITIALIZER leinad_chunk_t* leinad_chunk_setfromregion(leinad_region_t* region, leinad_chunk_t* chunk){
    
  { // iterate over the region, getting the individual blocks 
    Uint32 x,y,z;
    Uint32 xx,yy,zz;
    Uint32 xxx,yyy,zzz;
    Uint32 xxxx,yyyy,zzzz;
    Uint32 xxxxx,yyyyy,zzzzz;
    Uint32 xxxxxx,yyyyyy,zzzzzz;
    Uint32 xxxxxxx,yyyyyyy,zzzzzzz;

    // iterates over a 128x128x128
    for (yyyyyyy = 0; yyyyyyy < 2; yyyyyyy++) for (zzzzzzz = 0; zzzzzzz < 2; zzzzzzz++) for (xxxxxxx = 0; xxxxxxx < 2; xxxxxxx++)
     // iterates over a 64x64x64
     for (yyyyyy = yyyyyyy*2; yyyyyy< (yyyyyyy+1)*2; yyyyyy++) for (zzzzzz = zzzzzzz*2; zzzzzz< (zzzzzzz+1)*2; zzzzzz++) for (xxxxxx = xxxxxxx*2; xxxxxx< (xxxxxxx+1)*2; xxxxxx++)
      // iterates over a 32x32x32
      for (yyyyy = yyyyyy*2; yyyyy< (yyyyyy+1)*2; yyyyy++) for (zzzzz = zzzzzz*2; zzzzz< (zzzzzz+1)*2; zzzzz++) for (xxxxx = xxxxxx*2; xxxxx< (xxxxxx+1)*2; xxxxx++)
       // iterates over a 16x16x16
       for (yyyy = yyyyy*2; yyyy< (yyyyy+1)*2; yyyy++) for (zzzz = zzzzz*2; zzzz< (zzzzz+1)*2; zzzz++) for (xxxx = xxxxx*2; xxxx< (xxxxx+1)*2; xxxx++)
        // iterates over a 8x8x8
        for (yyy = yyyy*2; yyy< (yyyy+1)*2; yyy++) for (zzz = zzzz*2; zzz< (zzzz+1)*2; zzz++) for (xxx = xxxx*2; xxx< (xxxx+1)*2; xxx++)
         // iterates over a 4x4x4
         for (yy = yyy*2; yy< (yyy+1)*2; yy++) for (zz = zzz*2; zz< (zzz+1)*2; zz++) for (xx = xxx*2; xx< (xxx+1)*2; xx++)
          // iterates over a 2x2x2
          for (y = yy*2; y< (yy+1)*2; y++) for (z = zz*2; z< (zz+1)*2; z++) for (x = xx*2; x< (xx+1)*2; x++)
           { // works on the 1x1x1
            leinad_blockdata_clone(leinad_region_getblock(x,y,z, region),&chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x]);
           }
  }
    return chunk;
}



LEINAD_FBUILDER leinad_region_t* leinad_region_create_from_chunk(leinad_chunk_t* chunk){
    leinad_region_t* result = NULL;
    struct blockdata checking_block = {.id = LEINAD_BLOCK_invalid, .rotation_n_subpos = 0, .custom_data = 0};
    int aux_count = 0;
    Uint32 full_region_check = 0;

    LEINAD_AUX struct blockdata* maps[6] = { 0 }; // 2x2x2 -> 64x64x64 (8:8)
    LEINAD_AUX Uint8* checks[6] = {0};            // 2x2x2 -> 64x64x64 (8:1)

  { // init memory

    // 02x02x02
    maps[0] = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8*8*8);
        if (maps[0] == NULL) return NULL;
    checks[0] = SDL_malloc(sizeof(Uint8) * 1*8*8*8*8*8);
        if (checks[0] == NULL) return NULL;
        SDL_memset(checks[0],0,1*8*8*8*8*8);

    // 04x04x04
    maps[1] = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8*8);
        if (maps[1] == NULL) return NULL;
    checks[1] = SDL_malloc(sizeof(Uint8) * 1*8*8*8*8);
        if (checks[1] == NULL) return NULL;
        SDL_memset(checks[1],0,1*8*8*8*8);
    
    // 08x08x08
    maps[2] = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8);
        if (maps[2] == NULL) return NULL;
    checks[2] = SDL_malloc(sizeof(Uint8) * 1*8*8*8);
        if (checks[2] == NULL) return NULL;
        SDL_memset(checks[2],0,1*8*8*8);
    
    // 16x16x16
    maps[3] = SDL_malloc(sizeof(struct blockdata) * 8*8*8);
        if (maps[3] == NULL) return NULL;
    checks[3] = SDL_malloc(sizeof(Uint8) * 1*8*8);
        if (checks[3] == NULL) return NULL;
        SDL_memset(checks[3],0,1*8*8);
    
    // 32x32x32
    maps[4] = SDL_malloc(sizeof(struct blockdata) * 8*8);
        if (maps[4] == NULL) return NULL;
    checks[4] = SDL_malloc(sizeof(Uint8) * 1*8);
        if (checks[4] == NULL) return NULL;
        SDL_memset(checks[4],0,1*8);
    
    // 64x64x64
    maps[5] = SDL_malloc(sizeof(struct blockdata) * 8);
        if (maps[5] == NULL) return NULL;
    checks[5] = SDL_malloc(sizeof(Uint8) * 1);
        if (checks[5] == NULL) return NULL;
        SDL_memset(checks[5],0,1);
  }
    
  { // iterate over the whole region (128x128x128), checking the 2x2x2 regions 
    aux_count = 0;
    Uint32 xx,yy,zz;
    Uint32 xxx,yyy,zzz;
    Uint32 xxxx,yyyy,zzzz;
    Uint32 xxxxx,yyyyy,zzzzz;
    Uint32 xxxxxx,yyyyyy,zzzzzz;
    Uint32 xxxxxxx,yyyyyyy,zzzzzzz;

    // iterates over a 128x128x128
    for (yyyyyyy = 0; yyyyyyy < 2; yyyyyyy++) for (zzzzzzz = 0; zzzzzzz < 2; zzzzzzz++) for (xxxxxxx = 0; xxxxxxx < 2; xxxxxxx++)
     // iterates over a 64x64x64
     for (yyyyyy = yyyyyyy*2; yyyyyy< (yyyyyyy+1)*2; yyyyyy++) for (zzzzzz = zzzzzzz*2; zzzzzz< (zzzzzzz+1)*2; zzzzzz++) for (xxxxxx = xxxxxxx*2; xxxxxx< (xxxxxxx+1)*2; xxxxxx++)
      // iterates over a 32x32x32
      for (yyyyy = yyyyyy*2; yyyyy< (yyyyyy+1)*2; yyyyy++) for (zzzzz = zzzzzz*2; zzzzz< (zzzzzz+1)*2; zzzzz++) for (xxxxx = xxxxxx*2; xxxxx< (xxxxxx+1)*2; xxxxx++)
       // iterates over a 16x16x16
       for (yyyy = yyyyy*2; yyyy< (yyyyy+1)*2; yyyy++) for (zzzz = zzzzz*2; zzzz< (zzzzz+1)*2; zzzz++) for (xxxx = xxxxx*2; xxxx< (xxxxx+1)*2; xxxx++)
        // iterates over a 8x8x8
        for (yyy = yyyy*2; yyy< (yyyy+1)*2; yyy++) for (zzz = zzzz*2; zzz< (zzzz+1)*2; zzz++) for (xxx = xxxx*2; xxx< (xxxx+1)*2; xxx++)
         // iterates over a 4x4x4
         for (yy = yyy*2; yy< (yyy+1)*2; yy++) for (zz = zzz*2; zz< (zzz+1)*2; zz++) for (xx = xxx*2; xx< (xxx+1)*2; xx++)
          { // iterates over a 2x2x2

            // get first block
            leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(2*xx+0,2*yy+0,2*zz+0)],&checking_block);

            // compare with the rest

            // if different than any, don't set the byte
            if (
                leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+1,2*yy+0,2*zz+0)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+0,2*yy+0,2*zz+1)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+1,2*yy+0,2*zz+1)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+0,2*yy+1,2*zz+0)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+1,2*yy+1,2*zz+0)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+0,2*yy+1,2*zz+1)]))
                || leinad_blockdata_comparator(&checking_block, &(chunk->block[leinad_get_chunk_index(2*xx+1,2*yy+1,2*zz+1)]))
            ) {
                maps[0][aux_count].id = LEINAD_BLOCK_invalid;
                maps[0][aux_count].rotation_n_subpos = 0;
                maps[0][aux_count].custom_data = 0;
            } else {
                // set bit
                switch (aux_count % 8) {
                    case 7: checks[0][aux_count/8] |= 0b00000001; break; \
                    case 6: checks[0][aux_count/8] |= 0b00000010; break; \
                    case 5: checks[0][aux_count/8] |= 0b00000100; break; \
                    case 4: checks[0][aux_count/8] |= 0b00001000; break; \
                    case 3: checks[0][aux_count/8] |= 0b00010000; break; \
                    case 2: checks[0][aux_count/8] |= 0b00100000; break; \
                    case 1: checks[0][aux_count/8] |= 0b01000000; break; \
                    case 0: checks[0][aux_count/8] |= 0b10000000; break; \
                }

                // set block
                leinad_blockdata_clone(checking_block,&maps[0][aux_count]);
            }
            aux_count++;
          }
  }

  { // iterate over the result of the 2x2x2 grouping, checking the rest of subregion sizes until the last
    Uint32 i;
    short lvl;
    for (lvl = 0; lvl < 5; lvl++) {
        for (i = 0; i < 0b1000000000000000000 >> (lvl*3); i+=8) { // 
            // get first block
            leinad_blockdata_clone(maps[lvl][i],&checking_block);

            // compare with the rest

            // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
            if (
                checking_block.id == LEINAD_BLOCK_invalid
                || (( 
                leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+1]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+2]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+3]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+4]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+5]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+6]))
                || leinad_blockdata_comparator(&checking_block, &(maps[lvl][i+7]))
                    ) || checks[lvl][i/8] != 0xFF)
            ) {
                maps[lvl+1][i/8].id = LEINAD_BLOCK_invalid;
                maps[lvl+1][i/8].rotation_n_subpos = 0;
                maps[lvl+1][i/8].custom_data = 0;
            } else {

                // set bit
                switch ((i/8) % 8) {
                    case 7: checks[lvl+1][i/64] |= 0b00000001; break; \
                    case 6: checks[lvl+1][i/64] |= 0b00000010; break; \
                    case 5: checks[lvl+1][i/64] |= 0b00000100; break; \
                    case 4: checks[lvl+1][i/64] |= 0b00001000; break; \
                    case 3: checks[lvl+1][i/64] |= 0b00010000; break; \
                    case 2: checks[lvl+1][i/64] |= 0b00100000; break; \
                    case 1: checks[lvl+1][i/64] |= 0b01000000; break; \
                    case 0: checks[lvl+1][i/64] |= 0b10000000; break; \
                }

                // set block
                leinad_blockdata_clone(checking_block,&maps[lvl+1][i/8]);
            }
        }
    }
  }

  { // iterate over the result of the 64x64x64 grouping, checking the whole 128x128x128 region 
    // iterates over a 128x128x128
    // get first block
        leinad_blockdata_clone(maps[5][0],&checking_block);

    // compare with the rest
    // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the flag
    if (
        checking_block.id == LEINAD_BLOCK_invalid
        || (( 
           leinad_blockdata_comparator(&checking_block, &(maps[5][1]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][2]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][3]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][4]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][5]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][6]))
        || leinad_blockdata_comparator(&checking_block, &(maps[5][7]))
            ) || checks[5][0] != 0xFF)
    ) {
    } else {

        // set bit
        full_region_check |= 0x1;
        
        // set block
        // already in `checking_block`
    }
  }

    // region completa de 1 bloque
    if (full_region_check) {
        result = SDL_malloc( 0 +
            sizeof(struct leinad_region)    // generic data
            + sizeof(struct blockdata) * 1  // block data
        );
        result->ctrl_data = 
            is_full_region
        ;
        result->amounts_perLODlevel = 0;

        SDL_memset(result->redirection_flags,0,0b1001001001001001+3);

        leinad_blockdata_clone(checking_block,&result->region_data[0]);

    }
    // region con mas de un tipo de bloque, debe dividirse
    else {
        Uint32 i;
        Uint64 per_level_count = 0, total_block_count = 0, aux = 0;;
        short lvl_start;
        // clear sublevels when a higher one is available (by checking bits)

        for (lvl_start = 5; lvl_start >= 0; lvl_start--){
            aux = 0;
            for (i = 0; i < 0b1000000000000000000 >> (lvl_start * 3); i++) {
                if (checks[lvl_start][i/8] & (0b10000000 >> (i % 8))) {
                    switch (lvl_start) {
                        
                        case 5:
                            SDL_memset(&checks[lvl_start-5][16*16*16*i],0,16*16*16);
            
                        __attribute__((fallthrough)); 
                        case 4:
                            SDL_memset(&checks[lvl_start-4][8*8*8*i],0,8*8*8);
            
                        __attribute__((fallthrough)); 
                        case 3:
                            SDL_memset(&checks[lvl_start-3][4*4*4*i],0,4*4*4);
             
                        __attribute__((fallthrough)); 
                        case 2:
                            SDL_memset(&checks[lvl_start-2][2*2*2*i],0,2*2*2);
                
                        __attribute__((fallthrough)); 
                        case 1:
                            SDL_memset(&checks[lvl_start-1][i],0,1);
                
                        __attribute__((fallthrough)); 
                        case 0:
                            aux++;
                    }
                }
            }

            switch (lvl_start) {
                case 5: per_level_count |= aux << 0; break;
                case 4: per_level_count |= aux << (3); break;
                case 3: per_level_count |= aux << (3+6); break;
                case 2: per_level_count |= aux << (3+6+9); break;
                case 1: per_level_count |= aux << (3+6+9+12); break;
                case 0: per_level_count |= aux << (3+6+9+12+15); break;
            }
            
            total_block_count += aux;
        }

      { // GET THE AMOUNT OF THE REMAINING 1x1x1 TO COVER
        aux = LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS
          - (((per_level_count) & mask_amount_64x) >> (0))        * 64*64*64
          - (((per_level_count) & mask_amount_32x) >> (3))        * 32*32*32
          - (((per_level_count) & mask_amount_16x) >> (3+6))      * 16*16*16
          - (((per_level_count) & mask_amount_08x) >> (3+6+9))       * 8*8*8
          - (((per_level_count) & mask_amount_04x) >> (3+6+9+12))    * 4*4*4
          - (((per_level_count) & mask_amount_02x) >> (3+6+9+12+15)) * 2*2*2
        ;

        total_block_count += aux;
      }
       
        // everything counted, so now its just allocating the region and
        // filling in the data
        // I'm almost free :')

        result = SDL_malloc( 0 +
            sizeof(struct leinad_region)                    // redirection flags
            + sizeof(struct blockdata) * total_block_count  //block data
        );
        
        result->amounts_perLODlevel = per_level_count;

        SDL_memcpy(&result->redirection_flags[0],&checks[5][0],1);
        SDL_memcpy(&result->redirection_flags[0b1],&checks[4][0],1*8);
        SDL_memcpy(&result->redirection_flags[0b1001],&checks[3][0],1*8*8);
        SDL_memcpy(&result->redirection_flags[0b1001001],&checks[2][0],1*8*8*8);
        SDL_memcpy(&result->redirection_flags[0b1001001001],&checks[1][0],1*8*8*8*8);
        SDL_memcpy(&result->redirection_flags[0b1001001001001],&checks[0][0],1*8*8*8*8*8);

        // aux will now be used as the current offset when inserting data
        aux = 0;

      { // move full subregions
        short lvl;
        for (lvl = 5; lvl >= 0; lvl-- ){
            for (i = 0; i < 0b1000000000000000000>>(3*lvl); i++) {
                // if flag is set, insert the block into the array
                if (checks[lvl][i/8] & (0b10000000 >> (i % 8))) {
                    leinad_blockdata_clone(maps[lvl][i],&result->region_data[aux]);
                    aux++;
                }
            }
        }
      }

    // HEY, DONT FORGET TO COPY THE EMPTY ONES AT THE END!
        {
          for (i = 0; i < 8 * 8 * 8 * 8 * 8 * 8; i++) {
            Uint32 x, y, z;

            // if flag is unset, insert the 8 block group into the array
            if (
                ((~checks[0][i/(8)])) & (0b10000000 >> (i % 8)) // 0 at 2x2x2
             && ((~checks[1][i/(8*8)])) & (0b10000000 >> (i/(8) % 8)) // 0 at 4x4x4
             && ((~checks[2][i/(8*8*8)])) & (0b10000000 >> (i/(8*8) % 8)) // 0 at 8x8x8
             && ((~checks[3][i/(8*8*8*8)])) & (0b10000000 >> (i/(8*8*8) % 8)) // 0 at 16x16x16
             && ((~checks[4][i/(8*8*8*8*8)])) & (0b10000000 >> (i/(8*8*8*8) % 8)) // 0 at 32x32x32
             && ((~checks[5][i/(8*8*8*8*8*8)])) & (0b10000000 >> (i/(8*8*8*8*8) % 8)) // 0 at 64x64x64
            ) {
                x = 2*(((i & 0b1) >> 0)   + ((i & 0b1000) >> 2)   + ((i & 0b1000000) >> 4)   + ((i & 0b1000000000) >> 6)   + ((i & 0b1000000000000) >> 8)    + ((i & 0b1000000000000000) >> 10)  );
                z = 2*(((i & 0b10) >> 1)  + ((i & 0b10000) >> 3)  + ((i & 0b10000000) >> 5)  + ((i & 0b10000000000) >> 7)  + ((i & 0b10000000000000) >> 9)   + ((i & 0b10000000000000000) >> 11) );
                y = 2*(((i & 0b100) >> 2) + ((i & 0b100000) >> 4) + ((i & 0b100000000) >> 6) + ((i & 0b100000000000) >> 8) + ((i & 0b100000000000000) >> 10) + ((i & 0b100000000000000000) >> 12));

                // printf("aux: %lu\n",aux); fflush(stdout);
                // printf("x:%d y:%d z:%d\n",x,y,z);fflush(stdout);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+0,y+0,z+0)],&result->region_data[aux+0]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+1,y+0,z+0)],&result->region_data[aux+1]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+0,y+0,z+1)],&result->region_data[aux+2]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+1,y+0,z+1)],&result->region_data[aux+3]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+0,y+1,z+0)],&result->region_data[aux+4]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+1,y+1,z+0)],&result->region_data[aux+5]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+0,y+1,z+1)],&result->region_data[aux+6]);
                leinad_blockdata_clone(chunk->block[leinad_get_chunk_index(x+1,y+1,z+1)],&result->region_data[aux+7]);
                aux+=8;
            } 
        }

    }
  }


    SDL_free(checks[0]);
    SDL_free(checks[1]);
    SDL_free(checks[2]);
    SDL_free(checks[3]);
    SDL_free(checks[4]);
    SDL_free(checks[5]);

    SDL_free(maps[0]);
    SDL_free(maps[1]);
    SDL_free(maps[2]);
    SDL_free(maps[3]);
    SDL_free(maps[4]);
    SDL_free(maps[5]);

    result->pos[0] = chunk->pos[0]; result->pos[1] = chunk->pos[1]; result->pos[2] = chunk->pos[2];
    return result;
}

LEINAD_FBUILDER leinad_region_t* leinad_region_create_empty(float x, float y, float z) {
    leinad_region_t* region_full = SDL_malloc(sizeof(leinad_region_t) + sizeof(struct blockdata[1]));
    
    region_full->amounts_perLODlevel = 0 & mask_amount_64x;
    region_full->ctrl_data = is_full_region;
    SDL_memset(region_full->redirection_flags,0,sizeof(region_full->redirection_flags));
    
    ((Uint16*)(region_full->region_data))[0*8+0] = (Uint16) LEINAD_BLOCK_AIR;
    ((Uint16*)(region_full->region_data))[0*8+1] = (Uint16) 0;
    ((Uint16*)(region_full->region_data))[0*8+2] = (Uint16) 0;
    ((Uint16*)(region_full->region_data))[0*8+3] = (Uint16) 0;

    region_full->pos[0] = x; region_full->pos[1] = y; region_full->pos[2] = z;
    return region_full;
}



LEINAD_FRENDER void leinad_chunk_render_opaque(leinad_chunk_t *chunk, void* ptr){
    struct _chunkrenderdata* data = ptr;
    
    // if chunk is not loaded, skip
    if (
        chunk == NULL
    ) return;

    for (int y = 0; y < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; y++)
     for (int z = 0; z < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; z++)
      for (int x = 0; x < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; x++) {
        #define mesh_id (y*raise2(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS) + z*LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS + x)
      
        const vec3 current_mesh_pos = {chunk->pos[0] + x*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[1] + y*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[2] + z*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2};
        // if chunk is not meshed or mesh doesn't have vertices, skip
        if (
            chunk->mesh[mesh_id] == NULL || chunk->mesh[mesh_id]->vertex_opaque == NULL
            || raise2(chunk->pos[0] + x*LEINAD_MESH_RADIUS - data->pos.x) + raise2(chunk->pos[1] + y*LEINAD_MESH_RADIUS  - data->pos.y) + raise2(chunk->pos[2]+ z*LEINAD_MESH_RADIUS  - data->pos.z) > raise2(LEINAD_REGION_RADIUS)
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[0]) < data->fov_planes[0].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[1]) < data->fov_planes[1].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[2]) < data->fov_planes[2].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[3]) < data->fov_planes[3].w
        ) continue;



        SDL_PushGPUVertexUniformData(data->command_buffer, 1, chunk->pos, 3* sizeof(float));
        SDL_BindGPUVertexBuffers(data->renderpass, 0, &(SDL_GPUBufferBinding){.buffer = chunk->mesh[mesh_id]->vertex_opaque, .offset = 0 },1);

        // if (data->viewvec.x < 0 && chunk->mesh[mesh_id]->index_directional[0] != NULL) { // -x
        //     SDL_BindGPUIndexBuffer(data->renderpass, &(SDL_GPUBufferBinding){ .buffer = chunk->mesh[mesh_id]->index_directional[0], .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_32BIT);


        //     SDL_DrawGPUIndexedPrimitives(data->renderpass, chunk->mesh[mesh_id]->ind_x_ 
        //         , 1, 0, 0, 0);

        // }
        
        SDL_BindGPUIndexBuffer(data->renderpass, &(SDL_GPUBufferBinding){ .buffer = chunk->mesh[mesh_id]->index_opaque, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(data->renderpass, chunk->mesh[mesh_id]->ind_o 
            , 1, 0, 0, 0);

    }

}

LEINAD_FRENDER void leinad_chunk_render_transparent(leinad_chunk_t *chunk, void* ptr){
    struct _chunkrenderdata* data = ptr;

    // if chunk is not loaded, skip
    if (
        chunk == NULL
    ) return;

    for (int y = 0; y < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; y++)
     for (int z = 0; z < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; z++)
      for (int x = 0; x < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; x++) {
        #define mesh_id (y*raise2(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS) + z*LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS + x)
      
        const vec3 current_mesh_pos = {chunk->pos[0] + x*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[1] + y*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[2] + z*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2};
        // if chunk is not meshed or mesh doesn't have vertices, skip
        if (
            chunk->mesh[mesh_id] == NULL || chunk->mesh[mesh_id]->vertex_translucent == NULL
            || raise2(chunk->pos[0] + x*LEINAD_MESH_RADIUS - data->pos.x) + raise2(chunk->pos[1] + y*LEINAD_MESH_RADIUS  - data->pos.y) + raise2(chunk->pos[2]+ z*LEINAD_MESH_RADIUS  - data->pos.z) > raise2(LEINAD_REGION_RADIUS)
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[0]) < data->fov_planes[0].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[1]) < data->fov_planes[1].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[2]) < data->fov_planes[2].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[3]) < data->fov_planes[3].w
        ) continue;

        SDL_PushGPUVertexUniformData(data->command_buffer, 1, chunk->pos, 3* sizeof(float));
        SDL_BindGPUVertexBuffers(data->renderpass, 0, &(SDL_GPUBufferBinding){.buffer = chunk->mesh[mesh_id]->vertex_translucent, .offset = 0 },1);
        SDL_BindGPUIndexBuffer(data->renderpass, &(SDL_GPUBufferBinding){ .buffer = chunk->mesh[mesh_id]->index_translucent, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(data->renderpass, chunk->mesh[mesh_id]->ind_t, 1, 0, 0, 0);

        #undef mesh_id
    }

}

LEINAD_FRENDER void leinad_chunk_render_front(leinad_chunk_t *chunk, void* ptr){
    struct _chunkrenderdata* data = ptr;

    // if chunk is not loaded, skip
    if (
        chunk == NULL
    ) return;

    for (int y = 0; y < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; y++)
     for (int z = 0; z < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; z++)
      for (int x = 0; x < LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS; x++) {
        #define mesh_id (y*raise2(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS) + z*LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS + x)
        
        const vec3 current_mesh_pos = {chunk->pos[0] + x*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[1] + y*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2,chunk->pos[2] + z*LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS/2};
        // if chunk is not meshed or mesh doesn't have vertices, skip
        if (
            chunk->mesh[mesh_id] == NULL || chunk->mesh[mesh_id]->vertex_translucent == NULL
            || raise2(chunk->pos[0] + x*LEINAD_MESH_RADIUS - data->pos.x) + raise2(chunk->pos[1] + y*LEINAD_MESH_RADIUS  - data->pos.y) + raise2(chunk->pos[2]+ z*LEINAD_MESH_RADIUS  - data->pos.z) > raise2(LEINAD_REGION_RADIUS)
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[0]) < data->fov_planes[0].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[1]) < data->fov_planes[1].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[2]) < data->fov_planes[2].w
            || vec3_dot(current_mesh_pos,*(vec3*)&data->fov_planes[3]) < data->fov_planes[3].w
        ) continue;

        SDL_PushGPUVertexUniformData(data->command_buffer, 1, chunk->pos, 3* sizeof(float));
        SDL_BindGPUVertexBuffers(data->renderpass, 0, &(SDL_GPUBufferBinding){.buffer = chunk->mesh[mesh_id]->vertex_translucent, .offset = 0 },1);
        SDL_BindGPUIndexBuffer(data->renderpass, &(SDL_GPUBufferBinding){ .buffer = chunk->mesh[mesh_id]->index_translucent, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(data->renderpass, chunk->mesh[mesh_id]->ind_t, 1, 0, 0, 0);

    }

}
#pragma once

#include <SDL3/SDL.h>

#include "../libs/bit_manipulation.h"

#include "block.h"



// REGIONS

#define LEINAD_REGION_RADIUS 128


/**
 * Struct that defines how the data inside a region file is stored.
 * Such files, contain the block data of the region in a recursive manner,
 * if every block in a subregion has the same data, it only saves the data
 * once, otherwise, it subdivides in 8 equal sized cubical regions and tries
 * again, until it reaches 2x2x2, when it saves them directly.
 * Each region gets divided into 2x2x2 subregions and those subregions are
 * sorted from -x to +x, then -z to +z and then -y to +y, just like non-grouped
 * blocks.
 */
typedef struct leinad_region {

    #define mask_amount_64x 0b0000000000000000000000000000000000000000000000000000000000000111
    #define mask_amount_32x 0b0000000000000000000000000000000000000000000000000000000111111000
    #define mask_amount_16x 0b0000000000000000000000000000000000000000000000111111111000000000
    #define mask_amount_08x 0b0000000000000000000000000000000000111111111111000000000000000000
    #define mask_amount_04x 0b0000000000000000000111111111111111000000000000000000000000000000
    #define mask_amount_02x 0b0111111111111111111000000000000000000000000000000000000000000000
    // contains the amount of FILLED subregions that each level has, if
    //  full, ignore. Data is stored in multiples of 3 bits:
    // 3bits, 6bits, 9bits, 12bits, ... until adding up to 63
    Uint64 amounts_perLODlevel;


    #define is_full_region 0x01000000
    #define undef1 0x02000000
    #define undef2 0x04000000
    #define undef3 0x08000000
    #define undef4 0x10000000
    #define undef5 0x20000000
    #define undef6 0x40000000
    #define undef7 0x80000000
    // 24 remaining flags ...

    Uint32 ctrl_data;

    // tree-like organised, each byte represents the flags for its subregions
    // [breadth first]
    // all the filled subregions must be ignored
    Uint8 redirection_flags[0b1001001001001];


    // size depends on the specific contents of the region
    struct blockdata region_data[];

} leinad_region_t;


// CHUNKS

typedef struct leinad_chunk {
    struct blockdata block[LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS];
} leinad_chunk_t;



// FUNCTIONS

LEINAD_FCOMPARATOR int leinad_blockdata_comparator(void* a, void* b) {
    struct blockdata *aa = a, *bb = b;
    int result;

    result = aa->id - bb->id;

    if (result) return result;

    result = aa->custom_data - bb->custom_data;
    
    if (result) return result;

    result = aa->rotation_n_subpos - bb->rotation_n_subpos;
    
    return result;

}

void leinad_blockdata_clone(struct blockdata src, struct blockdata* dst) {
    dst->id = src.id;
    dst->rotation_n_subpos = src.rotation_n_subpos;
    dst->custom_data = src.custom_data;
}



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
    Uint32 index;

    // 128x128x128
    if (region->ctrl_data & is_full_region) {
        // (only exists 1 entry)
        index = 0;
    }
    else {
        offset = 0;
        check = ((x &64) >> 4) | ((z &64) >> 5) | ((y &64) >> 6);

        GETMASKS(umask, pmask, check);

        // 64x64x64
        if (region->redirection_flags[offset] & umask) {
            // subregion is full, so data is at this level

            // get the index of this region
            index =
                count_set_bits(region->redirection_flags[offset] & pmask)
            ;
        }
        else {
            offset = 1 + check;
            check = ((x &32) >> 3) | ((z &32) >> 4) | ((y &32) >> 5);

            GETMASKS(umask, pmask, check);

            // 32x32x32
            if (region->redirection_flags[offset] & umask) {
                // subregion is full, so data is at this level

                // get the index of this region
                index =
                    (region->amounts_perLODlevel & mask_amount_64x)
                    + count_set_bits(region->redirection_flags[offset] & pmask)
                ;

                for (offset_i = 0b1; offset_i < offset; offset_i++) {
                    index += count_set_bits(region->redirection_flags[offset_i]);                
                }
            }
            else {
                offset = (offset <<3) +1 + check;
                check = ((x &16) >> 2) | ((z &16) >> 3) | ((y &16) >> 4);

                GETMASKS(umask, pmask, check);

                // 16x16x16
                if (region->redirection_flags[offset] & umask) {
                    // subregion is full, so data is at this level

                    // get the index of this region
                    index =
                           (region->amounts_perLODlevel & mask_amount_64x)
                        + ((region->amounts_perLODlevel & mask_amount_32x) >> 3)
                        + count_set_bits(region->redirection_flags[offset] & pmask)
                    ;

                    for (offset_i = 0b1001; offset_i < offset; offset_i++) {
                        index += count_set_bits(region->redirection_flags[offset_i]);                
                    }
                }
                // reiterates                
                else {
                    offset = (offset <<3) +1 + check;
                    check = ((x &8) >> 1) | ((z &8) >> 2) | ((y &8) >> 3);

                    GETMASKS(umask, pmask, check);

                    // 8x8x8
                    if (region->redirection_flags[offset] & umask) {
                        // subregion is full, so data is at this level

                        // get the index of this region
                        index =
                               (region->amounts_perLODlevel & mask_amount_64x)
                            + ((region->amounts_perLODlevel & mask_amount_32x) >> 3)
                            + ((region->amounts_perLODlevel & mask_amount_16x) >> 9)
                            + count_set_bits(region->redirection_flags[offset] & pmask)
                        ;

                        for (offset_i = 0b1001001; offset_i < offset; offset_i++) {
                            index += count_set_bits(region->redirection_flags[offset_i]);                
                        }
                    }
                    
                    else {
                        offset = (offset <<3) +1 + check;
                        check = ((x &4) >> 0) | ((z &4) >> 1) | ((y &4) >> 2);

                        GETMASKS(umask, pmask, check);

                        // 4x4x4
                        if (region->redirection_flags[offset] & umask) {
                            // subregion is full, so data is at this level

                            // get the index of this region
                            index =
                                   (region->amounts_perLODlevel & mask_amount_64x)
                                + ((region->amounts_perLODlevel & mask_amount_32x) >> 3)
                                + ((region->amounts_perLODlevel & mask_amount_16x) >> 9)
                                + ((region->amounts_perLODlevel & mask_amount_08x) >> 18)
                                + count_set_bits(region->redirection_flags[offset] & pmask)
                            ;

                            for (offset_i = 0b1001001001 ; offset_i < offset; offset_i++) {
                                index += count_set_bits(region->redirection_flags[offset_i]);                
                            }
                        }
                        
                        else {
                            offset = (offset <<3) +1 + check;
                            check = ((x &2) << 1) | ((z &2) >> 0) | ((y &2) >> 1);

                            GETMASKS(umask, pmask, check);

                            // 2x2x2
                            if (region->redirection_flags[offset] & umask) {
                                // subregion is full, so data is at this level

                                // get the index of this region
                                index =
                                       (region->amounts_perLODlevel & mask_amount_64x)
                                    + ((region->amounts_perLODlevel & mask_amount_32x) >> 3)
                                    + ((region->amounts_perLODlevel & mask_amount_16x) >> 9)
                                    + ((region->amounts_perLODlevel & mask_amount_08x) >> 18)
                                    + ((region->amounts_perLODlevel & mask_amount_04x) >> 30)
                                    + count_set_bits(region->redirection_flags[offset] & pmask)
                                ;

                                for (offset_i = 0b1001001001001 ; offset_i < offset; offset_i++) {
                                    index += count_set_bits(region->redirection_flags[offset_i]);                
                                }
                            }
                            else {
                                // AQUI HAY QUE ELEGIR POSICION BASANDOSE EN QUE LOS 8 ESTAN EN LINEA
                                
                                // get the index of this region
                                index =
                                       (region->amounts_perLODlevel & mask_amount_64x)
                                    + ((region->amounts_perLODlevel & mask_amount_32x) >> 3)
                                    + ((region->amounts_perLODlevel & mask_amount_16x) >> 9)
                                    + ((region->amounts_perLODlevel & mask_amount_08x) >> 18)
                                    + ((region->amounts_perLODlevel & mask_amount_04x) >> 30)
                                    + ((region->amounts_perLODlevel & mask_amount_02x) >> 45)
                                    + (((x &1) << 2) | ((z &1) << 1) | ((y &1) << 0)) // index of the 2x2x2
                                ;

                                for (offset_i = 0b1001001001001 ; offset_i < offset; offset_i++) {
                                    index += count_set_bits(~region->redirection_flags[offset_i]);                
                                }
                            }
                            
                        }
                        
                    }
                    
                }
                
            }

            
        }

    }

    // fill in the data and return
    data.id = region->region_data[index].id;
    if (block_has_custom_placement(data.id)) 
        data.rotation_n_subpos = region->region_data[index].rotation_n_subpos;
    if (block_has_custom_data(data.id)) 
        data.custom_data = region->region_data[index].custom_data;
    return data;
}

LEINAD_FBUILDER leinad_chunk_t* leinad_chunk_create() {
    leinad_chunk_t* chunk;
    Uint32 i;

    chunk = SDL_malloc(sizeof(leinad_chunk_t));

    if (chunk == NULL) return NULL;
    
    for(i = 0; i< LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS; i++) {
        chunk->block[i] = (struct blockdata){.id = LEINAD_BLOCK_default, .rotation_n_subpos = 0, .custom_data = 0};
    }

    return chunk;
}

/**
 * @todo
 * attention citizens! THIS CODE CAN BE VERY HEAVILY OPTIMIZED, LIKE, CRAZY
 *  OPTIMIZED, IF I HAD A NICKEL FOR EVERY OPTIMIZATION THIS CODE COULD
 *  HAVE, I WOULD HAVE MORE THAN, IDK, 4 NICKELS, WHICH ARE A LOT
 *  PLEASE DONT FORGET THIS CODE PLEASE I BEG OF YOU :sob:
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
    for (yyyyyyy = 0; yyyyyyy < 2; yyyyyyy++)
    for (zzzzzzz = 0; zzzzzzz < 2; zzzzzzz++)
    for (xxxxxxx = 0; xxxxxxx < 2; xxxxxxx++)
        // iterates over a 64x64x64
        for (yyyyyy = yyyyyyy*2; yyyyyy< (yyyyyyy+1)*2; yyyyyy++)
        for (zzzzzz = zzzzzzz*2; zzzzzz< (zzzzzzz+1)*2; zzzzzz++)
        for (xxxxxx = xxxxxxx*2; xxxxxx< (xxxxxxx+1)*2; xxxxxx++)
            // iterates over a 32x32x32
            for (yyyyy = yyyyyy*2; yyyyy< (yyyyyy+1)*2; yyyyy++)
            for (zzzzz = zzzzzz*2; zzzzz< (zzzzzz+1)*2; zzzzz++)
            for (xxxxx = xxxxxx*2; xxxxx< (xxxxxx+1)*2; xxxxx++)
                // iterates over a 16x16x16
                for (yyyy = yyyyy*2; yyyy< (yyyyy+1)*2; yyyy++)
                for (zzzz = zzzzz*2; zzzz< (zzzzz+1)*2; zzzz++)
                for (xxxx = xxxxx*2; xxxx< (xxxxx+1)*2; xxxx++)
                    // iterates over a 8x8x8
                    for (yyy = yyyy*2; yyy< (yyyy+1)*2; yyy++)
                    for (zzz = zzzz*2; zzz< (zzzz+1)*2; zzz++)
                    for (xxx = xxxx*2; xxx< (xxxx+1)*2; xxx++)
                        // iterates over a 4x4x4
                        for (yy = yyy*2; yy< (yyy+1)*2; yy++)
                        for (zz = zzz*2; zz< (zzz+1)*2; zz++)
                        for (xx = xxx*2; xx< (xxx+1)*2; xx++)
                            // iterates over a 2x2x2
                            for (y = yy*2; y< (yy+1)*2; y++)
                            for (z = zz*2; z< (zz+1)*2; z++)
                            for (x = xx*2; x< (xx+1)*2; x++)
                                // works on the 1x1x1
                                {chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x] = leinad_region_getblock(x,y,z, region);}
    
  }
    return chunk;
}



LEINAD_FBUILDER leinad_region_t* leinad_region_create_from_chunk(leinad_chunk_t* chunk){
    leinad_region_t* result = NULL;
    struct blockdata checking_block = {.id = LEINAD_BLOCK_invalid, .rotation_n_subpos = 0, .custom_data = 0};
    int aux_count = 0;
    Uint32 full_region_check = 0;

    LEINAD_AUX struct blockdata* aux_subregion_02x02x02map = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8*8*8);
    LEINAD_AUX struct blockdata* aux_subregion_04x04x04map = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8*8);
    LEINAD_AUX struct blockdata* aux_subregion_08x08x08map = SDL_malloc(sizeof(struct blockdata) * 8*8*8*8);
    LEINAD_AUX struct blockdata* aux_subregion_16x16x16map = SDL_malloc(sizeof(struct blockdata) * 8*8*8);
    LEINAD_AUX struct blockdata* aux_subregion_32x32x32map = SDL_malloc(sizeof(struct blockdata) * 8*8);
    LEINAD_AUX struct blockdata* aux_subregion_64x64x64map = SDL_malloc(sizeof(struct blockdata) * 8);

    LEINAD_AUX Uint8* aux_subregion_02x02x02check = SDL_malloc(sizeof(Uint8) * 1*8*8*8*8*8);
    LEINAD_AUX Uint8* aux_subregion_04x04x04check = SDL_malloc(sizeof(Uint8) * 1*8*8*8*8);
    LEINAD_AUX Uint8* aux_subregion_08x08x08check = SDL_malloc(sizeof(Uint8) * 1*8*8*8);
    LEINAD_AUX Uint8* aux_subregion_16x16x16check = SDL_malloc(sizeof(Uint8) * 1*8*8);
    LEINAD_AUX Uint8* aux_subregion_32x32x32check = SDL_malloc(sizeof(Uint8) * 1*8);
    LEINAD_AUX Uint8* aux_subregion_64x64x64check = SDL_malloc(sizeof(Uint8) * 1);

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
                        for (yy = yyy*2; yy< (yyy+1)*2; yy++) for (zz = zzz*2; zz< (zzz+1)*2; zz++) for (xx = xxx*2; xx< (xxx+1)*2; xx++) {
                            // iterates over a 2x2x2

                            // get first block
                            leinad_blockdata_clone(checking_block,&chunk->block[yy*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + zz*LEINAD_REGION_RADIUS + xx]);

                            // compare with the rest

                            // if different than any, don't set the byte
                            if (
                                leinad_blockdata_comparator(&checking_block, &(chunk->block[yy*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + zz*LEINAD_REGION_RADIUS + xx+1]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[yy*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (zz+1)*LEINAD_REGION_RADIUS + xx]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[yy*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (zz+1)*LEINAD_REGION_RADIUS + xx+1]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[(yy+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + zz*LEINAD_REGION_RADIUS + xx]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[(yy+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + zz*LEINAD_REGION_RADIUS + xx+1]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[(yy+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (zz+1)*LEINAD_REGION_RADIUS + xx]))
                             || leinad_blockdata_comparator(&checking_block, &(chunk->block[(yy+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (zz+1)*LEINAD_REGION_RADIUS + xx+1]))
                            ) {
                                aux_subregion_02x02x02map[aux_count/8].id = LEINAD_BLOCK_invalid;
                                aux_subregion_02x02x02map[aux_count/8].rotation_n_subpos = 0;
                                aux_subregion_02x02x02map[aux_count/8].custom_data = 0;
                            } else {
                                // set bit
                                switch (aux_count % 8) {
                                    case 7: aux_subregion_02x02x02check[aux_count/8] |= 0b00000001; break; \
                                    case 6: aux_subregion_02x02x02check[aux_count/8] |= 0b00000010; break; \
                                    case 5: aux_subregion_02x02x02check[aux_count/8] |= 0b00000100; break; \
                                    case 4: aux_subregion_02x02x02check[aux_count/8] |= 0b00001000; break; \
                                    case 3: aux_subregion_02x02x02check[aux_count/8] |= 0b00010000; break; \
                                    case 2: aux_subregion_02x02x02check[aux_count/8] |= 0b00100000; break; \
                                    case 1: aux_subregion_02x02x02check[aux_count/8] |= 0b01000000; break; \
                                    case 0: aux_subregion_02x02x02check[aux_count/8] |= 0b10000000; break; \
                                }

                                // set block
                                aux_subregion_02x02x02map[aux_count/8] = checking_block;
                            }
                            aux_count++;
                        }

  }

  { // iterate over the result of the 2x2x2 grouping, checking the 4x4x4 regions 
    Uint32 i;

    // iterates over a 4x4x4
    for (i = 0; i < 64*64*64; i+=8) {
        // get first block
        leinad_blockdata_clone(aux_subregion_02x02x02map[i],&checking_block);

        // compare with the rest

        // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
        if (
            checking_block.id == LEINAD_BLOCK_invalid
            || (( 
                leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+1]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+2]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+3]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+4]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+5]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+6]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_02x02x02map[i+7]))
                ) || aux_subregion_02x02x02check[i/8] != 0xFF)
        ) {
            aux_subregion_04x04x04map[i/8].id = LEINAD_BLOCK_invalid;
            aux_subregion_04x04x04map[i/8].rotation_n_subpos = 0;
            aux_subregion_04x04x04map[i/8].custom_data = 0;
        } else {

            // set bit
            switch ((i/8) % 8) {
                case 7: aux_subregion_04x04x04check[i/64] |= 0b00000001; break; \
                case 6: aux_subregion_04x04x04check[i/64] |= 0b00000010; break; \
                case 5: aux_subregion_04x04x04check[i/64] |= 0b00000100; break; \
                case 4: aux_subregion_04x04x04check[i/64] |= 0b00001000; break; \
                case 3: aux_subregion_04x04x04check[i/64] |= 0b00010000; break; \
                case 2: aux_subregion_04x04x04check[i/64] |= 0b00100000; break; \
                case 1: aux_subregion_04x04x04check[i/64] |= 0b01000000; break; \
                case 0: aux_subregion_04x04x04check[i/64] |= 0b10000000; break; \
            }

            // set block
            aux_subregion_04x04x04map[i/8] = checking_block;
        }
    }
  }

  { // iterate over the result of the 4x4x4 grouping, checking the 8x8x8 regions 
    Uint32 i;

    // iterates over a 8x8x8
    for (i = 0; i < 32*32*32; i+=8) {
        // get first block
        leinad_blockdata_clone(aux_subregion_04x04x04map[i],&checking_block);

        // compare with the rest

        // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
        if (
            checking_block.id == LEINAD_BLOCK_invalid
            || (( 
                leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+1]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+2]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+3]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+4]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+5]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+6]))
            || leinad_blockdata_comparator(&checking_block, &(aux_subregion_04x04x04map[i+7]))
                ) || aux_subregion_04x04x04check[i/8] != 0xFF)
        ) {
            aux_subregion_08x08x08map[i/8].id = LEINAD_BLOCK_invalid;
            aux_subregion_08x08x08map[i/8].rotation_n_subpos = 0;
            aux_subregion_08x08x08map[i/8].custom_data = 0;
        } else {

            // set bit
            switch ((i/8) % 8) {
                case 7: aux_subregion_08x08x08check[i/64] |= 0b00000001; break; \
                case 6: aux_subregion_08x08x08check[i/64] |= 0b00000010; break; \
                case 5: aux_subregion_08x08x08check[i/64] |= 0b00000100; break; \
                case 4: aux_subregion_08x08x08check[i/64] |= 0b00001000; break; \
                case 3: aux_subregion_08x08x08check[i/64] |= 0b00010000; break; \
                case 2: aux_subregion_08x08x08check[i/64] |= 0b00100000; break; \
                case 1: aux_subregion_08x08x08check[i/64] |= 0b01000000; break; \
                case 0: aux_subregion_08x08x08check[i/64] |= 0b10000000; break; \
            }

            // set block
            aux_subregion_08x08x08map[i/8] = checking_block;
        }
    }
  }

  { // iterate over the result of the 8x8x8 grouping, checking the 16x16x16 regions 
    Uint32 i;

    // iterates over a 16x16x16
    for (i = 0; i < 16*16*16; i+=8) {
        // get first block
        leinad_blockdata_clone(aux_subregion_08x08x08map[i],&checking_block);

                // compare with the rest

                // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
                if (
                    checking_block.id == LEINAD_BLOCK_invalid
                    || (( 
                       leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+1]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+2]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+3]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+4]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+5]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+6]))
                    || leinad_blockdata_comparator(&checking_block, &(aux_subregion_08x08x08map[i+7]))
                        ) || aux_subregion_08x08x08check[i/8] != 0xFF)
                ) {
                    aux_subregion_16x16x16map[i/8].id = LEINAD_BLOCK_invalid;
                    aux_subregion_16x16x16map[i/8].rotation_n_subpos = 0;
                    aux_subregion_16x16x16map[i/8].custom_data = 0;
                } else {

                    // set bit
                    switch ((i/8) % 8) {
                        case 7: aux_subregion_16x16x16check[i/64] |= 0b00000001; break; \
                        case 6: aux_subregion_16x16x16check[i/64] |= 0b00000010; break; \
                        case 5: aux_subregion_16x16x16check[i/64] |= 0b00000100; break; \
                        case 4: aux_subregion_16x16x16check[i/64] |= 0b00001000; break; \
                        case 3: aux_subregion_16x16x16check[i/64] |= 0b00010000; break; \
                        case 2: aux_subregion_16x16x16check[i/64] |= 0b00100000; break; \
                        case 1: aux_subregion_16x16x16check[i/64] |= 0b01000000; break; \
                        case 0: aux_subregion_16x16x16check[i/64] |= 0b10000000; break; \
                    }

                    // set block
                    aux_subregion_16x16x16map[i/8] = checking_block;
                }
            }
  }

  { // iterate over the result of the 16x16x16 grouping, checking the 32x32x32 regions 
    Uint32 i;

    // iterates over a 32x32x32
    for (i = 0; i < 8*8*8; i+=8) {
        // get first block
        leinad_blockdata_clone(aux_subregion_16x16x16map[i],&checking_block);

            // compare with the rest

            // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
            if (
                checking_block.id == LEINAD_BLOCK_invalid
                || (( 
                   leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+1]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+2]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+3]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+4]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+5]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+6]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_16x16x16map[i+7]))
                    ) || aux_subregion_16x16x16check[i/8] != 0xFF)
            ) {
                aux_subregion_32x32x32map[i/8].id = LEINAD_BLOCK_invalid;
                aux_subregion_32x32x32map[i/8].rotation_n_subpos = 0;
                aux_subregion_32x32x32map[i/8].custom_data = 0;
            } else {

                // set bit
                switch ((i/8) % 8) {
                    case 7: aux_subregion_32x32x32check[i/64] |= 0b00000001; break; \
                    case 6: aux_subregion_32x32x32check[i/64] |= 0b00000010; break; \
                    case 5: aux_subregion_32x32x32check[i/64] |= 0b00000100; break; \
                    case 4: aux_subregion_32x32x32check[i/64] |= 0b00001000; break; \
                    case 3: aux_subregion_32x32x32check[i/64] |= 0b00010000; break; \
                    case 2: aux_subregion_32x32x32check[i/64] |= 0b00100000; break; \
                    case 1: aux_subregion_32x32x32check[i/64] |= 0b01000000; break; \
                    case 0: aux_subregion_32x32x32check[i/64] |= 0b10000000; break; \
                }

                // set block
                aux_subregion_32x32x32map[i/8] = checking_block;
            }
        }
  }

  { // iterate over the result of the 32x32x32 grouping, checking the 64x64x64 regions 
    Uint32 i;

    // iterates over a 64x64x64
    for (i = 0; i < 4*4*4; i+=8) {
        // get first block
        leinad_blockdata_clone(aux_subregion_32x32x32map[i],&checking_block);

            // compare with the rest

            // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the byte
            if (
                checking_block.id == LEINAD_BLOCK_invalid
                || (( 
                   leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+1]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+2]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+3]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+4]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+5]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+6]))
                || leinad_blockdata_comparator(&checking_block, &(aux_subregion_32x32x32map[i+7]))
                    ) || aux_subregion_32x32x32check[i/8] != 0xFF)
            ) {
                aux_subregion_64x64x64map[i/8].id = LEINAD_BLOCK_invalid;
                aux_subregion_64x64x64map[i/8].rotation_n_subpos = 0;
                aux_subregion_64x64x64map[i/8].custom_data = 0;
            } else {

                // set bit
                switch ((i/8) % 8) {
                    case 7: aux_subregion_64x64x64check[i/64] |= 0b00000001; break; \
                    case 6: aux_subregion_64x64x64check[i/64] |= 0b00000010; break; \
                    case 5: aux_subregion_64x64x64check[i/64] |= 0b00000100; break; \
                    case 4: aux_subregion_64x64x64check[i/64] |= 0b00001000; break; \
                    case 3: aux_subregion_64x64x64check[i/64] |= 0b00010000; break; \
                    case 2: aux_subregion_64x64x64check[i/64] |= 0b00100000; break; \
                    case 1: aux_subregion_64x64x64check[i/64] |= 0b01000000; break; \
                    case 0: aux_subregion_64x64x64check[i/64] |= 0b10000000; break; \
                }

                // set block
                aux_subregion_64x64x64map[i/8] = checking_block;
            }
        }
  }

  { // iterate over the result of the 64x64x64 grouping, checking the whole 128x128x128 region 
    // iterates over a 128x128x128
    // get first block
        leinad_blockdata_clone(aux_subregion_64x64x64map[0],&checking_block);

    // compare with the rest
    // if different than any, AND ANY SUBREGION IS NOT EQUAL don't set the flag
    if (
        checking_block.id == LEINAD_BLOCK_invalid
        || (( 
            leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[1]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[2]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[3]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[4]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[5]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[6]))
        || leinad_blockdata_comparator(&checking_block, &(aux_subregion_64x64x64map[7]))
            ) || aux_subregion_64x64x64check[0] != 0xFF)
    ) {
                aux_subregion_64x64x64map[0].id = LEINAD_BLOCK_invalid;
                aux_subregion_64x64x64map[0].rotation_n_subpos = 0;
                aux_subregion_64x64x64map[0].custom_data = 0;
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
            sizeof(Uint64)                      // count of blocks per level
            + sizeof(Uint32)                    // region data
            + sizeof(Uint8[0b1001001001001])    // redirection flags
            + sizeof(struct blockdata) * 1      // block data
        );
        result->ctrl_data = 
            is_full_region
        ;
        result->amounts_perLODlevel = 0;

        SDL_memset(result->redirection_flags,0,0b1001001001001);

        result->region_data[0] = checking_block;

    }
    // region con mas de un tipo de bloque, debe dividirse
    else {
        Uint32 i;
        Uint64 per_level_count = 0, total_block_count = 0, aux = 0;;

        // clear sublevels when a higher one is available (by checking bits)

      { // 64x64x64 checks
        aux = 0;
        for (i = 0; i < 8; i++) {
            if (aux_subregion_64x64x64check[i/8] & (0b10000000 >> (i % 8))) {
                SDL_memset(&aux_subregion_32x32x32check[i],0,1);
                SDL_memset(&aux_subregion_16x16x16check[2*2*2*i],0,2*2*2);
                SDL_memset(&aux_subregion_08x08x08check[4*4*4*i],0,4*4*4);
                SDL_memset(&aux_subregion_04x04x04check[8*8*8*i],0,8*8*8);
                SDL_memset(&aux_subregion_02x02x02check[16*16*16*i],0,16*16*16);
                aux++;
            }
        }
        SDL_Log("%lu 64x regions",aux);

        per_level_count += aux << 0;
        total_block_count += aux;
      }

      { // 32x32x32 checks
        aux = 0;
        for (i = 0; i < 8 * 8; i++) {
            if (aux_subregion_32x32x32check[i/8] & (0b10000000 >> (i % 8))) {
                SDL_memset(&aux_subregion_16x16x16check[i],0,1);
                SDL_memset(&aux_subregion_08x08x08check[2*2*2*i],0,2*2*2);
                SDL_memset(&aux_subregion_04x04x04check[4*4*4*i],0,4*4*4);
                SDL_memset(&aux_subregion_02x02x02check[8*8*8*i],0,8*8*8);
                aux++;
            }
        }
        SDL_Log("%lu 32x regions",aux);


        per_level_count += aux << 3;
        total_block_count += aux;
      }

      { // 16x16x16 checks
        aux = 0;
        for (i = 0; i < 8 * 8 * 8; i++) {
            if (aux_subregion_16x16x16check[i/8] & (0b10000000 >> (i % 8))) {
                SDL_memset(&aux_subregion_08x08x08check[i],0,1);
                SDL_memset(&aux_subregion_04x04x04check[2*2*2*i],0,2*2*2);
                SDL_memset(&aux_subregion_02x02x02check[4*4*4*i],0,4*4*4);
                aux++;
            }
        }
        SDL_Log("%lu 16x regions",aux);

        per_level_count += aux << (3+6);
        total_block_count += aux;
      }

      { // 8x8x8 checks
        aux = 0;
        for (i = 0; i < 8 * 8 * 8 * 8; i++) {
            if (aux_subregion_08x08x08check[i/8] & (0b10000000 >> (i % 8))) {
                SDL_memset(&aux_subregion_04x04x04check[i],0,1);
                SDL_memset(&aux_subregion_02x02x02check[2*2*2*i],0,2*2*2);
                aux++;
            }
        }
        SDL_Log("%lu 08x regions",aux);

        per_level_count += aux << (3+6+9);
        total_block_count += aux;
      }

      { // 4x4x4 checks
        aux = 0;
        for (i = 0; i < 8 * 8 * 8 * 8 * 8; i++) {
            if (aux_subregion_04x04x04check[i/8] & (0b10000000 >> (i % 8))) {
                SDL_memset(&aux_subregion_02x02x02check[i],0,1);
                aux++;
            }
        }
        SDL_Log("%lu 04x regions",aux);

        per_level_count += aux << (3+6+9+12);
        total_block_count += aux;
      }

      { // 2x2x2 checks
        aux = 0;
        for (i = 0; i < 8 * 8 * 8 * 8 * 8; i++) {
            if (aux_subregion_02x02x02check[i/8] & (0b10000000 >> (i % 8))) {
                aux++;
            }
        }
        SDL_Log("%lu 02x regions",aux);

        per_level_count += aux << (3+6+9+12+15);
        total_block_count += aux;
      }

      { // GET THE AMOUNT OF THE REMAINING 1x1x1 TO COVER
        aux = LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS;
        SDL_Log("aux: %lu, binary lvl count: %lx",aux,per_level_count);
        
        aux-= (((per_level_count) & mask_amount_64x) >> (0)) * 64*64*64;
        SDL_Log("aux: %lu, count64: %lu",aux,(((per_level_count) & mask_amount_64x) >> (0)));

        aux-= (((per_level_count) & mask_amount_32x) >> (3)) * 32*32*32;
        SDL_Log("aux: %lu, count32: %lu",aux,(((per_level_count) & mask_amount_32x) >> (3)));

        aux-= (((per_level_count) & mask_amount_16x) >> (3+6)) *16*16*16;
        SDL_Log("aux: %lu, count16: %lu",aux,(((per_level_count) & mask_amount_16x) >> (3+6)));
  
        aux-= (((per_level_count) & mask_amount_08x) >> (3+6+9)) * 8*8*8;
        SDL_Log("aux: %lu, count8: %lu",aux,(((per_level_count) & mask_amount_08x) >> (3+6+9)));
        
        aux-= (((per_level_count) & mask_amount_04x) >> (3+6+9+12)) * 4*4*4;
        SDL_Log("aux: %lu, count4: %lu",aux,(((per_level_count) & mask_amount_04x) >> (3+6+9+12)));

        aux-= (((per_level_count) & mask_amount_02x) >> (3+6+9+12+15)) * 2*2*2;
        SDL_Log("aux: %lu, count2: %lu",aux,(((per_level_count) & mask_amount_02x) >> (3+6+9+12+15)));
        
        total_block_count -= aux;
      }             /// EL PROBLEMA ES QUE AQUI LA SUMA SE HACE MAL ^^^^^

        // everything counted, so now its just allocating the region and
        // filling in the data
        // I'm almost free :')

        // 0x7891 = 0b0111100010010001
        //          64x 0b001
        //          32x 0b001
        //          16x 0b001
                          

        result = SDL_malloc( 0 +
            sizeof(Uint64)                                  // count of blocks per level
            + sizeof(Uint32)                                // region data
            + sizeof(Uint8[0b1001001001001001])             // redirection flags
            + sizeof(struct blockdata) * total_block_count  //block data
        );
        
        SDL_Log("%x\n",result);
        result->amounts_perLODlevel = per_level_count;

        SDL_memcpy(result->redirection_flags,&aux_subregion_64x64x64check[0],1);
        SDL_memcpy(result->redirection_flags + 0b1,&aux_subregion_32x32x32check[0],1*8);
        SDL_memcpy(result->redirection_flags + 0b1001,&aux_subregion_16x16x16check[0],1*8*8);
        SDL_memcpy(result->redirection_flags + 0b1001001,&aux_subregion_08x08x08check[0],1*8*8*8);
        SDL_memcpy(result->redirection_flags + 0b1001001001,&aux_subregion_04x04x04check[0],1*8*8*8*8);
        SDL_memcpy(result->redirection_flags + 0b1001001001001,&aux_subregion_02x02x02check[0],1*8*8*8*8*8);

        // aux will now be used as the current offset when inserting data
        aux = 0;

      { // map 64x64x64
        for (i = 0; i < 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_64x64x64check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_64x64x64map[i];
                aux++;
            }
        }
      }
      { // map 32x32x32
        for (i = 0; i < 8 * 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_32x32x32check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_32x32x32map[i];
                aux++;
            }
        }
      }
      { // map 16x16x16
        for (i = 0; i < 8 * 8 * 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_16x16x16check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_16x16x16map[i];
                aux++;
            }
        }
      }
      { // map 8x8x8
        for (i = 0; i < 8 * 8 * 8 * 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_08x08x08check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_08x08x08map[i];
                aux++;
            }
        }
      }
      { // map 4x4x4
        for (i = 0; i < 8 * 8 * 8 * 8 * 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_04x04x04check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_04x04x04map[i];
                aux++;
            }
        }
      }
      { // map 2x2x2 // HEY, DONT FORGET TO COPY THE EMPTY ONES AT THE END!
        for (i = 0; i < 8 * 8 * 8 * 8 * 8 * 8; i++) {
            // if flag is set, insert the block into the array
            if (aux_subregion_02x02x02check[i/8] & (0b10000000 >> (i % 8))) {
                result->region_data[aux] = aux_subregion_02x02x02map[i];
                aux++;
            }
        }

        for (i = 0; i < 8 * 8 * 8 * 8 * 8 * 8; i++) {
            Uint32 x, y, z;


            // if flag is unset, insert the 8 block group into the array
            if (
                (!aux_subregion_02x02x02check[i/8]) & (0b10000000 >> (i % 8)) // 0 at 2x2x2
             && (!aux_subregion_04x04x04check[i/(8*8)]) & (0b10000000 >> (i/(8) % 8)) // 0 at 4x4x4
             && (!aux_subregion_08x08x08check[i/(8*8*8)]) & (0b10000000 >> (i/(8*8) % 8)) // 0 at 8x8x8
             && (!aux_subregion_64x64x64check[i/(8*8*8*8)]) & (0b10000000 >> (i/(8*8*8) % 8)) // 0 at 16x16x16
             && (!aux_subregion_64x64x64check[i/(8*8*8*8*8)]) & (0b10000000 >> (i/(8*8*8*8) % 8)) // 0 at 32x32x32
             && (!aux_subregion_64x64x64check[i/(8*8*8*8*8*8)]) & (0b10000000 >> (i/(8*8*8*8*8) % 8)) // 0 at 64x64x64
            ) {
                x = (i & 0b1 + ((i & 0b1000) >> 2) + ((i & 0b1000000) >> 4) + ((i & 0b1000000000) >> 6) + ((i & 0b1000000000000) >> 8) + ((i & 0b1000000000000000) >> 10));
                y = (((i & 0b10) >> 1) + ((i & 0b1000) >> 3) + ((i & 0b1000000) >> 5) + ((i & 0b1000000000) >> 7) + ((i & 0b1000000000000) >> 9) + ((i & 0b1000000000000000) >> 11));
                z = (((i & 0b100) >> 2) + ((i & 0b1000) >> 4) + ((i & 0b1000000) >> 6) + ((i & 0b1000000000) >> 8) + ((i & 0b1000000000000) >> 10) + ((i & 0b1000000000000000) >> 12));


                result->region_data[aux+0] = chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x];
                result->region_data[aux+1] = chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x+1];
                result->region_data[aux+2] = chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (z+1)*LEINAD_REGION_RADIUS + x];
                result->region_data[aux+3] = chunk->block[y*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (z+1)*LEINAD_REGION_RADIUS + x+1];
                result->region_data[aux+4] = chunk->block[(y+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x];
                result->region_data[aux+5] = chunk->block[(y+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + z*LEINAD_REGION_RADIUS + x+1];
                result->region_data[aux+6] = chunk->block[(y+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (z+1)*LEINAD_REGION_RADIUS + x];
                result->region_data[aux+7] = chunk->block[(y+1)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (z+1)*LEINAD_REGION_RADIUS + x+1];
                aux+=8;
            } 
        }

    }
    }


    SDL_free(aux_subregion_02x02x02check);
    SDL_free(aux_subregion_04x04x04check);
    SDL_free(aux_subregion_08x08x08check);
    SDL_free(aux_subregion_16x16x16check);
    SDL_free(aux_subregion_32x32x32check);
    SDL_free(aux_subregion_64x64x64check);

    SDL_free(aux_subregion_02x02x02map);
    SDL_free(aux_subregion_04x04x04map);
    SDL_free(aux_subregion_08x08x08map);
    SDL_free(aux_subregion_16x16x16map);
    SDL_free(aux_subregion_32x32x32map);
    SDL_free(aux_subregion_64x64x64map);

    return result;
}


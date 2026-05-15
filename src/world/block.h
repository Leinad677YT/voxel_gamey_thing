#pragma once

#include <SDL3/SDL.h>
#include "../data/tags.h"


#include "blockdata.h"


LEINAD_FGET bool block_has_custom_placement(block_id id) {

    return block_data [
        (id >= LEINAD_BLOCK_init && id < LEINAD_BLOCK_end)?
        id : LEINAD_BLOCK_default
    ].flags & LEINAD_BLOCKFLAG_hascustomplacement;
}

LEINAD_FGET bool block_has_custom_data(block_id id) {
    return block_data [
        (id >= LEINAD_BLOCK_init && id < LEINAD_BLOCK_end)?
        id : LEINAD_BLOCK_default
    ].flags & LEINAD_BLOCKFLAG_hascustomplacement;
}

// data gotten by querying a block from a region
struct blockdata {
    Uint16 id;

    // 0xE000 masks rotation (0b1110000000000000)
    // // 1-6 - Up / North / East / South / West / Down 
    Uint16 rotation_n_subpos;
    
    // depends on block type
    Uint32 custom_data; 
};


// REGIONS

#define LEINAD_REGION_RADIUS 128
#define LEINAD_MESH_RADIUS 16

#define leinad_get_chunk_index(x,y,z) ((y)*128*128 + (z)*128 + (x))

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
    Uint8 redirection_flags[0b1001001001001001+3];


    // size depends on the specific contents of the region
    struct blockdata region_data[];

} leinad_region_t;


// CHUNKS

typedef struct leinad_chunk {
    struct blockdata block[LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS];
    struct render_mesh* mesh[LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS * LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS * LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS];
} leinad_chunk_t;

struct block_vertex {
    float x,y,z;
    float u,v;
};
#define LEINAD_BLOCK_RENDER_SCALE 1.f

LEINAD_FCOMPARATOR int leinad_blockdata_comparator(void* a, void* b);

// x,y,z are already modulo'd 128
LEINAD_FGET struct blockdata leinad_region_getblock(int x, int y, int z, leinad_region_t* region);

LEINAD_FBUILDER leinad_chunk_t* leinad_chunk_create();

LEINAD_FINITIALIZER leinad_chunk_t* leinad_chunk_setfromregion(leinad_region_t* region, leinad_chunk_t* chunk);

LEINAD_FBUILDER leinad_region_t* leinad_region_create_from_chunk(leinad_chunk_t* chunk);

LEINAD_FBUILDER leinad_region_t* leinad_region_create_empty();

LEINAD_FINITIALIZER void* leinad_chunk_create_mesh(leinad_chunk_t *chunk, short off_x, short off_y, short off_z);

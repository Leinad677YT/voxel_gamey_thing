#pragma once

#include <SDL3/SDL.h>
#include "../data/tags.h"


#include "blockdata.h"

/**
 * @todo may be better to replace by `has_rotation` and `has_subblock_placement`
 */
LEINAD_FGET bool block_has_custom_placement(block_id id) {

    return block_data [
        (id >= LEINAD_BLOCK_init && id < LEINAD_BLOCK_end)?
        id : LEINAD_BLOCK_default
    ].flags & LEINAD_BLOCKFLAG_hascustomplacement;
}

/**
 * @return `true` if the specified id can hold custom data, `false otherwise`
 * @todo currently irrelevant as all region formats store all data regardless
 */
LEINAD_FGET bool block_has_custom_data(block_id id) {
    return block_data [
        (id >= LEINAD_BLOCK_init && id < LEINAD_BLOCK_end)?
        id : LEINAD_BLOCK_default
    ].flags & LEINAD_BLOCKFLAG_hascustomplacement;
}

/**
 * Struct that specifies the format on which in-world blocks are stored and managed
 */
struct blockdata {
    /**
     * Block id to refer to for static data
     */
    Uint16 id;

    /**
     * Positional data or block-specific flags.  
     * When using positional:
     * - 0xE000 masks rotation (0b111<<13)
     * - // 1-6 - Up / North / East / South / West / Down //
     */
    Uint16 rotation_n_subpos;
    
    /**
     * Depends on block type, may be:
     * - storage index
     * - block flags
     * - blockstates
     * - ... 
     */
    Uint32 custom_data; 
};

/**
 * Struct that defines how vertices of blocks are sent to the gpu
 */
struct block_vertex {
    /**
     * Position on the world
     */
    float x,y,z;

    /**
     * Texture coordinates on the atlas
     */
    float u,v;

    /**
     * Color of the outline effect
     */
    float r,g,b,a;
};

/**
 * Length of 1 block on the renderer, ideally for gameplay, this should be 1,
 * but if it's consistent with all the other elements it should not matter
 */
#define LEINAD_BLOCK_RENDER_SCALE 1.f

LEINAD_FCOMPARATOR int leinad_blockdata_comparator(void* a, void* b);

LEINAD_FINITIALIZER void leinad_blockdata_clone(struct blockdata src, struct blockdata* dst);
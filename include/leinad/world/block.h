#pragma once

#include <SDL3/SDL.h>
#include "../data/tags.h"


#include "blockdata.h"

/**
 * @todo may be better to replace by `has_rotation` and `has_subblock_placement`
 */
LEINAD_FGET bool block_has_custom_placement(block_id_t id) {

    return block_data [
        (id >= LEINAD_BLOCK_init && id < LEINAD_BLOCK_end)?
        id : LEINAD_BLOCK_default
    ].flags & LEINAD_BLOCKFLAG_hascustomplacement;
}

/**
 * @return `true` if the specified id can hold custom data, `false otherwise`
 * @todo currently irrelevant as all region formats store all data regardless
 */
LEINAD_FGET bool block_has_custom_data(block_id_t id) {
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
     * When using positional rotation:
     * - 0xE0 masks rotation (0b111<<13)
     *   - // 1-6 - Up / North / East / South / West / Down //
     */
    Uint8 rotation_n_subpos;
    
    /**
     * Light levels of the block
     *   - 0x000F being blocklight
     *   - 0x00F0 being skylight (ambient light)
     */
    Uint8 light;
    
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

/**
 * Maximum light level a block can have, both for skylight and blocklight
 */
#define LEINAD_BLOCK_MAX_LIGHT_LEVEL 16.f

/**
 * Returns if 2 blocks are identical
 * @return `0` if they are equal, another value otherwise 
 */
LEINAD_FCOMPARATOR int leinad_blockdata_comparator(void* a, void* b);

/**
 * clones the data of one block into another, this is equivalent to `(*dst) = (struct blockdata) src`
 */
LEINAD_FINITIALIZER void leinad_blockdata_clone(struct blockdata src, struct blockdata* dst);

/**
 * @return `true` if the specified id can hold custom data, `false otherwise`
 * @todo currently irrelevant as all region formats store all data regardless
 */
LEINAD_FGET int leinad_get_block_light(struct blockdata block) {
    return LEINAD_BLOCK_MAX_LIGHT_LEVEL;

    return SDL_max(block.light & 0xF,block.light >> 4);
}


#define LEINAD_BLOCK_SHADE_pX 0.05f
#define LEINAD_BLOCK_SHADE_pY 0.02f
#define LEINAD_BLOCK_SHADE_pZ 0.08f

#define LEINAD_BLOCK_SHADE_nX 0.008f
#define LEINAD_BLOCK_SHADE_nY 0.012f
#define LEINAD_BLOCK_SHADE_nZ 0.016f

#pragma once

#include <SDL3/SDL.h>

// generic block data

typedef enum {
    LEINAD_BLOCK_init   = 0,
    LEINAD_BLOCK_STONE = 0,
    LEINAD_BLOCK_AIR,
    LEINAD_BLOCK_GLASS,
    LEINAD_BLOCK_end,
    
    LEINAD_BLOCK_default = LEINAD_BLOCK_end,
    LEINAD_BLOCK_NULL = LEINAD_BLOCK_default,
    LEINAD_BLOCK_amount,

    LEINAD_BLOCK_invalid = 0xFFFF // 
} block_id;

typedef enum {
    LEINAD_BLOCKTX_init   = 0,
    LEINAD_BLOCKTX_STONE = 0,
    LEINAD_BLOCKTX_GLASS,
    // LEINAD_BLOCKTX_debUG1,
    // LEINAD_BLOCKTX_debUG2,
    // LEINAD_BLOCKTX_debUG3,
    LEINAD_BLOCKTX_end,
    
    LEINAD_BLOCKTX_default = LEINAD_BLOCKTX_end,
    LEINAD_BLOCKTX_NULL = LEINAD_BLOCKTX_default,
    LEINAD_BLOCKTX_amount,

    LEINAD_BLOCKTX_invalid = 0xFFFF // 
} block_tx;


struct block { // all share the same atlas so no need to specify texture here
    Uint32 flags;
    #define LEINAD_BLOCKFLAG_default            0b0000000000000000
    #define LEINAD_BLOCKFLAG_isfullblock        0b0000000000000001
    #define LEINAD_BLOCKFLAG_hastransparency    0b0000000000000010
    #define LEINAD_BLOCKFLAG_istransparent      0b0000000000000100
    #define LEINAD_BLOCKFLAG_iscontiguous       0b0000000000001000
    #define LEINAD_BLOCKFLAG_hascustomplacement 0b0000000000010000

    union static_blockdata {

        struct full_single_texture{
            Uint32 tx_index;
            
            Uint8 filler[64 - 1*sizeof(Uint32)];
        } full_single_texture;

        struct full_multiple_texture{ 
            // y, -y, z, -z, x, -x
            Uint32 tx_index[6];
            
            Uint8 filler[64 - 6*sizeof(Uint32)];            
        } full_multiple_texture;
    
    } data;
};


// per-block data

#define leinad_get_block_data(block_id) block_data[block_id]

const struct block block_data[LEINAD_BLOCK_amount] = {
    { // LEINAD_BLOCK_STONE
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_STONE
        }
    },
    { // LEINAD_BLOCK_AIR
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_hastransparency
               | LEINAD_BLOCKFLAG_istransparent,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_invalid
        }
    },
    { // LEINAD_BLOCK_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_hastransparency,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_GLASS
        }
    },
    
    // default
    { // LEINAD_BLOCK_NULL
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_hastransparency
               | LEINAD_BLOCKFLAG_istransparent,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_invalid
        }
    }
};

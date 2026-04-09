#pragma once

#include <SDL3/SDL.h>

// generic block data

typedef enum {
    LEINAD_BLOCK_init   = 0,
    LEINAD_BLOCK_STONE = 0,
    LEINAD_BLOCK_end,
    
    LEINAD_BLOCK_default = LEINAD_BLOCK_end,
    LEINAD_BLOCK_NULL = LEINAD_BLOCK_default,
    LEINAD_BLOCK_amount,

    LEINAD_BLOCK_invalid = 0xFFFF
} block_id;



struct block {
    Uint16 flags;
    #define LEINAD_BLOCKFLAG_default 0x00
    #define LEINAD_BLOCKFLAG_hascustomplacement 0x01
};


// per-block data

const struct block block_data[LEINAD_BLOCK_amount] = {
    { // LEINAD_BLOCK_STONE
        .flags = LEINAD_BLOCKFLAG_default
    },
    
    // default
    { // LEINAD_BLOCK_NULL
        .flags = LEINAD_BLOCKFLAG_default
    }
};

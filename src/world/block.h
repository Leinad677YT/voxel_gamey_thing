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


// AUXILIARY

// data gotten by querying a block from a region
struct blockdata {
    Uint16 id;

    // 0xE000 masks rotation (0b1110000000000000)
    // // 1-6 - Up / North / East / South / West / Down 
    Uint16 rotation_n_subpos;
    
    // depends on block type
    Uint32 custom_data; 
};


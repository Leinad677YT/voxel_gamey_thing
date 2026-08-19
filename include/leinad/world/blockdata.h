#pragma once

#include <SDL3/SDL.h>

// generic block data

typedef enum {
    LEINAD_BLOCK_init   = 0,
    LEINAD_BLOCK_STONE = 0,
    LEINAD_BLOCK_AIR,
    LEINAD_BLOCK_GLASS,
    LEINAD_BLOCK_WHITE_STAINED_GLASS,
    LEINAD_BLOCK_LIGHT_GRAY_STAINED_GLASS,
    LEINAD_BLOCK_GRAY_STAINED_GLASS,
    LEINAD_BLOCK_BLACK_STAINED_GLASS,
    LEINAD_BLOCK_BROWN_STAINED_GLASS,
    LEINAD_BLOCK_RED_STAINED_GLASS,
    LEINAD_BLOCK_ORANGE_STAINED_GLASS,
    LEINAD_BLOCK_YELLOW_STAINED_GLASS,
    LEINAD_BLOCK_LIME_STAINED_GLASS,
    LEINAD_BLOCK_GREEN_STAINED_GLASS,
    LEINAD_BLOCK_CYAN_STAINED_GLASS,
    LEINAD_BLOCK_BLUE_STAINED_GLASS,
    LEINAD_BLOCK_LIGHT_BLUE_STAINED_GLASS,
    LEINAD_BLOCK_PINK_STAINED_GLASS,
    LEINAD_BLOCK_MAGENTA_STAINED_GLASS,
    LEINAD_BLOCK_PURPLE_STAINED_GLASS,
    LEINAD_BLOCK_OAK_LOG,
    LEINAD_BLOCK_BIRCH_LOG,
    LEINAD_BLOCK_end,
    
    LEINAD_BLOCK_default = LEINAD_BLOCK_end,
    LEINAD_BLOCK_NULL = LEINAD_BLOCK_default,

    LEINAD_BLOCK_amount,

    LEINAD_BLOCK_invalid = 0xFFFF // 
} block_id_t;

typedef enum {
    LEINAD_BLOCKTX_init   = 0,
    LEINAD_BLOCKTX_STONE = 0,
    LEINAD_BLOCKTX_GLASS,
    LEINAD_BLOCKTX_WHITE_STAINED_GLASS,
    LEINAD_BLOCKTX_LIGHT_GRAY_STAINED_GLASS,
    LEINAD_BLOCKTX_GRAY_STAINED_GLASS,
    LEINAD_BLOCKTX_BLACK_STAINED_GLASS,
    LEINAD_BLOCKTX_BROWN_STAINED_GLASS,
    LEINAD_BLOCKTX_RED_STAINED_GLASS,
    LEINAD_BLOCKTX_ORANGE_STAINED_GLASS,
    LEINAD_BLOCKTX_YELLOW_STAINED_GLASS,
    LEINAD_BLOCKTX_LIME_STAINED_GLASS,
    LEINAD_BLOCKTX_GREEN_STAINED_GLASS,
    LEINAD_BLOCKTX_CYAN_STAINED_GLASS,
    LEINAD_BLOCKTX_BLUE_STAINED_GLASS,
    LEINAD_BLOCKTX_LIGHT_BLUE_STAINED_GLASS,
    LEINAD_BLOCKTX_PINK_STAINED_GLASS,
    LEINAD_BLOCKTX_MAGENTA_STAINED_GLASS,
    LEINAD_BLOCKTX_PURPLE_STAINED_GLASS,
    LEINAD_BLOCKTX_OAK_LOG_TOP,
    LEINAD_BLOCKTX_OAK_LOG_SIDE,
    LEINAD_BLOCKTX_BIRCH_LOG_TOP,
    LEINAD_BLOCKTX_BIRCH_LOG_SIDE,
    LEINAD_BLOCKTX_end,
    
    LEINAD_BLOCKTX_default = LEINAD_BLOCKTX_end,
    LEINAD_BLOCKTX_NULL = LEINAD_BLOCKTX_default,

    LEINAD_BLOCKTX_amount,

    LEINAD_BLOCKTX_invalid = 0xFFFF
} block_tx_t;

typedef enum {
    LEINAD_BLOCKRENDER_init = 0,
    LEINAD_BLOCKRENDER_FULL_NO_ROT,
    LEINAD_BLOCKRENDER_FULL_RANDOM_ROT,
    LEINAD_BLOCKRENDER_FULL_SPECIFIC_ROT,
    LEINAD_BLOCKRENDER_STAIR,
    LEINAD_BLOCKRENDER_SLAB,
    LEINAD_BLOCKRENDER_end,
    
    LEINAD_BLOCKRENDER_default = LEINAD_BLOCKRENDER_end,
    LEINAD_BLOCKRENDER_NULL = LEINAD_BLOCKRENDER_default,

    LEINAD_BLOCKRENDER_amount,

    LEINAD_BLOCKRENDER_invalid = 0xFFFF
} block_render_t;

struct block {
    Uint32 flags;
    #define LEINAD_BLOCKFLAG_default            0x00000000

    #define LEINAD_BLOCKFLAG_istransparent      0x00000001
    #define LEINAD_BLOCKFLAG_isfullblock        0x00000002
     #define LEINAD_BLOCKFLAG_issingletexture    0x00000004
    #define LEINAD_BLOCKFLAG_hastransparency    0x00000008 // @todo CONSIDER REPLACING WITH PER-FACE OCCLUSION
    #define LEINAD_BLOCKFLAG_iscontiguous       0x00000010
    #define LEINAD_BLOCKFLAG_hascustomplacement 0x00000020 // @todo REPLACE WITH ROTATION AND SUB-BLOCK DISPLACEMENT


    block_render_t block_render_category : 16;

    /**
     *  amount of light to remove when light passes through it
     */
    Uint8 light_removal;

    union static_blockdata {

        struct full_single_texture {
            Uint32 tx_index;
            float r,g,b,a;
        } full_single_texture;

        struct full_multiple_texture {
            // y, -y, z, -z, x, -x
            Uint32 tx_index[6];
            float r,g,b,a;
        } full_multiple_texture;
    
    } data;
};


// per-block data

#define leinad_get_block_data(block_id) block_data[block_id]

const struct block block_data[LEINAD_BLOCK_amount] = {
    { // LEINAD_BLOCK_STONE
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_RANDOM_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_STONE,
            0.35f,0.35f,0.35f,1.f
        }
    },
    { // LEINAD_BLOCK_AIR
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_hastransparency
               | LEINAD_BLOCKFLAG_istransparent,
        .block_render_category = LEINAD_BLOCKRENDER_NULL,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_invalid,
            1.f,1.f,1.f,1.f
        }
    },
    { // LEINAD_BLOCK_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_GLASS,
            0.3f,0.1f,0.4f,1.f
        }
    },
    { // LEINAD_BLOCK_WHITE_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_WHITE_STAINED_GLASS,
            1.f,1.f,1.f,1.f
        }
    },
    { // LEINAD_BLOCK_LIGHT_GRAY_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_LIGHT_GRAY_STAINED_GLASS,
            0.8f,0.8f,0.8f,1.f
        }
    },
    { // LEINAD_BLOCK_GRAY_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_GRAY_STAINED_GLASS,
            0.5f,0.5f,0.5f,1.f
        }
    },
    { // LEINAD_BLOCK_BLACK_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_BLACK_STAINED_GLASS,
            0.1f,0.1f,0.1f,1.f
        }
    },
    { // LEINAD_BLOCK_BROWN_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_BROWN_STAINED_GLASS,
            0.7f,0.45f,0.3f,1.f
        }
    },
    { // LEINAD_BLOCK_RED_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_RED_STAINED_GLASS,
            1.f,0.f,0.f,1.f
        }
    },
    { // LEINAD_BLOCK_ORANGE_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_ORANGE_STAINED_GLASS,
            1.f,0.7f,0.f,1.f
        }
    },
    { // LEINAD_BLOCK_YELLOW_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_YELLOW_STAINED_GLASS,
            1.f,1.f,0.f,1.f
        }
    },
    { // LEINAD_BLOCK_LIME_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_LIME_STAINED_GLASS,
            0.7f,1.f,0.3f,1.f
        }
    },
    { // LEINAD_BLOCK_GREEN_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_GREEN_STAINED_GLASS,
            0.f,1.f,0.2f,1.f
        }
    },
    { // LEINAD_BLOCK_CYAN_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_CYAN_STAINED_GLASS,
            0.f,0.8f,0.8f,1.f
        }
    },
    { // LEINAD_BLOCK_BLUE_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_BLUE_STAINED_GLASS,
            0.f,0.f,1.f,1.f
        }
    },
    { // LEINAD_BLOCK_LIGHT_BLUE_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_LIGHT_BLUE_STAINED_GLASS,
            0.3f,0.4f,1.f,1.f
        }
    },
    { // LEINAD_BLOCK_PINK_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_PINK_STAINED_GLASS,
            1.f,0.7f,0.6f,1.f
        }
    },
    { // LEINAD_BLOCK_MAGENTA_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_MAGENTA_STAINED_GLASS,
            1.f,0.0f,1.0f,1.f
        }
    },
    { // LEINAD_BLOCK_PURPLE_STAINED_GLASS
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_iscontiguous
               | LEINAD_BLOCKFLAG_hastransparency,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_NO_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_PURPLE_STAINED_GLASS,
            0.6f,0.f,0.6f,1.f
        }
    },
    { // LEINAD_BLOCK_OAK_LOG
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_SPECIFIC_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_OAK_LOG_SIDE,
            0.4f,0.4f,0.2f,1.f
        }
    },
    { // LEINAD_BLOCK_BIRCH_LOG
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock,
        .block_render_category = LEINAD_BLOCKRENDER_FULL_SPECIFIC_ROT,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_BIRCH_LOG_SIDE,
            0.4f,0.4f,0.4f,1.f
        }
    },
    
    // default
    { // LEINAD_BLOCK_NULL
        .flags = LEINAD_BLOCKFLAG_default
               | LEINAD_BLOCKFLAG_isfullblock
               | LEINAD_BLOCKFLAG_hastransparency
               | LEINAD_BLOCKFLAG_istransparent,
        .block_render_category = LEINAD_BLOCKRENDER_NULL,
        .data.full_single_texture = {
            .tx_index = LEINAD_BLOCKTX_NULL,
            1.f,1.f,1.f,1.f
        }
    }
};

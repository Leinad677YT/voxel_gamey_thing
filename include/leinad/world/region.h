#pragma once

#include <SDL3/SDL.h>

#include "../datatype/dimension.h"
#include "../math/arithmetic.h"
#include "../data/types.h"
#include "../data/tags.h"

#include "block.h"

// REGIONS

#define LEINAD_REGION_RADIUS 128
#define LEINAD_MESH_RADIUS 32

#define leinad_get_chunk_index(x,y,z) ((y)*LEINAD_REGION_RADIUS*LEINAD_REGION_RADIUS + (z)*LEINAD_REGION_RADIUS + (x))

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

    float pos[3]; // global position of the lowest corner, not present when saving to disk

    #define mask_amount_64x 0b0000000000000000000000000000000000000000000000000000000000000111
    #define mask_amount_32x 0b0000000000000000000000000000000000000000000000000000000111111000
    #define mask_amount_16x 0b0000000000000000000000000000000000000000000000111111111000000000
    #define mask_amount_08x 0b0000000000000000000000000000000000111111111111000000000000000000
    #define mask_amount_04x 0b0000000000000000000111111111111111000000000000000000000000000000
    #define mask_amount_02x 0b0111111111111111111000000000000000000000000000000000000000000000
    // contains the amount of FILLED subregions that each level has, if
    //  full, ignore. Data is stored in multiples of 3 bits:
    //  3bits, 6bits, 9bits, 12bits, ... until adding up to 63
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


struct chunk_mesh {
    SDL_GPUBuffer* vertex_opaque;
    SDL_GPUBuffer* index_opaque;

    SDL_GPUBuffer* vertex_translucent;
    SDL_GPUBuffer* index_translucent;

    Uint32 vert_o,vert_t;
    Uint32 ind_o, ind_t;
};


typedef struct leinad_chunk {
    float pos[3];                   // global position of the lowest corner
    struct dimension* dimension;    // dimension of the chunk
    struct blockdata block[raise3(LEINAD_REGION_RADIUS)];
    struct chunk_mesh* mesh[raise3(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS)];
} leinad_chunk_t;


// x,y,z are already modulo'd 128
LEINAD_FGET struct blockdata leinad_region_getblock(int x, int y, int z, leinad_region_t* region);

LEINAD_FBUILDER leinad_chunk_t* leinad_chunk_create(float x, float y, float z, struct dimension* dimension);
LEINAD_FCLEANER void leinad_chunk_free(leinad_chunk_t* chunk);

LEINAD_FINITIALIZER leinad_chunk_t* leinad_chunk_setfromregion(leinad_region_t* region, leinad_chunk_t* chunk);

LEINAD_FBUILDER leinad_region_t* leinad_region_create_from_chunk(leinad_chunk_t* chunk);

LEINAD_FBUILDER leinad_region_t* leinad_region_create_empty(float x, float y, float z);

LEINAD_FINITIALIZER struct chunk_mesh* leinad_chunk_create_mesh(leinad_chunk_t *chunk, short off_x, short off_y, short off_z);

/**
 * @note struct used to send the data to the chunk rendering functions
 */
struct _chunkrenderdata {
    SDL_GPURenderPass* renderpass;
    SDL_GPUCommandBuffer* command_buffer;
    vec4 fov_planes[4];
    vec3 viewvec;
    struct leinad_position pos;
    struct leinad_rotation rotation;
};

/**
 * @param chunk chunk to work over
 * @param data data to render the chunk with, this being a pointer to a `struct _chunkrenderdata`
 */
LEINAD_FRENDER void leinad_chunk_render_opaque(leinad_chunk_t *chunk, void* data);

/**
 * @param chunk chunk to work over
 * @param data data to render the chunk with, this being a pointer to a `struct _chunkrenderdata`
 */
LEINAD_FRENDER void leinad_chunk_render_transparent(leinad_chunk_t *chunk, void* data);

/**
 * @param chunk chunk to work over
 * @param data data to render the chunk with, this being a pointer to a `struct _chunkrenderdata`
 */
LEINAD_FRENDER void leinad_chunk_render_front(leinad_chunk_t *chunk, void* data);
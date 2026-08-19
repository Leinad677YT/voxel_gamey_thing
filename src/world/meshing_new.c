#include <leinad/math/arithmetic.h>
#include "../libs/bit_manipulation.h"
#include <leinad/data/control_shortcuts.h>
#include <leinad/world/region.h>
#include <leinad/render.h>
#include <leinad/app.h>
#include <leinad/world/block.h>

#define STARTING_VERTEX_ALLOCATION (4 * 200)
#define STARTING_INDEX_ALLOCATION (6 * 200)

LEINAD_FINITIALIZER struct chunk_mesh* leinad_chunk_create_mesh(leinad_chunk_t *chunk, short off_x, short off_y, short off_z) {

    struct block_vertex* buffer_vertex_opaque = NULL;
    struct block_vertex* buffer_vertex_translucent = NULL;
    Uint32* buffer_index_opaque = NULL;
    Uint32* buffer_index_translucent = NULL;

    Uint32 ind_v_o, ind_v_t, ind_i_o, ind_i_t;
        ind_i_t = ind_v_o = ind_v_t = ind_i_o = 0;

    size_t size_v_o, size_v_t, size_i_o, size_i_t;
        size_i_t = size_i_o = STARTING_INDEX_ALLOCATION * sizeof(struct block_vertex);
        size_v_o = size_v_t = STARTING_VERTEX_ALLOCATION * sizeof(Uint32);

    SDL_GPUTransferBuffer* bufferTransferBuffer;

    buffer_vertex_opaque = SDL_malloc(size_v_o);
    buffer_vertex_translucent = SDL_malloc(size_v_t);

    buffer_index_opaque = SDL_malloc(size_i_o);
    buffer_index_translucent = SDL_malloc(size_i_t);


    if (
        buffer_vertex_opaque == NULL
     || buffer_vertex_translucent == NULL
     || buffer_index_opaque == NULL
     || buffer_index_translucent == NULL
    ) goto failure;

  { // iterate over all blocks

    for (int y = off_y * LEINAD_MESH_RADIUS; y < off_y * LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS; y++)
     for (int z = off_z * LEINAD_MESH_RADIUS; z < off_z * LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS; z++)
      for (int x = off_x * LEINAD_MESH_RADIUS; x < off_x * LEINAD_MESH_RADIUS + LEINAD_MESH_RADIUS; x++) {
        
        
        #define _block(x,y,z) chunk->block[leinad_get_chunk_index(x, y, z)]
        #define _blockdata(x,y,z) leinad_get_block_data(_block(x, y, z).id)
        #define _tx_index(x,y,z) leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.tx_index
        #define _blocktx_u(tx_idx) ((tx_idx % block_atlas.width) * (1.f/block_atlas.width))
        #define _blocktx_v(tx_idx) ((int)(tx_idx / block_atlas.width) * (float)(1.f/block_atlas.width))

        Uint8 faces = 0;

        short random_rot_offset_a = 0;
        short random_rot_offset_b = 0;

        struct uv {float u; float v;} uv_maps[4] = {
            {0.0f,                  0.0f                 },
            {0.0f,                  1.f/block_atlas.width},
            {1.f/block_atlas.width, 1.f/block_atlas.width},
            {1.f/block_atlas.width, 0.0f                 }
        };


        switch(_blockdata(x,y,z).block_render_category) {
            
            case LEINAD_BLOCKRENDER_FULL_SPECIFIC_ROT:
            // @todo

            case LEINAD_BLOCKRENDER_FULL_RANDOM_ROT:
                
                random_rot_offset_a = (short)(0.1*x*x + 0.02*y*y + 0.08*z*z) % 4;
                random_rot_offset_b = (short)(x*y*z + x*x*y + y*y*z + z*z*x) % 11;

                struct uv temp[4];

                if (random_rot_offset_b % 11 > 9) {
                    temp[0] = uv_maps[0]; temp[1] = uv_maps[1];
                    uv_maps[0] = uv_maps[2]; uv_maps[1] = uv_maps[3];
                    uv_maps[2] = temp[0]; uv_maps[3] = temp[1];
                }

                temp[0] = uv_maps[(0+random_rot_offset_a) % 4];
                temp[1] = uv_maps[(1+random_rot_offset_a) % 4];
                temp[2] = uv_maps[(2+random_rot_offset_a) % 4];
                temp[3] = uv_maps[(3+random_rot_offset_a) % 4];
                uv_maps[0] = temp[0];
                uv_maps[1] = temp[1];
                uv_maps[2] = temp[2];
                uv_maps[3] = temp[3]; 

            case LEINAD_BLOCKRENDER_FULL_NO_ROT:

                // for every face
                // if 
                //   faces outside chunk 
                //   OR collides with opaque
                //   OR contiguous block is same and has continuity
                // SKIP

                // +y
                if (
                    (y == LEINAD_REGION_RADIUS -1 // (off_y+1) * LEINAD_MESH_RADIUS -1
                    || (
                        leinad_get_block_data(_block(x,y+1,z).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x,y+1,z).id
                )))) faces |= 0b000001;
                
                // -y
                if (
                    (y == 0 // off_y * LEINAD_MESH_RADIUS
                    || (
                        leinad_get_block_data(_block(x,y-1,z).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x,y-1,z).id
                )))) faces |= 0b000010;

                // +z
                if (
                    (z ==  LEINAD_REGION_RADIUS -1 // (off_z+1) * LEINAD_MESH_RADIUS -1
                    || (
                        leinad_get_block_data(_block(x,y,z+1).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x,y,z+1).id
                )))) faces |= 0b000100;
                
                // -z
                if (
                    (z == 0 // off_z * LEINAD_MESH_RADIUS
                    ||  (
                        leinad_get_block_data(_block(x,y,z-1).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x,y,z-1).id
                )))) faces |= 0b001000;

                // +x
                if (
                    (x == LEINAD_REGION_RADIUS -1 // (off_x+1) * LEINAD_MESH_RADIUS -1
                    || (
                        leinad_get_block_data(_block(x+1,y,z).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x+1,y,z).id
                )))) faces |= 0b010000;
                
                // -x
                if (
                    (x == 0 // off_x * LEINAD_MESH_RADIUS
                    || (
                        leinad_get_block_data(_block(x-1,y,z).id).flags & LEINAD_BLOCKFLAG_hastransparency
                        && !(
                        leinad_get_block_data(_block(x,y,z).id).flags & LEINAD_BLOCKFLAG_iscontiguous
                        && _block(x,y,z).id == _block(x-1,y,z).id
                )))) faces |= 0b100000;

                // no custom placement (all faces share uv)
                        if (
                            _blockdata(x,y,z).flags & LEINAD_BLOCKFLAG_hastransparency
                        ) {

                            if (
                                ensure_capacity((void**)&buffer_index_translucent, sizeof(Uint32) * (ind_i_t + 6 * count_set_bits(faces)), &size_i_t) == LEINAD_RETURN_FAILURE
                            || ensure_capacity((void**)&buffer_vertex_translucent,sizeof(struct block_vertex) *  (ind_v_t + 4 * count_set_bits(faces)), &size_v_t) == LEINAD_RETURN_FAILURE
                            ) goto failure;

                            // indexes
                            if (faces & 0b100000) { // -x
                                
                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {
                                    .x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                    .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                    .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                    .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                    .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {
                                    .x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                    .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                    .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                    .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                    .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {
                                    .x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                    .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                    .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                    .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                    .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {
                                    .x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                    .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                    .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                    .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                    .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(0 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(3 + 0) % 4;

                                // update array idxs
                                ind_v_t += 4;
                                ind_i_t += 6;
                            };
                            if (faces & 0b010000) { // +x
                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                    .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                    .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                    .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                    .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(0 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(2 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(3 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(1 + 0) % 4;

                                // update array idxs
                                ind_v_t+=4;
                                ind_i_t+=6;
                            };
                            if (faces & 0b001000) { // -z

                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(0 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(3 + 0) % 4;

                                // update array idxs
                                ind_v_t+=4;
                                ind_i_t+=6;
                            };
                            if (faces & 0b000100) { // +z

                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(0 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(2 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(3 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(1 + 0) % 4;

                                // update array idxs
                                ind_v_t+=4;
                                ind_i_t+=6;
                            };
                            if (faces & 0b000010) { // -y

                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(0 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(2 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(3 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(1 + 0) % 4;

                                // update array idxs
                                ind_v_t+=4;
                                ind_i_t+=6;
                            };
                            if (faces & 0b000001) { // +y

                                // vertex
                                buffer_vertex_translucent[ind_v_t+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_translucent[ind_v_t+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_translucent[ind_v_t+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_translucent[ind_v_t+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };

                                // index
                                buffer_index_translucent[ind_i_t+0] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+1] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+2] = ind_v_t +(0 + 0) % 4;

                                buffer_index_translucent[ind_i_t+3] = ind_v_t +(1 + 0) % 4;
                                buffer_index_translucent[ind_i_t+4] = ind_v_t +(2 + 0) % 4;
                                buffer_index_translucent[ind_i_t+5] = ind_v_t +(3 + 0) % 4;

                                // update array idxs
                                ind_v_t+=4;
                                ind_i_t+=6;
                            };
                        }
                        else {

                            if (
                                ensure_capacity((void**)&buffer_index_opaque, sizeof(Uint32) * (ind_i_o + 6 * count_set_bits(faces)), &size_i_o) == LEINAD_RETURN_FAILURE
                            || ensure_capacity((void**)&buffer_vertex_opaque,sizeof(struct block_vertex) *  (ind_v_o + 4 * count_set_bits(faces)), &size_v_o) == LEINAD_RETURN_FAILURE
                            ) goto failure;

                            // indexes
                            if (faces & 0b100000) { // -x

                                
                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nX)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(0 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(3 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                            if (faces & 0b010000) { // +x

                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pX)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(0 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(2 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(3 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(1 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                            if (faces & 0b001000) { // -z

                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nZ)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(0 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(3 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                            if (faces & 0b000100) { // +z

                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pZ)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(0 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(2 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(3 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(1 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                            if (faces & 0b000010) { // -y

                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+0), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_nY)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(0 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(2 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(3 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(1 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                            if (faces & 0b000001) { // +y

                                // vertex
                                buffer_vertex_opaque[ind_v_o+0] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[1].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[1].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_opaque[ind_v_o+1] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+0),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[2].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[2].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_opaque[ind_v_o+2] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+0),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[0].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[0].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };
                                buffer_vertex_opaque[ind_v_o+3] = (struct block_vertex) {.x= LEINAD_BLOCK_RENDER_SCALE*(x+1),.y= LEINAD_BLOCK_RENDER_SCALE*(y+1), .z=LEINAD_BLOCK_RENDER_SCALE*(z+1),
                                    .u = _blocktx_u(_tx_index(x,y,z)) + uv_maps[3].u,
                                    .v = _blocktx_v(_tx_index(x,y,z)) + uv_maps[3].v,
                                   .r=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.r,
                                   .g=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.g,
                                   .b=leinad_get_block_data(_block(x,y,z).id).data.full_single_texture.b,
                                   .a=1-(0.1 + (float)leinad_get_block_light(_block(x,y,z)) / LEINAD_BLOCK_MAX_LIGHT_LEVEL)*(1-LEINAD_BLOCK_SHADE_pY)
                                   };

                                // index
                                buffer_index_opaque[ind_i_o+0] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+1] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+2] = ind_v_o +(0 + 0) % 4;

                                buffer_index_opaque[ind_i_o+3] = ind_v_o +(1 + 0) % 4;
                                buffer_index_opaque[ind_i_o+4] = ind_v_o +(2 + 0) % 4;
                                buffer_index_opaque[ind_i_o+5] = ind_v_o +(3 + 0) % 4;

                                // update array idxs
                                ind_v_o+=4;
                                ind_i_o+=6;
                            };
                        }

                break;
            
            case LEINAD_BLOCKRENDER_default:
                break;
                
        }
    }
  }

  { // allocate gpu buffers and transfer buffer
    #define current_mesh chunk->mesh[off_y * raise2(LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS) + off_z * LEINAD_REGION_RADIUS/LEINAD_MESH_RADIUS + off_x]
    current_mesh->ind_o = ind_i_o;
    current_mesh->vert_o = ind_v_o;
    current_mesh->ind_t = ind_i_t;
    current_mesh->vert_t = ind_v_t;

    // if (current_mesh->vertex != NULL) SDL_Log("released  IDX: %20p\t\tVTX: %20p",current_mesh->index,current_mesh->vertex);
    if (current_mesh->vertex_opaque != NULL) SDL_ReleaseGPUBuffer(APP.device, current_mesh->vertex_opaque);
    if (current_mesh->index_opaque != NULL) SDL_ReleaseGPUBuffer(APP.device, current_mesh->index_opaque);
    if (current_mesh->vertex_translucent != NULL) SDL_ReleaseGPUBuffer(APP.device, current_mesh->vertex_translucent);
    if (current_mesh->index_translucent != NULL) SDL_ReleaseGPUBuffer(APP.device, current_mesh->index_translucent);


    if (ind_v_o + ind_v_t == 0) {
        current_mesh->vertex_opaque = NULL;
        current_mesh->index_opaque  = NULL;
        current_mesh->vertex_translucent = NULL;
        current_mesh->index_translucent  = NULL;
        goto failure;
    }


    if (ind_v_o == 0) { // opaque
        current_mesh->vertex_opaque = NULL;
        current_mesh->index_opaque  = NULL;
    } else {

        SDL_GPUBufferCreateInfo createinfo = {
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = ind_v_o * sizeof(struct block_vertex),
            .props = 0
        };

        current_mesh->vertex_opaque = SDL_CreateGPUBuffer(APP.device, &createinfo);

        // create index buffer
        createinfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        createinfo.size = ind_i_o * sizeof(Uint32);

        current_mesh->index_opaque = SDL_CreateGPUBuffer(APP.device, &createinfo);
    }


    if (ind_v_t == 0) { // translucent
        current_mesh->vertex_translucent = NULL;
        current_mesh->index_translucent  = NULL;
    } else {

        SDL_GPUBufferCreateInfo createinfo = (SDL_GPUBufferCreateInfo) {
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = ind_v_t * sizeof(struct block_vertex),
            .props = 0
        };

        current_mesh->vertex_translucent = SDL_CreateGPUBuffer(APP.device, &createinfo);
        // create index buffer
        createinfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

        createinfo.size = ind_i_t * sizeof(Uint32);
        current_mesh->index_translucent = SDL_CreateGPUBuffer(APP.device, &createinfo);
    }

    SDL_GPUTransferBufferCreateInfo createinfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = 0 // space for vertex + index 
    };
    createinfo.size = (ind_i_o + ind_i_t) * sizeof(Uint32) + sizeof(struct block_vertex) * (ind_v_o + ind_v_t);

    bufferTransferBuffer = SDL_CreateGPUTransferBuffer(
        APP.device,
        &createinfo
    );


  }

  { // fill buffers
    struct block_vertex* transferData = SDL_MapGPUTransferBuffer(
        APP.device,
        bufferTransferBuffer,
        false
    );

    Uint32* indexData = (Uint32*) &transferData[ind_v_o + ind_v_t];

    SDL_memcpy((struct block_vertex*)transferData,buffer_vertex_opaque,ind_v_o * sizeof(struct block_vertex));
    SDL_memcpy((struct block_vertex*)&transferData[ind_v_o],buffer_vertex_translucent,ind_v_t * sizeof(struct block_vertex));
    SDL_memcpy((Uint32*)indexData,buffer_index_opaque,ind_i_o * sizeof(Uint32));
    SDL_memcpy((Uint32*)&indexData[ind_i_o],buffer_index_translucent,ind_i_t * sizeof(Uint32));

    SDL_UnmapGPUTransferBuffer(APP.device, bufferTransferBuffer);
  }

  { // upload buffers

        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(APP.device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        if (current_mesh->vertex_opaque != NULL) 
            SDL_UploadToGPUBuffer(
                copyPass,
                &(SDL_GPUTransferBufferLocation) {
                    .transfer_buffer = bufferTransferBuffer,
                    .offset = 0
                },
                &(SDL_GPUBufferRegion) {
                    .buffer = current_mesh->vertex_opaque,
                    .offset = 0,
                    .size = sizeof(struct block_vertex) * ind_v_o
                },
                false
            );

        if (current_mesh->vertex_translucent != NULL) 
            SDL_UploadToGPUBuffer(
                copyPass,
                &(SDL_GPUTransferBufferLocation) {
                    .transfer_buffer = bufferTransferBuffer,
                    .offset = sizeof(struct block_vertex) * ind_v_o
                },
                &(SDL_GPUBufferRegion) {
                    .buffer = current_mesh->vertex_translucent,
                    .offset = 0,
                    .size = sizeof(struct block_vertex) * ind_v_t
                },
                false
            );

        if (current_mesh->index_opaque != NULL) 
            SDL_UploadToGPUBuffer(
                copyPass,
                &(SDL_GPUTransferBufferLocation) {
                    .transfer_buffer = bufferTransferBuffer,
                    .offset = sizeof(struct block_vertex) * (ind_v_o + ind_v_t)
                },
                &(SDL_GPUBufferRegion) {
                    .buffer = current_mesh->index_opaque,
                    .offset = 0,
                    .size = sizeof(Uint32) * ind_i_o
                },
                false
            );

        if (current_mesh->index_translucent != NULL) 
            SDL_UploadToGPUBuffer(
                copyPass,
                &(SDL_GPUTransferBufferLocation) {
                    .transfer_buffer = bufferTransferBuffer,
                    .offset = sizeof(struct block_vertex) * (ind_v_o + ind_v_t) + sizeof(Uint32) * ind_i_o
                },
                &(SDL_GPUBufferRegion) {
                    .buffer = current_mesh->index_translucent,
                    .offset = 0,
                    .size = sizeof(Uint32) * ind_i_t,
                },
                false
            );

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
        SDL_ReleaseGPUTransferBuffer(APP.device, bufferTransferBuffer);


  }

  success:
    if (buffer_index_opaque != NULL) SDL_free(buffer_index_opaque);
    if (buffer_index_translucent != NULL) SDL_free(buffer_index_translucent);
    if (buffer_vertex_opaque != NULL) SDL_free(buffer_vertex_opaque);
    if (buffer_vertex_translucent != NULL) SDL_free(buffer_vertex_translucent);

    return NULL;
  failure:
    if (buffer_index_opaque != NULL) SDL_free(buffer_index_opaque);
    if (buffer_index_translucent != NULL) SDL_free(buffer_index_translucent);
    if (buffer_vertex_opaque != NULL) SDL_free(buffer_vertex_opaque);
    if (buffer_vertex_translucent != NULL) SDL_free(buffer_vertex_translucent);

    return NULL;
}
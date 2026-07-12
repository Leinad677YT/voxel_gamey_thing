#pragma once

#include "./region.h"

#include "../math/arithmetic.h"

#include "../player.h"


/**
 * Length away from center over each dimension of the chunk-cube that is actively loaded (can update)
 */
#define LOADED_CHUNKS_RADIUS 1
#define LOADED_CHUNKS_LENGTH (2*LOADED_CHUNKS_RADIUS+1)

/**
 * Length away from center over each dimension of the chunk-cube that is actively loaded (can update)
 */
#define LOADED_REGIONS_RADIUS (LOADED_CHUNKS_RADIUS + 1)
#define LOADED_REGIONS_LENGTH (2*LOADED_REGIONS_RADIUS+1)

/**
 * Struct that holds the data for loaded chunks and regions
 */
extern struct _loaded_chunks {

    // y-z-x sorted
    leinad_chunk_t *chunk[
        raise3(LOADED_CHUNKS_LENGTH)
    ];

    // y-z-x sorted
    leinad_region_t *region[
        raise3(LOADED_REGIONS_LENGTH) - raise3(LOADED_CHUNKS_LENGTH)
    ];

    double center_pos[3];

} loaded_chunks;

/**
 * Iterate over the loaded_chunks from furthest to nearest
 * @todo nearest to furthest
 */
void loaded_chunks_forall_decreasing(void(*fun)(leinad_chunk_t*,void*),void* arg);

/* order of the array inside the flat representation of a 5x5x5
        4   9   14  19   24
       3   8   13  18   23
      2   7   12  17   22
     1   6   11  16   21
    0   5   10  15   20

        29  34  39  44  49
       28  33  38  43  48
      27  32  37  42  47
     26  31  36  41  46
    25  30  35  40  45

        54  59  64  69  74
       53  58  63  68  73
      52  57  62  67  72
     51  56  61  66  71
    50  55  60  65  70
        
        79  84  89  94  99
       78  83  88  93  98
      77  82  87  92  97
     76  81  86  91  96
    75  80  85  90  95
        
        104 109 114 119 124
       103 108 113 118 123
      102 107 112 117 122
     101 106 111 116 121
    100 105 110 115 120

*/

void load_around_player(struct entity_player* player);

enum load_direction {
    LOAD_DIR_pX,
    LOAD_DIR_nX,
    LOAD_DIR_pY,
    LOAD_DIR_nY,
    LOAD_DIR_pZ,
    LOAD_DIR_nZ,
    LOAD_DIR_none
};

void unload_chunk(leinad_chunk_t** chunk, enum load_direction dir);
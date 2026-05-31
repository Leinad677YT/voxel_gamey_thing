#pragma once

#include "region.h"

#include "../math/arithmetic.h"


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
 * Struct that holds the data for 
 */
static struct {

    // y-z-x sorted
    leinad_chunk_t *chunk[
        raise3(LOADED_CHUNKS_LENGTH)
    ];

    // y-z-x sorted
    leinad_region_t *region[
        raise3(LOADED_REGIONS_LENGTH) - raise3(LOADED_CHUNKS_LENGTH)
    ];
} loaded_chunks = {
    .chunk = {0}
};

/**
 * Iterate over the loaded_chunks from nearest to furthest
 * @todo furthest to nearest
 */
static void loaded_chunks_forall_increasing(void(*fun)(leinad_chunk_t*,void*),void* arg){

    #define invert(x) (LOADED_CHUNKS_LENGTH -x -1)
    
    // volume to check
    for(int volume = 0; volume < 8; volume++)
     // position to check
     for(int k = 0; k < LOADED_CHUNKS_RADIUS; k++)
      for(int j = 0; j < LOADED_CHUNKS_RADIUS; j++)
       for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (volume){
            case 0: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 3: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 4: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 5: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 6: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 7: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
        }
    }

    for(int strip = 0; strip < 12; strip++)
     // position to check
     for(int j = 0; j < LOADED_CHUNKS_RADIUS; j++)
      for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (strip){
            case 0:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 3:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            
            case 4:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 5:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 6:  fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 7:  fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;

            case 8:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 9:  fun(loaded_chunks.chunk[j        *raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 10: fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 11: fun(loaded_chunks.chunk[invert(j)*raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
        }
    }

    for(int row = 0; row < 12; row++)
     // position to check
     for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        switch (row){
            case 0:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + i],        arg); break;
            case 1:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + invert(i)],arg); break;
            case 2:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + i        *LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 3:  fun(loaded_chunks.chunk[LOADED_CHUNKS_RADIUS*raise2(LOADED_CHUNKS_LENGTH) + invert(i)*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 4:  fun(loaded_chunks.chunk[i        *raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
            case 5:  fun(loaded_chunks.chunk[invert(i)*raise2(LOADED_CHUNKS_LENGTH) + LOADED_CHUNKS_RADIUS*LOADED_CHUNKS_LENGTH + LOADED_CHUNKS_RADIUS],arg); break;
        }
    }

    #undef invert
}




/* order inside the flat representation of a 5x5x5
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
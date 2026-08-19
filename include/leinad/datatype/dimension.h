#pragma once

#include <leinad/data/namespacing.h>
#include <leinad/data/types.h>
#include <leinad/data/tags.h>

#include <leinad/world.h>

enum dimension_generator {
    LEINAD_DIMENSION_GENERATOR_init = 0,
    LEINAD_DIMENSION_GENERATOR_FLAT = 0,
    LEINAD_DIMENSION_GENERATOR_NOISE,
    LEINAD_DIMENSION_GENERATOR_end,
    
    LEINAD_DIMENSION_GENERATOR_default = LEINAD_DIMENSION_GENERATOR_end,
    LEINAD_DIMENSION_GENERATOR_DEBUG = LEINAD_DIMENSION_GENERATOR_default,

    LEINAD_DIMENSION_GENERATOR_amount,

    LEINAD_DIMENSION_GENERATOR_invalid = 0xFF // 

};


/**
 * # DEBUG  
 * 1 instance of each blockstate separated from the rest
 */

struct LEINAD_DIMENSION_GENERATOR_DEBUG {
    enum dimension_generator type : 8; 
};


/**
 * # FLAT
 * Infinite flat world with layers upon layers of the same block each
 */

struct flat_generator_layer {
    Sint16 height;
    struct namespaced_id block_id;
};

struct LEINAD_DIMENSION_GENERATOR_FLAT {
    enum dimension_generator type : 8;
    struct {
        struct flat_generator_layer* layers;
        struct namespaced_id biome;
        
        bool lakes : 1;
        bool features : 1;

        bool has_structure_overrides : 1;
        // converts the provided resource/list into a tag

        void* structure_overrides;
    } settings;
    
};

/**
 * # NOISE
 * Perlin Noise based world generation
 */

enum biome_source {
    LEINAD_BIOME_SOURCE_init = 0,
    LEINAD_BIOME_SOURCE_CHECKERBOARD = 0,
    LEINAD_BIOME_SOURCE_MULTINOISE,
    LEINAD_BIOME_SOURCE_THE_END,
    LEINAD_BIOME_SOURCE_end,

    LEINAD_BIOME_SOURCE_default = LEINAD_BIOME_SOURCE_end,
    LEINAD_BIOME_SOURCE_FIXED = LEINAD_BIOME_SOURCE_default,

    LEINAD_BIOME_SOURCE_amount,
    
    LEINAD_BIOME_SOURCE_invalid = 0xFF //     
    
};

struct biome_source_generic {
    enum biome_source type : 8;
};

struct LEINAD_BIOME_SOURCE_CHECKERBOARD {
    enum biome_source type : 8;
    Uint8 scale;
    void* biomes; // list / tag
};

struct LEINAD_BIOME_SOURCE_FIXED {
    enum biome_source type : 8;
    struct namespaced_id biome;
};

struct LEINAD_BIOME_SOURCE_MULTINOISE {
    enum biome_source type : 8;
    struct {
        struct namespaced_id biome;
        void* parameters; // @todo
    } *biomes;
};

struct LEINAD_BIOME_SOURCE_THE_END {
    enum biome_source type : 8;
};



typedef union {
    struct biome_source_generic generic;
} biome_source_t;

struct LEINAD_DIMENSION_GENERATOR_NOISE {
    enum dimension_generator type : 8;
    struct namespaced_id settings; // noise definition
    biome_source_t biome_source;

};


/**
 * GENERICS
 */


struct dimension_generator_generic {
    enum dimension_generator type : 8;
};

typedef union {
    struct dimension_generator_generic generic;
    struct LEINAD_DIMENSION_GENERATOR_DEBUG debug;
    struct LEINAD_DIMENSION_GENERATOR_FLAT flat;
    struct LEINAD_DIMENSION_GENERATOR_NOISE noise;
} dimension_generator_t;

typedef struct dimension {
    struct namespaced_id dimension_type;
    dimension_generator_t* generator;
} *dimension_t;

LEINAD_FGET struct dimension* leinad_dimension_getfromid(struct namespaced_id id){ return WORLD.dimensions[0];}
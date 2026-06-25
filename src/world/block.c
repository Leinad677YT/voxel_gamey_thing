#include <SDL3/SDL.h>

#include <leinad/world/block.h>

// FUNCTIONS

/**
 * Returns if 2 blocks are identical
 * @return `0` if they are equal, another value otherwise 
 */
LEINAD_FCOMPARATOR int leinad_blockdata_comparator(void * a, void* b) {
    struct blockdata *aa = a, *bb = b;
    int result;

    result = aa->id - bb->id;

    if (result) return result;

    result = aa->custom_data - bb->custom_data;
    
    if (result) return result;

    result = aa->rotation_n_subpos - bb->rotation_n_subpos;
    
    return result;

}

/**
 * clones the data of one block into another, this is equivalent to `(*dst) = (struct blockdata) src`
 */
LEINAD_FINITIALIZER void leinad_blockdata_clone(struct blockdata src, struct blockdata* dst) {
    *dst = src;
}


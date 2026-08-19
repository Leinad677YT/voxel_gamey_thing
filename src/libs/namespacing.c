#include <leinad/data/control_shortcuts.h>
#include <leinad/data/namespacing.h>
#include <leinad/math/arithmetic.h>
/**
 * # NAMESPACES
 *
 * Namespaces are stored in a map-like style, but known pairs save their
 * internal id as their position on the array, to ensure direct access while
 * retaining the ability to check in constant time during runtime.
 */

// # HASHING

#define _MAX_HASH_LENGTH 8
#define _EMPTY_STRING_HASH 1234567

inline static int primary_hash(const char* string, const size_t len, const int prime_size) {

  #ifdef DEBUGMODE
    SDL_assert(string == NULL); // invalid string
    SDL_assert(prime_size == 0); // empty target
  #endif

    if (len == 0) return _EMPTY_STRING_HASH % prime_size; // empty string

    unsigned int hash = string[0];
    int n = min(_MAX_HASH_LENGTH, len);
    for (int i = 1; i < n; i++) hash = (hash<<5) + string[i];
    
    return (
            hash
           ) % prime_size;
}

inline static int secondary_hash(const int first_hash, const int fails, const int aux_prime, const int prime_size) {

  #ifdef DEBUGMODE
    SDL_assert(fails == 0); // executed before failing
  #endif

    return (
            first_hash + fails * (aux_prime - (first_hash % aux_prime))
           ) % prime_size;
}

#undef _MAX_HASH_LENGTH
#undef _EMPTY_STRING_HASH

// # DATA

static struct id* get_id(struct namespace* namespace, enum resource_type type, char* name, size_t len) {

    return NULL;
}

static struct namespace* get_namespace(char* name, size_t len) {

    return NULL;
};

static int iterate_datapack() {


    return LEINAD_RETURN_CONTINUE;
}

#include "../datapack/loading.c"

struct namespace_data NAMESPACE = {
    .namespaces = NULL,
    .namespace_array_size = 0,
    .get_namespace = get_namespace,
    .get_id = get_id,
    .load_all = load_all
};

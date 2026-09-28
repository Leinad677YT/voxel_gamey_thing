#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <SDL3/SDL.h> /* may be replaced carefully, currently in use on the string parsing stuff and error messages */

#define BASE_MAX_SNBT_CHARS 4000

/**
 * Sizes and value ranges for the compound hashmaps
 * @todo find the sweet spot for occupancy vs size on the hashmaps
 */
#define ENBT_COMPOUND_MAX_SMALL 13
#define ENBT_COMPOUND_RANGE_SMALL 24
#define ENBT_COMPOUND_MAX_MEDIUM 61
#define ENBT_COMPOUND_RANGE_MEDIUM 180
#define ENBT_COMPOUND_MAX_BIG 601
#define ENBT_COMPOUND_MAX_REFCOUNT 0xffffff

#define ENBT_MIN_LIST_ALLOCATION 1
#define ENBT_MIN_ARRAY_ALLOCATION 3
/**
 * Self implementation of NBT specs
 */
enum eNBT_Tag {
    TAG_End = 0x00, // unused while in memory
    TAG_Byte = 0x01,
    TAG_Short = 0x02,
    TAG_Int = 0x03,
    TAG_Long = 0x04,
    TAG_Float = 0x05,
    TAG_Double = 0x06,
    TAG_Byte_Array = 0x07,
    TAG_String = 0x08,
    TAG_List = 0x09,
    TAG_Compound = 0x0A,
    TAG_Int_Array = 0x0B,
    TAG_Long_Array = 0x0C,

    TAG_amount
};

/**
 * @note nbt tags are formed like the following:
 *                8b - tag as specified before
 *               16b - length of the name
 *  8b * name_length - name of the tag in UTF-8
 *      payload_size - payload
 * !EXCEPT FOR TAG_End, which does not have trailing data after the tag itself
 * 
 * @note sizes of lists and arrays are always Uint32
 *
 * @note lists contain 1byte for payload type, then size and then the values
 *
 * @note numbers are in big-endian format, so direct Uint8[] reading _should_
 *  :works_as_intended: from files
 */


typedef union enbt {
    struct eNBT_generic *_generic;

    struct eNBT_byte *_byte;
    struct eNBT_short *_short;
    struct eNBT_int *_int;
    struct eNBT_long *_long;
    struct eNBT_float *_float;
    struct eNBT_double *_double;
    struct eNBT_byte_array *_byte_array;
    struct eNBT_string *_string;
    struct eNBT_list *_list;
    struct eNBT_compound *_compound;
    struct eNBT_int_array *_int_array;
    struct eNBT_long_array *_long_array;
} enbt_t;

struct eNBT_generic {
    char* name;
    uint16_t name_length;
    uint16_t type;
    
    uint32_t flags;
    // on lists, contains the type of the elements
    #define ENBT_FLAG_DEFAULT           0x0
    #define ENBT_FLAG_LIST_TYPE         0x000000ff
};

struct eNBT_byte {
    struct eNBT_generic data;
    int8_t payload;
};

struct eNBT_short {
    struct eNBT_generic data;
    int16_t payload;
};

struct eNBT_int {
    struct eNBT_generic data;
    int32_t payload;
};

struct eNBT_long {
    struct eNBT_generic data;
    int64_t payload;
};

struct eNBT_float {
    struct eNBT_generic data;
    float payload;
};

struct eNBT_double {
    struct eNBT_generic data;
    double payload;
};

struct eNBT_byte_array {
    struct eNBT_generic data;
    uint32_t len;
    int8_t *array;
};

struct eNBT_int_array {
    struct eNBT_generic data;
    uint32_t len;
    int32_t *array;
};

struct eNBT_long_array {
    struct eNBT_generic data;
    uint32_t len;
    int64_t *array;
};


struct eNBT_string {
    struct eNBT_generic data;
    uint16_t size;
    char *array;
};

struct eNBT_list {
    struct eNBT_generic data;
    uint32_t size;
    uint32_t current_capacity;
    union enbt *list;
};



struct eNBT_NODE {
    union enbt val;
    struct eNBT_NODE *next;
};

struct eNBT_compound {
    struct eNBT_generic data;
    struct eNBT_COMPOUND_PAYLOAD {
        uint64_t size : 40;
        uint64_t refcount : 24;
        struct eNBT_NODE** small;
        struct eNBT_NODE** medium;
        struct eNBT_NODE** big;
    } *payload;
};

typedef union enbt_static {
    struct eNBT_generic _generic;

    struct eNBT_byte _byte;
    struct eNBT_short _short;
    struct eNBT_int _int;
    struct eNBT_long _long;
    struct eNBT_float _float;
    struct eNBT_float _double;
    struct eNBT_byte_array _byte_array;
    struct eNBT_string _string;
    struct eNBT_list _list;
    struct eNBT_compound _compound;
    struct eNBT_int_array _int_array;
    struct eNBT_long_array _long_array;
} enbt_static_t;


struct string_parsing_return {
    enum string_parsing_validation {
        success_string = 0,
        err_string_empty,
        err_string_invalid_character,
        err_string_quote_not_escaped,
        err_string_invalid_escaping,
        err_string_incomplete_escaping,
        err_string_out_of_memory,
        err_string_overflow_number,
        err_string_invalid_number,
        err_string_unsigned_number,
        err_string_empty_array
    } valid;
    int idx;
};

enum enbt_operation_validation {
    success_enbt = 0,
    err_enbt_out_of_memory,
    err_enbt_invalid_operation,
    err_enbt_invalid_index
};

/**
 * Allocates space for the specfied enbt type
 * 
 * @param name null-terminated string to copy the name from
 * @param name_length length of the name excluding the null-termination
 * @param flags flags to set on the enbt
 * @param type type to create the 
 * 
 * @return pointer to the allocated memory for the type-specific enbt or NULL
 * on error
 */
enbt_t enbt_create_any(const char* restrict name, const uint16_t name_length, const uint32_t flags, const enum eNBT_Tag type);

/**
 * Merges the contents of @param input with the ones on @param target
 * 
 * For example, considering the following arguments as snbt:
 * @param input {a:1, b:{c:[1,2], d:{e:2}}, f:2}
 * @param target {a:3, b:{c:2, d:{}}, g:1} 
 *
 * Then, @param target will become {a:3, b:{c:2, d:{e:2}}, f:2, g:1}
 * 
 * @return success_enbt on success, an error code on failure
 */
enum enbt_operation_validation enbt_merge_value(struct eNBT_compound* target, const struct eNBT_compound* input);

/**
 * Creates a string for the snbt representation of the given @param input
 *
 * @param input enbt to create the snbt of
 * @param written where the amount of bytes written will be stored
 * 
 * @return string of the snbt on success, NULL on failure
 */
char* enbt_to_snbt(const enbt_t input, size_t* written);

/**
 * Returns on @param enbt the nbt value contained in @param input, with an
 *  empty string key.
 * @param input must be of @param len length, as any remaining characters that
 * are not whitespaces will report errors.
 * 
 * > [!NOTE]
 * > Previous contents of @param enbt are undefined after this function. 
 */
struct string_parsing_return enbt_from_snbt(const char* input, size_t len, enbt_t* enbt);

/**
 * Frees the memory previously allocated by a @sa enbt_create_any() call
 * After the execution of this call, the pointer will be invalidated.
 *
 * If the pointer was to a compound with multiple references active, it's count
 * will be disminished by 1.
 */
void enbt_free(enbt_t enbt);
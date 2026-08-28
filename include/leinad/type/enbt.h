#pragma once

#include <SDL3/SDL.h>

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

#define ENBT_MIN_LIST_ALLOCATION 1

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

struct eNBT_generic {
    char* name;
    Uint16 name_length;
    Uint16 type;
    
    Uint32 flags;
    // on lists, contains the type of the elements
    #define ENBT_FLAG_DEFAULT 0x0
    #define ENBT_FLAG_LIST_TYPE 0x000000ff
};



struct eNBT_compound* enbt_create_compound(char* name, Uint16 name_length, Uint32 flags);


/**
 * 
 * @todo @param estimated_size is still WIP, use it as 0 as of now
 */
struct eNBT_list* enbt_create_list(Uint16 estimated_size, enum eNBT_Tag type, char* name, Uint16 name_length, Uint32 flags);



bool enbt_merge_value(void* target, const void* input);

char* enbt_to_snbt(const struct eNBT_generic* input, size_t* written);

/**
 * Returns on @param enbt the nbt value contained in @param input, with an
 *  empty string key.
 * @param input must be of @param len length, as any remaining characters that
 * are not whitespaces will report errors.
 * 
 * > [!NOTE]
 * > Previous contents of @param enbt are undefined after this function. 
 */
enum string_parsing_return enbt_from_snbt(const char* input, size_t len, struct eNBT_generic** enbt);



struct eNBT_generic* enbt_parse_nbt(Uint8 data[], Sint32 length);
struct eNBT_generic* enbt_parse_enbt(Uint8 data[], Sint32 length);

void enbt_free(void* enbt);


struct eNBT_byte {
    struct eNBT_generic data;
    Sint8 payload;
};

struct eNBT_short {
    struct eNBT_generic data;
    Sint16 payload;
};

struct eNBT_int {
    struct eNBT_generic data;
    Sint32 payload;
};

struct eNBT_long {
    struct eNBT_generic data;
    Sint64 payload;
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
    Sint32 len;
    Sint8 *array;
};

struct eNBT_int_array {
    struct eNBT_generic data;
    Sint32 len;
    Sint32 *array;
};

struct eNBT_long_array {
    struct eNBT_generic data;
    Sint32 len;
    Sint64 *array;
};


struct eNBT_string {
    struct eNBT_generic data;
    Uint16 size;
    char *array;
};

struct eNBT_list {
    struct eNBT_generic data;
    Uint32 size;
    Uint32 current_capacity;
    struct eNBT_generic **list;
};



struct eNBT_NODE {
    struct eNBT_generic *val;
    struct eNBT_NODE *next;
};

struct eNBT_compound {
    struct eNBT_generic data;
    Uint64 size;
    struct eNBT_NODE** small;
    struct eNBT_NODE** medium;
    struct eNBT_NODE** big;
};

enum string_parsing_return {
    success_string = 0,
    err_string_empty,
    err_string_invalid_character,
    err_string_quote_not_escaped,
    err_string_invalid_escaping,
    err_string_incomplete_escaping,
    err_string_out_of_memory,
    err_string_invalid_number
};
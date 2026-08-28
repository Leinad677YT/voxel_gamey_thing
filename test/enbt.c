#include <SDL3/SDL.h>

#include "../src/libs/enbt.c"


int main(void){
  { // ENBT -> SNBT OF SIMPLE TYPES
    struct eNBT_byte t_byte = {
        .data = {
            .type = TAG_Byte,
            .name_length = 9,
            .name = "test_byte",
            .flags = 0
        },
        .payload = -19
    };

    char* string = NULL;
    size_t written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)(&t_byte), &written);

    SDL_assert(string);
    SDL_Log("BYTE: %s",string);


    struct eNBT_short t_short = {
        .data = {
            .type = TAG_Short,
            .name_length = 10,
            .name = "test_short",
            .flags = 0
        },
        .payload = -677
    };

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)&t_short, &written);

    SDL_assert(string);
    SDL_Log("SHORT: %s",string);
    
    struct eNBT_int t_int = {
        .data = {
            .type = TAG_Int,
            .name_length = 8,
            .name = "test_int",
            .flags = 0
        },
        .payload = 1234567890
    };

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)&t_int, &written);

    SDL_assert(string);
    SDL_Log("INT: %s",string);
    
    struct eNBT_long t_long = {
        .data = {
            .type = TAG_Long,
            .name_length = 9,
            .name = "test_long",
            .flags = 0
        },
        .payload = 12345678987654321
    };

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)&t_long, &written);

    SDL_assert(string);
    SDL_Log("LONG: %s",string);

    struct eNBT_float t_float = {
        .data = {
            .type = TAG_Float,
            .name_length = 10,
            .name = "test_float",
            .flags = 0
        },
        .payload = -0.677f
    };

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)&t_float, &written);

    SDL_assert(string);
    SDL_Log("FLOAT: %s",string);

    struct eNBT_double t_double = {
        .data = {
            .type = TAG_Double,
            .name_length = 11,
            .name = "test_double",
            .flags = 0
        },
        .payload = 0.123456789
    };

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)&t_double, &written);

    SDL_assert(string);
    SDL_Log("DOUBLE: %s",string);
    
    SDL_free(string);
  }
  { // ENBT -> SNBT OF ARRAY TYPES

    char* string = NULL;
    size_t written = 0;

    struct eNBT_byte_array* t_byte_array = SDL_malloc(sizeof(struct eNBT_byte_array));
    *t_byte_array = (struct eNBT_byte_array) {
        .data = {
            .type = TAG_Byte_Array,
            .name_length = 15,
            .name = "test_byte_array",
            .flags = 0
        },
        .len = 123,
        .array = SDL_malloc(sizeof(Sint8[123]))
    };
    for(int i = 0;i < 123 -1; i++)
        t_byte_array->array[i] = i;
    t_byte_array->array[123-1] = -123; 

    string = enbt_to_snbt((void*)t_byte_array, &written);
    SDL_free(t_byte_array);

    SDL_assert(string);
    SDL_Log("BYTE_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;

    struct eNBT_int_array* t_int_array = SDL_malloc(sizeof(struct eNBT_long_array));
    *t_int_array = (struct eNBT_int_array) {
        .data = {
            .type = TAG_Int_Array,
            .name_length = 14,
            .name = "test_int_array",
            .flags = 0
        },
        .len = 5,
        .array = SDL_malloc(sizeof(Sint64[5]))
    };
    for(int i = 0;i < 5 -1; i++)
        t_int_array->array[i] = i;
    t_int_array->array[5-1] = 123456789; 

    string = enbt_to_snbt((void*)t_int_array, &written);
    SDL_free(t_int_array);

    SDL_assert(string);
    SDL_Log("INT_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;

    struct eNBT_long_array* t_long_array = SDL_malloc(sizeof(struct eNBT_long_array));
    *t_long_array = (struct eNBT_long_array) {
        .data = {
            .type = TAG_Long_Array,
            .name_length = 15,
            .name = "test_long_array",
            .flags = 0
        },
        .array = SDL_malloc(sizeof(Sint64[10])),
        .len = 10
    };
    for(int i = 0;i < 10 -1; i++)
        t_long_array->array[i] = i;
    t_long_array->array[10-1] = 12345678987654321; 

    string = enbt_to_snbt((void*)t_long_array, &written);
    SDL_free(t_long_array);

    SDL_assert(string);
    SDL_Log("LONG_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;

    struct eNBT_string* t_string = SDL_malloc(sizeof(struct eNBT_string));
    *t_string = (struct eNBT_string) {
        .data = {
            .type = TAG_String,
            .name_length = 11,
            .name = "test_string",
            .flags = 0
        },
        .array = "potato",
        .size = 6
    };

    string = enbt_to_snbt((struct eNBT_generic*)t_string, &written);
    SDL_free(t_string);

    SDL_assert(string);
    SDL_Log("STRING: %s",string);
    
    SDL_free(string);
    written = 0;

  }
  { // ENBT -> SNBT OF LISTS + COMPOUNDS

    char* string = NULL;
    size_t written;

    struct eNBT_list *t_list = enbt_create_list(3, TAG_String, "test_list", 9, 0); 

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)t_list, &written);

    SDL_assert(string);
    SDL_Log("LIST-empty: %s",string);

    struct eNBT_string* t_string1 = SDL_malloc(sizeof(struct eNBT_string));
    *t_string1 = (struct eNBT_string) {
        .data = {
            .type = TAG_String,
            .name_length = 1,
            .name = "t",
            .flags = 0
        },
        .array = "potato",
        .size = 6
    };

    SDL_free(string);
    written = 0;

    t_list->list[0] = (void*)t_string1;
    t_list->size = 1;

    string = enbt_to_snbt((struct eNBT_generic*)t_list, &written);

    SDL_assert(string);
    SDL_Log("LIST-one: %s",string);

    struct eNBT_string* t_string2 = SDL_malloc(sizeof(struct eNBT_string));
    *t_string2 = (struct eNBT_string) {
        .data = {
            .type = TAG_String,
            .name_length = 1,
            .name = "t",
            .flags = 0
        },
        .array = "mmm potatos...",
        .size = 19
    };

    SDL_free(string);
    written = 0;

    t_list->list[2] =t_list->list[1] = (void*)t_string2;
    t_list->size = 3;

    string = enbt_to_snbt((struct eNBT_generic*)t_list, &written);

    SDL_assert(string);
    SDL_Log("LIST-multiple: %s",string);
    
    struct eNBT_compound *t_compound = enbt_create_compound("test_compound",13,0); 

    SDL_free(string);
    written = 0;

    string = enbt_to_snbt((struct eNBT_generic*)t_compound, &written);

    SDL_assert(string);
    SDL_Log("COMPOUND-empty: %s",string);

    struct eNBT_string* t_string11 = SDL_malloc(sizeof(struct eNBT_string));
    *t_string11 = (struct eNBT_string) {
        .data = {
            .type = TAG_String,
            .name_length = 3,
            .name = "str",
            .flags = 0
        },
        .array = "potato",
        .size = 6
    };

    SDL_free(string);
    written = 0;

    t_compound->small = SDL_malloc(ENBT_COMPOUND_MAX_SMALL * sizeof(struct eNBT_NODE*));
    SDL_memset(t_compound->small,0,ENBT_COMPOUND_MAX_SMALL * sizeof(struct eNBT_NODE*));
    t_compound->small[0] = &(struct eNBT_NODE){(void*)t_string11,NULL};
    t_compound->size = 1;

    string = enbt_to_snbt((struct eNBT_generic*)t_compound, &written);

    SDL_assert(string);
    SDL_Log("COMPOUND-one: %s",string);

    struct eNBT_double t_double = {
        .data = {
            .type = TAG_Double,
            .name_length = 11,
            .name = "test_double",
            .flags = 0
        },
        .payload = 0.123456789
    };

    struct eNBT_long t_long = {
        .data = {
            .type = TAG_Long,
            .name_length = 9,
            .name = "test_long",
            .flags = 0
        },
        .payload = 12345678987654321
    };

    SDL_free(string);
    written = 0;

    t_compound->medium = SDL_malloc(ENBT_COMPOUND_MAX_MEDIUM * sizeof(struct eNBT_NODE*));
    SDL_memset(t_compound->medium,0,ENBT_COMPOUND_MAX_MEDIUM * sizeof(struct eNBT_NODE*));
    t_compound->medium[3] = &(struct eNBT_NODE){(void*)&t_double,NULL};
    t_compound->big = SDL_malloc(ENBT_COMPOUND_MAX_BIG * sizeof(struct eNBT_NODE*));
    SDL_memset(t_compound->big,0,ENBT_COMPOUND_MAX_BIG * sizeof(struct eNBT_NODE*));
    t_compound->big[3] = &(struct eNBT_NODE){(void*)t_list,&(struct eNBT_NODE){(void*)&t_long,NULL}};
    t_compound->size = 4;

    string = enbt_to_snbt((void*)t_compound, &written);

    SDL_assert(string);
    SDL_Log("COMPOUND-multiple: %s",string);


    SDL_free(string);
  }
}
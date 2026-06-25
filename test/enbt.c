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

    string = enbt_to_snbt(&t_byte, &written);

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

    string = enbt_to_snbt(&t_short, &written);

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

    string = enbt_to_snbt(&t_int, &written);

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

    string = enbt_to_snbt(&t_long, &written);

    SDL_assert(string);
    SDL_Log("LONG: %s",string);
  }
  { // ENBT -> SNBT OF ARRAY TYPES

    char* string = NULL;
    size_t written = 0;

    struct eNBT_byte_array* t_byte_array = SDL_malloc(sizeof(struct eNBT_byte_array) + sizeof(Sint8[123]));
    *t_byte_array = (struct eNBT_byte_array) {
        .data = {
            .type = TAG_Byte_Array,
            .name_length = 15,
            .name = "test_byte_array",
            .flags = 0
        },
        .len = 123
    };
    for(int i = 0;i < 123 -1; i++)
        t_byte_array->array[i] = i;
    t_byte_array->array[123-1] = -123; 

    string = enbt_to_snbt(t_byte_array, &written);
    SDL_free(t_byte_array);

    SDL_assert(string);
    SDL_Log("BYTE_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;

    struct eNBT_int_array* t_int_array = SDL_malloc(sizeof(struct eNBT_long_array) + sizeof(Sint64[5]));
    *t_int_array = (struct eNBT_int_array) {
        .data = {
            .type = TAG_Int_Array,
            .name_length = 14,
            .name = "test_int_array",
            .flags = 0
        },
        .len = 5
    };
    for(int i = 0;i < 5 -1; i++)
        t_int_array->array[i] = i;
    t_int_array->array[5-1] = 123456789; 

    string = enbt_to_snbt(t_int_array, &written);
    SDL_free(t_int_array);

    SDL_assert(string);
    SDL_Log("INT_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;
    struct eNBT_long_array* t_long_array = SDL_malloc(sizeof(struct eNBT_long_array) + sizeof(Sint64[10]));
    *t_long_array = (struct eNBT_long_array) {
        .data = {
            .type = TAG_Long_Array,
            .name_length = 15,
            .name = "test_long_array",
            .flags = 0
        },
        .len = 10
    };
    for(int i = 0;i < 10 -1; i++)
        t_long_array->array[i] = i;
    t_long_array->array[10-1] = 12345678987654321; 

    string = enbt_to_snbt(t_long_array, &written);
    SDL_free(t_long_array);

    SDL_assert(string);
    SDL_Log("LONG_ARRAY: %s",string);
    
    SDL_free(string);
    written = 0;

  }
}
#include <leinad/type/enbt.h>

struct eNBT_compound* enbt_create_compound(char* name, Uint16 name_length, Uint32 flags) {
    struct eNBT_compound *new = SDL_malloc(sizeof(struct eNBT_compound));
    
    if (new == NULL) goto ret;

    new->data = (struct eNBT_generic){
        .type = TAG_Compound,
        .name_length = name_length,
        .name = name,
        .flags = flags
    };
    new->size = 0;

ret:
    return new;
}

struct eNBT_list* enbt_create_list(Uint16 estimated_size, enum eNBT_Tag type, char* name, Uint16 name_length, Uint32 flags){
    struct eNBT_list *new;

    const int list_item_size[TAG_amount] = {
        1, // unused while in memory
        sizeof(struct eNBT_byte*),
        sizeof(struct eNBT_short*),
        sizeof(struct eNBT_int*),
        sizeof(struct eNBT_long*),
        sizeof(struct eNBT_float*),
        sizeof(struct eNBT_double*),
        sizeof(struct eNBT_byte_array*),
        sizeof(struct eNBT_string*),
        sizeof(struct eNBT_list*),
        sizeof(struct eNBT_compound*),
        sizeof(struct eNBT_int_array*),
        sizeof(struct eNBT_long_array*)
    };

    new = SDL_malloc(sizeof(struct eNBT_list));
    
    if (new == NULL) goto ret;
                                                /*lookup is redundant bc of being pointers*/
    new->list = SDL_malloc(SDL_max(ENBT_MIN_LIST_ALLOCATION, estimated_size) * list_item_size[type]);

    if (new->list == NULL) {SDL_free(new); goto ret;}


    new->data = (struct eNBT_generic){
        .type = TAG_List,
        .name_length = name_length,
        .name = name,
        .flags = flags
    };

    new->size = 0;
    new->current_capacity = SDL_max(ENBT_MIN_LIST_ALLOCATION,estimated_size);


ret:
    return new;
}



 

bool enbt_merge_value(void* target, const void* input) {

    // if input type is not a compound and matches target's type, replace its payload with the new one
    // otherwise, release old data and replace, or, in the case of compounds, replace all the matching fields
    switch(((struct eNBT_generic*)input)->type){
        case TAG_Byte:
            if (((struct eNBT_generic*)target)->type == TAG_Byte) 
                ((struct eNBT_byte*)target)->payload = ((struct eNBT_byte*)input)->payload;
            else {
                switch()
            }
    }
}


/**
 * @todo check if the compiler separates the first iteration of arrays because
 *       of the use of `i?"NORMAL":"FIRST"`
 */
char * enbt_to_snbt(const void *input, size_t* written){

    char * res = SDL_malloc(sizeof(char) * MAX_SNBT_CHARS);
    int idx = 0;
    int aux = 0;

    if (res == NULL) {idx = -1; goto ret;}

    // tag name
    res[idx] = '"';

    idx += SDL_utf8strlcpy(&res[idx+1],((struct eNBT_generic*)input)->name, ((struct eNBT_generic*)input)->name_length+1);
    res[idx+1] = '"'; res[idx+2] = ':'; idx+=3;
    int test;
    // tag content
    switch (((struct eNBT_generic*)input)->type){
        case TAG_Byte:
            aux = SDL_snprintf(&res[idx],6,"%db",((struct eNBT_byte*)input)->payload);
            break;
        case TAG_Short:
            aux = SDL_snprintf(&res[idx],8,"%ds",((struct eNBT_short*)input)->payload);
            break;
        case TAG_Int:
            aux = SDL_snprintf(&res[idx],12,"%d",((struct eNBT_int*)input)->payload);
            break;
        case TAG_Long:
            aux = SDL_snprintf(&res[idx],22,"%ldl",((struct eNBT_long*)input)->payload);
            break;
        case TAG_Float:
            aux = SDL_snprintf(&res[idx],21,"%f",((struct eNBT_float*)input)->payload);
            break;
        case TAG_Double:
            aux = SDL_snprintf(&res[idx],21,"%lf",((struct eNBT_double*)input)->payload);
            break;
        case TAG_Byte_Array:
            aux = 3; res[idx] = '['; res[idx+1] = 'B'; res[idx+2] = ';';
            for (int i = 0; i < ((struct eNBT_byte_array*)input)->len; i++)
                aux += SDL_snprintf(&res[idx+aux],7,i?",%db":"%db",((struct eNBT_byte_array*)input)->array[i]);
            res[idx+aux] = ']'; aux++;
            break;
        case TAG_Int_Array:
            aux = 3; res[idx] = '['; res[idx+1] = 'I'; res[idx+2] = ';';
            for (int i = 0; i < ((struct eNBT_int_array*)input)->len; i++)
                aux += SDL_snprintf(&res[idx+aux],13,i?",%d":"%d",((struct eNBT_int_array*)input)->array[i]);
            res[idx+aux] = ']'; aux++;
            break;
        case TAG_Long_Array:
            aux = 3; res[idx] = '['; res[idx+1] = 'L'; res[idx+2] = ';';
            for (int i = 0; i < ((struct eNBT_long_array*)input)->len; i++)
                aux += SDL_snprintf(&res[idx+aux],23,i?",%ldl":"%ldl",((struct eNBT_long_array*)input)->array[i]);
            res[idx+aux] = ']'; aux++;
            break;
        case TAG_List:
            // PENDING
            aux = 0;
            break;
        case TAG_Compound:
            // PENDING
            aux = 0;
            break;
        default:
            // PENDING
            aux = 0;
            break;

    }

    res[idx+aux] = 0;
    *written = (idx + aux +1);

    ret:
        return res;
}





char * enbt_from_snbt(const char* input, size_t len){
    return NULL;
}



struct eNBT_generic* enbt_parse_nbt(Uint8 data[], Sint32 length){
    return NULL;
}

struct eNBT_generic* enbt_parse_enbt(Uint8 data[], Sint32 length){
    return NULL;
}


void enbt_release_payload(void* enbt) {
    switch(((struct eNBT_generic*)enbt)->type) {
        default: break;
        
        case TAG_Byte_Array:
            SDL_free(((struct eNBT_byte_array*)enbt)->array);
            break;
        case TAG_Int_Array:
            SDL_free(((struct eNBT_int_array*)enbt)->array);
            break;
        case TAG_Long_Array:
            SDL_free(((struct eNBT_long_array*)enbt)->array);
            break;

        case TAG_String:
            SDL_free(((struct eNBT_string*)enbt)->array);
            break;

        case TAG_List:
            for (int i = ; i < ; i++) {

            }
            SDL_free(((struct eNBT_byte_array*)enbt)->array);
            break;
    }
}

void enbt_free(void* enbt) {
    return;
}


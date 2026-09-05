#include <leinad/type/enbt.h>

#define SNBT_PARSING_START_STACK_SIZE 100

struct eNBT_compound* enbt_create_compound(char* name, uint16_t name_length, uint32_t flags) {
    struct eNBT_compound *new = SDL_malloc(sizeof(struct eNBT_compound));
    
    if (new == NULL) goto ret;

    new->data = (struct eNBT_generic){
        .type = TAG_Compound,
        .name_length = name_length,
        .name = name,
        .flags = flags
    };
    new->size = 0;
    new->small = NULL;
    new->medium = NULL;
    new->big = NULL;

ret:
    return new;
}

/**
 * @todo optimize lists to be only the data itself and not an entire nbt component
 */
struct eNBT_list* enbt_create_list(uint16_t estimated_size, enum eNBT_Tag type, char* name, uint16_t name_length, uint32_t flags){
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
        .flags = flags | (ENBT_FLAG_LIST_TYPE & type)
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
                // PENDING
            }
    }

    return true;
}

// reallocs the memory on 1.5 geometric series until size fits in, assumes dir != NULL and size > 0
static int ensure_capacity(void** restrict dir, const size_t size, size_t* restrict current_max) {
    void* new_dir = NULL;
    bool req = false;

    while(size >= *current_max) {
        *current_max = (size_t)(*current_max * 1.5);
        req = true;
    }

    if (!req) return false;

    new_dir = SDL_realloc(*dir, *current_max);

    if (new_dir == NULL) return true;

    *dir = new_dir;
    return false;

}

/*auxiliary*/
static int create_snbt_of_compound(const void* input, char** res, size_t *current_max, int *idx, size_t *written);

/*auxiliary*/
static int create_snbt_of_content(const void* input, char** res, size_t *current_max, int *idx, size_t *written) {
    int aux = 0;
    switch (((struct eNBT_generic*)input)->type) {
        case TAG_Byte:
            if (ensure_capacity((void**)res,*written + 6,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],6,"%db",((struct eNBT_byte*)input)->payload);
            break;
        case TAG_Short:
            if (ensure_capacity((void**)res,*written + 8,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],8,"%ds",((struct eNBT_short*)input)->payload);
            break;
        case TAG_Int:
            if (ensure_capacity((void**)res,*written + 12,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],12,"%d",((struct eNBT_int*)input)->payload);
            break;
        case TAG_Long:
            if (ensure_capacity((void**)res,*written + 22,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],22,"%ldl",((struct eNBT_long*)input)->payload);
            break;
        case TAG_Float:
            if (ensure_capacity((void**)res,*written + 21,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],25,"%ff",((struct eNBT_float*)input)->payload);
            break;
        case TAG_Double:
            if (ensure_capacity((void**)res,*written + 21,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],25,"%lf",((struct eNBT_double*)input)->payload);
            break;
        case TAG_Byte_Array:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_byte_array*)input)->len * 7 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'B'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < ((struct eNBT_byte_array*)input)->len; i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],7,i?",%db":"%db",((struct eNBT_byte_array*)input)->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_Int_Array:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_byte_array*)input)->len * 13 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'I'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < ((struct eNBT_int_array*)input)->len; i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],13,i?",%d":"%d",((struct eNBT_int_array*)input)->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_Long_Array:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_byte_array*)input)->len * 23 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'L'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < ((struct eNBT_long_array*)input)->len; i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],23,i?",%ldl":"%ldl",((struct eNBT_long_array*)input)->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_String:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_string*)input)->size + 2,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],((struct eNBT_string*)input)->size+3,"\"%s\"",((struct eNBT_string*)input)->array);
            break;
        case TAG_List:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_list*)input)->size + 1,current_max)) goto fail;
            *written += 1 + ((struct eNBT_list*)input)->size; 
            (*res)[(*idx)] = '['; (*idx)++;
            for (int i = 0; i < ((struct eNBT_list*)input)->size; i++) {
                if (i) {(*res)[(*idx)] = ','; (*idx)++;}
                if (create_snbt_of_content(((struct eNBT_list*)input)->list[i],res,current_max,idx,written)) goto fail;
            }
            (*res)[(*idx)] = ']'; (*idx)++;
            break;
        case TAG_Compound:
            if (ensure_capacity((void**)res,*written + ((struct eNBT_compound*)input)->size + 1,current_max)) goto fail;
            *written += 1 + ((struct eNBT_compound*)input)->size; 
            (*res)[(*idx)] = '{'; (*idx)++;
            aux = 0;
            if (((struct eNBT_compound*)input)->small != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL && aux < ((struct eNBT_list*)input)->size; i++) {
                struct eNBT_NODE* temp = ((struct eNBT_compound*)input)->small[i];
                while(temp != NULL) {
                    if (aux) {
                        (*res)[(*idx)] = ',';
                        (*idx)++;
                    }
                    if (create_snbt_of_compound(temp->val,res,current_max,idx,written)) goto fail;
                    aux++; temp = temp->next;
                }
            }
            if (((struct eNBT_compound*)input)->medium != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && aux < ((struct eNBT_list*)input)->size; i++) {
                struct eNBT_NODE* temp = ((struct eNBT_compound*)input)->medium[i];
                while(temp != NULL) {
                    if (aux) {
                        (*res)[(*idx)] = ',';
                        (*idx)++;
                    }
                    if (create_snbt_of_compound(temp->val,res,current_max,idx,written)) goto fail;
                    aux++; temp = temp->next;
                }
            }
            if (((struct eNBT_compound*)input)->big != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_BIG && aux < ((struct eNBT_list*)input)->size; i++) {
                struct eNBT_NODE* temp = ((struct eNBT_compound*)input)->big[i];
                while(temp != NULL) {
                    if (aux) {
                        (*res)[(*idx)] = ',';
                        (*idx)++;
                    }
                    if (create_snbt_of_compound(temp->val,res,current_max,idx,written)) goto fail;
                    aux++; temp = temp->next;
                }
            }
            (*res)[(*idx)] = '}'; (*idx)++;
            break;
        default:
            // non-defined TAGs
            aux = 0;
            break;

    }
    if (((struct eNBT_generic*)input)->type != TAG_List && ((struct eNBT_generic*)input)->type != TAG_Compound) {
        *written += aux;
        *idx += aux;
    }
    return 0;

    fail:
        return 1;
}

/*auxiliary*/
static int create_snbt_of_compound(const void* input, char** res, size_t *current_max, int *idx, size_t *written){

    if (ensure_capacity((void**)&res,3+((struct eNBT_generic*)input)->name_length,current_max)) goto fail;

    // tag name
    (*res)[*idx] = '"';

    int aux = SDL_utf8strlcpy(&(*res)[*idx+1],((struct eNBT_generic*)input)->name, ((struct eNBT_generic*)input)->name_length+1);
    *idx += aux; *written += aux;
    (*res)[*idx+1] = '"'; (*res)[*idx+2] = ':';
    *idx+=3; *written += 3;

    // tag content
    if (create_snbt_of_content(input,res,current_max,idx,written)) goto fail;
    
    return 0;

    fail:
        return 1;

}


/**
 * @todo check if the compiler separates the first iteration of arrays because
 *       of the use of `i?"NORMAL":"FIRST"`
 */
char * enbt_to_snbt(const struct eNBT_generic*input, size_t* written){

    char * res = SDL_malloc(sizeof(char) * BASE_MAX_SNBT_CHARS);
    if (res == NULL) {*written = -1; goto ret;}

    size_t current_max = BASE_MAX_SNBT_CHARS;
    int idx = 0;
    *written = 0;

    if (ensure_capacity((void**)&res,4+((struct eNBT_generic*)input)->name_length,&current_max)) goto fail;

    // tag name
    res[idx] = '"';

    *written = idx += SDL_utf8strlcpy(&res[1],((struct eNBT_generic*)input)->name, ((struct eNBT_generic*)input)->name_length+1);
    res[idx+1] = '"'; res[idx+2] = ':'; 
    idx+=3; *written += 3;
    
    // tag content
    int status = create_snbt_of_content(input,&res,&current_max,&idx,written);
    if (status) goto fail;
    
    res[idx] = 0;
    (*written)++;

    // cut unused memory
    res = SDL_realloc(res,*written);

    ret:
        return res;

    fail:
        if (res != NULL) SDL_free(res);
        return NULL;
}

struct eNBT_generic* enbt_parse_nbt(uint8_t data[], int32_t length) {
    return NULL;
}

struct eNBT_generic* enbt_parse_enbt(uint8_t data[], int32_t length) {
    return NULL;
}

// HEY, REMEMBER, COMPOUNDS WILL NOT even HAVE small ALLOCATED AFTER THIS!
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
            switch(((struct eNBT_generic*)enbt)->flags & ENBT_FLAG_LIST_TYPE){
                case TAG_Byte:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Short:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                    enbt_free(((struct eNBT_short**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Float:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_float**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Double:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_double**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Byte_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_String:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_string**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_List:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_list**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Compound:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_compound**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
            }
            break;
        case TAG_Compound:
            if (((struct eNBT_compound*)enbt)->small != NULL) {
                uint64_t remaining = ((struct eNBT_compound*)enbt)->size;


                for(int i = 0; i < ENBT_COMPOUND_MAX_SMALL && remaining; i++ ) {
                    struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->small[i];
                    while (iter != NULL) {
                        enbt_free(((struct eNBT_compound*)enbt)->small[i]->val);
                        iter = iter->next;
                        remaining--;
                    }
                }
                SDL_free(((struct eNBT_compound*)enbt)->small);
                if (((struct eNBT_compound*)enbt)->medium != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && remaining; i++ ) {
                        struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->medium[i];
                        while (iter != NULL) {
                            enbt_free(((struct eNBT_compound*)enbt)->medium[i]->val);
                            iter = iter->next;
                            remaining--;
                        }
                    }
                    SDL_free(((struct eNBT_compound*)enbt)->medium);
                    if (((struct eNBT_compound*)enbt)->big != NULL) {
                        for(int i = 0; i < ENBT_COMPOUND_MAX_BIG && remaining; i++ ) {
                            struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->big[i];
                            while (iter != NULL) {
                                enbt_free(((struct eNBT_compound*)enbt)->big[i]->val);
                                iter = iter->next;
                                remaining--;
                            }
                        }
                        SDL_free(((struct eNBT_compound*)enbt)->big);
                    }

                }

            }
            break;
    }
}

void enbt_free(void* enbt) {
    if (enbt == NULL) return;
    switch(((struct eNBT_generic*)enbt)->type){
        default:
            break;
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
            switch(((struct eNBT_generic*)enbt)->flags & ENBT_FLAG_LIST_TYPE){
                case TAG_Byte:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Short:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                    enbt_free(((struct eNBT_short**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Float:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_float**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Double:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_double**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Byte_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long_Array:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_String:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_string**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_List:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_list**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Compound:
                    for (int i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_compound**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
            }
            SDL_free(((struct eNBT_list*)enbt)->list);
            break;
        case TAG_Compound:
            if (((struct eNBT_compound*)enbt)->small != NULL) {
                uint64_t remaining = ((struct eNBT_compound*)enbt)->size;


                for(int i = 0; i < ENBT_COMPOUND_MAX_SMALL && remaining; i++ ) {
                    struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->small[i];
                    while (iter != NULL) {
                        enbt_free(((struct eNBT_compound*)enbt)->small[i]->val);
                        iter = iter->next;
                        remaining--;
                    }
                }
                SDL_free(((struct eNBT_compound*)enbt)->small);
                if (((struct eNBT_compound*)enbt)->medium != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && remaining; i++ ) {
                        struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->medium[i];
                        while (iter != NULL) {
                            enbt_free(((struct eNBT_compound*)enbt)->medium[i]->val);
                            iter = iter->next;
                            remaining--;
                        }
                    }
                    SDL_free(((struct eNBT_compound*)enbt)->medium);
                    if (((struct eNBT_compound*)enbt)->big != NULL) {
                        for(int i = 0; i < ENBT_COMPOUND_MAX_BIG && remaining; i++ ) {
                            struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->big[i];
                            while (iter != NULL) {
                                enbt_free(((struct eNBT_compound*)enbt)->big[i]->val);
                                iter = iter->next;
                                remaining--;
                            }
                        }
                        SDL_free(((struct eNBT_compound*)enbt)->big);
                    }

                }

            }
            break;
    }

    SDL_free(((struct eNBT_generic*)enbt)->name);
    SDL_free(enbt);

    return;
}
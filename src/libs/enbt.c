#include <leinad/type/enbt.h>


/**
 * Creates a new empty nbt in-memory representation with the given @param type
 * 
 * > [!NOTE]
 * > Arrays and compounds won't have space allocated for child elements, while
 * > lists will have space for ENBT_MIN_LIST_ALLOCATION elements already. This
 * > is deliberate as arrays don't change in length after being set and
 * > compounds allocate size on the fly, while lists are realloc'd when needed
 * > as they are dynamic arrays.
 */
struct eNBT_generic* enbt_create_any(const char* restrict name, const uint16_t name_length, const uint32_t flags, const enum eNBT_Tag type) {
    struct eNBT_generic* target = NULL;
    char* new_name;

    // allocate per-type size
    switch (type) {
        case TAG_Byte:
            target = SDL_malloc(sizeof(struct eNBT_byte));
            break;
        case TAG_Short:
            target = SDL_malloc(sizeof(struct eNBT_short));
            break;
        case TAG_Int:
            target = SDL_malloc(sizeof(struct eNBT_int));
            break;
        case TAG_Long:
            target = SDL_malloc(sizeof(struct eNBT_long));
            break;
        case TAG_Float:
            target = SDL_malloc(sizeof(struct eNBT_float));
            break;
        case TAG_Double:
            target = SDL_malloc(sizeof(struct eNBT_double));
            break;
        case TAG_Byte_Array:
            target = SDL_malloc(sizeof(struct eNBT_byte_array));
            break;
        case TAG_String:
            target = SDL_malloc(sizeof(struct eNBT_string));
            break;
        case TAG_List:
            target = SDL_malloc(sizeof(struct eNBT_list));
            break;
        case TAG_Compound:
            target = SDL_malloc(sizeof(struct eNBT_compound));
            break;
        case TAG_Int_Array:
            target = SDL_malloc(sizeof(struct eNBT_int_array));
            break;
        case TAG_Long_Array:
            target = SDL_malloc(sizeof(struct eNBT_long_array));
            break;
        default:
            break;
    }

    if (target == NULL) return NULL;

    // allocate new name
    new_name = SDL_malloc(sizeof(char) * name_length);

    if (new_name == NULL) {
        SDL_free(target);
        return NULL;
    }

    // fill name
    for (int i = 0; i < name_length; i++) { new_name[i] = name[i]; }
    new_name[name_length] = '\0';

    // set generic data
    target->name = new_name;
    target->name_length = name_length;
    target->flags = flags;
    target->type = type;

    // set type-specific data
    switch (type) {
        case TAG_List:

            ((struct eNBT_list*)(target))->list = SDL_malloc(ENBT_MIN_LIST_ALLOCATION * sizeof(void*));

            if (((struct eNBT_list*)(target))->list == NULL) {
                SDL_free(target);
                SDL_free(new_name);
                return NULL;
            }


            ((struct eNBT_list*)(target))->size = 0;
            ((struct eNBT_list*)(target))->current_capacity = ENBT_MIN_LIST_ALLOCATION;
            
            break;

        case TAG_Compound:
            ((struct eNBT_compound*)(target))->size = 0;
            ((struct eNBT_compound*)(target))->small = NULL;
            ((struct eNBT_compound*)(target))->medium = NULL;
            ((struct eNBT_compound*)(target))->big = NULL;

            break;

        default:
            break;
    }

    return target;
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

    uint64_t remaining;

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
            remaining = ((struct eNBT_compound*)enbt)->size;
            if (((struct eNBT_compound*)enbt)->small != NULL) {


                for(int i = 0; i < ENBT_COMPOUND_MAX_SMALL && remaining; i++ ) {
                    struct eNBT_NODE* iter = ((struct eNBT_compound*)enbt)->small[i];
                    while (iter != NULL) {
                        enbt_free(((struct eNBT_compound*)enbt)->small[i]->val);
                        iter = iter->next;
                        remaining--;
                    }
                }
                SDL_free(((struct eNBT_compound*)enbt)->small);
            }
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
            }
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
            break;
    }
}

void enbt_free(void* enbt) {

    int i, j;

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
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Short:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_short**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Float:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_float**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Double:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_double**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Byte_Array:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_byte_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Int_Array:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_int_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Long_Array:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_long_array**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_String:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_string**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_List:
                    for (i = 0; i < ((struct eNBT_list*)enbt)->size; i++)
                        enbt_free(((struct eNBT_list**)((struct eNBT_list*)enbt)->list)[i]);
                    break;
                case TAG_Compound:
                    j = 0;
                    if (((struct eNBT_compound*)enbt)->small != NULL && j < ((struct eNBT_compound*)enbt)->size) {
                        for (i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++){
                            if (((struct eNBT_string**)((struct eNBT_compound*)enbt)->small)[i] != NULL) {
                                enbt_free(((struct eNBT_string**)((struct eNBT_compound*)enbt)->small)[i]);
                                ((struct eNBT_string**)((struct eNBT_compound*)enbt)->small)[i] = NULL;
                                if (++j >= ((struct eNBT_compound*)enbt)->size) break;
                            }
                        }
                    }
                    if (((struct eNBT_compound*)enbt)->medium != NULL && j < ((struct eNBT_compound*)enbt)->size) {
                        for (i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++){
                            if (((struct eNBT_string**)((struct eNBT_compound*)enbt)->medium)[i] != NULL) {
                                enbt_free(((struct eNBT_string**)((struct eNBT_compound*)enbt)->medium)[i]);
                                ((struct eNBT_string**)((struct eNBT_compound*)enbt)->medium)[i] = NULL;
                                if (++j >= ((struct eNBT_compound*)enbt)->size) break;
                            }
                        }
                    }
                    if (((struct eNBT_compound*)enbt)->big != NULL && j < ((struct eNBT_compound*)enbt)->size) {
                        for (i = 0; i < ENBT_COMPOUND_MAX_BIG; i++){
                            if (((struct eNBT_string**)((struct eNBT_compound*)enbt)->big)[i] != NULL) {
                                enbt_free(((struct eNBT_string**)((struct eNBT_compound*)enbt)->big)[i]);
                                ((struct eNBT_string**)((struct eNBT_compound*)enbt)->big)[i] = NULL;
                                if (++j >= ((struct eNBT_compound*)enbt)->size) break;
                            }
                        }
                    }
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

enum enbt_operation_validation enbt_merge_value(struct eNBT_compound* target, const struct eNBT_compound* input) {


    return success_enbt;
}

static int _hash(const char* str, const int len, const int max) {
    int ret = 0;
    for (int i = 0; i < len;i++) ret = (ret << 5) + str[i];
    return ret % max;
}

static struct eNBT_NODE** _find_compound_hash(struct eNBT_NODE** restrict array, const char* restrict key_str, const uint16_t key_len, const int MAX) {
    int idx = _hash(key_str, key_len, MAX);
    struct eNBT_NODE** element = &array[idx];
    while( *element != NULL) {
        if ((*element)->val->name_length != key_len) goto fail;
        for (idx = 0; idx < key_len; idx++) {
            if (key_str[idx] != (*element)->val->name[idx]) 
                goto fail;
        }

        return element;

        fail:
        element = &(*element)->next;
    }

    return element;
}

struct eNBT_NODE** enbt_compound_find_existing(const struct eNBT_compound* restrict compound, const char* restrict key_str, const uint16_t key_len) {
    
    int idx;
    struct eNBT_NODE** element = NULL;

    if (compound == NULL) return NULL;

    if (compound->small != NULL) {
        element = _find_compound_hash(compound->small,key_str, key_len,ENBT_COMPOUND_MAX_SMALL);
        if (element != NULL) return element;
    }

    if (compound->medium != NULL) {
        element = _find_compound_hash(compound->small,key_str, key_len,ENBT_COMPOUND_MAX_MEDIUM);
        if (element != NULL) return element;
    }

    if (compound->small != NULL) {
        element = _find_compound_hash(compound->small,key_str, key_len,ENBT_COMPOUND_MAX_BIG);
    }

    return element;
}

/**
 * Sets the payload of the enbt pointer held by @param target to match the
 * contents of @param input
 * 
 * On both error and success, a realocation may have happened, so any copies
 * of the target need to be updated using it's new value.
 * On error, the payload of the target enbt should be treated as released, and
 * you should assume a OOM error happened if the parameters were valid enbt.
 * @return success_enbt on success, any other error on failure.
 */
enum enbt_operation_validation enbt_set_value(void** target, const void* input) {

    struct eNBT_generic* new_ptr;
    enum enbt_operation_validation valid;


    // if input type is not a compound and matches target's type, replace its payload with the new one
    // otherwise, release old data and replace, or, in the case of compounds, replace all the matching fields
    switch ((*(struct eNBT_generic**)input)->type) {
        case TAG_Byte:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_byte));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Byte:
                    ((struct eNBT_byte*)*target)->payload = ((struct eNBT_byte*)input)->payload;
                    break;
            } break;
        case TAG_Short:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_short));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Short:
                    ((struct eNBT_short*)*target)->payload = ((struct eNBT_short*)input)->payload;
                    break;
            } break;
        case TAG_Int:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_int));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Int:
                    ((struct eNBT_int*)*target)->payload = ((struct eNBT_int*)input)->payload;
                    break;
            } break;
        case TAG_Long:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_long));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Long:
                    ((struct eNBT_long*)*target)->payload = ((struct eNBT_long*)input)->payload;
                    break;
            } break;
        case TAG_Float:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_float));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Short:
                    ((struct eNBT_float*)*target)->payload = ((struct eNBT_float*)input)->payload;
            } break;
        case TAG_Double:
            switch ((*(struct eNBT_generic**)target)->type) {
                default:
                    enbt_release_payload(*target);
                    new_ptr = SDL_realloc(*target, sizeof(struct eNBT_double));
                    if (new_ptr == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;
                    
                case TAG_Double:
                    ((struct eNBT_double*)*target)->payload = ((struct eNBT_double*)input)->payload;
            } break;
        case TAG_Byte_Array:
            enbt_release_payload(*target);
            if (((struct eNBT_generic*)target)->type != TAG_Byte_Array) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_byte_array));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            ((struct eNBT_byte_array*)*target)->array = SDL_malloc(sizeof(int8_t) * ((struct eNBT_byte_array*)input)->len);
            if (((struct eNBT_byte_array*)*target)->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < ((struct eNBT_byte_array*)input)->len; i++) {
                ((struct eNBT_byte_array*)*target)->array[i] = ((struct eNBT_byte_array*)input)->array[i];
            }

            ((struct eNBT_byte_array*)*target)->len = ((struct eNBT_byte_array*)input)->len;
            break;
        case TAG_Int_Array:
            enbt_release_payload(*target);
            if (((struct eNBT_generic*)target)->type != TAG_Int_Array) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_int_array));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            
            ((struct eNBT_int_array*)*target)->array = SDL_malloc(sizeof(int32_t) * ((struct eNBT_int_array*)input)->len);
            if (((struct eNBT_int_array*)*target)->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < ((struct eNBT_int_array*)input)->len; i++) {
                ((struct eNBT_int_array*)*target)->array[i] = ((struct eNBT_int_array*)input)->array[i];
            }

            ((struct eNBT_int_array*)*target)->len = ((struct eNBT_int_array*)input)->len;
            break;
        case TAG_Long_Array:
            enbt_release_payload(*target);
            if (((struct eNBT_generic*)target)->type != TAG_Long_Array) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_long_array));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            
            ((struct eNBT_long_array*)*target)->array = SDL_malloc(sizeof(int64_t) * ((struct eNBT_long_array*)input)->len);
            if (((struct eNBT_long_array*)*target)->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < ((struct eNBT_long_array*)input)->len; i++) {
                ((struct eNBT_long_array*)*target)->array[i] = ((struct eNBT_long_array*)input)->array[i];
            }

            ((struct eNBT_long_array*)*target)->len = ((struct eNBT_long_array*)input)->len;
            break;
        case TAG_String:
            enbt_release_payload(*target);
            if (((struct eNBT_generic*)target)->type != TAG_String) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_string));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            ((struct eNBT_string*)*target)->array = SDL_malloc(sizeof(char) * ((struct eNBT_string*)input)->size);
            if (((struct eNBT_string*)*target)->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < ((struct eNBT_string*)input)->size; i++) {
                ((struct eNBT_string*)*target)->array[i] = ((struct eNBT_string*)input)->array[i];
            }

            ((struct eNBT_string*)*target)->size = ((struct eNBT_string*)input)->size;
            break;
        case TAG_List:
            enbt_release_payload(*target);
            if (((struct eNBT_generic*)target)->type != TAG_List) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_list));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }

            ((struct eNBT_list*)*target)->list = SDL_malloc(sizeof(struct eNBT_generic*) * ((struct eNBT_list*)input)->current_capacity);
            if (((struct eNBT_list*)*target)->list == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < ((struct eNBT_list*)input)->size; i++) ((struct eNBT_list*)input)->list[i] = NULL;
            for (int i = 0; i < ((struct eNBT_list*)input)->size; i++) {
                if (((struct eNBT_list*)input)->list[i] != NULL) {

                    // create nbt object
                    ((struct eNBT_list*)*target)->list[i] = enbt_create_any(
                        ((struct eNBT_list*)input)->list[i]->name,
                        ((struct eNBT_list*)input)->list[i]->name_length,
                        ((struct eNBT_list*)input)->list[i]->flags,
                        ((struct eNBT_list*)input)->list[i]->type
                    );

                    // set contents of object
                    if (
                        (((struct eNBT_list*)*target)->list[i] == NULL)
                        || ((valid = enbt_set_value((void**)&((struct eNBT_list*)*target)->list[i], ((struct eNBT_list*)input)->list[i])) != success_enbt)
                    ) {
                        enbt_release_payload(*target);
                        return valid;
                    }
                }
            }

            ((struct eNBT_list*)*target)->size = ((struct eNBT_list*)input)->size;
            ((struct eNBT_list*)*target)->current_capacity = ((struct eNBT_list*)input)->current_capacity;
            break;
        case TAG_Compound:
            enbt_release_payload(*target);
            if (
                ((struct eNBT_generic*)target)->type != TAG_Compound
             && !((((struct eNBT_generic*)input)->flags & ENBT_FLAG_COMPOUND_REFCOUNT) < ENBT_FLAG_COMPOUND_REFCOUNT)
            ) {
                new_ptr = SDL_realloc(*target, sizeof(struct eNBT_compound));
                if (new_ptr == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            // if not over the limit of references, add one and copy pointer
            if ((((struct eNBT_generic*)input)->flags & ENBT_FLAG_COMPOUND_REFCOUNT) < ENBT_FLAG_COMPOUND_REFCOUNT) {
                enbt_free(*target);
                *target = ((struct eNBT_compound*)input);
                ((struct eNBT_compound*)*target)->data.flags++;
            }
            // otherwise, duplicate contents and set own count to 1
            else {

                struct eNBT_NODE** _small;
                struct eNBT_NODE** _medium;
                struct eNBT_NODE** _big;


                ((struct eNBT_compound*)*target)->small = NULL;
                ((struct eNBT_compound*)*target)->medium = NULL;
                ((struct eNBT_compound*)*target)->big = NULL;

                valid = success_enbt;


                // copy 'small' contents from input to target
                if (((struct eNBT_compound*)input)->small != NULL) {
                    
                    // allocate on target
                    _small = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_SMALL);
                    if (_small == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++) _small[i] = NULL;
                    for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++) {
                        
                        if (((struct eNBT_compound*)input)->small[i] == NULL) {
                            
                            struct eNBT_NODE** in = &((struct eNBT_compound*)input)->small[i];
                            struct eNBT_NODE** out = &_small[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) {
                                    ((struct eNBT_compound*)*target)->small = _small;
                                    goto __compound_cleanup;
                                }

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val->name, (*in)->val->name_length, (*in)->val->flags, (*in)->val->type);
                                if (
                                    ((*out)->val == NULL)
                                 || ((valid = enbt_set_value((void**) &(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    ((struct eNBT_compound*)*target)->small = _small;
                                    SDL_free(*out); *out = NULL;
                                    goto __compound_cleanup;
                                }

                                // iterate
                                in = &(*in)->next;
                                out = &(*out)->next;
                            }
                        }
                        else _small[i] = NULL;  
                    }
                }
                ((struct eNBT_compound*)*target)->small = _small;

                // copy 'medium' contents from input to target
                if (((struct eNBT_compound*)input)->medium != NULL) {
                    
                    // allocate on target
                    _medium = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_MEDIUM);
                    if (_medium == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM; i++) {
                        
                        if (((struct eNBT_compound*)input)->medium[i] == NULL) {
                            
                            struct eNBT_NODE** in = &((struct eNBT_compound*)input)->medium[i];
                            struct eNBT_NODE** out = &_medium[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) goto __compound_cleanup;

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val->name, (*in)->val->name_length, (*in)->val->flags, (*in)->val->type);
                                if (
                                    ((*out)->val == NULL)
                                 || ((valid = enbt_set_value((void**) &(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    ((struct eNBT_compound*)*target)->medium = _medium;
                                    SDL_free(*out); *out = NULL;
                                    goto __compound_cleanup;
                                }

                                // iterate
                                in = &(*in)->next;
                                out = &(*out)->next;
                            }
                        }
                        else _medium[i] = NULL;  
                    }
                }
                ((struct eNBT_compound*)*target)->medium = _medium;

                // copy 'big' contents from input to target
                if (((struct eNBT_compound*)input)->big != NULL) {
                    
                    // allocate on target
                    _big = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_BIG);
                    if (_big == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_BIG; i++) {
                        
                        if (((struct eNBT_compound*)input)->big[i] == NULL) {
                            
                            struct eNBT_NODE** in = &((struct eNBT_compound*)input)->big[i];
                            struct eNBT_NODE** out = &_big[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) goto __compound_cleanup;

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val->name, (*in)->val->name_length, (*in)->val->flags, (*in)->val->type);
                                if (
                                    ((*out)->val == NULL)
                                 || ((valid = enbt_set_value((void**) &(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    ((struct eNBT_compound*)*target)->big = _big;
                                    SDL_free(*out); *out = NULL;
                                    goto __compound_cleanup;
                                }

                                // iterate
                                in = &(*in)->next;
                                out = &(*out)->next;
                            }
                        }
                        else _big[i] = NULL;  
                    }
                }
                ((struct eNBT_compound*)*target)->big = _big;

                ((struct eNBT_compound*)*target)->data.flags = (((struct eNBT_compound*)input)->data.flags & ~ENBT_FLAG_COMPOUND_REFCOUNT) | 0b1;
            }


            return success_enbt;

          __compound_cleanup:
            enbt_release_payload(*target);
            if (valid == success_enbt) return err_enbt_out_of_memory;
            else return valid;
            break;
    }

    return true;
}

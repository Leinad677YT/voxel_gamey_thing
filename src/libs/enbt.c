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
enbt_t enbt_create_any(const char* restrict name, const uint16_t name_length, const uint32_t flags, const enum eNBT_Tag type) {
    enbt_t target; target._generic = NULL;
    char* new_name;

    // allocate per-type size
    switch (type) {
        case TAG_Byte:
            target._generic = SDL_malloc(sizeof(struct eNBT_byte));
            break;
        case TAG_Short:
            target._generic = SDL_malloc(sizeof(struct eNBT_short));
            break;
        case TAG_Int:
            target._generic = SDL_malloc(sizeof(struct eNBT_int));
            break;
        case TAG_Long:
            target._generic = SDL_malloc(sizeof(struct eNBT_long));
            break;
        case TAG_Float:
            target._generic = SDL_malloc(sizeof(struct eNBT_float));
            break;
        case TAG_Double:
            target._generic = SDL_malloc(sizeof(struct eNBT_double));
            break;
        case TAG_Byte_Array:
            target._generic = SDL_malloc(sizeof(struct eNBT_byte_array));
            break;
        case TAG_String:
            target._generic = SDL_malloc(sizeof(struct eNBT_string));
            break;
        case TAG_List:
            target._generic = SDL_malloc(sizeof(struct eNBT_list));
            break;
        case TAG_Compound:
            target._generic = SDL_malloc(sizeof(struct eNBT_compound));
            break;
        case TAG_Int_Array:
            target._generic = SDL_malloc(sizeof(struct eNBT_int_array));
            break;
        case TAG_Long_Array:
            target._generic = SDL_malloc(sizeof(struct eNBT_long_array));
            break;
        default:
            break;
    }

    if (target._generic == NULL) return target;

    // allocate new name
    new_name = SDL_malloc(sizeof(char) * (name_length + 1));

    if (new_name == NULL) {
        SDL_free(target._generic);
        return (enbt_t)((struct eNBT_generic*)NULL);
    }

    // fill name
    for (int i = 0; i < name_length; i++) { new_name[i] = name[i]; }
    new_name[name_length] = '\0';

    // set generic data
    target._generic->name = new_name;
    target._generic->name_length = name_length;
    target._generic->flags = flags;
    target._generic->type = type;

    // set type-specific data
    switch (type) {

        case TAG_Byte_Array:
            target._byte_array->array = NULL;
            target._byte_array->len = 0;            
            break;
        case TAG_Int_Array:
            target._int_array->array = NULL;
            target._int_array->len = 0;            
            break;
        case TAG_Long_Array:
            target._long_array->array = NULL;
            target._long_array->len = 0;            
            break;

        case TAG_List:

            target._list->list = SDL_malloc(ENBT_MIN_LIST_ALLOCATION * sizeof(void*));

            if (target._list->list == NULL) {
                SDL_free(target._generic);
                SDL_free(new_name);
                return (enbt_t)((struct eNBT_generic*)NULL);
            }


            target._list->size = 0;
            target._list->current_capacity = ENBT_MIN_LIST_ALLOCATION * sizeof(void*);
            
            break;

        case TAG_Compound:

            target._compound->payload = SDL_malloc(sizeof(struct eNBT_COMPOUND_PAYLOAD));
            if (target._compound->payload == NULL) {
                SDL_free(target._generic);
                SDL_free(new_name);
                return (enbt_t)((struct eNBT_generic*)NULL);
            }

            target._compound->payload->size = 0;
            target._compound->payload->refcount = 0;
            target._compound->payload->small = NULL;
            target._compound->payload->medium = NULL;
            target._compound->payload->big = NULL;

            break;

        default:
            break;
    }

    return target;
}

// reallocs the memory on 1.5 geometric series until size fits in, assumes dir != NULL and size > 0.
//  
// returns true on failure and false on success
static bool ensure_capacity(void** restrict dir, const size_t size, uint32_t* restrict current_max) {
    void* new_dir = NULL;
    bool req = false;

    while(size >= *current_max) {
        *current_max = SDL_ceil(*current_max * 1.5);
        req = true;
    }

    if (!req) return false;

    new_dir = SDL_realloc(*dir, *current_max);

    if (new_dir == NULL) return true;

    *dir = new_dir;
    return false;

}

/*auxiliary*/
static int create_snbt_of_compound(const enbt_t input, char** res, uint32_t *current_max, int *idx, size_t *written);

/*auxiliary*/
static int create_snbt_of_content(const enbt_t input, char** res, uint32_t *current_max, int *idx, size_t *written) {
    int aux = 0;
    switch (input._generic->type) {
        case TAG_Byte:
            if (ensure_capacity((void**)res,*written + 6,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],6,"%db",input._byte->payload);
            break;
        case TAG_Short:
            if (ensure_capacity((void**)res,*written + 8,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],8,"%ds",input._short->payload);
            break;
        case TAG_Int:
            if (ensure_capacity((void**)res,*written + 12,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],12,"%d",input._int->payload);
            break;
        case TAG_Long:
            if (ensure_capacity((void**)res,*written + 22,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],22,"%ldl",input._long->payload);
            break;
        case TAG_Float:
            if (ensure_capacity((void**)res,*written + 21,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],25,"%ff",input._float->payload);
            break;
        case TAG_Double:
            if (ensure_capacity((void**)res,*written + 21,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],25,"%lf",input._double->payload);
            break;
        case TAG_Byte_Array:
            if (ensure_capacity((void**)res,*written + input._byte_array->len * 7 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'B'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < input._byte_array->len / sizeof(uint8_t); i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],7,i?",%dB":"%dB",input._byte_array->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_Int_Array:
            if (ensure_capacity((void**)res,*written + input._int_array->len * 13 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'I'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < input._int_array->len / sizeof(uint32_t); i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],13,i?",%d":"%d",input._int_array->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_Long_Array:
            if (ensure_capacity((void**)res,*written + input._long_array->len * 23 + 4,current_max)) goto fail;
            aux = 3; (*res)[(*idx)] = '['; (*res)[(*idx)+1] = 'L'; (*res)[(*idx)+2] = ';';
            for (int i = 0; i < input._long_array->len / sizeof(uint64_t); i++)
                aux += SDL_snprintf(&(*res)[(*idx + aux)],23,i?",%ldL":"%ldL",input._long_array->array[i]);
            (*res)[(*idx)+aux] = ']'; aux++;
            break;
        case TAG_String:
            if (ensure_capacity((void**)res,*written + input._string->size + 2,current_max)) goto fail;
            aux = SDL_snprintf(&(*res)[(*idx)],input._string->size+3,"\"%s\"",input._string->array);
            break;
        case TAG_List:
            if (ensure_capacity((void**)res,*written + input._list->size + 1,current_max)) goto fail;
            *written += 1 + input._list->size; 
            (*res)[(*idx)] = '['; (*idx)++;
            for (int i = 0; i < input._list->size; i++) {
                if (i) {(*res)[(*idx)] = ','; (*idx)++;}
                if (create_snbt_of_content(input._list->list[i],res,current_max,idx,written)) goto fail;
            }
            (*res)[(*idx)] = ']'; (*idx)++;
            break;
        case TAG_Compound:
            if (ensure_capacity((void**)res,*written + input._compound->payload->size + 1,current_max)) goto fail;
            *written += 1 + input._compound->payload->size; 
            (*res)[(*idx)] = '{'; (*idx)++;
            aux = 0;
            if (input._compound->payload->small != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL && aux < input._compound->payload->size; i++) {
                struct eNBT_NODE* temp = input._compound->payload->small[i];
                while(temp != NULL) {
                    if (aux) {
                        (*res)[(*idx)] = ',';
                        (*idx)++;
                    }
                    if (create_snbt_of_compound(temp->val,res,current_max,idx,written)) goto fail;
                    aux++; temp = temp->next;
                }
            }
            if (input._compound->payload->medium != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && aux < input._compound->payload->size; i++) {
                struct eNBT_NODE* temp = input._compound->payload->medium[i];
                while(temp != NULL) {
                    if (aux) {
                        (*res)[(*idx)] = ',';
                        (*idx)++;
                    }
                    if (create_snbt_of_compound(temp->val,res,current_max,idx,written)) goto fail;
                    aux++; temp = temp->next;
                }
            }
            if (input._compound->payload->big != NULL)
             for (int i = 0; i < ENBT_COMPOUND_MAX_BIG && aux < input._compound->payload->size; i++) {
                struct eNBT_NODE* temp = input._compound->payload->big[i];
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
    if (input._generic->type != TAG_List && input._generic->type != TAG_Compound) {
        *written += aux;
        *idx += aux;
    }
    return 0;

    fail:
        return 1;
}

/*auxiliary*/
static int create_snbt_of_compound(const enbt_t input, char** res, uint32_t *current_max, int *idx, size_t *written){

    if (ensure_capacity((void**)&res,3+input._generic->name_length,current_max)) goto fail;

    // tag name
    (*res)[*idx] = '"';

    int aux = SDL_utf8strlcpy(&(*res)[*idx+1],input._generic->name, input._generic->name_length+1);
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
char * enbt_to_snbt(const enbt_t input, size_t* written){

    char * res = SDL_malloc(sizeof(char) * BASE_MAX_SNBT_CHARS);
    if (res == NULL) {*written = -1; goto ret;}

    uint32_t current_max = BASE_MAX_SNBT_CHARS;
    int idx = 0;
    *written = 0;

    if (ensure_capacity((void**)&res,4+input._generic->name_length,&current_max)) goto fail;

    // tag name
    res[idx] = '"';

    *written = idx += SDL_utf8strlcpy(&res[1],input._generic->name, input._generic->name_length+1);
    res[idx+1] = '"'; res[idx+2] = ':'; 
    idx+=3; *written += 3;
    
    // tag content
    int status = create_snbt_of_content(input,&res,&current_max,&idx,written);
    if (status) goto fail;
    
    res[idx] = 0;
    (*written)++;

    // cut unused memory
    res = SDL_realloc(res,*written + 1);
    res[*written] = '\0';

    ret:
        return res;

    fail:
        if (res != NULL) SDL_free(res);
        return NULL;
}

struct eNBT_generic* enbt_parse_nbt(uint8_t data[], uint32_t length) {
    return NULL;
}

struct eNBT_generic* enbt_parse_enbt(uint8_t data[], uint32_t length) {
    return NULL;
}

/**
 * Releases the payload of the pointer to enbt held by @param enbt
 * 
 * If @param enbt is a compound with more references, it will remove a
 * reference, create an empty compound and set it's pointer on @param enbt
 *
 * @return success_enbt on success, err_enbt_out_of_memory when unable to
 * allocate an empty compound for the provided multi-referenced compound.
 */
enum enbt_operation_validation enbt_release_payload(enbt_t enbt) {

    uint64_t remaining;

    switch(enbt._generic->type) {
        default:
            break;
        case TAG_Byte_Array:
            SDL_free(enbt._byte_array->array);
            break;
        case TAG_Int_Array:
            SDL_free(enbt._int_array->array);
            break;
        case TAG_Long_Array:
            SDL_free(enbt._long_array->array);
            break;
        case TAG_String:
            SDL_free(enbt._string->array);
            break;
        case TAG_List:
            for (int i = 0; i < enbt._list->size; i++)
                enbt_free(enbt._list->list[i]);
            SDL_free(enbt._list->list);
            break;
        case TAG_Compound:

            // if more references exist, remove one and create new empty
            if (enbt._compound->payload->refcount > 1) {
                enbt._compound->payload->refcount--;

                enbt._compound->payload = SDL_malloc(sizeof(struct eNBT_COMPOUND_PAYLOAD));
                if (enbt._compound->payload == NULL) return err_enbt_out_of_memory;

                enbt._compound->payload->size = 0;
                enbt._compound->payload->refcount = 0;
                enbt._compound->payload->small = NULL;
                enbt._compound->payload->big = NULL;
                enbt._compound->payload->medium = NULL;
                
            }
            
            // otherwise free contents
            else {
                remaining = enbt._compound->payload->size;
                if (enbt._compound->payload->small != NULL) {


                    for(int i = 0; i < ENBT_COMPOUND_MAX_SMALL && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->small[i];
                        while (iter != NULL) {
                            enbt_free(enbt._compound->payload->small[i]->val);
                            iter = iter->next;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->small);
                    enbt._compound->payload->small = NULL;
                }
                if (enbt._compound->payload->medium != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->medium[i];
                        while (iter != NULL) {
                            enbt_free(enbt._compound->payload->medium[i]->val);
                            iter = iter->next;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->medium);
                    enbt._compound->payload->medium = NULL;
                }
                if (enbt._compound->payload->big != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_BIG && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->big[i];
                        while (iter != NULL) {
                            enbt_free(enbt._compound->payload->big[i]->val);
                            iter = iter->next;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->big);
                    enbt._compound->payload->big = NULL;
                }
            }
            break;
    }
    return success_enbt;
}

/**
 * Releases the resources of @param enbt
 * 
 * @param enbt will not be valid after this call. If it was a multi-referenced
 * compound, it's reference count will be decreased by 1.
 */
void enbt_free(enbt_t enbt) {

    if (enbt._generic == NULL) return;
    
    switch(enbt._generic->type) {
        default:
            enbt_release_payload(enbt);
            break;

        case TAG_Compound:

            // if more references exist, remove one
            if (enbt._compound->payload->refcount > 1) enbt._compound->payload->refcount--;

            // otherwise free contents
            else {
                uint64_t remaining  = enbt._compound->payload->size;
                if (enbt._compound->payload->small != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_SMALL && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->small[i];
                        struct eNBT_NODE* aux_node;
                        
                        while (iter != NULL) {
                            enbt_free(iter->val);
                            aux_node = iter->next;
                            SDL_free(iter);
                            iter = aux_node;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->small);
                }

                if (enbt._compound->payload->medium != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->medium[i];
                        struct eNBT_NODE* aux_node;

                        while (iter != NULL) {
                            enbt_free(iter->val);
                            aux_node = iter->next;
                            SDL_free(iter);
                            iter = aux_node;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->medium);
                }

                if (enbt._compound->payload->big != NULL) {
                    for(int i = 0; i < ENBT_COMPOUND_MAX_BIG && remaining; i++ ) {
                        struct eNBT_NODE* iter = enbt._compound->payload->big[i];
                        struct eNBT_NODE* aux_node;
                        
                        while (iter != NULL) {
                            enbt_free(iter->val);
                            aux_node = iter->next;
                            SDL_free(iter);
                            iter = aux_node;
                            remaining--;
                        }
                    }
                    SDL_free(enbt._compound->payload->big);
                }
                SDL_free(enbt._compound->payload);
            }
    }

    SDL_free(enbt._generic->name);
    SDL_free(enbt._generic);

    return;
}

enum enbt_operation_validation enbt_merge_value(struct eNBT_compound* target, const struct eNBT_compound* input) {


    return success_enbt;
}

static uint _hash(const char* str, const int len, const uint max) {
    int ret = 0;
    for (int i = 0; i < len;i++) ret = (ret << 5) + str[i];
    return ret % max;
}

static struct eNBT_NODE** _find_compound_hash(struct eNBT_NODE** restrict array, const char* restrict key_str, const uint16_t key_len, const uint MAX) {
    uint idx = _hash(key_str, key_len, MAX);
    struct eNBT_NODE** element = &array[idx];
    while(*element != NULL) {
        if ((*element)->val._generic->name_length != key_len) goto fail;
        for (idx = 0; idx < key_len; idx++) {
            if (key_str[idx] != (*element)->val._generic->name[idx]) 
                goto fail;
        }

        return element;

        fail:
        element = &(*element)->next;
    }

    return element;
}

struct eNBT_NODE** enbt_compound_find_pos(struct eNBT_compound* restrict compound, const char* restrict key_str, const uint16_t key_len) {
        int idx;
    struct eNBT_NODE** ptr_small = NULL;
    struct eNBT_NODE** ptr_medium = NULL;
    struct eNBT_NODE** ptr_big = NULL;

    if (compound == NULL) return NULL;

    if (compound->payload->small != NULL) {
        ptr_small = _find_compound_hash(compound->payload->small,key_str, key_len,ENBT_COMPOUND_MAX_SMALL);
        if (ptr_small != NULL) return ptr_small;
    }

    if (compound->payload->medium != NULL) {
        ptr_medium = _find_compound_hash(compound->payload->medium,key_str, key_len,ENBT_COMPOUND_MAX_MEDIUM);
        if (ptr_medium != NULL) return ptr_medium;
    }

    if (compound->payload->big != NULL) {
        ptr_big = _find_compound_hash(compound->payload->big,key_str, key_len,ENBT_COMPOUND_MAX_BIG);
        if (ptr_medium != NULL) return ptr_big;
    }

    if (compound->payload->size < ENBT_COMPOUND_RANGE_SMALL) {
        if (compound->payload->small == NULL) {
            compound->payload->small = SDL_malloc(sizeof(struct eNBT_generic*) * ENBT_COMPOUND_MAX_SMALL);
            if (compound->payload->small == NULL) return NULL;
            for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++) compound->payload->small[i] = NULL;
            ptr_small = _find_compound_hash(compound->payload->small, key_str, key_len, ENBT_COMPOUND_MAX_SMALL);
        }
        return ptr_small;
    }
    else if (compound->payload->size < ENBT_COMPOUND_RANGE_MEDIUM) {
        if (compound->payload->medium == NULL) {
            compound->payload->medium = SDL_malloc(sizeof(struct eNBT_generic*) * ENBT_COMPOUND_MAX_MEDIUM);
            if (compound->payload->medium == NULL) return NULL;
            for (int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM; i++) compound->payload->medium[i] = NULL;
            ptr_medium = _find_compound_hash(compound->payload->medium, key_str, key_len, ENBT_COMPOUND_MAX_MEDIUM);
        }
        return ptr_medium;
    }
    else {
        if (compound->payload->big == NULL) {
            compound->payload->big = SDL_malloc(sizeof(struct eNBT_generic*) * ENBT_COMPOUND_MAX_BIG);
            if (compound->payload->big == NULL) return NULL;
            for (int i = 0; i < ENBT_COMPOUND_MAX_BIG; i++) compound->payload->big[i] = NULL;
            ptr_big = _find_compound_hash(compound->payload->big, key_str, key_len, ENBT_COMPOUND_MAX_BIG);
        }
        return ptr_big;   
    }
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
enum enbt_operation_validation enbt_set_value(enbt_t* target, const enbt_t input) {

    enbt_t new_ptr;
    enum enbt_operation_validation valid, valid_2;

    if ((valid = enbt_release_payload((enbt_t)target->_generic)) != success_enbt) return valid;

    // if input type is not a compound and matches target's type, replace its payload with the new one
    // otherwise, release old data and replace, or, in the case of compounds, replace all the matching fields
    switch (input._generic->type) {
        case TAG_Byte:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_byte));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Byte:
                    target->_byte->payload = input._byte->payload;
                    break;
            } break;
        case TAG_Short:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_short));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Short:
                    target->_short->payload = input._short->payload;
                    break;
            } break;
        case TAG_Int:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_int));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Int:
                    target->_int->payload = input._int->payload;
                    break;
            } break;
        case TAG_Long:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_long));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Long:
                    target->_long->payload = input._long->payload;
                    break;
            } break;
        case TAG_Float:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_float));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Float:
                    target->_float->payload = input._float->payload;
            } break;
        case TAG_Double:
            switch (target->_generic->type) {
                default:
                    new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_double));
                    if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                    *target = new_ptr;

                case TAG_Double:
                    target->_double->payload = input._double->payload;
            } break;
        case TAG_Byte_Array:
            if (target->_generic->type != TAG_Byte_Array) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_byte_array));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            target->_byte_array->array = SDL_malloc(sizeof(int8_t) * input._byte_array->len);
            if (target->_byte_array->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < input._byte_array->len; i++) {
                target->_byte_array->array[i] = input._byte_array->array[i];
            }

            target->_byte_array->len = input._byte_array->len;
            break;
        case TAG_Int_Array:
            if (target->_generic->type != TAG_Int_Array) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_int_array));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            target->_int_array->array = SDL_malloc(sizeof(int32_t) * input._int_array->len);
            if (target->_int_array->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < input._int_array->len; i++) {
                target->_int_array->array[i] = input._int_array->array[i];
            }

            target->_int_array->len = input._int_array->len;
            break;
        case TAG_Long_Array:
            if (target->_generic->type != TAG_Long_Array) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_long_array));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            target->_long_array->array = SDL_malloc(sizeof(int64_t) * input._long_array->len);
            if (target->_long_array->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < input._long_array->len; i++) {
                target->_long_array->array[i] = input._long_array->array[i];
            }

            target->_long_array->len = input._long_array->len;
            break;
        case TAG_String:
            if (target->_generic->type != TAG_String) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_string));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            target->_string->array = SDL_malloc(sizeof(char) * input._string->size);
            if (target->_string->array == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < input._string->size; i++) {
                target->_string->array[i] = input._string->array[i];
            }

            target->_string->size = input._string->size;
            break;
        case TAG_List:
            if (target->_generic->type != TAG_List) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_list));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }

            target->_list->list = SDL_malloc(input._list->current_capacity);
            if (target->_list->list == NULL) return err_enbt_out_of_memory;

            for (int i = 0; i < input._list->size; i++) input._list->list[i]._generic = NULL;
            for (int i = 0; i < input._list->size; i++) {
                if (input._list->list[i]._generic != NULL) {

                    // create nbt object
                    target->_list->list[i] = enbt_create_any(
                        input._list->list[i]._generic->name,
                        input._list->list[i]._generic->name_length,
                        input._list->list[i]._generic->flags,
                        input._list->list[i]._generic->type
                    );

                    // set contents of object
                    if (
                        (target->_list->list[i]._generic == NULL)
                        || ((valid = enbt_set_value(&target->_list->list[i], input._list->list[i])) != success_enbt)
                    ) {
                        if ((valid_2 = enbt_release_payload(*target)) != success_enbt) return valid_2;
                        return valid;
                    }
                }
            }

            target->_list->size = input._list->size;
            target->_list->current_capacity = input._list->current_capacity;
            break;
        case TAG_Compound:
            if (
                ((struct eNBT_generic*)target)->type != TAG_Compound
             || !((input._compound->payload->refcount) < ENBT_COMPOUND_MAX_REFCOUNT)
            ) {
                new_ptr._generic = SDL_realloc(target->_generic, sizeof(struct eNBT_compound));
                if (new_ptr._generic == NULL) return err_enbt_out_of_memory;
                *target = new_ptr;
            }
            // if not over the limit of references, add one and copy pointer
            if ((input._compound->payload->refcount) < ENBT_COMPOUND_MAX_REFCOUNT) {
                target->_compound->payload = input._compound->payload;
                target->_compound->payload->refcount++;
            }
            // otherwise, duplicate contents and set own count to 1
            else {

                struct eNBT_NODE** _small;
                struct eNBT_NODE** _medium;
                struct eNBT_NODE** _big;


                target->_compound->payload->small = NULL;
                target->_compound->payload->medium = NULL;
                target->_compound->payload->big = NULL;

                target->_compound->payload = SDL_malloc(sizeof(struct eNBT_COMPOUND_PAYLOAD));
                if (target->_compound->payload == NULL) goto __compound_cleanup;
                valid = success_enbt;


                // copy 'small' contents from input to target
                if (input._compound->payload->small != NULL) {
                    
                    // allocate on target
                    _small = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_SMALL);
                    if (_small == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++) _small[i] = NULL;
                    for (int i = 0; i < ENBT_COMPOUND_MAX_SMALL; i++) {
                        
                        if (input._compound->payload->small[i] == NULL) {
                            
                            struct eNBT_NODE** in = &input._compound->payload->small[i];
                            struct eNBT_NODE** out = &_small[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) {
                                    target->_compound->payload->small = _small;
                                    goto __compound_cleanup;
                                }

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val._generic->name, (*in)->val._generic->name_length, (*in)->val._generic->flags, (*in)->val._generic->type);
                                if (
                                    ((*out)->val._generic == NULL)
                                 || ((valid = enbt_set_value(&(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    target->_compound->payload->small = _small;
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
                target->_compound->payload->small = _small;

                // copy 'medium' contents from input to target
                if (input._compound->payload->medium != NULL) {
                    
                    // allocate on target
                    _medium = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_MEDIUM);
                    if (_medium == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_MEDIUM; i++) {
                        
                        if (input._compound->payload->medium[i] == NULL) {
                            
                            struct eNBT_NODE** in = &input._compound->payload->medium[i];
                            struct eNBT_NODE** out = &_medium[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) goto __compound_cleanup;

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val._generic->name, (*in)->val._generic->name_length, (*in)->val._generic->flags, (*in)->val._generic->type);
                                if (
                                    ((*out)->val._generic == NULL)
                                 || ((valid = enbt_set_value(&(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    target->_compound->payload->medium = _medium;
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
                target->_compound->payload->medium = _medium;

                // copy 'big' contents from input to target
                if (input._compound->payload->big != NULL) {
                    
                    // allocate on target
                    _big = SDL_malloc(sizeof(struct eNBT_NODE*) * ENBT_COMPOUND_MAX_BIG);
                    if (_big == NULL) goto __compound_cleanup;

                    // set contents on target at every index
                    for (int i = 0; i < ENBT_COMPOUND_MAX_BIG; i++) {
                        
                        if (input._compound->payload->big[i] == NULL) {
                            
                            struct eNBT_NODE** in = &input._compound->payload->big[i];
                            struct eNBT_NODE** out = &_big[i];
                            
                            // iterate on index
                            while (*in != NULL) {
                                // allocate node
                                *out = SDL_malloc(sizeof(struct eNBT_NODE));
                                if (out == NULL) goto __compound_cleanup;

                                // create and set recursively
                                (*out)->next = NULL;
                                (*out)->val = enbt_create_any((*in)->val._generic->name, (*in)->val._generic->name_length, (*in)->val._generic->flags, (*in)->val._generic->type);
                                if (
                                    ((*out)->val._generic == NULL)
                                 || ((valid = enbt_set_value(&(*out)->val, (*in)->val)) != success_enbt)
                                ) {
                                    target->_compound->payload->big = _big;
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
                target->_compound->payload->big = _big;

                target->_compound->data.flags = (input._compound->payload->refcount) = 1;
            }


            return success_enbt;

          __compound_cleanup:
            if ((valid_2 = enbt_release_payload(*target)) != success_enbt) return valid_2;
            if (valid == success_enbt) return err_enbt_out_of_memory;
            else return valid;
            break;
    }

    return true;
}

/**
 * Moves @param input into @param target
 * 
 * Useful for inserting data into compounds as an auxiliary function
 */
enum enbt_operation_validation enbt_compound_set_insert(struct eNBT_compound* target, enbt_t input) {

    if (target == NULL || input._generic == NULL) return err_enbt_invalid_operation;

    struct eNBT_NODE** node = enbt_compound_find_pos(target,input._generic->name,input._generic->name_length);

    if (*node == NULL) {
        // create node
        *node = SDL_malloc(sizeof(struct eNBT_NODE));
        if (*node == NULL) return err_enbt_out_of_memory;
        
        // fill node
        (*node)->next = NULL;

        target->payload->size++;
    }
    else {
        enbt_free((*node)->val);
    }
    
    (*node)->val = input;
    return success_enbt;
}

/**
 * Moves @param input into @param target
 * 
 * Useful for inserting data into lists as an auxiliary function
 */
enum enbt_operation_validation enbt_list_append(struct eNBT_list* target, enbt_t input) {

    if (target == NULL || input._generic == NULL) return err_enbt_invalid_operation;

    if (ensure_capacity((void**)&target->list, sizeof(struct eNBT_generic*) * (target->size +1), &target->current_capacity)) {
        return err_enbt_out_of_memory;
    }

    target->list[target->size++] = input;

    return success_enbt;
}
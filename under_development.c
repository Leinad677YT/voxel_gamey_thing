#include <stdio.h>

#include <leinad/type/enbt.h>
#include "src/libs/enbt.c"

#define input_size 100
#define scanfformat(input_size) "%" #input_size "s"

// 0 = INVALID, 1 = ALWAYS VALID, 2 = CANNOT START REGULAR UNQUOTED STRINGS
const char valid_unquoted_string_char[0x100] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,2,0,2,2,0,
    2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int check_valid_snbt_string_char(unsigned char c) {
    return valid_unquoted_string_char[c];
}


struct snbt_return_key {
    enum string_parsing_return valid;
    int start_idx;
    int key_len;
    int new_idx;
};

struct snbt_return_value {
    enum string_parsing_return valid;
    struct eNBT_generic* enbt;
};

/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra char available after len) found on @param input
 */
static enum string_parsing_return is_valid_key_quote_double(const char* input, int len, bool empty_is_valid) {

    int i, mid_search = 0;

    if (len == 0 && !empty_is_valid) return err_string_empty;
    for (i = 0; i < len; i++) {
        switch (input[i]) {
            case '\"':
                return err_string_quote_not_escaped;
                break;
            case '\\':
                switch (input[i+1]) {
                    case '\"':
                    case '\'':
                    case '\\':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 's':
                    case 't':
                        i++;
                        continue;
                    case 'x':
                        i+=3;
                        continue;
                    case 'u':
                        i+=5;
                        continue;
                    case 'U':
                        i+=9;
                        continue;
                    case 'N':
                        if (len < i+2 || input[i+2] != '{') return err_string_incomplete_escaping;
                        for (i+=3; i < len; i++) if (input[i] == '}') {mid_search = 1; break;}
                        if (mid_search) return err_string_incomplete_escaping;
                        continue;
                    default:
                        return err_string_invalid_escaping;
                }
        }
    }
    return success_string;
}

/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra char available after len) found on @param input
 */
static enum string_parsing_return is_valid_key_quote_simple(const char* input, int len, bool empty_is_valid) {

    int i, mid_search = 0;

    if (len == 0 && !empty_is_valid) return err_string_empty;
    for (i = 0; i < len; i++) {
        switch (input[i]) {
            case '\'':
                return err_string_quote_not_escaped;
                break;
            case '\\':
                switch (input[i+1]) {
                    case '\"':
                    case '\'':
                    case '\\':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 's':
                    case 't':
                        i++;
                        continue;
                    case 'x':
                        i+=3;
                        continue;
                    case 'u':
                        i+=5;
                        continue;
                    case 'U':
                        i+=9;
                        continue;
                    case 'N':
                        if (len < i+2 || input[i+2] != '{') return err_string_incomplete_escaping;
                        for (i+=3; i < len; i++) if (input[i] == '}') {mid_search = 1; break;}
                        if (mid_search) return err_string_incomplete_escaping;
                        continue;
                    default:
                        return err_string_invalid_escaping;
                }
        }
    }
    return success_string;
}

/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra char available after len) found on @param input
 */
static enum string_parsing_return is_valid_key_quote_none(const char* input, int len, bool empty_is_valid) {

    int i, mid_search = 0;

    if (len == 0 && !empty_is_valid) return err_string_empty;
    for (i = 0; i < len; i++) {
        if (!check_valid_snbt_string_char(input[i] & 0xff)) return err_string_invalid_character;
    }
    return success_string;
}


static struct snbt_return_key read_key(const int initial_idx, const char* input, size_t len, bool empty_is_valid) {

    int parse_return;
    int to_parse_len;
    int i;
    // char aux_holder = '\0';
    
    for (i = initial_idx; i < len; i++) {

        if (input[i] == '\0') {i = len+2; break;}
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\r' || input[i] == '\n') continue;

        // START KEY PARSING
        if (input[i] == '\"') {
            i++;
            to_parse_len = 0;
            while (
                i < len && input[i + to_parse_len] != '\0' 
             && !(input[i + to_parse_len] == '"' && input[i + to_parse_len -1] != '\\')
            ) {
                to_parse_len++;
            }
            // aux_holder = input[i + to_parse_len];
            // input[i + to_parse_len] = '\0';

            parse_return = is_valid_key_quote_double(&input[i], to_parse_len, empty_is_valid);
            // input[i + to_parse_len] = aux_holder;
            return (struct snbt_return_key){.start_idx=i,.key_len=to_parse_len,.valid=parse_return,.new_idx=i+to_parse_len+1};
        }
        else if (input[i] == '\'') {
            i++;
            to_parse_len = 0;
            while (
                i < len && input[i + to_parse_len] != '\0' 
             && !(input[i + to_parse_len] == '\'' && input[i + to_parse_len -1] != '\\')
            ) {
                to_parse_len++;
            }
            // aux_holder = input[i + to_parse_len];
            // input[i + to_parse_len] = '\0';

            parse_return = is_valid_key_quote_simple(&input[i], to_parse_len, empty_is_valid);
            // input[i + to_parse_len] = aux_holder;
            return (struct snbt_return_key){.start_idx=i,.key_len=to_parse_len,.valid=parse_return,.new_idx=i+to_parse_len+1};
        } else {
            to_parse_len = 0;
            while (
                i + to_parse_len < len && input[i + to_parse_len] != '\0'
             && (input[i + to_parse_len] != ':' && input[i + to_parse_len] != ' ' && input[i + to_parse_len] != '\t' && input[i + to_parse_len] != '\r' && input[i + to_parse_len] != '\n')
            ) {
                to_parse_len++;
            }
            // aux_holder = input[i + to_parse_len];
            // input[i + to_parse_len] = '\0';

            parse_return = is_valid_key_quote_none(&input[i], to_parse_len, empty_is_valid);
            // input[i + to_parse_len] = aux_holder;
            return (struct snbt_return_key){.start_idx=i,.key_len=to_parse_len,.valid=parse_return,.new_idx=i+to_parse_len};
        }
    }
    if (i == len+2) return (struct snbt_return_key){.start_idx=-2,.key_len=-2,.valid=-2,.new_idx=-2};
    return (struct snbt_return_key){.start_idx=-1,.key_len=-1,.valid=-1,.new_idx=-1};
}

struct snbt_return_value snbt_read_value(const int initial_idx, const char* input, size_t len, struct snbt_return_key key) {
    struct snbt_return_value ret = {0};

    enum eNBT_Tag test = TAG_Short;

    struct snbt_return_key aux_key;

    enum number_data {
        _negative       = 0b00001,
        _exponent       = 0b00010,
        _exponent_sign  = 0b00100,
        _exponent_num   = 0b00100,
        _decimal        = 0b01000,
        _decimal_num    = 0b10000
    } number_data = 0;
    
    union number {
        Sint8  num_byte;
        Sint16 num_short;
        Sint32 num_int;
        Sint64 num_long;
        float  num_float;
        double num_double;
    } number_value;

    enum _possibility {
        _unknown,
        _list,
        _byte_array,
        _int_array,
        _long_array,
        _compound,
        _number,
        _string
    } possible_value = _unknown;
    

    char aux_str[26] = {0};

    int i, j;
    for (i = initial_idx; i < len; i++) {

        switch (input[i]) {

            // whitespaces
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                continue;

            // list-like
            case '[':
                if (i+2 < len && input[i+2] == ';') {
                    if      (input[i+1] == 'B') possible_value = _byte_array;
                    else if (input[i+1] == 'I') possible_value = _int_array;
                    else if (input[i+1] == 'L') possible_value = _long_array;
                    else break;
                } else possible_value = _list;
                break;

            // compound
            case '{':
                possible_value =_compound;
                break;

            // number
            case '.': 
                number_data |= _decimal;
                possible_value = _number;
                break;
            case '-':
                number_data |= _negative;
                aux_str[0] = '-';
            case '+':
                i++;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                possible_value = _number;
                break;

            // @todo special true/false cases
            case 't':
                if (i+3 < len && input[i+1] == 'r' && input[i+2] == 'u' && input[i+3] == 'e' && (input[i+1] == 'r')) break;
            case 'f':
                if (i+4 < len && input[i+1] == 'a' && input[i+2] == 'l' && input[i+3] == 's' && input[i+4] == 'e' && (input[i+1] == 'r')) break;
            // string
            default:
                if (check_valid_snbt_string_char(input[i] & 0xff) != 1) break;
            case '\"':
            case '\'':
                possible_value = _string;
                break;
        }

        switch(possible_value) {
            case _unknown:
                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character};
            
            case _list:
            case _byte_array:
            case _int_array:
            case _long_array:
            case _compound:
                break;
            case _number:
                printf("number\n");
                for (j = 0; i + j < len; j++) {
                    if (j  > 24) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                    aux_str[(number_data & _negative) + j] = input[i+j];
                    switch (input[i+j]) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            if (!(number_data & _exponent)) number_data |= _decimal_num;
                            else number_data |= _exponent_num;
                            continue;
                        case '.':
                            if (number_data & _exponent) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character};
                            number_data |= _decimal;
                            continue;
                        case 'e':
                        case 'E':
                            if (number_data & _exponent || !(number_data & _decimal_num)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character};
                            number_data |= _exponent;
                            continue;

                        // e notation
                        case '-':
                        case '+':
                            if (
                                !(number_data & _exponent)
                              || (number_data & _exponent && number_data & _exponent_num)
                              || (number_data & _exponent && number_data & _exponent_sign)
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character};
                            else continue;

                        case 'b':
                        case 'B':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_byte));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Byte;
                         // byte
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (!SDL_sscanf(aux_str,"%hhi",&((struct eNBT_byte*)(ret.enbt))->payload))
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            // fill data

                            goto exit_number_loop;

                        case 's':
                        case 'S':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_short));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Short;
                         // short
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (!SDL_sscanf(aux_str,"%hi",&((struct eNBT_short*)(ret.enbt))->payload))
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            // fill data
                            
                            goto exit_number_loop;
                      
                        case_int:
                        case 'i':
                        case 'I':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_int));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Int;
                         // int
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (!SDL_sscanf(aux_str,"%i",&((struct eNBT_int*)(ret.enbt))->payload))
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            // fill data
                            
                            goto exit_number_loop;
                        case 'l':
                        case 'L':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_long));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Long;
                         // long
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (!SDL_sscanf(aux_str,"%li",&((struct eNBT_long*)(ret.enbt))->payload))
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            // fill data

                            goto exit_number_loop;
                      
                        case_double:
                        case 'D':
                        case 'd':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_double));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Double;
                         // double
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (number_data & _exponent){
                                if (!SDL_sscanf(aux_str,"%le",&((struct eNBT_double*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            } else {
                                if (!SDL_sscanf(aux_str,"%le",&((struct eNBT_double*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            }
                            // fill data

                            goto exit_number_loop;
                        case 'f':
                        case 'F':
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_float));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Float;
                         // float
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (number_data & _exponent){
                                if (!SDL_sscanf(aux_str,"%e",&((struct eNBT_float*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            } else {
                                if (!SDL_sscanf(aux_str,"%f",&((struct eNBT_float*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                            }
                            // fill data
                            
                            goto exit_number_loop;


                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r':
                        case ',':
                        case '\0':
                         // int / double
                            if (number_data & _decimal || number_data & _exponent) goto case_double;
                            goto case_int;
                        

                        default: return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character};
                    }
                }
              exit_number_loop:
              // fill generic data
                ret.enbt->flags = ENBT_FLAG_DEFAULT;
              // fill key
                ret.enbt->name = SDL_malloc(sizeof(char) * (key.key_len +1));
                if (ret.enbt->name == NULL) {
                    SDL_free(ret.enbt);
                    return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                }
                ret.enbt->name_length = key.key_len;
                for (int c = 0; c < key.key_len; c++) ret.enbt->name[c] = input[key.start_idx + c];
                ret.enbt->name[key.key_len] = '\0';

                break;

            case _string:
                #define _enbt ((struct eNBT_string*)(ret.enbt))

              // read string
                aux_key = read_key(i,input,len,true);
                if (aux_key.valid != success_string) return (struct snbt_return_value){.valid = aux_key.valid, .enbt = NULL};

              // allocate string
                ret.enbt = SDL_malloc(sizeof(struct eNBT_string));
                if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};

              // fill string
                _enbt->array = SDL_malloc(sizeof(char) * (aux_key.key_len +1));
                if (_enbt->array == NULL) {
                    SDL_free(ret.enbt);
                    return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                }
                _enbt->size = aux_key.key_len;
                for (int c = 0; c < aux_key.key_len; c++) _enbt->array[c] = input[aux_key.start_idx + c];
                _enbt->array[aux_key.key_len] = '\0';

              // fill generic data
                ret.enbt->type = TAG_String;
                ret.enbt->flags = ENBT_FLAG_DEFAULT;
              // fill key
                ret.enbt->name = SDL_malloc(sizeof(char) * (key.key_len +1));
                if (ret.enbt->name == NULL) {
                    SDL_free(ret.enbt);
                    SDL_free(_enbt->array);
                    return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                }
                ret.enbt->name_length = key.key_len;
                for (int c = 0; c < key.key_len; c++) ret.enbt->name[c] = input[key.start_idx + c];
                ret.enbt->name[key.key_len] = '\0';

                #undef _enbt
                break;
        }
        return ret;
    }

    // this should not get triggered ever
    return ret;
}

/**
 * Returns on @param enbt the nbt value contained in @param input, with an
 *  empty string key.
 * @param input must be of @param len length, as any remaining characters that
 * are not whitespaces will report errors.
 * 
 * > [!NOTE]
 * > Previous contents of @param enbt are undefined after this function. 
 */
enum string_parsing_return enbt_from_snbt(const char* input, size_t len, struct eNBT_generic** enbt) {
    int idx;

    struct snbt_return_key key = {
        .valid = success_string,
        .key_len = 0,
        .new_idx = 0,
        .start_idx = 0
    };

    struct snbt_return_value value;

    value = snbt_read_value(0,input, len,key);


    *enbt = value.enbt;

    return value.valid;
}

int main(void){
    char input[input_size+1] = {0};
    
    struct eNBT_generic* enbt = NULL;
    char* output_str;
    size_t output_len;
    char fake = '\0';
    
    enum string_parsing_return parse_return;
    
    int reiterate = true;
    
    while (reiterate) {
    
        printf("> ");
        fgets(input,input_size,stdin);
    
        parse_return = enbt_from_snbt(input,input_size,&enbt);

        printf(
            """"""
            "\n"
            "valid: %d\n"
            """""",
            parse_return
        );


        if (parse_return == success_string && enbt != NULL) output_str = enbt_to_snbt(enbt, &output_len);
        else output_str = &fake;
        printf(
            """"""
            "type: %d\n"
            "enbt: %s\n"
            """""",
            enbt?enbt->type : TAG_End,output_str
        );

        enbt_free(enbt);
        if (output_str != &fake) SDL_free(output_str);

        if (getc(stdin) != '\n') reiterate = false;
    }
    printf("\n");


    return 0;
}
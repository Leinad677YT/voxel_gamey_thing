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
    enum string_parsing_validation valid;
    int start_idx;
    int key_len;
    int new_idx;
};

struct snbt_return_value {
    enum string_parsing_validation valid;
    struct eNBT_generic* enbt;
    int new_idx;
};

/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra '\0' char available after len) found on @param input
 */
static enum string_parsing_validation is_valid_key_quote_double(const char* input, int len, bool empty_is_valid) {

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
 * validates a key of length @param len (assumes an extra '\0' char available after len) found on @param input
 */
static enum string_parsing_validation is_valid_key_quote_simple(const char* input, int len, bool empty_is_valid) {

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
 * validates a key of length @param len (assumes an extra '\0' char available after len) found on @param input
 */
static enum string_parsing_validation is_valid_key_quote_none(const char* input, int len, bool empty_is_valid) {

    int i, mid_search = 0;

    if (len == 0 && !empty_is_valid) return err_string_empty;
    for (i = 0; i < len; i++) {
        if (!check_valid_snbt_string_char(input[i] & 0xff)) return err_string_invalid_character;
    }
    return success_string;
}


/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra '\0' char available after len) found on @param input
 */
static struct snbt_return_key read_key(const int initial_idx, const char* input, size_t len, bool empty_is_valid) {

    int parse_return;
    int to_parse_len;
    int i;
    // char aux_holder = '\0';
    
    for (i = initial_idx; i < len; i++) {
        switch(input[i]){
            case '\0':
                i = len+2;
                goto exit_loop;

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                continue;

            case '\"':
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
            
            case '\'':
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

            default:
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
  exit_loop:
    if (i == len+2) return (struct snbt_return_key){.start_idx=-2,.key_len=-2,.valid=-2,.new_idx=-2};
    return (struct snbt_return_key){.start_idx=-1,.key_len=-1,.valid=-1,.new_idx=-1};
}

/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra '\0' char available after len) found on @param input
 */
struct snbt_return_value snbt_read_value(const int initial_idx, const char* input, size_t len, struct snbt_return_key key) {

    struct snbt_return_value ret = {0};

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

    // compound
        struct snbt_return_value aux_value;

    // string
    // compound
        struct snbt_return_key aux_key;
    
    // number
      enum number_data {
        _negative       = 0x1,
        _exponent       = 0x2,
        _exponent_sign  = 0x4,
        _exponent_num   = 0x8,
        _decimal        = 0x10,
        _decimal_num    = 0x20,
        _nonzero        = 0x40,
        _firstzero      = 0x80,
        _pre_num        = 0x100,
        _underscore     = 0x200,
        _unsigned       = 0x400
      } number_data = 0;
      Sint64 number_value;
      Sint32 base = 10;    
      char aux_str[30] = {0};

    int i = 0, j = 0, k = 0;
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
                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i};
            
            case _list:
            case _byte_array:
            case _int_array:
            case _long_array:
                break;
            case _compound:

                // create the base compound
                ret.enbt = (struct eNBT_generic*) enbt_create_compound(SDL_malloc(sizeof(char) * (key.key_len +1)), key.key_len, ENBT_FLAG_DEFAULT);

                // ensure allocations
                if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                if (ret.enbt->name == NULL) {
                    SDL_free(ret.enbt);
                    return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                }

                // fill key
                for (int c = 0; c < key.key_len; c++) ret.enbt->name[c] = input[key.start_idx + c];
                ret.enbt->name[key.key_len] = '\0';


                // loop over contents
                for (j = 1; i+j > len; j++) {



                    // read key
                    aux_key = read_key(i+j, input, len, false);

                    if (aux_key.valid != success_string) {
                        ret.new_idx = i + j;
                        ret.valid = aux_key.valid;
                        goto __compound_cleanup;
                    }

                    // read ':'
                    for (j = j; i+j < len; j++) {
                        switch (input[i+j]) {
                            case ' ':
                            case '\t':
                            case '\n':
                            case '\r':
                                continue;
                            case ':':
                                j++;
                                break;
                            default:
                                ret.valid = err_string_invalid_character;
                                ret.new_idx = i+j;
                                goto __compound_cleanup;
                        }
                    }

                    // read value
                    aux_value = snbt_read_value(i+j, input, len, aux_key);
                    if (aux_value.valid != success_string) {
                        ret.valid = aux_value.valid;
                        ret.new_idx = aux_value.new_idx;
                        goto __compound_cleanup;
                    }

                    
                }
                break;

            __compound_cleanup:
                enbt_free(ret.enbt);
                ret.enbt = NULL;
                return (ret);

            case _number:
                for (k = j = 0; i + j < len; j++) {
                    if (j  > 24) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number};
                    aux_str[(number_data & _negative) + j + k] = input[i+j];
                    switch (input[i+j]) {
                        case '0':
                            if (number_data & _unsigned) return (struct snbt_return_value) {.enbt = NULL, .valid= err_string_invalid_character, .new_idx = i+j};
                            if (!(number_data & _nonzero) && j == 0) {
                                switch (input[i+j+1]) {
                                    default:
                                        number_data |= _firstzero;
                                        k--;
                                        break;
                                    case 'x':
                                    case 'X':
                                        k-=2;
                                        j++;
                                        base = 16;
                                        break;
                                    case 'b':
                                    case 'B':
                                        k-=2;
                                        j++;
                                        base = 2;
                                        break;
                                }
                            } else {
                                if (!(number_data & _nonzero)) {
                                    number_data |= _firstzero;
                                    k--;
                                }
                                number_data |= _pre_num;
                                number_data &= ~_underscore;
                            }
                            goto __eval_digit;
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            if ((base <= input[i+j] - '0') || (number_data & _unsigned)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            number_data |= _nonzero | _pre_num;
                            number_data &= ~_underscore;
                        __eval_digit:
                            if (!(number_data & _exponent)) number_data |= _decimal_num;
                            else number_data |= _exponent_num;
                            continue;
                        case 'a':
                        case 'A':
                        // case 'b':
                        // case 'B':
                        case 'c':
                        case 'C':
                        // case 'd':
                        // case 'D':
                        // case 'e':
                        // case 'E':
                        // case 'f':
                        // case 'F':
                            if ((base !=16) || (number_data & _unsigned)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            else goto __eval_digit;
                        
                        case '_':
                            if (!(number_data & _pre_num) || (number_data & _unsigned)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            number_data |= _underscore;
                            k--;
                            continue;
                        case '.':
                            if (number_data & (_exponent | _underscore | _unsigned)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            number_data |= _decimal;
                            number_data &= ~_pre_num;
                            continue;
                        case 'e':
                        case 'E':
                            if (number_data & (_exponent | _unsigned) || !(number_data & _decimal_num)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            if (base == 16) goto __eval_digit;
                            number_data |= _exponent;
                            number_data &= ~_pre_num;
                            continue;

                        // e notation
                        case '-':
                        case '+':
                            if (
                                !(number_data & (_exponent | _unsigned))
                              || (number_data & _exponent && number_data & _exponent_num)
                              || (number_data & _exponent && number_data & _exponent_sign)
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            else {
                                number_data &= ~_pre_num;
                                continue;
                            }

                        // signedness notation
                        // case 's':
                        // case 'S':
                        case 'u':
                        case 'U':
                            if ((number_data & _negative)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_unsigned_number, .new_idx = i};
                            if (!(number_data & _decimal_num)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
                            number_data |= _unsigned;
                            k--;
                            continue;

                        case_byte:
                        case 'b':
                        case 'B':
                            if (base == 16 && !(number_data & _unsigned)) goto __eval_digit;
                            if (
                                (base == 10 && (number_data & _firstzero))
                             || (base != 10 && (number_data & (_underscore | _exponent | _decimal)))
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};

                            ret.enbt = SDL_malloc(sizeof(struct eNBT_byte));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Byte;
                         // byte
                            aux_str[(number_data & _negative) +j +k] = '\0';
                            errno = 0;
                            if (number_data & _unsigned) number_value = strtoull(aux_str, NULL, base);
                            else number_value = strtoll(aux_str, NULL, base);
                            if (errno == ERANGE || ((number_data&_unsigned)?(number_value > SDL_MAX_UINT8):(number_value > SDL_MAX_SINT8)) || number_value < SDL_MIN_SINT8) {
                                SDL_free(ret.enbt);
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_overflow_number, .new_idx = i};
                            }

                            ((struct eNBT_byte*)(ret.enbt))->payload = number_value;
                            
                            goto exit_number_loop;

                        case_short:
                        case 's':
                        case 'S':

                            if (!(number_data & _unsigned)) switch (input[i+j+1]) {
                                case 'b':
                                case 'B':
                                    j++; k--;
                                    goto case_byte;
                                case 's':
                                case 'S':
                                    j++; k--;
                                default:
                                    break;
                                case 'i':
                                case 'I':
                                    j++; k--;
                                    goto case_int;
                                case 'l':
                                case 'L':
                                    j++; k--;
                                    goto case_long;
                            }

                            if (
                                ((number_data & _unsigned) && number_data & _negative)
                             || (base == 10 && (number_data & _firstzero))
                             || (base != 10 && (number_data & (_underscore | _exponent | _decimal)))
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_short));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Short;
                         // short
                            aux_str[(number_data & _negative) +j +k] = '\0';
                            errno = 0;
                            if (number_data & _unsigned) number_value = strtoull(aux_str, NULL, base);
                            else number_value = strtoll(aux_str, NULL, base);
                            if (errno == ERANGE || ((number_data&_unsigned)?(number_value > SDL_MAX_UINT16):(number_value > SDL_MAX_SINT16)) || number_value < SDL_MIN_SINT16) {
                                SDL_free(ret.enbt);
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_overflow_number, .new_idx = i};
                            }

                            ((struct eNBT_short*)(ret.enbt))->payload = number_value;
                            
                            goto exit_number_loop;
                      
                        case_int:
                        case 'i':
                        case 'I':
                            if (
                                (base == 10 && (number_data & _firstzero))
                             || (base != 10 && (number_data & (_underscore | _exponent | _decimal)))
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_int));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Int;
                         // int
                            aux_str[(number_data & _negative) +j +k] = '\0';
                            errno = 0;
                            if (number_data & _unsigned) number_value = strtoull(aux_str, NULL, base);
                            else number_value = strtoll(aux_str, NULL, base);
                            if (errno == ERANGE || ((number_data&_unsigned)?(number_value > SDL_MAX_UINT32):(number_value > SDL_MAX_SINT32)) || number_value < SDL_MIN_SINT32) {
                                SDL_free(ret.enbt);
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_overflow_number, .new_idx = i};
                            }

                            ((struct eNBT_int*)(ret.enbt))->payload = number_value;
                            
                            goto exit_number_loop;

                        case_long:
                        case 'l':
                        case 'L':
                            if (
                                (base == 10 && (number_data & _firstzero))
                             || (base != 10 && (number_data & (_underscore | _exponent | _decimal)))
                            ) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_long));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Long;
                         // long
                            aux_str[(number_data & _negative) +j +k] = '\0';
                            errno = 0;
                            if (number_data & _unsigned) number_value = strtoull(aux_str, NULL, base);
                            else number_value = strtoll(aux_str, NULL, base);
                            if (errno == ERANGE) {
                                SDL_free(ret.enbt);
                                return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_overflow_number, .new_idx = i};
                            }

                            ((struct eNBT_long*)(ret.enbt))->payload = number_value;

                            goto exit_number_loop;
                      
                        case_double:
                        case 'D':
                        case 'd':
                            if (base == 16) goto __eval_digit; 
                            if (number_data & (_underscore)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_double));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Double;
                         // double
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (number_data & _exponent){
                                if (!SDL_sscanf(aux_str,"%le",&((struct eNBT_double*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            } else {
                                if (!SDL_sscanf(aux_str,"%le",&((struct eNBT_double*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            }
                            // fill data

                            goto exit_number_loop;
                        case 'f':
                        case 'F':
                            if (base == 16) goto __eval_digit;
                            if (number_data & (_underscore)) return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            ret.enbt = SDL_malloc(sizeof(struct eNBT_float));
                            if (ret.enbt == NULL) return (struct snbt_return_value){.valid= err_string_out_of_memory, .enbt = NULL};
                            ret.enbt->type = TAG_Float;
                         // float
                            aux_str[(number_data & _negative)+j] = '\0';
                            if (number_data & _exponent){
                                if (!SDL_sscanf(aux_str,"%e",&((struct eNBT_float*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            } else {
                                if (!SDL_sscanf(aux_str,"%f",&((struct eNBT_float*)(ret.enbt))->payload))
                                    return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_number, .new_idx = i};
                            }
                            // fill data
                            
                            goto exit_number_loop;


                        case '\0':
                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r':
                        case ',':
                        case ']':
                        case '}':
                         // int / double
                            if (number_data & _decimal || number_data & _exponent) goto case_double;
                            goto case_int;
                        

                        default: return (struct snbt_return_value) {.enbt = NULL, .valid = err_string_invalid_character, .new_idx = i+j};
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
                if (aux_key.valid != success_string) return (struct snbt_return_value){.valid = aux_key.valid, .enbt = NULL, .new_idx = i};

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

        switch(input[i+j+1]){
            case '\0':
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case ',':
            case ']':
            case '}':
                break;

            default:
                enbt_free(ret.enbt);
                return (struct snbt_return_value){.enbt = NULL, .valid = err_string_invalid_character,.new_idx = i+j+1};
        }

        ret.new_idx = i+j+1;
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
struct string_parsing_return enbt_from_snbt(const char* input, size_t len, struct eNBT_generic** enbt) {
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

    return (struct string_parsing_return){.valid = value.valid, .idx = value.new_idx};
}

int main(void){
    char input[input_size+1] = {0};
    
    struct eNBT_generic* enbt = NULL;
    char* output_str;
    size_t output_len;
    char fake = '\0';
    
    struct string_parsing_return parse_return;
    
    int reiterate = true;
    
    while (reiterate) {
    
        printf("> ");
        fgets(input,input_size,stdin);
    
        parse_return = enbt_from_snbt(input,input_size,&enbt);

        printf(
            """"""
            "\n"
            "valid: %d\n"
            "idx: %d\n"
            """""",
            parse_return.valid,
            parse_return.idx
        );


        if (parse_return.valid == success_string && enbt != NULL) output_str = enbt_to_snbt(enbt, &output_len);
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
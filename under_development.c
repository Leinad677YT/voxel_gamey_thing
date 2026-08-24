#include <stdio.h>

#include <leinad/type/enbt.h>

#define input_size 100
#define scanfformat(input_size) "%" #input_size "s"

// 0 = INVALID, 1 = ALWAYS VALID, 2 = CANNOT START REGULAR UNQUOTED STRINGS
const char valid_unquoted_string_char[1<<(sizeof(char)*8)] = {
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

static int check_valid_snbt_string_char(char c) {
    return valid_unquoted_string_char[c];
}


enum string_parsing_return {
    success_string = 0,
    err_string_empty,
    err_string_invalid_character,
    err_string_quote_not_escaped,
    err_string_invalid_escaping,
    err_string_incomplete_escaping
};

struct snbt_return_key {
    enum string_parsing_return valid;
    int start_idx;
    int key_len;
    int new_idx;
};

struct snbt_return_value {
    enum string_parsing_return valid;
    enum eNBT_Tag type;
};


/**
 * - Auxiliary for @sa enbt_from_snbt
 * validates a key of length @param len (assumes an extra char available after len) found on @param input
 */
static enum string_parsing_return is_valid_key_quote_double(const char* input, int len) {

    int i, mid_search = 0;

    if (len == 0) return err_string_empty;
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
static enum string_parsing_return is_valid_key_quote_simple(const char* input, int len) {

    int i, mid_search = 0;

    if (len == 0) return err_string_empty;
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
static enum string_parsing_return is_valid_key_quote_none(const char* input, int len) {

    int i, mid_search = 0;

    if (len == 0) return err_string_empty;
    for (i = 0; i < len; i++) {
        if (!check_valid_snbt_string_char(input[i] & 0xff)) return err_string_invalid_character;
    }
    return success_string;
}


struct snbt_return_key read_key(const int initial_idx, const char* input, size_t len) {

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

            parse_return = is_valid_key_quote_double(&input[i], to_parse_len);
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

            parse_return = is_valid_key_quote_simple(&input[i], to_parse_len);
            // input[i + to_parse_len] = aux_holder;
            return (struct snbt_return_key){.start_idx=i,.key_len=to_parse_len,.valid=parse_return,.new_idx=i+to_parse_len+1};
        } else {
            to_parse_len = 0;
            while (
                i + to_parse_len < len && input[i + to_parse_len] != '\0'
             && (input[i + to_parse_len] != ' ' && input[i + to_parse_len] != '\t' && input[i + to_parse_len] != '\r' && input[i + to_parse_len] != '\n')
            ) {
                to_parse_len++;
            }
            // aux_holder = input[i + to_parse_len];
            // input[i + to_parse_len] = '\0';

            parse_return = is_valid_key_quote_none(&input[i], to_parse_len);
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

    enum _possibility {
        _unknown,
        _list,
        _byte_array,
        _int_array,
        _long_array,
        _compound,
        _number,
        _floating_point_number,
        _string
    } possible_value = _unknown;
    
    int i;
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
                possible_value =_list;
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
                possible_value = _floating_point_number;
                break;
            case '-':
            case '+':
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

            // string
            default:
                if (check_valid_snbt_string_char(input[i] == 2)) break;
            case '\"':
            case '\'':
                possible_value = _string;
                break;
        }

        switch(possible_value) {
            case _unknown:
                return (struct snbt_return_value) {.type = TAG_End, .valid = err_string_invalid_character};
            
            case _list:
            case _byte_array:
            case _int_array:
            case _long_array:
            case _compound:
            case _number:
            case _floating_point_number:
                break;

            case _string:
                aux_key = read_key(i,input,len);
                break;
        }
    }

    return ret;
}
struct eNBT_generic* enbt_from_snbt(const char* input, size_t len) {
    int idx;

    struct snbt_return_key key = {
        .valid = success_string,
        .key_len = 0,
        .new_idx = 0,
        .start_idx = 0
    };

    snbt_read_value(0,input, len,key);


    return NULL;
}

int main(void){
    char input[input_size+1] = {0};
    char output[input_size+1] = {0};
    int i = 0;

    enum string_parsing_return parse_return = success_string;

    fgets(input,input_size,stdin);

    printf("> %s\n",input);

    while (i < input_size) {
        struct snbt_return_key key;
        int j;

        key = read_key(i,input,input_size);
        if (key.key_len == -2 || key.key_len == -1) break;

        for (j = 0; j < key.key_len; j++) output[j] = input[key.start_idx+j];
        output[j] = '\0';

        printf(
            """"""
            "\n"
            "key: %s\n"
            "len: %d\n"
            "valid: %d\n"
            """""",
            output,key.key_len,key.valid
        );

        getc(stdin);
        
        if (key.valid) return 1;
        i = key.new_idx;
    }
    printf("\n");


    return 0;
}
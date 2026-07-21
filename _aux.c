#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>


#define LEINAD_MAX_LINE_LENGTH 65

static int readline(int fd, char output[LEINAD_MAX_LINE_LENGTH]);

static bool cmp_str(char* restrict str1, char* restrict str2, const int len);

struct buffer_pair {
    void* index;
    void* vertex;
} buffers[1728] = {0};

bool pending_buffers[1728] = {0};

int main(){


    int type1fails = 0;
    int type2fails = 0;
    int type3fails = 0;
    int type4fails = 0;
    int allocs = 0;
    int releases = 0;

    int fd = open("OUTPUT_new.txt",O_RDONLY);


    printf("[INFO] opened fd: %d\n",fd);

    char output[LEINAD_MAX_LINE_LENGTH +1];

    for (int line = 0; line < 7205; line++){

        struct buffer_pair read_pair;

        readline(fd, output);

        char validator[10] = {0};

        sscanf(output,"%9s IDX: %20p\t\tVTX: %20p",validator,&read_pair.index,&read_pair.vertex);

        if (cmp_str(validator, "allocated",9)) {

            allocs++;

            int pair;

            for (pair = 0; pair < 1728; pair++) {
                if (pending_buffers[pair]) continue;
                else {
                    buffers[pair].index = read_pair.index;
                    buffers[pair].vertex = read_pair.vertex;
                    pending_buffers[pair] = true;
                    break;
                }
            }

            if (pair == 1728) {
                printf("[!ERROR] Exceeded the allocation limit!\n");
                type1fails++;
            }
        }

        else if (cmp_str(validator, "released",8)) {

            releases++;

            int pair;

            for (pair = 0; pair < 1728; pair++) {
                if (!pending_buffers[pair]) continue;
                else {
                    if (buffers[pair].index == read_pair.index) {
                        if (buffers[pair].vertex == read_pair.vertex) {
                            pending_buffers[pair] = false;
                            break;
                        }
                        else {
                            printf("[!ERROR] Pair does not match! index is correct but vertex is not");
                            type2fails++;
                        }
                    }
                    else if (buffers[pair].vertex == read_pair.vertex) {
                        printf("[!ERROR] Pair does not match! vertex is correct but index is not");
                        type3fails++;
                    }
                }
            }

            if (pair == 1728) {
                printf("[!ERROR] Released a non allocated buffer!\n");
                type4fails++;
            }

        }
        else continue;
    }

    int failures = 1728;
    for (int pair = 0; pair < 1728; pair++) if (!pending_buffers[pair]) failures--;

    printf(
        """"""
        "[RESULT] failures: %d, allocations: %d, releases: %d\n"
        "[RESULT] allocations when not enough freed space: %d\n"
        "[RESULT] pair mismatches with vertex failing: %d\n"
        "[RESULT] pair mismatches with index failing: %d\n"
        "[RESULT] releases of unallocated buffers: %d\n"
        """""",
        failures, allocs, releases,
        type1fails, type2fails, type3fails, type4fails
    );

    return 0;
}


static int readline(int fd, char output[LEINAD_MAX_LINE_LENGTH]) {
    size_t offset;
    size_t read_chars;
    int i, j;
    char line[LEINAD_MAX_LINE_LENGTH+1] = {0};

    // GET CURRENT OFFSET
    offset = lseek(fd,0,SEEK_CUR);

    read_chars = read(fd, line, LEINAD_MAX_LINE_LENGTH);

    for (i = 0;i < read_chars; i++) {
        if (line[i] == '\0' || line[i] == '\n') break;
    }

    for (j = 0; j < i;j++) {
        output[j] = line[j];
    }

    output[j] = '\0';

    // remove the line from the IO offset
    lseek(fd, offset+i+1, SEEK_SET);

    return i;
}


static bool cmp_str(char* restrict str1, char* restrict str2, const int len) {

    for (int i = 0; i < len; i++) {
        if (str1[i] != str2[i]) return false;
    }

    return true;
}
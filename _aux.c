#include <stdio.h>
#include <stdlib.h>

#define pow2(x) (1<<x)
#define raise2(x) (x*x)
#define raise3(x) (x*x*x)
#define LOADED_CHUNKS_RADIUS 2
#define LOADED_CHUNKS_LENGTH (2*LOADED_CHUNKS_RADIUS+1)

typedef int leinad_chunk_t;

struct {
    leinad_chunk_t* chunk[raise3(LOADED_CHUNKS_LENGTH)];
} loaded_chunks;

void fun(leinad_chunk_t*x) {
    printf("%d\n",*x); fflush(stdout);
}

int main(void){
    // init
    for(int i = 0; i < raise3(LOADED_CHUNKS_LENGTH); i++){
        loaded_chunks.chunk[i] = malloc(sizeof(leinad_chunk_t));
        *loaded_chunks.chunk[i] = i;
    }

    // row to check
    for(int volume = 0; volume < pow2(3); volume++)
     // position to check
     for(int k = 0; k < LOADED_CHUNKS_RADIUS; k++)
      for(int j = 0; j < LOADED_CHUNKS_RADIUS; j++)
       for(int i = 0; i < LOADED_CHUNKS_RADIUS; i++) {
        printf("iterating over: vol=%d  -  k=%d j=%d i=%d  -  ",volume,k,j,i); fflush(stdout);
        switch (volume){
            #define invert(x) (LOADED_CHUNKS_LENGTH -x -1)
            case 0: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i]);         break;
            case 1: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)]); break;
            case 2: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i]);         break;
            case 3: fun(loaded_chunks.chunk[k        *raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)]); break;
            case 4: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + i]);         break;
            case 5: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + j        *LOADED_CHUNKS_LENGTH + invert(i)]); break;
            case 6: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + i]);         break;
            case 7: fun(loaded_chunks.chunk[invert(k)*raise2(LOADED_CHUNKS_LENGTH) + invert(j)*LOADED_CHUNKS_LENGTH + invert(i)]); break;
            #undef invert
        }
    }

    return 0;
}
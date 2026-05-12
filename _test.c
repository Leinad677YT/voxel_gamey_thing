#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

#define TEST_ITERS 0x10000000
#define TEST_AMOUNT 5
#define LOOP_AMOUNT 10

int a[TEST_ITERS] = {0};
int b[TEST_ITERS] = {0};


extern int fast_sqrt(int n);

int main(){
    int n, fails = 0;
    double accum_error = 0;
    int time_lib =0, time_own =0, max_difference = 0;
    struct timeval t1,t2;

    for(int i = 0; i < TEST_ITERS; i++){
        a[i] = (int)sqrt(i);
    }

    for (int test_iter = 0; test_iter < TEST_AMOUNT; test_iter++){

        gettimeofday(&t1,NULL);
        for (int test_loop = 0; test_loop < LOOP_AMOUNT; test_loop++){
            for(int i = 4; i < TEST_ITERS; i++){
                b[i] = (int)fast_sqrt(i);
            }
        }
        gettimeofday(&t2,NULL);
        time_own += ((t2.tv_usec - t1.tv_usec)+ 1000000 * (t2.tv_sec - t1.tv_sec))/TEST_AMOUNT;

        
        gettimeofday(&t1,NULL);
        for (int test_loop = 0; test_loop < LOOP_AMOUNT; test_loop++){
            for(int i = 4; i < TEST_ITERS; i++){
                a[i] = (int)sqrt(i);
            }
        }
        gettimeofday(&t2,NULL);
        time_lib += ((t2.tv_usec - t1.tv_usec)+ 1000000 * (t2.tv_sec - t1.tv_sec))/TEST_AMOUNT;


    }

    for(int i = 0; i < TEST_ITERS; i++){
        if (a[i] != b[i]) {
            fails++;
            accum_error+= (a[i]-b[i]);
            max_difference = abs(a[i]-b[i]) > max_difference ? abs(a[i]-b[i]) : max_difference;
        }    }


    printf("library sqrt() time: %d\n",time_lib);
    printf("Own sqrt() time:     %d\n",time_own);
    printf("%d fails out of %d with a mean error of %lf and a max error of %d\n\n",fails,TEST_ITERS,accum_error/(double)TEST_ITERS,max_difference);
}

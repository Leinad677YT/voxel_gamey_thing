#include <stdio.h>

typedef struct {
    int patata;
    char* test;
    void*(*fun)(int);
} potato;


union {
    potato test;
    int patata;
} patatas;

void* print(int n){
    printf("%d",n);
    return NULL;
}

void* stack_dir(int n){
    return (void*)&n;
}

int main(){
    potato potato;

    potato.fun = stack_dir;

    printf("%p\n",potato.fun(1));

    potato.fun = print;

    potato.fun(3);
}
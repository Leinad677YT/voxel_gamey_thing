#include <SDL3/SDL.h>

#include "stack.h"

#define next(x,mod) (x+1 % mod)

struct leinad_stack_data {
    int top, max_size;
    void** vec;
};

bool leinad_create_stack(leinad_stack_t* stack, int max_stack_size) {

    if (max_stack_size <= 0) return false;

    *stack = SDL_malloc(sizeof(struct leinad_stack_data));
    if (*stack == NULL) return false;

    (*stack)->vec = SDL_malloc(sizeof(void*) * max_stack_size);
    if ((*stack)->vec == NULL) {
        SDL_free(*stack);
        return false;
    }

    (*stack)->max_size = max_stack_size;
    (*stack)->top = -1;
    
    return true;
}

void leinad_destroy_stack(leinad_stack_t* stack) {
    if (*stack == NULL) return;
    
    if ((*stack)->vec == NULL) {
        SDL_free(*stack);
        return;
    }

    SDL_free((*stack)->vec);
    SDL_free(*stack);
}

bool leinad_isempty_stack(leinad_stack_t stack) {
    return (stack->top == -1);
}

void* leinad_peek_stack(leinad_stack_t stack) {
    if (stack->top == -1) return NULL;

    return stack->vec[stack->top];
}

bool leinad_push_stack(leinad_stack_t* stack, void* element) {
    if ((*stack)->top >= (*stack)->max_size) return false;

    (*stack)->top++;
    (*stack)->vec[(*stack)->top] = element;

    return true;
}

void* leinad_pop_stack(leinad_stack_t* stack) {
    void* x;

    if ((*stack)->top == -1) return NULL;

    x = (*stack)->vec[(*stack)->top];
    (*stack)->top--;

    return x;
}

void* leinad_forall_stack(
    leinad_stack_t* stack,
    void* (*iterator)(void*, void*),
    void* accumulator,
    bool reversed
) {
    int current = reversed? 0 : (*stack)->top;
    void* ret = accumulator;

    if (current == -1 || (*stack)->top == -1) return NULL;

    if (reversed) {
        while (current <= (*stack)->top) {
            ret = iterator(ret,(*stack)->vec[current]);
            current++;
        }
    }
    else {
        while (current >= -1) {
            ret = iterator(ret,(*stack)->vec[current]);
            current--;
        }
    }

    return ret;    
}

#pragma once

#include <stdbool.h>

struct leinad_stack_data;
typedef struct leinad_stack_data* leinad_stack_t;

bool leinad_create_stack(leinad_stack_t* stack, int max_stack_size);
void leinad_destroy_stack(leinad_stack_t* stack);

bool leinad_isempty_stack(leinad_stack_t stack);

void* leinad_peek_stack(leinad_stack_t stack);

bool leinad_push_stack(leinad_stack_t* stack, void* element);
void* leinad_pop_stack(leinad_stack_t* stack);

void* leinad_forall_stack(
    leinad_stack_t* stack,
    void* (*iterator)(void* acc, void* element),
    void* accumulator,
    bool reversed
);

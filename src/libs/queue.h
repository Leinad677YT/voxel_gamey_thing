#pragma once

#include <stdbool.h>

struct leinad_queue_data;
typedef struct leinad_queue_data* leinad_queue_t;

bool leinad_create_queue(leinad_queue_t* queue, int max_queue_size);
void leinad_destroy_queue(leinad_queue_t* queue);

bool leinad_isempty_queue(leinad_queue_t queue);

void* leinad_peek_queue(leinad_queue_t queue);

bool leinad_enqueue(leinad_queue_t* queue, void* element);
void* leinad_dequeue(leinad_queue_t* queue);

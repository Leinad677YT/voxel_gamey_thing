#include <SDL3/SDL.h>
#include "queue.h"

#define next(x,mod) (x+1 % mod)

struct leinad_queue_data {
    int head, tail, size, max_size;
    void** vec;
};

bool leinad_create_queue(leinad_queue_t* queue, int max_queue_size) {

    if (max_queue_size <= 0) return false;

    *queue = SDL_malloc(sizeof(struct leinad_queue_data));
    if (*queue == NULL) return false;

    (*queue)->vec = SDL_malloc(sizeof(void*) * max_queue_size);
    if ((*queue)->vec == NULL) {
        SDL_free(*queue);
        return false;
    }

    (*queue)->max_size = max_queue_size;
    (*queue)->size = 0;
    (*queue)->head = 0;
    (*queue)->tail = -1;

    return true;
}

void leinad_destroy_queue(leinad_queue_t* queue) {
    if (*queue == NULL) return;
    
    if ((*queue)->vec == NULL) {
        SDL_free(*queue);
        return;
    }

    SDL_free((*queue)->vec);
    SDL_free(*queue);
}

bool leinad_isempty_queue(leinad_queue_t queue) {
    return !queue->size;
}

void* leinad_peek_queue(leinad_queue_t queue) {
    if (queue->size == 0) return NULL;

    return queue->vec[queue->head];
}

bool leinad_enqueue(leinad_queue_t* queue, void* element) {
    if ((*queue)->size >= (*queue)->max_size) return false;

    (*queue)->size++;
    (*queue)->tail = next((*queue)->tail,(*queue)->max_size);
    (*queue)->vec[(*queue)->tail] = element;

    return true;
}

void* leinad_dequeue(leinad_queue_t* queue) {
    void* x;

    if ((*queue)->size == 0) return NULL;

    (*queue)->size--;
    x = (*queue)->vec[(*queue)->head];
    (*queue)->head = next((*queue)->head,(*queue)->max_size);

    return x;
}


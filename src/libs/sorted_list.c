#include <SDL3/SDL.h>

#include "sorted_list.h"

struct leinad_slist_node {
    void* element;
    struct leinad_slist_node* prev;
    struct leinad_slist_node* next;
};

struct leinad_slist_data {
    struct leinad_slist_node* head;
    struct leinad_slist_node* last;
    int (*comparator)(const void*, const void*);
    int size;
};

bool leinad_create_slist(leinad_slist_t* list, int (*comparator)(const void*, const void*)) {
    *list = SDL_malloc(sizeof(struct leinad_slist_data));

    if (*list == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't allocate a sorted list");
        return false;
    }

    (*list)->head = SDL_malloc(sizeof(struct leinad_slist_node));

    if ((*list)->head == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't allocate a sorted list");
        return false;
    }

    (*list)->head->element = NULL;
    (*list)->head->next = NULL;
    (*list)->head->prev = NULL;

    (*list)->last = (*list)->head;
    (*list)->size = 0;

    (*list)->comparator = comparator;


    return true;
}

void leinad_destroy_slist(leinad_slist_t* list){
    
    leinad_slist_node_t current = (*list)->head;

    while ((*list)->size > 0) {
        (*list)->head = (*list)->head->next;
        SDL_free(current);
        (*list)->size--;
    }

    SDL_free((*list)->head);
    SDL_free(*list);
}

bool leinad_isempty_slist(const leinad_slist_t list) {
    return !list->size;
}

int leinad_getsize_slist(const leinad_slist_t list) {
    return list->size;
}

leinad_slist_node_t* leinad_getfirst_slist(const leinad_slist_t list) {
    return &list->head->next;
}

leinad_slist_node_t* leinad_getlast_slist(const leinad_slist_t list) {
    return &list->last;
}


void* leinad_getelement_slist(const leinad_slist_node_t node) {
    return node->element; 
}

void leinad_remove_slist(leinad_slist_t* list, leinad_slist_node_t* node) {
    if (list == NULL || (*list)->size <= 0 || node == NULL || (*node)->prev == NULL) return;

    (*list)->size--;
    (*node)->prev->next = (*node)->next;

    SDL_free(*node);

}


leinad_slist_node_t* leinad_find_slist(const leinad_slist_t* list, const void* element) {
    leinad_slist_node_t* current = &(*list)->head;
    leinad_slist_node_t next = (*list)->head->next;
    int to_iterate = (*list)->size -1;

    while (to_iterate) {
        if (
            next == NULL
            || (*list)->comparator(element, (*current)->element) <= 0
        ) break;

        current = &(*current)->next;
        next = next->next;
        to_iterate--;
    }

    if (
        next == NULL 
        || (*list)->comparator(element, (*current)->element) != 0
    ) return NULL;
    
    return current;
}

bool leinad_insert_slist(leinad_slist_t* list, void* element) {
    leinad_slist_node_t* current = &(*list)->head;
    leinad_slist_node_t* next;
    int to_iterate = (*list)->size -1;

    while (to_iterate) {
        if (
            (*current)->next == NULL
            || (*list)->comparator(element, (*current)->next->element) <= 0
        ) break;

        current = &(*current)->next;
        to_iterate--;
    }

    next = SDL_malloc(sizeof(struct leinad_slist_node));

    if (next == NULL) {
        return false;
    }


    *next = SDL_malloc(sizeof(struct leinad_slist_node));

    if (*next == NULL) {
        SDL_free(next);
        return false;
    }


    (*next)->prev = *current;
    (*next)->next = (*current)->next;
    (*next)->element = element;

    (*current)->next = *next;

    return true;
}

void* leinad_forall_slist(leinad_slist_node_t* start, void* (*iterator)(void* accumulator, void* node_p), void* accumulator, bool reversed) {
    leinad_slist_node_t* current = start;
    void* ret = accumulator; 

    if (current == NULL) return NULL;


    if (reversed) {
        while (*current != NULL ) {
            if (ret == NULL) return NULL;
            ret = iterator(ret,current);
            current = &(*current)->prev;
        }
    }
    else {
        while (*current != NULL ) {
            if (ret == NULL) return NULL;
            ret = iterator(ret,current);
            current = &(*current)->next;
        }
    }

    return ret;    
}
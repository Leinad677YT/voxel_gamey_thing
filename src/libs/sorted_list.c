#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "sorted_list.h"
#include "return_types.h"

struct leinad_sorted_list_node {
    void* element;
    struct leinad_sorted_list_node* prev;
    struct leinad_sorted_list_node* next;
};

struct leinad_sorted_list_data {
    struct leinad_sorted_list_node* head;
    struct leinad_sorted_list_node* last;
    void* (*comparator)(const void*, const void*);
    int size;
};

bool leinad_create_sorted_list(leinad_sorted_list_t* list, void* (*comparator)(const void*, const void*)) {
    *list = malloc(sizeof(struct leinad_sorted_list_data));

    if (*list == NULL) {
        perror("Couldn't allocate a sorted list");
        return false;
    }

    (*list)->head = malloc(sizeof(struct leinad_sorted_list_node));

    (*list)->head->element = NULL;
    (*list)->head->next = NULL;
    (*list)->head->prev = NULL;

    (*list)->last = (*list)->head;
    (*list)->size = 0;

    (*list)->comparator = comparator;


    return true;
}

void leinad_destroy_sorted_list(leinad_sorted_list_t* list){
    
    leinad_sorted_list_node_t current = (*list)->head;

    while ((*list)->size > 0) {
        (*list)->head = (*list)->head->next;
        free(current);
        (*list)->size--;
    }

    free((*list)->head);
    free(*list);

    return true;
}

bool leinad_is_empty_sorted_list(const leinad_sorted_list_t list) {
    return !list->size;
}

int leinad_getsize_sorted_list(const leinad_sorted_list_t list) {
    return list->size;
}

void* leinad_getelement_sorted_list(const leinad_sorted_list_node_t node){
    return node->element; 
}

void leinad_remove_sorted_list(leinad_sorted_list_node_t* node){

}


leinad_sorted_list_node_t* leinad_find_sorted_list(const leinad_sorted_list_t* list, const void* element) {
    leinad_sorted_list_node_t* current = &(*list)->head;
    int to_iterate = (*list)->size;

    while (to_iterate) {


        if (
            (*current)->next == NULL
            || (*list)->comparator(element,(*current)->element) < 0
        ) break;

        (*current) = (*current)->next;
        to_iterate--;
    }

    if ((*current)->next == NULL) return NULL;
    
    return &(*current)->next;
}

leinad_collection_return_t leinad_insert_sorted_list(leinad_sorted_list_t* list, const void* element) {
    leinad_sorted_list_node_t* current = &(*list)->head;
    int to_iterate = (*list)->size;

    while (to_iterate) {
        if (
            (*current)->next == NULL
            || (*list)->comparator(element,(*current)->element) <= 0
        ) break;

        (*current) = (*current)->next;
        to_iterate--;
    }

    if (
        (*current)->next == NULL 
        || (*list)->comparator(element,(*current)->element) == 0) return 
    return &(*current)->next;

}

int leinad_forall_sorted_list(const leinad_sorted_list_t list, void* (*iterator)(leinad_sorted_list_node_t*), bool reversed) {

}
#ifndef _LEINAD_LIST_H
    #define _LEINAD_LIST_H

#include <stdbool.h>

#include "return_types.h"

struct leinad_sorted_list_node;
typedef struct leinad_sorted_list_node* leinad_sorted_list_node_t;

struct leinad_sorted_list_data;
typedef struct leinad_sorted_list_data* leinad_sorted_list_t;


bool leinad_create_sorted_list(leinad_sorted_list_t* list, void* (*comparator)(const void*, const void*));
void leinad_destroy_sorted_list(leinad_sorted_list_t* list);

bool leinad_is_empty_sorted_list(const leinad_sorted_list_t list);
int leinad_getsize_sorted_list(const leinad_sorted_list_t list);

void* leinad_getelement_sorted_list(const leinad_sorted_list_node_t node);
void leinad_remove_sorted_list(leinad_sorted_list_node_t* node);

leinad_sorted_list_node_t* leinad_find_sorted_list(const leinad_sorted_list_t* list, const void* element);
leinad_collection_return_t leinad_insert_sorted_list(leinad_sorted_list_t* list, const void* element);

int leinad_forall_sorted_list(const leinad_sorted_list_t list, void* (*iterator)(leinad_sorted_list_node_t*), bool reversed);
#endif
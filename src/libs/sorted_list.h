#pragma once

#include <stdbool.h>

struct leinad_slist_node;
typedef struct leinad_slist_node* leinad_slist_node_t;

struct leinad_slist_data;
typedef struct leinad_slist_data* leinad_slist_t;

/// comparator(A,B) should give results with the same signs as
//   the result of `A - B` if they were numbers
bool leinad_create_slist(leinad_slist_t* list, int (*comparator)(const void*, const void*));
void leinad_destroy_slist(leinad_slist_t* list);

bool leinad_isempty_slist(const leinad_slist_t list);
int leinad_getsize_slist(const leinad_slist_t list);

leinad_slist_node_t* leinad_getfirst_slist(const leinad_slist_t list);
leinad_slist_node_t* leinad_getlast_slist(const leinad_slist_t list);

void* leinad_getelement_slist(const leinad_slist_node_t node);
void leinad_remove_slist(leinad_slist_t* list, leinad_slist_node_t* node);

leinad_slist_node_t* leinad_find_slist(const leinad_slist_t* list, const void* node);
bool leinad_insert_slist(leinad_slist_t* list, void* element);

/// iterator MUST consider that the first accumulator value is
//   `accumulator` as well as the possibility of NULL `element` values
//   if accumulator is null, the list iteration will stop 
void* leinad_forall_slist(
    leinad_slist_node_t* start,
    void* (*iterator)(void* acc, void* node),
    void* accumulator,
    bool reversed
);

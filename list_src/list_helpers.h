#ifndef LIST_HELPERS_H
#define LIST_HELPERS_H

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

error_t mark_list_nodes_as_free(struct List_node *node, size_t size, size_t start_ID);
error_t list_realloc(List *lst);
error_t update_lists_fictitious_elem(struct List *lst);
error_t increase_list_capacity(struct List *lst);


#endif

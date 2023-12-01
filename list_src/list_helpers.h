#ifndef LIST_HELPERS_H
#define LIST_HELPERS_H

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

error_t mark_list_nodes_as_free(struct List_node *node, size_t size, size_t start_ID);
error_t list_realloc(List *lst);
error_t update_lists_fictitious_elem(struct List *lst);
error_t increase_list_capacity(struct List *lst);
error_t create_node(int node_ID, struct node_charachteristics *nd_description,
					FILE *graphic_dump_code_file_ptr);
error_t connect_nodes(struct List *lst, FILE *graphic_dump_code_file_ptr);

#endif

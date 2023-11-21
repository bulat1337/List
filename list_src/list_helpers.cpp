#include "list_helpers.h"

struct List
{
	struct List_node *node;
	int head;
	int tail;
	int current_free;
	size_t capacity;
};

extern FILE *log_file;

error_t mark_list_nodes_as_free(struct List_node *node, size_t size, size_t start_ID)
{
	error_t error_code = ALL_GOOD;

	for(size_t ID = start_ID; ID < size - 1; ID++)
	{
		node[ID].next = ID + 1;
		node[ID].prev = FREE_ELEM_MARKER;
	}

	node[size - 1].next = 0;
	node[size - 1].prev = FREE_ELEM_MARKER;

	return error_code;
}

error_t list_realloc(List *lst)
{
	error_t error_code = ALL_GOOD;

	size_t data_size = sizeof(struct List_node) * lst->capacity;

	lst->node = (struct List_node *)realloc(lst->node, data_size);
	if(lst->node == NULL)
	{
		return NOT_ENOUGH_MEM;
	}

	size_t new_allocated_areas_start_ID = (lst->capacity / 2) - 1; // TD: hardcode
	lst->current_free = new_allocated_areas_start_ID;
	WRITE_IN_LOG("new_allocated_areas_start_ID == %lu\n", new_allocated_areas_start_ID);
	mark_list_nodes_as_free(lst->node, lst->capacity, new_allocated_areas_start_ID);
	WRITE_IN_LOG("lst->node[new_allocated_areas_start_ID].next == %d\n",
		lst->node[new_allocated_areas_start_ID].next);

	return error_code;
}

error_t update_lists_fictitious_elem(struct List *lst)
{
	(lst->node)[0].next = lst->head;
	(lst->node)[0].prev = lst->tail;

	return ALL_GOOD;
}

error_t increase_list_capacity(struct List *lst)
{
	if(lst->capacity == 0)
	{
		lst->capacity += LIST_REALLOC_FROM_ZERO_CAPACITY;
	}
	else
	{
		lst->capacity *= LIST_REALLOC_COEFF;
	}

	return ALL_GOOD;
}

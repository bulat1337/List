#include <stdio.h>
#include <stdlib.h>

#include "list.h"

enum Err_ID list_ctor(struct List *lst, size_t starter_capacity)
{
	enum Err_ID function_error = ALL_GOOD;

	lst->capacity = starter_capacity;

	lst->data = (lst_elem_t *)calloc(lst->capacity, sizeof(lst_elem_t));

	lst->next = (int *)calloc(lst->capacity, sizeof(int));
	set_to_default_free_elem_marker(lst->next, lst->capacity, 0);

	lst->prev = (int *)calloc(lst->capacity, sizeof(int));
	set_to_default_free_elem_marker(lst->prev, lst->capacity, 0);

	size_t amount_of_free_elems = lst->capacity;
	STACK_CTOR(&(lst->free), amount_of_free_elems);
	STACK_PUSH(&(lst->free), -1);
	push_all_elems_from_end(&(lst->free), lst->capacity - 1, 0);

	lst->head = free_elem_marker;
	lst->tail = free_elem_marker;

	(lst->next)[0] = 0;	// почему они печатаются как ничто
	(lst->prev)[0] = 0;


	lst->data[0] = POIZON;

	list_dump(lst);

	return function_error;
}

enum Err_ID list_dump(const struct List *lst)
{
	enum Err_ID function_error = ALL_GOOD;

	FILE *log_file = stdout;
	fprintf(log_file, "ID:");
	fprintf(log_file, "        data:");
	fprintf(log_file, "        next:");
	fprintf(log_file, "        prev:");
	fprintf(log_file, "        free:\n");
	for(size_t ID = 0; ID < lst->capacity; ID++)
	{
		fprintf(log_file, "[%lu]%13.3lf", ID, (lst->data)[ID]);
		fprintf(log_file, "%13.d", (lst->next)[ID]);
		fprintf(log_file, "%13.d", (lst->prev)[ID]);
		if(ID <= lst->free.capacity)
		{
			fprintf(log_file, "%13.d\n", (lst->free.data)[ID]);
		}
		else
		{
			fprintf(log_file, "\n");
		}
	}

	fprintf(log_file, "current free: %d\n", (lst->free.data)[lst->free.size - 1]);
	fprintf(log_file, "head: %d\n", lst->head);
	fprintf(log_file, "tail: %d\n", lst->tail);
	// dump_all_elems(log_file, lst->data, lst->capacity);

	return function_error;
}

enum Err_ID set_to_default_free_elem_marker(int *array, size_t size, size_t start_ID)
{
	enum Err_ID function_error = ALL_GOOD;
	for(size_t ID = start_ID; ID < size; ID++)
	{
		array[ID] = free_elem_marker;
	}

	return function_error;
}

// enum Err_ID dump_all_elems(FILE *log_file, lst_elem_t *array, size_t size)
// {
// 	enum Err_ID function_error = ALL_GOOD;
// 	for(size_t ID = 0; ID < size; ID++)
// 	{
// 		fprintf(log_file, "[%lu] %.3lf\n", ID, array[ID]);
// 	}
//
// 	return function_error;
// }

enum Err_ID push_all_elems_from_end(struct Stack *stk, size_t end_ID, size_t start_ID)
{
	enum Err_ID function_error = ALL_GOOD;
	for(size_t ID = end_ID; ID > start_ID; ID--)
	{
		STACK_PUSH(stk, (elem_t)ID);
		printf("pushin %lu\n", ID);
	}

	return function_error;
}

enum Err_ID list_add(struct List *lst, lst_elem_t value)
{
	enum Err_ID function_error = ALL_GOOD;

	int new_elem_ID = STACK_POP(&(lst->free)).deleted_element;

	if(new_elem_ID == free_elem_marker)
	{
		printf("new_elem_ID == %d so im reallocating\n", new_elem_ID);
		if(lst->capacity == 0) //func
		{
			lst->capacity += list_expantion_size_from_zero;
		}
		else
		{
			lst->capacity *= list_reformation_coeff;
		}

		list_realloc(lst);
		new_elem_ID = STACK_POP(&(lst->free)).deleted_element;
	}
	else
	{
		printf("new_elem_ID == %d so im NOT reallocating\n", new_elem_ID);
	}

	if(lst->head == -1) // hardcode
	{
		lst->head = 1;
		lst->tail = 1;

		(lst->next)[new_elem_ID] = 0;
		(lst->prev)[new_elem_ID] = 0;

		(lst->data)[new_elem_ID] = value;
	}
	else
	{
		printf("new_elem_ID == %d\n", new_elem_ID);
		(lst->data)[new_elem_ID] = value;
		(lst->next)[new_elem_ID] = lst->head;
		(lst->prev)[lst->head]   = new_elem_ID;
		(lst->head) 			 = new_elem_ID;
		(lst->prev)[lst->head] 	 = 0;
	}

	list_dump(lst);

	return function_error;
}

struct List_del_result list_del(struct List *lst)
{
	struct List_del_result result =
	{
		.error_code = ALL_GOOD,
		.deleted_element = POIZON,
	};

	int new_head = (lst->next)[lst->head];

	result.deleted_element = (lst->data)[lst->head];

	(lst->data)[lst->head] = 0;
	(lst->next)[lst->head] = -1;
	(lst->prev)[lst->head] = -1;

	STACK_PUSH(&(lst->free), lst->head);

	lst->head = new_head;
	(lst->prev)[new_head] = 0;

	list_dump(lst);

	return result;
}

enum Err_ID list_insert(struct List *lst, lst_elem_t value, int data_ID)
{
	enum Err_ID function_error = ALL_GOOD;

	if( ((lst->next)[data_ID] == free_elem_marker) ||
		(data_ID <= 0) ||
		(!cmp_double((lst->data)[data_ID], POIZON)) )
	{
		return INVALID_INDEX;
	}

	int new_elem_ID = STACK_POP(&(lst->free)).deleted_element;


	(lst->data)[new_elem_ID] = value;
	(lst->next)[new_elem_ID] = (lst->next)[data_ID];
	(lst->prev)[(lst->next)[data_ID]] = new_elem_ID;
	(lst->next)[data_ID] = new_elem_ID;
	(lst->prev)[new_elem_ID] = data_ID;

	list_dump(lst);

	return function_error;
}

enum Err_ID list_realloc(List *lst)
{
	enum Err_ID function_error = ALL_GOOD;

	size_t data_size = sizeof(lst_elem_t) * lst->capacity;
	size_t next_prev_size = sizeof(int) * lst->capacity;

	lst->data = (lst_elem_t *)realloc(lst->data, data_size);
	if(lst->data == NULL)
	{
		return NOT_ENOUGH_MEM;
	}

	lst->next = (int *)realloc(lst->next, next_prev_size);
	if(lst->next == NULL)
	{
		return NOT_ENOUGH_MEM;
	}

	lst->prev = (int *)realloc(lst->prev, next_prev_size);
	if(lst->prev == NULL)
	{
		return NOT_ENOUGH_MEM;
	}


	if(lst->data == NULL)
	{
		return NOT_ENOUGH_MEM;
	}

	size_t new_allocated_areas_start_ID = (lst->capacity / 2) - 1;
	push_all_elems_from_end(&(lst->free), lst->capacity - 1, new_allocated_areas_start_ID);
	set_to_default_free_elem_marker(lst->next, lst->capacity, new_allocated_areas_start_ID);
	set_to_default_free_elem_marker(lst->prev, lst->capacity, new_allocated_areas_start_ID);

	return function_error;
}

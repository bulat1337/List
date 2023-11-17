#include <stdio.h>
#include <stdlib.h>

#include "list.h"

enum Err_ID list_ctor(struct List *lst, size_t starter_capacity)
{
	enum Err_ID function_error = ALL_GOOD; // TD: function - obvious

	lst->capacity = starter_capacity;

	lst->node = (List_node *)calloc(lst->capacity, sizeof(List_node));
	// TD: calloc ret value check

	lst->current_free = 0;

	mark_list_nodes_as_free(lst->node, lst->capacity, 0);

	size_t amount_of_free_elems = lst->capacity;
	STACK_CTOR(&(lst->free), amount_of_free_elems);
	push_all_elems_from_end(&(lst->free), lst->capacity - 1, 0);

	lst->head = FREE_ELEM_MARKER;
	lst->tail = FREE_ELEM_MARKER;

	(lst->node)[0].next = lst->head;
	(lst->node)[0].prev = lst->tail;


	lst->node[0].data = POISON;

	lst->log_file = fopen("log_file", "w"); // TD: check ret value

	list_dump(lst);

	return function_error;
}

enum Err_ID list_dump(const struct List *lst) // TD: 100 percents cases
{
	enum Err_ID function_error = ALL_GOOD;

	FILE *log_file = lst->log_file;
	fprintf(log_file, "ID:"); //  TD: LOG(...)
	fprintf(log_file, "        data:");
	fprintf(log_file, "        next:");
	fprintf(log_file, "        prev:");
	fprintf(log_file, "        free:\n");
	for(size_t ID = 0; ID < lst->capacity; ID++)
	{
		fprintf(log_file, "[%lu]%13.3lf", ID, (lst->node)[ID].data);
		fprintf(log_file, "%13.d", (lst->node)[ID].next);
		fprintf(log_file, "%13.d", (lst->node)[ID].prev);
		if(ID <= lst->free.capacity)
		{
			fprintf(log_file, "%13.d\n", (lst->free.data)[ID]);
		}
		else
		{
			fprintf(log_file, "\n");
		}
	}

	size_t current_free_ID = get_current_free_ID(lst);

	fprintf(log_file, "current free: %d\n", (lst->free.data)[current_free_ID]);
	fprintf(log_file, "head: %d\n", lst->head);
	fprintf(log_file, "tail: %d\n", lst->tail);
	fprintf(log_file, "capacity: %lu\n", lst->capacity);
	fprintf(log_file, "free elems_left: %lu\n", lst->free.size);

	return function_error;
}

enum Err_ID mark_list_nodes_as_free(struct List_node *node, size_t size, size_t start_ID)
{
	enum Err_ID function_error = ALL_GOOD;

	for(size_t ID = start_ID; ID < size - 1; ID++)
	{
		node[ID].next = ID + 1;
		node[ID].prev = FREE_ELEM_MARKER;
	}

	node[size - 1].next = 0;
	node[size - 1].prev = FREE_ELEM_MARKER;

	return function_error;
}

enum Err_ID push_all_elems_from_end(struct Stack *stk, size_t end_ID, size_t start_ID)
{
	enum Err_ID function_error = ALL_GOOD;
	for(size_t ID = end_ID; ID > start_ID; ID--)
	{
		STACK_PUSH(stk, (elem_t)ID);
	}

	return function_error;
}

enum Err_ID list_add(struct List *lst, lst_elem_t value)
{
	enum Err_ID function_error = ALL_GOOD;

	struct Stack_pop_result pop_result = STACK_POP(&(lst->free));
	

	if(pop_result.error_code == SIZE_ZERO)
	{
		fprintf(lst->log_file, "pop_result.error_code == %d so im reallocating\n",
			pop_result.error_code);

		increase_list_capacity(lst);

		list_realloc(lst);
		pop_result.deleted_element = STACK_POP(&(lst->free)).deleted_element;
	}
	else
	{
		fprintf(lst->log_file, "pop_result.error_code == %d so im NOT reallocating\n",
			pop_result.error_code);
	}

	if(lst->head == FREE_ELEM_MARKER)
	{
		lst->head = 1;
		lst->tail = 1;

		(lst->node)[pop_result.deleted_element].next = 0;
		(lst->node)[pop_result.deleted_element].prev = 0;

		(lst->node)[pop_result.deleted_element].data = value;
	}
	else
	{
		fprintf(lst->log_file, "pop_result.deleted_element == %d\n", pop_result.deleted_element);
		(lst->node)[pop_result.deleted_element].data = value;
		(lst->node)[pop_result.deleted_element].next = lst->head;
		(lst->node)[lst->head].prev   = pop_result.deleted_element;
		(lst->head) 			 = pop_result.deleted_element;
		(lst->node)[lst->head].prev 	 = 0;
	}

	update_lists_fictitious_elem(lst);

	list_dump(lst);

	return function_error;
}

struct List_del_result list_del(struct List *lst)
{
	struct List_del_result result =
	{
		.function_error = ALL_GOOD,
		.deleted_element = POISON,
	};

	int new_head = (lst->node)[lst->head].next;

	result.deleted_element = (lst->node)[lst->head].data;

	(lst->node)[lst->head].data =  0;
	(lst->node)[lst->head].next = FREE_ELEM_MARKER;
	(lst->node)[lst->head].prev = FREE_ELEM_MARKER;

	STACK_PUSH(&(lst->free), lst->head);

	lst->head = new_head;
	(lst->node)[new_head].prev = 0;

	update_lists_fictitious_elem(lst);

	list_dump(lst);

	return result;
}

struct List_insert_result list_insert(struct List *lst, lst_elem_t value, int data_ID)
{
	struct List_insert_result result =
	{
		.function_error = ALL_GOOD,
		.elem_data_ID = data_ID,
	};

	if( ((lst->node)[data_ID].next == FREE_ELEM_MARKER) ||
		(data_ID <= 0) ||
		(!cmp_double((lst->node)[data_ID].data, POISON)) )
	{
		result.function_error = INVALID_INDEX;
		result.elem_data_ID = FREE_ELEM_MARKER;
		return result;
	}

	struct Stack_pop_result pop_result = STACK_POP(&(lst->free));

	if(pop_result.error_code == SIZE_ZERO)
	{
		fprintf(lst->log_file, "pop_result.error_code == %d so im reallocating\n",
			pop_result.error_code);

		increase_list_capacity(lst);

		list_realloc(lst);
		pop_result.deleted_element = STACK_POP(&(lst->free)).deleted_element;
	}
	else
	{
		fprintf(lst->log_file, "pop_result.error_code == %d so im NOT reallocating\n",
			pop_result.error_code);
	}


	(lst->node)[pop_result.deleted_element].data = value;
	(lst->node)[pop_result.deleted_element].next = (lst->node)[data_ID].next;
	(lst->node)[(lst->node)[data_ID].next].prev = pop_result.deleted_element;
	(lst->node)[data_ID].next = pop_result.deleted_element;
	(lst->node)[pop_result.deleted_element].prev = data_ID;

	update_lists_fictitious_elem(lst);

	list_dump(lst);

	return result;
}

enum Err_ID list_realloc(List *lst)
{
	enum Err_ID function_error = ALL_GOOD;

	size_t data_size = sizeof(struct List_node) * lst->capacity;

	lst->node = (struct List_node *)realloc(lst->node, data_size);
	if(lst->node == NULL)
	{
		return NOT_ENOUGH_MEM;
	}

	size_t new_allocated_areas_start_ID = (lst->capacity / 2) - 1; // TD: hardcode
	int saved_last_elems_next = lst->node[new_allocated_areas_start_ID].next;
	push_all_elems_from_end(&(lst->free), lst->capacity - 1, new_allocated_areas_start_ID);
	lst->current_free = new_allocated_areas_start_ID;
	mark_list_nodes_as_free(lst->node, lst->capacity, new_allocated_areas_start_ID);
	lst->node[new_allocated_areas_start_ID].next = saved_last_elems_next;

	return function_error;
}

enum Err_ID update_lists_fictitious_elem(struct List *lst)
{
	(lst->node)[0].next = lst->head;
	(lst->node)[0].prev = lst->tail;

	return ALL_GOOD;
}

enum Err_ID increase_list_capacity(struct List *lst)
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

enum Err_ID list_verifier(struct List *lst)
{
	fprintf(lst->log_file, "LIST_VERIFIER_LOG:\n");
	enum Err_ID function_error = ALL_GOOD;

	int list_ID = lst->head;
	while(list_ID != lst->tail)
	{
		if(lst->node[lst->node[list_ID].next].prev != list_ID)
		{
			fprintf(lst->log_file, "ERROR: lst->node[lst->node[%d].next].prev [%d] != %d\n",
				list_ID, lst->node[lst->node[list_ID].next].prev, list_ID);
			function_error = LIST_LINK_ERROR;
		}
		else
		{
			fprintf(lst->log_file, "lst->node[lst->node[%d].next].prev [%d] == %d\n",
				list_ID, lst->node[lst->node[list_ID].next].prev, list_ID);
		}

		list_ID = lst->node[list_ID].next;
	}

	return function_error;
}

struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct List *lst)
{
	struct Generate_code_for_graphic_dump_result result =
	{
		.function_error = ALL_GOOD,
		.graphic_dump_code_file_ptr = fopen("list_graphic_dump.dot", "w"),
	};

	if(result.graphic_dump_code_file_ptr == NULL)
	{
		fprintf(stderr, "Unable to open list_graphic_dump.dot\n");

		result.function_error = UNABLE_TO_OPEN_FILE;
		return result;
	}

	#define WRITE_TO_DUMP_FILE(...) fprintf(result.graphic_dump_code_file_ptr, __VA_ARGS__);

	WRITE_TO_DUMP_FILE("digraph list{\n"
	"rankdir = TB;\n"
	"ranksep = %d\n"
	"splines = ortho;\n"
	"bgcolor = \"#FFDAB9\";\n"
	"edge[minlen = 3, penwidth = 3; color = \"black\"];\n"
	"node[shape = \"rectangle\", style = \"rounded, filled\",\n"
	"\tfillcolor = \"#F08080\",\n"
	"\tfontsize = 30,\n"
	"\theight = 3,\n"
	"\tpenwidth = 5, color = \"#F0E68C\"]\n"
	"\n"
	"{rank = max;\n"
	"\tbelow_node [label = \"Don't look at me\", width = 3, style = invis];\n"
	"}\n", 1 + lst->capacity / 10);

	size_t current_free_ID = get_current_free_ID(lst);

	//	print_list_manager
	WRITE_TO_DUMP_FILE("{rank = min;\n"
	"\tlist_manager [shape=Mrecord, fillcolor = \"#1E90FF\","
	" label= \"{HEAD: %d| TAIL: %d| CUR_FREE: %d | CAPACITY: %lu | FREE_ELEMS_LEFT: %lu}\"];\n"
	"}\n", lst->head, lst->tail, lst->current_free, lst->capacity, lst->free.size);

	//	printf_nodes
	WRITE_TO_DUMP_FILE("{rank = same;\n");

	// create_node(name, color, label);

	//	print_dummy
	WRITE_TO_DUMP_FILE("\tdummy [shape = Mrecord,label = "
	"\"{DUMMY | ID: 0 | val: %.2lf | {next: %d | prev: %d}}\"];\n",
		lst->node[0].data, lst->node[0].next, lst->node[0].prev);

	if(lst->head >= 0)
	{
		//	print_head
		WRITE_TO_DUMP_FILE("\tnode_%d [shape = Mrecord, "
		"fillcolor = \"#00FA9A\", label = \"{ID: %d | val: %.2lf |{next: %d | prev: %d}}\" ];\n",
			lst->head, lst->head, lst->node[lst->head].data, lst->node[lst->head].next,
				lst->node[lst->head].prev);
	}

	if(lst->tail >= 0)
	{
		//	print_tail
		WRITE_TO_DUMP_FILE("\tnode_%d [shape = Mrecord, "
		"fillcolor = \"#DC143C\", label = \"{ID: %d | val: %.2lf |{next: %d | prev: %d}}\" ];\n",
			lst->tail, lst->tail, lst->node[lst->tail].data, lst->node[lst->tail].next,
				lst->node[lst->tail].prev);
	}

	if(lst->head >= 0)
	{
		//	print_regular_nodes
		for(int elem_ID = lst->node[lst->head].next;
				elem_ID != lst->tail; elem_ID = lst->node[elem_ID].next)
		{
			WRITE_TO_DUMP_FILE(
			"\tnode_%d [shape = Mrecord, label = \"{ID: %d | val: %.2lf |{next: %d | prev: %d}}\" ];\n",
				elem_ID, elem_ID, lst->node[elem_ID].data,
					lst->node[elem_ID].next, lst->node[elem_ID].prev);
		}
	}

	//	print_free_nodes
	{	//	ограничение области видимости elem_ID
		int elem_ID = -1;
		for(int ID = (int)lst->free.size - 1; ID >= 0; ID--)
		{
			elem_ID = lst->free.data[ID];

			//	get_free_elem_color
			char *color_code = "#808080"; // TD: consts with informative name

			if(elem_ID == lst->free.data[current_free_ID])
			{
				color_code = "#C0C0C0";
			}

			WRITE_TO_DUMP_FILE(
			"\tnode_%d [shape = Mrecord, fillcolor = \"%s\","
			" label = \"{ID: %d | val: %.2lf |{next: %d | prev: %d}}\" ];\n",
				elem_ID, color_code, elem_ID, lst->node[elem_ID].data,
					lst->node[elem_ID].next, lst->node[elem_ID].prev);
		}
	}

	WRITE_TO_DUMP_FILE("}\n\n");

	// TD: connect_nodes();

	//	print_aligning_edges
	WRITE_TO_DUMP_FILE(
	"\tlist_manager -> dummy[weight = 5, style = invis];\n"
	"\tbelow_node -> dummy[weight = 5, style = invis];\n");

	WRITE_TO_DUMP_FILE("\n\t");

	WRITE_TO_DUMP_FILE("dummy -> ");
	for(int elem_ID = 1; elem_ID < (int)lst->capacity - 1; elem_ID++)
	{
		WRITE_TO_DUMP_FILE("node_%d -> ", elem_ID);
	}

	WRITE_TO_DUMP_FILE(
		"node_%lu [weight = 5, style = invis];\n", lst->capacity - 1);


	WRITE_TO_DUMP_FILE("\n\t");

	if(lst->head >= 0)
	{
		//	print_logic_edges
		for(int elem_ID = lst->head;
				elem_ID != lst->tail; elem_ID = lst->node[elem_ID].next)
		{
			WRITE_TO_DUMP_FILE("node_%d -> ", elem_ID);
		}
	}

	if(lst->tail >= 0)
	{
		WRITE_TO_DUMP_FILE("node_%d;\n", lst->tail);
	}

	WRITE_TO_DUMP_FILE("}");

	#undef WRITE_TO_DUMP_FILE

	fclose(result.graphic_dump_code_file_ptr);
	return result;
}

// size_t get_current_free_ID(const struct List * lst)
// {
// 	size_t current_free_ID = 0;
//
// 	if(lst->free.size != 0)
// 	{
// 		current_free_ID = lst->free.size - 1;
// 	}
//
// 	return current_free_ID;
// }

enum Err_ID list_linearize(struct List *lst)
{
	enum Err_ID error_code = ALL_GOOD;
	lst_elem_t *linearized_data = (lst_elem_t *)calloc(lst->capacity, sizeof(lst_elem_t));

	linearized_data[0] = POISON;

	int current_data_ID = lst->node[lst->head].prev;
	size_t linearized_data_ID = 0;

	do
	{
		current_data_ID = lst->node[current_data_ID].next;

		linearized_data_ID++;

		linearized_data[linearized_data_ID] = lst->node[current_data_ID].data;

		printf("%lf[%lu]\tcurrent_data_ID: %d\n",
			linearized_data[linearized_data_ID], linearized_data_ID, current_data_ID);

	} while(current_data_ID != lst->tail);



	// memcpy(void *dest, const void *source, size_t count)

	return error_code;
}

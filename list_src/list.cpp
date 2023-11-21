#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "list_helpers.h"

struct List
{
	struct List_node *node;
	int head;
	int tail;
	int current_free;
	size_t capacity;
};

FILE *log_file;

#define SMART_CALLOC(allocation_ptr, type, amount_of_elems)\
	allocation_ptr = (type *)calloc((amount_of_elems), sizeof(type));\
	if(allocation_ptr == NULL)\
	{\
		fprintf(stderr, "Unable to allocate "#allocation_ptr"\n");\
		error_code = NOT_ENOUGH_MEM;\
		return error_code;\
	}

#define SMART_FOPEN(file_ptr, file_name, open_mode)\
	file_ptr = fopen(file_name, open_mode);\
	if(file_ptr == NULL)\
	{\
		fprintf(stderr, "Unable to open "#file_name"\n");\
		error_code = UNABLE_TO_OPEN_FILE;\
		return error_code;\
	}\

struct List_ctor_result list_ctor(size_t starter_capacity)
{
	struct List_ctor_result result =
	{
		.error_code = ALL_GOOD,
		.lst = (struct List *)calloc(1, sizeof(struct List)),
	};

	result.lst->capacity = starter_capacity;

	result.lst->node =(List_node *)calloc(result.lst->capacity, sizeof(List_node));
	if(result.lst->node == NULL)
	{
		fprintf(stderr, "Unable to allocate result.lst->node\n");
		result.error_code = NOT_ENOUGH_MEM;
		result.lst = NULL;
		return result;
	}

	result.lst->current_free = 1;

	mark_list_nodes_as_free(result.lst->node, result.lst->capacity, 0);

	result.lst->head = FREE_ELEM_MARKER;
	result.lst->tail = FREE_ELEM_MARKER;

	(result.lst->node)[0].next = result.lst->head;
	(result.lst->node)[0].prev = result.lst->tail;


	result.lst->node[0].data = POISON;

	log_file = fopen("log_file.txt", "w");
	if(log_file == NULL)
	{
		fprintf(stderr, "Unable to open log_file.txt\n");
		fprintf(stderr, "log_file is now stdout\n");
		log_file = stdout;
	}

	list_dump(result.lst, result.error_code, __func__);

	return result;
}

error_t list_dump(const struct List *lst, error_t error_code, const char *func_name)
{
	error_t dump_function_error_code = ALL_GOOD;

	DUMP_W_COND(func_name, "dump called from %s function:\n", func_name);

	if(lst == NULL)
	{
		WRITE_IN_LOG("FATAL_ERROR: pointer to the list structure is NULL\n");
		WRITE_IN_LOG("\tlst[%p]\n", lst);

		return LST_NULL_PTR;
	}

	#define ERROR_CHECK(err_type)\
		if(error_code & err_type)\
		{\
			WRITE_IN_LOG("-");\
			WRITE_IN_LOG(#err_type);\
			WRITE_IN_LOG("\n");\
		}\

	DUMP_W_COND(lst->node, "ID:");
	DUMP_W_COND(lst->node->data, "        data:");
	DUMP_W_COND(lst->node->next, "        next:");
	DUMP_W_COND(lst->node->prev, "        prev:\n");
	for(size_t ID = 0; ID < lst->capacity; ID++)
	{
		DUMP_W_COND(&ID &&  &((lst->node)[ID].data),"[%lu]%13.3lf", ID, (lst->node)[ID].data);
		DUMP_W_COND(&((lst->node)[ID].next), "%13.d", (lst->node)[ID].next);
		DUMP_W_COND(&((lst->node)[ID].prev), "%13.d", (lst->node)[ID].prev);

		WRITE_IN_LOG("\n");

	}

	// size_t current_free_ID = get_current_free_ID(lst);

	DUMP_W_COND(&(lst->current_free), "current free: %d\n", lst->current_free);
	DUMP_W_COND(&(lst->head), "head: %d\n", lst->head);
	DUMP_W_COND(&(lst->tail), "tail: %d\n", lst->tail);
	DUMP_W_COND(&(lst->capacity), "capacity: %lu\n", lst->capacity);

	ERROR_CHECK(NOT_ENOUGH_MEM);
	ERROR_CHECK(UNABLE_TO_OPEN_FILE);
	ERROR_CHECK(DATA_POIZON_VALUE);
	ERROR_CHECK(INVALID_INDEX);
	ERROR_CHECK(LIST_LINK_ERROR);
	ERROR_CHECK(LST_NULL_PTR);

	#undef ERROR_CHECK

	return dump_function_error_code;
}

error_t list_add(struct List *lst, lst_elem_t value)
{
	error_t error_code = ALL_GOOD;

	if(lst->node[lst->current_free].next == 0)
	{
		WRITE_IN_LOG("lst->node[lst->current_free].next == %d so im reallocating\n",
			lst->node[lst->current_free].next);

		increase_list_capacity(lst);

		list_realloc(lst);
	}

	int next_current_free = lst->node[lst->current_free].next;

	if(lst->head == FREE_ELEM_MARKER)
	{
		lst->head = 1;
		lst->tail = 1;

		(lst->node)[lst->current_free].next = 0;
		(lst->node)[lst->current_free].prev = 0;

		(lst->node)[lst->current_free].data = value;
	}
	else
	{
		WRITE_IN_LOG("lst->current_free == %d\n", lst->current_free);
		(lst->node)[lst->current_free].data = value;
		(lst->node)[lst->current_free].next = lst->head;
		(lst->node)[lst->head].prev 		= lst->current_free;
		(lst->head) 			 			= lst->current_free;
		(lst->node)[lst->head].prev 	 	= 0;
	}

	lst->current_free = next_current_free;
	update_lists_fictitious_elem(lst);

	list_dump(lst, error_code, __func__);

	return error_code;
}

struct List_del_result list_del(struct List *lst)
{
	struct List_del_result result =
	{
		.error_code = ALL_GOOD,
		.deleted_element = POISON,
	};

	int new_head = (lst->node)[lst->head].next;

	result.deleted_element = (lst->node)[lst->head].data;

	(lst->node)[lst->head].data =  0;
	(lst->node)[lst->head].next = FREE_ELEM_MARKER;
	(lst->node)[lst->head].prev = FREE_ELEM_MARKER;

	int temp_old_current_free = lst->current_free;
	lst->current_free = lst->head;
	lst->node[lst->current_free].next = temp_old_current_free;

	// STACK_PUSH(&(lst->free), lst->head);

	lst->head = new_head;
	(lst->node)[new_head].prev = 0;

	update_lists_fictitious_elem(lst);

	list_dump(lst, result.error_code, __func__);

	return result;
}

struct List_insert_result list_insert(struct List *lst, lst_elem_t value, int data_ID)
{
	struct List_insert_result result =
	{
		.error_code = ALL_GOOD,
		.elem_data_ID = data_ID,
	};

	if(((lst->node)[data_ID].next == FREE_ELEM_MARKER) ||
		(data_ID <= 0) ||
		(!cmp_double((lst->node)[data_ID].data, POISON)) )
	{
		result.error_code = INVALID_INDEX;
		result.elem_data_ID = FREE_ELEM_MARKER;
		return result;
	}

	if(lst->node[lst->current_free].next == 0)
	{
		WRITE_IN_LOG("lst->node[lst->current_free].next == %d so im reallocating\n",
			lst->node[lst->current_free].next);

		increase_list_capacity(lst);

		list_realloc(lst);
	}

	int next_current_free = lst->node[lst->current_free].next;

	(lst->node)[lst->current_free].data = value;
	(lst->node)[lst->current_free].next = (lst->node)[data_ID].next;
	(lst->node)[(lst->node)[data_ID].next].prev = lst->current_free;
	(lst->node)[data_ID].next = lst->current_free;
	(lst->node)[lst->current_free].prev = data_ID;

	lst->current_free = next_current_free;

	update_lists_fictitious_elem(lst);

	list_dump(lst, result.error_code, __func__);

	return result;
}

error_t list_verifier(struct List *lst)
{
	WRITE_IN_LOG("LIST_VERIFIER_LOG:\n");
	error_t error_code = ALL_GOOD;

	int list_ID = lst->head;
	while(list_ID != lst->tail)
	{
		if(lst->node[lst->node[list_ID].next].prev != list_ID)
		{
			WRITE_IN_LOG("ERROR: lst->node[lst->node[%d].next].prev [%d] != %d\n",
				list_ID, lst->node[lst->node[list_ID].next].prev, list_ID);
			error_code = LIST_LINK_ERROR;
		}
		else
		{
			WRITE_IN_LOG("lst->node[lst->node[%d].next].prev [%d] == %d\n",
				list_ID, lst->node[lst->node[list_ID].next].prev, list_ID);
		}

		list_ID = lst->node[list_ID].next;
	}

	return error_code;
}

struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct List *lst)
{
	struct Generate_code_for_graphic_dump_result result =
	{
		.error_code = ALL_GOOD,
		.graphic_dump_code_file_ptr = fopen("list_graphic_dump.dot", "w"),
	};

	if(result.graphic_dump_code_file_ptr == NULL)
	{
		fprintf(stderr, "Unable to open list_graphic_dump.dot\n");

		result.error_code = UNABLE_TO_OPEN_FILE;
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

	//	print_list_manager
	WRITE_TO_DUMP_FILE("{rank = min;\n"
	"\tlist_manager [shape=Mrecord, fillcolor = \"#1E90FF\","
	" label= \"{HEAD: %d| TAIL: %d| CUR_FREE: %d | CAPACITY: %lu}\"];\n"
	"}\n", lst->head, lst->tail, lst->current_free, lst->capacity);

	//	printf_nodes
	WRITE_TO_DUMP_FILE("{rank = same;\n");

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

	if((lst->head >= 0) && (lst->node[lst->head].next != 0))
	{
		//	print_regular_nodes
		for(int elem_ID = lst->node[lst->head].next;
				elem_ID != lst->tail; elem_ID = lst->node[elem_ID].next)
		{
			WRITE_TO_DUMP_FILE(
			"\tnode_%d [shape = Mrecord, label = \"{ID: %d | val: %.2lf |"
			"{next: %d | prev: %d}}\" ];\n",
				elem_ID, elem_ID, lst->node[elem_ID].data,
					lst->node[elem_ID].next, lst->node[elem_ID].prev);
		}
	}

	//	print_free_nodes
	{	//	ограничение области видимости elem_ID

		int elem_ID = lst->current_free;

		while(elem_ID != 0)
		{
			char *color_code = "#808080"; // TD: consts with informative name

			if(elem_ID == lst->current_free)
			{
				color_code = "#C0C0C0";
			}

			WRITE_TO_DUMP_FILE(
			"\tnode_%d [shape = Mrecord, fillcolor = \"%s\","
			" label = \"{ID: %d | val: %.2lf |{next: %d | prev: %d}}\" ];\n",
				elem_ID, color_code, elem_ID, lst->node[elem_ID].data,
					lst->node[elem_ID].next, lst->node[elem_ID].prev);

			elem_ID = lst->node[elem_ID].next;
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

error_t list_linearize(struct List *lst)
{
	error_t error_code = ALL_GOOD;
	lst_elem_t *linearized_data = NULL;
	SMART_CALLOC(linearized_data, lst_elem_t, lst->capacity);

	// fill linearized_data w non_free_elems
	linearized_data[0] = POISON;

	int current_data_ID = lst->node[lst->head].prev;
	size_t linearized_data_ID = 0;

	do
	{
		current_data_ID = lst->node[current_data_ID].next;

		linearized_data_ID++;

		linearized_data[linearized_data_ID] = lst->node[current_data_ID].data;

		WRITE_IN_LOG("%lf[%lu]\tcurrent_data_ID: %d\n",
			linearized_data[linearized_data_ID], linearized_data_ID, current_data_ID);

	} while(current_data_ID != lst->tail);

	int last_non_free_elem_ID = linearized_data_ID;
	WRITE_IN_LOG("last_non_free_elem_ID == %d\n", last_non_free_elem_ID);

	// fill linearized_data w free_elems
	linearized_data_ID++;
	current_data_ID = lst->current_free;

	WRITE_IN_LOG("lst->current_free == %d\n", lst->current_free);

	while(lst->node[current_data_ID].next != 0)
	{
		linearized_data[linearized_data_ID] = lst->node[current_data_ID].data;

		WRITE_IN_LOG("%lf[%lu]\tcurrent_data_ID: %d\n",
			linearized_data[linearized_data_ID], linearized_data_ID, current_data_ID);

		current_data_ID = lst->node[current_data_ID].next;
		linearized_data_ID++;
	}

	linearized_data[linearized_data_ID] = lst->node[current_data_ID].data;

	WRITE_IN_LOG("%lf[%lu]\tcurrent_data_ID: %d\n",
		linearized_data[linearized_data_ID], linearized_data_ID, current_data_ID);

	// copy non_free_elems in data
	for(size_t ID = 0; ID < last_non_free_elem_ID + 1; ID++)
	{
		lst->node[ID].data = linearized_data[ID];
		lst->node[ID].next = (int)(ID + 1);
		lst->node[ID].prev = (int)(ID - 1);

	}

	lst->node[last_non_free_elem_ID].next = 0;

	// copy free_elems in data
	for(size_t ID = last_non_free_elem_ID + 1; ID < lst->capacity - 1; ID++)
	{
		lst->node[ID].data = linearized_data[ID];
		lst->node[ID].next = (int)(ID + 1);
		lst->node[ID].prev = FREE_ELEM_MARKER;

	}

	lst->node[lst->capacity - 1].next = 0;

	lst->head = 1;
	lst->tail = last_non_free_elem_ID;
	lst->current_free = last_non_free_elem_ID + 1;

	WRITE_IN_LOG("head after linearization: %d\n", lst->head);
	WRITE_IN_LOG("tail after linearization: %d\n", lst->tail);
	WRITE_IN_LOG("current_free after linearization: %d\n", lst->current_free);

	list_dump(lst, error_code, __func__);

	return error_code;
}

// Функция для удаления элемента по индексу data_ID
struct List_remove_result list_remove(struct List *lst, int data_ID)
{
	struct List_remove_result result =
	{
		.error_code = ALL_GOOD,
		.removed_data = POISON, // Или другое значение по умолчанию
	};

	if (data_ID <= 0 || data_ID >= lst->capacity)
	{
		result.error_code = INVALID_INDEX;
		return result;
	}

	if (lst->node[data_ID].next == 0 || !cmp_double(lst->node[data_ID].data, POISON))
	{
		// Попытка удаления пустого элемента или элемента с концом списка (tail)
		result.error_code = INVALID_INDEX;
		return result;
	}

	int prev_ID = lst->node[data_ID].prev;
	int next_ID = lst->node[data_ID].next;

	lst_elem_t removed_data = lst->node[data_ID].data;

	// Обновляем связи соседних элементов
	lst->node[prev_ID].next = next_ID;
	lst->node[next_ID].prev = prev_ID;

	// Отмечаем удаленный элемент как свободный
	lst->node[data_ID].next = lst->current_free;
	lst->current_free = data_ID;

	result.removed_data = removed_data;

	update_lists_fictitious_elem(lst);

	list_dump(lst, result.error_code, __func__);

	return result;
}

// error_t create_node(char *node_name, char *node_color, char *label);


#undef SMART_CALLOC
#undef SMART_FOPEN


#ifndef LIST_H
#define LIST_H

#include <math.h>

#include "../stack_src/stack.h"

typedef double lst_elem_t;

struct List_node
{
	lst_elem_t data;
	int next;
	int prev;
};


/*

d;	[1] [2] [f] [3] [f] [f] [5]

n;

p;

*/

struct List
{
	struct List_node *node;
	int head;
	int tail;
	int current_free;
	struct Stack free;
	size_t capacity;
	FILE *log_file; // TD: make static in .cpp / DEBUG_ON
};

struct List_del_result
{
	enum Err_ID function_error;
	lst_elem_t deleted_element;
};

struct List_insert_result
{
	enum Err_ID function_error;
	int elem_data_ID;
};

struct Generate_code_for_graphic_dump_result
{
	enum Err_ID function_error;
	FILE *graphic_dump_code_file_ptr;
};

// struct
// {
// 	error;
//
// 	union
// 	{
// 		int elem;
// 		double delem;
// 		...
// 	};
// };

const int FREE_ELEM_MARKER = -1;
const lst_elem_t POISON = NAN;
const size_t LIST_REALLOC_FROM_ZERO_CAPACITY = 5;
const size_t LIST_REALLOC_COEFF  = 2;

// typedef enum ююю
// DSL
// linear -> not all functions break linearization
enum Err_ID list_ctor(struct List *lst, size_t starter_capacity);
enum Err_ID mark_list_nodes_as_free(struct List_node *node, size_t size, size_t start_ID);
enum Err_ID push_all_elems_from_end(struct Stack *stk, size_t end_ID, size_t start_ID); // TD: rename
enum Err_ID list_dump(const struct List *lst);
enum Err_ID list_add(struct List *lst, lst_elem_t value);
struct List_del_result list_del(struct List *lst); // TD: rename, delete by ID
struct List_insert_result list_insert(struct List *lst, lst_elem_t value, int data_ID);
enum Err_ID list_realloc(List *lst);
enum Err_ID update_lists_fictitious_elem(struct List *lst); // TD: why give to user?
enum Err_ID increase_list_capacity(struct List *lst);
enum Err_ID list_verifier(struct List *lst);
struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct List *lst);
// size_t get_current_free_ID(const struct List * lst);
enum Err_ID list_linearize(struct List *lst);


#endif

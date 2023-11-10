#ifndef LIST_H
#define LIST_H

#include <math.h>

#include "../stack_src/stack.h"

typedef double lst_elem_t;

// struct List_node
// {
// 	lst_elem_t data;
// 	int next;
// 	int prev;
// }

struct List
{
	lst_elem_t *data;
	int *next;
	int *prev;
	int head;
	int tail;
	struct Stack free;
	size_t capacity;
};

struct List_del_result
{
	enum Err_ID error_code;
	lst_elem_t deleted_element;
};

const int free_elem_marker = -1; // consts are btetter to name in caps
const lst_elem_t POIZON = NAN; // if double? grammar
const size_t list_expantion_size_from_zero = 5; // grammar
const size_t list_reformation_coeff  = 2; // more straightforward names

enum Err_ID list_ctor(struct List *lst, size_t starter_capacity);
enum Err_ID set_to_default_free_elem_marker(int *array, size_t size, size_t start_ID); //
enum Err_ID push_all_elems_from_end(struct Stack *stk, size_t end_ID, size_t start_ID);
// enum Err_ID dump_all_elems(FILE *log_file, lst_elem_t *array, size_t size);
enum Err_ID list_dump(const struct List *lst);
enum Err_ID list_add(struct List *lst, lst_elem_t value);
struct List_del_result list_del(struct List *lst);
enum Err_ID list_insert(struct List *lst, lst_elem_t value, int data_ID); // return физ адрс элта
enum Err_ID list_realloc(List *lst);


#endif

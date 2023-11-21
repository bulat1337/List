#ifndef LIST_H
#define LIST_H

#include <math.h>

#include "../stack_src/stack.h"

#define WRITE_IN_LOG(...)\
	fprintf(log_file, __VA_ARGS__);\

typedef double lst_elem_t;

struct List_node
{
	lst_elem_t data;
	int next;
	int prev;
};

struct List;

struct List_del_result
{
	enum Err_ID error_code;
	lst_elem_t deleted_element;
};

struct List_insert_result
{
	enum Err_ID error_code;
	int elem_data_ID;
};

struct Generate_code_for_graphic_dump_result
{
	enum Err_ID error_code;
	FILE *graphic_dump_code_file_ptr;
};

struct List_ctor_result
{
	enum Err_ID error_code;
	struct List *lst;
};

// Определение структуры для результата операции вставки/удаления
struct List_remove_result
{
	enum Err_ID error_code;
	lst_elem_t removed_data;
};

struct node_charachteristics
{
	char *name;
	char *color;
	char *label;
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
const size_t node_label_str_size = 100;

typedef enum Err_ID error_t;
// DSL
// linear -> not all functions break linearization
struct List_ctor_result list_ctor(size_t starter_capacity);
error_t list_dump(const struct List *lst, error_t error_code, const char *func_name);
error_t list_add(struct List *lst, lst_elem_t value);
struct List_del_result list_del(struct List *lst); // TD: rename, delete by ID
struct List_insert_result list_insert(struct List *lst, lst_elem_t value, int data_ID);
error_t list_verifier(struct List *lst);
struct Generate_code_for_graphic_dump_result generate_code_for_graphic_dump(struct List *lst);
error_t list_linearize(struct List *lst);
struct List_remove_result list_remove(struct List *lst, int data_ID);


#endif

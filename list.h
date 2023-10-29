#ifndef LIST_H
#define LIST_H

typedef double elem_t;

struct List
{
	elem_t *data;
	elem_t *next;
	elem_t *prev;
	elem_t head;
	elem_t tail;
	// struct Stack free;
};

void list_ctor(struct List *lst, size_t starter_capacity);

#endif

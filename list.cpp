#include <stdio.h>
#include <stdlib.h>

#include "list.h"

void list_ctor(struct List *lst, size_t starter_capacity)
{
	lst->data = (elem_t *)calloc(starter_capacity, sizeof(elem_t));
}

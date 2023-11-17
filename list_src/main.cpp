#include <stdio.h>

#include "list.h"

int main()
{
	struct List lst = {};
	list_ctor(&lst, 10);

	list_add(&lst, 40);
	list_add(&lst, 50);
	list_add(&lst, 70);
	list_add(&lst, 80);
	list_add(&lst, 90);
	list_add(&lst, 100);

	list_insert(&lst, 25, 3);
	list_insert(&lst, 25, 5);

	list_add(&lst, 75);
	list_add(&lst, 75);

	// list_linearize(&lst);

// 	list_verifier(&lst);
//
	generate_code_for_graphic_dump(&lst);

	return 0;
}

#include <stdio.h>

#include "list.h"

int main()
{
	struct List *lst = list_ctor(10).lst;

	list_add(lst, 40);
	list_add(lst, 50);
	list_add(lst, 70);
	list_add(lst, 80);
	list_add(lst, 90);


	list_insert(lst, 25, 3);
	list_insert(lst, 25, 5);

	list_add(lst, 900);
	list_add(lst, 900);

	for(size_t ID = 0; ID < 100; ID++)
	{
		list_add(lst, 900);
	}





	// list_linearize(lst);

	// struct List_remove_result remove_result = list_remove(lst, 2);

	list_verifier(lst);

	generate_code_for_graphic_dump(lst);
	list_dtor(lst);

	return 0;
}

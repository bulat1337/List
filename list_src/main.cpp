#include <stdio.h>

#include "list.h"

int main()
{
	struct List lst = {};
	list_ctor(&lst, 10);
	list_add(&lst, 40);
	list_add(&lst, 50);
	list_add(&lst, 60);
	list_add(&lst, 70);
	list_add(&lst, 80);
	list_add(&lst, 90);
	list_add(&lst, 100);
	list_add(&lst, 110);
	list_add(&lst, 120);
	list_add(&lst, 130);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);
	list_add(&lst, 666);

	// list_del(&lst);


	return 0;
}

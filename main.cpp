#include <stdio.h>

#include "list.h"

int main()
{
	struct List lst = {};
	list_ctor(&lst, 10);
	
	return 0;
}

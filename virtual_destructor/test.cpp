
#include "test.h"

#include "test_class.h"
#include <stdio.h>
#include "test_string.h"


int main(int argc, const char** argv)
{
	printf("test virtual_destructor . \n");
	
	test_CBase();
	test_CBaseTest();
	test_string();

	return 0;
}


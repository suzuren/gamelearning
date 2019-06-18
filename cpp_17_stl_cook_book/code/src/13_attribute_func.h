
#include <stdio.h>
#include <stdlib.h>


static  __attribute__((constructor)) void before()
{

	printf("__attribute__ before Hello \n");
}

static  __attribute__((destructor)) void after()
{
	printf("__attribute__ after World!\n");
}

void test_attribute_func()
{
	printf("test_attribute_func\n");
}

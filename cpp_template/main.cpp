#include <stdio.h>
#include <string>


#include "variadic_templates.h"
#include "fibonacci_templates.h"
#include "sfinae_templates.h"

int main(int argc, const char** argv)
{
	printf("hello world!\n");
	
	//test_variadic_templates();
	//test_fibonacci_templates();

	test_sfinae_templates();

	return 0;
}





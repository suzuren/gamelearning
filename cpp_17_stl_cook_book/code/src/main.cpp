#include <iostream>
#include "daemonize.h"
#include "01_Structured_Binding_Declaration.h"
#include "02_stl_container.h"
#include "03_iterator.h"
#include "04_lambda.h"
#include "05_stl_basic_algorithm.h"
int main(int argc, const char** argv)
{
	std::cout << "hello world!" << std::endl;
	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));
	//01
	//test_Structured_Binding_Declaration();
	//02
	//test_stl_container();
	//03
	//test_iterator();
	//04
	//test_lambda();
	//05
	test_stl_basic_algorithm();


	return 0;
}
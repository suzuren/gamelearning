#include <iostream>
#include "daemonize.h"
//#include "01_Structured_Binding_Declaration.h"
//#include "02_stl_container.h"
//#include "03_iterator.h"
//#include "04_lambda.h"
//#include "05_stl_basic_algorithm.h"
//#include "06_stl_advanced_algorithm.h"
//#include "07_strings_streams_regular_expressions.h"
//#include "08_utils_class.h"
//#include "09_gen_rand_number.h"
//#include "10_file_process.h"
//#include "11_c_func.h"
//#include "12_slot_card_func.h"
//#include "13_attribute_func.h"

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
	//test_stl_basic_algorithm();
	//06
	//test_stl_advanced_algorithm();
	//07
	//strings_streams_regular_expressions(argc, argv);
	//08
	//test_utils_class();
	//09
	//test_gen_rand_number();
	//10
	//test_file_process();
	//----------------
	//11
	//test_c_func();
	//12
	//test_slot_card_func();
	//13
	//test_attribute_func();

	//const double db = 1.618033988749895;
	//double tmp = db;
	//std::cout << db;
	//printf("\n");
	//std::cout << tmp;
	//printf("\n");
	//std::cout << (tmp == 1.61803398874989);
	//printf("\n");

	return 0;
}
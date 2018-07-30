#include <stdio.h>
#include <string>
using namespace std;
#include "Timer.hpp"
#include "type_name.hpp"

#include "shared.hpp"
#include "test_regex_match.h"
#include "test_lambda.h"

#include "test_chrono_time.hpp"

int main(int argc, const char** argv)
{
	printf("hello world!\n");
	
	test_typename();
	test_timer();
	test_regex();
	test_lambda();
	test_shared();

	return 0;
}





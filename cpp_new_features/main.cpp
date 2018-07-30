#include <stdio.h>
#include <string>
using namespace std;

#include "define.h"
#include "cplusplus_extern.h"
#include "template.h"
#include "smart_pointer.h"
#include "enum_class.h"

int main(int argc, const char** argv)
{
	printf("hello world! test cpp new features ... \n\n");
	
	//test_define();
	//TEST_LOG_PRINTF("x=%d", 100);
	//test_long_size();
	//test_extern_c();
	test_enum_class();
	//test_templateFunc();
	//test_smart_pointer();
	

	return 0;
}





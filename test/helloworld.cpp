
#include <stdio.h>

#include <memory.h>

#include "test_helloworld.h"
#include "test_socket_server.h"
#include "test_userright.h"
#include "KMP_algorithm.h"

int main(int argc, const char** argv)
{
	printf("hello test!\n");

	//double d1 = 0.01;
	//double d2 = 0.001;
	//double d3 = 0.0001;
	//printf("d1:%g\n", d1);
	//printf("d2:%g\n", d2);
	//printf("d3:%g\n", d3);

	//stGangCardResult GangCardResult;
	//tets_memory_zero(GangCardResult);
	//func1();
	//test_for();

	//test_socket_server();
	//testFuncUserRight();
	test_KMP();

	return 0;
}

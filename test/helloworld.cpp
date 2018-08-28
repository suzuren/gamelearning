
#include <stdio.h>

#include <memory.h>

#include "test_helloworld.h"
#include "test_socket_server.h"

int main(int argc, const char** argv)
{
	printf("hello test!\n");

	stGangCardResult GangCardResult;
	tets_memory_zero(GangCardResult);
	func1();

	test_socket_server();

	return 0;
}


#include "example_test.h"

#include <stdio.h>
#include <sys/resource.h>
#include <errno.h>

int GenCoreDumpFile(unsigned int size)
{
	struct rlimit flimit;
	flimit.rlim_cur = size;
	flimit.rlim_max = size;
	if (setrlimit(RLIMIT_CORE, &flimit) != 0)
	{
		return errno;
	}
	return 0;
}

int
main(int argc, char *argv[]) {
	GenCoreDumpFile((unsigned int)(1024UL * 1024 * 1024 * 2));
	printf("hello world.\n");

	//test_skynet_mq();

	//test_skynet_timer();

	test_skynet_socket();
}
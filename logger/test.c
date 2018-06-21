
#include <stdio.h>

#include "logger.h"


int main(int argc, const char** argv)
{
	printf("test logger ...\n");
	log_constructor_logger("data");
	int i = 0;
	for (; i < 1000900; i++)
	{
		LOG_DEBUG("hello world - %d.", i);
	}

	//log_destroy_logger();
	while (1)
	{
		//LOG_DEBUG("hello world - %d.", i++);
	}
	sleep(10);
	return 0;
}


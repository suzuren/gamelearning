
#include <stdio.h>

#include "logger.h"


int main(int argc, const char** argv)
{
	//log_constructor_logger("data");

	int i = 0;
	for (; i < 1000; i++)
	{
		LOG_DEBUG("hello world - %d.", i);
	}

	//log_destroy_logger();
	sleep(10);
	return 0;
}


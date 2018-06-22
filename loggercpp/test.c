
#include <stdio.h>

#include "logger.h"


int main(int argc, const char** argv)
{
	printf("test logger ...\n");
	log_constructor_logger("data");
	int i = 0;
	//for (; i < 100; i++)
	//{
	//	//LOG_DEBUG("hello world - %d.", i);
	//}

	//log_destroy_logger();
	while (1)
	{
		LOG_DEBUG("hello world - %d.", i++);
		log_thread_sleep(LOG_BREATHING_SPACE);
	}
	sleep(10);
	return 0;
}



#include <stdio.h>
#include "timer.h"






int main(int argc, const char** argv)
{
	printf("test timer ...\r\n");

	timer_init_info();
	timer_test();
	timer_update_time();

	


	timer_destroy_info();


	return 0;
}


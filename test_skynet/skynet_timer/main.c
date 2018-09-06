
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include "spinlock.h"
#include "skynet_timer.h"

//--------------------------------------------------------------------------

void test_mask()
{
	int mask = TIME_NEAR;
	for (int i = 0; i <= 5; i++)
	{
		printf("mian - i:%d,mask:%d,mask-1:%d\n", i, mask, mask - 1);
		mask <<= TIME_LEVEL_SHIFT;
	}
	//mian - i:0,mask:256,     mask-1:255
	//mian - i:1,mask:16384,   mask-1:16383
	//mian - i:2,mask:1048576, mask-1:1048575
	//mian - i:3,mask:67108864,mask-1:67108863
	//mian - i:4,mask:0,mask-1:-1
	//mian - i:5,mask:0,mask-1:-1
}

//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    printf("test skynet timer.\n");

	test_mask();

	uint64_t start_time = skynet_thread_time();


	skynet_timer_init();

	//uint32_t starttime = skynet_starttime();
	//uint64_t current = skynet_now();
	uint32_t handle = 16777217;
	int time = 1;
	int session = 5678;
	int ret_session = skynet_timeout(handle, time, session);
	printf("main - start_time:%lu, handle:%u, time:%d, session:%d,ret_session:%d\n", start_time, handle, time, session, ret_session);

	while (true)
	{
		//if (start_time + 1000 < skynet_thread_time())
		{
			skynet_updatetime();
			start_time = skynet_thread_time();
		}
		
		usleep(2500);
	}



     return 0;
}


//--------------------------------------------------------------------------
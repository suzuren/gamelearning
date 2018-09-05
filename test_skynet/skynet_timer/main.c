
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



//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    printf("test skynet timer.\n");

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
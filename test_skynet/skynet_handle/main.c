
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>

#include "rwlock.h"
#include "skynet_handle.h"
//--------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    printf("test skynet handle.\n");

	int handle_index = 1;
	int slot_size = 4;
	int handle_count = 0;
	uint32_t arr_handle[512] = { 0 };
	for (int i=0; i<slot_size; i++)
	{
		uint32_t handle = (i + handle_index) & HANDLE_MASK;
		int hash = handle & (slot_size - 1);
		handle_index = handle + 1;

		arr_handle[handle_count++] = handle;
		printf("test handle sa -slot_size:%d, handle:%02d,handle_index:%02d,hash:%d\n",
			slot_size, handle, handle_index, hash);
	}

	slot_size *= 2;
	for (int i=0; i<handle_count; i++)
	{
		uint32_t handle = arr_handle[i];
		uint32_t hash = handle & (slot_size - 1);

		printf("test handle ar -slot_size:%d, handle:%02d,handle_index:%02d,hash:%d\n",
			slot_size, handle, handle_index, hash);

	}



	// ‭0011‬

	int harbor = 1;
	skynet_handle_init(harbor);


	int profile = 1;
	void *inst = NULL;
	struct skynet_module * mod = NULL;
	struct skynet_context * ctx = skynet_malloc(sizeof(*ctx));
	//CHECKCALLING_INIT(ctx)

	ctx->mod = mod;
	ctx->instance = inst;
	ctx->ref = 2;
	ctx->cb = NULL;
	ctx->cb_ud = NULL;
	ctx->session_id = 0;
	ctx->logfile = NULL;

	ctx->init = false;
	ctx->endless = false;

	ctx->cpu_cost = 0;
	ctx->cpu_start = 0;
	ctx->message_count = 0;
	ctx->profile = profile;
	// Should set to 0 first to avoid skynet_handle_retireall get an uninitialized handle
	ctx->handle = 0;
	ctx->handle = skynet_handle_register(ctx);

	printf("skynet_handle_register - handle:%d\n", ctx->handle);

	uint32_t handle = skynet_handle_findname("test_service_name");
	printf("skynet_handle_findname - handle:%d\n", handle);

	const char * pname = skynet_handle_namehandle(ctx->handle, "test_service_name");
	printf("skynet_handle_namehandle - pname:%s\n", pname);

	handle = skynet_handle_findname("test_service_name");
	printf("skynet_handle_findname - handle:%d\n", handle);

	int ret = skynet_handle_retire(handle);
	printf("skynet_handle_retire - ret:%d\n", ret);

	skynet_handle_retireall();

     return 0;
}


//--------------------------------------------------------------------------
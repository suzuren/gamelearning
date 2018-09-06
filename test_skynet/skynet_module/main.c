
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "skynet.h"

#include "skynet_module.h"

//--------------------------------------------------------------------------
#define skynet_malloc malloc
#define skynet_free free

void
skynet_error(struct skynet_context * context, const char *msg, ...) {
	//printf("skynet_error - context:%p,msg:%s\n", context, msg);
	char tmp[2048] = { 0 };
	va_list ap;

	va_start(ap, msg);
	int len = vsnprintf(tmp, 2048, msg, ap);
	va_end(ap);
	printf("skynet_error - context:%p,len:%d,msg:%s\n", context, len, tmp);
}
//--------------------------------------------------------------------------

void test_func()
{
	void *  par = (void *)(intptr_t)(~0);
	printf("test func - par:%p\n", par);
	// 0xffffffffffffffff
}

int main(int argc, char *argv[])
{
	test_func();

    printf("test skynet module.\n");

	skynet_module_init("./?.so");
	
	const char * name = "logger";
	const char *param = NULL;

	struct skynet_module * mod = skynet_module_query(name);
	if (mod == NULL)
	{
		printf("skynet_module_query - mod:%p\n",mod);
		return 0;
	}
	void *inst = skynet_module_instance_create(mod);
	if (inst == NULL)
	{
		printf("skynet_module_instance_create - inst:%p\n",inst);
		return 0;
	}

	struct skynet_context * ctx = skynet_malloc(sizeof(*ctx));

	// logger_init(struct logger * inst, struct skynet_context *ctx, const char * parm)
	// return 0 success
	// return 1 failed
	int r = skynet_module_instance_init(mod, inst, ctx, param);
	if (r == 0)
	{
		struct skynet_context * ret = ctx;// skynet_context_release(ctx);
		if (ret)
		{
			ctx->init = true;
		}
		//skynet_globalmq_push(queue);
		if (ret)
		{
			skynet_error(ret, "LAUNCH %s %s", name, param ? param : "");
		}
		//return 1;
	}
	else {
		skynet_error(ctx, "FAILED launch %s", name);
		//uint32_t handle = ctx->handle;
		//skynet_context_release(ctx);
		//skynet_handle_retire(handle);
		//struct drop_t d = { handle };
		//skynet_mq_release(queue, drop_message, &d);
		return 0;
	}

	skynet_module_instance_release(mod, inst);

     return 1;
}

//--------------------------------------------------------------------------
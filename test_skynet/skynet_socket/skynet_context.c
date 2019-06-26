
#include "skynet_malloc.h"
#include "skynet_timer.h"
#include "skynet_mq.h"
#include "skynet_context.h"

#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdint.h>

#ifdef CALLING_CHECK

#define CHECKCALLING_BEGIN(ctx) if (!(spinlock_trylock(&ctx->calling))) { assert(0); }
#define CHECKCALLING_END(ctx) spinlock_unlock(&ctx->calling);
#define CHECKCALLING_INIT(ctx) spinlock_init(&ctx->calling);
#define CHECKCALLING_DESTROY(ctx) spinlock_destroy(&ctx->calling);
#define CHECKCALLING_DECL struct spinlock calling;

#else

#define CHECKCALLING_BEGIN(ctx)
#define CHECKCALLING_END(ctx)
#define CHECKCALLING_INIT(ctx)
#define CHECKCALLING_DESTROY(ctx)
#define CHECKCALLING_DECL

#endif

int
skynet_callback(uint32_t handle, int type, int session, uint32_t source, const void * msg, size_t sz) {
	if (handle == HANDLE_TIME)
	{
		printf("callback timer  - handle:%d,type:%d,session:%d,source:%d,msg:%s,sz:%ld\n", handle, type, session, source, (char *)msg, sz);
	}
	else if (handle == HANDLE_SOCKET)
	{
		printf("callback socker - handle:%d,type:%d,session:%d,source:%d,msg:%s,sz:%ld\n", handle, type, session, source, (char *)msg, sz);
	}
	else
	{
		printf("callback unknow - handle:%d,type:%d,session:%d,source:%d,msg:%s,sz:%ld\n", handle, type, session, source, (char *)msg, sz);
	}
	return 0;
}

void skynet_context_init()
{
	skynet_mq_init();
	skynet_timer_init();
	struct message_queue * queue_time = skynet_mq_create(HANDLE_TIME);
	skynet_globalmq_push(queue_time);
	struct message_queue * queue_socket = skynet_mq_create(HANDLE_SOCKET);
	skynet_globalmq_push(queue_socket);
}


int skynet_context_push(uint32_t handle, struct skynet_message *message) {
	struct message_queue * queue = skynet_globalmq_find(handle);
	//printf("push - handle:%d,queue:%p\n", handle,queue);
	if (queue == NULL) {
		return -1;
	}
	skynet_mq_push(queue, message);

	return 0;
}

static void
dispatch_message(uint32_t handle, struct skynet_message *msg) {
	//assert(ctx->init);
	//CHECKCALLING_BEGIN(ctx)
	//pthread_setspecific(G_NODE.handle_key, (void *)(uintptr_t)(handle));
	int type = msg->sz >> MESSAGE_TYPE_SHIFT;
	size_t sz = msg->sz & MESSAGE_TYPE_MASK;
	//if (ctx->logfile) {
	//	skynet_log_output(ctx->logfile, msg->source, type, msg->session, msg->data, sz);
	//}
	//++ctx->message_count;
	int reserve_msg;
	uint64_t cpu_start;	// in microsec
	uint64_t cpu_cost = 0;	// in microsec
	if (1) {
		cpu_start = skynet_thread_time();
		reserve_msg = skynet_callback(handle, type, msg->session, msg->source, msg->data, sz);
		uint64_t cost_time = skynet_thread_time() - cpu_start;
		cpu_cost += cost_time;
		
		char result_cpu_start[32] = { 0 };
		double t_cpu_start = (double)cpu_start / 1000000.0;
		sprintf(result_cpu_start, "%lf", t_cpu_start);

		char result_cpu_cost[32] = { 0 };
		double t_cpu_cost = (double)cpu_cost / 1000000.0;
		sprintf(result_cpu_cost, "%lf", t_cpu_cost);

		printf("cpu_start:%s,cpu_cost:%s\n", result_cpu_start, result_cpu_cost);
	}
	else {
		reserve_msg = skynet_callback(handle, type, msg->session, msg->source, msg->data, sz);
	}
	if (!reserve_msg) {
		skynet_free(msg->data);
	}
	//CHECKCALLING_END(ctx)
}


struct message_queue *
skynet_context_message_dispatch(struct message_queue *q) {
	if (q == NULL) {
		q = skynet_globalmq_pop();
		if (q == NULL)
			return NULL;
	}

	uint32_t handle = skynet_mq_handle(q);

	//printf("disp - handle:%d,queue:%p\n", handle, q);

	int i, n = 1;
	struct skynet_message msg;

	for (i = 0; i<n; i++) {
		if (skynet_mq_pop(q, &msg)) {
			return skynet_globalmq_pop();
		}

		int overload = skynet_mq_overload(q);
		if (overload) {
			fprintf(stderr, "May overload, message queue length = %d", overload);
		}

		if (handle == 0) {
			skynet_free(msg.data);
		}
		else {
			dispatch_message(handle, &msg);
		}
	}

	struct message_queue *nq = skynet_globalmq_pop();
	if (nq) {
		// If global mq is not empty , push q back, and return next queue (nq)
		// Else (global mq is empty or block, don't push q back, and return q again (for next dispatch)
		skynet_globalmq_push(q);
		q = nq;
	}

	return q;
}



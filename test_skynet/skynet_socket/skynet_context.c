#include "socket_server.h"
#include "skynet_malloc.h"
#include "skynet_timer.h"
#include "skynet_mq.h"
#include "skynet_context.h"
#include "skynet_socket.h"

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

static 	int udp_server_index = 0;

static 	int tcp_server_index = 0;

int
skynet_callback(uint32_t handle, int type, int session, uint32_t source, const void * msg, size_t sz) {
	
	printf("------------------------------------\n");

	if (handle == HANDLE_TIME)
	{
		printf("callback timer  - handle:%d,type:%d,session:%d,source:%d,msg:%s,sz:%ld\n", handle, type, session, source, (char *)msg, sz);
	}
	else if (handle == HANDLE_SOCKET)
	{
		struct skynet_socket_message *sm =(struct skynet_socket_message *)msg;
		printf("callback socker - handle:%d,type:%d,session:%d,source:%d,sz:%ld,buffer:%s,type:%d,id:%d,ud:%d\n",
			handle, type, session, source, sz, sm->buffer, sm->type, sm->id, sm->ud);
	}
	else if (handle == HANDLE_SOCKET_UDP_SERVER || handle == HANDLE_SOCKET_UDP_CLIENT)
	{
		struct skynet_socket_message *sm = (struct skynet_socket_message *)msg;
		printf("callback socker - handle:%d,type:%d,session:%d,source:%d,sz:%ld,buffer:%s,type:%d,id:%d,ud:%d\n",
			handle, type, session, source, sz, sm->buffer, sm->type, sm->id, sm->ud);
		if (handle == HANDLE_SOCKET_UDP_SERVER && udp_server_index == 0)
		{
			int address_size;
			const char * udp_send_address = skynet_socket_udp_address(sm, &address_size);

			char temp_buffer[512] = { 0 };
			sprintf(temp_buffer, "udp_server_helloworld_%03d", udp_server_index++);
			int udp_send_sz = (int)strlen(temp_buffer) + 1;
			char * udp_send_buffer = skynet_malloc(udp_send_sz);
			memcpy(udp_send_buffer, temp_buffer, udp_send_sz);

			int ret_send = skynet_socket_udp_send(HANDLE_SOCKET_UDP_CLIENT, sm->id, udp_send_address, udp_send_buffer, udp_send_sz);

			printf("skynet_socket_udp_send - address_size:%d,ret_send:%d,udp_send_sz:%d\n", address_size, ret_send, udp_send_sz);

		}
		skynet_free(sm->buffer);
		//skynet_free(sm);
	}
	else if (handle == HANDLE_SOCKET_TCP_SERVER)
	{
		struct skynet_socket_message *sm = (struct skynet_socket_message *)msg;
		char static_data[128] = { 0 };
		if (sm->type == SKYNET_SOCKET_TYPE_CONNECT || sm->type == SKYNET_SOCKET_TYPE_ERROR || sm->type == SKYNET_SOCKET_TYPE_ACCEPT)
		{
			memcpy(static_data, msg + sizeof(struct skynet_socket_message), sz - sizeof(struct skynet_socket_message));
		}
		printf("callback socker - handle:%d,type:%d,session:%d,source:%d,sz:%ld,buffer:%s,type:%d,tcp_server_index:%d,id:%d,ud:%d,static_data:%s\n",
			handle, type, session, source, sz, sm->buffer, sm->type, tcp_server_index, sm->id, sm->ud, static_data);
		if (sm->type == SKYNET_SOCKET_TYPE_ACCEPT)
		{
			// 把服务端接受连接的fd加入epoll管理，这样客户端发送数据的时候，这个fd就会能收到数据并且接受
			skynet_socket_start(HANDLE_SOCKET_TCP_SERVER, sm->ud);
		}
		else if (sm->type == SKYNET_SOCKET_TYPE_DATA && tcp_server_index == 0)
		{
			char temp_buffer[512] = { 0 };
			sprintf(temp_buffer, "tcp_server_helloworld_%03d", tcp_server_index++);
			int tcp_send_sz = (int)strlen(temp_buffer) + 1;
			char * tcp_send_buffer = skynet_malloc(tcp_send_sz);
			memcpy(tcp_send_buffer, temp_buffer, tcp_send_sz);
			int ret_send = skynet_socket_send(HANDLE_SOCKET_TCP_SERVER, sm->id, tcp_send_buffer, tcp_send_sz);

			printf("callback skynet_socket_tcp  - ret_send:%d,tcp_send_sz:%d\n", ret_send, tcp_send_sz);
		}
	}
	else if (handle == HANDLE_SOCKET_TCP_CLIENT)
	{
		char static_data[128] = { 0 };
		struct skynet_socket_message *sm = (struct skynet_socket_message *)msg;
		if (sm->type == SKYNET_SOCKET_TYPE_CONNECT || sm->type == SKYNET_SOCKET_TYPE_ERROR || sm->type == SKYNET_SOCKET_TYPE_ACCEPT)
		{
			memcpy(static_data, msg + sizeof(struct skynet_socket_message), sz - sizeof(struct skynet_socket_message));
		}
		printf("callback socker - handle:%d,type:%d,session:%d,source:%d,sz:%ld,buffer:%s,type:%d,id:%d,ud:%d,static_data:%s\n",
			handle, type, session, source, sz, sm->buffer, sm->type, sm->id, sm->ud, static_data);

	}
	else
	{
		printf("callback unknow - handle:%d,type:%d,session:%d,source:%d,msg:%s,sz:%ld\n", handle, type, session, source, (char *)msg, sz);
	}
	return 0;
}

struct skynet_context {
	struct message_queue *queue;
	uint64_t cpu_cost;	// in microsec
	uint64_t cpu_start;	// in microsec
	uint32_t handle;
};

static struct skynet_context ctx_time;
static struct skynet_context ctx_sock;
static struct skynet_context ctx_sock_udp_s;
static struct skynet_context ctx_sock_udp_c;
static struct skynet_context ctx_sock_tcp_s;
static struct skynet_context ctx_sock_tcp_c;

void skynet_context_init()
{
	skynet_mq_init();
	skynet_timer_init();

	ctx_time.handle = HANDLE_TIME;
	ctx_time.queue = skynet_mq_create(ctx_time.handle);
	skynet_globalmq_push(ctx_time.queue);

	ctx_sock.handle = HANDLE_SOCKET;
	ctx_sock.queue = skynet_mq_create(ctx_sock.handle);
	skynet_globalmq_push(ctx_sock.queue);

	ctx_sock_udp_s.handle = HANDLE_SOCKET_UDP_SERVER;
	ctx_sock_udp_s.queue = skynet_mq_create(ctx_sock_udp_s.handle);
	skynet_globalmq_push(ctx_sock_udp_s.queue);

	ctx_sock_udp_c.handle = HANDLE_SOCKET_UDP_CLIENT;
	ctx_sock_udp_c.queue = skynet_mq_create(ctx_sock_udp_c.handle);
	skynet_globalmq_push(ctx_sock_udp_c.queue);

	ctx_sock_tcp_s.handle = HANDLE_SOCKET_TCP_SERVER;
	ctx_sock_tcp_s.queue = skynet_mq_create(ctx_sock_tcp_s.handle);
	skynet_globalmq_push(ctx_sock_tcp_s.queue);

	ctx_sock_tcp_c.handle = HANDLE_SOCKET_TCP_CLIENT;
	ctx_sock_tcp_c.queue = skynet_mq_create(ctx_sock_tcp_c.handle);
	skynet_globalmq_push(ctx_sock_tcp_c.queue);

	//printf("init - handle:%d,queue:%p\n", ctx_time.handle, ctx_time.queue);
	//printf("init - handle:%d,queue:%p\n", ctx_sock.handle, ctx_sock.queue);
}


int skynet_context_push(uint32_t handle, struct skynet_message *message) {
	//struct message_queue * queue = skynet_globalmq_find(handle);
	//printf("push - handle:%d,queue:%p\n", handle,queue);
	//if (queue == NULL) {
	//	return -1;
	//}
	if (handle == HANDLE_TIME)
	{
		skynet_mq_push(ctx_time.queue, message);
	}
	else if (handle == HANDLE_SOCKET)
	{
		skynet_mq_push(ctx_sock.queue, message);
	}
	else if (handle == HANDLE_SOCKET_UDP_SERVER)
	{
		skynet_mq_push(ctx_sock_udp_s.queue, message);
	}
	else if (handle == HANDLE_SOCKET_UDP_CLIENT)
	{
		skynet_mq_push(ctx_sock_udp_c.queue, message);
	}
	else if (handle == HANDLE_SOCKET_TCP_SERVER)
	{
		skynet_mq_push(ctx_sock_tcp_s.queue, message);
	}
	else if (handle == HANDLE_SOCKET_TCP_CLIENT)
	{
		skynet_mq_push(ctx_sock_tcp_c.queue, message);
	}
	else
	{
		return -1;
	}
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

		//printf("cpu_start:%s,cpu_cost:%s\n", result_cpu_start, result_cpu_cost);
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

	//printf("disp 2 - handle:%d,queue:%p\n", handle, q);

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



#ifndef _EXAMPLE_TEST_H
#define _EXAMPLE_TEST_H

#include "skynet_malloc.h"
#include "skynet_mq.h"
#include "socket_server.h"
#include "skynet_context.h"
#include "skynet_timer.h"
#include "skynet_socket.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>


#define PTYPE_TEXT 0
#define PTYPE_RESPONSE 1
#define PTYPE_MULTICAST 2
#define PTYPE_CLIENT 3
#define PTYPE_SYSTEM 4
#define PTYPE_HARBOR 5
#define PTYPE_SOCKET 6
// read lualib/skynet.lua examples/simplemonitor.lua
#define PTYPE_ERROR 7	
// read lualib/skynet.lua lualib/mqueue.lua lualib/snax.lua
#define PTYPE_RESERVED_QUEUE 8
#define PTYPE_RESERVED_DEBUG 9
#define PTYPE_RESERVED_LUA 10
#define PTYPE_RESERVED_SNAX 11

#define PTYPE_TAG_DONTCOPY 0x10000
#define PTYPE_TAG_ALLOCSESSION 0x20000


#include <stdio.h>
#include <string.h>
#include <assert.h>

struct drop_t {
	uint32_t handle;
};

static void
drop_message(struct skynet_message *msg, void *ud) {
	struct drop_t *d = ud;
	//skynet_free(msg->data);
	uint32_t source = d->handle;
	printf("drop_message - source:%d,session:%d\n", source, msg->session);
	assert(source);
	// report error to the message source
	//skynet_send(NULL, source, msg->source, PTYPE_ERROR, 0, NULL, 0);
}

void test_skynet_mq()
{
	printf("test_skynet_mq.\n");

	// ��ʼ��ȫ����Ϣ����
	skynet_mq_init();

	uint32_t handle = 1;
	// ����������Ϣ����
	struct message_queue * q1 = skynet_mq_create(handle);

	// ������Ϣ
	struct skynet_message msg;
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_1";
	size_t sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// ����Ϣ�Ž�������Ϣ����
	skynet_mq_push(q1, &msg);

	// ������Ϣ
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_2";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// ����Ϣ�Ž�������Ϣ����
	skynet_mq_push(q1, &msg);
	// �Ѷ�����Ϣ���з���ȫ����Ϣ����
	skynet_globalmq_push(q1);

	// ---------------------------------------------------------------------------------

	handle = 2;
	// ����������Ϣ����
	struct message_queue * q2 = skynet_mq_create(handle);

	// ʹ�������й��� �������
	//for (int i = 0; i < 1024 + 2; i++)
	{
		msg.source = handle;
		msg.session = handle;
		msg.data = "queue_2_msg";
		sz = strlen(msg.data);
		msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
		skynet_mq_push(q2, &msg);
	}

	// �Ѷ�����Ϣ���з���ȫ����Ϣ����
	skynet_globalmq_push(q2);

	handle = 3;
	struct message_queue * q3 = skynet_mq_create(handle);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_3_msg_1";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q3, &msg);

	skynet_globalmq_push(q3);

	handle = 4;
	struct message_queue * q4 = skynet_mq_create(handle);
	skynet_globalmq_push(q4);

	handle = 5;
	struct message_queue * q5 = skynet_mq_create(handle);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_5_msg_1";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q5, &msg);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_5_msg_2";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q5, &msg);


	skynet_globalmq_push(q5);


	// ���ж������ж���Ҫ����Ϣ
	// ���û����Ϣ ��ִ�� skynet_mq_pop(q, &msg); ȡ����Ϣ��ʱ�� ��ȡ��Ϣʧ�ܣ�
	// ���ʱ�� in_global ��־���ƿգ�������Ϣ����ȫ�ֶ������棬
	// �� skynet_mq_mark_release(q); ����ͷŶ��е�ʱ�򣬻��ж� in_global ��־�Ƿ���ȫ�ֶ�������
	// �������ȫ�ֶ�������ִ�� skynet_globalmq_push(q);�����Ż�ȫ�ֶ������棬
	// ��������������� skynet_mq_release(q, drop_message, &d); �Ѿ��� q �ͷŵ��ˣ�
	// ���ʱ��Ž�ȫ�ֶ�������Ķ����������Ѿ����ͷŹ��ģ������ٴ�ѭ��������һ����������q��ʱ��
	// ����������� q,�Ѿ����ͷŹ��ˣ������ٴ�ִ�� skynet_mq_mark_release(q); ��ʱ����������
	// assert(q->release == 0); �����ʧ��,���³������.
	// ����
	// ������Ϣʧ��֮�󣬲�Ҫ���skynet_mq_mark_release(q); 
	// Ҫ�� ִ�� skynet_mq_push() �����з�һ����Ϣ�������� in_global �����Ϊȫ�ֶ���״̬������ִ�к��� skynet_globalmq_push �Ż�ȫ�ֶ������棬
	// �ٽ��б���ͷ�

	// handle Ҳ����Ϊ 0 ��Ȼ���ͷŵ�ʱ�� �� drop_message ���������ʧ��,���³������

	for (;;)
	{
		struct message_queue * q = skynet_globalmq_pop();
		printf("skynet_globalmq_pop - q:%p\n", q);

		if (!q)break;

		uint32_t handle = skynet_mq_handle(q);
		int length = skynet_mq_length(q);

		struct skynet_message msg;
		int ret = skynet_mq_pop(q, &msg);
		//skynet_mq_push(q, &msg);

		printf("handle:%d,length:%d,ret:%d\n", handle, length, ret);
		int type = msg.sz >> MESSAGE_TYPE_SHIFT;
		size_t sz = msg.sz & MESSAGE_TYPE_MASK;
		char * chbuf = msg.data;
		if (ret)
		{
			// ������Ϣʧ��
			msg.source = 5;
			msg.session = 5;
			msg.data = "queue_5_msg_1";
			sz = strlen(msg.data);
			msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
			skynet_mq_push(q, &msg);
		}
		else
		{
			printf("source:%d,session:%d,type:%d,sz:%ld,data:%s\n", msg.source, msg.session, type, sz, chbuf);

			int overload = skynet_mq_overload(q);
			if (overload)
			{
				//printf("May overload, message queue length = %d\n", overload);
			}
			skynet_mq_mark_release(q);
			struct drop_t d = { handle };
			skynet_mq_release(q, drop_message, &d);
		}
	}	
}

void test_skynet_timer()
{
	printf("test_skynet_timer.\n");

	skynet_context_init();
	skynet_timeout(HANDLE_TIME, 0, 33);
	skynet_timeout(HANDLE_TIME, 0, 37);
	struct message_queue * q = NULL;
	for (;;)
	{
		q = skynet_context_message_dispatch(q);
		if (q == NULL)
		{
			break;
		}
	}

	for (;;)
	{
		struct message_queue * q = skynet_globalmq_pop();
		if (!q)break;
		skynet_mq_mark_release(q);
		uint32_t handle = skynet_mq_handle(q);
		struct drop_t d = { handle };
		skynet_mq_release(q, drop_message, &d);
	}
}


static volatile int SIG = 0;

static void
handle_hup(int signal) {
	if (signal == SIGHUP) {
		SIG = 1;
	}
}

static void
signal_hup() {
	// make log file reopen

	//struct skynet_message smsg;
	//smsg.source = 0;
	//smsg.session = 0;
	//smsg.data = NULL;
	//smsg.sz = (size_t)PTYPE_SYSTEM << MESSAGE_TYPE_SHIFT;
	//uint32_t logger = skynet_handle_findname("logger");
	//if (logger) {
	//	skynet_context_push(logger, &smsg);
	//}
}

struct monitor {
	int count;
	//struct skynet_monitor ** m;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int sleep;
	int quit;
};

static void
create_thread(pthread_t *thread, void *(*start_routine) (void *), void *arg) {
	if (pthread_create(thread, NULL, start_routine, arg)) {
		fprintf(stderr, "Create thread failed");
		exit(1);
	}
}

int
skynet_context_total() {
	return 1;
}

#define CHECK_ABORT if (skynet_context_total()==0) break;


static void *
thread_socket(void *p) {
	//struct monitor * m = p;
	//skynet_initthread(THREAD_SOCKET);
	printf("test thread_socket.\n");
	for (;;) {
		int r = skynet_socket_poll();
		//if (r == 0)
		//	break;
		if (r<0) {
			//CHECK_ABORT
			continue;
		}
		//wakeup(m, 0);
	}
	printf("test thread_socket exit.\n");

	return NULL;
}

static void *
thread_timer(void *p) {
	struct monitor * m = p;
	//skynet_initthread(THREAD_TIMER);
	for (;;) {
		skynet_updatetime();
		skynet_socket_updatetime();
		CHECK_ABORT
		//wakeup(m, m->count - 1);
		usleep(2500);
		if (SIG) {
			signal_hup();
			SIG = 0;
		}
	}
	// wakeup socket thread
	skynet_socket_exit();
	// wakeup all worker thread
	pthread_mutex_lock(&m->mutex);
	m->quit = 1;
	pthread_cond_broadcast(&m->cond);
	pthread_mutex_unlock(&m->mutex);
	return NULL;
}


static void *
thread_worker(void *p) {
	struct monitor *m = p;

	//struct worker_parm *wp = p;
	//int id = wp->id;
	//int weight = wp->weight;
	//struct monitor *m = wp->m;
	//struct skynet_monitor *sm = m->m[id];
	//skynet_initthread(THREAD_WORKER);
	struct message_queue * q = NULL;
	while (!m->quit) {
		q = skynet_context_message_dispatch(q);
		usleep(2500);
		if (q == NULL) {
			//if (pthread_mutex_lock(&m->mutex) == 0) {
			//	++m->sleep;
			//	// "spurious wakeup" is harmless,
			//	// because skynet_context_message_dispatch() can be call at any time.
			//	if (!m->quit)
			//		pthread_cond_wait(&m->cond, &m->mutex);
			//	--m->sleep;
			//	if (pthread_mutex_unlock(&m->mutex)) {
			//		fprintf(stderr, "unlock mutex error");
			//		exit(1);
			//	}
			//}
		}
	}
	return NULL;
}


void test_skynet_socket()
{
	printf("test_skynet_socket.\n");

	struct sigaction sa;
	sa.sa_handler = &handle_hup;
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);
	sigaction(SIGHUP, &sa, NULL);


	skynet_context_init();
	skynet_socket_init();

	int thread = 3;
	struct monitor *m = skynet_malloc(sizeof(*m));
	memset(m, 0, sizeof(*m));
	m->count = thread;
	m->sleep = 0;

	pthread_t pid[thread];

	create_thread(&pid[0], thread_worker, m);
	create_thread(&pid[1], thread_timer, m);
	create_thread(&pid[2], thread_socket, m);

	//int skynet_socket_listen(uint32_t handle, const char *host, int port, int backlog);

	int id_udp_server = skynet_socket_udp(HANDLE_SOCKET_UDP_SERVER, "0.0.0.0", 8900);

	int ret_connect = skynet_socket_udp_connect(HANDLE_SOCKET_UDP_CLIENT, id_udp_server, "127.0.0.1", 8900);

	printf("skynet_socket_udp - id_udp_server:%d,ret_connect:%d\n", id_udp_server, ret_connect);

	//struct skynet_socket_message sm;
	//sm.type = 1;
	//sm.type = id_udp_server;
	//sm.ud = 8900;
	//sm.buffer = "127.0.0.1";
	//int address_size;
	//const char * udp_send_address = skynet_socket_udp_address(&sm, &address_size);

	int client_index = 0;
	for (;;)
	{
		char temp_buffer[512] = { 0 };
		sprintf(temp_buffer, "client_helloworld_%03d", client_index++);
		//char * temp_buffer = "client_helloworld\0";
		int udp_send_sz = (int)strlen(temp_buffer) + 1;
		char * udp_send_buffer = skynet_malloc(udp_send_sz);
		memcpy(udp_send_buffer, temp_buffer, udp_send_sz);

		//int ret_send = skynet_socket_udp_send(HANDLE_SOCKET, id_udp_server, udp_send_address, udp_send_buffer, udp_send_sz);
		int ret_send = skynet_socket_send(HANDLE_SOCKET_UDP_CLIENT, id_udp_server, udp_send_buffer, udp_send_sz);

		printf("skynet_socket_udp id_udp - ret_send:%d,udp_send_sz:%d\n", ret_send, udp_send_sz);

		// wait for 2 seconds
		usleep(2000000);
		break;
	}

	int listen_id = skynet_socket_listen(HANDLE_SOCKET_TCP_SERVER, "0.0.0.0", 8901, 128);
	skynet_socket_start(HANDLE_SOCKET_TCP_SERVER, listen_id);
	// ��һ���ⲿ�ľ�� �󶨵��ײ�� epoll ��
	int bind_id = 1;// skynet_socket_bind(HANDLE_SOCKET_TCP_SERVER, listen_id);

	int connect_id = skynet_socket_connect(HANDLE_SOCKET_TCP_CLIENT, "0.0.0.0", 8901);

	printf("listen_id:%d, binding stdin bind_id:%d,connect_id:%d\n", listen_id, bind_id, connect_id);

	int client_index_tcp = 0;
	for (;;)
	{
		char temp_buffer[512] = { 0 };
		sprintf(temp_buffer, "tcp_client_helloworld_%03d", client_index_tcp++);
		int tcp_send_sz = (int)strlen(temp_buffer) + 1;
		char * tcp_send_buffer = skynet_malloc(tcp_send_sz);
		memcpy(tcp_send_buffer, temp_buffer, tcp_send_sz);
		int ret_send = 1;// skynet_socket_send(HANDLE_SOCKET_TCP_CLIENT, connect_id, tcp_send_buffer, tcp_send_sz);

		printf("skynet_socket_tcp  - ret_send:%d,tcp_send_sz:%d\n", ret_send, tcp_send_sz);

		// wait for 2 seconds
		usleep(2000000);
		break;
	}


	for (;;)
	{
		usleep(2000000);
	}

	for (int i = 0; i < thread; i++) {
		pthread_join(pid[i], NULL);
	}
	skynet_socket_free();
}




#endif


#include <stdio.h>

#include <memory.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>


#define MAX_INFO 128
// MAX_SOCKET will be 2^MAX_SOCKET_P
#define MAX_SOCKET_P 16
#define MAX_EVENT 64
#define MIN_READ_BUFFER 64
#define SOCKET_TYPE_INVALID 0
#define SOCKET_TYPE_RESERVE 1
#define SOCKET_TYPE_PLISTEN 2
#define SOCKET_TYPE_LISTEN 3
#define SOCKET_TYPE_CONNECTING 4
#define SOCKET_TYPE_CONNECTED 5
#define SOCKET_TYPE_HALFCLOSE 6
#define SOCKET_TYPE_PACCEPT 7
#define SOCKET_TYPE_BIND 8

#define MAX_SOCKET (1<<MAX_SOCKET_P)   // 2^16 = 65535

#define PRIORITY_HIGH 0
#define PRIORITY_LOW 1

#define HASH_ID(id) (((unsigned)id) % MAX_SOCKET)

#define PROTOCOL_TCP 0
#define PROTOCOL_UDP 1
#define PROTOCOL_UDPv6 2

#define UDP_ADDRESS_SIZE 19	// ipv6 128bit + port 16bit + 1 byte type

#define MAX_UDP_PACKAGE 65535


typedef int poll_fd;


struct write_buffer {
	struct write_buffer * next;
	void *buffer;
	char *ptr;
	int sz;
	bool userobject;
	uint8_t udp_address[UDP_ADDRESS_SIZE];
};

// C 库宏 offsetof(type, member - designator) 会生成一个类型为 size_t 的整型常量，它是一个结构成员相对于结构开头的字节偏移量。
// 成员是由 member - designator 给定的，结构的名称是在 type 中给定的

#define SIZEOF_TCPBUFFER (offsetof(struct write_buffer, udp_address[0]))
#define SIZEOF_UDPBUFFER (sizeof(struct write_buffer))


struct wb_list {
	struct write_buffer * head;
	struct write_buffer * tail;
};


struct socket {
	uintptr_t opaque;
	struct wb_list high;
	struct wb_list low;
	int64_t wb_size;
	int fd;
	int id;
	uint16_t protocol;
	uint16_t type;
	union {
		int size;
		uint8_t udp_address[UDP_ADDRESS_SIZE];
	} p;
};





struct socket_object_interface {
	void * (*buffer)(void *);
	int(*size)(void *);
	void(*free)(void *);
};


struct event {
	void * s;
	bool read;
	bool write;
};

struct socket_server {
	int recvctrl_fd;
	int sendctrl_fd;
	int checkctrl;
	poll_fd event_fd;
	int alloc_id;
	int event_n;
	int event_index;
	struct socket_object_interface soi;
	struct event ev[MAX_EVENT];
	struct socket slot[MAX_SOCKET];
	char buffer[MAX_INFO];
	uint8_t udpbuffer[MAX_UDP_PACKAGE];
	fd_set rfds;
};



// int [-2^31 , 2^31-1] -> [-2147483648，2147483647]
//       0x7fffffff ->  ‭2147483647‬  ->                                  ‭01111111111111111111111111111111‬
// ‭FFFFFFFF80000000‬ -> -2147483648  ->  ‭1111111111111111111111111111111110000000000000000000000000000000‬

static int reserve_id(struct socket_server *ss)
{
	int i;
	for (i = 0; i<MAX_SOCKET; i++)
	{
		int id = __sync_add_and_fetch(&(ss->alloc_id), 1);
		if (id < 0)
		{
			id = __sync_and_and_fetch(&(ss->alloc_id), 0x7fffffff);
		}
		struct socket *s = &ss->slot[HASH_ID(id)];
		if (s->type == SOCKET_TYPE_INVALID)
		{
			if (__sync_bool_compare_and_swap(&s->type, SOCKET_TYPE_INVALID, SOCKET_TYPE_RESERVE))
			{
				s->id = id;
				s->fd = -1;
				return id;
			}
			else
			{
				// retry
				--i;
			}
		}
	}
	return -1;
}


static inline void
clear_wb_list(struct wb_list *list) {
	list->head = NULL;
	list->tail = NULL;
}

struct socket_server *
	socket_server_create() {


	struct socket_server *ss = (struct socket_server *)malloc(sizeof(*ss));
	ss->event_fd = -1;
	ss->recvctrl_fd = -1;
	ss->sendctrl_fd = -1;
	ss->checkctrl = 1;

	for (int i = 0; i<MAX_SOCKET; i++) {
		struct socket *s = &ss->slot[i];
		s->type = SOCKET_TYPE_INVALID;
		clear_wb_list(&s->high);
		clear_wb_list(&s->low);
	}
	ss->alloc_id = 0;
	ss->alloc_id = 2147483645;
	ss->event_n = 0;
	ss->event_index = 0;
	memset(&ss->soi, 0, sizeof(ss->soi));
	FD_ZERO(&ss->rfds);
	assert(ss->recvctrl_fd < FD_SETSIZE);

	return ss;
}

void test_socket_server()
{
	printf("test_socket_server!\n");
	struct socket_server *ss = socket_server_create();

	int int_test = 2147483647;
	printf("test_socket_server 1 - int_test:%d\n", int_test);
	int_test += 1;
	printf("test_socket_server 2 - int_test:%d\n", int_test);
	int_test &= 0x7fffffff;
	printf("test_socket_server 3 - int_test:%d\n", int_test);


	printf("test_socket_server - SIZEOF_TCPBUFFER:%ld\n", SIZEOF_TCPBUFFER);
	printf("test_socket_server - SIZEOF_UDPBUFFER:%ld\n", SIZEOF_UDPBUFFER);



	//int id = reserve_id(ss);
	//printf("test_socket_server - id:%d\n", id);

	for (int i = 0; i < 10; i++)
	{
		int id = reserve_id(ss);
		struct socket *s = &ss->slot[HASH_ID(id)];
		printf("test_socket_server - reserve_id:%10d,alloc_id:%10d,type:%d,id:%10d,fd:%d\n",
			id,ss->alloc_id,s->type,s->id,s->fd);
	}






}
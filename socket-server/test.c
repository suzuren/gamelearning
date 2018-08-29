#include "socket_server.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>



#include <memory.h>
#include <string.h>

static void *
_poll(void * ud) {
	struct socket_server *ss = ud;
	struct socket_message result;
	char buffer[512];
	for (;;) {
		int type = socket_server_poll(ss, &result, NULL);
		// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
		switch (type) {
		case SOCKET_EXIT:
			return NULL;
		case SOCKET_DATA:
			memset(buffer, 0, sizeof(buffer));
			memcpy(buffer, result.data, result.ud);
			printf("message(%lu) [id=%d] size=%d - %s\n",result.opaque,result.id, result.ud, buffer);
			free(result.data);
			break;
		case SOCKET_CLOSE:
			printf("close(%lu) [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_OPEN:
			printf("open(%lu) [id=%d] %s\n",result.opaque,result.id,result.data);
			break;
		case SOCKET_ERROR:
			printf("error(%lu) [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_ACCEPT:
			printf("accept(%lu) [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);
			socket_server_start(ss, result.opaque, result.ud);
			break;
		}
	}
}

static void
test(struct socket_server *ss) {
	pthread_t pid;
	pthread_create(&pid, NULL, _poll, ss);


	int l = socket_server_listen(ss,200,"127.0.0.1",8888,32);
	printf("listening %d\n",l);
	socket_server_start(ss,201,l);

	int c = socket_server_connect(ss, 100, "127.0.0.1", 8888);
	printf("connecting %d\n", c);
	
	char * buffer = malloc(1024);
	memset(buffer, 0, 1024);
	strcpy(buffer, "hello world tcp!");
	unsigned int sz = strlen(buffer);
	int64_t s = socket_server_send(ss, c, (const void *)buffer, (int)sz);
	printf("send data %ld\n", s);


	//int c = socket_server_connect(ss, 100, "127.0.0.1", 80);
	//printf("connecting %d\n", c);

	//int b = socket_server_bind(ss,300,1);
	//printf("binding stdin %d\n",b);



	//int i;
	//for (i=0;i<3;i++) {
	//	socket_server_connect(ss, 400+i, "127.0.0.1", 8888);
	//}

	// udp
	//int us = socket_server_udp(ss, 300, "0.0.0.0", 8899);
	//printf("socket_server_udp %d\n", us);
	//int uc = socket_server_udp_connect(ss, us, "127.0.0.1", 8899);
	//printf("socket_server_udp_connect %d\n", uc);
	//char * udp_buffer = malloc(1024);
	//memset(udp_buffer, 0, 1024);
	//strcpy(udp_buffer, "hello world udp!");
	//sz = strlen(udp_buffer);
	//int64_t uz = socket_server_udp_send(ss, us,(struct socket_udp_address *)&i, (const void *)udp_buffer, (int)sz);
	//printf("socket_server_udp_send %ld\n", uz);



	sleep(2);

	socket_server_exit(ss);

	pthread_join(pid, NULL); 
}

int
main() {
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);

	struct socket_server * ss = socket_server_create();
	test(ss);
	socket_server_release(ss);

	return 0;
}

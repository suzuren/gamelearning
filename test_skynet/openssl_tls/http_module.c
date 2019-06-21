
#include "socket_server.h"
#include "http_socket.h"
#include "http_module.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

int httpc_request(char * method, char *  host, char *  uri, char * recvheader, char * header, char * content)
{
	char * protocol;
	char * domain_name;
	char * ip;
	int port;
	int check_ret = http_check_protocol(host, &protocol, &domain_name, &ip, &port);
	printf("protocol:%s, domain_name:%s, ip:%s, port:%d, check:%d\n", protocol, domain_name, ip, port, check_ret);
	if (check_ret == 0)
	{
		return 0;
	}
	int c = http_socket_connect(ip, port);
	int i = http_init_interface(protocol, c);
	printf("c:%d,i:%d\n", c, i);
	
	return 1;
}


static void *
http_socket_thread(void *p) {
	for (;;)
	{
		int r = http_socket_poll();
		if (r == 0)
		{
			break;
		}
		if (r<0)
		{
			continue;
		}
	}
	return NULL;
}

void test_http_module()
{
	//test_socket_server();
	http_socket_init();
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	pthread_t pid;
	pthread_create(&pid, NULL, http_socket_thread, NULL);

	char * method = "GET";
	char *  host = "https://tg.safeabc.cn";
	//char * host = "http://47.94.250.154:38018";
	char *  uri = "";
	char * recvheader = "";
	char * header = "";
	char * content = "";
	httpc_request(method, host, uri, recvheader, header, content);


	sleep(5);
	http_socket_exit();
	pthread_join(pid, NULL);
	http_socket_free();
}


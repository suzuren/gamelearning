
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
	int i = http_init_interface(c,protocol);
	int r = http_socket_request(c,method, domain_name, uri, recvheader, header, content);
	printf("c:%d,i:%d,r:%d\n", c, i, r);
	
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
	char *  uri = "/admin/ApiTest/getPhone";
	char recvheader[4096] = { 0 };
	char * header = "";
	char * content = "";
	httpc_request(method, host, uri, recvheader, header, content);
	sleep(3);
	http_socket_exit();
	pthread_join(pid, NULL);
	http_socket_free();
}

/*
[root@forest openssl_tls]# ./testHttpModule
hello world.
protocol:https, domain_name:tg.safeabc.cn, ip:61.132.229.14, port:443, check:1
open(100) [id=1] 61.132.229.14
message(100) [id=1] size=64
message(100) [id=1] size=128
message(100) [id=1] size=256
message(100) [id=1] size=512
message(100) [id=1] size=1024
message(100) [id=1] size=1077
message(100) [id=1] size=242
tls_context_finished
header_size:78
request_header:GET /admin/ApiTest/getPhone HTTP/1.1
host:tg.safeabc.cn
content-length:0

-
message(100) [id=1] size=290
read_size:261
recvbuffer:HTTP/1.1 200 OK
Server: nginx
Date: Mon, 24 Jun 2019 09:05:16 GMT
Content-Type: application/json; charset=utf-8
Transfer-Encoding: chunked
Connection: keep-alive
X-Powered-By: PHP/7.2.6

37
{"phone":"15846784489","id":"5c75e2859dc6d623724bec0f"}
0

-
c:1,i:1,r:1
exit(0) [id=0] size=0
[root@forest openssl_tls]#

*/

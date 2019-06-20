
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

#include "kmp_algorithm.h"
#include "socket_server.h"
#include "http_module.h"

static void *
_socket_server_poll(void * ud) {
	struct socket_server *ss = ud;
	struct socket_message result;
	for (;;) {
		int type = socket_server_poll(ss, &result, NULL);
		// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
		switch (type)
		{
		case SOCKET_EXIT:
			{
				printf("exit(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
				return NULL;
			}break;
		case SOCKET_DATA:
			{
				printf("message(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
				free(result.data);
			}break;
		case SOCKET_CLOSE:
			{
				printf("close(%lu) [id=%d]\n", result.opaque, result.id);
			}break;
		case SOCKET_OPEN:
			{
				printf("open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
			}break;
		case SOCKET_ERR:
			{
				printf("error(%lu) [id=%d]\n", result.opaque, result.id);
			}break;
		case SOCKET_ACCEPT:
			{
				printf("accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);
			}break;
		default:
			{
				printf("Unknown socket message type %d.", type);
			}break;
		}
	}
}

static uint64_t
test_systime() {
	struct timespec ti;
	clock_gettime(CLOCK_REALTIME, &ti);
	//uint64_t sec = (uint64_t)ti.tv_sec;
	uint64_t cs = (uint64_t)(ti.tv_nsec / 10000000);
	return cs;
}

void test_socket_server()
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	struct socket_server * ss = socket_server_create(test_systime());
	pthread_t pid;
	pthread_create(&pid, NULL, _socket_server_poll, ss);
	int c = socket_server_connect(ss, 100, "127.0.0.1", 80);
	printf("connecting %d\n", c);
	int l = socket_server_listen(ss, 200, "127.0.0.1", 8888, 32);
	printf("listening %d\n", l);
	socket_server_start(ss, 201, l);
	int b = socket_server_bind(ss, 300, 1);
	printf("binding stdin %d\n", b);
	int i;
	for (i = 0; i<3; i++)
	{
		socket_server_connect(ss, 400 + i, "127.0.0.1", 8888);
	}
	sleep(5);
	socket_server_exit(ss);
	pthread_join(pid, NULL);
	socket_server_release(ss);
}


char * socket_hosttoip(const char * phost)
{
	static char buffer[32];
	memset(buffer, 0, sizeof(buffer));
	if (phost == NULL)
	{
		return NULL;
	}
	struct hostent *phostent = gethostbyname(phost);
	if (phostent == NULL)
	{
		return NULL;
	}
	switch (phostent->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
	{
		const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer, sizeof(buffer));
		if (ret_inet != NULL)
		{
			return buffer;
		}
	}break;
	default:
	{
		//
	}
	break;
	}
	return NULL;
}

int check_protocol(char* host,char** protocol,char** domain_name, char** ip,int* port)
{
	*protocol = "";
	*domain_name = "";
	*port = 0;
	char * http_str = "http://";
	int http_des_len = (int)strlen(http_str);
	int http_next[http_des_len];
	int http_nextval[http_des_len];
	kmp_get_next(http_str, http_next);
	kmp_get_nextval(http_str, http_nextval);
	int http_pos = kmp_index(host, http_str, 0, http_nextval);
	if (http_pos != -1)
	{
		*protocol = "http";
		*domain_name = (host + http_pos) + http_des_len;
		*port = 80;
	}
	else
	{
		char * https_str = "https://";
		int https_des_len = (int)strlen(https_str);
		int https_next[https_des_len];
		int https_nextval[https_des_len];
		kmp_get_next(https_str, https_next);
		kmp_get_nextval(https_str, https_nextval);
		int https_pos = kmp_index(host, https_str, 0, https_nextval);
		if (https_pos != -1)
		{
			*protocol = "https";
			*domain_name = (host + https_pos) + https_des_len;
			*port = 443;
		}
	}
	char * pdomain = *domain_name;
	unsigned int domain_len = strlen(pdomain);
	if (domain_len == 0)
	{
		return 0;
	}
	bool is_ip_port = true;
	unsigned int index = 0;
	unsigned int last_colon_index = 0;
	static char buffer_ip[32] = { 0 };
	char buffer_port[32] = { 0 };
	// 47.94.250.154:38018
	for (; index < domain_len; index++)
	{
		char ch = (*(pdomain + index));
		if (isdigit(ch) == 0 && ch != '.' && ch != ':')
		{
			is_ip_port = false;
			break;
		}
		if (ch == ':')
		{
			last_colon_index = index;
		}
	}
	if (is_ip_port && last_colon_index != 0)
	{
		for (unsigned int idx = 0; idx < last_colon_index; idx++)
		{
			char ch = (*(pdomain + idx));
			printf("idx:%d,%c\n", idx, ch);
			buffer_ip[idx] = ch;
		}
		for (unsigned int idx = last_colon_index + 1; idx < domain_len; idx++)
		{
			char ch = (*(pdomain + idx));
			printf("port - idx:%d,%c\n", idx, ch);
			buffer_port[idx] = ch;
		}
		printf("domain_len:%d, colon_index:%d, pdomain:%s, ip:%s, port:%s\n", domain_len, last_colon_index, pdomain, buffer_ip, buffer_port);

		*ip = buffer_ip;

	}
	else
	{
		*ip = socket_hosttoip((const char *)*domain_name);
		if (*ip == NULL)
		{
			return 0;
		}
	}
	return 1;
}

void httpc_request(char * method, char *  host, char *  uri, char * recvheader, char * header, char * content)
{
	char * protocol;
	char * domain_name;
	char * ip;
	int port;
	check_protocol(host, &protocol, &domain_name, &ip, &port);
	printf("protocol:%s-, domain_name:%s-,ip:%s,port:%d\n",protocol, domain_name, ip, port);



	//struct socket_server * ss = socket_server_create(test_systime());
	//int c = socket_server_connect(ss, 100, "127.0.0.1", 80);

}

void test_http_module()
{
	//test_socket_server();
	char * method = "GET";
	//char *  host = "https://tg.safeabc.cn";
	char * host = "http://47.94.250.154:38018";
	char *  uri = "";
	char * recvheader = "";
	char * header = "";
	char * content = "";
	httpc_request(method, host, uri, recvheader, header, content);

}


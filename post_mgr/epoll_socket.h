
#ifndef __EPOLL_SOCKET_
#define __EPOLL_SOCKET_



#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>  // standard IO
#include <stdlib.h> //standard  libary
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> //unix standard

#include <sys/select.h> // select IO multiplexing model
#include <sys/epoll.h>

#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <sys/un.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <netdb.h>
#include <stdbool.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8793

bool SetSocketEvents(int epfd, int fd, int op)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLOUT;
	epoll_ctl(epfd, op, fd, &ev);
	return true;
}

bool SetSocketNonblock(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return false;
	}
	flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	if (flag < 0)
	{
		return false;
	}
	return true;
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
	
	//printf("official hostname:%s\n", phostent->h_name);	
	//for (char ** paliases = phostent->h_aliases; *paliases != NULL; paliases++)
	//{
	//	printf("aliases:%s\n", *paliases);
	//}
	//printf("AF_INET:%d,AF_INET6:%d,h_addrtype:%d\n", AF_INET, AF_INET6, phostent->h_addrtype);
	switch (phostent->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
	{
		//for (char ** paddr = phostent->h_addr_list; *paddr != NULL; paddr++)
		//{
		//	char buffer_addr[32] = { 0 };
		//	const char * ret_inet = inet_ntop(phostent->h_addrtype, *paddr, buffer_addr, sizeof(buffer_addr));
		//	if (ret_inet != NULL)
		//	{
		//		printf("type:%d,address list:%s\n", phostent->h_addrtype, buffer_addr);
		//	}
		//}
		//char buffer_first_addr[32] = { 0 };
		//const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer_first_addr, sizeof(buffer_first_addr));
		//if (ret_inet != NULL)
		//{
		//	printf("type:%d,first address list:%s\n", phostent->h_addrtype, buffer_first_addr);
		//}
		const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer, sizeof(buffer));		
		if (ret_inet != NULL)
		{
			return buffer;
		}
	}break;
	default:
	{
		printf("unknown address type\n");
	}
	break;
	}
	return NULL;
}

int socket_connect(const char *ip, int port)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1)
	{
		printf("create socket filed.\n");
		return -1;
	}
	SetSocketNonblock(client_fd);
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1 && errno != EINPROGRESS)
	{
		printf("connect server filed\n");
		return -1;
	}
	if (0 == ret)
	{
		printf("connect success\n");
		return client_fd;
	}
	else
	{
		struct epoll_event events[64];
		int epfd = epoll_create(64);
		SetSocketEvents(epfd, client_fd, EPOLL_CTL_ADD);
		while (true)
		{
			int nfds = epoll_wait(epfd, events, 64, -1);
			if (nfds <= 0)
			{
				continue;
			}
			for (int i = 0; i < nfds; ++i)
			{
				if (events[i].events & EPOLLIN)
				{
					//printf("epoll - EPOLLIN errno:%d\n", errno);
				}
				else if (events[i].events & EPOLLET)
				{
					//printf("epoll - EPOLLET errno:%d\n", errno);
				}
				else if (events[i].events & EPOLLERR)
				{
					//printf("epoll - EPOLLERR errno:%d\n", errno);
					close(epfd);
					return -1;
				}
				else if (events[i].events & EPOLLOUT)
				{
					//printf("epoll - EPOLLOUT errno:%d\n", errno);
					close(epfd);
					return client_fd;
				}
				else
				{
					printf("epoll - else errno:%d\n", errno);
				}
			}
		}
	}
	return -1;
}


#endif



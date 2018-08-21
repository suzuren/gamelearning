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
#include<sys/poll.h>

#include <stdbool.h>

#define IPADDRESS "127.0.0.1"
#define PORT 9876

bool SetSocketEvents(int epfd,int fd, int op)
{
	struct epoll_event ev;

	/*设置监听描述符*/
	ev.data.fd = fd;
	/*设置处理事件类型*/
	//ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLOUT;
	ev.events = EPOLLIN | EPOLLERR | EPOLLOUT;
	/*注册事件*/
	epoll_ctl(epfd, op, fd, &ev);

	return true;
}

bool SetSocketNonblock(int fd)
{
	//下面获取套接字的标志
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		printf("fcntl F_GETFL filed.flags:%d\n", flag);
		//错误处理
		return false;
	}
	//下面设置套接字为非阻塞
	flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	if (flag < 0)
	{
		//错误处理
		printf("fcntl F_SETFL filed.flags:%d\n", flag);
		return false;
	}
	return true;
}

int socket_connect(const char *ip, int port)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1)
	{
		printf("create socket filed.\n");
		exit(1);
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
		exit(1);
	}
	if (0 == ret)
	{   //如果connect()返回0则连接已建立 
		//下面恢复套接字阻塞状态 
		//if (fcntl(client_fd, F_SETFL, flags) < 0)
		//{
		//	//错误处理 
		//}

		//下面是连接成功后要执行的代码 
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
					// 115：当链接设置为非阻塞时，目标没有及时应答，返回此错误，socket可以继续使用
					printf("epoll - EPOLLIN errno:%d\n", errno);
				}
				else if (events[i].events & EPOLLET)
				{
					printf("epoll - EPOLLET errno:%d\n", errno);
				}
				else if (events[i].events & EPOLLERR)
				{
					printf("epoll - EPOLLERR errno:%d\n", errno);
					close(epfd);
					return -1;
				}
				else if (events[i].events & EPOLLOUT)
				{
					printf("epoll - EPOLLOUT errno:%d\n", errno);
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



int socket_bind(const char *ip, int port)
{
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1)
	{
		perror("create socket error");
		exit(1);
	}

	SetSocketNonblock(listen_fd);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind socket error");
		exit(1);
	}

	if (listen(listen_fd, 5) == -1)
	{
		perror("listen socket error");
		exit(1);
	}
	printf("listen success\n");
	return listen_fd;
}


const char* getStrTime()
{
	time_t now = time(0);
	struct tm * pTime = localtime(&now);

	static char szDate[32] = { 0 };

	sprintf(szDate, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

	return szDate;
}



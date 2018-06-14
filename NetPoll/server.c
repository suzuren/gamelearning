#include "poll_socket.h"

/*

poll()函数：这个函数是某些Unix系统提供的用于执行与select()函数同等功能的函数，下面是这个函数的声明：
#include <poll.h>
int poll(struct pollfd fds[], nfds_t nfds, int timeout)；
参数说明 :

fds：是一个struct pollfd结构类型的数组，用于存放需要检测其状态的Socket描述符；每当调用这个函数之后，
系统不会清空这个数组，操作起来比较方便；特别是对于socket连接比较多的情况下，在一定程度上可以提高处
理的效率；这一点与select()函数不同，调用select()函数之后，select()函数会清空它所检测的socket描述符
集合，导致每次调用select()之前都必须把socket描述符重新加入到待检测的集合中；因此，select()函数适合
于只检测一个socket描述符的情况，而poll()函数适合于大量socket描述符的情况；


nfds：nfds_t类型的参数，用于标记数组fds中的结构体元素的总数量；

timeout：是poll函数调用阻塞的时间，单位：毫秒；

返回值 :
>0：数组fds中准备好读、写或出错状态的那些socket描述符的总数量；

== 0：数组fds中没有任何socket描述符准备好读、写，或出错；此时poll超时，超时时间是timeout毫秒；换句话
说，如果所检测的socket描述符上没有任何事件发生的话，那么poll()函数会阻塞timeout所指定的毫秒时间长度之
后返回，如果timeout == 0，那么poll() 函数立即返回而不阻塞，如果timeout == INFTIM，那么poll() 函数会一
直阻塞下去，直到所检测的socket描述符上的感兴趣的事件发生是才返回，如果感兴趣的事件永远不发生，那么poll()
就会永远阻塞下去；

- 1： poll函数调用失败，同时会自动设置全局变量errno；

*/

#define MAX_CLIENT_COUNT 65535

static void accpet_client(struct pollfd *clients_fd, int *iClientCount, int listen_fd);
static void recv_client_msg(struct pollfd *clients_fd, int *iClientCount);
static void handle_client_msg(int fd, char *buf);

int main(int argc, char const *argv[])
{

	int listen_fd = socket_bind("0.0.0.0", PORT);

	printf("socket_bind listen_fd:%d\n", listen_fd);

	struct pollfd clients_fd[MAX_CLIENT_COUNT] = { 0 };
	memset(clients_fd, -1, sizeof(clients_fd));

	clients_fd[0].fd = listen_fd;
	clients_fd[0].events = POLLIN;

	int iClientCount = 1;
	while(1)
	{
		int nread = poll(clients_fd, iClientCount, 0);
		if (nread == -1)
		{
			printf("poll error - nread:%d,errno:%d\n", nread, errno);
			exit(1);
		}
		else if (nread == 0)
		{
			continue;
		}
		else
		{
			if (clients_fd[0].revents & POLLIN)
			{
				accpet_client(clients_fd, &iClientCount, clients_fd[0].fd);
				--nread;
				if (nread <= 0)
				{
					continue;
				}
			}

			recv_client_msg(clients_fd, &iClientCount);
			if (nread <= 0)
			{
				continue;
			}
		}
	}
}


static void accpet_client(struct pollfd *clients_fd, int *iClientCount,int listen_fd)
{
	socklen_t client_len = 0;
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));

	int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1)
	{
		printf("accept failed: %s.\n", strerror(errno));
		return;
	}
	else
	{
		int i = 0;
		for (; i < MAX_CLIENT_COUNT; ++i)
		{
			if (clients_fd[i].fd == -1)
			{
				clients_fd[i].fd = client_fd;
				clients_fd[i].events = POLLIN;
				(*iClientCount)++;
				break;
			}
		}
		if (i == MAX_CLIENT_COUNT)
		{
			close(client_fd);
			printf("too much clients\n");
		}
		printf("new client accpeted,client_fd:%d,*iClientCount:%d\n", client_fd, (*iClientCount));
	}
}


static void recv_client_msg(struct pollfd *clients_fd, int *iClientCount)
{
	for (int i = 1; i < MAX_CLIENT_COUNT; ++i)
	{
		if (clients_fd[i].fd == -1)
		{
			continue;
		}
		if ( clients_fd[i].revents & POLLIN)
		{
			char buf[65535] = { 0 };
			int nread = read(clients_fd[i].fd, buf, 65535);
			if (nread <= 0)
			{
				printf("one socket close,client_fd:%d,errno:%d - %s\n", clients_fd[i], errno,strerror(errno));

				close(clients_fd[i].fd);
				--(*iClientCount);
				clients_fd[i].fd = -1;
			}
			else
			{
				handle_client_msg(clients_fd[i].fd, buf);
			}
			if (--nread <= 0)
			{
				continue;
			}
		}
	}
}


static void handle_client_msg(int fd, char *buf)
{
	assert(buf);
	int len = strlen(buf);
	printf("recv len:%d,buf:%s", len, buf);
	write(fd, buf, len);
}

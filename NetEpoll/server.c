#include "epoll_socket.h"



#define MAX_CLIENT_COUNT 65535

static void accpet_client(int epfd, int *iClientCount, int listen_fd);
static void recv_client_msg(int epfd, int * fd, int *iClientCount);
static void handle_client_msg(int fd, char *buf);

int main(int argc, char const *argv[])
{

	int listen_fd = socket_bind("0.0.0.0", PORT);

	printf("socket_bind listen_fd:%d\n", listen_fd);


	/*声明epoll_event结构体变量，ev用于注册事件，数组用于回传要处理的事件*/
	struct epoll_event events[MAX_CLIENT_COUNT];

	/*生成用于处理accept的epoll专用文件描述符*/
	int epfd = epoll_create(MAX_CLIENT_COUNT);

	SetSocketEvents(epfd, listen_fd, EPOLL_CTL_ADD);

	int iClientCount = 1;
	while(1)
	{
		/*等待事件发生*/
		int nfds = epoll_wait(epfd, events, MAX_CLIENT_COUNT, 0);
		if (nfds <= 0)
		{
			continue;
		}

		/*处理发生的事件*/
		for (int i = 0; i<nfds; ++i)
		{
			/*检测到用户链接*/
			if (events[i].data.fd == listen_fd)
			{
				accpet_client(epfd, &iClientCount, listen_fd);

			}//if
			 /*如果是已链接用户，并且收到数据，进行读入*/
			else if (events[i].events & EPOLLIN)
			{
				if (events[i].data.fd < 0)
				{
					continue;
				}
				recv_client_msg(epfd, &(events[i].data.fd), &iClientCount);
									
			}//else
			else if (events[i].events & EPOLLOUT)
			{
				if (events[i].data.fd < 0)
				{
					continue;
				}
				char buf[65535] = { 0 };
				int n = 0;
				int send_len = write(events[i].data.fd, buf, n);
				if (send_len < 0)
				{
					printf("error writing to the sockfd!\n");
					break;
				}//if

				SetSocketEvents(epfd, events[i].data.fd, EPOLL_CTL_MOD);

			}//else
		}//for

	}
}


static void accpet_client(int epfd, int *iClientCount,int listen_fd)
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
		/*设置为非阻塞*/
		SetSocketNonblock(client_fd);

		SetSocketEvents(epfd, client_fd, EPOLL_CTL_ADD);

		(*iClientCount)++;

		printf("new client accpeted,client_fd:%d,*iClientCount:%d\n", client_fd, (*iClientCount));
	}
}

static void recv_client_msg(int epfd, int * fd, int *iClientCount)
{
	char buf[65535] = { 0 };
	int nread = read(*fd, buf, 65535);
	if (nread <= 0)
	{
		printf("one socket close,client_fd:%d,errno:%d - %s\n", fd, errno, strerror(errno));

		close(*fd);
		*fd = -1;
		--(*iClientCount);
	}
	else
	{
		handle_client_msg(*fd, buf);

		SetSocketEvents(epfd, *fd, EPOLL_CTL_MOD);

	}
}


static void handle_client_msg(int fd, char *buf)
{
	assert(buf);
	int len = strlen(buf);
	printf("recv len:%d,buf:%s", len, buf);
	write(fd, buf, len);
}

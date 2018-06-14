#include "poll_socket.h"

/*

poll()���������������ĳЩUnixϵͳ�ṩ������ִ����select()����ͬ�ȹ��ܵĺ��������������������������
#include <poll.h>
int poll(struct pollfd fds[], nfds_t nfds, int timeout)��
����˵�� :

fds����һ��struct pollfd�ṹ���͵����飬���ڴ����Ҫ�����״̬��Socket��������ÿ�������������֮��
ϵͳ�������������飬���������ȽϷ��㣻�ر��Ƕ���socket���ӱȽ϶������£���һ���̶��Ͽ�����ߴ�
���Ч�ʣ���һ����select()������ͬ������select()����֮��select()�����������������socket������
���ϣ�����ÿ�ε���select()֮ǰ�������socket���������¼��뵽�����ļ����У���ˣ�select()�����ʺ�
��ֻ���һ��socket���������������poll()�����ʺ��ڴ���socket�������������


nfds��nfds_t���͵Ĳ��������ڱ������fds�еĽṹ��Ԫ�ص���������

timeout����poll��������������ʱ�䣬��λ�����룻

����ֵ :
>0������fds��׼���ö���д�����״̬����Щsocket����������������

== 0������fds��û���κ�socket������׼���ö���д���������ʱpoll��ʱ����ʱʱ����timeout���룻���仰
˵�����������socket��������û���κ��¼������Ļ�����ôpoll()����������timeout��ָ���ĺ���ʱ�䳤��֮
�󷵻أ����timeout == 0����ôpoll() �����������ض������������timeout == INFTIM����ôpoll() ������һ
ֱ������ȥ��ֱ��������socket�������ϵĸ���Ȥ���¼������ǲŷ��أ��������Ȥ���¼���Զ����������ôpoll()
�ͻ���Զ������ȥ��

- 1�� poll��������ʧ�ܣ�ͬʱ���Զ�����ȫ�ֱ���errno��

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

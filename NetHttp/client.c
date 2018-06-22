#include "epoll_socket.h"



int main(int argc, char const *argv[])
{
	// http://www.tuling123.com/openapi/api
	char * pip_tuling = socket_hosttoip("www.tuling123.com");
	printf("socket_hosttoip - pip_tuling:%s\n", pip_tuling);
	int client_fd = socket_connect(pip_tuling, 80);
	printf("socket_connect - client_fd:%d\n", client_fd);
	
	int iCount = 0;
	char buffer[65535] = { 0 };	
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%s", http_get_post_head());
	int len_buff = strlen(buffer);
	while (len_buff>0)
	{
		int len_send = write(client_fd, buffer, len_buff);
		printf("len_buff:%d,len_send:%d,buffer:\n%s", len_buff, len_send, buffer);
		len_buff -= len_send;
	}

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		int nread = read(client_fd, buffer, 65535);
		if (nread == 0)
		{
			close(client_fd);
			break;
		}
		else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			continue;
		}
		else
		{
			printf("recv - nread:%d,len:%d,buffer:\n%s.", nread,strlen(buffer), buffer);
		}
		usleep(3* 1000 * 1000); // 3 seconds
	}
}




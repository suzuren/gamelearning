#include "epoll_socket.h"
//#include<process.h>


int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, PORT);

	printf("socket_connect client_fd:%d\n", client_fd);

	int iCount = 0;
	char buffer[65535] = { 0 };	

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));

		sprintf(buffer, "[%s] pid:%d,hello world %03d\n", getStrTime(), getpid(),iCount++);

		int len_buff = strlen(buffer);
		int len_send = write(client_fd, buffer, len_buff);
		if (len_buff != len_send)
		{
			printf("send buf error\n");
		}
		
		printf("len_buff:%d,len_send:%d,buf:%s", len_buff, len_send, buffer);

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
			//printf("recv - len:%d,buf:%s.", strlen(read_buf), read_buf);
		}
		usleep(3* 1000 * 1000); // 3 seconds
	}
}




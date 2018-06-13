#include "select.h"



int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, PORT);

	printf("socket_connect client_fd:%d\n", client_fd);

	int iCount = 0;
	char write_buf[65535] = { 0 };	

	while (1)
	{
		memset(write_buf, 0, sizeof(write_buf));

		sprintf(write_buf, "%s %d", "hello world", iCount++);

		int len_buff = strlen(write_buf);
		int len_send = write(client_fd, write_buf, len_buff);
		if (len_buff != len_send)
		{
			printf("send buf error\n");
		}


		printf("%s len_buff:%d,len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, write_buf);

		char read_buf[65535] = { 0 };
		int nread = read(client_fd, read_buf, 65535);
		if (nread <= 0)
		{
			close(client_fd);
			break;
		}
		else
		{
			printf("recv - len:%d,buf:%s.\n", strlen(read_buf), read_buf);
		}
		sleep(8);
	}
}




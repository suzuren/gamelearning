#include "select.h"
#include "network_packet.h"


int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, 6613);

	printf("socket_connect client_fd:%d\n", client_fd);

	int iCount = 0;
	char write_buf[65535] = { 0 };	
	clinet_packet cpack;
	memset(&cpack,0,sizeof(clinet_packet));
	cpack.header.type = 2;
	cpack.header.cmd = 3008;
	cpack.header.len = 10;
	sprintf(cpack.protobuf, "%s", "helloworld");
	memcpy(write_buf, &cpack, 16);
	while (1)
	{
		//memset(write_buf, 0, sizeof(write_buf));

		//sprintf(write_buf, "%s %d\n", "hello world", iCount++);

		//int len_buff = strlen(write_buf);
		int len_buff = 16;
		int len_send = write(client_fd, write_buf, len_buff);
		if (len_buff != len_send)
		{
			printf("send buf error\n");
		}
		//else
		//{
		//	break;
		//}

		printf("%s len_buff:%d,len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, write_buf);

		return 1;

		char read_buf[65535] = { 0 };
		int nread = read(client_fd, read_buf, 65535);
		if (nread == 0)
		{
			close(client_fd);
			break;
		}
		else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))// || errno == EINTR))
		{
			// EINTR 加上这个忽略 select 模式的服务端就会收不到read=0不知道为什么

			// EAGAIN 提示你的应用程序现在没有数据可读请稍后再试。
			// EWOULDBLOCK 用于非阻塞模式，不需要重新读或者写
			// EINTR 指操作被中断唤醒，需要重新读 / 写
			continue;
		}
		else
		{
			//printf("recv - len:%d,buf:%s.\n", strlen(read_buf), read_buf);
		}
		sleep(1);
	}
}




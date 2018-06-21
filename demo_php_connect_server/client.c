#include "select.h"

#pragma pack(1)
struct PhpHeader
{
	unsigned short      datalen;     // 消息数据长度(不包括包头)
	unsigned short      cmd;      	 // 消息id	
};
#pragma pack()

int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, PORT);

	printf("socket_connect client_fd:%d\n", client_fd);

	int iCount = 0;
	char write_buf[65535] = { 0 };	
	char temp[512] = { 0 };
	sprintf(temp, "hello world");

	struct PhpHeader *pHeader = (struct PhpHeader*)write_buf;
	pHeader->cmd = 23;

	pHeader->datalen = strlen(temp);
	sprintf(write_buf +4, "%s", temp);

	int pack_size = pHeader->datalen + 4;

	for (int i = 0; i < pack_size; i++)
	{
		printf("%c ", write_buf[i]);
	}
	printf("\n");
	//memset(write_buf, 0, sizeof(write_buf));

	//sprintf(write_buf, "%s %d\n", "hello world", iCount++);

	int len_buff = pack_size;
	int len_send = write(client_fd, write_buf, len_buff);
	if (len_buff != len_send)
	{
		printf("send buf error\n");
	}

	//printf("%s len_buff:%d,len_send:%d,temp:%s,buf:%s\n", getStrTime(), len_buff, len_send, temp, write_buf);
	while (1)
	{

		//break;
		char read_buf[65535] = { 0 };
		int nread = read(client_fd, read_buf, 65535);
		if (nread == 0)
		{
			close(client_fd);
			break;
		}
		else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
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

			for (int i = 0; i < nread; i++)
			{
				printf("%c ", read_buf[i]);
			}
			printf("\n");
		}
		break;
		sleep(1000000);
	}
}




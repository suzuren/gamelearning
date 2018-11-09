#include "select.h"

#include <arpa/inet.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
bool CheckStorageMode();

char* itoa_parser(int num, int radix)
{
	static char str[32];
	memset(str, 0, sizeof(str));
	static char index[17] = "0123456789ABCDEF";
	unsigned unum;
	int i = 0, j, k;
	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else
	{
		unum = (unsigned)num;
	}
	do
	{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	if (str[0] == '-')
	{
		k = 1;
	}
	else
	{
		k = 0;
	}
	char temp;
	for (j = k; j <= (i - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}


char * http_build_post_head(const char * api,const char * body)
{
	static char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST /landlords-pokerbot/");strcat(buffer, api);strcat(buffer, " HTTP/1.1\r\n");
	strcat(buffer, "Host: 127.0.0.1:8888\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: ");	strcat(buffer, itoa_parser(strlen(body), 10));	strcat(buffer, "\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	strcat(buffer, body);
	return buffer;
}

int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, PORT);
	printf("socket_connect client_fd:%d\n", client_fd);

	char buffer[65535] = { 0 };
	char write_buf[65535] = { 0 };
	//sprintf(write_buf, "%s", http_build_post_head("helloworld1","helloworld2"));
	sprintf(write_buf, "%s", "helloworld");

	unsigned short pack_size = strlen(write_buf);
	unsigned int heard_szie = 2;
	buffer[0] = (pack_size >> 8) & 0xff;
	buffer[1] = pack_size & 0xff;	
	memcpy(buffer + heard_szie, write_buf, pack_size);
	

	unsigned short all_pack_size = pack_size + heard_szie;
	int total_len_send = 0;

	for (; all_pack_size>0;)
	{
		int len_buff = strlen(write_buf);
		int len_send = write(client_fd, write_buf + total_len_send, all_pack_size);

		if (len_send > 0)
		{
			all_pack_size -= len_send;
			total_len_send += len_send;

			printf("%s len_buff:%d,len_send:%d,all_pack_size:%d,total_len_send:%d,buf:%s\n",
				getStrTime(), len_buff, len_send, all_pack_size, total_len_send, write_buf);

		}
	}


	while (true)
	{
		//memset(write_buf, 0, sizeof(write_buf));
		//sprintf(write_buf, "%s %d\n", "hello world", iCount++);

		//int len_buff = strlen(write_buf);
		//int len_send = write(client_fd, write_buf, len_buff);
		//if (len_buff != len_send)
		//{
		//	printf("send buf error\n");
		//}

		//printf("%s len_buff:%d,len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, write_buf);

		char read_buf[65535] = { 0 };
		int nread = read(client_fd, read_buf, 65535);
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
			printf("recv - len:%ld,buf:%s.\n", strlen(read_buf), read_buf);
		}
		sleep(1);
	}


}




bool CheckStorageMode()
{
	int iFlag = 0x12345678;
	char *cByte = (char *)&iFlag;
	return(*cByte == 0x12);
}



#include "epoll_socket.h"

#include <iconv.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/stat.h>

int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen,
	char *outbuf, size_t outlen) {
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	*pout = '\0';

	return 0;
}

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

char * http_get_post_head()
{
	static char buffer[2048];
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST /openapi/api HTTP/1.0\r\n");
	strcat(buffer, "Host: www.tuling123.com\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: 147\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n");

	//printf("len:%ld,buffer:%s", strlen(buffer), buffer);
	return buffer;
}

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
	//sprintf(buffer, "%s", http_get_post_head());
	//int len_buff = strlen(buffer);

	char * ppostdata = http_get_post_head();
	g2u(ppostdata, strlen(ppostdata), buffer, 2048);
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
			printf("recv - len:%d,buffer:%s.", strlen(buffer), buffer);
		}
		usleep(3* 1000 * 1000); // 3 seconds
	}
}




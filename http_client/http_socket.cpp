
#include "http_socket.h"

bool SetSocketEvents(int epfd, int fd, int op)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLOUT;
	epoll_ctl(epfd, op, fd, &ev);
	return true;
}

bool SetSocketNonblock(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return false;
	}
	flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	if (flag < 0)
	{
		return false;
	}
	return true;
}

char * socket_hosttoip(const char * phost)
{
	static char buffer[32];
	memset(buffer, 0, sizeof(buffer));
	if (phost == NULL)
	{
		return NULL;
	}
	struct hostent *phostent = gethostbyname(phost);
	if (phostent == NULL)
	{
		return NULL;
	}
	switch (phostent->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
	{
		const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer, sizeof(buffer));
		if (ret_inet != NULL)
		{
			return buffer;
		}
	}break;
	default:
	{
		//
	}
	break;
	}
	return NULL;
}

int socket_connect(const char *ip, int port, int * fd)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1)
	{
		return -1;
	}
	SetSocketNonblock(client_fd);
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1 && errno != EINPROGRESS)
	{
		return -1;
	}
	*fd = client_fd;
	if (ret == 0)
	{
		return 1;
	}
	return 0;
}


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
	strcat(buffer, "Host: 47.94.250.154:38018\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: ");	strcat(buffer, itoa_parser(strlen(body), 10));	strcat(buffer, "\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	strcat(buffer, body);
	return buffer;
}



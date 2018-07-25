

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
		close(client_fd);
		return -1;
	}
	bool flag = SetSocketNonblock(client_fd);
	if (flag == false)
	{
		close(client_fd);
		return -1;
	}
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1 && errno != EINPROGRESS)
	{
		close(client_fd);
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

static inline int hextoint(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

bool atoi_parser(int * ptrnum,char * ptrstr,int len)
{
	*ptrnum = 0;
	if (ptrnum == NULL || ptrstr == NULL || len == 0)
	{
		return false;
	}
	for (int i = 0; i < len; i++)
	{
		(*ptrnum) += (hextoint(ptrstr[i]));
	}
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

bool http_body_is_final(std::string & strdata)
{
	std::string::size_type pos_http_body_end = strdata.find("\r\n0\r\n\r\n");
	if (pos_http_body_end != std::string::npos)
	{
		return true;
	}
	return false;
}


unsigned long long	GetMillisecond()
{
	//struct timespec
	//{
	//	time_t tv_sec; // seconds[秒]
	//	long tv_nsec; // nanoseconds[纳秒]
	//};
	//int clock_gettime(clockid_t clk_id, struct timespect *tp);
	////@clk_id:
	//CLOCK_REALTIME:系统实时时间, 随系统实时时间改变而改变
	//CLOCK_MONOTONIC : 从系统启动这一刻起开始计时, 不受系统时间被用户改变的影响
	//CLOCK_PROCESS_CPUTIME_ID : 本进程到当前代码系统CPU花费的时间
	//CLOCK_THREAD_CPUTIME_ID : 本线程到当前代码系统CPU花费的时间

	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;

	return millisecond;
}
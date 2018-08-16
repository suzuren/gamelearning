

#include "epoll_socket.h"

bool SetSocketEvents(int epfd, int fd, int op)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
	int ret = epoll_ctl(epfd, op, fd, &ev);
	if (ret == -1)
	{
		return false;
	}
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

int socket_connect(const char *ip, int port, int * fd)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1)
	{
		return -1;
	}
	do
	{
		bool flag = SetSocketNonblock(client_fd);
		if (flag == false)
		{
			break;
		}
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = inet_addr(ip);
		int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (ret == -1 && errno != EINPROGRESS)
		{
			break;
		}
		*fd = client_fd;
		if (ret == 0)
		{
			return 1;
		}
	}
	while (false);
	close(client_fd);
	return -1;
}

int socket_bind(const char *ip, int port)
{
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1)
	{
		return -1;
	}
	do
	{
		bool flag = SetSocketNonblock(listen_fd);
		if (flag == false)
		{
			break;
		}
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = inet_addr(ip);
		if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
		{
			break;
		}
		if (listen(listen_fd, 16) == -1)
		{
			break;
		}
		return listen_fd;
	} while (false);
	close(listen_fd);
	return -1;
}

int socket_wait(int epfd, struct epoll_event * events, int maxevents,int timeout)
{
	int nfds = epoll_wait(epfd, events, maxevents, timeout);
	if (nfds <= 0)
	{
		return -1;
	}
	return nfds;
}

std::string FormatToString(const char* fmt, ...)
{
	va_list ap;
	char szBuffer[2048] = { 0 };
	va_start(ap, fmt);
	vsnprintf(szBuffer, sizeof(szBuffer), fmt, ap);
	va_end(ap);
	return szBuffer;
}

unsigned long long	GetMillisecond()
{
	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;

	return millisecond;
}

void thread_sleep(unsigned int msec)
{
	struct timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}


#include "websocket.h"
#include <string>
using namespace std;

static int socket_stage = CONN_IDLE;
static bool socket_isconnect = false;
static int connect_fd = -1;
static std::string strip = "127.0.0.1";
static unsigned int port = 8001;



int WebConnect()
{
	int ret = -1;
	if (connect_fd >0)
	{
		connect_fd = -1;
	}

	if (socket_stage == CONN_IDLE)
	{
		ret = tcp_connect(&connect_fd, strip.c_str(), port, 0);
	}
	else
	{
		if (connect_fd < 0)
		{
			ret = tcp_connect(&connect_fd, strip.c_str(), port, 0);
		}
		else
		{
			ret = 0;
		}
	}
	int saveErrorNo = errno;
	if (ret <0)
	{
		if (ret == SOCKET_CREATE_FAILED)
		{
			//log_error("*STEP: helper create socket failed, saveErrorNo[%d], msg[%s]", saveErrorNo, strerror(saveErrorNo));
			return -1;
		}
		if (saveErrorNo != EINPROGRESS)
		{
			//log_error("*STEP: PROXY connect to logic failed, saveErrorNo[%d], msg[%s]", saveErrorNo , strerror(saveErrorNo));
			return -1;
		}
		socket_stage = CONN_CONNECTING;
		//log_debug("*STEP: PROXY connecting to logic, unix fd[%d]", connect_fd);
	}
	else
	{
		socket_stage = CONN_CONNECTED;
		socket_isconnect = true;
	}
	return 0;
}


int tcp_connect(int* fd, const char* address, int port, int block)
{
	*fd = SOCKET_INVALID;
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("fd:%d,errno:%d", fd, errno);
		return SOCKET_CREATE_FAILED;
	}

	sockaddr_in tempsock;
	bzero((char*)&tempsock, sizeof(tempsock));
	tempsock.sin_family = AF_INET;
	tempsock.sin_port = htons(port);
	tempsock.sin_addr.s_addr = inet_addr(address);
	socklen_t addrlen = sizeof(struct sockaddr_in);

	int flags = 1;
	if (((flags = fcntl(*fd, F_GETFL, 0)) < 0 || fcntl(*fd, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		printf("flags:%d,errno:%d", flags, errno);
		close(*fd);
		return -1;
	}
	int ret = -1;
	if ((ret = ::connect(*fd, (struct sockaddr *)&tempsock, addrlen)) < 0)
	{
		printf("ret:%d,errno:%d", ret, errno);
		if (errno != EINPROGRESS)
		{
			close(*fd);
			*fd = SOCKET_INVALID;
			return -1;
		}
	}
	printf("ret:%d",ret);
	if (0 == ret) // success return
	{
		return 0;
	}

	//struct pollfd {
	//	int		fd;			文件描述符
	//	short	events;		等待的需要测试事件
	//	short	revents;	实际发生了的事件，也就是返回结果
	//};
	//int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	//nfds：nfds_t类型的参数，用于标记数组fds中的结构体元素的总数量;
	//timeout：是poll函数调用阻塞的时间，单位:毫秒

	int pollRet = -1;
	struct pollfd event;

	memset(&event, 0, sizeof(event));
	event.fd = *fd;
	event.events = POLLOUT | POLLIN;
	//event.events = POLLOUT | POLLIN | POLLERR | POLLHUP | POLLNVAL;
	pollRet = poll((struct pollfd *)&event, 1, 50);

	if (pollRet > 0)
	{
		int error = -1;
		socklen_t len = sizeof(error);
		if (getsockopt(*fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len) < 0)
		{
			pollRet = -1;
		}
		if (error != 0)
		{
			pollRet = -1;
		}
	}
	if (pollRet <= 0)
	{
		close(*fd);
		*fd = SOCKET_INVALID;
		return -1;
	}
	return 0;
}




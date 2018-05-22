
#include "websocket.h"
#include <string>
using namespace std;

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>


static int socket_stage = CONN_IDLE;
static bool socket_isconnect = false;
static int connect_fd = -1;
static std::string strip = "127.0.0.1";
static unsigned int port = 8001;
static fd_set read_fds, write_fds, except_fds;
static int max_fds = 1;

static pthread_t p_thread;

int check_events()
{
	struct timeval tvalue;
	tvalue.tv_sec = 10;
	tvalue.tv_usec = 0;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&except_fds);

	FD_SET(connect_fd, &read_fds);
	FD_SET(connect_fd, &write_fds);
	FD_SET(connect_fd, &except_fds);

	int ret = select(max_fds + 1, &read_fds, &write_fds, &except_fds, &tvalue);
	if (ret == -1)
	{
		printf("select error:%d\n", errno);
	}
	else if (ret == 0)
	{
		//continue;
		printf("time out\n");
	}
	else
	{
		if (FD_ISSET(connect_fd, &read_fds))
		{
			socket_isconnect = true;
			char curr_recv_buf[1024 * 8] = { 0 };
			int curr_recv_len = ::recv(connect_fd, curr_recv_buf, 1024 * 8, 0);
			if (-1 == curr_recv_len)
			{
				if (errno != EAGAIN && errno != EINTR && errno != EINPROGRESS)
				{
					socket_stage = CONN_FATAL_ERROR;
					//log_info ("recv failed from fd[%d], msg[%s]", netfd, strerror(errno));
				}
				else
				{
					socket_stage = CONN_DATA_RECVING;
				}
			}
			else if (0 == curr_recv_len)
			{
				socket_stage = CONN_DISCONNECT;
				//log_info ("CCTcpHandle::handle_input()--curr_recv_len=0,connection disconnect by user fd[%d], msg[%s]", netfd, strerror(saveErrorNo));
			}
			else
			{
				printf("curr_recv_len:%d,curr_recv_buf:%s", curr_recv_len,curr_recv_buf);
			}
		}
		if (FD_ISSET(connect_fd, &write_fds))
		{
			char curr_send_buf[512] = { 0 };
			sprintf(curr_send_buf,"%s","hello world.");
			int curr_send_len = strlen(curr_send_buf);

			int ret = ::send(connect_fd, curr_send_buf, curr_send_len, 0);
			int saveErrorNo = errno;
			if (-1 == ret)
			{
				if (saveErrorNo == EINTR || saveErrorNo == EAGAIN || saveErrorNo == EINPROGRESS)
				{
					socket_stage = CONN_DATA_SENDING;
					return socket_stage;
				}
				socket_stage = CONN_FATAL_ERROR;
				return socket_stage;
			}

			if (ret == curr_send_len)
			{
				//log_debug("send complete, send len=[%d]",ret);
				socket_stage = CONN_SEND_DONE;
				return socket_stage;
			}
			else if (ret < curr_send_len)
			{
				//log_debug("had sent part of data, send len=[%d]",ret);
				socket_stage = CONN_DATA_SENDING;
				return socket_stage;
			}

		}
	}
	return 0;
}

void *thread_run(void* p)
{
	while (true)
	{
		check_events();
	}

	return NULL;
}


void web_run()
{
	int ret = pthread_create(&p_thread, 0, thread_run, NULL);
	if (ret == -1)
	{
		printf("can't create thread: %s\n", strerror(errno));
	}
}


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

void send_handshark()
{
	char buff[2048] = { 0 };
	//strcat(buff, "GET /chat HTTP/1.1\r\n");
	//strcat(buff, "Host: server.example.com\r\n");
	//strcat(buff, "Upgrade: websocket\r\n");
	//strcat(buff, "Connection: Upgrade\r\n");
	//strcat(buff, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");	// rand Base64 encode value
	//strcat(buff, "Sec-WebSocket-Protocol: chat, superchat\r\n");		// 用户定义的字符串，用来区分同URL下，不同的服务所需要的协议
	//strcat(buff, "Sec-WebSocket-Version: 13\r\n");						// 使用的Websocket Draft（协议版本）
	//strcat(buff, "Origin: http://example.com\r\n");


	strcat(buff, "GET /chat HTTP/1.1\r\n");
	strcat(buff, "host: ws://127.0.0.1:8001/ws\r\n");
	strcat(buff, "upgrade: websocket\r\n");
	strcat(buff, "connection: Upgrade\r\n");
	strcat(buff, "sec-websocket-key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
	strcat(buff, "sec-websocket-protocol: chat, superchat\r\n");
	strcat(buff, "sec-websocket-version: 13\r\n");
	strcat(buff, "origin: ws://127.0.0.1:8001/\r\n");


	int curr_send_len = strlen(buff);
	int ret = ::send(connect_fd, buff, curr_send_len, 0);

	printf("connect_fd:%d,ret:%d,curr_send_len:%d,buff:%s\n", connect_fd,ret, curr_send_len, buff);
}

int tcp_connect(int* fd, const char* address, int port, int block)
{
	*fd = SOCKET_INVALID;
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("fd:%d,errno:%d\n", *fd, errno);
		return SOCKET_CREATE_FAILED;
	}

	sockaddr_in tempsock;
	bzero((char*)&tempsock, sizeof(tempsock));
	tempsock.sin_family = AF_INET;
	tempsock.sin_port = htons(port);
	tempsock.sin_addr.s_addr = inet_addr(address);
	socklen_t addrlen = sizeof(struct sockaddr_in);


	int ret = -1;
	if ((ret = ::connect(*fd, (struct sockaddr *)&tempsock, addrlen)) < 0)
	{
		printf("ret:%d,errno:%d\n", ret, errno);
		if (errno != EINPROGRESS)
		{
			close(*fd);
			*fd = SOCKET_INVALID;
			return -1;
		}
	}

	int flags = 1;
	if (((flags = fcntl(*fd, F_GETFL, 0)) < 0 || fcntl(*fd, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		printf("fcntl - flags:%d,errno:%d\n", flags, errno);
		close(*fd);
		return -1;
	}

	printf("connect - ret:%d\n",ret);
	if (0 == ret) // success return
	{
		send_handshark();

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




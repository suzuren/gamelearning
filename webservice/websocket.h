#ifndef __NET_SOCKET_H_
#define __NET_SOCKET_H_

#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include<sys/poll.h>

#define SOCKET_INVALID          -1
#define SOCKET_CREATE_FAILED    -2
#define SOCKET_BIND_FAILED      -3
#define SOCKET_LISTEN_FAILED    -4

enum CConnState
{
	CONN_IDLE,
	CONN_FATAL_ERROR,
	CONN_DATA_ERROR,
	CONN_CONNECTING,
	CONN_DISCONNECT,
	CONN_CONNECTED,
	CONN_DATA_SENDING,
	CONN_DATA_RECVING,
	CONN_SEND_DONE,
	CONN_RECV_DONE,
	CONN_APPEND_SENDING,
	CONN_APPEND_DONE,
	CONN_XML_POLICY
};



int WebConnect();

int tcp_connect(int* fd, const char* address, int port, int block);

#endif

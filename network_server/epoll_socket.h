
#ifndef __EPOLL_SOCKET_
#define __EPOLL_SOCKET_



#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>  // standard IO
#include <stdlib.h> //standard  libary
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> //unix standard

#include <sys/select.h> // select IO multiplexing model
#include <sys/epoll.h>

#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <sys/un.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <netdb.h>
#include <stdbool.h>
#include <string>
#include <iomanip>
#include <cstdarg>

#define MAX_TIME_OUT		(30*1000)
#define MAX_SOCKET_CONNECT	10240
#define SOCKET_TCP_BUFFER   65535

#define IPADDR				"127.0.0.1"
#define PORT				9876

bool SetSocketEvents(int epfd, int fd, int op);
bool SetSocketNonblock(int fd);
int socket_bind(const char *ip, int port);
int socket_connect(const char *ip, int port, int * fd);
unsigned long long	GetMillisecond();
std::string FormatToString(const char* fmt, ...);
int socket_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
void thread_sleep(unsigned int msec);

#endif



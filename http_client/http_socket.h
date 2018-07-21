
#ifndef __HTTP_SOCKET_
#define __HTTP_SOCKET_



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

#define IPADDRESS "127.0.0.1"
#define PORT 8793



bool SetSocketEvents(int epfd, int fd, int op);
bool SetSocketNonblock(int fd);
char * socket_hosttoip(const char * phost);
int socket_connect(const char *ip, int port, int * fd);
char * http_build_post_head(const char * api, const char * body);


#endif



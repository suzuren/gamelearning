#ifndef skynet_socket_h
#define skynet_socket_h

#include "socket_info.h"

struct skynet_context;

#define SKYNET_SOCKET_TYPE_DATA 1
#define SKYNET_SOCKET_TYPE_CONNECT 2
#define SKYNET_SOCKET_TYPE_CLOSE 3
#define SKYNET_SOCKET_TYPE_ACCEPT 4
#define SKYNET_SOCKET_TYPE_ERROR 5
#define SKYNET_SOCKET_TYPE_UDP 6
#define SKYNET_SOCKET_TYPE_WARNING 7

struct skynet_socket_message {
	int type;
	int id;
	int ud;
	char * buffer;
};

void skynet_socket_init();
void skynet_socket_exit();
void skynet_socket_free();
int skynet_socket_poll();
void skynet_socket_updatetime();

int skynet_socket_send(uint32_t handle, int id, void *buffer, int sz);
int skynet_socket_send_lowpriority(uint32_t handle, int id, void *buffer, int sz);
int skynet_socket_listen(uint32_t handle, const char *host, int port, int backlog);
int skynet_socket_connect(uint32_t handle, const char *host, int port);
int skynet_socket_bind(uint32_t handle, int fd);
void skynet_socket_close(uint32_t handle, int id);
void skynet_socket_shutdown(uint32_t handle, int id);
void skynet_socket_start(uint32_t handle, int id);
void skynet_socket_nodelay(uint32_t handle, int id);

int skynet_socket_udp(uint32_t handle, const char * addr, int port);
int skynet_socket_udp_connect(uint32_t handle, int id, const char * addr, int port);
int skynet_socket_udp_send(uint32_t handle, int id, const char * address, const void *buffer, int sz);
const char * skynet_socket_udp_address(struct skynet_socket_message *, int *addrsz);

struct socket_info * skynet_socket_info();

#endif

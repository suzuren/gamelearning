#ifndef __HTTP_SOCKET_H__
#define __HTTP_SOCKET_H__

#define SKYNET_SOCKET_TYPE_DATA 1
#define SKYNET_SOCKET_TYPE_CONNECT 2
#define SKYNET_SOCKET_TYPE_CLOSE 3
#define SKYNET_SOCKET_TYPE_ACCEPT 4
#define SKYNET_SOCKET_TYPE_ERROR 5
#define SKYNET_SOCKET_TYPE_UDP 6
#define SKYNET_SOCKET_TYPE_WARNING 7


#define PTYPE_TEXT 0
#define PTYPE_RESPONSE 1
#define PTYPE_MULTICAST 2
#define PTYPE_CLIENT 3
#define PTYPE_SYSTEM 4
#define PTYPE_HARBOR 5
#define PTYPE_SOCKET 6
// read lualib/skynet.lua examples/simplemonitor.lua
#define PTYPE_ERROR 7	
// read lualib/skynet.lua lualib/mqueue.lua lualib/snax.lua
#define PTYPE_RESERVED_QUEUE 8
#define PTYPE_RESERVED_DEBUG 9
#define PTYPE_RESERVED_LUA 10
#define PTYPE_RESERVED_SNAX 11

#define PTYPE_TAG_DONTCOPY 0x10000
#define PTYPE_TAG_ALLOCSESSION 0x20000



struct skynet_socket_message {
	int type;
	int id;
	int ud;
	char * buffer;
};

void http_socket_init();
void http_socket_exit();
void http_socket_free();
int http_socket_poll();

int http_socket_connect(const char *host, int port);
int http_socket_send(int id, void *buffer, int sz);
int http_socket_send_lowpriority(int id, void *buffer, int sz);

int http_check_protocol(char* host, char** protocol, char** domain_name, char** ip, int* port);

int http_init_interface(int fd, char* protocol);
int http_socket_request(int fd, char* method, char* host, char* uri, char* recvheader, char* header, char* content);

#endif

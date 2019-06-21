#ifndef __HTTP_SOCKET_H__
#define __HTTP_SOCKET_H__

void http_socket_init();
void http_socket_exit();
void http_socket_free();
int http_socket_poll();

int http_socket_connect(const char *host, int port);
int http_socket_send(int id, void *buffer, int sz);
int http_socket_send_lowpriority(int id, void *buffer, int sz);

int http_check_protocol(char* host, char** protocol, char** domain_name, char** ip, int* port);

int http_init_interface(char* protocol, int fd);

#endif

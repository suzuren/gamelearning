
LUA_INC := /usr/local/lua/include

all: ltls.so libtls.so libsocket_server.so libskynet_mq.so libhttp_socket.so libhttp_module.so testHttpModule

#TLS_MODULE=ltls tls
TLS_LIB=/root/code/openssl/
TLS_INC=/root/code/openssl/include/

SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall

ltls.so : ltls.c
	gcc $(CFLAGS) $(SHARED) -L$(TLS_LIB) -I$(TLS_INC) $^ -o $@ -lssl -I$(LUA_INC)

libtls.so : libtls.c
	gcc $(CFLAGS) $(SHARED) -L$(TLS_LIB) -I$(TLS_INC) $^ -o $@ -lssl

libsocket_server.so : socket_server.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

libskynet_mq.so : skynet_mq.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

libhttp_socket.so : http_socket.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@ -L. -ltls -lsocket_server -lskynet_mq

libhttp_module.so : http_module.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@ -L. -ltls -lsocket_server -lskynet_mq -lhttp_socket

testHttpModule : main.c
	gcc -g -Wall $^ -o $@ -L. -lrt -lpthread -ltls -lsocket_server -lskynet_mq -lhttp_socket -lhttp_module

clean:
	rm -f *.so core.* testHttpModule

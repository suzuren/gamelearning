
LUA_INC := /usr/local/lua/include

all: ltls.so libtls.so libsocket_server.so libhttp_module.so testHttpModule

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

libhttp_module.so : http_module.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@ -L. -ltls -lsocket_server

testHttpModule : main.c
	gcc -g -Wall $^ -o $@ -L. -lrt -lpthread -ltls -lsocket_server -lhttp_module

clean:
	rm -f *.so core.* testHttpModule

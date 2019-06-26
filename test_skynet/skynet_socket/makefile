
TARGET := testSkynetSocket

all: libsocket_server.so libskynet_mq.so libskynet_timer.so libskynet_context.so libskynet_socket.so $(TARGET)

SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall

libsocket_server.so : socket_server.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

libskynet_mq.so : skynet_mq.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@
	
libskynet_timer.so : skynet_timer.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

libskynet_context.so : skynet_context.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@
	
libskynet_socket.so : skynet_socket.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

$(TARGET) : main.c
	gcc -g -Wall $^ -o $@ -L./ -lrt -lpthread -lsocket_server -lskynet_mq -lskynet_timer -lskynet_context -lskynet_socket

clean:
	rm -f *.so core.* $(TARGET)


TARGET := testSkynetSocket

all: libsocket_server.so libskynet_mq.so $(TARGET)

SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall

libsocket_server.so : socket_server.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

libskynet_mq.so : skynet_mq.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@ -L./ -lsocket_server
	
$(TARGET) : main.c
	gcc -g -Wall $^ -o $@ -L./ -lrt -lpthread -lsocket_server -lskynet_mq

clean:
	rm -f *.so core.* $(TARGET)

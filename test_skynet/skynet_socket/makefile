

all: libsocket_server.so

TARGET := testSkynetSocket
SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall

libsocket_server.so : socket_server.c
	gcc $(CFLAGS) $(SHARED) $^ -o $@

$(TARGET) : main.c
	gcc -g -Wall $^ -o $@ -L. -lrt -lpthread -lsocket_server

clean:
	rm -f *.so core.* $(TARGET)

CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -liconv -lstdc++ -std=c++11

all: client
clean:
	rm -f *.o *.pid client
client: client.o http_mgr.o http_socket.o
	g++ -o $@ $^ $(LDFLAGS)


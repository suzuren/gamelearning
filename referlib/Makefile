CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lstdc++ -std=c++11

all: server client

clean:
	rm -f *.o *.pid server client

server: eventmanager.o tcp.o server.o
	g++ -o $@ $^ $(LDFLAGS)

client: eventmanager.o tcp.o client.o
	g++ -o $@ $^ $(LDFLAGS)




CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lstdc++ -std=c++11

all: client

clean:
	rm -f *.o *.pid server client


client: client.o
	g++ -o $@ $^ $(LDFLAGS)




CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -liconv -lstdc++ -std=c++17

all: client

clean:
	rm -f *.o *.pid client

client: client.o
	g++ -o $@ $^ $(LDFLAGS)


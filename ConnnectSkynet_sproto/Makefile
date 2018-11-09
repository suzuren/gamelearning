

CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lstdc++ -std=c++11
LUA_INC := /usr/local/lua/include

all: client

clean:
	cd sproto && make clean
	rm -f *.o *.so *.pb client
	
client: client.c
	cd sproto && make
	gcc -g -O2 -Wall -I$(LUA_INC) $^ -o $@
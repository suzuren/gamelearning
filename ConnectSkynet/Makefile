

CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lstdc++ -std=c++11
LUA_INC := /usr/local/lua/include

all: client pbcc.so loginServer.login.s2c.pb

clean:
	rm -f *.o *.so *.pb client
	cd pbc && make clean

client: client.c
	cd pbc && make
	gcc -g -O2 -Wall -I$(LUA_INC) $^ -o $@  -L./pbc/build  -lpbc

pbcc.so : lua-pbcc.c
	gcc -g -O2 -Wall -I$(LUA_INC)  -fPIC --shared $^ -o $@

GAME_PROTO = $(wildcard ./*.proto)
loginServer.login.s2c.pb : $(GAME_PROTO)
	protoc -o $@ -I. --include_imports $^

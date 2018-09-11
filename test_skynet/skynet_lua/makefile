

LUA_INC := /usr/local/lua/include

all: skynet.so

skynet.so : lua-skynet.c lua-seri.c lua-netpack.c
	gcc -g -O2 -Wall -lrt -I$(LUA_INC) -fPIC --shared $^ -o $@

clean :
	rm -f *.so



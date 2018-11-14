

LUA_INC := /usr/local/lua/include

all: skynet.so testNetpack

skynet.so : lua-skynet.c lua-seri.c lua-netpack.c
	gcc -g -O2 -Wall -lrt -I$(LUA_INC) -fPIC --shared $^ -o $@

testNetpack : lua-netpack-test.c
	gcc -o $@ $^  -I$(LUA_INC)
clean :
	rm -f *.so testNetpack



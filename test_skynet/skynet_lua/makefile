

LUA_INC := /usr/local/lua/include

all: skynet.so testNetpack testupvalue.so

skynet.so : lua-skynet.c lua-seri.c lua-netpack.c
	gcc -g -O2 -Wall -lrt -I$(LUA_INC) -fPIC --shared $^ -o $@

testNetpack : lua-netpack-test.c
	gcc -o $@ $^  -I$(LUA_INC)

testupvalue.so : lua-testupvalue.c
		gcc -Wall -O2 -shared -o testupvalue.so lua-testupvalue.c -fPIC -I$(LUA_INC)  -lm
clean :
	rm -f *.so testNetpack



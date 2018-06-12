
CC = gcc

SHARED := -fPIC --shared
LUA_INC := /usr/local/lua/include

CFLAGS = -g -O2 -Wall -I$(LUA_INC) 

ROOT_DIR := $(abspath $(lastword $(MAKEFILE_LIST)))

SUB_DIRS = $(shell ls -l | grep ^d | awk '{print $$9}')


all : testclib.so testclibtwo.so pbcc.so protobuf.so loginServer.login.s2c.pb wordfilter.so sui.so 

testclib.so : test_clib.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

testclibtwo.so : test_clib2.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

pbcc.so : lua-pbcc.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@
	
protobuf.so : pbc-lua53.c
	make -C $(SUB_DIRS)
	$(CC) -g -O2 -Wall -I$(LUA_INC)  -fPIC --shared $^ -o $@  -L./pbc/build  -lpbc

GAME_PROTO = $(wildcard ./*.proto)
loginServer.login.s2c.pb : $(GAME_PROTO)
	protoc -o $@ -I. --include_imports $^

wordfilter.so : lua-wordfilter.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

sui.so : lua-sui.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

clean :
	rm -f *.so *.pb
	make -C $(SUB_DIRS) clean

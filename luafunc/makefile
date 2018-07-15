
CC = gcc

SHARED := -fPIC --shared
LUA_INC := /usr/local/lua/include

CFLAGS = -g -O2 -Wall -I$(LUA_INC) 

all : struct.so


struct.so : lua-struct.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

clean:
	rm -rf *.log logs/ struct.so

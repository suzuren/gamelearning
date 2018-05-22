CC ?= gcc


SHARED := -fPIC --shared
LUA_INC := /usr/local/lua/include

CFLAGS = -g -O2 -Wall -I$(LUA_INC) 

all : testclib.so testclibtwo.so

testclib.so : test_clib.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

testclibtwo.so : test_clib2.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

clean :
	rm -f *.so
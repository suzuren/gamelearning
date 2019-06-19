
LUA_INC := /usr/local/lua/include

all: ltls.so tls.so

#TLS_MODULE=ltls tls
TLS_LIB=/root/code/openssl/
TLS_INC=/root/code/openssl/include/

SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall -I$(LUA_INC)

ltls.so : ltls.c
	gcc $(CFLAGS) $(SHARED) -L$(TLS_LIB) -I$(TLS_INC) $^ -o $@ -lssl

tls.so : tls.c
	gcc $(CFLAGS) $(SHARED) -L$(TLS_LIB) -I$(TLS_INC) $^ -o $@ -lssl

clean:
	rm -rf *.so

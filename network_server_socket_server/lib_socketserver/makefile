
LIB=socket_server

OBJ=socket_server.o
OBJS=`ls *.o`

all:$(LIB)

.c.o:
	gcc -O -fPIC -rdynamic $(G) -c $<

$(LIB): $(OBJ)
	gcc -shared $(G) -o lib$@.so $(OBJS)
	ar rvu lib$@.a $(OBJS)

clean:
	rm -f *.o *.so *.a
	
#socket-server : socket_server.c test.c
#	gcc -g -Wall -o $@ $^ -lpthread
#clean:
#	rm socket-server

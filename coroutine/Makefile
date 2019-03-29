all : main

main : main.c coroutine.c
	gcc -g -Wall -o $@ $^

clean :
	rm -f main *.o *.so

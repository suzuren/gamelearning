
testDaemon : main.c skynet_daemon.c
	gcc -g -Wall -o $@ $^ -lpthread

clean:
	rm -rf testDaemon *.pid

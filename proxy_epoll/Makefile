all:proxy_epoll
	cp ./proxy_epoll ./subproxy/proxy_epoll

proxy_epoll:proxy_epoll.h proxy_epoll.c
	g++ -o proxy_epoll proxy_epoll.h proxy_epoll.c
	
clean:
	rm -f *.log *.pid proxy_epoll ./subproxy/*.log ./subproxy/*.pid ./subproxy/proxy_epoll

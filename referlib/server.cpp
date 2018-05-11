#include <functional>
#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include "eventmanager.h"
#include "iobuffer.h"
#include "notification.h"
#include "tcp.h"

using epoll_threadpool::EventManager;
using epoll_threadpool::IOBuffer;
using epoll_threadpool::Notification;
using epoll_threadpool::TcpSocket;
using epoll_threadpool::TcpListenSocket;

using namespace std;
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define TCP_PROTO "tcp"
#define LOG_FILE "./server.log"
#define PID_FILE "//server.pid"

#define MAXEVENTS 64

void Sleep(unsigned int msec)
{
	timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}

unsigned long long	getTickCount64()
{
	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long uTick = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;
	return uTick;
}

void errout(const char *msg)
{
	if (msg)
	{
		printf("%s\n", msg);
	}
	exit(1);
}

char * get_curdir()
{
	static char buf[256] = { 0 };
	int count = readlink("/proc/self/exe", buf, sizeof(buf));
	char * p = strrchr(buf, '/');
	if (p != NULL)
	{
		*(p + 1) = 0;
	}
	return buf;
}

char * get_exename()
{
	static char buf[256] = { 0 };
	int count = readlink("/proc/self/exe", buf, sizeof(buf));
	return buf;
}

void daemonize()
{
	char szCurDir[256] = { 0 };
	getcwd(szCurDir, sizeof(szCurDir));

	pid_t pid;
	if ((pid = fork()) < 0)
	{
		errout("fork error\n");
	}
	else if (pid != 0)
	{
		exit(0);
	}
	setsid();

	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGHUP, &sig, NULL);

	chdir(szCurDir);
}

static int check_pid(const char *pidfile)
{
	char pidfilebuf[256] = { 0 };
	int count = readlink("/proc/self/exe", pidfilebuf, sizeof(pidfilebuf));
	char * p = strrchr(pidfilebuf, '/');
	if (p != NULL)
	{
		*(p + 1) = 0;
	}
	unsigned int len = strlen(pidfilebuf);
	sprintf(pidfilebuf + len, "%s", pidfile);

	int pid = 0;
	FILE *f = fopen((const char *)pidfilebuf, "r");
	if (f == NULL)
	{
		return 0;
	}
	int n = fscanf(f, "%d", &pid);
	fclose(f);
	if (n != 1 || pid == 0 || pid == getpid())
	{
		return 0;
	}
	if (kill(pid, 0) && errno == ESRCH)
	{
		return 0;
	}
	return pid;
}

static int write_pid(const char *pidfile)
{
	char pidfilebuf[256] = { 0 };
	int count = readlink("/proc/self/exe", pidfilebuf, sizeof(pidfilebuf));
	char * p = strrchr(pidfilebuf, '/');
	if (p != NULL)
	{
		*(p + 1) = 0;
	}
	unsigned int len = strlen(pidfilebuf);
	sprintf(pidfilebuf + len, "%s", pidfile);

	FILE *f;
	int pid = 0;
	int fd = open((const char *)pidfilebuf, O_RDWR | O_CREAT, 0644);
	if (fd == -1)
	{
		fprintf(stderr, "Can't create pidfile [%s].\n", pidfile);
		return 0;
	}
	f = fdopen(fd, "r+");
	if (f == NULL)
	{
		fprintf(stderr, "Can't open pidfile [%s].\n", pidfile);
		return 0;
	}

	if (flock(fd, LOCK_EX | LOCK_NB) == -1)
	{
		int n = fscanf(f, "%d", &pid);
		fclose(f);
		if (n != 1)
		{
			fprintf(stderr, "Can't lock and read pidfile.\n");
		}
		else
		{
			fprintf(stderr, "Can't lock pidfile, lock is held by pid %d.\n", pid);
		}
		return 0;
	}
	pid = getpid();
	if (!fprintf(f, "%d\n", pid))
	{
		fprintf(stderr, "Can't write pid.\n");
		close(fd);
		return 0;
	}
	fflush(f);

	return pid;
}

shared_ptr<TcpListenSocket> createListenSocket(EventManager *em, int &port)
{
	shared_ptr<TcpListenSocket> s;
	while (!s)
	{
		s = TcpListenSocket::create(em, port);
	}
	return s;
}

static map<int, shared_ptr<TcpSocket> > sockets;

void OnReceiveCallback(IOBuffer* iobuf)
{
	printf("OnReceiveCallback - size:%d\n", iobuf->size());
	iobuf->print_data();

	map<int, shared_ptr<TcpSocket> >::iterator it_socket = sockets.begin();
	for (; it_socket != sockets.end(); it_socket++)
	{
		printf("it_socket.first:%d\n", it_socket->first);
		shared_ptr<TcpSocket> socket = it_socket->second;
		socket->write(iobuf);
	}
}
void OnDisconnectCallback()
{
	//sockets.erase(fd());
	printf("socket disconnect ...\n");
}
void OnAcceptCallback(shared_ptr<TcpSocket> socket)
{
	printf("accent socket - fd:%d\n", socket->fd());
	if (sockets.find(socket->fd()) == sockets.end())
	{
		socket->setReceiveCallback(OnReceiveCallback);
		socket->setDisconnectCallback(OnDisconnectCallback);
		socket->start();
		sockets[socket->fd()] = socket;
	}
	else
	{
		//sockets[socket->fd()] = socket;
	}
}

int main(int argc, char *argv[])
{
	printf("hello server!\n");

	//daemonize();

	//int pid = check_pid(PID_FILE);
	//if (pid)
	//{
	//	fprintf(stderr, "server is already running, pid = %d.\n", pid);
	//	return 1;
	//}
	//pid = write_pid(PID_FILE);
	//if (pid == 0)
	//{
	//	return 1;
	//}

	// ---

	EventManager em;
	em.start(4);
	int port = 60933;
	shared_ptr<TcpListenSocket> pListenSocket = createListenSocket(&em, port);
	assert(pListenSocket != NULL);
	int fd = pListenSocket->fd();
	printf("server listen success.port:%d,fd:%d!\n", port, fd);
	
	pListenSocket->setAcceptCallback(OnAcceptCallback);

	// ---

	unsigned long long startTime = 0;
	unsigned long long endTime = 0;
	unsigned long long sleepTime = 0;
	while (true)
	{
		startTime = getTickCount64();
		
		// doing some

		endTime = getTickCount64();
		sleepTime = endTime - startTime;
		if (sleepTime > (100 - 10))
		{
			sleepTime = 10;
		}
		else
		{
			sleepTime = 100 - sleepTime;
		}
		Sleep(sleepTime);
	}

	printf("server shutdown ...\n");

	return 0;
}


#include <functional>
#include <map>

#include <string.h>
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


#include "websocket.h"

#define TCP_PROTO "tcp"
#define LOG_FILE "./client.log"
#define PID_FILE "//client.pid"

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


int main(int argc, char *argv[])
{
	printf("hello client!\n");

	//daemonize();
	//int pid = check_pid(PID_FILE);
	//if (pid)
	//{
	//	fprintf(stderr, "client is already running, pid = %d.\n", pid);
	//	return 1;
	//}
	//pid = write_pid(PID_FILE);
	//if (pid == 0)
	//{
	//	return 1;
	//}

	// ---

	int ret = WebConnect();


	// ---

	unsigned long long startTime = 0;
	unsigned long long endTime = 0;
	unsigned long long sleepTime = 0;
	int i = 0;
	while (true)
	{
		startTime = getTickCount64();
		
		// doing some
		
		char data[256] = { 0 };
		sprintf(data, "hello world,i=%d.", i++);
		int len = strlen(data);
		//printf("clinet write - len:%d, data:%s\n", len, data);
		
		endTime = getTickCount64();
		sleepTime = endTime - startTime;
		if (sleepTime > (1000 - 10))
		{
			sleepTime = 10;
		}
		else
		{
			sleepTime = 1000 - sleepTime;
		}
		Sleep(sleepTime);
	}

	printf("client shutdown ...\n");

	return 0;
}


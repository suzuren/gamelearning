
#ifndef __NETWORK_DAEMONIZE_H_
#define __NETWORK_DAEMONIZE_H_

#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/resource.h>

#include "network_mgr.h"

int GenCoreDumpFile(size_t size = 1024 * 1024 * 32)
{
	struct rlimit flimit;
	flimit.rlim_cur = size;
	flimit.rlim_max = size;
	if (setrlimit(RLIMIT_CORE, &flimit) != 0)
	{
		return errno;
	}
	return 0;
}

void daemonize()
{
	pid_t pid;
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	umask(0);
	if ((pid = fork()) < 0)
	{
		printf("fork error\n");
	}
	else if (pid != 0)
	{
		exit(0);
	}
	setsid();
	chdir("/");
	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));
}

int check_pid(const char *pidfile)
{
	char pidfilebuf[256] = { 0 };
	int count = readlink("/proc/self/exe", pidfilebuf, sizeof(pidfilebuf));
	if (count == -1)
	{
		return 0;
	}
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

int write_pid(const char *pidfile)
{
	char pidfilebuf[256] = { 0 };
	int count = readlink("/proc/self/exe", pidfilebuf, sizeof(pidfilebuf));
	if (count == -1)
	{
		return 0;
	}
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

void reload(int signal)
{
	if (signal == SIGUSR2)
	{
		printf("reload config ... \n");
		CNetworkMgr::Instance().TestNetwork();
	}
}

static std::atomic_bool g_run(true);

void shutdown(int signal)
{
	if (signal == SIGUSR1)
	{
		printf("shutdown ... \n");
		g_run = std::move(false);
	}
}


#endif



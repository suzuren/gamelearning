
#include <stdio.h>

#include "logger.h"

#include <signal.h>
#include <sys/file.h>
#include <sys/resource.h>

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

static int check_pid(const char *pidfile)
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

static int write_pid(const char *pidfile)
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

int main(int argc, const char** argv)
{
	printf("test logger ...\n");
	daemonize();
	int pid = check_pid("logger.pid");
	if (pid)
	{
		fprintf(stderr, "logger is already running, pid = %d.\n", pid);
		return 1;
	}
	pid = write_pid("logger.pid");
	if (pid == 0)
	{
		return 1;
	}
	log_constructor_logger("server_data");
	int i = 0;
	//for (; i < 100; i++)
	//{
	//	//LOG_DEBUG("hello world - %d.", i);
	//}
	while (1)
	{
		log_time_update_logger();

		LOG_DEBUG("hello world - %d.", i++);
		//log_thread_sleep(LOG_BREATHING_SPACE);
		if (i == 99999999)
		{
			//break;
		}
		log_thread_sleep(3);
	}	
	log_shutdown_logger();
	log_thread_sleep(LOG_BREATHING_SPACE * 10);
	return 0;
}


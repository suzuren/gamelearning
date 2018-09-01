

#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include "skynet_daemon.h"

#define THREAD_WORKER 0
#define THREAD_MAIN 1
#define THREAD_SOCKET 2
#define THREAD_TIMER 3
#define THREAD_MONITOR 4

struct skynet_node {
	int total;
	int init;
	uint32_t monitor_exit;
	pthread_key_t handle_key;
	bool profile;	// default is off
};

static struct skynet_node G_NODE;

//--------------------------------------------------------------------------

void
skynet_initthread(int m) {
	uintptr_t v = (uint32_t)(-m);
	pthread_setspecific(G_NODE.handle_key, (void *)v);
}

void 
skynet_globalinit(void) {
	G_NODE.total = 0;
	G_NODE.monitor_exit = 0;
	G_NODE.init = 1;
	if (pthread_key_create(&G_NODE.handle_key, NULL)) {
		fprintf(stderr, "pthread_key_create failed");
		exit(1);
	}
	// set mainthread's key
	skynet_initthread(THREAD_MAIN);
}

//--------------------------------------------------------------------------

static void
create_thread(pthread_t *thread, void *(*start_routine) (void *), void *arg)
{
	if (pthread_create(thread,NULL, start_routine, arg)) {
		fprintf(stderr, "Create thread failed");
		exit(1);
	}
}


//--------------------------------------------------------------------------


static void *
thread_worker(void *p) {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int sleep = 100;
	int quit = 0;

	skynet_initthread(THREAD_WORKER);
	
	while (!quit)
    {
		if (pthread_mutex_lock(&mutex) == 0)
        {
			++ sleep;
			if (!quit)
            {
                pthread_cond_wait(&cond, &mutex);
            }
			-- sleep;
			if (pthread_mutex_unlock(&mutex))
            {
				fprintf(stderr, "unlock mutex error");
				exit(1);
			}
        }
	}
	return NULL;
}

static void
start(int thread) {
	pthread_t pid[thread];

	for (int i=0;i<thread;i++) {
		create_thread(&pid[i], thread_worker, NULL);
	}

	for (int i=0;i<thread;i++) {
		pthread_join(pid[i], NULL); 
	}
}


//--------------------------------------------------------------------------

int main(int argc, char *argv[])
 {
     printf("test skynet_daemon \n");

    const char * pidfile = "skynet.pid";

	if (pidfile)
    {
		if (daemon_init(pidfile))
        {
            printf("test daemon failed.\n");
			exit(1);
		}
	}

    start(8);

	if (pidfile)
    {
		daemon_exit(pidfile);
	}

    printf("test daemon success.\n");

     return 0;
 }
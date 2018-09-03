#ifndef SKYNET_CONTEXT_HANDLE_H
#define SKYNET_CONTEXT_HANDLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

//--------------------------------------------------------------------------
#define skynet_malloc malloc
#define skynet_free	  free

// reserve high 8 bits for remote id
#define HANDLE_MASK 0xffffff
#define HANDLE_REMOTE_SHIFT 24

struct skynet_context;

typedef int(*skynet_cb)(struct skynet_context * context, void *ud, int type, int session, uint32_t source, const void * msg, size_t sz);

typedef void * (*skynet_dl_create)(void);
typedef int(*skynet_dl_init)(void * inst, struct skynet_context *, const char * parm);
typedef void(*skynet_dl_release)(void * inst);
typedef void(*skynet_dl_signal)(void * inst, int signal);

struct skynet_module {
	const char * name;
	void * module;
	skynet_dl_create create;
	skynet_dl_init init;
	skynet_dl_release release;
	skynet_dl_signal signal;
};

struct skynet_message {
	uint32_t source;
	int session;
	void * data;
	size_t sz;
};

struct skynet_context {
	void * instance;
	struct skynet_module * mod;
	void * cb_ud;
	skynet_cb cb;
	struct message_queue *queue;
	FILE * logfile;
	uint64_t cpu_cost;	// in microsec
	uint64_t cpu_start;	// in microsec
	char result[32];
	uint32_t handle;
	int session_id;
	int ref;
	int message_count;
	bool init;
	bool endless;
	bool profile;
	//CHECKCALLING_DECL
};

//--------------------------------------------------------------------------




// reserve high 8 bits for remote id
#define HANDLE_MASK 0xffffff
#define HANDLE_REMOTE_SHIFT 24

//struct skynet_context;

uint32_t skynet_handle_register(struct skynet_context *);
int skynet_handle_retire(uint32_t handle); // handle 服务退休
struct skynet_context * skynet_handle_grab(uint32_t handle); // 通过 handle 抢占 context 服务
void skynet_handle_retireall(); // 退休全部服务

uint32_t skynet_handle_findname(const char * name);
const char * skynet_handle_namehandle(uint32_t handle, const char *name);

void skynet_handle_init(int harbor);

#endif

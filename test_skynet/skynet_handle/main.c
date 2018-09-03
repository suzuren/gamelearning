
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>

#include "rwlock.h"

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


#define DEFAULT_SLOT_SIZE 4
#define MAX_SLOT_SIZE 0x40000000

struct handle_name
{
	char * name;
	uint32_t handle;
};

struct handle_storage {
	struct rwlock lock;

	uint32_t harbor;
	uint32_t handle_index;
	int slot_size;
	struct skynet_context ** slot;

	int name_cap;
	int name_count;
	struct handle_name *name;
};

static struct handle_storage *H = NULL;


//--------------------------------------------------------------------------

void
skynet_handle_init(int harbor) {
	assert(H == NULL);
	struct handle_storage * s = skynet_malloc(sizeof(*H));
	s->slot_size = DEFAULT_SLOT_SIZE;
	s->slot = skynet_malloc(s->slot_size * sizeof(struct skynet_context *));
	memset(s->slot, 0, s->slot_size * sizeof(struct skynet_context *));

	rwlock_init(&s->lock);
	// reserve 0 for system
	s->harbor = (uint32_t)(harbor & 0xff) << HANDLE_REMOTE_SHIFT;
	s->handle_index = 1;
	s->name_cap = 2;
	s->name_count = 0;
	s->name = skynet_malloc(s->name_cap * sizeof(struct handle_name));

	H = s;

	// Don't need to free H
}


uint32_t
skynet_context_handle(struct skynet_context *ctx) {
	return ctx->handle;
}

uint32_t
skynet_handle_register(struct skynet_context *ctx) {
	struct handle_storage *s = H;

	rwlock_wlock(&s->lock);

	for (;;) {
		int i;
		for (i = 0; i<s->slot_size; i++) {
			uint32_t handle = (i + s->handle_index) & HANDLE_MASK;
			int hash = handle & (s->slot_size - 1);
			if (s->slot[hash] == NULL) {
				s->slot[hash] = ctx;
				s->handle_index = handle + 1;

				rwlock_wunlock(&s->lock);

				handle |= s->harbor;
				return handle;
			}
		}
		assert((s->slot_size * 2 - 1) <= HANDLE_MASK);
		struct skynet_context ** new_slot = skynet_malloc(s->slot_size * 2 * sizeof(struct skynet_context *));
		memset(new_slot, 0, s->slot_size * 2 * sizeof(struct skynet_context *));
		for (i = 0; i<s->slot_size; i++) {
			int hash = skynet_context_handle(s->slot[i]) & (s->slot_size * 2 - 1);
			assert(new_slot[hash] == NULL);
			new_slot[hash] = s->slot[i];
		}
		skynet_free(s->slot);
		s->slot = new_slot;
		s->slot_size *= 2;
	}
}


//--------------------------------------------------------------------------


uint32_t
skynet_handle_findname(const char * name) {
	struct handle_storage *s = H;

	rwlock_rlock(&s->lock);

	uint32_t handle = 0;

	int begin = 0;
	int end = s->name_count - 1;
	while (begin <= end) {
		int mid = (begin + end) / 2;
		struct handle_name *n = &s->name[mid];
		int c = strcmp(n->name, name);
		if (c == 0) {
			handle = n->handle;
			break;
		}
		if (c<0) {
			begin = mid + 1;
		}
		else {
			end = mid - 1;
		}
	}

	rwlock_runlock(&s->lock);

	return handle;
}


//--------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    printf("test skynet handle.\n");

	int harbor = 1;
	skynet_handle_init(harbor);


	int profile = 1;
	void *inst = NULL;
	struct skynet_module * mod = NULL;
	struct skynet_context * ctx = skynet_malloc(sizeof(*ctx));
	//CHECKCALLING_INIT(ctx)

	ctx->mod = mod;
	ctx->instance = inst;
	ctx->ref = 2;
	ctx->cb = NULL;
	ctx->cb_ud = NULL;
	ctx->session_id = 0;
	ctx->logfile = NULL;

	ctx->init = false;
	ctx->endless = false;

	ctx->cpu_cost = 0;
	ctx->cpu_start = 0;
	ctx->message_count = 0;
	ctx->profile = profile;
	// Should set to 0 first to avoid skynet_handle_retireall get an uninitialized handle
	ctx->handle = 0;
	ctx->handle = skynet_handle_register(ctx);

	printf("skynet_handle_register - handle:%d\n", ctx->handle);

	uint32_t handle = skynet_handle_findname("");

	printf("skynet_handle_findname - handle:%d\n", handle);


     return 0;
}


//--------------------------------------------------------------------------
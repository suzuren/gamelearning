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


/*

// 高八位是 harbor id 使用? 低 24位 是 handle index 使用 ，无符号整形表示
?HANDLE_MASK = 0xffffff == 0000000 11111111 11111111 11111111??

所以 s->harbor = (uint32_t) (harbor & 0xff) << HANDLE_REMOTE_SHIFT;

最新的代码 handle id 算法 
if (handle > HANDLE_MASK)
{
	// 0 is reserved
	handle = 1;
}

handle |= s->harbor;

所以handle 是 根据 1 开始 最高为 0xffffff 高八位是表示 harbor

老代码的handleid 算法 
uint32_t handle = (i+s->handle_index) & HANDLE_MASK;
直接使用掩码 去除高八位 那时候没有循环


在这个 组播服务中
multicastd.lua

local harbor_id = skynet.harbor(skynet.self())
local channel_id = harbor_id
-- new LOCAL channel , The low 8bit is the same with harbor_id


static int mc_nextid(lua_State *L)
{
	uint32_t id = (uint32_t)luaL_checkinteger(L, 1);
	id += 256;
	lua_pushinteger(L, (uint32_t)id);

	return 1;
}


所以说 新的 channel id 频道id的低八位都是 harbor_id，每次加 256 = ?0001 00000000?
256的低八位都是0，所以不会改变 harbor_id


*/


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

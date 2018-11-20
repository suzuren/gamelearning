

#include <lua.h>
#include <lauxlib.h>


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define skynet_malloc malloc
#define skynet_free	  free

//--------------------------------------------------------------------------


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


static int l_new(lua_State *L) {
	struct skynet_context * ctx = skynet_malloc(sizeof(struct skynet_context));
	memset(ctx, 0, sizeof(struct skynet_context));
	printf("l_new function - ctx_0:%p\n", ctx);
	lua_pushlightuserdata(L, (void*)ctx);
	lua_setfield(L, LUA_REGISTRYINDEX, "skynet_context");
	return 1;
}

static int l_init(lua_State *L)
{
	//if (!lua_isuserdata(L, 1))
	//{
	//	return luaL_error(L, "not an userdata");
	//}
	struct skynet_context* ctx = (struct skynet_context*)lua_touserdata(L, 1);
	printf("l_init function - ctx_0:%p\n", ctx);
	
	return 1;
}


static int l_getf(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");
	struct skynet_context* ctx = (struct skynet_context*)lua_touserdata(L, -1);
	printf("l_getf function - ctx_0:%p\n", ctx);


	return 1;
}



//--------------------------------------------------------------------------

static int l_setf(lua_State *L)
{
	//if (!lua_isuserdata(L, 1))
	//{
	//	return luaL_error(L, "not an userdata");
	//}
	struct skynet_context* ctx = (struct skynet_context*)lua_touserdata(L, 1);
	printf("l_setf function - ctx_1:%p\n", ctx);
	lua_setfield(L, LUA_REGISTRYINDEX, "skynet_context");

	return 1;
}

static int l_getv(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");
	struct skynet_context *ctx = lua_touserdata(L, lua_upvalueindex(1));
	printf("l_getv function - ctx_1:%p\n", ctx);


	return 1;
}

static int l_getv_2(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");
	struct skynet_context *ctx = lua_touserdata(L, lua_upvalueindex(1));
	printf("l_getv_2 function - ctx_2:%p\n", ctx);


	return 1;
}
//--------------------------------------------------------------------------


static int add(lua_State *L) {
	int op1 = luaL_checknumber(L, 1);
	int op2 = luaL_checknumber(L, 2);
	//将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
	lua_pushnumber(L, op1 + op2);
	//返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
	return 1;
}

static int add_with_upvalue(lua_State *L) {
	int k = lua_tonumber(L, lua_upvalueindex(1));
	const char* another_upvalue = lua_tostring(L, lua_upvalueindex(2));
	printf("one k:%d, second upvalue is %s\n", k, another_upvalue);

	int op1 = luaL_checknumber(L, 1);
	int op2 = luaL_checknumber(L, 2);
	//将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
	lua_pushnumber(L, op1 + op2 + k);
	//返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
	return 1;
}

static const luaL_Reg no_upvalue_func[] = {
	{ "add", add },
	{ "lnew", l_new },
	{ "linit", l_init },
	{ "lgetf", l_getf },
	{ 0, 0 }
};

static const luaL_Reg with_upvalue_func[] = {
	{ "add_with_upvalue", add_with_upvalue },
	{ 0, 0 }
};

static const luaL_Reg with_upvalue_func_1[] = {
	{ "lsetf", l_setf },
	{ "lgetv", l_getv },
	{ 0, 0 }
};

static const luaL_Reg with_upvalue_func_2[] = {
	{ "lgetv2", l_getv_2 },
	{ 0, 0 }
};

//--------------------------------------------------------------------------


LUALIB_API int luaopen_testupvalue(lua_State * L) {
	lua_newtable(L);
	//register function no upvalue
	luaL_setfuncs(L, no_upvalue_func, 0);
	
	struct skynet_context * ctx = skynet_malloc(sizeof(struct skynet_context));
	printf("luaopen function - ctx_1:%p\n", ctx);
	memset(ctx, 0, sizeof(struct skynet_context));
	lua_pushlightuserdata(L, ctx);
	luaL_setfuncs(L, with_upvalue_func_1, 1);

	struct skynet_context * ctx_2 = skynet_malloc(sizeof(struct skynet_context));
	printf("luaopen function - ctx_2:%p\n", ctx_2);
	memset(ctx_2, 0, sizeof(struct skynet_context));
	lua_setfield(L, LUA_REGISTRYINDEX, "skynet_context_2");
	lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context_2");
	lua_pushlightuserdata(L, ctx_2);
	luaL_setfuncs(L, with_upvalue_func_2, 1);

	//set two upvalue
	lua_pushnumber(L, 103);
	lua_pushstring(L, "i am upvalue");
	//register function with two upvalue
	//push了两个upvalue值所以第三个参数是2
	luaL_setfuncs(L, with_upvalue_func, 2);
	return 1;
}
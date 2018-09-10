#define LUA_LIB

//#include "skynet.h"
#include "lua-seri.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"

#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include <time.h>

#if defined(__APPLE__)
#include <sys/time.h>
#endif

//#include "skynet.h"


#define skynet_malloc malloc
#define skynet_free   free


// return nsec
static int64_t
get_time() {
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_MONOTONIC, &ti);
	return (int64_t)1000000000 * ti.tv_sec + ti.tv_nsec;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)1000000000 * tv.tv_sec + tv.tv_usec * 1000;
#endif
}


//struct snlua {
//	lua_State * L;
//	struct skynet_context * ctx;
//	const char * preload;
//};

//static int
//ltraceback (lua_State *L) {
//	const char *msg = lua_tostring(L, 1);
//	if (msg)
//		luaL_traceback(L, L, msg, 1);
//	else {
//		lua_pushliteral(L, "(no error message)");
//	}
//	return 1;
//}



//static const char *
//get_dest_string(lua_State *L, int index) {
//	const char * dest_string = lua_tostring(L, index);
//	if (dest_string == NULL) {
//		luaL_error(L, "dest address type (%s) must be a string or number.", lua_typename(L, lua_type(L,index)));
//	}
//	return dest_string;
//}



static int
ltostring(lua_State *L) {
	if (lua_isnoneornil(L,1)) {
		return 0;
	}
	char * msg = lua_touserdata(L,1);
	int sz = luaL_checkinteger(L,2);
	lua_pushlstring(L,msg,sz);
	return 1;
}

static int
lpackstring(lua_State *L) {
	luaseri_pack(L);
	char * str = (char *)lua_touserdata(L, -2);
	int sz = lua_tointeger(L, -1);
	lua_pushlstring(L, str, sz);
	skynet_free(str);
	return 1;
}

static int
ltrash(lua_State *L) {
	int t = lua_type(L,1);
	switch (t) {
	case LUA_TSTRING: {
		break;
	}
	case LUA_TLIGHTUSERDATA: {
		void * msg = lua_touserdata(L,1);
		luaL_checkinteger(L,2);
		skynet_free(msg);
		break;
	}
	default:
		luaL_error(L, "skynet.trash invalid param %s", lua_typename(L,t));
	}

	return 0;
}

static int
lhpc(lua_State *L) {
	lua_pushinteger(L, get_time());
	return 1;
}

#define MAX_LEVEL 3

struct source_info {
	const char * source;
	int line;
};


LUAMOD_API int
luaopen_skynet_core(lua_State *L) {

	luaL_checkversion(L);

	//luaL_Reg l[] = {
	//	{ "traceback", ltraceback },		
	//	{ NULL, NULL },
	//};

	// functions without skynet_context
	luaL_Reg l2[] = {
		{ "tostring", ltostring },
		{ "pack", luaseri_pack },
		{ "unpack", luaseri_unpack },
		{ "packstring", lpackstring },
		{ "trash" , ltrash },
		{ "hpc", lhpc },	// getHPCounter
		{ NULL, NULL },
	};

	//lua_newtable(L);

	//lua_createtable(L, 0, sizeof(l)/sizeof(l[0]) + sizeof(l2)/sizeof(l2[0]) -2);

	//lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");
	//struct skynet_context *ctx = lua_touserdata(L,-1);
	//if (ctx == NULL) {
	//	return luaL_error(L, "Init skynet context first");
	//}


	//luaL_setfuncs(L,l,1);
	//luaL_setfuncs(L,l2,0);

	luaL_newlib(L, l2);

	return 1;
}

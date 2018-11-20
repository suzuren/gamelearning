#include <stdlib.h>
#include <string.h>
#include <lua.hpp>

extern "C" {
	int luaopen_fuck(lua_State *L);
}

struct fuck {
	char *name;
	int  size;
};

int lnew(lua_State *L)
{
	struct fuck *asshole = (struct fuck*)calloc(1, sizeof(struct fuck));
	lua_pushlightuserdata(L, (void*)asshole);
	lua_setfield(L, LUA_REGISTRYINDEX, "asshole");
	return 0;
}

int linit(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "asshole");
	struct fuck *asshole = (struct fuck*)lua_touserdata(L, -1);
	if (!asshole) {
		lua_pop(L, 1);
		return luaL_error(L,"找不到屁股啊啊啊啊啊");
	}
	lua_pop(L, 1);

	size_t size;
	const char *name = lua_tolstring(L, 1, &size);
	asshole->name = (char*)malloc(size * sizeof(char));
	memcpy(asshole->name, name, size);
	asshole->size = size;
	return 0;
}

int lget(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "asshole");
	struct fuck *asshole = (struct fuck*)lua_touserdata(L, -1);
	if (!asshole) {
		lua_pop(L, 1);
		return luaL_error(L, "啊啊啊 屁股找不到拉");
	}
	lua_pop(L, 1);

	lua_pushlstring(L, asshole->name, asshole->size);
	return 1;
}

int luaopen_fuck(lua_State *L)
{
	static const luaL_Reg func[] = {
		{ "new", lnew },
		{ "init", linit },
		{ "get", lget },
		{ NULL, NULL },
	};
	luaL_newlib(L, func);
	return 1;
}

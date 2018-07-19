#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>



struct VariationInfo {
	int64_t score;
	int64_t insure;
};

struct user_info {
	int8_t isInit;
	int8_t isModified;

	struct VariationInfo variationInfo;

	// info
	uint32_t userID;
	char nickName[128];
	int64_t score;
	int64_t insure;

	// infoPlus
	int32_t userStatus;
	int8_t isAndroid;
};

void print_info_valve(struct user_info * pinfo)
{
	if (pinfo == NULL)
	{
		return;
	}
	printf("isInit:%d,isModified:%d\n", pinfo->isInit, pinfo->isInit);
	printf("variationInfo - score:%ld,insure:%ld\n", pinfo->variationInfo.score, pinfo->variationInfo.insure);
	printf("info - userID:%d,nickName:%s,score:%ld,insure:%ld\n", pinfo->userID, pinfo->nickName, pinfo->score, pinfo->insure);
	printf("plus - userStatus:%d,isAndroid:%d\n", pinfo->userStatus, pinfo->isAndroid);
}
static int l_new(lua_State *L) {
	struct user_info * pinfo = malloc(sizeof(struct user_info));
	memset(pinfo, 0, sizeof(struct user_info));
	lua_pushlightuserdata(L, (void*)pinfo);
	return 1;
}

static int l_reset(lua_State *L)
{
	if (lua_isuserdata(L, 1))
	{
		struct user_info* pinfo = (struct user_info*)lua_touserdata(L, 1);
		memset(pinfo, 0, sizeof(struct user_info));
	}
	return 0;
}


static void testSet(lua_State *L, struct user_info* p, const char * k)
{
	if (strcmp(k, "isModified") == 0)
	{
		p->isModified = lua_toboolean(L, -1);
	}
	else if (strcmp(k, "variationInfo") == 0)
	{
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			if (strcmp(lua_tostring(L, -2), "score") == 0)
				p->variationInfo.score = lua_tointeger(L, -1);
			else if (strcmp(lua_tostring(L, -2), "insure") == 0)
				p->variationInfo.insure = lua_tointeger(L, -1);	
			lua_pop(L, 1);
		}
	}

	else if (strcmp(k, "userID") == 0)
	{
		p->userID = lua_tointeger(L, -1);
	}
	else if (strcmp(k, "nickName") == 0)
	{
		strcpy(p->nickName, lua_tostring(L, -1));
	}
	else if (strcmp(k, "score") == 0)
	{
		p->score = lua_tointeger(L, -1);
	}
	else if (strcmp(k, "insure") == 0)
	{
		p->insure = lua_tointeger(L, -1);
	}

	else if (strcmp(k, "userStatus") == 0)
	{
		p->userStatus = lua_tointeger(L, -1);
	}
	else if (strcmp(k, "isAndroid") == 0)
	{
		p->isAndroid = lua_toboolean(L, -1);
	}

}

static int l_initialize(lua_State *L) {
	if (!lua_isuserdata(L, 1))
	{
		return luaL_error(L, "not an userdata");
	}
	struct user_info* pinfo = (struct user_info*)lua_touserdata(L, 1);
	if (pinfo->isInit)
		return luaL_error(L, "is already initialized");
	pinfo->isInit = 1;
	int index = lua_gettop(L);  // 取 table 索引值
	printf("initialize - nIndex:%d\n", index);
	lua_pushnil(L); // nil 入栈作为初始 key
	while (lua_next(L, 2) != 0)
	{
		// // 现在栈顶（-1）是 value，-2 位置是对应的 key
		//const  int  valve = lua_tointeger(L, -1);
		const char * key = lua_tostring(L, -2);
		printf("initialize - key:%s\n", key);
		testSet(L, pinfo, key);
		lua_pop(L, 1);
	}
	print_info_valve(pinfo);
	lua_pushnil(L);
	while (lua_next(L, 3) != 0) {
		//testSet(L, pinfo, lua_tostring(L, -2));
		lua_pop(L, 1);
	}
	return 0;
}

static int l_destroy(lua_State *L)
{
	if (lua_isuserdata(L, 1))
	{
		struct user_info* pinfo = (struct user_info*)lua_touserdata(L, 1);
		memset(pinfo, 0, sizeof(struct user_info));
		free(pinfo);
	}
	return 0;
}

int luaopen_struct(lua_State *L) {
	luaL_checkversion(L);

	luaL_Reg l[] = {
		//{ "addAttribute", l_addAttribute },
		//{ "setAttribute", l_setAttribute },
		//{ "getAttribute", l_getAttribute },
		//{ "writeUserScore", l_writeUserScore },
		//{ "distillVariation", l_distillVariation },
		{ "destroy", l_destroy },
		{ "initialize", l_initialize },
		{ "reset", l_reset },
		{ "new", l_new },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);

	return 1;
}

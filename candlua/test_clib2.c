#include <lua.h>
#include <lauxlib.h>

static int add(lua_State *L) {
	double op1 = luaL_checknumber(L, 1);
	double op2 = luaL_checknumber(L, 2);
	//将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
	lua_pushnumber(L, op1 + op2);
	//返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
	return 1;
}

static int add_with_upvalue(lua_State *L) {
	int k = lua_tonumber(L, lua_upvalueindex(1));
	const char* another_upvalue = lua_tostring(L, lua_upvalueindex(2));
	printf("second upvalue is %s\n", another_upvalue);

	double op1 = luaL_checknumber(L, 1);
	double op2 = luaL_checknumber(L, 2);
	//将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
	lua_pushnumber(L, op1 + op2 + k);
	//返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
	return 1;
}

static const luaL_Reg no_upvalue_func[] = {
	{ "add", add },
	{ 0, 0 }
};

static const luaL_Reg with_upvalue_func[] = {
	{ "add_with_upvalue", add_with_upvalue },
	{ 0, 0 }
};

// void luaL_setfuncs (lua_State L, const luaL_Reg l, int nup);
// Registers all functions in the array l(see luaL_Reg) into the table on the top of the stack(below optional upvalues, see next).
// When nup is not zero, all functions are created sharing nup upvalues, which must be previously pushed on the stack on top of the
// library table.These values are popped from the stack after the registration.

// 意思是第三个参数 nup 如果非零, 则所有通过luaL_setfuncs注册的函数都共享 nup个 upvalues. 这些 upvalues 必须在注册之前 pushed 到栈上.
// luaL_setfuncs 这个函数可以注册c函数到lua,另外还可以设置闭包函数使用的变量upvalue.

LUALIB_API int luaopen_testclibtwo(lua_State * L) {
	lua_newtable(L);
	/*register function no upvalue*/
	luaL_setfuncs(L, no_upvalue_func, 0);

	/*set two upvalue */
	lua_pushnumber(L, 100);
	lua_pushstring(L, "i am upvalue");
	/*register function with two upvalue*/
	/*push了两个upvalue值所以第三个参数是2*/
	luaL_setfuncs(L, with_upvalue_func, 2);
	return 1;
}
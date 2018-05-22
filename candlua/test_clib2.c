#include <lua.h>
#include <lauxlib.h>

static int add(lua_State *L) {
	double op1 = luaL_checknumber(L, 1);
	double op2 = luaL_checknumber(L, 2);
	//�������Ľ��ѹ��ջ�С�����ж������ֵ��������������ѹ��ջ�С�
	lua_pushnumber(L, op1 + op2);
	//����ֵ������ʾ��C�����ķ���ֵ��������ѹ��ջ�еķ���ֵ������
	return 1;
}

static int add_with_upvalue(lua_State *L) {
	int k = lua_tonumber(L, lua_upvalueindex(1));
	const char* another_upvalue = lua_tostring(L, lua_upvalueindex(2));
	printf("second upvalue is %s\n", another_upvalue);

	double op1 = luaL_checknumber(L, 1);
	double op2 = luaL_checknumber(L, 2);
	//�������Ľ��ѹ��ջ�С�����ж������ֵ��������������ѹ��ջ�С�
	lua_pushnumber(L, op1 + op2 + k);
	//����ֵ������ʾ��C�����ķ���ֵ��������ѹ��ջ�еķ���ֵ������
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

// ��˼�ǵ��������� nup �������, ������ͨ��luaL_setfuncsע��ĺ��������� nup�� upvalues. ��Щ upvalues ������ע��֮ǰ pushed ��ջ��.
// luaL_setfuncs �����������ע��c������lua,���⻹�������ñհ�����ʹ�õı���upvalue.

LUALIB_API int luaopen_testclibtwo(lua_State * L) {
	lua_newtable(L);
	/*register function no upvalue*/
	luaL_setfuncs(L, no_upvalue_func, 0);

	/*set two upvalue */
	lua_pushnumber(L, 100);
	lua_pushstring(L, "i am upvalue");
	/*register function with two upvalue*/
	/*push������upvalueֵ���Ե�����������2*/
	luaL_setfuncs(L, with_upvalue_func, 2);
	return 1;
}
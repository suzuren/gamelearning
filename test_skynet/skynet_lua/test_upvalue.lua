
local testupvalue = require "testupvalue"

local ret = testupvalue.add(10,20)
print("testupvalue.add(10,20) - ", ret)

local ret = testupvalue.add_with_upvalue(10,20)
print("testupvalue.add_with_upvalue(10,20) - ",ret)


print("LUA_REGISTRYINDEX---------------------")

testupvalue.lnew()
testupvalue.linit()
testupvalue.lgetf()

print("lua_upvalueindex---------------------")
testupvalue.lsetf()
testupvalue.lgetv()

print("lua_upvalueindex_2---------------------")

testupvalue.lgetv2()








--[[

skynet��������� service_snlua.c�ļ���
	lua_pushlightuserdata(L, ctx);
	lua_setfield(L, LUA_REGISTRYINDEX, "skynet_context");

skynet��������� lua-skynet.c�ļ���
	lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");



	lua_pushlightuserdata(L, ctx);  ����� ���� ѹ�뵽 ջ��

lua_setfield(L, LUA_REGISTRYINDEX, "skynet_context");  t['skynet_context']  = ջ����ֵ

lua_getfield(L, LUA_REGISTRYINDEX, "skynet_context");   �� t['skynet_context'] ��ֵ ���뵽ջ��



------------------------------------------------------------------------------------------------------------------------
-- ִ�н��

[root@forest skynet_lua]# lua test_upvalue.lua 
luaopen function - ctx_1:0x14b63f0
luaopen function - ctx_2:0x14b65c0
testupvalue.add(10,20) - 	30.0
one k:103, second upvalue is i am upvalue
testupvalue.add_with_upvalue(10,20) - 	133.0
LUA_REGISTRYINDEX---------------------
l_new function - ctx_0:0x14b66e0
l_init function - ctx_0:(nil)
l_getf function - ctx_0:0x14b66e0
lua_upvalueindex---------------------
l_setf function - ctx_1:(nil)
l_getv function - ctx_1:0x14b63f0
lua_upvalueindex_2---------------------
l_getv_2 function - ctx_2:0x14b65c0



]]


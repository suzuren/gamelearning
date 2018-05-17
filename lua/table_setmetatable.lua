local printr = require "print_r"

local _a1 = {20, 1, key1 = "hello", key2 = "world", lang = "lua"}
print("the table _a1:")
for _,v in pairs(_a1) do
    print(v)
end

local _a2 = {
    key1 = "hello new",
    key2 = "world new"
}

print("\nthe old table _a2:")
for _,v in pairs(_a2) do
    print(v)
end

print("\na2的metatable:",getmetatable(_a2))
print("language:",_a2["lang"])

-- 关注函数及__index
setmetatable(_a2, {__index = _a1})

print("\nthe new table _a2:")
for _,v in pairs(_a2) do
    print(v)
end

print("\na2的metatable:",getmetatable(_a2))
print("\nlanguage:", _a2["lang"])

--数组是从1开始
print("_a2数组第一个元素:", _a2[1])

print("\n _a1 table:")
printr(_a1)
print("\n _a2 table:")
printr(_a2)

print("\n")

myTable = {key1 = "lua", key3 = "java"}

setmetatable(myTable, {
	__newindex = function(myTable, key, value)
	print(key .. " is not exists in myTable " .. value)
    end
})

myTable.key1 = "hello"
myTable.key2 = "world"    --给key2赋值会执行__newindex函数

print(myTable.key1, myTable.key2)




--[[
小贴士：

大家可以参考lua手册，metatable是被译作元表，Lua 中的每个值都可以用一个 metatable。这个 metatable 就是一个原始的 Lua table ，它用来定义原始值在特定操作下的行为。

一个 metatable 可以控制一个对象做数学运算操作、比较操作、连接操作、取长度操作、取下标操作时的行为，metatable 中还可以定义一个函数，让 userdata 作垃圾收集时调用它。对于这些操作，Lua 都将其关联上一个被称作事件的指定健。当 Lua 需要对一个值发起这些操作中的一个时，它会去检查值中 metatable 中是否有对应事件。如果有的话，键名对应的值（元方法）将控制 Lua 怎样做这个操作。

metatable通过其包含的函数来给所挂接的table定义一些特殊的操作，包括:

__add: 定义所挂接table的加法操作
__mul: 定义乘法操作
__div: 定义除法操作
__sub: 定义减法操作
__unm: 定义负操作, 即: -table的含义
__tostring: 定义当table作为tostring()函式之参数被呼叫时的行为(例如: print(table)时将呼叫tostring(table)作为输出结果)
__concat: 定义连接操作(".."运算符)
__index: 定义当table中不存在的key值被试图获取时的行为
__newindex: 定义在table中产生新key值时的行为

]]

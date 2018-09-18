
local printr = require "print_r"

local tempTable = {11,22,33}

print("for 1 3 -----------------------------------------------------")

for i=1, 3 do
	print(string.format("i:%d,table:%d",i,tempTable[i]))
end

print("for in pairs ------------------------------------------------")

for i, v in pairs(tempTable) do
	print(string.format("i:%d,table:%d",i,v))
end


print("protocalNo, protocalObj---------------------------------------")


local protocalNo, protocalObj = 20200, {rate=300}

print("protocalNo:",protocalNo)
printr(protocalObj)


print("-------------------------------------------------------------")

for i=-2, 1 do
	print("loop for - i: ",i)
end

print("-------------------------------------------------------------")


list = nil

list = {next = list, value = 10}
list = {next = list, value = 11}
list = {next = list, value = 12}
list = {next = list, value = 13}
list = {next = list, value = 14}
list = {next = list, value = 15}

local l = list
while l do
	print(l.value)
	l = l.next
end



print("-------------------------------------------------------------")

function ListNew ()
	return {first = 0, last = -1}
end

List = {}

function List.new ()
	return {first = 0, last = -1}
end

function List.pushleft (list, value)
	local first = list.first - 1
	list.first = first
	list[first] = value
end

function List.pushright (list, value)
	local last = list.last + 1
	list.last = last
	list[last] = value
end

function List.popleft (list)
	local first = list.first
	if first > list.last then error("list is empty") end
	local value = list[first]
	list[first] = nil -- to allow garbage collection
	list.first = first + 1
	return value
end

function List.popright (list)
	local last = list.last
	if list.first > last then error("list is empty") end
	local value = list[last]
	list[last] = nil -- to allow garbage collection
	list.last = last - 1
	return value
end


printr(List)


objList = ListNew()
List.pushleft (objList, 11)
List.pushleft (objList, 12)
List.pushleft (objList, 13)
List.pushright (objList, 14)
List.pushright (objList, 15)
List.popleft (objList)

printr(objList)


print("-------------------------------------------------------------")



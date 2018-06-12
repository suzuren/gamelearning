
local printr = require "print_r"
local wordFilter = require "wordfilter"
local filter = wordFilter.new()

local rows = {}
table.insert(rows, {ID=21,Word="劳教"})
table.insert(rows, {ID=32,Word="勃朗宁"})
table.insert(rows, {ID=43,Word="勃朗宁微型"})
table.insert(rows, {ID=55,Word="勒索"})
table.insert(rows, {ID=66,Word="動乱"})
table.insert(rows, {ID=18,Word="勾引"})
table.insert(rows, {ID=29,Word="包厢公主"})
table.insert(rows, {ID=12,Word="包夜"})
table.insert(rows, {ID=23,Word="包房公主"})

print("---------------------------------------------- addWord ------------------------------------------------------------------------------------------------------")

print("sensitive word table rows:")
--printr(rows)

for _, row in ipairs(rows) do
	print(string.format("ID:%d,Word:%s",row.ID,row.Word))
	wordFilter.addWord(filter, row.Word)
end

print("---------------------------------------------- hasMatch -----------------------------------------------------------------------------------------------------")

local sensitiveWord_1 = "勾引"
local ret_1 = wordFilter.hasMatch(filter,sensitiveWord_1)
print(string.format("1 - ret:%s,sensitiveWord:%s",ret_1,sensitiveWord_1))

local sensitiveWord_2 = "包房公主"
local ret_2 = wordFilter.hasMatch(filter,sensitiveWord_2)
print(string.format("2 - ret:%s,sensitiveWord:%s",ret_2,sensitiveWord_2))

local sensitiveWord_3 = "公主"
local ret_3 = wordFilter.hasMatch(filter,sensitiveWord_3)
print(string.format("3 - ret:%s,sensitiveWord:%s",ret_3,sensitiveWord_3))

print("---------------------------------------------- doFiltering --------------------------------------------------------------------------------------------------")

local checkMsg_1 = "找公主玩"
local retMsg_1 = wordFilter.doFiltering(filter,checkMsg_1)
print(string.format("1 - checkMsg:%s,retMsg:%s",checkMsg_1,retMsg_1))

local checkMsg_2 = "找包房公主玩"
local retMsg_2 = wordFilter.doFiltering(filter,checkMsg_2)
print(string.format("2 - checkMsg:%s,retMsg:%s",checkMsg_2,retMsg_2))

print("---------------------------------------------- copy ---------------------------------------------------------------------------------------------------------")

local filter_copy = wordFilter.copy(filter)

local sensitiveWord_copy = "勾引"
local ret_copy = wordFilter.hasMatch(filter_copy,sensitiveWord_copy)
print(string.format("copy - ret:%s,sensitiveWord:%s",ret_copy,sensitiveWord_copy))

local checkMsg_copy = "找包房公主玩"
local retMsg_copy = wordFilter.doFiltering(filter_copy,checkMsg_copy)
print(string.format("copy - checkMsg:%s,retMsg:%s",checkMsg_copy,retMsg_copy))


print("---------------------------------------------- destroy ------------------------------------------------------------------------------------------------------")

wordFilter.destroy(filter)
wordFilter.destroy(filter_copy)

local sensitiveWord_destroy= "勾引"
local ret_destroy_1= wordFilter.hasMatch(filter,sensitiveWord_destroy)
local ret_destroy_2= wordFilter.hasMatch(filter_copy,sensitiveWord_destroy)
print(string.format("destroy - ret_filter:%s,ret_filter_copy:%s,sensitiveWord:%s",ret_destroy_1,ret_destroy_2,sensitiveWord_destroy))


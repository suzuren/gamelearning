
local printr = require "print_r"

local args = {}
--for word in string.gmatch(..., "%S+") do
--	table.insert(args, word)
--end


local word = "bootstrap"
table.insert(args, word)
LUA_SERVICE = "./service/?.lua"
LUA_PATH = "./lualib/?.lua;./lualib/?/init.lua"
LUA_CPATH = "./luaclib/?.so"
SERVICE_NAME = args[1]

local main, pattern

local err = {}
for pat in string.gmatch(LUA_SERVICE, "([^;]+);*") do
	local filename = string.gsub(pat, "?", SERVICE_NAME)
	local f, msg = loadfile(filename)
	print("loader SERVICE_NAME gsub -- ",pat,  filename,  f,  msg)
	if not f then
		table.insert(err, msg)
	else
		pattern = pat
		main = f

		break
	end
end

if not main then
	error(table.concat(err, "\n"))
end

LUA_SERVICE = nil
package.path , LUA_PATH = LUA_PATH
package.cpath , LUA_CPATH = LUA_CPATH

local service_path = string.match(pattern, "(.*/)[^/?]+$")

if service_path then
	service_path = string.gsub(service_path, "?", args[1])
	package.path = service_path .. "?.lua;" .. package.path
	SERVICE_PATH = service_path
else
	local p = string.match(pattern, "(.*/).+$")
	SERVICE_PATH = p
end

if LUA_PRELOAD then
	local f = assert(loadfile(LUA_PRELOAD))
	f(table.unpack(args))
	LUA_PRELOAD = nil
end


print("----------------------------------------------------------------")
printr(args)
print("----------------------------------------------------------------")

local ret_sub = select(2, table.unpack(args))

print("select ret_sub - ",ret_sub)

main(ret_sub)

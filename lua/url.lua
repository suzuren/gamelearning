
local printr = require "print_r"

local url = {}

local function decode_func(c)
	return string.char(tonumber(c, 16))
end

local function decode(str)
	local str = str:gsub('+', ' ')
	return str:gsub("%%(..)", decode_func)
end

function url.parse(u)
	local path,query = u:match "([^?]*)%??(.*)"
	if path then
		path = decode(path)
	end
	return path, query
end

function url.parse_query(q)
	local r = {}
	for k,v in q:gmatch "(.-)=([^&]*)&?" do
		r[decode(k)] = decode(v)
	end
	return r
end

local path, query = url.parse("/interface")
print(string.format("path-%s-, query-%s-", path, query))

local post = url.parse_query("type=onlineQuery&uidlist=12345,67890")
print(string.format("post----------------"))
printr(post)
print(string.format("----------------"))

local post = url.parse_query("type=ping")
print(string.format("post----------------"))
printr(post)
print(string.format("----------------"))

return url

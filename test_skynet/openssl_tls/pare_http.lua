

local function check_protocol(host)
	local protocol = host:match("^[Hh][Tt][Tt][Pp][Ss]?://")
	if protocol then
		host = string.gsub(host, "^"..protocol, "")
		protocol = string.lower(protocol)
		if protocol == "https://" then
			return "https", host
		elseif protocol == "http://" then
			return "http", host
		else
			error(string.format("Invalid protocol: %s", protocol))
		end
	else
		return "http", host
	end
end
local tag_host = "https://tg.abc.cn"
local protocol, host = check_protocol(tag_host)
local hostname, port = host:match"([^:]+):?(%d*)$"
print(protocol)
print(host)
print(hostname)
print(port)
--[[
Êä³ö
https
tg.abc.cn
tg.abc.cn
]]

local skynet = require "skynet"

local message = "hello world-hello world-hello world"
local sz = #message
local csz = string.char(sz)
local bsz = string.byte(csz)
print("message -",message)
print("sz -",sz)
print("csz -",csz) -- 超过 31 才是可打印字符
print("bsz -",bsz)


local function pack_package(...)
	local message = skynet.packstring(...)
	local size = #message
	assert(size <= 255 , "too long")
	return string.char(size), message
end

local ret_size,ret_pack = pack_package(message)
print("ret_size,ret_pack -",ret_size,ret_pack)

local ret_unpack = skynet.unpack(ret_pack)
print("ret_unpack -",ret_unpack)

local sz = string.byte(sz)



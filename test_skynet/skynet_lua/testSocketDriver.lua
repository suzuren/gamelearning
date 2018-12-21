

local printr = require "print_r"
local driver = require "socketdriver"

local buffer_pool = {}	-- store all message buffer object

local newbuffer = driver.buffer()

local s = {
	id = 102,
	buffer = newbuffer,
	connected = false,
	connecting = true,
	read_required = false,
	co = false,
	callback = nil,
	protocol = "TCP",
}

for i=1 , 2 do

local size,data = driver.mallocdata()
print("driver.mallocdata() - size, data, newbuffer ",size, data,newbuffer)

local buffer_size = driver.push(s.buffer, buffer_pool, data, size)
print("driver.push() - buffer_size",buffer_size)

print("buffer_pool sta ---------------------------------------------")
printr(buffer_pool)
print("buffer_pool end ---------------------------------------------")

end

local ret_readall = driver.readall(s.buffer, buffer_pool)
print("driver.readall() - ret_readall ",ret_readall)


local ret_header= driver.header("hel")
print("driver.header() - ret_header ",ret_header)


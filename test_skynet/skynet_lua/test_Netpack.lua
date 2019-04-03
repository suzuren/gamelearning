
local skynet = require "skynet"
local netpack = require "skynet.netpack"
local printr = require "print_r"



local socket	-- listen socket
local queue		-- message queue
local maxclient	= 1024 -- max client
local client_number = 0
local CMD = setmetatable({}, { __gc = function() netpack.clear(queue) end })
local nodelay = false



local function dispatch_queue()
	local fd, msg, sz = netpack.pop(queue)
	if fd then
		-- may dispatch even the handler.message blocked
		-- If the handler.message never block, the queue should be empty, so only fork once and then exit.
		-- skynet.fork(dispatch_queue)
		-- dispatch_msg(fd, msg, sz)

		print("dispatch_msg 1 - ", fd, msg, sz)

		for fd, msg, sz in netpack.pop, queue do
			--dispatch_msg(fd, msg, sz)
			print("dispatch_msg 2 - ", fd, msg, sz)
		end
	end
end

print("----------------------------------------------------------------")

local MSG = {}

local function dispatch_msg(fd, msg, sz)
	print("lua - dispatch_msg - ",fd, msg, sz)
end

MSG.data = dispatch_msg

local function dispatch_queue()
		
	print("lua - dispatch_queue -")

	local fd, msg, sz = netpack.pop(queue)
	if fd then
		dispatch_msg(fd, msg, sz)
		for fd, msg, sz in netpack.pop, queue do
			dispatch_msg(fd, msg, sz)
		end
	end
end

MSG.more = dispatch_queue

function MSG.open(fd, msg)
	print("lua - MSG.open - ",fd, msg)
end

function MSG.close(fd)
	print("lua - MSG.close - ",fd)
end

function MSG.error(fd, msg)
	print("lua - MSG.error - ",fd, msg)
end

function MSG.warning(fd, size)
	print("lua - MSG.warning - ",fd, size)
end

printr(MSG)

print("----------------------------------------------------------------")

for i=1 , 1 do
	local msg, sz = netpack.newdata(i)
	print("msg, sz - ",msg, sz)
	local q, type, fd, msg, sz = netpack.filter( queue, msg, sz)
	print("lua - netpack.filter q, type, fd, msg, sz - ", q, type, fd, msg, sz)
	queue = q
	if type then
		MSG[type](fd, msg, sz)
	end
end

print("----------------------------------------------------------------")




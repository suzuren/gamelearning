
local skynet = require "skynet"
local netpack = require "skynet.netpack"



local socket	-- listen socket
local queue		-- message queue
local maxclient	-- max client
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
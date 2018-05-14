local gateserver = require "gateserver_ws"
local printr = require "print_r"
local connection = {} -- fd -> { fd , ip, uid（登录后有）game（登录后有）key（登录后有）}
local name = "" --gated1


local handler = {}

function handler.open(source, conf)
	print("start listen port: %d", conf.port)
	name = conf.name
end

function handler.connect(fd, addr)
	local c = {
		fd = fd,
		ip = addr,
		uid = nil,
		agent = nil,
	}
	connection[fd] = c
	gateserver.openclient(fd)
	print("New client from: ", addr, " fd: ", fd)
end

function handler.message(fd, msg, sz)
	local c = connection[fd]
	local uid = c.uid
	print("client message fd:", fd, " msg: ", msg," sz:",sz," uid:",uid)
end

local CMD = {}

--true/false
function CMD.register(source, data)
	local c = connection[data.fd]
	if not c then
		return false
	end
	
	c.uid = data.uid
	c.agent = data.agent
	c.key = data.key
	return true
end

local function close_agent(fd)
	local c = connection[fd]
	if c then
		connection[fd] = nil
		gateserver.closeclient(fd)
	end

	return true
end 


--true/false
function CMD.kick(source, fd)
	print("cmd.kick fd:", fd)
	return close_agent(fd)
end

function handler.disconnect(fd)
	print("handler.disconnect fd:", fd)
	return close_agent(fd)
end

function handler.error(fd, msg)
	print("handler.error:", msg)
	handler.disconnect(fd)
end

function handler.warning(fd, size)
	print("handler.warning fd:", fd, " size:", size)
end

function handler.command(cmd, source, ...)
	print("gate server handler command:", cmd)
	local f = assert(CMD[cmd])
	return f(source, ...)
end

print("gateserver.start ...")
print("handler function")
printr(handler)
print("CMD function")
printr(CMD)
gateserver.start(handler)



local c = require "skynet.core"

local skynet = {
	-- read skynet.h
	PTYPE_TEXT = 0,
	PTYPE_RESPONSE = 1,
	PTYPE_MULTICAST = 2,
	PTYPE_CLIENT = 3,
	PTYPE_SYSTEM = 4,
	PTYPE_HARBOR = 5,
	PTYPE_SOCKET = 6,
	PTYPE_ERROR = 7,
	PTYPE_QUEUE = 8,	-- used in deprecated mqueue, use skynet.queue instead
	PTYPE_DEBUG = 9,
	PTYPE_LUA = 10,
	PTYPE_SNAX = 11,
	PTYPE_TRACE = 12,	-- use for debug trace
}


skynet.pack = assert(c.pack)
skynet.packstring = assert(c.packstring)
skynet.unpack = assert(c.unpack)
skynet.tostring = assert(c.tostring)
skynet.trash = assert(c.trash)


return skynet


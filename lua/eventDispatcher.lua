
local printr = require "print_r"


local _eventType2handler = {}

local function isTableEmpty(t)
	local isEmpty = true
	for k,v in pairs(t) do
		isEmpty = false
		break
	end
	return isEmpty
end

local function cmd_addEventListener(eventType, address, method)
	if type(address)~="number" then
		address = 66666666
	end

	if type(_eventType2handler[eventType])~="table" then
		_eventType2handler[eventType] = {}
	end

	if type(_eventType2handler[eventType][address])~="table" then
		_eventType2handler[eventType][address] = {}
	end

	_eventType2handler[eventType][address][method] = true
end

local function cmd_removeEventListener(eventType, address, method)
	if _eventType2handler[eventType] and _eventType2handler[eventType][address] and _eventType2handler[eventType][address][method] then
		_eventType2handler[eventType][address][method] = nil
		if isTableEmpty(_eventType2handler[eventType][address]) then
			_eventType2handler[eventType][address] = nil
			if isTableEmpty(_eventType2handler[eventType]) then
				_eventType2handler[eventType] = nil
			end
		end
	end
end

local function cmd_dispatch(eventType, eventData)
	if eventType and _eventType2handler[eventType] then
		for address, methodTable in pairs(_eventType2handler[eventType]) do
			for method, _ in pairs(methodTable) do
				print("address, method, eventData",address, method, eventData)
				-- skynet.send(address, "lua", method, eventData)
			end
		end
	end
end

local	EVT_GS_CLIENT_DISCONNECT							= 0x10000000
local	EVT_GS_LOGIN_SUCCESS								= 0x10000001
local	EVT_GS_SIT_DOWN										= 0x10000002
local	EVT_GS_SERVER_REGISTER_SUCCESS						= 0x10000003

cmd_addEventListener(EVT_GS_SERVER_REGISTER_SUCCESS,1111,"onEventServerRegisterSuccess")
cmd_addEventListener(EVT_GS_SERVER_REGISTER_SUCCESS,1112,"onEventServerRegisterSuccess2")
cmd_addEventListener(EVT_GS_SERVER_REGISTER_SUCCESS,1113,"onEventServerRegisterSuccess3")
cmd_addEventListener(EVT_GS_LOGIN_SUCCESS,2222,"onEventLoginSuccess")
cmd_addEventListener(EVT_GS_SIT_DOWN,55555,"onEventSitDown")
cmd_addEventListener(EVT_GS_SIT_DOWN,55552,"onEventSitDown2")

print("cmd_addEventListener -----------------------------------------------------------")
printr(_eventType2handler)
print("cmd_dispatch -------------------------------------------------------------------")
cmd_dispatch(EVT_GS_SERVER_REGISTER_SUCCESS,"eventData")


--------------------------------------------------------------------------------------------------------------------------------------------------------------


--函数
function g_func(param)
	--print("param+1:",param+1)
end
 
--变量
local g_number = 1
 
--做事情
function doSM(num, fn)
    --传进来的变量g_number 
    local number = num
     
    --传进来的函数g_func
    fn(number)
end

 
--主函数
function main()
    --把变量和函数传入doSM函数
    doSM(g_number , g_func)
end

main()

--------------------------------------------------------------------------------------------------------------------------------------------------------------

local printr = require "print_r"


local proto = {}
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
}


function skynet.register_protocol(class)
	local name = class.name
	local id = class.id
	assert(proto[name] == nil and proto[id] == nil)
	assert(type(name) == "string" and type(id) == "number" and id >=0 and id <=255)
	proto[name] = class
	proto[id] = class
end



skynet.register_protocol ({
	name = "client",
	id = skynet.PTYPE_CLIENT,
})

printr(proto)

--------------------------------------------------------------------------------------------------------------------------------------------------------------








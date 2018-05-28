local printr = require "print_r"

--[[
local conf = {
	methods = {
		["cmd"] = {["func"]=functionReference, ["isRet"]=false},
	},
	initFunc = function() end,
}
--]]

local function createService(conf)

	if type(conf.initFunc)=="function" then
		conf.initFunc()
	end
	local cmd = "start"


	local methodItem = assert(conf.methods[cmd], string.format("%s: handler not found for \"%s\"", "common.lua", cmd))
	if methodItem.isRet then
		methodItem.func()
	end

	print("methodItem----------------------------------------------------------")
	printr(methodItem)
end

return {
	createService = createService,
}

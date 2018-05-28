
local printr = require "print_r"
local commonServiceHelper = require "common"

local function cmd_start()
	print("cmd_start func")
end

local function cmd_decode()
	print("cmd_decode func")
end

local function cmd_reverseDecode()
	print("cmd_reverseDecode func")
end

local function cmd_encode()
	print("cmd_encode func")
end

local conf = {
	methods = {
		["start"] = {["func"]=cmd_start, ["isRet"]=true},
		["decode"] = {["func"]=cmd_decode, ["isRet"]=true},
		["reverseDecode"] = {["func"]=cmd_reverseDecode, ["isRet"]=true},
		["encode"] = {["func"]=cmd_encode, ["isRet"]=true},
	},
	initFunc = function()
		print("initFunc = function()")
	end,
}

commonServiceHelper.createService(conf)


print("conf----------------------------------------------------------")
printr(conf)
print("commonServiceHelper-------------------------------------------")
printr(commonServiceHelper)


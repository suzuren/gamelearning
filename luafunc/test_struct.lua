
local printr = require "print_r"
local struct = require "struct"

local tagItem = struct.new()

print("tagItem - ",tagItem)

struct.reset(tagItem)


local info = {
	variationInfo = {	score = 300,insure = 100,},
	userID			= 1003,
	nickName		= "alice",
	score			= 300,
	insure			= 100,
}



local infoPlus = 
{
	userStatus		= 3,
	isAndroid		= true,
}

struct.initialize(tagItem,info, infoPlus)

struct.destroy()


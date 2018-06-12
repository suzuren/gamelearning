
local pbcc = require "pbcc"
local protobuf = require "protobuf"
local printr = require "print_r"



print("-- test packNetPacket 1 ---------------------------------------------------------------------------------------------------")


local loginSuccess = {
	userID = 1003,
	gameID = 3333,
	faceID = 5,
	gender = 1,
	nickName = "alice",
	isRegister = true,
	memberOrder = 7,
	signature = "great minds think alike.",
	platformFace = "http://www.qq.com/",
	isChangeNickName = false,
}

local pbFile = "loginServer.login.s2c.pb"
local typeName = "loginServer.login.s2c.Login"
local obj = {code="RC_IP_BANNED",msg = "hello world!",data = loginSuccess}
local protoNo = 0x000100
local returnString = true

protobuf.register_file(pbFile);

print(string.format("comme - pbFile:%s      typeName:%s",pbFile,typeName))


local msg, size = protobuf.encode(typeName, obj, pbcc.packNetPacket, protoNo, returnString)

print("pack protoNo:")
print(string.format("0x%06X",protoNo))
print("pack table:")
printr(obj);
print("pack size:")
print(size)	-- 22


print("-- test unpackNetPayload 1 ------------------------------------------------------------------------------------------------")

local pNo, pStr,ignore1,ignore2 = pbcc.unpackNetPayload(msg, size)

print(string.format("pNo:0x%06X, ignore1:%d,ignore2:%d",pNo, ignore1,ignore2))

local sz = 0
local pbObj  = protobuf.decode(typeName, pStr, sz)

print("unpack table:")
printr(pbObj);


--print("-- test unpackNetPayload 2 ------------------------------------------------------------------------------------------------")

local protocalNo = "0x010000"
local pNoStr = string.sub(protocalNo, 3)
local pLen = string.len(protocalNo)-2
--print(pNoStr,pLen)-- 010000	6
local pNo, pStr = pbcc.unpackNetPayload(pNoStr, pLen)
--print(pNo,pStr)
-- 3223600	00
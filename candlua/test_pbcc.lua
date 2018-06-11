
local pbcc = require "pbcc"
local protobuf = require "protobuf"
local printr = require "print_r"

print("-- test unpackNetPayload 1 ------------------------------------------------------------------------------------------------")

local protocalNo = "0x010000"

local pNoStr = string.sub(protocalNo, 3)
local pLen = string.len(protocalNo)-2
print(pNoStr,pLen)
-- 010000	6

local pNo, pStr = pbcc.unpackNetPayload(pNoStr, pLen)

print(pNo,pStr)
-- 3223600	00

print("-- test packNetPacket 1 ---------------------------------------------------------------------------------------------------")
local typeName = "loginServer.login.s2c.Login"
local obj = {code="RC_IP_BANNED"}
local protoNo = 0x000100
local returnString = true

protobuf.register_file("loginServer.login.s2c.pb");

local msg, size = protobuf.encode(typeName, obj, pbcc.packNetPacket, protoNo, returnString)

print(msg, size)


print("-- test unpackNetPayload 1 ------------------------------------------------------------------------------------------------")

local pNo, pStr,ignore1,ignore2 = pbcc.unpackNetPayload(msg, size)

local sz = 2
local pbObj  = protobuf.decode(typeName, pStr, sz)

print(pNo,ignore1,ignore2,pbObj)

print(string.format("pNo:0x%06X,pStr:%s",pNo,pbObj))

printr(pbObj)
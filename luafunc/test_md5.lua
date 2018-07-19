
local md5 = require "md5"

local str = "123456"
local pass = md5.sumhexa(str)
print("pass:",pass)
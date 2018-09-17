
local printr = require "print_r"

local tempTable = {11,22,33}

print("for 1 3 -----------------------------------------------------")

for i=1, 3 do
	print(string.format("i:%d,table:%d",i,tempTable[i]))
end

print("for in pairs ------------------------------------------------")

for i, v in pairs(tempTable) do
	print(string.format("i:%d,table:%d",i,v))
end


print("protocalNo, protocalObj---------------------------------------")


local protocalNo, protocalObj = 20200, {rate=300}

print("protocalNo:",protocalNo)
printr(protocalObj)


print("-------------------------------------------------------------")

for i=0, 10 do
	print("loop for - i: ",i)
end
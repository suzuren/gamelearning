
local arc4 = require( "arc4random" )

local maxloop = 10


print("arc4 - 0 - 3 -----------------------------------------------------")
for i = 1, maxloop do
	local rnumber = arc4.random(0, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math -  0 - 3 -----------------------------------------------------")
for i = 1, maxloop do
	local rnumber = math.random(0, 3)
	print(string.format("i:%02d",rnumber))
end

print("arc4 - 1 - 3 -----------------------------------------------------")
for i = 1, maxloop do
	local rnumber = arc4.random(1, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math - 1 - 3 ------------------------------------------------------")
for i = 1, maxloop do
	local rnumber = math.random(1, 3)
	print(string.format("i:%02d",rnumber))
end

print("arc4 - -3 - 3 -----------------------------------------------------")
for i = 1, maxloop do
	local rnumber = arc4.random(-3, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math - -3 - 3 ------------------------------------------------------")
for i = 1, maxloop do
	local rnumber = math.random(-3, 3)
	print(string.format("i:%02d",rnumber))
end



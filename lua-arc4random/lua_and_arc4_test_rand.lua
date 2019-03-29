
local arc4 = require( "arc4random" )
math.randomseed(os.time())

local maxloop = 30


print("arc4 - 0 - 3 ------------------------------------------------------")
-- rand 0 1 2 3
for i = 1, maxloop do
	local rnumber = arc4.random(0, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math -  0 - 3 -----------------------------------------------------")
-- rand 0 1 2 3
for i = 1, maxloop do
	local rnumber = math.random(0, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("arc4 - 1 - 3 ------------------------------------------------------")
-- rand 1 2 3
for i = 1, maxloop do
	local rnumber = arc4.random(1, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math - 1 - 3 ------------------------------------------------------")
-- rand 1 2 3
for i = 1, maxloop do
	local rnumber = math.random(1, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("arc4 - -3 - 3 -----------------------------------------------------")
-- rand -3 -2 -1 0 1 2 3
for i = 1, maxloop do
	local rnumber = arc4.random(-3, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end

print("math - -3 - 3 -----------------------------------------------------")
-- rand -3 -2 -1 0 1 2 3
for i = 1, maxloop do
	local rnumber = math.random(-3, 3)
	print(string.format("i:%02d,num:%d",i,rnumber))
end


print("arc4 - *100 -------------------------------------------------------")
-- rand -3 -2 -1 0 1 2 3
for i = 1, maxloop do
	local rnumber = arc4.random()*3
	print(string.format("i:%02d,num:%f",i,rnumber))
end

print("math - *100 -------------------------------------------------------")
-- rand -3 -2 -1 0 1 2 3
for i = 1, maxloop do
	local rnumber = math.random()*3
	print(string.format("i:%02d,num:%f",i,rnumber))
end


--[[
arc4 - 0 - 3 ------------------------------------------------------
i:01,num:2
i:02,num:1
i:03,num:2
i:04,num:2
i:05,num:0
i:06,num:2
i:07,num:3
i:08,num:2
i:09,num:1
i:10,num:3
i:11,num:0
i:12,num:0
i:13,num:0
i:14,num:2
i:15,num:2
i:16,num:1
i:17,num:1
i:18,num:2
i:19,num:0
i:20,num:2
i:21,num:3
i:22,num:0
i:23,num:1
i:24,num:1
i:25,num:0
i:26,num:0
i:27,num:3
i:28,num:2
i:29,num:1
i:30,num:0
math -  0 - 3 -----------------------------------------------------
i:01,num:3
i:02,num:1
i:03,num:3
i:04,num:3
i:05,num:3
i:06,num:0
i:07,num:1
i:08,num:3
i:09,num:1
i:10,num:2
i:11,num:1
i:12,num:2
i:13,num:1
i:14,num:2
i:15,num:3
i:16,num:3
i:17,num:2
i:18,num:2
i:19,num:0
i:20,num:2
i:21,num:0
i:22,num:0
i:23,num:0
i:24,num:3
i:25,num:0
i:26,num:1
i:27,num:0
i:28,num:0
i:29,num:3
i:30,num:0
arc4 - 1 - 3 ------------------------------------------------------
i:01,num:2
i:02,num:1
i:03,num:1
i:04,num:3
i:05,num:3
i:06,num:1
i:07,num:1
i:08,num:2
i:09,num:1
i:10,num:2
i:11,num:2
i:12,num:3
i:13,num:2
i:14,num:2
i:15,num:1
i:16,num:1
i:17,num:2
i:18,num:2
i:19,num:2
i:20,num:3
i:21,num:1
i:22,num:3
i:23,num:3
i:24,num:2
i:25,num:1
i:26,num:3
i:27,num:3
i:28,num:1
i:29,num:2
i:30,num:3
math - 1 - 3 ------------------------------------------------------
i:01,num:2
i:02,num:3
i:03,num:2
i:04,num:1
i:05,num:2
i:06,num:2
i:07,num:2
i:08,num:3
i:09,num:1
i:10,num:3
i:11,num:2
i:12,num:3
i:13,num:2
i:14,num:3
i:15,num:1
i:16,num:2
i:17,num:3
i:18,num:3
i:19,num:1
i:20,num:3
i:21,num:2
i:22,num:1
i:23,num:1
i:24,num:2
i:25,num:3
i:26,num:2
i:27,num:1
i:28,num:1
i:29,num:2
i:30,num:1
arc4 - -3 - 3 -----------------------------------------------------
i:01,num:-2
i:02,num:-2
i:03,num:-1
i:04,num:3
i:05,num:-1
i:06,num:3
i:07,num:-3
i:08,num:2
i:09,num:-1
i:10,num:-1
i:11,num:0
i:12,num:3
i:13,num:2
i:14,num:2
i:15,num:-3
i:16,num:1
i:17,num:1
i:18,num:3
i:19,num:-1
i:20,num:1
i:21,num:-2
i:22,num:-3
i:23,num:-2
i:24,num:1
i:25,num:0
i:26,num:2
i:27,num:0
i:28,num:-3
i:29,num:0
i:30,num:-2
math - -3 - 3 -----------------------------------------------------
i:01,num:-2
i:02,num:3
i:03,num:3
i:04,num:2
i:05,num:-2
i:06,num:0
i:07,num:-1
i:08,num:2
i:09,num:0
i:10,num:1
i:11,num:0
i:12,num:-3
i:13,num:0
i:14,num:3
i:15,num:3
i:16,num:2
i:17,num:-2
i:18,num:2
i:19,num:1
i:20,num:-1
i:21,num:1
i:22,num:-2
i:23,num:0
i:24,num:3
i:25,num:2
i:26,num:-1
i:27,num:-2
i:28,num:3
i:29,num:-1
i:30,num:1
arc4 - *100 -------------------------------------------------------
i:01,num:2.795003
i:02,num:2.395921
i:03,num:2.931273
i:04,num:2.106737
i:05,num:1.699068
i:06,num:0.030331
i:07,num:2.749696
i:08,num:1.955111
i:09,num:2.284358
i:10,num:1.443319
i:11,num:1.437281
i:12,num:0.793813
i:13,num:1.974803
i:14,num:1.280936
i:15,num:0.827271
i:16,num:1.901805
i:17,num:1.368240
i:18,num:1.102614
i:19,num:1.846234
i:20,num:0.967220
i:21,num:1.648038
i:22,num:2.522418
i:23,num:1.362223
i:24,num:1.772756
i:25,num:2.862916
i:26,num:2.877971
i:27,num:1.049434
i:28,num:0.384181
i:29,num:2.362304
i:30,num:1.949155
math - *100 -------------------------------------------------------
i:01,num:2.869405
i:02,num:1.765920
i:03,num:1.971912
i:04,num:2.576029
i:05,num:1.318680
i:06,num:2.771909
i:07,num:1.195310
i:08,num:2.444301
i:09,num:2.052656
i:10,num:2.732916
i:11,num:1.447472
i:12,num:0.647475
i:13,num:2.850757
i:14,num:2.760385
i:15,num:0.442980
i:16,num:2.643187
i:17,num:1.923242
i:18,num:1.295860
i:19,num:1.858789
i:20,num:0.843178
i:21,num:2.358006
i:22,num:0.922374
i:23,num:1.341101
i:24,num:0.678320
i:25,num:0.562599
i:26,num:0.828704
i:27,num:1.669331
i:28,num:1.249504
i:29,num:0.508821
i:30,num:2.720412

]]
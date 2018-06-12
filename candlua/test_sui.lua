
local printr = require "print_r"
local sui = require "sui"
local CItemBuffer = require "cItemBuffer"

local USER_STATUS = {
	US_NULL 			= 0x00,								--没有状态
	US_LS 				= 0x01,								--登录服务器
	US_GS 				= 0x02,								--游戏服务器
	US_GS_OFFLINE 		= 0x03,								--游戏掉线
	US_LS_GS 			= 0x04,								--登录在线，游戏在线
	US_LS_GS_OFFLINE 	= 0x05,								--登录在线，游戏掉线
}

local SCORE_TYPE = {
	ST_NULL						= 0x00,				--无效积分
	ST_WIN						= 0x01,				--胜局积分
	ST_LOSE						= 0x02,				--输局积分
	ST_DRAW						= 0x03,				--和局积分
	ST_FLEE						= 0x04,				--逃局积分
	ST_PRESENT					= 0x10,				--赠送积分
	ST_SERVICE					= 0x11,				--服务积分
}

CItemBuffer.init(sui)
local userInfo = {
	userID			= 1003,
	gameID			= 33333,
	platformID		= 11,
	nickName		= "alice",
	signature		= "hello world!",

	gender			= 1,
	faceID			= 2,
	platformFace	= "https://github.com",
	userRight		= 0x000001,
	masterRight		= 0x000002,
	memberOrder		= 0x000003,
	masterOrder		= 0x000004,
	score			= 300,
	insure			= 100,
	medal			= 10,
	gift			= 6,
	present			= 7,
	experience		= 65300,
	loveliness		= 9000,
	winCount		= 800,
	lostCount		= 700,
	drawCount		= 5,
	fleeCount		= 6,
	contribution	= 100,
	dbStatus		= 0x01,

	inoutIndex		= 10058,
}



local userInfoPlus = 
{
	logonTime		= 69875544,
	userStatus		= USER_STATUS.US_LS,
	isAndroid		= false,
	agent			= 0x589254,
	ipAddr			= "127.0.0.1",
	machineID		= "98ujdeyhnjuyhhjjjikkshd",
}

print("---------------------------------------------- userinfo ------------------------------------------------------------------------------------------------------")

print("table userInfo:");
printr(userInfo);


print("---------------------------------------------- userInfoPlus --------------------------------------------------------------------------------------------------")

print("table userInfoPlus:");
printr(userInfoPlus);

print("---------------------------------------------- getAttribute --------------------------------------------------------------------------------------------------")

local tempUserItem = CItemBuffer.allocate()
sui.initialize(tempUserItem, userInfo, userInfoPlus)

local userAttribute = sui.getAttribute(tempUserItem, {
			"userID", "gameID", "faceID", "gender", "nickName", "memberOrder", "signature", "platformFace",
			"medal", "experience", "loveliness", "score", "insure", "gift", "present", "contribution",
		})

printr(userAttribute)

print("---------------------------------------------- addAttribute --------------------------------------------------------------------------------------------------")

sui.addAttribute(tempUserItem, {score=30})
local userAttribute = sui.getAttribute(tempUserItem, {"score"})
printr(userAttribute)

print("---------------------------------------------- setAttribute --------------------------------------------------------------------------------------------------")

sui.setAttribute(tempUserItem, {score=30})
local userAttribute = sui.getAttribute(tempUserItem, {"score"})
printr(userAttribute)

print("---------------------------------------------- writeUserScore --------------------------------------------------------------------------------------------------")


sui.writeUserScore(tempUserItem, {
	score		=	50,
	insure		=	60,
	grade		=	70,
	revenue		=	3,
	medal		=	8,
	gift		=	88,
	present		=	87,
	loveliness	=	63,
		}, SCORE_TYPE.ST_WIN, 365)

local userAttribute = sui.getAttribute(tempUserItem, {
			"score", "insure", "grade", "revenue", "medal", "gift", "present", "loveliness", 
		})

printr(userAttribute)

print("---------------------------------------------- distillVariation --------------------------------------------------------------------------------------------------")

local isModified, tmpVariation = sui.distillVariation(tempUserItem)
print(string.format("isModified:%s",isModified))
printr(tmpVariation)

print("---------------------------------------------- reset --------------------------------------------------------------------------------------------------")

-- CItemBuffer.release(tempUserItem)
sui.reset(tempUserItem)
sui.destroy(tempUserItem)


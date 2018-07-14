local printr = require "print_r"

print("------------------------------------------------------------------------------------------------------------------")

local bulletUtility = require "utility.bullet"

bulletUtility.setChairPerTable(6);
bulletUtility.setMultipleLimit(2, 30000000)

local bulletKind = bulletUtility.getBulletKindByScore(200)
print("utility.bullet getBulletKindByScore - ",bulletKind)

local baseScore = bulletUtility.getBulletBaseByKind(bulletKind)
print("utility.bullet getBulletBaseByKind - ",baseScore)

local currentBulletMultiple = bulletUtility.getBulletMultipleByKind(500,3)
print("utility.bullet getBulletMultipleByKind - ",currentBulletMultiple)

local angle = bulletUtility.getAngle(3)
print("utility.bullet getAngle - ",angle)

print("------------------------------------------------------------------------------------------------------------------")

local currencyUtility = require "utility.currency"


local strScore = currencyUtility.formatCurrency(123456789012)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(12345678901)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(1234567890)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(123456789)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(12345678)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(12345678)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(1234567)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(123456)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(12345)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(1234)
print("utility.currency - ",strScore)
local strScore = currencyUtility.formatCurrency(123)
print("utility.currency - ",strScore)

print("------------------------------------------------------------------------------------------------------------------")
local ipUtility = require "utility.ip"
local addr = ipUtility.getAddressOfIP("127.0.0.1:80")
print("utility.ip - ",addr)

print("------------------------------------------------------------------------------------------------------------------")

local jsonHttpResponseUtility = require "utility.jsonHttpResponse"

local statuscode,jsonObj = jsonHttpResponseUtility.getSimpleResponse(false, "invalid argument")
print("utility.jsonHttpResponse getSimpleResponse - ",statuscode,jsonObj)

local statuscode,jsonObj = jsonHttpResponseUtility.getResponse({isSuccess=true, data="success"})
print("utility.jsonHttpResponse getResponse - ",statuscode,jsonObj)

print("------------------------------------------------------------------------------------------------------------------")


local pathUtility = require "utility.path"

local pathType = {
    pt_single={min=1, max=80, intervalTicks=100},
    pt_pipeline={min=1, max=80, intervalTicks=2000},
}
--printr(pathType)


for pathType, pathConfig in pairs(pathType) do
    pathUtility.initPathConfig(pathType, pathConfig.min, pathConfig.max, pathConfig.intervalTicks)
end

local PATH_TYPE = {
	PT_SINGLE = "pt_single",
	PT_PIPELINE = "pt_pipeline",
}
local nowTick = 3
pathUtility.checkPathStatus(PATH_TYPE.PT_SINGLE, nowTick)
pathUtility.checkAllPathStatus(nowTick)

local buildTick= 9
local pathID = pathUtility.getPathID(PATH_TYPE.PT_SINGLE, buildTick)
print("pathUtility.getPathID - PT_SINGLE",pathID)
local pathID = pathUtility.getPathID(PATH_TYPE.PT_PIPELINE, nowTick)
print("pathUtility.getPathID - PT_PIPELINE",pathID)

pathUtility.resetAll()

print("------------------------------------------------------------------------------------------------------------------")

require "utility.string"
local readFileUtility = require "utility.readFile"

local tableConfig,count = readFileUtility.loadCsvFile("DropTable.csv")
print("loadCsvFile - DropTable.csv ---------------")
print("readFileUtility.loadCsvFile - tableConfig,count - ",tableConfig,count)
printr(tableConfig)

local _fengHuangDropConfig = {}
for i = 1, count do
    local info = {
        id = tonumber(tableConfig[i].Id),
        rewardList = {},
        minRate = tonumber(tableConfig[i].MinRate),
        maxRate = tonumber(tableConfig[i].MaxRate),
    }

    local max = tonumber(tableConfig[i].MaxRate)

    local itemList = tableConfig[i].Drop:split("|")
    for _, items in pairs(itemList) do
        local itemPart = items:split(":")
        local goods = {
            goodsID = tonumber(itemPart[1]),
            goodsCount = tonumber(itemPart[2]),
        }
        table.insert(info.rewardList,goods)
    end

    table.insert(_fengHuangDropConfig,info)
end
print("_fengHuangDropConfig ---------------")
printr(_fengHuangDropConfig)

print("------------------------------------------------------------------------------------------------------------------")


local signUtility = require "utility.sign"
local sign = signUtility.getSign()
print("signUtility.getSign - sign",sign)
--804c585c24244cf15576109ef7034009

print("------------------------------------------------------------------------------------------------------------------")

local timeUtility = require "utility.time"
local startTime = timeUtility.makeTimeStamp("2018-07-13 14:28:52")
print("timeUtility.makeTimeStamp - ",startTime)


print("------------------------------------------------------------------------------------------------------------------")



local xpcallUtility = require "utility.xpcall"
xpcallUtility.errorMessageSaver("error msg")
local dataerr = xpcallUtility.getErrorMessage()

print("xpcallUtility.getErrorMessage - ",dataerr)

print("------------------------------------------------------------------------------------------------------------------")


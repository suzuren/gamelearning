
local printr = require "print_r"

local _invalidGunHash = {}

local function cmd_LoadData(userID)
	local info = {
		timeCount = 0,
		gunCount = 2,
		gold = 3,
		iFishCount = 4,
		lastTime = 8,
	}

	_invalidGunHash[userID]=info
end


local function cmd_ReleaseUserData(userID)
	_invalidGunHash[userID] = nil
end


local function cmd_AddInvalidGunData(userID,score)
	local info = _invalidGunHash[userID]
	if info then
		info.gunCount = info.gunCount + 1
		info.gold = info.gold + score
	end
end

print("invalidGunHash 0 ------------------------------------------------------")
printr(_invalidGunHash)
print("invalidGunHash 1 ------------------------------------------------------")
cmd_LoadData(1)
cmd_LoadData(2)
cmd_LoadData(3)
cmd_LoadData(4)
printr(_invalidGunHash)
print("invalidGunHash 2 ------------------------------------------------------")
cmd_AddInvalidGunData(1,5)
cmd_AddInvalidGunData(2,3)
printr(_invalidGunHash)
print("invalidGunHash 3 ------------------------------------------------------")
cmd_ReleaseUserData(3)
printr(_invalidGunHash)
print("invalidGunHash end ------------------------------------------------------")



print("testRemoveTable 0 ------------------------------------------------------")

local testRemoveTable = {1, 2, 3, 4, 5, 6}
printr(testRemoveTable)
print("testRemoveTable 2 ------------------------------------------------------")

table.remove(testRemoveTable, 1)
printr(testRemoveTable)

print("testRemoveTable 3 ------------------------------------------------------")

table.remove(testRemoveTable)
printr(testRemoveTable)

print("testRemoveTable 4 ------------------------------------------------------")

table.remove(testRemoveTable,#testRemoveTable)
printr(testRemoveTable)

print("testRemoveTable end ------------------------------------------------------")


local _serverSignature
local isPullingStarted = _serverSignature~=nil

print("isPullingStarted - ",isPullingStarted)
print("_serverSignature - ",_serverSignature)

print("isPullingStarted end ------------------------------------------------------")


print("tabelsize sta ------------------------------------------------------")

local t = {[1]=1,[9]=3,["6"]=0,[2]=5};
print(#t) -- 2

local t = {[1]=1,[8]=2,["7"]=2,[0]=5};
print(#t) -- 1

local t = {[2]=1,[5]=2,["8"]=3,[5]=5};
print(#t) -- 0

function count(tab)
  local n = 0;
  for _, v in pairs(ht) do
    n = n + 1;
  end
  return n;
end

local dbs_st = {
    tk_biz = {
      host     = "rm-xxxx",
      user     = "xxxx",
      password = "xxxx",
      port     = 3306,
      database = "xxxx",
      timeout  = 30
    }
  }
print(#dbs_st) -- 0
print(#dbs_st.tk_biz) -- 0

local n = 0;
for _, v in pairs(dbs_st) do
	n = n + 1;
end
print(n) -- 1

local n = 0;
for _, v in pairs(dbs_st.tk_biz) do
	n = n + 1;
end
print(n) -- 6


print("tabelsize end ------------------------------------------------------")

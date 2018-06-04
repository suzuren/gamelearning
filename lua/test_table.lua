

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



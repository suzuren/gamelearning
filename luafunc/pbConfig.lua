
local printr = require "print_r"

local _data = {
	common = {
		s2c = {
			[0xff0000] = "common.misc.s2c.SystemMessage",
		},
		c2s = {},
		files = {
			"common.misc.s2c.pb",
		},
	},
	loginServer = {
		s2c = {
			[0x000000] = "loginServer.heartBeat.s2c.HeartBeat",
			[0x000100] = "loginServer.login.s2c.Login",
			[0x000101] = "loginServer.login.s2c.Logout",
		},
		c2s = {
			[0x000000] = "loginServer.heartBeat.c2s.HeartBeat",
			[0x000100] = "loginServer.login.c2s.Login",
		},
		files = {
			"loginServer.heartBeat.c2s.pb",
			"loginServer.heartBeat.s2c.pb",
			"loginServer.login.c2s.pb",
			"loginServer.login.s2c.pb",
			"loginServer.server.c2s.pb",
			"loginServer.server.s2c.pb",
		},
	},
	gameServer = {
		s2c = {
			[0x010000] = "gameServer.heartBeat.s2c.HeartBeat",
			
			[0x010100] = "gameServer.login.s2c.Login",
		},
		c2s = {
			[0x010000] = "gameServer.heartBeat.c2s.HeartBeat",
			[0x010100] = "gameServer.login.c2s.Login",
			[0x010109] = "gameServer.login.c2s.Logout",
		},
		files = {
			"gameServer.heartBeat.c2s.pb",
			"gameServer.heartBeat.s2c.pb",
			"gameServer.login.c2s.pb",
			"gameServer.login.s2c.pb",
		},
	},
	fish = {
		s2c = {
			[0x020000] = "fish.s2c.UserFire",
			[0x020002] = "fish.s2c.SceneEnd",
			[0x020003] = "fish.s2c.SwitchScene",
		},
		c2s = {
			[0x020000] = "fish.c2s.UserFire",
			[0x020008] = "fish.c2s.BigNetCatchFish",
			[0x02000C] = "fish.c2s.CatchSweepFish",
		},
		files = {
			"fish.c2s.pb",
			"fish.s2c.pb",
			"fish.volcano.s2c.pb",
		},
	},
}

-- mergeConfig("loginServer", "common")

local function mergeConfig(...)
	local config = {
		s2c = {},
		c2s = {},
		files = {},
	}
	
	for _, sectionName in ipairs{...} do
		local c = _data[sectionName]
		if c then
			for k, v in pairs(c.s2c) do
				config.s2c[k] = v
			end
			
			for k, v in pairs(c.c2s) do
				config.c2s[k] = v
			end
			
			for _, v in ipairs(c.files) do
				table.insert(config.files, v)
			end
		end
	end
	
	return config
end

local function getConfig(type)
	if type=="loginServer" then
		return mergeConfig("loginServer", "common")
	elseif type=="fish" then
		return mergeConfig("gameServer", "fish", "common")
	else
		error(string.format("invalid type \"%s\"", type), 2)
	end
end

--[[
return {
	getConfig = getConfig,
}
]]

local mode = "loginServer"
local conf = getConfig(mode)
print("---------------------------------------------------------------------------------")
printr(conf)
print("---------------------------------------------------------------------------------")
printr(_data)
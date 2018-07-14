
local CMD = {}
local fd = 0
local logPath
local logLevel
local logDate = os.date("%Y%m%d", os.time())

-- 日志级别
local level = {
	["DEBUG"] = 1,
	["INFO"] = 2,
	["WARN"] = 3,
	["ERROR"] = 4,
	["FATAL"] = 5,
	["NON"] = 6
}

function CMD.logOpen()
	logPath = "./logs"
	logLevel = "DEBUG"
	if logPath == nil then
		return
	end
	if logLevel == nil then
		logLevel = "DEBUG"
	end
	os.execute("mkdir -p " .. logPath)
	
	fd = io.open(logPath .. "/temp.log", "a+")
	if fd == nil then
		print("can't open log file[".. logPath .. "/temp.log]")
	end
end

function CMD.xLog(lev, logStr)
	if logPath == nil then
		return
	end
	if tonumber(os.date("%Y%m%d")) > tonumber(logDate) then
		fd:close()
		os.execute("mv " .. logPath .. "/temp.log " ..logPath.."/"..logDate..".log")
		logDate = os.date("%Y%m%d")
		fd = io.open(logPath .. "/temp.log", "w+")
	end
	local tempDate = os.date("%Y%m%d %H:%M:%S")
	if fd ~= nil and fd ~= 0 then
		if level[lev] >= level[logLevel] then
			fd:write("["..tempDate .. "][".. lev .. "]" .. logStr .. "\n")
			fd:flush()
  			--skynet.error("["..tempDate .. "][".. lev .. "]" .. logStr)
			print("["..tempDate .. "][".. lev .. "]" .. logStr .. "\n")
		end
	end
end

function CMD.logClose()
	if fd ~= nil and fd ~= 0 then
		fd:close()
	end
end



function serialize(t)
	local mark={}
	local assign={}	
	local function ser_table(tbl,parent)
		mark[tbl]=parent
		local tmp={}
		for k,v in pairs(tbl) do
			local key= type(k)=="number" and "["..k.."]" or k
			if type(v)=="table" then
				local dotkey= parent..(type(k)=="number" and key or "."..key)
				if mark[v] then
					table.insert(assign,dotkey.."="..mark[v])
				else
					table.insert(tmp, key.."="..ser_table(v,dotkey))
				end
			elseif type(v)=="boolean" then
				if v then
					table.insert(tmp, key.."=true")
				else
					table.insert(tmp, key.."=false")
				end
			else
				table.insert(tmp, key.."="..v)
			end
		end
		return "{"..table.concat(tmp,",").."}"
	end
 
	return ser_table(t,"ret")..table.concat(assign," ")
end


--[[
skynet.start(function()
	CMD.logOpen()
	skynet.dispatch("lua", function(session, source, cmd, ...)
		local f = assert(CMD[cmd], string.format("%s: handler not found for \"%s\"", SERVICE_NAME, cmd))
		f(...)
	end)
end)
]]


local test_table = {1,2,3,4,5,6,7,8,9}
local test_table = {"test","hello","cpp",3,"world"}
--print(serialize(test_table))



CMD.logOpen()
CMD.xLog("DEBUG", "test logserver 0.")
CMD.xLog("DEBUG", "test logserver 1.")
CMD.xLog("INFO",serialize(test_table))



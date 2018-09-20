
print("-------------------------------------------------------------")

-- 下面代码打印在当前环境中所有的全局变量的名字
for n in pairs(_G)	do
	print(n)
end

print("-------------------------------------------------------------")

function getfield (f)
	local v = _G -- start with the table of globals
	for w in string.gfind(f, "[%w_]+") do
		v = v[w]
	end
	return v
end

print("-------------------------------------------------------------")

function newAccount (initialBalance)
	local self = {balance = initialBalance}
	local withdraw = function (v)
		self.balance = self.balance - v
	end

	local deposit = function (v)
		self.balance = self.balance + v
	end

	local getBalance = function ()
		return self.balance
	end

	return {
	withdraw = withdraw,
	deposit = deposit,
	getBalance = getBalance
	}
end


acc1 = newAccount(100.00)
acc1.withdraw(40.00)
print(acc1.getBalance()) --> 60


print("-------------------------------------------------------------")

function newObject (value)
	return function (action, v)
		if action == "get" then return value
		elseif action == "set" then value = v
		else error("invalid action")
		end
	end
end

d = newObject(3)
print(d("get"))  --> 3
d("set", 10)
print(d("get"))  --> 10

print("-------------------------------------------------------------")


local arrayTable = {1,2}
print(#(arrayTable))

for line=1 ,10 do
	table.insert(arrayTable, line)
end

arrayTable[-1] = 119
arrayTable[0] = 118

print(#(arrayTable))

for i in pairs(arrayTable) do
	print(" pairs",i,arrayTable[i])
end

for i in ipairs(arrayTable) do
	print("ipairs",i,arrayTable[i])
end

print("-------------------------------------------------------------")

lines = {
	luaH_set = 10,
	luaH_get = 24,
	luaH_present = 48,
}

function pairsByKeys (t, f)
	local a = {}
	for n in pairs(t) do
		table.insert(a, n)
	end
	table.sort(a, f)
	local i = 0  -- iterator variable
	local iter = function () -- iterator function
		i = i + 1
		if a[i] == nil then return nil
		else return a[i], t[a[i]]
		end
	end
	return iter
end

-- 以字母顺序打印这些函数名

for name, line in pairsByKeys(lines) do
	print(name, line)
end

print("-------------------------------------------------------------")



print("-------------------------------------------------------------")



print("-------------------------------------------------------------")

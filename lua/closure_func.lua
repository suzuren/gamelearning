

--------------------------------------------------------------------------------------------------------------------------------------------------------------


-- 在Lua中，闭包（closure）是由一个函数和该函数会访问到的非局部变量（或者是upvalue）组成的，其中非局部变量（non-local variable）是指不是在局部作用范围内
-- 定义的一个变量，但同时又不是一个全局变量，主要应用在嵌套函数和匿名函数里，因此若一个闭包没有会访问的非局部变量，那么它就是通常说的函数。也就是说，在Lua中，
-- 函数是闭包一种特殊情况。另外在Lua的C API中，所有关于Lua中的函数的核心API都是以closure来命名的，也可视为这一观点的延续。
-- 在Lua中，函数是一种第一类型值（First-Class Value），它们具有特定的词法域（Lexical Scoping）。

--------------------------------------------------------------------------------------------------------------------------------------------------------------

local string = require("string")
local table = require("table")

-- 函数作为返回值,这里的i也叫外部局部变量，就是lua中的upvalue
function newCounter()
	local i = 0
	return function()
      		i = i + 1
      		return i
   	end
end

------------------------------------------------------------------------------

--c1,c2是建立在同一个函数，同一个局部变量的不同实例上面的两个不同的闭包
--闭包中的upvalue各自独立，调用一次test（）就会产生一个新的闭包
local c1=newCounter()
local c2=newCounter()

--print(c1())	-->1
--print(c1())	-->2  -- 重复调用时每一个调用都会记住上一次调用后的值，就是说 i=1 了已经
--print(c2())	-->1  -- 闭包不同所以 upvalue 不同
--print(c2())	-->2

------------------------------------------------------------------------------

local string_counter = "string_counter = "

c = newCounter()
for i= 1,10 do
string_counter = string_counter ..tostring(string.format("%d,", c()))
--print(c())
end
print(string_counter)

--------------------------------------------------------------------------------------------------------------------------------------------------------------

function list_iter(t)
            local i=0
            --local n=table.getn(t)
			local n=#t
            return function()
                i=i+1
                if i<=n then return t[i] end
            end
        end
-- 这里的list_iter是一个工厂，每次调用都会产生一个新的闭包该闭包内部包括了upvalue(t,i,n)
-- 因此每调用一次该函数产生的闭包那么该闭包就会根据记录上一次的状态，以及返回list的下一个

------------------------------------------------------------------------------

-- while中使用：
local string_element1 = "string_element1 = "
t={10,20,90}
iter=list_iter(t)  -- 调用迭代器产生一个闭包
while true do
	local element=iter()
	if element==nil then break end
	string_element1 = string_element1 ..tostring(string.format("%d,", element))
	--print(element)
end
print(string_element1)

------------------------------------------------------------------------------

--泛型for使用：
local string_element2 = "string_element2 = "

t={10,0,29}
for element in list_iter(t) do -- 这里的list_iter()工厂函数只会被调用一次产生一个闭包函数，后面的每一次迭代都是用该闭包函数，而不是工厂函数
	string_element2 = string_element2 ..tostring(string.format("%d,", element))
	--print(element)
end
print(string_element2)


--------------------------------------------------------------------------------------------------------------------------------------------------------------






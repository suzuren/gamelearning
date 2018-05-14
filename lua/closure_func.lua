
local string = require("string")

-- 函数作为返回值,这里的i也叫外部局部变量，就是lua中的upvalue
function newCounter()
	local i = 0
	return function()
      		i = i + 1
      		return i
   	end
end

--c1,c2是建立在同一个函数，同一个局部变量的不同实例上面的两个不同的闭包
--闭包中的upvalue各自独立，调用一次test（）就会产生一个新的闭包
local c1=newCounter()
local c2=newCounter()
             	
print(c1())	-->1
print(c1())	-->2//重复调用时每一个调用都会记住上一次调用后的值，就是说i=1了已经
print(c2())	-->1//闭包不同所以upvalue不同    
print(c2())	-->2


local string_counter = "string_counter = "

c = newCounter()
for i= 1,10 do
string_counter = string_counter ..tostring(string.format("%d,", c()))
--print(c())
end
print(string_counter)







function list_iter(t)
            local i=0
            local n=table.getn(t)
            return function()
                i=i+1
                if i<=n then return t[i] end
            end
        end
-- 这里的list_iter是一个工厂，每次调用都会产生一个新的闭包该闭包内部包括了upvalue(t,i,n)
-- 因此每调用一次该函数产生的闭包那么该闭包就会根据记录上一次的状态，以及返回list的下一个


-- while中使用：
t={10,20,90}
iter=list_iter(t)  -- 调用迭代器产生一个闭包
while true do
	local element=iter()
	if element==nil then break end
	print(element)
end

--泛型for使用：
t={10,0,29}
for element in list_iter(t) do -- 这里的list_iter()工厂函数只会被调用一次产生一个闭包函数，后面的每一次迭代都是用该闭包函数，而不是工厂函数
	print(element)
end









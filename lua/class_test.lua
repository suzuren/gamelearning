classA = 
{
    a = 0,
    b = 0,
    --__index = classA;
};

classA.__index = classA;

function classA:new(a, b)
    local o = {};
    setmetatable(o, self);
    
    o.a = a;
    o.b = b;
    
    return o;
end

function classA:getA()
    return self.a;
end

function classA:getB()
    return self.b;
end


local test_a = classA:new(1, 2);
print('test_a.a = ' .. test_a:getA());
print('test_a.b = ' .. test_a:getB());




-- local parent = require("parent")
local child = require("class_child")

local c = child:new()
-- 从parent继承的show方法
c:show()
-- child自己的方法。
c:hello()
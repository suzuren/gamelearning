
Class = {}
Class.__index = Class
  
function Class.new(x,y)
    local cls = {}
    setmetatable(cls, Class)
    cls.x = x
    cls.y = y
    return cls
end
function Class:test()
-- 等价于
-- function Class.test(self)
    print(self.x,self.y)
end
  
  
object = Class.new(10,20)

object:test() --10 20
-- 等价于
object.test(object) --10 20


--（1）定义的时候：Class:test()与 Class.test(self)是等价的；   . have self
--（2）调用的时候：object:test() 与object.test(object)等价。  : have not self



object2 = Class:new(10,20)
object2:test()
object2:test(object2)

-- table: 0x22458e0	10
-- table: 0x22458e0	10















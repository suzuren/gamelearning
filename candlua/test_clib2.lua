local mytest = require "testclibtwo"

ret = mytest.add(10,20)
print(ret)

ret = mytest.add_with_upvalue(10,20)
print(ret)
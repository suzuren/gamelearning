local printr = require "print_r"

print("------------------------------------------------------------------------------------------------------------------")

local jsonUtil = require "cjson.util"

local msgData = {"qw","er","ty","ui"}
local value = jsonUtil.serialise_value(msgData)
print("jsonUtil.serialise_value - ",value)

local msgData = {a="qw",b="er",c="ty",d="ui"}
local value = jsonUtil.serialise_value(msgData)
print("jsonUtil.serialise_value - ",value)

local filename = "makefile"
local data = jsonUtil.file_load(filename)
print("jsonUtil.file_load - ",data)

local filename = "hello.log"
local data = "hello world!"
jsonUtil.file_save(filename, data)


local val1 = 3
local val2 = 3
local ret = jsonUtil.compare_values(val1, val2)
print("jsonUtil.compare_values - ",ret)
local val1 = 1
local val2 = 3
local ret = jsonUtil.compare_values(val1, val2)
print("jsonUtil.compare_values - ",ret)
local val1 = 3
local val2 = 1
local ret = jsonUtil.compare_values(val1, val2)
print("jsonUtil.compare_values - ",ret)


--local testname="test", func=nul, input, should_work, output
--local correct, result = jsonUtil.run_test(testname, func, input, should_work, output)
--print("jsonUtil.run_test - ",correct, result)
local tests={a={1,2},b={3,4},c={5,6},d={7,8}}
local ret = jsonUtil.run_test_group(tests)
print("jsonUtil.run_test_group - ",ret)


--local script="script"
--local env = 3
--local env = jsonUtil.run_script(script, env)
--print("jsonUtil.run_script - ",env)




print("------------------------------------------------------------------------------------------------------------------")

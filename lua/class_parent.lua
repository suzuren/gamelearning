local _M = {}

function _M:new(name)
    return setmetatable({ name = name}, { __index = _M })
end

function _M:show()
    print(self.name .. ": show in parent.")
end

function _M:hello(arg)
        print(self.name .. ": hello in parent:" .. tostring(arg))
end

return  _M
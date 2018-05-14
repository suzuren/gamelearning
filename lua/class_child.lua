local parent = require("class_parent")

local _M = {}


function _M:new()
        local obj = parent:new("the child")
        local super_mt = getmetatable(obj)
        -- �������������в�ѯ����ʱ����ȥ������ȥ���ҡ�
        setmetatable(_M, super_mt)
        -- �������ú󣬿���ͨ��self.super.method(self, ...) ���ø�����ѱ����ǵķ�����
        obj.super = setmetatable({}, super_mt)
    return setmetatable(obj, { __index = _M })
end


-- ���Ǹ���ķ�����
function _M:hello()
        -- ֻ��ʹ�����ַ������û���ķ�����
        self.super.hello(self, "call from child")
        print(tostring(self.name) .. ": hello in child")
end

return _M
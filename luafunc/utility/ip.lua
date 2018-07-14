local function getAddressOfIP(ip)
	local i = string.find(ip, ":", 1, true) -- ÕÒµ½ : ·ûºÅµÄÎ»ÖÃ  127.0.0.1
	-- print("string.find - ",i)
	if i ~= nil then
		ip = string.sub(ip, 1, i-1)
		-- print("string.sub - ",ip)
	end
	return ip
end

return {
	getAddressOfIP = getAddressOfIP,
}

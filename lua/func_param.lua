--函数
function g_func(param)
	print("param+1:",param+1)
end
 
--变量
local g_number = 1
 
--做事情
function doSM(num, fn)
    --传进来的变量g_number 
    local number = num
     
    --传进来的函数g_func
    fn(number)
end

 
--主函数
function main()
    --把变量和函数传入doSM函数
    doSM(g_number , g_func)
end

main()

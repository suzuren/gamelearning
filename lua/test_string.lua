
print("-------------------------------------------------------------")

local testString = string.upper("a??o")
print(testString)

local testString = string.lower("AUYTU")
print(testString)

print("-------------------------------------------------------------")

-- string.sub(s,2, -2)返回去除第一个和最后一个字符后的子串。
s = "[in brackets]1"
print(string.sub(s, 2, -2))

print("-------------------------------------------------------------")

-- string.char 获取 0 个或多个整数，将每一个数字转换成字符，然后返回一个所有这些字符连接起来的字符串。
print(string.char(97))  --> a
i = 99
print(string.char(i, i+1, i+2))  --> cde

-- string.byte(s, i)将字符串 s 的第 i 个字符的转换成整数；第二个参数是可选的， 缺省情况下 i=1。
print(string.byte("abc"))  --> 97
print(string.byte("abc", 2))  --> 98
print(string.byte("abc", -1))  --> 99 -- 使用负数索引访问字符串的最后一个字符

print("-------------------------------------------------------------")

PI = 3.141592653
print(string.format("pi = %.4f", PI))
--> pi = 3.1416
d = 5; m = 11; y = 1990
print(string.format("%02d/%02d/%04d", d, m, y))
--> 05/11/1990
tag, title = "h1", "a title"
print(string.format("<%s>%s</%s>", tag, title, tag))
--> <h1>a title</h1>

print("-------------------------------------------------------------")

s = "hello world"
i, j = string.find(s, "hello")
print(i, j)  --> 1 5
print(string.sub(s, i, j)) --> hello
print(string.find(s, "world")) --> 7 11
i, j = string.find(s, "l")
print(i, j)  --> 3 3
print(string.find(s, "lll"))  --> nil

print("-------------------------------------------------------------")

--string.gsub 函数有三个参数：目标串，模式串，替换串。他基本作用是用来查找匹配模式的串，并将使用替换串其替换掉：

s = string.gsub("Lua is cute", "cute", "great")
print(s)  --> Lua is great
s = string.gsub("all lii", "l", "x")
print(s)  --> axx xii
s = string.gsub("Lua is great", "perl", "tcl")
print(s)  --> Lua is great

-- 第四个参数是可选的，用来限制替换的范围

s = string.gsub("all lii", "l", "x", 1)
print(s)  --> axl lii
s = string.gsub("all lii", "l", "x", 2)
print(s)  --> axx lii

--string.gsub 的第二个返回值表示他进行替换操作的次数。例如，下面代码涌来计算一个字符串中空格出现的次数：

local _, count = string.gsub(s, " ", " ")
print(count)
-- 注意，_ 只是一个哑元变量

print("-------------------------------------------------------------")

--还可以在模式串中使用字符类。字符类指可以匹配一个特定字符集合内任何字符
--的 模 式 项 。 比 如 ， 字 符 类 %d 匹 配 任 意 数 字 。 所 以 你 可 以 使 用 模 式 串
--'%d%d/%d%d/%d%d%d%d'搜索 dd/mm/yyyy 格式的日期：

s = "Deadline is 30/05/1999, firm"
date = "%d%d/%d%d/%d%d%d%d"
print(string.sub(s, string.find(s, date)))  --> 30/05/1999

print("-------------------------------------------------------------")

--[[

.  任意字符
%a 字母
%c 控制字符
%d 数字
%l 小写字母
%p 标点字符
%s 空白符
%u 大写字母
%w 字母和数字
%x 十六进制数字
%z 代表 0 的字符

上面字符类的大写形式表示小写所代表的集合的补集。例如，'%A'非字母的字符

可以使用修饰符来修饰模式增强模式的表达能力，Lua 中的模式修饰符有四个：

+  匹配前一字符 1 次或多次
*  匹配前一字符 0 次或多次
-  匹配前一字符 0 次或多次
?  匹配前一字符 0 次或 1 次


]]
print(string.gsub("hello, up-down!", "%A", "."))
--> hello..up.down. 4
-- 非字母字符用.替换

print("-------------------------------------------------------------")

-- 模式串 '%a+' 匹配一个或多个字母或者一个单词：

print(string.gsub("one, and two; and three", "%a+", "word"))
--> word, word word; word word

-- '%d+' 匹配一个或多个数字（整数）：
i, j = string.find("the number 1298 is even", "%d+")
print(i,j) --> 12 15

print("-------------------------------------------------------------")

--由于 '.*' 进行的是最长匹配，这个模式将匹配程序中第一个 "/*" 和最后一个 "*/" 之间所有部分：
test = "int x; /* x */ int y; /* y */"
print(string.gsub(test, "/%*.*%*/", "<COMMENT>"))
--> int x; <COMMENT>

--然而模式 '.-' 进行的是最短匹配，她会匹配 "/*" 开始到第一个 "*/" 之前的部分：

test = "int x; /* x */ int y; /* y */"
print(string.gsub(test, "/%*.-%*/", "<COMMENT>"))
--> int x; <COMMENT> int y; <COMMENT>

print("-------------------------------------------------------------")

--检查字符串 s 是否以数字开头，而
s="wiuyeiekdk"
print(string.find(s, "^%d"))

--检查字符串 s 是否是一个整数。
s="111"
print(string.find(s, "^[+-]?%d+$"))

--'%b' 用来匹配对称的字符。常写为 '%bxy' ，x 和 y 是任意两个不同的字符；x 作为
--匹配的开始，y 作为匹配的结束。比如，'%b()' 匹配以 '(' 开始，以 ')' 结束的字符串：

print(string.gsub("a (enclosed (in) parentheses) line","%b()", ""))
--> a line

--常用的这种模式有：'%b()' ，'%b[]'，'%b%{%}' 和 '%b<>'。你也可以使用任何字符作为分隔符。

print("-------------------------------------------------------------")

pair = "name = Anna"
_, _, key, value = string.find(pair, "(%a+)%s*=%s*(%a+)")
print(key, value) --> name Anna

--'%a+' 表示菲空的字母序列；'%s*' 表示 0 个或多个空白。在上面的例子中，整个模
--式代表：一个字母序列，后面是任意多个空白，然后是 '=' 再后面是任意多个空白，然
--后是一个字母序列。两个字母序列都是使用圆括号括起来的子模式，当他们被匹配的时
--候，他们就会被捕获。当匹配发生的时候，find 函数总是先返回匹配串的索引下标
--（上面例子中我们存储哑元变量 _ 中），然后返回子模式匹配的捕获部分

-- 下面的例子情况类似

date = "17/7/1990"
_, _, d, m, y = string.find(date, "(%d+)/(%d+)/(%d+)")
print(d, m, y)  --> 17 7 1990

print("-------------------------------------------------------------")

--下面例子中，对一个字符串中的每一个字母进行复制，并用连字符将复制的字母和原字母连接起来：
print(string.gsub("hello Lua!", "(%a)", "%1-%1"))
--> h-he-el-ll-lo-o L-Lu-ua-a!

print("-------------------------------------------------------------")

--下面代码互换相邻的字符:
print(string.gsub("hello Lua", "(.)(.)", "%2%1"))
--> ehll ouLa


print("-------------------------------------------------------------")



--最后一个捕获值应用之处可能是功能最强大的。我们可以使用一个函数作为
--string.gsub 的第三个参数调用 gsub。在这种情况下，string.gsub 每次发现一个匹配的时候
--就会调用给定的作为参数的函数，捕获值可以作为被调用的这个函数的参数，而这个函
--数的返回值作为 gsub 的替换串。
--下面的代码将一个字符串中全局变量$varname 出现的地方替换为变量 varname 的值：

function expand (s)
	s = string.gsub(s, "$(%w+)", function (n)
		print("expand function - ",n,_G[n])
		return _G[n]
	end)
	return s
end

name = "Lua"; status = "great"
print(expand("$name is $status, isn't it?"))
--> Lua is great, isn't it?

print("-------------------------------------------------------------")


//--------------------------------------------------------------------------------

#include <iostream>
#include <optional>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <vector>
#include <string>

//--------------------------------------------------------------------------------

void creating_strings()
{
	std::cout << "creating_strings - start - \n";

	std::string a{ "a" };
	auto b("b");
	std::string_view c{ "c" };
	auto d("d");
	std::cout << a << ", " << b << '\n';
	std::cout << c << ", " << d << '\n';

	std::cout << a + b << '\n';
	std::cout << a + std::string{ c } << '\n';


	std::ostringstream o;
	o << a << " " << b << " " << c << " " << d;
	auto concatenated(o.str());
	std::cout << concatenated << '\n';


	//将新字符串中所有的字符转换成大写字符。这里我们使用C库中的 toupper  函数来完成将字母转换为大写的工作
	std::transform(std::begin(concatenated), std::end(concatenated), std::begin(concatenated), ::toupper);
	std::cout << concatenated << '\n';

	std::cout << "creating_strings - end - \n";
}

//--------------------------------------------------------------------------------

std::string trim_whitespace_surrounding(const std::string &s)
{
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	//检查函数是否返回了合理的位置，当没有找到时，函数会返回一个特殊的位置—— string::npos
	if (std::string::npos == first)
	{
		return{};
	}
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}
void trim_whitespace()
{
	std::cout << "trim_whitespace - start - \n";

	std::string s{ " \t\n string surrounded by ugly"" whitespace \t\n " };
	std::cout << "{" << s << "}\n";
	std::cout << "{"
		<< trim_whitespace_surrounding(s)
		<< "}\n";
	std::cout << "trim_whitespace - end - \n";
}

//--------------------------------------------------------------------------------

int strings_streams_regular_expressions()
{
	//--------------------------------------------------------------------------------
	//01
	//creating_strings();

	//--------------------------------------------------------------------------------
	//02
	trim_whitespace();


	return 0;
}



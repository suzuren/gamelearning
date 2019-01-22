


//--------------------------------------------------------------------------------

#include <iostream>
#include <optional>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <regex>
#include <iomanip>

//--------------------------------------------------------------------------------


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

	//std::string s{ " \t\n string surrounded by ugly"" whitespace \t\n " };
	//std::cout << "{" << s << "}\n";
	//std::cout << "{"
	//	<< trim_whitespace_surrounding(s)
	//	<< "}\n";


	//string::substr  来构造前后没有空格的字符串。接受一个首字符相对位置和字符串长度，
	//然后就会构造一个子字符串进行返回。举个栗子， string{ "abcdef" }.substr(2, 2)  将返回 cd

	std::cout << "std::string::npos:" << std::string::npos << "\n";

	const char whitespace[]{ "\t\n" };

	std::string strFirstWhite{ "\t\n0_whilehello" };
	const size_t first_0(strFirstWhite.find_first_not_of(whitespace));
	const size_t last_0(strFirstWhite.find_last_not_of(whitespace));
	std::string strFirstTemp = strFirstWhite.substr(first_0, (last_0 - first_0 + 1));
	std::cout << "first_0:" << first_0 << " last_0:" << last_0 << " {" << strFirstTemp << "}\n";

	std::string strLastWhite{ "1_whilehello\t\n" };
	const size_t first_1(strLastWhite.find_first_not_of(whitespace));
	const size_t last_1(strLastWhite.find_last_not_of(whitespace));
	std::string strLastTemp = strLastWhite.substr(first_1, (last_1 - first_1 + 1));
	std::cout << "first_1:" << first_1 << " last_1:" << last_1 << " {" << strLastTemp << "}\n";

	std::string strFirstLastWhite{ "\t\n2_whilehello\t\n" };
	const size_t first_2(strFirstLastWhite.find_first_not_of(whitespace));
	const size_t last_2(strFirstLastWhite.find_last_not_of(whitespace));
	std::string strFirstLastTemp = strFirstLastWhite.substr(first_2, (last_2 - first_2 + 1));
	std::cout << "first_2:" << first_2 << " last_2:" << last_2 << " {" << strFirstLastTemp << "}\n";

	std::string strMiddleWhite{ "3_while\t\nhello" };
	const size_t first_3(strMiddleWhite.find_first_not_of(whitespace));
	const size_t last_3(strMiddleWhite.find_last_not_of(whitespace));
	std::string strMiddleTemp =  strMiddleWhite.substr(first_3, (last_3 - first_3 + 1));
	std::cout << "first_3:" << first_3 << " last_3:" << last_3 << " {" << strMiddleTemp << "}\n";


	// 测试字符串
	std::string str = std::string("Hello World!");

	// 搜索用的字符串和字符
	std::string search_str = std::string("o");
	const char* search_cstr = "Good Bye!";

	std::cout << str.find_first_of(search_str) << '\n';
	std::cout << str.find_first_of(search_str, 5) << '\n';
	std::cout << str.find_first_of(search_cstr) << '\n';
	std::cout << str.find_first_of(search_cstr, 0, 4) << '\n';
	// 'x' 不在 "Hello World' 中，从而它将返回 std::string::npos
	std::cout << str.find_first_of('x') << '\n';


	std::cout << "trim_whitespace - end - \n";

	/*
trim_whitespace - start -
std::string::npos:18446744073709551615
first_0:2 last_0:13 {0_whilehello}
first_1:0 last_1:11 {1_whilehello}
first_2:2 last_2:13 {2_whilehello}
first_3:0 last_3:13 {3_while
hello}
4
7
1
4
18446744073709551615
trim_whitespace - end -
	
	*/
}

//--------------------------------------------------------------------------------


void print_string_view(std::string_view v)
{
	const auto words_begin(v.find_first_not_of(" \t\n"));
	v.remove_prefix(std::min(words_begin, v.size()));

	const auto words_end(v.find_last_not_of(" \t\n"));
	if (words_end != std::string_view::npos)
	{
		v.remove_suffix(v.size() - words_end - 1);
	}
	std::cout << "length: " << v.length() << " [" << v << "]\n";
}

void test_string_view(int argc, const char *argv[])
{
	std::cout << "test_string_view - start - \n";

	print_string_view(argv[0]);
	print_string_view({});
	print_string_view("a const char * array");
	print_string_view("an std::string_view literal   ");
	print_string_view("an std::string instance ");
	print_string_view(" \t\n foobar \n \t ");
	char cstr[]{ 'a', 'b', 'c' };
	print_string_view(std::string_view(cstr, sizeof(cstr)));

	std::cout << "test_string_view - end - \n";
	
	/*

test_string_view - start -
length: 9 [./testCpp]
length: 0 []
length: 20 [a const char * array]
length: 27 [an std::string_view literal]
length: 23 [an std::string instance]
length: 6 [foobar]
length: 3 [abc]
test_string_view - end -

	*/
}

//--------------------------------------------------------------------------------

template <typename InputIt>
void print_link_extraction(InputIt it, InputIt end_it)
{
	while (it != end_it)
	{
		const std::string link{ *it++ };
		if (it == end_it) { break; }
		const std::string desc{ *it++ };
		std::cout << std::left << std::setw(28) << desc << " : " << link << '\n';
	}
}


std::string str_html_text{ "\
<li><a href=\"https://isocpp.org/tour\">Tour</a></li>\
<li><a href=\"https://isocpp.org/guidelines\">Core Guidelines</a></li>\
<li><a href=\"https://isocpp.org/faq\">Super-FAQ</a></li>\
<li><a href=\"https://isocpp.org/std\">Standardization</a></li>\
" };


void test_link_extraction()
{
	std::cout << "test_link_extraction - start - \n";
	const std::regex link_re{ "<a href=\"([^\"]*)\"[^<]*>([^<]*)</a>" };
	const std::string in{ str_html_text };
	std::sregex_token_iterator it_http{ std::begin(in), std::end(in), link_re,{ 1, 2 } };
	print_link_extraction(it_http, {});


	const std::string s{ " abc abbccc " };
	const std::regex re{ "a(b*)(c*)" };
	std::sregex_token_iterator it{ std::begin(s), std::end(s), re,{ 1, 2 } };
	std::cout << (*it) << '\n'; // prints b
	++it;
	std::cout << (*it) << '\n'; // prints c
	++it;
	std::cout << (*it) << '\n'; // prints bb
	++it;
	std::cout << (*it) << '\n'; // prints ccc

	std::cout << "test_link_extraction - end - \n";

}

/*

一个简单HTML连接可写为 <a href="some_url.com/foo">A great link</a>  。
只需要 some_url.com/foo  和 A great link  部分


test_link_extraction - start -
Tour                         : https://isocpp.org/tour
Core Guidelines              : https://isocpp.org/guidelines
Super-FAQ                    : https://isocpp.org/faq
Standardization              : https://isocpp.org/std
test_link_extraction - end -

*/

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------

int strings_streams_regular_expressions(int argc, const char *argv[])
{
	//--------------------------------------------------------------------------------
	//01
	//creating_strings();

	//--------------------------------------------------------------------------------
	//02
	//trim_whitespace();

	//--------------------------------------------------------------------------------
	//03
	//test_string_view(argc, argv);

	//--------------------------------------------------------------------------------
	//04
	//test_link_extraction();

	//--------------------------------------------------------------------------------
	//05

	return 0;
}



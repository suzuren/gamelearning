#pragma once


// enable_if example: two ways of using enable_if
#include <iostream>
#include <type_traits>
#include <vector>
#include <map>

// template< bool B, class T = void >
// struct enable_if;
// 若 B 为 true ，则 std::enable_if 拥有公开成员 typedef type ，等于 T ；否则，无该成员 typedef 。
// 下面例子 is_odd 的返回值 ::type 类型为 T类型，也就是 bool 类型

// 1. the return type (bool) is only valid if T is an integral type:
template <class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i)
{
	return bool(i % 2);
}

// 2. the second template argument is only valid if T is an integral type:
template < class T, class = typename std::enable_if<std::is_integral<T>::value>::type>	bool is_even(T i)
{
	return !bool(i % 2);
}

template<typename T>
typename std::enable_if<
	!std::is_same<T, bool>::value &&
	!std::is_same<T, std::string>::value, T>::type
	read()
{
	std::cout << "func test_enable_if - " << "read type -> not bool and string" << std::endl;
	T ret(0.0);
	return ret;
}


template<typename T>
typename std::enable_if<
	std::is_same<T, bool>::value, T>::type read()
{
	std::cout << "func test_enable_if - " << "read type -> bool" << std::endl;
	T ret(true);
	return ret;
}

template<typename T>
typename std::enable_if<
	std::is_same<T, std::string>::value, T>::type read()
{
	std::cout << "func test_enable_if - " << "read type -> string" << std::endl;
	T ret("");
	return ret;
}

/*

enable_if_t 和 enable_if 的区别

std::enable_if_t 是C++14 才支持的写法， C++11 为std::enable_if
std::enable_if 大概实现如下 :

template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };


当 enable_if 的条件为true 时，优先匹配 struct enable_if<true,T> 这个模板，因此会多一个 type 类型， 和T 保持一致。
std::enable_if_t 大概实现如下:

template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type;

也就是说， enable_if_t 只是using enable_if 的type 。

*/


template<typename EnumT, typename std::enable_if<std::is_enum<EnumT>::value>::type * = nullptr>
void  func_enable_if(EnumT e1, EnumT e2)
{
	std::cout << "func test enable_if - " << "func_enable_if is_enum " << std::endl;

}

template<typename InterT, typename std::enable_if<std::is_integral<InterT>::value>::type * = nullptr>
void  func_enable_if(InterT i1, InterT i2)
{
	std::cout << "func test enable_if - " << "func_enable_if is_integral " << std::endl;

}


template<typename EnumT, typename std::enable_if_t<std::is_enum<EnumT>::value> * = nullptr>
void  func_enable_if_t(EnumT e1, EnumT e2)
{
	std::cout << "func test enable_if_t - " << "func_enable_if_t is_enum " << std::endl;

}

template<typename InterT, typename std::enable_if_t<std::is_integral<InterT>::value> * = nullptr>
void  func_enable_if_t(InterT i1, InterT i2)
{
	std::cout << "func test enable_if_t - " << "func_enable_if_t is_integral " << std::endl;
}



int test_enable_if()
{

	short int i = 1;    // code does not compile if type of i is not integral

	std::cout << "func test_enable_if - " << std::boolalpha << std::endl;
	std::cout << "func test_enable_if - " << "i is odd: " << is_odd(i) << std::endl;
	std::cout << "func test_enable_if - " << "i is even: " << is_even(i) << std::endl;

	//T value = read<T>();

	double dvalue = read<double>();
	int ivalue = read<int>();
	bool bvalue = read<bool>();	
	std::string strvalue = read<std::string>();

	std::cout << "func test_enable_if - read - " << "double:" << dvalue << ", int:" << ivalue << ", bool:" << bvalue << ", string:" << strvalue << std::endl;

	enum testEnum
	{
		test_start,
	};

	func_enable_if<int>(1, 4);
	func_enable_if_t<int>(4, 1);

	func_enable_if<testEnum>(test_start, test_start);
	func_enable_if_t<testEnum>(test_start, test_start);



	return 0;
}
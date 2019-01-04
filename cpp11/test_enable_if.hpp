#pragma once


// enable_if example: two ways of using enable_if
#include <iostream>
#include <type_traits>

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
	std::cout << "func test_enable_if - " << "read type -> not bool" << std::endl;
	T ret;
	return ret;
}

template<typename T>
typename std::enable_if<
	std::is_same<T, bool>::value, T>::type read()
{
	std::cout << "func test_enable_if - " << "read type -> bool" << std::endl;
	T ret;
	return ret;
}

template<typename T>
typename std::enable_if<
	std::is_same<T, std::string>::value, T>::type read()
{
	std::cout << "func test_enable_if - " << "read type -> string" << std::endl;
	T ret;
	return ret;
}

int test_enable_if()
{

	short int i = 1;    // code does not compile if type of i is not integral

	std::cout << "func test_enable_if - " << std::boolalpha << std::endl;
	std::cout << "func test_enable_if - " << "i is odd: " << is_odd(i) << std::endl;
	std::cout << "func test_enable_if - " << "i is even: " << is_even(i) << std::endl;

	//T value = read<T>();

	int ivalue = read<int>();

	bool bvalue = read<bool>();
	
	std::string strvalue = read<std::string>();

	return 0;
}
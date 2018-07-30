#ifndef _TYPE_NAME_H__
#define _TYPE_NAME_H__

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>

template <class T>
std::string type_name()
{
	typedef typename std::remove_reference<T>::type TR;
	std::unique_ptr<char, void(*)(void*)> own
		(
#ifndef __GNUC__
nullptr,
#else
abi::__cxa_demangle(typeid(TR).name(), nullptr,
		nullptr, nullptr),
#endif
		std::free
		);
	std::string r = own != nullptr ? own.get() : typeid(TR).name();
	if (std::is_const<TR>::value)
		r += " const";
	if (std::is_volatile<TR>::value)
		r += " volatile";
	if (std::is_lvalue_reference<T>::value)
		r += "&";
	else if (std::is_rvalue_reference<T>::value)
		r += "&&";
	return r;
}

#include <iostream>

void test_typename()
{
	std::string strtype = type_name<volatile int *>();
	std::cout << "test_typename - strtype:" << strtype << endl;

	printf("test_typename - type_name:%s\n", type_name<int>().c_str());
	printf("test_typename - type_name:%s\n", type_name<const int>().c_str());
	printf("test_typename - type_name:%s\n", type_name<int const>().c_str());

	std::cout << "test_typename - typeid int   name:" << typeid(int).name() << "   hash_code:" << typeid(int).hash_code() <<endl;
	std::cout << "test_typename - typeid char  name:" << typeid(char).name() << "   hash_code:" << typeid(char).hash_code() << endl;
	std::cout << "test_typename - typeid short name:" << typeid(short).name() << "   hash_code:" << typeid(short).hash_code() << endl;
	std::cout << "test_typename - typeid long  name:" << typeid(long).name() << "   hash_code:" << typeid(long).hash_code() << endl;
}

#endif

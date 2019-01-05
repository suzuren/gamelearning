

#include <iostream>
#include <tuple>
#include <string>
#include <stdexcept>

template<unsigned long long parameter>
struct fibonacci
{
	//enum { value = fibonacci<parameter - 1>::value + fibonacci<parameter - 2>::value };
	const static unsigned long long value = fibonacci<parameter - 1>::value + fibonacci<parameter - 2>::value;
};

template<>
struct fibonacci<0>
{
	//enum { value = 1 };
	const static unsigned long long value = 1;
};

template<>
struct fibonacci<1>
{
	//enum { value = 1 };
	const static unsigned long long value = 1;
};

void test_fibonacci_templates()
{
	std::cout << "func test_fibonacci_templates - 1  value: " << fibonacci<1>::value << std::endl;
	std::cout << "func test_fibonacci_templates - 2  value: " << fibonacci<2>::value << std::endl;
	std::cout << "func test_fibonacci_templates - 3  value: " << fibonacci<3>::value << std::endl;
	std::cout << "func test_fibonacci_templates - 4  value: " << fibonacci<4>::value << std::endl;
	std::cout << "func test_fibonacci_templates - 5  value: " << fibonacci<5>::value << std::endl;
	std::cout << "func test_fibonacci_templates - 6  value: " << fibonacci<6>::value << std::endl;
	//...
	std::cout << "func test_fibonacci_templates - 20 value: " << fibonacci<20>::value << std::endl;
	std::cout << "func test_fibonacci_templates - maxvalue: " << fibonacci<919>::value << std::endl;

}




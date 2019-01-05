

#include<iostream>
#include<utility>
#include<type_traits>

template<typename T>
struct has_member_foo
{
private:
	template<typename U>
	static auto Check(int) -> decltype(std::declval<U>().foo(), std::true_type());
	template<typename U>
	static std::false_type Check(...);
public:
	enum { value = std::is_same<decltype(Check<T>(0)), std::true_type>::value };
};

struct myStruct
{
	void foo() { std::cout << "hello" << std::endl; }
};

struct another
{
	void test() { std::cout << "test" << std::endl; }
};

// 这个has_member_foo的作用就是检查类型是否存在非静态成员foo函数
// 具体的实现思路是这样的：定义两个重载函数Check，由于模板实例化的过程中会优先选择匹配程度最高的重载函数，
// 在模板实例化的过程中检查类型是否存在foo函数，如果存在则返回std::true_type，否则返回std::false_type.
// 巧妙的利用了C++的SFINAE特性.

void test_sfinae_templates()
{
	if (has_member_foo<myStruct>::value)
	{
		std::cout << "func test_sfinae_templates - " << "myStruct has foo funciton" << std::endl;
	}
	else
	{
		std::cout << "func test_sfinae_templates - " << "myStruct does't have foo funciton" << std::endl;
	}

	if (has_member_foo<another>::value)
	{
		std::cout << "func test_sfinae_templates - " << "another has foo function" << std::endl;
	}
	else
	{
		std::cout << "func test_sfinae_templates - " << "another does't have foo function" << std::endl;
	}

}

//func test_sfinae_templates - myStruct has foo funciton
//func test_sfinae_templates - another does't have foo function



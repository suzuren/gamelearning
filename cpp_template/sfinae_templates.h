

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

// ���has_member_foo�����þ��Ǽ�������Ƿ���ڷǾ�̬��Աfoo����
// �����ʵ��˼·�������ģ������������غ���Check������ģ��ʵ�����Ĺ����л�����ѡ��ƥ��̶���ߵ����غ�����
// ��ģ��ʵ�����Ĺ����м�������Ƿ����foo��������������򷵻�std::true_type�����򷵻�std::false_type.
// �����������C++��SFINAE����.

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



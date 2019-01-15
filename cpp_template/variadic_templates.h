

#include <iostream>
#include <tuple>
#include <string>
#include <stdexcept>

// 打印变参的个数模板
template<typename... T>
void PrintArgCount(T... args)
{
	std::cout << "func PrintArgCount - arg count: " << sizeof...(args) << std::endl;
}

// 递归函数展开参数包是一种标准做法，必须要有一个同名的终止函数来终止递归
// 递归打印变参的参数模板
void PrintVariableParamet()
{
	//std::cout << "func PrintArgCount - empty" << std::endl;
}
// 递归展开函数
template <class T, class ...Args>
void PrintVariableParamet(T head, Args... rest)
{
	std::cout << "func PrintArgCount - parameter:" << head << std::endl;
	PrintVariableParamet(rest...);
}

// 递归可变参数求和模板
template<typename T>
T VariableParameterSum(T t)
{
	return t;
}
template<typename T, typename ... Types>
T VariableParameterSum(T first, Types ... rest)
{
	return first + VariableParameterSum<T>(rest...);
}

// 逗号表达式展开参数包
// c语言提供一种特殊的运算符，逗号运算符，优先级别最低，它将两个及其以上的式子联接起来，从左往右逐个计算表达式，
// 整个表达式的值为最后一个表达式的值。
// d = (a = b, c);  这个表达式会按顺序执行：b会先赋值给a，接着括号中的逗号表达式返回c的值，因此d将等于c。
template <class T>
void PrintParamet(T t)
{
	std::cout << "func PrintVariableParamet_2 - parameter:" << t << std::endl;
}

template <class ...Args>
void PrintVariableParamet_2(Args... args)
{
	int arr[] = { (PrintParamet(args), 0)... };

	//会创建一个元素值都为0的数组int arr[sizeof...(Args)]
	//由于是逗号表达式，在创建数组的过程中会先执行逗号表达式前面的部分 PrintParamet(args) 打印出参数，
	//也就是说在构造 int 数组的过程中就将参数包展开了，这个数组的目的纯粹是为了在数组构造的过程展开参数包。
	std::cout << "func PrintVariableParamet_2 - sizeof(arr):" << sizeof(arr) << ", sizeof(arr[0]) : " << sizeof(arr[0]) << std::endl;
}

// 函数作为参数，支持 lambda 表达式
template<class F, class... Args>
void PrintVariableParamet_3(const F& f, Args&&...args)
{
	//完美转发 std::forward
	std::initializer_list<int>{(f(std::forward<Args>(args)), 0)...};
}

// 可变模版参数类
std::tuple<double, char, std::string> get_student(int id)
{
	if (id == 0) return std::make_tuple(3.8, 'A', "Lisa Simpson");
	if (id == 1) return std::make_tuple(2.9, 'C', "Milhouse Van Houten");
	if (id == 2) return std::make_tuple(1.7, 'D', "Ralph Wiggum");
	throw std::invalid_argument("id");
}

void tuple_print_student()
{
	auto student0 = get_student(0);
	std::cout<<"func tuple_print_student - student - "
			<< "ID: 0, "
			<< "GPA: " << std::get<0>(student0) << ", "
			<< "grade: " << std::get<1>(student0) << ", "
			<< "name: " << std::get<2>(student0) << '\n';

	double gpa1;
	char grade1;
	std::string name1;
	std::tie(gpa1, grade1, name1) = get_student(1);
	std::cout << "func tuple_print_student - student - "
			<< "ID: 1, "
			<< "GPA: " << gpa1 << ", "
			<< "grade: " << grade1 << ", "
			<< "name: " << name1 << '\n';

	// C++17 structured binding:
	auto[gpa2, grade2, name2] = get_student(2);
	std::cout << "func tuple_print_student - student - "
			<< "ID: 2, "
			<< "GPA: " << gpa2 << ", "
			<< "grade: " << grade2 << ", "
			<< "name: " << name2 << '\n';
}

void test_variadic_templates()
{
	// 打印变参的个数
	PrintArgCount();
	PrintArgCount(1);
	PrintArgCount(1, 2);
	// 递归遍历可变参数
	PrintVariableParamet(1, 2, 3);
	// 递归可变参数求和
	int iParameterSumValue = VariableParameterSum(1, 2, 3, 4);
	std::cout << "func VariableParameterSum - value: " << iParameterSumValue << std::endl;
	// 逗号表达式展开参数包
	PrintVariableParamet_2(1, 2, 3);
	// C++ 11 lambda 表达式展开参数包
	PrintVariableParamet_3([](int i) {	std::cout << "func PrintVariableParamet_3 - parameter: " << i << std::endl; }, 1, 2, 3);
	// C++14 的新特性泛型 lambda 表达式的，更泛化的 lambda 表达式
	PrintVariableParamet_3([](auto i) {	std::cout << "func PrintVariableParamet_3 - parameter: " << i << std::endl; }, 1, 2.3, "hello");
	//可变参数模板
	tuple_print_student();
}




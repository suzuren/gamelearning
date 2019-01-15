#include <iostream>

//--------------------------------------------------------------------------------

#include <iostream>
#include <deque>
#include <list>
#include <vector>
#include <functional>

//先实现一个简单的函数，其返回值是一个Lambda表达式。其需要传入一个容器，并
//且返回一个函数对象，这个函数对象会以引用的方式捕获容器。且函数对象本身接受传
//入一个整型参数。当向函数对象传入一个整型时，表达式将会把传入的整型，添加到捕
//获的容器尾部
static auto consumer(auto &container)
{
	return [&](auto value) // 其中 & 以引用的方式捕获外部的 d l v 容器
	{
		container.push_back(value);
	};
}

static void print(const auto &c)
{
	for (auto i : c) {
		std::cout << i << ", ";
	}
	std::cout << '\n';
}

void tets_consumer()
{
	std::deque<int> d;
	std::list<int> l;
	std::vector<int> v;

	//	使用 consumer  函数对象与刚刚实例化的容器进行配合：我们将在 vector  中存
	//	储生成自定义的函数对象。然后，用一个 vector  存放着三个函数对象。每个函数对象都
	//	会捕获对应的容器对象。这些容器对象都是不同的类型，不过都是函数对象。所
	//	以， vector  中的实例类型为 std::function<void(int)>  。所有函数对象都将隐式转换成
	//	一个 std::function  对象，这样就可以存储在 vector  中了

	const std::vector<std::function<void(int)>> consumers{ consumer(d), consumer(l), consumer(v) };

	//  将10个整型值传入自定义函数对象
	for (size_t i{ 0 }; i < 10; ++i)
	{
		for (auto &&consume : consumers)
		{
			consume(i);
		}
	}

	std::cout << "tets_consumer d - ";
	print(d);
	std::cout << "tets_consumer l - ";
	print(l);
	std::cout << "tets_consumer v - ";
	print(v);
}

//--------------------------------------------------------------------------------

//	实现一个辅助函数 concat  ，其可以去任意多的参数。这些参数都是函数，比
//	如f，g和h。并且一个函数的结果是另一个函数的输入，可以写成 f(g(h(...)))
//	当用户提供函数f，g和h时，我们现将其转换为 f(
//	concat(g, h))  ，然后再是 f(g(concat(h)))  ，类似这样进行递归，直到得
//	到 f(g(h(...)))  为止。用户提供的这些函数都可以由Lambda表达式进行捕获，并且
//	Lambda表达式将在之后获得相应的参数p，然后前向执行这些函数 f(g(h(p)))  。这个
//	Lambda表达式就是我们要返回的。if constexpr结构会检查在递归步骤中，当前函数是否
//	串联了多个函数
//	到达递归的末尾，编译器会选择 if constexpr  的另一分支。这个例子中，我们只
//	是返回函数 t  ，因为其传入的只有参数了
template <typename T, typename ...Ts>
auto concat(T t, Ts ...ts)
{
	if constexpr (sizeof...(ts) > 0)
	{
		return [=](auto ...parameters)
		{
			return t(concat(ts...)(parameters...));
		};
	}
	else
	{
		return t;
	}
}

//	concat  会递归多次调用自己，每次调用参数都会减少：
template <typename T, typename ...Ts>
auto sub_concat(T t, Ts ...ts)
{
	if constexpr (sizeof...(ts) > 0)
	{
		return [=](auto ...parameters)
		{
			return t(concat(ts...)(parameters...));
		};
	}
	else
	{
		return [=](auto ...parameters)
		{
			return t(parameters...);
		};
	}
}

//简单点的版本，这次串联了三个函数：
template <typename F, typename G, typename H>
auto sig_concat(F f, G g, H h)
{
	return [=](auto ... params) {
		return f(g(h(params...)));
	};
}

void concatenat_func()
{
	auto twice([](int i) { return i * 2; });
	auto thrice([](int i) { return i * 3; });

	auto combined(concat(twice, thrice, std::plus<int>{}));

	//	串联他们。这里我们将两个乘法器函数和一个STL函数 std::plus<int>  放在一
	//	起，STL的这个函数可以接受两个参数，并返回其加和。这样我们就得到了函
	//	数 twice(thrice(plus(a, b)))

	std::cout << "concatenat_func - ";

	std::cout << combined(2, 3) << '\n';  //2 * 3 * (2 + 3)  为 30

}

//--------------------------------------------------------------------------------

//函数可以接受任意数量的参数，并且返回一个Lambda表达式，这个Lambda表达式只接受一个参数。表达式
//可以通过这个参数调用所有已提供的函数。这样，我们可以定义 auto call_all
//(multicall(f, g, h))  函数对象，然后调用 call_all(123)  ，从而达到同时调用 f(123);
//g(123); h(123);  的效果。这个函数看起来比较复杂，是因为我们需要一个语法技巧来展
//开参数包functions，并在 std::initializer_list  实例中包含一系列可调用的函数对象。

static auto multicall(auto ...functions)
{
	return [=](auto x)
	{
		(void)std::initializer_list<int>{((void)functions(x), 0)...};
	};
}

//能接受一个函数f和一个参数包 xs  。这里要表示的就是参数包中的每个参
//数都会传入f中运行。这种方式类似于 for_each(f, 1, 2, 3)  调用，从而会产生一系列调
//用—— f(1); f(2); f(3);  。本质上来说，这个函数使用同样的技巧来为函数展开参数使用同一输入调用多个函数包 xs

static auto for_each(auto f, auto ...xs)
{
	(void)std::initializer_list<int>{((void)f(xs), 0)...};
}

/*
我们刚刚实现的辅助函数还是挺复杂的。我们使用了 std::initializer_list  来帮助我们展开
参数包。为什么这里不用特殊的数据结构呢？再来看一下 for_each  的实现：
auto for_each ([](auto f, auto ...xs) {
(void)std::initializer_list<int>{
((void)f(xs), 0)...
};
});

这段代码的核心在于 f(xs)  表达式。 xs  是一个参数包，我们需要将其进行解包，才能获取
出独立的参数，以便调用函数f。不幸的是，我们知道这里不能简单的使用 ...  标记，写
成 f(xs)...  。

所以，我能做的只能是构造出一个 std::initializer_list  列表，其具有一个可变的构造函
数。表达式可以直接通过 return std::initializer_list<int>{f(xs)...};  方式构建，不过其也
有缺点。在让我们看一下 for_each  的实现，看起来要比之前简单许多：
auto for_each ([](auto f, auto ...xs) {
return std::initializer_list<int>{f(xs)...};
});

这看起来非常简单易懂，但是我们要了解其缺点所在：
1. 其使用f函数的所有调用返回值，构造了一个初始化列表。但我们并不关心返回值。
2. 虽然其返回的初始化列表，但是我们想要一个“即发即弃”的函数，这些函数不用返回任何东西。
3. f在这里可能是一个函数，因为其不会返回任何东西，可能在编译时就会被优化掉。

要想 for_each  修复上面所有的问题，会让其变的更加复杂。例子中做到了一下几点：
1. 不返回初始化列表，但会将所有表达式使用 (void)std::initializer_list<int>{...}  转换为void类型。
2. 初始化表达式中，其将 f(xs)...  包装进 (f(xs),0)...  表达式中。这会让程序将返回值完全抛弃，不过0将会放置在初始化列表中。
3.  f(xs)  在 (f(xs), 0)...  表达式中，将会再次转换成 void  ，所以这里就和没有返回值一样。

这些不幸的事导致例程如此复杂丑陋，不过其能为所有可变的函数对象工作，并且不管这些函数对象是否返回值，或返回什么样的值。
这种技术可以很好控制函数调用的顺序，严格保证多个函数/函数对象以某种顺序进行调用。

不推荐使用C风格的类型转换，因为C++有自己的转换操作。我们可以使
用 reinterpret_cast<void>(expression)  代替例程中的代码行，不过这样会降低代码的可
读性，会给后面的阅读者带来一些困扰。

*/



//brace_print  函数能接受两个字符，并返回一个新的函数对象，这个函数对象可以接受一
//个参数 x  。其将会打印这个参数，当然会让之前的两个字符将这个参数包围：
static auto brace_print(char a, char b)
{
	return [=](auto x) {	std::cout << a << x << b << ", ";	};
}

void test_may_fun_input()
{
	std::cout << "test_may_fun_input - " << std::endl;

	auto f(brace_print('(', ')'));
	auto g(brace_print('[', ']'));
	auto h(brace_print('{', '}'));
	auto nl([](auto) { std::cout << '\n'; });
	auto call_fgh(multicall(f, g, h, nl));
	for_each(call_fgh, 1, 2, 3, 4, 5);
}

//--------------------------------------------------------------------------------



int test_lambda()
{

	//-------------------------------------------------------------

	auto plus([](auto l, auto r) { return l + r; });
	std::cout << "test_lambda - " << plus(1, 2) << '\n';
	std::cout << "test_lambda - " << plus(std::string{ "a" }, "b") << '\n';

	//-------------------------------------------------------------
	// 可以不使用变量的方式对Lambda表达式进行保存。我们只需要在使用到的地方对其进行定义
	std::cout << "test_lambda - " << [](auto l, auto r) { return l + r; }(6, 2) << '\n';

	//-------------------------------------------------------------
	//函数对获取的值进行修改，使用mutable关键字对函数进行修饰
	// 不会捕获外部的任何变量，但是定义了一个新的 count  变量。其类型通过初始化的值的类型进行推断，由于初始化为0，所以其类型为 int  。
	auto counter([count = 0]() mutable { return ++count; });
	std::cout << "test_lambda - ";
	for (size_t i{ 0 }; i < 5; ++i)
	{
		std::cout << counter() << ", ";
	}
	std::cout << '\n';

	//-------------------------------------------------------------

	std::cout << "test_lambda - ";
	int x = 10;
	auto add_x = [x](int a) mutable { x *= 2; return a + x; };  // 复制捕捉x
	std::cout << "x:" << x << "  add_x:" << add_x(10) << std::endl; // 输出 10 30

	//-------------------------------------------------------------
	// 捕获已经存在的变量的引用，在闭包中进行修改。
	int a{ 0 };
	auto incrementer([&a] { ++a; });
	incrementer();
	incrementer();
	incrementer();
	std::cout << "test_lambda - ";
	std::cout << "Value of 'a' after 3 incrementer() calls: " << a << '\n';
	
	//-------------------------------------------------------------
	// 一个函数对象可以接受参数，并且将其传入另一个函数对象中进行保存 其中 [=] 捕获的是外部 plus 函数的地址
	std::cout << "test_lambda - ";
	auto plus_ten([=](int x) { return plus(10, x); });
	std::cout << plus_ten(5) << '\n';

	//-------------------------------------------------------------
	//创建很多Lambda表达式，其捕获类型是完全不同的，但是其函数签名的类型是相同的
	tets_consumer();
	//-------------------------------------------------------------
	//使用Lambda表达式来模仿并置函数
	concatenat_func();

	//-------------------------------------------------------------
	//使用同一输入调用多个函数
	test_may_fun_input();

	//-------------------------------------------------------------


	return 0;
}





/*

//-------------------------------------------------------------

Lambda表达式的特点：

[capture list] (parameters)
	mutable (optional)
	constexpr (optional)
	exception attr (optional)
	-> return type (optional)
{
	body
}

Lambda表达式的最短方式可以写为 []{}  。其没有参数，没有捕获任何东西，并且也不做实质性的执行。

//-------------------------------------------------------------

捕获列表 capture list
指定我们需要捕获什么。其由很多种方式，我们展示两种比较“懒惰”的方式：
	当我们将Lambda表达式写成 [=] () {...}  时，我们会捕获到外部所有变量的副本。
	当我们将Lambda表达式写成 [&] () {...}  时，我们会捕获到外部所有变量的引用。
当然，我们也可以在捕获列表中单独的去写需要捕获的变量。比如 [a, &b] () {...}  ，就是
捕获 a  的副本和 b  的引用。这样的话，就需要很多的字，不过这通常会很安全，这样捕获列
表就不会去捕获那些不需要捕获的变量。


通过捕获列表捕获变量的副本和/或引用：
	[a, &b] () {...}				：捕获 a  的副本和 b  的引用。
	[&, a] () {...}					：除了捕获 a  为副本外，其余捕获的变量皆为引用。
	[=, &b, i{22}, this] () {...}	：捕获 b  的引用， this  的副本，并将新变量 i  初始化成 22，并且其余捕获的变量都为其副本。
	Note:
	当你需要捕获一个对象的成员变量时，不能直接去捕获成员变量。需要先去捕获对象的 this  指针或引用。

//-------------------------------------------------------------

mutable (optional)
	当函数对象需要去修改通过副本传入的变量时，表达式必须用 mutable  修饰。这就相当于对
	捕获的对象使用非常量函数。

//-------------------------------------------------------------

constexpr (optional)
如果我们显式的将Lambda表达式修饰为 constexpr  ，那么编译器将不会通过编译，因为其不
满足 constexpr  函数的标准。 constexpr  函数有很多条件，多以编译器会在编译时对Lambda
表达式进行评估，看其在编译时是否为一个常量参数。这样就会让程序的二进制文件体积减
少很多。
当我们不显式的将Lambda表达式声明为 constexpr  时，编译器就会自己进行判断，如果满足
条件那么会将Lambda表达式隐式的声明为 constexpr  。当我们需要一个Lambda表达式
为 constexpr  时，我们最好显式的对Lambda的表达式进行声明，当编译不通过时，编译器会
告诉我们哪里做错了。

//-------------------------------------------------------------

exception attr (optional)
这里指定在调用和运行错误时，是否抛出异常。

//-------------------------------------------------------------

return type (optional)
当我们想完全控制返回类型时，我们不会让编译器来做类型推导。我们可以写成这样 [] () -> Foo {}  ，
这样就告诉编译器，这个Lambda表达式总是返回 Foo  类型的结果。

//-------------------------------------------------------------

有以下几种情况来捕获其所在作用域中的变量：

[]		：默认不捕获任何变量；
[=]		：默认以值捕获所有变量；
[&]		：默认以引用捕获所有变量；
[x]		：仅以值捕获 x，其它变量不捕获；
[&x]	：仅以引用捕获 x，其它变量不捕获；
[=, &x]	：默认以值捕获所有变量，但是 x 是例外，通过引用捕获；
[&, x]	：默认以引用捕获所有变量，但是 x 是例外，通过值捕获；
[this]	：通过引用捕获当前对象（其实是复制指针）；
[*this]	：通过传值方式捕获当前对象；



//-------------------------------------------------------------

*/
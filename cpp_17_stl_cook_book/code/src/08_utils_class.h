

#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>
#include <string>
#include <cstdlib>
#include <any>
#include <variant>

#include <vector>
#include <random>
#include <iomanip>
#include <limits>
#include <algorithm>

template <class T>
std::string type_name()
{
	typedef typename std::remove_reference<T>::type TR;
	std::unique_ptr<char, void(*)(void*)> own(abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr), std::free);
	std::string r = own != nullptr ? own.get() : typeid(TR).name();
	if (std::is_const<TR>::value)
	{
		r += " const";
	}
	if (std::is_volatile<TR>::value)
	{
		r += " volatile";
	}
	if (std::is_lvalue_reference<T>::value)
	{
		r += "&";
	}
	else if (std::is_rvalue_reference<T>::value)
	{
		r += "&&";
	}
	return r;
}

//--------------------------------------------------------------------------------

#include <iostream>

#include <chrono>
#include <ratio>
#include <cmath>
#include <iomanip>
#include <optional>

//--------------------------------------------------------------------------------

//std::chrono::duration  经常用来表示所用时间的长度，其为秒的倍数或小数。所有STL的持续
//都由整型类型进行特化。本节中，我们将使用 double  进行特化。

//类 std::chrono::steady_clock 表示单调时钟。此时钟的时间点无法减少，因为物理时间向前移动。
//此时钟与壁钟时间无关（例如，它能是上次重启开始的时间），且最适于度量间隔。

/*

system_clock  ， steady_clock  和 high_resolution_clock  ，这三个时钟对象都
在 std::chrono  命名空间中。他们之间的区别

时钟类型 特性

system_clock
其表示系统级别的实时挂钟。想要获取本地时间的话，这是个正确的选择。

steady_clock
该时钟表示单调型的时间。这就表示这个时间是不可能倒退
的。而时间倒退可能会在其他时钟上发生，当其最小精度不
同，或是在冬令时和夏令时交替时。


high_resolution_clock STL中可统计最细粒度时钟周期的时钟。

*/


using seconds = std::chrono::duration<double>;
using milliseconds = std::chrono::duration<double, std::ratio_multiply<seconds::period, std::milli>>;
using microseconds = std::chrono::duration<double, std::ratio_multiply<seconds::period, std::micro>>;

static std::pair<std::string, seconds> get_input()
{
	
	const auto tic(std::chrono::steady_clock::now());

	std::string s{ "C++17" };
	//if (!(std::cin >> s))
	//{
	//	return{ {},{} };
	//}
	const auto toc(std::chrono::steady_clock::now());

	//std::cout << "\n C++17 begin:" << tic << "\n C++17 end:" << toc << std::endl;

	return{ s, toc - tic };
}

void ratio_conversion()
{
	while (true)
	{
		std::cout << "Please type the word \"C++17\" as fast as you can.\n> ";
		const auto[user_input, diff] = get_input();
		if (user_input == "")
		{
			break;
		}
		if (user_input == "C++17")
		{
			std::cout << "Bravo. You did it in:\n"
				<< std::fixed << std::setprecision(2)
				<< std::setw(12) << diff.count() << " seconds.\n"
				<< std::setw(12) << milliseconds(diff).count() << " milliseconds.\n"
				<< std::setw(12) << microseconds(diff).count() << " microseconds.\n";
			break;
		}
		else
		{
			std::cout << "Sorry, your input does not match. You may try again.\n";
		}
	}

	//std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
	//std::cout << "24 hours ago, the time was " << std::put_time(std::localtime(&now_c), "%F %T") << '\n';

	//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	//std::cout << "Hello World\n";
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "Printing took "<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<< " us.\n";

}

/*
Please type the word "C++17" as fast as you can.
> Bravo. You did it in:
0.00 seconds.
0.00 milliseconds.
0.59 microseconds.

//24 hours ago, the time was 2019-01-21 18:55:37
//Hello World
//Printing took 1 us.

*/
//--------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream &os,const std::chrono::time_point<std::chrono::system_clock> &t)
{
	const auto tt(std::chrono::system_clock::to_time_t(t));
	const auto loct(std::localtime(&tt));
	return os << std::put_time(loct, "%c");
}

using days = std::chrono::duration<std::chrono::hours::rep, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>>;
//days - std::chrono::duration<long, std::ratio<86400l, 1l>

constexpr days operator "" _days(unsigned long long h)
{
	return days{ h };
}

void relative_absolute_times()
{
	std::chrono::system_clock::time_point now_0(std::chrono::system_clock::now());
	std::chrono::time_point<std::chrono::system_clock> now(std::chrono::system_clock::now());
	//std::cout << "The current date and time is " << now <<'\n';

	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::cout << "The current date and time is " << std::put_time(std::localtime(&now_c), "%c") << '\n';
	std::cout << "The current date and time is " << std::put_time(std::localtime(&now_c), "%F %T") << '\n';
	std::cout << "The current date and time is " << std::put_time(std::localtime(&now_c), "%y_%m_%d %T") << '\n';


	std::chrono::hours chrono_12h{ 12 };
	std::cout << "In 12 hours, it will be " << (now + chrono_12h) << '\n';
	using namespace std::chrono_literals;

	std::cout << "1_ 12 hours and 15 minutes ago, it was " << (now - 12h - 15min)
		<< " ,1 week ago, it was " << (now - 7_days) << '\n';

	std::cout << "2_ 12 hours and 15 minutes ago, it was " << (now - (std::chrono::hours(12)) - std::chrono::minutes(15))
		<< " ,1 week ago, it was " << (now - 7_days) << '\n';


	printf("test_typename - type_name:%s\n", type_name<int const>().c_str());
	printf("test_typename - type_name:%s\n", type_name<days>().c_str());
	printf("test_typename - type_name:%s\n", type_name<std::pair<int, char>>().c_str());
	printf("test_typename - type_name:%s\n", type_name<std::chrono::system_clock::time_point>().c_str());
	printf("test_typename - type_name:%s\n", type_name<std::chrono::time_point<std::chrono::system_clock>>().c_str());
	
	//printf("test_typename - type_name:%s\n", type_name<12h>().c_str());
	//printf("test_typename - type_name:%s\n", type_name<15min>().c_str());

}

/*

The current date and time is Wed Jan 23 14:54:56 2019
The current date and time is 2019-01-23 14:54:56
The current date and time is 19_01_23 14:54:56
In 12 hours, it will be Thu Jan 24 02:54:56 2019
1_ 12 hours and 15 minutes ago, it was Wed Jan 23 02:39:56 2019 ,1 week ago, it was Wed Jan 16 14:54:56 2019
2_ 12 hours and 15 minutes ago, it was Wed Jan 23 02:39:56 2019 ,1 week ago, it was Wed Jan 16 14:54:56 2019
test_typename - type_name:int const
test_typename - type_name:std::chrono::duration<long, std::ratio<86400l, 1l> >
test_typename - type_name:std::pair<int, char>
test_typename - type_name:std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long, std::ratio<1l, 1000000000l> > >
test_typename - type_name:std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long, std::ratio<1l, 1000000000l> > >

*/

//--------------------------------------------------------------------------------

static void print_student(size_t id, const std::string &name,double gpa)
{
	std::cout << "Student " << std::quoted(name)
		<< ", ID: " << id
		<< ", GPA: " << gpa << '\n';
}

void apply_functions_on_tuples()
{
	using student = std::tuple<size_t, std::string, double>;
	student john{ 123, "John Doe", 3.7 };

	const auto &[id, name, gpa] = john;
	print_student(id, name, gpa);

	std::cout << "-----\n";

	auto arguments_for_later = {
		std::make_tuple(234, "John Doe", 3.7),
		std::make_tuple(345, "Billy Foo", 4.0),
		std::make_tuple(456, "Cathy Bar", 3.5),
	};

	for (const auto &[id, name, gpa] : arguments_for_later) {
		print_student(id, name, gpa);
	}
	std::cout << "-----\n";

	std::apply(print_student, john);
	std::cout <<" -----\n";

	for (const auto &args : arguments_for_later) {
		std::apply(print_student, args);
	}
	std::cout << "-----\n";
}

//--------------------------------------------------------------------------------

template <typename T, typename ... Ts>
void print_args(std::ostream &os, const T &v, const Ts &...vs)
{
	os << v;
	(void)std::initializer_list<int>{((os << ", " << vs), 0)...};
}

template <typename ... Ts>
std::ostream& operator<<(std::ostream &os, const std::tuple<Ts...> &t)
{
	auto print_to_os([&os](const auto &...xs)
	{
		print_args(os, xs...);
	});
	os << "(";
	std::apply(print_to_os, t);
	return os << ")";
}

//std::minmax_element  会返回一对迭代器，其分别表示输入范围内的最小值和最大值
//std::accumulate  将会返回输入范围内所有值的加和
template <typename T>
std::tuple<double, double, double, double>
sum_min_max_avg(const T &range)
{
	auto min_max(std::minmax_element(std::begin(range), std::end(range)));
	auto sum(std::accumulate(std::begin(range), std::end(range), 0.0));
	return{ sum, *min_max.first, *min_max.second,sum / range.size() };
}

template <typename T1, typename T2>
static auto zip(const T1 &a, const T2 &b)
{
	auto z([](auto ...xs)
	{
		return[xs...](auto ...ys)
		{
			return std::tuple_cat(std::make_tuple(xs, ys) ...);
		};
	});
	return std::apply(std::apply(z, a), b);
}

void test_tuple()
{
	auto student_desc(std::make_tuple("ID", "Name", "GPA"));
	auto student(std::make_tuple(123456, "John Doe", 3.7));
	std::cout << student_desc << '\n' << student << '\n';

	std::cout << std::tuple_cat(student_desc, student) << '\n';

	auto zipped(zip(student_desc, student));
	std::cout << zipped << '\n';

	auto numbers = { 0.0, 1.0, 2.0, 3.0, 4.0 };
	std::cout << zip(std::make_tuple("Sum", "Minimum", "Maximum", "Average"), sum_min_max_avg(numbers)) << '\n';

	std::tuple<int, int, int> int_tuple= {1,2,3};
	std::tuple<std::string, std::string, std::string> str_tuple= { "a","b","c" };

	auto str_int_tuple(std::tuple_cat(int_tuple, str_tuple));
	std::cout << str_int_tuple << '\n';

}

/*

(ID, Name, GPA)
(123456, John Doe, 3.7)
(ID, Name, GPA, 123456, John Doe, 3.7)
(ID, 123456, Name, John Doe, GPA, 3.7)
(Sum, 10, Minimum, 0, Maximum, 4, Average, 2)

*/
//--------------------------------------------------------------------------------

using int_list = std::list<int>;
void print_anything(const std::any &a)
{
	if (!a.has_value())
	{
		std::cout << "Nothing.\n";
	}
	else if (a.type() == typeid(std::string))
	{
		// 使用 std::any_cast  将a转化成一个string类型的引用
		std::cout << "It's a string: " << std::quoted(std::any_cast<const std::string&>(a)) << '\n';
	}
	else if (a.type() == typeid(char))
	{
		// 使用 std::any_cast  将a转化成一个char *类型
		std::cout << "It's a char: " << std::any_cast<char>(a) << '\n';
	}
	else if (a.type() == typeid(int))
	{
		// 使用 any_cast<int>  将a转换成 int  型数值
		std::cout << "It's an integer: " << std::any_cast<int>(a) << '\n';
	}
	else if (a.type() == typeid(int_list))
	{
		const auto &l(std::any_cast<const int_list&>(a));
		std::cout << "It's a list: ";
		std::copy(std::begin(l), std::end(l), std::ostream_iterator<int>{std::cout, ", "});
		std::cout << '\n';
	}
	else
	{
		std::cout << "typename:" << a.type().name() << " hash_code:" << a.type().hash_code() << " Can't handle this item.\n";
	}
}
void test_any()
{
	print_anything({});
	print_anything(std::string("abc"));
	print_anything('y');
	print_anything("dfg");
	print_anything(123);
	print_anything(int_list{ 1, 2, 3 });
	print_anything(std::any(std::in_place_type_t<int_list>{}, { 1, 2,3 }));
}

/*

Nothing.
It's a string: "abc"
It's a char: y
typename:PKc hash_code:3698062409364629473 Can't handle this item.
It's an integer: 123
It's a list: 1, 2, 3,
It's a list: 1, 2, 3,

*/
//--------------------------------------------------------------------------------

class cat
{
	std::string name;
public:
	cat(std::string n):name{ n }
	{
	}
	void meow() const
	{
		std::cout << name << " says Meow!\n";
	}
};

class dog
{
	std::string name;
public:
	dog(std::string n):name{ n }
	{
	}
	void woof() const
	{
		std::cout << name << " says Woof!\n";
	}
};

using animal = std::variant<dog, cat>;

template <typename T>
bool is_type(const animal &a)
{
	return std::holds_alternative<T>(a);
}

// 看起来像是一个函数对象。其实际是一个双重函数对
// 象，因为其 operator()  实现了两次。一种实现是接受 dog  作为参数输入，另一个实现是
// 接受 cat  类型作为参数输入。对于两种实现，其会调用 woof  或 meow  函数

struct animal_voice
{
	void operator()(const dog &d) const { d.woof(); }
	void operator()(const cat &c) const { c.meow(); }
};


void test_variant()
{
	std::list<animal> l{ cat{ "Tuba" }, dog{ "Balou" }, cat{ "Bobby" } };
	for (const animal &a : l)
	{
		switch (a.index())
		{
			case 0:
			{
				std::get<dog>(a).woof();
			}break;
			case 1:
			{
				std::get<cat>(a).meow();
			}break;
		}
	}
	std::cout << "-----\n";
	for (const animal &a : l)
	{
		if (const auto d(std::get_if<dog>(&a)); d != nullptr)
		{
			d->woof();
		}
		else if (const auto c(std::get_if<cat>(&a)); c != nullptr)
		{
			c->meow();
		}
	}
	std::cout << "-----\n";

	for (const animal &a : l)
	{
		std::visit(animal_voice{}, a);
	}
	std::cout << "-----\n";

	std::cout << "There are "
		<< std::count_if(std::begin(l), std::end(l), is_type<cat>)
		<< " cats and "
		<< std::count_if(std::begin(l), std::end(l), is_type<dog>)
		<< " dogs in the list.\n";
}

/*

Tuba says Meow!
Balou says Woof!
Bobby says Meow!
-----
Tuba says Meow!
Balou says Woof!
Bobby says Meow!
-----
Tuba says Meow!
Balou says Woof!
Bobby says Meow!
-----
There are 2 cats and 1 dogs in the list.

*/
//--------------------------------------------------------------------------------

template <typename RD>
void histogram(size_t partitions, size_t samples)
{
	using rand_t = typename RD::result_type;
	partitions = std::max<size_t>(partitions, 10);
	RD rd;
	rand_t div((double(RD::max()) + 1) / partitions);
	std::vector<size_t> v(partitions);
	for (size_t i{ 0 }; i < samples; ++i)
	{
		++v[rd() / div];
	}
	rand_t max_elm(*std::max_element(std::begin(v), std::end(v)));
	rand_t max_div(std::max(max_elm / 100, rand_t(1)));
	for (size_t i{ 0 }; i < partitions; ++i)
	{
		std::cout << std::setw(2) << i << ": " << std::string(v[i] / max_div, '*') << '\n';
	}
}

void random_generator()
{
	size_t partitions{ std::stoull("10") };
	size_t samples{ std::stoull("1000") };

	std::cout << "random_device" << '\n';
	histogram<std::random_device>(partitions, samples);

	std::cout << "ndefault_random_engine" << '\n';
	histogram<std::default_random_engine>(partitions, samples);

	std::cout << "nminstd_rand0" << '\n';
	histogram<std::minstd_rand0>(partitions, samples);

	std::cout << "nminstd_rand" << '\n';
	histogram<std::minstd_rand>(partitions, samples);

	std::cout << "nmt19937" << '\n';
	histogram<std::mt19937>(partitions, samples);

	std::cout << "nmt19937_64" << '\n';
	histogram<std::mt19937_64>(partitions, samples);

	std::cout << "nranlux24_base" << '\n';
	histogram<std::ranlux24_base>(partitions, samples);

	std::cout << "nranlux48_base" << '\n';
	histogram<std::ranlux48_base>(partitions, samples);

	std::cout << "nranlux24" << '\n';
	histogram<std::ranlux24>(partitions, samples);

	std::cout << "nranlux48" << '\n';
	histogram<std::ranlux48>(partitions, samples);

	std::cout << "nknuth_b" << '\n';
	histogram<std::knuth_b>(partitions, samples);
}

/*

1. 通常，选择使用 std::default_random_engine  就够用了。
2. 需要生成不确定的随机数时，我们可以使用 std::random_device  。
3. 通过 std::random_device  (或从系统时钟获取的时间戳)对随机数引擎进行初始化，这是为
了让其在每次调用时，生成不同的随机数。这种方式也叫做“设置种子”。

*/

//--------------------------------------------------------------------------------


template <typename T>
void print_distro(T distro, size_t samples)
{
	std::default_random_engine e;
	std::map<int, size_t> m;
	for (size_t i{ 0 }; i < samples; ++i)
	{
		m[distro(e)] += 1;
	}
	size_t max_elm(std::max_element(std::begin(m), std::end(m), [](const auto &a, const auto &b) {	return a.second < b.second; })->second);
	size_t max_div(std::max(max_elm / 100, size_t(1)));
	for (const auto[randval, count] : m)
	{
		if (count < max_elm / 200)
		{
			continue;
		}
		std::cout << std::setw(3) << randval << " : " << std::string(count / max_div, '*') << '\n';
	}
}

void random_distro()
{
	size_t samples{ std::stoull("1000") };
	std::cout << "uniform_int_distribution\n";
	print_distro(std::uniform_int_distribution<int>{0, 9}, samples);

	std::cout << "normal_distribution\n";
	print_distro(std::normal_distribution<double>{0.0, 2.0}, samples);
	std::initializer_list<double> intervals{ 0, 5, 10, 30 };
	std::initializer_list<double> weights{ 0.2, 0.3, 0.5 };

	std::cout << "piecewise_constant_distribution\n";
	print_distro(std::piecewise_constant_distribution<double>{std::begin(intervals), std::end(intervals), std::begin(weights)}, samples);
	
	std::cout << "piecewise_linear_distribution\n";
	std::initializer_list<double> weights2{ 0, 1, 1, 0 };
	print_distro(std::piecewise_linear_distribution<double>{std::begin(intervals), std::end(intervals), std::begin(weights2)}, samples);

	std::cout << "bernoulli_distribution\n";
	print_distro(std::bernoulli_distribution{ 0.75 }, samples);

	std::cout << "discrete_distribution\n";
	print_distro(std::discrete_distribution<int>{ {1, 2, 4, 8} },samples);

	std::cout << "binomial_distribution\n";
	print_distro(std::binomial_distribution<int>{10, 0.3},samples);

	std::cout << "negative_binomial_distribution\n";
	print_distro(std::negative_binomial_distribution<int>{10, 0.8},samples);

	std::cout << "geometric_distribution\n";
	print_distro(std::geometric_distribution<int>{0.4}, samples);

	std::cout << "exponential_distribution\n";
	print_distro(std::exponential_distribution<double>{0.4},samples);

	std::cout << "gamma_distribution\n";
	print_distro(std::gamma_distribution<double>{1.5, 1.0},samples);

	std::cout << "weibull_distribution\n";
	print_distro(std::weibull_distribution<double>{1.5, 1.0},samples);

	std::cout << "extreme_value_distribution\n";
	print_distro(std::extreme_value_distribution<double>{0.0, 1.0},samples);

	std::cout << "lognormal_distribution\n";
	print_distro(std::lognormal_distribution<double>{0.5, 0.5},samples);

	std::cout << "chi_squared_distribution\n";
	print_distro(std::chi_squared_distribution<double>{1.0},samples);

	std::cout << "cauchy_distribution\n";
	print_distro(std::cauchy_distribution<double>{0.0, 0.1},samples);

	std::cout << "fisher_f_distribution\n";
	print_distro(std::fisher_f_distribution<double>{1.0, 1.0},samples);

	std::cout << "student_t_distribution\n";
	print_distro(std::student_t_distribution<double>{1.0},samples);
}

/*

分布 描述

uniform_int_distribution
该分布接受一组上下限值作为构造函数的参数。之后我们得到
的随机值就都是在这个范围中。我们可能得到的每个值的可能
性是相同的，我们的直方图将会是一个平坦的图像。这个分布
就像是掷骰子，因为掷到骰子的每一个面的概率都是相等的。

normal_distribution
称为正态分布或高斯分布，在自然界中几乎无处不在。其STL
版本能接受一个平均值和一个标准差作为构造函数的参数，其
直方图的形状像是屋顶一样。其分布于人类个体高度或动物的
IQ值，或学生的成绩，都与这个分布相符。

bernoulli_distribution
当我们想要一个掷硬币的结果时，使用伯努利分布就非常完
美。其只会产生0和1，并且其构造函数的参数是产生1的概
率。

discrete_distribution
当我们有一些限制的时候，我们就可以使用离散分布，需要我
们为每个间隔的概率进行定义，从而得到离散集合的值。其构
造函数会接受一个权重值类别，并且通过权重值的可能性产生
相应的随机数。当我们想要对血型进行随机建模时，每种血型
的概率是不一样，这样这种引擎就能很完美的契合这种需求。

*/

//--------------------------------------------------------------------------------

void test_random_vector()
{
	std::random_device rd;
	std::mt19937 g{ rd() };

	std::cout << "random_vector - " << '\n';

	int loop_index = 0;
	do
	{
		std::vector<int> vecRand{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		std::shuffle(std::begin(vecRand), std::end(vecRand), g);
		std::copy(std::begin(vecRand), std::end(vecRand), std::ostream_iterator<int>{std::cout, ", "});
		std::cout << '\n';
		loop_index++;
	} while (loop_index < 10);
}

//--------------------------------------------------------------------------------

int test_utils_class()
{
	//--------------------------------------------------------------------------------
	//01
	//ratio_conversion();

	//--------------------------------------------------------------------------------
	//02
	//relative_absolute_times();

	//--------------------------------------------------------------------------------
	//03
	//apply_functions_on_tuples();

	//--------------------------------------------------------------------------------
	//04
	//test_tuple();

	//--------------------------------------------------------------------------------
	//05
	//test_any();

	//--------------------------------------------------------------------------------
	//06
	//test_variant();

	//--------------------------------------------------------------------------------
	//07
	//random_generator();

	//--------------------------------------------------------------------------------
	//08
	//random_distro();

	//--------------------------------------------------------------------------------
	//09
	test_random_vector();

	return 0;
}



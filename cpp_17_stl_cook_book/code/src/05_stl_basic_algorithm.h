
//--------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <iterator>
#include <algorithm>
//#include<bits/stdc++.h>
#include <random>
#include <sstream>

//--------------------------------------------------------------------------------

namespace std
{
	std::ostream& operator<<(std::ostream &os, const std::pair<int, std::string> &p)
	{
		return os << "(" << p.first << ", " << p.second << ")";
	}
}

//--------------------------------------------------------------------------------

static void print(const std::vector<int> &v)
{
	std::copy(std::begin(v), std::end(v), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';
}

//--------------------------------------------------------------------------------

struct city
{
	std::string name;
	unsigned population;
};

bool operator==(const city &a, const city &b)
{
	return a.name == b.name && a.population == b.population;
}

std::ostream& operator<<(std::ostream &os, const city &city)
{
	return os << "{" << city.name << ", " << city.population << "}";
}

template <typename C>
static auto opt_print(const C &container)
{
	return[end_it(std::end(container))](const auto &item)
	{
		if (item != end_it)
		{
			std::cout << *item << '\n';
		}
		else
		{
			std::cout << "<end>\n";
		}
	};
}

void stl_finding_items()
{
	const std::vector<city> c{
		{ "Aachen", 246000 },
		{ "Berlin", 3502000 },
		{ "Braunschweig", 251000 },
		{ "Cologne", 1060000 }
	};

	//std::cout << "stl_finding_items - ";
	auto print_city(opt_print(c));
	auto found_cologne(std::find(std::begin(c), std::end(c),city{ "Cologne", 1060000 }));
	std::cout << "stl_finding_items - ";
	print_city(found_cologne);

	std::cout << "stl_finding_items - ";
	auto found_cologne_2(std::find_if(std::begin(c), std::end(c),[](const auto &item) {return item.name == "Aachen";}));
	print_city(found_cologne_2);


	auto population_more_than([](unsigned i)
	{		return [=](const city &item)  // [=] 按值捕获 i ，item 是 find_if 循环的时候传递进去的
			{
				return item.population > i;
			};
	});

	auto found_large(std::find_if(std::begin(c), std::end(c),population_more_than(2000000)));
	std::cout << "stl_finding_items - ";
	print_city(found_large);


	const std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	auto print_int(opt_print(v));
	// 对于要成功的 std::binary_search ，范围 [first, last) 必须至少相对于 value 部分有序
	bool contains_7{ std::binary_search(std::begin(v), std::end(v), 7) };
	std::cout << "stl_finding_items - contains_7 - ";
	std::cout << contains_7 << '\n';

	// std::equal_range  。其不会返回对应元素的迭代器给我们，不过会返回一组迭代器。
	//第一个迭代器是指向第一个不小于给定值的元素。第二个迭代器指向第一个大于给定值
	//的元素。我们的范围为数字1到10，那么第一个迭代器将指向7，因为其是第一个不小于7
	//的元素。第二个迭代器指向8，因为其实第一个大于7的元素
	auto[lower_it, upper_it](std::equal_range(std::begin(v), std::end(v), 7));
	std::cout << "stl_finding_items - lower_it - ";
	print_int(lower_it);
	std::cout << "stl_finding_items - upper_it - ";
	print_int(upper_it);
	
	//lower_bound  函数只会返回第一个迭代器，	而 upper_bound  则会返回第二个迭代器
	std::cout << "stl_finding_items - lower_it - lower_it - ";
	print_int(std::lower_bound(begin(v), end(v), 7));

	std::cout << "stl_finding_items - upper_it - upper_it - ";
	print_int(std::upper_bound(begin(v), end(v), 7));

	//stl_finding_items - {Cologne, 1060000}
	//stl_finding_items - {Aachen, 246000}
	//stl_finding_items - {Berlin, 3502000}
	//stl_finding_items - contains_7 - 1
	//	stl_finding_items - lower_it - 7
	//	stl_finding_items - upper_it - 8
	//	stl_finding_items - lower_it - lower_it - 7
	//	stl_finding_items - upper_it - upper_it - 8


}
//--------------------------------------------------------------------------------

static auto norm(int min, int max, int new_max)
{
	const double diff(max - min);
	return [=](int val)
	{
		return int((val - min) / diff * new_max);
	};
}

static auto clampval(int min, int max)
{
	return [=](int val) -> int
	{
		return std::clamp(val, min, max);
	};
}

void stl_reducing_range_in_vector()
{
	std::vector<int> v{ 0, 1000, 5, 250, 300, 800, 900, 321 };

	// std::minmax_element  函数将帮助我们获得这两个值 - 最大值和最小值
	const auto[min_it, max_it](std::minmax_element(std::begin(v), std::end(v)));
	std::vector<int> v_norm;
	v_norm.reserve(v.size());
	//使用 std::transform  从第一个 vector  拷贝到第二个 vector
	std::transform(std::begin(v), std::end(v), std::back_inserter(v_norm),norm(*min_it, *max_it, 255));

	std::cout << "stl_reducing_range_in_vector - ";
	std::copy(std::begin(v_norm), std::end(v_norm),	std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';
	std::transform(std::begin(v), std::end(v), std::begin(v_norm),clampval(0, 255));

	std::cout << "stl_reducing_range_in_vector - ";
	std::copy(std::begin(v_norm), std::end(v_norm), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';

	//stl_reducing_range_in_vector - 0, 255, 1, 63, 76, 204, 229, 81, 
	//stl_reducing_range_in_vector - 0, 255, 5, 250, 255, 255, 255, 255,


}

//--------------------------------------------------------------------------------

template <typename Itr>
static void print(Itr it, size_t chars)
{
	std::copy_n(it, chars, std::ostream_iterator<char>{std::cout});
	std::cout << '\n';
}

void stl_pattern_search_string()
{
	const std::string long_string{
		"Lorem ipsum dolor sit amet, consetetur"
		" sadipscing elitr, sed diam nonumy eirmod" };
	const std::string needle{ "elitr" };

	auto match(std::search(std::begin(long_string), std::end(long_string), std::begin(needle), std::end(needle)));
	std::cout << "stl_pattern_search_string - match_1 -";
	print(match, 5);
	std::cout << "stl_pattern_search_string - match_2 -";
	auto match_2(std::search(std::begin(long_string), std::end(long_string), std::default_searcher(std::begin(needle), std::end(needle))));
	print(match_2, 5);
	std::cout << "stl_pattern_search_string - match_3 -";
	auto match_3(std::search(std::begin(long_string), std::end(long_string), std::boyer_moore_searcher(std::begin(needle), std::end(needle))));
	print(match_3, 5);

	std::cout << "stl_pattern_search_string - match_4 -";
	auto match_4(std::search(std::begin(long_string), std::end(long_string), std::boyer_moore_horspool_searcher(std::begin(needle), std::end(needle))));
	print(match_4, 5);

	//stl_pattern_search_string - match_1 -elitr
	//stl_pattern_search_string - match_2 - elitr
	//stl_pattern_search_string - match_3 - elitr
	//stl_pattern_search_string - match_4 - elitr

}

//--------------------------------------------------------------------------------

int test_stl_basic_algorithm()
{
	//01
	//-------------------------------------------------------------
	std::vector<std::pair<int, std::string>> v{ { 1, "one" },{ 2, "two" },{ 3, "three" }, { 4, "four" },{ 5, "five" } };
	std::map<int, std::string> m;
	std::copy_n(std::begin(v), 3, std::inserter(m, std::begin(m)));

	//std::cout << "test_stl_basic_algorithm - ";
	//for (const auto &[key, value] : m)
	//{
	//	std::cout << "(key:" << key << ",value:" << value << ") ";
	//}
	//std::cout << '\n';

	std::cout << "test_stl_basic_algorithm - copying_items m";
	auto shell_it(std::ostream_iterator<std::pair<int, std::string>>{std::cout, ", "});
	std::copy(std::begin(m), std::end(m), shell_it);
	std::cout << '\n';
	std::cout << "test_stl_basic_algorithm - copying_items v";
	std::copy(std::begin(v), std::end(v), shell_it);
	std::cout << '\n';

	//	test_stl_basic_algorithm - m(1, one), (2, two), (3, three),
	//	test_stl_basic_algorithm - v(1, one), (2, two), (3, three), (4, four), (5, five),


	//02
	//-------------------------------------------------------------
	m.clear();
	std::move(std::begin(v), std::end(v), std::inserter(m, std::begin(m)));

	std::cout << "test_stl_basic_algorithm - copying_items m";
	std::copy(std::begin(m), std::end(m), shell_it);
	std::cout << '\n';
	std::cout << "test_stl_basic_algorithm - copying_items v";
	std::copy(std::begin(v), std::end(v), shell_it);
	std::cout << '\n';

	//	test_stl_basic_algorithm - m(1, one), (2, two), (3, three), (4, four), (5, five),
	//	test_stl_basic_algorithm - v(1, ), (2, ), (3, ), (4, ), (5, ),
	//因为移动算法将其源搬移		到 map  中，所以这时的 vector  是空的。
	//在重新分配空间前，我们通常不应该访问成为移动源的项，但是为了这个实验，我们忽略它


	// 03
	//-------------------------------------------------------------

	//std::cout << "test_stl_basic_algorithm - sorting_containers - ";

	std::vector<int> vecRand{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::random_device rd;
	std::mt19937 g{ rd() };
	//std::cout << "test_stl_basic_algorithm - sorting_containers - g:" << g << std::endl;

	std::cout << "test_stl_basic_algorithm - sorting_containers - is_sorted:";
	std::cout << is_sorted(std::begin(vecRand), std::end(vecRand)) << '\n';
	//std::shuffle  将打乱 vector  中的内容
	std::shuffle(std::begin(vecRand), std::end(vecRand), g);
	std::cout << "test_stl_basic_algorithm - sorting_containers - is_sorted:";
	std::cout << std::is_sorted(std::begin(vecRand), std::end(vecRand)) << '\n';
	std::cout << "test_stl_basic_algorithm - sorting_containers -     print:";
	print(vecRand);

	std::sort(std::begin(vecRand), std::end(vecRand));
	std::cout << "test_stl_basic_algorithm - sorting_containers - is_sorted:";
	std::cout << std::is_sorted(std::begin(vecRand), std::end(vecRand)) << '\n';
	std::cout << "test_stl_basic_algorithm - sorting_containers -     print:";

	print(vecRand);

	std::shuffle(std::begin(vecRand), std::end(vecRand), g);
	// partition  将数值小于5的元素排到前面
	std::partition(std::begin(vecRand), std::end(vecRand), [](int i) { return i < 5; });
	std::cout << "test_stl_basic_algorithm - sorting_containers -     print:";
	print(vecRand);


	std::shuffle(std::begin(vecRand), std::end(vecRand), g);
	auto middle(std::next(std::begin(vecRand), int(vecRand.size()) / 2));
	//std::partial_sort  。使用这个函数对容器的内容进行排序，不过只是在某种程度上的排序。
	//其会将 vector  中最小的N个数，放在容器的前半部分。其余的留在 vector  的后半部分，不进行排序
	std::partial_sort(std::begin(vecRand), middle, std::end(vecRand));
	std::cout << "test_stl_basic_algorithm - sorting_containers -     print:";
	print(vecRand);

	//04
	//-------------------------------------------------------------


	std::cout << "test_stl_basic_algorithm - sorting_containers -        mv:";

	struct mystruct
	{
		int a;
		int b;
	};
	std::vector<mystruct> mv{ { 5, 100 },{ 1, 50 },{ -123, 1000 },{ 3, 70 },{ -10, 20 } };
	//mystruct::a:
	std::sort(std::begin(mv), std::end(mv), [](const mystruct &lhs, const mystruct &rhs) {return lhs.b < rhs.b;});
	for (const auto &[a, b] : mv)
	{
		std::cout << "{" << a << ", " << b << "} ";
	}
	std::cout << '\n';
	
	//05
	//-------------------------------------------------------------

	//std::replace 使用这个函数将所有4替换成 0。
	std::vector<int> rmv_v = { 1, 2, 4, 4, 5, 4, 7, 8, 9, 4 };
	std::cout << "removing_items_from_containers - print:";
	print(rmv_v);
	std::replace(std::begin(rmv_v), std::end(rmv_v), 4, 0);
	std::cout << "removing_items_from_containers - print:";
	print(rmv_v);

	std::vector<int> v2;
	std::vector<int> v3;
	auto odd_number([](int i) { return i % 2 != 0; });
	auto even_number([](int i) { return i % 2 == 0; });
	// 将奇、偶数拷贝到v2和v3中
	// 接受一个容器范围，一个输出迭代器和一个值作为参数。并且将所有不满足条件的元素拷贝到输出迭代器的容器中
	std::remove_copy_if(std::begin(rmv_v), std::end(rmv_v), std::back_inserter(v2), odd_number);
	// std::copy_if -> 与std::copy功能相同，可以多接受一个谓词函数作为是否进行拷贝的依据。
	std::copy_if(std::begin(rmv_v), std::end(rmv_v), std::back_inserter(v3), even_number);
	
	std::cout << "removing_items_from_containers - print:";
	print(rmv_v);
	std::cout << "removing_items_from_containers -    v2:";
	print(v2);
	std::cout << "removing_items_from_containers -    v3:";
	print(v3);

	//06
	//-------------------------------------------------------------
	std::cout << "transforming_items_in_containers vTran:";
	// std::transform  函数工作原理和 std::copy  差不多，不过在拷贝的过程中会对源容器中的元素进行变换，这个变换函数由用户提供。
	std::vector<int> vevTran{ 1, 2, 3, 4, 5 };
	std::transform(std::begin(vevTran), std::end(vevTran), std::ostream_iterator<int>{std::cout, ", "}, [](int i) { return i * i; });
	std::cout << '\n';
	auto int_to_string([](int i)
	{
		std::stringstream ss;
		ss << i << "^2 = " << i * i;
		return ss.str();
	});
	std::vector<std::string> vs;
	std::transform(std::begin(vevTran), std::end(vevTran), std::back_inserter(vs),int_to_string);
	std::cout << "transforming_items_in_containers -  vs:\n";
	std::copy(std::begin(vs), std::end(vs), std::ostream_iterator<std::string>{std::cout, "\n"});

	/*
test_stl_basic_algorithm - copying_items m(1, one), (2, two), (3, three), 
test_stl_basic_algorithm - copying_items v(1, one), (2, two), (3, three), (4, four), (5, five), 
test_stl_basic_algorithm - copying_items m(1, one), (2, two), (3, three), (4, four), (5, five), 
test_stl_basic_algorithm - copying_items v(1, ), (2, ), (3, ), (4, ), (5, ), 
test_stl_basic_algorithm - sorting_containers - is_sorted:1
test_stl_basic_algorithm - sorting_containers - is_sorted:0
test_stl_basic_algorithm - sorting_containers -     print:5, 3, 8, 9, 6, 2, 7, 4, 1, 10, 
test_stl_basic_algorithm - sorting_containers - is_sorted:1
test_stl_basic_algorithm - sorting_containers -     print:1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 
test_stl_basic_algorithm - sorting_containers -     print:3, 4, 1, 2, 8, 9, 7, 10, 6, 5, 
test_stl_basic_algorithm - sorting_containers -     print:1, 2, 3, 4, 5, 10, 8, 7, 9, 6, 
test_stl_basic_algorithm - sorting_containers -        mv:{-10, 20} {1, 50} {3, 70} {5, 100} {-123, 1000} 
removing_items_from_containers - print:1, 2, 4, 4, 5, 4, 7, 8, 9, 4, 
removing_items_from_containers - print:1, 2, 0, 0, 5, 0, 7, 8, 9, 0, 
removing_items_from_containers - print:1, 2, 0, 0, 5, 0, 7, 8, 9, 0, 
removing_items_from_containers -    v2:2, 0, 0, 0, 8, 0, 
removing_items_from_containers -    v3:2, 0, 0, 0, 8, 0, 
transforming_items_in_containers vTran:1, 4, 9, 16, 25, 
transforming_items_in_containers -  vs:
1^2 = 1
2^2 = 4
3^2 = 9
4^2 = 16
5^2 = 25
	*/

	//07 finding_items
	//-------------------------------------------------------------
	//std::cout << "finding_items:";
	stl_finding_items();

	//-------------------------------------------------------------
	// 08
	stl_reducing_range_in_vector();

	//-------------------------------------------------------------
	// 09
	stl_pattern_search_string();

	//-------------------------------------------------------------
	std::vector<std::pair<int, int>> vecType;
	std::pair<int, int> temp{ 1 ,2 };
	vecType.emplace_back(temp);
	vecType.emplace_back(3, 4);


	return 0;
}




/*

算法函数 作用
std::remove
接受一个容器范围和一个具体的值作为参数，并且移除对应的值。
返回一个新的end迭代器，用于修改容器的范围。

std::replace
接受一个容器范围和两个值作为参数，将使用第二个数值替换所有
与第一个数值相同的值。

std::remove_copy
接受一个容器范围，一个输出迭代器和一个值作为参数。并且将所
有不满足条件的元素拷贝到输出迭代器的容器中。
std::replace_copy
与std::replace功能类似，但与std::remove_copy更类似些。源容器
的范围并没有变化。

std::copy_if
与std::copy功能相同，可以多接受一个谓词函数作为是否进行拷贝
的依据。




算法函数 作用
std::sort 接受一定范围的元素，并对元素进行排序。

std::is_sorted 接受一定范围的元素，并判断该范围的元素是否经过排序。

std::shuffle 类似于反排序函数；其接受一定范围的元素，并打乱这些元素。

std::partial_sort
接受一定范围的元素和另一个迭代器，前两个参数决定排序的范围，
后两个参数决定不排序的范围。

std::partition
能够接受谓词函数。所有元素都会在谓词函数返回true时，被移动到
范围的前端。剩下的将放在范围的后方。



算法函数 作用

std::find
可将一个搜索范围和一个值作为参数。函数将返回找到的第一个值
的迭代器。线性查找。

std::find_if 与std::find原理类似，不过其使用谓词函数替换比较值。

std::binary_search
可将一个搜索范围和一个值作为参数。执行二分查找，当找到对应
元素时，返回true；否则，返回false。

std::lower_bound
可将一个查找返回和一个值作为参数，并且执行二分查找，返回第
一个不小于给定值元素的迭代器。

std::upper_bound
与std::lower_bound类似，不过会返回第一个大于给定值元素的迭
代器。

std::equal_range
可将一个搜索范围和一个值作为参数，并且返回一对迭代器。其第
一个迭代器和std::lower_bound返回结果一样，第二个迭代器和
std::upper_bound返回结果一样。

*/

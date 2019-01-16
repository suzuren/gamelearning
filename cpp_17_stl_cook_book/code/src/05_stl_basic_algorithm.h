
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


	//-------------------------------------------------------------
	std::vector<std::pair<int, int>> vecType;
	std::pair<int, int> temp{ 1 ,2 };
	vecType.emplace_back(temp);

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



*/

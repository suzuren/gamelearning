#include <iostream>
#include <iterator>
#include <vector>
#include <numeric>
//----------------------------------------------------------------------------------------------------------------------------

class num_iterator
{
private:
	int i;
public:
	explicit num_iterator(int position = 0) : i{ position } {}
	int operator*() const { return i; }
	num_iterator& operator++() { ++i; return *this; }
	bool operator!=(const num_iterator &other) const { return i != other.i; }
	bool operator==(const num_iterator &other) const { return !(*this != other); }
};
class num_range
{
private:
	int a;
	int b;
public:
	num_range(int from, int to)	: a{ from }, b{ to }{}
	num_iterator begin() const { return num_iterator{ a }; }
	num_iterator end() const { return num_iterator{ b }; }
};
namespace std
{
	// 对 std::iterator_traits  进行特化。这个特化就是告诉 STL
	// 的 num_iterator  是一种前向迭代器，并且指向的对象是 int  类型的值。
	template <>	struct iterator_traits<num_iterator>
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = int;
	};
}

//----------------------------------------------------------------------------------------------------------------------------

void test_reverse_iterator()
{
	std::list<int> l{ 1, 2, 3, 4, 5 };

	// 完成反向打印，调用 std::list  的成员函 rbegin  和 rend  获得反向迭代器，并且将数字推入输出流 ostream_iterator  适配器中
	std::cout << "test_ostream_iterator - rbegin - ";
	std::copy(l.rbegin(), l.rend(), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';

	std::cout << "test_ostream_iterator - begin - ";
	std::copy(l.begin(), l.end(), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';


	// 容器不提供 rbegin  和 rend  函数的话，就需要使用双向迭代器来帮忙了，这里可以
	// 使用工厂函数 std::make_reverse_iterator  创建双向迭代器。其能接受普通迭代器，然后将其转换为反向迭代器
	std::cout << "test_ostream_iterator - make_reverse_iterator end - ";
	std::copy(std::make_reverse_iterator(std::end(l)), std::make_reverse_iterator(std::begin(l)), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';

	std::cout << "test_ostream_iterator - make_reverse_iterator - begin";
	std::copy(std::make_reverse_iterator(std::begin(l)), std::make_reverse_iterator(std::end(l)), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';


}

//----------------------------------------------------------------------------------------------------------------------------

class cstring_iterator_sentinel {};

class cstring_iterator
{
	const char *s{ nullptr };
public:
	explicit cstring_iterator(const char *str) : s{ str } {}
	char operator*() const { return *s; }
	cstring_iterator& operator++() { ++s; return *this; }
	bool operator!=(const cstring_iterator_sentinel) const { return s != nullptr && *s != '\0'; }
};
class cstring_range
{
	const char *s{ nullptr };
public:
	cstring_range(const char *str) : s{ str } {}
	cstring_iterator begin() const { return cstring_iterator{ s }; }
	cstring_iterator_sentinel end() const { return{}; }
};

//----------------------------------------------------------------------------------------------------------------------------


void iterator_class_func()
{
	//--------------------------------------------------
	std::cout << "iterator_class_func - ";
	for (int i : num_range{ 100, 110 })	{
		std::cout << i << ", ";
	}std::cout << '\n';

	//--------------------------------------------------

	std::cout << "iterator_class_func min_max - ";
	num_range r{ 100, 110 };
	auto min_max(std::minmax_element(r.begin(), r.end()));
	std::cout << *min_max.first << " - " << *min_max.second << '\n';

	//--------------------------------------------------

	test_reverse_iterator();

	//--------------------------------------------------

	std::cout << "cstring_range - for:  ";
	auto cstringItem = cstring_range("a b c d e f g h i j k l m n");
	for (char c : cstringItem)
	{
		std::cout << c;
	}
	std::cout << '\n';

	std::cout << "cstring_range - loop  ";
	for (auto iter = cstringItem.begin(); iter != cstringItem.end(); ++(iter))
	{
		std::cout << *(iter);
	}
	std::cout << '\n';


	//--------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------------

class zip_iterator {
	using it_type = std::vector<double>::iterator;
	// // zip迭代器的容器中，每个需要保存两个迭代器：
	it_type it1;
	it_type it2;
	
public:
	zip_iterator(it_type iterator1, it_type iterator2) : it1{ iterator1 }, it2{ iterator2 } {}
	// 增加zip迭代器就意味着增加两个成员迭代器
	zip_iterator& operator++() { ++it1;	++it2;	return *this; }
	// 如果zip中的两个迭代器来自不同的容器，那么他们一定不相等。通常，这里会用逻辑或
	// (|| )替换逻辑与(&&)，但是这里我们需要考虑两个容器长度不一样的情况。这样的话，我
	// 们需要在比较的时候同时匹配两个容器。这样，我们就能遍历完其中一个容器时，及时停下循环
	bool operator!=(const zip_iterator& o) const { return it1 != o.it1 && it2 != o.it2; }
	// 逻辑等操作符可以使用逻辑不等的操作符的实现，是需要将结果取反即可
	bool operator==(const zip_iterator& o) const { return !operator!=(o); }
	// 解引用操作符用来访问两个迭代器指向的值：
	std::pair<double, double> operator*() const { return{ *it1, *it2 }; }
};

namespace std {
	template <>
	struct iterator_traits<zip_iterator> {
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<double, double>;
		using difference_type = long int;
	};
}

class zipper
{
	using vec_type = std::vector<double>;
	vec_type &vec1;
	vec_type &vec2;
public:
	zipper(vec_type &va, vec_type &vb) : vec1{ va }, vec2{ vb } {}
	zip_iterator begin() const { return{ std::begin(vec1), std::begin(vec2) }; }
	zip_iterator end() const { return{ std::end(vec1), std::end(vec2) }; }
};

int test_iterator()
{
	//01
	iterator_class_func();
	//02
	std::vector<double> a{ 1.0, 2.0, 3.0 };
	std::vector<double> b{ 4.0, 5.0, 6.0 };
	zipper zipped{ a, b };
	const auto add_product([](double sum, const auto &p) {return sum + p.first * p.second; });
	// 使用 std::accumulate  将所有值累加在一起
	const auto dot_product(std::accumulate(std::begin(zipped), std::end(zipped), 0.0, add_product));
	std::cout << "dot_product - ";
	std::cout << dot_product << '\n';
	// dot_product = 32
	return 0;
}


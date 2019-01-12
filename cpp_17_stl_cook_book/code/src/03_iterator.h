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
	// �� std::iterator_traits  �����ػ�������ػ����Ǹ��� STL
	// �� num_iterator  ��һ��ǰ�������������ָ��Ķ����� int  ���͵�ֵ��
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

	// ��ɷ����ӡ������ std::list  �ĳ�Ա�� rbegin  �� rend  ��÷�������������ҽ�������������� ostream_iterator  ��������
	std::cout << "test_ostream_iterator - rbegin - ";
	std::copy(l.rbegin(), l.rend(), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';

	std::cout << "test_ostream_iterator - begin - ";
	std::copy(l.begin(), l.end(), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';


	// �������ṩ rbegin  �� rend  �����Ļ�������Ҫʹ��˫�����������æ�ˣ��������
	// ʹ�ù������� std::make_reverse_iterator  ����˫������������ܽ�����ͨ��������Ȼ����ת��Ϊ���������
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
	// // zip�������������У�ÿ����Ҫ����������������
	it_type it1;
	it_type it2;
	
public:
	zip_iterator(it_type iterator1, it_type iterator2) : it1{ iterator1 }, it2{ iterator2 } {}
	// ����zip����������ζ������������Ա������
	zip_iterator& operator++() { ++it1;	++it2;	return *this; }
	// ���zip�е��������������Բ�ͬ����������ô����һ������ȡ�ͨ������������߼���
	// (|| )�滻�߼���(&&)����������������Ҫ���������������Ȳ�һ��������������Ļ�����
	// ����Ҫ�ڱȽϵ�ʱ��ͬʱƥ���������������������Ǿ��ܱ���������һ������ʱ����ʱͣ��ѭ��
	bool operator!=(const zip_iterator& o) const { return it1 != o.it1 && it2 != o.it2; }
	// �߼��Ȳ���������ʹ���߼����ȵĲ�������ʵ�֣�����Ҫ�����ȡ������
	bool operator==(const zip_iterator& o) const { return !operator!=(o); }
	// �����ò�����������������������ָ���ֵ��
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
	// ʹ�� std::accumulate  ������ֵ�ۼ���һ��
	const auto dot_product(std::accumulate(std::begin(zipped), std::end(zipped), 0.0, add_product));
	std::cout << "dot_product - ";
	std::cout << dot_product << '\n';
	// dot_product = 32
	return 0;
}


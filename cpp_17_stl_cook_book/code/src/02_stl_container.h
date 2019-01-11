#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

std::string g_str_test_text = 
"Nam quam nunc, blandit vel.\
luctus pulv inar, hend rerit id, lorem.\
Sed consequat, leoeget biben dum sodales, augue.\
velit cursus sociis natoque penatibus et magnis.\
dispart urient montes, nasc etur ridiculus maecenas tempussedip sum.\
";


void erase_remove_if()
{
	std::vector<int> vecTest{ 1, 2, 2, 3, 6 };
	for (const auto value : vecTest)
	{
		std::cout << "test_stl_container - erase_remove_if - init vecTest_" << "size:" << vecTest.size() << " value:" << value << std::endl;
	}
	// 在内部，remove 遍历整个区间，用需要保留的元素的值覆盖掉那些要被删除的元素的值。
	// 这种覆盖是通过对那些需要被覆盖的元素的赋值来完成的。
	auto vecTestNewEnd(std::remove(vecTest.begin(), vecTest.end(), 2));
	vecTest.erase(vecTestNewEnd, vecTest.end());

	for (const auto value : vecTest)
	{
		std::cout << "test_stl_container - erase_remove_if - rmve vecTest_" << "size:" << vecTest.size() << " value:" << value << std::endl;
	}

	// 奇数函数
	const auto oddFunc([](int i) {return i % 2 != 0; });

	vecTest.erase(std::remove_if(std::begin(vecTest), std::end(vecTest), oddFunc), std::end(vecTest));

	for (const auto value : vecTest)
	{
		std::cout << "test_stl_container - erase_remove_if - rmif vecTest_" << "size:" << vecTest.size() << " value:" << value << std::endl;
	}


	// 将其容量修改为正确的大小。需要注意的是，这个操作会让 vector  重新分配一段内存，
	// 以匹配相应元素长度， vector  中已存的元素会移动到新的内存块中。
	vecTest.shrink_to_fit();
	for (const auto value : vecTest)
	{
		std::cout << "test_stl_container - erase_remove_if - sfit vecTest_" << "size:" << vecTest.size() << " value:" << value << std::endl;
	}
}

template <typename T>
void quick_remove_at(std::vector<T> &vec, std::size_t idx)
{
	for (const auto value : vec)
	{
		std::cout << "test_stl_container - quick_remove_at sta vec_" << "idx:" << idx
			<<" size:" << vec.size() << " value:" << value << std::endl;
	}

	if (idx < vec.size())
	{
		vec[idx] = std::move(vec.back());
		vec.pop_back();
	}

	for (const auto value : vec)
	{
		std::cout << "test_stl_container - quick_remove_at end vec_" << "idx:" << idx
			<< "size:" << vec.size() << " value:" << value << std::endl;
	}
}

template <typename T>
void quick_remove_at(std::vector<T> &vec, typename std::vector<T>::iterator iter)
{
	for (const auto value : vec)
	{
		std::cout << "test_stl_container - quick_remove_at sta vec_"<< "size:" << vec.size() << " value:" << value << std::endl;
	}

	if (iter != std::end(vec))
	{
		*iter = std::move(vec.back());
		vec.pop_back();
	}

	for (const auto value : vec)
	{
		std::cout << "test_stl_container - quick_remove_at en vec_"
			<< "size:" << vec.size() << " value:" << value << std::endl;
	}

}

template<typename ... Ts>
auto fold_expression_sum(Ts ... ts)
{
	return (ts + ...);
}

template <typename R, typename ... Ts>
auto fold_expression_matches(const R& range, Ts ... ts)
{
	// std::count 函数会返回范围内与第三个参数相同元素的个数
	return (std::count(std::begin(range), std::end(range), ts) + ...);
}

//void insert_sorted(std::vector<std::string> &v, const std::string &word)
void insert_sorted(std::vector<std::string> &v, const char * word)
{
	std::cout << "test_stl_container - insert_sorted - void_not " << " word:" << word << std::endl;
	// std::lower_bound
	// Returns an iterator pointing to the first element in the range [first, last) 
	// that is not less than (i.e. greater or equal to) value, or last if no such element is found.
	const auto insert_pos(std::lower_bound(std::begin(v), std::end(v),	word));
	v.insert(insert_pos, word);
}

template <typename C, typename T>
void insert_sorted(C &v, const T &item)
{
	std::cout << "test_stl_container - insert_sorted - template " << " item:" << item << std::endl;
	
	const auto insert_pos(std::lower_bound(std::begin(v), std::end(v), item));
	v.insert(insert_pos, item);
}

void vector_sort()
{
	std::vector<std::string> vecStr{ "some", "random", "words",	"without", "order", "aaa", "yyy" };
	bool vec_is_sorted = std::is_sorted(begin(vecStr), end(vecStr));
	std::cout << "test_stl_container - vector_sort vec_is_sorted:" << vec_is_sorted << std::endl;
	sort(begin(vecStr), end(vecStr));
	vec_is_sorted = std::is_sorted(begin(vecStr), end(vecStr));
	std::cout << "test_stl_container - vector_sort vec_is_sorted:" << vec_is_sorted << std::endl;

	insert_sorted(vecStr, "foobar");
	insert_sorted(vecStr, "zzz");
	insert_sorted<std::vector<std::string>, std::string>(vecStr, "egd");
	for (const auto value : vecStr)
	{
		std::cout << "test_stl_container - vector_sort"	<< "size:" << vecStr.size() << " value:" << value << std::endl;
	}

}

struct coord
{
	int x;
	int y;
};
bool operator==(const coord &l, const coord &r)
{
	return l.x == r.x && l.y == r.y;
}
namespace std
{
	template <>
	struct hash<coord>
	{
		using argument_type = coord;
		using result_type = size_t;
		result_type operator()(const argument_type &c) const
		{
			return static_cast<result_type>(c.x) + static_cast<result_type>(c.y);
		}
	};
}

void map_insert()
{
	struct billionaire
	{
		std::string name;
		double dollars;
		std::string country;
	};
	std::list<billionaire> billionaires
	{
		{ "Bill Gates", 86.0, "USA" },
		{ "Warren Buffet", 75.6, "USA" },
		{ "Jeff Bezos", 72.8, "USA" },
		{ "Amancio Ortega", 71.3, "Spain" },
		{ "Mark Zuckerberg", 56.0, "USA" },
		{ "Carlos Slim", 54.5, "Mexico" },
		{ "Bernard Arnault", 41.5, "France" }
	};
	// std::pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);
	// 其函数第一个参数 k  是插入的键， args  表示这个键对应的值。如果我们成功的插入了元
	// 素，那么函数就会返回一个迭代器，其指向新节点在表中的位置，组对中布尔变量的值被置
	// 为true。当插入不成功，组对中的布尔变量值会置为false，并且迭代器指向与新元素冲突的位置。

	std::map<std::string, std::pair<const billionaire, size_t>> mpBill;
	for (const auto &b : billionaires)
	{
		auto[iterator, success] = mpBill.try_emplace(b.country, b, 1);
		if (success == false)
		{
			iterator->second.second += 1;
		}
	}
	for (const auto &[key, value] : mpBill)
	{
		const auto &[b, count] = value;
		std::cout << b.country << " : " << count
			<< " billionaires. Richest is "
			<< b.name << " with " << b.dollars
			<< " B$\n";
	}

	std::cout << "test_stl_container - map_insert ------------------------------------------------ " << std::endl;

	std::map<int, std::string> race_placement{
		{ 1, "Mario" },{ 2, "Luigi" },{ 3, "Bowser" },
		{ 4, "Peach" },{ 5, "Yoshi" },{ 6, "Koopa" },
		{ 7, "Toad" },{ 8, "Donkey Kong Jr." }
	};
	

	for (const auto &[key, value] : race_placement)
	{
		std::cout << "test_stl_container - map_insert - key: " << key << " value: " << value << std::endl;
	}

	std::cout << "test_stl_container - map_insert ------------------------------------------------ " << std::endl;

	auto iter_a(race_placement.extract(3));
	auto iter_b(race_placement.extract(8));
	std::swap(iter_a.key(), iter_b.key());

	race_placement.insert(std::move(iter_a));
	race_placement.insert(std::move(iter_b));

	for (const auto &[key, value] : race_placement)
	{
		std::cout << "test_stl_container - map_insert - key: " << key << " value: " << value << std::endl;
	}

	std::cout << "test_stl_container - map_insert ------------------------------------------------ " << std::endl;
	
	std::unordered_map<coord, int> unorderedMp{
		{ { 0, 0 }, 1 },
		{ { 0, 0 }, 4 },
		{ { 0, 1 }, 2 },
		{ { 2, 1 }, 3 },
		{ { 2, 8 }, 5 },
		{ { 2, 2 }, 4 }
	};

	for (const auto &[key, value] : unorderedMp) {
		std::cout << "{(" << key.x << ", " << key.y
			<< "): " << value << "} ";
	}
	std::cout << '\n';

	std::cout << "test_stl_container - unordered_map ------------------------------------------------ " << std::endl;

}

void map_move_vector()
{
	std::map<std::string, size_t> words{ {"c",3},{ "a",5 },{ "k",8 },{ "h",2 },{ "b",1 } };
	std::cout << "map_move_vector - word";
	for (const auto &[key, value] : words)
	{
		std::cout << "{(" << key << "): " << value << "} ";
	}
	std::cout << '\n';

	std::vector<std::pair<std::string, size_t>> word_counts;
	std::move(std::begin(words), std::end(words), std::back_inserter(word_counts));

	std::cout << "map_move_vector - move";
	for (const auto &[key, value] : words)
	{
		std::cout << "{(" << key << "): " << value << "} ";
	}
	std::cout << '\n';

	std::cout << "map_move_vector - init";
	for (const auto &[key, value] : word_counts)
	{
		std::cout << "{(" << key << "): " << value << "} ";
	}
	std::cout << '\n';

	std::sort(std::begin(word_counts), std::end(word_counts), [](const auto &a, const auto &b) { return a.second > b.second; });

	std::cout << "map_move_vector - sort";
	for (const auto &[key, value] : word_counts)
	{
		std::cout << "{(" << key << "): " << value << "} ";
	}
	std::cout << '\n';

	//map_move_vector - word{ (a) : 5 } {(b) : 1} {(c) : 3} {(h) : 2} {(k) : 8}
	//map_move_vector - init{ (a) : 5 } {(b) : 1} {(c) : 3} {(h) : 2} {(k) : 8}
	//map_move_vector - sort{ (k) : 8 } {(a) : 5} {(c) : 3} {(h) : 2} {(b) : 1}

}

std::string filter_ws(const std::string &s)
{
	const char *ws{ " \r\n\t" };
	const auto a(s.find_first_not_of(ws));
	const auto b(s.find_last_not_of(ws));
	if (a == std::string::npos) {
		return{};
	}
	return s.substr(a, b);
}

std::multimap<size_t, std::string> get_sentence_stats(const std::string	&content)
{
	std::multimap<size_t, std::string> ret;
	const auto end_it(std::end(content));
	auto it1(std::begin(content));
	auto it2(std::find(it1, end_it, '.'));
	while (it1 != end_it && std::distance(it1, it2) > 0)
	{
		std::string s{ filter_ws({ it1, it2 }) };
		if (s.length() > 0)
		{
			const auto words(std::count(std::begin(s), std::end(s), ' ') + 1);
			ret.emplace(std::make_pair(words, std::move(s)));
		}
		it1 = std::next(it2, 1); // 向前移动一个元素个数
		it2 = std::find(it1, end_it, '.');
	}
	return ret;
}

void test_multimap()
{
	std::string content = g_str_test_text;
	auto mmp_sentence = get_sentence_stats(content);
	for (const auto &[word_count, sentence]	: mmp_sentence)
	{
		std::cout << word_count << " words: " << sentence <<	".\n";
	}

	//9 words: Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lore.
	//9 words : Sed consequat, leo eget bibendum sodales, augue velit cursus nunc.
	//11 words : Cum sociis natoque penatibus et magnis disparturient montes, nascetur ridiculus mu.
	//15 words : Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sedipsu.

}

int test_stl_container()
{
	//01
	//erase_remove_if();
	//02
	//std::vector<int> vecTest{ 1, 8, 4, 10 };
	//quick_remove_at(vecTest, 1);
	//quick_remove_at(vecTest, std::find(std::begin(vecTest), std::end(vecTest), 4));
	//03 折叠表达式 fold expression
	//int iSum{ fold_expression_sum(1, 2, 3) };
	//std::cout << "test_stl_container - fold_expression_sum int" << " iSum: " << iSum << std::endl;
	//std::string strSum{ fold_expression_sum(std::string("hi, "), std::string("hello "), std::string("world!")) };
	//std::cout << "test_stl_container - fold_expression_sum str" << " strSum: " << strSum << std::endl;

	//std::vector<int> vecMatches{ 1, 2, 3, 4, 5 };
	//auto matches_a = fold_expression_matches(vecMatches, 2, 5); // return 2
	//auto matches_b = fold_expression_matches(vecMatches, 100, 200); // return 0
	//auto matches_c = fold_expression_matches("abcdefg", 'x', 'y', 'z'); // return 0
	//auto matches_d = fold_expression_matches("abcdefg", 'a', 'b', 'f'); // return 3
	//std::cout << "test_stl_container - fold_expression_matches " << "matches_a: " << matches_a
	//	<< " matches_b: " << matches_b << " matches_c: " << matches_c << " matches_d: " << matches_d << std::endl;

	//04 保持对std::vector实例的排序
	//vector_sort();
	//05 向std::map实例中高效并有条件的插入元素
	//map_insert();
	//06 map move vector
	//map_move_vector();
	//07 std::multimap
	//test_multimap();


	return 0;
}



//--------------------------------------------------------------------------------

#include <iostream>
#include <optional>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <vector>
#include <string>

//--------------------------------------------------------------------------------

template <typename T>
class trie
{
	std::map<T, trie> tries;
public:
	template <typename It>
	void insert(It it, It end_it)
	{
		if (it == end_it)
		{
			return;
		}
		tries[*it].insert(std::next(it), end_it);
	}
	template <typename C>
	void insert(const C &container)
	{
		insert(std::begin(container), std::end(container));
	}
	void insert(const std::initializer_list<T> &il)
	{
		insert(std::begin(il), std::end(il));
	}
	void print(std::vector<T> &v) const
	{
		if (tries.empty())
		{
			std::copy(std::begin(v), std::end(v), std::ostream_iterator<T>{std::cout, " "});
			std::cout << '\n';
		}
		for (const auto &p : tries)
		{
			v.push_back(p.first);
			p.second.print(v);
			v.pop_back();
		}
	}
	void print() const
	{
		std::vector<T> v;
		print(v);
	}

	//返回值 optional<reference_wrapper<const trie>>  。我们选择 std::optional  作
	//为包装器，因为其可能没有我们所要找打tire。当我们仅插入“hello my friend”，那么就不会找
	//到“goodbye my friend”。这样，我们仅返回{}就可以了，其代表返回一个空 optional  对象给
	//调用者。不过这还是没有解释，我们为什么使用 reference_wrapper  代替 optional<const trie
	//&>  。 optional  的实例，其为 trie&  类型，是不可赋值的，因此不会被编译。使
	//用 reference_warpper  实现一个引用，就是用来对对象进行赋值。
	//
	//代码的主要部分在于 auto found (tries.find(*it));  。我们使用find来替代中括号操作
	//符。当我们使用中括号操作符进行查找时，trie将会为我们创建丢失的元素(顺带一提，当我们
	//尝试这样做，类的函数为 const  ，所以这样做事不可能的。这样的修饰能帮助我们减少bug的发生)

	template <typename It> std::optional<std::reference_wrapper<const trie>> subtrie(It it, It end_it) const
	{
		if (it == end_it)
		{
			return ref(*this);
		}
		auto found(tries.find(*it));
		if (found == end(tries))
		{
			return{};
		}
		return found->second.subtrie(std::next(it), end_it);
	}
	template <typename C>
	auto subtrie(const C &c)
	{
		return subtrie(std::begin(c), std::end(c));
	}
};

void stl_trie()
{
	std::cout << "stl_trie - start - \n";

	trie<std::string> t;
	t.insert({ "hi", "how", "are", "you" });
	t.insert({ "hi", "i", "am", "great", "thanks" });
	t.insert({ "what", "are", "you", "doing" });
	t.insert({ "i", "am", "watching", "a", "movie" });
	std::cout << "recorded sentences:\n";
	t.print();

	std::cout << "\npossible suggestions after \"hi\":\n";
	if (auto st(t.subtrie(std::initializer_list<std::string>{"hi"}));st)
	{
		st->get().print();
	}

	std::cout << "stl_trie - end - \n";

	/*
	stl_trie - start - 
	recorded sentences:
	hi how are you 
	hi i am great thanks 
	i am watching a movie 
	what are you doing 

	possible suggestions after "hi":
	how are you 
	i am great thanks 
	stl_trie - end - 

	*/
}

int test_stl_advanced_algorithm()
{

	//--------------------------------------------------------------------------------
	//01
	stl_trie();




	return 0;
}



#include <iostream>

class num_iterator
{
private:
	int i;
public:
	explicit num_iterator(int position = 0) : i{ position } {}
	int operator*() const { return i; }
	num_iterator& operator++()
	{
		++i;
		return *this;
	}
	bool operator!=(const num_iterator &other) const
	{
		return i != other.i;
	}
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

void iterator_class_func()
{
	std::cout << "iterator_class_func - ";
	for (int i : num_range{ 100, 110 })
	{
		std::cout << i << ", ";
	}
	std::cout << '\n';
}

int test_iterator()
{
	//01
	iterator_class_func();


	return 0;
}


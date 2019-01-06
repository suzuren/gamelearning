#include <iostream>
#include <vector>
#include <tuple>

int test_Structured_Binding_Declaration()
{
	std::vector<int> vecTest{ 1, 2, 3 };

	std::cout << " vecTest.size - " << vecTest.size() << std::endl;
	
	for (const auto v : vecTest)
	{
		std::cout << " vecTest - " << v<< std::endl;
	}

	struct tagTest
	{
		int i;
		double d;
	};

	std::vector<tagTest> vecTagTest = { {1, 2.1}, {3, 4.5} };

	for (const auto &[i, j] : vecTagTest)
	{
		std::cout << " vecTagTest - "<<"i: " << i << " j: "<< j<<std::endl;
	}

	std::tuple<int, float, long> testTuple(1, 2.6, 3);
	auto[a, b, c] = testTuple;
	std::cout << " testTuple - " << " a:" << a << "  b:" << b << "  c:" << c << std::endl;

	auto x{ 1 };
	auto y{ 2 };
	auto z{ 6 };
	auto k{ 9 };
	auto testInitList = { x, y, z, k }; // testInitList is std::initializer_list<int>
	for (const auto v : testInitList)
	{
		std::cout << " testInitList - size:" << testInitList .size() << " value:"<< v << std::endl;
	}


	return 0;
}
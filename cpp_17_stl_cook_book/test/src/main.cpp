#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, const char** argv)
{
	

	std::cout << "hello test!" << std::endl;

	std::vector<int> vecTest{ 1, 2, 3 };
	vecTest.emplace_back(4);
	vecTest.emplace_back(5);
	vecTest.push_back(6);
	vecTest.push_back(7);
	std::cout << " vecTest.size: " << vecTest.size() << std::endl;
	
	for (const auto v : vecTest)
	{
		std::cout << " vecTest: " << v<< std::endl;
	}
	auto iter_find_1 = std::find(std::begin(vecTest), std::end(vecTest), 1);
	auto index_1 = std::distance(std::begin(vecTest), iter_find_1);

	auto iter_find_3 = std::find(std::begin(vecTest), std::end(vecTest), 3);
	auto index_3 = std::distance(std::begin(vecTest), iter_find_3);

	auto iter_find_7 = std::find(std::begin(vecTest), std::end(vecTest), 7);
	auto index_7 = std::distance(std::begin(vecTest), iter_find_7);

	auto iter_find_9 = std::find(std::begin(vecTest), std::end(vecTest), 9);
	auto index_9 = std::distance(std::begin(vecTest), iter_find_9);

	std::cout << " vecTest - index_1:" << index_1 << " index_3:" << index_3 
		                << " index_7:" << index_7 << " index_9:" << index_9 << std::endl;

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



	return 0;
}
#include <iostream>
#include <vector>


int main(int argc, const char** argv)
{
	

	std::cout << "hello test!" << std::endl;

	std::vector<int> vecTest{ 1, 2, 3 };

	std::cout << " vecTest.size: " << vecTest.size() << std::endl;
	
	for (const auto v : vecTest)
	{
		std::cout << " vecTest: " << v<< std::endl;
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

	return 0;
}
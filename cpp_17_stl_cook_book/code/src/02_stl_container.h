#include <iostream>
#include <vector>
#include <algorithm>


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


int test_stl_container()
{
	//01
	//erase_remove_if();
	//02
	//std::vector<int> vecTest{ 1, 8, 4, 10 };
	//quick_remove_at(vecTest, 1);
	//quick_remove_at(vecTest, std::find(std::begin(vecTest), std::end(vecTest), 4));
	//03



	return 0;
}



//--------------------------------------------------------------------------------

#include <iostream>

#include <chrono>
#include <ratio>
#include <cmath>
#include <iomanip>
#include <optional>

//--------------------------------------------------------------------------------

//std::chrono::duration  经常用来表示所用时间的长度，其为秒的倍数或小数。所有STL的持续
//都由整型类型进行特化。本节中，我们将使用 double  进行特化。

//类 std::chrono::steady_clock 表示单调时钟。此时钟的时间点无法减少，因为物理时间向前移动。
//此时钟与壁钟时间无关（例如，它能是上次重启开始的时间），且最适于度量间隔。

using seconds = std::chrono::duration<double>;
using milliseconds = std::chrono::duration<double, std::ratio_multiply<seconds::period, std::milli>>;
using microseconds = std::chrono::duration<double, std::ratio_multiply<seconds::period, std::micro>>;

static std::pair<std::string, seconds> get_input()
{
	
	const auto tic(std::chrono::steady_clock::now());

	std::string s{ "C++17" };
	//if (!(std::cin >> s))
	//{
	//	return{ {},{} };
	//}
	const auto toc(std::chrono::steady_clock::now());

	//std::cout << "\n C++17 begin:" << tic << "\n C++17 end:" << toc << std::endl;

	return{ s, toc - tic };
}

void ratio_conversion()
{
	while (true)
	{
		std::cout << "Please type the word \"C++17\" as fast as you can.\n> ";
		const auto[user_input, diff] = get_input();
		if (user_input == "")
		{
			break;
		}
		if (user_input == "C++17")
		{
			std::cout << "Bravo. You did it in:\n"
				<< std::fixed << std::setprecision(2)
				<< std::setw(12) << diff.count() << " seconds.\n"
				<< std::setw(12) << milliseconds(diff).count() << " milliseconds.\n"
				<< std::setw(12) << microseconds(diff).count() << " microseconds.\n";
			break;
		}
		else
		{
			std::cout << "Sorry, your input does not match. You may try again.\n";
		}
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
	std::cout << "24 hours ago, the time was " << std::put_time(std::localtime(&now_c), "%F %T") << '\n';

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	std::cout << "Hello World\n";
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Printing took "<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<< " us.\n";

}

/*
Please type the word "C++17" as fast as you can.
> Bravo. You did it in:
0.00 seconds.
0.00 milliseconds.
0.59 microseconds.

24 hours ago, the time was 2019-01-21 18:55:37
Hello World
Printing took 1 us.

*/
//--------------------------------------------------------------------------------

int test_utils_class()
{
	//--------------------------------------------------------------------------------
	//01
	ratio_conversion();


	return 0;
}




#ifndef _TIMER_H__
#define _TIMER_H__

#include<chrono>
using namespace std;
using namespace std::chrono;

class Timer
{
public:
	Timer() : m_begin(high_resolution_clock::now()) {}
	void reset() { m_begin = high_resolution_clock::now(); }

	//默认输出毫秒
	int64_t elapsed() const
	{
		return duration_cast<chrono::milliseconds>(high_resolution_clock::now() - m_begin).count();
	}

	//默认输出秒
	double elapsed_second() const
	{
		return duration_cast<duration<double>>(high_resolution_clock::now() - m_begin).count();
	}

	//微秒
	int64_t elapsed_micro() const
	{
		return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
	}

	//纳秒
	int64_t elapsed_nano() const
	{
		return duration_cast<chrono::nanoseconds>(high_resolution_clock::now() - m_begin).count();
	}

	////秒
	//int64_t elapsed_seconds() const
	//{
	//	return duration_cast<chrono::seconds>(high_resolution_clock::now() - m_begin).count();
	//}

	//分
	int64_t elapsed_minutes() const
	{
		return duration_cast<chrono::minutes>(high_resolution_clock::now() - m_begin).count();
	}

	//时
	int64_t elapsed_hours() const
	{
		return duration_cast<chrono::hours>(high_resolution_clock::now() - m_begin).count();
	}

private:
	time_point<high_resolution_clock> m_begin;

public:
	//add function

	static int64_t system_second()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static int64_t system_millsecond()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

};



void test_timer()
{
	Timer objTimer;
	printf("objTimer\n");
	printf("elapsed:%ld\n", objTimer.elapsed());
	printf("elapsed_second:%f\n", objTimer.elapsed_second());
	printf("elapsed_micro:%ld\n", objTimer.elapsed_micro());
	printf("elapsed_nano:%ld\n", objTimer.elapsed_nano());
	printf("elapsed_minutes:%ld\n", objTimer.elapsed_minutes());
	printf("elapsed_hours:%ld\n", objTimer.elapsed_hours());
	printf("system_second:%ld\n", Timer::system_second());
	printf("system_millsecond:%ld\n", Timer::system_millsecond());

}

#endif


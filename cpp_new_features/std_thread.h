#ifndef _STD_THREAD_H__
#define _STD_THREAD_H__
#include <atomic>
#include <thread>
#include <iostream>

struct testTag
{
	int i;
	int j;
};

atomic_llong total(0);
void runThread_(void * param)
{
	for (long long i=0; i<1024; i++)
	{
		total += i;
	}
}
int test_std_thread()
{
	
	std::atomic<struct testTag> atomic_testtag;
	struct testTag set_testtag;
	set_testtag.i = 2;
	set_testtag.j = 8;
	atomic_testtag.store(set_testtag);
	struct testTag get_testtag = atomic_testtag.load();
	std::cout << "test_std_thread -  atomic_testtag.is_lock_free:" << atomic_testtag.is_lock_free() << std::endl;
	std::cout << "test_std_thread -  get_testtag.i:" << get_testtag.i << std::endl;
	std::cout << "test_std_thread -  get_testtag.j:" << get_testtag.j << std::endl;

	set_testtag.i = 6;
	set_testtag.j = 9;
	std::atomic<struct testTag> atomic_testtag_2{ set_testtag };
	get_testtag = atomic_testtag_2.load();
	std::cout << "test_std_thread -  get_testtag.i:" << get_testtag.i << std::endl;
	std::cout << "test_std_thread -  get_testtag.j:" << get_testtag.j << std::endl;

	//testtag.j = 8;
	std::cout << "test_std_thread ... " << std::endl;
	std::function<void(void)> startfunc = std::bind(runThread_, nullptr);
	thread t1(startfunc);
	thread t2(startfunc);
	std::cout << "test_std_thread -  t1.joinable:" << t1.joinable() << std::endl;
	std::cout << "test_std_thread -  t2.joinable:" << t2.joinable() << std::endl;
	t1.join();
	t2.join();
	std::cout << "test_std_thread -  total:" << total << std::endl;
	return 0;
}

#endif



/*
// ATOMIC TYPEDEFS
typedef atomic<bool>				atomic_bool;
typedef atomic<char>				atomic_char;
typedef atomic<signed char>			atomic_schar;
typedef atomic<unsigned char>		atomic_uchar;
typedef atomic<short>				atomic_short;
typedef atomic<unsigned short>		atomic_ushort;
typedef atomic<int>					atomic_int;
typedef atomic<unsigned int>		atomic_uint;
typedef atomic<long>				atomic_long;
typedef atomic<unsigned long>		atomic_ulong;
typedef atomic<long long>			atomic_llong;
typedef atomic<unsigned long long>	atomic_ullong;
*/
#ifndef _MEMPOOL_H__
#define _MEMPOOL_H__
#include <atomic>
#include <thread>
#include <iostream>
#include<math.h>

unsigned int fit_block_size(unsigned int size)
{
	unsigned int i = 10;
	size = (size >> 10);					// 2^10 = 1024，所以首先 size = size / 1024;
	for (; size; i++, size = (size >> 1));  // 2^1 = 2，所以 size = size /2; 不断的循环直到 size = 0; 因为是 unsigned int 类型,会把小数自动清除
	return 1 << (i<10 ? 10 : i);			// i<10 ? 10 : i 这个保证了 最小返回 n >= 10，也就是 2^n ; 2^10 = 1024; 2^11 = 2048; 2^12=4096; 2^13=8192; 2^14=16384; 2^15=32768;
}

int test_mempool()
{
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	for (unsigned int i = 0; i < 16; i++)
	{
		unsigned int size  =fit_block_size(i);
		std::cout << "test_mempool - i:" << i << " size:" << size << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	for (unsigned int i = 1022; i < 1026; i++)
	{
		unsigned int size = fit_block_size(i);
		std::cout << "test_mempool - i:" << i << " size:" << size << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	for (unsigned int i = 2044; i < 2050; i++)
	{
		unsigned int size = fit_block_size(i);
		std::cout << "test_mempool - i:" << i << " size:" << size << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	for (unsigned int i = 4094; i < 4099; i++)
	{
		unsigned int size = fit_block_size(i);
		std::cout << "test_mempool - i:" << i << " size:" << size << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	for (unsigned int i = 0; i < 64; i++)
	{
		unsigned int size = pow(2, i);
		std::cout << "test_mempool - i:" << i << " size:" << size << std::endl;
	}

	return 0;
}

#endif


//test_mempool - i : 0  size : 1
//test_mempool - i : 1  size : 2
//test_mempool - i : 2  size : 4
//test_mempool - i : 3  size : 8
//test_mempool - i : 4  size : 16
//test_mempool - i : 5  size : 32
//test_mempool - i : 6  size : 64
//test_mempool - i : 7  size : 128
//test_mempool - i : 8  size : 256
//test_mempool - i : 9  size : 512
//test_mempool - i : 10 size : 1024
//test_mempool - i : 11 size : 2048
//test_mempool - i : 12 size : 4096
//test_mempool - i : 13 size : 8192
//test_mempool - i : 14 size : 16384
//test_mempool - i : 15 size : 32768
//test_mempool - i : 16 size : 65536
//test_mempool - i : 17 size : 131072
//test_mempool - i : 18 size : 262144
//test_mempool - i : 19 size : 524288
//test_mempool - i : 20 size : 1048576
//test_mempool - i : 21 size : 2097152
//test_mempool - i : 22 size : 4194304
//test_mempool - i : 23 size : 8388608
//test_mempool - i : 24 size : 16777216
//test_mempool - i : 25 size : 33554432
//test_mempool - i : 26 size : 67108864
//test_mempool - i : 27 size : 134217728
//test_mempool - i : 28 size : 268435456
//test_mempool - i : 29 size : 536870912
//test_mempool - i : 30 size : 1073741824
//test_mempool - i : 31 size : 2147483648

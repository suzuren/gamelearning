
#include <stdio.h>
#include <string.h>
#include "memory_cache.h"


int main(int argc, const char** argv)
{
	printf("test memory poll ...\n");

	CMemPool * _mem_pool = new(CMemPool);
	if (_mem_pool == NULL)
	{
		return -1;
	}
	CMemCache _mem_cache(*_mem_pool);

	_mem_cache.skip(_mem_cache.data_len());

	const char * ptr = "hello world mem pool-";
	_mem_cache.append(ptr, strlen(ptr));
	char * data = _mem_cache.data();
	unsigned int length = _mem_cache.data_len();
	unsigned int alloc_size = _mem_pool->alloc_size();
	printf("mem cache - alloc_size:%d,length:%d,data:%s\n", alloc_size,length, data);

	_mem_cache.append(ptr, strlen(ptr));
	_mem_cache.append(ptr, strlen(ptr));


	data = _mem_cache.data();
	length = _mem_cache.data_len();
	alloc_size = _mem_pool->alloc_size();
	printf("mem cache - alloc_size:%d,length:%d,data:%s\n", alloc_size, length, data);

	_mem_cache.skip(strlen(ptr));

	data = _mem_cache.data();
	length = _mem_cache.data_len();
	alloc_size = _mem_pool->alloc_size();
	printf("mem cache - alloc_size:%d,length:%d,data:%s\n", alloc_size, length, data);


	_mem_cache.skip(_mem_cache.data_len());


	return 0;
}


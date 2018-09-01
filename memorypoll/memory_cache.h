
#ifndef _COMM_SOCKCOMMU_CACHE_H_
#define _COMM_SOCKCOMMU_CACHE_H_

#include "memory_pool.h"

//纯cache对象
class CMemCache
{
public:
	CMemCache(CMemPool& mp);
    ~CMemCache();

    char* data();
    unsigned int data_len();
    void append(const char* data, unsigned data_len);
    void skip(unsigned length);

private:
    //内存池对象
    CMemPool& _mem_pool;
    //内存的存根
    char* _mem_stub;
    //内存大小
    unsigned int _block_size;
    //实际使用内存起始偏移量
    unsigned int _data_head;
    //实际使用内存长度
    unsigned int _data_len;
};

#endif


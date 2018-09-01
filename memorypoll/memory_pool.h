
#ifndef _COMM_MEMPOOL_H_
#define _COMM_MEMPOOL_H_


static const unsigned int C_EXTEND_SET = 128;
static const unsigned int C_SLOT_1 = 1 << 20;					// 1M = 2^20 = 2^10 * 2^10 = 1024 * 1024 = 1048576
static const unsigned int C_SLOT_2 = 1 << 26;					// 64M = 2^26 = 2^6 * 2^20 = 64 * 2^20 = 67108864
static const unsigned int C_ALLOC_CAPCITY = 2 * C_SLOT_2;		// 128M = 2^27 = 2^7 * 2^20 = 128 * 2^20 = 134217728
static const unsigned int C_MAX_POOL_SIZE = 2000 * (1 << 20);	// 2G = 2 * 1000 * 1M = 2097152000
static const unsigned int C_MAX_WATER_MARK = 1 << 30;			// 1G = 

#define USE_JE_MALLOC

#ifdef USE_JE_MALLOC
#	include "/root/code/jemalloc/include/jemalloc/jemalloc.h"
#	define MEM_MALLOC 	je_malloc
#	define MEM_REMALLOC je_realloc
#	define MEM_FREE		je_free
#else
#	define MEM_MALLOC 	malloc
#	define MEM_REMALLOC realloc
#	define MEM_FREE		free
#endif

class CMemPool
{
public:
    CMemPool()
    {
        _allocated_size = 0;
    }
    ~CMemPool()
	{
		_allocated_size = 0;
	}
	
	void * reallocate(void * mem, unsigned int size, unsigned int & result_size);

    void* allocate(unsigned int size, unsigned int & result_size);

    int recycle(void* mem, unsigned int mem_size);
    
	unsigned int alloc_size()
	{
		return _allocated_size;
	}
private:
    unsigned int fit_block_size(unsigned int size)
    {
        unsigned int i = 10;
        size = (size>>10);
        for(; size; i++, size = (size>>1));
        return 1 << (i<10 ? 10 : i);
    }
    //已分配内存总大小
    unsigned int _allocated_size;
};

#endif


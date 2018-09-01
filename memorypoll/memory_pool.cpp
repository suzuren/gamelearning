
#include "memory_pool.h"


//////////////////////////////////////////////////////////////////////////

void* CMemPool::allocate(unsigned int size, unsigned int & result_size)
{
	unsigned int block_size = fit_block_size(size);
	void * allocated_mem = NULL;
	result_size = 0;
	if (_allocated_size + block_size > C_MAX_POOL_SIZE)
	{
		return NULL;
	}
	allocated_mem = MEM_MALLOC(block_size);
	if (allocated_mem != NULL)
	{
		result_size = block_size;
		_allocated_size += block_size;
	}
	return allocated_mem;
}

void * CMemPool::reallocate(void * mem, unsigned int size, unsigned int & result_size)
{
    unsigned int block_size = fit_block_size(size);
    void * allocated_mem = NULL;
	result_size = 0;
    if (mem == NULL || _allocated_size + block_size > C_MAX_POOL_SIZE)
    {
        return NULL;
    }
    allocated_mem = MEM_REMALLOC(mem, block_size);
    if (allocated_mem != NULL)
    {
        result_size = block_size;
        _allocated_size += block_size;
    }    
    return allocated_mem;
}

int CMemPool::recycle(void* mem, unsigned int mem_size)
{
	int ret = -1;
	if (mem != NULL && mem_size <= _allocated_size)
	{
		_allocated_size -= mem_size;
		MEM_FREE(mem);
		ret = 0;
	}
	return ret;
}

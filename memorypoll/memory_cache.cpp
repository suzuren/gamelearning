
#include <string.h>
#include <assert.h>
#include "memory_cache.h"

CMemCache::CMemCache(CMemPool& mp) : _mem_pool(mp), _mem_stub(NULL), _block_size(0), _data_head(0), _data_len(0)
{
}

CMemCache::~CMemCache()
{
}

char* CMemCache::data()
{
	if (_data_len == 0)
	{
		return NULL;
	}
    assert(_data_head < _block_size);
    return _mem_stub + _data_head;
}

unsigned int CMemCache::data_len()
{
	return _data_len;
}

void CMemCache::append(const char* data, unsigned data_len)
{
    //assert(data_len < 0x00010000);
    if (_mem_stub == NULL)
    {
		_mem_stub = (char*)_mem_pool.allocate(data_len, _block_size);
        assert(_mem_stub);
		if (_mem_stub == NULL)
		{
			throw(0);
		}
        memcpy(_mem_stub, data, data_len);
        _data_head = 0;
        _data_len = data_len;
        return;
    }    
    //
    //	data_len < _block_size - _data_head - _data_len
    //	append directly
    //
    if (data_len + _data_head + _data_len <= _block_size)
    {
        memcpy(_mem_stub + _data_head + _data_len, data, data_len);
        _data_len += data_len;
    }
    //
    //	_block_size-_data_len <= data_len
    //	reallocate new block. memmove, recycle
    //
    else if (data_len + _data_len > _block_size)
    {
        unsigned new_block_size = 0;
        char* mem = (char*)_mem_pool.allocate(data_len+_data_len, new_block_size);
        //assert(mem);
        if(!mem)
        throw(0);

        memcpy(mem, _mem_stub + _data_head, _data_len);
        memcpy(mem + _data_len, data, data_len);
		_mem_pool.recycle(_mem_stub, _block_size);

		_mem_stub = mem;
        _block_size = new_block_size;
        _data_head = 0;
        _data_len += data_len;
    }
    //
    //	_block_size - _data_head - _data_len < data_len < _block_size-_datalen
    //	memmove data to block head, append new data
    //
    else
    //if ((data_len + _data_head + _data_len > _block_size) && (data_len + _data_len < _block_size))
    {
		memmove(_mem_stub, _mem_stub + _data_head, _data_len);
		memcpy(_mem_stub + _data_len, data, data_len);

        _data_head = 0;
        _data_len += data_len;
    }
}
void CMemCache::skip(unsigned length)
{
    //	empty data
	if (_mem_stub == NULL)
	{
		return;
	}
    //	not enough data
    else if (length >= _data_len)
    {
		_mem_pool.recycle(_mem_stub, _block_size);
		_mem_stub = NULL;
		_block_size = 0;
        _data_head = 0;
        _data_len = 0;
    }
    //	skip part of data
    else
    {
        _data_head += length;
        _data_len -= length;
    }
}

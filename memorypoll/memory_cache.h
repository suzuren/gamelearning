
#ifndef _COMM_SOCKCOMMU_CACHE_H_
#define _COMM_SOCKCOMMU_CACHE_H_

#include "memory_pool.h"

//��cache����
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
    //�ڴ�ض���
    CMemPool& _mem_pool;
    //�ڴ�Ĵ��
    char* _mem_stub;
    //�ڴ��С
    unsigned int _block_size;
    //ʵ��ʹ���ڴ���ʼƫ����
    unsigned int _data_head;
    //ʵ��ʹ���ڴ泤��
    unsigned int _data_len;
};

#endif


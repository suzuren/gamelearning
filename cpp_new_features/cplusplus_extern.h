
#ifndef _CPLUSPLUS_H__
#define _CPLUSPLUS_H__

#include <stdio.h>

//�������͵�ͷ�ļ����Ա�#include��C�ļ��н��б��룬Ҳ���Ա�#include��C++�ļ��н��б��롣
//����extern "C"��������C++�Ժ��������������ȷ���(symbol)������������(name mangling)��
//��˱������CĿ���ļ���C++Ŀ���ļ��еı������������Ƶȷ��Ŷ�����ͬ��(������ͬ)��
//���������Կɿ��ض��������͵�Ŀ���ļ��������ӡ�������������Ϊ��C��C++����ͷ�ļ��ĵ���������
#ifdef __cplusplus
extern "C"{
#endif

	void test_extern_c()
	{
		int i = 0;
		printf("test_extern_c - i:%d\n", i);

		long long llCppValue = __cplusplus;
		printf("test_extern_c - llCppValue:%lld\n", llCppValue);
	}

#ifdef __cplusplus
}
#endif




#endif


#ifndef _CPLUSPLUS_H__
#define _CPLUSPLUS_H__

#include <stdio.h>

//这种类型的头文件可以被#include到C文件中进行编译，也可以被#include到C++文件中进行编译。
//由于extern "C"可以抑制C++对函数名、变量名等符号(symbol)进行名称重整(name mangling)，
//因此编译出的C目标文件和C++目标文件中的变量、函数名称等符号都是相同的(否则不相同)，
//链接器可以可靠地对两种类型的目标文件进行链接。这样该做法成为了C与C++混用头文件的典型做法。
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

#ifndef _DEFINE_H__
#define _DEFINE_H__

#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

const char * hello() {
	return __func__;
}
const char * world() {
	return __func__;
}

int test_define()
{
	printf("test_define ... \n");
	
	cout << "test_define - clib: " << __STDC_HOSTED__ << endl;
	cout << "test_define - c: " << __STDC__ << endl;
	//cout << "c version: " << __STDC_VERSION__ << endl;
	cout << "test_define - ISO/IEC: " << __STDC_ISO_10646__ << endl;

	cout << "test_define - func name: " << __func__ << " hello:" << hello() << " world: " << world() << endl;

	return 0;
}


#define TEST_LOG_PRINTF(...) \
{ \
	fprintf(stderr,"%s:line:%d-",__FILE__,__LINE__); \
	fprintf(stderr,__VA_ARGS__); \
	fprintf(stderr,"\n"); \
}

void test_long_size()
{
	//long long llValveMin = LLONG_MIN;
	//long long llValveMax = LLONG_MAX;
	//unsigned long long ullValveMax = ULLONG_MAX;
	//printf("llValveMin:%lld,llValveMax:%lld,ullValveMax:%lld\n", llValveMin, llValveMax ullValveMax);
	int llsize = sizeof(long long);
	int ullsize = sizeof(unsigned long long);
	// 
	printf("test_long_size - llsize:%d,ullsize:%d\n", llsize, ullsize);
	// llsize:8,ullsize:8

	long long llValveMin = -9223372036854775807;
	long long llValveMax = 9223372036854775807;
	unsigned long long llValveMaxAdd = llValveMax + llValveMax;
	unsigned long long ullValveMax = 0xffffffffffffffff;
	printf("test_long_size - llValveMin:%lld,llValveMax:%lld,llValveMaxAdd:%llu,ullValveMax:%llu\n", llValveMin, llValveMax, llValveMaxAdd, ullValveMax);
//llValveMin:-9223372036854775807, llValveMax : 9223372036854775807, llValveMaxAdd : 18446744073709551614, ullValveMax : 18446744073709551615

	static_assert(sizeof(int) == 4, "test_long_size - this 64bit machine should follow this");

}

#endif

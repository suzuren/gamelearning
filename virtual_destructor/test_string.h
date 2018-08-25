
#include <stdio.h>
#include <iostream>
#include <string.h>


int bd_string(char * instr)
{
	if (instr == NULL) return 0;
	char * ptr = instr;
	int k = 0;
	while ((*(instr + k)) != '\0')
	{
		int i = 1 + k;
		while ((*(ptr + i)) != '\0')
		{
			if ((*(instr + k)) == (*(ptr + i)))
			{
				return 1;
			}
			i++;
		}
		k++;
	}
	return 0;
}

char * GetMemory()
{
	// 返回临时变量 所以错误 需要加上static 静态
	char p[] = "hello";
	printf("GetMemory - p:%s\n", p);

	return p;
}

void test_string()
{
	char * p1 = "hello";
	char p2[5];	p2[0] = 'h'; p2[1] = 'e'; p2[2] = 'l'; p2[3] = 'l'; p2[4] = 'o';
	char p3[6];	p3[0] = 'h'; p3[1] = 'e'; p3[2] = 'l'; p3[3] = 'l'; p3[4] = 'o'; p3[5] = '\0';
	//char p2[] = "hello";
	//char p3[] = "hello\0";


	int L1 = strlen(p1);
	int L2 = strlen(p2);
	int L3 = strlen(p3);
	printf("L1:%d,L2:%d,L3:%d\n", L1, L2, L3);

	char * input_str = "hello";
	int ret = bd_string(input_str);
	printf("bd_string - ret1:%d\n", ret);

	char * input_str2 = "helo";
	int ret2 = bd_string(input_str2);
	printf("bd_string - ret2:%d\n", ret2);


	char * ret_ptr = GetMemory();
	printf("GetMemory - ret_ptr:%s\n", ret_ptr);
}



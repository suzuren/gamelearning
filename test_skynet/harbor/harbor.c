
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
//--------------------------------------------------------------------------
static unsigned int HARBOR = ~0;

// reserve high 8 bits for remote id
#define HANDLE_MASK 0xffffff
#define HANDLE_REMOTE_SHIFT 24

// 0xffffff -> 0000 0000   ?1111 1111   1111 1111   1111 1111?


//--------------------------------------------------------------------------


int
skynet_harbor_message_isremote(uint32_t handle) {
	assert(HARBOR != ~0);
	int h = (handle & ~HANDLE_MASK);

	//printf("skynet_harbor_message_isremote 1 - HARBOR:%d\n", HARBOR);
	//printf("skynet_harbor_message_isremote 2 - handle:%d\n", handle);
	//printf("skynet_harbor_message_isremote 3 - h:%d\n", h);
	return h != HARBOR && h != 0;
}


void
skynet_harbor_init(int harbor) {
	HARBOR = (unsigned int)harbor << HANDLE_REMOTE_SHIFT;
	// << ��������� -> HARBOR = harbor * 2^HANDLE_REMOTE_SHIFT = 
}

//--------------------------------------------------------------------------


char* itoa_parser(unsigned int num, int radix)
{
	static char str[32];
	memset(str, 0, sizeof(str));
	static char index[17] = "0123456789ABCDEF";
	unsigned int unum;
	int i = 0, j, k;
	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else
	{
		unum = (unsigned)num;
	}
	do
	{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	if (str[0] == '-')
	{
		k = 1;
	}
	else
	{
		k = 0;
	}

	char temp;
	for (j = k; j <= (i - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}

void test_isremote()
{

	// �����ַ��һ�� 32bit ������ͬһ�����ڵĵ�ַ�ĸ� 8bit ��ͬ���� 8bit ������һ���������Ǹ��ڵ㡣����ֻ�ܱ�ʾ2^8 = 256������skynetһ��ֻ�ܱ�ʶ256���ڵ�
	//unsigned int init_harbor = 1 << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_1 = (1 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_2 = (2 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_3 = (3 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_4 = (4 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_5 = (5 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_6 = (6 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_7 = (7 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_8 = (8 & 0xff) << HANDLE_REMOTE_SHIFT;
	unsigned int init_harbor_9 = (9 & 0xff) << HANDLE_REMOTE_SHIFT;
	
	//printf("test_isremote - init_harbor_1:%d,init_harbor_2:%d, init_harbor_3:%d, init_harbor_4:%d, init_harbor_5:%d, init_harbor_6:%d, init_harbor_7:%d, init_harbor_8:%d, init_harbor_9:%d\n",
	//	init_harbor_1, init_harbor_2, init_harbor_3, init_harbor_4, init_harbor_5, init_harbor_6, init_harbor_7, init_harbor_8, init_harbor_9);

	unsigned int arr_harbor[] = { init_harbor_1, init_harbor_2, init_harbor_3, init_harbor_4, init_harbor_5, init_harbor_6, init_harbor_7, init_harbor_8, init_harbor_9 };
	for (int i = 0; i < sizeof(arr_harbor) / sizeof(arr_harbor[0]); i++)
	{
		//char buffer[512] = { 0 };
		char * ptr = itoa_parser(arr_harbor[i], 2);
		printf("init_harbor -  i:%d,buffer:%s\t,harbor:%d\n",i+1, ptr, arr_harbor[i]);
	}

	
	//init_harbor -  i:0,buffer:0001 0000  00000000000000000000,harbor:16777216
	//init_harbor -  i:1,buffer:0010 0000  00000000000000000000,harbor:33554432
	//init_harbor -  i:2,buffer:0011 0000  00000000000000000000,harbor:50331648
	//init_harbor -  i:3,buffer:0100 0000  00000000000000000000,harbor:67108864
	//init_harbor -  i:4,buffer:0101 0000  00000000000000000000,harbor:83886080
	//init_harbor -  i:5,buffer:0110 0000  00000000000000000000,harbor:100663296
	//init_harbor -  i:6,buffer:0111 0000  00000000000000000000,harbor:117440512
	//init_harbor -  i:7,buffer:1000 0000  00000000000000000000,harbor:134217728
	//init_harbor -  i:8,buffer:1001 0000  00000000000000000000,harbor:150994944

	
		//  A = 0011 1100
		//  B = 0000 1101
		//-----------------
		//A&B = 0000 1100
		//A|B = 0011 1101
		//A^B = 0011 0001
		//~A  = 1100 0011

	// init_harbor_1:16777216 -> ?0001000000000000000000000000?
	// init_harbor_2:33554432 -> ?0010000000000000000000000000?
	int handle_index_1 = 1;
	int slot_size = 4;
	int handle_count_1 = 0;
	uint32_t arr_handle_1[512] = { 0 };
	for (int i = 0; i<slot_size; i++)
	{
		// �����ַ��һ�� 32bit ������ͬһ�����ڵĵ�ַ�ĸ� 8bit ��ͬ���� 8bit ������һ���������Ǹ��ڵ㡣
		uint32_t handle = (i + handle_index_1) & HANDLE_MASK;  // ����handle��ʱ��ֻȡǰ��24λ��ǰ��8λ��ȡ��
		int hash = handle & (slot_size - 1);
		handle_index_1 = handle + 1;
		handle |= init_harbor_1;    // �� init_harbor_1 ����Ķ����������1��ȡ�����ŵ� handle���棬���൱�� handle������ init_harbor_1��ǰ��8λ����Ϊ�����24λ����0,
									// ÿһ��harbor��ǰ��8λ����һ�������Ǻ����24λ����0�����Կ��Ը���handle��ǰ��8λ�ж��Ƿ���ͬһ��harbor����������������ģ�
									// Ҳ�Ϳ����ж����handle�Ƿ�����Զ�˵Ľڵ�

		arr_handle_1[handle_count_1++] = handle;

		printf("harbor 1 - slot_size:%d, handle:%02d,handle_index_1:%02d,hash:%d\n",
			slot_size, handle, handle_index_1, hash);
	}

	int handle_count_2 = 0;
	int handle_index_2 = 1;
	uint32_t arr_handle_2[512] = { 0 };
	for (int i = 0; i<slot_size; i++)
	{
		uint32_t handle = (i + handle_index_2) & HANDLE_MASK;
		int hash = handle & (slot_size - 1);
		handle_index_2 = handle + 1;
		handle |= init_harbor_2;
		arr_handle_2[handle_count_2++] = handle;

		printf("harbor 2 - slot_size:%d, handle:%02d,handle_index_2:%02d,hash:%d\n",
			slot_size, handle, handle_index_2, hash);
	}



	for (int i = 0; i < slot_size; i++)
	{
		uint32_t handle = arr_handle_1[i];
		assert(init_harbor_1 != ~0);      // �ж�Ǯ init_harbor_1��Ҫ��ʼ����Ĭ�� init_harbor_1 = ~0���� static unsigned int HARBOR = ~0;

		int h = (handle & ~HANDLE_MASK);  // ����������handle��ʱ����Ϊ�� harbor�ĸ߰�λ�ӽ�ȥ�� �����ж�handle�Ƿ���ͬһ��harbor���ɵ�ʱ��
										  // �Ͱ�handle�ĵ�24λȥ����ֻȡ�߰�λ�Ϳ����ˣ������ӾͿ���ʹ�� h �Ƿ�� init_harbor_1��ͬ�����жϣ���Ϊinit_harbor_1�ĵ�24λ����0
										  // ����ȥ�� handle �ĵ�24λ��ֻ����8λ�������ʹ��init_harbor_1���ɵ�handle����ôh�͵��� init_harbor_1										
		bool isremote = (h != init_harbor_1 && h != 0);
		printf("isremote 1  -slot_size:%d, handle:%d,init_harbor_1:%d,h:%d,isremote:%d\n",
			slot_size, handle, init_harbor_1, h, isremote);
	}

	for (int i = 0; i < slot_size; i++)
	{
		uint32_t handle = arr_handle_2[i];
		assert(init_harbor_2 != ~0);
		int h = (handle & ~HANDLE_MASK);
		bool isremote = (h != init_harbor_1 && h != 0);
		printf("isremote 2  -slot_size:%d, handle:%d,init_harbor_1:%d,h:%d,isremote:%d\n",
			slot_size, handle, init_harbor_1, h, isremote);
	}
}

int main(int argc, char *argv[])
{
    printf("harbor 1 - HARBOR:%d\n",HARBOR);
    printf("harbor 2 - HARBOR:%u\n",HARBOR);
    skynet_harbor_init(1); // 1<<24 = 2^24 = 16777216
    printf("harbor 3 - HARBOR:%u\n",HARBOR);


	int isremote = skynet_harbor_message_isremote(HARBOR);
	printf("harbor 4 - isremote:%d\n", isremote);


	test_isremote();



     return 0;
}

//harbor 1 - HARBOR:-1
//harbor 2 - HARBOR:4294967295
//harbor 2 - HARBOR:16777216


//--------------------------------------------------------------------------

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
	// << 左移运算符 -> HARBOR = harbor * 2^HANDLE_REMOTE_SHIFT = 
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

	// 服务地址是一个 32bit 整数，同一进程内的地址的高 8bit 相同。这 8bit 区分了一个服务处于那个节点。所以只能表示2^8 = 256，所以skynet一共只能标识256个节点
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
		// 服务地址是一个 32bit 整数，同一进程内的地址的高 8bit 相同。这 8bit 区分了一个服务处于那个节点。
		uint32_t handle = (i + handle_index_1) & HANDLE_MASK;  // 生成handle的时候只取前面24位，前面8位不取，
		int hash = handle & (slot_size - 1);
		handle_index_1 = handle + 1;
		handle |= init_harbor_1;    // 把 init_harbor_1 里面的二进制里面的1都取出来放到 handle里面，就相当于 handle加上了 init_harbor_1的前面8位，因为后面的24位都是0,
									// 每一个harbor的前面8位都不一样，但是后面的24位都是0，所以可以根据handle的前面8位判断是否是同一个harbor相或操作运算符出来的，
									// 也就可以判断这个handle是否属于远端的节点

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
		assert(init_harbor_1 != ~0);      // 判断钱 init_harbor_1需要初始化，默认 init_harbor_1 = ~0，如 static unsigned int HARBOR = ~0;

		int h = (handle & ~HANDLE_MASK);  // 在上面生成handle的时候，因为把 harbor的高八位加进去了 所以判断handle是否是同一个harbor生成的时候，
										  // 就把handle的低24位去掉，只取高八位就可以了，这样子就可以使用 h 是否和 init_harbor_1相同，来判断，因为init_harbor_1的低24位都是0
										  // 所以去掉 handle 的低24位，只留高8位，如果是使用init_harbor_1生成的handle，那么h就等于 init_harbor_1										
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
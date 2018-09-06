#ifndef SKYNET_TIMER_H
#define SKYNET_TIMER_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
//--------------------------------------------------------------------------


#define skynet_malloc malloc
#define skynet_free	  free


#define PTYPE_TEXT 0
#define PTYPE_RESPONSE 1
#define PTYPE_MULTICAST 2
#define PTYPE_CLIENT 3
#define PTYPE_SYSTEM 4
#define PTYPE_HARBOR 5
#define PTYPE_SOCKET 6

//--------------------------------------------------------------------------

struct skynet_message {
	uint32_t source;
	int session;
	void * data;
	size_t sz;
};

// type is encoding in skynet_message.sz high 8bit
#define MESSAGE_TYPE_MASK (SIZE_MAX >> 8)
#define MESSAGE_TYPE_SHIFT ((sizeof(size_t)-1) * 8)

//uint64_t skynet_now(void);

//--------------------------------------------------------------------------

#define TIME_NEAR_SHIFT 8
#define TIME_NEAR (1 << TIME_NEAR_SHIFT)	// 2^8 = 256 -> 0001 0000 0000
#define TIME_NEAR_MASK (TIME_NEAR-1)		//       255 -> 0000 1111 1111

#define TIME_LEVEL_SHIFT 6					// 6
#define TIME_LEVEL (1 << TIME_LEVEL_SHIFT)	// 2^6 = 64  -> 0100 0000
#define TIME_LEVEL_MASK (TIME_LEVEL-1)		// 64-1= 63  -> 0011 1111

//--------------------------------------------------------------------------

//设置超时时间及相应的回调消息，如果time <= 0, 立马派送消息，如果time>0, 将其加入计时器列表中，等时间到达后派发。time时间精度为百分之一秒
int skynet_timeout(uint32_t handle, int time, int session);
//程序更新时间并触发相应计时器事件
void skynet_updatetime(void);

uint32_t skynet_starttime(void);
uint64_t skynet_thread_time(void);	// for profile, in micro second

void skynet_timer_init(void);

#endif

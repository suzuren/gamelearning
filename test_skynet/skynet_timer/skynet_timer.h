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

//���ó�ʱʱ�估��Ӧ�Ļص���Ϣ�����time <= 0, ����������Ϣ�����time>0, ��������ʱ���б��У���ʱ�䵽����ɷ���timeʱ�侫��Ϊ�ٷ�֮һ��
int skynet_timeout(uint32_t handle, int time, int session);
//�������ʱ�䲢������Ӧ��ʱ���¼�
void skynet_updatetime(void);

uint32_t skynet_starttime(void);
uint64_t skynet_thread_time(void);	// for profile, in micro second

void skynet_timer_init(void);

#endif

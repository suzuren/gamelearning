#ifndef _EXAMPLE_TEST_H
#define _EXAMPLE_TEST_H

#include "skynet_malloc.h"
#include "skynet_mq.h"

#define PTYPE_TEXT 0
#define PTYPE_RESPONSE 1
#define PTYPE_MULTICAST 2
#define PTYPE_CLIENT 3
#define PTYPE_SYSTEM 4
#define PTYPE_HARBOR 5
#define PTYPE_SOCKET 6
// read lualib/skynet.lua examples/simplemonitor.lua
#define PTYPE_ERROR 7	
// read lualib/skynet.lua lualib/mqueue.lua lualib/snax.lua
#define PTYPE_RESERVED_QUEUE 8
#define PTYPE_RESERVED_DEBUG 9
#define PTYPE_RESERVED_LUA 10
#define PTYPE_RESERVED_SNAX 11

#define PTYPE_TAG_DONTCOPY 0x10000
#define PTYPE_TAG_ALLOCSESSION 0x20000


#include <stdio.h>
#include <string.h>
#include <assert.h>

struct drop_t {
	uint32_t handle;
};

static void
drop_message(struct skynet_message *msg, void *ud) {
	struct drop_t *d = ud;
	//skynet_free(msg->data);
	uint32_t source = d->handle;
	printf("drop_message - source:%d,session:%d\n", source, msg->session);
	assert(source);
	// report error to the message source
	//skynet_send(NULL, source, msg->source, PTYPE_ERROR, 0, NULL, 0);
}

void test_skynet_mq()
{
	// ��ʼ��ȫ����Ϣ����
	skynet_mq_init();

	uint32_t handle = 1;
	// ����������Ϣ����
	struct message_queue * q1 = skynet_mq_create(handle);

	// ������Ϣ
	struct skynet_message msg;
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_1";
	size_t sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// ����Ϣ�Ž�������Ϣ����
	skynet_mq_push(q1, &msg);

	// ������Ϣ
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_2";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// ����Ϣ�Ž�������Ϣ����
	skynet_mq_push(q1, &msg);
	// �Ѷ�����Ϣ���з���ȫ����Ϣ����
	skynet_globalmq_push(q1);

	// ---------------------------------------------------------------------------------

	handle = 2;
	// ����������Ϣ����
	struct message_queue * q2 = skynet_mq_create(handle);

	// ʹ�������й��� �������
	//for (int i = 0; i < 1024 + 2; i++)
	{
		msg.source = handle;
		msg.session = handle;
		msg.data = "queue_2_msg";
		sz = strlen(msg.data);
		msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
		skynet_mq_push(q2, &msg);
	}

	// �Ѷ�����Ϣ���з���ȫ����Ϣ����
	skynet_globalmq_push(q2);

	handle = 3;
	struct message_queue * q3 = skynet_mq_create(handle);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_3_msg_1";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q3, &msg);

	skynet_globalmq_push(q3);

	handle = 4;
	struct message_queue * q4 = skynet_mq_create(handle);
	skynet_globalmq_push(q4);

	handle = 5;
	struct message_queue * q5 = skynet_mq_create(handle);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_5_msg_1";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q5, &msg);

	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_5_msg_2";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	skynet_mq_push(q5, &msg);


	skynet_globalmq_push(q5);


	// ���ж������ж���Ҫ����Ϣ
	// ���û����Ϣ ��ִ�� skynet_mq_pop(q, &msg); ȡ����Ϣ��ʱ�� ��ȡ��Ϣʧ�ܣ�
	// ���ʱ�� in_global ��־���ƿգ�������Ϣ����ȫ�ֶ������棬
	// �� skynet_mq_mark_release(q); ����ͷŶ��е�ʱ�򣬻��ж� in_global ��־�Ƿ���ȫ�ֶ�������
	// �������ȫ�ֶ�������ִ�� skynet_globalmq_push(q);�����Ż�ȫ�ֶ������棬
	// ��������������� skynet_mq_release(q, drop_message, &d); �Ѿ��� q �ͷŵ��ˣ�
	// ���ʱ��Ž�ȫ�ֶ�������Ķ����������Ѿ����ͷŹ��ģ������ٴ�ѭ��������һ����������q��ʱ��
	// ����������� q,�Ѿ����ͷŹ��ˣ������ٴ�ִ�� skynet_mq_mark_release(q); ��ʱ����������
	// assert(q->release == 0); �����ʧ��,���³������.
	// ����
	// ������Ϣʧ��֮�󣬲�Ҫ���skynet_mq_mark_release(q); 
	// Ҫ�� ִ�� skynet_mq_push() �����з�һ����Ϣ�������� in_global �����Ϊȫ�ֶ���״̬������ִ�к��� skynet_globalmq_push �Ż�ȫ�ֶ������棬
	// �ٽ��б���ͷ�

	// handle Ҳ����Ϊ 0 ��Ȼ���ͷŵ�ʱ�� �� drop_message ���������ʧ��,���³������

	for (;;)
	{
		struct message_queue * q = skynet_globalmq_pop();
		printf("skynet_globalmq_pop - q:%p\n", q);

		if (!q)break;

		uint32_t handle = skynet_mq_handle(q);
		int length = skynet_mq_length(q);

		struct skynet_message msg;
		int ret = skynet_mq_pop(q, &msg);
		//skynet_mq_push(q, &msg);

		printf("handle:%d,length:%d,ret:%d\n", handle, length, ret);
		int type = msg.sz >> MESSAGE_TYPE_SHIFT;
		size_t sz = msg.sz & MESSAGE_TYPE_MASK;
		char * chbuf = msg.data;
		if (ret)
		{
			// ������Ϣʧ��
			msg.source = 5;
			msg.session = 5;
			msg.data = "queue_5_msg_1";
			sz = strlen(msg.data);
			msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
			skynet_mq_push(q, &msg);
		}
		else
		{
			printf("source:%d,session:%d,type:%d,sz:%ld,data:%s\n", msg.source, msg.session, type, sz, chbuf);

			int overload = skynet_mq_overload(q);
			if (overload)
			{
				//printf("May overload, message queue length = %d\n", overload);
			}
			skynet_mq_mark_release(q);
			struct drop_t d = { handle };
			skynet_mq_release(q, drop_message, &d);
		}
	}	
}


#endif

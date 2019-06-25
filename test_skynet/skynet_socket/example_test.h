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
	// 初始化全局消息队列
	skynet_mq_init();

	uint32_t handle = 1;
	// 创建二级消息队列
	struct message_queue * q1 = skynet_mq_create(handle);

	// 创建消息
	struct skynet_message msg;
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_1";
	size_t sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// 把消息放进二级消息队列
	skynet_mq_push(q1, &msg);

	// 创建消息
	msg.source = handle;
	msg.session = handle;
	msg.data = "queue_1_msg_2";
	sz = strlen(msg.data);
	msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
	// 把消息放进二级消息队列
	skynet_mq_push(q1, &msg);
	// 把二级消息队列放入全局消息队列
	skynet_globalmq_push(q1);

	// ---------------------------------------------------------------------------------

	handle = 2;
	// 创建二级消息队列
	struct message_queue * q2 = skynet_mq_create(handle);

	// 使二级队列过载 输出数据
	//for (int i = 0; i < 1024 + 2; i++)
	{
		msg.source = handle;
		msg.session = handle;
		msg.data = "queue_2_msg";
		sz = strlen(msg.data);
		msg.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);
		skynet_mq_push(q2, &msg);
	}

	// 把二级消息队列放入全局消息队列
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


	// 所有二级队列都需要有消息
	// 如果没有消息 在执行 skynet_mq_pop(q, &msg); 取出消息的时候 会取消息失败，
	// 这个时候 in_global 标志被制空，二级消息不在全局队列里面，
	// 在 skynet_mq_mark_release(q); 标记释放队列的时候，会判断 in_global 标志是否在全局队列里面
	// 如果不在全局队列里面执行 skynet_globalmq_push(q);把它放回全局队列里面，
	// 但是在下面接下来 skynet_mq_release(q, drop_message, &d); 已经把 q 释放掉了，
	// 这个时候放进全局队列里面的二级队列是已经被释放过的，所以再次循环弹出下一个二级队列q的时候，
	// 这个二级队列 q,已经被释放过了，所以再次执行 skynet_mq_mark_release(q); 的时候函数林里面
	// assert(q->release == 0); 会断言失败,导致程序崩溃.
	// 结论
	// 弹出消息失败之后，不要标记skynet_mq_mark_release(q); 
	// 要先 执行 skynet_mq_push() 给队列放一个消息，让他把 in_global 标记置为全局队列状态，并且执行函数 skynet_globalmq_push 放回全局队列里面，
	// 再进行标记释放

	// handle 也不能为 0 不然在释放的时候 在 drop_message 函数会断言失败,导致程序崩溃

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
			// 弹出消息失败
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

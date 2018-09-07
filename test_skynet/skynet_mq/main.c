
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>

#include "spinlock.h"
#include "skynet_mq.h"

//--------------------------------------------------------------------------
static void
drop_message(struct skynet_message *msg, void *ud) {
	struct drop_t *d = ud;
	skynet_free(msg->data);
	uint32_t source = d->handle;
	assert(source);
	// report error to the message source
	//skynet_send(NULL, source, msg->source, PTYPE_ERROR, 0, NULL, 0);
}

//--------------------------------------------------------------------------


void tets_message_size_and_type()
{
	int len = 1024;
	int type = PTYPE_SOCKET;
	size_t data_info = len | (size_t)type << MESSAGE_TYPE_SHIFT;

	int de_type = data_info >> MESSAGE_TYPE_SHIFT;
	size_t de_size = data_info & MESSAGE_TYPE_MASK;

	printf("tets_message_size_and_type - size_t:%ld,MESSAGE_TYPE_SHIFT:%ld,MESSAGE_TYPE_MASK:%ld,len:%d,type:%d,data_info:%ld - de_size:%ld,de_type:%d\n",sizeof(size_t), MESSAGE_TYPE_SHIFT, MESSAGE_TYPE_MASK,len,type, data_info, de_size, de_type);

	// tets_message_size_and_type - size_t:8,MESSAGE_TYPE_SHIFT:56,MESSAGE_TYPE_MASK:72057594037927935,len:1024,type:6,data_info:432345564227568640 - de_size:1024,de_type:6
	// 72057594037927935  -> 0000 0000 ‭1111 1111   1111 1111   1111 1111   1111 1111   1111 1111   1111 1111   1111 1111‬
    //‭ 432345564227568640 -> 0000 0110 0000 0000   0000 0000   0000 0000   0000 0000   0000 0000   0000 0100   0000 0000‬
	// ---------------------------------------------------------------------------------------------------------------------
	//                    &  0000 0000 0000 0000   0000 0000   0000 0000   0000 0000   0000 0000   0000 0100   0000 0000‬
	// ---------------------------------------------------------------------------------------------------------------------
	//                    ==                                                                             100   0000 0000‬
	// ---------------------------------------------------------------------------------------------------------------------
	//                    ==                                                                             十进制的 1024


	// 432345564227568640 -> 0000 0110 0000 0000   0000 0000   0000 0000   0000 0000   0000 0000   0000 0100   0000 0000‬
	// 右移 56 位         -> 0000 0110 == 十进制的 6

}

int main(int argc, char *argv[])
{
	tets_message_size_and_type();
    printf("test skynet message queue.\n");

	skynet_mq_init();

	unsigned int handle = 16777217;

	struct message_queue * queue = skynet_mq_create(handle);

	skynet_globalmq_push(queue);
	skynet_globalmq_pop();
	
	size_t sz = 1024;
	void * msg = skynet_malloc(sz);
	uint32_t source = 1234;
	int session = 5678;
	int type = PTYPE_TEXT;

	struct skynet_message smsg;
	smsg.source = source;
	smsg.session = session;
	smsg.data = msg;
	smsg.sz = sz | (size_t)type << MESSAGE_TYPE_SHIFT;
	skynet_mq_push(queue, &smsg);

	int length = skynet_mq_length(queue);
	printf("skynet_mq_length - length:%d\n", length);
	handle = skynet_mq_handle(queue);
	printf("skynet_mq_handle - handle:%u\n", handle);

	int ret_pop = skynet_mq_pop(queue, &smsg);
	printf("skynet_mq_pop - ret_pop:%u\n", ret_pop);

	int overload = skynet_mq_overload(queue);
	printf("skynet_mq_overload - overload:%d\n", overload);


	skynet_mq_mark_release(queue);

	struct drop_t d = { handle };
	skynet_mq_release(queue, drop_message, &d);

     return 0;
}


//--------------------------------------------------------------------------
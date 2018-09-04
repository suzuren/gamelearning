
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

int main(int argc, char *argv[])
{
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
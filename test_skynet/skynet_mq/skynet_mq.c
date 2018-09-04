//#include "skynet.h"
#include "skynet_mq.h"
//#include "skynet_handle.h"
#include "spinlock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define DEFAULT_QUEUE_SIZE 64
#define MAX_GLOBAL_MQ 0x10000

// 0 means mq is not in global mq.
// 1 means mq is in global mq , or the message is dispatching.

#define MQ_IN_GLOBAL 1
#define MQ_OVERLOAD 1024

struct message_queue {
	struct spinlock lock;
	uint32_t handle;				// 处理消息服务的handle
	int cap;						// 该节点的数据数组容量
	int head;						// queue数组的头位置
	int tail;						// queue数组的尾位置
	int release;					// 释放标记
	int in_global;					// 是否在全局队列中
	int overload;					// 最大负载
	int overload_threshold;			// 最大负载阀值
	struct skynet_message *queue;	// 该节点的数据数组（该数组循环使用），可以容下 cap 大小个数据,queue 占用
									// 空间大小 skynet_malloc(sizeof(struct skynet_message) * cap);
	struct message_queue *next;		// 下一个队列
};

struct global_queue {
	struct message_queue *head;
	struct message_queue *tail;
	struct spinlock lock;
};

static struct global_queue *Q = NULL;

void 
skynet_globalmq_push(struct message_queue * queue) {
	struct global_queue *q= Q;

	SPIN_LOCK(q)
	assert(queue->next == NULL); // 每次把消息队列挂入全局对立的时候一定是消息对列只有一个节点，
								 // 它的下一个节点指针指向空
	if(q->tail) {
		q->tail->next = queue;
		q->tail = queue;
	} else {
		q->head = q->tail = queue; // 全局队列之前为空，第一次入队，则头指针和尾指针都指向同一个节点
	}
	SPIN_UNLOCK(q)
}

struct message_queue * 
skynet_globalmq_pop() {
	struct global_queue *q = Q;

	SPIN_LOCK(q)
	struct message_queue *mq = q->head;
	if(mq) { // 把消息队列移出全局队列，首先全局队列不能为空
		q->head = mq->next; // 头指向第一个
		if(q->head == NULL) // 如果最初的第一个节点的下一个节点指针是为空，则说明这个全局队列只有一个节点
		{
			assert(mq == q->tail); // 只有一个节点的时候，头指针和尾指针都是指向同一个节点
			q->tail = NULL; // 把唯一的节点弹出后 尾指针需要指向空
		}
		mq->next = NULL; // 节点弹出不能关联其他节点，需要把下一个的节点指针置空
	}
	SPIN_UNLOCK(q)

	return mq;
}

struct message_queue * 
skynet_mq_create(uint32_t handle) {
	struct message_queue *q = skynet_malloc(sizeof(*q));
	q->handle = handle;
	q->cap = DEFAULT_QUEUE_SIZE;
	q->head = 0;
	q->tail = 0;
	SPIN_INIT(q)
	// When the queue is create (always between service create and service init) ,
	// set in_global flag to avoid push it to global queue .
	// If the service init success, skynet_context_new will call skynet_mq_push to push it to global queue.
	q->in_global = MQ_IN_GLOBAL;
	q->release = 0;
	q->overload = 0;
	q->overload_threshold = MQ_OVERLOAD;
	q->queue = skynet_malloc(sizeof(struct skynet_message) * q->cap);
	q->next = NULL; // 刚创建的队列只有一个节点，下一个队列的指针指向空

	return q;
}

static void 
_release(struct message_queue *q) {
	assert(q->next == NULL);
	SPIN_DESTROY(q)
	skynet_free(q->queue);
	skynet_free(q);
}

uint32_t 
skynet_mq_handle(struct message_queue *q) {
	return q->handle;
}

int
skynet_mq_length(struct message_queue *q) {
	int head, tail,cap;

	SPIN_LOCK(q)
	head = q->head;
	tail = q->tail;
	cap = q->cap;
	SPIN_UNLOCK(q)
	
	if (head <= tail) {
		return tail - head;
	}
	return tail + cap - head;
}

//skynet-server.c
//int overload = skynet_mq_overload(q);
//if (overload) {
//	skynet_error(ctx, "May overload, message queue length = %d", overload);
//}

//获取负载情况
int
skynet_mq_overload(struct message_queue *q) {
	if (q->overload) {
		int overload = q->overload;
		q->overload = 0; //这里清零是为了避免持续产生报警，在skynet-server.c中
		return overload;
	} 
	return 0;
}

int
skynet_mq_pop(struct message_queue *q, struct skynet_message *message) {
	int ret = 1;
	SPIN_LOCK(q)

	if (q->head != q->tail) // 数组不空的时候
	{
		*message = q->queue[q->head++]; // 从数组头位置取出第一个数据，然后头位置往后移动
		ret = 0;
		int head = q->head;
		int tail = q->tail;
		int cap = q->cap;

		if (head >= cap) { // 头位置移动到尾部之后，把头位置直接放到最开始的0位置处，下次从这个地方处理
			q->head = head = 0;
		}
		int length = tail - head;
		if (length < 0) { // 数据尾部在头部前面
			length += cap;
		}
		while (length > q->overload_threshold) {
			q->overload = length;
			q->overload_threshold *= 2;
		}
	} else {
		// reset overload_threshold when queue is empty
		q->overload_threshold = MQ_OVERLOAD;
	}

	// 数据数组为空的时候，把消息队列移出全局消息队列
	if (ret) {
		q->in_global = 0;
	}
	
	SPIN_UNLOCK(q)

	return ret;
}

// 扩大每个消息队列节点里面的数据数组
static void
expand_queue(struct message_queue *q) {
	// 把queue循环数组扩大cap两倍
	struct skynet_message *new_queue = skynet_malloc(sizeof(struct skynet_message) * q->cap * 2);

	// index = (q->head + i) % q->cap;
	// 当 cap = 4; head=2的时候；
	// i=0;index = 2,
	// i=1;index = 3,
	// i=2;index = 0,
	// i=3;index = 1,
	// 所以这个时候是可以从head这里循环cap个，因为%cap，所以index不会有大于cap的情况

	int i;
	for (i=0;i<q->cap;i++) {
		new_queue[i] = q->queue[(q->head + i) % q->cap]; // 从 head 处开始，循环赋值到new_queue
	}
	q->head = 0;		// 这个时候重新设置数据的头位置和数据的尾位置
	q->tail = q->cap;
	q->cap *= 2;        // 数据容量扩大两倍
	
	skynet_free(q->queue);
	q->queue = new_queue;
}

void 
skynet_mq_push(struct message_queue *q, struct skynet_message *message) {
	assert(message);
	SPIN_LOCK(q)

	q->queue[q->tail] = *message; //  直接把结构体赋值，之前在初始化的时候已经分配空间
	if (++ q->tail >= q->cap) {
		q->tail = 0;
		// 如果数据数组到达容量的最大值，则回到最初的开始，形成一个循环的数组不断的使用
	}

	if (q->head == q->tail) {
		// 队列满了 就扩大队列
		expand_queue(q);
	}

	if (q->in_global == 0) {
		q->in_global = MQ_IN_GLOBAL;
		skynet_globalmq_push(q); // 把消息加入消息队列，这个时候消息队列非空，则插入全局队列
	}
	
	SPIN_UNLOCK(q)
}

void 
skynet_mq_init() {
	struct global_queue *q = skynet_malloc(sizeof(*q));
	memset(q,0,sizeof(*q));
	SPIN_INIT(q);
	Q=q;
}

//服务释放标记
void 
skynet_mq_mark_release(struct message_queue *q) {
	SPIN_LOCK(q)
	assert(q->release == 0);
	q->release = 1;
	if (q->in_global != MQ_IN_GLOBAL) {
		skynet_globalmq_push(q);
	}
	SPIN_UNLOCK(q)
}

static void
_drop_queue(struct message_queue *q, message_drop drop_func, void *ud) {
	struct skynet_message msg;
	while(!skynet_mq_pop(q, &msg)) {
		drop_func(&msg, ud);
	}
	_release(q);
}

void 
skynet_mq_release(struct message_queue *q, message_drop drop_func, void *ud) {
	SPIN_LOCK(q)
	
	if (q->release) {
		SPIN_UNLOCK(q)
		_drop_queue(q, drop_func, ud);
	} else {
		skynet_globalmq_push(q);
		SPIN_UNLOCK(q)
	}
}

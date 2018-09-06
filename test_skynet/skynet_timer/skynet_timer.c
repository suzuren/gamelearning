//#include "skynet.h"

#include "skynet_timer.h"
//#include "skynet_mq.h"
//#include "skynet_server.h"
//#include "skynet_handle.h"
#include "spinlock.h"

#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdbool.h>

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#include <sys/time.h>
#include <mach/task.h>
#include <mach/mach.h>
#endif

//--------------------------------------------------------------------------

int
skynet_context_push(uint32_t handle, struct skynet_message *message)
{
	printf("skynet_context_push - handle:%u,session:%d,source:%d,sz:%ld,data:%p\n",
		handle, message->session, message->source, message->sz, message->data);

	//skynet_context_push - handle:16777217,session:5678,source:0,sz:72057594037927936,data:(nil)
	//														  2^56 = 72057594037927936
	// #define MESSAGE_TYPE_SHIFT ((sizeof(size_t)-1) * 8)
	printf("skynet_context_push - sizeof(size_t):%lu - >%lu,SIZE_MAX:%lu,MESSAGE_TYPE_MASK:%lu\n", sizeof(size_t), (sizeof(size_t) - 1) * 8, SIZE_MAX, MESSAGE_TYPE_MASK);
	//skynet_context_push - sizeof(size_t) : 8 - >56 ,SIZE_MAX:18446744073709551615,MESSAGE_TYPE_MASK:72057594037927935

	// 18446744073709551615 右移8位操作是把最后的8位去掉在前面补充8个0 如下
	// 1844 6744 0737 0955 1615
	// FFFF FFFF FFFF FFFF
	// 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 这个二进制进行右移8位操作
	// 0000 0000 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 -> FFFFFFFFFFFFFF -> 72057594037927935 == MESSAGE_TYPE_MASK
	

	// 八字节 64位
	// message->sz       = 72057594037927936 ->0000 0001 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	// MESSAGE_TYPE_MASK = 72057594037927935 ->0000 0000 11111111 11111111 11111111 11111111 11111111 11111111 11111111
	size_t sz = message->sz & MESSAGE_TYPE_MASK;
	printf("skynet_context_push - mask sz:%lu\n", sz);

	//struct skynet_context * ctx = skynet_handle_grab(handle);
	//if (ctx == NULL) {
	//	return -1;
	//}
	//skynet_mq_push(ctx->queue, message);
	//skynet_context_release(ctx);

	return 0;
}

struct skynet_context {
	void * instance;
	//struct skynet_module * mod;
	void * cb_ud;
	//skynet_cb cb;
	//struct message_queue *queue;
	//FILE * logfile;
	uint64_t cpu_cost;	// in microsec
	uint64_t cpu_start;	// in microsec
	char result[32];
	uint32_t handle;
	int session_id;
	int ref;
	int message_count;
	bool init;
	bool endless;
	bool profile;
};

void
skynet_error(struct skynet_context * context, const char *msg, ...) {
	printf("skynet_error - context:%p,msg:%s\n", context, msg);
}
//--------------------------------------------------------------------------

typedef void (*timer_execute_func)(void *ud,void *arg);


//每个计时器事件回调 
struct timer_event {
	uint32_t handle;
	int session;
};

struct timer_node {
	struct timer_node *next;
	uint32_t expire; // //计时器事件触发时间
};

struct link_list {
	struct timer_node head;
	struct timer_node *tail;
};

struct timer {
	struct link_list near[TIME_NEAR];	// 256 最近的时间
	struct link_list t[4][TIME_LEVEL];	// 64   根据时间久远分级
	struct spinlock lock;
	uint32_t time;						// 计时器，每百分之一秒更新一次 -> 服务器经过的tick 数， 每10毫秒 tick 一次，T->time 增加1；
	uint32_t starttime;					// 起始时间 秒
	uint64_t current;					// 当前时间gettime与current_point的时间差 单位为百分之一秒
	uint64_t current_point;				// 上一次update的时间， 百分之一秒
};

static struct timer * TI = NULL;

static inline struct timer_node *
link_clear(struct link_list *list) {
	struct timer_node * ret = list->head.next;
	list->head.next = 0;
	list->tail = &(list->head);

	return ret;
}

static inline void
link(struct link_list *list,struct timer_node *node) {
	list->tail->next = node;
	list->tail = node;
	node->next=0;
}

static void
add_node(struct timer *T,struct timer_node *node) {
	uint32_t time=node->expire;    // 加上了超时的时间 ，绝对时间
	uint32_t current_time=T->time; // 当前时间，绝对时间
	
//#define TIME_NEAR_MASK  255 ->  0000  0000   0000 0000   0000 0000   1111 1111

	// uint32_t 类型需要32位表示，把超时时间的高24位等于当前时间的高24位来判断是否是近的时间
	// near 这个数组大小正好是256大小，0-255表示，
	// time&TIME_NEAR_MASK 这个time是32位只能取出低八位，所以 time&TIME_NEAR_MASK 范围是 0 - 255
	// 所以这个 time&TIME_NEAR_MASK 这个数值做 near 数组的下标是永远都不会越界的
	// 相当于低八位不考虑，time - current_time <= 255算近的时
	if ((time|TIME_NEAR_MASK)==(current_time|TIME_NEAR_MASK))
	{
		link(&T->near[time&TIME_NEAR_MASK],node);
	}
	else
	{
//#define TIME_NEAR  256 ->  0000  0000   0000 0000   0000 0001   0000 0000
//#       mask     16384 ->  0000  0000   0000 0000   0100 0000   0000 0000
//#       mask-1   16383 ->  0000  0000   0000 0000   ‭0011 1111   1111 1111‬
		int i; 
		uint32_t mask=TIME_NEAR << TIME_LEVEL_SHIFT;
		for (i=0;i<3;i++)
		{
// i = 0    mask         ->  ‭0000  0000   0000 0000   0100 0000   0000 0000  - 1 -> 0000  0000   0000 0000   ‭0011 1111   1111 1111‬ -> 所以 i=0 存放超时时间 2^14-1 到 2^8 （因为 0 到 2^8-1 的时间已经给near近的时间数组了）
// i = 1    mask         ->  ‭0000  0000   0001 0000   0000 0000   0000 0000  - 1 -> 0000  0000   0000 1111   ‭1111 1111   1111 1111‬ -> 所以 i=1 存放超时时间 2^20-1 到 2^14
// i = 2    mask         ->  ‭0000  0100   0000 0000   0000 0000   0000 0000  - 1 -> 0000  0011   1111 1111   ‭1111 1111   1111 1111‬ -> 所以 i=2 存放超时时间 2^26-1 到 2^20
// i = 3    mask         ->  ‭0000  0100   0000 0000   0000 0000   0000 0000  - 1 -> 0000  0011   1111 1111   ‭1111 1111   1111 1111‬ -> 所以 i=3 存放超时时间 2^32-1 到 2^26
// 因为 i 为 3 的时候，不能进入循环，所以 mask 的值和 i 为 2 的值是一样的
// 所以当超时时间是和当前时间的高 18 位一样的时候可以进入 i = 0 的 t[0][x] 数组里面
// 所以当超时时间是和当前时间的高 12 位一样的时候可以进入 i = 1 的 t[1][x] 数组里面
// 所以当超时时间是和当前时间的高  6 位一样的时候可以进入 i = 2 的 t[2][x] 数组里面
// 剩下的都是直接进入到 i = 3 的数组里面
			if ((time|(mask-1))==(current_time|(mask-1)))
			{
				break;
			}
			mask <<= TIME_LEVEL_SHIFT;
		}
//  t[4][TIME_LEVEL];	// 64
//#define TIME_LEVEL_MASK 63  ->  0000  0000   0000 0000   0000 0000   0011 1111 这个保证数组不会越界

		link(&T->t[i][((time>>(TIME_NEAR_SHIFT + i*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);
	}
}

static void
timer_add(struct timer *T,void *arg,size_t sz,int time) {
	struct timer_node *node = (struct timer_node *)skynet_malloc(sizeof(*node)+sz);
	memcpy(node+1,arg,sz);

	SPIN_LOCK(T);

		node->expire=time+T->time;
		add_node(T,node);

	SPIN_UNLOCK(T);
}

static void
move_list(struct timer *T, int level, int idx)
{
	//直接把t数组level下标的链表直接拿出来
	//通过add_node(T, current); 这样子，
	//数组t -> level idx 下标下面的链表每一个节点都重新计算
	struct timer_node *current = link_clear(&T->t[level][idx]);
	while (current) {// 如果该数组元素的链表为空，则不移动
		struct timer_node *temp=current->next;
		add_node(T,current);
		current=temp;
	}
}

static void
timer_shift(struct timer *T) {
	int mask = TIME_NEAR; // 256
	uint32_t ct = ++T->time; // tick 一次
	if (ct == 0)
	{
		// 当前时间为0时，需要处理之前溢出情况下加入的计时器，也就是timer.t[3][0]中的计时器。
		// timer.t[3][i](i != 0)中的计时器会在之后跨level的时候处理
		move_list(T, 3, 0);
	}
	else
	{
		//                time = T->time
		//link(&T->t[i][((time >> (TIME_NEAR_SHIFT + i*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)], node);
		//            idx=time >> (8 + i*6) & 63

		// i=0 link(&T->t[i][((time >> TIME_NEAR_SHIFT) & TIME_LEVEL_MASK)], node); = link(&T->t[i][((time >> 8) & 63)]
		// i=0        idx=time >>   8 & 63
		// i=1        idx=time >>  (8 + 1*6) & 63
		// i=1        idx=time >>  (8 + 2*6) & 63
		// i=1        idx=time >>  (8 + 3*6) & 63
		uint32_t time = ct >> TIME_NEAR_SHIFT; // 8
		int i=0;
// i = 0  mask  ->  ‭0000  0000   0000 0000   0000 0001   0000 0000  - 1 -> 0000  0000   0000 0000   ‭0000 0000   1111 1111‬
// i = 1  mask  ->  ‭0000  0000   0000 0000   0100 0000   0000 0000  - 1 -> 0000  0000   0000 0000   ‭0011 1111   1111 1111‬
// i = 2  mask  ->  ‭0000  0000   0001 0000   0000 0000   0000 0000  - 1 -> 0000  0000   0000 1111   ‭1111 1111   1111 1111‬
// i = 3  mask  ->  ‭0000  0100   0000 0000   0000 0000   0000 0000  - 1 -> 0000  0011   1111 1111   ‭1111 1111   1111 1111‬
// i = 4  mask  ->  ‭0000  0000   0000 0000   0000 0000   0000 0000  - 1 -> 1111  1111   1111 1111   ‭1111 1111   1111 1111‬
		while ((ct & (mask-1))==0) // 走到尽头，就是mask在下面每次右移6位
		{
			int idx=time & TIME_LEVEL_MASK; // 63 -> ct >> 8 & 63
			if (idx!=0)
			{
				move_list(T, i, idx);
				break;				
			}
			mask <<= TIME_LEVEL_SHIFT; // 6
			time >>= TIME_LEVEL_SHIFT; // 6
			++i;
		}
	}
}

static inline void
dispatch_list(struct timer_node *current) {
	do {
		struct timer_event * event = (struct timer_event *)(current+1);
		struct skynet_message message;
		message.source = 0;
		message.session = event->session;
		message.data = NULL;
		message.sz = (size_t)PTYPE_RESPONSE << MESSAGE_TYPE_SHIFT;

		skynet_context_push(event->handle, &message);
		
		struct timer_node * temp = current;
		current=current->next;
		skynet_free(temp);	
	} while (current);
}

static inline void
timer_execute(struct timer *T) {
	int idx = T->time & TIME_NEAR_MASK; // 在add_node函数的时候把节点也是通过相与放入 link(&T->near[time&TIME_NEAR_MASK],node);
	
	while (T->near[idx].head.next) {
		struct timer_node *current = link_clear(&T->near[idx]);
		SPIN_UNLOCK(T);
		// dispatch_list don't need lock T
		dispatch_list(current);
		SPIN_LOCK(T);
	}
}

static void 
timer_update(struct timer *T) {
	SPIN_LOCK(T);

	// try to dispatch timeout 0 (rare condition)
	timer_execute(T);

	// shift time first, and then dispatch timer message
	timer_shift(T);

	timer_execute(T);

	SPIN_UNLOCK(T);
}

static struct timer *
timer_create_timer() {
	struct timer *r=(struct timer *)skynet_malloc(sizeof(struct timer));
	memset(r,0,sizeof(*r)); // 初始化0 ，time字段也是在这个时候初始化0，后面shift的时候++time;
	// 创建五个数组, 每个数组的每个 slot 表示一个时间段 同时又是个链表，用来存储
	// struct link_list near[TIME_NEAR];	// 256 最近的时间
	// struct link_list t[4][TIME_LEVEL];	// 64  根据时间久远分级


	int i,j;

	for (i=0;i<TIME_NEAR;i++) {
		link_clear(&r->near[i]);
	}

	for (i=0;i<4;i++) {
		for (j=0;j<TIME_LEVEL;j++) {
			link_clear(&r->t[i][j]);
		}
	}

	SPIN_INIT(r)

	r->current = 0;

	return r;
}

int
skynet_timeout(uint32_t handle, int time, int session) {
	if (time <= 0) {
		struct skynet_message message;
		message.source = 0;
		message.session = session;
		message.data = NULL;
		message.sz = (size_t)PTYPE_RESPONSE << MESSAGE_TYPE_SHIFT;

		if (skynet_context_push(handle, &message)) {
			return -1;
		}
	} else {
		struct timer_event event;
		event.handle = handle;
		event.session = session;
		timer_add(TI, &event, sizeof(event), time);
	}

	return session;
}

// centisecond: 1/100 second
static void
systime(uint32_t *sec, uint32_t *cs) {
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_REALTIME, &ti);		 // 系统实时时间,随系统实时时间改变而改变
	*sec = (uint32_t)ti.tv_sec;				 // struct timer -> starttime 赋值，时间精度 秒
	*cs = (uint32_t)(ti.tv_nsec / 10000000); // struct timer -> current 赋值，百分之一秒的精度
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*sec = tv.tv_sec;
	*cs = tv.tv_usec / 10000;
#endif
}

static uint64_t
gettime() {
	uint64_t t;
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_MONOTONIC, &ti); // 从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
	t = (uint64_t)ti.tv_sec * 100;
	t += ti.tv_nsec / 10000000;			 // 百分之一秒的精度 也就是10毫秒的精度
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint64_t)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;
#endif
	return t;
}

void
skynet_updatetime(void) {
	uint64_t cp = gettime();
	if(cp < TI->current_point) {
		skynet_error(NULL, "time diff error: change from %lld to %lld", cp, TI->current_point);
		TI->current_point = cp;
	} else if (cp != TI->current_point) {
		uint32_t diff = (uint32_t)(cp - TI->current_point);
		printf("skynet_updatetime - cp:%lu,current_point:%lu,diff:%u,current:%lu\n", cp, TI->current_point, diff, TI->current);
		// 这里基本可以保证diff 的值为1， 也就是10 毫秒的时间间隔 因为线程只睡眠了 usleep(2500);  2500微妙
		TI->current_point = cp;
		TI->current += diff;
		int i;
		for (i=0;i<diff;i++) {
			timer_update(TI);
		}
	}
}

uint32_t
skynet_starttime(void) {
	return TI->starttime;
}

// 目前这个接口弃用
uint64_t 
skynet_now(void) {
	return TI->current;
}

void 
skynet_timer_init(void) {
	TI = timer_create_timer();
	uint32_t current = 0;
	systime(&TI->starttime, &current);
	TI->current = current;
	TI->current_point = gettime();

	printf("skynet_timer_init - starttime:%u,current:%lu,current_point:%lu\n", TI->starttime, TI->current, TI->current_point);

}

// for profile

#define NANOSEC 1000000000 //1秒 = 1000 000 000 纳秒
#define MICROSEC 1000000  // 1秒 = 1000 000 微妙


uint64_t
skynet_thread_time(void) {
#if  !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ti); // 本线程到当前代码系统CPU花费的时间
	// 返回微妙								秒转化为微秒					纳秒转化为微秒，先出
	uint64_t micro_second = (uint64_t)ti.tv_sec * MICROSEC + (uint64_t)ti.tv_nsec / (NANOSEC / MICROSEC);

	//printf("skynet_thread_time - micro_second:%lu\n", micro_second);
	return micro_second;
#else
	struct task_thread_times_info aTaskInfo;
	mach_msg_type_number_t aTaskInfoCount = TASK_THREAD_TIMES_INFO_COUNT;
	if (KERN_SUCCESS != task_info(mach_task_self(), TASK_THREAD_TIMES_INFO, (task_info_t )&aTaskInfo, &aTaskInfoCount)) {
		return 0;
	}

	return (uint64_t)(aTaskInfo.user_time.seconds) + (uint64_t)aTaskInfo.user_time.microseconds;
#endif
}

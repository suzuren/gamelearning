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

	// 18446744073709551615 ����8λ�����ǰ�����8λȥ����ǰ�油��8��0 ����
	// 1844 6744 0737 0955 1615
	// FFFF FFFF FFFF FFFF
	// 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 ��������ƽ�������8λ����
	// 0000 0000 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 -> ?FFFFFFFFFFFFFF? -> ?72057594037927935? == MESSAGE_TYPE_MASK
	

	// ���ֽ� 64λ
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

#define TIME_NEAR_SHIFT 8
#define TIME_NEAR (1 << TIME_NEAR_SHIFT)	// 2^8 = 256 -> ?0001 0000 0000?
#define TIME_NEAR_MASK (TIME_NEAR-1)		//       255 -> 0000 ?1111 1111?

#define TIME_LEVEL_SHIFT 6					// 6
#define TIME_LEVEL (1 << TIME_LEVEL_SHIFT)	// 2^6 = 64  -> ?0100 0000?
#define TIME_LEVEL_MASK (TIME_LEVEL-1)		// 64-1= 63  -> ?0011 1111?

//ÿ����ʱ���¼��ص� 
struct timer_event {
	uint32_t handle;
	int session;
};

struct timer_node {
	struct timer_node *next;
	uint32_t expire; // //��ʱ���¼�����ʱ��
};

struct link_list {
	struct timer_node head;
	struct timer_node *tail;
};

struct timer {
	struct link_list near[TIME_NEAR];	// 256 �����ʱ��
	struct link_list t[4][TIME_LEVEL];	// 64   ����ʱ���Զ�ּ�
	struct spinlock lock;
	uint32_t time;						// ��ʱ����ÿ�ٷ�֮һ�����һ�� -> ������������tick ���� ÿ10���� tick һ�Σ�T->time ����1��
	uint32_t starttime;					// ��ʼʱ�� ��
	uint64_t current;					// ��ǰʱ��gettime��current_point��ʱ��� ��λΪ�ٷ�֮һ��
	uint64_t current_point;				// ��һ��update��ʱ�䣬 �ٷ�֮һ��
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
	uint32_t time=node->expire;
	uint32_t current_time=T->time;
	
	if ((time|TIME_NEAR_MASK)==(current_time|TIME_NEAR_MASK)) {
		link(&T->near[time&TIME_NEAR_MASK],node);
	} else {
		int i;
		uint32_t mask=TIME_NEAR << TIME_LEVEL_SHIFT;
		for (i=0;i<3;i++) {
			if ((time|(mask-1))==(current_time|(mask-1))) {
				break;
			}
			mask <<= TIME_LEVEL_SHIFT;
		}

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
move_list(struct timer *T, int level, int idx) {
	struct timer_node *current = link_clear(&T->t[level][idx]);
	while (current) {
		struct timer_node *temp=current->next;
		add_node(T,current);
		current=temp;
	}
}

static void
timer_shift(struct timer *T) {
	int mask = TIME_NEAR;
	uint32_t ct = ++T->time;
	if (ct == 0) {
		move_list(T, 3, 0);
	} else {
		uint32_t time = ct >> TIME_NEAR_SHIFT;
		int i=0;

		while ((ct & (mask-1))==0) {
			int idx=time & TIME_LEVEL_MASK;
			if (idx!=0) {
				move_list(T, i, idx);
				break;				
			}
			mask <<= TIME_LEVEL_SHIFT;
			time >>= TIME_LEVEL_SHIFT;
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
	int idx = T->time & TIME_NEAR_MASK;
	
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
	memset(r,0,sizeof(*r));
	// �����������, ÿ�������ÿ�� slot ��ʾһ��ʱ��� ͬʱ���Ǹ����������洢
	// struct link_list near[TIME_NEAR];	// 256 �����ʱ��
	// struct link_list t[4][TIME_LEVEL];	// 64   ����ʱ���Զ�ּ�


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
	clock_gettime(CLOCK_REALTIME, &ti);		 // ϵͳʵʱʱ��,��ϵͳʵʱʱ��ı���ı�
	*sec = (uint32_t)ti.tv_sec;				 // struct timer -> starttime ��ֵ��ʱ�侫�� ��
	*cs = (uint32_t)(ti.tv_nsec / 10000000); // struct timer -> current ��ֵ���ٷ�֮һ��ľ���
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
	clock_gettime(CLOCK_MONOTONIC, &ti); // ��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��
	t = (uint64_t)ti.tv_sec * 100;
	t += ti.tv_nsec / 10000000;			 // �ٷ�֮һ��ľ��� Ҳ����10����ľ���
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
		// ����������Ա�֤diff ��ֵΪ1�� Ҳ����10 �����ʱ���� ��Ϊ�߳�ֻ˯���� usleep(2500);  2500΢��
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

// Ŀǰ����ӿ�����
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

#define NANOSEC 1000000000 //1�� = 1000 000 000 ����
#define MICROSEC 1000000  // 1�� = 1000 000 ΢��

//struct timespec
//{
//time_t tv_sec; // seconds[��]
//long tv_nsec; // nanoseconds[����]
//};
//int clock_gettime(clockid_t clk_id, struct timespect *tp);
////@clk_id:
//CLOCK_REALTIME:ϵͳʵʱʱ��,��ϵͳʵʱʱ��ı���ı�
//CLOCK_MONOTONIC:��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��
//CLOCK_PROCESS_CPUTIME_ID:�����̵���ǰ����ϵͳCPU���ѵ�ʱ��
//CLOCK_THREAD_CPUTIME_ID:���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��

uint64_t
skynet_thread_time(void) {
#if  !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ti); // ���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��
	// ����΢��								��ת��Ϊ΢��					����ת��Ϊ΢�룬�ȳ�
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

#define LUA_LIB

//#include "skynet_malloc.h"
//
//#include "skynet_socket.h"

#define SKYNET_SOCKET_TYPE_DATA 1
#define SKYNET_SOCKET_TYPE_CONNECT 2
#define SKYNET_SOCKET_TYPE_CLOSE 3
#define SKYNET_SOCKET_TYPE_ACCEPT 4
#define SKYNET_SOCKET_TYPE_ERROR 5
#define SKYNET_SOCKET_TYPE_UDP 6
#define SKYNET_SOCKET_TYPE_WARNING 7

struct skynet_socket_message {
	int type;
	int id;
	int ud;
	char * buffer;
};

#include <lua.h>
#include <lauxlib.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define skynet_malloc malloc
#define skynet_free   free

#define QUEUESIZE 1024
#define HASHSIZE 4096
#define SMALLSTRING 2048

#define TYPE_DATA 1
#define TYPE_MORE 2
#define TYPE_ERROR 3
#define TYPE_OPEN 4
#define TYPE_CLOSE 5
#define TYPE_WARNING 6

/*
	Each package is uint16 + data , uint16 (serialized in big-endian) is the number of bytes comprising the data .
 */

struct netpack {
	int id;
	int size;
	void * buffer;
};

struct uncomplete {
	struct netpack pack;
	struct uncomplete * next;
	int read;
	int header;
};

struct queue {
	int cap;
	int head;
	int tail;
	struct uncomplete * hash[HASHSIZE];
	struct netpack queue[QUEUESIZE];
};

static void
clear_list(struct uncomplete * uc) {
	while (uc) {
		skynet_free(uc->pack.buffer);
		void * tmp = uc;
		uc = uc->next;
		skynet_free(tmp);
	}
}

static int
lclear(lua_State *L) {
	struct queue * q;// = lua_touserdata(L, 1);
	if (q == NULL) {
		return 0;
	}
	int i;
	for (i=0;i<HASHSIZE;i++) {
		clear_list(q->hash[i]);
		q->hash[i] = NULL;
	}
	if (q->head > q->tail) {
		q->tail += q->cap;
	}
	for (i=q->head;i<q->tail;i++)
	{
		struct netpack *np = &q->queue[i % q->cap]; // 因为是循环队列，所以需要模队列的容量，才能取正确的下标
		skynet_free(np->buffer);
	}
	q->head = q->tail = 0;

	return 0;
}

// HASHSIZE -> 4096
// oxfff ->4095 -> 24λ

// int [-2^31 , 2^31-1] -> [-2147483648，2147483647]
//       0x7fffffff = ‭2147483647‬ = ‭01111111 11111111 11111111 11111111‬
//                         >> 24 = ‭00000000 00000000 00000000 01111111 = 0x7F	 = 127		取高 8  位
//                         >> 12 = 00000000 0000‭‭0111 11111111 11111111 = 0x‭7FFFF‬ = ‭524287‬	取高 20 位

static inline int
hash_fd(int fd) {
	int a = fd >> 24;
	int b = fd >> 12;
	int c = fd;

	printf("func hash_fd - a:%d,b:%d,c:%d\n", a, b, c);

	return (int)(((uint32_t)(a + b + c)) % HASHSIZE);
}

static struct uncomplete *
find_uncomplete(struct queue *q, int fd) {
	if (q == NULL)
		return NULL;
	int h = hash_fd(fd); // 取出这个 fd 的 哈希值
	struct uncomplete * uc = q->hash[h];
	if (uc == NULL)
		return NULL;
	if (uc->pack.id == fd) // 如果这个节点的 fd 就是是要找的 fd， 则返回这个节点地址，
							// 否则计入下面循环，遍历这个链表找到这个fd的节点进行返回
	{
		q->hash[h] = uc->next; // 把这个不完整的数据包节点取下来，返回该节点的地址
		return uc;
	}
	struct uncomplete * last = uc;
	while (last->next) // 如果还有下一个节点 继续找，是否存在有未接收完成数据的 fd
	{
		uc = last->next;
		if (uc->pack.id == fd) // 找到该 fd
		{
			last->next = uc->next; // 把存有不完整数据的该 fd 节点取下来
			return uc; // 返回该节点的地址
		}
		last = uc; // 查找下一个节点
	}
	return NULL;
}

static struct queue *
get_queue(lua_State *L) {
	struct queue *q; // = lua_touserdata(L, 1);
	if (q == NULL) {
		//q = lua_newuserdata(L, sizeof(struct queue));
		q->cap = QUEUESIZE;
		q->head = 0;
		q->tail = 0;
		int i;
		for (i=0;i<HASHSIZE;i++) {
			q->hash[i] = NULL;
		}
		//lua_replace(L, 1); // 把栈顶元素放置到给定位置而不移动其它元素 
							// （因此覆盖了那个位置处的值），然后将栈顶元素弹出。
	}
	return q;
}

static void
expand_queue(lua_State *L, struct queue *q) {
	struct queue *nq; // = lua_newuserdata(L, sizeof(struct queue) + q->cap * sizeof(struct netpack));
	nq->cap = q->cap + QUEUESIZE; // 按照 1024 的数量增加存网络数据包的队列
	nq->head = 0;	// 刚开始的时候，头部从 0 下标开始
	nq->tail = q->cap; // 尾部在 cap 容量最大处，因为只有在 cap 容量存完之后，才会开始扩增队列
	memcpy(nq->hash, q->hash, sizeof(nq->hash)); // 把前一个已经满的队列的 hash 数组拷贝到新创建的队列里面
	memset(q->hash, 0, sizeof(q->hash)); //全部清空数值，在这里不能释放资源，因为在新创建的队列里面还需要这些内存资源
	int i;
	for (i=0;i<q->cap;i++)
	{
		int idx = (q->head + i) % q->cap; // 因为是循环队列，所以需要 模 队列的容量，才能取到正确的下标
		nq->queue[i] = q->queue[idx]; // 按照之前的规则 全部重新放进新的队列里面
	}
	q->head = q->tail = 0;
	//lua_replace(L,1);
}

static void
push_data(lua_State *L, int fd, void *buffer, int size, int clone) {
	if (clone) {
		void * tmp = skynet_malloc(size);
		memcpy(tmp, buffer, size);
		buffer = tmp;
	}
	struct queue *q = get_queue(L);
	struct netpack *np = &q->queue[q->tail]; // 取下尾部指向的元素地址，此时这个元素还没存网络数据包
	if (++q->tail >= q->cap) // ++tail 重新指向下一个没有存数据的元素，如果 tail > cap 容量，则循环存储
		q->tail -= q->cap;
	np->id = fd;
	np->buffer = buffer;
	np->size = size;
	if (q->head == q->tail) // 存放数据的循环队列满了
	{
		expand_queue(L, q); // 加大容量
	}
}

static struct uncomplete *
save_uncomplete(lua_State *L, int fd) {
	struct queue *q = get_queue(L);
	int h = hash_fd(fd);
	struct uncomplete * uc = skynet_malloc(sizeof(struct uncomplete));
	memset(uc, 0, sizeof(*uc));
	uc->next = q->hash[h];
	uc->pack.id = fd;
	q->hash[h] = uc;

	return uc;
}

static inline int
read_size(uint8_t * buffer) {
	int r = (int)buffer[0] << 8 | (int)buffer[1];
	return r;
}

static void
push_more(lua_State *L, int fd, uint8_t *buffer, int size) {
	if (size == 1) {
		struct uncomplete * uc = save_uncomplete(L, fd);
		uc->read = -1;
		uc->header = *buffer; // 先保存进 int 整型，接下来在 filter_data_ 函数里有新的数据进来的时候直接左移八位就可以
		return;
	}
	int pack_size = read_size(buffer);
	buffer += 2;
	size -= 2;

	if (size < pack_size) {
		struct uncomplete * uc = save_uncomplete(L, fd);
		uc->read = size;
		uc->pack.size = pack_size;
		uc->pack.buffer = skynet_malloc(pack_size);
		memcpy(uc->pack.buffer, buffer, size);
		return;
	}
	push_data(L, fd, buffer, pack_size, 1);

	buffer += pack_size;
	size -= pack_size;
	if (size > 0) {
		push_more(L, fd, buffer, size);
	}
}

static void
close_uncomplete(lua_State *L, int fd) {
	struct queue *q; // = lua_touserdata(L, 1);
	struct uncomplete * uc = find_uncomplete(q, fd);
	if (uc) {
		skynet_free(uc->pack.buffer);
		skynet_free(uc);
	}
}

static int
filter_data_(lua_State *L, int fd, uint8_t * buffer, int size) {
	struct queue *q; // = lua_touserdata(L, 1);
	struct uncomplete * uc = find_uncomplete(q, fd);
	if (uc) // 这个 fd 之前有接收过部分数据
	{
		// fill uncomplete
		if (uc->read < 0) // 之前只接收过一个字节的数据
		{
			// read size
			assert(uc->read == -1);
			int pack_size = *buffer;
			pack_size |= uc->header << 8 ; // push_more 函数里面 已经把这个字节的数值赋值给 header
			++buffer;
			--size;
			uc->pack.size = pack_size;
			uc->pack.buffer = skynet_malloc(pack_size);
			uc->read = 0;
		}
		int need = uc->pack.size - uc->read;
		if (size < need) // 加上这个包还是没有完整
		{
			memcpy(uc->pack.buffer + uc->read, buffer, size);
			uc->read += size;
			int h = hash_fd(fd);
			uc->next = q->hash[h];
			q->hash[h] = uc; // 把这个没接收完整数据包的节点重新挂接到链表里面
			return 1;
		}
		memcpy(uc->pack.buffer + uc->read, buffer, need); // 可以把这个包接收完整 拷贝进这个不完整的节点让他变成完整的节点
		buffer += need;
		size -= need;
		if (size == 0)
		{
			//lua_pushvalue(L, lua_upvalueindex(TYPE_DATA)); // 把栈上给定索引处的元素作一个副本压栈。
			//lua_pushinteger(L, fd);
			//lua_pushlightuserdata(L, uc->pack.buffer); // 把一个轻量用户数据压栈。
														//用户数据是保留在 Lua 中的 C 值。 轻量用户数据 表示一个指针 void*。
														//它是一个像数字一样的值： 你不需要专门创建它，它也没有独立的元表，
														//而且也不会被收集（因为从来不需要创建）。 只要表示的 C 地址相同，
														//两个轻量用户数据就相等。
			//lua_pushinteger(L, uc->pack.size);
			skynet_free(uc);
			return 5;
		}
		// more data
		push_data(L, fd, uc->pack.buffer, uc->pack.size, 0);
		skynet_free(uc);
		push_more(L, fd, buffer, size);
		//lua_pushvalue(L, lua_upvalueindex(TYPE_MORE));
		return 2;
	}
	else
	{
		if (size == 1)
		{
			struct uncomplete * uc = save_uncomplete(L, fd);
			uc->read = -1;
			uc->header = *buffer;
			return 1;
		}
		int pack_size = read_size(buffer);
		buffer+=2;
		size-=2;

		if (size < pack_size)
		{
			struct uncomplete * uc = save_uncomplete(L, fd);
			uc->read = size;
			uc->pack.size = pack_size;
			uc->pack.buffer = skynet_malloc(pack_size);
			memcpy(uc->pack.buffer, buffer, size);
			return 1;
		}
		if (size == pack_size)
		{
			// just one package
			//lua_pushvalue(L, lua_upvalueindex(TYPE_DATA));
			//lua_pushinteger(L, fd);
			void * result = skynet_malloc(pack_size);
			memcpy(result, buffer, size);
			//lua_pushlightuserdata(L, result);
			//lua_pushinteger(L, size);
			return 5;
		}
		// more data
		push_data(L, fd, buffer, pack_size, 1);
		buffer += pack_size;
		size -= pack_size;
		push_more(L, fd, buffer, size);
		//lua_pushvalue(L, lua_upvalueindex(TYPE_MORE));
		return 2;
	}
}

static inline int
filter_data(lua_State *L, int fd, uint8_t * buffer, int size) {
	int ret = filter_data_(L, fd, buffer, size);
	// buffer is the data of socket message, it malloc at socket_server.c : function forward_message .
	// it should be free before return,
	skynet_free(buffer);
	return ret;
}


static inline void
write_size(uint8_t * buffer, int len) {
	buffer[0] = (len >> 8) & 0xff;
	buffer[1] = len & 0xff;
}

int main()
{
	printf("hello world!\n");


	int fds[] = { 1,2,3, 4096 };

	for (int i = 0; i < sizeof(fds) / sizeof(fds[0]); i++)
	{
		int h = hash_fd(fds[i]);
		printf("func main - i:%d,fds:%d,h:%d\n\n", i, fds[i], h);
	}
	printf("\n");

	return 1;
}
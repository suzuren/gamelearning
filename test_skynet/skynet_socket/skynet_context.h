#ifndef SKYNET_CONTEXT_H
#define SKYNET_CONTEXT_H

#include <stdint.h>


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


// handle do not define 0
#define HANDLE_TIME		1
#define HANDLE_SOCKET	2

struct skynet_message;
struct message_queue;

void skynet_context_init();
int skynet_context_push(uint32_t handle, struct skynet_message *message);
struct message_queue * skynet_context_message_dispatch(struct message_queue *q);


#endif

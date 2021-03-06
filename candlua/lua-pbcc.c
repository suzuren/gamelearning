//#include "skynet_malloc.h"


#include <lua.h>
#include <lauxlib.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// prt does not contain the 2 bytes big-endian network packet header
static int l_unpackNetPayload(lua_State *L) {
	char *ptr;
	int size;
	//int test = 0;
	
	//printf("l_unpackNetPayload - lua_type(L,1):%d,LUA_TSTRING:%d\n", lua_type(L, 1), LUA_TSTRING);

	if (lua_type(L,1) == LUA_TSTRING) {
		//test=1;
		ptr = (char *)luaL_checklstring(L,1,(size_t *)&size);
	} else {
		//test=2;
		ptr = (char *)lua_touserdata(L, 1);
		size = luaL_checkinteger(L, 2);
	}

	
	//printf("l_unpackNetPayload - ptr:%p - %s,size:%d\n", ptr, ptr,size);

	if( size < 3 )
	{
		return luaL_error(L, "parse protobuf packet failed, invalid format");
	}
	//int i=1;
	int k = 0;
	//printf("l_unpackNetPayload 1 - size:%d,pbNo:%d - 0x%06X,ptr:%d %d %d %d %d %d\n", size, pbNo, pbNo, ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);

	
	//int pbNo = ( (unsigned char)ptr[1] << 16 ) | ( (unsigned char)ptr[2] << 8 ) | (unsigned char)ptr[3];
	
	int pbNo = ( (unsigned char)ptr[3] << 16 ) | ( (unsigned char)ptr[4] << 8 ) | (unsigned char)ptr[5];

	//printf("l_unpackNetPayload 2 - size:%d,pbNo:%d - 0x%06X,ptr:%d %d %d %d %d %d\n", size, pbNo, pbNo, ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);


	//size += 4;			// actual network payload size
	//buffer[0] = (size >> 8) & 0xff;		// size = 6
	//buffer[1] = size & 0xff;
	//buffer[2] = 0;
	//buffer[3] = (protoNo >> 16) & 0xff;
	//buffer[4] = (protoNo >> 8) & 0xff;	// 2^8 = 256 and 0x000100 = 256
	//buffer[5] = protoNo & 0xff;

	//lua_pushlstring(L, buffer, size + 2);
	//lua_pushinteger(L, size + 2);
	//free(buffer);
	
	lua_pushinteger(
		L, pbNo
	);
	
	//lua_pushlstring(L, ptr + 4, size - 4);
	lua_pushlstring(L, ptr + 6, size - 6);
	lua_pushinteger(L,k);
	lua_pushinteger(L,(unsigned char)ptr[0]);

	//printf("l_unpackNetPayload - size:%d,k:%d,(unsigned char)ptr[0]:%d\n", size-4,k, (unsigned char)ptr[0]);

	return 4;
}

/**
 * return network packet in buffer for socket.write.
 * @param uint8_t* ptr
 * @param int size
 * @param int protoNo
 */

// local msg, size = protobuf.encode(typeName, obj, pbcc.packNetPacket, protoNo, returnString)

static int l_packNetPBPacket(lua_State *L) 
{

	if(!lua_islightuserdata(L, 1))
	{
		return luaL_error(L, "expect lightuserdata got %s", luaL_typename(L, 1));
	}
	uint8_t *ptr = (uint8_t*)lua_touserdata(L, 1);
	
	int size = luaL_checkinteger(L, 2);
	if(size >= 0x10000 )  // 0x10000 = 65536
	{
		return luaL_error(L, "message length(%d) exceed the upper limit", size);
	}
	
	lua_Unsigned protoNo = (lua_Unsigned)luaL_checkinteger(L, 3);
	int test = lua_toboolean(L, 4);

	//printf("l_packNetPBPacket - size:%d, protoNo:0x%06llX,test:%d\n", size, protoNo, test);
	//							size:2, protoNo:0x000100,test:1
	//uint8_t *buffer = malloc(size+6);
	char *buffer = malloc(size + 6);
	if (size > 0)
	{
		memcpy(buffer+6, ptr, size);
	}
	
	size += 4;			// actual network payload size
	buffer[0] = (size >> 8) & 0xff;		// size = 6
	buffer[1] = size & 0xff;
	buffer[2] = 0;
	buffer[3] = (protoNo >> 16) & 0xff;
	buffer[4] = (protoNo >> 8) & 0xff;	// 2^8 = 256 and 0x000100 = 256
	buffer[5] = protoNo & 0xff;

	//printf("l_packNetPBPacket - buffer:%d %d %d %d %d %d\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] );
	//							buffer:0 6 0 0 1 0
	int i=0;
	int k = 0;
	for(i=3;i<size+2;i++)
	{
		k+=((unsigned char)buffer[i]);
		k&=0xff;
	}
	//printf("l_packNetPBPacket - k:%d\n", k);

	if(!test)
	{
		lua_pushlightuserdata(L, buffer);
		lua_pushinteger(L, size+2);
	}
	else
	{
		lua_pushlstring(L, buffer, size + 2);
		lua_pushinteger(L, size + 2);
		free(buffer);
	}
	return 2;
}

int luaopen_pbcc(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "unpackNetPayload", l_unpackNetPayload },
		{ "packNetPacket", l_packNetPBPacket },
		{ NULL, NULL },
	};
	luaL_newlib(L,l);

	return 1;
}


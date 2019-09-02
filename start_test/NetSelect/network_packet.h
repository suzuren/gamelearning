#ifndef _NETWORK_PACKET_H_
#define _NETWORK_PACKET_H_

#pragma  pack(1)

// ---------------------------------------------------------------------------------------------

typedef struct client_header_t {
	unsigned short     type;      	// 服务器类型（要转发到哪个服务器类型）
	unsigned short     cmd;      	// 消息id
	unsigned short     len;			// 消息数据长度(不包括包头)	
}client_header;

#define CLIENT_PACKET_MAX_SIZE             (4096*4)
#define CLIENT_PACKET_HEADER_SIZE          (sizeof(client_header))
#define CLIENT_PACKET_MAX_DATA_SIZE		   (CLIENT_PACKET_MAX_SIZE - CLIENT_PACKET_HEADER_SIZE)

typedef struct clinet_packet_t {
	client_header		header;
	unsigned char       protobuf[CLIENT_PACKET_MAX_DATA_SIZE];
}clinet_packet;

// ---------------------------------------------------------------------------------------------

typedef struct server_header_t {
	unsigned short     type;      	// 服务器类型（从哪个服务器类型转发过来的）
	unsigned short     cmd;      	// 消息id
	unsigned short     len;			// 消息数据长度(不包括包头)	
	unsigned int       uin;         // uin(服务器内部转发用) 
	int			       cid;			// 客户端过来的数据把他的fd -> id带上，这样才不需要解析pb数据就能转发回去
}server_header;

#define SERVER_PACKET_MAX_SIZE             (4096*4)
#define SERVER_PACKET_HEADER_SIZE          (sizeof(server_header))
#define SERVER_PACKET_MAX_DATA_SIZE		   (SERVER_PACKET_MAX_SIZE - SERVER_PACKET_HEADER_SIZE)

typedef struct server_packet_t {
	server_header		header;
	unsigned char       protobuf[SERVER_PACKET_MAX_DATA_SIZE];
}server_packet;

// ---------------------------------------------------------------------------------------------

#define INCOMPLETE_PACKET_MAX_SIZE         (4096*4)

typedef struct incomplete_packet_t {
	int					len;
	unsigned char       buf[INCOMPLETE_PACKET_MAX_SIZE];
}incomplete_packet;

// ---------------------------------------------------------------------------------------------

#pragma pack()

#endif // _NETWORK_PACKET_H_

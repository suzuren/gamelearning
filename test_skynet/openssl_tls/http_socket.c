
#include "skynet_mq.h"
#include "support_algorithm.h"
#include "socket_server.h"
#include "http_module.h"
#include "libtls.h"
#include "http_socket.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

static struct socket_server * SOCKET_SERVER = NULL;


uint64_t
getsystime() {
	struct timespec ti;
	clock_gettime(CLOCK_REALTIME, &ti);
	//uint64_t sec = (uint64_t)ti.tv_sec;
	uint64_t cs = (uint64_t)(ti.tv_nsec / 10000000);
	return cs;
}

void
http_socket_init() {
	if (SOCKET_SERVER == NULL)
	{
		SOCKET_SERVER = socket_server_create(getsystime());
	}
}

void
http_socket_exit() {
	socket_server_exit(SOCKET_SERVER);
}

void
http_socket_free() {
	socket_server_release(SOCKET_SERVER);
	SOCKET_SERVER = NULL;
}

// mainloop thread
static void
forward_message(int type, bool padding, struct socket_message * result) {
	struct skynet_socket_message *sm;
	size_t sz = sizeof(*sm);
	if (padding) {
		if (result->data) {
			size_t msg_sz = strlen(result->data);
			if (msg_sz > 128) {
				msg_sz = 128;
			}
			sz += msg_sz;
		}
		else {
			result->data = "";
		}
	}
	sm = (struct skynet_socket_message *)skynet_malloc(sz);
	sm->type = type;
	sm->id = result->id;
	sm->ud = result->ud;
	if (padding) {
		sm->buffer = NULL;
		memcpy(sm + 1, result->data, sz - sizeof(*sm));
	}
	else {
		sm->buffer = result->data;
	}

	struct skynet_message message;
	message.source = 0;
	message.session = 0;
	message.data = sm;
	// type 编码到了 size 参数的高 8 位, 单个消息包限制长度在 16 M （24 bit)内
	message.sz = sz | ((size_t)PTYPE_SOCKET << MESSAGE_TYPE_SHIFT);

	//if (skynet_context_push((uint32_t)result->opaque, &message)) {
	if (http_mq_push(&message)) {
		// todo: report somewhere to close socket
		// don't call skynet_socket_close here (It will block mainloop)
		skynet_free(sm->buffer);
		skynet_free(sm);
	}
}

int
http_socket_poll() {
	struct socket_server *ss = SOCKET_SERVER;
	assert(ss);
	struct socket_message result;
	int more = 1;
	int type = socket_server_poll(ss, &result, &more);
	switch (type)
	{
	case SOCKET_EXIT:
	{
		printf("exit(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
		return 0;
	}break;
	case SOCKET_DATA:
	{
		printf("message(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
		//skynet_free(result.data);
		forward_message(SKYNET_SOCKET_TYPE_DATA, false, &result);
	}break;
	case SOCKET_CLOSE:
	{
		printf("close(%lu) [id=%d]\n", result.opaque, result.id);
	}break;
	case SOCKET_OPEN:
	{
		printf("open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
	}break;
	case SOCKET_ERR:
	{
		printf("error(%lu) [id=%d]\n", result.opaque, result.id);
	}break;
	case SOCKET_ACCEPT:
	{
		printf("accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);
	}break;
	default:
	{
		printf("Unknown socket message type %d.", type);
		return -1;
	}break;
	}
	if (more) {
		return -1;
	}

	//switch (type) {
	//case SOCKET_EXIT:
	//	return 0;
	//case SOCKET_DATA:
	//	//forward_message(SKYNET_SOCKET_TYPE_DATA, false, &result);
	//	break;
	//case SOCKET_CLOSE:
	//	//forward_message(SKYNET_SOCKET_TYPE_CLOSE, false, &result);
	//	break;
	//case SOCKET_OPEN:
	//	//forward_message(SKYNET_SOCKET_TYPE_CONNECT, true, &result);
	//	break;
	//case SOCKET_ERR:
	//	//forward_message(SKYNET_SOCKET_TYPE_ERROR, true, &result);
	//	break;
	//case SOCKET_ACCEPT:
	//	//forward_message(SKYNET_SOCKET_TYPE_ACCEPT, true, &result);
	//	break;
	//case SOCKET_UDP:
	//	//forward_message(SKYNET_SOCKET_TYPE_UDP, false, &result);
	//	break;
	//case SOCKET_WARNING:
	//	//forward_message(SKYNET_SOCKET_TYPE_WARNING, false, &result);
	//	break;
	//default:
	//	//skynet_error(NULL, "Unknown socket message type %d.", type);
	//	return -1;
	//}
	//if (more) {
	//	return -1;
	//}
	return 1;
}

void
http_socket_updatetime() {
	socket_server_updatetime(SOCKET_SERVER, getsystime());
}

int
http_socket_connect(const char *host, int port) {
	uint32_t source = 100;
	return socket_server_connect(SOCKET_SERVER, source, host, port);
}

int
http_socket_send(int id, void *buffer, int sz) {
	return socket_server_send(SOCKET_SERVER, id, buffer, sz);
}

int
http_socket_send_lowpriority(int id, void *buffer, int sz) {
	return socket_server_send_lowpriority(SOCKET_SERVER, id, buffer, sz);
}





static char * socket_hosttoip(const char * phost)
{
	static char buffer[32];
	memset(buffer, 0, sizeof(buffer));
	if (phost == NULL)
	{
		return NULL;
	}
	struct hostent *phostent = gethostbyname(phost);
	if (phostent == NULL)
	{
		return NULL;
	}
	switch (phostent->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
	{
		const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer, sizeof(buffer));
		if (ret_inet != NULL)
		{
			return buffer;
		}
	}break;
	default:
	{
		//
	}
	break;
	}
	return NULL;
}

int http_check_protocol(char* host,char** protocol,char** domain_name, char** ip,int* port)
{
	*protocol = "";
	*domain_name = "";
	*port = 0;
	char * http_str = "http://";
	int http_des_len = (int)strlen(http_str);
	int http_next[http_des_len];
	int http_nextval[http_des_len];
	kmp_get_next(http_str, http_next);
	kmp_get_nextval(http_str, http_nextval);
	int http_pos = kmp_index(host, http_str, 0, http_nextval);
	if (http_pos != -1)
	{
		*protocol = "http";
		*domain_name = (host + http_pos) + http_des_len;
		*port = 80;
	}
	else
	{
		char * https_str = "https://";
		int https_des_len = (int)strlen(https_str);
		int https_next[https_des_len];
		int https_nextval[https_des_len];
		kmp_get_next(https_str, https_next);
		kmp_get_nextval(https_str, https_nextval);
		int https_pos = kmp_index(host, https_str, 0, https_nextval);
		if (https_pos != -1)
		{
			*protocol = "https";
			*domain_name = (host + https_pos) + https_des_len;
			*port = 443;
		}
		else
		{
			return 0;
		}
	}
	char * pdomain = *domain_name;
	unsigned int domain_len = strlen(pdomain);
	if (domain_len == 0)
	{
		return 0;
	}
	bool is_ip_port = true;
	unsigned int index = 0;
	unsigned int last_colon_index = 0;
	static char buffer_ip[32] = { 0 };
	char buffer_port[16] = { 0 };
	for (; index < domain_len; index++)
	{
		char ch = (*(pdomain + index));
		if (isdigit(ch) == 0 && ch != '.' && ch != ':')
		{
			is_ip_port = false;
			break;
		}
		if (ch == ':')
		{
			last_colon_index = index;
		}
	}
	if (is_ip_port && last_colon_index != 0)
	{
		for (unsigned int idx = 0; idx < last_colon_index && idx < 32; idx++)
		{
			char ch = (*(pdomain + idx));
			//printf("idx:%d,%c\n", idx, ch);
			buffer_ip[idx] = ch;
		}
		int port_len = 0;
		for (unsigned int idx = last_colon_index + 1; idx < domain_len && port_len < 16; idx++)
		{
			char ch = (*(pdomain + idx));
			//printf("port - idx:%d,%c\n", idx, ch);
			buffer_port[port_len++] = ch;
		}
		//printf("domain_len:%d, colon_index:%d, pdomain:%s, ip:%s, port:%s\n", domain_len, last_colon_index, pdomain, buffer_ip, buffer_port);
		*ip = buffer_ip;
		int port_ret = atoi_parser(port, buffer_port, port_len);
		if (!port_ret)
		{
			return 0;
		}		
	}
	else
	{
		*ip = socket_hosttoip((const char *)*domain_name);
		if (*ip == NULL)
		{
			return 0;
		}
	}
	return 1;
}

static struct ssl_ctx* CTX_P = NULL;
static struct tls_context* TLS_P = NULL;

size_t http_read_func(char** out_read)
{
	sleep(2);
	*out_read = NULL;
	struct skynet_message msg;
	int ret = http_mq_pop(&msg);
	int type = msg.sz >> MESSAGE_TYPE_SHIFT;
	//size_t sz = msg.sz & MESSAGE_TYPE_MASK;
	size_t readlen = 0;
	//if(ret == 0)
	//{
	//	printf("ret:%d,type:%d,sz:%ld\n", ret, type, sz);
	//}
	if (!ret && type == PTYPE_SOCKET)
	{
		struct skynet_socket_message * sm = msg.data;
		if (sm->type == SKYNET_SOCKET_TYPE_DATA)
		{
			readlen = sm->ud;
			*out_read = sm->buffer;
			skynet_free(sm);
		}
	}
	return readlen;
}

int http_init_interface(int fd,char* protocol)
{
	if (strcmp(protocol, "http") == 0)
	{
		//
	}
	else if (strcmp(protocol, "https") == 0)
	{
		//

	}
	else {
		fprintf(stderr, "invalid protocol:%s e.g[http, https]", protocol);
		return 0;
	}

	if (CTX_P == NULL)
	{
		CTX_P = new_ctx();
	}
	if (TLS_P == NULL)
	{
		TLS_P = new_tls("client", CTX_P);
	}
	char* out_read;
	int all_read = tls_context_handshake(TLS_P, 0, NULL, &out_read);
	http_socket_send(fd, out_read, all_read);
	//printf("all_read:%d\n", all_read);
	//printf("out_read:%s-\n", out_read);
	//http_socket_send发送之后会自动释放
	//if (out_read != NULL)
	//{
	//	skynet_free(out_read);
	//}
	while (!tls_context_finished(TLS_P))
	{
		char* exchange;
		size_t slen = http_read_func(&exchange);
		//if (slen > 0)
		//{
		//	printf("slen:%ld\n", slen);
		//	printf("exchange:%s-\n", exchange);
		//}
		all_read = tls_context_handshake(TLS_P, slen, exchange, &out_read);
		if (exchange != NULL)
		{
			skynet_free(exchange);
		}
		if (all_read>0)
		{
			http_socket_send(fd, out_read, all_read);
			//http_socket_send发送之后会自动释放
			//if (out_read != NULL)
			//{
			//	skynet_free(out_read);
			//}
		}
	}
	printf("tls_context_finished\n");

	return 1;
}

void http_socket_tls_write(int fd, void *buffer, int sz)
{
	char* out_read;
	int read_size = tls_context_write(TLS_P, sz, buffer, &out_read);
	//http_socket_send发送之后会自动释放
	http_socket_send(fd, out_read, read_size);
	//if (out_read != NULL)
	//{
	//	skynet_free(out_read);
	//}
}

int http_socket_tls_read(int fd, char ** recvbuffer)
{
	char* encrypted_data;
	size_t encrypted_data_size = http_read_func(&encrypted_data);
	char* out_read;
	int read_size = tls_context_read(TLS_P, encrypted_data_size, encrypted_data, &out_read);
	*recvbuffer = out_read;
	return read_size;
}

int http_socket_request(int fd, char* method, char* host, char* uri, char* recvheader, char* header, char* content)
{
	char header_content[1024] = { 0 };
	sprintf(header_content, "host:%s\r\n", host);
	char request_header[2048] = { 0 };
	sprintf(request_header, "%s %s HTTP/1.1\r\n%scontent-length:0\r\n\r\n", method, uri, header_content);
	int header_size = (int)strlen(request_header);
	http_socket_tls_write(fd, request_header, header_size);
	printf("header_size:%d\n", header_size);
	printf("request_header:%s-\n", request_header);

	char * recvbuffer;
	int read_size = http_socket_tls_read(fd, &recvbuffer);
	printf("read_size:%d\n", read_size);
	printf("recvbuffer:%s-\n", recvbuffer);
	if (recvbuffer != NULL)
	{
		skynet_free(recvbuffer);
	}

	return 1;
}

//local function request(interface, method, host, url, recvheader, header, content)
//local read = interface.read
//local write = interface.write
//local header_content = ""
//if header then
//if not header.host then
//header.host = host
//end
//for k, v in pairs(header) do
//header_content = string.format("%s%s:%s\r\n", header_content, k, v)
//end
//else
//header_content = string.format("host:%s\r\n", host)
//end
//
//if content then
//local data = string.format("%s %s HTTP/1.1\r\n%scontent-length:%d\r\n\r\n", method, url, header_content, #content)
//write(data)
//write(content)
//else
//local request_header = string.format("%s %s HTTP/1.1\r\n%scontent-length:0\r\n\r\n", method, url, header_content)
//write(request_header)
//end
//
//local tmpline = {}
//local body = internal.recvheader(read, tmpline, "")
//if not body then
//error(socket.socket_error)
//end
//
//local statusline = tmpline[1]
//local code, info = statusline:match "HTTP/[%d%.]+%s+([%d]+)%s+(.*)$"
//code = assert(tonumber(code))
//
//local header = internal.parseheader(tmpline, 2, recvheader or {})
//if not header then
//error("Invalid HTTP response header")
//end
//
//local length = header["content-length"]
//if length then
//length = tonumber(length)
//end
//local mode = header["transfer-encoding"]
//if mode then
//if mode ~= "identity" and mode ~= "chunked" then
//error("Unsupport transfer-encoding")
//end
//end
//
//if mode == "chunked" then
//body, header = internal.recvchunkedbody(read, nil, header, body)
//if not body then
//error("Invalid response body")
//end
//else
//--identity mode
//if length then
//if #body >= length then
//body = body:sub(1, length)
//else
//local padding = read(length - #body)
//body = body ..padding
//end
//else
//--no content - length, read all
//body = body .. interface.readall()
//end
//end
//
//return code, body
//end


static void *
_socket_server_poll(void * ud) {
	struct socket_server *ss = ud;
	struct socket_message result;
	for (;;) {
		int type = socket_server_poll(ss, &result, NULL);
		// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
		switch (type)
		{
		case SOCKET_EXIT:
		{
			printf("exit(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
			return NULL;
		}break;
		case SOCKET_DATA:
		{
			printf("message(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
			free(result.data);
		}break;
		case SOCKET_CLOSE:
		{
			printf("close(%lu) [id=%d]\n", result.opaque, result.id);
		}break;
		case SOCKET_OPEN:
		{
			printf("open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
		}break;
		case SOCKET_ERR:
		{
			printf("error(%lu) [id=%d]\n", result.opaque, result.id);
		}break;
		case SOCKET_ACCEPT:
		{
			printf("accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);
		}break;
		default:
		{
			printf("Unknown socket message type %d.", type);
		}break;
		}
	}
}

void test_socket_server()
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	struct socket_server * ss = socket_server_create(getsystime());
	pthread_t pid;
	pthread_create(&pid, NULL, _socket_server_poll, ss);
	int c = socket_server_connect(ss, 100, "127.0.0.1", 80);
	printf("connecting %d\n", c);
	int l = socket_server_listen(ss, 200, "127.0.0.1", 8888, 32);
	printf("listening %d\n", l);
	socket_server_start(ss, 201, l);
	int b = socket_server_bind(ss, 300, 1);
	printf("binding stdin %d\n", b);
	int i;
	for (i = 0; i<3; i++)
	{
		socket_server_connect(ss, 400 + i, "127.0.0.1", 8888);
	}
	sleep(5);
	socket_server_exit(ss);
	pthread_join(pid, NULL);
	socket_server_release(ss);
}

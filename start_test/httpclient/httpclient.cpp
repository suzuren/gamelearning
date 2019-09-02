#include <pthread.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>

#include <time.h>

#include "select.h"

#include <string>

#include "support_algorithm.h"

char * http_build_post_data(int * datelen, const char * api, const char * host, const char * body)
{
	char buffer[1024] = { 0 };
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST "); strcat(buffer, api); strcat(buffer, " HTTP/1.1\r\n");
	strcat(buffer, "Host: "); strcat(buffer, host); strcat(buffer, "\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: ");	strcat(buffer, itoa_parser(strlen(body), 10));	strcat(buffer, "\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	strcat(buffer, body);
	int size = (int)strlen(buffer);
	char * pdata = (char *)malloc(size);
	memcpy(pdata, buffer, size);
	*datelen = size;
	return pdata;
}

int test_http_chair_api()
{
	// http://service.winic.org:8009/sys_port/gateway/index.asp
	// id=taranazi&pwd=GAN123pengjin&to=13380353070&content=您好，您的验证码是333333【XX游戏】！&time=
	const char * host = "service.winic.org";
	int port = 80;
	char *ip = socket_hosttoip(host);
	if (ip == NULL)
	{
		return 0;
	}
	int fd = socket_connect(ip, port);
	printf("fd:%d,ip:%s\n", fd, ip);

	const char * api = "/sys_port/gateway/index.asp";
	//const char * host = "service.winic.org";
	char body[1024] = { 0 };
	const char *id = "taranazi";
	const char *pwd = "GAN123pengjin";
	const char *to = "13380353070";
	const char *content = "您好，您的验证码是 333333 [XX游戏]！";
	sprintf(body, "id=%s&pwd=%s&to=%s&content=%s&time=", id, pwd, to, content);

	int datalen;
	char * pdata = http_build_post_data(&datalen, api, host, body);
	//printf("datalen:%d,pdata:\n-%s-\n", datalen, pdata);
	
	char wbuffer[10846];
	memset(wbuffer, 0, sizeof(wbuffer));
	memcpy(wbuffer, pdata, datalen);
	free(pdata);

	printf("datalen:%d,wbuffer:\n-%s-\n\n\n", datalen, wbuffer);

	int wlenght = 0;
	while (true)
	{
		int size_send = write(fd, wbuffer + wlenght, datalen);

		if (size_send != datalen)
		{
			printf("send buf error\n");
		}
		wlenght += size_send;
		datalen -= size_send;
		if (datalen == 0)
		{
			break;
		}
	}

	char rbuffer[16384 * 5] = { 0 };
	int  rlenght = 0;
	for (;;)
	{
		ms_sleep(3);
		memset(rbuffer, 0, sizeof(rbuffer));
		int rsize = read(fd, rbuffer + rlenght, sizeof(rbuffer) - rlenght);
		//printf("read function - rsize:%d,errno:%d,EINTR:%d,EAGAIN:%d\n",rsize,errno,EINTR,EAGAIN);
		if (rsize < 0)
		{
			if (errno == EINTR) // 指操作被中断唤醒，需要重新读 / 写
			{
				continue;
			}
			if (errno == EAGAIN) // 现在没有数据可读请稍后再试
			{
				continue;
			}
			fprintf(stderr, "socket : read socket error-%d-%s.\n\n", errno, strerror(errno));
			close(fd);
			break;
		}
		if (rsize == 0)
		{
			close(fd);
			printf("read socket close.\n");
			break;
		}
		rlenght += rsize;
		printf("rlenght:%d, rsize:%d,rbuffer:\n-%s-\n", rlenght, rsize, rbuffer);
	}
	printf("socket thread exit!\n");
	return 1;
}

int test_http_international_api()
{
	//http://service2.winic.org/service.asmx/SendInternationalMessages?
	//uid=taranazi&pwd=GAN123pengjin&tos=13380353070&msg=您好，您的验证码是333333【XX游戏】！&otime=
	const char * host = "service2.winic.org";
	int port = 80;
	char *ip = socket_hosttoip(host);
	if (ip == NULL)
	{
		return 0;
	}
	int fd = socket_connect(ip, port);
	printf("fd:%d,ip:%s\n", fd, ip);
	const char * api = "/service.asmx/SendInternationalMessages";
	char body[1024] = { 0 };
	const char *id = "taranazi";
	const char *pwd = "GAN123pengjin";
	const char *to = "13380353070";
	const char *content = "您好，您的验证码是 333333 [XX游戏]！";
	sprintf(body, "uid=%s&pwd=%s&tos=%s&msg=%s&otime=", id, pwd, to, content);

	int datalen;
	char * pdata = http_build_post_data(&datalen, api, host, body);
	//printf("datalen:%d,pdata:\n-%s-\n", datalen, pdata);

	char wbuffer[10846];
	memset(wbuffer, 0, sizeof(wbuffer));
	memcpy(wbuffer, pdata, datalen);
	free(pdata);

	printf("datalen:%d,wbuffer:\n-%s-\n\n\n", datalen, wbuffer);

	int wlenght = 0;
	while (true)
	{
		int size_send = write(fd, wbuffer + wlenght, datalen);

		if (size_send != datalen)
		{
			printf("send buf error\n");
		}
		wlenght += size_send;
		datalen -= size_send;
		if (datalen == 0)
		{
			break;
		}
	}

	char rbuffer[16384 * 5] = { 0 };
	int  rlenght = 0;
	for (;;)
	{
		ms_sleep(3);
		memset(rbuffer, 0, sizeof(rbuffer));
		int rsize = read(fd, rbuffer + rlenght, sizeof(rbuffer) - rlenght);
		//printf("read function - rsize:%d,errno:%d,EINTR:%d,EAGAIN:%d\n",rsize,errno,EINTR,EAGAIN);
		if (rsize < 0)
		{
			if (errno == EINTR) // 指操作被中断唤醒，需要重新读 / 写
			{
				continue;
			}
			if (errno == EAGAIN) // 现在没有数据可读请稍后再试
			{
				continue;
			}
			fprintf(stderr, "socket : read socket error-%d-%s.\n\n", errno, strerror(errno));
			close(fd);
			break;
		}
		if (rsize == 0)
		{
			close(fd);
			printf("read socket close.\n");
			break;
		}
		rlenght += rsize;
		printf("rlenght:%d, rsize:%d,rbuffer:\n-%s-\n", rlenght, rsize, rbuffer);
	}
	printf("socket thread exit!\n");
	return 1;
}

void test_CodeCharacter()
{
	char arrCodeCharacter[10] = { '0','1','2','3','4','5','6','7','8','9' };
	char chCode[7] = { 0 };
	for (int index = 0; index < 6; index++)
	{
		int iPos = GetRandInt(0, 9);
		chCode[index] = arrCodeCharacter[iPos];
		printf("index:%d,iPos:%d,chCode:%s\n", index, iPos, chCode);
	}
	std::string strCode;
	strCode.append(chCode, 6);
	printf("strCode:%s,chCode:%s\n",strCode.data(), chCode);
}

/*
HTTP/1.1 200 OK
Cache-Control: private
Content-Type: text/html; Charset=gb2312
Expires: Sun, 21 Jul 2019 21:16:41 GMT
Server: Microsoft-IIS/8.5
X-Powered-By: ASP.NET
Date: Sun, 21 Jul 2019 21:16:41 GMT
Connection: close
Content-Length: 39

*/

const char * pHttpData = "Connection: close\r\nContent-Length: 39\r\n\r\n-01/Send:1/Consumption:.1/Tmoney:0/sid:";
#define MAX_READ_HTTP_DATA 512
#define MAX_BODY_HTTP_DATA 128

struct tagPostData
{
	int rszie;
	char rbuffer[MAX_READ_HTTP_DATA];

	int bodylen;
	char body[MAX_BODY_HTTP_DATA];

	bool success;
	tagPostData()
	{
		rszie = 0;
		memset(rbuffer, 0, sizeof(rbuffer));

		bodylen = 0;
		memset(body, 0, sizeof(body));
	}
};

void test_ParseData()
{

	struct tagPostData tmpData;
	tmpData.rszie = strlen(pHttpData);
	memcpy(tmpData.rbuffer, pHttpData, tmpData.rszie);

	printf("rszie:%d,rbuffer:-%s-\n", tmpData.rszie, tmpData.rbuffer);

	char * Content_Length_str = (char *)"Content-Length: ";
	int Content_Length_size = (int)strlen(Content_Length_str);
	int Content_Length_nextval[Content_Length_size];
	kmp_get_nextval(Content_Length_str, Content_Length_nextval);
	int Content_Length_pos = kmp_index(tmpData.rbuffer, Content_Length_str, 0, Content_Length_nextval);

	int Content_Length_end = Content_Length_pos + Content_Length_size;

	printf("Content_Length_pos:%d,Content_Length_end:%d\n", Content_Length_pos, Content_Length_end);

	if (Content_Length_pos != -1 && (Content_Length_end + 3) < tmpData.rszie)
	{
		char * Content_Length_ptr = tmpData.rbuffer + Content_Length_end;
		char Content_Length_buf[4] = { 0 };
		int Content_Length_index = 0;
		for (int index = 0; index < 3; index++)
		{
			char ch = (*(Content_Length_ptr + index));
			if (isdigit(ch) == 1)
			{
				Content_Length_buf[Content_Length_index++] = ch;
			}
			else
			{
				break;
			}
		}
		int Content_Length = atoi(Content_Length_buf);
		//atoi_parser(&Content_Length, Content_Length_buf, Content_Length_index);

		printf("Content_Length_buf:%s,Content_Length:%d\n", Content_Length_buf, Content_Length);

		if (Content_Length > 0 && Content_Length < MAX_BODY_HTTP_DATA)
		{
			char * change_line_str = (char *)"\r\n\r\n";
			int change_line_size = (int)strlen(change_line_str);
			int change_line_nextval[change_line_size];
			kmp_get_nextval(change_line_str, change_line_nextval);
			int change_line_pos = kmp_index(tmpData.rbuffer, change_line_str, 0, change_line_nextval);
			int change_line_end = change_line_pos + change_line_size;

			printf("change_line_pos:%d,change_line_end:%d\n", change_line_pos, change_line_end);

			if (change_line_pos != -1 && (change_line_end + Content_Length) == tmpData.rszie)
			{
				char * change_line_ptr = tmpData.rbuffer + change_line_end;
				memcpy(tmpData.body, change_line_ptr, Content_Length);
				tmpData.bodylen = Content_Length;
				tmpData.success = true;
				
				printf("bodylen:%d,body:%s\n", tmpData.bodylen, tmpData.body);
			}
		}
	}

}

#define MAX_SOCKET_P 16
#define SOCKET_TYPE_INVALID 0
#define SOCKET_TYPE_RESERVE 1

#define MAX_SOCKET (1<<MAX_SOCKET_P)

#define HASH_ID(id) (((unsigned)id) % MAX_SOCKET)

#define PROTOCOL_UNKNOWN 255

#define ATOM_CAS(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)
#define ATOM_AND(ptr,n) __sync_and_and_fetch(ptr, n)

struct socket {
	int fd;
	int id;
	uint8_t protocol;
	uint8_t type;
	uint16_t udpconnecting;
};

struct socket_server {
	int alloc_id;
	struct socket slot[MAX_SOCKET];
};

static int
reserve_id(struct socket_server *ss) {
	int i;
	for (i = 0; i < MAX_SOCKET; i++) {
		int id = ATOM_INC(&(ss->alloc_id));
		if (id < 0) {
			id = ATOM_AND(&(ss->alloc_id), 0x7fffffff);
		}
		struct socket *s = &ss->slot[HASH_ID(id)];
		if (s->type == SOCKET_TYPE_INVALID) {
			if (ATOM_CAS(&s->type, SOCKET_TYPE_INVALID, SOCKET_TYPE_RESERVE)) {
				s->id = id;
				s->protocol = PROTOCOL_UNKNOWN;
				// socket_server_udp_connect may inc s->udpconncting directly (from other thread, before new_fd), 
				// so reset it to 0 here rather than in new_fd.
				s->udpconnecting = 0;
				s->fd = -1;
				return id;
			}
			else {
				// retry
				--i;
			}
		}
	}
	return -1;
}

void test_reserve_id()
{
	struct socket_server ss;
	ss.alloc_id = 0x7ffffffa;
	for (int index = 0; index < 9; index++)
	{
		int id = reserve_id(&ss);
		printf("index:%d,id:%d,alloc_id:0x%x - %d\n", index, id, ss.alloc_id, ss.alloc_id);
	}
	/*
	index:0,id:2147483644,alloc_id:0x7ffffffc - 2147483644
	index:1,id:2147483645,alloc_id:0x7ffffffd - 2147483645
	index:2,id:2147483646,alloc_id:0x7ffffffe - 2147483646
	index:3,id:0,alloc_id:0x0 - 0
	index:4,id:1,alloc_id:0x1 - 1
	index:5,id:2,alloc_id:0x2 - 2
	index:6,id:3,alloc_id:0x3 - 3
	index:7,id:4,alloc_id:0x4 - 4
	index:8,id:5,alloc_id:0x5 - 5

	*/
}

int main(int argc, char const *argv[])
{
	//test_http_chair_api();
	
	//test_http_international_api();

	//test_CodeCharacter();

	//test_ParseData();

	test_reserve_id();

	return 0;
}




#ifndef __HTTP_HEADER_H_
#define __HTTP_HEADER_H_


//  POST http://localhost/moodweb/Regist HTTP/1.1
//  Host: localhost
//	Connection: keep - alive
//	Content-Length : 10
//	Cache-Control : max - age = 0
//	Origin : http ://localhost
//	Upgrade-Insecure-Requests : 1
//	User-Agent : Mozilla / 5.0
//	Content-Type : application / x - www - form - urlencoded
//	Accept : text / html, application / xhtml + xml, application / xml;
//  Referer: http://localhost/moodweb/lw.html
//  Accept-Encoding: gzip, deflate
//  Accept-Language: zh-CN,zh;q=0.8
//  Cookie: JSESSIONID=D103732271DAB38D4FA763F463CC7695

#include "base_unit.h"

struct http_header_option
{
	char url[512];
	char version[32];
	char host[64];
	char content[512];
	int content_length;
};

void http_reset_header(struct http_header_option * pheader)
{
	if (pheader == NULL)
	{
		return;
	}
	memset(pheader->url, 0, sizeof(pheader->url));
	memset(pheader->version, 0, sizeof(pheader->version));
	memset(pheader->host, 0, sizeof(pheader->host));
	memset(pheader->content, 0, sizeof(pheader->content));
	pheader->content_length = 0;
}

void http_init_header(struct http_header_option * pheader)
{
	if (pheader == NULL)
	{
		return;
	}
	sprintf(pheader->url,"%s","/openapi/api");
	sprintf(pheader->version, "%s", "HTTP/1.1");
	sprintf(pheader->host, "%s", "tuling123.com");
	pheader->content_length = 0;
	
}

char * http_build_header(struct http_header_option * pheader)
{
	if (pheader == NULL)
	{
		return NULL;
	}
	static char buffer[2048];
	memset(buffer, 0, sizeof(buffer));
	// 请求行
	strcat(buffer, "POST ");
	strcat(buffer, pheader->url);
	strcat(buffer, " ");
	strcat(buffer, pheader->version);
	strcat(buffer, "\r\n");
	//请求头部
	strcat(buffer, "Host: ");
	strcat(buffer, pheader->host);
	strcat(buffer, "\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n");
	strcat(buffer, "Origin: http://");
	strcat(buffer, pheader->host);
	strcat(buffer, "\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: ");
	strcat(buffer, itoa_parser(pheader->content_length, 10));
	strcat(buffer, "\r\n");
	strcat(buffer, "Accept: text/html,application/xhtml+xml,application/xml;\r\n");
	strcat(buffer, "Accept-Language: zh-CN,zh;q=0.8\r\n");
	strcat(buffer, "\r\n");
	return buffer;
}

char * http_get_post_head()
{
	static char buffer[2048];
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST /openapi/api HTTP/1.0\r\n");
	strcat(buffer, "Host: tuling123.com\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: 0\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	//printf("len:%ld,buffer:%s", strlen(buffer), buffer);
	return buffer;
}


#endif



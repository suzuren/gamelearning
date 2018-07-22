
#include <stdio.h>

#include <memory.h>

#include <string>
#include <vector>
#include <stdlib.h>


static inline int hextoint(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

static inline int power_func(int a, int n)
{
	int z = 0;
	if (a == 0 && n == 0)
	{
		return z;
	}
	if (a < 0 || n < 0)
	{
		return z;
	}	
	if (n == 0)
	{
		z = 1;
		return z;
	}
	int b = a;
	std::vector<int> vecdata;
	for (int i = 0; i < n; i++)
	{
		vecdata.push_back(a);
	}
	for (unsigned int u = 1; u < vecdata.size(); u++)
	{
		b *= vecdata[u];
	}
	z = b;
	return z;
}



bool atoi_parser(int * ptrnum, char * ptrstr, int len)
{
	*ptrnum = 0;
	if (ptrnum == NULL || ptrstr == NULL || len == 0)
	{
		return false;
	}
	for (int i = 0; i < len; i++)
	{
		int ihex = hextoint(ptrstr[i]);
		int ipow = power_func(10, i);		
		(*ptrnum) += (ihex * ipow);
	}
	return true;
}

bool http_body_is_final(std::string & strdata)
{
	std::string::size_type pos_http_body_end = strdata.find("\r\n0\r\n\r\n");
	if (pos_http_body_end != std::string::npos)
	{
		return true;
	}
	return false;
}

void response_parser(std::string & strdata,std::string & str_content)
{
	str_content.clear();
	std::string str_parser = strdata;
	printf("response_parser - str_parser.size:%d -%s-\r\n", str_parser.size(), str_parser.c_str());

	if(http_body_is_final(str_parser))
	{
		do
		{
			std::string::size_type pos_chunked_size = str_parser.find("\r\n");
			std::string str_chunked_size = str_parser.substr(0, pos_chunked_size);
			if (str_chunked_size.size() > 64)
			{
				break;
			}
			char buffer[64] = { 0 };
			sprintf(buffer,"%s", str_chunked_size.c_str());
			int chunked_size = strtol(buffer, NULL, 16);
			printf("response_parser - chunked_size:%d,,buffer:%s\n", chunked_size, buffer);
			if (chunked_size == 0)
			{
				break;
			}
			str_content += str_parser.substr(pos_chunked_size + 2, chunked_size);
			str_parser = str_parser.substr(pos_chunked_size + 2 + chunked_size + 2, std::string::npos);

			printf("response_parser - str_chunked_size size:%d,chunked_size:%d,-%s-\n", str_chunked_size.size(), chunked_size, str_chunked_size.c_str());
			printf("response_parser - str_content size:%d-%s-\n", str_content.size(), str_content.c_str());
			printf("response_parser - str_parser size:%d-%s-\n", str_parser.size(), str_parser.c_str());
		} while (true);
	}
}


int main(int argc, const char** argv)
{
	printf("hello test string!\n");

	std::string str_check_body;
	std::string str_http_body;
	std::string str_http_content;
	
	const char * ptr = "GMT\r\n\r\n11\r\n12345678901234567\r\n12\r\nabcdefghijklmnopqr\r\n0\r\n\r\n";
	str_check_body.append(ptr, strlen(ptr));

	std::string::size_type pos_http_body_begin = str_check_body.find("\r\n\r\n");
	
	if (pos_http_body_begin != std::string::npos)
	{
		str_http_body = str_check_body.substr(pos_http_body_begin + 4, std::string::npos);
		response_parser(str_http_body, str_http_content);
	}
	//printf("\nbody size:%d,\n-%s-\n", str_http_body.size(), str_http_body.c_str());
	//printf("\ncont size:%d,\n-%s-\n", str_http_content.size(), str_http_content.c_str());

	//printf("AnalysisSocketData - pos begin:%d,end:%d,check_size:%d,str_http_body:%d-%s-",
	//	pos_http_body_begin, pos_http_body_end, str_check_body.size(), str_http_body.size(), str_http_body.c_str());


	return 0;
}

/*pos begin:3,end:18,str_http_body:18-7f
1234567
0

-

*/
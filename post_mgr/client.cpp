#include "epoll_socket.h"
#include "http_header.h"

#include <vector>
#include <string>
#include <sstream>

char * http_get_post_head()
{
	static char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST /landlords-pokerbot/register HTTP/1.0\r\n");
	strcat(buffer, "Host: 47.94.250.154:38018\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: 29\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	//printf("len:%ld,buffer:%s", strlen(buffer), buffer);
	strcat(buffer, "mac_address=FC-AA-14-D3-A4-E8");
	return buffer;
}

int main(int argc, char const *argv[])
{
	int client_fd = socket_connect("47.94.250.154", 38018);
	printf("socket_connect - client_fd:%d\n", client_fd);
	
	int iCount = 0;
	char buffer[65535] = { 0 };	
	memset(buffer, 0, sizeof(buffer));

	sprintf(buffer, "%s", http_get_post_head());

	int len_buff = strlen(buffer);
	while (len_buff>0)
	{
		int len_send = write(client_fd, buffer, len_buff);
		printf("len_buff:%d,len_send:%d,buffer:\n%s\n", len_buff, len_send, buffer);
		len_buff -= len_send;
	}

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		int nread = read(client_fd, buffer, 65535);
		if (nread == 0)
		{
			close(client_fd);
			break;
		}
		else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			continue;
		}
		else
		{
			//printf("recv - nread:%d,len:%d,buffer:\n%s.\n", nread,strlen(buffer), buffer);
			//for (int i = 0; i < 18; i++)
			//{
			//	printf("i:%d - %c\n", i,buffer[i]);
			//}

			std::string response;
			response.append(buffer, nread);
			printf("\n\nresponse:\n%s\n",response.c_str());
			std::string temp = response;
			std::string::size_type pos_http_ver = temp.find("\r\n");
			std::string str_http_line;
			std::string str_http_body;
			if (std::string::npos != pos_http_ver)
			{
				//temp.erase(pos_http_ver);
				str_http_line = temp.substr(0, pos_http_ver);
			}
			bool bLineCompare = (str_http_line == "HTTP/1.1 200 ");
			//printf("\n\nstr_http_line:%d-%s,npos:%d,pos_http_ver:%d,bIsCome:%d\n", str_http_line.size(), str_http_line.c_str(), std::string::npos,pos_http_ver, bIsCome);
			if (bLineCompare)
			{
				std::string::size_type pos_http_body = temp.find("\r\n\r\n");
				str_http_body = temp.substr(pos_http_body+4, std::string::npos);
				//printf("npos:%d,pos_http_body:%d,str_http_body:%s - temp:\n%s\n", std::string::npos, pos_http_body, str_http_body.c_str(), temp.c_str());
				printf("\n\nstr_http_body:%d-%s", str_http_body.size(), str_http_body.c_str());

			}
		}
		usleep(3* 1000 * 1000); // 3 seconds
	}
}




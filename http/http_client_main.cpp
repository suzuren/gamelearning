
//#include <glog/logging.h>
#include "http_client.h"

int main (int argc, char *argv[])
{
	std::string request_url = "ws://127.0.0.1:8001/ws";
	std::string local_file_name = "";
    HttpClient *http_client;
	char str[10];
	sprintf(str, "%d", 99978);
	std::string path = request_url + std::string(str) + std::string(".html");
	http_client = new HttpClient(request_url, path);
	http_client->Run();
	delete http_client;

    return 0;
}

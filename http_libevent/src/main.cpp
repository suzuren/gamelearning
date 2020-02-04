
#include <iostream>
#include "daemonize.h"
#include "http_server.h"

int main(int argc, const char** argv)
{
	std::cout << "hello world!" << std::endl;
	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));

	HttpServerListen();

	return 0;
}
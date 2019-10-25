
#include "plmhead.h"

#include <map>
#include <string>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
	std::cout << "hello world" << std::endl;

	if (CSpdlogPlm::Start() == false)
	{
		return 0;
	}

	spdlog::set_default_logger(CSpdlogPlm::GetLoggerPtr("logger_name"));


	SPDLOG_INFO("SPDLOG_INFO init success...");
	CSpdlogPlm::String("CSpdlogPlm::String init success...");

	CSpdlogPlm::Format(spdlog::level::debug, "CSpdlogPlm::Format init success... debug {}", 333);
	CSpdlogPlm::Format(spdlog::level::info, "CSpdlogPlm::Format init success... info {}", 333);

	return 1;
}
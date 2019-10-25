
#include "spdlogdll.h"


CSpdlogDLL::CSpdlogDLL()
{
}

CSpdlogDLL::~CSpdlogDLL()
{

}

void prepare_logdir()
{
	spdlog::drop_all();
#ifdef _WIN32
	system("if not exist logs mkdir logs");
	system("del /F /Q logs\\*");
#else
	auto rv = system("mkdir -p logs");
	if (rv != 0)
	{
		throw std::runtime_error("Failed to mkdir -p logs");
	}
	rv = system("rm -f logs/*");
	if (rv != 0)
	{
		throw std::runtime_error("Failed to rm -f logs/*");
	}
#endif
}

bool CSpdlogDLL::Start(std::string logger_name, std::string filename)
{

	std::string str_logger_name = logger_name;
	std::string str_filename = filename;
	
	if (str_logger_name.empty() == true)
	{
		str_logger_name = "logger_name";
	}
	//auto logger = spdlog::get(str_logger_name);
	//if(logger != nullptr)
	//{
	//	return true;
	//}
	if (m_mpLoggerName.find(str_logger_name) != m_mpLoggerName.end())
	{
		return true;
	}
	if (str_filename.empty() == true)
	{
		str_filename = "spdlog";
	}
	std::stringstream filenamepath;
	filenamepath << "logs\\" << str_filename << ".txt";

	prepare_logdir();


	try
	{
		// 每个文件64M 1024个旋转文件
		//g_sp_logger = spdlog::rotating_logger_mt("rotating_logger_name", filenamepath.str(), 1048576 * 64, 1024);
		// 每天一个文件
		auto sp_logger = spdlog::daily_logger_mt(str_logger_name, filenamepath.str(), 0, 0, false);
		spdlog::set_default_logger(sp_logger);

		//SPDLOG_INFO("CSpdlogDLL start init...");

		m_mpLoggerName.insert(std::make_pair(str_logger_name, sp_logger));

	}
	catch (const spdlog::spdlog_ex &ex)
	{
		std::printf("spd initialization failed: %s\n", ex.what());
		return false;
	}

	return true;
}

bool CSpdlogDLL::Stop()
{
	//spdlog::drop_all();
	//spdlog::shutdown();

	return true;
}

std::shared_ptr<spdlog::logger> CSpdlogDLL::GetLoggerPtr(std::string logger_name)
{
	std::shared_ptr<spdlog::logger> splogger = nullptr;
	auto iter = m_mpLoggerName.find(logger_name);
	if (iter != m_mpLoggerName.end())
	{
		splogger = iter->second;
	}
	return splogger;
}

bool CSpdlogPlm::Start(std::string logger_name, std::string filename)
{
	g_SpdlogDLLManager.Start(logger_name, filename);
	spdlog::set_default_logger(CSpdlogPlm::GetLoggerPtr("logger_name"));

	return true;
}

//停止服务
bool CSpdlogPlm::Stop()
{
	return g_SpdlogDLLManager.Stop();
}

std::shared_ptr<spdlog::logger> CSpdlogPlm::GetLoggerPtr(std::string logger_name)
{
	return g_SpdlogDLLManager.GetLoggerPtr(logger_name);
}

void CSpdlogPlm::String(std::string strData)
{
	if (spdlog::default_logger()->level() == spdlog::level::level_enum::trace)
	{
		spdlog::default_logger()->trace("{}", strData);
		spdlog::default_logger()->flush_on(spdlog::level::trace);
	}
	else if (spdlog::default_logger()->level() == spdlog::level::level_enum::debug)
	{
		spdlog::default_logger()->debug("{}", strData);
	}
	else if (spdlog::default_logger()->level() == spdlog::level::level_enum::info)
	{
		spdlog::default_logger()->info("{}", strData);
		spdlog::default_logger()->flush_on(spdlog::level::info);

	}
	else if (spdlog::default_logger()->level() == spdlog::level::level_enum::warn)
	{
		spdlog::default_logger()->warn("{}", strData);
	}
	else if (spdlog::default_logger()->level() == spdlog::level::level_enum::err)
	{
		spdlog::default_logger()->error("{}", strData);
	}
	else if (spdlog::default_logger()->level() == spdlog::level::level_enum::critical)
	{
		spdlog::default_logger()->critical("{}", strData);
	}
	else
	{
		spdlog::default_logger()->log(spdlog::level::level_enum::info, "{}", strData);
	}
}

CSpdlogDLL g_SpdlogDLLManager;

//extern "C" __declspec(dllexport) void * CreateSpdlogDllManager(int instype, const char *content)
//{
//	return &g_SpdlogDLLManager;
//}


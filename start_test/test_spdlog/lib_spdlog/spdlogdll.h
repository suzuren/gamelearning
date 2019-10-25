
#pragma once

#include "spdlog\spdlog.h"
#include "spdlog\sinks\daily_file_sink.h"

#include <map>
#include <string>
#include <iostream>
#include <sstream>

class CSpdlogDLL
{
protected:
	std::map<std::string, std::shared_ptr<spdlog::logger>>	m_mpLoggerName;

public:
	CSpdlogDLL();
	virtual ~CSpdlogDLL();

public:
	bool Start(std::string logger_name = std::string(), std::string filename = std::string());
	bool Stop();

	std::shared_ptr<spdlog::logger> GetLoggerPtr(std::string logger_name);
};

class _declspec(dllexport) CSpdlogPlm
{
public:

	static bool Start(std::string logger_name = std::string(), std::string filename = std::string());

	static bool Stop();

	static std::shared_ptr<spdlog::logger> GetLoggerPtr(std::string logger_name);

	static void String(std::string strData);

	template<typename... Args>
	static void Format(spdlog::level::level_enum level,std::string strFormat, const Args &... args)
	{
		spdlog::default_logger()->set_level(level);
		if (spdlog::default_logger()->level() == spdlog::level::level_enum::trace)
		{
			//spdlog::default_logger()->trace(strFormat, std::forward<Args>(args)...);
			spdlog::default_logger()->trace(strFormat, args...);
		}
		else if (spdlog::default_logger()->level() == spdlog::level::level_enum::debug)
		{
			spdlog::default_logger()->debug(strFormat, args...);
		}
		else if (spdlog::default_logger()->level() == spdlog::level::level_enum::info)
		{
			spdlog::default_logger()->info(strFormat, args...);
		}
		else if (spdlog::default_logger()->level() == spdlog::level::level_enum::warn)
		{
			spdlog::default_logger()->warn(strFormat, args...);
		}
		else if (spdlog::default_logger()->level() == spdlog::level::level_enum::err)
		{
			spdlog::default_logger()->error(strFormat, args...);
		}
		else if (spdlog::default_logger()->level() == spdlog::level::level_enum::critical)
		{
			spdlog::default_logger()->critical(strFormat, args...);
		}
		else
		{
			spdlog::default_logger()->log(spdlog::level::level_enum::info, strFormat, args...);
		}
	}
};


extern CSpdlogDLL g_SpdlogDLLManager;

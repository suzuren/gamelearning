
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stdarg.h>

struct tagServerConfig
{
	int iLoadindex;
	int iServerType;
	int iServerID;
	int iArrServer[17];
	tagServerConfig()
	{
		Init();
	}
	void operator=(const tagServerConfig& cfg)
	{
		iLoadindex = cfg.iLoadindex;
		iServerType = cfg.iServerType;
		iServerID = cfg.iServerID;
		for (int i = 0; i < 17; i++)
		{
			iArrServer[i] = cfg.iArrServer[i];
		}
	}
	void Init()
	{
		memset(this, 0, sizeof(tagServerConfig));
		iLoadindex = 0;
	}
};

std::map<int, std::vector<tagServerConfig>> g_mpServerConfig;

void InitServer()
{
	for (int i = 0; i < 2; i++)
	{
		tagServerConfig cfg;
		cfg.iServerType = i + 1;
		std::vector<tagServerConfig> vecCfg;
		for (int j = 0; j < 5; j++)
		{
			cfg.iServerID = i + 1;
			vecCfg.push_back(cfg);
		}
		g_mpServerConfig.insert(std::make_pair(cfg.iServerType, vecCfg));
	}
}

std::map<int, std::vector<tagServerConfig>> & GetServerConfig()
{
	return g_mpServerConfig;
}


std::string StrFormat(const char* fmt, ...)
{
	char temp[2048];
	memset(temp, 0, sizeof(temp));
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(temp, sizeof(temp), fmt, ap);
	va_end(ap);
	return temp;
}

void test_stl_function()
{
	InitServer();
	int iLoopIndex = 0;
	do
	{
		auto & mpServerConfig = GetServerConfig();
		for (auto iter_loop = mpServerConfig.begin(); iter_loop != mpServerConfig.end(); iter_loop++)
		{
			int iServerType = iter_loop->first;
			auto & vecCfg = iter_loop->second;
			std::sort(std::begin(vecCfg), std::end(vecCfg), [](const auto &a, const auto &b) { return a.iLoadindex < b.iLoadindex; });
			std::string strVecCfg;
			for (unsigned int i = 0; i < vecCfg.size(); i++)
			{
				auto & cfg = vecCfg[i];
				//strVecCfg += StrFormat("i:%d,iServerID:%d,iLoadindex:%d ", i, cfg.iServerID, cfg.iLoadindex);
				strVecCfg += StrFormat("i:%d,iLoadindex:%d ", i, cfg.iLoadindex);
			}

			auto & cfg = vecCfg[0];
			cfg.iLoadindex++;
			if (cfg.iLoadindex >= 0x7FFFFFFF)
			{
				cfg.iLoadindex = 0;
			}
			printf("test_stl_function - iServerType:%d,iLoadindex:%d,iServerType:%d,iServerID:%d,strVecCfg ->%s\n",
				iServerType, cfg.iLoadindex, cfg.iServerType, cfg.iServerID, strVecCfg.c_str());

		}
	} while (iLoopIndex++ < 99);

}











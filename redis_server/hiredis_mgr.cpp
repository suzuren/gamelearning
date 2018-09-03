
#include "hiredis_mgr.h"

#include <string.h>
#include <memory.h>
// ---------------------------------------------------------------------------------------

bool CRedisMgr::Init()
{
	m_RedisHandle.SetConfig("47.52.103.72", 13000);

	return true;
}

bool CRedisMgr::Connect()
{
	bool ret = false;

	ret = m_RedisHandle.Connect();

	return ret;
}

void CRedisMgr::ShutDown()
{
	m_RedisHandle.FreeRedis();
	m_RedisHandle.FreeReply();
}

void CRedisMgr::OnTick()
{
	m_RedisHandle.CheckReconnect();
}

// ---------------------------------------------------------------------------------------

const static char *s_key_slog = "SLOG";
const static char *s_key_svrsinfo = "SVRINFO";

void CRedisMgr::Test()
{
	int rec = -10;

	rec = m_RedisHandle.IncrRecord("TEST_REDIS_KEY",10,60);
	printf("redis IncrRecord - rec:%d\n", rec);

	rec = m_RedisHandle.IsExist(s_key_slog);
	printf("redis IsExist - rec:%d\n", rec);
	rec = m_RedisHandle.IsExist(s_key_svrsinfo);
	printf("redis IsExist - rec:%d\n", rec);

	rec = m_RedisHandle.Delete("TEST_REDIS_KEY");
	printf("redis Delete - rec:%d\n", rec);

	rec = m_RedisHandle.SelectDB(0);
	printf("redis SelectDB - rec:%d\n", rec);
	
	rec = m_RedisHandle.SetString("TEST_REDIS_KEY", "test_key", 60);
	printf("redis SetString - rec:%d\n", rec);

	std::string str = m_RedisHandle.GetString("TEST_REDIS_KEY");
	printf("redis GetString - str:%s\n", str.data());

	m_RedisHandle.SetHashBinData("TEST_REDIS_HASH_KEY", 8,"test_hash_key", strlen("test_hash_key"));
	char buffer[512] = { 0 };
	rec = m_RedisHandle.GetHashBinData("TEST_REDIS_HASH_KEY", 8, buffer, sizeof(buffer));
	printf("redis GetHashBinData - rec:%d,buffer:%s\n", rec, buffer);
	m_RedisHandle.DelHashBinData("TEST_REDIS_HASH_KEY", 8);

	m_RedisHandle.SetBinData("TEST_REDIS_SET_BIN_KEY", 16, "test_set_bin_data", strlen("test_set_bin_data"), 60);
	memset(buffer, 0, sizeof(buffer));
	rec = m_RedisHandle.GetBinData("TEST_REDIS_SET_BIN_KEY", 16, buffer, sizeof(buffer));
	printf("redis GetBinData - rec:%d,buffer:%s\n", rec, buffer);
	m_RedisHandle.DelBinData("TEST_REDIS_SET_BIN_KEY", 16);




}



// ---------------------------------------------------------------------------------------

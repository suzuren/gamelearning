
#include <stdio.h>
#include <sstream>
#include <memory.h>
#include <stdlib.h>

#include "hiredis_operate.h"

CHiredisOperate::CHiredisOperate()
{
	m_redis = NULL;
	m_reply = NULL;
}
CHiredisOperate::~CHiredisOperate()
{
	FreeReply();
	FreeRedis();
}

bool	CHiredisOperate::SetConfig(const char * ip, unsigned int port)
{
	m_redisIp   = ip;
	m_redisPort = port;
	return true;
}

bool	CHiredisOperate::Connect()
{
	FreeRedis();
	//struct timeval timeout = {1, 500000};
	//m_redis = redisConnectWithTimeout(m_redisIp.c_str(), m_redisPort, timeout);
	m_redis = redisConnect(m_redisIp.c_str(), m_redisPort);
	//printf("CHiredisOperate::Connect - m_redis:%p,err:%d\n", m_redis, m_redis->err);
	if (m_redis == NULL || m_redis->err != REDIS_OK)
	{
		FreeRedis();
		return false;
	}
	else
	{
		return true;
	}
	FreeRedis();
	return false;
}

bool	CHiredisOperate::IsConnect()
{
	if (m_redis == NULL)
	{
		return false;
	}
	bool bRet = true;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"PING"));
	if(m_reply == NULL || m_reply->type != REDIS_REPLY_STATUS || strcasecmp(m_reply->str,"PONG") != 0)
	{
		bRet = false;
	}
	FreeReply();
	return bRet;
}
void	CHiredisOperate::CheckReconnect()
{
	if(!IsConnect())
	{
		Connect();	
		return;
	}
}
void	CHiredisOperate::FreeReply()
{
	if(m_reply != NULL)
	{
		freeReplyObject(m_reply);
		m_reply = NULL;
	}
}
void 	CHiredisOperate::FreeRedis()
{
	if(m_redis != NULL)
	{
		redisFree(m_redis);
		m_redis = NULL;
	}
}

void    CHiredisOperate::SetKeepAlive(const char * key, int iTimeOut)
{
	if (m_redis == NULL)
	{
		return;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "expire %s %d", key, iTimeOut));
	FreeReply();
}

long long CHiredisOperate::IncrRecord(const char* key, int nAdd, int expire)
{
	if (m_redis == NULL)
	{
		return 0;
	}

	char value[32] = {0};
	long long rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	if(m_reply != NULL)
	{
		rec = m_reply->integer;
		FreeReply();
	}
	if(rec == -3)
	{
		return -1;
	}
	if(rec == 1)		//存在,更新
	{
		m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "INCRBY %s %d", key,nAdd));
		if(m_reply != NULL)
		{
			rec = m_reply->integer;
			FreeReply();
		}
		else
		{
			rec = -1;
		}
		return rec;
	}
	else//不存在,创建该key并初始化为nAdd
	{
		reinterpret_cast<redisReply*>(redisCommand(m_redis, "SET %s %d", key, nAdd));
		if(expire != -1)
		{
			sprintf(value, "%d", expire);
			m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXPIRE %s %s", key, value));
			if(m_reply != NULL)
			{
				rec = m_reply->integer;
				FreeReply();
			}
			else
			{
				rec = -1;
			}
			return rec;
		}
	}
	return 0;
}
int 	CHiredisOperate::IsExist(const char* key)
{
	if (m_redis == NULL)
	{
		return 0;
	}
	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	if(m_reply != NULL)
	{
		rec = m_reply->integer;
		FreeReply();
	}
	return rec;	
}
int 	CHiredisOperate::Delete(const char* key)
{
	if (m_redis == NULL)
	{
		return 0;
	}
	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "DEL %s", key));
	if(m_reply != NULL)
	{
		rec = m_reply->integer;
		FreeReply();
	}
	return rec;	
}
int 	CHiredisOperate::SelectDB(int index)
{
	if (m_redis == NULL)
	{
		return 0;
	}
	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SELECT %d", index));
	if(m_reply != NULL)
	{
		rec = m_reply->integer;
		FreeReply();
	}
	return rec;
}
int	 CHiredisOperate::SetString(const char* key,const char * val,int expire)
{
	if (m_redis == NULL)
	{
		return 0;
	}

	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SET %s %s EX %d",key,val,expire));
	if(m_reply != NULL)
	{
		rec = m_reply->integer;
		FreeReply();
	}
	return rec;	
}
std::string	CHiredisOperate::GetString(const char* key)
{
	std::string res;
	if (m_redis == NULL)
	{
		return res;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"GET %s",key));
	if(NULL != m_reply && m_reply->str != NULL && m_reply->len > 0)
	{
		res = m_reply->str;
	}
	FreeReply();	
	return res;
}

unsigned int CHiredisOperate::GetHashBinData(const char* key, unsigned int pid, char *pdata, unsigned int len)
{
	if (m_redis == NULL)
	{
		return 0;
	}
	if (!(key != NULL &&  pid > 0 && pdata != NULL && len > 0))
	{
		return 0;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"HGET %s %d",key, pid));
	if (m_reply == NULL)
	{
		return 0;
	}
	if (!(len >= m_reply->len))
	{
		return 0;
	}
	memcpy(pdata, m_reply->str, m_reply->len);
	unsigned int datalen = m_reply->len;
	FreeReply();
	return datalen;
}

void CHiredisOperate::SetHashBinData(const char* key, unsigned int pid, const char *pdata, unsigned int len)
{
	if (m_redis == NULL)
	{
		return;
	}
	if (!(key != NULL && pid > 0 && pdata != NULL))
	{
		return;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"HSET %s %d %b", key, pid, pdata, len));
	FreeReply();
}

void CHiredisOperate::DelHashBinData(const char* key, unsigned int pid)
{
	if (m_redis == NULL)
	{
		return;
	}
	if (!(key != NULL && pid > 0))
	{
		return;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"HDEL %s %d", key, pid));
	FreeReply();
}
// 操作key 二进制数据
unsigned int  CHiredisOperate::GetBinData(const char* key, unsigned long long pid,char* pdata, unsigned int len)
{
	if (m_redis == NULL)
	{
		return 0;
	}
	if (!(key != NULL &&  pid > 0 && pdata != NULL && len > 0))
	{
		return 0;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"GET %s%llu",key, pid));
	if (m_reply == NULL)
	{
		return 0;
	}
	if (!(len >= m_reply->len))
	{
		return 0;
	}
	memcpy(pdata, m_reply->str, m_reply->len);
	unsigned int datalen = m_reply->len;	
	FreeReply();
	return datalen;
}
void    CHiredisOperate::SetBinData(const char* key, unsigned long long pid,const char* pdata, unsigned int len,int expire)
{
	if (m_redis == NULL)
	{
		return;
	}
	if (!(key != NULL && pid > 0 && pdata != NULL))
	{
		return;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"SET %s%llu %b EX %d", key, pid, pdata, len,expire));
	FreeReply();
}
void    CHiredisOperate::DelBinData(const char* key, unsigned long long pid)
{
	if (m_redis == NULL)
	{
		return;
	}
	if (!(key != NULL && pid > 0))
	{
		return;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"DEL %s%llu", key, pid));
	FreeReply();
}
bool CHiredisOperate::GetMultiHashBinData(const char* key, unsigned int pid[], unsigned int pid_num, GetMultiHashBinDataCB func, void *func_param)
{
	if (m_redis == NULL)
	{
		return false;
	}
	if (!(key != NULL && pid_num > 0 && func != NULL))
	{
		return false;
	}
	static std::stringstream ss;
	ss.clear();
	ss.str("");
	for (unsigned int i = 0; i < pid_num; ++i){
		ss << " " << pid[i];
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"HMGET %s %s",key, ss.str().c_str()));
	if (m_reply != NULL)
	{
		return false;
	}
	bool ret = func(m_reply, func_param);
	FreeReply();
	return ret;
}





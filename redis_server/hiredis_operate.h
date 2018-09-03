
#ifndef HIREDIS_OP_H__
#define HIREDIS_OP_H__

#include "hiredis.h"
#include <string>

class CHiredisOperate
{
public:
	typedef bool (*GetMultiHashBinDataCB)(const redisReply *reply, void *param);
	CHiredisOperate();
	~CHiredisOperate();
	
	bool	SetConfig(const char * ip, unsigned int port);
	bool	Connect();
	bool	IsConnect();
	void	CheckReconnect();
	
	void	FreeReply();
	void 	FreeRedis();

	// 设置一个key的过期的时间:秒
	void    SetKeepAlive( const char * key, int iTimeOut=30*60 );
	long long 	IncrRecord(const char* key, int nAdd, int expire = -1);
	int		IsExist(const char* key);
	int		Delete(const char* key);
	int		SelectDB(int index);

	int		SetString(const char* key,const char * val,int expire = -1);
	std::string	GetString(const char* key);
	

	//@brief	获得Hash二进制数据
	unsigned int  GetHashBinData(const char* key, unsigned int pid, char *pdata, unsigned int len);
	//@brief	保存Hash二进制数据
	void    SetHashBinData(const char* key, unsigned int pid, const char *pdata, unsigned int len);
	//@brief	删除Hash二进制数据
	void    DelHashBinData(const char* key, unsigned int pid);

	// 操作key 二进制数据
	unsigned int  GetBinData(const char* key,unsigned long long pid,char* pdata, unsigned int len);
	void	SetBinData(const char* key, unsigned long long pid,const char* pdata, unsigned int len,int expire = -1);
	void	DelBinData(const char* key, unsigned long long pid);
	
	//@brief	通过Key与field批量获得Hash二进制数据,需要提供回调函数，在回调函数中处理数据
	//input		(键值，字段列表，字段数量，回调处理函数，转给回调函数的参数)
	//notice	不能用用于分布区的情况
	bool GetMultiHashBinData(const char* key, unsigned int pid[], unsigned int pid_num, GetMultiHashBinDataCB func, void *func_param = NULL);

protected:
	redisContext *          m_redis;				    //连接hiredis
    redisReply *            m_reply;					//hiredis回复
	std::string				m_redisIp;
	unsigned int			m_redisPort;

};




#endif // HIREDIS_OP_H__






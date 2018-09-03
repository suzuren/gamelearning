
#ifndef __NETWORK_MGR_H_
#define __NETWORK_MGR_H_

#include "hiredis_operate.h"

class CRedisMgr
{
protected:
	CRedisMgr() {}
	~CRedisMgr()	{}
private:
	const CRedisMgr & operator=(const CRedisMgr &);
public:
	static  CRedisMgr & Instance()
	{
		static  CRedisMgr  s_SingleObj;
		return  s_SingleObj;
	}
private:
	CHiredisOperate m_RedisHandle;

public:
	bool	Init();
	bool	Connect();
	void    ShutDown();
	void    OnTick();

public:

	void Test();

};


#endif



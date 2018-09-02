
#ifndef __NETWORK_MGR_H_
#define __NETWORK_MGR_H_

class CNetworkMgr
{
protected:
	CNetworkMgr() {}
	~CNetworkMgr()	{}
private:
	const CNetworkMgr & operator=(const CNetworkMgr &);
public:
	static  CNetworkMgr & Instance()
	{
		static  CNetworkMgr  s_SingleObj;
		return  s_SingleObj;
	}
private:


public:
	bool	Init();
	bool	Connect();
	void    ShutDown();
	void    OnNetworkTick();

public:


};


#endif



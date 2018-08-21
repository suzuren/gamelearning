
#include "network_mgr.h"

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetworkRequest()
{
	auto sptrRequest = m_sptrNetAsyncOper->GetAsyncRequest();
	DispatchNetworkCallBack(std::move(sptrRequest));
}

void CNetworkMgr::DispatchNetworkCallBack(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (m_sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetCallBack->OnProcessNetworkEvent(sptrRequest);
	}
}

// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{
	m_sptrNetAsyncOper = std::make_shared<CNetworkTask>();
	if (m_sptrNetAsyncOper == nullptr)
	{
		return false;
	}
	bool flag = m_sptrNetAsyncOper->Init();
	if (flag == false)
	{
		return false;
	}
	flag = m_sptrNetAsyncOper->Start(IPADDR, PORT);
	if (flag == false)
	{
		return false;
	}
	printf("CNetworkMgr::Init - flag:%d,fd:%d\n", flag, m_sptrNetAsyncOper->GetListenFd());
	return true;
}

void CNetworkMgr::ShutDown()
{
	m_sptrNetAsyncOper->ShutDown();
	m_sptrNetAsyncOper.reset();
	m_sptrNetAsyncOper = nullptr;

}

void CNetworkMgr::SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack)
{
	if (sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetCallBack = sptrAsyncNetCallBack;
	}
}


void CNetworkMgr::OnNetworkTick()
{
	DispatchNetworkRequest();
}

void CNetworkMgr::TestNetwork()
{

}



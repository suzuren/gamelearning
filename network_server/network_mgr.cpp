
#include "network_mgr.h"

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetworkRequest()
{
	if (m_sptrNetTaskOper != nullptr)
	{
		auto sptrRequest = m_sptrNetTaskOper->GetAsyncRequest();
		if (m_sptrAsyncNetTaskCallBack != nullptr)
		{
			m_sptrAsyncNetTaskCallBack->OnProcessNetworkEvent(sptrRequest);
		}
	}
	if (m_sptrNetWrapOper != nullptr)
	{
		auto sptrRequest = m_sptrNetWrapOper->GetAsyncRequest();
		if (m_sptrAsyncNetWrapCallBack != nullptr)
		{
			m_sptrAsyncNetWrapCallBack->OnProcessNetworkEvent(sptrRequest);
		}
	}
}

bool CNetworkMgr::StartTask()
{
	m_sptrNetTaskOper = std::make_shared<CNetworkTask>();
	if (m_sptrNetTaskOper == nullptr)
	{
		return false;
	}
	bool flag = m_sptrNetTaskOper->Init();
	if (flag == false)
	{
		return false;
	}
	flag = m_sptrNetTaskOper->Start("0.0.0.0", PORT);
	if (flag == false)
	{
		return false;
	}
	printf("CNetworkMgr::Init - flag:%d,fd:%d\n", flag, m_sptrNetTaskOper->GetListenFd());

	return true;
}

bool CNetworkMgr::StartWrap()
{
	m_sptrNetWrapOper = std::make_shared<CNetworkWrap>();
	if (m_sptrNetWrapOper == nullptr)
	{
		return false;
	}
	bool flag = m_sptrNetWrapOper->Init();
	if (flag == false)
	{
		return false;
	}
	flag = m_sptrNetWrapOper->Start("127.0.0.1", PORT);
	if (flag == false)
	{
		return false;
	}
	printf("CNetworkMgr::Init - flag:%d,fd:%d\n", flag, m_sptrNetWrapOper->GetClientFd());

	return true;
}


// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{
	if (StartTask() == false)
	{
		return false;
	}
	if (StartWrap() == false)
	{
		return false;
	}
	return true;
}

void CNetworkMgr::ShutDown()
{
	if (m_sptrNetTaskOper != nullptr)
	{
		m_sptrNetTaskOper->ShutDown();
		m_sptrNetTaskOper.reset();
		m_sptrNetTaskOper = nullptr;
	}
	if (m_sptrNetWrapOper != nullptr)
	{
		m_sptrNetWrapOper->ShutDown();
		m_sptrNetWrapOper.reset();
		m_sptrNetWrapOper = nullptr;
	}
}

void CNetworkMgr::SetAsyncNetTaskCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack)
{
	if (sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetTaskCallBack = sptrAsyncNetCallBack;
	}
}
void CNetworkMgr::SetAsyncNetWrapCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack)
{
	if (sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetWrapCallBack = sptrAsyncNetCallBack;
	}
}

void CNetworkMgr::OnNetworkTick()
{
	DispatchNetworkRequest();
}

void CNetworkMgr::TestNetwork()
{

}



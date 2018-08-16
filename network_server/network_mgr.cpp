
#include "network_mgr.h"

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetworkEvent()
{
	auto sptrResponse = m_sptrNetAsyncOper->GetAsyncExecuteResult();
	DispatchNetworkCallBack(std::move(sptrResponse));
}

void CNetworkMgr::DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (m_sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetCallBack->OnProcessNetworkEvent(sptrResponse);
	}
}

// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{

	return true;
}

void CNetworkMgr::ShutDown()
{
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
	DispatchNetworkEvent();
}

void CNetworkMgr::TestNetwork()
{

}



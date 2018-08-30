
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
}

bool CNetworkMgr::StartTask()
{
	if (m_sptrNetTaskOper != nullptr)
	{
		return false;
	}
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
	printf("CNetworkMgr::StartTask - flag:%d\n", flag);

	return true;
}



// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{
	if (StartTask() == false)
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
}

void CNetworkMgr::SetAsyncNetTaskCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack)
{
	if (sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetTaskCallBack = sptrAsyncNetCallBack;
	}
}

void CNetworkMgr::OnNetworkTick()
{
	DispatchNetworkRequest();
}

void CNetworkMgr::TestNetwork()
{
}


void CNetworkMgr::TestNetworkTaskSendData(int contextid, struct packet_buffer & data)
{
	if (m_sptrNetTaskOper == nullptr)
	{
		return;
	}
	std::shared_ptr<struct tagTaskSendData> sptrData = std::make_shared<struct tagTaskSendData>();
	memcpy(sptrData->buffer, &data, data.header.length);
	sptrData->length = data.header.length;
	sptrData->fd = contextid;
	m_sptrNetTaskOper->SendData(sptrData);

}


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
	printf("CNetworkMgr::StartTask - flag:%d,fd:%d\n", flag, m_sptrNetTaskOper->GetListenFd());

	return true;
}

bool CNetworkMgr::StartWrap()
{
	if (m_sptrNetWrapOper != nullptr)
	{
		return false;
	}
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
	//printf("CNetworkMgr::StartWrap - flag:%d,fd:%d\n", flag, m_sptrNetWrapOper->GetClientFd());

	return true;
}


// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{
	if (StartTask() == false)
	{
		return false;
	}
	//if (StartWrap() == false)
	//{
	//	return false;
	//}
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

void CNetworkMgr::TestNetworkConnect()
{
	if (StartWrap() == false)
	{
		return;
	}
}

void CNetworkMgr::TestNetworkSendData()
{
	if (m_sptrNetWrapOper == nullptr)
	{
		return;
	}
	char buffer[PACKET_MAX_DATA_SIZE] = { 0 };
	snprintf(buffer, sizeof(buffer), "%s -> hello world!", m_sptrNetWrapOper->GetThreadFlag().c_str());


	struct packet_buffer data;
	data.header.identity = 335;
	data.header.command = NETWORK_EVENT_TEST;
	strncpy(data.buffer, buffer, strnlen(buffer, sizeof(buffer)));
	data.header.length = PACKET_HEADER_SIZE + strlen(data.buffer);
	int hsize = PACKET_HEADER_SIZE;
	int bsize = strlen(data.buffer);

	printf("\n\nCNetworkMgr::TestNetworkSendData - length:%d,hsize:%d,bsize:%d\n", data.header.length, hsize, bsize);

	std::shared_ptr<struct tagSendData> sptrData = std::make_shared<struct tagSendData>();
	memcpy(sptrData->buffer,&data, data.header.length);

	sptrData->length = data.header.length;
	m_sptrNetWrapOper->SendData(sptrData);
}

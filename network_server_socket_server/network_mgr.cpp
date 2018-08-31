
#include "network_mgr.h"

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetRequest()
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

bool CNetworkMgr::Connect()
{
	if (m_sptrNetTaskOper == nullptr)
	{
		return false;
	}
	bool flag = m_sptrNetTaskOper->Connect("127.0.0.1", PORT);
	if (flag == false)
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
	DispatchNetRequest();
}

// ---------------------------------------------------------------------------------------

void CNetworkMgr::TestClientSendData()
{
	char * buffer = (char *)malloc(1024);
	memset(buffer, 0, 1024);

	struct packet_header * ptrHeader = (struct packet_header *)buffer;
	ptrHeader->command = NETWORK_EVENT_TEST;
	ptrHeader->handler = 203;

	const char * ptrHello = "hello world tcp!";
	strcpy(buffer + PACKET_HEADER_SIZE, ptrHello);
	unsigned int size = PACKET_HEADER_SIZE + strlen(ptrHello);
	ptrHeader->length = (int)size;
	if (m_sptrNetTaskOper != nullptr)
	{
		m_sptrNetTaskOper->SendData((const void *)buffer, ptrHeader->length);
	}
}

#include "mysql_mgr.h"
#include <iomanip>


// ---------------------------------------------------------------------------------------

unsigned long long	CNetworkMgr::GetMillisecond()
{
	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;
	return millisecond;
}

std::string CNetworkMgr::FormatToString(const char* fmt, ...)
{
	va_list ap;
	char szBuffer[2048] = { 0 };
	va_start(ap, fmt);
	vsnprintf(szBuffer, sizeof(szBuffer), fmt, ap);
	va_end(ap);
	return szBuffer;
}

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetworkEvent()
{
	int count = 0;
	for (int i = 0; i < DB_INDEX_TYPE_MAX; ++i)
	{
		if (m_sptrDBAsyncOper[i] == nullptr)
		{
			continue;
		}
		do
		{
			auto sptrResponse = m_sptrDBAsyncOper[i]->GetAsyncExecuteResult();
			DispatchNetworkCallBack(std::move(sptrResponse));
			if (++count >= 128)
			{
				sptrResponse = nullptr;
				break;
			}
		} while (true);
	}
}

void CNetworkMgr::DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (m_sptrAsyncNetworkCallBack != nullptr)
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
	for (int i = 0; i < DB_INDEX_TYPE_MAX; ++i)
	{
		if (m_sptrDBAsyncOper[i] == nullptr)
		{
			continue;
		}
		m_sptrDBAsyncOper[i]->ShutDown();
		m_sptrDBAsyncOper[i].reset();
		m_sptrDBAsyncOper[i] = nullptr;
	}
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



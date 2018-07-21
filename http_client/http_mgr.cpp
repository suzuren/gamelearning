
#include "http_mgr.h"

bool CHttpMgr::Init()
{
	m_tagconnect.epfd = epoll_create(MAX_SOCKET_CONNECT);
	return true;
}

struct tagsocket_info * CHttpMgr::GetSocketTarget()
{
	struct tagsocket_info * psocket = NULL;
	if (m_vecsocket_storage.size() > 0)
	{
		psocket = m_vecsocket_storage[0];
		m_vecsocket_storage.erase(m_vecsocket_storage.begin());
	}
	if (psocket == NULL)
	{
		psocket = new tagsocket_info();
	}
	return psocket;
}

void CHttpMgr::AddSocketTarget(struct tagsocket_info * psocket)
{
	if (psocket != NULL)
	{
		psocket->Init();
		m_vecsocket_storage.push_back(psocket);
	}
}


struct tagsocket_info * CHttpMgr::AddSocketEpoll(int fd)
{
	SetSocketEvents(m_tagconnect.epfd, fd, EPOLL_CTL_ADD);
	struct tagsocket_info * psocket = GetSocketTarget();
	if (psocket == NULL)
	{
		return NULL;
	}
	psocket->Init();
	psocket->fd = fd;
	m_mpsocket.insert(std::make_pair(fd, psocket));
	return psocket;
}

bool CHttpMgr::SetSocketAStatus(int fd,int status)
{
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		iter->second->astatus = status;
	}
}

bool CHttpMgr::SetSocketCStatus(int fd, int status)
{
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		iter->second->cstatus = status;
	}
}

bool CHttpMgr::SetSocketRStatus(int fd, int status)
{
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		iter->second->rstatus = status;
	}
}

bool CHttpMgr::SetSocketWStatus(int fd, int status)
{
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		iter->second->wstatus = status;
	}
}


int CHttpMgr::GetSocketAStatus(int fd)
{
	int status = SOCKET_STATUS_INIT;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		status = iter->second->astatus;
	}
	return status;
}

int CHttpMgr::GetSocketCStatus(int fd)
{
	int status = SOCKET_STATUS_INIT;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		status = iter->second->cstatus;
	}
	return status;
}

int CHttpMgr::GetSocketRStatus(int fd)
{
	int status = SOCKET_STATUS_INIT;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		status = iter->second->rstatus;
	}
	return status;
}

int CHttpMgr::GetSocketWStatus(int fd)
{
	int status = SOCKET_STATUS_INIT;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		status = iter->second->wstatus;
	}
	return status;
}

int CHttpMgr::AnalysisSocketData(int fd)
{
	int rlen = 0;
	char * ptrbuf = NULL;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		ptrbuf = iter->second->rbuffer;
		rlen = iter->second->rlen;
	}
	else
	{
		return -1;
	}
	if (ptrbuf == NULL || rlen == 0)
	{
		return -1;
	}

	std::string response;
	response.append(ptrbuf, rlen);
	printf("AnalysisSocketData - \n\nresponse:\n%s\n", response.c_str());
	std::string temp = response;
	std::string::size_type pos_http_ver = temp.find("\r\n");
	std::string str_http_line;
	std::string str_http_body;
	if (std::string::npos != pos_http_ver)
	{
		str_http_line = temp.substr(0, pos_http_ver);
	}
	else
	{
		printf("AnalysisSocketData - 1\n");
		return -1;
	}

	bool bLineCompare = (str_http_line == "HTTP/1.1 200 ");
	if (bLineCompare)
	{
		std::string::size_type pos_http_body = temp.find("\r\n\r\n");
		str_http_body = temp.substr(pos_http_body + 4, std::string::npos);
		printf("AnalysisSocketData - \n\nstr_http_body:%d-%s", str_http_body.size(), str_http_body.c_str());
		//SetSocketRStatus(fd, SOCKET_STATUS_READED);
		SetSocketAStatus(fd, SOCKET_STATUS_CLOSE);		
	}
	else
	{
		printf("AnalysisSocketData - 2 bLineCompare:%d,str_http_line:%s-\n", bLineCompare, str_http_line.c_str());
		return -1;
	}
	return 1;
}

int CHttpMgr::ReadSocketData(int fd)
{
	int rlen = 0;
	char * ptrbuf = NULL;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		ptrbuf = iter->second->rbuffer;
		rlen = iter->second->rlen;
	}
	else
	{
		return -1;
	}

	int nread = read(fd, ptrbuf+ rlen, SOCKET_TCP_BUFFER - rlen);
	if (nread == 0)
	{
		SetSocketRStatus(fd, SOCKET_STATUS_READED);
		//close(client_fd);
		return -1;
	}
	else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
	{
		return -1;
	}
	else
	{
		if (iter != m_mpsocket.end())
		{
			SetSocketRStatus(fd, SOCKET_STATUS_READING);
			(iter->second->rlen) += nread;
			printf("ReadSocketData - fd:%d,nread:%d,rlen:%d,ptrbuf:\n%s\n",
				fd, nread, iter->second->rlen, ptrbuf);
		}
	}
	return nread;
}

int CHttpMgr::WriteSocketData(int fd)
{
	int wlen = 0;
	int flen = 0;
	char * ptrbuf = NULL;
	auto iter = m_mpsocket.find(fd);
	if (iter != m_mpsocket.end())
	{
		ptrbuf = iter->second->wbuffer;
		wlen = iter->second->wlen;
		flen = iter->second->flen;
	}
	else
	{
		return -1;
	}
	int nwrite = write(fd, ptrbuf + flen, wlen - flen);
	if (nwrite>0 && iter != m_mpsocket.end())
	{
		(iter->second->flen) += nwrite;
		SetSocketWStatus(fd, SOCKET_STATUS_WRITEING);
	}
	if (flen>0 && wlen>0 && flen == wlen)
	{
		SetSocketWStatus(fd, SOCKET_STATUS_WRITEED);
		//SetSocketRStatus(fd, SOCKET_STATUS_READ);
		printf("WriteSocketData - fd:%d,wlen:%d,ptrbuf:\n%s\n\n", fd, wlen, ptrbuf);
	}
	return nwrite;
}

int	CHttpMgr::PostData(const char * api, const char * body)
{
	if (api == NULL || body == NULL)
	{
		return -1;
	}
	int client_fd = -1;
	int ret = socket_connect("47.94.250.154", 38018, &client_fd);
	if (ret == -1)
	{
		return -1;
	}
	struct tagsocket_info * psocket = AddSocketEpoll(client_fd);
	if (psocket == NULL)
	{
		return -1;
	}
	char * prtbuffer = psocket->wbuffer;
	sprintf(prtbuffer, "%s", http_build_post_head(api, body));
	psocket->wlen = strlen(prtbuffer);
	if (ret == 1)
	{
		SetSocketCStatus(client_fd, SOCKET_STATUS_CONNECTED);
		SetSocketWStatus(client_fd, SOCKET_STATUS_WRITE);
	}
	else if(ret == 0)
	{
		SetSocketCStatus(client_fd, SOCKET_STATUS_CONNECTING);
	}
	SetSocketAStatus(client_fd, SOCKET_STATUS_WAIT);

	//printf("client_fd:%d,status:%d,prtbuffer:\n%s\n\n", client_fd, psocket->status,prtbuffer);
	return 1;
}

void CHttpMgr::UpdateSocketStatus()
{
	m_tagconnect.reevents();
	int nfds = epoll_wait(m_tagconnect.epfd, m_tagconnect.events, MAX_SOCKET_CONNECT, -1);
	//printf("UpdateSocketStatus - nfds:%d\n", nfds);

	if (nfds <= 0)
	{
		return;
	}
	for (int i = 0; i < nfds; ++i)
	{
		int fd = m_tagconnect.events[i].data.fd;
		unsigned int events = m_tagconnect.events[i].events;
		int cstatus = GetSocketCStatus(fd);
		if (events & EPOLLIN)
		{
			SetSocketRStatus(fd, SOCKET_STATUS_READ);
			int rstatus = GetSocketRStatus(fd);
			printf("UpdateSocketStatus - EPOLLIN fd:%d,rstatus:%d,errno:%d\n", fd, rstatus, errno);
		}
		else if (events & EPOLLET)
		{
			//printf("UpdateSocketStatus - EPOLLET fd:%d,errno:%d\n", fd, errno);
		}
		else if (events & EPOLLERR)
		{
			//printf("UpdateSocketStatus - EPOLLERR fd:%d,errno:%d\n", fd, errno);
			SetSocketAStatus(fd, SOCKET_STATUS_ERROR);
		}
		else if (events & EPOLLOUT)
		{
			printf("UpdateSocketStatus - nfds:%d,fd:%d,cstatus:%d\n", nfds, fd, cstatus);
			if (cstatus == SOCKET_STATUS_CONNECTING)
			{
				SetSocketCStatus(fd, SOCKET_STATUS_CONNECTED);
				SetSocketWStatus(fd, SOCKET_STATUS_WRITE);
			}
		}
		else
		{
			SetSocketAStatus(fd, SOCKET_STATUS_CONNECT_FAILURE);
		}
	}
}

void CHttpMgr::UpdateSocketData()
{
	std::vector<struct tagsocket_info *> vecsocket;
	auto iter = m_mpsocket.begin();
	for (; iter != m_mpsocket.end(); iter++)
	{
		int fd = iter->second->fd;

		int astatus = iter->second->astatus;
		int cstatus = iter->second->cstatus;
		int rstatus = iter->second->rstatus;
		int wstatus = iter->second->wstatus;

		int aastatus = GetSocketAStatus(fd);
		int ccstatus = GetSocketAStatus(fd);
		int rrstatus = GetSocketAStatus(fd);
		int wwstatus = GetSocketAStatus(fd);

		printf("UpdateSocketData - fd:%d,astatus:%d,cstatus,rstatus:%d,wstatus:%d,aastatus:%d,ccstatus,rrstatus:%d,wwstatus:%d\n",
			fd, astatus, cstatus, rstatus, wstatus, aastatus, ccstatus, rrstatus, wwstatus);
		if (wstatus == SOCKET_STATUS_WRITE || wstatus == SOCKET_STATUS_WRITEING)
		{
			WriteSocketData(fd);
		}
		else if (rstatus == SOCKET_STATUS_READ || rstatus == SOCKET_STATUS_READING)
		{
			ReadSocketData(fd);
		}
		else if (rstatus == SOCKET_STATUS_READ || rstatus == SOCKET_STATUS_READING)
		{
			AnalysisSocketData(fd);
		}
		else if (astatus == SOCKET_STATUS_CLOSE || astatus == SOCKET_STATUS_CONNECT_FAILURE || astatus == SOCKET_STATUS_ERROR)
		{
			vecsocket.push_back(iter->second);
		}
	}
	for (unsigned int index = 0; index < vecsocket.size(); index++)
	{
		struct tagsocket_info * psocket = vecsocket[index];
		m_mpsocket.erase(psocket->fd);
		close(psocket->fd);
		AddSocketTarget(psocket);
	}
}

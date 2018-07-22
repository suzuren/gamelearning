
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
	bool flag = SetSocketEvents(m_tagconnect.epfd, fd, EPOLL_CTL_ADD);
	struct tagsocket_info * psocket = GetSocketTarget();
	if (psocket == NULL || flag == false)
	{
		close(fd);
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

void CHttpMgr::response_parser(std::string & strdata, std::string & str_content)
{
	str_content.clear();
	std::string str_parser = strdata;
	if (http_body_is_final(str_parser))
	{
		do
		{
			std::string::size_type pos_chunked_size = str_parser.find("\r\n");
			std::string str_chunked_size = str_parser.substr(0, pos_chunked_size);
			if (str_chunked_size.size() > 64)
			{
				break;
			}
			char buffer[64] = { 0 };
			sprintf(buffer, "%s", str_chunked_size.c_str());
			int chunked_size = strtol(buffer, NULL, 16);
			if (chunked_size == 0)
			{
				break;
			}
			str_content += str_parser.substr(pos_chunked_size + 2, chunked_size);
			str_parser = str_parser.substr(pos_chunked_size + 2 + chunked_size + 2, std::string::npos);
		} while (true);
	}
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

	std::string http_response;
	http_response.append(ptrbuf, rlen);
	//printf("AnalysisSocketData - \n\nresponse:\n%s\n", http_response.c_str());

	std::string::size_type pos_http_state_code = http_response.find("HTTP/1.1 200 \r\n");
	std::string::size_type pos_http_encode = http_response.find("Transfer-Encoding: chunked\r\n");

	//printf("AnalysisSocketData pos - size:%d,npos:%d,code:%d,encode:%d\n",str_temp.size(), std::string::npos, pos_http_state_code, pos_http_encode);

	if (pos_http_state_code != std::string::npos && pos_http_encode != std::string::npos)
	{
		std::string str_check_body = http_response;
		std::string::size_type pos_http_body_begin = str_check_body.find("\r\n\r\n");
		std::string::size_type pos_http_body_end = str_check_body.find("\r\n0\r\n\r\n");
		if(pos_http_body_begin != std::string::npos && pos_http_body_end != std::string::npos)
		{
			//str_http_body = str_temp.substr(pos_http_body + 4, std::string::npos);
			str_check_body = str_check_body.substr(pos_http_body_begin+4, pos_http_body_end);

			response_parser(str_check_body, iter->second->content);

			SetSocketRStatus(fd, SOCKET_STATUS_READED);
			SetSocketAStatus(fd, SOCKET_STATUS_CLOSE);

			printf("AnalysisSocketData - \n\nhttp_response:-%s-\n", http_response.c_str());
			printf("AnalysisSocketData - \n\nhttp_content:-%s-\n", iter->second->content.c_str());

			return 1;
		}		
		//printf("AnalysisSocketData - pos begin:%d,end:%d, \n\nstr_http_body:%d-%s-", pos_http_body_begin, pos_http_body_end, str_http_body.size(), str_http_body.c_str());
	}
	else if (pos_http_state_code != std::string::npos && pos_http_encode == std::string::npos)
	{

	}
	else
	{
		//printf("AnalysisSocketData - 2 bLineCompare:%d,str_http_line:%s-\n", bLineCompare, str_http_line.c_str());
		return -1;
	}
	return 0;
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
		printf("1 ReadSocketData - nread:%d,errno:%d\n", nread, errno);
		//close(client_fd);
		return -1;
	}
	else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
	{
		printf("2 ReadSocketData - nread:%d,errno:%d\n", nread, errno);
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
		
	}
	int wstatus = GetSocketWStatus(fd);
	printf("WriteSocketData - fd:%d,wstatus:%d,wlen:%d,flen:%d,ptrbuf:\n-%s-\n\n", fd, wstatus, wlen, flen, ptrbuf);
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
	//printf("1111111111111111111111111\n");
	int nfds = epoll_wait(m_tagconnect.epfd, m_tagconnect.events, MAX_SOCKET_CONNECT, 0);
	//printf("2222222222222222222222222222\n");
	//printf("UpdateSocketStatus - nfds:%d\n", nfds);

	if (nfds <= 0)
	{
		return;
	}
	for (int i = 0; i < nfds; ++i)
	{
		int fd = m_tagconnect.events[i].data.fd;
		unsigned int events = m_tagconnect.events[i].events;
		int astatus = GetSocketAStatus(fd);
		int cstatus = GetSocketCStatus(fd);
		int rstatus = GetSocketRStatus(fd);
		int wstatus = GetSocketWStatus(fd);
		printf("UpdateSocketStatus - fd:%d,astatus:%d,cstatus:%d,rstatus:%d,wstatus:%d,events:%d\n",
			fd, astatus, cstatus, rstatus, wstatus, events);
		if (events & EPOLLIN)
		{
			SetSocketRStatus(fd, SOCKET_STATUS_READ);
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
		
		//printf("UpdateSocketData - fd:%d,astatus:%d,cstatus:%d,rstatus:%d,wstatus:%d\n",fd, astatus, cstatus, rstatus, wstatus);

		if (wstatus == SOCKET_STATUS_WRITE || wstatus == SOCKET_STATUS_WRITEING)
		{
			WriteSocketData(fd);
		}
		if (rstatus == SOCKET_STATUS_READ || rstatus == SOCKET_STATUS_READING)
		{
			ReadSocketData(fd);
		}
		if (rstatus == SOCKET_STATUS_READ || rstatus == SOCKET_STATUS_READING)
		{
			printf("111111111111111111111\n");
			AnalysisSocketData(fd);
			printf("2222222222222222222222\n");
		}
		if (astatus == SOCKET_STATUS_CLOSE || astatus == SOCKET_STATUS_CONNECT_FAILURE || astatus == SOCKET_STATUS_ERROR)
		{
			vecsocket.push_back(iter->second);
		}
	}
	for (unsigned int index = 0; index < vecsocket.size(); index++)
	{
		struct tagsocket_info * psocket = vecsocket[index];
		int fd = psocket->fd;
		m_mpsocket.erase(fd);
		SetSocketEvents(m_tagconnect.epfd, fd, EPOLL_CTL_DEL);
		close(fd);
		AddSocketTarget(psocket);
	}
}



#include "logger.h"

//------------------------------------------------------------------------------


#define OPEN_MODE_READ					("rb")
#define OPEN_MODE_WRITE					("wb")
#define OPEN_MODE_READ_WRITE			("rb+")
#define OPEN_MODE_APPEND				("ab+")

#define MAX_EACH_LINE_WORD				(1024*512)



struct log_queue_node
{
	int len;
	std::string data;
	struct log_queue_node * pnext;
};

struct log_info
{
	pthread_mutex_t	   time_mutex;
	time_t		sec_time;
	struct tm	tag_time;

	char	curdir[256];
	char    prename[32];	// file name prefix
	char    filename[32];	// file name

	FILE *  pstream;
	int     curday;
	int		retainday;
	int     cursize;   // current file size
	int     maxsize;   // max file size
	int		backcount; // backup files count

	int		queuesize;
	pthread_mutex_t	   queue_mutex;
	struct log_queue_node * pqueue_head;
	struct log_queue_node * pqueue_tail;
	std::vector<struct log_queue_node *> vecnode;

	pthread_t pthread;
	int		runthread;
};

static struct log_info * _LOGGER = NULL;

//------------------------------------------------------------------------------

FILE * file_open(const char * pname, const char * pmode)
{
	if (pname == NULL || pmode == NULL)
	{
		return NULL;
	}
	FILE * pstream = fopen(pname, pmode);
	return pstream;
}

void file_close(FILE ** pstream)
{
	if (*pstream != NULL)
	{
		fclose(*pstream);
	}
	*pstream = NULL;
}

void file_flush(FILE * pstream)
{
	if (pstream != NULL)
	{
		fflush(pstream);
	}
}

int	file_seek(FILE * pstream,int offset, int fromwhere)
{
	if (pstream == NULL)
	{
		return -1;
	}
	return fseek(pstream, offset, fromwhere);
}

int file_size(FILE * pstream)
{
	if (pstream == NULL)
	{
		return 0;
	}

	int itell = ftell(pstream);
	int ret_seek = file_seek(pstream,0, SEEK_END);
	if (ret_seek != 0)
	{
		file_seek(pstream, itell, SEEK_SET);
		return 0;
	}
	int isize = ftell(pstream);
	file_seek(pstream, itell, SEEK_SET);
	return isize;
}


int file_write_buffer(FILE * pstream,unsigned int count,const void * buffer)
{
	if (pstream == NULL || count == 0 || buffer == NULL)
	{
		return -1;
	}
	if (fwrite(buffer, 1, count, pstream) == count)
	{
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------------

void log_time_init(struct tm & tag_time)
{
	tag_time.tm_sec = 0;
	tag_time.tm_min = 0;
	tag_time.tm_hour = 0;
	tag_time.tm_mday = 0;
	tag_time.tm_mon = 0;
	tag_time.tm_year = 0;
	tag_time.tm_wday = 0;
	tag_time.tm_yday = 0;
	tag_time.tm_isdst = 0;
}

void log_time_update(struct tm tag_time, time_t sec_time)
{
	if (_LOGGER == NULL)
	{
		return;
	}
	pthread_mutex_lock(&_LOGGER->time_mutex);
	_LOGGER->tag_time.tm_sec = tag_time.tm_sec;
	_LOGGER->tag_time.tm_min = tag_time.tm_min;
	_LOGGER->tag_time.tm_hour = tag_time.tm_hour;
	_LOGGER->tag_time.tm_mday = tag_time.tm_mday;
	_LOGGER->tag_time.tm_mon = tag_time.tm_mon;
	_LOGGER->tag_time.tm_year = tag_time.tm_year;
	_LOGGER->tag_time.tm_wday = tag_time.tm_wday;
	_LOGGER->tag_time.tm_yday = tag_time.tm_yday;
	_LOGGER->tag_time.tm_isdst = tag_time.tm_isdst;

	_LOGGER->sec_time = sec_time;
	pthread_mutex_unlock(&_LOGGER->time_mutex);
}

void log_time_get_tag(struct tm & tag_time)
{
	if (_LOGGER == NULL)
	{
		return;
	}
	pthread_mutex_lock(&_LOGGER->time_mutex);
	tag_time.tm_sec = _LOGGER->tag_time.tm_sec;
	tag_time.tm_min = _LOGGER->tag_time.tm_min;
	tag_time.tm_hour = _LOGGER->tag_time.tm_hour;
	tag_time.tm_mday = _LOGGER->tag_time.tm_mday;
	tag_time.tm_mon = _LOGGER->tag_time.tm_mon;
	tag_time.tm_year = _LOGGER->tag_time.tm_year;
	tag_time.tm_wday = _LOGGER->tag_time.tm_wday;
	tag_time.tm_yday = _LOGGER->tag_time.tm_yday;
	tag_time.tm_isdst = _LOGGER->tag_time.tm_isdst;
	pthread_mutex_unlock(&_LOGGER->time_mutex);
}

void log_time_update_logger()
{
	time_t sec_time = time(NULL);
	struct tm * ptime = localtime(&sec_time);
	struct tm tag_time;
	tag_time.tm_sec = ptime->tm_sec;
	tag_time.tm_min = ptime->tm_min;
	tag_time.tm_hour = ptime->tm_hour;
	tag_time.tm_mday = ptime->tm_mday;
	tag_time.tm_mon = ptime->tm_mon;
	tag_time.tm_year = ptime->tm_year;
	tag_time.tm_wday = ptime->tm_wday;
	tag_time.tm_yday = ptime->tm_yday;
	tag_time.tm_isdst = ptime->tm_isdst;

	log_time_update(tag_time, sec_time);
}

//------------------------------------------------------------------------------
struct log_queue_node * queue_get_node(struct log_info * pinfo)
{
	struct log_queue_node * pnode = NULL;
	if (pinfo != NULL)
	{
		if (pinfo->vecnode.size() > 0)
		{
			pnode = pinfo->vecnode[0];
			pinfo->vecnode.erase(pinfo->vecnode.begin());
		}
		else
		{
			pnode = new(struct log_queue_node);
			pnode->len = 0;
			pnode->pnext = NULL;
			pnode->data.clear();
		}
	}
	return pnode;
}

void queue_set_node(struct log_info * pinfo,struct log_queue_node * pnode)
{
	if (pinfo != NULL && pnode != NULL)
	{
		pnode->len = 0;
		pnode->pnext = NULL;
		pnode->data.clear();
		pinfo->vecnode.push_back(pnode);
	}
}

void queue_push(struct log_info * pinfo,const void * buffer, int size)
{
	pthread_mutex_lock(&pinfo->queue_mutex);
	if (pinfo->runthread != 1)
	{
		pthread_mutex_unlock(&pinfo->queue_mutex);
		return;
	}
	struct log_queue_node * pnode = queue_get_node(pinfo);
	if (pnode == NULL)
	{
		pthread_mutex_unlock(&pinfo->queue_mutex);
		return;
	}
	pnode->len = size;
	pnode->pnext = NULL;
	pnode->data.append((const char *)buffer, size);
	
	int * queuesize = &pinfo->queuesize;
	struct log_queue_node * * head = &pinfo->pqueue_head;
	struct log_queue_node * * tail = &pinfo->pqueue_tail;

	if ((*head) == NULL && (*tail) == NULL)
	{
		*head = pnode;
		*tail = pnode;
	}
	else
	{
		(*tail)->pnext = pnode;
		*tail = pnode;
	}
	(*queuesize)++;

	pthread_mutex_unlock(&pinfo->queue_mutex);
}

int queue_pop(struct log_info * pinfo, std::string & data, int * size)
{
	data.clear();
	pthread_mutex_lock(&pinfo->queue_mutex);
	int * queuesize = &pinfo->queuesize;
	struct log_queue_node * * head = &pinfo->pqueue_head;
	struct log_queue_node * * tail = &pinfo->pqueue_tail;

	int ret = 0;
	struct log_queue_node * pnode = (*head);
	if (pnode != NULL)
	{
		*head = pnode->pnext;
		if (pnode->pnext == NULL)
		{
			*tail = pnode->pnext;
		}
		data = pnode->data;
		*size = pnode->len;
		(*queuesize)--;
		queue_set_node(pinfo, pnode);
		ret = 1;
	}
	else
	{
		//empty queue
		*size = 0;
		ret = -1;
	}
	pthread_mutex_unlock(&pinfo->queue_mutex);

	return ret;
}


int queue_get_size(struct log_info * pinfo)
{
	int size = 0;
	pthread_mutex_lock(&pinfo->queue_mutex);
	size = pinfo->queuesize;
	pthread_mutex_unlock(&pinfo->queue_mutex);
	return size;
}

void queue_clear(struct log_info * pinfo)
{
	struct log_queue_node * heard = pinfo->pqueue_head;
	while (heard != NULL)
	{
		if (heard->len > 0)
		{
			file_write_buffer(pinfo->pstream, heard->data.size(), heard->data.c_str());
			file_flush(pinfo->pstream);
			pinfo->cursize += (heard->data.size());			
			struct log_queue_node * pnode = heard;
			heard = heard->pnext;
			queue_set_node(pinfo, pnode);
			
		}
	}
	std::string data = "logger thread exit\r\n";
	file_write_buffer(pinfo->pstream, data.size(), data.c_str());
	file_flush(pinfo->pstream);
	pinfo->cursize += data.size();
	while (pinfo->vecnode.size()>0)
	{
		struct log_queue_node * pnode = pinfo->vecnode[0];
		pinfo->vecnode.erase(pinfo->vecnode.begin());
		if (pnode != NULL)
		{
			delete pnode;
			pnode = NULL;
		}
	}
}

//------------------------------------------------------------------------------

void log_file_check(struct log_info * pinfo)
{
	static char buffer_dir_new[32];
	static char buffer_dir_mk[512];
	static char buffer_dir_cur[32];
	static char buffer_dir_ago[32];
	static char buffer_dir_rm[512];
	static char buffer_file_open[512];
	static char buffer_file_back[512];
	static char buffer_file_ago[512];

	struct tm tag_time;
	log_time_get_tag(tag_time);	
	if (pinfo->curday != tag_time.tm_mday)
	{
		//make new dir
		memset(buffer_dir_new, 0, sizeof(buffer_dir_new));
		memset(buffer_dir_mk, 0, sizeof(buffer_dir_mk));
		sprintf(buffer_dir_new, "logs_%04d_%02d_%02d/", tag_time.tm_year + 1900, tag_time.tm_mon + 1, tag_time.tm_mday);
		sprintf(buffer_dir_mk, "%s%s", pinfo->curdir, buffer_dir_new);
		if (mkdir(buffer_dir_mk, 0777) != 0)
		{
			//printf("\r\nfailed - buffer_dir_mk:%s\r\n", buffer_dir_mk);
		}
		else
		{
			//printf("\r\nsuccess - buffer_dir_mk:%s\r\n", buffer_dir_mk);
		}
		memset(buffer_file_open, 0, sizeof(buffer_file_open));

		pinfo->cursize = 0;
		pinfo->backcount = 0;
		pinfo->curday = tag_time.tm_mday;
		
		memset(pinfo->filename, 0, sizeof(pinfo->filename));
		sprintf(pinfo->filename, "%s.log", pinfo->prename);
		sprintf(buffer_file_open, "%s%s%s", pinfo->curdir, buffer_dir_new, pinfo->filename);
		file_close(&pinfo->pstream);
		pinfo->pstream = file_open(buffer_file_open, OPEN_MODE_APPEND);
		pinfo->cursize = file_size(pinfo->pstream);
		file_seek(pinfo->pstream, 0, SEEK_END);
		//printf("1 pstream:%p,cursize:%d,buffer_file_open:%s\r\n", pinfo->pstream,pinfo->cursize, buffer_file_open);
	}
	if (pinfo->cursize >= pinfo->maxsize)
	{
		file_close(&pinfo->pstream);
		
		memset(buffer_dir_cur, 0, sizeof(buffer_dir_cur));
		memset(buffer_file_open, 0, sizeof(buffer_file_open));
		memset(buffer_file_back, 0, sizeof(buffer_file_back));

		sprintf(buffer_dir_cur, "logs_%04d_%02d_%02d/", tag_time.tm_year + 1900, tag_time.tm_mon + 1, tag_time.tm_mday);

		sprintf(buffer_file_open, "%s%s%s", pinfo->curdir, buffer_dir_cur, pinfo->filename);
		sprintf(buffer_file_back, "%s%s%s_back_%d.log", pinfo->curdir, buffer_dir_cur, pinfo->prename, pinfo->backcount);
		// rename
		rename(buffer_file_open, buffer_file_back);
		pinfo->pstream = file_open(buffer_file_open, OPEN_MODE_APPEND);
		file_seek(pinfo->pstream, 0, SEEK_END);
		pinfo->cursize = 0;
		pinfo->backcount++;
		//printf("2 pstream:%p,buffer_file_open:%s\r\n", pinfo->pstream, buffer_file_open);

		//printf("buffer_file_open:%s\r\n", buffer_file_open);
		//printf("buffer_file_back:%s\r\n", buffer_file_back);
	}
	for (int index = 0; index < 7; ++index)
	{
		unsigned long long agoday = pinfo->retainday + index;
		time_t time_ago = pinfo->sec_time - (agoday * 60 * 60 * 24);
		struct tm * ptime_ago = localtime_r(&time_ago, &tag_time);

		memset(buffer_dir_ago, 0, sizeof(buffer_dir_ago));
		memset(buffer_file_ago, 0, sizeof(buffer_file_ago));
		sprintf(buffer_dir_ago, "logs_%04d_%02d_%02d/", ptime_ago->tm_year + 1900, ptime_ago->tm_mon + 1, ptime_ago->tm_mday);
		sprintf(buffer_file_ago, "%s%s%s", pinfo->curdir, buffer_dir_ago, pinfo->filename);
		
		if (access(buffer_file_ago, F_OK) == 0)
		{
			//printf("3 buffer_file_ago:%s\r\n", buffer_file_ago);
			unlink(buffer_file_ago);
		}
		int backcount = 0;
		do
		{
			memset(buffer_file_ago, 0, sizeof(buffer_file_ago));
			sprintf(buffer_file_ago, "%s%s%s_back_%d.log", pinfo->curdir, buffer_dir_ago, pinfo->prename, backcount);
			if (access(buffer_file_ago, F_OK) == 0)
			{
				//printf("buffer_file_ago:%s\r\n", buffer_file_ago);
				unlink(buffer_file_ago);
			}
			else
			{
				break;
			}
			backcount++;
		} while (1);
		memset(buffer_dir_rm, 0, sizeof(buffer_dir_rm));
		sprintf(buffer_dir_rm, "%s%s", pinfo->curdir, buffer_dir_ago);		
		if (rmdir(buffer_dir_rm) == 0)
		{
			//printf("4 buffer_dir_rm:%s\r\n", buffer_dir_rm);
		}
	}	
}

void log_thread_sleep(unsigned int msec)
{
	struct timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}

void log_write_queue_data(struct log_info * pinfo)
{
	while (pinfo->runthread == 1)
	{
		if (pinfo != NULL && queue_get_size(pinfo) > 0)
		{
			log_file_check(pinfo);

			int len = -2;
			static std::string data;
			int ret = queue_pop(pinfo, data, &len);
			if (ret == 1)
			{
				file_write_buffer(pinfo->pstream, data.size(), data.c_str());
				file_flush(pinfo->pstream);
				pinfo->cursize += data.size();
			}
			else
			{
				log_thread_sleep(LOG_BREATHING_SPACE);
			}
		}
		else
		{
			log_thread_sleep(LOG_BREATHING_SPACE);
		}
	}
}


void log_destroy_logger()
{
	struct log_info * pinfo = _LOGGER;
	if (pinfo != NULL)
	{
		pinfo->runthread = 0;
		queue_clear(pinfo);
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pthread_mutex_destroy(&(pinfo->queue_mutex));
		pthread_join(pinfo->pthread, NULL);
		pthread_cancel(pinfo->pthread);
		file_close(&pinfo->pstream);
		delete(pinfo);
		_LOGGER = NULL;
	}
}


void* runthreadfunc(void* parm)
{
	struct log_info * pinfo = (struct log_info *)parm;
	log_file_check(pinfo);
	log_write_queue_data(pinfo);
	//printf("thread exit\n");
	log_destroy_logger();
	return NULL;
}

//------------------------------------------------------------------------------

const char* log_get_time()
{
	struct tm tag_time;
	log_time_get_tag(tag_time);
	static char cdate[32];
	memset(cdate, 0, sizeof(cdate));
	sprintf(cdate, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]", tag_time.tm_year + 1900, tag_time.tm_mon + 1, tag_time.tm_mday, tag_time.tm_hour, tag_time.tm_min, tag_time.tm_sec);
	return cdate;
}

void log_get_curdir(char * pbuf)
{
	int count = readlink("/proc/self/exe", pbuf, 256);
	if (count != -1)
	{
		char * p = strrchr(pbuf, '/');
		if (p != NULL)
		{
			*(p + 1) = 0;
		}
	}
}

void log_init_info(struct log_info * pinfo)
{
	if (pinfo != NULL)
	{
		pthread_mutex_init(&pinfo->time_mutex, NULL);
		pinfo->sec_time = 0;
		log_time_init(pinfo->tag_time);

		log_time_update_logger();		

		log_get_curdir(pinfo->curdir);
		memset(pinfo->prename, 0, sizeof(pinfo->prename));
		sprintf(pinfo->prename, "temp");
		memset(pinfo->filename, 0, sizeof(pinfo->filename));	

		pinfo->pstream = NULL;
		pinfo->curday = -1;
		pinfo->retainday = 5;	//retain 5 day
		pinfo->cursize = 0;
		pinfo->maxsize = 1024 * 1024 * 50; // 50M
		pinfo->backcount = 0;

		pinfo->queuesize = 0;
		pthread_mutex_init(&pinfo->queue_mutex, NULL);
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pinfo->vecnode.clear();

		pthread_create(&pinfo->pthread, 0, runthreadfunc, pinfo);
		pinfo->runthread = 1;
	}
}

struct log_info * log_create_logger()
{
	struct log_info * pinfo = new(struct log_info);
	return pinfo;
}

void log_constructor_logger(const char * prename)
{
	if (_LOGGER == NULL){
		_LOGGER = log_create_logger();
		log_init_info(_LOGGER);
	}
	if (prename != NULL){
		memset(_LOGGER->prename, 0, sizeof(_LOGGER->prename));
		sprintf(_LOGGER->prename, "%s", prename);
	}
}

void log_shutdown_logger()
{
	struct log_info * pinfo = _LOGGER;
	if (pinfo != NULL)
	{
		__sync_add_and_fetch(&(pinfo->runthread), -1);
	}
}

//------------------------------------------------------------------------------


const char* log_set_header(int level, const char *  pfunc, const char *  pfile, unsigned int line)
{
	static const char levelname[LOG_LEVEL_CRITIC + 1][16] = { " [debug] "," [warning] "," [normal] "," [error] "," [critic] " };	
	static char cheader[1024] = { 0 };
	memset(cheader, 0, sizeof(cheader));

	std::string str_file;
	std::string str_tempfile = pfile;
	std::string::size_type pos = str_tempfile.rfind('/');
	if (pos != std::string::npos){
		str_file = str_tempfile.substr(pos + 1);
	}
	else{
		str_file = str_tempfile;
	}
	sprintf(cheader, "%s%s[%s:%d][%s]", log_get_time(), levelname[level], str_file.c_str(), line, pfunc);
	return cheader;
}

int log_write(const void * buffer, int len)
{
	if (_LOGGER == NULL)
	{
		return 0;
		//printf("_LOGGER is null - len:%d,buffer:%s", len, buffer);
		_LOGGER = log_create_logger();
	}
	queue_push(_LOGGER, buffer, len);
	return 1;
}

void log_format_write(int level, const char * pfunc, const char * pfile, unsigned int line, char const* pformat, ...)
{
	static char  buffer[MAX_EACH_LINE_WORD];
	memset(buffer, 0, sizeof(buffer));
	const char * pheader = log_set_header(level, pfunc, pfile, line);
	sprintf(buffer, "%s [", pheader);	
	unsigned int usedlen = strlen(buffer);
	va_list ap;
	va_start(ap, pformat);
	vsnprintf(&buffer[usedlen], MAX_EACH_LINE_WORD - usedlen, pformat, ap);
	va_end(ap);
	usedlen = strlen(buffer);
	if (usedlen + 4 >= MAX_EACH_LINE_WORD)
	{
		return;
	}
	sprintf(&buffer[usedlen], "]\r\n");
	usedlen += 3;

	log_write(buffer, usedlen);
}

//------------------------------------------------------------------------------



#include "logger.h"

//------------------------------------------------------------------------------


#define OPEN_MODE_READ					"rb"
#define OPEN_MODE_WRITE					"wb"
#define OPEN_MODE_READ_WRITE			"rb+"
#define OPEN_MODE_APPEND				"ab+"

struct log_queue_node
{
	int len;
	std::string data;
	struct log_queue_node * pnext;
};

struct log_info
{
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
	pthread_mutex_t	   mutex;
	struct log_queue_node * pqueue_head;
	struct log_queue_node * pqueue_tail;
	pthread_t pthread;
	int		runthread;
	std::vector<struct log_queue_node *> vecnode;
};

static struct log_info * _LOGGER = NULL;

//------------------------------------------------------------------------------

void test_file_stream()
{
	const char * pname = "temp.log";
	FILE * pstream = file_open(pname, OPEN_MODE_APPEND);
	char buffer[1024] = { 0 };
	sprintf(buffer, "hello world.");
	int ret_seek = file_seek(pstream, 8, SEEK_SET);
	int ret_write = file_write_buffer(pstream, strlen(buffer), buffer);
	file_flush(pstream);
	int ret_size = file_size(pstream);
	file_close(&pstream);
	printf("file stream - ret_seek:%d,ret_write:%d,ret_size:%d\n", ret_seek, ret_write, ret_size);
}

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
	//确保写入到文件-不一定是磁盘但是别处可以读取
	if (pstream != NULL)
	{
		fflush(pstream);
	}
}

//|SEEK_SET | SEEK_CUR		| SEEK_END	|
//|文件开头 | 当前读写的位置| 文件尾	|
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

	int itell = ftell(pstream); // 记录当前文件的位置
	int ret_seek = file_seek(pstream,0, SEEK_END);
	if (ret_seek != 0)
	{
		file_seek(pstream, itell, SEEK_SET);
		return 0;
	}
	int isize = ftell(pstream); // 文件位置指针当前位置相对于文件首的偏移字节数
	file_seek(pstream, itell, SEEK_SET); // 恢复当前文件位置
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

struct log_queue_node * queue_get_node(struct log_info * pinfo)
{
	struct log_queue_node * pnode = NULL;
	//printf("queue_get_node - vecnode.size:%ld\n", pinfo->vecnode.size());
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
	pthread_mutex_lock(&pinfo->mutex);

	struct log_queue_node * pnode = queue_get_node(pinfo);
	if (pnode == NULL)
	{
		pthread_mutex_unlock(&pinfo->mutex);
		return;
	}
	pnode->len = size;
	pnode->pnext = NULL;
	pnode->data = (char *)buffer;
	
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

	pthread_mutex_unlock(&pinfo->mutex);
}

int queue_pop(struct log_info * pinfo, std::string & data, int * size)
{
	data.clear();

	pthread_mutex_lock(&pinfo->mutex);
	int * queuesize = &pinfo->queuesize;
	struct log_queue_node * * head = &pinfo->pqueue_head;
	struct log_queue_node * * tail = &pinfo->pqueue_tail;

	//printf("queue_pop 1 - head:%p,tail:%p\n", pinfo->pqueue_head, pinfo->pqueue_tail);

	int ret = 0;
	struct log_queue_node * pnode = (*head);
	if (pnode != NULL)
	{
		*head = pnode->pnext;
		if (pnode->pnext == NULL)
		{
			*tail = pnode->pnext;
			//printf("queue_pop 2 - head:%p,tail:%p\n", pinfo->pqueue_head, pinfo->pqueue_tail);
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
	pthread_mutex_unlock(&pinfo->mutex);

	return ret;
}


int queue_get_size(struct log_info * pinfo)
{
	int size = 0;

	pthread_mutex_lock(&pinfo->mutex);
	size = pinfo->queuesize;
	pthread_mutex_unlock(&pinfo->mutex);

	return size;
}

void queue_clear(struct log_info * pinfo)
{
	struct log_queue_node * heard = pinfo->pqueue_head;
	while (heard != NULL)
	{
		if (heard->len > 0)
		{
			heard->data.clear();
			heard->len = 0;
			heard = heard->pnext;
		}
	}
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

	time_t now_tm = time(0);
	struct tm * ptime = localtime(&now_tm);
	if (pinfo->curday != ptime->tm_mday)
	{
		//make new dir
		memset(buffer_dir_new, 0, sizeof(buffer_dir_new));
		memset(buffer_dir_mk, 0, sizeof(buffer_dir_mk));
		sprintf(buffer_dir_new, "logs_%04d_%02d_%02d/", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday);
		sprintf(buffer_dir_mk, "%s%s", pinfo->curdir, buffer_dir_new);
		if (mkdir(buffer_dir_mk, 0777) != 0)
		{
			//printf("failed - buffer_dir_mk:%s", buffer_dir_mk);
		}
		else
		{
			//printf("success - buffer_dir_mk:%s", buffer_dir_mk);
		}
		memset(buffer_file_open, 0, sizeof(buffer_file_open));

		pinfo->cursize = 0;
		pinfo->backcount = 0;
		pinfo->curday = ptime->tm_mday;
		
		memset(pinfo->filename, 0, sizeof(pinfo->filename));
		sprintf(pinfo->filename, "%s.log", pinfo->prename);
		sprintf(buffer_file_open, "%s%s%s", pinfo->curdir, buffer_dir_new, pinfo->filename);
		//printf("buffer_file_open:%s\n", buffer_file_open);
		file_close(&pinfo->pstream);
		pinfo->pstream = file_open(buffer_file_open, OPEN_MODE_APPEND);
		file_seek(pinfo->pstream, 0, SEEK_END);
	}
	if (pinfo->cursize >= pinfo->maxsize)
	{
		file_close(&pinfo->pstream);
		
		memset(buffer_dir_cur, 0, sizeof(buffer_dir_cur));
		memset(buffer_file_open, 0, sizeof(buffer_file_open));
		memset(buffer_file_back, 0, sizeof(buffer_file_back));

		sprintf(buffer_dir_cur, "logs_%04d_%02d_%02d/", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday);

		sprintf(buffer_file_open, "%s%s%s", pinfo->curdir, buffer_dir_cur, pinfo->filename);
		sprintf(buffer_file_back, "%s%s%s_back_%d.log", pinfo->curdir, buffer_dir_cur, pinfo->prename, pinfo->backcount);
		// rename
		rename(buffer_file_open, buffer_file_back);
		pinfo->pstream = file_open(buffer_file_open, OPEN_MODE_APPEND);
		pinfo->cursize = 0;
		pinfo->backcount++;

		//printf("buffer_file_open:%s\n", buffer_file_open);
		//printf("buffer_file_back:%s\n", buffer_file_back);
	}
	//delete back file	
	for (int index = 0; index < 7; ++index)
	{
		int agoday = pinfo->retainday + index;
		time_t ago_tm = now_tm - (agoday * 60 * 60 * 24);
		struct tm * ptime_ago = localtime(&ago_tm);

		memset(buffer_dir_ago, 0, sizeof(buffer_dir_ago));
		memset(buffer_file_ago, 0, sizeof(buffer_file_ago));
		sprintf(buffer_dir_ago, "logs_%04d_%02d_%02d/", ptime_ago->tm_year + 1900, ptime_ago->tm_mon + 1, ptime_ago->tm_mday);
		sprintf(buffer_file_ago, "%s%s%s", pinfo->curdir, buffer_dir_ago, pinfo->filename);
		
		//printf("buffer_file_ago:%s,ret_access:%d\n", buffer_file_ago, ret_access);
		if (access(buffer_file_ago, F_OK) == 0)
		{
			unlink(buffer_file_ago);
		}
		int backcount = 0;
		do
		{
			memset(buffer_file_ago, 0, sizeof(buffer_file_ago));
			sprintf(buffer_file_ago, "%s%s%s_back_%d.log", pinfo->curdir, buffer_dir_ago, pinfo->prename, backcount);
			//printf("buffer_file_ago:%s,ret_access:%d\n", buffer_file_ago, ret_access);
			if (access(buffer_file_ago, F_OK) == 0)
			{
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
		rmdir(buffer_dir_rm);
	}
	
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
			//printf("log_write_queue_data - head:%p,tail:%p,queuesize:%d,ret:%d,len:%d,buffer:%s\n", pinfo->pqueue_head, pinfo->pqueue_tail, pinfo->queuesize, ret, len,buffer);
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

void* runthreadfunc(void* parm)
{
	struct log_info * pinfo = (struct log_info *)parm;
	log_write_queue_data(pinfo);
	//printf("thread exit\n");
	return NULL;
}

//------------------------------------------------------------------------------

void log_thread_sleep(unsigned int msec)
{
	struct timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}

const char* log_get_time()
{
	time_t now = time(0);
	struct tm * pTime = localtime(&now);
	static char cdate[32] = { 0 };
	memset(cdate, 0, sizeof(cdate));
	sprintf(cdate, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
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
		log_get_curdir(pinfo->curdir);
		//printf("log_init_info - curdir:%s\n", pinfo->curdir);
		memset(pinfo->prename, 0, sizeof(pinfo->prename));
		memset(pinfo->filename, 0, sizeof(pinfo->filename));
		sprintf(pinfo->prename,"temp");
		pinfo->pstream = NULL;
		pinfo->curday = -1;
		pinfo->retainday = 5;	//retain 3 day
		pinfo->cursize = 0;
		pinfo->maxsize = 1024 * 1024 * 30; // 50M
		pinfo->backcount = 0;
		pinfo->queuesize = 0;
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pthread_mutex_init(&(pinfo->mutex), NULL);
		pthread_create(&(pinfo->pthread), 0, runthreadfunc, pinfo);
		pinfo->runthread = 1;
	}
}

struct log_info * log_create_logger()
{
	struct log_info * pinfo = new(struct log_info);
	log_init_info(pinfo);
	return pinfo;
}

void log_constructor_logger(const char * prename)
{
	if (_LOGGER == NULL)
	{
		_LOGGER = log_create_logger();
	}
	if (prename != NULL)
	{
		memset(_LOGGER->prename, 0, sizeof(_LOGGER->prename));
		sprintf(_LOGGER->prename, "%s", prename);
		//printf("log_constructor_logger - prename:%s\n", _LOGGER->prename);
	}
}


void log_destroy_logger()
{
	struct log_info * pinfo = _LOGGER;
	if (pinfo != NULL)
	{
		queue_clear(pinfo);
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pinfo->runthread = 0;
		pthread_mutex_destroy(&(pinfo->mutex));
		pthread_join(pinfo->pthread, NULL);
		pthread_cancel(pinfo->pthread);
		file_close(&pinfo->pstream);
		delete(pinfo);
		_LOGGER = NULL;
	}
}

//------------------------------------------------------------------------------


const char* log_set_header(int level, const char *  pfunc, const char *  pfile, unsigned int line)
{
	static const char levelname[LOG_LEVEL_CRITIC + 1][16] = { " [debug] "," [warning] "," [normal] "," [error] "," [critic] " };	
	static char cheader[1024] = { 0 };
	memset(cheader, 0, sizeof(cheader));

	sprintf(cheader, "%s%s[%s:%d][%s]", log_get_time(), levelname[level], pfile, line, pfunc);

	return cheader;
}

int log_write(const void * buffer, int len)
{
	if (_LOGGER == NULL)
	{
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

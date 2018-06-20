

#include "logger.h"

//------------------------------------------------------------------------------


#define OPEN_MODE_READ					"rb"
#define OPEN_MODE_WRITE					"wb"
#define OPEN_MODE_READ_WRITE			"rb+"
#define OPEN_MODE_APPEND				"ab+"

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
	file_close(pstream);
	printf("file stream - ret_seek:%d,ret_write:%d,ret_size:%d\n", ret_seek, ret_write, ret_size);
}

FILE * file_open(const char * pname, const char * pmode)
{
	if (pname == NULL)
	{
		return NULL;
	}
	FILE * pstream = fopen(pname, pmode ? pmode : "rb");
	return pstream;
}

void file_close(FILE * pstream)
{
	if (pstream != NULL)
	{
		fclose(pstream);
	}
	pstream = NULL;
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


int file_write_buffer(FILE * pstream,int count,const void * buffer)
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

struct log_queue
{
	int len;
	char *ptr;
	struct log_queue * pnext;
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
	struct log_queue * pqueue_head;
	struct log_queue * pqueue_tail;
	pthread_t pthread;
	int		runthread;
};

static struct log_info * _LOGGER = NULL;



void queue_push(struct log_info * pinfo,const void * buffer, int size)
{
	void * ptr = malloc(size);
	memcpy(ptr, buffer, size);

	struct log_queue * pqueue = (struct log_queue *)malloc(sizeof(struct log_queue));

	pqueue->len = size;
	pqueue->ptr = ptr;
	pqueue->pnext = NULL;

	pthread_mutex_lock(&pinfo->mutex);
	int * queuesize = &pinfo->queuesize;
	struct log_queue ** head = &pinfo->pqueue_head;
	struct log_queue ** tail = &pinfo->pqueue_tail;	

	if (*heard == NULL && *tail == NULL)
	{
		*heard = pqueue;
		*tail = pqueue;
	}
	else
	{
		(*tail)->pnext = pqueue;
		*tail = pqueue;
	}
	((*queuesize)++);

	pthread_mutex_unlock(&pinfo->mutex);
}

int queue_pop(struct log_info * pinfo, char ** buffer, int * size)
{
	pthread_mutex_lock(&pinfo->mutex);
	int * queuesize = &pinfo->queuesize;
	struct log_queue ** head = &pinfo->pqueue_head;
	struct log_queue ** tail = &pinfo->pqueue_tail;

	int ret = 0;
	struct log_queue * pqueue = *heard;
	if (pqueue != NULL)
	{
		*heard = pqueue->pnext;
		*buffer = pqueue->ptr;
		*size = pqueue->len;
		((*queuesize)--);
		free(pqueue);
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

void queue_clear(struct log_queue * heard)
{	
	while (heard != NULL)
	{
		if (heard->ptr != NULL)
		{
			free(heard->ptr);
			heard->ptr = NULL;
			heard->len = 0;
			heard = heard->pnext;
		}
	}
}

//struct log_info
//{
//	char    prename[32];	// file name prefix
//	char    filename[32];	// file name
//	FILE *  pstream;
//	int     curday;
//	int		retainday;
//	int     cursize;   // current file size
//	int     maxsize;   // max file size
//	int		backcount; // backup files count
//	int		queuesize;
//	pthread_mutex_t	   mutex;
//	struct log_queue * pqueue_head;
//	struct log_queue * pqueue_tail;
//	pthread_t pthread;
//	int		runthread;
//};


void check_file(struct log_info * pinfo)
{
	static char namebuffer[256];
	static time_t now_tm = time(0);
	static struct tm * ptime = localtime(&now_tm);
	if (pinfo->curday != ptime->tm_mday)
	{
		pinfo->cursize = 0;
		pinfo->curday = ptime->tm_mday;
		memset(namebuffer, 0, sizeof(namebuffer));
		memset(pinfo->filename, 0, sizeof(pinfo->filename));
		sprintf(pinfo->filename, "%s_%d_%d.log", pinfo->prename, ptime->tm_mon + 1, ptime->tm_mday);		
		sprintf(namebuffer, "%s%s", pinfo->curdir, pinfo->filename);
		file_close(pinfo->pstream);
		pinfo->pstream = file_open(namebuffer, OPEN_MODE_APPEND);
	}
	if (pinfo->cursize >= pinfo->maxsize)
	{
		file_close(pinfo->pstream);
		// rename

		memset(namebuffer, 0, sizeof(namebuffer));
		sprintf(namebuffer, "%s%s", pinfo->curdir, pinfo->filename);
		pinfo->pstream = file_open(namebuffer, OPEN_MODE_APPEND);
	}
}

void queue_write_data(struct log_info * pinfo)
{
	while (pinfo->runthread == 1)
	{
		if (pinfo != NULL && pinfo->queuesize > 0)
		{
			check_file(pinfo);


			int len = -2;
			char * buffer = NULL;
			int ret = queue_pop(pinfo, &buffer, &len);
			printf("queue_pop - head:%p,queuesize:%d,ret:%d,len:%d\n", pinfo->pqueue_head, pinfo->queuesize, ret, len);
			if (ret == 1)
			{
				file_write_buffer(pinfo->pstream, len, buffer);
				file_flush(pinfo->pstream);
				free(buffer);
				pinfo->cursize = += len;
			}
			else
			{
				usleep(3000000);
			}
		}
		else
		{
			usleep(3000000);
		}
	}
}

void* runthread(void* parm)
{
	struct log_info * pinfo = (struct log_info *)parm;
	queue_write_data(pinfo);
	return NULL;
}

//------------------------------------------------------------------------------

const char* gettime()
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
	char * p = strrchr(pbuf, '/');
	if (p != NULL)
	{
		*(p + 1) = 0;
	}
}

void log_init_info(struct log_info * pinfo)
{
	if (pinfo != NULL)
	{
		log_get_curdir(pinfo->curdir);
		printf("log_init_info - curdir:%s", pinfo->curdir);
		memset(pinfo->prename, 0, sizeof(pinfo->prename));
		memset(pinfo->filename, 0, sizeof(pinfo->filename));
		sprintf(pinfo->prename,"temp");
		pinfo->pstream = NULL;
		pinfo->curday = -1;
		pinfo->retainday = 5;	//retain 3 day
		pinfo->cursize = 0;
		pinfo->maxsize = 1024 * 1024 * 50; // 50M
		pinfo->backcount = 0;
		pinfo->queuesize = 0;
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pthread_mutex_init(&(pinfo->mutex), NULL);
		pthread_create(&(pinfo->pthread), 0, runthread, pinfo);
		pinfo->runthread = 1;
	}
}

struct log_info * log_create_logger()
{
	struct log_info * pinfo = (struct log_info *)malloc(sizeof(struct log_info));
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
		printf("log_constructor_logger - prename:%s\n", _LOGGER->prename);
	}
}


void log_destroy_logger()
{
	struct log_info * pinfo = _LOGGER;
	if (pinfo != NULL)
	{
		queue_clear(pinfo->pqueue_head);
		pinfo->pqueue_head = NULL;
		pinfo->pqueue_tail = NULL;
		pinfo->runthread = 0;
		pthread_mutex_destroy(&(pinfo->mutex));
		pthread_join(pinfo->pthread, NULL);
		pthread_cancel(pinfo->pthread);
		file_close(pinfo->pstream);
		free(pinfo);
	}
}



const char* log_set_header(int level, const char *  pfunc, const char *  pfile, unsigned int line)
{
	static const char levelname[LOG_LEVEL_CRITIC + 1][16] = { " [debug] "," [warning] "," [normal] "," [error] "," [critic] " };	
	static char cheader[1024] = { 0 };
	memset(cheader, 0, sizeof(cheader));

	sprintf(cheader, "%s%s[%s:%d][%s]", gettime(), levelname[level], pfile, line, pfunc);

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
	char  buffer[MAX_EACH_LINE_WORD] = { 0 };
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



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
	FILE * pstream = open_file(pname, OPEN_MODE_APPEND);
	char buffer[1024] = { 0 };
	sprintf(buffer, "hello world.");
	int ret_seek = seek_file(pstream, 8, SEEK_SET);
	int ret_write = write_buffer(pstream, strlen(buffer), buffer);
	flush_file(pstream);
	int ret_size = size_file(pstream);
	close_file(pstream);
	printf("file stream - ret_seek:%d,ret_write:%d,ret_size:%d\n", ret_seek, ret_write, ret_size);
}

FILE * open_file(const char * pname, const char * pmode)
{
	if (pname == NULL)
	{
		return NULL;
	}
	FILE * pstream = fopen(pname, pmode ? pmode : "rb");
	return pstream;
}

void close_file(FILE * pstream)
{
	if (pstream != NULL)
	{
		fclose(pstream);
	}
	pstream = NULL;
}

void flush_file(FILE * pstream)
{
	//确保写入到文件-不一定是磁盘但是别处可以读取
	if (pstream != NULL)
	{
		fflush(pstream);
	}
}

//|SEEK_SET | SEEK_CUR		| SEEK_END	|
//|文件开头 | 当前读写的位置| 文件尾	|
int	seek_file(FILE * pstream,int offset, int fromwhere)
{
	if (pstream == NULL)
	{
		return -1;
	}
	return fseek(pstream, offset, fromwhere);
}

int size_file(FILE * pstream)
{
	if (pstream == NULL)
	{
		return 0;
	}

	int itell = ftell(pstream); // 记录当前文件的位置
	int ret_seek = seek_file(pstream,0, SEEK_END);
	if (ret_seek != 0)
	{
		seek_file(pstream, itell, SEEK_SET);
		return 0;
	}
	int isize = ftell(pstream); // 文件位置指针当前位置相对于文件首的偏移字节数
	seek_file(pstream, itell, SEEK_SET); // 恢复当前文件位置
	return isize;
}


int write_buffer(FILE * pstream,int count,const void * buffer)
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
	char    prename[32];
	FILE *  pstream;
	int     curday;
	int		retainday;
	int     maxsize;
	int		queuesize;
	pthread_mutex_t	   mutex;
	struct log_queue * pqueue_head;
	struct log_queue * pqueue_tail;
	pthread_t pthread;
	int		runthread;
};

static struct log_info * _LOGGER = NULL;



void queue_push(struct log_queue ** heard, struct log_queue ** tail, pthread_mutex_t * mutex,int * queuesize,const void * buffer, int size)
{
	void * ptr = malloc(size);
	memcpy(ptr, buffer, size);

	struct log_queue * pqueue = (struct log_queue *)malloc(sizeof(struct log_queue));

	pqueue->len = size;
	pqueue->ptr = ptr;
	pqueue->pnext = NULL;

	pthread_mutex_lock(mutex);
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

	pthread_mutex_unlock(mutex);
}

int queue_pop(struct log_queue ** heard, pthread_mutex_t * mutex, int * queuesize, char ** buffer, int * size)
{
	pthread_mutex_lock(mutex);
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
	pthread_mutex_unlock(mutex);

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

void write_queue_data(struct log_info * pinfo)
{
	while (pinfo->runthread == 1)
	{
		if (pinfo != NULL && pinfo->queuesize > 0)
		{
			if (pinfo->pstream == NULL)
			{
				pinfo->pstream = open_file(pinfo->prename, OPEN_MODE_APPEND);
			}
			int len = -2;
			char * buffer = NULL;
			int ret = queue_pop(&pinfo->pqueue_head, &pinfo->mutex, &pinfo->queuesize, &buffer, &len);
			printf("queue_pop - head:%p,ret:%d,len:%d\n", _LOGGER->pqueue_head, ret, len);
			if (ret == 1)
			{
				write_buffer(pinfo->pstream, len, buffer);
				flush_file(pinfo->pstream);
				free(buffer);
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
	write_queue_data(pinfo);
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

void log_init_info(struct log_info * pinfo)
{
	if (pinfo != NULL)
	{
		memset(pinfo->prename, 0, sizeof(pinfo->prename));
		sprintf(pinfo->prename,"temp.log");
		pinfo->pstream = NULL;
		pinfo->curday = -1;
		pinfo->retainday = 3;	//retain 3 day
		pinfo->maxsize = 1024 * 1024 * 50; // 50M
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
		sprintf(_LOGGER->prename, "%s.log", prename);
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
		close_file(pinfo->pstream);
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
	queue_push(&_LOGGER->pqueue_head, &_LOGGER->pqueue_tail, &_LOGGER->mutex, &_LOGGER->queuesize, buffer, len);
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

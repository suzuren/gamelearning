
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <stdarg.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

//------------------------------------------------------------------------------


#define LOG_LEVEL_DEBUG					0
#define LOG_LEVEL_WARNING				1
#define LOG_LEVEL_NORMAL				2
#define LOG_LEVEL_ERROR					3
#define LOG_LEVEL_CRITIC				4

#define MAX_EACH_LINE_WORD				(1024*512) //每次写入最大字符数

#define LOG_BREATHING_SPACE				(50)

//------------------------------------------------------------------------------
// file stream

FILE *	file_open(const char * pname, const char * pmode);
void	file_close(FILE ** pstream);
void	file_flush(FILE * pstream);
int		file_seek(FILE * pstream, int offset, int fromwhere);
int		file_size(FILE * pstream);
int		file_write_buffer(FILE * pstream, int count, const void * buffer);

//------------------------------------------------------------------------------

// write log
void log_thread_sleep(unsigned int msec);
void log_constructor_logger(const char * prename);
void log_destroy_logger();
void log_format_write(int level, const char * pfunc, const char * pfile, unsigned int line, char const* pformat, ...);

#define LOG_DEBUG(fmt, arg...) \
		log_format_write(LOG_LEVEL_DEBUG,__FUNCTION__, __FILE__, __LINE__,fmt, ##arg); 



#endif // __LOGGER_H__


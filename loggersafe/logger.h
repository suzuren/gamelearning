
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <time.h>

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

#define LOG_BREATHING_SPACE				(10)

//------------------------------------------------------------------------------

// write log
void log_time_update_logger();
void log_constructor_logger(const char * prename);
void log_shutdown_logger();
void log_thread_sleep(unsigned int msec);
void log_format_write(int level, const char * pfunc, const char * pfile, unsigned int line, char const* pformat, ...);

#define LOG_DEBUG(fmt, arg...) \
		log_format_write(LOG_LEVEL_DEBUG,__FUNCTION__, __FILE__, __LINE__,fmt, ##arg); 

#define LOG_WARNING(fmt, arg...) \
	  	log_format_write(LOG_LEVEL_WARNING,__FUNCTION__, __FILE__, __LINE__,fmt, ##arg) ; 


#define LOG_ERROR(fmt, arg...) \
		log_format_write(LOG_LEVEL_ERROR,__FUNCTION__, __FILE__, __LINE__,fmt, ##arg) ; 


#define LOG_CRITIC(fmt, arg...) \
		log_format_write(LOG_LEVEL_CRITIC,__FUNCTION__, __FILE__, __LINE__,fmt, ##arg) ; 


#endif // __LOGGER_H__


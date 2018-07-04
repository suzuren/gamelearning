

#include "timer.h"


struct timer_info
{
	pthread_mutex_t	time_mutex;
	struct tm		time_target;
	struct timeval	time_value;
	struct timezone time_zone;
};

static struct timer_info * _TIMER = NULL;


void timer_target_init(struct tm * ptime_target)
{
	if (ptime_target == NULL)
	{
		return;
	}
	ptime_target->tm_sec   = 0;
	ptime_target->tm_min   = 0;
	ptime_target->tm_hour  = 0;
	ptime_target->tm_mday  = 0;
	ptime_target->tm_mon   = 0;
	ptime_target->tm_year  = 0;
	ptime_target->tm_wday  = 0;
	ptime_target->tm_yday  = 0;
	ptime_target->tm_isdst = 0;
}

void timer_get_time_value(struct timer_info * pinfo)
{
	pthread_mutex_lock(&pinfo->time_mutex);
	gettimeofday(&pinfo->time_value, &pinfo->time_zone);
	pthread_mutex_unlock(&pinfo->time_mutex);
}

unsigned long long timer_getmicroseconds(struct timeval *ptvalue)
{
	unsigned long long microseconds = 0;
	if (ptvalue != NULL)
	{
		microseconds = ptvalue->tv_sec * 1000000 + ptvalue->tv_usec;
	}
	return microseconds;
}

unsigned long long timer_getseconds(struct timeval *ptvalue)
{
	unsigned long long seconds = 0;
	if (ptvalue != NULL)
	{
		seconds = ptvalue->tv_sec + ptvalue->tv_usec / 1000000;
	}
	return seconds;
}

void timer_update_target(struct timer_info *pinfo)
{
	if (pinfo == NULL)
	{
		return;
	}
	long int seconds = timer_getseconds(&pinfo->time_value);
	pthread_mutex_lock(&pinfo->time_mutex);
	struct tm * ptime = localtime(&seconds);
	pinfo->time_target.tm_sec   = ptime->tm_sec;
	pinfo->time_target.tm_min   = ptime->tm_min;
	pinfo->time_target.tm_hour  = ptime->tm_hour;
	pinfo->time_target.tm_mday  = ptime->tm_mday;
	pinfo->time_target.tm_mon   = ptime->tm_mon;
	pinfo->time_target.tm_year  = ptime->tm_year;
	pinfo->time_target.tm_wday  = ptime->tm_wday;
	pinfo->time_target.tm_yday  = ptime->tm_yday;
	pinfo->time_target.tm_isdst = ptime->tm_isdst;
	pthread_mutex_unlock(&pinfo->time_mutex);
}

void timer_get_target(struct tm * ptime_target,struct timer_info *pinfo)
{
	if (ptime_target == NULL || pinfo == NULL)
	{
		return;
	}
	pthread_mutex_lock(&pinfo->time_mutex);
	ptime_target->tm_sec   = pinfo->time_target.tm_sec;
	ptime_target->tm_min   = pinfo->time_target.tm_min;
	ptime_target->tm_hour  = pinfo->time_target.tm_hour;
	ptime_target->tm_mday  = pinfo->time_target.tm_mday;
	ptime_target->tm_mon   = pinfo->time_target.tm_mon;
	ptime_target->tm_year  = pinfo->time_target.tm_year;
	ptime_target->tm_wday  = pinfo->time_target.tm_wday;
	ptime_target->tm_yday  = pinfo->time_target.tm_yday;
	ptime_target->tm_isdst = pinfo->time_target.tm_isdst;
	pthread_mutex_unlock(&pinfo->time_mutex);
}

const char* timer_get_time_format(struct timer_info *pinfo)
{
	static char cdate[32];
	memset(cdate, 0, sizeof(cdate));
	if (pinfo != NULL)
	{
		struct tm time_target;
		timer_get_target(&time_target, pinfo);
		sprintf(cdate, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]", time_target.tm_year + 1900, time_target.tm_mon + 1, time_target.tm_mday, time_target.tm_hour, time_target.tm_min, time_target.tm_sec);
	}
	return cdate;
}

const char* timer_get_seconds_format(long int seconds)
{
	struct timer_info *pinfo = _TIMER;
	static char cdate[32];
	memset(cdate, 0, sizeof(cdate));
	if (pinfo != NULL)
	{
		struct tm time_target;
		timer_get_target(&time_target, pinfo);
		struct tm * ptime = localtime_r(&seconds, &time_target);
		sprintf(cdate, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
	}
	return cdate;
}

void timer_init_info()
{
	if (_TIMER == NULL)
	{
		struct timer_info * pinfo = (struct timer_info *)malloc(sizeof(struct timer_info));
		if (pinfo == NULL)
		{
			return;
		}
		_TIMER = pinfo;
		pthread_mutex_init(&pinfo->time_mutex, NULL);
		timer_target_init(&pinfo->time_target);
		timer_get_time_value(pinfo);
		timer_update_target(pinfo);		
	}
}

void timer_update_time()
{
	timer_get_time_value(_TIMER);
	timer_update_target(_TIMER);
}

void timer_destroy_info()
{
	struct timer_info * pinfo = _TIMER;
	if (pinfo != NULL)
	{
		pthread_mutex_destroy(&pinfo->time_mutex);
		free(pinfo);
		_TIMER = NULL;
	}
}

void timer_test()
{
	printf("timer_test - _TIMER:%p\r\n", _TIMER);
	printf("time string:%s\r\n", timer_get_time_format(_TIMER));
	printf("time value - tv_sec:%ld,tv_usec:%ld\r\n", _TIMER->time_value.tv_sec, _TIMER->time_value.tv_usec);
	printf("time zone - tz_minuteswest:%d,tz_dsttime:%d\r\n", _TIMER->time_zone.tz_minuteswest, _TIMER->time_zone.tz_dsttime);


	unsigned long long u64ArrayTime[] = { 1499220668, 1499220969 };
	cout << "u64ArrayTime: -- {" << endl;
	for (unsigned int i = 0; i < sizeof(u64ArrayTime) / sizeof(u64ArrayTime[0]); i++)
	{
		cout << left;
		cout << "i:" << setw(2) << i << " time:" << u64ArrayTime[i] << " format:" << timer_get_seconds_format(u64ArrayTime[i]) << endl;
	}
	cout << "}" << endl;
}



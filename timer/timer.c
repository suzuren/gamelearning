

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


//------------------------------ 获得本地时间
int	getLocalTime(struct tm* _Tm, const unsigned long long * _Time)
{
	if (!_Tm || !_Time)
	{
		return -1;
	}

	time_t _time = *_Time;
	if (_Tm)
	{
		memset(_Tm, 0xff, sizeof(struct tm));
	}
	struct tm* tmp = ::localtime(&_time);
	if (tmp && _Tm)
	{
		memcpy(_Tm, tmp, sizeof(struct tm));
	}
	return 0;
}

int	getLocalTime(struct tm* _Tm, unsigned long long _Time)
{
	if (!_Tm || !_Time)
	{
		return -1;
	}

	return getLocalTime(_Tm, &_Time);
}

//------------------------------ 获得时间相差天数
int	diffTimeDay(unsigned long long _early, unsigned long long _late)
{
	if (_early == 0 || _late == 0)
	{
		return 0;
	}

	static tm	tm_early;
	static tm	tm_late;
	memset(&tm_early, 0, sizeof(tm_early));
	memset(&tm_late, 0, sizeof(tm_late));

	getLocalTime(&tm_early, _early);
	getLocalTime(&tm_late, _late);

	if (tm_early.tm_year > tm_late.tm_year)
	{
		return 0;
	}

	//同年同日
	if (tm_early.tm_year == tm_late.tm_year && tm_early.tm_yday == tm_late.tm_yday)
	{
		return 0;
	}

	//同年判断
	if (tm_early.tm_year == tm_late.tm_year)
	{
		if (tm_early.tm_yday >= tm_late.tm_yday)
		{
			return 0;
		}
		return (tm_late.tm_yday - tm_early.tm_yday);
	}

	int iDay = 0;
	//不同年时
	if (tm_early.tm_year != tm_late.tm_year)
	{
		tm tm_temp = tm_early;

		//获取12月31日时间
		tm_temp.tm_mon = 11;
		tm_temp.tm_mday = 31;
		tm_temp.tm_yday = 0;
		unsigned long long _temp = mktime(&tm_temp);

		getLocalTime(&tm_temp, _temp);
		iDay = tm_temp.tm_yday - tm_early.tm_yday;

		iDay += 1;//跨年+1

		//获得相差年天数
		for (int i = tm_early.tm_year + 1; i < tm_late.tm_year; i++)
		{
			tm_temp.tm_year++;
			tm_temp.tm_yday = 0;

			_temp = mktime(&tm_temp);
			getLocalTime(&tm_temp, _temp);

			iDay += tm_temp.tm_yday;
			iDay += 1;//跨年+1
		}
	}

	return (iDay + tm_late.tm_yday);
}

//------------------------------ 


static unsigned long long	g_uSystemTime = 0;
//------------------------------ 获得系统时间
//unsigned long long	getTime()
//{
//	return ::time(NULL);
//}
//------------------------------ 


unsigned long long	setSysTime()
{
	//static unsigned long long g_uTime = 0;
	//if (!g_uTime)
	//{
	//	/*
	//	tm tm_temp;
	//	tm_temp.tm_year	= 2010-1900;//年1900+
	//	tm_temp.tm_mon	= 0;		//月[0,11]
	//	tm_temp.tm_mday	= 1;		//日[1,31]
	//	tm_temp.tm_hour	= 0;		//时[0,23]
	//	tm_temp.tm_min	= 0;		//分[0,59]
	//	tm_temp.tm_sec	= 0;		//秒[0,59]
	//	g_uTime	= mktime(&tm_temp);
	//	*/
	//}

	//g_uSystemTime = getTime();

	//if (g_uSystemTime > g_uTime)
	//	g_uSystemTime -= g_uTime;

	//在这里可以制定一个初始时间,用来做时间片

	g_uSystemTime = ::time(NULL);
	return g_uSystemTime;
}

//------------------------------ 获得系统时间
unsigned long long	getSysTime()
{
	if (!g_uSystemTime)
		setSysTime();

	return g_uSystemTime;
}

void test_diffTimeDay()
{
	static unsigned long long uProcessTime = 0;
	unsigned long long uTime = getSysTime();
	if (!uProcessTime)
	{
		uProcessTime = uTime;
	}

	bool bNewDay = (diffTimeDay(uProcessTime, uTime) != 0);
	printf("test_diffTimeDay 1 - new_time:%llu - %s\n", uProcessTime, timer_get_seconds_format(uProcessTime));
	printf("test_diffTimeDay 1 - old_time:%llu - %s, bNewDay:%d\n", uTime, timer_get_seconds_format(uTime), bNewDay);

	unsigned long long new_time = 1499220969;
	unsigned long long old_time = 1499920668;
	bNewDay = (diffTimeDay(new_time, old_time) != 0);
	printf("test_diffTimeDay 2 - new_time:%llu - %s\n", new_time, timer_get_seconds_format(new_time));
	printf("test_diffTimeDay 2 - old_time:%llu - %s, bNewDay:%d\n", old_time, timer_get_seconds_format(old_time), bNewDay);

	new_time = 1536116640;
	old_time = 1536117640;
	bNewDay = (diffTimeDay(new_time, old_time) != 0);
	printf("test_diffTimeDay 3 - new_time:%llu - %s\n", new_time, timer_get_seconds_format(new_time));
	printf("test_diffTimeDay 3 - old_time:%llu - %s, bNewDay:%d\n", old_time, timer_get_seconds_format(old_time), bNewDay);

	new_time = 1536116640;
	old_time = 1536189640;
	bNewDay = (diffTimeDay(new_time, old_time) != 0);
	printf("test_diffTimeDay 4 - new_time:%llu - %s\n", new_time, timer_get_seconds_format(new_time));
	printf("test_diffTimeDay 4 - old_time:%llu - %s, bNewDay:%d\n", old_time, timer_get_seconds_format(old_time), bNewDay);

}


void test_AlldaySeconds()
{
	for (unsigned int cbIndexHour = 0; cbIndexHour < 24; cbIndexHour++)
	{
		for (unsigned int cbIndexMinute = 0; cbIndexMinute < 60; cbIndexMinute++)
		{
			for (unsigned int cbIndexSecond = 0; cbIndexSecond < 60; cbIndexSecond++)
			{
				unsigned int curSecond = cbIndexHour * 3600 + cbIndexMinute * 60 + cbIndexSecond;
				printf("test_AlldaySeconds function - Hour:%02d,Minute:%02d,Second:%02d,curSecond:%d\n", cbIndexHour, cbIndexMinute, cbIndexSecond, curSecond);

			}
		}
	}
}

/*


*/
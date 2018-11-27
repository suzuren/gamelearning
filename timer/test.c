
#include <stdio.h>
#include "timer.h"




unsigned long long	GetMillisecond()
{
	//struct timespec
	//{
	//	time_t tv_sec; // seconds[��]
	//	long tv_nsec; // nanoseconds[����]
	//};
	//int clock_gettime(clockid_t clk_id, struct timespect *tp);
	////@clk_id:
	//CLOCK_REALTIME:ϵͳʵʱʱ��, ��ϵͳʵʱʱ��ı���ı�
	//CLOCK_MONOTONIC : ��ϵͳ������һ����ʼ��ʱ, ����ϵͳʱ�䱻�û��ı��Ӱ��
	//CLOCK_PROCESS_CPUTIME_ID : �����̵���ǰ����ϵͳCPU���ѵ�ʱ��
	//CLOCK_THREAD_CPUTIME_ID : ���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��

	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;

	return millisecond;
}

void ms_sleep(unsigned int msec)
{
	struct timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}

int main(int argc, const char** argv)
{
	printf("test timer ...\r\n");

	timer_init_info();
	timer_test();
	test_diffTimeDay();
	test_AlldaySeconds();
	unsigned long long	bms = GetMillisecond();

	int iLoop = 1;
	while (1)
	{
		timer_update_time();
		ms_sleep(3000);
		
		iLoop--;

		if (iLoop <= 0)
		{
			break;
		}
		
	}
	unsigned long long	ems = GetMillisecond();
	//���3����
	printf("bms:%lld,ems:%lld\n", bms, ems);

	timer_destroy_info();


	return 0;
}


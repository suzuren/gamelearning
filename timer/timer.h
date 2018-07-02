
#ifndef __TIMER_H__
#define __TIMER_H__



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <memory.h>

void timer_init_info();
void timer_update_time();
void timer_destroy_info();


void timer_test();



#endif //__TIMER_H__
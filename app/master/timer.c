/*
 * timer.c
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#include "master.h"
#include "timer.h"
//#include "raw_packet.h"


static timer_element_t timer_queue[MAX_TIMER_NUMS];
static int init_first_run = 0;

void timer_init(void)
{
	memset(&timer_queue, 0, sizeof(timer_queue));
}

int timer_register(uint duration, uint first_run, timer_func_t func, void *data, timer_exit_t on_exit, char *desc)
{
	int i;
	struct timeval tv;

	for(i = 0; i < MAX_TIMER_NUMS; i ++) {
		if(timer_queue[i].valid == 0) break;
	}
	if(i >= MAX_TIMER_NUMS) {
		ERR_LOG("Timer queue full!");
		return -1;
	}
	timer_queue[i].data = data;
	gettimeofday(&tv, NULL);
	timer_queue[i].issue_time.tv_sec = tv.tv_sec;
	timer_queue[i].issue_time.tv_usec = tv.tv_usec;
	timer_queue[i].next_time = (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	timer_queue[i].duration = duration;
	timer_queue[i].timer_func = func;
	timer_queue[i].on_exit = on_exit;
	timer_queue[i].privates = 0;
	strncpy(timer_queue[i].desc, desc, sizeof(timer_queue[i].desc));
	timer_queue[i].valid = 1;

	if (!first_run){
		timer_queue[i].next_time += timer_queue[i].duration;
	}
	
	return i;
}

void timer_reschedule(int handle, int second, int reason)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	timer_queue[handle].next_time = (uint64_t)(tv.tv_sec + second) * 1000 + tv.tv_usec / 1000;
	timer_queue[handle].schedule_reason = reason;
	init_first_run = 1;
}

int timer_cancel(int handle)
{
	if (handle < MAX_TIMER_NUMS){
		timer_queue[handle].valid = 0;
		if (timer_queue[handle].on_exit){
			timer_queue[handle].on_exit(&timer_queue[handle]);
		}
		return 0;
	}
	return -1;
}


int timer_set_duration(int handle, int duration)
{
	if ((handle < MAX_TIMER_NUMS) && (timer_queue[handle].valid)){		
		timer_queue[handle].duration = duration;
	}
	return -1;
}


void timer_scheduler(void)
{
	int i, ret;
	struct timeval tv;
	uint64_t ts, old_ts;

//	DBG_PRINTF("TS");

	//raw_packet_timeout(); // Always process raw packet timeout

	gettimeofday(&tv, NULL);
	ts = (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;

	if(!init_first_run) {
		init_first_run = 1;
		for(i = 0; i < MAX_TIMER_NUMS; i++) {
			if(timer_queue[i].valid) {
				old_ts = (uint64_t)timer_queue[i].issue_time.tv_sec * 1000 + timer_queue[i].issue_time.tv_usec / 1000;
				timer_queue[i].issue_time.tv_sec = tv.tv_sec;
				timer_queue[i].issue_time.tv_usec = tv.tv_usec;
				timer_queue[i].next_time += (ts - old_ts);
			}
		}
	}

//	DBG_PRINTF("TS: %lld:%lld\n", ts, );
	for(i = 0; i < MAX_TIMER_NUMS; i++) {
		if(timer_queue[i].valid) {
//			DBG_PRINTF("TS: %d:%lld:%lld\n", i, ts, timer_queue[i].next_time);
			if(ts > timer_queue[i].next_time) {
				if(timer_queue[i].timer_func) {
					timer_queue[i].issue_time.tv_sec = tv.tv_sec;
					timer_queue[i].issue_time.tv_usec = tv.tv_usec;
					ret = timer_queue[i].timer_func(&timer_queue[i], timer_queue[i].data);
					switch(ret) {
					case TIMER_RUN_FOREVER:
						timer_queue[i].next_time += timer_queue[i].duration;
						timer_queue[i].schedule_reason	= REASON_TIMER;
						break;
					case TIMER_RUN_ONCE:
						timer_cancel(i);
						break;
					default:
						DBG_PRINTF("Invalid timer function return value: %d", ret);
					}
				}
				else {
					DBG_PRINTF("Empty function timer element");
				}
			}
		}
	}
}

/*
	When system time change, the timer need to be reset the nexttime . otherwise it will continue run 
*/

void timer_reschedule_all(void)
{
	int i;
	struct timeval tv;
	for(i = 0; i < MAX_TIMER_NUMS; i++) {
		if(timer_queue[i].valid) {
			gettimeofday(&tv, NULL);
			timer_queue[i].next_time = (uint64_t)(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
			timer_queue[i].schedule_reason = REASON_SYSTIME;
		}
	}
}


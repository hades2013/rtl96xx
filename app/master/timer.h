/*
 * timer.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef TIMER_H_
#define TIMER_H_

#define	MAX_TIMER_NUMS	64

#define	TIMER_RUN_FOREVER	0
#define	TIMER_RUN_ONCE		1

#define REASON_TIMER 0
#define REASON_SYSTIME 1
#define REASON_FIRST 2


typedef int (*timer_func_t)(void *timer, void *data);
typedef void (*timer_exit_t)(void *timer);

typedef struct {
	int valid;
	int index;
	char desc[16];
	void *data;
	struct timeval issue_time;
	uint duration;
	uint64_t next_time;
	timer_func_t timer_func;
	timer_exit_t on_exit;
	uint32_t privates;// for some uses in timer func
	int schedule_reason;
}
timer_element_t;



void timer_init(void);
int timer_register(uint duration, uint first_run, timer_func_t func, void *data, timer_exit_t on_exit, char *desc);
void timer_scheduler(void);
void timer_reschedule(int handle, int second, int reason);
void timer_reschedule_all(void);
int timer_cancel(int handle);
int timer_set_duration(int handle, int duration);




#endif /* TIMER_H_ */

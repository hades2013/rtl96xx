/*****************************************************************************
------------------------------------------------------------------------------
   Description: monitor_alarm.c header file
------------------------------------------------------------------------------
  --------------------------------------------------------------------------
*****************************************************************************/
#ifndef _BGMONITOR_ALARM_H_
#define _BGMONITOR_ALARM_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include <sys/time.h>

typedef void (monitor_alarm_callback) (unsigned int clientreg, void *clientarg);

/*
 * alarm flags
 */
#define MA_REPEAT 0x01          /* keep repeating every X seconds */
#define NO_ERROR   0
#ifdef ERROR
#undef ERROR
#define ERROR  1
#else
#define ERROR  1
#endif

typedef struct tag_monitor_alarm
{
    struct timeval  t;
    unsigned int    flags;
    unsigned int    clientreg;
    struct timeval  t_last;
    struct timeval  t_next;
    void           *clientarg;
    monitor_alarm_callback *thecallback;
    struct tag_monitor_alarm *next;
}monitor_alarm_t;

/*
 * the ones you should need
 */
void monitor_alarm_unregister(unsigned int clientreg);
void monitor_alarm_unregister_all(void);
unsigned int monitor_alarm_register(unsigned int when,
                                    unsigned int flags,
                                    monitor_alarm_callback * thecallback,
                                    void *clientarg);



/*
 * the ones you shouldn't
 */
void init_monitor_alarm(void);
void ma_update_entry(monitor_alarm_t *alrm);
monitor_alarm_t *ma_find_next(void);
void run_alarms(void);
void alarm_handler(int);
void set_an_alarm(void);
unsigned int get_next_alarm_delay_time(struct timeval *delta);
ULONG GET_INT32U_FROM_PKT(UCHAR *pucData, USHORT usOffset);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _MONITOR_ALARM_H_ */

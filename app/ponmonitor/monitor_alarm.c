/*****************************************************************************
------------------------------------------------------------------------------

   Description: 系统检测定时任务接口

------------------------------------------------------------------------------
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <malloc.h>
#include <unistd.h>
#include "monitor.h"
#include "monitor_alarm.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
monitor_alarm_t *thealarms = NULL;
static int start_alarms = 0;
static unsigned int regnum = 1;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//#define SIGALRM 1


long uptime(void)
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}

/*replace gettimeofday(),because sys time maybe changed ugly by our friends:tester*/
long get_sys_uptime(struct timeval *tv )
{
    tv->tv_sec = uptime();		
    tv->tv_usec = 0;

    return tv->tv_sec;
}

void init_monitor_alarm(void)
{
    start_alarms = 1;
    set_an_alarm();
    return;
}


void ma_update_entry(monitor_alarm_t *a)
{
    if ((a->t_last.tv_sec == 0) && (a->t_last.tv_usec == 0))
    {
        struct timeval t_now;
        /*
         * Never been called yet, call time `t' from now.  
         */
        get_sys_uptime(&t_now);

        a->t_last.tv_sec = t_now.tv_sec;
        a->t_last.tv_usec = t_now.tv_usec;

        a->t_next.tv_sec = t_now.tv_sec + a->t.tv_sec;
        a->t_next.tv_usec = t_now.tv_usec + a->t.tv_usec;

        while(a->t_next.tv_usec >= 1000000) 
        {
            a->t_next.tv_usec -= 1000000;
            a->t_next.tv_sec += 1;
        }
    } 
	else if ((a->t_next.tv_sec == 0) && (a->t_next.tv_usec == 0))
	{
        /*
         * We've been called but not reset for the next call.  
         */
        if (a->flags & MA_REPEAT)
		{
            if ((a->t.tv_sec == 0) && (a->t.tv_usec == 0))
			{
                MONITOR_DBG_ERR("monitor_alarm: update_entry: illegal interval specified\n");
                monitor_alarm_unregister(a->clientreg);
                return;
            }

            a->t_next.tv_sec = a->t_last.tv_sec + a->t.tv_sec;
            a->t_next.tv_usec = a->t_last.tv_usec + a->t.tv_usec;

            while (a->t_next.tv_usec >= 1000000)
			{
                a->t_next.tv_usec -= 1000000;
                a->t_next.tv_sec += 1;
            }
        }
		else
		{
            /*
             * Single time call, remove it.  
             */
            monitor_alarm_unregister(a->clientreg);
        }
    }
}

/**
 * This function removes the callback function from a list of registered
 * alarms, unregistering the alarm.
 *
 * @param clientreg is a unique unsigned integer representing a registered
 *	alarm which the client wants to unregister.
 *
 * @return void
 *
 * @see monitor_alarm_register
 * @see monitor_alarm_register_hr
 * @see monitor_alarm_unregister_all
 */
void monitor_alarm_unregister(unsigned int clientreg)
{
    monitor_alarm_t *ma_ptr, **prev_next = &thealarms;

    for (ma_ptr = thealarms;
         ma_ptr != NULL && ma_ptr->clientreg != clientreg;
         ma_ptr = ma_ptr->next)
	{
        prev_next = &(ma_ptr->next);
    }

    if (ma_ptr != NULL)
	{
        *prev_next = ma_ptr->next;
        MONITOR_DBG_INFO("monitor_alarm unregistered alarm %d\n", ma_ptr->clientreg);
        /*
         * Note:  do not free the clientarg, its the clients responsibility 
         */
        free(ma_ptr);
    }
	else 
	{
        MONITOR_DBG_INFO("monitor_alarm no alarm %d to unregister\n", clientreg);
    }
}

/**
 * This function unregisters all alarms currently stored.
 *
 * @return void
 *
 * @see monitor_alarm_register
 * @see monitor_alarm_register_hr
 * @see monitor_alarm_unregister
 */
void
monitor_alarm_unregister_all(void)
{
    monitor_alarm_t *ma_ptr, *ma_tmp;

    for (ma_ptr = thealarms; ma_ptr != NULL; ma_ptr = ma_tmp)
    {
        ma_tmp = ma_ptr->next;
        free(ma_ptr);
    }
    MONITOR_DBG_INFO("monitor_alarm: ALL alarms unregistered\n");
    thealarms = NULL;
}  

monitor_alarm_t *ma_find_next(void)
{
    monitor_alarm_t *a, *lowest = NULL;

    for (a = thealarms; a != NULL; a = a->next) 
	{
        if (lowest == NULL)
		{
            lowest = a;
        }
		else if (a->t_next.tv_sec == lowest->t_next.tv_sec)
		{
            if (a->t_next.tv_usec < lowest->t_next.tv_usec)
			{
                lowest = a;
            }
        } else if (a->t_next.tv_sec < lowest->t_next.tv_sec)
		{
            lowest = a;
        }
    }
    return lowest;
}

monitor_alarm_t *ma_find_specific(unsigned int clientreg)
{
    monitor_alarm_t *ma_ptr;
    for (ma_ptr = thealarms; ma_ptr != NULL; ma_ptr = ma_ptr->next) 
	{
        if (ma_ptr->clientreg == clientreg) 
		{
            return ma_ptr;
        }
    }
    return NULL;
}

void run_alarms(void)
{
    int             done = 0;
    monitor_alarm_t *a = NULL;
    unsigned int    clientreg;
    struct timeval  t_now;

    /*
     * Loop through everything we have repeatedly looking for the next thing to
     * call until all events are finally in the future again.  
     */
    MONITOR_TRACE();
    while (!done)
	{
        if(NULL == (a = ma_find_next()))
		{
            return;
        }

        get_sys_uptime(&t_now);

        if ((a->t_next.tv_sec < t_now.tv_sec) 
			||((a->t_next.tv_sec == t_now.tv_sec)
			/*&&(a->t_next.tv_usec < t_now.tv_usec)*/)) 
		{
            clientreg = a->clientreg;
            MONITOR_PRINTF("monitor_alarm: run alarm %d\n", clientreg);
            (*(a->thecallback)) (clientreg, a->clientarg);
            MONITOR_PRINTF("monitor_alarm: alarm %d completed\n", clientreg);

            if ((a = ma_find_specific(clientreg)) != NULL)
			{
                a->t_last.tv_sec = t_now.tv_sec;
                a->t_last.tv_usec = t_now.tv_usec;
                a->t_next.tv_sec = 0;
                a->t_next.tv_usec = 0;
                ma_update_entry(a);
            } 
			else 
			{
                MONITOR_DBG_INFO("monitor_alarm: alarm %d deleted itself\n",
                            clientreg);
            }
        } 
		else
		{
            done = 1;
        }
    }
}



void alarm_handler(int a)
{
    (void)a;

	MONITOR_TRACE();
    run_alarms();
    set_an_alarm();
	return;
	
}


unsigned int get_next_alarm_delay_time(struct timeval *delta)
{
    monitor_alarm_t *ma_ptr;
    struct timeval  t_diff, t_now;
	
	MONITOR_TRACE();

    ma_ptr = ma_find_next();

    if (ma_ptr)
	{
        get_sys_uptime(&t_now);

        if ((t_now.tv_sec > ma_ptr->t_next.tv_sec) ||
            ((t_now.tv_sec == ma_ptr->t_next.tv_sec) &&
             (t_now.tv_usec > ma_ptr->t_next.tv_usec))) 
		{
            /*
             * Time has already passed.  Return the smallest possible amount of
             * time.  
             */
            delta->tv_sec = 0;
            delta->tv_usec = 1;
            return ma_ptr->clientreg;
        } 
		else
		{
            /*
             * Time is still in the future.  
             */
            t_diff.tv_sec = ma_ptr->t_next.tv_sec - t_now.tv_sec;
            t_diff.tv_usec = ma_ptr->t_next.tv_usec - t_now.tv_usec;

            while (t_diff.tv_usec < 0) 
			{
                t_diff.tv_sec -= 1;
                t_diff.tv_usec += 1000000;
            }

            delta->tv_sec = t_diff.tv_sec;
            delta->tv_usec = t_diff.tv_usec;
            return ma_ptr->clientreg;
        }
    }

    /*
     * Nothing Left.  
     */
    return 0;
}


void set_an_alarm(void)
{
    struct timeval  delta;
    unsigned int  nextalarm = get_next_alarm_delay_time(&delta);

    /*
     * We don't use signals if they asked us nicely not to.  It's expected
     * they'll check the next alarm time and do their own calling of
     * run_alarms().  
     */

    if (nextalarm )
	{
#ifdef HAVE_SETITIMER
        struct itimerval it;

        it.it_value.tv_sec = delta.tv_sec;
        it.it_value.tv_usec = delta.tv_usec;
        it.it_interval.tv_sec = 0;
        it.it_interval.tv_usec = 0;

        signal(SIGALRM, alarm_handler);
        setitimer(ITIMER_REAL, &it, NULL);
        MONITOR_DBG_INFO(("monitor_alarm: schedule alarm %d in %d.%03d seconds\n",
                    nextalarm, delta.tv_sec, (delta.tv_usec / 1000)));
#else  /* HAVE_SETITIMER */
#ifdef SIGALRM
        signal(SIGALRM, alarm_handler);
        alarm((unsigned int)(delta.tv_sec));
        MONITOR_PRINTF("monitor_alarm :schedule alarm %d in roughly %d seconds\n", nextalarm, delta.tv_sec);
#endif  /* SIGALRM */
#endif  /* HAVE_SETITIMER */

    }
	else
	{
        MONITOR_PRINTF("monitor_alarm: no alarms found to schedule\n");
    }
}


/**
 * This function registers function callbacks to occur at a speciifc time
 * in the future.
 *
 * @param when is an unsigned integer specifying when the callback function
 *             will be called in seconds.
 *
 * @param flags is an unsigned integer that specifies how frequent the callback
 *	function is called in seconds.  Should be SA_REPEAT or 0.  If  
 *	flags  is  set with MA_REPEAT, then the registered callback function
 *	will be called every MA_REPEAT seconds.  If flags is 0 then the 
 *	function will only be called once and then removed from the 
 *	registered alarm list.
 *
 * @param thecallback is a pointer SNMPAlarmCallback which is the callback 
 *	function being stored and registered.
 *
 * @param clientarg is a void pointer used by the callback function.  This 
 *	pointer is assigned to monitor_alarm->clientarg and passed into the
 *	callback function for the client's specifc needs.
 *
 * @return Returns a unique unsigned integer(which is also passed as the first 
 *	argument of each callback), which can then be used to remove the
 *	callback from the list at a later point in the future using the
 *	monitor_alarm_unregister() function.  If memory could not be allocated
 *	for the monitor_alarm struct 0 is returned.
 *
 * @see monitor_alarm_unregister
 * @see monitor_alarm_register_hr
 * @see monitor_alarm_unregister_all
 */
unsigned int monitor_alarm_register(unsigned int when, unsigned int flags,
                    monitor_alarm_callback * thecallback, void *clientarg)
{
    monitor_alarm_t **ma_pptr;
    
    if (thealarms != NULL) 
	{
        for (ma_pptr = &thealarms; (*ma_pptr) != NULL; ma_pptr = &((*ma_pptr)->next))
        {
        }
    }
	else
	{
        ma_pptr = &thealarms;
    }

    *ma_pptr = MONITOR_MALLOC_STRUCT(tag_monitor_alarm);
    if (*ma_pptr == NULL)
	{
		MONITOR_SYS_ERR();
        return 0;
	}

    if (0 == when)
	{
        (*ma_pptr)->t.tv_sec = 0;
        (*ma_pptr)->t.tv_usec = 1;
    }
	else
	{
        (*ma_pptr)->t.tv_sec = (long)(when);
        (*ma_pptr)->t.tv_usec = 0;
    }
    (*ma_pptr)->flags = flags;
    (*ma_pptr)->clientarg = clientarg;
    (*ma_pptr)->thecallback = thecallback;
    (*ma_pptr)->clientreg = regnum++;
    (*ma_pptr)->next = NULL;
    ma_update_entry(*ma_pptr);

    MONITOR_PRINTF("monitor_alarm: registered alarm %d, t = %d.%03d, flags=0x%02x\n",
                (*ma_pptr)->clientreg, (*ma_pptr)->t.tv_sec,
                ((*ma_pptr)->t.tv_usec / 1000), (*ma_pptr)->flags);

    if (start_alarms)
	{
        set_an_alarm();
	}
    return (*ma_pptr)->clientreg;
}

/*****************************************************************************
 功能描述  : 从报文中获取一个32位的整数。
*****************************************************************************/
ULONG GET_INT32U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return ERROR;
    }

    return (pucData[usOffset] << 24) | (pucData[usOffset + 1] << 16) | (pucData[usOffset + 2] << 8) | (pucData[usOffset + 3]);
}
/*****************************************************************************
 功能描述  : 从报文中获取一个16位的整数。
*****************************************************************************/
unsigned short GET_INT16U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return (unsigned short)ERROR;
    }

    return (pucData[usOffset] << 8) | (pucData[usOffset + 1]);
}

/*****************************************************************************
 功能描述  : 从报文中获取一个8位的整数。
*****************************************************************************/
unsigned char GET_INT8U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return (unsigned char)ERROR;
    }

    return pucData[usOffset];
}

#ifdef  __cplusplus
}
#endif


/*****************************************************************************
   Description: ����ϵͳ�����Լ������                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
//#include <linux/delay.h>
#include "stdio.h"
#include <sys/socket.h>
#include "thread.h"
#include "lw_drv_pub.h"
#include "time.h"
#include <unistd.h>
#include "monitor_alarm.h"

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/
#if defined(CONFIG_PRODUCT_EPN104) 
extern void Register_PonStateAlarm(void);
#endif


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
STATIC struct thread_master * g_monitor_master = NULL;
//STATIC struct thread * pstMinitorTimerThread = NULL;

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/


int main(void)
{
    struct thread thread;
    g_monitor_master = thread_master_create();

    Register_PonStateAlarm();
	Syslog_Init();

    /*lint --e{716}*/
    while (1)
    {
        if(thread_fetch(g_monitor_master, &thread))
        {
            thread_call (&thread);
        }
    }
}


/*****************************************************************************
  ��������: ��monitor����Ӷ�ʱ����
*****************************************************************************/
UINT32 Monitor_SetTimer(UINT32 uiInteval, INT32 (*func)(struct thread *))
{
    (void)thread_add_timer (g_monitor_master, func, NULL, (long)uiInteval);
    return 0;
}





#ifdef  __cplusplus
}
#endif


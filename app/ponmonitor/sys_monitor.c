/*****************************************************************************
   Description: 定义系统周期性检查任务                                                                             
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
 * 外部变量说明                                 *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
#if defined(CONFIG_PRODUCT_EPN104) 
extern void Register_PonStateAlarm(void);
#endif


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
STATIC struct thread_master * g_monitor_master = NULL;
//STATIC struct thread * pstMinitorTimerThread = NULL;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
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
  函数描述: 在monitor中添加定时任务
*****************************************************************************/
UINT32 Monitor_SetTimer(UINT32 uiInteval, INT32 (*func)(struct thread *))
{
    (void)thread_add_timer (g_monitor_master, func, NULL, (long)uiInteval);
    return 0;
}





#ifdef  __cplusplus
}
#endif


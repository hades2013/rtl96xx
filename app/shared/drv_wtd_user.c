/*****************************************************************************

      File name:drv_wtd_user.c
      Description:user state of watchdog reboot
      Author:liaohongjun
      Date:
              2012/7/31
==========================================================
      Note:
*****************************************************************************/

#ifdef  __cplusplus
    extern "C"{
#endif

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <drv_wtd_user.h>
#include "shutils.h"
#include <sys/types.h> 
#include <signal.h>

/*begin added by liaohongjun 2012/7/31 of QID0002*/
int Drv_Watchdog_Reboot(void)
{
    int fd = 0;
    int ret = 0; 
    pid_t getPid = 0;

    /*kill app watchdog first*/
    getPid = get_pid_by_name(APP_WTD_NAME);
    if(getPid)
    {
        if (-1 == kill(getPid, SIGKILL))
        {
              printf("kill app_watchdog failed!\n");
              return DRV_WTD_REBOOT_ERR;
        }
    }
    else
    {
        printf("[%s,%d]:Get pid of app_watchdog = %d\n",__FUNCTION__,__LINE__,getPid);
        return DRV_WTD_REBOOT_ERR;
    }
    
    fd = open ( WTD_DEV_NAME, O_RDWR );
    if( fd <0 )
    {
        printf("[%s,%d]:open %s faild!\n",__FUNCTION__,__LINE__,WTD_DEV_NAME);
        return DRV_WTD_REBOOT_ERR;
    }

    /* reboot wtd device */
    ret = ioctl (fd ,(unsigned long )WDIOC_GETTIMEOUT,0);
    
    if( ret < 0 )
    {
        close(fd);
        return DRV_WTD_REBOOT_ERR;
    }

    close(fd);
    return DRV_WTD_REBOOT_OK;
}
/*end added by liaohongjun 2012/7/31 of QID0002*/

#ifdef  __cplusplus
}
#endif



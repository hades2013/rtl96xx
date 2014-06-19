#ifndef _DRV_WTD_USER_H_
#define _DRV_WTD_USER_H_

#ifdef  __cplusplus
extern "C"{
#endif


#define DRV_WTD_REBOOT_OK 0
#define DRV_WTD_REBOOT_ERR -1

#define APP_WTD_NAME "app_watchdog"
#define WTD_DEV_NAME "/dev/mw_wtd_dev"

extern int Drv_Watchdog_Reboot(void);


#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif


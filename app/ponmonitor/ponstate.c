/******************************************************************************
  ��������   : reset ��������
******************************************************************************/

#ifdef  __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <unistd.h>
#include <lw_type.h>
#include <sys/time.h>
#include "monitor.h"
#include "ponstate.h"
#include "lw_drv_pub.h"
#include <lw_drv_req.h>
#include "monitor_alarm.h"	
#include "../shared/systemlog.h"


//#define MONITOR_DBG_INFO printf
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define PONSTATE_MONITOR_REPEAT_TIME     5
#define MAX_PON_ERR_COUNT               6
#define MAX_PON_ALWAY_LASER_COUNT       3

/*begin modified by liaohongjun 2012/11/19 of EPN204QID0008*/
/*****************************************************************************
 ��������  : ����reset������ع���
*****************************************************************************/
void ponState_main(void)
{
    unsigned int uiGpioState      = 0;
    unsigned int uiStatus         = 0;
    unsigned int uiPonErrFlag     = 0;
    unsigned int uiPonOffFlag     = 0;
    unsigned int uiPonAlwaysState = 0;
    unsigned int uiGpioMultiVal   = 0;
    unsigned int uiIntStatus      = 0;
	unsigned int uiLaserOnTestFlag = LASEON_TEST_DISABLE;
    int iRet = 0;
    int count = 0;
	static  char scPonErrCount = 0;
    static  char scPonAlwayLaserCount = 0;
	unsigned int oam_disc_state = 0;


	/*Begin add by huangmingjian 2013/03/19 for EPN204QID0033*/ 
	/* ���������ʱ��ִ�г�������*/
	if (DRV_OK == Ioctl_GetLaserOnTestFlag(&uiLaserOnTestFlag))
    {		
		if(LASEON_TEST_ENABLE == uiLaserOnTestFlag)
		{
			MONITOR_DBG_INFO("Laseron test have be enalbe!");
        	return;	
		}
    }
	else
	{
        MONITOR_DBG_INFO("Read laser on test flag failed!");		
        return;	
	}
	/*End add by huangmingjian 2013/03/19 for EPN204QID0033*/
	
	/*Begin add by huangmingjian 2014/02/23*/ 
	/*when onu is offline,we dont check it in ponmonitor*/
	if(0 != Ioctl_GetOamDiscoveryState(&oam_disc_state))
	{
		MONITOR_DBG_INFO("Get Oam Discovery State failed!");
	}
	if(EPON_OAM_STATE_COMPLETE != oam_disc_state)
	{
		MONITOR_DBG_INFO("onu is offline oam_disc_state=%d", oam_disc_state);
		return;	
	}
	/*End add by huangmingjian 2014/02/23*/ 

    /* ��ģ���Ѿ����ڵ�Դ�ر�״̬��,ʲô������ */
    if (MAX_PON_ALWAY_LASER_COUNT < scPonAlwayLaserCount)
    {
        /*����ϵͳ�澯��*/
        //(void)Ioctl_SetSysLed(DRV_SYS_LED_FILED_OFFSET,0);
        MONITOR_DBG_INFO("Pon has Power DOWN!");
        return;
    }
    if (0 != Ioctl_GetGpioVal(PON_MON_GPIO, &uiGpioState))
    {
        MONITOR_DBG_INFO("Read pon status gpio failed!");
        return;
    }

    /* ����ڷ���,�����жϿ���״̬,���ж������ж� */
    if (DRV_GPIO_VALUE_HIGH == uiGpioState)
    {

	
        /* ��鵱ǰ�ж��Ƿ񴥷��� */
        (void)Ioctl_GetPonIntChangeFlag(&uiStatus);
        if (FALSE == uiStatus)
        {
            /* ���ֳ����� */
            scPonErrCount++;
			USER_LOG(LangId,SYSTEM,LOG_ERR,M_LONGLIGHT,"",""); //add by leijinbao 2013/9/17
            MONITOR_DBG_INFO("Pon status detected always laser on %d !!\n", scPonErrCount);
        }
        else
        {
            /* ����жϴ�����־λ */
            (void)Ioctl_SetPonIntChangeFlag(0);
            /* �����ж� */
         //   (void)Ioctl_SetGpioIntStatus(DRV_GPIO_9_PON_TX, 1);
            scPonErrCount = 0;
            MONITOR_DBG_INFO("Pon status detected normal.\n");
        }

        /* �ﵽ�����������ֵ */
        if (MAX_PON_ERR_COUNT <= scPonErrCount)
        {
            scPonAlwayLaserCount ++;
            //(void)Ioctl_SetPonStatusErrFlag(TRUE);     
            if(MAX_PON_ALWAY_LASER_COUNT < scPonAlwayLaserCount)
            {
                /* �ر��ж� */
              //  (void)Ioctl_SetGpioIntStatus(DRV_GPIO_9_PON_TX, 0);                 
                /*�رչ�ģ���Դ*/
				Ioctl_SetGpioDirOut(PON_VCC_GPIO);
                if(DRV_OK != Ioctl_SetGpioVal(PON_VCC_GPIO, 0))
                {
                    MONITOR_DBG_INFO("Set GPIO_11 low failed.\n");
                }
                
                /*����ϵͳ�澯��*/
              //  (void)Ioctl_SetSysLed(DRV_SYS_LED_FILED_OFFSET,0);
              	 Ioctl_SetGpioDirOut(PON_ALARM_GPIO);
              	 Ioctl_SetGpioVal(PON_ALARM_GPIO, 0);
                /*todo:send trap info*/
                MONITOR_DBG_INFO("Pon status always laser on detected 3 times,Power down PON!\n");
            }     
            else
            {
                /*��ģ���������,������̻ᴥ���ж�һ��*/
				Ioctl_SetGpioDirOut(PON_VCC_GPIO);
                if(DRV_OK != Ioctl_SetGpioVal(PON_VCC_GPIO, 0))
                {
                    MONITOR_DBG_INFO("Set GPIO_11 low failed.\n");
                }
                sleep(5);
				Ioctl_SetGpioDirOut(PON_VCC_GPIO);
                if(DRV_OK != Ioctl_SetGpioVal(PON_VCC_GPIO, 1))
                {
                    MONITOR_DBG_INFO("Set GPIO_11 hight failed.\n");
                } 
                sleep(2);
                MONITOR_DBG_INFO("Pon status always laser on detected!\n");
            }
            scPonErrCount = 0;
        }
    }
    else
    {
        /* �������жϿ�����ر��ж� */
        scPonErrCount = 0;
        scPonAlwayLaserCount = 0;
		#if 0
        (void)Ioctl_GetGpioIntStatus(DRV_GPIO_9_PON_TX, &uiIntStatus);
        if (TRUE == uiIntStatus)
        {
            (void)Ioctl_SetGpioIntStatus(DRV_GPIO_9_PON_TX, 0);
            MONITOR_DBG_INFO("close gpio_9 interrupt.");
        }
		#endif
        MONITOR_DBG_INFO("Pon status detected normal.\n");
    }
    
    return;
}
/*end modified by liaohongjun 2012/11/19 of EPN204QID0008*/

int Monitor_ponStateProc(struct thread *thread)
{
    (void)thread;
    ponState_main();
    Monitor_SetTimer(PONSTATE_MONITOR_REPEAT_TIME, Monitor_ponStateProc);
    return 0;
}

/*****************************************************************************
 ��������  : ע��reset�����Ķ�ʱ�ص�����
*****************************************************************************/
void Register_PonStateAlarm(void)
{
    Monitor_SetTimer(PONSTATE_MONITOR_REPEAT_TIME, Monitor_ponStateProc);
}


#ifdef  __cplusplus
}
#endif


/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  odm_service.c
*
* DESCRIPTION: 
*	
*
* Date Created: Sep 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/odm_service.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "odm_service.h"
#include "log.h"
#include "oam.h"
#include "odm_pon.h"
#include "odm_port.h"
#include "ctc_2_1.h"
#include "stats.h"
#include "hal_global.h"
#include "oam_alarm.h"

#include "lw_drv_pub.h"
#include "lw_drv_req.h"

//wfxu #include "wdt.h"
#if defined(ONU_1PORT)
//wfxu #include "phy.h"
#endif
#ifdef OPL_COM_UART1
//wfxu#include <opl_msg_process.h>
#endif
//wfxu #include "uart.h"
#include "opconn_usr_ioctrl.h"

static VOS_THREAD_t g_pstMpcpThreadId = NULL;
static VOS_THREAD_t g_pstOamTimerThreadId = NULL;
static VOS_THREAD_t g_pstProtocolThreadId = NULL;
static VOS_THREAD_t g_pstLedCtrlThreadId = NULL;
static VOS_THREAD_t g_pstMacCtrlThreadId = NULL;
static VOS_THREAD_t g_pstRegStatusThreadId = NULL;
static VOS_THREAD_t g_pstAlarmThreadId = NULL;
static VOS_THREAD_t g_pstWdtThreadId = NULL;
static VOS_THREAD_t g_pstUart1TestThreadId = NULL;


#ifdef OPL_COM_UART1
/*uart1 communication*/
static VOS_THREAD_t g_pstOnuEventThreadId = NULL;
static VOS_THREAD_t g_pstOnuUart1ProcessThreadId = NULL;
static VOS_THREAD_t g_pstOnuUart1RxThreadId = NULL;
static VOS_THREAD_t g_pstOnuUart1TimerThreadId = NULL;
#endif

#define MODULE MOD_SYS

/*event flag*/
UINT8 nack_event_enbale     = TRUE;
UINT8 register_event_enbale = TRUE;
UINT8 link_event_enbale     = TRUE;
UINT8 link_event_status     = FALSE;
UINT8 nack_event_status     = FALSE;
UINT8 register_event_status = FALSE;

extern OPL_CNT_t g_astCntPon[DAL_CNT_PON_NUM];
extern OPL_CNT_t g_astCntMpcp[DAL_CNT_MPCP_NUM];
extern UINT8 g_onuSilenceEn;
extern UINT32 oam_ver_type;

//#define PHY_STATUS_REG_ADDR 0x11
//#define PHY_SPEED_1000M 	0x2
#define PHY_SPEED_OFFSET 	14
#define PHY_SPEED_MASK 		0x3

#define MAX_REG_REQUEST_TIME (120)

/* INTR L2 definition */
typedef enum ODM_PMAC_INT_TYPE_s
{
	ODM_PMAC_SIGNAL_VAL_INT = 16,
    ODM_PMAC_SYNC_ERR_RX_INT = 17,
    ODM_PMAC_SYNC_ERR_TX_INT =18,
    ODM_PMAC_LINK_CHANGE_INT = 29,
	ODM_PMAC_CRC8_ERR_INT = 30,
	ODM_PMAC_LOS_IND_INT = 31,
	ODM_PMAC_INT_END
}ODM_PMAC_INT_TYPE_e;

UINT8 g_ucLOSInt = 0;
UINT8 g_bPonLos = FALSE;

static TIMER_OBJ_t * g_pstLOSLedTimer = NULL;
static int odmLOSLedTimerCb(void);

/*begin added by liaohongjun 2012/11/12 of EPN104QID0081*/
static TIMER_OBJ_t * g_pstAlwaysLaserAlarmTimer = NULL;

static int odmAlwaysLaserLedAlarmTimerCb(void);
/*end added by liaohongjun 2012/11/12 of EPN104QID0081*/

static TIMER_OBJ_t * g_pstPonLedTimer = NULL;
static int odmPonLedTimerCb(void);

STATUS odmServiceInit()
{
    OPL_EVENT_UNIT_t stEventUnit;
    OPL_RESULT ulRet;
    UINT32 retVal=0;

    /* disable UART1 for ALARM LED */
    /*begin modified by liaohongjun 2012/12/25 of EPN204QID0019*/
    //HAL_API_GBL_REG_FIELD_SET(REG_GPIO_MUX/4, 1, 1, 0,retVal);
    /*end modified by liaohongjun 2012/12/25 of EPN204QID0019*/
    odmAlarmLedControl(FALSE);

    #if 0
    if (OPL_OK != odmUart1TestThreadInit())
	{
		OP_ALARM("Uart1Test task init failed");
		return ERROR;
	}
    
    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_MUX/4, 1, 1, 1,retVal);
    oplRegWrite(REG_GB_UART_MODE,0x01);
    #endif

    

	#if  0
	if (OPL_OK != odmWdtThreadInit())
	{
		OP_ALARM("wdt task init failed");
		return ERROR;
	}
	#endif /* #if 0 */



	if (OK != odmMpcpThreadInit() )
	{
		OP_ALARM("mpcp task init failed");
		return ERROR;
	}

	//OamDbgActiveEnable(1);

	if (OK != odmOamTimerThreadInit())
	{
        OP_ALARM("OAM timer task init failed");
        return ERROR;
	}

#if 0 /*wfxu 0402 will be done*/
#if defined(ONU_1PORT)
#ifdef PHY_TYPE_VSC8641   /*enhanced LED method.*/ 
    mdioRegisterWrite(PHY_DEVICE_ADDR, 31, 1);
	mdioRegisterFieldWrite(PHY_DEVICE_ADDR, 0x11, 4, 1, 1);
	mdioRegisterWrite(PHY_DEVICE_ADDR, 0x10, 0);
	mdioRegisterWrite(PHY_DEVICE_ADDR, 0x11, 0x410);
	mdioRegisterWrite(PHY_DEVICE_ADDR, 31, 0);
#else
    mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_LEDCR, 0, 1, 1);
    mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_LEDCR, 3, 1, 1);
    mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_LEDCR, 12, 3, 1);
#endif



	
	if (OK != odmMacCtrlThreadInit())
	{
		OP_ALARM("MAC control task init faild");
		return ERROR;
	}
#endif 
#endif
/*wfxu 0402 will be done*/

    /* register interrupt event handler */
    stEventUnit.eventId = HAL_EVENT_INT_HANDLING;
    stEventUnit.eventFunc = odmInterruptHandler;
    ulRet = halAppEventRegister(&stEventUnit);

    /*begin modified by liaohongjun 2012/11/12 of EPN104QID0081*/
    #ifdef CONFIG_BOSA
    /*begin modified by liaohongjun 2012/11/19 of EPN204QID0008*/
    /*EPN204 use PON_ACT for system led*/
    HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);
    #if 0
    g_pstAlwaysLaserAlarmTimer = vosTimerGetFree();
    if (g_pstAlwaysLaserAlarmTimer)
    {
        vosTimerStart(g_pstAlwaysLaserAlarmTimer, REOCURRING_TIMER, 
            500, (FUNCPTR)odmAlwaysLaserLedAlarmTimerCb,1,2,3,4,5,6,7,8);
    }
    #endif
    /*end modified by liaohongjun 2012/11/19 of EPN204QID0008*/
    #else
    g_pstLOSLedTimer = vosTimerGetFree();
    if (g_pstLOSLedTimer)
    {
        vosTimerStart(g_pstLOSLedTimer, REOCURRING_TIMER, 
            500, (FUNCPTR)odmLOSLedTimerCb,1,2,3,4,5,6,7,8);
    }
    #endif
    /*end modified by liaohongjun 2012/11/12 of EPN104QID0081*/
    
    g_pstPonLedTimer = vosTimerGetFree();
    if (g_pstPonLedTimer)
    {
        vosTimerStart(g_pstPonLedTimer, REOCURRING_TIMER, 
            500, (FUNCPTR)odmPonLedTimerCb,1,2,3,4,5,6,7,8);
    }

	odmAlarmThreadInit();
	
    return OK;
}

STATUS odmServiceShutdown()
{
	if (OK != odmOamTimerThreadShutdown())
	{
        OP_DEBUG(DEBUG_LEVEL_WARNING, "OAM timer task shutdown failed");
        return ERROR;
	}

    if (OK != odmMpcpThreadShutdown())
	{
        OP_DEBUG(DEBUG_LEVEL_WARNING, "Mpcp task shutdown failed");
        return ERROR;
	}



#if defined(ONU_1PORT)
	if (OK != odmMacCtrlThreadShutdown())
	{
		OP_ALARM("MAC control task shutdown faild");
		return ERROR;
	}
#endif 


    vosTimerCancel(g_pstLOSLedTimer);
    g_pstLOSLedTimer = NULL;

    vosTimerCancel(g_pstPonLedTimer);
    g_pstPonLedTimer = NULL;

    return OK;
}

STATUS odmOamTimerThreadInit(void)
{
    int ret;

    if (g_pstOamTimerThreadId) {
        return ERROR;
    }

    g_pstOamTimerThreadId = vosThreadCreate("tOamTimer", OP_VOS_THREAD_STKSZ, 60,
                        (void *)OamTimerThread, (void *)NULL);
    if (g_pstOamTimerThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmOamTimerThreadShutdown(void)
{
    if (!g_pstOamTimerThreadId) {
        return ERROR;
    }

    if (g_pstOamTimerThreadId != NULL) {
        vosThreadDestroy(g_pstOamTimerThreadId);
    }

    return OK;
}

#ifdef OPL_COM_UART1
static void odmOnuEventThread()
{
    UINT8 buff[10] = {0};	

    ODM_PON_STATUS_t odmPonStatus;
	
	while(1)
	{
	    vosMemSet(&odmPonStatus,0x0,sizeof(ODM_PON_STATUS_t));
		odmPonStatusGet(&odmPonStatus);
		if ((STATE_NACK == odmPonStatus.bfDiscCState)
		   &&(FALSE == nack_event_status))
		{
    		nack_event_status     = TRUE;
    		register_event_status = FALSE;
    		link_event_status     = FALSE;
    		OP_DEBUG(DEBUG_LEVEL_WARNING, "Pon status nack!");
    		/*report nack event*/    		
    		msgNackAndSend(0,buff,10);
    		
		}
		else if ((STATE_REGISTERD == odmPonStatus.bfDiscCState)
		        &&(FALSE == register_event_status))
		{
    		nack_event_status     = FALSE;
    		link_event_status     = FALSE;
    		register_event_status = TRUE;
    		OP_DEBUG(DEBUG_LEVEL_WARNING, "Pon status register!");
    		/*report register event*/
    		msgAunthentiedAndSend(0,buff,10);
		}
        else 
        {
            /*PON LINK FAULT*/
            if ((FALSE == link_event_status)&&
               (STATE_REGISTER_ACK != odmPonStatus.bfDiscCState)&&
               (STATE_NACK != odmPonStatus.bfDiscCState)&&
               (STATE_REGISTERD != odmPonStatus.bfDiscCState))
            {
                OP_DEBUG(DEBUG_LEVEL_WARNING, "Pon status others!");
                nack_event_status     = FALSE;
        		register_event_status = FALSE;
        		link_event_status     = TRUE;
        		msgLinkFaultAndSend(0,buff,10);
    		}
        }
		sleep(1);
	}
}

STATUS odmOnuUart1CommThreadInit(void)
{
    int ret;

    if (OK != msgInit()) 
    {
        return ERROR;
    }
    g_pstOnuEventThreadId = vosThreadCreate("tOnuEvent", OP_VOS_THREAD_STKSZ, 70,
                        (void *)odmOnuEventThread, (void *)NULL);
    if (g_pstOnuEventThreadId == NULL) {
        return ERROR;
    }
    g_pstOnuUart1ProcessThreadId = vosThreadCreate("tOnuUart1Process", OP_VOS_THREAD_STKSZ, 70,
                        (void *)msgProcessThread, (void *)NULL);
    if (g_pstOnuUart1ProcessThreadId == NULL) {
        return ERROR;
    }
    g_pstOnuUart1RxThreadId = vosThreadCreate("tOnuUart1Rx", OP_VOS_THREAD_STKSZ, 70,
                        (void *)msgRxPktThread, (void *)NULL);
    if (g_pstOnuUart1RxThreadId == NULL) {
        return ERROR;
    }
    g_pstOnuUart1TimerThreadId = vosThreadCreate("tOnuUart1Timer", OP_VOS_THREAD_STKSZ, 70,
                        (void *)TimerThread, (void *)NULL);
    if (g_pstOnuUart1TimerThreadId == NULL) {
        return ERROR;
    }
    return OK;
}

STATUS odmOnuUart1CommThreadShutdowm(void)
{
    int ret;

    if (!g_pstOnuEventThreadId) {
        return ERROR;
    } 
    if (g_pstOnuEventThreadId != NULL) {
        vosThreadDestroy(g_pstOnuEventThreadId);
    }

    if (!g_pstOnuUart1ProcessThreadId) {
        return ERROR;
    } 
    if (g_pstOnuUart1ProcessThreadId != NULL) {
        vosThreadDestroy(g_pstOnuUart1ProcessThreadId);
    }

    if (!g_pstOnuUart1RxThreadId) {
        return ERROR;
    } 
    if (g_pstOnuUart1RxThreadId != NULL) {
        vosThreadDestroy(g_pstOnuUart1RxThreadId);
    }

    if (!g_pstOnuUart1TimerThreadId) {
        return ERROR;
    } 
    if (g_pstOnuUart1TimerThreadId != NULL) {
        vosThreadDestroy(g_pstOnuUart1TimerThreadId);
    }
    msgExit();
    return OK;
}
#endif

STATUS odmLOSLedInit(void)
{
    UINT32 retVal;
    UINT32 pmacLOSSingal = 0;

    /* check LOS */
	HAL_API_GBL_REG_FIELD_GET(REG_PMAC_INT/4, 16, 1, pmacLOSSingal, retVal);
	if (0 == pmacLOSSingal)
    {
        #ifdef CONFIG_BOSA
        /*EPN204 use PON_ACT for system led,do nothing*/
        #else
        HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 0, retVal);	
        #endif
        g_ucLOSInt = 1;
    }
    else {
        #ifdef CONFIG_BOSA
        /*EPN204 use PON_ACT for system led,do nothing*/
        #else        
        HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);	
        #endif
        g_ucLOSInt = 0;
    }

    return OK;
}


void odmInterruptHandler(void *pvParam)
{
    HAL_EVENT_INT_HANDLING_t *pstIntEvent = (HAL_EVENT_INT_HANDLING_t *)pvParam;
	INT32  i;
    UINT32 retVal = INVALID_32;
	UINT32 enableStatus = 0;

	if (NULL == pvParam)
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: NULL pointer", __FUNCTION__);
		return;
	}

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
        "%s: interrupt level1 0x%08X\r\n", 
        __FUNCTION__, pstIntEvent->ulModInt);

	for (i = OPL_ZERO; i<OPL_END_INTR; i++)
	{ 
	    if (pstIntEvent->ulModInt & (1<<(i)))
	    {
	        switch (i)
	        {
	      	case HAL_INTR_PMAC:
        		if ((pstIntEvent->ulSubModInt[i] & (1 << ODM_PMAC_SIGNAL_VAL_INT)) &&
                    (pstIntEvent->ulSubModInt[i] & (1 << ODM_PMAC_LOS_IND_INT)))
        		{
					odmPonMpcpFsmHoldoverControlGet(&enableStatus);

                    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
                        "interrupt level2 ODM_PMAC_SIGNAL_VAL_INT & ODM_PMAC_LOS_IND_INT holdover(%d).\r\n", enableStatus);
                    g_ucLOSInt = 0;
    				if (!enableStatus)
    				{
	                    if (g_onuSilenceEn)
	                    {
	                		odmPonRegSet(0x4006,0x4A);
	                    }
	                    else
	                    {
	                		odmPonRegSet(0x4006,0x44);
	                    }
	                }
        		}
                else if (pstIntEvent->ulSubModInt[i] & (1 << ODM_PMAC_LOS_IND_INT))
        		{
                    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
                        "interrupt level2 ODM_PMAC_LOS_IND_INT\r\n");
                    g_ucLOSInt = 1;
        		}
				break;
			default:
				break;
	        }
	    }	
    }
	return;
}
/*begin added by liaohongjun 2012/11/12 of EPN104QID0081*/
#ifdef CONFIG_BOSA
#if 0
int odmAlwaysLaserLedAlarmTimerCb(void)
{
    UINT32 retVal;
    static int counter = 0;
    unsigned int uiPonErrFlag = 0;
    int iRet = 0;
    
    /*g_ucLOSInt set to 0 when LOS Singal was detected*/
    if (0 == g_ucLOSInt)
    {
        iRet = Ioctl_GetPonStatusErrFlag(&uiPonErrFlag);
        /* 长发光状态,sys LED 告警 */
        if ((DRV_OK == iRet) && (TRUE == uiPonErrFlag))
        {  
            counter++;
            if (counter % 2 == 0){
                HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 0, retVal);	
                counter = 0;
            }
            else {
                HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);	
            }
        }  
        else
        {
            HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);
        }
    }
    else
    {
        HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);
    }    
    
    return 0;
}
#endif
#endif
/*end added by liaohongjun 2012/11/12 of EPN104QID0081*/

int odmLOSLedTimerCb(void)
{
    UINT32 retVal, fieldData;
	ODM_PON_STATUS_t odmPonStatus;
    static int counter = 0;

#if defined(ONU_1PORT_V1E_RF100_GNA) || defined(ONU_4PORT_V1E_RF400_GNA)
	HAL_API_GBL_REG_FIELD_GET(REG_GPIO_SET/4, GPIO_TX_POWER_CTRL, 1, fieldData, retVal);
	if (0 == fieldData)
	{
		HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 0, retVal);
		return;
	}
#endif
    if (g_ucLOSInt)
    {
		odmPonStatusGet(&odmPonStatus);

		/* avoid g_ucLOSInt is not correct, plus add pon status check */
		if (STATE_REGISTERD == odmPonStatus.bfDiscCState)
		{
			HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);	
		}
		else
		{
	        counter++;
	        if (counter % 2 == 0){
	            HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 0, retVal);	
	            counter = 0;
	        }
	        else {
	            HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);	
	        }
		}            
    }
    else {
		HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 8, 1, 1, retVal);	
    }

    return 0;
}

int odmPonLedTimerCb(void)
{
    UINT32 retVal;
	UINT8 oamStatus = 0;
    static int counter = 0;
	ODM_PON_STATUS_t odmPonStatus;
    UINT32 enableStatus = 0;
	UINT8 uiLos = TRUE;

	OamStatusGet(&oamStatus);
    odmPonMpcpFsmHoldoverControlGet(&enableStatus);

	if (EX_DISCOVERY_SEND_ANY == oamStatus && 0 == g_ucLOSInt)
	{
		HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 0, 1, 0, retVal);	
	}
	else if ((DISCOVERY_FAULT < oamStatus && oamStatus < EX_DISCOVERY_SEND_ANY) &&
        0 == g_ucLOSInt)
	{
        counter++;
        if (counter % 2 == 0){
		    HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 0, 1, 1, retVal);	
            counter = 0;
        }
        else {
		    HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 0, 1, 0, retVal);	
        }
	}
	else {
		odmPonStatusGet(&odmPonStatus);
		if (TRUE == enableStatus && 
	        STATE_REGISTERD == odmPonStatus.bfDiscCState) 
		{
			HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 0, 1, 0, retVal);	
		}
		else
		{
			HAL_API_GBL_REG_FIELD_SET(REG_CP_LED_CTRL/4, 0, 1, 1, retVal);
			uiLos = FALSE;
		}
	}

	if (uiLos != g_bPonLos)
	{
	    g_bPonLos = uiLos;
	}
    return 0;
}

STATUS odmAlarmLedControl(UINT8 ucOn)
{
    UINT32 retVal;

    if (ucOn)
    {
        HAL_API_GBL_REG_FIELD_SET(REG_GPIO_DIRECTION/4, 5, 1, 1, retVal);
    	HAL_API_GBL_REG_FIELD_SET(REG_GPIO_CLEAR/4, 5, 1, 1, retVal);
        HAL_API_GBL_REG_FIELD_SET(REG_GPIO_SET/4, 5, 1, 0, retVal);
    }
    else
    {
        HAL_API_GBL_REG_FIELD_SET(REG_GPIO_DIRECTION/4, 5, 1, 1, retVal);
    	HAL_API_GBL_REG_FIELD_SET(REG_GPIO_CLEAR/4, 5, 1, 1, retVal);
        HAL_API_GBL_REG_FIELD_SET(REG_GPIO_SET/4, 5, 1, 1, retVal);
    }
}

static void odmMacCtrlThread(void)
{
    HAL_API_GBL_MDIO_REG_t stMdioRegParam;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;
    HAL_API_GBL_OWR_t stGblRegParam;
	OPL_API_DATA_t stApiData;
	UINT32 curSpeed;
	UINT32 oldSpeed;
    UINT32 adminState = 0;
	UINT32 autonegState = 0;
    UINT32 portState;
    UINT32 oldhwPhyState, hwPhyState;
	
	UINT32 curDuplex;
	UINT32 oldDuplex;
	UINT32 duplexcValue;

	if (IS_ONU_RESPIN)
	{
		dalPortSpecificDuplexRead(1, &oldDuplex);
	}

#if defined(ONU_1PORT)

	odmPortLinkStateGet(1, &oldhwPhyState);
	odmPortSpeedGet(1, &oldSpeed);
    //dalPortSpecificSpeedRead(1, &oldSpeed);

	while(1)
	{
        odmPortAutoEnableGet(1, &adminState);
        if (1 == adminState)
        {			
			/* read PHY 0x11 register, and check PHY speed */
			if (OPL_OK == dalPortSpecificSpeedRead(1, &curSpeed) && oldSpeed != curSpeed)
			{			
				dalPhySpecificAutonegRead(1, &autonegState);
				if (DAL_AUTONEG_COMPLETED == autonegState) /*bug3278: autonegotiation with the partner of only 100Mbps speed.*/
				{   
					//printf("Speed old %d cur %d.\n", oldSpeed,curSpeed);
	    			/* instead of directly writing register, call the function from dal layer that can do something.*/
	    		    stApiData.apiId = HAL_API_PORT_SPEED_SET;
			        stApiData.length = sizeof(HAL_PORT_CFG_INFO_t);
			        halPortCfgInfo.portId = 1;
			        halPortCfgInfo.speed = curSpeed;
			        stApiData.param = (void *)&halPortCfgInfo;
			        halAppApiCallSync(&stApiData);

					oldSpeed = curSpeed;
				}
    		}
			
			if (IS_ONU_RESPIN)
			{
				if (OPL_OK == dalPortSpecificDuplexRead(1, &curDuplex) && oldDuplex != curDuplex)
				{
					switch(curDuplex)
					{
						case DAL_HALF_DUPLEX:
							duplexcValue = ODM_PORT_DUPLEX_HALF;
							break;
						case DAL_FULL_DUPLEX:
							duplexcValue = ODM_PORT_DUPLEX_FULL;
							break;	
						default: 
					       OPL_LOG_TRACE();
							break;
					}
					//printf("Duplex old %d cur %d.\n", oldDuplex,curDuplex);
					/* instead of directly writing register, call the function from dal layer that can do something.*/
				   	stApiData.apiId = HAL_API_PORT_DUPLEX_SET;
			        stApiData.length = sizeof(HAL_PORT_CFG_INFO_t);
			        halPortCfgInfo.portId = 1;
			        halPortCfgInfo.duplex = duplexcValue;
			        stApiData.param = (void *)&halPortCfgInfo;
			        halAppApiCallSync(&stApiData);

					oldDuplex = curDuplex; 
				}	
	        }

        }
		
		/* bug3272:flush the dynamic fdb with the state from up to down.*/
		odmPortAdminGet(1, &portState);
		if (TRUE == portState){
			odmPortLinkStateGet(1, &hwPhyState);
			if (oldhwPhyState && !hwPhyState){ /*up-->down*/
				//printf("%s, flush the dynamic fdb\n", __FUNCTION__);
				dalArlFlushDynamic();
			}
		    oldhwPhyState = hwPhyState;
		}
		sleep(1);
	}
#endif

    vosThreadExit(0);
}

STATUS odmMacCtrlThreadInit(void)
{
    int ret;
    UINT32 retVal = INVALID_32;

    if (g_pstMacCtrlThreadId) {
        return ERROR;
    }

    g_pstMacCtrlThreadId = vosThreadCreate("tMacCtrl", OP_VOS_THREAD_STKSZ, 60,
                        (void *)odmMacCtrlThread, (void *)NULL);
    if (g_pstMacCtrlThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmMacCtrlThreadShutdown(void)
{
    UINT32 retVal = INVALID_32;

    if (!g_pstMacCtrlThreadId) {
        return ERROR;
    }

    if (g_pstMacCtrlThreadId != NULL) {
        vosThreadDestroy(g_pstMacCtrlThreadId);
    }

    return OK;
}


STATUS odmMpcpThreadInit(void)
{
    int ret;

    if (g_pstMpcpThreadId) {
        return ERROR;
    }

    g_pstMpcpThreadId = vosThreadCreate("tMpcpCtrl", OP_VOS_THREAD_STKSZ, 40,
                        (void *)odmMpcpCtrlThread, (void *)NULL);
    if (g_pstMpcpThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmMpcpThreadShutdown(void)
{
    UINT32 retVal = INVALID_32;

    if (!g_pstMpcpThreadId) {
        return ERROR;
    }

    if (g_pstMpcpThreadId != NULL) {
        vosThreadDestroy(g_pstMpcpThreadId);
    }

    return OK;
}

void oam_alarm_pthread(void)
{
	InitOamAlarmParam();
    OamClrPeirodStats();
	
	while(1)
	{
		vosSleep(OAM_ALARM_TIME);
		OamPeriodHandle();
	}
	return;
}


/*add for alarm*/
static void odmAlarmThread()
{
	oam_alarm_pthread();
}

STATUS odmAlarmThreadInit(void)
{
    if (g_pstAlarmThreadId) {
        return ERROR;
    }

    g_pstAlarmThreadId = vosThreadCreate("tAlarm", OP_VOS_THREAD_STKSZ, 90,
                        (void *)odmAlarmThread, (void *)NULL);
    if (g_pstAlarmThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmAlarmThreadShutdown(void)
{
    if (!g_pstAlarmThreadId) {
        return ERROR;
    }

    if (g_pstAlarmThreadId != NULL) {
        vosThreadDestroy(g_pstAlarmThreadId);
        g_pstAlarmThreadId = NULL;
    }

    return OK;
}

#define WDT_FEED_SECOND (2)

static void odmWdtCtrlThread(void)
{
	while(1)
	{
		oplWdtFeed();

		vosSleep(1);
	}
}

STATUS odmWdtThreadInit(void)
{
    int ret;


    if (g_pstWdtThreadId) {
        return ERROR;
    }

	if (OPL_OK != oplWdtInit(124, WDT_FEED_SECOND*1000000, 1)){
		return ERROR;
	}

    g_pstWdtThreadId = vosThreadCreate("tWdtCtrl", OP_VOS_THREAD_STKSZ, 70,
                        (void *)odmWdtCtrlThread, (void *)NULL);
	
    if (g_pstWdtThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmWdtThreadShutdown(void)
{
    UINT32 retVal = INVALID_32;

    if (!g_pstWdtThreadId) {
        return ERROR;
    }

    if (g_pstWdtThreadId != NULL) {
        vosThreadDestroy(g_pstWdtThreadId);
    }

    return OK;
}

#if 0 /*wfxu 0402 not need*/
STATUS odmUart1TestThreadInit(void)
{
    int ret;


    if (g_pstUart1TestThreadId) {
        return ERROR;
    }

	if (OPL_OK != serial_init()){
		return ERROR;
	}

     
    
    g_pstUart1TestThreadId = vosThreadCreate("tUart1Test", OP_VOS_THREAD_STKSZ, 70,
                        (void *)uart1Test, (void *)NULL);
	
    if (g_pstUart1TestThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS odmUart1TestThreadShutdown(void)
{
    UINT32 retVal = INVALID_32;

    if (!g_pstUart1TestThreadId) {
        return ERROR;
    }

    if (g_pstUart1TestThreadId != NULL) {
        vosThreadDestroy(g_pstUart1TestThreadId);
    }

    return OK;
}

#endif /*wfxu 0402*/


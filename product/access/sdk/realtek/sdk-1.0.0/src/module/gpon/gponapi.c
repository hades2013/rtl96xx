/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: 41095 $
 * $Date: 2013-07-11 23:54:49 +0800 (Thu, 11 Jul 2013) $
 *
 * Purpose : GPON API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

/*
 * Include Files
 */

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#ifdef CONFIG_GPON_FEATURE
#include "re8686.h"
#endif

#endif

#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <rtk/irq.h>
#include <rtk/classify.h>
#include <rtk/ponmac.h>
#include <rtk/gponv2.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <rtk/cpu_tag.h>
#endif
#include <module/gpon/gpon_platform.h>
#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_res.h>
#include <module/gpon/gpon_fsm.h>
#include <module/gpon/gpon_ploam.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_debug.h>
#include <module/gpon/gpon_pm.h>
#include <module/gpon/gpon_int.h>
#include <module/gpon/gpon.h>



/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)

int32  rtk_gponapp_omci_rx_wrapper(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo);

 #ifdef CONFIG_GPON_FEATURE
extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
#endif

#endif

gpon_drv_obj_t *g_gponmac_drv = NULL;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


/* Module Name    : GPON     */


/* Function Name:
 *      rtk_gponapp_driver_initialize
 * Description:
 *      GPON MAC Drv Initialization.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The first function call for GPON MAC Drv.
 */
int32
rtk_gponapp_driver_initialize(void)
{
	uint32 data;
	int32  ret;

	if(g_gponmac_drv)
	{
	   rtk_gponapp_device_deInitialize();
	   rtk_gponapp_driver_deInitialize();
	}

	if((ret = rtk_gpon_init())!=RT_ERR_OK)
	{
		 RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    	  return ret;
	}
	if((ret = rtk_gpon_portMacForceMode_set(PORT_LINKDOWN))!=RT_ERR_OK)
	{
			RT_ERR(ret,(MOD_GPON | MOD_DAL),"");
			return ret;
	}


	g_gponmac_drv = (gpon_drv_obj_t*)GPON_OS_Malloc(sizeof(gpon_drv_obj_t));

	g_gponmac_drv->status = GPON_DRV_STATUS_INIT;
	g_gponmac_drv->dev = NULL;

	/* FSM Handler Initialization */
	gpon_fsm_init();

	/* Start the GMAC thread here */
	g_gponmac_drv->lock = GPON_OS_CreateLock();


	/* set the GPON mode enable */
	if((ret = rtk_ponmac_mode_set(PONMAC_MODE_GPON))!=RT_ERR_OK)
	{
		  RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    	  return ret;
	}

	/* DBRu period */
	data = 1;
	if((ret = rtk_gpon_dbruPeriod_set(data))!=RT_ERR_OK)
	{
		  RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    	  return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_driver_deInitialize
 * Description:
 *      To Stop the GPON MAC Drv. The last function call for GPON MAC Drv.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The Device should be de-initialized before this function call.
 */
int32 rtk_gponapp_driver_deInitialize(void)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(g_gponmac_drv->status!=GPON_DRV_STATUS_INIT || g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_ONU_ALREADY_REGISTERED;
    }

    if(g_gponmac_drv->lock)
    {
        GPON_OS_DestroyLock(g_gponmac_drv->lock);
    }

    GPON_OS_Free(g_gponmac_drv);
    g_gponmac_drv = NULL;
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_device_initialize
 * Description:
 *      GPON MAC Device Initialization. To start the device of the GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      This function should be called after the Drv Initialization and before any other operation.
 *      It should be called after Device is de-initialized but the Drv is not be de-initialized.
 */
int32 rtk_gponapp_device_initialize(void)
{
	int32 ret;
	rtk_port_t pon;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if((ret = rtk_gpon_port_get(&pon))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}

	/*force change cf port to pon and enable it*/
	if((ret = rtk_classify_cfSel_set(pon,CLASSIFY_CF_SEL_ENABLE))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}

	if(g_gponmac_drv->status!=GPON_DRV_STATUS_INIT || g_gponmac_drv->dev)
	{
	    return RT_ERR_GPON_ONU_ALREADY_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	g_gponmac_drv->dev = (gpon_dev_obj_t*)GPON_OS_Malloc(sizeof(gpon_dev_obj_t));

	ret = gpon_dev_initialize(g_gponmac_drv->dev,0);
	if(ret==RT_ERR_OK)
	{
		g_gponmac_drv->status = GPON_DRV_STATUS_READY;
	}
	else
	{
		GPON_OS_Free(g_gponmac_drv->dev);
		g_gponmac_drv->dev = NULL;
		g_gponmac_drv->status=GPON_DRV_STATUS_INIT;

	}
	GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_device_deInitialize
 * Description:
 *      GPON MAC Device De-Initialization. To stop the device of the GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the Drv is de-initialized and the GPON MAC is not activated.
 */
int32 rtk_gponapp_device_deInitialize(void)
{
	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
	{
	    return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
	{
	    gpon_dev_deInitialize(g_gponmac_drv->dev);
	    g_gponmac_drv->status = GPON_DRV_STATUS_INIT;
	}

	GPON_OS_Free(g_gponmac_drv->dev);
	g_gponmac_drv->dev = NULL;

	GPON_OS_Unlock(g_gponmac_drv->lock);
	return RT_ERR_OK;
}



/* Function Name:
 *      rtk_gponapp_evtHdlStateChange_reg
 * Description:
 *      This function is called to register the callback function of the State Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlStateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->state_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlDsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the D/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlDsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->dsfec_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlUsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the U/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlUsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usfec_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlUsPloamUrgEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM urgent queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlUsPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usploam_urg_epty_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlUsPloamNrmEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM normal queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlUsPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usploam_nrm_epty_callback= func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlPloam_reg
 * Description:
 *      This function is called to register the event handler of the Rx PLOAM.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlPloam_reg(rtk_gpon_eventHandleFunc_ploam_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->ploam_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlPloam_dreg
 * Description:
 *      This function is called to deregister the event handler of the Rx PLOAM.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlPloam_dreg(void)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->ploam_callback = NULL;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_evtHdlOmci_reg
 * Description:
 *      This function is called to register the event handler of the Rx OMCI.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlOmci_reg(rtk_gpon_eventHandleFunc_omci_t func)
{
	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);
	g_gponmac_drv->dev->omci_callback = func;
	GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_callbackQueryAesKey_reg
 * Description:
 *      This function is called to register the callback function of the AES Key Query.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_callbackQueryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func)
{
	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);
	g_gponmac_drv->dev->aeskey_callback = func;
	GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_evtHdlAlarm_reg
 * Description:
 *      This function is called to register the alarm event handler of the alarm.
 * Input:
 *      alarmType       - the alarm type
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlAlarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(alarmType>=RTK_GPON_ALARM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->fault_callback[alarmType] = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_serialNumber_set
 * Description:
 *      GPON MAC Set Serial Number.
 * Input:
 *      pSN             - the pointer of Serial Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gponapp_serialNumber_set(rtk_gpon_serialNumber_t *pSN)
{
   if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(!pSN)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_sn_set(g_gponmac_drv->dev,pSN);

    GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_serialNumber_get
 * Description:
 *      GPON MAC get Serial Number.
 * Input:
 *      None
 * Output:
 *      pSN             - the pointer of Serial Number
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gponapp_serialNumber_get(rtk_gpon_serialNumber_t *pSN)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pSN)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_sn_get(g_gponmac_drv->dev,pSN);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_password_set
 * Description:
 *      GPON MAC set Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gponapp_password_set(rtk_gpon_password_t *pPwd)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(!pPwd)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_pwd_set(g_gponmac_drv->dev,pPwd);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_password_get
 * Description:
 *      GPON MAC get Password.
 * Input:
 *      None
 * Output:
 *      pPwd             - the pointer of Password
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gponapp_password_get(rtk_gpon_password_t *pPwd)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pPwd)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_pwd_get(g_gponmac_drv->dev,pPwd);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_parameter_set
 * Description:
 *      GPON MAC set parameters.
 * Input:
 *      type            - the parameter type
 *      pPara           - the pointer of Parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gponapp_parameter_set(rtk_gpon_patameter_type_t type, void *pPara)
{
    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PARA_TYPE_ONU_ACTIVATION:
        {
            gpon_dev_para_onu_set(g_gponmac_drv->dev,(rtk_gpon_onu_activation_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_LASER:
        {
            gpon_dev_para_laser_set(g_gponmac_drv->dev,(rtk_gpon_laser_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PHY:
        {
            gpon_dev_para_dsPhy_set(g_gponmac_drv->dev,(rtk_gpon_ds_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PLOAM:
        {
            gpon_dev_para_dsPloam_set(g_gponmac_drv->dev,(rtk_gpon_ds_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_BWMAP:
        {
            gpon_dev_para_dsBwmap_set(g_gponmac_drv->dev,(rtk_gpon_ds_bwMap_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_GEM:
        {
            gpon_dev_para_dsGem_set(g_gponmac_drv->dev,(rtk_gpon_ds_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_ETH:
        {
            gpon_dev_para_dsEth_set(g_gponmac_drv->dev,(rtk_gpon_ds_eth_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_OMCI:
        {
            gpon_dev_para_dsOmci_set(g_gponmac_drv->dev,(rtk_gpon_ds_omci_para_t*)pPara);
            break;
        }
#if 0
        case RTK_GPON_PARA_TYPE_DS_TDM:
        {
            gpon_dev_para_dsTdm_set(g_gponmac_drv->dev,(rtk_gpon_ds_tdm_para_t*)pPara);
            break;
        }
#endif
        case RTK_GPON_PARA_TYPE_US_PHY:
        {
            gpon_dev_para_usPhy_set(g_gponmac_drv->dev,(rtk_gpon_us_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_PLOAM:
        {
            gpon_dev_para_usPloam_set(g_gponmac_drv->dev,(rtk_gpon_us_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_DBR:
        {
            gpon_dev_para_usDbr_set(g_gponmac_drv->dev,(rtk_gpon_us_dbr_para_t*)pPara);
            break;
        }
        default:
        {
            break;
        }
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_parameter_get
 * Description:
 *      GPON MAC get parameters, which is set by rtk_gpon_parameter_set.
 * Input:
 *      type            - the parameter type
 * Output:
 *      pPara           - the pointer of Parameter
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_parameter_get(rtk_gpon_patameter_type_t type, void *pPara)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PARA_TYPE_ONU_ACTIVATION:
        {
            gpon_dev_para_onu_get(g_gponmac_drv->dev,(rtk_gpon_onu_activation_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_LASER:
        {
            gpon_dev_para_laser_get(g_gponmac_drv->dev,(rtk_gpon_laser_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PHY:
        {
            gpon_dev_para_dsPhy_get(g_gponmac_drv->dev,(rtk_gpon_ds_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PLOAM:
        {
            gpon_dev_para_dsPloam_get(g_gponmac_drv->dev,(rtk_gpon_ds_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_BWMAP:
        {
            gpon_dev_para_dsBwmap_get(g_gponmac_drv->dev,(rtk_gpon_ds_bwMap_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_GEM:
        {
            gpon_dev_para_dsGem_get(g_gponmac_drv->dev,(rtk_gpon_ds_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_ETH:
        {
            gpon_dev_para_dsEth_get(g_gponmac_drv->dev,(rtk_gpon_ds_eth_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_OMCI:
        {
            gpon_dev_para_dsOmci_get(g_gponmac_drv->dev,(rtk_gpon_ds_omci_para_t*)pPara);
            break;
        }
#if 0
        case RTK_GPON_PARA_TYPE_DS_TDM:
        {
            gpon_dev_para_dsTdm_get(g_gponmac_drv->dev,(rtk_gpon_ds_tdm_para_t*)pPara);
            break;
        }
#endif
        case RTK_GPON_PARA_TYPE_US_PHY:
        {
            gpon_dev_para_usPhy_get(g_gponmac_drv->dev,(rtk_gpon_us_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_PLOAM:
        {
            gpon_dev_para_usPloam_get(g_gponmac_drv->dev,(rtk_gpon_us_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_DBR:
        {
            gpon_dev_para_usDbr_get(g_gponmac_drv->dev,(rtk_gpon_us_dbr_para_t*)pPara);
            break;
        }
        default:
        {
            break;
        }
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState       - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is working now.
 */
int32 rtk_gponapp_activate(rtk_gpon_initialState_t initState)
{
	int32 ret;
	rtk_port_t pon;
	int32 ponPort;

	rtk_gpon_port_get(&pon);
	ponPort =1<< pon;
	/*disable imr for gpon interrupt before register isr*/
	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
	{
	    return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
	}

	if(initState>RTK_GPONMAC_INIT_STATE_O7)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}


	if((ret= rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
	    return ret ;
	}

#if defined(CONFIG_SDK_KERNEL_LINUX)
	/*register gpon isr*/
	if((ret = rtk_irq_isr_register(INTR_TYPE_GPON,gpon_isr_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif
	/*change state to drv*/
	GPON_OS_Lock(g_gponmac_drv->lock);
	g_gponmac_drv->status = GPON_DRV_STATUS_ACTIVATE;

	gpon_dev_activate(g_gponmac_drv->dev,initState);
	GPON_OS_Unlock(g_gponmac_drv->lock);

	/* clear switch interrupt state for GPON*/
	if((ret = rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	/* switch interrupt mask for GPON */
	if((ret = rtk_intr_imr_set(INTR_TYPE_GPON,ENABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}


#ifdef CONFIG_GPON_FEATURE
	/*register omci rx handler*/
	if((ret = drv_nic_register_rxhook(ponPort,RE8686_RXPRI_OMCI,&rtk_gponapp_omci_rx_wrapper))!=RT_ERR_OK){
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif
	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_deActivate
 * Description:
 *      GPON MAC de-Activate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is out of work now.
 */
int32 rtk_gponapp_deActivate(void)
{
	int32 ret;
	rtk_port_t pon;
	int32 ponPort;

	if((ret = rtk_gpon_port_get(&pon))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	ponPort =1<< pon;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

#ifdef CONFIG_GPON_FEATURE
	/*register omci rx handler*/
	if((ret = drv_nic_unregister_rxhook(ponPort,RE8686_RXPRI_OMCI,&rtk_gponapp_omci_rx_wrapper))!=RT_ERR_OK){
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif
	/*disable imr for gpon interrupt before register isr*/
	if((ret= rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
	    	return ret ;
	}
#if defined(CONFIG_SDK_KERNEL_LINUX)
	/*unregister gpon isr*/
	if((ret = rtk_irq_isr_unregister(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	/*unregister DyingGasp isr*/
	if((ret = rtk_irq_isr_unregister(INTR_TYPE_DYING_GASP)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif

	/* switch interrupt mask for GPON */
	if((ret = rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
	{
	    return RT_ERR_GPON_ONU_NOT_ACTIVATED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	gpon_dev_deactivate(g_gponmac_drv->dev);
	g_gponmac_drv->status = GPON_DRV_STATUS_READY;

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_ponStatus_get
 * Description:
 *      GPON MAC Get PON Status.
 * Input:
 *      None
 * Output:
 *      pStatus         - pointer of status
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
int32 rtk_gponapp_ponStatus_get(rtk_gpon_fsm_status_t *pStatus)
{

	if(!pStatus)
	{
	    return RT_ERR_NULL_POINTER;
	}

	*pStatus = RTK_GPONMAC_FSM_STATE_UNKNOWN;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
	{
	    return RT_ERR_GPON_ONU_NOT_ACTIVATED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	gpon_dev_get_status(g_gponmac_drv->dev,pStatus);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_isr_entry
 * Description:
 *      GPON MAC ISR entry
 * Input:
 *      data            - user data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called in interrupt process or a polling thread
 */
void rtk_gponapp_isr_entry(void)
{
	int32  ret;

	/* switch interrupt clear GPON mask */
	 gpon_isr_entry();

	/* switch interrupt clear GPON state */
	if((ret=rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    	return ;
	}

	GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"GPON ISR finished!", ret);

	return;
}

/* Function Name:
 *      rtk_gponapp_tcont_create
 * Description:
 *      GPON MAC Create a TCont by assigning an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A TCont ID is returned in pAttr.
 */
int32 rtk_gponapp_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if((!pInd)||(!pAttr))
	{
	    return RT_ERR_NULL_POINTER;
	}

	if(pInd->alloc_id>GPON_DEV_MAX_ALLOC_ID)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	if(pInd->type>RTK_GPON_TCONT_TYPE_5 || pInd->type<RTK_GPON_TCONT_TYPE_1)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_tcont_physical_add(g_gponmac_drv->dev,pInd->alloc_id,pAttr);
	if(ret == RT_ERR_OK)
	ret = gpon_dev_tcont_logical_add(g_gponmac_drv->dev,pInd,pAttr);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_tcont_destroy
 * Description:
 *      GPON MAC Remove a TCont.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_tcont_destroy(rtk_gpon_tcont_ind_t *pInd)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
		return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
		return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pInd)
	{
		return RT_ERR_NULL_POINTER;
	}

	if(pInd->alloc_id>GPON_DEV_MAX_ALLOC_ID)
	{
		return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_tcont_logical_del(g_gponmac_drv->dev,pInd);
	ret = gpon_dev_tcont_physical_del(g_gponmac_drv->dev,pInd->alloc_id);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_tcont_get
 * Description:
 *      GPON MAC Get a TCont with an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The TCont ID is returned in pAttr.
 */
int32 rtk_gponapp_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if((!pInd)||(!pAttr))
	{
	    return RT_ERR_NULL_POINTER;
	}

	if(pInd->alloc_id> GPON_DEV_MAX_ALLOC_ID)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_tcont_logical_get(g_gponmac_drv->dev,pInd,pAttr);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_dsFlow_set
 * Description:
 *      GPON MAC set a D/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pAttr)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(flowId>=GPON_DEV_MAX_FLOW_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(pAttr->gem_port_id>GPON_DEV_MAX_GEMPORT_ID && pAttr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        return RT_ERR_OUT_OF_RANGE;
    }
    else if(pAttr->gem_port_id<=GPON_DEV_MAX_GEMPORT_ID)
    {
        if(pAttr->type>RTK_GPON_FLOW_TYPE_TDM)
        {
            return RT_ERR_OUT_OF_RANGE;
        }
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    if(pAttr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        ret = gpon_dev_dsFlow_add(g_gponmac_drv->dev,flowId,pAttr);
    }
    else
    {
        ret = gpon_dev_dsFlow_del(g_gponmac_drv->dev,flowId);
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_dsFlow_get
 * Description:
 *      GPON MAC get a D/S flow.
 * Input:
 *      flowId          - the flow id
 * Output:
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
	 int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pAttr)
	{
	    return RT_ERR_NULL_POINTER;
	}

	if(flowId>=GPON_DEV_MAX_FLOW_NUM)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_dsFlow_get(g_gponmac_drv->dev,flowId, pAttr);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_usFlow_set
 * Description:
 *      GPON MAC set a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pAttr)
	{
	    return RT_ERR_NULL_POINTER;
	}

	if(flowId>=GPON_DEV_MAX_FLOW_NUM)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	if(pAttr->gem_port_id>GPON_DEV_MAX_GEMPORT_ID && pAttr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}
	else if(pAttr->gem_port_id<=GPON_DEV_MAX_GEMPORT_ID)
	{
	    if(pAttr->type>RTK_GPON_FLOW_TYPE_TDM)
	    {
	        return RT_ERR_OUT_OF_RANGE;
	    }

	    if(pAttr->tcont_id>=GPON_DEV_MAX_TCONT_NUM)
	    {
	        return RT_ERR_OUT_OF_RANGE;
	    }

	    if(pAttr->type==RTK_GPON_FLOW_TYPE_TDM)
	    {
	        if(pAttr->channel>=GPON_DEV_MAX_TDM_CHANNEL_NUM)
	        {
	            return RT_ERR_OUT_OF_RANGE;
	        }
	    }
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	if(pAttr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
	{
	    ret = gpon_dev_usFlow_add(g_gponmac_drv->dev,flowId,pAttr);
	}
	else
	{
	    ret = gpon_dev_usFlow_del(g_gponmac_drv->dev,flowId);
	}

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_usFlow_get
 * Description:
 *      GPON MAC get a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	if(!pAttr)
	{
	    return RT_ERR_NULL_POINTER;
	}

	if(flowId>=GPON_DEV_MAX_FLOW_NUM)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_usFlow_get(g_gponmac_drv->dev,flowId,pAttr);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_ploam_send
 * Description:
 *      GPON MAC Send a PLOAM in upstream.
 * Input:
 *      urgent          - specify it is a urgent(1) or normal(0) PLOAM message
 *      pPloam          - the pointer of PLOAM message
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A error is returned if the PLOAM is not sent.
 */
int32 rtk_gponapp_ploam_send(int32 urgent, rtk_gpon_ploam_t *pPloam)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pPloam)
	{
	    return RT_ERR_NULL_POINTER;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_ploam_tx(g_gponmac_drv->dev,urgent,pPloam);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_broadcastPass_set
 * Description:
 *      GPON MAC set the broadcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the broadcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_broadcastPass_set(int32 mode)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_bcastPass_set(g_gponmac_drv->dev,mode);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_broadcastPass_get
 * Description:
 *      GPON MAC get the broadcast pass mode.
 * Input:
 *      pMode           - the pointer of broadcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_broadcastPass_get(int32 *pMode)
{
   int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pMode)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_bcastPass_get(g_gponmac_drv->dev, pMode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_nonMcastPass_set
 * Description:
 *      GPON MAC set the non-multidcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the non-multidcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_nonMcastPass_set(int32 mode)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
		return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
		return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_nonMcastPass_set(g_gponmac_drv->dev,mode);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_nonMcastPass_get
 * Description:
 *      GPON MAC get the non-multidcast pass mode.
 * Input:
 *      pMode           - the pointer of non-multidcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_nonMcastPass_get(int32 *pMode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pMode)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_nonMcastPass_get(g_gponmac_drv->dev, pMode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_multicastAddrCheck_set
 * Description:
 *      GPON MAC set the address pattern.
 * Input:
 *      ipv4_pattern    - Address pattern of DA[47:24] for IPv4 packets.
 *      ipv6_pattern    - Address pattern of DA[47:32] for IPv6 packets.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern)
{
	 int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_mcastCheck_set(g_gponmac_drv->dev,ipv4_pattern,ipv6_pattern);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;

}

/* Function Name:
 *      rtk_gponapp_multicastAddrCheck_get
 * Description:
 *      GPON MAC get the address pattern.
 * Input:
 *      pIpv4_pattern    - Address pattern of DA[47:24] for IPv4 packets..
 *      pIpv6_pattern    - Address pattern of DA[47:24] for IPv6 packets..
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pIpv4_Pattern || !pIpv6_Pattern)
	{
	    return RT_ERR_NULL_POINTER;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_mcastCheck_get(g_gponmac_drv->dev,pIpv4_Pattern,pIpv6_Pattern);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_macFilterMode_set
 * Description:
 *      GPON MAC set the mac filter mode.
 * Input:
 *      mode            - MAC table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(mode>RTK_GPON_MACTBL_MODE_EXCLUDE)
	{
	    return RT_ERR_OUT_OF_RANGE;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_macFilterMode_set(g_gponmac_drv->dev,mode);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_macFilterMode_get
 * Description:
 *      GPON MAC get the mac filter mode.
 * Input:
 *      pMode           - pointer of MAC filter table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode)
{
	 int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	if(!pMode)
	{
	    return RT_ERR_NULL_POINTER;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_macFilterMode_get(g_gponmac_drv->dev, pMode);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_mcForceMode_set
 * Description:
 *      GPON MAC set the multicast force mode.
 * Input:
 *      ipv4            - IPv4 multicast force mode.
 *      ipv6            - IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_mcForceMode_set(g_gponmac_drv->dev,ipv4,ipv6);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_mcForceMode_get
 * Description:
 *      GPON MAC get the multicast force mode.
 * Input:
 *      pIpv4           - The pointer of IPv4 multicast force mode.
 *      pIv6            - The pointer of IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_mcForceMode_get(g_gponmac_drv->dev, pIpv4, pIpv6);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_macEntry_add
 * Description:
 *      GPON MAC Add a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_macEntry_add(rtk_gpon_macTable_entry_t *pEntry)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEntry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_add(g_gponmac_drv->dev,pEntry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_macEntry_del
 * Description:
 *      GPON MAC Remove a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_macEntry_del(rtk_gpon_macTable_entry_t *pEntry)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEntry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_del(g_gponmac_drv->dev,pEntry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_macEntry_get
 * Description:
 *      GPON MAC Get a MAC entry by the table index.
 * Input:
 *      index           - index of MAC filter table entry.
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEntry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_get(g_gponmac_drv->dev,index,pEntry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_rdi_set
 * Description:
 *      GPON MAC set the RDI indicator in upstream.
 * Input:
 *      enable          - specify to turn on/off RDI.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_rdi_set(int32 enable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_rdi_set(g_gponmac_drv->dev,enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_rdi_get
 * Description:
 *      GPON MAC get the RDI indicator in upstream.
 * Input:
 *      pEnable         - the pointer of RDI indicator.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_rdi_get(int32 *pEnable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEnable)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_rdi_get(g_gponmac_drv->dev, pEnable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_powerLevel_set
 * Description:
 *      GPON MAC set ONU power level, it will update the TT field of
 *      Serial_Number_ONU PLOAMu message.
 * Input:
 *      level           - the power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_powerLevel_set(uint8 level)
{
	int32 ret;

	if(!g_gponmac_drv)
	{
	    return RT_ERR_GPON_DRV_NOT_STARTED;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    return RT_ERR_GPON_ONU_NOT_REGISTERED;
	}

	GPON_OS_Lock(g_gponmac_drv->lock);

	ret = gpon_dev_powerLevel_set(g_gponmac_drv->dev,level);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	return ret;
}

/* Function Name:
 *      rtk_gponapp_powerLevel_get
 * Description:
 *      GPON MAC get ONU power level.
 * Input:
 *      pLevel          - the pointer of power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_powerLevel_get(uint8 *pLevel)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pLevel)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_powerLevel_get(g_gponmac_drv->dev, pLevel);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_alarmStatus_get
 * Description:
 *      GPON MAC get the alarm status.
 * Input:
 *      alarm           - the alarm type.
 *      pStatus         - the pointer of alarm status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(alarm>=RTK_GPON_ALARM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_alarm_status_get(g_gponmac_drv->dev,alarm,pStatus);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_globalCounter_get
 * Description:
 *      GPON MAC get global performance counter.
 * Input:
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_ONU_ACTIVATION:
        {
            gpon_pm_onuActivation_get(g_gponmac_drv->dev,&pPara->activate);
            break;
        }
        case RTK_GPON_PMTYPE_DS_PHY:
        {
            gpon_pm_dsPhy_get(g_gponmac_drv->dev,&pPara->dsphy);
            break;
        }
        case RTK_GPON_PMTYPE_DS_PLOAM:
        {
            gpon_pm_dsPloam_get(g_gponmac_drv->dev,&pPara->dsploam);
            break;
        }
        case RTK_GPON_PMTYPE_DS_BWMAP:
        {
            gpon_pm_dsBwMap_get(g_gponmac_drv->dev,&pPara->dsbwmap);
            break;
        }
        case RTK_GPON_PMTYPE_DS_GEM:
        {
            gpon_pm_dsGem_get(g_gponmac_drv->dev,&pPara->dsgem);
            break;
        }
        case RTK_GPON_PMTYPE_DS_ETH:
        {
            gpon_pm_dsEth_get(g_gponmac_drv->dev,&pPara->dseth);
            break;
        }
        case RTK_GPON_PMTYPE_DS_OMCI:
        {
            gpon_pm_dsOmci_get(g_gponmac_drv->dev,&pPara->dsomci);
            break;
        }
        case RTK_GPON_PMTYPE_US_PHY:
        {
            gpon_pm_usPhy_get(g_gponmac_drv->dev,&pPara->usphy);
            break;
        }
        case RTK_GPON_PMTYPE_US_DBR:
        {
            gpon_pm_usDbr_get(g_gponmac_drv->dev,&pPara->usdbr);
            break;
        }
        case RTK_GPON_PMTYPE_US_PLOAM:
        {
            gpon_pm_usPloam_get(g_gponmac_drv->dev,&pPara->usploam);
            break;
        }
        case RTK_GPON_PMTYPE_US_GEM:
        {
            gpon_pm_usGem_get(g_gponmac_drv->dev,&pPara->usgem);
            break;
        }
        case RTK_GPON_PMTYPE_US_ETH:
        {
            gpon_pm_usEth_get(g_gponmac_drv->dev,&pPara->useth);
            break;
        }
        case RTK_GPON_PMTYPE_US_OMCI:
        {
            gpon_pm_usOmci_get(g_gponmac_drv->dev,&pPara->usomci);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_tcontCounter_get
 * Description:
 *      GPON MAC get Tcont performance counter.
 * Input:
 *      tcontId         - the TCont id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_TCONT_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_TCONT_GEM:
        {
            gpon_pm_tcontGem_get(g_gponmac_drv->dev, tcontId,&pPara->gem);
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_ETH:
        {
            gpon_pm_tcontEth_get(g_gponmac_drv->dev, tcontId,&pPara->eth);
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_IDLE:
        {
            gpon_pm_tcontIdle_get(g_gponmac_drv->dev, tcontId,&pPara->idle);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_flowCounter_get
 * Description:
 *      GPON MAC get Flow performance counter.
 * Input:
 *      flowId          - the flow id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara)
{
    int32 rsl = RT_ERR_OK;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_FLOW_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_FLOW_DS_GEM:
        {
            rsl = gpon_pm_dsFlowGem_get(g_gponmac_drv->dev, flowId, &pPara->dsgem);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_DS_ETH:
        {
            rsl = gpon_pm_dsFlowEth_get(g_gponmac_drv->dev, flowId, &pPara->dseth);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_GEM:
        {
            rsl = gpon_pm_usFlowGem_get(g_gponmac_drv->dev, flowId, &pPara->usgem);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_ETH:
        {
            rsl = gpon_pm_usFlowEth_get(g_gponmac_drv->dev, flowId, &pPara->useth);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return rsl;
}

/* Function Name:
 *      rtk_gponapp_version_get
 * Description:
 *      GPON MAC get the version infomation for debug.
 * Input:
 *      pHver           - the pointer of Hardware versiotn
 *      pSver           - the pointer of Software versiotn
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_version_get(rtk_gpon_device_ver_t *pHver, rtk_gpon_driver_ver_t *pSver)
{

    pHver->gmac_ver  = 0xFF;

    pSver->release   = 0xFF;
    pSver->load      = 0xFF;
    pSver->build     = 0xFFFF;

    if(g_gponmac_drv)
    {
        pSver->release   = GPON_DRIVER_SW_VERSION_RELEASE;
        pSver->load      = GPON_DRIVER_SW_VERSION_LOAD;
        pSver->build     = GPON_DRIVER_SW_VERSION_BUILD;

        if(g_gponmac_drv->dev)
        {
            GPON_OS_Lock(g_gponmac_drv->lock);
            gpon_dev_versoin_get(g_gponmac_drv->dev,pHver);
            GPON_OS_Unlock(g_gponmac_drv->lock);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_txForceLaser_set
 * Description:
 *      GPON MAC set the Laser status.
 * Input:
 *      status          - specify to force turn on/off laser
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_txForceLaser_set(rtk_gpon_laser_status_t status)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(status>RTK_GPON_LASER_STATUS_FORCE_OFF)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_forceLaser_set(g_gponmac_drv->dev,status);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_txForceLaser_get
 * Description:
 *      GPON MAC get the Laser status.
 * Input:
 *      pStatus         - pointer of force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_txForceLaser_get(rtk_gpon_laser_status_t *pStatus)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pStatus)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_forceLaser_get(g_gponmac_drv->dev,pStatus);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_txForceIdle_set
 * Description:
 *      GPON MAC set to force insert the idle in upstream.
 * Input:
 *      pStatus         - specify to force send Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_txForceIdle_set(int32 on)
{
     if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_forceIdle_set(g_gponmac_drv->dev,on);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_txForceIdle_get
 * Description:
 *      GPON MAC get the status to force insert the idle in upstream.
 * Input:
 *      pStatus         - pointer of force Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_txForceIdle_get(int32 *pOn)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pOn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_forceIdle_get(g_gponmac_drv->dev, pOn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}



/* Function Name:
 *      rtk_gponapp_dsFecSts_get
 * Description:
 *      GPON MAC get the status to FEC in downstream from Ident field.
 * Input:
 *      pStatus         - pointer of D/S FEC status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_dsFecSts_get(int32* pEn)
{
      int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_dsFec_get(g_gponmac_drv->dev,pEn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_version_show
 * Description:
 *      GPON MAC show version infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_version_show(void)
{
	 rtk_gpon_device_ver_t hver;

#if defined(FPGA_DEFINED)
		uint16 date, time, revision;
#endif

	if(!g_gponmac_drv)
	{
	    osal_printf("GPON MAC is not started.\r\n");
	    return RT_ERR_OK;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    osal_printf("GPON MAC Device is not initialized.\r\n");
	    return RT_ERR_OK;
	}
	gpon_dev_versoin_get(g_gponmac_drv->dev,&hver);

	osal_printf("============================================================\r\n");
	osal_printf("    GPON ONU MAC Device Version\r\n");

#if defined(FPGA_DEFINED)
	osal_printf("GPON FPGA date: 0x%04x, time: 0x%04x, revision: 0x%04x\r\n",date, time, revision);
#endif
	osal_printf("GPON MAC Version: 0x%x\r\n",hver.gmac_ver);
	osal_printf("Software Version: %d.%02d.%04d\r\n",GPON_DRIVER_SW_VERSION_RELEASE,GPON_DRIVER_SW_VERSION_LOAD,GPON_DRIVER_SW_VERSION_BUILD);
	osal_printf("Building    Time: %s, %s\r\n",__TIME__,__DATE__);
	osal_printf("============================================================\r\n");
	return RT_ERR_OK;

}

/* Function Name:
 *      rtk_gponapp_devInfo_show
 * Description:
 *      GPON MAC show the whole driver infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_devInfo_show(void)
{
     if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	  return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    osal_printf("============================================================\r\n");
    osal_printf("    GPON ONU MAC Driver Info\r\n");
    osal_printf("Software Base Addr: %p\r\n",g_gponmac_drv);
    osal_printf("Driver Status: %s\r\n",gpon_dbg_drv_status_str(g_gponmac_drv->status));
    osal_printf("Semaphore ID: %p\r\n",(void *)g_gponmac_drv->lock);
    osal_printf("Dev Pointer: %p\r\n",g_gponmac_drv->dev);
    osal_printf("Device Base Addr: 0x%lx\r\n",(long unsigned int)g_gponmac_drv->dev->base_addr);
    osal_printf("Device Timer: %p\r\n",(void *)g_gponmac_drv->dev->timer);
    osal_printf("============================================================\r\n");
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_gtc_show
 * Description:
 *      GPON MAC show the whole GTC infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_gtc_show(void)
{
    uint32 i;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return RT_ERR_OK;

    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return RT_ERR_OK;
    }

    osal_printf("============================================================\r\n");
    osal_printf("    GPON ONU MAC GTC Status\r\n");
    osal_printf("GPON MAC Status: %s\r\n",gpon_dbg_fsm_status_str(g_gponmac_drv->dev->status));
    osal_printf("ONU ID: %u\r\n",g_gponmac_drv->dev->onuid);
    osal_printf("Vendor ID: %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->serial_number.vendor[0],g_gponmac_drv->dev->serial_number.vendor[1],g_gponmac_drv->dev->serial_number.vendor[2],g_gponmac_drv->dev->serial_number.vendor[3]);
    osal_printf("Specific: %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->serial_number.specific[0],g_gponmac_drv->dev->serial_number.specific[1],g_gponmac_drv->dev->serial_number.specific[2],g_gponmac_drv->dev->serial_number.specific[3]);
    osal_printf("Password: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->password.password[0],g_gponmac_drv->dev->password.password[1],g_gponmac_drv->dev->password.password[2],g_gponmac_drv->dev->password.password[3],
        g_gponmac_drv->dev->password.password[4],g_gponmac_drv->dev->password.password[5],g_gponmac_drv->dev->password.password[6],g_gponmac_drv->dev->password.password[7],g_gponmac_drv->dev->password.password[8],g_gponmac_drv->dev->password.password[9]);
    osal_printf("Power Level: %u\r\n",g_gponmac_drv->dev->power_level);
    osal_printf("TO1 timer: %lu ms, TO2 timer: %lu ms\r\n",(long unsigned int)g_gponmac_drv->dev->parameter.onu.to1_timer,(long unsigned int)g_gponmac_drv->dev->parameter.onu.to2_timer);
    osal_printf("Laser: Optical EN: %d, Optical Polarity: %d, CDR EN: %d, CDR Polarity: %d, LOS Filter %d\r\n",g_gponmac_drv->dev->parameter.laser.laser_optic_los_en,g_gponmac_drv->dev->parameter.laser.laser_optic_los_polar,g_gponmac_drv->dev->parameter.laser.laser_cdr_los_en,g_gponmac_drv->dev->parameter.laser.laser_cdr_los_polar,g_gponmac_drv->dev->parameter.laser.laser_los_filter_en);
    osal_printf("Laser: Laser On: 0x%x, Laser Off: 0x%x\r\n",g_gponmac_drv->dev->parameter.laser.laser_us_on,g_gponmac_drv->dev->parameter.laser.laser_us_off);
    osal_printf("D/S Physical: Scramble EN: %d, FEC Bypass: %d, FEC Thrd: %d\r\n",g_gponmac_drv->dev->parameter.dsphy.ds_scramble_en,g_gponmac_drv->dev->parameter.dsphy.ds_fec_bypass,g_gponmac_drv->dev->parameter.dsphy.ds_fec_thrd);
/* ploam_crc_check is removed in GPON_MAC_SWIO_v1.1 */
    osal_printf("D/S PLOAM: ONU ID  Filter: %d, Broadcast Accept: %d, Drop CRC error %d\r\n",g_gponmac_drv->dev->parameter.dsploam.ds_ploam_onuid_filter,g_gponmac_drv->dev->parameter.dsploam.ds_ploam_broadcast_accpt,g_gponmac_drv->dev->parameter.dsploam.ds_ploam_drop_crc_err);
    osal_printf("D/S BWMAP: CRC Check EN: %d, ONU ID filter EN: %d, PLENd mode: %d\r\n",g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_crc_chk,g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_onuid_filter,g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_plend_mode);
    osal_printf("D/S GEM: Assemble Timer: %d\r\n",g_gponmac_drv->dev->parameter.dsgem.assemble_timer);
    osal_printf("D/S Eth: FCS Check EN: %d, PTI mask: 0x%1x, PTI Pattern: 0x%1x\r\n",g_gponmac_drv->dev->parameter.dseth.ds_eth_crc_chk,g_gponmac_drv->dev->parameter.dseth.ds_eth_pti_mask,g_gponmac_drv->dev->parameter.dseth.ds_eth_pti_ptn);
    osal_printf("D/S OMCI: PTI mask: 0x%1x, PTI Pattern: 0x%1x\r\n",g_gponmac_drv->dev->parameter.dsomci.ds_omci_pti_mask,g_gponmac_drv->dev->parameter.dsomci.ds_omci_pti_ptn);

    osal_printf("U/S Physical: Scramble EN: %d, Burst Enable Polarity: %d, Small SStart: %d, Suppress Laser: %d\r\n",g_gponmac_drv->dev->parameter.usphy.us_scramble_en,g_gponmac_drv->dev->parameter.usphy.us_burst_en_polar,g_gponmac_drv->dev->parameter.usphy.small_sstart_en,g_gponmac_drv->dev->parameter.usphy.suppress_laser_en);
    osal_printf("U/S PLOAM: PLOAMu EN: %d\r\n",g_gponmac_drv->dev->parameter.usploam.us_ploam_en);
    osal_printf("U/S DBR: DBRu EN: %d\r\n",g_gponmac_drv->dev->parameter.usdbr.us_dbru_en);

    osal_printf("Chunk: head size %d, task ind %d, err code 0x%x, src 0x%x\r\n",g_gponmac_drv->dev->chunk_header.head_size,g_gponmac_drv->dev->chunk_header.task_ind,g_gponmac_drv->dev->chunk_header.err_code,g_gponmac_drv->dev->chunk_header.src);
    osal_printf("Shaper: tick %d, inc size %d, thrd high 0x%04x, thrd low 0x%04x\r\n",g_gponmac_drv->dev->shaper.tick,g_gponmac_drv->dev->shaper.inc_size,g_gponmac_drv->dev->shaper.thrd_high,g_gponmac_drv->dev->shaper.thrd_low);
    osal_printf("Mac Table: Broadcast Pass: %d, Non Multicast Pass: %d, Multicast Check Pattern for Ipv4: %08lx, Multicast Check Pattern for Ipv6: %08lx, Multicast Filter Mode: %s\r\n",
                    g_gponmac_drv->dev->mactbl.bc_pass,
                    g_gponmac_drv->dev->mactbl.non_mc_pass,
                    (long unsigned int)g_gponmac_drv->dev->mactbl.ipv4_mc_check_ptn,
                    (long unsigned int)g_gponmac_drv->dev->mactbl.ipv6_mc_check_ptn,
                    gpon_dbg_mac_filter_mode_str(g_gponmac_drv->dev->mactbl.op_mode));
    osal_printf("Request and Data Output En: %d\r\n",g_gponmac_drv->dev->req_en);
    osal_printf("RDI Indicator: %d\r\n",g_gponmac_drv->dev->rdi);
    osal_printf("TDM Module En: %d\r\n",g_gponmac_drv->dev->tdm_en);
    osal_printf("DEC Status: D/S %d, U/S %d\r\n",g_gponmac_drv->dev->us_ploam_req_urg,g_gponmac_drv->dev->us_ploam_req_nrm);
    osal_printf("U/S PLOAM Send Req: Urgent %d, Normal %d\r\n",g_gponmac_drv->dev->ds_fec,g_gponmac_drv->dev->us_fec);
    osal_printf("U/S OMCI Flow ID: %d\r\n",g_gponmac_drv->dev->us_omci_flow);
    osal_printf("AES Key Switch Time: 0x%lx\r\n",(long unsigned int)g_gponmac_drv->dev->aes_key_switch_time);
    osal_printf("AES Key[%d]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->key_index,g_gponmac_drv->dev->aes_key.key[0],g_gponmac_drv->dev->aes_key.key[1],g_gponmac_drv->dev->aes_key.key[2],g_gponmac_drv->dev->aes_key.key[3],
        g_gponmac_drv->dev->aes_key.key[4],g_gponmac_drv->dev->aes_key.key[5],g_gponmac_drv->dev->aes_key.key[6],g_gponmac_drv->dev->aes_key.key[7],g_gponmac_drv->dev->aes_key.key[8],g_gponmac_drv->dev->aes_key.key[9],g_gponmac_drv->dev->aes_key.key[10],g_gponmac_drv->dev->aes_key.key[11],
        g_gponmac_drv->dev->aes_key.key[12],g_gponmac_drv->dev->aes_key.key[13],g_gponmac_drv->dev->aes_key.key[14],g_gponmac_drv->dev->aes_key.key[15]);
    osal_printf("Burst Overhead: guard bits %d, t1 bits %d, t2 bits %d, t3 pattern 0x%02x\r\n",g_gponmac_drv->dev->burst_overhead.guard_bits,g_gponmac_drv->dev->burst_overhead.type1_bits,g_gponmac_drv->dev->burst_overhead.type2_bits,g_gponmac_drv->dev->burst_overhead.type3_ptn);
    osal_printf("Burst Overhead: Delimiter %02x%02x%02x\r\n",g_gponmac_drv->dev->burst_overhead.delimiter[0],g_gponmac_drv->dev->burst_overhead.delimiter[1],g_gponmac_drv->dev->burst_overhead.delimiter[2]);
    osal_printf("Burst Overhead: t3 pre-range %d, t3 ranged %d\r\n",g_gponmac_drv->dev->burst_overhead.type3_preranged,g_gponmac_drv->dev->burst_overhead.type3_ranged);
    osal_printf("Burst Overhead: Total Length: pre-range %d, ranged %d\r\n",g_gponmac_drv->dev->burst_overhead.boh_len_preranged,g_gponmac_drv->dev->burst_overhead.boh_len_ranged);
    osal_printf("Burst Overhead: Pattern: valid %d, repeat byte %d\r\n",g_gponmac_drv->dev->burst_overhead.boh_valid,g_gponmac_drv->dev->burst_overhead.boh_repeat);
    if(g_gponmac_drv->dev->burst_overhead.boh_valid)
    {
        osal_printf("Burst Overhead: Pattern:");
        for(i=0;i<g_gponmac_drv->dev->burst_overhead.boh_valid;i++)
        {
            osal_printf(" 0x%02x",g_gponmac_drv->dev->burst_overhead.bursthead[i]);
        }
        osal_printf("\r\n");
    }
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        osal_printf("Alarm [%s]: %d\r\n",gpon_dbg_alarm_str((rtk_gpon_alarm_type_t)i),g_gponmac_drv->dev->alarm_tbl[i]);
    }
    osal_printf("Callback: State Change: %p\r\n",g_gponmac_drv->dev->state_change_callback);
    osal_printf("Callback: D/S FEC Change: %p\r\n",g_gponmac_drv->dev->dsfec_change_callback);
    osal_printf("Callback: U/S FEC Change: %p\r\n",g_gponmac_drv->dev->usfec_change_callback);
    osal_printf("Callback: U/S PLOAM Urgent Q Empty: %p\r\n",g_gponmac_drv->dev->usploam_urg_epty_callback);
    osal_printf("Callback: U/S PLOAM Normal Q Empty: %p\r\n",g_gponmac_drv->dev->usploam_nrm_epty_callback);
    osal_printf("Callback: PLOAM: %p\r\n",g_gponmac_drv->dev->ploam_callback);
    osal_printf("Callback: OMCI: %p\r\n",g_gponmac_drv->dev->omci_callback);
    osal_printf("Callback: AES Key: %p\r\n",g_gponmac_drv->dev->aeskey_callback);
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        osal_printf("Callback: Alarm [%s]: %p\r\n",gpon_dbg_alarm_str((rtk_gpon_alarm_type_t)i),g_gponmac_drv->dev->fault_callback[i]);
    }
    osal_printf("============================================================\r\n");
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_tcont_show
 * Description:
 *      GPON MAC show the TCont infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_tcont_show(uint32 tcont)
{
    uint32 i;
    int32        found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return RT_ERR_OK;

    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return RT_ERR_OK;
    }

    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_tcont[i])
        {
            if(tcont==0xFFFF || tcont==i)
            {
                if(!found)
                {
                    osal_printf("============================================================\r\n");
                    osal_printf("     GPON ONU MAC TCONT Status\r\n");
                    osal_printf("TCont ID | Alloc ID | Physical | Logical | Type \r\n");
                    found = TRUE;
                }
                osal_printf("    %4d |     %4d |        %1s |       %1s |    %1d \r\n",g_gponmac_drv->dev->p_tcont[i]->tcont_id,
                    g_gponmac_drv->dev->p_tcont[i]->alloc_id,g_gponmac_drv->dev->p_tcont[i]->ploam_en?"*":" ",g_gponmac_drv->dev->p_tcont[i]->omci_en?"*":" ",
                    g_gponmac_drv->dev->p_tcont[i]->type);
            }
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        if(tcont!=0xFFFF)
        {
            osal_printf("The TCont is not created.\r\n");
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_dsFlow_show
 * Description:
 *      GPON MAC show the D/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_dsFlow_show(uint32 flow)
{
	uint32 i;
	int32        found = FALSE;
	if(!g_gponmac_drv)
	{
	    osal_printf("GPON MAC is not started.\r\n");
	    return RT_ERR_OK;
	}

	if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
	{
	    osal_printf("GPON MAC Device is not initialized.\r\n");
	    return RT_ERR_OK;
	}

	for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
	{
	    if(g_gponmac_drv->dev->p_dsflow[i])
	    {
	        if(flow==0xFFFF || flow==i)
	        {
	            if(!found)
	            {
	                osal_printf("============================================================\r\n");
	                osal_printf("     GPON ONU MAC D/S Flow Status\r\n");
	                osal_printf("Flow ID | GEM Port | Type | Multicast | AES\r\n");
	                found = TRUE;
	            }
	            osal_printf("   %4d |     %4d | %4s |         %1s |   %1s\r\n",g_gponmac_drv->dev->p_dsflow[i]->flow_id,
	                g_gponmac_drv->dev->p_dsflow[i]->gem_port,gpon_dbg_flow_type_str(g_gponmac_drv->dev->p_dsflow[i]->type),g_gponmac_drv->dev->p_dsflow[i]->multicast?"*":" ",
	                g_gponmac_drv->dev->p_dsflow[i]->aes_en?"*":" ");
	        }
	    }
	}
	if(found)
	{
	    osal_printf("============================================================\r\n");
	}
	else
	{
	    if(flow!=0xFFFF)
	    {
	        osal_printf("The D/S Flow is not created.\r\n");
	    }
	}
	return RT_ERR_OK;

}

/* Function Name:
 *      rtk_gponapp_usFlow_show
 * Description:
 *      GPON MAC show the U/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_usFlow_show(uint32 flow)
{
    uint32 i;
    int32        found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	 return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_usflow[i])
        {
            if(flow==0xFFFF || flow==i)
            {
                if(!found)
                {
                    osal_printf("============================================================\r\n");
                    osal_printf("    GPON ONU MAC U/S Flow Status\r\n");
                    osal_printf("Flow ID | GEM Port | Type | TCont | Channel | OMCI\r\n");
                    found = TRUE;
                }
                osal_printf("   %4d |     %4d | %4s |  %4d |     %3d |    %1s\r\n",g_gponmac_drv->dev->p_usflow[i]->flow_id,
                    g_gponmac_drv->dev->p_usflow[i]->gem_port,gpon_dbg_flow_type_str(g_gponmac_drv->dev->p_usflow[i]->type),g_gponmac_drv->dev->p_usflow[i]->tcont,
                    g_gponmac_drv->dev->p_usflow[i]->channel,
                    (g_gponmac_drv->dev->p_usflow[i]->flow_id==g_gponmac_drv->dev->us_omci_flow)?"*":" ");
            }
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        if(flow!=0xFFFF)
        {
            osal_printf("The U/S Flow is not created.\r\n");
        }
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_gponapp_macTable_show
 * Description:
 *      GPON MAC show Ethernet Mac Table in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_macTable_show(void)
{
    uint32      i;
    int32       found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	 return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_mac_tbl[i])
        {
            if(!found)
            {
                osal_printf("============================================================\r\n");
                osal_printf("     GPON ONU MAC Multicast MAC Filter Table\r\n");
                osal_printf("Index | MAC Address\r\n");
                found = TRUE;
            }
            osal_printf("  %3d | %02x-%02x-%02x-%02x-%02x-%02x\r\n",i,g_gponmac_drv->dev->p_mac_tbl[i]->addr[0],
                g_gponmac_drv->dev->p_mac_tbl[i]->addr[1],g_gponmac_drv->dev->p_mac_tbl[i]->addr[2],g_gponmac_drv->dev->p_mac_tbl[i]->addr[3],
                g_gponmac_drv->dev->p_mac_tbl[i]->addr[4],g_gponmac_drv->dev->p_mac_tbl[i]->addr[5]);
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        osal_printf("The MAC Table is empty.\r\n");
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_gponapp_globalCounter_show
 * Description:
 *      GPON MAC show Global Counter in COM port.
 * Input:
 *      type    - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_globalCounter_show(rtk_gpon_global_performance_type_t type)
{
     rtk_gpon_global_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	 return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    if(rtk_gponapp_globalCounter_get(type,&counter)!=RT_ERR_OK)
    {
		osal_printf("Read Counter fail\r\n");
		return RT_ERR_OK;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_ONU_ACTIVATION:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: Activation\r\n");
            osal_printf("SN Req          : %u\r\n",counter.activate.rx_sn_req);
            osal_printf("Ranging Req     : %u\r\n",counter.activate.rx_ranging_req);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_PHY:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS PHY\r\n");
            osal_printf("BIP Error bits  : %lu\r\n",(long unsigned int)counter.dsphy.rx_bip_err_bit);
            osal_printf("BIP Error blocks: %lu\r\n",(long unsigned int)counter.dsphy.rx_bip_err_block);
            osal_printf("FEC Correct bits: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_bit);
            osal_printf("FEC Correct bytes: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_byte);
            osal_printf("FEC Correct codewords: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_cw);
            osal_printf("FEC codewords Uncor: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_uncor_cw);
            osal_printf("Superframe LOS  : %u\r\n",counter.dsphy.rx_lom);
            osal_printf("PLEN fail       : %u\r\n",counter.dsphy.rx_plen_err);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_PLOAM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS PLOAM\r\n");
            osal_printf("Total RX PLOAMd    : %u\r\n",counter.dsploam.rx_ploam_cnt);
            osal_printf("CRC Err RX PLOAM   : %u\r\n",counter.dsploam.rx_ploam_err);
            osal_printf("Corrected RX PLOAMd: %u\r\n",counter.dsploam.rx_ploam_correctted);
            osal_printf("Proc RX PLOAMd     : %u\r\n",counter.dsploam.rx_ploam_proc);
            osal_printf("Overflow Rx PLOAM  : %u\r\n",counter.dsploam.rx_ploam_overflow);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_BWMAP:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS BWMAP\r\n");
            osal_printf("Total RX BwMap  : %lu\r\n",(long unsigned int)counter.dsbwmap.rx_bwmap_cnt);
            osal_printf("CRC Err RX BwMap: %u\r\n",counter.dsbwmap.rx_bwmap_crc_err);
            osal_printf("Overflow BwMap  : %u\r\n",counter.dsbwmap.rx_bwmap_overflow);
            osal_printf("Invalid BwMap 0 : %u\r\n",counter.dsbwmap.rx_bwmap_inv0);
            osal_printf("Invalid BwMap 1 : %u\r\n",counter.dsbwmap.rx_bwmap_inv1);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS GEM\r\n");
            osal_printf("D/S GEM LOS     : %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_los);
            osal_printf("D/S GEM Idle    : %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_idle);
            osal_printf("D/S GEM Non Idle: %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_non_idle);
            osal_printf("D/S HEC correct : %lu\r\n",(long unsigned int)counter.dsgem.rx_hec_correct);
            osal_printf("Over Interleave : %lu\r\n",(long unsigned int)counter.dsgem.rx_over_interleaving);
            osal_printf("Mis GEM Pkt Len : %u\r\n",counter.dsgem.rx_gem_len_mis);
            osal_printf("Multi Flow Match: %u\r\n",counter.dsgem.rx_match_multi_flow);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS ETH\r\n");
            osal_printf("Total Unicast   : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_unicast);
            osal_printf("Total Multicast : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast);
            osal_printf("Fwd Multicast   : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast_fwd);
            osal_printf("Leak Multicast  : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast_leak);
            osal_printf("FCS Error       : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_fcs_err);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_OMCI:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS OMCI\r\n");
            osal_printf("Total RX OMCI   : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci);
            osal_printf("CRC Error OMCI  : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_crc_err);
            osal_printf("Processed OMCI  : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_proc);
            osal_printf("Dropped OMCI    : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_drop);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_PHY:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US PHY\r\n");
            osal_printf("TX BOH          : %u\r\n",counter.usphy.tx_boh_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_DBR:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US DBR\r\n");
            osal_printf("TX DBRu         : %u\r\n",counter.usdbr.tx_dbru_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_PLOAM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US PLOAM\r\n");
            osal_printf("Total TX PLOAM  : %u\r\n",counter.usploam.tx_ploam_cnt);
            osal_printf("Process TX PLOAM: %u\r\n",counter.usploam.tx_ploam_proc);
            osal_printf("TX Urgent PLOAM : %u\r\n",counter.usploam.tx_ploam_urg);
            osal_printf("Proc Urg PLOAM  : %u\r\n",counter.usploam.tx_ploam_urg_proc);
            osal_printf("TX Normal PLOAM : %u\r\n",counter.usploam.tx_ploam_nor);
            osal_printf("Proc Nrm PLOAM  : %u\r\n",counter.usploam.tx_ploam_nor_proc);
            osal_printf("TX S/N PLOAM    : %u\r\n",counter.usploam.tx_ploam_sn);
            osal_printf("TX NoMsg PLOAM  : %u\r\n",counter.usploam.tx_ploam_nomsg);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US GEM\r\n");
            osal_printf("TX GEM Blocks   : %u\r\n",counter.usgem.tx_gem_cnt);
            osal_printf("TX GEM Bytes    : %lu\r\n",(long unsigned int)counter.usgem.tx_gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US ETH\r\n");
            osal_printf("Aborted Packet  : %lu\r\n",(long unsigned int)counter.useth.tx_eth_abort_ebb);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_OMCI:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US OMCI\r\n");
            osal_printf("Process OMCI    : %lu\r\n",(long unsigned int)counter.usomci.tx_omci_proc);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_gponapp_tcontCounter_show
 * Description:
 *      GPON MAC show TCont Counter in COM port.
 * Input:
 *      idx         - TCont index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type)
{
     rtk_gpon_tcont_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	 return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    if(rtk_gponapp_tcontCounter_get(idx,type,&counter)!=RT_ERR_OK)
    {
        osal_printf("Read Counter fail[idx %d, type %d]\r\n",idx,type);
	 return RT_ERR_OK;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_TCONT_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Gem\r\n",idx);
            osal_printf("U/S sent GEM    : %lu\r\n",(long unsigned int)counter.gem.gem_pkt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Eth\r\n",idx);
            osal_printf("U/S sent Eth Pkt: %lu\r\n",(long unsigned int)counter.eth.eth_pkt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_IDLE:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Idle\r\n",idx);
            osal_printf("U/S sent Idle Byte: %llu\r\n",counter.idle.idle_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_flowCounter_show
 * Description:
 *      GPON MAC show Flow Counter in COM port.
 * Input:
 *      idx         - Flow index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type)
{
     rtk_gpon_flow_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
	 return RT_ERR_OK;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
	 return RT_ERR_OK;
    }

    if(rtk_gponapp_flowCounter_get(idx,type,&counter)!=RT_ERR_OK)
    {
        osal_printf("Read Counter fail[idx %d, type %d]\r\n",idx,type);
	 return RT_ERR_OK;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_FLOW_DS_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: D/S Flow %d Gem\r\n",idx);
            osal_printf("D/S GEM packets : %lu\r\n",(long unsigned int)counter.dsgem.gem_block);
            osal_printf("D/S GEM bytes   : %lu\r\n",(long unsigned int)counter.dsgem.gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_DS_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: D/S Flow %d Eth\r\n",idx);
            osal_printf("RX Eth packetts : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_rx);
            osal_printf("Fwd Eth packets : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_fwd);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: U/S Flow %d Gem\r\n",idx);
            osal_printf("U/S GEM counts  : %lu\r\n",(long unsigned int)counter.usgem.gem_block);
            osal_printf("U/S GEM bytes   : %llu\r\n",counter.usgem.gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: U/S Flow %d Eth\r\n",idx);
            osal_printf("U/S Eth packets   : %lu\r\n",(long unsigned int)counter.useth.eth_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      rtk_gponapp_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      pOmci       - pointer of OMCI message data
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_omci_tx(rtk_gpon_omci_msg_t *pOmci)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_omci_tx(g_gponmac_drv->dev, pOmci);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if defined(CONFIG_SDK_KERNEL_LINUX)

/* Function Name:
 *      rtk_gponapp_omci_rx
 * Description:
 *      Receive OMCI message.
 * Input:
 *      pBuf        - pointer of received data
 *      len         - received data length
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_omci_rx_wrapper(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
	uint32 ret;

#ifdef FPGA_DEFINED
    if(pRxInfo->opts3.bit.reason == 209)
#else
    if(pRxInfo->opts1.bit.pctrl == 1)
#endif
    {
     	if((ret = rtk_gponapp_omci_rx(skb->data,skb->len))!=RT_ERR_OK){
    		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
    		return 0;
    	}
    }
	return 0;
}

#endif
/* Function Name:
 *      rtk_gponapp_omci_rx
 * Description:
 *      Receive OMCI message.
 * Input:
 *      pBuf        - pointer of received data
 *      len         - received data length
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gponapp_omci_rx(uint8 *pBuf, uint32 len)
{
    rtk_gpon_omci_msg_t omci;
    int32 ret;


    if(len > RTK_GPON_OMCI_MSG_LEN)
        osal_memcpy(omci.msg, pBuf, RTK_GPON_OMCI_MSG_LEN);
    else
        osal_memcpy(omci.msg, pBuf, len);


    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_omci_rx(g_gponmac_drv->dev, &omci);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_autoTcont_set
 * Description:
 *      GPON set the TCONT auto configuration.
 * Input:
 *      state          - state value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_autoTcont_set(int32 state)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_tcont_set(g_gponmac_drv->dev,state);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_autoTcont_get
 * Description:
 *      GPON get the TCONT auto configuration.
 * Input:
 *      pState         - the pointer of state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_autoTcont_get(int32 *pState)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pState)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_tcont_get(g_gponmac_drv->dev,pState);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_autoBoh_set
 * Description:
 *      GPON set the BOH auto configuration.
 * Input:
 *      offset          - state value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_autoBoh_set(int32 state)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_boh_set(g_gponmac_drv->dev,state);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_autoBoh_get
 * Description:
 *      GPON get the BOH auto configuration.
 * Input:
 *      pOffset         - the pointer of state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_autoBoh_get(int32 *pState)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pState)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_boh_get(g_gponmac_drv->dev,pState);

    GPON_OS_Unlock(g_gponmac_drv->lock);

	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gponapp_eqdOffset_set
 * Description:
 *      GPON set the EQD offset.
 * Input:
 *      offset          - offset value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_eqdOffset_set(int32 offset)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_eqd_offset_set(g_gponmac_drv->dev,offset);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_eqdOffset_get
 * Description:
 *      GPON get the EQD offset.
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_eqdOffset_get(int32 *pOffset)
{
     int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pOffset)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_eqd_offset_get(g_gponmac_drv->dev,pOffset);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

/* Function Name:
 *      rtk_gponapp_dbg_set
 * Description:
 *      GPON get the EQD offset.
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_debug_set(int32 enable)
{

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_enable(enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_initial
 * Description:
 *      gpon initial
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_initial(uint32 data)
{
	if(data>0){};
    rtk_gponapp_driver_initialize();
   	rtk_gponapp_device_initialize();

	return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_deinitial
 * Description:
 *      GPON deinitial
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_deinitial(void)
{

   if(g_gponmac_drv){
		rtk_gponapp_device_deInitialize();
		rtk_gponapp_driver_deInitialize();
	}

   return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_deinitial
 * Description:
 *      GPON deinitial
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gponapp_unit_test(uint32 id)
{

  if(id) {};
   return RT_ERR_OK;
}



#if defined(CONFIG_SDK_KERNEL_LINUX)
/* Function Name:
 *      rtk_gponapp_callbackExtMsgSetHandle_reg
 * Description:
 *      This function is called to register the extension set handler
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_callbackExtMsgSetHandle_reg(rtk_gpon_extMsgSetHandleFunc_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }


    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->extMsgSet_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_callbackExtMsgGetHandle_reg
 * Description:
 *      This function is called to register the extension get handler
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_callbackExtMsgGetHandle_reg(rtk_gpon_extMsgGetHandleFunc_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }


    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->extMsgGet_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}



/* Function Name:
 *      rtk_gponapp_runOmciWrapper_set
 * Description:
 *      This function is called for extension message set
 * Input:
 *      optId      	 - extend option id
 *      pValue          - value for running
 *      count          - count of type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_extMsg_set(rtk_gpon_extMsg_t extMsg)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

	if(!g_gponmac_drv->dev->extMsgSet_callback)
	{
		return RT_ERR_GPON_DRV_NOT_STARTED;
	}

    g_gponmac_drv->dev->extMsgSet_callback(extMsg);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gponapp_extMsg_get
 * Description:
 *      This function is called for run extension message get
 * Input:
 *      pExtMsg   - retunr extension message result
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_extMsg_get(rtk_gpon_extMsg_t *pExtMsg)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

	if(!g_gponmac_drv->dev->extMsgGet_callback)
	{
		return RT_ERR_GPON_DRV_NOT_STARTED;
	}

    g_gponmac_drv->dev->extMsgGet_callback(pExtMsg);
    return RT_ERR_OK;
}




static int __init rtk_gponapp_module_init(void)
{
   /*
   	rtk_gponapp_initial();
   */
   return RT_ERR_OK;
}


static void __exit rtk_gponapp_module_exit(void)
{
	rtk_gponapp_deinitial();
}



EXPORT_SYMBOL(rtk_gponapp_driver_initialize);
EXPORT_SYMBOL(rtk_gponapp_driver_deInitialize);
EXPORT_SYMBOL(rtk_gponapp_device_initialize);
EXPORT_SYMBOL(rtk_gponapp_device_deInitialize);
EXPORT_SYMBOL(rtk_gponapp_evtHdlStateChange_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlDsFecChange_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlUsFecChange_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlUsPloamUrgEmpty_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlUsPloamNrmEmpty_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlPloam_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlOmci_reg);
EXPORT_SYMBOL(rtk_gponapp_callbackQueryAesKey_reg);
EXPORT_SYMBOL(rtk_gponapp_evtHdlAlarm_reg);
EXPORT_SYMBOL(rtk_gponapp_serialNumber_set);
EXPORT_SYMBOL(rtk_gponapp_serialNumber_get);
EXPORT_SYMBOL(rtk_gponapp_password_set);
EXPORT_SYMBOL(rtk_gponapp_password_get);
EXPORT_SYMBOL(rtk_gponapp_parameter_set);
EXPORT_SYMBOL(rtk_gponapp_parameter_get);
EXPORT_SYMBOL(rtk_gponapp_activate);
EXPORT_SYMBOL(rtk_gponapp_deActivate);
EXPORT_SYMBOL(rtk_gponapp_ponStatus_get);
EXPORT_SYMBOL(rtk_gponapp_isr_entry);
EXPORT_SYMBOL(rtk_gponapp_tcont_create);
EXPORT_SYMBOL(rtk_gponapp_tcont_destroy);
EXPORT_SYMBOL(rtk_gponapp_tcont_get);
EXPORT_SYMBOL(rtk_gponapp_dsFlow_set);
EXPORT_SYMBOL(rtk_gponapp_dsFlow_get);
EXPORT_SYMBOL(rtk_gponapp_usFlow_set);
EXPORT_SYMBOL(rtk_gponapp_usFlow_get);
EXPORT_SYMBOL(rtk_gponapp_ploam_send);
EXPORT_SYMBOL(rtk_gponapp_broadcastPass_set);
EXPORT_SYMBOL(rtk_gponapp_broadcastPass_get);
EXPORT_SYMBOL(rtk_gponapp_nonMcastPass_set);
EXPORT_SYMBOL(rtk_gponapp_nonMcastPass_get);
EXPORT_SYMBOL(rtk_gponapp_multicastAddrCheck_set);
EXPORT_SYMBOL(rtk_gponapp_multicastAddrCheck_get);
EXPORT_SYMBOL(rtk_gponapp_macFilterMode_set);
EXPORT_SYMBOL(rtk_gponapp_macFilterMode_get);
EXPORT_SYMBOL(rtk_gponapp_mcForceMode_set);
EXPORT_SYMBOL(rtk_gponapp_mcForceMode_get);
EXPORT_SYMBOL(rtk_gponapp_macEntry_add);
EXPORT_SYMBOL(rtk_gponapp_macEntry_del);
EXPORT_SYMBOL(rtk_gponapp_macEntry_get);
EXPORT_SYMBOL(rtk_gponapp_rdi_set);
EXPORT_SYMBOL(rtk_gponapp_rdi_get);
EXPORT_SYMBOL(rtk_gponapp_powerLevel_set);
EXPORT_SYMBOL(rtk_gponapp_powerLevel_get);
EXPORT_SYMBOL(rtk_gponapp_alarmStatus_get);
EXPORT_SYMBOL(rtk_gponapp_globalCounter_get);
EXPORT_SYMBOL(rtk_gponapp_tcontCounter_get);
EXPORT_SYMBOL(rtk_gponapp_flowCounter_get);
EXPORT_SYMBOL(rtk_gponapp_version_get);
EXPORT_SYMBOL(rtk_gponapp_txForceLaser_set);
EXPORT_SYMBOL(rtk_gponapp_txForceLaser_get);
EXPORT_SYMBOL(rtk_gponapp_txForceIdle_set);
EXPORT_SYMBOL(rtk_gponapp_txForceIdle_get);
EXPORT_SYMBOL(rtk_gponapp_dsFecSts_get);
EXPORT_SYMBOL(rtk_gponapp_version_show);
EXPORT_SYMBOL(rtk_gponapp_devInfo_show);
EXPORT_SYMBOL(rtk_gponapp_gtc_show);
EXPORT_SYMBOL(rtk_gponapp_tcont_show);
EXPORT_SYMBOL(rtk_gponapp_dsFlow_show);
EXPORT_SYMBOL(rtk_gponapp_usFlow_show);
EXPORT_SYMBOL(rtk_gponapp_macTable_show);
EXPORT_SYMBOL(rtk_gponapp_globalCounter_show);
EXPORT_SYMBOL(rtk_gponapp_tcontCounter_show);
EXPORT_SYMBOL(rtk_gponapp_flowCounter_show);
EXPORT_SYMBOL(rtk_gponapp_omci_tx);
EXPORT_SYMBOL(rtk_gponapp_omci_rx);
EXPORT_SYMBOL(rtk_gponapp_debug_set);
EXPORT_SYMBOL(rtk_gponapp_autoTcont_set);
EXPORT_SYMBOL(rtk_gponapp_autoTcont_get);
EXPORT_SYMBOL(rtk_gponapp_autoBoh_set);
EXPORT_SYMBOL(rtk_gponapp_autoBoh_get);
EXPORT_SYMBOL(rtk_gponapp_eqdOffset_set);
EXPORT_SYMBOL(rtk_gponapp_eqdOffset_get);
EXPORT_SYMBOL(rtk_gponapp_evtHdlPloam_dreg);
EXPORT_SYMBOL(rtk_gponapp_callbackExtMsgSetHandle_reg);
EXPORT_SYMBOL(rtk_gponapp_callbackExtMsgGetHandle_reg);
EXPORT_SYMBOL(rtk_gponapp_extMsg_get);
EXPORT_SYMBOL(rtk_gponapp_extMsg_set);
EXPORT_SYMBOL(rtk_gponapp_initial);




MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek RTL9600 GPON driver");
MODULE_AUTHOR("Star Chang <starchang@realtek.comw>");
module_init(rtk_gponapp_module_init);
module_exit(rtk_gponapp_module_exit);

#endif

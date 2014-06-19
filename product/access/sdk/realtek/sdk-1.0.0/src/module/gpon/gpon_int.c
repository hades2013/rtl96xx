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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : GMac Driver Interrupt Processor
 *
 * Feature : GMac Driver Interrupt Processor
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_int.h>
#include <module/gpon/gpon_fsm.h>
#include <module/gpon/gpon_ploam.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_res.h>
#include <rtk/irq.h>

extern gpon_drv_obj_t *g_gponmac_drv;


static int32 gpon_isr_dsGtc_check( gpon_dev_obj_t* obj)
{

	uint32 value;
	rtk_enable_t enable;
	uint32 los, lof, lom, ds_fec;
	int32 ret;

	if((ret = rtk_gpon_gtcDsIntrDlt_get(GPON_GTC_DS_INTR_ALL,&value))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GTC D/S Interrupt status %04x",value);

	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_LOS_DLT,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOS_DLT,&los))!=RT_ERR_OK)
	    {
	    		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
			return ret;
	    }
	    gpon_alarm_event(obj,RTK_GPON_ALARM_LOS,los);
	}
	if((ret= rtk_gpon_gtcDsIntrDlt_check(GPON_LOF_DLT,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOF_DLT,&lof))!=RT_ERR_OK)
	    {
	    	RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	    }
	    gpon_alarm_event(obj,RTK_GPON_ALARM_LOF,lof);
	}
	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_SN_REQ_HIS,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    gpon_fsm_event(obj,GPON_FSM_EVENT_RX_SN_REQ);
	}
	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_RNG_REQ_HIS,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    gpon_fsm_event(obj,GPON_FSM_EVENT_RX_RANGING_REQ);
	}
	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_PLM_BUF_REQ,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    gpon_ploam_rx(obj);
	}
	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_LOM_DLT,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOM_DLT,&lom))!=RT_ERR_OK)
	    {
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	    }
	    gpon_alarm_event(obj,RTK_GPON_ALARM_LOM,lom);
	}
	if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_DS_FEC_STS_DLT,value,&enable))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
		return ret;
	}
	if(enable)
	{
	      if((ret = rtk_gpon_gtcDsIntr_get(GPON_DS_FEC_STS_DLT,&ds_fec))!=RT_ERR_OK)
	      	{
			RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
			return ret;
	      	}
	    	gpon_dev_dsFec_report(obj,ds_fec);
	}
	return RT_ERR_OK;
}


static int32 gpon_isr_usGtc_check(gpon_dev_obj_t* obj)
{
	
	uint32 value;
	rtk_enable_t enable;
	uint32 us_fec;
	int32 ret;
	
	rtk_gpon_gtcUsIntrDlt_get(GPON_GTC_US_INTR_ALL,&value);
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GTC U/S Interrupt status %04x",value);

	if((ret = rtk_gpon_gtcUsIntrDlt_get(GPON_US_FEC_STS,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    rtk_gpon_gtcUsIntr_get(GPON_US_FEC_STS,&us_fec);
	    gpon_dev_usFec_report(obj,us_fec);
	}
	if((ret = rtk_gpon_gtcUsIntrDlt_get(GPON_PLM_URG_EMPTY,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    gpon_ploam_usUrgPloamQ_empty_report(obj);
	}
	if((ret = rtk_gpon_gtcUsIntrDlt_get(GPON_PLM_NRM_EMPTY,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    gpon_ploam_usNrmPloamQ_Empty_report(obj);
	}

	return RT_ERR_OK;
}


static int32 gpon_isr_usGem_check(gpon_dev_obj_t* obj)
{

	uint32 value;
	rtk_enable_t enable;
	int32 ret;

	if(obj==NULL)
		return RT_ERR_NULL_POINTER;
	
	rtk_gpon_gemUsIntr_get(GPON_GEM_US_INTR_ALL,&value);
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GEM U/S Interrupt status %04x",value);

	if((ret=rtk_gpon_gemUsIntr_get(GPON_SD_VALID_LONG,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S SD Long interrupt");
	}
	 if((ret=rtk_gpon_gemUsIntr_get(GPON_SD_DIFF_HUGE,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S SD Diff interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_REQUEST_DELAY,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S request delay interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_BC_LESS6,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S byte count less than 6 interrupt");
	}
	 if((ret=rtk_gpon_gemUsIntr_get(GPON_ERR_PLI,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S error PLI interrupt");
	}
	 if((ret=rtk_gpon_gemUsIntr_get(GPON_BURST_TM_LARGER_GTC,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S burst large interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_TOO_MUCH_AT_END,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank too much interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_REMAIN_AFRD,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank remain interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_OVERFL,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank overflow interrupt");
	}
	if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_UNDERFL,&enable))!=RT_ERR_OK)
	{
		return ret;
	}
	if(enable)
	{
	    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank underflow interrupt");
	}

	return RT_ERR_OK;
}

void gpon_isr_entry(void)
{
	uint32 mask,stat;
	rtk_enable_t enable;
	gpon_dev_obj_t* obj;
	int32 ret;

	GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"dal_apollo_gpon_isr_entry");

	if(!g_gponmac_drv || !g_gponmac_drv->dev || g_gponmac_drv->status!=GPON_DRV_STATUS_ACTIVATE)
	{
	    return;
	}

	obj = g_gponmac_drv->dev;

	GPON_OS_Lock(g_gponmac_drv->lock);

	/* disable top mask */
	rtk_gpon_topIntrMask_get(GPON_INTR_ALL,&mask);
	rtk_gpon_topIntrMask_set(GPON_INTR_ALL,0);
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Top Interrupt mask 0");

	/* read top int status */
	rtk_gpon_topIntr_get(GPON_INTR_ALL,&stat);
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Top Interrupt status %04x",stat);

	/* check int status */
	if((ret =rtk_gpon_topIntr_get(GPON_INTR_GTC_DS,&enable))!=RT_ERR_OK)
	{
		GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_DS failed, ret = %d",ret);
		return ;
	}
	/*dsGtc interrupt handler*/
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_DS is %d",enable);
	if(enable)
	{
	   	gpon_isr_dsGtc_check(obj);
	}
	/*check gtc us interrupt status*/
	if((ret =rtk_gpon_topIntr_get(GPON_INTR_GTC_US,&enable))!=RT_ERR_OK)
	{
		GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_US failed, ret = %d",ret);
		return ;
	}
	/*usGtc interrupt handler*/
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_US is %d",enable);
	if(enable)
	{
		gpon_isr_usGtc_check(obj);
	}
	/*check gem us interrupt status*/
	if((ret =rtk_gpon_topIntr_get(GPON_INTR_GEM_US,&enable))!=RT_ERR_OK)
	{
		GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GEM_US failed, ret = %d",ret);
		return ;
	}
	/*usGem interrupt handler*/
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GEM_US is %d",enable);
	if(enable)
       {
      		gpon_isr_usGem_check(obj);
       }
	/* enable top mask */
	rtk_gpon_topIntrMask_set(GPON_INTR_ALL,mask);
	GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Restore Top Interrupt mask 0x%x",mask);

	GPON_OS_Unlock(g_gponmac_drv->lock);

	/* switch interrupt clear GPON state */
	if((ret=rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
		return ;
	}

}




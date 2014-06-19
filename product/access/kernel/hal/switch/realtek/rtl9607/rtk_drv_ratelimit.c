/*****************************************************************************
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/
#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <hal/common/halctrl.h>

#include <rtk/rate.h>
#include <linux/string.h>
#include <linux/kernel.h>

#ifndef CONFIG_SDK_KERNEL_LINUX
#define CONFIG_SDK_KERNEL_LINUX
#endif
#include <dal/apollomp/raw/apollomp_raw_qos.h>

#define QOS_RATE_INPUT_MAX   1048568
#define QOS_RATE_INPUT_MIN  8
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static DRV_RET_E Hal_SetRatelimitCpu(void);
static DRV_RET_E Hal_getEmptyShareMeterIndx(UINT32 *pulMeterId);
static DRV_RET_E Hal_getSameShareMeterIndx(UINT32 ulMeterValue, UINT32 *pulMeterId);

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/

/*****************************************************************************
    Func Name:  Hal_SetRatelimitInit
  Description:  rate limit init
        Input:  
       Output: 
       Return:  DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetRatelimitInit(void)
{
    	return Hal_SetRatelimitCpu();
}

/*****************************************************************************
    Func Name:  Hal_SetRatelimitCpu
  Description:  set cpu receive rate limit
        Input:  ulKBps
       Output: 
       Return:  DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E Hal_SetRatelimitCpu(void)
{    
    rtk_qid_t queue;
    ret_t ret;
    rtk_api_ret_t ApiRet;
    DRV_RET_E enRet;
    UINT32 uiIpMeterId;
    UINT32 uiOthMeterId;
	/*begin added by wanghuanyu for 181*/	
   ApiRet = rtk_rate_portEgrBandwidthCtrlRate_set(PHY_CPU_PORTID, CPU_PORT_TOTAL_RATE_LIMIT*8*5);
   /*end added by wanghuanyu for 181*/
 //   ApiRet = rtk_rate_portEgrBandwidthCtrlRate_set(PHY_CPU_PORTID, 4);
    if (RT_ERR_OK != ApiRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
	ApiRet = rtk_rate_egrBandwidthCtrlIncludeIfg_set(ENABLED);
	if (RT_ERR_OK != ApiRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*enable cpu queue shaper*/
    /*First, get the same share meter.*/
    (void)Hal_getSameShareMeterIndx(CPU_PORT_TOTAL_RATE_LIMIT, &uiIpMeterId);
    if (RTL_METER_ID_INVALID == uiIpMeterId)
    {
        /*Then, create a new share meter for ip queue.*/
        /*1.get a new meter id.*/
        enRet = Hal_getEmptyShareMeterIndx(&uiIpMeterId);
        if ((DRV_OK != enRet) || (RTL_METER_ID_INVALID == uiIpMeterId))
        {
            return DRV_ERR_UNKNOW;
        }
        /*2.set share meter value.*/
        ret = rtk_rate_shareMeter_set(uiIpMeterId, CPU_PORT_TOTAL_RATE_LIMIT, ENABLED);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*First, get the same share meter.*/
    (void)Hal_getSameShareMeterIndx(CPU_QUEUE_RATE_LIMIT, &uiOthMeterId);
    if (RTL_METER_ID_INVALID == uiOthMeterId)
    {
        /*Then, create a new share meter for other queue.*/
        /*1.get a new meter id.*/
        enRet = Hal_getEmptyShareMeterIndx(&uiOthMeterId);
        if ((DRV_OK != enRet) || (RTL_METER_ID_INVALID == uiOthMeterId))
        {
            return DRV_ERR_UNKNOW;
        }
        /*2.set share meter value.*/
        ret = rtk_rate_shareMeter_set(uiOthMeterId, CPU_QUEUE_RATE_LIMIT*8, ENABLED);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    /* acl trap to cpu 的情况下不生效*/
    for(queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
    {       
		/*begin add wanghuanyu for 181*/
    	if(queue==7)
		{
			continue;
		}
		/*end add wanghuanyu for 181 */
        if(queue == DRV_PACKET_PRI_IP_CPU)
        {
           // (void)rtk_rate_egrQueueBwCtrlMeterIdx_set(PHY_CPU_PORTID, queue, uiOthMeterId);
            (void)rtk_rate_egrQueueBwCtrlMeterIdx_set(PHY_CPU_PORTID, queue, uiIpMeterId);            
        }
        else
        {
            (void)rtk_rate_egrQueueBwCtrlMeterIdx_set(PHY_CPU_PORTID, queue, uiOthMeterId);
        }

        (void)rtk_rate_egrQueueBwCtrlEnable_set(PHY_CPU_PORTID, queue, ENABLED);
    }
    
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetRatelimitByPort
  Description:  set rate limit by port
        Input:  direction
                lPortMask
                ulKBps
       Output: 
       Return:  DRV_OK
                DRV_ERR_PARA
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetRatelimitByPort(UINT32 direction, logic_pmask_t lPortMask, UINT32 ulKBps)
{
    port_num_t lport;
    rtk_port_t port;
    rtk_api_ret_t rtkApiRet;

    /*8kbps step with RTL832805*/
    if(!(ulKBps % 8))
    {
        ulKBps = (ulKBps/8)*8;
    }

	/*begin add for ingress port rate limit by shipeng 2013-09-05*/
	rtkApiRet = apollomp_raw_qos_bwCtrlIgrBound_set(0xc, 0xc);
	if (RT_ERR_OK != rtkApiRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
	/*end add for ingress port rate limit by shipeng 2013-09-05*/
    
    IfLgcMaskSet(&lPortMask, lport)
    {
        port = PortLogic2PhyPortId(lport);
        if(0 == ulKBps)
        {
            /*not limit*/
            if(DRV_RATELIMIT_TX == direction)
            {
                /*Exclude ifg.*/
                rtkApiRet = rtk_rate_portEgrBandwidthCtrlRate_set(port, QOS_RATE_INPUT_MAX);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
				rtkApiRet = rtk_rate_egrBandwidthCtrlIncludeIfg_set(DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
            }
            else if(DRV_RATELIMIT_RX == direction)
            {
                /*Exclude ifg and disable ingress flowctrl.*/
                rtkApiRet = rtk_rate_portIgrBandwidthCtrlRate_set(port, QOS_RATE_INPUT_MAX);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
				rtkApiRet = rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
				rtkApiRet = apollomp_raw_qos_bwCtrlIgrRateFc_set(port, DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
            }
            else
            {
                return DRV_ERR_PARA;
            }
        }
        else
        {
            /*set limit*/
            if(DRV_RATELIMIT_TX == direction)
            {
                /*Exclude ifg.*/
                rtkApiRet = rtk_rate_portEgrBandwidthCtrlRate_set(port, ulKBps);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
				rtkApiRet = rtk_rate_egrBandwidthCtrlIncludeIfg_set(DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
            }
            else if(DRV_RATELIMIT_RX == direction)
            {
                /*Exclude ifg and disable ingress flowctrl.*/
                rtkApiRet = rtk_rate_portIgrBandwidthCtrlRate_set(port, ulKBps);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
				rtkApiRet = rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
				rtkApiRet = apollomp_raw_qos_bwCtrlIgrRateFc_set(port, DISABLED);
				if (RT_ERR_OK != rtkApiRet)
			    {
			        return DRV_SDK_GEN_ERROR;
			    }
            }
            else
            {
                return DRV_ERR_PARA;
            }
        }
    }

    return DRV_OK;
}

static UINT32 _Hal_GetPortRateByPercentage(port_num_t lport, UINT32 percentage)
{
    UINT32 uiRate = 0, uiSpeed = 0;

    if(DRV_OK == Hal_GetPortSpeed(lport, &uiSpeed))
    {
        if(PORT_SPEED_10 == uiSpeed)
        {
            /*10Mbps port, kbps*/
            uiRate = 100*percentage;
        }
        else if(PORT_SPEED_100 == uiSpeed)
        {
            /*100Mbps port, kbps*/
            uiRate = 1000*percentage;
        }
        else if(PORT_SPEED_1000 == uiSpeed)
        {
            /*1000Mbps port, kbps*/
            uiRate = 10000*percentage;
        }
        else
        {
            return 0xffffffff;
        }
    }

    return uiRate;
}

static DRV_RET_E Hal_storm_controlRate_set(rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_t rate, rtk_enable_t ifg_include)
{
    rtk_api_ret_t retVal;
    rtk_enable_t enable;
    rtk_rate_storm_group_ctrl_t  stormCtrl;
	DRV_RET_E enRet;
	UINT32 uiNewMeterId;

    if ((port > RTK_MAX_PORT_ID) ||
		(storm_type >= STORM_GROUP_END) ||
		(rate > QOS_RATE_INPUT_MAX) ||
		(rate < QOS_RATE_INPUT_MIN) ||
		(rate % 8))
    {
        return DRV_ERR_PARA;
    }

    if (rate == QOS_RATE_INPUT_MAX)
	{
		enable = DISABLED;
	}
	else
	{
        enable = ENABLED;
	}

    /*added by feihuaxin ,maybe it should move to init */
    memset(&stormCtrl, 0, sizeof(rtk_rate_storm_group_ctrl_t));
    stormCtrl.unknown_unicast_enable = 1;
    stormCtrl.unknown_multicast_enable = 1;
    stormCtrl.multicast_enable = 1;
    stormCtrl.broadcast_enable = 1;
    
    if ((retVal = rtk_rate_stormControlEnable_set(&stormCtrl)) != RT_ERR_OK)
    {
		return DRV_SDK_GEN_ERROR;
	}

	if ((retVal = rtk_rate_stormControlPortEnable_set(port, storm_type, enable)) != RT_ERR_OK)
	{
		return DRV_SDK_GEN_ERROR;
	}
	
	if (ENABLED == enable)
	{			
		(void)Hal_getSameShareMeterIndx(rate, &uiNewMeterId);
		if (RTL_METER_ID_INVALID == uiNewMeterId)
    	{
			enRet = Hal_getEmptyShareMeterIndx(&uiNewMeterId);
		    if ((DRV_OK != enRet) || (RTL_METER_ID_INVALID == uiNewMeterId))
		    {
		        return DRV_ERR_UNKNOW;
		    }
		}

		if ((retVal = rtk_rate_shareMeter_set(uiNewMeterId, rate, ifg_include)) != RT_ERR_OK)
		{
			return DRV_SDK_GEN_ERROR;
		}
		
		if ((retVal = rtk_rate_stormControlMeterIdx_set(port, storm_type, uiNewMeterId)) != RT_ERR_OK)
		{
			return DRV_SDK_GEN_ERROR;
		}				
	}

    return DRV_OK;
}


/*****************************************************************************
    Func Name:  Hal_SetRatelimitStormCtl
  Description:  set storm control by port mask
        Input:  lPortMask
                ctlType
                percentage
       Output: 
       Return:  DRV_OK
                DRV_ERR_PARA
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetRatelimitStormCtl(STORM_CTLTYPE_E ctlType, logic_pmask_t lPortMask, STORM_CTLRATE_S stStorm)
{
    port_num_t lport;
    rtk_port_t phyPort;
    UINT32 ulRate;
    DRV_RET_E rtkApiRet;

    /*8305 does not support this type.*/
    if (STORM_RATE_PPS == stStorm.rateType)
    {
        return DRV_ERR_PARA;
    }
    
    ulRate = stStorm.value;
    
    switch(ctlType)
    {
        case STORM_DLF_CTRL:
			//printk("%s %d %d %d\n", __FUNCTION__, __LINE__, stStorm.value, stStorm.rateType);
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT))) 
                {
                    ulRate = QOS_RATE_INPUT_MAX;
                }
                else
                {
                    switch (stStorm.rateType)
                    {
                        case STORM_RATE_KBPS:
                            break;
                        case STORM_RATE_PERCENT:
                            ulRate = _Hal_GetPortRateByPercentage(lport, stStorm.value);
                            break;
                        default:
                            return DRV_ERR_PARA;
                    } 
                }

				#if 0
                /*Reset multicast and broadcast rate control.*/
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_MULTICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_MULTICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_BROADCAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                #endif
				
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_UNICAST, 
                                                      ulRate, 
                                                      DISABLED);
                if (DRV_OK != rtkApiRet)
                {                	
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_MCAST_CTRL:
			//printk("%s %d %d %d\n", __FUNCTION__, __LINE__, stStorm.value, stStorm.rateType);
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = QOS_RATE_INPUT_MAX;
                }
                else
                {
                    switch (stStorm.rateType)
                    {
                        case STORM_RATE_KBPS:
                            break;
                        case STORM_RATE_PERCENT:
                            ulRate = _Hal_GetPortRateByPercentage(lport, stStorm.value);
                            break;
                        default:
                            return DRV_ERR_PARA;
                    }
                }

				#if 0
                /*Reset unicast and broadcast rate control.*/
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_UNICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_BROADCAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
				#endif
                
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED);
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED);
                if (DRV_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_BCAST_CTRL:
			//printk("%s %d %d %d\n", __FUNCTION__, __LINE__, stStorm.value, stStorm.rateType);
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = QOS_RATE_INPUT_MAX;
                }
                else
                {
                    switch (stStorm.rateType)
                    {
                        case STORM_RATE_KBPS:
                            break;
                        case STORM_RATE_PERCENT:
                            ulRate = _Hal_GetPortRateByPercentage(lport, stStorm.value);
                            break;
                        default:
                            return DRV_ERR_PARA;
                    }
                }

				#if 0
                /*Reset multicast and unicast rate control.*/
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_MULTICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_MULTICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                (void)Hal_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_UNICAST, 
                                                QOS_RATE_INPUT_MAX, 
                                                DISABLED);
                #endif
				
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_BROADCAST, 
                                                      ulRate, 
                                                      DISABLED);
                if (DRV_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_ALL_CTRL:
			//printk("%s %d %d %d\n", __FUNCTION__, __LINE__, stStorm.value, stStorm.rateType);
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = QOS_RATE_INPUT_MAX;
                }
                else
                {
                    switch (stStorm.rateType)
                    {
                        case STORM_RATE_KBPS:
                            break;
                        case STORM_RATE_PERCENT:
                            ulRate = _Hal_GetPortRateByPercentage(lport, stStorm.value);
                            break;
                        default:
                            return DRV_ERR_PARA;
                    }
                }

                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_UNICAST, 
                                                      ulRate, 
                                                      DISABLED);
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED);
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED);
                rtkApiRet = Hal_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_BROADCAST, 
                                                      ulRate, 
                                                      DISABLED);
                if (DRV_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        default:
            return DRV_ERR_PARA;
    }       

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_getEmptyShareMeterIndx
  Description: Get valid meter id.
        Input: UINT32 *pulMeterId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E Hal_getEmptyShareMeterIndx(UINT32 *pulMeterId)
{
    ret_t ret;
    rtk_uint32 uiMeterId;
    rtk_uint32 uiMeterValue;
    rtk_uint32 uiIfg;

    *pulMeterId = RTL_METER_ID_INVALID;
    
    for (uiMeterId = 0; uiMeterId < HAL_MAX_NUM_OF_METERING(); uiMeterId++)
    {
        uiMeterValue = 0;
        ret = rtk_rate_shareMeter_get(uiMeterId, &uiMeterValue, &uiIfg);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }

        if (0 == uiMeterValue)
        {
            *pulMeterId = uiMeterId;
            break;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_getSameShareMeterIndx
  Description: Get the same share meter id.
        Input: UINT32 ulMeterValue  
                UINT32 *pulMeterId   
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E Hal_getSameShareMeterIndx(UINT32 ulMeterValue, UINT32 *pulMeterId)
{
    ret_t ret;
    rtk_uint32 ulMeterId;
    rtk_uint32 ulMeterValueTemp;
    rtk_uint32 ulIfg;

    *pulMeterId = RTL_METER_ID_INVALID;
    
    for (ulMeterId = 0; ulMeterId <  HAL_MAX_NUM_OF_METERING(); ulMeterId++)
    {
        ulMeterValueTemp = 0;
        ret = rtk_rate_shareMeter_get(ulMeterId, &ulMeterValueTemp, &ulIfg);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }

        if (0 != ulMeterValueTemp)
        {
            if (ulMeterValue == ulMeterValueTemp)
            {
                *pulMeterId = ulMeterId;
                break;
            }
        }
    }

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif



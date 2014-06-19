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
#ifdef CHIPSET_RTL8328
#include <common/error.h>
#include <rtk/rate.h>
#include <common/rt_type.h>
#include <hal/common/halctrl.h>
#elif defined(CHIPSET_RTL8305)
#include "rtk_api.h"
#include "rtk_api_ext.h"
#include "rtl8367b_asicdrv.h"
#include "rtl8367b_asicdrv_meter.h"
#endif

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
#if 0
    port_num_t lport;
    UINT32 unit;
    rtk_port_t port;
    rtk_rate_thresh_t stThresh;

    stThresh.FC_On = 0x8F;
    stThresh.FC_Off = 0x7F;

    /*set storm control mode, rate limit mode*/
    LgcPortFor(lport)
    {
        unit = PortLogci2ChipId(lport);
        port = PortLogic2PhyPortId(lport);

        /*Bandwith control exclude ipg&preamble.*/
        #if 0
        rtl8367b_setAsicPortSmallIpg(port, (rtk_uint32)(DISABLED));
        rtl8367b_setAsicPortEgressRateIfg((rtk_uint32)(DISABLED));
        #endif
        //(void)rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(unit, port, DISABLED);
        //(void)rtk_rate_portEgrBandwidthCtrlIncludeIfg_set(unit, port, DISABLED);
        //(void)rtk_rate_igrBandwidthFlowctrlEnable_set(unit, port, ENABLED);
        //(void)rtk_rate_igrBandwidthFlowctrlThresh_set(unit, port, &stThresh);
    }

    UnitFor(unit)
    {
        (void)rtk_rate_stormControlRefreshMode_set(unit, BASED_ON_BYTE);
        (void)rtk_rate_stormControlIncludeIfg_set(unit, ENABLED);
    }
#endif
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

    ApiRet = rtk_rate_egrBandwidthCtrlRate_set(PHY_CPU_PORTID, CPU_PORT_TOTAL_RATE_LIMIT, ENABLED);
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
        ret = rtl8367b_setAsicShareMeter(uiIpMeterId, CPU_PORT_TOTAL_RATE_LIMIT, ENABLED);
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
        if ((DRV_OK != enRet) || (RTL_METER_ID_INVALID == uiIpMeterId))
        {
            return DRV_ERR_UNKNOW;
        }
        /*2.set share meter value.*/
        ret = rtl8367b_setAsicShareMeter(uiOthMeterId, CPU_QUEUE_RATE_LIMIT, ENABLED);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    
    for(queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
    {        
        if(queue == DRV_PACKET_PRI_IP_CPU)
        {
            (void)rtk_rate_egrQueueBwCtrlRate_set(PHY_CPU_PORTID, queue, uiIpMeterId);            
        }
        else
        {
            (void)rtk_rate_egrQueueBwCtrlRate_set(PHY_CPU_PORTID, queue, uiOthMeterId);
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
    
    IfLgcMaskSet(&lPortMask, lport)
    {
        port = PortLogic2PhyPortId(lport);
        if(0 == ulKBps)
        {
            /*not limit*/
            if(DRV_RATELIMIT_TX == direction)
            {
                /*Exclude ifg.*/
                rtkApiRet = rtk_rate_egrBandwidthCtrlRate_set(port, RTL8367B_QOS_RATE_INPUT_MAX, DISABLED);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            else if(DRV_RATELIMIT_RX == direction)
            {
                /*Exclude ifg and disable ingress flowctrl.*/
                rtkApiRet = rtk_rate_igrBandwidthCtrlRate_set(port, RTL8367B_QOS_RATE_INPUT_MAX, DISABLED, DISABLED);
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
                rtkApiRet = rtk_rate_egrBandwidthCtrlRate_set(port, ulKBps, DISABLED);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            else if(DRV_RATELIMIT_RX == direction)
            {
                /*Exclude ifg and disable ingress flowctrl.*/
                rtkApiRet = rtk_rate_igrBandwidthCtrlRate_set(port, ulKBps, DISABLED, DISABLED);
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
    rtk_api_ret_t rtkApiRet;

    /*8305 does not support this type.*/
    if (STORM_RATE_PPS == stStorm.rateType)
    {
        return DRV_ERR_PARA;
    }
    
    ulRate = stStorm.value;
    
    switch(ctlType)
    {
        case STORM_DLF_CTRL:
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT))) 
                {
                    ulRate = RTL8367B_QOS_RATE_INPUT_MAX;
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

                /*Reset multicast and broadcast rate control.*/
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_MULTICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_MULTICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_BROADCAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_UNICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_MCAST_CTRL:
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = RTL8367B_QOS_RATE_INPUT_MAX;
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

                /*Reset unicast and broadcast rate control.*/
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_UNICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_BROADCAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                rtkApiRet += rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_BCAST_CTRL:
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = RTL8367B_QOS_RATE_INPUT_MAX;
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

                /*Reset multicast and unicast rate control.*/
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_MULTICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_MULTICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                (void)rtk_storm_controlRate_set(phyPort, 
                                                STORM_GROUP_UNKNOWN_UNICAST, 
                                                RTL8367B_QOS_RATE_INPUT_MAX, 
                                                DISABLED, MODE0);
                
                /*Exclude ifg and use mode0(mode1 uses share metre).*/
                rtkApiRet = rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_BROADCAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case STORM_ALL_CTRL:
            IfLgcMaskSet(&lPortMask, lport)
            {
                phyPort = PortLogic2PhyPortId(lport);
                if((0 == stStorm.value) ||
                   ((100 == stStorm.value) && (stStorm.rateType == STORM_RATE_PERCENT)))
                {
                    ulRate = RTL8367B_QOS_RATE_INPUT_MAX;
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
                rtkApiRet = rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_UNICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                rtkApiRet = rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_UNKNOWN_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                rtkApiRet += rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_MULTICAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                rtkApiRet += rtk_storm_controlRate_set(phyPort, 
                                                      STORM_GROUP_BROADCAST, 
                                                      ulRate, 
                                                      DISABLED, MODE0);
                if (RT_ERR_OK != rtkApiRet)
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
    
    for (uiMeterId = 0; uiMeterId < RTL8367B_METERNO; uiMeterId++)
    {
        uiMeterValue = 0;
        ret = rtl8367b_getAsicShareMeter(uiMeterId, &uiMeterValue, &uiIfg);
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
    
    for (ulMeterId = 0; ulMeterId < RTL8367B_METERNO; ulMeterId++)
    {
        ulMeterValueTemp = 0;
        ret = rtl8367b_getAsicShareMeter(ulMeterId, &ulMeterValueTemp, &ulIfg);
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



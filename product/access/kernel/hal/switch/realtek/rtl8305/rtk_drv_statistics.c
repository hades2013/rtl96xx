/*****************************************************************************

------------------------------------------------------------------------------
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
#include <rtk/stat.h>
#include <common/rt_type.h>
#include <hal/common/halctrl.h>
#elif defined(CHIPSET_RTL8305)
#include "rtk_api.h"
#include "rtk_api_ext.h"
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
    Func Name:  Hal_GetMibPortStatistics
  Description:  get mib counter by port
        Input:  lport
                PktType
       Output:  p_ullStat
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution:  RTL8328:In port 0~15, fragment packets will be sometimes treated as
                aglignment packets. In port 16~23, alignment packets will be
                treated as FCS error packets.
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMibPortStatistics(port_num_t lport, MIB_IfStatType_t PktType, UINT64 *p_ullStat)
{
    rtk_stat_port_type_t stPortStatType;
    UINT64 ullValue = 0;
    rtk_port_t port = 0;
    
    /* 014043 */
#if defined(CHIPSET_RTL8305)
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
#else
    if(!VALID_PORT(lport))
#endif
    /* 014043 */
    {
        return DRV_ERR_PARA;
    }
    if(NULL == p_ullStat)
    {
        return DRV_ERR_PARA;
    }
    *p_ullStat = 0;
    port = PortLogic2PhyPortId(lport);

    switch (PktType)
    {
        case INOCTETS:
            stPortStatType = STAT_IfInOctets;
            break;
        case INUCAST:
            stPortStatType = STAT_IfInUcastPkts;
            break;
        case INNUCAST:
            (void)rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case INDISCARDS:
            stPortStatType = STAT_EtherStatsDropEvents;
            break;
        case INERRS:
            (void)rtk_stat_port_get(port, STAT_EtherStatsDropEvents, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case INUNKNOWN:
            return DRV_ERR_PARA;
        case OUTOCTETS:
            stPortStatType = STAT_IfOutOctets;
            break;
        case OUTUCAST:
            stPortStatType = STAT_IfOutUcastPkts;
            break;
        case OUTNUCAST:
            (void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case OUTDISCARDS:
            (void)rtk_stat_port_get(port, STAT_EtherStatsCollisions, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsLateCollisions, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case OUTERRS:
            (void)rtk_stat_port_get(port, STAT_EtherStatsCollisions, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsLateCollisions, (rtk_stat_counter_t *)&ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case OUTQLEN:
        case HCINOCTETS:
        case HCINUCAST:
        case HCINMCAST:
        case HCINBCAST:
        case HCOUTOCTETS:
        case HCOUTUCAST:
        case HCOUTMCAST:
        case HCOUTBCAST:
            return DRV_ERR_PARA;
        case PORTDELAYEXEEDE_DISCARDS:
            stPortStatType = STAT_Dot1dBasePortDelayExceededDiscards;
            break;        
        case PORTMTUEXCEED_DISCARDS:
            return DRV_ERR_PARA; 
        case TPPPORT_IN_FRAMES:
            return DRV_ERR_PARA; 
        case TPPORT_OUT_FRAMS:
            return DRV_ERR_PARA; 
        case PORT_IN_DISCARDS:
            stPortStatType = STAT_EtherStatsDropEvents;
            break;
        case DOT3STATSALIGNERR:
            return DRV_ERR_PARA; 
        case DOT3STATSFCSERR:
            stPortStatType = STAT_Dot3StatsFCSErrors;
            break;
        case DOT3STATSSINGLECOLLISION:
            stPortStatType = STAT_Dot3StatsSingleCollisionFrames;
            break;
        case DOT3STATSMULTICOLLISION:
            stPortStatType = STAT_Dot3StatsMultipleCollisionFrames;
            break;
        case DOT3STATSSQETESTERR:
            return DRV_ERR_PARA; 
        case DOT3STATSDEFERREDTX:
            stPortStatType = STAT_Dot3StatsDeferredTransmissions;
            break;
        case DOT3STATSLATECOLLISION:
            stPortStatType = STAT_Dot3StatsLateCollisions;
            break;
        case DOT3STATSEXCOLLSION:
            stPortStatType = STAT_Dot3StatsExcessiveCollisions;
            break;
        case DOT3STATSINTERMACTXERR:
            return DRV_ERR_PARA;
        case DOT3STATSCARRIERSENSEERR:
            return DRV_ERR_PARA;
        case DOT3STATSFRAMETOOLONG:
            stPortStatType = STAT_EtherOversizeStats;
            break;
        case DOT3STATSINTERMACRXERR:
            return DRV_ERR_PARA;
        case DOT3STATSSYMBOLERR:
            stPortStatType = STAT_Dot3StatsSymbolErrors;
            break;
        default:
            return DRV_ERR_PARA;
    }

    if(RT_ERR_OK == rtk_stat_port_get(port, stPortStatType, p_ullStat))
    {
        return DRV_OK;
    }
    
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name:  Hal_SetMibCountSync
  Description:  sync port statistics
        Input:  
       Output:  
       Return:  DRV_OK                
      Caution:  do not call this function consecutively
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMibCountSync(void)
{
    /*no need to sync mib count with RTL8328*/
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMibCountReset
  Description:  port counter reset
        Input:  lport
       Output:  
       Return:  DRV_OK
                DRV_ERR_PARA
                DRV_ERR_UNKNOW
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMibCountReset(port_num_t lport)
{
    /* 014043 */
#if defined(CHIPSET_RTL8305)
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
#else
    if(!VALID_PORT(lport))
#endif
    /* 014043 */
    {
        return DRV_ERR_PARA;
    }
    if(RT_ERR_OK != rtk_stat_port_reset(PortLogic2PhyPortId(lport)))
    {
        return DRV_ERR_UNKNOW;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_GetPortCounter
  Description:  get port counter
        Input:  lport
                counterName                
       Output:  returnValue
       Return:  DRV_OK
                DRV_ERR_PARA                
      Caution:  RTL8328:In port 0~15, fragment packets will be sometimes treated as
                aglignment packets. In port 16~23, alignment packets will be
                treated as FCS error packets.
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetPortCounter(port_num_t lport, CounterName_E counterName, UINT64 *returnValue)
{
    UINT64 ullValue = 0;
    rtk_port_t port = 0;
    
    /* 014043 */
#if defined(CHIPSET_RTL8305)
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
#else
    if(!VALID_PORT(lport))
#endif
    /* 014043 */
    {
        return DRV_ERR_PARA;
    }
    if(NULL == returnValue)
    {
        return DRV_ERR_PARA;
    }

    *returnValue = 0;
    port = PortLogic2PhyPortId(lport);
    switch(counterName)
    {
        case INPUT_TOTAL_PACKETS:
            (void)rtk_stat_port_get(port, STAT_IfInUcastPkts, &ullValue);
            *returnValue += ullValue;
			/* 014043 */	
			#if defined(CHIPSET_RTL8305)
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, &ullValue);
            *returnValue += ullValue;

            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, &ullValue);
            *returnValue -= ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, &ullValue);
            *returnValue -= ullValue;

            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsUnderSizePkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsFCSErrors, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get( port, STAT_EtherStatsFragments, &ullValue);
            *returnValue += ullValue;
#else
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, &ullValue);
            *returnValue += ullValue;

			/* 014043 */
            ullValue = 0;
            (void)Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            *returnValue += ullValue;

            /*除去统计入错误帧的pause帧，*/            
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;            
            /*除去ignore错误包带入的计数*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsUnderSizePkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsFCSErrors, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            //(void)rtk_stat_port_get(port, DOT3_STATS_ALIGNMENT_ERRORS_INDEX, &ullValue);
            //*returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get( port, STAT_EtherStatsFragments, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsJabbers, &ullValue);
            *returnValue += ullValue;
            //uiValue = 0;
            //(void)rtk_stat_port_get(unit, port, DOT3_STATS_SYMBOL_ERRORS_INDEX, &uiValue);
            //*returnValue += uiValue;  
            ullValue = 0;
            Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
#endif
            /* 014538 */
            return DRV_OK;
        case INPUT_TOTAL_BYTES:
            (void)rtk_stat_port_get(port, STAT_IfInOctets, returnValue);
            return DRV_OK;
        case INPUT_TOTAL_BROADCASTS:
			/* 014043 */
			#if defined(CHIPSET_RTL8305)
            (void)rtk_stat_port_get(port, STAT_IfInBroadcastPkts, &ullValue);
            #else
			(void)rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, returnValue);
			#endif
			/* 014043 */
            return DRV_OK;
        case INPUT_TOTAL_MULTICASTS:
			/* 014043 */
			#if defined(CHIPSET_RTL8305)
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfInMulticastPkts, &ullValue);
			#else
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, &ullValue);
			#endif
			/* 014043 */
            *returnValue += ullValue;
             /*除去统计入多播包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            return DRV_OK;
        case INPUT_ERRORS:
             ullValue = 0;
             Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            *returnValue += ullValue;
            /*除去统计入错误包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
             return DRV_OK;
        case INPUT_PAUSE_PACKETS:
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_PACKETS:
            (void)rtk_stat_port_get(port, STAT_IfOutUcastPkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)Hal_GetMibPortStatistics(lport, OUTERRS, &ullValue);
            *returnValue += ullValue;
            return DRV_OK;
        case OUTPUT_TOTAL_BYTES:
            (void)rtk_stat_port_get(port, STAT_IfOutOctets, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_BROADCASTS:
            (void)rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_MULTICASTS:
            (void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, returnValue);
            return DRV_OK;
        case OUTPUT_ERRORS:
           return Hal_GetMibPortStatistics(lport, OUTERRS, returnValue);
        case OUTPUT_PAUSE_PACKETS:
            (void)rtk_stat_port_get(port, STAT_Dot3OutPauseFrames, returnValue);
            return DRV_OK;
        default:
            return DRV_ERR_PARA;
    }
}

/*****************************************************************************
    Func Name:  Hal_GetPortStatistics
  Description:  get port statistics
        Input:  lport
                statisticsName                
       Output:  returnValue
       Return:  DRV_OK
                DRV_ERR_PARA
                DRV_ERR_UNKNOW
      Caution:  RTL8328:In port 0~15, fragment packets will be sometimes treated as
                aglignment packets. In port 16~23, alignment packets will be
                treated as FCS error packets.
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortStatistics(port_num_t lport, stat_reg_t statisticsName, UINT64 *returnValue)
{
    rtk_stat_port_type_t stPortStatType;
    UINT64 ullValue = 0;
    rtk_port_t port = 0;
/* 014043 */
#if defined(CHIPSET_RTL8305)
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
#else
    if(!VALID_PORT(lport))
#endif
/*014043 */
    {
        return DRV_ERR_PARA;
    }
    if(NULL == returnValue)
    {
        return DRV_ERR_PARA;
    }

    *returnValue = 0;
    port = PortLogic2PhyPortId(lport);

    switch(statisticsName)
    {
        //RX
        case RxDropPkts:
			#if defined(CHIPSET_RTL8305)
            stPortStatType = STAT_Dot1dTpPortInDiscards;
            break;
			#else
            /*ignore:rx drop as lack of buffer*/
             ullValue = 0;
             Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            *returnValue += ullValue;
            /*除去统计入错误包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            /*用输入总错误包减其他错误包的方法得到ignored包数*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsUnderSizePkts, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsFCSErrors, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            #if 0
            ullValue = 0;
            (void)rtk_stat_port_get(port, DOT3_STATS_ALIGNMENT_ERRORS_INDEX, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            #endif
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsFragments, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsJabbers, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            /*处理symbol错误帧，同时统计入其他错误包导致重复统计问题*/
            //uiValue = 0;
            //(void)rtk_stat_port_get(PortLogci2ChipId(lport), PortLogic2PhyPortId(lport), DOT3_STATS_SYMBOL_ERRORS_INDEX, &uiValue);
            return DRV_OK;           
			#endif
        case RxOctets:
            stPortStatType = STAT_IfInOctets;
            break;
        case RxBroadcastPkts:         			
		#if defined(CHIPSET_RTL8305)
		    ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, &ullValue);
            *returnValue += ullValue;
			/*除去出口广播包*/
		    ullValue = 0;
			(void)rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, &ullValue);
			if((*returnValue) < ullValue)
            {
                *returnValue = 0;
            }
			else
			{
			    *returnValue -= ullValue;
			}
			return DRV_OK;
		#else
            stPortStatType = STAT_EtherStatsBroadcastPkts;
            break;
        #endif		
            
        case RxMulticastPkts:
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, &ullValue);
            *returnValue += ullValue;
            /*除去统计入多播包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3InPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
			/*除去出口组播包*/
		#if defined(CHIPSET_RTL8305)
		    ullValue = 0;
			(void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, &ullValue);
			if((*returnValue) < ullValue)
            {
                *returnValue = 0;
            }
			else
			{
			    *returnValue -= ullValue;
			}
        #endif		
            return DRV_OK;
        case RxSAChanges:
            return DRV_ERR_PARA;
        case RxUdersizePkts:
            stPortStatType = STAT_EtherStatsUnderSizePkts;
            break;
        case RxOversizePkts:
            stPortStatType = STAT_EtherOversizeStats;
            break;
        case RxFragment:
            stPortStatType = STAT_EtherStatsFragments;
            break;
        case RxJabbers:
            stPortStatType = STAT_EtherStatsJabbers;
            break;
        case RxUnicastPkts:
            stPortStatType = STAT_IfInUcastPkts;
            break;
        case RxAlignmentErrors:
            return DRV_ERR_PARA;
        case RxFCSErrors:
            stPortStatType = STAT_Dot3StatsFCSErrors;
            break;
        case RxGoodOctets:
            stPortStatType = STAT_IfInOctets;
            break;
        case RxExcessSizeDisc:
            stPortStatType = STAT_EtherOversizeStats;
            break;
        case RxPausePkts:
            stPortStatType = STAT_Dot3InPauseFrames;
            break;
        case RxSymbolErrors:
            stPortStatType = STAT_Dot3StatsSymbolErrors;
            break;
        case RxQosQPkt:
            return DRV_ERR_PARA;
        case RxQosQOctet:
            return DRV_ERR_PARA;
        //TX
        case TxDropPkts:
            #if 0
            stPortStatType = IF_OUT_DISCARDS_INDEX;
            break;
            #else
            return DRV_ERR_PARA;
            #endif
        case TxOctets:
            stPortStatType = STAT_IfOutOctets;
            break;
        case TxBroadcastPkts:
            stPortStatType = STAT_IfOutBroadcastPkts;
            break;
        case TxMulticastPkts:
            /*输出多播包数去除pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_IfOutMulticastPkts, &ullValue);
            *returnValue += ullValue;
            /*除去统计入多播包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3OutPauseFrames, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            return DRV_OK;
        case TxCollisions:
            stPortStatType = STAT_EtherStatsCollisions;
            break;
        case TxUnicastPkts:
            stPortStatType = STAT_IfOutUcastPkts;
            break;
        case TxSingleCollision:
            stPortStatType = STAT_Dot3StatsSingleCollisionFrames;
            break;
        case TxMultipleCollision:
            stPortStatType = STAT_Dot3StatsMultipleCollisionFrames;
            break;
        case TxDeferredTransmit:
            stPortStatType = STAT_Dot3StatsDeferredTransmissions;
            break;
        case TxLateCollision:
            stPortStatType = STAT_Dot3StatsLateCollisions;
            break;
        case TxPausePkts:
            stPortStatType = STAT_Dot3OutPauseFrames;
            break;
        case TxFrameInDisc:
            return DRV_ERR_PARA;
        case TxQoSQPkt:
            return DRV_ERR_PARA;
        case TxQoSQOctet:
            return DRV_ERR_PARA;
        case Pkt64Octets:
            stPortStatType = STAT_EtherStatsPkts64Octets;
            break;
        case Pkt65to127Octets:
            stPortStatType = STAT_EtherStatsPkts65to127Octets;
            break;
        case Pkt128to255Octets:
            stPortStatType = STAT_EtherStatsPkts128to255Octets;
            break;
        case Pkt256to511Octets:
            stPortStatType = STAT_EtherStatsPkts256to511Octets;
            break;
        case Pkt512to1023Octets:
            stPortStatType = STAT_EtherStatsPkts512to1023Octets;
            break;
        case Pkt1024to1522octets:
            stPortStatType = STAT_EtherStatsPkts1024to1518Octets;
            break;
        case RxErrPkts:
            ullValue = 0;
            Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            *returnValue += ullValue;
            /*除去统计入错误包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, STAT_Dot3StatsFCSErrors, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
            return DRV_OK;          
        case TxErrPkts:
            return Hal_GetMibPortStatistics(lport, OUTERRS, returnValue);
        default:
            return DRV_ERR_PARA;
    }
    
    
    if(RT_ERR_OK == rtk_stat_port_get(port, stPortStatType, returnValue))
    {
        return DRV_OK;
    }
    
    return DRV_ERR_UNKNOW;
}

#ifdef  __cplusplus
}
#endif



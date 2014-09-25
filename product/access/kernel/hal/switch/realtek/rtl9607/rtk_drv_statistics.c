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
#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/stat.h>

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

DRV_RET_E Hal_GetMibPortStatisticsRaw(port_num_t lport, UINT32 mibType, UINT64 *p_ullStat)
{
    UINT64 ullValue = 0;
    rtk_port_t port = 0;
    
    if(NULL == p_ullStat)
    {
        return DRV_ERR_PARA;
    }
    *p_ullStat = 0;
    port = PortLogic2PhyPortId(lport);
        
    if(RT_ERR_OK == rtk_stat_port_get(port, mibType, &ullValue))
    {
        *p_ullStat = ullValue;
        return DRV_OK;
    }
    
    return DRV_ERR_UNKNOW;
}


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
    
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
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
            stPortStatType = IF_IN_OCTETS_INDEX;
            break;
        case INUCAST:
            stPortStatType = IF_IN_UCAST_PKTS_INDEX;
            break;
        case INNUCAST:
            (void)rtk_stat_port_get(port, ETHER_STATS_MULTICAST_PKTS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, ETHER_STATS_BROADCAST_PKTS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case INDISCARDS:
            stPortStatType = ETHER_STATS_DROP_EVENTS_INDEX;
            break;
        case INERRS:
            (void)rtk_stat_port_get(port, ETHER_STATS_DROP_EVENTS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case INUNKNOWN:
            return DRV_ERR_PARA;
        case OUTOCTETS:
            stPortStatType = IF_OUT_OCTETS_INDEX;
            break;
        case OUTUCAST:
            stPortStatType = IF_OUT_UCAST_PKTS_CNT_INDEX;
            break;
        case OUTNUCAST:
            (void)rtk_stat_port_get(port, IF_OUT_MULTICAST_PKTS_CNT_INDEX, &ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_OUT_BROADCAST_PKTS_CNT_INDEX, &ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case OUTDISCARDS:
            (void)rtk_stat_port_get(port, ETHER_STATS_COLLISIONS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, DOT3_STATS_LATE_COLLISIONS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            return DRV_OK;
        case OUTERRS:
            (void)rtk_stat_port_get(port, ETHER_STATS_COLLISIONS_INDEX, &ullValue);
            *p_ullStat += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, DOT3_STATS_LATE_COLLISIONS_INDEX, &ullValue);
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
            stPortStatType = DOT1D_BASE_PORT_DELAY_EXCEEDED_DISCARDS_INDEX;
            break;        
        case PORTMTUEXCEED_DISCARDS:
            return DRV_ERR_PARA; 
        case TPPPORT_IN_FRAMES:
            return DRV_ERR_PARA; 
        case TPPORT_OUT_FRAMS:
            return DRV_ERR_PARA; 
        case PORT_IN_DISCARDS:
            stPortStatType = ETHER_STATS_DROP_EVENTS_INDEX;
            break;
        case DOT3STATSALIGNERR:
            return DRV_ERR_PARA; 
        case DOT3STATSFCSERR:
            stPortStatType = ETHER_STATS_CRC_ALIGN_ERRORS_INDEX;
            break;
        case DOT3STATSSINGLECOLLISION:
            stPortStatType = DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX;
            break;
        case DOT3STATSMULTICOLLISION:
            stPortStatType = DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX;
            break;
        case DOT3STATSSQETESTERR:
            return DRV_ERR_PARA; 
        case DOT3STATSDEFERREDTX:
            stPortStatType = DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX;
            break;
        case DOT3STATSLATECOLLISION:
            stPortStatType = DOT3_STATS_LATE_COLLISIONS_INDEX;
            break;
        case DOT3STATSEXCOLLSION:
            stPortStatType = DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX;
            break;
        case DOT3STATSINTERMACTXERR:
            return DRV_ERR_PARA;
        case DOT3STATSCARRIERSENSEERR:
            return DRV_ERR_PARA;
        case DOT3STATSFRAMETOOLONG:
            stPortStatType = ETHER_STATS_OVERSIZE_PKTS_INDEX;
            break;
        case DOT3STATSINTERMACRXERR:
            return DRV_ERR_PARA;
        case DOT3STATSSYMBOLERR:
            stPortStatType = DOT3_STATS_SYMBOL_ERRORS_INDEX;
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
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
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
    
    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
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
            (void)rtk_stat_port_get(port, IF_IN_UCAST_PKTS_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_IN_MULTICAST_PKTS_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_IN_BROADCAST_PKTS_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, DOT3_IN_PAUSE_FRAMES_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
			(void)rtk_stat_port_get(port, ETHER_STATS_CRC_ALIGN_ERRORS_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get( port, ETHER_STATS_FRAGMENTS_INDEX, &ullValue);
            *returnValue += ullValue;
            return DRV_OK;
        case INPUT_TOTAL_BYTES:
            (void)rtk_stat_port_get(port, IF_IN_OCTETS_INDEX, returnValue);
            return DRV_OK;
        case INPUT_TOTAL_BROADCASTS:
            (void)rtk_stat_port_get(port, IF_IN_BROADCAST_PKTS_INDEX, &ullValue);
            return DRV_OK;
        case INPUT_TOTAL_MULTICASTS:
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_IN_MULTICAST_PKTS_INDEX, &ullValue);
            *returnValue += ullValue;
             /*除去统计入多播包的pause帧*/
            ullValue = 0;
            (void)rtk_stat_port_get(port, DOT3_IN_PAUSE_FRAMES_INDEX, &ullValue);
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
            (void)rtk_stat_port_get(port, DOT3_IN_PAUSE_FRAMES_INDEX, &ullValue);
            if((*returnValue) < ullValue)
            {
                *returnValue = 0;
                return DRV_ERR_UNKNOW;
            }
            *returnValue -= ullValue;
             return DRV_OK;
        case INPUT_PAUSE_PACKETS:
            (void)rtk_stat_port_get(port, DOT3_IN_PAUSE_FRAMES_INDEX, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_PACKETS:
            (void)rtk_stat_port_get(port, IF_OUT_UCAST_PKTS_CNT_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_OUT_MULTICAST_PKTS_CNT_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)rtk_stat_port_get(port, IF_OUT_BROADCAST_PKTS_CNT_INDEX, &ullValue);
            *returnValue += ullValue;
            ullValue = 0;
            (void)Hal_GetMibPortStatistics(lport, OUTERRS, &ullValue);
            *returnValue += ullValue;
            return DRV_OK;
        case OUTPUT_TOTAL_BYTES:
            (void)rtk_stat_port_get(port, IF_OUT_OCTETS_INDEX, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_BROADCASTS:
            (void)rtk_stat_port_get(port, IF_OUT_BROADCAST_PKTS_CNT_INDEX, returnValue);
            return DRV_OK;
        case OUTPUT_TOTAL_MULTICASTS:
            (void)rtk_stat_port_get(port, IF_OUT_MULTICAST_PKTS_CNT_INDEX, returnValue);
            return DRV_OK;
        case OUTPUT_ERRORS:
           return Hal_GetMibPortStatistics(lport, OUTERRS, returnValue);
        case OUTPUT_PAUSE_PACKETS:
            (void)rtk_stat_port_get(port, DOT3_OUT_PAUSE_FRAMES_INDEX, returnValue);
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

    if((!VALID_PORT(lport)) && (LOGIC_CPU_PORT != lport))
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
            stPortStatType = DOT1D_TP_PORT_IN_DISCARDS_INDEX;
            break;
        case RxOctets:
            stPortStatType = IF_IN_OCTETS_INDEX;
            break;
		case RxUnicastPkts:
            stPortStatType = IF_IN_UCAST_PKTS_INDEX;
            break;
        case RxBroadcastPkts:
			stPortStatType = IF_IN_BROADCAST_PKTS_INDEX;
			break;
        case RxMulticastPkts:
			stPortStatType = IF_IN_MULTICAST_PKTS_INDEX;
			break;
        case RxSAChanges:
            return DRV_ERR_PARA;        
        case RxFragment:
            stPortStatType = ETHER_STATS_FRAGMENTS_INDEX;
            break;
        case RxJabbers:
            stPortStatType = ETHER_STATS_JABBERS_INDEX;
            break;        
        case RxAlignmentErrors:
            return DRV_ERR_PARA;
        case RxFCSErrors:
			rtk_stat_port_get(port, ETHER_STATS_FRAGMENTS_INDEX, &ullValue);		    
			*returnValue += ullValue;
			rtk_stat_port_get(port, ETHER_STATS_CRC_ALIGN_ERRORS_INDEX, &ullValue);	    
			*returnValue += ullValue;
			rtk_stat_port_get(port, ETHER_STATS_JABBERS_INDEX, &ullValue);		    
			*returnValue += ullValue;
            return DRV_OK;
        case RxGoodOctets:
            stPortStatType = IF_IN_OCTETS_INDEX;
            break;
        case RxExcessSizeDisc:
            stPortStatType = DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX;
            break;
        case RxPausePkts:
            stPortStatType = DOT3_IN_PAUSE_FRAMES_INDEX;
            break;
        case RxSymbolErrors:
            stPortStatType = DOT3_STATS_SYMBOL_ERRORS_INDEX;
            break;
        case RxQosQPkt:
            return DRV_ERR_PARA;
        case RxQosQOctet:
            return DRV_ERR_PARA;
		case RxUdersizePkts:			
            stPortStatType = ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX;			
            break;
        case RxOversizePkts:
            stPortStatType = ETHER_STATS_RX_OVERSIZE_PKTS_INDEX;
            break;
		case RxPkt64Octets:
            stPortStatType = ETHER_STATS_RX_PKTS_64OCTETS_INDEX;
            break;
        case RxPkt65to127Octets:
            stPortStatType = ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX;
            break;
        case RxPkt128to255Octets:
            stPortStatType = ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX;
            break;
        case RxPkt256to511Octets:
            stPortStatType = ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX;
            break;
        case RxPkt512to1023Octets:
            stPortStatType = ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX;
            break;
        case RxPkt1024to1518octets:
            stPortStatType = ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX;
            break;
		case RxPkt1519toMAXoctets:
            stPortStatType = ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX;
            break;
			
        //TX
        case TxDropPkts:           
            return DRV_ERR_PARA;
        case TxOctets:
            stPortStatType = IF_OUT_OCTETS_INDEX;
            break;
		case TxUnicastPkts:
            stPortStatType = IF_OUT_UCAST_PKTS_CNT_INDEX;
            break;
        case TxBroadcastPkts:
            stPortStatType = IF_OUT_BROADCAST_PKTS_CNT_INDEX;
            break;
        case TxMulticastPkts:
            stPortStatType = IF_OUT_MULTICAST_PKTS_CNT_INDEX;
            break;
        case TxCollisions:
            stPortStatType = ETHER_STATS_COLLISIONS_INDEX;
            break;        
        case TxSingleCollision:
            stPortStatType = DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX;
            break;
        case TxMultipleCollision:
            stPortStatType = DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX;
            break;
        case TxDeferredTransmit:
            stPortStatType = DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX;
            break;
        case TxLateCollision:
            stPortStatType = DOT3_STATS_LATE_COLLISIONS_INDEX;
            break;
        case TxPausePkts:
            stPortStatType = DOT3_OUT_PAUSE_FRAMES_INDEX;
            break;
        case TxFrameInDisc:
            return DRV_ERR_PARA;
        case TxQoSQPkt:
            return DRV_ERR_PARA;
        case TxQoSQOctet:
            return DRV_ERR_PARA;
		case TxUdersizePkts:			
            stPortStatType = ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX;			
            break;
        case TxOversizePkts:
            stPortStatType = ETHER_STATS_TX_OVERSIZE_PKTS_INDEX;
            break;
        case TxPkt64Octets:
            stPortStatType = ETHER_STATS_TX_PKTS_64OCTETS_INDEX;
            break;
        case TxPkt65to127Octets:
            stPortStatType = ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX;
            break;
        case TxPkt128to255Octets:
            stPortStatType = ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX;
            break;
        case TxPkt256to511Octets:
            stPortStatType = ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX;
            break;
        case TxPkt512to1023Octets:
            stPortStatType = ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX;
            break;
        case TxPkt1024to1518octets:
            stPortStatType = ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX;
            break;
		case TxPkt1519toMAXoctets:
            stPortStatType = ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX;
            break;
        case RxErrPkts:
            ullValue = 0;
            Hal_GetMibPortStatistics(lport, INERRS, &ullValue);
            *returnValue += ullValue;
            /*除去统计入错误包的pause帧*/
            ullValue = 0;
			(void)rtk_stat_port_get(port, DOT3_OUT_PAUSE_FRAMES_INDEX, &ullValue);
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



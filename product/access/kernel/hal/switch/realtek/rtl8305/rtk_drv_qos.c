/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <linux/kernel.h>
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>
//#include <rtk/port.h>
//#include <rtk/switch.h>
//#include <rtk/qos.h>
//#include <rtk/flowctrl.h>
#include "hal_common.h"
//#include <common/rt_error.h>
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include "rtk_api.h"
#include "rtk_api_ext.h"



/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static DRV_RET_E Hal_SetQosCpuInit(void);


/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
UINT32 _qosIntPriToDp[QOS_MAX_INTERNAL_PRIORITY]={QOS_COLOR_GREEEN,QOS_COLOR_GREEEN,QOS_COLOR_GREEEN,
    QOS_COLOR_GREEEN,QOS_COLOR_GREEEN,QOS_COLOR_GREEEN,QOS_COLOR_GREEEN,QOS_COLOR_GREEEN};

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
#define HAL_DRV_QOS_1P_PRI_REMAP_GROUP 0
#define HAL_DRV_QOS_DSCP_PRI_REMAP_GROUP 0
#define HAL_DRV_QOS_PRI_SELECT_GROUP 0
#define HAL_DRV_QOS_1P_PRI_REMARK_GROUP 0
#endif


/*****************************************************************************
    Func Name:  Hal_SetQosPortPrioriyMap
  Description:  map port priority to queue
        Input:  stQueueMap
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetQosInit(void)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    port_num_t lport;
    UINT32 unit, i;    
    rtk_port_t port;
    #endif
    #ifdef CHIPSET_RTL8305  
    rtk_qos_pri2queue_t pri2que;
    #endif
    
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UnitFor(unit)
    {            
        /*set qos congestion avoidance WTD*/
        /*设置为WTD 会影响端口限速准确率*/
        (void)rtk_qos_congAvoidAlgo_set(unit, CONG_AVOID_WTD);
    }
    
    LgcPortFor(lport)
    {
        unit = PortLogci2ChipId(lport);
        port = PortLogic2PhyPortId(lport);

        /*enable remark by acl and etc.*/
        //(void)rtk_qos_1pRemarkEnable_set(unit, port, ENABLED);
        //(void)rtk_qos_dscpRemarkEnable_set(unit, port, ENABLED);

        /*remap select profile group0, this group used for 1p and DSCP ingress priority remap*/
        (void)rtk_qos_port1pPriRemapGroup_set(unit, port, HAL_DRV_QOS_1P_PRI_REMAP_GROUP);
        (void)rtk_qos_portDscpPriRemapGroup_set(unit, port, HAL_DRV_QOS_DSCP_PRI_REMAP_GROUP);

        /*set all port priority select group 0, this group used for trust mode*/
        (void)rtk_qos_portPriSelGroup_set(unit, port, HAL_DRV_QOS_PRI_SELECT_GROUP);

        /*set untag 1p priroty map group select by group*/
        (void)rtk_qos_port1pRemarkGroup_set(unit, port, HAL_DRV_QOS_1P_PRI_REMARK_GROUP);
        (void)rtk_qos_port1pPriMapGroup_set(unit, port, HAL_DRV_QOS_1P_PRI_REMARK_GROUP);
        (void)rtk_qos_1pRemarkEnable_set(unit, port, DISABLED);
        
        for(i = 0; i < QOS_MAX_INTERNAL_PRIORITY; i++)
        {
            #if (QUEUE_NUM == 4)
            /*set internal priority map to four queue, 0,1->queue0, 2,3->queue1, 4,5->queue2, 6,7->queue3*/
            if(RT_ERR_OK != rtk_qos_portPriMap_set(unit, port, i, i/2))
            {
                return DRV_ERR_UNKNOW;
            }
            #elif (QUEUE_NUM == 8)
            /*set internal priority map to four queue, N->queueN*/
            if(RT_ERR_OK != rtk_qos_portPriMap_set(unit, port, i, i))
            {
                return DRV_ERR_UNKNOW;
            }
            #elif (QUEUE_NUM == 2)
            /*set internal priority map to four queue, 0,1,2,3->queue0, 4,5,6,7->queue1*/
            if(RT_ERR_OK != rtk_qos_portPriMap_set(unit, port, i, i/4))
            {
                return DRV_ERR_UNKNOW;
            }
            #endif
        }   
    }
    
    #endif
    
    #ifdef CHIPSET_RTL8305     
    if(RT_ERR_OK!=rtk_qos_init(QUEUE_NUM))
    {
        return DRV_ERR_UNKNOW;
    } 
    #if (QUEUE_NUM == 4)
    /*set internal priority map to four queue, 0,1->queue0, 2,3->queue1, 4,5->queue2, 6,7->queue3*/
    pri2que.pri2queue[0]=0;
    pri2que.pri2queue[1]=0;
    pri2que.pri2queue[2]=1;
    pri2que.pri2queue[3]=1;
    pri2que.pri2queue[4]=2;
    pri2que.pri2queue[5]=2;
    pri2que.pri2queue[6]=3;
    pri2que.pri2queue[7]=3;
    if(RT_ERR_OK != rtk_qos_priMap_set(QUEUE_NUM,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }
    #elif (QUEUE_NUM == 8)
    /*set internal priority map to four queue, N->queueN*/
    pri2que.pri2queue[0]=0;
    pri2que.pri2queue[1]=1;
    pri2que.pri2queue[2]=2;
    pri2que.pri2queue[3]=3;
    pri2que.pri2queue[4]=4;
    pri2que.pri2queue[5]=5;
    pri2que.pri2queue[6]=6;
    pri2que.pri2queue[7]=7;
    if(RT_ERR_OK != rtk_qos_priMap_set(QUEUE_NUM,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }
    #elif (QUEUE_NUM == 2)
    pri2que.pri2queue[0]=0;
    pri2que.pri2queue[1]=0;
    pri2que.pri2queue[2]=0;
    pri2que.pri2queue[3]=0;
    pri2que.pri2queue[4]=1;
    pri2que.pri2queue[5]=1;
    pri2que.pri2queue[6]=1;
    pri2que.pri2queue[7]=1;
    /*set internal priority map to four queue, 0,1,2,3->queue0, 4,5,6,7->queue1*/
    if(RT_ERR_OK != rtk_qos_priMap_set(QUEUE_NUM,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }  
	#endif
    #endif  
	#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    return Hal_SetQosCpuInit();
	#else
	return DRV_OK;
	#endif
}

/*****************************************************************************
    Func Name:  Hal_Set1pIngressPriorityRemap
  Description:  map ingress 1p priority to internal priority
        Input:  st1pToIntPri
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_Set1pIngressPriorityRemap(qos_8021p_to_intpri_t st1pToIntPri)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;
    #endif
    UINT32 pri1p, intPri;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 dp;
    #endif

   
    for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
    {
        if(st1pToIntPri.internalPri[pri1p] >= QOS_MAX_INTERNAL_PRIORITY)
        {
            return DRV_ERR_PARA;
        }
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UnitFor(unit)
    {
        for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
        {
            intPri = st1pToIntPri.internalPri[pri1p];
            dp = _qosIntPriToDp[intPri];
            /*remap 1p priority to internal priority, and set ingress dp color*/
            if(RT_ERR_OK != rtk_qos_1pPriRemapGroup_set(unit, HAL_DRV_QOS_1P_PRI_REMAP_GROUP, pri1p, intPri, dp))
            {
                return DRV_ERR_UNKNOW;
            }
            if(RT_ERR_OK != rtk_qos_1pRemarkGroup_set(unit, HAL_DRV_QOS_1P_PRI_REMARK_GROUP, intPri, _qosIntPriToDp[intPri], pri1p))
            {
                return DRV_ERR_UNKNOW;
            }
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
    {
        intPri = st1pToIntPri.internalPri[pri1p];
       // dp = _qosIntPriToDp[intPri];
        /*remap 1p priority to internal priority, and set ingress dp color*/
        if(RT_ERR_OK != rtk_qos_1pPriRemap_set(pri1p, intPri))
        {
            return DRV_ERR_UNKNOW;
        }       
    }
    #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetCos2QidRemap
  Description:  map ingress 1p priority to queue
        Input:  st1pToIntPri
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetCos2QidRemap(qos_8021p_to_intpri_t st1pPri2qid)
{
    rtk_qos_pri2queue_t stqid;
	UINT32 pri1p;

	for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
    {
        if(st1pPri2qid.internalPri[pri1p] >= QOS_MAX_1P_QUEUENO)
        {
            return DRV_ERR_PARA;
        }
		
        stqid.pri2queue[pri1p] = (rtk_uint32)st1pPri2qid.internalPri[pri1p];
    }
	
    #ifdef CHIPSET_RTL8305
    if(RT_ERR_OK != rtk_qos_priMap_set(QUEUE_NUM, &stqid))
    {
        return DRV_ERR_UNKNOW;
    }       

    #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetDscpIngressPriorityRemap
  Description:  map ingress dscp priority to internal priority
        Input:  stDscpToIntPri
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetDscpIngressPriorityRemap(qos_dscp_to_intpri_t stDscpToIntPri)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;
    #endif
    UINT32 priDscp, intPri;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 dp;
    #endif
    for(priDscp = 0; priDscp < QOS_MAX_DSCP_PRIORITY; priDscp++)
    {
        if(stDscpToIntPri.internalPri[priDscp] >= QOS_MAX_INTERNAL_PRIORITY)
        {
            return DRV_ERR_PARA;
        }
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UnitFor(unit)
    {
        for(priDscp = 0; priDscp < QOS_MAX_DSCP_PRIORITY; priDscp++)
        {
            intPri = stDscpToIntPri.internalPri[priDscp];
            dp = _qosIntPriToDp[intPri];
            /*remap DSCP priority to internal priority, and set ingress dp color*/
            if(RT_ERR_OK != rtk_qos_dscpPriRemapGroup_set(unit, HAL_DRV_QOS_DSCP_PRI_REMAP_GROUP, priDscp, intPri, dp))
            {
                return DRV_ERR_UNKNOW;
            }
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    for(priDscp = 0; priDscp < QOS_MAX_DSCP_PRIORITY; priDscp++)
    {
        intPri = stDscpToIntPri.internalPri[priDscp];
      //  dp = _qosIntPriToDp[intPri];
        /*remap DSCP priority to internal priority, and set ingress dp color*/
        if(RT_ERR_OK != rtk_qos_dscpPriRemap_set(priDscp, intPri))
        {
            return DRV_ERR_UNKNOW;
        }
    }
    #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetQosTrustMode
  Description:  set qos trust mode
        Input:  mode, QOS_COS and QOS_DSCP, QOS_PORT
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION 
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetQosTrustMode(UINT32 mode)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;    
    rtk_qos_priSelWeight_t stPriSelWeight;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_priority_select_t stPriSelWeight;
    #endif 

   
    if(QOS_COS == mode)
    {
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        /*set flow based as highest priority*/
        stPriSelWeight.weight_of_flowBased = 6;

        /*set 802.1p as higher priority*/
        stPriSelWeight.weight_of_innerTag = 5;
        stPriSelWeight.weight_of_outerTag = 4;        

        stPriSelWeight.weight_of_dot1q = 3;
        
        /*set port based as lower priority*/
        stPriSelWeight.weight_of_portBased = 2;
        
        /*set DSCP as lowest priority*/
        stPriSelWeight.weight_of_dscp = 1;              
        /*End:modify by liaohongjun 2012-1-9 */
        #endif
        #ifdef CHIPSET_RTL8305
        /*set flow based as highest priority*/
        stPriSelWeight.acl_pri = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.cvlan_pri = 6;
        stPriSelWeight.svlan_pri = 5;        
        stPriSelWeight.dot1q_pri = 4;        
        /*set port based as lower priority*/
        stPriSelWeight.port_pri = 3;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.dscp_pri = 2;              
        stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.smac_pri = 0;
        #endif
    }
    else if(QOS_DSCP == mode)
    {
         #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        /*set flow based as highest priority*/
        stPriSelWeight.weight_of_flowBased = 6;

        /*set DSCP as higher priority*/
        stPriSelWeight.weight_of_dscp = 5;

        /*set 802.1p as lower priority*/
        stPriSelWeight.weight_of_innerTag = 4;
        stPriSelWeight.weight_of_outerTag = 3;        

        /*set port based as lowest priority*/
        stPriSelWeight.weight_of_dot1q = 2;
        
        stPriSelWeight.weight_of_portBased = 1;   
        #endif
        #ifdef CHIPSET_RTL8305
          /*set flow based as highest priority*/
        stPriSelWeight.acl_pri = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.cvlan_pri = 5;
        stPriSelWeight.svlan_pri = 4;        
        stPriSelWeight.dot1q_pri = 3;        
        /*set port based as lower priority*/
        stPriSelWeight.port_pri = 2;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.dscp_pri = 6;              
        stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.smac_pri = 0;
        #endif
    }
    else if(QOS_PORT == mode)
    {
        #ifdef CHIPSET_RTL8305
          /*set flow based as highest priority*/
        stPriSelWeight.acl_pri = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.cvlan_pri = 5;
        stPriSelWeight.svlan_pri = 4;        
        stPriSelWeight.dot1q_pri = 3;        
        /*set port based as lower priority*/
        stPriSelWeight.port_pri = 6;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.dscp_pri = 2;              
        stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.smac_pri = 0;
        #endif
    }
    else if(QOS_SVLAN == mode)
    {
        #ifdef CHIPSET_RTL8305
          /*set flow based as highest priority*/
        stPriSelWeight.acl_pri = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.cvlan_pri = 5;
        stPriSelWeight.svlan_pri = 6;        
        stPriSelWeight.dot1q_pri = 4;        
        /*set port based as lower priority*/
        stPriSelWeight.port_pri = 3;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.dscp_pri = 2;              
        stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.smac_pri = 0;
        #endif
    }
    else
    {
        return DRV_ERR_PARA;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UnitFor(unit)
    {
        if(RT_ERR_OK != rtk_qos_priSelGroup_set(unit, HAL_DRV_QOS_PRI_SELECT_GROUP, &stPriSelWeight))
        {
            return DRV_ERR_UNKNOW;
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    if(RT_ERR_OK != rtk_qos_priSel_set(&stPriSelWeight))
    {
        return DRV_ERR_UNKNOW;
    }
    #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetQosQueueSchedul
  Description:  set qos schedule mode
        Input:  mode, HQ-WRR, WRR, WFQ
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetQosQueueSchedul(QueueMode_S stQueueSchedule)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_qos_scheduling_type_t queueMode;
    #endif
    rtk_qos_queue_weights_t stQueueWeight;
    port_num_t lport;
    UINT32 i,queue;
    rtk_port_t port;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;
    #endif
    
    #if (QUEUE_NUM > RTK_MAX_NUM_OF_QUEUE)
    queue = RTK_MAX_NUM_OF_QUEUE;
    #else
    queue = QUEUE_NUM;
    #endif
    
    switch(stQueueSchedule.QueueMode)
    {
        case QUEUE_MODE_HQ_WRR:
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            queueMode = WRR;
            #endif
            /*set highest internal priority mapped queue as H-Queue*/
            stQueueSchedule.Weight[queue - 1] = 0;
            break;
        case QUEUE_MODE_WFQ:
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            queueMode = WFQ;
            #endif
            break;
        case QUEUE_MODE_WRR:
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            queueMode = WRR;
            #endif
            break;
        default:
            return DRV_ERR_PARA;
    }    

    memset(&stQueueWeight, 0, sizeof(rtk_qos_queue_weights_t));
    for(i = 0; i < queue; i++)
    {
        stQueueWeight.weights[i] = stQueueSchedule.Weight[i];
    }
    
    LgcPortFor(lport)
    {
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        unit = PortLogci2ChipId(lport);
#endif
        port = PortLogic2PhyPortId(lport);
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        (void)rtk_qos_schedulingAlgorithm_set(unit, port, queueMode);
        (void)rtk_qos_schedulingQueue_set(unit, port, &stQueueWeight);
#endif
#ifdef CHIPSET_RTL8305
        (void)rtk_qos_schedulingQueue_set(port, &stQueueWeight);
#endif
    }     
    return DRV_OK;
}


/*****************************************************************************
    Func Name:  Hal_SetQosCpuInit
  Description:  set cpu port qos
        Input:  
       Output: 
       Return:  DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E Hal_SetQosCpuInit(void)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 i;
    #endif
    rtk_qos_queue_weights_t stQueueWeight;
    #ifdef CHIPSET_RTL8305  
    rtk_qos_pri2queue_t pri2que;
    #endif
    
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    /*remap select profile group0, this group used for 1p and DSCP ingress priority remap*/
    (void)rtk_qos_port1pPriRemapGroup_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, HAL_DRV_QOS_1P_PRI_REMAP_GROUP);
    (void)rtk_qos_portDscpPriRemapGroup_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, HAL_DRV_QOS_DSCP_PRI_REMAP_GROUP);

    /*set all port priority select group 0, this group used for trust mode*/
    (void)rtk_qos_portPriSelGroup_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, HAL_DRV_QOS_PRI_SELECT_GROUP);
   
    /*set cpu port 8 queue priority map, internal priority N->queueN*/
    for(i = 0; i < QOS_MAX_INTERNAL_PRIORITY; i++)
    {
        rtk_qos_portPriMap_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, i, i);
    }    
    
    /*see DRV_PACKET_PRI_XX*/
    (void)rtk_qos_schedulingAlgorithm_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, WFQ);
    stQueueWeight.weights[0] = 1;
    stQueueWeight.weights[1] = 1;
    stQueueWeight.weights[2] = 1;/*manage packet*/
    stQueueWeight.weights[3] = 1;
    stQueueWeight.weights[4] = 1;
    stQueueWeight.weights[5] = 1;
    stQueueWeight.weights[6] = 1;
    stQueueWeight.weights[7] = 1;
    (void)rtk_qos_schedulingQueue_set(PHY_CPU_CHIPID, PHY_CPU_PORTID, &stQueueWeight);
    #endif
    #ifdef CHIPSET_RTL8305
    pri2que.pri2queue[0]=0;
    pri2que.pri2queue[1]=1;
    pri2que.pri2queue[2]=2;
    pri2que.pri2queue[3]=3;
    pri2que.pri2queue[4]=4;
    pri2que.pri2queue[5]=5;
    pri2que.pri2queue[6]=6;
    pri2que.pri2queue[7]=7;  
    rtk_qos_priMap_set(PHY_CPU_CHIPID,&pri2que);
    stQueueWeight.weights[0] = 1;
    stQueueWeight.weights[1] = 1;
    stQueueWeight.weights[2] = 1;/*manage packet*/
    stQueueWeight.weights[3] = 1;
    stQueueWeight.weights[4] = 1;
    stQueueWeight.weights[5] = 1;
    stQueueWeight.weights[6] = 1;
    stQueueWeight.weights[7] = 1;
    rtk_qos_schedulingQueue_set(PHY_CPU_CHIPID,&stQueueWeight);
    #endif
    return DRV_OK;
}




#ifdef  __cplusplus
}
#endif



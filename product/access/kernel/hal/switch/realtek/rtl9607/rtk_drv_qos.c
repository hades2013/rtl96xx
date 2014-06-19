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

#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/default.h>
//#include <rtk/port.h>
//#include <rtk/switch.h>
#include <rtk/qos.h>
//#include <rtk/flowctrl.h>
#include "hal_common.h"
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <rtk/ponmac.h>


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
    rtk_qos_pri2queue_t pri2que;
    port_num_t lport = 0;
    port_num_t port = 0;    
    uint32  priority = 0;
    uint32 dscpVal = 0;
	rtk_qos_queue_weights_t stQueueWeight;
    rtk_pri_t  dot1pPriRemap[RTK_DOT1P_PRIORITY_MAX + 1] = {2,1,0,3,4,5,6,7};
        
    if(RT_ERR_OK!=rtk_qos_init())
    {
        return DRV_ERR_UNKNOW;
    } 
	/*set qos wrr mode */
	//rtk_qos_schedulingType_set(RTK_QOS_WRR);
	/*FOR CPU RATELIMIT*/
	pri2que.pri2queue[0]=0;
    pri2que.pri2queue[1]=1;
    pri2que.pri2queue[2]=2;
    pri2que.pri2queue[3]=3;
    pri2que.pri2queue[4]=4;
    pri2que.pri2queue[5]=5;
    pri2que.pri2queue[6]=6;
    pri2que.pri2queue[7]=7;  
    rtk_qos_priMap_set(1,&pri2que);
	
//	stQueueWeight.weights[0] = 1;
   // stQueueWeight.weights[1] = 2;
   // stQueueWeight.weights[2] = 3;/*manage packet*/
    //stQueueWeight.weights[3] = 4;
   // stQueueWeight.weights[4] = 5;
    //stQueueWeight.weights[5] = 6;
    //stQueueWeight.weights[6] = 7;
  //  stQueueWeight.weights[7] = 8;
    rtk_qos_portPriMap_set(PortLogic2PhyPortId(LOGIC_CPU_PORT),1);
//    rtk_qos_schedulingQueue_set(PortLogic2PhyPortId(LOGIC_CPU_PORT),&stQueueWeight);
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
#ifdef CONFIG_PRODUCT_5500
    /*add other port to group 0 */
    LgcPortFor(lport)
    {
        port = PortLogic2PhyPortId(lport);
        (void)rtk_qos_portPriMap_set(port,0);
    }  
	

	rtk_qos_portPriMap_set(LOGIC_PON_PORT, 0);/*Add by huangmingjian 2013-09-11*/


    /*pri remapping  to 3 bits inner  pri*/
    for (priority = 0; priority <= RTK_DOT1P_PRIORITY_MAX; priority++)
    { 
        if(RT_ERR_OK != rtk_qos_1pPriRemapGroup_set(0, priority,dot1pPriRemap[priority], 0))
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    for (dscpVal = 0; dscpVal <= RTK_VALUE_OF_DSCP_MAX; dscpVal++)
    {
        if(RT_ERR_OK != rtk_qos_dscpPriRemapGroup_set(0, dscpVal, dscpVal/8, 0))
        {
            return DRV_ERR_UNKNOW;
        }        
    }
    /*Modified by feihuaxin changing the group 4 to 0 2013-7-10*/
    if(RT_ERR_OK != rtk_qos_priMap_set(0,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }
#else
    if(RT_ERR_OK != rtk_qos_priMap_set(0,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }
#endif
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
    if(RT_ERR_OK != rtk_qos_priMap_set(0,&pri2que))
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
    if(RT_ERR_OK != rtk_qos_priMap_set(0,&pri2que))
    {
        return DRV_ERR_UNKNOW;
    }  
	#endif

	rtk_qos_priSelWeight_t stPriSelWeight;

	memset(&stPriSelWeight, 0, sizeof(rtk_qos_priSelWeight_t));

	/*set flow based as highest priority*/
    stPriSelWeight.weight_of_acl = 7;
    /*set 802.1p as higher priority*/
    stPriSelWeight.weight_of_vlanBased = 6;
    stPriSelWeight.weight_of_svlanBased = 5;        
    stPriSelWeight.weight_of_dot1q = 4;        
    /*set port based as lower priority*/
    stPriSelWeight.weight_of_portBased = 3;        
    /*set DSCP as lowest priority*/
    stPriSelWeight.weight_of_dscp = 2;              
    //stPriSelWeight.dmac_pri = 1; 
    stPriSelWeight.weight_of_saBaed = 0;

	if(RT_ERR_OK != rtk_qos_priSelGroup_set(0, &stPriSelWeight))
    {
        return DRV_ERR_UNKNOW;
    }
    
	return DRV_OK;
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
    UINT32 pri1p, intPri;
   
    for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
    {
        if(st1pToIntPri.internalPri[pri1p] >= QOS_MAX_INTERNAL_PRIORITY)
        {
            return DRV_ERR_PARA;
        }
    }
    
    for(pri1p = 0; pri1p < QOS_MAX_1P_PRIORITY; pri1p++)
    {
        intPri = st1pToIntPri.internalPri[pri1p];
       // dp = _qosIntPriToDp[intPri];
        /*remap 1p priority to internal priority, and set ingress dp color*/
        if(RT_ERR_OK != rtk_qos_1pPriRemapGroup_set(0, pri1p, intPri, 0))
        {
            return DRV_ERR_UNKNOW;
        }       
    }
    
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
	    
    if(RT_ERR_OK != rtk_qos_priMap_set(QUEUE_NUM, &stqid))
    {
        return DRV_ERR_UNKNOW;
    }       

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
    UINT32 priDscp, intPri;

    for(priDscp = 0; priDscp < QOS_MAX_DSCP_PRIORITY; priDscp++)
    {
        if(stDscpToIntPri.internalPri[priDscp] >= QOS_MAX_INTERNAL_PRIORITY)
        {
            return DRV_ERR_PARA;
        }
    }
    
    for(priDscp = 0; priDscp < QOS_MAX_DSCP_PRIORITY; priDscp++)
    {
        intPri = stDscpToIntPri.internalPri[priDscp];
      //  dp = _qosIntPriToDp[intPri];
        /*remap DSCP priority to internal priority, and set ingress dp color*/
        if(RT_ERR_OK != rtk_qos_dscpPriRemapGroup_set(0, priDscp, intPri, 0))
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
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
    rtk_qos_priSelWeight_t stPriSelWeight;

	memset(&stPriSelWeight, 0, sizeof(rtk_qos_priSelWeight_t));
   
    if(QOS_COS == mode)
    {
        /*set flow based as highest priority*/
        stPriSelWeight.weight_of_acl = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.weight_of_vlanBased = 4;
        stPriSelWeight.weight_of_svlanBased = 5;        
        stPriSelWeight.weight_of_dot1q = 6;        
        /*set port based as lower priority*/
        stPriSelWeight.weight_of_portBased = 3;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.weight_of_dscp = 2;              
        //stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.weight_of_saBaed = 0;
    }
    else if(QOS_DSCP == mode)
    {
        /*set flow based as highest priority*/
        stPriSelWeight.weight_of_acl = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.weight_of_vlanBased = 3;
        stPriSelWeight.weight_of_svlanBased = 4;        
        stPriSelWeight.weight_of_dot1q = 5;        
        /*set port based as lower priority*/
        stPriSelWeight.weight_of_portBased = 2;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.weight_of_dscp = 6;              
        //stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.weight_of_saBaed = 0;
    }
    else if(QOS_PORT == mode)
    {
          /*set flow based as highest priority*/
        stPriSelWeight.weight_of_acl = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.weight_of_vlanBased = 5;
        stPriSelWeight.weight_of_svlanBased = 4;        
        stPriSelWeight.weight_of_dot1q = 3;        
        /*set port based as lower priority*/
        stPriSelWeight.weight_of_portBased = 6;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.weight_of_dscp = 2;              
        //stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.weight_of_saBaed = 0;
    }
    else if(QOS_SVLAN == mode)
    {
        /*set flow based as highest priority*/
        stPriSelWeight.weight_of_acl = 7;
        /*set 802.1p as higher priority*/
        stPriSelWeight.weight_of_vlanBased = 4;
        stPriSelWeight.weight_of_svlanBased = 6;        
        stPriSelWeight.weight_of_dot1q = 5;        
        /*set port based as lower priority*/
        stPriSelWeight.weight_of_portBased = 3;        
        /*set DSCP as lowest priority*/
        stPriSelWeight.weight_of_dscp = 2;              
        //stPriSelWeight.dmac_pri = 1; 
        stPriSelWeight.weight_of_saBaed = 0;
    }
    else
    {
        return DRV_ERR_PARA;
    }
    
    if(RT_ERR_OK != rtk_qos_priSelGroup_set(0, &stPriSelWeight))
    {
        return DRV_ERR_UNKNOW;
    }
    
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
    rtk_qos_queue_weights_t stQueueWeight;
    port_num_t lport;
    UINT32 i,queue;
    rtk_port_t port;
    rtk_qos_scheduling_type_t queueMode;
	rtk_ponmac_queueCfg_t  ponQueueCfg;
    rtk_ponmac_queue_t     ponQueue;
    #if (QUEUE_NUM > RTK_MAX_NUM_OF_QUEUE)
    queue = RTK_MAX_NUM_OF_QUEUE;
    #else
    queue = QUEUE_NUM;
    #endif
   
    
    switch(stQueueSchedule.QueueMode)
    {
        case QUEUE_MODE_HQ_WRR:
            /*set highest internal priority mapped queue as H-Queue*/
            stQueueSchedule.Weight[queue - 1] = 0;
			queueMode=RTK_QOS_WRR;
            break;
        case QUEUE_MODE_WFQ:
			queueMode=RTK_QOS_WFQ;
            break;
        case QUEUE_MODE_WRR:
			queueMode=RTK_QOS_WRR;
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
        port = PortLogic2PhyPortId(lport);
		rtk_qos_schedulingType_set(queueMode);
        (void)rtk_qos_schedulingQueue_set(port, &stQueueWeight);
    }     
	// ponQueueCfg.type =  STRICT_PRIORITY or  WFQ_WRR_PRIORITY
   //  pQueueCfg->weight = weight;
     //rtk_ponmac_queue_set(pQueue, pQueueCfg);
	for(i = 0; i < queue; i++)
  	{
	 //  ponQueue.schedulerId =0;
     //  ponQueue.queueId = i;
	  // rtk_ponmac_queue_del(&queue);
  	   memset(&ponQueueCfg,0,sizeof(ponQueueCfg));
	   memset(&ponQueue,0,sizeof(ponQueue));
	   
       ponQueue.schedulerId =0;
       ponQueue.queueId = i;
	   rtk_ponmac_queue_get(&ponQueue,&ponQueueCfg);
	   ponQueueCfg.weight=stQueueSchedule.Weight[i];
	   
	   /*Begin modified by huangmingjian 2013-09-12 to let hq-wrr works suceessfully*/   
	   if(0 == ponQueueCfg.weight)
	   {
		   ponQueueCfg.type = STRICT_PRIORITY;
	   }
	   else
	   {
		   ponQueueCfg.type = WFQ_WRR_PRIORITY;
	   }
	   /*End modified by huangmingjian 2013-09-12 to let hq-wrr works suceessfully*/ 
	   
	   rtk_ponmac_queue_add(&ponQueue,&ponQueueCfg);
  	}
    return DRV_OK;
}

#if 0
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
    
    //rtk_qos_pri2queue_t pri2que;
     port_num_t port = 0;   

	port = PortLogic2PhyPortId(LOGIC_CPU_PORT);
    (void)rtk_qos_portPriMap_set(port,1);
   
    
    return DRV_OK;
}
#endif

#ifdef  __cplusplus
}
#endif



/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 40998 $
 * $Date: 2013-07-10 12:56:34 +0800 (Wed, 10 Jul 2013) $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */

/*
 * Include Files
 */
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>
#include <osal/time.h>
#include <ioal/mem32.h>

/*
 * Symbol Definition
 */



static uint32    ponmac_init = INIT_NOT_COMPLETED;
static rtk_ponmac_mode_t ponMode = PONMAC_MODE_GPON;
/*
 * Macro Declaration
 */


static int32 _ponmac_physicalQueueId_get(uint32 schedulerId, uint32 logicalQId, uint32 *pPhysicalQid)
{

#if(GPON_MAC_MODE)
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/8) + logicalQId;
#else
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/2) + logicalQId;
#endif

    return RT_ERR_OK;
}


static int32 _ponmac_schedulerQueue_get(uint32 physicalQid, uint32 *pSchedulerId, uint32 *pLogicalQId)
{
    int32   ret;
    uint32  qMask;
    uint32  maxSchedhlerId;
    uint32  schedhlerBase;
    uint32  i;

    *pLogicalQId = physicalQid%32;


#if(GPON_MAC_MODE)
    maxSchedhlerId = 8;
    schedhlerBase = (physicalQid/32) * 8;
#else
    maxSchedhlerId = 2;
    schedhlerBase = (physicalQid/32) * 2;
#endif


    for(i = schedhlerBase; i<schedhlerBase+maxSchedhlerId ; i++ )
    {
        if ((ret = apollomp_raw_ponMacScheQmap_get(i, &qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(qMask & 1<<(*pLogicalQId))
        {
            *pSchedulerId = i;
            return RT_ERR_OK;
        }
    }
    *pSchedulerId = schedhlerBase;
    return RT_ERR_OK;
}


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */


/* Function Name:
 *      dal_apollomp_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
int32
dal_apollomp_ponmac_init(void)
{
    int32   ret;
    uint32 tcont,wData;
    uint32 physicalQid;
    uint32 flowId;
    uint32 wfqTbValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

#ifdef FPGA_DEFINED
    /*fpga do not have APOLLOMP_WSDS_ANA_24*/
#else /*FPGA_DEFINED*/

#ifdef CONFIG_BEN_DIFFERENTIAL
    wData = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_BEN_SEL_CMLf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#else /*CONFIG_BEN_DIFFERENTIAL*/
    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif /*CONFIG_BEN_DIFFERENTIAL*/
#endif /*FPGA_DEFINED*/

    /*init PON BW_THRESHOLD*/
    wData = 13;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*init PON REQ_BW_THRESHOLD*/
    wData = 14;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_REQ_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_PON_CFGr, APOLLOMP_PIR_EXCEED_DROPf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*drant out all queue when pon mac init*/
    
    if(INIT_COMPLETED == ponmac_init)
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;
        uint32 schedulerId;
 
        for(schedulerId = 0 ; schedulerId < HAL_MAX_NUM_OF_GPON_TCONT() ; schedulerId++ )
        {
            memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));
            logicalQueue.schedulerId = schedulerId;
    
            for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
            {
                logicalQueue.queueId = queueId;
    
                if((ret= dal_apollomp_ponmac_queue_get(&logicalQueue, &queueCfg)) == RT_ERR_OK)
                {
                    /*delete queue*/
                    if((ret= dal_apollomp_ponmac_queue_del(&logicalQueue)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                        return ret;                    
                    }                
                }
            }
        }
    }


    for(tcont = 0; tcont <HAL_MAX_NUM_OF_GPON_TCONT(); tcont++ )
    {
        /*disable T-cont schedule */
        if ((ret = apollomp_raw_ponMacTcontEnable_set(tcont, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*clear T-cont schedule mask*/
        if ((ret = apollomp_raw_ponMacScheQmap_set(tcont, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }



    /*set all queue to strict, disable CIR/PIR and disable egress drop*/

    for(physicalQid=0 ; physicalQid<HAL_MAX_NUM_OF_PON_QUEUE() ; physicalQid++)
    {
        /*set queue schedule type*/
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set PIR CIR*/
        if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, 0x0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, HAL_PONMAC_PIR_CIR_RATE_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }


        /*set egress drop*/
        if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set default weight to 1*/
        if ((ret = apollomp_raw_ponMac_wfqWeight_set(physicalQid, 0x1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
#if defined(CONFIG_GPON_FEATURE)
    /* do not add queue 0~7 to scheduler id 0*/
#else
    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;

        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        ponmac_init = INIT_COMPLETED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_apollomp_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }
#endif
    /*mapping all flow to t-cont 0 queue 0*/
    {
        rtk_ponmac_queue_t logicalQueue;
#if defined(CONFIG_GPON_FEATURE)
        /* mapping all flow to t-cont 31 queue 31 */
        logicalQueue.schedulerId = 31;
        logicalQueue.queueId     = 31;
#else
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;
#endif
        ponmac_init = INIT_COMPLETED;
        for(flowId=0 ; flowId<HAL_CLASSIFY_SID_NUM() ; flowId++)
        {
            if((ret= dal_apollomp_ponmac_flow2Queue_set(flowId, &logicalQueue)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    /*configure WFQ specific tb value on RSVD_EGR_SCH to 0xff*/
   wfqTbValue = 0xff;
   if((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH2r, APOLLOMP_PON_WFQ_LEAKY_BUCKETf, &wfqTbValue))!=RT_ERR_OK)
   {
   	   ponmac_init = INIT_NOT_COMPLETED;
       return ret;
   }

    ponmac_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_init */


/* Function Name:
 *      dal_apollomp_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    uint32  qMask;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* add queue to t-cont schedule mask*/
    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(qMask == 0)
    {/*first queue add to this t-cont enable t-cont schedule*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    /*add queue to qMask*/
    qMask = qMask | (1<<pQueue->queueId);
    if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set Queue id and flow id mapping */
    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    /*set queue schedule type*/
    if(pQueueCfg->type == STRICT_PRIORITY)
    {
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_WFQ)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*set wfq weight*/
        if ((ret = apollomp_raw_ponMac_wfqWeight_set(physicalQid, pQueueCfg->weight)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }


    /*set PIR CIR*/
    if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, pQueueCfg->cir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, pQueueCfg->pir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, pQueueCfg->egrssDrop)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_add */



/* Function Name:
 *      dal_apollomp_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    rtk_enable_t   enable;
    uint32  qMask;
    uint32  physicalQid;
    apollomp_raw_queue_type_t  queueType;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = apollomp_raw_ponMac_queueType_get(physicalQid, &queueType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(APOLLOMP_QOS_QUEUE_STRICT == queueType)
        pQueueCfg->type = STRICT_PRIORITY;
    else
        pQueueCfg->type = WFQ_WRR_PRIORITY;

    /*set wfq weight*/
    if ((ret = apollomp_raw_ponMac_wfqWeight_get(physicalQid, &(pQueueCfg->weight))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set PIR CIR*/
    if ((ret = apollomp_raw_ponMacCirRate_get(physicalQid, &(pQueueCfg->cir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_get(physicalQid, &(pQueueCfg->pir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_get(physicalQid, &(pQueueCfg->egrssDrop))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_get */


/* Function Name:
 *      dal_apollomp_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  qMask;
    rtk_enable_t   enable;
    uint32  queueId;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);


    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;


    /*remove queue from qMask*/
    qMask = qMask & (~(1<<pQueue->queueId));
    if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*drant out queue*/
    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = apollomp_raw_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(0 == qMask)
    {/*no queue map to this t-cont disable schedule for this t-cont*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_del */



/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue        - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = apollomp_raw_ponMacSidToQueueMap_set(flow, physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_flow2Queue_set */


/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_ponMacSidToQueueMap_get(flow, &physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _ponmac_schedulerQueue_get(physicalQid, &(pQueue->schedulerId), &(pQueue->queueId))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_flow2Queue_get */


/* Function Name:
 *      dal_apollomp_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *	  state 	     - enable or disable
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
	int32 ret;
	uint32 data;
 	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
   	RT_INIT_CHK(ponmac_init);

	RT_PARAM_CHK((PONMAC_MODE_END <= mode), RT_ERR_INPUT);

    data = 0x0;
    /* PON MAC select phy 4 enable/disable */
    if ((ret = reg_field_write(APOLLOMP_CHIP_INF_SELr, APOLLOMP_PHY4_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /*WSDS_ANA_10*/
    data = 0x11B8;
    if ((ret = reg_write(APOLLOMP_WSDS_ANA_10r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    /*WSDS_ANA_1C*/
    data = 0;
    if ((ret = reg_write(APOLLOMP_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
	switch(mode)
	{
    	case PONMAC_MODE_GPON:
    	{
    	    /* set the GPON mode enable */
    		data = 0;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    		data = 1;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    	    /* set the serdes mode to GPON mode */
    	    data = 0x8;
    	    if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }

    	    /* WSDS_ANA_1A.REG_RX_KP2_2 = 6 */
    	    data = 0x6;
    	    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_KP2_2f, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    
    	    /* reset serdes including digital and analog */
    	    /* no force sds */
    	    data = 0x0;
    	    if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    
    	    /* reset serdes including digital and analog, and GPON MAC */
    	    data = 0x1;
    	    if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_CMD_SDS_RST_PSf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    osal_time_mdelay(10);
    
    	    /* force sds for not reset GPON MAC when SD down */
    	    data = 0xc;
    	    if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    data = 0x0;
    	    if ((ret = reg_write(APOLLOMP_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    
    	    /* Serdes amplitude 500 mv */
    	    data = 0x1164;
    	    if ((ret = reg_write(APOLLOMP_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    /* BEN on */
    	    data = 0x1;
    	    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_BEN_OEf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    /* pon port allow undersize packet */
    	    data = 1;
    	    if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	 	 /* turn off 'serdes_tx_dis when ben=0' */
    	 	data = 0;
    	 	if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_06r, APOLLOMP_CFG_DMY1f, &data)) != RT_ERR_OK)
    	 	{
    	 	    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	 	    return ret;
    	 	}
	    }
	    break;
	    
        case PONMAC_MODE_EPON:
        {
            rtk_port_macAbility_t portAbility;
            /*Link down pon port first*/
            if ((ret = rtk_port_macForceAbility_get(4, &portAbility)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
                return ret;
            }
                portAbility.linkStatus = PORT_LINKDOWN;
            if ((ret = rtk_port_macForceAbility_set(4, portAbility)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
                return ret;
            }

    		data = 0;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    		data = 1;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}

    	    /* set the serdes mode to EPON mode */
    	    data = 0xc;
    	    if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    
            /* change mode to EPON, must reset switch */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_SW_RSTf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);
    	    
       	    /* WSDS_ANA_1A.REG_RX_KP2_2 = 6 */
    	    data = 0x6;
    	    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_KP2_2f, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    /* Serdes amplitude 500 mv */
    	    data = 0x1164;
    	    if ((ret = reg_write(APOLLOMP_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }            

            /* reset serdes including digital and analog */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        
            /* BEN on */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_BEN_OEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
    
            /*serdes patch*/
            /*WSDS_ANA_18*/
            ioal_mem32_write(0x22060, 0xa8b2);
            /*WSDS_ANA_1A*/
            ioal_mem32_write(0x22068, 0xdde4);
            /*SDS_REG1*/
            ioal_mem32_write(0x22804, 0x0);
        
            /*PCS derdes patch*/    
            data = 0x3;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        
            /*data = 0x4e6a;*/
            data = 0x4eff;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG13r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            /*data = 0x1562;*/
            data = 0x1400;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG15r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            data = 0xbd2a;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG16r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        
            data = 0x1059;
            if ((ret = reg_write(APOLLOMP_SDS_REG7r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        
            data = 0x3F;
            if ((ret = reg_field_write(APOLLOMP_SDS_REG24r,APOLLOMP_CFG_FIB2G_TXCFG_NP_P0f,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        }
        break;
        
        case PONMAC_MODE_FIBER:
        {
    		data = 0;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    		data = 0;
    		if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    	    /* set the serdes mode to fiber auto-detect mode */
    	    data = 0x7;
    	    if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
            /*WSDS_ANA_0E.REG_TX_EN_EMPHAS =0   REG_TX_AMP=1*/
    	    data = 0x0;
    	    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Er, APOLLOMP_REG_TX_EN_EMPHASf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    data = 0x1;
    	    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Er, APOLLOMP_REG_TX_AMPf, &data)) != RT_ERR_OK)
    	    {
    	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    	        return ret;
    	    }
    	    /*force MAC4 speed 1000M*/
            /*ABLTY_FORCE_MODE = 0x10 */
            data = 0x10;
            if ((ret = reg_write(APOLLOMP_ABLTY_FORCE_MODEr, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            /*force spped to 1000M*/    	    
            data = 0x76;
            if ((ret = reg_array_write(APOLLOMP_FORCE_P_ABLTYr,4 ,REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        }
        break;
        
        default:
            break;        
    }      
	ponMode = mode;
	return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *pState       - mode state
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
 	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* check Init status */
   	RT_INIT_CHK(ponmac_init);

    *pMode=ponMode;
    
	return RT_ERR_OK;
}



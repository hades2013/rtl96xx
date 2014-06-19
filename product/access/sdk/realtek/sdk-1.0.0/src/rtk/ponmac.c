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
 * $Revision: 41959 $
 * $Date: 2013-08-09 18:25:45 +0800 (星期五, 09 八月 2013) $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/ponmac.h>
#include <dal/dal_mgmt.h>
#include <osal/lib.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

static uint32 transceiver_addrs[][2]={
    {20,16},
    {40,16},
    {96,2},
    {98,2},
    {102,2},
    {104,2}, 
};
/* Function Name:
 *      rtk_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_init(void)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_init */


/* Function Name:
 *      rtk_ponmac_queue_add
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
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_add( pQueue, pQueueCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_add */



/* Function Name:
 *      rtk_ponmac_queue_get
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
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_get( pQueue, pQueueCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_get */


/* Function Name:
 *      rtk_ponmac_queue_del
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
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_del( pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_del */



/* Function Name:
 *      rtk_ponmac_flow2Queue_set
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
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_flow2Queue_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_flow2Queue_set( flow, pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_flow2Queue_set */


/* Function Name:
 *      rtk_ponmac_flow2Queue_get
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
rtk_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_flow2Queue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_flow2Queue_get( flow, pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_flow2Queue_get */


/* Function Name:
 *      dal_apollo_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *	  state 	     - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_set( mode);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      dal_apollo_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *state 	     - point of state, enable or disable
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(pMode);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_schedulingType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_schedulingType_get(pQueueType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_schedulingType_set */

/* Function Name:
 *      rtk_pon_schedulingType_set
 * Description:
 *      Set the scheduling type.
 * Input:
 *      queueType - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_schedulingType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_schedulingType_set(queueType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_schedulingType_set */


/* Function Name:
 *      rtk_ponmac_queueDrainOut_set
 * Description:
 *      Set pon queue drain out.
 * Input:
 *      pQueue - Specified which PON queue will be drain out
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_queueDrainOut_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queueDrainOut_set(pQueue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_queueDrainOut_set */

/* Function Name:
 *      rtk_ponmac_opticalPolarity_get
 * Description:
 *      Get the current optical output polarity
 * Input:
 *      None
 * Output:
 *      pPolarity  - the current output polarity
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_opticalPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_get(pPolarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_opticalPolarity_get */

/* Function Name:
 *      rtk_ponmac_opticalPolarity_set
 * Description:
 *      Set optical output polarity
 * Input:
 *      polarity - the optical output polarity
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_opticalPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_set(polarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_opticalPolarity_set */

/* Function Name:
 *      rtk_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pState  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_losState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_losState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_losState_get */

static int32 _wapper_i2c_read(uint32 devID, uint32 idx, uint32 *dataP)
{
    int32 ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_read(TRANSCEIVER_PORT, devID, idx, dataP);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
*      rtk_ponmac_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pPara           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK       - successful
*      others          - fail
* Note:
*
*/
#include <linux/kernel.h>

int32
rtk_ponmac_transceiver_get(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pData)
{
    int32   ret;
    uint32  devID = 0, idx;
    uint32 transceiver_data[TRANSCEIVER_LEN];
    
    osal_memset(transceiver_data, 0, TRANSCEIVER_LEN);

    /* function body */
    if (NULL == RT_MAPPER->i2c_read)
    	return RT_ERR_DRIVER_NOT_FOUND;

    if  ((type == RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME) || (type == RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM))
    {
	    devID = TRANSCEIVER_A0;
    }else if ((type > RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM)  && (type < RTK_TRANSCEIVER_PARA_TYPE_MAX))
    {
	    devID = TRANSCEIVER_A1;
    }

    for (idx = transceiver_addrs[type][0]; idx < transceiver_addrs[type][0]+transceiver_addrs[type][1]; idx++)
    {
	ret = _wapper_i2c_read(devID, idx, &(transceiver_data[idx-transceiver_addrs[type][0]]));
	pData->buf[idx-transceiver_addrs[type][0]]=transceiver_data[idx-transceiver_addrs[type][0]];
    }
    return ret;
}   /* end of rtk_ponmac_transceiver_get */ 

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
 *           (1) queue configuration (PIR/CIR/Queue schuedule type)
 *           (2) flow and queue mapping
 */



/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

#include <math.h>
/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

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
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* function body */
    SETSOCKOPT(RTDRV_PONMAC_INIT, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_init */


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
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ponmac_cfg.queue, pQueue, sizeof(rtk_ponmac_queue_t));
    osal_memcpy(&ponmac_cfg.queueCfg, pQueueCfg, sizeof(rtk_ponmac_queueCfg_t));
    SETSOCKOPT(RTDRV_PONMAC_QUEUE_ADD, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_queue_add */


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
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&ponmac_cfg.queue, pQueue, sizeof(rtk_ponmac_queue_t));
    osal_memcpy(&ponmac_cfg.queueCfg, pQueueCfg, sizeof(rtk_ponmac_queueCfg_t));

    GETSOCKOPT(RTDRV_PONMAC_QUEUE_GET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);
    osal_memcpy(pQueueCfg, &ponmac_cfg.queueCfg, sizeof(rtk_ponmac_queueCfg_t));

    return RT_ERR_OK;
}   /* end of rtk_ponmac_queue_get */


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
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ponmac_cfg.queue, pQueue, sizeof(rtk_ponmac_queue_t));
    SETSOCKOPT(RTDRV_PONMAC_QUEUE_DEL, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_queue_del */



/* Function Name:
 *      rtk_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue       - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ponmac_cfg.flow, &flow, sizeof(uint32));
    osal_memcpy(&ponmac_cfg.queue, pQueue, sizeof(rtk_ponmac_queue_t));
    SETSOCKOPT(RTDRV_PONMAC_FLOW2QUEUE_SET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_flow2Queue_set */


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
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ponmac_cfg.flow, &flow, sizeof(uint32));
    GETSOCKOPT(RTDRV_PONMAC_FLOW2QUEUE_GET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);
    osal_memcpy(pQueue, &ponmac_cfg.queue, sizeof(rtk_ponmac_queue_t));

    return RT_ERR_OK;
}   /* end of rtk_ponmac_flow2Queue_get */


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
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueueType), RT_ERR_NULL_POINTER);

    /* function body */

    //osal_memcpy(&ponmac_cfg.queueType, pQueueType, sizeof(rtk_qos_scheduling_type_t));
    GETSOCKOPT(RTDRV_PONMAC_SCHEDULINGTYPE_GET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);
    osal_memcpy(pQueueType, &ponmac_cfg.queueType, sizeof(rtk_qos_scheduling_type_t));

    return RT_ERR_OK;
}   /* end of rtk_ponmac_schedulingType_get */

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
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* function body */
    osal_memcpy(&ponmac_cfg.queueType, &queueType, sizeof(rtk_qos_scheduling_type_t));
    SETSOCKOPT(RTDRV_PONMAC_SCHEDULINGTYPE_SET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ponmac_cfg.queue, pQueue, sizeof(rtk_ponmac_queue_t));
    SETSOCKOPT(RTDRV_PONMAC_QUEUEDRAINOUT_SET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_queueDrainOut_set */

static void
_get_data_by_type(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pSrcData, rtk_transceiver_data_t *pDstData)
{
    double tmp = 0;
    switch(type)    
    {
	case RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME:
	case RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM:
		pSrcData->buf[TRANSCEIVER_LEN-1]='\0';
		osal_memcpy(pDstData, pSrcData, sizeof(rtk_transceiver_data_t));
		break;
	case RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE:
		if (128 >= pSrcData->buf[0]) //MSB: pSrcData->buf[0]; LSB: pSrcData->buf[1]
		{
			tmp = (-1)*((~(pSrcData->buf[0]))+1)+((double)(pSrcData->buf[1])*1/256);
			snprintf(pDstData->buf, sizeof(rtk_transceiver_data_t), "%f C", tmp);
		}else{  
			tmp = pSrcData->buf[0]+((double)(pSrcData->buf[1])*1/256);
			snprintf(pDstData->buf, sizeof(rtk_transceiver_data_t), "%f C", tmp);
		}
		break;
	case RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE:
		tmp = (double)((pSrcData->buf[0] << 8) | pSrcData->buf[1])*1/10000;
		snprintf(pDstData->buf, sizeof(rtk_transceiver_data_t), "%f V", tmp);
		break;
	case RTK_TRANSCEIVER_PARA_TYPE_TX_POWER:
	case RTK_TRANSCEIVER_PARA_TYPE_RX_POWER:
		tmp = log10(((double)((pSrcData->buf[0] << 8) | pSrcData->buf[1])*1/10000))*10;
		snprintf(pDstData->buf, sizeof(rtk_transceiver_data_t), "%f  dBm", tmp); 
		break;
	}
	return;
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
int32
rtk_ponmac_transceiver_get(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pData)
{
    rtdrv_ponmacCfg_t ponmac_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ponmac_cfg, 0, sizeof(rtdrv_ponmacCfg_t));
    osal_memcpy(&ponmac_cfg.type, &type, sizeof(rtk_transceiver_parameter_type_t));
    GETSOCKOPT(RTDRV_PONMAC_TRANSCEIVER_GET, &ponmac_cfg, rtdrv_ponmacCfg_t, 1);
    /* implement this line */
    _get_data_by_type(type, &ponmac_cfg.data, pData);

    return RT_ERR_OK;
}   /* end of rtk_transceiver_get */    

/* Function Name:
 *      rtk_ponmac_mode_get
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
rtk_ponmac_mode_get(rtk_ponmac_mode_t *pQueueType)
{
    rtdrv_ponmacCfg_t ponmac_mode;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueueType), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_PONMAC_MODE_GET, &ponmac_mode, rtdrv_ponmacCfg_t, 1);
    osal_memcpy(pQueueType, &ponmac_mode.mode, sizeof(rtk_ponmac_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_ponmac_mode_get */


int32
rtk_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
    rtdrv_ponmacCfg_t ponmac_mode;

    /* function body */
    ponmac_mode.mode = mode;
    GETSOCKOPT(RTDRV_PONMAC_MODE_SET, &ponmac_mode, rtdrv_ponmacCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ponmac_mode_get */





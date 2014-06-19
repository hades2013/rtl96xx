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

#ifndef __RTK_PONMAC_H__
#define __RTK_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */

#define GPON_MAC_MODE 1
#define TRANSCEIVER_LEN 24                                                                                                       
#define TRANSCEIVER_PORT 0
#define TRANSCEIVER_A0 0x50
#define TRANSCEIVER_A1 0x51

typedef struct rtk_ponmac_queueCfg_s
{
    uint32 cir;
    uint32 pir;
    rtk_qos_queue_type_t type;
    uint32 weight;
    rtk_enable_t egrssDrop;

} rtk_ponmac_queueCfg_t;


typedef struct rtk_ponmac_queue_s
{
    uint32 schedulerId;
    uint32 queueId;/*0~31*/
} rtk_ponmac_queue_t;


typedef struct rtk_transceiver_data_s{
    uint8 buf[TRANSCEIVER_LEN];
}rtk_transceiver_data_t;

typedef enum rtk_transceiver_patameter_type_e{
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME = 0,
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM,
    RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE,
    RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE,
    RTK_TRANSCEIVER_PARA_TYPE_TX_POWER,
    RTK_TRANSCEIVER_PARA_TYPE_RX_POWER,
    RTK_TRANSCEIVER_PARA_TYPE_MAX
}rtk_transceiver_parameter_type_t;

typedef enum rtk_ponmac_mode_s
{
	PONMAC_MODE_GPON=0,
	PONMAC_MODE_EPON,
	PONMAC_MODE_FIBER,
	PONMAC_MODE_END
}rtk_ponmac_mode_t;

typedef enum rtk_ponmac_polarity_s
{
	PONMAC_POLARITY_LOW=0,
	PONMAC_POLARITY_HIGH,
	PONMAC_POLARITY_END
}rtk_ponmac_polarity_t;

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
extern int32
rtk_ponmac_init(void);


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
extern int32
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


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
extern int32
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


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
extern int32
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



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
extern int32
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


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
extern int32
rtk_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      rtk_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *      state 	     - disable or enable
 * Output:
 *
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_mode_set(rtk_ponmac_mode_t mode);


/* Function Name:
 *      rtk_ponmac_mode_get
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
rtk_ponmac_mode_get(rtk_ponmac_mode_t *pMode);


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
extern int32
rtk_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType);

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
extern int32
rtk_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType);


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
extern int32
rtk_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue);

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
extern int32
rtk_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity);

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
extern int32
rtk_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity);

/* Function Name:
 *      rtk_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_losState_get(rtk_enable_t *pEnable);

/* Function Name:
*      rtk_ponmac_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pData           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK       - successful
*      others          - fail
* Note:
*
*/
extern int32
rtk_ponmac_transceiver_get(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pData);
#endif /* __RTK_PONMAC_H__ */


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
 * $Revision: 40777 $
 * $Date: 2013-07-05 10:19:42 +0800 (Fri, 05 Jul 2013) $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) queue configuration (PIR/CIR/Queue schuedule type)  
 *           (2) flow and queue mapping
 */
 
#ifndef __DAL_RTL9601B_PONMAC_H__
#define __DAL_RTL9601B_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */

typedef enum rtl9601b_ponmac_draintOutState_e
{
	RTL9601B_PONMAC_DRAINOUT_FIN = 0,
	RTL9601B_PONMAC_DRAINOUT_DOING,
	RTL9601B_PONMAC_DRAINOUT_END
}rtl9601b_ponmac_draintOutState_t;

/*
 * Macro Declaration
 */

/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* Function Name:
 *      dal_rtl9601b_ponmac_init
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
dal_rtl9601b_ponmac_init(void);


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_add
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
extern int32 
dal_rtl9601b_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);



/* Function Name:
 *      dal_rtl9601b_ponmac_queue_get
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
extern int32 
dal_rtl9601b_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_del
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
extern int32 
dal_rtl9601b_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_set
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
extern int32 
dal_rtl9601b_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_get
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
dal_rtl9601b_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_mode_set
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
extern int32 
dal_rtl9601b_ponmac_mode_set(rtk_ponmac_mode_t mode);

/* Function Name:
 *      dal_rtl9601b_ponmac_mode_get
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
extern int32 
dal_rtl9601b_ponmac_mode_get(rtk_ponmac_mode_t *pMode);


/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
dal_rtl9601b_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType);

/* Function Name:
 *      dal_rtl9601b_pon_schedulingType_set
 * Description:
 *      Set the scheduling type.
 * Input:
 *      queueType - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9601b_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType);


/* Function Name:
 *      dal_rtl9601b_ponmac_queueDrainOut_set
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
dal_rtl9601b_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_get
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
dal_rtl9601b_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity);


/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_set
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
dal_rtl9601b_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity);


/* Function Name:
 *      dal_rtl9601b_ponmac_losState_get
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
dal_rtl9601b_ponmac_losState_get(rtk_enable_t *pState);


#endif /* __DAL_RTL9601B_PONMAC_H__ */


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
 * $Revision: 41137 $
 * $Date: 2013-07-12 16:16:47 +0800 (Fri, 12 Jul 2013) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/time.h> 
#include <dal/dal_mgmt.h> 

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : TIME */
 
/* Function Name:
 *      rtk_time_transparentPort_set
 * Description:
 *      Set transparent ports to the specified device.
 * Input:
 *      port   - ports
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_portTransparentEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    if (NULL == RT_MAPPER->time_portTransparentEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portTransparentEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portTransparentEnable_set */

/* Function Name:
 *      rtk_time_transparentPort_get
 * Description:
 *      Get transparent ports to the specified device.
 * Input:
 *      port - ports
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer enable point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_portTransparentEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_portTransparentEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portTransparentEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portTransparentEnable_get */

/* Function Name:
 *      rtk_time_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize Time module before calling any Time APIs.
 */
int32
rtk_time_init(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_init)
        return RT_ERR_OK;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_init */

/* Function Name:
 *      rtk_time_portPtpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_portPtpEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    if (NULL == RT_MAPPER->time_portPtpEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portPtpEnable_get */

/* Function Name:
 *      rtk_time_portPtpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      None
 */
int32
rtk_time_portPtpEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_portPtpEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portPtpEnable_set */

/* Function Name:
 *      rtk_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_curTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_curTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_get( pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_curTime_get */

/* Function Name:
 *      rtk_time_curTime_latch
 * Description:
 *      Latch the current time.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_curTime_latch(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_curTime_latch)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_latch();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_curTime_latch */


/* Function Name:
 *      rtk_time_refTime_get
 * Description:
 *      Get the reference time.
 * Input:
 *      None
 * Output:
 *      pSign      - pointer buffer of sign
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp/pSign point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_refTime_get(uint32 *pSign, rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_refTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_refTime_get( pSign, pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_refTime_get */

/* Function Name:
 *      rtk_time_refTime_set
 * Description:
 *      Set the reference time.
 * Input:
 *      sign      - significant
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
rtk_time_refTime_set(uint32 sign, rtk_time_timeStamp_t timeStamp)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_refTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_refTime_set( sign, timeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_refTime_set */

/* Function Name:
 *      rtk_time_frequency_set
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_frequency_set(uint32 freq)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_frequency_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_set( freq);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_frequency_set */

/* Function Name:
 *      rtk_time_frequency_get
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_frequency_get(uint32 *freq)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_frequency_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_get( freq);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_frequency_get */

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_set
 * Description:
 *      Set ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      igr_action    - ingress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_ptpIgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t igr_action)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpIgrMsgAction_set( type, igr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpIgrMsgAction_set */

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_get
 * Description:
 *      Get ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      igr_action    - ingress action.
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_NULL_POINTER  - Pointer igr_action point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ptpIgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t *igr_action)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpIgrMsgAction_get( type, igr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpIgrMsgAction_get */

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_set
 * Description:
 *      Set egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      egr_action    - egress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_ptpEgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t egr_action)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpEgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpEgrMsgAction_set( type, egr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpEgrMsgAction_set */

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_get
 * Description:
 *      Get egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      egr_action    - egress action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Pointer egr_action point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ptpEgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t *egr_action)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpEgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpEgrMsgAction_get( type, egr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpEgrMsgAction_get */

/* Function Name:
 *      rtk_time_meanPathDelay_set
 * Description:
 *      Set rtk_time_meanPathDelay_set of PTP system time.
 * Input:
 *      delay - mean path delay value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_meanPathDelay_set(uint32 delay)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_meanPathDelay_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_meanPathDelay_set( delay);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_meanPathDelay_set */

/* Function Name:
 *      rtk_time_meanPathDelay_get
 * Description:
 *      Get rtk_time_meanPathDelay_get of PTP system time.
 * Input:
 *      delay    - mean path delay.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - - Pointer delay point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_meanPathDelay_get(uint32 *delay)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_meanPathDelay_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_meanPathDelay_get( delay);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_meanPathDelay_get */

/* Function Name:
 *      rtk_time_rxTime_set
 * Description:
 *      Set the RX time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
int32
rtk_time_rxTime_set(rtk_time_timeStamp_t timeStamp)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_rxTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_rxTime_set( timeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_rxTime_set */

/* Function Name:
 *      rtk_time_rxTime_get
 * Description:
 *      Get the RX time.
 * Input:
 *      None.
 * Output:
 *      pTimeStamp - pointer buffer of the RX time.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
int32
rtk_time_rxTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_rxTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_rxTime_get( pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_rxTime_get */


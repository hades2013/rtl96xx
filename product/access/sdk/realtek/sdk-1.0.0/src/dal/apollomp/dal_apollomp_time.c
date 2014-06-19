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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
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
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/time.h>
#include <dal/apollomp/dal_apollomp_time.h>


/*
 * Data Declaration
 */
 static uint32    time_init = {INIT_NOT_COMPLETED};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : TIME */

/* Function Name:
 *      dal_apollomp_time_transparentPort_set
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
dal_apollomp_time_portTransparentEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_PTP_TRANSPARENT_CFGr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_TRANSPARENT_PORTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_portTransparentEnable_set */

/* Function Name:
 *      dal_apollomp_time_transparentPort_get
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
dal_apollomp_time_portTransparentEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_PTP_TRANSPARENT_CFGr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_TRANSPARENT_PORTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;

} /* end of dal_apollomp_time_portTransparentEnable_get */

/* Function Name:
 *      dal_apollomp_time_init
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
dal_apollomp_time_init(void)
{
    int32   ret;
    uint32  port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    time_init = INIT_COMPLETED;

    /* disable PTP */
    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_apollomp_time_portPtpEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
            time_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_init */

/* Function Name:
 *      dal_apollomp_time_portPtpEnable_get
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
dal_apollomp_time_portPtpEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_PTP_P_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PTP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_portPtpEnable_get */

/* Function Name:
 *      dal_apollomp_time_portPtpEnable_set
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
dal_apollomp_time_portPtpEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_PTP_P_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PTP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_portPtpEnable_set */

/* Function Name:
 *      dal_apollomp_time_curTime_get
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
dal_apollomp_time_curTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    uint32  tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_SECr, APOLLOMP_SEC_31_0f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_SECr, APOLLOMP_SEC_47_32f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->sec = tmp[1];
    pTimeStamp->sec = pTimeStamp->sec << 32;
    pTimeStamp->sec |= tmp[0];

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_NSECr, APOLLOMP_NSEC_UNITf, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->nsec = tmp[0] << 3;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_curTime_get */

/* Function Name:
 *      dal_apollomp_time_curTime_latch
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
dal_apollomp_time_curTime_latch(void)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    val = TRUE;
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_CTRLr, APOLLOMP_PTP_TIME_LATCHf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_curTime_latch */


/* Function Name:
 *      dal_apollomp_time_refTime_get
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
dal_apollomp_time_refTime_get(uint32 *pSign, rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    uint32  tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSign), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_OFFSET_SECr, APOLLOMP_SEC_31_0f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_OFFSET_SECr, APOLLOMP_SEC_47_32f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->sec = tmp[1] & 0x0000ffff;
    pTimeStamp->sec = (uint64)pTimeStamp->sec << 32;
    pTimeStamp->sec |= tmp[0];

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_OFFSET_8NSECr, APOLLOMP_NSEC_UNITf, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->nsec = tmp[0];

    if(pTimeStamp->sec & 0x800000000000LL)
    {
        *pSign = PTP_REF_SIGN_NEGATIVE;
        if(pTimeStamp->nsec == 0)
            pTimeStamp->sec = (~(pTimeStamp->sec -1))& 0x0000ffffffffffffLL;
        else
        {
            pTimeStamp->sec = (~(pTimeStamp->sec -1) -1)  & 0x0000ffffffffffffLL;
            pTimeStamp->nsec = (1000000000/8 - pTimeStamp->nsec) << 3;
        }
    }
    else
    {
        *pSign = PTP_REF_SIGN_POSTIVE;
        pTimeStamp->nsec = tmp[0] << 3;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_refTime_get */

/* Function Name:
 *      dal_apollomp_time_refTime_set
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
dal_apollomp_time_refTime_set(uint32 sign, rtk_time_timeStamp_t timeStamp)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((PTP_REF_SIGN_END <= sign), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_TIME_REFTIME_SEC_MAX < timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((1000000000 <= timeStamp.nsec), RT_ERR_INPUT);

    if(sign == PTP_REF_SIGN_POSTIVE)
    {
        val = (uint32)(timeStamp.sec & 0xFFFFFFFF);
    }
    else
    {
        if(timeStamp.nsec != 0)
            val = (uint32)((~(timeStamp.sec + 1)+1) & 0xFFFFFFFFUL);
        else
            val = (uint32)((~timeStamp.sec +1) & 0xFFFFFFFFUL);
    }
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_OFFSET_SECr, APOLLOMP_SEC_31_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    if(sign == PTP_REF_SIGN_POSTIVE)
    {
        val = (uint32)((timeStamp.sec >> 32) & 0xFFFF);
    }
    else
    {
        if(timeStamp.nsec != 0)
            val = (uint32)(((~(timeStamp.sec + 1)+1) >> 32) & 0xFFFF);
        else
            val = (uint32)(((~timeStamp.sec +1) >> 32) & 0xFFFF);
    }
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_OFFSET_SECr, APOLLOMP_SEC_47_32f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    if(sign == PTP_REF_SIGN_POSTIVE)
    {
        val = (uint32)((timeStamp.nsec >> 3) & APOLLOMP_TIME_REFTIME_NSEC_MAX) ;
    }
    else
    {
        if(timeStamp.nsec != 0)
            val = (uint32)(((1000000000 - timeStamp.nsec) >> 3) & APOLLOMP_TIME_REFTIME_NSEC_MAX);
        else
            val = 0;
    }
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_OFFSET_8NSECr, APOLLOMP_NSEC_UNITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    val = TRUE;
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_CTRLr, APOLLOMP_CMDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_refTime_set */

/* Function Name:
 *      dal_apollomp_time_frequency_set
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
dal_apollomp_time_frequency_set(uint32 freq)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_TIME_FREQUENCY_MAX < freq), RT_ERR_INPUT);

    val = freq;
    if ((ret = reg_field_write(APOLLOMP_PTP_TIME_FREQr, APOLLOMP_FREQf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_frequency_set */

/* Function Name:
 *      dal_apollomp_time_frequency_get
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
dal_apollomp_time_frequency_get(uint32 *freq)
{
    int32   ret;
    uint32  val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == freq), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PTP_TIME_FREQr, APOLLOMP_FREQf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    *freq = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_frequency_get */

/* Function Name:
 *      dal_apollomp_time_ptpIgrMsgAction_set
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
dal_apollomp_time_ptpIgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t igr_action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((PTP_MSG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((PTP_IGR_ACTION_END <= igr_action), RT_ERR_INPUT);

    val = (uint32)igr_action;
    if ((ret = reg_array_field_write(APOLLOMP_PTP_IGR_MSG_ACTr, REG_ARRAY_INDEX_NONE, type, APOLLOMP_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_ptpIgrMsgAction_set */

/* Function Name:
 *      dal_apollomp_time_ptpIgrMsgAction_get
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
dal_apollomp_time_ptpIgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t *igr_action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((PTP_MSG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == igr_action), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_PTP_IGR_MSG_ACTr, REG_ARRAY_INDEX_NONE, type, APOLLOMP_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    *igr_action = (rtk_time_ptpIgrMsg_action_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_ptpIgrMsgAction_get */

/* Function Name:
 *      dal_apollomp_time_ptpEgrMsgAction_set
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
dal_apollomp_time_ptpEgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t egr_action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((PTP_MSG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((PTP_EGR_ACTION_END <= egr_action), RT_ERR_INPUT);

    val = (uint32)egr_action;
    if ((ret = reg_array_field_write(APOLLOMP_PTP_EGR_MSG_ACTr, REG_ARRAY_INDEX_NONE, type, APOLLOMP_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_ptpEgrMsgAction_set */

/* Function Name:
 *      dal_apollomp_time_ptpEgrMsgAction_get
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
dal_apollomp_time_ptpEgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t *egr_action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((PTP_MSG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == egr_action), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_PTP_EGR_MSG_ACTr, REG_ARRAY_INDEX_NONE, type, APOLLOMP_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }
    *egr_action = (rtk_time_ptpEgrMsg_action_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_ptpEgrMsgAction_get */

/* Function Name:
 *      dal_apollomp_time_meanPathDelay_set
 * Description:
 *      Set dal_apollomp_time_meanPathDelay_set of PTP system time.
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
dal_apollomp_time_meanPathDelay_set(uint32 delay)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_TIME_MEANPATHDELAY_MAX < delay), RT_ERR_INPUT);

    val = delay;
    if ((ret = reg_field_write(APOLLOMP_PTP_MEANPATH_DELAYr, APOLLOMP_DELAYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_meanPathDelay_set */

/* Function Name:
 *      dal_apollomp_time_meanPathDelay_get
 * Description:
 *      Get dal_apollomp_time_meanPathDelay_get of PTP system time.
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
dal_apollomp_time_meanPathDelay_get(uint32 *delay)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == delay), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PTP_MEANPATH_DELAYr, APOLLOMP_DELAYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    *delay = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_meanPathDelay_get */

/* Function Name:
 *      dal_apollomp_time_rxTime_set
 * Description:
 *      Configure user RX timestamp.
 * Input:
 *      timeStamp - RX timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *
 */
int32
dal_apollomp_time_rxTime_set(rtk_time_timeStamp_t timeStamp)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_TIME_RXTIME_SEC_MAX <= timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_TIME_RXTIME_NSEC_MAX <= timeStamp.nsec), RT_ERR_INPUT);

    val = (uint32)(timeStamp.sec & 0x7);
    if ((ret = reg_field_write(APOLLOMP_PTP_RX_TIMEr, APOLLOMP_SEC_2_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    val = timeStamp.nsec >> 3;
    if ((ret = reg_field_write(APOLLOMP_PTP_RX_TIMEr, APOLLOMP_NSEC_UNITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_time_rxTime_set */

/* Function Name:
 *      dal_apollomp_time_rxTime_get
 * Description:
 *      Get RX timestamp.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the RX time
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *
 */
int32
dal_apollomp_time_rxTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PTP_RX_TIMEr, APOLLOMP_SEC_2_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->sec = val;

    if ((ret = reg_field_read(APOLLOMP_PTP_RX_TIMEr, APOLLOMP_NSEC_UNITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PTP), "");
        return ret;
    }
    pTimeStamp->nsec = val << 3;

    return RT_ERR_OK;
} /* end of dal_apollomp_time_rxTime_get */


 /*
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
 * Purpose : Define diag shell commands for PTP.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) PTP commands.
 */

/*
 * Include Files
 */

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <rtk/time.h>

/*
 * time init
 */
cparser_result_t
cparser_cmd_time_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_init(), ret);

    return CPARSER_OK;
    
}    /* end of cparser_cmd_time_init */

/*
 * time get ptp ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_time_get_ptp_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)    
    {        
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpEnable_get(port, &state), ret);        
        diag_util_mprintf("\n Port %d PTP state: %s", port, diagStr_enable[state]);    
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ptp_ports_all_state */

/*
 * time set ptp ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpEnable_set(port, state), ret);
    }

    return CPARSER_OK;

}    /* end of cparser_cmd_time_set_ptp_ports_all_state_enable_disable */

/*
 * time get cur-time
 */
cparser_result_t
cparser_cmd_time_get_cur_time(
    cparser_context_t *context)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_curTime_get(&timeStamp), ret);
    
    diag_util_mprintf("Current Time: %lld sec   %u nanosec\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_cur_time */

/*
 * time latch cur-time
 */
cparser_result_t
cparser_cmd_time_latch_cur_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_curTime_latch(), ret);

    return CPARSER_OK;
    
}    /* end of cparser_cmd_time_latch_cur_time */

/*
 * time get ref-time
 */
cparser_result_t
cparser_cmd_time_get_ref_time(
    cparser_context_t *context)
{
    int32 ret;
    uint32 sign;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_refTime_get(&sign, &timeStamp), ret);
    
    diag_util_mprintf("Reference Time: %s  %lld sec   %u nanosec\n", diagStr_timeSign[sign], timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ref_time */

/*
 * time set ref-time ( increase | decrease ) <UINT64:second> <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_set_ref_time_increase_decrease_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;
    uint32 sign;
    rtk_time_timeStamp_t timeStamp;
    
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
        sign = PTP_REF_SIGN_POSTIVE;
    else if('d' == TOKEN_CHAR(3, 0))
        sign = PTP_REF_SIGN_NEGATIVE;
    else
        return CPARSER_NOT_OK;

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rtk_time_refTime_set(sign, timeStamp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ref_time_increase_decrease_second_nanosecond */

/*
 * time get frequency
 */
cparser_result_t
cparser_cmd_time_get_frequency(
    cparser_context_t *context)
{
    int32 ret;
    uint32 freq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_frequency_get(&freq), ret);
    
    diag_util_mprintf("System Frequency: %u\n", freq);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_frequency */

/*
 * time set frequency <UINT:frequency>
 */
cparser_result_t
cparser_cmd_time_set_frequency_frequency(
    cparser_context_t *context,
    uint32_t  *frequency_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_frequency_set(*frequency_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_frequency_frequency */

/*
 * time get ptp ( <PORT_LIST:ports> | all ) transparent state
 */
cparser_result_t
cparser_cmd_time_get_ptp_ports_all_transparent_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)    
    {        
        DIAG_UTIL_ERR_CHK(rtk_time_portTransparentEnable_get(port, &state), ret);        
        diag_util_mprintf("\n Port %d PTP transparent state: %s", port, diagStr_enable[state]);    
    }
    
    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ptp_ports_all_transparent_state */

/*
 * time set ptp ( <PORT_LIST:ports> | all ) transparent state ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_ports_all_transparent_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portTransparentEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ptp_ports_all_transparent_state_enable_disable */

/*
 * time set ingress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action ( nop | trap-to-cpu | forward-to-trans | forward-to-trans-and-mirror-to-cpu )
 */
cparser_result_t
cparser_cmd_time_set_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_trap_to_cpu_forward_to_trans_forward_to_trans_and_mirror_to_cpu(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpIgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
        
    if ('n' == TOKEN_CHAR(6, 0))
    {
        action = PTP_IGR_ACTION_NONE;
    }
    else if('t' == TOKEN_CHAR(6, 0))
    {
        action = PTP_IGR_ACTION_TRAP2CPU;
    }
    else if('f' == TOKEN_CHAR(6, 0))
    {
        if ('-' == TOKEN_CHAR(6, 16))
           action = PTP_IGR_ACTION_FORWARD2TRANS_AND_MIRROR2CPU;
        else
           action = PTP_IGR_ACTION_FORWARD2TRANS;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpIgrMsgAction_set(type, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_trap_to_cpu_forward_to_trans_forward_to_trans_and_mirror_to_cpu */

/*
 * time get ingress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action 
 */
cparser_result_t
cparser_cmd_time_get_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpIgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpIgrMsgAction_get(type, &action), ret);
    diag_util_mprintf("Ingress type %s Action: %s\n", diagStr_timeType[type], diagStr_timeIgrAct[action]);    

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action */

/*
 * time set egress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action ( nop | latch-time | latch-time-and-mirror-to-cpu | modify-correction )
 */
cparser_result_t
cparser_cmd_time_set_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_latch_time_latch_time_and_mirror_to_cpu_modify_correction(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpEgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
        
    if ('n' == TOKEN_CHAR(6, 0))
    {
        action = PTP_EGR_ACTION_NONE;
    }
    else if('m' == TOKEN_CHAR(6, 0))
    {
        action = PTP_EGR_ACTION_MODIFY_CORRECTION;
    }
    else if('l' == TOKEN_CHAR(6, 0))
    {
        if ('-' == TOKEN_CHAR(6, 10))
           action = PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU;
        else
           action = PTP_EGR_ACTION_LATCH_TIME;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpEgrMsgAction_set(type, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_latch_time_latch_time_and_mirror_to_cpu_modify_correction */

/*
 * time get egress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action
 */
cparser_result_t
cparser_cmd_time_get_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpEgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpEgrMsgAction_get(type, &action), ret);
    diag_util_mprintf("Egress type %s Action: %s\n", diagStr_timeType[type], diagStr_timeEgrAct[action]);    

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action */

/*
 * time get mean-path-delay
 */
cparser_result_t
cparser_cmd_time_get_mean_path_delay(
    cparser_context_t *context)
{
    int32 ret;
    uint32  delay;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_meanPathDelay_get(&delay), ret);
    
    diag_util_mprintf("Mean path delay: %u sec\n", delay);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_mean_path_delay */

/*
 * time set mean-path-delay <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_set_mean_path_delay_nanosecond(
    cparser_context_t *context,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_meanPathDelay_set(*nanosecond_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_mean_path_delay_nanosecond */

/*
 * time get rxtime
 */
cparser_result_t
cparser_cmd_time_get_rxtime(
    cparser_context_t *context)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_rxTime_get(&timeStamp), ret);
    
    diag_util_mprintf("Rx Time: %lld sec   %u nanosec\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_rxtime */

/*
 * time set rxtime <UINT64:second> <UINT:nanosecond> */
cparser_result_t
cparser_cmd_time_set_rxtime_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;
    
    DIAG_UTIL_PARAM_CHK();

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rtk_time_rxTime_set(timeStamp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_rxtime_second_nanosecond */


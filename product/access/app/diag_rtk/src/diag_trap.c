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
 * Purpose : Definition those Trap command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
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

#include <dal/apollo/raw/apollo_raw_trap.h>

/*
 * trap init
 */
cparser_result_t
cparser_cmd_trap_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_init */


/*
 * trap set ( cdp | csstp ) action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_action_t action;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if ('d' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_TRAP2CPU;
    }
	else if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		action = ACTION_FORWARD;
	}
	else if(!osal_strcmp(TOKEN_STR(4),"forward-exclude-cpu"))
	{
		action = ACTION_FORWARD_EXCLUDE_CPU;
	}
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(&rmaMac, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu */

/*
 * trap get ( cdp | csstp ) action
 */
cparser_result_t
cparser_cmd_trap_get_cdp_csstp_action(
    cparser_context_t *context)
{
    rtk_action_t action;
    uint32 ret = CPARSER_NOT_OK;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_get(&rmaMac, &action), ret);

    if(rmaMac.octet[5] == 0xcc)
    {
        diag_util_mprintf("CDP ");
    }
    else if(rmaMac.octet[5] == 0xcd)
    {
        diag_util_mprintf("CSSTP ");
    }

    switch(action)
    {
        case ACTION_DROP:
            diag_util_mprintf("%s\n",DIAG_STR_DROP);
            break;
        case ACTION_TRAP2CPU:
            diag_util_mprintf("%s\n",DIAG_STR_TRAP2CPU);
            break;
        case ACTION_FORWARD:
            diag_util_mprintf("%s\n",DIAG_STR_FORWARD);
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_cdp_csstp_action */




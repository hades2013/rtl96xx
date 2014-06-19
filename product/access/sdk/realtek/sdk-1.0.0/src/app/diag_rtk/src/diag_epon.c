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
 * $Revision: 41061 $
 * $Date: 2013-07-11 12:28:51 +0800 (Thu, 11 Jul 2013) $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
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
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <hal/common/halctrl.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <rtk/epon.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_epon.h>
#endif


#ifdef CONFIG_SDK_RTL9601B
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#endif


#if defined (FPGA_DEFINED)
void _epon_ind_phyReg_write(uint8 phyId, uint8 reg, uint16 data)
{
    #define PATCH_W_OFFSET 0xC000

    io_mii_phy_reg_write(8, 1, data);

    data = (PATCH_W_OFFSET | ((phyId & 0x1F) << 5) |(reg & 0x1F));
    io_mii_phy_reg_write(8, 0, data);
}
#endif

/*
 * epon init
 */
cparser_result_t
cparser_cmd_epon_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

#if defined (FPGA_DEFINED)
    uint32 value;

    /* Temporary code for epon init */
    /* PHY */
    _epon_ind_phyReg_write(0, 31, 7);
    _epon_ind_phyReg_write(1, 31, 7);
    _epon_ind_phyReg_write(2, 31, 7);
    _epon_ind_phyReg_write(3, 31, 7);
    _epon_ind_phyReg_write(4, 31, 7);
    _epon_ind_phyReg_write(5, 31, 7);
    _epon_ind_phyReg_write(6, 31, 7);
    _epon_ind_phyReg_write(7, 31, 7);

    _epon_ind_phyReg_write(0, 30, 160);
    _epon_ind_phyReg_write(1, 30, 160);
    _epon_ind_phyReg_write(2, 30, 160);
    _epon_ind_phyReg_write(3, 30, 160);
    _epon_ind_phyReg_write(4, 30, 160);
    _epon_ind_phyReg_write(5, 30, 160);
    _epon_ind_phyReg_write(6, 30, 160);
    _epon_ind_phyReg_write(7, 30, 160);

    _epon_ind_phyReg_write(0, 28, 0x28);
    _epon_ind_phyReg_write(1, 28, 0x28);
    _epon_ind_phyReg_write(2, 28, 0x28);
    _epon_ind_phyReg_write(3, 28, 0x28);
    _epon_ind_phyReg_write(4, 28, 0x28);
    _epon_ind_phyReg_write(5, 28, 0x28);
    _epon_ind_phyReg_write(6, 28, 0x28);
    _epon_ind_phyReg_write(7, 28, 0x28);

    _epon_ind_phyReg_write(0, 0, 0x1940);
    _epon_ind_phyReg_write(1, 0, 0x1940);
    _epon_ind_phyReg_write(2, 0, 0x1940);
    _epon_ind_phyReg_write(3, 0, 0x1940);
    _epon_ind_phyReg_write(4, 0, 0x1940);
    _epon_ind_phyReg_write(5, 0, 0x1940);
    _epon_ind_phyReg_write(6, 0, 0x1940);
    _epon_ind_phyReg_write(7, 0, 0x1940);

    _epon_ind_phyReg_write(0, 31, 7);
    _epon_ind_phyReg_write(1, 31, 7);
    _epon_ind_phyReg_write(2, 31, 7);
    _epon_ind_phyReg_write(3, 31, 7);
    _epon_ind_phyReg_write(4, 31, 7);
    _epon_ind_phyReg_write(5, 31, 7);
    _epon_ind_phyReg_write(6, 31, 7);
    _epon_ind_phyReg_write(7, 31, 7);

    _epon_ind_phyReg_write(0, 30, 32);
    _epon_ind_phyReg_write(1, 30, 32);
    _epon_ind_phyReg_write(2, 30, 32);
    _epon_ind_phyReg_write(3, 30, 32);
    _epon_ind_phyReg_write(4, 30, 32);
    _epon_ind_phyReg_write(5, 30, 32);
    _epon_ind_phyReg_write(6, 30, 32);
    _epon_ind_phyReg_write(7, 30, 32);

    _epon_ind_phyReg_write(0, 27, 0x2f0a);
    _epon_ind_phyReg_write(1, 27, 0x2f0a);
    _epon_ind_phyReg_write(2, 27, 0x2f0a);
    _epon_ind_phyReg_write(3, 27, 0x2f0a);
    _epon_ind_phyReg_write(4, 27, 0x2f0a);
    _epon_ind_phyReg_write(5, 27, 0x2f0a);
    _epon_ind_phyReg_write(6, 27, 0x2f0a);
    _epon_ind_phyReg_write(7, 27, 0x2f0a);

    _epon_ind_phyReg_write(0, 31, 0);
    _epon_ind_phyReg_write(1, 31, 0);
    _epon_ind_phyReg_write(2, 31, 0);
    _epon_ind_phyReg_write(3, 31, 0);
    _epon_ind_phyReg_write(4, 31, 0);
    _epon_ind_phyReg_write(5, 31, 0);
    _epon_ind_phyReg_write(6, 31, 0);
    _epon_ind_phyReg_write(7, 31, 0);

    _epon_ind_phyReg_write(0, 4, 0x05E1);
    _epon_ind_phyReg_write(1, 4, 0x05E1);
    _epon_ind_phyReg_write(2, 4, 0x05E1);
    _epon_ind_phyReg_write(3, 4, 0x05E1);
    _epon_ind_phyReg_write(4, 4, 0x05E1);
    _epon_ind_phyReg_write(5, 4, 0x05E1);
    _epon_ind_phyReg_write(6, 4, 0x05E1);
    _epon_ind_phyReg_write(7, 4, 0x05E1);

    _epon_ind_phyReg_write(0, 0, 0x1340);
    _epon_ind_phyReg_write(1, 0, 0x1340);
    _epon_ind_phyReg_write(2, 0, 0x1340);
    _epon_ind_phyReg_write(3, 0, 0x1340);
    _epon_ind_phyReg_write(4, 0, 0x1340);
    _epon_ind_phyReg_write(5, 0, 0x1340);
    _epon_ind_phyReg_write(6, 0, 0x1340);
    _epon_ind_phyReg_write(7, 0, 0x1340);

    _epon_ind_phyReg_write(0x11, 0x00, 0x000a);
    _epon_ind_phyReg_write(0x11, 0x01, 0x0100);
    
    _epon_ind_phyReg_write(0x1d, 0x1a, 0x0000);
    _epon_ind_phyReg_write(0x1d, 0x02, 0x2d16);
    _epon_ind_phyReg_write(0x1d, 0x16, 0xa8b2);
    _epon_ind_phyReg_write(0x1d, 0x03, 0x6041);
    _epon_ind_phyReg_write(0x1d, 0x18, 0xdde4);

    _epon_ind_phyReg_write(0x1d, 0x06, 0xf4f0);
    _epon_ind_phyReg_write(0x1d, 0x05, 0x4003);
    _epon_ind_phyReg_write(0x1d, 0x0f, 0x4fe6);

    _epon_ind_phyReg_write(0x10, 0x01, 0x000c);

    /*FPON FEC patch*/
    _epon_ind_phyReg_write(27, 13, 0x4e6a);
    _epon_ind_phyReg_write(27, 15, 0x1562);
    _epon_ind_phyReg_write(27, 16, 0xbd2a);
    _epon_ind_phyReg_write(27, 12, 0x3);
    _epon_ind_phyReg_write(26, 7 , 0x1059);
#endif


#if defined (FPGA_DEFINED)
    /*reset EPON module*/
    io_mii_phy_reg_write(0x8, 0x7, 0xC00);
#endif

    DIAG_UTIL_ERR_CHK(rtk_epon_init(), ret); 

#if defined (FPGA_DEFINED)
    io_mii_phy_reg_write(0x8, 0x7, 0xE00);
#endif


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_init */
/*
 * epon get bypass-fec state
 */
cparser_result_t
cparser_cmd_epon_get_bypass_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_get(&enable), ret); 
            diag_util_printf("state:%s",diagStr_enable[enable]);
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif             
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_bypass_fec_state */

/*
 * epon set bypass-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_bypass_fec_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_set(enable), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif             
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_bypass_fec_state_disable_enable */

/*
 * epon get llid-table <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_get_llid_table_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    llidEntry.llidIdx = *index_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d\n",
                       *index_ptr,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout);
            

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table_index */

/*
 * epon get llid-table
 */
cparser_result_t
cparser_cmd_epon_get_llid_table(
    cparser_context_t *context)
{
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    for(index = 0; index<HAL_MAX_LLID_ENTRY(); index++)
    {
        llidEntry.llidIdx = index;
        DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
        diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d\n",
                            index,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout);
    }                

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table */

/*
 * epon set llid-table <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

     llidEntry.llidIdx = *index_ptr;
        
     DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
     llidEntry.valid = enable;
     DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_state_disable_enable */

/*
 * epon set llid-table <UINT:index> llid <UINT:llid>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_llid_llid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *llid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
     
    DIAG_UTIL_PARAM_CHK();

    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.llid = *llid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_llid_llid */

/*
 * epon set llid-table <UINT:index> report-timer <UINT:timer>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_report_timer_timer(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *timer_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();


    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.reportTimer = *timer_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_report_timer_timer */

/*
 * epon get mpcp-gate action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_gate_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_GATE_ASIC_HANDLE:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_HANDLEf,&val), ret); 
            
            switch(val)
            {
                case 0:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case 1:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",val);
                    break;                
            }
            break;
        }
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_gate_action */

/*
 * epon set mpcp-gate action ( asic-only | trap-and-asic ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('a'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE;
            else
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_set(act), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 val;

            if('a'==TOKEN_CHAR(4,0))
                val = 0;
            else
                val = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_HANDLEf,&val), ret); 
            break;
        }
#endif    
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic */

/*
 * epon get mpcp-invalid-len action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_invalid_len_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_mpcpHandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_MPCP_DROP:
                    diag_util_mprintf("Drop\n");
                    break;    
                case APOLLOMP_EPON_MPCP_PASS:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 tmp_val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_MPCP_CTRr,RTL9601B_INVALID_LEN_HANDLEf,&tmp_val), ret); 
            
            switch(tmp_val)
            {
                case 0:
                    diag_util_mprintf("Drop\n");
                    break;    
                case 1:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",tmp_val);
                    break;                
            }
            break;
        }
#endif   
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_invalid_len_action */

/*
 * epon set mpcp-invalid-len action ( drop | pass ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('d'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_MPCP_DROP;
            else
                act = APOLLOMP_EPON_MPCP_PASS;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_set(act), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 writeVal;
            if('d'==TOKEN_CHAR(4,0))
                writeVal = 0;
            else
                writeVal = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_INVALID_LEN_HANDLEf,&writeVal), ret); 
            break;
        }
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass */

/*
 * epon get register mode
 */
cparser_result_t
cparser_cmd_epon_get_register_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_get(&mode), ret); 
            
            switch(mode)
            {
                case APOLLOMP_EPON_SW_REG:
                    diag_util_mprintf("SW register\n");
                    break;    
                case APOLLOMP_EPON_HW_REG:
                    diag_util_mprintf("HW register\n");
                    break;    
                default:
                    diag_util_mprintf("unknown mode:%d\n",mode);
                    break;                
            }
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("HW register\n");
            break;
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mode */

/*
 * epon set register mode ( asic | sw ) 
 */
cparser_result_t
cparser_cmd_epon_set_register_mode_asic_sw(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;

            if('a'==TOKEN_CHAR(4,0))
                mode = APOLLOMP_EPON_HW_REG;
            else
                mode = APOLLOMP_EPON_SW_REG;
                
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_set(mode), ret); 
            
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("HW register\n");
            break;
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mode_asic_sw */

/*
 * epon get register llid-idx
 */
cparser_result_t
cparser_cmd_epon_get_register_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    DIAG_UTIL_PARAM_CHK();
      
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    diag_util_mprintf("register llid table index:%d\n",regReq.llidIdx);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_llid_idx */

/*
 * epon set register llid-idx <UINT:index> 
 */
cparser_result_t
cparser_cmd_epon_set_register_llid_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    DIAG_UTIL_PARAM_CHK();
      

    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    regReq.llidIdx = *index_ptr; 
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_llid_idx_index */

/*
 * epon get register state
 */
cparser_result_t
cparser_cmd_epon_get_register_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 


    diag_util_printf("state:%s",diagStr_enable[regReq.doRequest]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_state */

/*
 * epon set register state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_register_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;


    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    regReq.doRequest = enable; 
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_state_disable_enable */

/*
 * epon get register mac-address
 */
cparser_result_t
cparser_cmd_epon_get_register_mac_address(
    cparser_context_t *context)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_get(&regMac), ret); 
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&regMac.octet[0]));
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_regReq_t regReq;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&regReq.mac.octet[0]));
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mac_address */

/*
 * epon set register mac-address <MACADDR:mac> 
 */
cparser_result_t
cparser_cmd_epon_set_register_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    osal_memcpy(&regMac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_set(&regMac), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_regReq_t regReq;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret);
            memcpy(&regReq.mac.octet, &regMac.octet, ETHER_ADDR_LEN);
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mac_address_mac */

/*
 * epon get register pendding-grant
 */
cparser_result_t
cparser_cmd_epon_get_register_pendding_grant(
    cparser_context_t *context)
{
    uint32   grantNum;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_get(&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_pendding_grant */

/*
 * epon set register pendding-grant <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_register_pendding_grant_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_set(*number_ptr), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,number_ptr), ret); 
            break;
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_pendding_grant_index */




/*
 * epon set force-laser-on state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_force_laser_on_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_force_laser_on_state_disable_enable */

/*
 * epon get force-laser-on state
 */
cparser_result_t
cparser_cmd_epon_get_force_laser_on_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("state:%s\n",diagStr_enable[enable]);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_force_laser_on_state */

/*
 * epon get local-time
 */
cparser_result_t
cparser_cmd_epon_get_local_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 localTime;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_localTime_get(&localTime), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_LOCAL_TIMEr,RTL9601B_LOCAL_TIMEf,&localTime), ret); 
            
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("Local time:0x%x\n",localTime);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_local_time */

/*
 * epon get grant-shift
 */
cparser_result_t
cparser_cmd_epon_get_grant_shift(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_grantShift_get(&shiftTime), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_GRANT_SHIFT_STARTr,RTL9601B_GRANT_SHIFTf,&shiftTime), ret); 
                  
            break;
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("shift TQ:0x%x\n",shiftTime);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_grant_shift */

/*
 * epon set grant-shift <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_grant_shift_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
       
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_grantShift_set(*number_ptr), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GRANT_SHIFT_STARTr,RTL9601B_GRANT_SHIFTf,number_ptr), ret); 
                  
            break;
#endif   
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_grant_shift_number */

/*
 * epon get discovery-shift
 */
cparser_result_t
cparser_cmd_epon_get_discovery_shift(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_GRANT_SHIFTr,APOLLOMP_DIS_SHIFT_TIMEf,&shiftTime), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("shift TQ:0x%x\n",shiftTime);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_discovery_shift */

/*
 * epon set discovery-shift <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_discovery_shift_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime;
       
    DIAG_UTIL_PARAM_CHK();
    
    shiftTime = *number_ptr;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_GRANT_SHIFTr,APOLLOMP_DIS_SHIFT_TIMEf,&shiftTime), ret); 
            
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_discovery_shift_number */

/*
 * epon get discovery-random-delay state 
 */
cparser_result_t
cparser_cmd_epon_get_discovery_random_delay_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_DEBUG1r,APOLLOMP_DIS_RANDOM_DELAY_ENf,&enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("state:%s\n",diagStr_enable[enable]);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_discovery_random_delay_state */

/*
 * epon set discovery-random-delay state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_discovery_random_delay_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_DEBUG1r,APOLLOMP_DIS_RANDOM_DELAY_ENf,&enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_discovery_random_delay_state_disable_enable */

/*
 * epon get mpcp-gate-trap type
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_gate_trap_type(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 type;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&type), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    
    if(type == 0)
        diag_util_printf("trap all gate\n");
    else if(type == 1)
        diag_util_printf("trap discovery gate\n");
    else if(type == 2)
        diag_util_printf("trap normal gate\n");
    else
        diag_util_printf("unknown type\n");
        
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_gate_trap_type */

/*
 * epon set mpcp-gate-trap type ( all | discovery | normal )
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_gate_trap_type_all_discovery_normal(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 type;
    DIAG_UTIL_PARAM_CHK();

    if('a'==TOKEN_CHAR(4,0))
        type = 0;
    else if('d'==TOKEN_CHAR(4,0))
        type = 1;
    else if('n'==TOKEN_CHAR(4,0))
        type = 2;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&type), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_gate_trap_type_all_discovery_normal */

/*
 * epon get reg-ack flag
 */
cparser_result_t
cparser_cmd_epon_get_reg_ack_flag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_FLAGf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("ACK flag:0x%x\n",value); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_reg_ack_flag */

/*
 * epon set reg-ack flag <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_flag_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_FLAGf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_reg_ack_flag_value */

/*
 * epon get reg-ack llid-idx
 */
cparser_result_t
cparser_cmd_epon_get_reg_ack_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_REG_ACKr,APOLLOMP_LLID_IDXf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_REG_ACKr,RTL9601B_LLID_IDXf,&value), ret); 
            break;
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("LLID idx:0x%x\n",value); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_reg_ack_llid_idx */

/*
 * epon set reg-ack llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();


    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_LLID_IDXf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_REG_ACKr,RTL9601B_LLID_IDXf,&value), ret); 
            break;
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_reg_ack_llid_idx_value */

/*
 * epon set reg-ack trigger
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_trigger(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = 1;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_TRIGGERf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_reg_ack_trigger */



/*
 * epon get laser-on time
 */
cparser_result_t
cparser_cmd_epon_get_laser_on_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 

    diag_util_printf("laser on time(TQ):0X%x\n",laserOn); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_on_time */

/*
 * epon set laser-on time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_on_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 
    laserOn = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_set(laserOn, laserOff), ret); 
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_on_time_value */

/*
 * epon get laser-off time 
 */
cparser_result_t
cparser_cmd_epon_get_laser_off_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 

    diag_util_printf("laser off time(TQ):0X%x\n",laserOff); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_off_time */

/*
 * epon set laser-off time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_off_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 
    laserOff = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_set(laserOn, laserOff), ret); 
 
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_off_time_value */

/*
 * epon get laser-on shift-time
 */
cparser_result_t
cparser_cmd_epon_get_laser_on_shift_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_on_shift_time */

/*
 * epon set laser-on shift-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_on_shift_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_on_shift_time_value */

/*
 * epon get laser-off shift-time
 */
cparser_result_t
cparser_cmd_epon_get_laser_off_shift_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_off_shift_time */

/*
 * epon set laser-off shift-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_off_shift_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_off_shift_time_value */

/*
 * epon get sync-time
 */
cparser_result_t
cparser_cmd_epon_get_sync_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value1;
    uint32 value2;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SYNC_TIMEr,APOLLOMP_NORMAL_SYNC_TIMEf,&value1), ret); 
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SYNC_TIMEr,APOLLOMP_DISC_SYNC_TIMEf,&value2), ret); 

            break;
#endif   

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint8 sysncTime;
            DIAG_UTIL_ERR_CHK(rtk_epon_syncTime_get(&sysncTime), ret); 
            value2 = sysncTime;
            value1 = sysncTime;
            break;
        }
#endif   
         
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    diag_util_printf("normal sync time (TQ):%d\n",value1); 
    diag_util_printf("discovery sync time (TQ):%d\n",value2); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_sync_time */

/*
 * epon set sync-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_sync_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SYNC_TIMEr,APOLLOMP_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_sync_time_value */

/*
 * epon active llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_active_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
    llidIdx = *value_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
       case APOLLOMP_CHIP_ID:
       {
            apollomp_raw_epon_llid_table_t   llidEntry;
            /*set llid to disable*/
            
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(llidIdx ,&llidEntry), ret); 
            
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(llidIdx,&llidEntry), ret); 
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regLlidIdx_set(llidIdx), ret); 
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regReguest_set(ENABLED), ret); 
            break;
      }  
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_llid_entry_t llidEntry;
            rtk_epon_regReq_t  regReq;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
            regReq.doRequest = ENABLED;
            regReq.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 

            break;
        } 
#endif              
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_active_llid_idx_value */

/*
 * epon de-active llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_de_active_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
    llidIdx = *value_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            /*set llid to disable*/
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(llidIdx ,&llidEntry), ret); 
            
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(llidIdx,&llidEntry), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_llid_entry_t llidEntry;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 
            break;
        }
#endif  
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_de_active_llid_idx_value */

/*
 * epon set report-mode ( normal | report-0-f | force-0 | force-f )
 */
cparser_result_t
cparser_cmd_epon_set_report_mode_normal_report_0_f_force_0_force_f(
    cparser_context_t *context)
{
    rtk_epon_report_mode_t mode;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();


    if('n'==TOKEN_CHAR(3,0))
        mode = RTK_EPON_REPORT_NORMAL;
    else if('r'==TOKEN_CHAR(3,0))
        mode = RTK_EPON_REPORT_0_F;
    else
    {
        if('0'==TOKEN_CHAR(3,6))        
            mode = RTK_EPON_REPORT_FORCE_0;
        else
            mode = RTK_EPON_REPORT_FORCE_F;
    }
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_apollomp_epon_reportMode_set(mode), ret); 
            break;

#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9601b_epon_reportMode_set(mode), ret); 
            break;

#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_report_mode_normal_report_0_f_force_0_force_f */

/*
 * epon get report-mode
 */
cparser_result_t
cparser_cmd_epon_get_report_mode(
    cparser_context_t *context)
{
    rtk_epon_report_mode_t mode;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_apollomp_epon_reportMode_get(&mode), ret); 
            break;

#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9601b_epon_reportMode_get(&mode), ret); 
            break;

#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    diag_util_mprintf("report mode:");
    switch(mode)
    {
        case RTK_EPON_REPORT_NORMAL:
            diag_util_mprintf("normal\n");
            break;    
        case RTK_EPON_REPORT_0_F:
            diag_util_mprintf("0x0 or 0xFFFF\n");
            break;    
        case RTK_EPON_REPORT_FORCE_0:
            diag_util_mprintf("force 0x0\n");
            break;    
        case RTK_EPON_REPORT_FORCE_F:
            diag_util_mprintf("force 0xFFFF\n");
            break;    
        default:
            diag_util_mprintf("unknown\n");
            break;            
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_report_mode */

/*
 * epon dump mib-counter
 */
cparser_result_t
cparser_cmd_epon_dump_mib_counter(
    cparser_context_t *context)
{
    rtk_epon_counter_t counter;
    int32 ret = RT_ERR_FAILED;
    int i;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    counter.llidIdx = 0;
    DIAG_UTIL_ERR_CHK(rtk_epon_mibCounter_get(&counter), ret); 


    for(i=0;i<8;i++)
        diag_util_mprintf("queue %d             : %8d\n",i,counter.llidIdxCnt.queueTxFrames[i]);

    diag_util_mprintf("mpcpTxReport        : %8d\n",counter.llidIdxCnt.mpcpTxReport);
    diag_util_mprintf("mpcpRxGate          : %8d\n",counter.llidIdxCnt.mpcpRxGate);
    diag_util_mprintf("onuLlidNotBcst      : %8d\n",counter.llidIdxCnt.onuLlidNotBcst);

    diag_util_mprintf("mpcpRxDiscGate      : %8d\n",counter.mpcpRxDiscGate);
    diag_util_mprintf("mpcpTxRegRequest    : %8d\n",counter.mpcpTxRegRequest);
    diag_util_mprintf("crc8Err             : %8d\n",counter.crc8Err);
    diag_util_mprintf("notBcstBitNotOnuLlid: %8d\n",counter.notBcstBitNotOnuLlid);
    diag_util_mprintf("bcstBitPlusOnuLLid  : %8d\n",counter.bcstBitPlusOnuLLid);
    diag_util_mprintf("bcstNotOnuLLid      : %8d\n",counter.bcstNotOnuLLid);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_dump_mib_counter */

/*
 * epon reset mib-counter
 */
cparser_result_t
cparser_cmd_epon_reset_mib_counter(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_ERR_CHK(rtk_epon_mibLlidIdx_reset(0), ret); 
    DIAG_UTIL_ERR_CHK(rtk_epon_mibGlobal_reset(), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_reset_mib_counter */


/*
 * epon set us-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_us_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_usFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_us_fec_state_disable_enable */

/*
 * epon get us-fec state
 */
cparser_result_t
cparser_cmd_epon_get_us_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_usFecState_get(&enable), ret); 
        
    diag_util_mprintf("US FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_us_fec_state */

/*
 * epon set ds-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_ds_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_dsFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_ds_fec_state_disable_enable */

/*
 * epon get ds-fec state
 */
cparser_result_t
cparser_cmd_epon_get_ds_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_dsFecState_get(&enable), ret); 
        
    diag_util_mprintf("DS FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_ds_fec_state */

/*
 * epon set fec-global state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_fec_global_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_fecState_set(enable), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_fec_global_state_disable_enable */

/*
 * epon get fec-global state
 */
cparser_result_t
cparser_cmd_epon_get_fec_global_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(rtk_epon_fecState_get(&enable), ret);
    
    diag_util_mprintf("Global FEC state:%s\n",diagStr_enable[enable]);

    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_fec_global_state */

/*
 * epon set laser-polarity mode ( low | high )
 */
cparser_result_t
cparser_cmd_epon_set_laser_polarity_mode_low_high(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_polarity_t polarity;

    DIAG_UTIL_PARAM_CHK();

    if('l'==TOKEN_CHAR(4,0))
        polarity = EPON_POLARITY_LOW;
    else
        polarity = EPON_POLARITY_HIGH;

    DIAG_UTIL_ERR_CHK(rtk_epon_opticalPolarity_set(polarity), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_polarity_mode_low_high */

/*
 * epon get laser-polarity mode
 */
cparser_result_t
cparser_cmd_epon_get_laser_polarity_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_polarity_t polarity;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
 
    DIAG_UTIL_ERR_CHK(rtk_epon_opticalPolarity_get(&polarity), ret);
    
    if(EPON_POLARITY_HIGH==polarity) 
        diag_util_mprintf("Optical polarity: High\n");
    else
        diag_util_mprintf("Optical polarity: Low\n");
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_polarity_mode */

/*
 * epon set laser-state mode ( normal | force-on | force-off )
 */
cparser_result_t
cparser_cmd_epon_set_laser_state_mode_normal_force_on_force_off(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_laser_status_t state;

    DIAG_UTIL_PARAM_CHK();

    if('n'==TOKEN_CHAR(4,0))
        state = RTK_EPON_LASER_STATUS_NORMAL;
    else
    {
        if('f'==TOKEN_CHAR(4,7))
            state = RTK_EPON_LASER_STATUS_FORCE_OFF;
        else
            state = RTK_EPON_LASER_STATUS_FORCE_ON;
    }
    DIAG_UTIL_ERR_CHK(rtk_epon_forceLaserState_set(state), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_state_mode_normal_force_on_force_off */

/*
 * epon get laser-state mode
 */
cparser_result_t
cparser_cmd_epon_get_laser_state_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_laser_status_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
 
    DIAG_UTIL_ERR_CHK(rtk_epon_forceLaserState_get(&state), ret);
    
    diag_util_mprintf("Laser status:");
    if(RTK_EPON_LASER_STATUS_NORMAL==state) 
        diag_util_mprintf("normal\n");
    else if(RTK_EPON_LASER_STATUS_FORCE_ON==state)
        diag_util_mprintf("force-on\n");
    else
        diag_util_mprintf("force-off\n");
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_state_mode */

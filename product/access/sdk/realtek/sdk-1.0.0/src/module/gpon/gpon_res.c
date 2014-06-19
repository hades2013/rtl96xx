/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: 41336 $
 * $Date: 2013-07-19 20:13:16 +0800 (Fri, 19 Jul 2013) $
 *
 * Purpose : GPON MAC Driver Resource Management
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_res.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_ploam.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_fsm.h>
#include <ioal/mem32.h>
#include <rtk/gponv2.h>


#define FPGA_PATCH 1

#define GPON_TEST_VAL 0x12345678


static void gpon_res_set_bits(uint8 *buf, uint8 start, uint8 stop, uint8 bit)
{
    uint8 by,bi,cur,ptn=bit&0x01;
    for(cur=start;cur<=stop;cur++)
    {
        by = cur/8;
        bi = 7-(cur%8);
        buf[by] &= ~(1<<bi);
        buf[by] |= ptn<<bi;
    }
}

int32 gpon_dev_versoin_get(gpon_dev_obj_t* obj, rtk_gpon_device_ver_t* ver)
{
    uint32 version,ret;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    GPON_UTIL_ERR_CHK(rtk_gpon_macVersion_get(&version),ret);

    ver->gmac_ver = version;

    return RT_ERR_OK;
}

int32 gpon_dev_initialize(gpon_dev_obj_t* obj, uint32 baseaddr)
{
    uint32  rd;
    uint32  i;
    uint32  cnt32;
    uint64  cnt64;
    int32 ret;

    /* memory initialization */
    obj->base_addr = baseaddr;
    obj->status = RTK_GPONMAC_FSM_STATE_O1;
    obj->timer = 0;
    obj->ber_timer = 0;
    obj->onuid = GPON_DEV_DEFAULT_ONU_ID;

    osal_memcpy(&obj->serial_number.vendor[0], "RTKG", RTK_GPON_VENDOR_ID_LEN);
    obj->serial_number.specific[0] = 0x11;
    obj->serial_number.specific[1] = 0x11;
    obj->serial_number.specific[2] = 0x11;
    obj->serial_number.specific[3] = 0x11;
    osal_memset(&obj->password.password[0], 'a' ,RTK_GPON_PASSWORD_LEN);

    obj->parameter.onu.to1_timer = 10000;  /* 10s */
    obj->parameter.onu.to2_timer = 100;     /* 100ms */
    obj->parameter.laser.laser_optic_los_en = TRUE;
    obj->parameter.laser.laser_optic_los_polar = TRUE;
    obj->parameter.laser.laser_cdr_los_en = FALSE;
    obj->parameter.laser.laser_cdr_los_polar = FALSE;
    obj->parameter.laser.laser_los_filter_en = FALSE;
    obj->parameter.laser.laser_us_on = 0x18;
    obj->parameter.laser.laser_us_off = 0x30;
    obj->parameter.dsphy.ds_scramble_en = TRUE;
    obj->parameter.dsphy.ds_fec_bypass = FALSE;
    obj->parameter.dsphy.ds_fec_thrd = 0;
    obj->parameter.dsploam.ds_ploam_onuid_filter = TRUE;
    obj->parameter.dsploam.ds_ploam_broadcast_accpt = TRUE;
    obj->parameter.dsploam.ds_ploam_drop_crc_err = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_crc_chk = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_onuid_filter = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_plend_mode = 0;
    obj->parameter.dsgem.assemble_timer = 16;
    obj->parameter.dseth.ds_eth_crc_chk = TRUE;
    obj->parameter.dseth.ds_eth_pti_mask = 0x01;
    obj->parameter.dseth.ds_eth_pti_ptn = 0x01;
    obj->parameter.dsomci.ds_omci_pti_mask = 0x01;
    obj->parameter.dsomci.ds_omci_pti_ptn = 0x01;
    obj->parameter.usphy.us_scramble_en = TRUE;
    obj->parameter.usphy.us_burst_en_polar = TRUE;
    obj->parameter.usphy.small_sstart_en = TRUE;
    obj->parameter.usphy.suppress_laser_en = FALSE;
    obj->parameter.usploam.us_ploam_en = TRUE;
    obj->parameter.usploam.us_ind_normal_ploam = TRUE;
    obj->parameter.usdbr.us_dbru_en = TRUE;
    obj->power_level = 0x01;
    osal_memset(&obj->aes_key,0xAA,sizeof(rtk_gpon_aes_key_t));
    obj->chunk_header.head_size = 0x04;
    obj->chunk_header.task_ind = 0x00;
    obj->chunk_header.err_code = 0x02;
    obj->chunk_header.src = 0x05;
    obj->shaper.tick = 0;
    obj->shaper.inc_size = 0x80;
    obj->shaper.thrd_high = 0;
    obj->shaper.thrd_low = 0;
    obj->mactbl.op_mode = RTK_GPON_MACTBL_MODE_EXCLUDE;
    obj->mactbl.bc_pass = TRUE;
    obj->mactbl.non_mc_pass = TRUE;
    /* Add ipv4 force mode and ipv6 force mode in v1.2 */
    obj->mactbl.ipv4_force_mode = RTK_GPON_MCFORCE_MODE_NORMAL;
    obj->mactbl.ipv6_force_mode = RTK_GPON_MCFORCE_MODE_NORMAL;
    /* mc check pattern add IPv4 and IPv6 in GPON_MAC_SWIO_r1.1 */
    obj->mactbl.ipv4_mc_check_ptn = 0x01005E;
    obj->mactbl.ipv6_mc_check_ptn = 0x3333;
    obj->req_en = FALSE;
    obj->rdi = FALSE;
    obj->tdm_en = FALSE;
    obj->ds_fec = FALSE;
    obj->us_fec = FALSE;
    obj->us_ploam_req_urg = FALSE;
    obj->us_ploam_req_nrm = FALSE;
    obj->us_omci_flow = GPON_DEV_MAX_FLOW_NUM;
    obj->aes_key_switch_time = 0xFFFFFFFF;
    obj->key_index = 0;

    osal_memset(&obj->burst_overhead,0,sizeof(gpon_burst_oh_t));

    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
        obj->p_tcont[i] = NULL;
    }
    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        obj->p_dsflow[i] = NULL;
        obj->p_usflow[i] = NULL;
    }
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        obj->p_mac_tbl[i] = NULL;
    }
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        obj->alarm_tbl[i] = TRUE;
    }

    obj->state_change_callback = NULL;
    obj->dsfec_change_callback = NULL;
    obj->usfec_change_callback = NULL;
    obj->usploam_urg_epty_callback = NULL;
    obj->usploam_nrm_epty_callback = NULL;
    obj->ploam_callback = NULL;
	obj->extMsgGet_callback = NULL;	
	obj->extMsgSet_callback = NULL;
    #if 0
    obj->omci_callback = gpon_omci_cb;
    #endif
    obj->omci_callback = NULL;
    obj->aeskey_callback = NULL;
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        obj->fault_callback[i] = NULL;
    }

    obj->cnt_boh_tx = 0;
    obj->cnt_ploamu_tx = 0;
    obj->cnt_dbru_tx = 0;
    obj->cnt_gem_tx = 0;

    obj->cnt_cpu_ploam_rx = 0;
    obj->cnt_cpu_omci_rx = 0;
    obj->cnt_cpu_ploam_tx = 0;
    obj->cnt_cpu_ploam_tx_nor = 0;
    obj->cnt_cpu_ploam_tx_urg = 0;

    /* debug flag */
    obj->auto_tcont_state = 1;
    obj->auto_boh_state = 1;
    obj->eqd_offset = 0;

    /*BER timer */
    obj->ber_interval = 1000;

    /* register test */
#if 1 /*!defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init ok, scott */
     GPON_UTIL_ERR_CHK(rtk_gpon_test_get(&rd),ret);

    if(rd!=GPON_TEST_VAL)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"register read test fail (0x%04x) [baseaddr:%p]",rd,obj->base_addr);
        return RT_ERR_GPON_INITIAL_FAIL;
    }
#endif

#if 1 /* for init ok, scott */
     GPON_UTIL_ERR_CHK(rtk_gpon_test_set(0xEDCB),ret);

     GPON_UTIL_ERR_CHK(rtk_gpon_test_get(&rd),ret);


    if(rd!=0xEDCB)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"register write test fail (0x%04x) [baseaddr:%p]",rd,obj->base_addr);
        return RT_ERR_GPON_INITIAL_FAIL;
    }
#endif

    GPON_UTIL_ERR_CHK(rtk_gpon_test_set(GPON_TEST_VAL),ret);

    /* register initialization */
    /* interrupt disable */
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);


    /* clean configurations */
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
       GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(i),ret);
    }
    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_del(i),ret);
    }
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_del(i),ret);
    }

    /* clear counters */
    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortPktCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortByteCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortEthRxCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortEthFwdCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsEthCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsGemCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsDataByteCnt_get(i, &cnt64),ret);
    }
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsIdleByteCnt_get(i, &cnt64),ret);
    }
    for(i=GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR;i<GPON_REG_DSGTCPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGtcMiscCnt_get(i, &cnt32),ret);
    }
    for(i=GPON_REG_DSGEMPMMISC_MULTICAST_RX;i<GPON_REG_DSGEMPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMiscCnt_get(i, &cnt32),ret);
    }
    for(i=GPON_REG_USGTCPMMISC_PLOAM_BOH_TX;i<GPON_REG_USGTCPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_usGtcMiscCnt_get(i, &cnt32),ret);
    }


    /* configuration initialization */
    GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_losCfg_set(obj->parameter.laser.laser_optic_los_en,obj->parameter.laser.laser_optic_los_polar,obj->parameter.laser.laser_cdr_los_en,obj->parameter.laser.laser_cdr_los_polar,obj->parameter.laser.laser_los_filter_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_laserTime_set(obj->parameter.laser.laser_us_on,obj->parameter.laser.laser_us_off),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsScrambleState_set(obj->parameter.dsphy.ds_scramble_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsFecBypass_set(obj->parameter.dsphy.ds_fec_bypass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsFecThrd_set(obj->parameter.dsphy.ds_fec_thrd),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamOnuIdFilterState_set(obj->parameter.dsploam.ds_ploam_onuid_filter),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamBcAcceptState_set(obj->parameter.dsploam.ds_ploam_broadcast_accpt),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamDropCrcState_set(obj->parameter.dsploam.ds_ploam_drop_crc_err),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapCrcCheckState_set(obj->parameter.dsbwmap.ds_bwmap_crc_chk),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapFilterOnuIdState_set(obj->parameter.dsbwmap.ds_bwmap_onuid_filter),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPlendStrictMode_set(obj->parameter.dsbwmap.ds_bwmap_plend_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFrameTimeOut_set(obj->parameter.dsgem.assemble_timer),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFcsCheckState_set(obj->parameter.dseth.ds_eth_crc_chk),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsEthPti_set(obj->parameter.dseth.ds_eth_pti_mask,obj->parameter.dseth.ds_eth_pti_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsOmciPti_set(obj->parameter.dsomci.ds_omci_pti_mask,obj->parameter.dsomci.ds_omci_pti_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usScrambleState_set(obj->parameter.usphy.us_scramble_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usBurstPolarity_set(obj->parameter.usphy.us_burst_en_polar),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usSmalSstartProcState_set(obj->parameter.usphy.small_sstart_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usSuppressLaserState_set(obj->parameter.usphy.suppress_laser_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ploamState_set(obj->parameter.usploam.us_ploam_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_indNrmPloamState_set(obj->parameter.usploam.us_ind_normal_ploam),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dbruState_set(obj->parameter.usdbr.us_dbru_en),ret);
    /* set the PTI value of OMCI and Ether.
       For ALU OLT, it only accept OMCI with NON_END_FRAG=0 and END_FRAG=1. */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPtiVector_set(0,1,0,1),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortBcPassState_set(obj->mactbl.bc_pass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortNonMcPassState_set(obj->mactbl.non_mc_pass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ipv4McAddrPtn_set(obj->mactbl.ipv4_mc_check_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ipv6McAddrPtn_set(obj->mactbl.ipv6_mc_check_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacFilterMode_set(obj->mactbl.op_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V4,obj->mactbl.ipv4_force_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V6,obj->mactbl.ipv6_force_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_rdi_set(obj->rdi),ret);
    /* default configurations */
    GPON_UTIL_ERR_CHK(rtk_gpon_forceLaser_set(RTK_GPON_LASER_STATUS_NORMAL),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_extraSnTxTimes_set(0),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_usPloamCrcGenState_set(TRUE),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usPloamOnuIdFilterState_set(TRUE),ret);

    gpon_ploam_init(obj);

    /* for serial number set to ploamd */
    gpon_ploam_snPloam_set(obj);

    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"GMac init success!");
    return RT_ERR_OK;
}

int32 gpon_dev_deInitialize(gpon_dev_obj_t* obj)
{
    int32 ret;
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    return RT_ERR_OK;
}

int32 gpon_dev_sn_set(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn)
{
    osal_memcpy(&obj->serial_number,sn,sizeof(rtk_gpon_serialNumber_t));

    return gpon_ploam_snPloam_set(obj);
}

int32 gpon_dev_sn_get(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn)
{
    osal_memcpy(sn,&obj->serial_number,sizeof(rtk_gpon_serialNumber_t));
    return RT_ERR_OK;
}

int32 gpon_dev_pwd_set(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd)
{
    osal_memcpy(&obj->password,pwd,sizeof(rtk_gpon_password_t));
    return RT_ERR_OK;
}

int32 gpon_dev_pwd_get(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd)
{
    osal_memcpy(pwd,&obj->password,sizeof(rtk_gpon_password_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_onu_set(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para)
{
    if(obj->parameter.onu.to1_timer!=para->to1_timer)
    {
        obj->parameter.onu.to1_timer = para->to1_timer;
    }
    if(obj->parameter.onu.to2_timer!=para->to2_timer)
    {
        obj->parameter.onu.to2_timer = para->to2_timer;
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_onu_get(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para)
{
    osal_memcpy(para,&obj->parameter.onu,sizeof(rtk_gpon_onu_activation_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_laser_set(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->laser_optic_los_en != 0) && (para->laser_optic_los_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_optic_los_polar != 0) && (para->laser_optic_los_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_cdr_los_en != 0) && (para->laser_cdr_los_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_cdr_los_polar != 0) && (para->laser_cdr_los_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_los_filter_en != 0) && (para->laser_los_filter_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->laser_us_on > GPON_DEV_MAX_LASER_ON), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->laser_us_off > GPON_DEV_MAX_LASER_OFF), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.laser.laser_optic_los_en!=para->laser_optic_los_en
        || obj->parameter.laser.laser_optic_los_polar!=para->laser_optic_los_polar
        || obj->parameter.laser.laser_cdr_los_en!=para->laser_cdr_los_en
        || obj->parameter.laser.laser_cdr_los_polar!=para->laser_cdr_los_polar
        || obj->parameter.laser.laser_los_filter_en!=para->laser_los_filter_en)
    {
        obj->parameter.laser.laser_optic_los_en = para->laser_optic_los_en;
        obj->parameter.laser.laser_optic_los_polar = para->laser_optic_los_polar;
        obj->parameter.laser.laser_cdr_los_en = para->laser_cdr_los_en;
        obj->parameter.laser.laser_cdr_los_polar = para->laser_cdr_los_polar;
        obj->parameter.laser.laser_los_filter_en = para->laser_los_filter_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_losCfg_set(obj->parameter.laser.laser_optic_los_en,obj->parameter.laser.laser_optic_los_polar,obj->parameter.laser.laser_cdr_los_en,obj->parameter.laser.laser_cdr_los_polar,obj->parameter.laser.laser_los_filter_en),ret);
    }
    if(obj->parameter.laser.laser_us_on!=para->laser_us_on || obj->parameter.laser.laser_us_off!=para->laser_us_off)
    {
        obj->parameter.laser.laser_us_on = para->laser_us_on;
        obj->parameter.laser.laser_us_off = para->laser_us_off;
        GPON_UTIL_ERR_CHK(rtk_gpon_laserTime_set(obj->parameter.laser.laser_us_on,obj->parameter.laser.laser_us_off),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_laser_get(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para)
{
    para->laser_optic_los_en = obj->parameter.laser.laser_optic_los_en;
    para->laser_optic_los_polar = obj->parameter.laser.laser_optic_los_polar;
    para->laser_cdr_los_en = obj->parameter.laser.laser_cdr_los_en;
    para->laser_cdr_los_polar = obj->parameter.laser.laser_cdr_los_polar;
    para->laser_los_filter_en = obj->parameter.laser.laser_los_filter_en;
    para->laser_us_on = obj->parameter.laser.laser_us_on;
    para->laser_us_off = obj->parameter.laser.laser_us_off;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPhy_set(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_scramble_en != 0) && (para->ds_scramble_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->ds_fec_bypass != 0) && (para->ds_fec_bypass != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_fec_thrd > GPON_DEV_MAX_DS_FEC_DET_THRSH), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsphy.ds_scramble_en!=para->ds_scramble_en)
    {
        obj->parameter.dsphy.ds_scramble_en = para->ds_scramble_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsScrambleState_set(obj->parameter.dsphy.ds_scramble_en),ret);
    }
    if(obj->parameter.dsphy.ds_fec_bypass!=para->ds_fec_bypass)
    {
        obj->parameter.dsphy.ds_fec_bypass = para->ds_fec_bypass;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsFecBypass_set(obj->parameter.dsphy.ds_fec_bypass),ret);
    }
    if(obj->parameter.dsphy.ds_fec_thrd!=para->ds_fec_thrd)
    {
        obj->parameter.dsphy.ds_fec_thrd = para->ds_fec_thrd;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsFecThrd_set(obj->parameter.dsphy.ds_fec_thrd),ret);
    }
    return RT_ERR_OK;
}

int gpon_dev_para_dsPhy_get(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para)
{
    para->ds_scramble_en = obj->parameter.dsphy.ds_scramble_en;
    para->ds_fec_bypass = obj->parameter.dsphy.ds_fec_bypass;
    para->ds_fec_thrd = obj->parameter.dsphy.ds_fec_thrd;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPloam_set(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_ploam_onuid_filter != 0) && (para->ds_ploam_onuid_filter != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->ds_ploam_broadcast_accpt != 0) && (para->ds_ploam_broadcast_accpt != 1)), RT_ERR_OUT_OF_RANGE);
    /* Add in GPON_MAC_SWIO_v1.1 */
    RT_PARAM_CHK(((para->ds_ploam_drop_crc_err != 0) && (para->ds_ploam_drop_crc_err != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsploam.ds_ploam_onuid_filter!=para->ds_ploam_onuid_filter)
    {
        obj->parameter.dsploam.ds_ploam_onuid_filter = para->ds_ploam_onuid_filter;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamOnuIdFilterState_set(obj->parameter.dsploam.ds_ploam_onuid_filter),ret);
    }
    if(obj->parameter.dsploam.ds_ploam_broadcast_accpt!=para->ds_ploam_broadcast_accpt)
    {
        obj->parameter.dsploam.ds_ploam_broadcast_accpt = para->ds_ploam_broadcast_accpt;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamBcAcceptState_set(obj->parameter.dsploam.ds_ploam_broadcast_accpt),ret);
    }
    if(obj->parameter.dsploam.ds_ploam_drop_crc_err!=para->ds_ploam_drop_crc_err)
    {
        obj->parameter.dsploam.ds_ploam_drop_crc_err = para->ds_ploam_drop_crc_err;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamDropCrcState_set(obj->parameter.dsploam.ds_ploam_drop_crc_err),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPloam_get(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para)
{
    osal_memcpy(para,&obj->parameter.dsploam,sizeof(rtk_gpon_ds_ploam_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsBwmap_set(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_bwmap_crc_chk != 0) && (para->ds_bwmap_crc_chk != 1)), RT_ERR_OUT_OF_RANGE);
    /* Added in GPON_MAC_SWIO_v1.1 */
    RT_PARAM_CHK(((para->ds_bwmap_onuid_filter != 0) && (para->ds_bwmap_onuid_filter != 1)), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(((para->ds_bwmap_plend_mode != 0) && (para->ds_bwmap_plend_mode != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsbwmap.ds_bwmap_crc_chk!=para->ds_bwmap_crc_chk)
    {
        obj->parameter.dsbwmap.ds_bwmap_crc_chk = para->ds_bwmap_crc_chk;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapCrcCheckState_set(obj->parameter.dsbwmap.ds_bwmap_crc_chk),ret);
    }
    if(obj->parameter.dsbwmap.ds_bwmap_onuid_filter!=para->ds_bwmap_onuid_filter)
    {
        obj->parameter.dsbwmap.ds_bwmap_onuid_filter = para->ds_bwmap_onuid_filter;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapFilterOnuIdState_set(obj->parameter.dsbwmap.ds_bwmap_onuid_filter),ret);
    }
    if(obj->parameter.dsbwmap.ds_bwmap_plend_mode!=para->ds_bwmap_plend_mode)
    {
        obj->parameter.dsbwmap.ds_bwmap_plend_mode = para->ds_bwmap_plend_mode;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPlendStrictMode_set(obj->parameter.dsbwmap.ds_bwmap_plend_mode),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsBwmap_get(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para)
{
    osal_memcpy(para,&obj->parameter.dsbwmap,sizeof(rtk_gpon_ds_bwMap_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsGem_set(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK((para->assemble_timer > GPON_DEV_MAX_ASSM_TIMEOUT_FRM), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsgem.assemble_timer!=para->assemble_timer)
    {
        obj->parameter.dsgem.assemble_timer = para->assemble_timer;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFrameTimeOut_set(obj->parameter.dsgem.assemble_timer),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsGem_get(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para)
{
    para->assemble_timer = obj->parameter.dsgem.assemble_timer;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsEth_set(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para)
{

    int32 ret;
    RT_PARAM_CHK(((para->ds_eth_crc_chk != 0) && (para->ds_eth_crc_chk != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_eth_pti_mask > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_eth_pti_ptn > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dseth.ds_eth_crc_chk!=para->ds_eth_crc_chk)
    {
        obj->parameter.dseth.ds_eth_crc_chk = para->ds_eth_crc_chk;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFcsCheckState_set(obj->parameter.dseth.ds_eth_crc_chk),ret);
    }
    if(obj->parameter.dseth.ds_eth_pti_mask!=para->ds_eth_pti_mask || obj->parameter.dseth.ds_eth_pti_ptn!=para->ds_eth_pti_ptn)
    {
        obj->parameter.dseth.ds_eth_pti_mask = para->ds_eth_pti_mask;
        obj->parameter.dseth.ds_eth_pti_ptn = para->ds_eth_pti_ptn;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsEthPti_set(obj->parameter.dseth.ds_eth_pti_mask,obj->parameter.dseth.ds_eth_pti_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsEth_get(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para)
{
    para->ds_eth_crc_chk = obj->parameter.dseth.ds_eth_crc_chk;
    para->ds_eth_pti_mask = obj->parameter.dseth.ds_eth_pti_mask;
    para->ds_eth_pti_ptn = obj->parameter.dseth.ds_eth_pti_ptn;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsOmci_set(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK((para->ds_omci_pti_mask > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_omci_pti_ptn > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsomci.ds_omci_pti_mask!=para->ds_omci_pti_mask || obj->parameter.dsomci.ds_omci_pti_ptn!=para->ds_omci_pti_ptn)
    {
        obj->parameter.dsomci.ds_omci_pti_mask = para->ds_omci_pti_mask;
        obj->parameter.dsomci.ds_omci_pti_ptn = para->ds_omci_pti_ptn;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsOmciPti_set(obj->parameter.dsomci.ds_omci_pti_mask,obj->parameter.dsomci.ds_omci_pti_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsOmci_get(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para)
{
    para->ds_omci_pti_mask = obj->parameter.dsomci.ds_omci_pti_mask;
    para->ds_omci_pti_ptn = obj->parameter.dsomci.ds_omci_pti_ptn;

    return RT_ERR_OK;
}

int32 gpon_dev_para_usPhy_set(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_scramble_en != 0) && (para->us_scramble_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->us_burst_en_polar != 0) && (para->us_burst_en_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->small_sstart_en != 0) && (para->small_sstart_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->suppress_laser_en != 0) && (para->suppress_laser_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usphy.us_scramble_en!=para->us_scramble_en)
    {
        obj->parameter.usphy.us_scramble_en = para->us_scramble_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usScrambleState_set(obj->parameter.usphy.us_scramble_en),ret);
    }
    if(obj->parameter.usphy.us_burst_en_polar!=para->us_burst_en_polar)
    {
        obj->parameter.usphy.us_burst_en_polar = para->us_burst_en_polar;
        GPON_UTIL_ERR_CHK(rtk_gpon_usBurstPolarity_set(obj->parameter.usphy.us_burst_en_polar),ret);
    }
    if(obj->parameter.usphy.small_sstart_en!=para->small_sstart_en)
    {
        obj->parameter.usphy.small_sstart_en = para->small_sstart_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usSmalSstartProcState_set(obj->parameter.usphy.small_sstart_en),ret);
    }
    if(obj->parameter.usphy.suppress_laser_en!=para->suppress_laser_en)
    {
        obj->parameter.usphy.suppress_laser_en = para->suppress_laser_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usSuppressLaserState_set(obj->parameter.usphy.suppress_laser_en),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPhy_get(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usphy,sizeof(rtk_gpon_us_physical_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPloam_set(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_ploam_en != 0) && (para->us_ploam_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usploam.us_ploam_en!=para->us_ploam_en)
    {
        obj->parameter.usploam.us_ploam_en = para->us_ploam_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_ploamState_set(obj->parameter.usploam.us_ploam_en),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPloam_get(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usploam,sizeof(rtk_gpon_us_ploam_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_usDbr_set(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_dbru_en != 0) && (para->us_dbru_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usdbr.us_dbru_en!=para->us_dbru_en)
    {
        obj->parameter.usdbr.us_dbru_en = para->us_dbru_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_dbruState_set(obj->parameter.usdbr.us_dbru_en),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_para_usDbr_get(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usdbr,sizeof(rtk_gpon_us_dbr_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_onuId_get(gpon_dev_obj_t* obj, uint8 *para)
{
    *para = obj->onuid;
    return RT_ERR_OK;
}

int32 gpon_dev_para_drvStatus_get(gpon_drv_obj_t* obj, gpon_drv_status_t *para)
{
    *para = obj->status;
    return RT_ERR_OK;
}


int32 gpon_dev_activate(gpon_dev_obj_t* obj, rtk_gpon_initialState_t initState)
{
    uint32 ds_fec, us_fec;
    int32 ret;
    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Device Active %d [baseaddr:%p]",initState,obj->base_addr);

    obj->cnt_boh_tx = 0;
    obj->cnt_ploamu_tx = 0;
    obj->cnt_dbru_tx = 0;
    obj->cnt_gem_tx = 0;

    obj->cnt_cpu_ploam_rx = 0;
    obj->cnt_cpu_omci_rx = 0;
    obj->cnt_cpu_ploam_tx = 0;
    obj->cnt_cpu_ploam_tx_nor = 0;
    obj->cnt_cpu_ploam_tx_urg = 0;

    if(initState==RTK_GPONMAC_INIT_STATE_O7)
    {
        obj->status = RTK_GPONMAC_FSM_STATE_O7;
    }
    else
    {
        obj->status = RTK_GPONMAC_FSM_STATE_O1;
    }

    /* Set State to chip */
	GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

	/* Check Alarm State */
	gpon_alarm_status_check(obj);

	/* Check FEC State */
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntr_get(GPON_DS_FEC_STS_DLT,&ds_fec),ret);
	gpon_dev_dsFec_report(obj,ds_fec);

	GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntr_get(GPON_US_FEC_STS,&us_fec),ret);
	gpon_dev_usFec_report(obj,us_fec);

	/* Enable INT */
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_PLM_BUF_REQ, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_RNG_REQ_HIS, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_SN_REQ_HIS, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOM_DLT, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_DS_FEC_STS_DLT, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOF_DLT, ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOS_DLT, ENABLED),ret);

	GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_US_FEC_STS,ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_PLM_NRM_EMPTY,ENABLED),ret);
	GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_PLM_URG_EMPTY,ENABLED),ret);

    return RT_ERR_OK;
}

int32 gpon_dev_deactivate(gpon_dev_obj_t* obj)
{
    int32 ret;
    /* Disable INT */
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);

    /* deinit ploam: send DyingGasp to OLT */
    /*gpon_ploam_deinit(obj);*/

    gpon_alarm_status_reset(obj);

    /* Check FEC State */
    gpon_dev_dsFec_report(obj,FALSE);
    gpon_dev_usFec_report(obj,FALSE);

    /* Reset status */
    if(obj->status!=RTK_GPONMAC_FSM_STATE_O7)
    {
        obj->status = RTK_GPONMAC_FSM_STATE_O1;
    }

    /* Stop Timer */
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    if(obj->ber_timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop ber timer %p",obj->ber_timer);
        GPON_OS_StopTimer(obj->ber_timer);
        obj->ber_timer = 0;
    }

    /* Set State to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

    /* Set ONU_ID to default ONU_ID 255 */
    gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID);

    if(obj->status!=RTK_GPONMAC_FSM_STATE_O7)
    {
		gpon_fsm_event(obj, GPON_FSM_EVENT_LOS_DETECT);
    }

    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Device DeActive [baseaddr:%p]",obj->base_addr);

    return RT_ERR_OK;
}

int32 gpon_dev_get_status(gpon_dev_obj_t* obj, rtk_gpon_fsm_status_t* status)
{
    *status = obj->status;
    return RT_ERR_OK;
}

int32 GPON_Dev_get_fec_status(gpon_dev_obj_t* obj, int32 *ds, int32 *us)
{
    *ds = obj->ds_fec;
    *us = obj->us_fec;
    return RT_ERR_OK;
}

int32 gpon_dev_dsFec_report(gpon_dev_obj_t* obj, int32 status)
{
    if(obj->ds_fec!=status)
    {
        obj->ds_fec = status;
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"D/S FEC Status Change Event: FEC %s [baseaddr:%p]",(obj->ds_fec)?"ON":"OFF",obj->base_addr);
        if(obj->dsfec_change_callback)
        {
            (*obj->dsfec_change_callback)(obj->ds_fec);
        }
    }
    return RT_ERR_OK;
}

int32 gpon_dev_usFec_report(gpon_dev_obj_t* obj, int32 status)
{
    if(obj->us_fec!=status)
    {
        obj->us_fec = status;
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S FEC Status Change Event: FEC %s [baseaddr:%p]",(obj->us_fec)?"ON":"OFF",obj->base_addr);
        if(obj->usfec_change_callback)
        {
            (*obj->usfec_change_callback)(obj->us_fec);
        }
    }
    return RT_ERR_OK;
}

int32 gpon_dev_upstreamOverhead_calculate(gpon_dev_obj_t* obj)
{
    gpon_burst_oh_t* overhead = &obj->burst_overhead;

    osal_memset(overhead->bursthead,0,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    if (overhead->guard_bits > 32)
    {
    overhead->guard_bits = 32;
    }

    gpon_res_set_bits(overhead->bursthead,overhead->guard_bits,overhead->guard_bits+overhead->type1_bits-1,1);
    overhead->boh_repeat = (overhead->guard_bits+overhead->type1_bits+overhead->type2_bits)/8;
    overhead->bursthead[overhead->boh_repeat] = overhead->type3_ptn;
    overhead->bursthead[overhead->boh_repeat+1] = overhead->delimiter[0];
    overhead->bursthead[overhead->boh_repeat+2] = overhead->delimiter[1];
    overhead->bursthead[overhead->boh_repeat+3] = overhead->delimiter[2];

    if(overhead->type3_preranged)
    {
        overhead->boh_len_preranged = overhead->boh_repeat+overhead->type3_preranged+3;
	 if(overhead->boh_len_preranged > GPON_DEV_MAX_BOH_LEN)
	 {
            overhead->boh_len_preranged = GPON_DEV_MAX_BOH_LEN;
	 }
    }
    else
    {
        overhead->boh_len_preranged = GPON_DEV_TOTAL_OVERHEAD_BITS/8;
    }

    if(overhead->type3_ranged)
    {
        overhead->boh_len_ranged = overhead->boh_repeat+overhead->type3_ranged+3;
	 if(overhead->boh_len_ranged > GPON_DEV_MAX_BOH_LEN)
	 {
            overhead->boh_len_ranged = GPON_DEV_MAX_BOH_LEN;
	 }
    }
    else
    {
        overhead->boh_len_ranged = GPON_DEV_TOTAL_OVERHEAD_BITS/8;
    }
    return RT_ERR_OK;
}

	/* This API seem to wrong.
	   1. It should be boh_len_preranged>GPON_DEV_BURST_OVERHEAD_LEN.
	   Because the minimum boh is 96 bits, maximum is 128 bytes.
	   2. The oh should be fill as [guard bit, Preamble1, p2, p3 repeat pattern...delimiter1,d2,d3].															.
	   Scott */
int32 gpon_dev_burstHead_preRanged_set(gpon_dev_obj_t* obj)
{
    uint8 i,len;
    int32 ret;
    uint8 oh[GPON_DEV_BURST_OVERHEAD_LEN];

    if(obj->burst_overhead.boh_len_preranged>GPON_DEV_BURST_OVERHEAD_LEN)
    {
        len = GPON_DEV_BURST_OVERHEAD_LEN;
    }
    else
    {
        len = obj->burst_overhead.boh_len_preranged;
    }

    osal_memset(oh,0,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    for(i=0;i<obj->burst_overhead.boh_repeat;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[i];
    }
    for(;i<len-3;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat];
    }
    for(;i<len;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat+i+4-len];
    }

    if(1 == obj->auto_boh_state)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Set BOH: repeat %d, length %d, %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_preranged,oh[0],oh[1],oh[2],oh[3],oh[4],oh[5],oh[6],oh[7],oh[8],oh[9],oh[10],oh[11],oh[12],oh[13],oh[14],oh[15],obj->base_addr);
        GPON_UTIL_ERR_CHK(rtk_gpon_burstOverhead_set(obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_preranged,len,oh),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_burstHead_ranged_set(gpon_dev_obj_t* obj)
{
    uint8 i,len;
    int32 ret;
    uint8 oh[GPON_DEV_BURST_OVERHEAD_LEN];
    if(obj->burst_overhead.boh_len_ranged>GPON_DEV_BURST_OVERHEAD_LEN)
    {
        len = GPON_DEV_BURST_OVERHEAD_LEN;
    }
    else
    {
        len = obj->burst_overhead.boh_len_ranged;
    }

    osal_memset(oh,0,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    for(i=0;i<obj->burst_overhead.boh_repeat;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[i];
    }
    for(;i<len-3;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat];
    }
    for(;i<len;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat+i+4-len];
    }

    if(1 == obj->auto_boh_state)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Set BOH: repeat %d, length %d, %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_ranged,oh[0],oh[1],oh[2],oh[3],oh[4],oh[5],oh[6],oh[7],oh[8],oh[9],oh[10],oh[11],oh[12],oh[13],oh[14],oh[15],obj->base_addr);
        GPON_UTIL_ERR_CHK(rtk_gpon_burstOverhead_set(obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_ranged,len,oh),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_onuid_set(gpon_dev_obj_t* obj, uint8 onuid)
{
    int32 ret;
    if(GPON_DEV_DEFAULT_ONU_ID==onuid && obj->onuid!=GPON_DEV_DEFAULT_ONU_ID)
    {
        gpon_dev_tcont_physical_reset(obj);
        gpon_dev_dsFlow_del(obj, GPON_OMCI_FLOW_ID);
        gpon_dev_usFlow_del(obj, GPON_OMCI_FLOW_ID);
    }

    obj->onuid = onuid;
    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"set ONU ID %d [baseaddr:%p]",obj->onuid,obj->base_addr);
    GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);


    /* add default tcont */
    if(onuid!=GPON_DEV_DEFAULT_ONU_ID)
    {
            gpon_dev_tcont_physical_add(obj,onuid,NULL);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_tcont_logical_add(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    uint32 i;
    uint32 idle = GPON_DEV_MAX_TCONT_NUM;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<GPON_DEV_MAX_TCONT_NUM;i++)
#else
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
#endif
    {
        /* Search an idle tcont */
        if(idle == GPON_DEV_MAX_TCONT_NUM && !obj->p_tcont[i])
        {
            idle = i;
        }
        /* found it */
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            break;
        }
    }

    if(i==GPON_DEV_MAX_TCONT_NUM)    /* not found */
    {
        if(idle==GPON_DEV_MAX_TCONT_NUM)    /* no idle tcont */
        {
            return RT_ERR_GPON_EXCEED_MAX_TCONT;
        }
        else    /* create new tcont */
        {
            obj->p_tcont[idle] = (gpon_tcont_obj_t*)GPON_OS_Malloc(sizeof(gpon_tcont_obj_t));
            obj->p_tcont[idle]->tcont_id = idle;
            obj->p_tcont[idle]->alloc_id = ind->alloc_id;
            obj->p_tcont[idle]->type = ind->type;
            obj->p_tcont[idle]->ploam_en = FALSE;
            obj->p_tcont[idle]->omci_en = TRUE;
            attr->tcont_id = obj->p_tcont[idle]->tcont_id;
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create Logical TCont %d: alloc %d, type %d [baseaddr:%p]",idle,obj->p_tcont[idle]->alloc_id,obj->p_tcont[idle]->type,obj->base_addr);
            return RT_ERR_OK;
        }
    }
    else    /* found */
    {
        attr->tcont_id = obj->p_tcont[i]->tcont_id;
        if(obj->p_tcont[i]->omci_en)    /* added before */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            obj->p_tcont[i]->omci_en = TRUE;
            obj->p_tcont[i]->type = ind->type;

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Enable Logical TCont %d: alloc %d, type %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->p_tcont[i]->type,obj->base_addr);
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_logical_del(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind)
{
    uint32 i;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<GPON_DEV_MAX_TCONT_NUM;i++)
#else
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            /* found it */
            break;
        }
    }

    if(i==GPON_DEV_MAX_TCONT_NUM)    /* not found */
    {
        return RT_ERR_INPUT;
    }
    else    /* found */
    {
        if(!obj->p_tcont[i]->omci_en)    /* not added */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            /* remove from logical */
            obj->p_tcont[i]->omci_en = FALSE;

            /* if removed from physical, free it */
            if(!obj->p_tcont[i]->ploam_en)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove Logical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                GPON_OS_Free(obj->p_tcont[i]);
                obj->p_tcont[i] = NULL;
            }
            else
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Disable Logical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
            }
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_logical_get(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    uint32 i;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<GPON_DEV_MAX_TCONT_NUM;i++)
#else
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            /* found it */
            break;
        }
    }

    attr->tcont_id = GPON_DEV_MAX_TCONT_NUM;

    if(i==GPON_DEV_MAX_TCONT_NUM)    /* not found */
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get alloc %d",ind->alloc_id);
        return RT_ERR_OUT_OF_RANGE;
    }
    else    /* found */
    {
        attr->tcont_id = obj->p_tcont[i]->tcont_id;
        if(!obj->p_tcont[i]->omci_en)    /* not added */
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get logical alloc %d",ind->alloc_id);
            return RT_ERR_OUT_OF_RANGE;
        }
        else
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Get Logical TCont %d: alloc %d",i,obj->p_tcont[i]->alloc_id);
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_physical_add(gpon_dev_obj_t* obj, uint32 alloc,rtk_gpon_tcont_attr_t* attr)
{
    uint32 i=GPON_DEV_MAX_TCONT_NUM,ret;
    uint32 idle = GPON_DEV_MAX_TCONT_NUM;
   // uint32 read_alloc_id=0;


    /* add for default tcont */
    if(alloc < GPON_DEV_DEFAULT_ONU_ID)
    {
       // GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_get(GPON_OMCI_TCONT_ID, &read_alloc_id),ret);
       // if(read_alloc_id == alloc)
       //     return RT_ERR_OK;

        idle = GPON_OMCI_TCONT_ID;
    }
    else /* normal case */
    {
    	
		if(attr!=NULL && !obj->p_tcont[attr->tcont_id])
		{
			idle = attr->tcont_id;
		}else
		{
	        /* search alloc id from table */
	        for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
	        {
	            /* Search an idle tcont */
	            if(idle == GPON_DEV_MAX_TCONT_NUM && !obj->p_tcont[i])
	            {
	                idle = i;
	            }
	            /* found it */
	            if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==alloc)
	            {
	                break;
	            }
	        }
		}
    }

    if(i==GPON_DEV_MAX_TCONT_NUM)    /* not found */
    {
        if(idle==GPON_DEV_MAX_TCONT_NUM)    /* no idle tcont */
        {
            return RT_ERR_GPON_EXCEED_MAX_TCONT;
        }
        else    /* create new tcont */
        {
            obj->p_tcont[idle] = (gpon_tcont_obj_t*)GPON_OS_Malloc(sizeof(gpon_tcont_obj_t));
            obj->p_tcont[idle]->tcont_id = idle;
            obj->p_tcont[idle]->alloc_id = alloc;
            obj->p_tcont[idle]->type = RTK_GPON_TCONT_TYPE_3;
            obj->p_tcont[idle]->ploam_en = TRUE;
            obj->p_tcont[idle]->omci_en = FALSE;

            /* write to chip */
            GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_set(obj->p_tcont[idle]->tcont_id,obj->p_tcont[idle]->alloc_id),ret);

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create Physical TCont %d: alloc %d [baseaddr:%p]",idle,obj->p_tcont[idle]->alloc_id,obj->base_addr);
            return RT_ERR_OK;
        }
    }
    else    /* found */
    {
        if(!obj->p_tcont[i]->ploam_en)
        {
            obj->p_tcont[i]->ploam_en = TRUE;

            /* write to chip */
            GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_set(obj->p_tcont[i]->tcont_id,obj->p_tcont[i]->alloc_id),ret);

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Enable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
        }
        return RT_ERR_OK;
    }
}

int32 gpon_dev_tcont_physical_del(gpon_dev_obj_t* obj, uint32 alloc)
{
    uint32 i,ret;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<GPON_DEV_MAX_TCONT_NUM;i++)
#else
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==alloc)
        {
            /* found it */
            break;
        }
    }

    if(i==GPON_DEV_MAX_TCONT_NUM)    /* not found */
    {
        return RT_ERR_INPUT;
    }
    else    /* found */
    {
        if(!obj->p_tcont[i]->ploam_en)    /* not added */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            /* write to chip */
            GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(obj->p_tcont[i]->tcont_id),ret);

            /* remove from physical */
            obj->p_tcont[i]->ploam_en = FALSE;

            /* if removed from physical, free it */
            if(!obj->p_tcont[i]->omci_en)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                GPON_OS_Free(obj->p_tcont[i]);
                obj->p_tcont[i] = NULL;
            }
            else
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Disable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
            }
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_physical_reset(gpon_dev_obj_t* obj)
{
    uint32 i,ret;

    /* search alloc id from table */
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
        if(obj->p_tcont[i])
        {
            if(obj->p_tcont[i]->ploam_en)    /* added */
            {
                /* write to chip */
                GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(obj->p_tcont[i]->tcont_id),ret);

                /* remove from physical */
                obj->p_tcont[i]->ploam_en = FALSE;

                /* if removed from physical, free it */
                if(!obj->p_tcont[i]->omci_en)
                {
                    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Remove Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                    GPON_OS_Free(obj->p_tcont[i]);
                    obj->p_tcont[i] = NULL;
                }
                else
                {
                    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Disable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr)
{
    uint32 i;
    rtk_gem_cfg_t gemCfg;
    int32 ret;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is used by other gem port */
    if(flow_id != GPON_OMCI_FLOW_ID)
    {
        if(obj->p_dsflow[flow_id] && obj->p_dsflow[flow_id]->gem_port!=attr->gem_port_id)
        {
            return RT_ERR_INPUT;
        }
    }

    /* gem port is added to other flow */
    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        if(obj->p_dsflow[i] && obj->p_dsflow[i]->flow_id!=flow_id && obj->p_dsflow[i]->gem_port==attr->gem_port_id)
        {
            /* found it */
            return RT_ERR_INPUT;
        }
    }

    if(obj->p_dsflow[flow_id] && obj->p_dsflow[flow_id]->gem_port==attr->gem_port_id)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Update D/S Flow %d: GEM port %d type %d multicast %d AES %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->multicast,attr->aes_en,obj->base_addr);
        GPON_OS_Free(obj->p_dsflow[flow_id]);
        obj->p_dsflow[flow_id] = NULL;
    }
    else
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create D/S Flow %d: GEM port %d type %d multicast %d AES %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->multicast,attr->aes_en,obj->base_addr);
    }

    obj->p_dsflow[flow_id] = (gpon_ds_flow_obj_t*)GPON_OS_Malloc(sizeof(gpon_ds_flow_obj_t));
    obj->p_dsflow[flow_id]->flow_id = flow_id;
    obj->p_dsflow[flow_id]->gem_port = attr->gem_port_id;
    obj->p_dsflow[flow_id]->type = attr->type;
    obj->p_dsflow[flow_id]->multicast = attr->multicast;
    obj->p_dsflow[flow_id]->aes_en = attr->aes_en;


    osal_memset(&gemCfg,0,sizeof(rtk_gem_cfg_t));
    gemCfg.enAES = obj->p_dsflow[flow_id]->aes_en ? 1:0;

    /* write to chip */
    switch(obj->p_dsflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
    		gemCfg.isEthernet = DISABLED;
		gemCfg.isOMCI= ENABLED;

            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
            	gemCfg.isEthernet = ENABLED;
		gemCfg.isOMCI= DISABLED;
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }
     gemCfg.gemPortId = obj->p_dsflow[flow_id]->gem_port;
     gemCfg.isMulticast = obj->p_dsflow[flow_id]->multicast ? 1:0;

     GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_set(obj->p_dsflow[flow_id]->flow_id,gemCfg),ret);


    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_del(gpon_dev_obj_t* obj, uint32 flow_id)
{
    int32 ret;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_dsflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove D/S Flow %d: GEM port %d type %d [baseaddr:%p]",
        flow_id,obj->p_dsflow[flow_id]->gem_port,obj->p_dsflow[flow_id]->type,obj->base_addr);
    /* write to chip */
    GPON_UTIL_ERR_CHK( rtk_gpon_dsGemPort_del(obj->p_dsflow[flow_id]->flow_id),ret);

    GPON_OS_Free(obj->p_dsflow[flow_id]);
    obj->p_dsflow[flow_id] = NULL;

    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr)
{
    int32 ret;
    rtk_gem_cfg_t gemCfg;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_dsflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    attr->gem_port_id = obj->p_dsflow[flow_id]->gem_port;
    attr->type = obj->p_dsflow[flow_id]->type;
    attr->multicast = obj->p_dsflow[flow_id]->multicast;
    attr->aes_en = obj->p_dsflow[flow_id]->aes_en;

    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_get(flow_id, &gemCfg),ret);

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr)
{

    int32 ret;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is used by other gem port */
    if(flow_id != GPON_OMCI_FLOW_ID)
    {
        if(obj->p_usflow[flow_id] && obj->p_usflow[flow_id]->gem_port!=attr->gem_port_id)
        {
            return RT_ERR_INPUT;
        }
    }

    /* The checking is removed because the Apollo spec allow many_flow-to-one_gem_port case. */

    /* check the OMCI */
    if(attr->type==RTK_GPON_FLOW_TYPE_OMCI)
    {
        if(obj->us_omci_flow!=GPON_DEV_MAX_FLOW_NUM)
        {
            return RT_ERR_INPUT;
        }
    }

    /* check the TDM */
    if(attr->type==RTK_GPON_FLOW_TYPE_TDM)
    {
        if(!obj->tdm_en)
        {
            return RT_ERR_GPON_INVALID_HANDLE;
        }
    }

    if(obj->p_usflow[flow_id] && obj->p_usflow[flow_id]->gem_port==attr->gem_port_id)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Update U/S Flow %d: GEM port %d type %d tcont %d channel %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->tcont_id,attr->channel,obj->base_addr);
        GPON_OS_Free(obj->p_usflow[flow_id]);
        obj->p_usflow[flow_id] = NULL;
    }
    else
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create U/S Flow %d: GEM port %d type %d tcont %d channel %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->tcont_id,attr->channel,obj->base_addr);
    }

    obj->p_usflow[flow_id] = (gpon_us_flow_obj_t*)GPON_OS_Malloc(sizeof(gpon_us_flow_obj_t));
    obj->p_usflow[flow_id]->flow_id = flow_id;
    obj->p_usflow[flow_id]->gem_port = attr->gem_port_id;
    obj->p_usflow[flow_id]->type = attr->type;
    obj->p_usflow[flow_id]->tcont = attr->tcont_id;
    obj->p_usflow[flow_id]->channel = GPON_DEV_MAX_TDM_CHANNEL_NUM;

    /* write to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_set(obj->p_usflow[flow_id]->flow_id,obj->p_usflow[flow_id]->gem_port),ret);

    switch(obj->p_usflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
            obj->us_omci_flow = flow_id;
            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
            break;
        }
        case RTK_GPON_FLOW_TYPE_TDM:
        {
            obj->p_usflow[flow_id]->channel = attr->channel;
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_del(gpon_dev_obj_t* obj, uint32 flow_id)
{
    int32 ret;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_usflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    /* check the TDM */
    if(obj->p_usflow[flow_id]->type==RTK_GPON_FLOW_TYPE_TDM)
    {
        if(!obj->tdm_en)
        {
            return RT_ERR_GPON_INVALID_HANDLE;
        }
    }

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove U/S Flow %d: GEM port %d type %d [baseaddr:%p]",
        flow_id,obj->p_usflow[flow_id]->gem_port,obj->p_usflow[flow_id]->type,obj->base_addr);
    /* write to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_set(obj->p_usflow[flow_id]->flow_id,0),ret);
    switch(obj->p_usflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
            obj->us_omci_flow = GPON_DEV_MAX_FLOW_NUM;
            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
            break;
        }
        case RTK_GPON_FLOW_TYPE_TDM:
        {
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }

    GPON_OS_Free(obj->p_usflow[flow_id]);
    obj->p_usflow[flow_id] = NULL;

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr)
{

    uint32 gemPortId,ret;
    RT_PARAM_CHK((flow_id >= GPON_DEV_MAX_FLOW_NUM), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_usflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    attr->gem_port_id = obj->p_usflow[flow_id]->gem_port;
    attr->type = obj->p_usflow[flow_id]->type;
    attr->tcont_id = obj->p_usflow[flow_id]->tcont;
    attr->channel = obj->p_usflow[flow_id]->channel;

    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_get(flow_id,&gemPortId),ret);
    return RT_ERR_OK;
}

int32 gpon_dev_bcastPass_set(gpon_dev_obj_t* obj, int32 status)
{
    int32 ret;
    RT_PARAM_CHK(((status != 0)&&(status != 1)), RT_ERR_OUT_OF_RANGE);

    if(status!=obj->mactbl.bc_pass)
    {
        obj->mactbl.bc_pass = status;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortBcPassState_set(obj->mactbl.bc_pass),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_bcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus)
{
    *pStatus = obj->mactbl.bc_pass;
    return RT_ERR_OK;
}

int32 gpon_dev_nonMcastPass_set(gpon_dev_obj_t* obj, int32 status)
{
    int32 ret;
    RT_PARAM_CHK(((status != 0)&&(status != 1)), RT_ERR_OUT_OF_RANGE);

    if(status!=obj->mactbl.non_mc_pass)
    {
        obj->mactbl.non_mc_pass = status;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortNonMcPassState_set(obj->mactbl.non_mc_pass),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_nonMcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus)
{
    *pStatus = obj->mactbl.non_mc_pass;
    return RT_ERR_OK;
}

int32 gpon_dev_mcastCheck_set(gpon_dev_obj_t* obj, uint32 ipv4_addr, uint32 ipv6_addr)
{
    int32 ret;
    if(ipv4_addr!=obj->mactbl.ipv4_mc_check_ptn)
    {
        obj->mactbl.ipv4_mc_check_ptn = ipv4_addr;
        GPON_UTIL_ERR_CHK(rtk_gpon_ipv4McAddrPtn_set(obj->mactbl.ipv4_mc_check_ptn),ret);
    }
    if(ipv6_addr!=obj->mactbl.ipv6_mc_check_ptn)
    {
        obj->mactbl.ipv6_mc_check_ptn = ipv6_addr;
        GPON_UTIL_ERR_CHK(rtk_gpon_ipv6McAddrPtn_set(obj->mactbl.ipv6_mc_check_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_mcastCheck_get(gpon_dev_obj_t* obj, uint32* ipv4_addr, uint32* ipv6_addr)
{
    *ipv4_addr = obj->mactbl.ipv4_mc_check_ptn;
    *ipv6_addr = obj->mactbl.ipv6_mc_check_ptn;
    return RT_ERR_OK;
}

int32 gpon_dev_macFilterMode_set(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t mode)
{
    int32 ret;
    RT_PARAM_CHK((mode > RTK_GPON_MACTBL_MODE_EXCLUDE), RT_ERR_OUT_OF_RANGE);

    if(mode!=obj->mactbl.op_mode)
    {
        obj->mactbl.op_mode = mode;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacFilterMode_set(obj->mactbl.op_mode),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_macFilterMode_get(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t *pMode)
{
    *pMode = obj->mactbl.op_mode;
    return RT_ERR_OK;
}

int32 gpon_dev_mcForceMode_set(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    int32 ret;
    RT_PARAM_CHK((ipv4 > RTK_GPON_MCFORCE_MODE_DROP), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((ipv6 > RTK_GPON_MCFORCE_MODE_DROP), RT_ERR_OUT_OF_RANGE);

    if((ipv4!=obj->mactbl.ipv4_force_mode) || (ipv6!=obj->mactbl.ipv6_force_mode))
    {
        obj->mactbl.ipv4_force_mode = ipv4;
        obj->mactbl.ipv6_force_mode = ipv6;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(obj->mactbl.ipv4_force_mode,obj->mactbl.ipv6_force_mode),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_mcForceMode_get(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6)
{
    *ipv4 = obj->mactbl.ipv4_force_mode;
    *ipv6 = obj->mactbl.ipv6_force_mode;
    return RT_ERR_OK;
}

int32 gpon_dev_macTableEntry_add(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry)
{
    uint32 i,idle = GPON_DEV_MAX_MACTBL_NUM,ret;
    rtk_mac_t mac;
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(obj->p_mac_tbl[i] && !osal_memcmp(obj->p_mac_tbl[i]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8)))
        {
            return RT_ERR_INPUT;
        }
        if(idle==GPON_DEV_MAX_MACTBL_NUM && !obj->p_mac_tbl[i])
        {
            idle = i;
        }
    }

    if(idle==GPON_DEV_MAX_MACTBL_NUM)   /* MAC table full */
    {
        return RT_ERR_GPON_MAC_FILTER_TABLE_FULL;
    }
    else
    {
        obj->p_mac_tbl[idle] = (gpon_macTable_obj_t*)GPON_OS_Malloc(sizeof(gpon_macTable_obj_t));
        if(obj->p_mac_tbl[idle] == NULL)
            return RT_ERR_GPON_MAC_FILTER_TABLE_ALLOC_FAIL;

        osal_memcpy(obj->p_mac_tbl[idle]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8));
        osal_memcpy(&mac.octet[0],&entry->mac_addr[0],ETHER_ADDR_LEN);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_set(idle,mac),ret);
        return RT_ERR_OK;
    }
}

int32 gpon_dev_macTableEntry_del(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry)
{
    uint32 i,ret;
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(obj->p_mac_tbl[i] && !osal_memcmp(obj->p_mac_tbl[i]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8)))
        {
            break;
        }
    }

    if(i==GPON_DEV_MAX_MACTBL_NUM)   /* the entry is not exist */
    {
        return RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND;
    }
    else
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_del(i),ret);
        GPON_OS_Free(obj->p_mac_tbl[i]);
        obj->p_mac_tbl[i] = NULL;
        return RT_ERR_OK;
    }
}

int32 gpon_dev_macTableEntry_get(gpon_dev_obj_t* obj, uint32 index, rtk_gpon_macTable_entry_t* entry)
{
    RT_PARAM_CHK((index >= GPON_DEV_MAX_MACTBL_NUM), RT_ERR_OUT_OF_RANGE);

    if(!obj->p_mac_tbl[index])
    {
        return RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND;
    }

    osal_memcpy(entry->mac_addr,obj->p_mac_tbl[index]->addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8));
    return RT_ERR_OK;
}

int32 gpon_dev_rdi_set(gpon_dev_obj_t* obj, int32 enable)
{
    int32 ret;
    RT_PARAM_CHK(((enable != 0)&&(enable != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->rdi!=enable)
    {
        obj->rdi = enable;
        GPON_UTIL_ERR_CHK(rtk_gpon_rdi_set(obj->rdi),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_rdi_get(gpon_dev_obj_t* obj, int32 *pEnable)
{
    *pEnable = obj->rdi;
    return RT_ERR_OK;
}

int32 gpon_dev_powerLevel_set(gpon_dev_obj_t* obj, uint8 level)
{
    RT_PARAM_CHK((level > GPON_DEV_MAX_TX_PWR_LEVEL), RT_ERR_OUT_OF_RANGE);
    if(obj->power_level!=level)
    {
        obj->power_level = level;
        gpon_ploam_snPloam_set(obj);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_powerLevel_get(gpon_dev_obj_t* obj, uint8 *pLevel)
{
    *pLevel = obj->power_level;
    return RT_ERR_OK;
}

int32 gpon_dev_auto_tcont_set(gpon_dev_obj_t *obj, int32 state)
{
    RT_PARAM_CHK((state > 1), RT_ERR_OUT_OF_RANGE);

    obj->auto_tcont_state = state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_tcont_get(gpon_dev_obj_t *obj, int32 *pState)
{
    *pState = obj->auto_tcont_state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_boh_set(gpon_dev_obj_t *obj, int32 state)
{
    RT_PARAM_CHK((state > 1), RT_ERR_OUT_OF_RANGE);

    obj->auto_boh_state = state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_boh_get(gpon_dev_obj_t *obj, int32 *pState)
{
    *pState = obj->auto_boh_state;

    return RT_ERR_OK;
}

int32 gpon_dev_eqd_offset_set(gpon_dev_obj_t *obj, int32 offset)
{
    obj->eqd_offset = offset;

    return RT_ERR_OK;
}

int32 gpon_dev_eqd_offset_get(gpon_dev_obj_t *obj, int32 *pOffset)
{
    *pOffset = obj->eqd_offset;

    return RT_ERR_OK;
}


int32 gpon_dev_berInterval_set(gpon_dev_obj_t* obj, uint32 interval)
{
    obj->ber_interval = interval;
    return RT_ERR_OK;
}

int32 gpon_dev_berInterval_get(gpon_dev_obj_t* obj, uint32 *interval)
{
    *interval = obj->ber_interval;
    return RT_ERR_OK;
}


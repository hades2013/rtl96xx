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
 *
 * Purpose : Definition those table or hsba dispaly format.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) hsb/hsa display
 *
 */



#ifndef __DIAG_DISPLAY_H__
#define __DIAG_DISPLAY_H__


static void _diag_debug_hsb_display(rtk_hsb_t *hsbData)
{
    diag_util_printf("spa: %d pktLen: %d ponIdx: %d\n",
                                hsbData->spa,
                                hsbData->pkt_len,
                                hsbData->pon_idx);

    diag_util_printf("da: %s ",
                        diag_util_inet_mactoa(&hsbData->da.octet[0]));
    diag_util_printf("sa: %s etherType: 0x%4.4x\n",
                        diag_util_inet_mactoa(&hsbData->sa.octet[0]),
                        hsbData->ether_type);

    diag_util_printf("ctag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->ctag_if,
                        (hsbData->ctag&0xE000)>>13,
                        (hsbData->ctag&0x1000)>>12,
                        hsbData->ctag&0xFFF);

    diag_util_printf("stag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->stag_if,
                        (hsbData->stag&0xE000)>>13,
                        (hsbData->stag&0x1000)>>12,
                        hsbData->stag&0xFFF);

    diag_util_printf("dip: %s ",
                        diag_util_inet_ntoa(hsbData->dip));
    diag_util_printf("sip: %s iptype: %d tos_dscp: 0x%2.2x\n",
                        diag_util_inet_ntoa(hsbData->sip),
                        hsbData->ip_type,
                        hsbData->tos_dscp);
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("l4ok l3ok gt1 gt5 gre icmp udp tcp\n");
            diag_util_printf("%-4d %-4d %-3d %-3d %-3d %-4d %-3d %-3d\n",
                                hsbData->cks_ok_l4,
                                hsbData->cks_ok_l3,
                                hsbData->ttl_gt1,
                                hsbData->ttl_gt5,
                                hsbData->gre_if,
                                hsbData->icmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("ip6hnrg l4ok l3ok gt1 gt5 gre icmp udp tcp\n");
            diag_util_printf("%-7d %-4d %-4d %-3d %-3d %-3d %-4d %-3d %-3d\n",
                                hsbData->ip6_nh_rg,
                                hsbData->cks_ok_l4,
                                hsbData->cks_ok_l3,
                                hsbData->ttl_gt1,
                                hsbData->ttl_gt5,
                                hsbData->gre_if,
                                hsbData->icmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if);
            break;
#endif
    }

    diag_util_printf("ptp oam rlpp rldp llc snap pppoe session\n");
    diag_util_printf("%-3d %-3d %-4d %-4d %-3d %-4d %-5d 0x%4.4x\n",
                        hsbData->ptp_if,
                        hsbData->omapdu,
                        hsbData->rlpp_if,
                        hsbData->rldp_if,
                        hsbData->llc_other,
                        hsbData->snap_if,
                        hsbData->pppoe_if,
                        hsbData->pppoe_session);



    diag_util_printf("userfield valid: 0x%4.4x\n",hsbData->user_valid);
    diag_util_printf("00-07: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                        hsbData->user_field_0,
                        hsbData->user_field_1,
                        hsbData->user_field_2,
                        hsbData->user_field_3,
                        hsbData->user_field_4,
                        hsbData->user_field_5,
                        hsbData->user_field_6,
                        hsbData->user_field_7);
    diag_util_printf("08-15: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                        hsbData->user_field_8,
                        hsbData->user_field_9,
                        hsbData->user_field_10,
                        hsbData->user_field_11,
                        hsbData->user_field_12,
                        hsbData->user_field_13,
                        hsbData->user_field_14,
                        hsbData->user_field_15);

    if(hsbData->cputag_if)
    {
        diag_util_printf("aware cpu-tag fields\n");
        diag_util_printf("l3c l4c txpmsk efidEn efid priSel pri keep vsel dislrn\n");
        diag_util_printf("%-3d %-3d 0x%2.2x   %-6d %-4d %-6d %-3d %-4d %-4d %d\n",
                        hsbData->cputag_l3c,
                        hsbData->cputag_l4c,
                        hsbData->cputag_txpmsk,
                        hsbData->cputag_efid_en,
                        hsbData->cputag_efid,
                        hsbData->cputag_prisel,
                        hsbData->cputag_pri,
                        hsbData->cputag_keep,
                        hsbData->cputag_vsel,
                        hsbData->cputag_dislrn);

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                diag_util_printf("psel extspa pppoe_act pppoe_idx l2br pon_sid dsl_vcmsk\n");
                diag_util_printf("%-4d %-6d %-9d %-9d %-4d %-7d 0x%4.4x\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_extspa,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid,
                                hsbData->cputag_dsl_vcmsk);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("psel l34keep extspa pppoe_act pppoe_idx l2br pon_sid\n");
                diag_util_printf("%-4d %-7d %-6d %-9d %-9d %-4d %-7d\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_l34keep,
                                hsbData->cputag_extspa,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid);
                break;
#endif
        }


    }
}

static void _diag_debug_hsa_display(rtk_hsa_t *hsaData)
{
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("Port      CPU  5    4    PON  2    1    0\n");
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("Port      CPU  5    PON  3    2    1    0\n");
            break;
#endif
    }



    diag_util_printf("user_pri: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsam_user_pri>>18)&7,
                                (hsaData->rng_nhsam_user_pri>>15)&7,
                                (hsaData->rng_nhsam_user_pri>>12)&7,
                                (hsaData->rng_nhsam_user_pri>>9)&7,
                                (hsaData->rng_nhsam_user_pri>>6)&7,
                                (hsaData->rng_nhsam_user_pri>>3)&7,
                                hsaData->rng_nhsam_user_pri&7);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("qid:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>22)&7,
                                        (hsaData->rng_nhsab_qid>>19)&7,
                                        (hsaData->rng_nhsab_qid>>16)&7,
                                        (hsaData->rng_nhsab_qid>>9)&0x7F,
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("qid:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>22)&7,
                                        (hsaData->rng_nhsab_qid>>19)&7,
                                        (hsaData->rng_nhsab_qid>>12)&0x7F,
                                        (hsaData->rng_nhsab_qid>>9)&7,
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);
            break;
#endif
    }


    diag_util_printf("dmp:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsab_dpm>>12)&3,
                                (hsaData->rng_nhsab_dpm>>10)&3,
                                (hsaData->rng_nhsab_dpm>>8)&3,
                                (hsaData->rng_nhsab_dpm>>6)&3,
                                (hsaData->rng_nhsab_dpm>>4)&3,
                                (hsaData->rng_nhsab_dpm>>2)&3,
                                hsaData->rng_nhsab_dpm&3);

    diag_util_printf("untagset: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsac_untagset>>6)&1,
                                (hsaData->rng_nhsac_untagset>>5)&1,
                                (hsaData->rng_nhsac_untagset>>4)&1,
                                (hsaData->rng_nhsac_untagset>>3)&1,
                                (hsaData->rng_nhsac_untagset>>2)&1,
                                (hsaData->rng_nhsac_untagset>>1)&1,
                                hsaData->rng_nhsac_untagset&1);

    diag_util_printf("spa ctag_act tag_if vid cfi pri vidzero\n");
    diag_util_printf("%-3d %-8d %-6d %-3d %-3d %-3d %-7d\n",
                                hsaData->rng_nhsab_spa,
                                hsaData->rng_nhsac_ctag_act,
                                hsaData->rng_nhsac_ctag_if,
                                hsaData->rng_nhsac_vid,
                                hsaData->rng_nhsac_cfi,
                                hsaData->rng_nhsac_pri,
                                hsaData->rng_nhsac_vidzero);


    diag_util_printf("stag_type stag_if sp2s svid svidx dei spri pkt_spri vidsel frctag frctag_if\n");
    diag_util_printf("%-9d %-7d %-4d %-4d %-5d %-3d %-4d %-8d %-6d %-6d %-9d\n",
                                hsaData->rng_nhsas_stag_type,
                                hsaData->rng_nhsas_stag_if,
                                hsaData->rng_nhsas_sp2s,
                                hsaData->rng_nhsas_svid,
                                hsaData->rng_nhsas_svidx,
                                hsaData->rng_nhsas_dei,
                                hsaData->rng_nhsas_spri,
                                hsaData->rng_nhsas_pkt_spri,
                                hsaData->rng_nhsas_vidsel,
                                hsaData->rng_nhsas_frctag,
                                hsaData->rng_nhsas_frctag_if);
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("1p_rem 1p_rem_en dscp_rem dscp_rem_en keep ptp ipv4 ipv6 1042 pppoe\n");
            diag_util_printf("%-6d %-9d %-8d %-11d %-4d %-3d %-4d %-4d %-4d %-5d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_ptp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("1prem 1premen dscprem dscpremen cpukeep keep ptp tcp udp ipv4 ipv6 1042 pppoe\n");
            diag_util_printf("%-5d %-7d %-7d %-9d %-7d %-4d %-3d %-3d %-3d %-4d %-4d %-4d %-5d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en,
                                        hsaData->rng_hhsaf_cpukeep,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_ptp,
                                        hsaData->rng_nhsaf_tcp,
                                        hsaData->rng_nhsaf_udp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if);

            break;
#endif
    }



    diag_util_printf("endsc  bgdsc  cpupri fwdrsn pon_sid pktlen regen_crc\n");
    diag_util_printf("0x%-4.4x 0x%-4.4x %-6d %-6d %-7d %-6d %-d\n",
                                hsaData->rng_nhsab_endsc,
                                hsaData->rng_nhsab_bgdsc,
                                hsaData->rng_nhsab_cpupri,
                                hsaData->rng_nhsab_fwdrsn,
                                hsaData->rng_nhsab_pon_sid,
                                hsaData->rng_nhsab_pktlen,
                                hsaData->rng_nhsaf_regen_crc);


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("vc_spa: %d vc_mask: 0x%4.4x ext_mask: 0x%4.4x\n",
                                        hsaData->rng_nhsab_vc_spa,
                                        hsaData->rng_nhsab_vc_mask,
                                        hsaData->rng_nhsab_ext_mask);

            diag_util_printf("l3: %d org: %d l2trans: %d l34trans: %d src_mode: %d l3chsum: 0x%4.4x l4schsum 0x%4.4x\n",
                                        hsaData->rng_nhsan_l3,
                                        hsaData->rng_nhsan_org,
                                        hsaData->rng_nhsan_l2trans,
                                        hsaData->rng_nhsan_l34trans,
                                        hsaData->rng_nhsan_src_mode,
                                        hsaData->rng_nhsan_l3chsum,
                                        hsaData->rng_nhsan_l4chsum);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("ext_mask l3 org ipmc l2trans l34trans src_mode l3chsum l4schsum\n");
            diag_util_printf("0x%4.4x   %-2d %-3d %-4d %-7d %-8d %-8d 0x%4.4x  0x%4.4x\n",
                                        hsaData->rng_nhsab_ext_mask,
                                        hsaData->rng_nhsan_l3,
                                        hsaData->rng_nhsan_org,
                                        hsaData->rng_nhsan_ipmc,
                                        hsaData->rng_nhsan_l2trans,
                                        hsaData->rng_nhsan_l34trans,
                                        hsaData->rng_nhsan_src_mode,
                                        hsaData->rng_nhsan_l3chsum,
                                        hsaData->rng_nhsan_l4chsum);

            break;
#endif
    }


    diag_util_printf("pppoe_idx: %d pppoe_act: %d ttl_extmask: 0x%2.2x ttl_pmask: 0x%2.2x\n",
                                hsaData->rng_nhsan_pppoe_idx,
                                hsaData->rng_nhsan_pppoe_act,
                                hsaData->rng_nhsan_ttlm1_extmask,
                                hsaData->rng_nhsan_ttlm1_pmask);

    diag_util_printf("newmac: %s smac_idx: %d ",
                        diag_util_inet_mactoa(&hsaData->rng_nhsan_newmac.octet[0]),
                        hsaData->rng_nhsan_smac_idx);
    diag_util_printf("newip: %s newport: 0x%4.4x\n",
                        diag_util_inet_ntoa(hsaData->rng_nhsan_newip),
                        hsaData->rng_nhsan_newport);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("floodpkt: %d ctag_ponac: %d ponvid: %d ponpri: %d ponsvid: %d ponspir: %d\n",
                                        hsaData->rng_nhsab_floodpkt,
                                        hsaData->rng_nhsac_ctag_ponact,
                                        hsaData->rng_nhsac_ponvid,
                                        hsaData->rng_nhsac_ponpri,
                                        hsaData->rng_nhsas_ponsvid,
                                        hsaData->rng_nhsas_ponspri);

            diag_util_printf("Port        CPU  5    PON  3    2    1    0\n");
    		diag_util_printf("un-Stagset: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsas_untagset>>6)&1,
                                (hsaData->rng_nhsas_untagset>>5)&1,
                                (hsaData->rng_nhsas_untagset>>4)&1,
                                (hsaData->rng_nhsas_untagset>>3)&1,
                                (hsaData->rng_nhsas_untagset>>2)&1,
                                (hsaData->rng_nhsas_untagset>>1)&1,
                                hsaData->rng_nhsas_untagset&1);

            break;
#endif
    }
	
}

static void _diag_debug_hsd_display(rtk_hsa_debug_t *hsdData)
{
    diag_util_printf("newmac: %s ",
                        diag_util_inet_mactoa(&hsdData->hsa_debug_newmac.octet[0]));

    diag_util_printf("l34mac: %s ",
                        diag_util_inet_mactoa(&hsdData->hsa_debug_34smac.octet[0]));

    diag_util_printf("newip: %s newprt: %d\n",
                        diag_util_inet_ntoa(hsdData->hsa_debug_newip),hsdData->hsa_debug_newprt);

    diag_util_printf("ep dsl_vc 34pppoe ttlpmsk ttlexmsk l4cksum l3cksum pppoeact\n");
    diag_util_printf("%-2d %-6d %-7d 0x%-5.5x 0x%-6.6x %-7d %-7d %-8d\n",
                        hsdData->hsa_debug_ep,
                        hsdData->hsa_debug_dsl_vc,
                        hsdData->hsa_debug_34pppoe,
                        hsdData->hsa_debug_ttlpmsk,
                        hsdData->hsa_debug_ttlexmsk,
                        hsdData->hsa_debug_l4cksum,
                        hsdData->hsa_debug_l3cksum,
                        hsdData->hsa_debug_pppoeact);

    diag_util_printf("src_mod l34trans l2trans org l3r sv_dei styp pktlen_ori qid\n");
    diag_util_printf("%-7d %-8d %-7d %-3d %-3d %-6d %-4d %-10d %-3d\n",
                        hsdData->hsa_debug_src_mod,
                        hsdData->hsa_debug_l34trans,
                        hsdData->hsa_debug_l2trans,
                        hsdData->hsa_debug_org,
                        hsdData->hsa_debug_l3r,
                        hsdData->hsa_debug_sv_dei,
                        hsdData->hsa_debug_styp,
                        hsdData->hsa_debug_pktlen_ori,
                        hsdData->hsa_debug_qid);
    diag_util_printf("stdsc cpupri spri cori cmdy crms cins cvid cfi regencrc pppoe\n");
    diag_util_printf("%-5d %-6d %-4d %-4d %-4d %-4d %-4d %-4d %-3d %-8d %-5d\n",
                        hsdData->hsa_debug_stdsc,
                        hsdData->hsa_debug_cpupri,
                        hsdData->hsa_debug_spri,
                        hsdData->hsa_debug_cori,
                        hsdData->hsa_debug_cmdy,
                        hsdData->hsa_debug_crms,
                        hsdData->hsa_debug_cins,
                        hsdData->hsa_debug_cvid,
                        hsdData->hsa_debug_cfi,
                        hsdData->hsa_debug_regencrc,
                        hsdData->hsa_debug_pppoe);
    diag_util_printf("rfc1042 ipv6 ipv4 ptp remdscp_pri rem1q_pri remdscp_en rem1q_en\n");
    diag_util_printf("%-7d %-4d %-4d %-3d %-11d %-9d %-10d %-8d\n",
                        hsdData->hsa_debug_rfc1042,
                        hsdData->hsa_debug_ipv6,
                        hsdData->hsa_debug_ipv4,
                        hsdData->hsa_debug_ptp,
                        hsdData->hsa_debug_remdscp_pri,
                        hsdData->hsa_debug_rem1q_pri,
                        hsdData->hsa_debug_remdscp_en,
                        hsdData->hsa_debug_rem1q_en);
    diag_util_printf("svid instag inctag pktlen spa dpc extmsk vcmsk ponsid trprsn\n");
    diag_util_printf("%-4d %-6d %-6d %-6d %-3d %-3d 0x%-4.4x 0x%-3.3x %-6d %-6d\n",
                        hsdData->hsa_debug_svid,
                        hsdData->hsa_debug_instag,
                        hsdData->hsa_debug_inctag,
                        hsdData->hsa_debug_pktlen,
                        hsdData->hsa_debug_spa,
                        hsdData->hsa_debug_dpc,
                        hsdData->hsa_debug_extmsk,
                        hsdData->hsa_debug_vcmsk,
                        hsdData->hsa_debug_ponsid,
                        hsdData->hsa_debug_trprsn);
}


#endif /* end of __DIAG_DISPLAY_H__ */

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
 * $Revision: 40837 $
 * $Date: 2013-07-08 16:29:59 +0800 (Mon, 08 Jul 2013) $
 *
 * Purpose : GMac Driver OMCI Processor
 *
 * Feature : GMac Driver OMCI Processor
 *
 */

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <re8686.h>

#endif

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_debug.h>
#include <module/gpon/gpon.h>



int32 gpon_omci_tx(gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t* omci)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    rtk_gpon_omci_msg_t *tmp_omci;
    struct sk_buff *skb;
  	int32 length = sizeof(rtk_gpon_omci_msg_t);
#endif
    int32 ret=0;
    rtk_port_t ponPort;

    rtk_gpon_port_get(&ponPort);

    if(obj->us_omci_flow!=GPON_DEV_MAX_FLOW_NUM)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"Send OMCI [port %d]: %02x%02x %02x %02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->us_omci_flow,omci->msg[0],omci->msg[1],omci->msg[2],omci->msg[3],omci->msg[4],omci->msg[5],omci->msg[6],omci->msg[7],obj->base_addr);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[8],omci->msg[9],omci->msg[10],omci->msg[11],omci->msg[12],omci->msg[13],omci->msg[14],omci->msg[15],
            omci->msg[16],omci->msg[17],omci->msg[18],omci->msg[19],omci->msg[20],omci->msg[21],omci->msg[22],omci->msg[23]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[24],omci->msg[25],omci->msg[26],omci->msg[27],omci->msg[28],omci->msg[29],omci->msg[30],omci->msg[31],
            omci->msg[32],omci->msg[33],omci->msg[34],omci->msg[35],omci->msg[36],omci->msg[37],omci->msg[38],omci->msg[39]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x %02x%02x %02x%02x%02x%02x ",
            omci->msg[40],omci->msg[41],omci->msg[42],omci->msg[43],omci->msg[44],omci->msg[45],omci->msg[46],omci->msg[47] );

#if defined(CONFIG_SDK_KERNEL_LINUX)
    	skb = dev_alloc_skb(length);
    	if (!skb)
    		return RT_ERR_FAILED;

    	tmp_omci = (rtk_gpon_omci_msg_t *)skb_put(skb, length);
        osal_memcpy(tmp_omci->msg, omci->msg, RTK_GPON_OMCI_MSG_LEN);

      {
          struct tx_info ptxInfo;
          osal_memset(&ptxInfo, 0, sizeof(struct tx_info));

          ptxInfo.opts1.bit.cputag_psel = 1;
          ptxInfo.opts1.bit.dislrn = 1;
          ptxInfo.opts2.bit.cputag  = 1;
          ptxInfo.opts2.bit.efid  = 1;
          ptxInfo.opts2.bit.enhance_fid  = 0;
          ptxInfo.opts3.bit.tx_portmask  = (1 << ponPort);
          ptxInfo.opts3.bit.tx_dst_stream_id  = GPON_OMCI_FLOW_ID;

          ret = re8686_send_with_txInfo(skb, &ptxInfo, 0);
      }
#endif
        if(0 == ret)
        {
            obj->cnt_cpu_omci_tx++;
            obj->cnt_cpu_omci_tx_nor++;
        }
        else
        {
        }
        return ret;
    }
    else
    {
    	osal_printf("flow_id=%d\n",obj->us_omci_flow);
        return RT_ERR_INPUT;
    }
}

int32 gpon_omci_rx(gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t *omci)
{
    if(obj->status!=RTK_GPONMAC_FSM_STATE_O5 && obj->status!=RTK_GPONMAC_FSM_STATE_O6)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Rcv OMCI in state %s", gpon_dbg_fsm_status_str(obj->status));
        return RT_ERR_OUT_OF_RANGE;
    }
    else
    {
        obj->cnt_cpu_omci_rx++;
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"Rcv OMCI: %02x%02x %02x %02x %02x%02x%02x%02x ",
            omci->msg[0],omci->msg[1],omci->msg[2],omci->msg[3],omci->msg[4],omci->msg[5],omci->msg[6],omci->msg[7]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[8],omci->msg[9],omci->msg[10],omci->msg[11],omci->msg[12],omci->msg[13],omci->msg[14],omci->msg[15],
            omci->msg[16],omci->msg[17],omci->msg[18],omci->msg[19],omci->msg[20],omci->msg[21],omci->msg[22],omci->msg[23]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[24],omci->msg[25],omci->msg[26],omci->msg[27],omci->msg[28],omci->msg[29],omci->msg[30],omci->msg[31],
            omci->msg[32],omci->msg[33],omci->msg[34],omci->msg[35],omci->msg[36],omci->msg[37],omci->msg[38],omci->msg[39]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x%02x%02x%02x",
            omci->msg[40],omci->msg[41],omci->msg[42],omci->msg[43]);

        if(obj->omci_callback)
        {
            (*obj->omci_callback)(omci);
        }
    }
    return RT_ERR_OK;
}

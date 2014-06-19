/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_dma1.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 02, 2009
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_dma1.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include <hal.h>
/* begin added by jiangmingli for tw_bug 2066 */
#include "rstp_bpdu.h"
#include "rstp_in.h"
/* end added by jiangmingli for tw_bug 2066 */
#include <opl_host.h>
#include <unistd.h>
#include "odm_port.h"

extern void wait_for_host_test_interrupt(void);
extern int get_ipmux_el_rx_buf_data(char *pbuf, int* plen);
extern int set_ipmux_el_tx_buf_data(char *appbuf, int len);

extern void eopl_dump(const void *pkt, UINT16 len, const UINT8 *pCaption);

#define HAL_HOST_DMA1_DUMP(addr, len) 	eopl_dump(addr, len, " Host Interface:")


static VOS_THREAD_t g_pstHostDma1ThreadId = NULL;

char g_acDma1RecvBuf[1600];
int g_acDma1RecvLen;

char g_acDma1DispatchBuf[1600];
int g_acDma1DispatchLen;

extern UINT8 customerTpid[2];

extern void odmStpRcvBpdu(unsigned char ucPortid, unsigned short usVlanid, void *pkt, unsigned short len);
extern int odmMulticastIgmpMsgHandler_MultiPort(
  UINT8 portNum,UINT16 vlan,UINT8 withTag,UINT8 *payLoad,UINT16 len, uint8 *raw_pkt);

static void halDma1Thread(void)
{
    int i;
    char stpMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
    char igmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
	UINT32 portNum;
	UINT32 pktType;
	UINT32 withTag;
	UINT32 vlan = 0;
	UINT32 hdrLen = 0;
    UINT8 *payLoad = NULL;
    int index;

    while (1)
    {
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
        syscall(NR_TEST_WAITINTERRUPT);
        while (0 != syscall(NR_TEST_GETDATA, &g_acDma1RecvBuf[0], &g_acDma1RecvLen) &&
            0 < g_acDma1RecvLen)
        {
            /* begin added by jiangmingli for host debug */
            if (OPL_TRUE == g_bHostUsDbgEn)
            {
                //opl_dump_data(g_acDma1RecvBuf,g_acDma1RecvLen,16);
                printf("\r\nRcv Pkt from atheros, len=%u\r\n",g_acDma1RecvLen);
                for(index = 0; index < g_acDma1RecvLen; index++)
                {  			
                    if((index != 0) && (index%16 == 0))
                    { 
                        printf("\r\n  %02x ",(UINT8)g_acDma1RecvBuf[index]);
                    }
                    else 
                    {  
                        printf("%02x ",(UINT8)g_acDma1RecvBuf[index]); 
                    }  
                }  
                printf("\r\n\r\n"); 
                /*printf("%s,%d\n",__FUNCTION__,__LINE__);*/
            }
            /* end added by jiangmingli for host debug */
    
            /* get DA SA */
            ATHEROS_HEADER_FRAME_t *pInHeader = (ATHEROS_HEADER_FRAME_t *)g_acDma1RecvBuf;

            portNum = pInHeader->portNum;
            payLoad = pInHeader->payLoad;
            hdrLen = 12;
            withTag = FALSE;

            /* begin added by jiangmingli for tw_bug 2066 */
            if (0 == OPL_MEMCMP(pInHeader->sa, rstp_dev_mac, 6))
            {
				abPortLoop[portNum-1] = 1;
            }
            /* end added by jiangmingli for tw_bug 2066 */

            if (payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
            {
                withTag = TRUE;
                vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
                hdrLen = 12+4;
            }

            if (!vosMemCmp(&g_acDma1RecvBuf[0], stpMac, 6))
            {
                pktType = STP_PKT_TYPE;
                g_acDma1DispatchLen = g_acDma1RecvLen - 2;
                if (withTag)
                {
                    odmStpRcvBpdu((UINT8)portNum, (UINT16)vlan, (void *)&payLoad[4], 
                        (UINT16)(g_acDma1DispatchLen - hdrLen - 2));
                }
                else
                {
                    odmStpRcvBpdu((UINT8)portNum, (UINT16)vlan, (void *)payLoad, 
                        (UINT16)(g_acDma1DispatchLen - hdrLen - 2));
                }
            }
            else if (!vosMemCmp(&g_acDma1RecvBuf[0], igmpMac, 3))
            {
			    pktType = IGMP_PKT_TYPE;

                /* add inbound header for hacking AR8306/AR8228 to sync 88E6045/88E6046/88E6097 */
            	host_inbound_hdr_t *pIgmpInHeader = (host_inbound_hdr_t *)&g_acDma1DispatchBuf[0];
                pIgmpInHeader->iport = portNum;
                pIgmpInHeader->reserved = 0;

                vosMemCpy(&g_acDma1DispatchBuf[4], &g_acDma1RecvBuf[0], 12);
                vosMemCpy(&g_acDma1DispatchBuf[16], &g_acDma1RecvBuf[14], g_acDma1RecvLen - 14);
                g_acDma1DispatchLen = g_acDma1RecvLen - 2;

                if (withTag)
                {
                    odmMulticastIgmpMsgHandler_MultiPort(
                        (UINT8)portNum, (UINT16)vlan, (UINT8)withTag, &payLoad[4], 
                        (UINT16)(g_acDma1DispatchLen - hdrLen -4), (UINT8 *)g_acDma1DispatchBuf);
                }
                else {
                    odmMulticastIgmpMsgHandler_MultiPort(
                        (UINT8)portNum, (UINT16)vlan, (UINT8)withTag, payLoad, 
                        (UINT16)(g_acDma1DispatchLen - hdrLen -4), (UINT8 *)g_acDma1DispatchBuf);
                }
            }
#if defined(ONU_4PORT_AR8228)
			else {
				UINT32 vlanMode = ODM_VLAN_TRANSPARENT;
				odmPortVlanModeGet(portNum, &vlanMode);
				if (ODM_VLAN_AGGREGATION == vlanMode){
					//printf("N:1 vlan %d.\n", vlan);
					//odmVlanAggLearnAction(portNum, vlan, pInHeader->sa);
				}
				
			}
#endif
            /* HAL_HOST_DMA1_DUMP(g_acDma1RecvBuf, g_acDma1RecvLen); */
        }
#else
        vosSleep(1);
#endif
    }
}

STATUS halDma1ThreadInit(void)
{
    int ret;

    if (g_pstHostDma1ThreadId) {
        return ERROR;
    }

    g_pstHostDma1ThreadId = vosThreadCreate("tHostDma1", OP_VOS_THREAD_STKSZ, 50,
                        (void *)halDma1Thread, (void *)NULL);
    if (g_pstHostDma1ThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS halDma1ThreadShutdown(void)
{
    int ret;

    if (!g_pstHostDma1ThreadId) {
        return ERROR;
    }

    if (g_pstHostDma1ThreadId != NULL) {
        vosThreadDestroy(g_pstHostDma1ThreadId);
    }

    return OK;
}

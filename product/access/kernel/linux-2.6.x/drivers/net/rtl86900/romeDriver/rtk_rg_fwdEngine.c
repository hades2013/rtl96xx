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
*/



/*
 * Include Files
 */

#ifdef CONFIG_APOLLO_ROMEDRIVER

#define CONFIG_ROME_NAPT_SHORTCUT

#ifdef CONFIG_APOLLO_MODEL
#else
#include <linux/slab.h> //for kmalloc/kfree
#endif

#ifdef __KERNEL__

#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in.h>


#ifdef CONFIG_RTL8686NIC
#include <re8686.h>
extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
//extern int re8670_start_xmit (struct sk_buff *skb, struct net_device *dev);
extern struct net_device *eth_net_dev;
#endif

#endif

#ifdef CONFIG_APOLLO_MODEL

#include "utility.h"
#include "rtl_types.h"
#include <rtl_glue.h>
#include <rtl_utils.h>
//#include <re8686.h>
#include <time.h>		//for time in fragment queuing
#endif

#include <common/error.h>
#include <rtk_rg_fwdEngine.h>
#include <rtk_rg_liteRomeDriver.h>
#include <rtk_rg_internal.h>
#include <rtk_rg_define.h>
#include <rtk_rg_struct.h>
#include <rtk_rg_igmpsnooping.h>
#include <rtk/init.h>
#include <rtk/l34.h>

#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
#include <rtk/intr.h>		//for get and clear link-down indicator register
#endif

#ifdef CONFIG_RG_DEBUG
#include <rtk_rg_debug.h>
#endif

#if 1
//#ifdef CONFIG_APOLLO_RLE0371
#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17
#define _RTK_RG_CHM _rtk_rg_standard_chksum

#if defined(CONFIG_DUALBAND_CONCURRENT)
static rtk_mac_t master_ipc_macAddr;
#endif

static u16
_rtk_rg_standard_chksum(u8 *dataptr, u16 len)
{
  u32 acc;
  u16 src;
  u8 *octetptr;

  acc = 0;

  octetptr = (u8*)dataptr;
  while (len > 1)
  {

    src = (*octetptr) << 8;
    octetptr++;
    src |= (*octetptr);
    octetptr++;
    acc += src;
    len -= 2;
  }
  if (len > 0)
  {
    src = (*octetptr) << 8;
    acc += src;
  }

  acc = (acc >> 16) + (acc & 0x0000ffffUL);
  if ((acc & 0xffff0000) != 0) {
    acc = (acc >> 16) + (acc & 0x0000ffffUL);
  }
  return htons((u16)acc);
}

u16 inet_chksum(u8 *dataptr, u16 len)
{
  u32 acc;

  acc = _RTK_RG_CHM(dataptr, len);
  while (acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return (u16)~(acc & 0xffff);
}

u16 inet_chksum_pseudo(u8 *tcphdr, u16 tcplen,
       u32 srcip, u32 destip,  u8 proto)
{
	u32 acc;
	u8 swapped;

	acc = 0;
	swapped = 0;
	/* iterate through all pbuf in chain */

	acc += _RTK_RG_CHM(tcphdr, tcplen);
	while (acc >> 16) 
	{
		acc = (acc & 0xffffUL) + (acc >> 16);
	}
	if (tcplen % 2 != 0) 
	{
		swapped = 1 - swapped;
		acc = ((acc & 0xff) << 8) | ((acc & 0xff00UL) >> 8);
	}

	if (swapped) 
	{
		acc = ((acc & 0xff) << 8) | ((acc & 0xff00UL) >> 8);
	}

	acc += (srcip & 0xffffUL);
	acc += ((srcip >> 16) & 0xffffUL);
	acc += (destip & 0xffffUL);
	acc += ((destip>> 16) & 0xffffUL);
	acc += (u32)htons((u16)proto);
	acc += (u32)htons(tcplen);

	while (acc >> 16) 
	{
		acc = (acc & 0xffffUL) + (acc >> 16);
	}
	return (u16)~(acc & 0xffffUL);

}
#endif

/*void _rtk_rg_fwdEngineTxDescSetting(void *pTxInfoPtr,void *ptxPtr,void *ptxMaskPtr)
{
	rtk_rg_txdesc_t *pTxInfo=(rtk_rg_txdesc_t *)pTxInfoPtr;
	rtk_rg_txdesc_t *ptx=(rtk_rg_txdesc_t *)ptxPtr;
	rtk_rg_txdesc_t *ptxMask=(rtk_rg_txdesc_t *)ptxMaskPtr;	

	if(ptxMask == NULL)
	{
		pTxInfo->opts1.bit.ipcs = ptx->opts1.bit.ipcs;
		pTxInfo->opts1.bit.l4cs = ptx->opts1.bit.l4cs;
		pTxInfo->opts1.bit.cputag_ipcs = ptx->opts1.bit.cputag_ipcs;
		pTxInfo->opts1.bit.cputag_l4cs = ptx->opts1.bit.cputag_l4cs;
		pTxInfo->opts2.dw = ptx->opts2.dw;
		pTxInfo->opts3.dw = ptx->opts3.dw;
		pTxInfo->opts4.dw = ptx->opts4.dw;
	}
	else
	{
		if(ptxMask->opts1.dw)
		{
			pTxInfo->opts1.dw &= (~ptxMask->opts1.dw);
			pTxInfo->opts1.dw |= (ptx->opts1.dw & ptxMask->opts1.dw);
		}
		if(ptxMask->opts2.dw)
		{
			pTxInfo->opts2.dw &= (~ptxMask->opts2.dw);
			pTxInfo->opts2.dw |= (ptx->opts2.dw & ptxMask->opts2.dw);
		}
		if(ptxMask->opts3.dw)
		{
			pTxInfo->opts3.dw &= (~ptxMask->opts3.dw);
			pTxInfo->opts3.dw |= (ptx->opts3.dw & ptxMask->opts3.dw);
		}
		if(ptxMask->opts4.dw)
		{
			pTxInfo->opts4.dw &= (~ptxMask->opts4.dw);
			pTxInfo->opts4.dw |= (ptx->opts4.dw & ptxMask->opts4.dw);
		}
	}
}*/

int _rtk_rg_packetParser(u8 *pData, u32 len, rtk_rg_pktHdr_t *pPktHdr)
{

    int off;
    u8 protocol=0;
	rtk_rg_pptpMsgHead_t *pPPTPHdr;
	unsigned int pptpMagic;
	
	pPktHdr->tagif=0;
	pPktHdr->ctagVid=0;
	pPktHdr->httpFirstPacket=0;
	pPktHdr->bindLookUpFinished=0;

	pPktHdr->pTos = NULL;//avoid access with not IP packet .
	pPktHdr->pIpv4TTL = NULL;//avoid access with not IP packet .
	pPktHdr->pIpv6Sip = NULL;//avoid access with not IPv6 packet .
	pPktHdr->pIpv6Dip = NULL;//avoid access with not IPv6 packet .

    if(len==0)
    {
        return FAIL;
    }	


	pPktHdr->pDmac=&pData[0]; //DA
	pPktHdr->pSmac=&pData[6]; //SA
    off=12;

#if 0 //cpu tag will parse by GMAC	
    if((pData[off]==0x88)&&(pData[off+1]==0x99)) //CPU TAG
    {

        if(((pData[off+8]==0x88)&&(pData[off+9]==0xa8))||((pData[off+8]==0x81)&&(pData[off+9]==0x00))||((pData[off+8]==0x88)&&((pData[off+9]==0x63)||(pData[off+9]==0x64)))||
                ((pData[off+8]==0x86)&&(pData[off+9]==0xdd))||((pData[off]==0x08)&&(pData[off+1]==0x00)))
        {
            //TO CPU
            off+=8;
        }
        else
        {
            //FROM CPU
            off+=12;
        }
    }
#endif	

#if 0 //CVLAN/SVLAN Tag is removed by GMAC, so it will parsed by _rtk_rg_internalVlanDecision(). the value should comes from CPUTag.
	if((*(u16*)(pData+off))==htons(0x88a8))//STAG
    //if((pData[off]==0x88)&&(pData[off+1]==0xa8)) //STAG
    {
		pPktHdr->tagif|=SVLAN_TAGIF;    
    	pPktHdr->pSVlanTag=&pData[off];
		pPktHdr->stagPri=pData[off+2]>>5;
		pPktHdr->stagDei=(pData[off+2]>>4)&1;
		pPktHdr->stagVid=((pData[off+2]&0xf)<<8)|(pData[off+3]);
        off+=4;
    }

	if((*(u16*)(pData+off))==htons(0x8100))//CTAG
    //if((pData[off]==0x81)&&(pData[off+1]==0x00)) //CTAG
    {
		pPktHdr->tagif|=CVLAN_TAGIF;        
    	pPktHdr->pSVlanTag=&pData[off];		
		pPktHdr->ctagPri=pData[off+2]>>5;
		pPktHdr->ctagCfi=(pData[off+2]>>4)&1;
		pPktHdr->ctagVid=((pData[off+2]&0xf)<<8)|(pData[off+3]);
        off+=4;
    }
#endif


	pPktHdr->etherType=ntohs(*(u16 *)&pData[off]);
	if(((*(u16*)(pData+off))==htons(0x8863))||((*(u16*)(pData+off))==htons(0x8864)))//PPPoE
    //if((pData[off]==0x88)&&((pData[off+1]==0x63)||(pData[off+1]==0x64))) //PPPoE
    {
   		pPktHdr->tagif|=PPPOE_TAGIF;    	
		pPktHdr->sessionId=ntohs(*(u16*)&pData[off+4]);
        off+=8;
    }

	if(((*(u16*)(pData+off))==htons(0x0800))||((*(u16*)(pData+off))==htons(0x0021)))//IPv4 or IPv4 with PPPoE
    //if(((pData[off]==0x08)&&(pData[off+1]==0x00))||((pData[off]==0x00)&&(pData[off+1]==0x21))) //IPv4 or IPv4 with PPPoE
    {
#if 0	
        rtlglue_printf("IPv4:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "HLen" COLOR_NM "=%d][" COLOR_Y "TOS" COLOR_NM "=%d(DSCP=%d)][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "ID" COLOR_NM "=%d][" COLOR_Y "R" COLOR_NM "=%d," COLOR_Y "DF" COLOR_NM "=%d," COLOR_Y "MF" COLOR_NM "=%d]\n"
                       ,pData[off+2]>>4,(pData[off+2]&0xf)*4,pData[off+3],pData[off+3]>>2,(pData[off+4]<<8)|pData[off+5],(pData[off+6]<<8)|pData[off+7]
                       ,(pData[off+8]>>7)&1,(pData[off+8]>>6)&1,(pData[off+8]>>5)&1);
        rtlglue_printf("     [" COLOR_Y "FrgOff" COLOR_NM "=%d][" COLOR_Y "TTL" COLOR_NM "=%d][" COLOR_Y "PROTO" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n"
                       ,((pData[off+8]&0x1f)<<8)|pData[off+9],pData[off+10],pData[off+11],(pData[off+12]<<8)|pData[off+13]);
#endif
		pPktHdr->l3Offset=off+2;

#ifdef CONFIG_APOLLO_RLE0371
		
		if(pPktHdr->tagif&PPPOE_TAGIF)
		{			
			if(pData[off+2]==0x44) 
			{
				//tysu: patch for TTL-1 bug in pppoe frame - overwrite the ip header length
				pData[off+2]=0x45;

				//tysu: patch for IP CHECKSUM bug in pppoe frame - overwrite the total length field
				pData[off+4]=(len-pPktHdr->l3Offset)>>8;
				pData[off+5]=(len-pPktHdr->l3Offset)&0xff;
			}
		}
#endif
		//pPktHdr->tos = pData[off+3];
		pPktHdr->pTos = (u8*)&pData[off+3];
		if((pData[off+8]>>5)&1) 
			pPktHdr->ipv4MoreFragment=1; //MF=1
		else 
			pPktHdr->ipv4MoreFragment=0;
		pPktHdr->ipv4FragmentOffset=((pData[off+8]&0x1f)<<8)|pData[off+9];	//Fragment Offset
		if((pPktHdr->ipv4MoreFragment!=0)||(pPktHdr->ipv4FragmentOffset!=0)) 
			pPktHdr->ipv4FragPacket=1;
		else
			pPktHdr->ipv4FragPacket=0;
		
		pPktHdr->l3Len=(pData[off+4]<<8)|pData[off+5];
		pPktHdr->pL3Len=(u16*)&pData[off+4];

		//if(pPktHdr->ipv4FragPacket) 
			pPktHdr->pIpv4Identification=(u16*)&pData[off+6];
		pPktHdr->pIpv4TTL=(u8*)&pData[off+10];
		pPktHdr->tagif|=IPV4_TAGIF;
		pPktHdr->ipv4Sip=ntohl(*(u32*)&pData[off+14]);
		pPktHdr->ipv4Dip=ntohl(*(u32*)&pData[off+18]);
		pPktHdr->pIpv4Sip=(u32*)&pData[off+14];
		pPktHdr->pIpv4Dip=(u32*)&pData[off+18];		
		pPktHdr->pIpv4Checksum=(u16*)&pData[off+12];

        protocol=pData[off+11];
        off+=(pData[off+2]&0xf)*4+2;
    }

	else if(((*(u16*)(pData+off))==htons(0x86dd))||((*(u16*)(pData+off))==htons(0x0057)))//IPv6 or IPv6 with PPPoE
    //else if(((pData[off]==0x86)&&(pData[off+1]==0xdd)) || ((pData[off]==0x00)&&(pData[off+1]==0x57)))		//IPv6 or IPv6 with PPPoE
    {
#if 0		
        rtlglue_printf("IPv6:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "TC" COLOR_NM "=%02x][" COLOR_Y "FL" COLOR_NM "=%02x%02x%x][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "HopLimit" COLOR_NM "=%d]\n"
                       ,pData[off+2]>>4, (pData[off+2]&0xf)+(pData[off+3]>>4), (pData[off+3]&0xf)+(pData[off+4]>>4), (pData[off+4]&0xf)+(pData[off+5]>>4), (pData[off+5]&0xf), pData[off+6]+pData[off+7], pData[off+8], pData[off+9]);
#endif

		pPktHdr->tagif|=IPV6_TAGIF;

		pPktHdr->l3Offset=off+2;
		pPktHdr->pTos=(u8*)&pData[off+2];		//IPv6's traffic class is between version and flow label
		pPktHdr->ipv6PayloadLen=(pData[off+6]<<8)|pData[off+7];
		pPktHdr->pIPv6HopLimit=&pData[off+9];
		pPktHdr->pIpv6Sip=&pData[off+10];
		pPktHdr->pIpv6Dip=&pData[off+26];

        protocol=pData[off+8];
		pPktHdr->l3Len=42;		//header length plus ether type
        if(protocol==0)	//hop-by-hop
        {
#if 0        
            rtlglue_printf("Hop-By-Hop:[" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "Length" COLOR_NM "=%d]\n"
                           ,pData[off+42], pData[off+43]);
            rtlglue_printf("          [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x]\n"
                           ,pData[off+44], pData[off+45], pData[off+46], pData[off+47], pData[off+48], pData[off+49]);
            for(i=0; i<pData[off+43]; i++)
            {
                rtlglue_printf("         [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x %02x %02x]\n"
                               ,pData[off+50+i*8], pData[off+51+i*8], pData[off+52+i*8], pData[off+53+i*8]
                               ,pData[off+54+i*8], pData[off+55+i*8], pData[off+56+i*8], pData[off+57+i*8]);
            }
#endif			

            protocol=pData[off+42];
			pPktHdr->l3Len=(50+pData[off+43]*8);
        }
		
		off+=pPktHdr->l3Len;
    }
	else if((*(u16*)(pData+off))==htons(0x0806)) //ARP
	//else if((pData[off]==0x08)&&(pData[off+1]==0x06))
	{
		pPktHdr->ipv4Sip=ntohl(*(u32*)&pData[off+16]);
		pPktHdr->ipv4Dip=ntohl(*(u32*)&pData[off+26]);
		pPktHdr->arpOpCode=ntohs(*(u16*)&pData[off+8]);
		pPktHdr->pIpv4Sip=(u32*)&pData[off+16];
		pPktHdr->pIpv4Dip=(u32*)&pData[off+26];	
	}

	pPktHdr->ipProtocol=protocol;

    if(protocol==0x6) //TCP
    {
    	//dump_packet(pData,len,"par");
#if 0    
        rtlglue_printf("TCP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Seq" COLOR_NM "=0x%x][" COLOR_Y "Ack" COLOR_NM "=0x%x][" COLOR_Y "HLen" COLOR_NM "=%d]\n"
                       ,(pData[off]<<8)|(pData[off+1]),(pData[off+2]<<8)|(pData[off+3]),(pData[off+4]<<24)|(pData[off+5]<<16)|(pData[off+6]<<8)|(pData[off+7]<<0)
                       ,(pData[off+8]<<24)|(pData[off+9]<<16)|(pData[off+10]<<8)|(pData[off+11]<<0),pData[off+12]>>4<<2);
        rtlglue_printf("    [" COLOR_Y "URG" COLOR_NM "=%d][" COLOR_Y "ACK" COLOR_NM "=%d][" COLOR_Y "PSH" COLOR_NM "=%d][" COLOR_Y "RST" COLOR_NM "=%d][" COLOR_Y "SYN" COLOR_NM "=%d][" COLOR_Y "FIN" COLOR_NM "=%d][" COLOR_Y "Win" COLOR_NM "=%d]\n"
                       ,(pData[off+13]>>5)&1,(pData[off+13]>>4)&1,(pData[off+13]>>3)&1,(pData[off+13]>>2)&1,(pData[off+13]>>1)&1,(pData[off+13]>>0)&1
                       ,(pData[off+14]<<8)|pData[off+15]);
        rtlglue_printf("    [" COLOR_Y "CHM" COLOR_NM "=0x%x][" COLOR_Y "Urg" COLOR_NM "=0x%x]\n",(pData[off+16]<<8)|(pData[off+17]<<0),(pData[off+18]<<8)|(pData[off+19]<<0));
#endif
		pPktHdr->l4Offset=off;
		pPktHdr->tagif|=TCP_TAGIF;		
		pPktHdr->sport=ntohs(*(u16*)&pData[off]);
		pPktHdr->dport=ntohs(*(u16*)&pData[off+2]);
		pPktHdr->tcpSeq=ntohl(*(u32*)&pData[off+4]);
		pPktHdr->tcpAck=ntohl(*(u32*)&pData[off+8]);		
		pPktHdr->pTcpSeq=(u32*)&pData[off+4];
		pPktHdr->pTcpAck=(u32*)&pData[off+8];
		*((u8*)(&pPktHdr->tcpFlags))=pData[off+13];
		pPktHdr->pSport=(u16*)&pData[off];
		pPktHdr->pDport=(u16*)&pData[off+2];
		pPktHdr->headerLen=pData[off+12]>>4<<2;
		pPktHdr->pL4Checksum=(u16*)&pData[off+16];
		//DEBUG("sip=%x dip=%x sport=0x%x dport=0x%x ip_protocol=%d syn=%d ack=%d fin=%d rst=%d\n",pPktHdr->ipv4Sip,pPktHdr->ipv4Dip,pPktHdr->sport,pPktHdr->dport,pPktHdr->ipProtocol,pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);			
		off+=(pData[off+12]>>4)<<2;

		//check HTTP request first packet & assign payloadbuff
		if(rg_db.systemGlobal.urlFilter_valid_entry[0]!=-1)
		{		
			if(off+2<=len)
			{
				if((pData[off]=='G')&&(pData[off+1]=='E')){
					pPktHdr->httpFirstPacket=1;
					pPktHdr->pL4Payload=&pData[off];
				}
				else if((pData[off]=='P')&&(pData[off+1]=='O')){
					pPktHdr->httpFirstPacket=1;
					pPktHdr->pL4Payload=&pData[off];
				}
			}			
		}

		//Check for PPTP packet
		//Verify data length, if the length is unable to analyze outgoing-call-request or outgoing-call-reply, this should be invalid PPTP control packet
		if (off + sizeof(rtk_rg_pptpMsgHead_t) + sizeof(rtk_rg_pptpCallIds_t) <= len)	//pptpMsgHead and pptpCallIds are just enough to get session ID and Peer's call ID
		{
			// Move up to PPTP message header
			pPPTPHdr = (rtk_rg_pptpMsgHead_t *) (((u8 *) pData) + off);

			// Save the control message type
			pPktHdr->pptpCtrlType = ntohs(pPPTPHdr->type);
			
			// Verify PPTP Control Message  
			pptpMagic = ntohl(pPPTPHdr->magic);
			if ((ntohs(pPPTPHdr->msgType) == PPTP_CTRL_MSG_TYPE) && (pptpMagic == PPTP_MAGIC))
			{
				// When packet is reply type, must have result code and error code to decide whether ID field is valid
				if ((pPktHdr->pptpCtrlType == PPTP_OutCallReply || pPktHdr->pptpCtrlType == PPTP_InCallReply))
				{
					// Verify data length:
					if((off + sizeof(rtk_rg_pptpMsgHead_t) + sizeof(rtk_rg_pptpCallIds_t) + sizeof(rtk_rg_pptpCodes_t)) <= len)
					{
						pPktHdr->tagif|=PPTP_TAGIF;
						pPktHdr->pPptpCallId = (rtk_rg_pptpCallIds_t *) (pPPTPHdr + 1);
						pPktHdr->pptpCodes = *(rtk_rg_pptpCodes_t *) (pPPTPHdr + 2);
						//DEBUG("PPTP CTRL TYPE is %d, CID1 is %04x, CID2 is %04x, resCode is %d, errCode is %d",pPktHdr->pptpCtrlType,pPktHdr->pPptpCallId->cid1,pPktHdr->pPptpCallId->cid2,
							//pPktHdr->pptpCodes.resCode,pPktHdr->pptpCodes.errCode);
					}
				}
				else
				{
					pPktHdr->tagif|=PPTP_TAGIF;
					pPktHdr->pPptpCallId = (rtk_rg_pptpCallIds_t *) (pPPTPHdr + 1);
					//DEBUG("PPTP CTRL TYPE is %d, CID1 is %04x, CID2 is %04x",pPktHdr->pptpCtrlType,pPktHdr->pPptpCallId->cid1,pPktHdr->pPptpCallId->cid2);
				}
			}
		}
    }
    else if(protocol==0x11) //UDP
    {
#if 0    
        rtlglue_printf("UDP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n",(pData[off]<<8)|(pData[off+1]),(pData[off+2]<<8)|(pData[off+3])
                       ,(pData[off+4]<<8)|(pData[off+5]),(pData[off+6]<<8)|(pData[off+7]));
#endif
		pPktHdr->l4Offset=off;		
		pPktHdr->tagif|=UDP_TAGIF;
		pPktHdr->sport=ntohs(*(u16*)&pData[off]);
		pPktHdr->dport=ntohs(*(u16*)&pData[off+2]);
		pPktHdr->pSport=(u16*)&pData[off];
		pPktHdr->pDport=(u16*)&pData[off+2];
		pPktHdr->pL4Checksum=(u16*)&pData[off+6];
		off+=8;
    }
	else if(protocol==0x1) //ICMP
	{
		pPktHdr->tagif|=ICMP_TAGIF;
		pPktHdr->l4Offset=off;		
		pPktHdr->ICMPType=(ntohs(*(u16*)&pData[off])&0xff00)>>8;
		pPktHdr->ICMPCode=ntohs(*(u16*)&pData[off])&0xff;
		pPktHdr->ICMPIdentifier=ntohs(*(u16*)&pData[off+4]);
		pPktHdr->ICMPOverMTU=0;
	}
	else if(protocol==0x2) //IGMP
	{
		pPktHdr->tagif|=IGMP_TAGIF;
	}
	else if(protocol==0x3a)	//ICMPv6
	{
		pPktHdr->tagif|=ICMPV6_TAGIF;
		pPktHdr->l4Offset=off;		
		pPktHdr->ICMPv6Type=(ntohs(*(u16*)&pData[off])&0xff00)>>8;
		if(pPktHdr->ICMPv6Type==0x88)	//Neighbor Advertisement
			pPktHdr->ICMPv6Flag=(ntohl(*(u32*)&pData[off+4])&0xf0000000)>>28;
	}
	else if(protocol==0x2f)	//GRE
	{
		pPktHdr->tagif|=GRE_TAGIF;
		//DEBUG("flags and version %x, protocol type is %x, len is %x, callID is %x, sequence is %x",
			//ntohs(*(u16*)&pData[off]),ntohs(*(u16*)&pData[off+2]),ntohs(*(u16*)&pData[off+4]),ntohs(*(u16*)&pData[off+6]),ntohs(*(u16*)&pData[off+8]));
	}
	
	return SUCCESS;
}

int _rtk_rg_algCheckEnable(unsigned char isTCP, unsigned short checkPort)
{
	int algIdx;
	unsigned int algBitValue;

	algIdx=checkPort>>5;
	algBitValue=0x1<<(checkPort&0x1f);

	//DEBUG("the algIdx is %d, TCPPortEnable is %x, algbitvalue is %x",
		//algIdx,rg_db.algTcpExternPortEnabled[algIdx],algBitValue);
	
	if(isTCP)
	{
		if(rg_db.algTcpExternPortEnabled[algIdx]&algBitValue)
		{
			//DEBUG("the TCP port %d has enabled ALG!!",checkPort);
			return SUCCESS;
		}
	}
	else
	{
		if(rg_db.algUdpExternPortEnabled[algIdx]&algBitValue)
		{
			//DEBUG("the UDP port %d is ALG enabled!!",checkPort);
			return SUCCESS;
		}
	}

	return FAIL;
}

int _rtk_rg_algSrvInLanCheckEnable(unsigned char isTCP, unsigned short checkPort)
{
	int algIdx;
	unsigned int algBitValue;

	algIdx=checkPort>>5;
	algBitValue=0x1<<(checkPort&0x1f);

	//DEBUG("the algIdx is %d, TCPPortEnable is %x, algbitvalue is %x",
		//algIdx,rg_db.algTcpExternPortEnabled[algIdx],algBitValue);
	
	if(isTCP)
	{
		if(rg_db.algTcpExternPortEnabled_SrvInLan[algIdx]&algBitValue)
		{
			//DEBUG("the TCP port %d has enabled ALG when Server In LAN!!",checkPort);
			return SUCCESS;
		}
	}
	else
	{
		if(rg_db.algUdpExternPortEnabled_SrvInLan[algIdx]&algBitValue)
		{
			//DEBUG("the UDP port %d has enabled ALG when Server In LAN!!",checkPort);
			return SUCCESS;
		}
	}

	return FAIL;
}

int _rtk_rg_algFunctionCheck(int direct, rtk_rg_pktHdr_t *pPktHdr)
{
	int i,checkPort,checkPort_SrvInLan;
	unsigned int checkSetting;

	if(direct == NAPT_DIRECTION_OUTBOUND)
	{
		checkPort_SrvInLan=pPktHdr->sport;
		checkPort=pPktHdr->dport;
	}
	else
	{
		checkPort_SrvInLan=pPktHdr->dport;
		checkPort=pPktHdr->sport;
	}

	//**** If the ALG register function is NULL, and the ALG is turn on, we will trap this packet to protocol stack directly.

	//DEBUG("%s packet, the mask is %x, check port is %d",pPktHdr->tagif&TCP_TAGIF?"TCP":"UDP",rg_db.algFunctionMask,checkPort);

	//Check ServerInWAN and Passthrough first, then check ServerInLAN:
	if(pPktHdr->tagif&TCP_TAGIF)
	{				
		if(_rtk_rg_algCheckEnable(1,checkPort)==SUCCESS)
		{
			//DEBUG("Port %d enable!!",checkPort);
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algTcpFunctionMapping[i].portNum==checkPort))
				{
					//DEBUG("TCP before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algTcpFunctionMapping[i].portNum,	rg_db.algTcpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
					if(rg_db.algTcpFunctionMapping[i].registerFunction!=NULL)
					{
						pPktHdr->algAction=RG_ALG_ACT_TO_FWDENGINE;
						pPktHdr->algFunctionMappingIdx=i;
						return RG_FWDENGINE_RET_CONTINUE;
					}
					else
						return RG_FWDENGINE_RET_TO_PS;
				}
				else
				{
					//check next
					checkSetting>>=1;
					i++;
				}
			}	
		}
		if(_rtk_rg_algSrvInLanCheckEnable(1,checkPort_SrvInLan)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{				
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algTcpFunctionMapping[i].portNum==checkPort_SrvInLan))
				{
					//DEBUG("TCP SrvInLAN before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algTcpFunctionMapping[i].portNum,	rg_db.algTcpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
					if(rg_db.algTcpFunctionMapping[i].registerFunction!=NULL)
					{
						pPktHdr->algAction=RG_ALG_ACT_TO_FWDENGINE;
						pPktHdr->algFunctionMappingIdx=i;
						return RG_FWDENGINE_RET_CONTINUE;
					}
					else
						return RG_FWDENGINE_RET_TO_PS;
				}
				else
				{
					//check next
					checkSetting>>=1;
					i++;
				}
			}	
		}
	}
	else if(pPktHdr->tagif&UDP_TAGIF)
	{
		if(_rtk_rg_algCheckEnable(0,checkPort)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algUdpFunctionMapping[i].portNum==checkPort))
				{
					//DEBUG("UDP before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algUdpFunctionMapping[i].portNum,rg_db.algUdpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
					if(rg_db.algUdpFunctionMapping[i].registerFunction!=NULL)
					{
						pPktHdr->algAction=RG_ALG_ACT_TO_FWDENGINE;
						pPktHdr->algFunctionMappingIdx=i;
						return RG_FWDENGINE_RET_CONTINUE;
					}
					else
						return RG_FWDENGINE_RET_TO_PS;
				}
				else
				{			
					//check next
					checkSetting>>=1;
					i++;
				}
			}
		}
		if(_rtk_rg_algSrvInLanCheckEnable(0,checkPort_SrvInLan)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algUdpFunctionMapping[i].portNum==checkPort_SrvInLan))
				{
					//DEBUG("UDP SrvInLAN before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algUdpFunctionMapping[i].portNum,rg_db.algUdpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
					if(rg_db.algUdpFunctionMapping[i].registerFunction!=NULL)
					{
						pPktHdr->algAction=RG_ALG_ACT_TO_FWDENGINE;
						pPktHdr->algFunctionMappingIdx=i;
						return RG_FWDENGINE_RET_CONTINUE;
					}
					else
						return RG_FWDENGINE_RET_TO_PS;
				}
				else
				{			
					//check next
					checkSetting>>=1;
					i++;
				}
			}
		}
	}

	//didn't hit any ALG port
	pPktHdr->algAction=RG_ALG_ACT_NORMAL;
	
	return RG_FWDENGINE_RET_CONTINUE;
}


int _rtk_rg_algForward(int direct, u8 after, struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	int ret=FAIL;
#if 0
	int i,checkPort,checkPort_SrvInLan;
	unsigned int checkSetting;

	if(direct == NAPT_DIRECTION_OUTBOUND)
	{
		checkPort_SrvInLan=pPktHdr->sport;
		checkPort=pPktHdr->dport;
	}
	else
	{
		checkPort_SrvInLan=pPktHdr->dport;
		checkPort=pPktHdr->sport;
	}
#endif
	//DEBUG("%s packet, the mask is %x, check port is %d",pPktHdr->tagif&TCP_TAGIF?"TCP":"UDP",checkSetting,checkPort);

	//Check ServerInWAN and Passthrough first, then check ServerInLAN:
	if(pPktHdr->tagif&TCP_TAGIF)
	{	
		if(pPktHdr->algAction==RG_ALG_ACT_TO_FWDENGINE)
		{
			//DEBUG("do the register function[%d]!%p",i,rg_db.algTcpFunctionMapping[i].registerFunction);
			ret = rg_db.algTcpFunctionMapping[pPktHdr->algFunctionMappingIdx].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
		}
			
#if 0
		if(_rtk_rg_algCheckEnable(1,checkPort)==SUCCESS)
		{
			//DEBUG("Port %d enable!!",checkPort);
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				//DEBUG("TCP before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algTcpFunctionMapping[i].portNum,	rg_db.algTcpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algTcpFunctionMapping[i].portNum==checkPort) && 
					(rg_db.algTcpFunctionMapping[i].registerFunction!=NULL))
				{
					//DEBUG("do the register function[%d]!%p",i,rg_db.algTcpFunctionMapping[i].registerFunction);
					ret = rg_db.algTcpFunctionMapping[i].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
					break;
				}
				else
				{
					//check next
					checkSetting>>=1;
					i++;
				}
			}	
		}
		if(_rtk_rg_algSrvInLanCheckEnable(1,checkPort_SrvInLan)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				//DEBUG("TCP SrvInLAN before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algTcpFunctionMapping[i].portNum,	rg_db.algTcpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algTcpFunctionMapping[i].portNum==checkPort_SrvInLan) && 
					(rg_db.algTcpFunctionMapping[i].registerFunction!=NULL))
				{
					//DEBUG("do the register function[%d]!%p",i,rg_db.algTcpFunctionMapping[i].registerFunction);
					ret = rg_db.algTcpFunctionMapping[i].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
					break;
				}
				else
				{
					//check next
					checkSetting>>=1;
					i++;
				}
			}	
		}
#endif
	}
	else if(pPktHdr->tagif&UDP_TAGIF)
	{
		if(pPktHdr->algAction==RG_ALG_ACT_TO_FWDENGINE)
		{
			//DEBUG("do the register function[%d]!%p",i,rg_db.algUdpFunctionMapping[i].registerFunction);
			ret = rg_db.algUdpFunctionMapping[pPktHdr->algFunctionMappingIdx].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
		}
#if 0
		if(_rtk_rg_algCheckEnable(0,checkPort)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				//DEBUG("UDP before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algUdpFunctionMapping[i].portNum,rg_db.algUdpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algUdpFunctionMapping[i].portNum==checkPort) && 
					(rg_db.algUdpFunctionMapping[i].registerFunction!=NULL))
				{
					ret = rg_db.algUdpFunctionMapping[i].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
					break;
				}
				else
				{			
					//check next
					checkSetting>>=1;
					i++;
				}
			}
		}
		if(_rtk_rg_algSrvInLanCheckEnable(0,checkPort_SrvInLan)==SUCCESS)
		{
			//do the matching alg hook function
			checkSetting=rg_db.algFunctionMask;
			i=0;
			while(checkSetting>0 || i<MAX_ALG_FUNCTIONS)
			{
				//DEBUG("UDP SrvInLAN before execute function...%d, port %d, isNULL?%s",(rg_db.algFunctionMask&(0x1<<i)),rg_db.algUdpFunctionMapping[i].portNum,rg_db.algUdpFunctionMapping[i].registerFunction!=NULL?"NO":"YES");
				if(((rg_db.algFunctionMask&(0x1<<i))>0) && 
					(rg_db.algUdpFunctionMapping[i].portNum==checkPort_SrvInLan) && 
					(rg_db.algUdpFunctionMapping[i].registerFunction!=NULL))
				{
					ret = rg_db.algUdpFunctionMapping[i].registerFunction(direct,after,(unsigned char *)skb,(unsigned char *)pPktHdr);
					break;
				}
				else
				{			
					//check next
					checkSetting>>=1;
					i++;
				}
			}
		}
#endif
	}
	
	return ret;
}

rtk_rg_pptpGreEntry_t *_rtk_rg_lookupPPTPOutboundGreFlow(int netifIdx, unsigned int internalIp,unsigned short internalCallID, unsigned int remoteIp)
{
	rtk_rg_pptpGreLinkList_t *pEntry=NULL;

	for (pEntry = rg_db.pPPTPGreOutboundHead[netifIdx]->pPrev; pEntry != rg_db.pPPTPGreOutboundHead[netifIdx]; pEntry = pEntry->pPrev)
	{
		if((pEntry->greEntry.valid==1) &&
			(pEntry->greEntry.internalIpAddr==internalIp)&&
			(pEntry->greEntry.internalCallID==internalCallID)&&
			(pEntry->greEntry.remoteIpAddr==remoteIp))
			break;
	}
	if(pEntry == rg_db.pPPTPGreOutboundHead[netifIdx])		//not found
		return NULL;

	return &pEntry->greEntry;
}

rtk_rg_pptpGreEntry_t *_rtk_rg_lookupPPTPOutboundGreFlowByCallID(int netifIdx, unsigned short externalCallID, unsigned short remoteCallID)
{
	rtk_rg_pptpGreLinkList_t *pEntry=NULL;
	
	for (pEntry = rg_db.pPPTPGreOutboundHead[netifIdx]->pPrev; pEntry != rg_db.pPPTPGreOutboundHead[netifIdx]; pEntry = pEntry->pPrev)
	{
		if(pEntry->greEntry.valid==1 && 
			((externalCallID>=0 && (pEntry->greEntry.externalCallID==externalCallID)) ||
			(remoteCallID>=0 && (pEntry->greEntry.remoteCallID==remoteCallID))))
			break;
	}
	if(pEntry == rg_db.pPPTPGreOutboundHead[netifIdx])		//not found
		return NULL;

	return &pEntry->greEntry;
}

int _rtk_rg_PPTPExtCallIDGetAndUse(uint16 wishCallID)
{
	int wishIdx;
	uint32 wishBitValue;
	int i;

	i=wishCallID;
	while(1)
	{
		wishIdx=i>>5; // =wishCallID/32
		wishBitValue=1<<(i&0x1f);
		
		if((rg_db.algPPTPExtCallIDEnabled[wishIdx]&wishBitValue)==0)
		{
			//DEBUG("callID %d is set!",i);
			rg_db.algPPTPExtCallIDEnabled[wishIdx]|=wishBitValue;
			return i;
		}
		
		i++;
		i&=0xffff;
		if(i==wishCallID) break;		
	}	
	return FAIL;
}

void _rtk_rg_PPTPExtCallIDFree(int callID)
{
	int idx;
	uint32 bitValue;
	int i;

	i=callID;
	idx=i>>5; // =callID/32
	bitValue=1<<(i&0x1f);
	
	if((rg_db.algPPTPExtCallIDEnabled[idx]&bitValue)>0)
		rg_db.algPPTPExtCallIDEnabled[idx]&=(~bitValue);
}

int _rtk_rg_PPTP_GREModify(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	if(((rg_db.algFunctionMask & RTK_RG_ALG_PPTP_TCP_PASSTHROUGH_BIT) > 0 &&
		rg_db.algTcpFunctionMapping[RTK_RG_ALG_PPTP_TCP_PASSTHROUGH].registerFunction!=NULL) ||
		((rg_db.algFunctionMask & RTK_RG_ALG_PPTP_UDP_PASSTHROUGH_BIT) > 0 &&
		rg_db.algUdpFunctionMapping[RTK_RG_ALG_PPTP_UDP_PASSTHROUGH].registerFunction!=NULL))
	{
		//Check DIP with each WAN's gateway IP
		
		//If match, lookup DIP from CallID in key(ExtCallID),
		//then lookup routing table for DMAC from ARP table
		//finally change CallID in key to IntCallID

		//If none match, lookup routing table from DIP,
		//replace SIP to routing WAN's gateway IP,
		//then lookup DMAC from ARP or NH (depend on routing type)


	
		return RG_FWDENGINE_RET_DIRECT_TX;
	}

	return RG_FWDENGINE_RET_TO_PS;
}

int _rtk_rg_algRegFunc_TCP_PPTP(int direct, int after, unsigned char *pSkb,unsigned char *pPktHdr)
{
//FIXME:here should be rewrite for model code
#ifdef __KERNEL__
	rtk_rg_pktHdr_t *pPktInfo;
	struct sk_buff *skb;
	rtk_rg_pptpGreEntry_t *pGreEntry;
	int usableCallID=-1;

	pPktInfo=(rtk_rg_pktHdr_t *)pPktHdr;
	skb=(struct sk_buff *)pSkb;

	if((pPktInfo->tagif&PPTP_TAGIF)==0)		//not PPTP packet
		return FAIL;
/*
	PPTP_StartCtrlConnRequest	= 1,
	PPTP_StartCtrlConnReply 	= 2,
	PPTP_StopCtrlConnRequest	= 3,
	PPTP_StopCtrlConnReply	= 4,
	PPTP_EchoRequest			= 5,
	PPTP_EchoReply			= 6,
	PPTP_OutCallRequest 		= 7,
	PPTP_OutCallReply			= 8,
	PPTP_InCallRequest		= 9,
	PPTP_InCallReply			= 10,
	PPTP_InCallConn 			= 11,
	PPTP_CallClearRequest		= 12,
	PPTP_CallDiscNotify 		= 13,
	PPTP_WanErrorNotify 		= 14,
	PPTP_SetLinkInfo			= 15
*/

	if(after==0)
	{
		//Pre function
		if(direct==NAPT_DIRECTION_OUTBOUND)
		{
			switch(pPktInfo->pptpCtrlType)
			{
				case PPTP_OutCallRequest:
					//find ExtCallID for use
					//Keep internalIpAddr and MAC
					//Keep internalCallID
					DEBUG("$$$$ PRE PPTP_OutCallRequest  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlow(pPktInfo->netifIdx,pPktInfo->ipv4Sip,pPktInfo->pPptpCallId->cid1,pPktInfo->ipv4Dip);
					if(pGreEntry==NULL)
					{
						usableCallID=_rtk_rg_PPTPExtCallIDGetAndUse(pPktInfo->pPptpCallId->cid1);
						if(usableCallID==FAIL)
						{
							DEBUG("PPTP_OutCallRequest: Thers is no usable CallID now...");
							return FAIL;
						}
						//Setup this entry
						pGreEntry=&rg_db.pPPTPGreOutboundHead[pPktInfo->netifIdx]->greEntry;

						bzero(pGreEntry,sizeof(rtk_rg_pptpGreEntry_t));
						pGreEntry->internalIpAddr=pPktInfo->ipv4Sip;
						memcpy(pGreEntry->internalMacAddr.octet,pPktInfo->pSmac,ETHER_ADDR_LEN);
						pGreEntry->internalCallID=pPktInfo->pPptpCallId->cid1;
						//Find out usable extCallID and keep it in greEntry
						pGreEntry->externalCallID=usableCallID;
						pGreEntry->valid=1;
						DEBUG("@@@the GreEntry has been set in PRE:internalIP=%08x, internalMac=%02x:%02x:%02x:%02x:%02x:%02x, internalCallID is %d, externalCallID is %d",
							pGreEntry->internalIpAddr,
							pGreEntry->internalMacAddr.octet[0],
							pGreEntry->internalMacAddr.octet[1],
							pGreEntry->internalMacAddr.octet[2],
							pGreEntry->internalMacAddr.octet[3],
							pGreEntry->internalMacAddr.octet[4],
							pGreEntry->internalMacAddr.octet[5],
							pGreEntry->internalCallID,pGreEntry->externalCallID);						

						rg_db.pPPTPGreOutboundHead[pPktInfo->netifIdx]=rg_db.pPPTPGreOutboundHead[pPktInfo->netifIdx]->pNext;
					}

					//Replace internal CallID to external CallID
					pPktInfo->pPptpCallId->cid1=pGreEntry->externalCallID;
					break;
				case PPTP_CallClearRequest:
					DEBUG("$$$$ PRE PPTP_CallClearRequest  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlow(pPktInfo->netifIdx,pPktInfo->ipv4Sip,pPktInfo->pPptpCallId->cid1,pPktInfo->ipv4Dip);
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_CallClearRequest: Error..we can not find the GRE entry ..");
						return FAIL;
					}
					
					//Replace internal CallID to external CallID
					pPktInfo->pPptpCallId->cid1=pGreEntry->externalCallID;
					break;
				default:
					break;
			}
			
		}
		else
		{
			switch(pPktInfo->pptpCtrlType)
			{
				case PPTP_OutCallReply:
					//Keep remoteCallID
					DEBUG("$$$$ PPTP_OutCallReply  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlowByCallID(pPktInfo->netifIdx,pPktInfo->pPptpCallId->cid2,-1);		//peer's CallID
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_OutCallReply: Error..we can not find the GRE entry from ExternalCallID %d..",pPktInfo->pPptpCallId->cid2);
						return FAIL;
					}
					pGreEntry->remoteCallID=pPktInfo->pPptpCallId->cid1;
					DEBUG("@@@ the GreEntry has been set in PRE:retmoteCallID is %d",pGreEntry->remoteCallID);
					//Replace externalCallID by internalCallID
					pPktInfo->pPptpCallId->cid2=pGreEntry->internalCallID;
					break;
				case PPTP_WanErrorNotify:
					//replace peer's CallID to internalCallID
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlowByCallID(pPktInfo->netifIdx,pPktInfo->pPptpCallId->cid1,-1);		//peer's CallID
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_WanErrorNotify: Error..we can not find the GRE entry from ExternalCallID %d..",pPktInfo->pPptpCallId->cid1);
						return FAIL;
					}
					//Replace externalCallID by internalCallID
					pPktInfo->pPptpCallId->cid1=pGreEntry->internalCallID;
					break;
				case PPTP_CallDiscNotify:
					//release GRE entry if we receive CallDisconnectNotify from WAN interface
					DEBUG("$$$$ PPTP_CallDiscNotify  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlowByCallID(pPktInfo->netifIdx,-1,pPktInfo->pPptpCallId->cid1);
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_CallDiscNotify: Error..we can not find the GRE entry from RemoteCallID %d..",pPktInfo->pPptpCallId->cid1);
						return FAIL;
					}
					pGreEntry->valid=0;
					break;
				default:
					break;
			}
		}
	}
	else
	{
		//Post function
		if(direct==NAPT_DIRECTION_OUTBOUND)
		{
			switch(pPktInfo->pptpCtrlType)
			{
				case PPTP_OutCallRequest:
					//use ExtCallID to find match entry
					DEBUG("$$$$ POST PPTP_OutCallRequest  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlowByCallID(pPktInfo->netifIdx,pPktInfo->pPptpCallId->cid1,-1);
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_OutCallRequest: Error..we can not find the GRE entry from ExternalCallID %d..",pPktInfo->pPptpCallId->cid1);
						return FAIL;
					}
					//Keep remoteIpAddr and MAC
					pGreEntry->remoteIpAddr=pPktInfo->ipv4Dip;
					memcpy(pGreEntry->remoteMacAddr.octet,pPktInfo->pDmac,ETHER_ADDR_LEN);
					DEBUG("@@@ the GreEntry has been set in PRE:remoteIP is %08x, remoteMac is %02x:%02x:%02x:%02x:%02x:%02x",
						pGreEntry->remoteIpAddr,pGreEntry->remoteMacAddr.octet[0],pGreEntry->remoteMacAddr.octet[1],pGreEntry->remoteMacAddr.octet[2],
						pGreEntry->remoteMacAddr.octet[3],pGreEntry->remoteMacAddr.octet[4],pGreEntry->remoteMacAddr.octet[5]);
					break;
				default:
					break;
			}
		}
		else
		{
			switch(pPktInfo->pptpCtrlType)
			{
				case PPTP_CallDiscNotify:
					//release GRE entry if we receive CallDisconnectNotify from LAN interface
					DEBUG("$$$$ PPTP_CallDiscNotify  pPktInfo->netifIdx is %d",pPktInfo->netifIdx);
					pGreEntry=_rtk_rg_lookupPPTPOutboundGreFlowByCallID(pPktInfo->netifIdx,-1,pPktInfo->pPptpCallId->cid1);
					if(pGreEntry==NULL)
					{
						DEBUG("PPTP_CallDiscNotify: Error..we can not find the GRE entry from RemoteCallID %d..",pPktInfo->pPptpCallId->cid1);
						return FAIL;
					}
					pGreEntry->valid=0;
					break;
				default:
					break;
			}
		}
	}

#endif
	return SUCCESS;
}

rtk_rg_ftpCtrlFlowEntry_t *_rtk_rg_lookupFTPCtrlFlow(unsigned int internalIp,unsigned short int internalPort, unsigned int remoteIp, unsigned short int remotePort)
{
	rtk_rg_ftpCtrlFlowEntry_t *pEntry=NULL;

	/*DEBUG("the input is %s, int %x,%d ret %x,%d",
		isTcp==1?"TCP":"UDP",
		internalIp,internalPort,
		remoteIp,remotePort);*/
	
	for (pEntry = rg_db.pAlgFTPCtrlFlowHead->pPrev; pEntry != rg_db.pAlgFTPCtrlFlowHead; pEntry = pEntry->pPrev)
	{
		/*DEBUG("the entry is %s, int %x,%d ret %x,%d",
			pEntry->isTcp==1?"TCP":"UDP",
			pEntry->internalIpAddr,pEntry->internalPort,
			pEntry->remoteIpAddr,pEntry->remotePort);*/
		if((pEntry->internalIpAddr==internalIp)&&
			(pEntry->internalPort==internalPort)&&
			(pEntry->remoteIpAddr==remoteIp)&&
			(pEntry->remotePort==remotePort))
			break;
	}
	if(pEntry == rg_db.pAlgFTPCtrlFlowHead)		//not found
		return NULL;

	return pEntry;
}

int _rtk_rg_algRegFunc_TCP_FTP(int direct, int after, unsigned char *pSkb,unsigned char *pPktHdr)
{
//FIXME:here should be rewrite for model code
#ifdef __KERNEL__
	char *startCP,*endCP,*pData;
	char portString[30]={0};
	unsigned int ipAddr,gwIPAddr,newLen,newDelta;
	unsigned short portNum;
	int dataLen=0,dataOff=0,ret,flowIdx;
	rtk_rg_naptEntry_t naptEntry;
	rtk_rg_pktHdr_t *pPktInfo;
	struct sk_buff *skb;
	rtk_rg_ftpCtrlFlowEntry_t *pFtpCtrlFlow;
	pPktInfo = (rtk_rg_pktHdr_t *)pPktHdr;
	skb= (struct sk_buff *)pSkb;

	pData=skb->data;
	
	dataOff = pPktInfo->l4Offset + pPktInfo->headerLen;
	//if(pPktInfo->tagif&PPPOE_TAGIF)dataOff+=8;		//shift for pppoe packet
	dataLen = skb->len - dataOff;
	
	//DEBUG("the l4offset is %d, headerlen is %d, skb->len is %d, dataLen is %d, dataoff is %d",pPktInfo->l4Offset,pPktInfo->headerLen,skb->len,dataLen,dataOff);

	if(after==0)
	{
		//Pre function
		if(direct==NAPT_DIRECTION_OUTBOUND)
		{
			//DEBUG("the data is %x, port is %x",*((unsigned int *)(pData+dataOff)),htonl(FTP_PORT_STR));

			//Check if we had add FTP-flow 5-tuple link list entry
			//if not, add one, set Delta to 0
			//check each list entry, if not add, add
			pFtpCtrlFlow = _rtk_rg_lookupFTPCtrlFlow(pPktInfo->ipv4Sip,pPktInfo->sport,pPktInfo->ipv4Dip,pPktInfo->dport);
			if(pFtpCtrlFlow == NULL)
			{
				//DEBUG("ADD NEW CTRL FLOW of FTP!!DIP is %x, DPORT is %d",pPktInfo->ipv4Dip,pPktInfo->dport);
				//add one
				rg_db.pAlgFTPCtrlFlowHead->remoteIpAddr=pPktInfo->ipv4Dip;
				rg_db.pAlgFTPCtrlFlowHead->internalIpAddr=pPktInfo->ipv4Sip;
				rg_db.pAlgFTPCtrlFlowHead->remotePort=pPktInfo->dport;
				rg_db.pAlgFTPCtrlFlowHead->internalPort=pPktInfo->sport;

				//move to next one
				rg_db.pAlgFTPCtrlFlowHead = rg_db.pAlgFTPCtrlFlowHead->pNext;
			}
			else if((pPktInfo->tcpFlags.syn==1)&&(pPktInfo->tcpFlags.ack==0))	//If the same Ctrl Flow send SYN packet, we need to reset Delta to 0
			{	
				//DEBUG("Got SYN at same ctrl-flow..reset Delta!");
				pFtpCtrlFlow->Delta=0;
			}
			else if(pFtpCtrlFlow->Delta > 0)
			{
				*pPktInfo->pTcpSeq=htonl(pPktInfo->tcpSeq + pFtpCtrlFlow->Delta);
				//DEBUG("pre outbound Delta is %d",pFtpCtrlFlow->Delta);
			}
		}
		else
		{
			//Do nothing
		}
	}
	else
	{
		//Post function
		if(direct==NAPT_DIRECTION_OUTBOUND)
		{
			pFtpCtrlFlow = _rtk_rg_lookupFTPCtrlFlow(pPktInfo->ipv4Sip,pPktInfo->sport,pPktInfo->ipv4Dip,pPktInfo->dport);
			if(dataLen > 0 && *((unsigned int *)(pData+dataOff)) == htonl(FTP_PORT_STR))
			{
				//DEBUG("len is %d, l4offset is %d, headerlen is %d",dataLen,pPktInfo->l4Offset, pPktInfo->headerLen);
				//DEBUG("POST_FUNCTION outbound: i am in _rtk_rg_algRegFunc_TCP_FTP!! data len is %d dport is %d",dataLen,pPktInfo->dport);
				//DEBUG("DATA is:");

				startCP=pData+dataOff+5;
				ipAddr = simple_strtoul(startCP,&endCP,0);

				ipAddr<<=8;
	
				startCP=endCP+1;
				ipAddr += simple_strtoul(startCP,&endCP,0);

				ipAddr<<=8;
	
				startCP=endCP+1;
				ipAddr += simple_strtoul(startCP,&endCP,0);

				ipAddr<<=8;
	
				startCP=endCP+1;
				ipAddr += simple_strtoul(startCP,&endCP,0);
				
				//DEBUG("the ipAddr is %x",ipAddr);
	
				startCP=endCP+1;
				portNum = simple_strtoul(startCP,&endCP,0);

				portNum<<=8;

				startCP=endCP+1;
				portNum += simple_strtoul(startCP,&endCP,0);

				//DEBUG("the port is %d",portNum);

				gwIPAddr=ntohl(*pPktInfo->pIpv4Sip);
				snprintf(portString,sizeof(portString),"PORT %d,%d,%d,%d,%d,%d\r\n",
					(gwIPAddr>>24)&0xff,
					(gwIPAddr>>16)&0xff,
					(gwIPAddr>>8)&0xff,
					gwIPAddr&0xff,
					portNum>>8,
					portNum&0xff);

				//DEBUG("the modified port command is \"%s\"",portString);
				//DEBUG("data before len is %d, after is %d",dataLen,strlen(portString));

				newLen=strlen(portString);
				newDelta=(newLen-dataLen);
				//DEBUG("the newDelta is %d",newDelta);
				
				if(skb->tail+newDelta > skb->end)	
				{
					//need to re-alloc skb data structure
					FIXME("Overflow! we need to re-alloc skb data stucture...");
				}
				else
				{
					//copy the new string into skb and enlarge the length
					memcpy(skb->data+dataOff,portString,newLen);

					//fix packet length
					//DEBUG("sip is %x, sport is %d",pPktInfo->ipv4Sip,pPktInfo->sport);					
					if(newDelta > 0)
					{
						skb->len += newDelta;
						*pPktInfo->pL3Len=htons(pPktInfo->l3Len + newDelta);
					}
				}

				pFtpCtrlFlow->Delta+=newDelta;
			
				//Add to napt connection here
				bzero(&naptEntry,sizeof(naptEntry));
				naptEntry.is_tcp=1;
				naptEntry.local_ip=pPktInfo->ipv4Sip;
				naptEntry.remote_ip=pPktInfo->ipv4Dip;
				naptEntry.local_port=portNum;
				naptEntry.remote_port=RTK_RG_ALG_FTP_DATA_TCP_PORT;
				naptEntry.external_port=portNum;
				naptEntry.wan_intf_idx=pPktInfo->extipIdx;
				ret = rtk_rg_naptConnection_add(&naptEntry,&flowIdx);
				if(ret!=RT_ERR_RG_OK && ret!=RT_ERR_RG_NAPT_FLOW_DUPLICATE)
					assert_ok(ret);
			}
		}
		else
		{
			pFtpCtrlFlow = _rtk_rg_lookupFTPCtrlFlow(*pPktInfo->pIpv4Dip,*pPktInfo->pDport,pPktInfo->ipv4Sip,pPktInfo->sport);

			if(pFtpCtrlFlow != NULL && pFtpCtrlFlow->Delta > 0)
			{
				//DEBUG("post inbound Delta is %d, old ack is %x",pFtpCtrlFlow->Delta,*pPktInfo->pTcpAck);
				*pPktInfo->pTcpAck=htonl(pPktInfo->tcpAck - pFtpCtrlFlow->Delta);
				//DEBUG("new ack is %x",*pPktInfo->pTcpAck);
			}
		}
	}
#endif
	return SUCCESS;
}

int _rtk_rg_arpAgent(u8 *pData, u32 len, rtk_rg_pktHdr_t *pPktHdr)
{
	int i,l2Idx;
		
	DEBUG("ARP Agent, arpOpCode=%d",pPktHdr->arpOpCode);
	if((pPktHdr->arpOpCode==2) || (pPktHdr->arpOpCode==1))		//check both ARP reply and request for complete rg add wan interface
	{
#ifdef CONFIG_RG_NAPT_ARP_AUTO_LEARN
		_rtk_rg_arpAndMacEntryAdd(pPktHdr->ipv4Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->ingressPort,NULL,0,pPktHdr->internalVlanID,0,1);
#endif

		for(i=0;i<MAX_ASIC_NETIF_TBL_SIZE;i++)
		{
			if(rg_db.systemGlobal.intfArpRequest[i].finished==0)
			{
				//rtlglue_printf("netif=%d ipv4Sip=%x reqIp=%x\n",i,pPktHdr->ipv4Sip,rg_db.systemGlobal.intfArpRequest[i].reqIp);
				if(pPktHdr->ipv4Sip==rg_db.systemGlobal.intfArpRequest[i].reqIp)				
				{
					_rtk_rg_arpAndMacEntryAdd(pPktHdr->ipv4Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->ingressPort,&l2Idx,0,pPktHdr->internalVlanID,0,1);
					//macEntry.vlan_id=RG_GLB_INTF_INFO[i].wan_intf.wan_intf_conf.egress_vlan_id;						
					if(rg_db.systemGlobal.intfArpRequest[i].gwMacReqCallBack!=NULL)
					{
						rg_db.systemGlobal.intfArpRequest[i].gwMacReqCallBack(rg_db.systemGlobal.intfArpRequest[i].reqIp,l2Idx);
					}
					rg_db.systemGlobal.intfArpRequest[i].finished=1;
					return SUCCESS;
				}
			}
		}
	}
	return FAIL;
}

int _rtk_rg_neighborAgent(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	DEBUG("Neighbor Agent, ICMPv6 type=%d",pPktHdr->ICMPv6Type);
	//Check for Neighbor advertisement packet
	if(pPktHdr->ICMPv6Type==0x88)
	{
		int neighborIdx=-1,i;

		//Check for validation
		if(*pPktHdr->pIPv6HopLimit!=255)	//hop limit should be 255, solicitation flag should be set
			return FAIL;
		
#ifdef CONFIG_RG_IPV6_NEIGHBOR_AUTO_LEARN
		//Learning source IP with source Link-layer address and keep in neighbor table
		_rtk_rg_neighborAndMacEntryAdd(pPktHdr->pIpv6Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->pRxDesc->opts3.bit.src_port_num,&neighborIdx,0);
#endif

		//Check for solicited Neighbor Advetisement packets for complete RG WAN interface addition
		if((pPktHdr->ICMPv6Flag&0x2)<=0)
			return FAIL;

		for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
		{
			if(rg_db.systemGlobal.intfNeighborDiscovery[i].finished==0)
			{
				/*DEBUG("the target address is");
				for(j=0;j<16;j++)
					rtlglue_printf("%02x ",skb->data[pPktHdr->l4Offset+8+j]);*/
				if(memcmp(&skb->data[pPktHdr->l4Offset+8],rg_db.systemGlobal.intfNeighborDiscovery[i].reqIp.ipv6_addr,IPV6_ADDR_LEN)==0)				
				{
					//FIXME: there may have other options in the future[rfc 4861]
					if(skb->data[pPktHdr->l4Offset+24]!=0x2)	//ICMPv6 option: Target link-layer address
						return FAIL;
					//DEBUG("MATCH!!%x%x%x%x%x%x",skb->data[pPktHdr->l4Offset+26],skb->data[pPktHdr->l4Offset+27],skb->data[pPktHdr->l4Offset+28],
					//skb->data[pPktHdr->l4Offset+29],skb->data[pPktHdr->l4Offset+30],skb->data[pPktHdr->l4Offset+31]);
					_rtk_rg_neighborAndMacEntryAdd(pPktHdr->pIpv6Sip,pPktHdr->sipL3Idx,&skb->data[pPktHdr->l4Offset+26],pPktHdr->pRxDesc->opts3.bit.src_port_num,&neighborIdx,0);
					//macEntry.vlan_id=RG_GLB_INTF_INFO[i].wan_intf.wan_intf_conf.egress_vlan_id;						
					if(rg_db.systemGlobal.intfNeighborDiscovery[i].ipv6GwMacReqCallBack!=NULL)
					{
						//DEBUG("wan intf is %d, function pointer is %p",i,rg_db.systemGlobal.intfNeighborDiscovery[i].ipv6GwMacReqCallBack);
						//DEBUG("rg_db.v6neighbor[%d].rtk_v6neighbor.l2Idx is %d",neighborIdx,rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.l2Idx);
						rg_db.systemGlobal.intfNeighborDiscovery[i].ipv6GwMacReqCallBack(rg_db.systemGlobal.intfNeighborDiscovery[i].reqIp.ipv6_addr,rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.l2Idx);
						//DEBUG("");
					}
					rg_db.systemGlobal.intfNeighborDiscovery[i].finished=1;
					return SUCCESS;
				}
			}
		}
	}
	return FAIL;
}


int _rtk_rg_portAndProtocolBasedVlanCheck(rtk_rg_pktHdr_t *pPktHdr, int *interVLANId)
{
	int groupID,port;
	rtk_vlan_protoVlanCfg_t *pVlanCfg;
	//Check ingress port(MAC only, no extension port)
	if(pPktHdr->ingressPort>=RTK_RG_MAC_PORT_CPU)
	{
		port=RTK_RG_MAC_PORT_CPU;
		//groupID=rg_db.systemGlobal.protoBasedVID[RTK_RG_MAC_PORT_CPU].protoGroupID;
		//pVlanCfg=&rg_db.systemGlobal.protoBasedVID[RTK_RG_MAC_PORT_CPU].protoVLANCfg;
	}
	else
	{
		port=pPktHdr->ingressPort;
		//groupID=rg_db.systemGlobal.protoBasedVID[pPktHdr->ingressPort].protoGroupID;
		//pVlanCfg=&rg_db.systemGlobal.protoBasedVID[pPktHdr->ingressPort].protoVLANCfg;
	}

	//if(pVlanCfg->valid==0)return FAIL;
	
	//Check protocol Group, if match, use the VLAN ID as the ingress ID
	for(groupID=0;groupID<MAX_PORT_PROTO_GROUP_SIZE;groupID++)
	{
		pVlanCfg=&rg_db.systemGlobal.protoBasedVID[port].protoVLANCfg[groupID];
		if(pVlanCfg->valid==0)
			continue;
		
		switch(rg_db.systemGlobal.protoGroup[groupID].frametype)
		{
			case FRAME_TYPE_ETHERNET:
				if(pPktHdr->etherType==rg_db.systemGlobal.protoGroup[groupID].framevalue)
				{
					DEBUG("Protocol Match!!using VLANID %d as ingress VLAN for untag packets!!",pVlanCfg->vid)
					*interVLANId=pVlanCfg->vid;
					return SUCCESS;
				}
				else
					DEBUG("Protocol unMatch...pkt=%04x, protoGroup=%04x...",pPktHdr->etherType,rg_db.systemGlobal.protoGroup[groupID].framevalue);
				break;
	    	case FRAME_TYPE_RFC1042:
	    	case FRAME_TYPE_LLCOTHER:
			default:
				FIXME("ProtoGroup has un-supported frametype %d, fail to check...",rg_db.systemGlobal.protoGroup[groupID].frametype);
				break;
		}
	}

	return FAIL;
	
}

int _rtk_rg_internalVlanDecision(rtk_rg_pktHdr_t *pPktHdr, int *interVLANId,struct sk_buff *skb)
{
	int ret,protoVLANID;
	//The decided internal VLAN ID will be returned by reference of interVLANId

	//FIXME:here should be check ACL before 1Q and other VLAN decision
	//uint8 cVlanPri;
	if(pPktHdr->pRxDesc->opts2.bit.ctagva==1)
	{
		//1Q-based VLAN
		*interVLANId=((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xf)<<0x8)+((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xff00)>>0x8);
		//DEBUG("1Q interVLANId=%d\n",*interVLANId);		
		//update for pPktHdr VLAN message, because gw removed vlan: chuck
		pPktHdr->ctagVid=*interVLANId;
		pPktHdr->ctagPri=((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xf0)>>5);
		pPktHdr->ctagCfi=((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0x10)>>4);
		
		return RT_ERR_RG_OK;
	}
	else
	{
		ret=_rtk_rg_portAndProtocolBasedVlanCheck(pPktHdr,&protoVLANID);
		if(ret==SUCCESS)
		{
			*interVLANId=protoVLANID;
			TRACE("Port-Proto-Based interVLANId=%d\n",*interVLANId);
		}
		else
		{
			//port-based VLAN
			*interVLANId=rg_db.systemGlobal.portBasedVID[pPktHdr->ingressPort];
			TRACE("Port-Based interVLANId=%d\n",*interVLANId);
		}
		
		return RT_ERR_RG_OK;
	}
}

rtk_rg_ipClassification_t _rtk_rg_sip_classification(ipaddr_t sip, rtk_rg_pktHdr_t *pPktHdr)
{
	int i;
	int hitNat=0;
	
	for(i=0;i<MAX_EXTIP_HW_TABLE_SIZE;i++)
	{
		if(rg_db.extip[i].rtk_extip.valid==1 && sip==rg_db.extip[i].rtk_extip.intIpAddr)
		{
			hitNat=1;
			break;
		}
	}
	
	pPktHdr->sipL3Idx=_rtk_rg_l3lookup(sip);
	if(rg_db.l3[pPktHdr->sipL3Idx].rtk_l3.internal==1)
	{	
		if(hitNat)
			return IP_CLASS_NI;
		else
			return IP_CLASS_NPI;
	}
	else
	{
		if(hitNat)
			return IP_CLASS_LP;
		else
			return IP_CLASS_RP;
	}
}


rtk_rg_ipClassification_t _rtk_rg_dip_classification(ipaddr_t dip, rtk_rg_pktHdr_t *pPktHdr)
{
	int i;
	int hitNat=0;
	int hitExtipIdx=0;
	
	for(i=0;i<MAX_EXTIP_HW_TABLE_SIZE;i++)
	{
		if(rg_db.extip[i].rtk_extip.valid==1  && dip==rg_db.extip[i].rtk_extip.extIpAddr)
		{
			hitNat=1;
			hitExtipIdx=i;
			break;
		}
	}

	if(hitNat)
	{
		switch(rg_db.extip[hitExtipIdx].rtk_extip.type)
		{
			case L34_EXTIP_TYPE_NAPT:
				return IP_CLASS_NPE;
			case L34_EXTIP_TYPE_NAT:
				return IP_CLASS_NE;
			case L34_EXTIP_TYPE_LP:
				return IP_CLASS_LP;
			default:
				return FAIL;
		}
	}
	else
	{
		pPktHdr->dipL3Idx=_rtk_rg_l3lookup(dip);
		if(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.internal==1)
		{
			if(rg_db.extip[hitExtipIdx].rtk_extip.type==L34_EXTIP_TYPE_NAPT)
				return IP_CLASS_NPI;
			else
				return IP_CLASS_NI;
		}
		else
			return IP_CLASS_RP;
	}
	return FAIL;
}

/* this function will lookup routing,netif,arp,nexthop,extip,l2 tables idx */
int _rtk_rg_routingDecisionTablesLookup(rtk_rg_pktHdr_t *pPktHdr, rtk_rg_sipDipClassification_t sipDipClass)
{
	rtk_rg_arp_linkList_t *pSoftwareArpEntry;
	//DEBUG("the class is %s",sipDipClass==SIP_DIP_CLASS_NAPT?"NAPT":"NAPTR or ROUTING");
	/* Only outbound packet will hit binding table. */
	if(((sipDipClass==SIP_DIP_CLASS_NAPT) || (sipDipClass==SIP_DIP_CLASS_ROUTING)) && (rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.rt2waninf==1))
	{
		//Port/VLAN Binding
		if(pPktHdr->bindLookUpFinished==0)
		{
			pPktHdr->bindNextHopIdx=_rtk_rg_portBindingLookup(pPktHdr->pRxDesc->opts3.bit.src_port_num,0,pPktHdr->internalVlanID,sipDipClass);
			pPktHdr->bindLookUpFinished=1;
			if(pPktHdr->bindNextHopIdx!=FAIL)
			{
				/* Rome driver should make sure that binding->nexthop is equal to napt->nexthop. */
				pPktHdr->nexthopIdx=pPktHdr->bindNextHopIdx; //for Port/VLAN Bidning Route SMAC/DMAC

				/* Special case: for binding but interface route, DMAC decision should be from L3->ARP->L2 */
				if((pPktHdr->dipL3Idx!=7) && (rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process==L34_PROCESS_ARP))
				{
					pPktHdr->dipArpOrNBIdx=(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.arpStart<<2)+(pPktHdr->ipv4Dip& ((0x1<<(31-rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.ipMask))-1));
					pPktHdr->dmacL2Idx=rg_db.arp[pPktHdr->dipArpOrNBIdx].rtk_arp.nhIdx;	
				}
				else
				{
				pPktHdr->dmacL2Idx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.nhIdx; //for Port/VLAN Bidning Route DMAC
				}
				pPktHdr->netifIdx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.ifIdx; //for Port/VLAN Bidning Route SMAC
				/* ext ip table decision */
				pPktHdr->extipIdx=pPktHdr->netifIdx; /* for NAPT:DMAC, SMAC */			
				//DEBUG("Binding to interface pPktHdr->netifIdx = %d",pPktHdr->netifIdx);
				return RG_FWDENGINE_RET_CONTINUE;
			}
		}
		else
		{
			if(pPktHdr->bindNextHopIdx!=FAIL) return RG_FWDENGINE_RET_CONTINUE;
		}
	}

	//Interface Routing
	if(sipDipClass==SIP_DIP_CLASS_NAPTR)
		pPktHdr->dipL3Idx=_rtk_rg_l3lookup(pPktHdr->ipv4Dip);
	//DEBUG("ipv4=%x dipL3Idx=%d process=%d\n",pPktHdr->ipv4Dip,pPktHdr->dipL3Idx,rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process);	
	/* arp table decision */
	if(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process==L34_PROCESS_ARP)
	{
		pPktHdr->netifIdx=rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.netifIdx; //for Normal Route SMAC
		pPktHdr->dipArpOrNBIdx=(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.arpStart<<2)+(pPktHdr->ipv4Dip& ((1<<(31-rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.ipMask))-1));
		pPktHdr->dmacL2Idx=rg_db.arp[pPktHdr->dipArpOrNBIdx].rtk_arp.nhIdx;	
		//DEBUG("ARP netif=%d arp=%d l2=%d\n",pPktHdr->netifIdx,pPktHdr->dipArpOrNBIdx,pPktHdr->dmacL2Idx);		

		//if ARP miss, return to PS
		if(rg_db.arp[pPktHdr->dipArpOrNBIdx].rtk_arp.valid==0)
			return RG_FWDENGINE_RET_TO_PS;
	}

	/* nexthop table decision */
	else if(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process==L34_PROCESS_NH)
	{
		pPktHdr->nexthopIdx=rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.nhStart; //for Normal Route DMAC
		pPktHdr->dmacL2Idx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.nhIdx; //for Normal Route DMAC
		pPktHdr->netifIdx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.ifIdx; //for Normal Route SMAC
		//DEBUG("NH netif=%d nh=%d l2=%d\n",pPktHdr->netifIdx,pPktHdr->nexthopIdx,pPktHdr->dmacL2Idx);
	}
	else if(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process==L34_PROCESS_CPU)
	{
		//FIXME:till now default route to CPU is to protocol, if sw routing link-list is implement, 
		//the default route in hw will means routing should check sw routing link-list, too.
		if(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.ipAddr > 0)
		{
			//DEBUG("L34_PROCESS_CPU:software ARP table lookup");
			_rtk_rg_softwareArpTableLookUp(pPktHdr->dipL3Idx,pPktHdr->ipv4Dip,&pSoftwareArpEntry);
			if(pSoftwareArpEntry==NULL)return RG_FWDENGINE_RET_TO_PS;	//arp not valid
			
			pPktHdr->netifIdx=rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.netifIdx; //for Normal Route SMAC
			//pPktHdr->dipArpOrNBIdx=(rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.arpStart<<2)+(pPktHdr->ipv4Dip& ((1<<(31-rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.ipMask))-1));
			pPktHdr->dmacL2Idx=rg_db.arp[pSoftwareArpEntry->idx].rtk_arp.nhIdx;
		}
		else
			return RG_FWDENGINE_RET_TO_PS;
	}
	
	//Check MTU
	if(rg_db.netif[pPktHdr->netifIdx].rtk_netif.mtu < pPktHdr->l3Len)		//TooBig, trap to protocol stack
	{
		//FIXME: ip fragment should be done in fwdEngine, not protocol stack...
		//and we should check "Don't fragmets" flag, if enable, we should return ICMP "Packet is too big" infomation.
		if(pPktHdr->tagif&ICMP_TAGIF)
		{
			pPktHdr->ICMPOverMTU=1;
			return RG_FWDENGINE_RET_CONTINUE;
		}	
		//DEBUG("packet L3 size(%d) is bigger than interface[%d]'s MTU(%d)",
			//pPktHdr->l3Len,pPktHdr->netifIdx,rg_db.netif[pPktHdr->netifIdx].rtk_netif.mtu);
		return RG_FWDENGINE_RET_TO_PS;
	}

	/* ext ip table decision */
	pPktHdr->extipIdx=pPktHdr->netifIdx; /* for NAPT:DMAC, SMAC */
	return RG_FWDENGINE_RET_CONTINUE;
	
}
#if 0
int _rtk_rg_v6RoutingDecisionTablesLookup(rtk_rg_pktHdr_t *pPktHdr, int isLAN2WAN)
{
	rtk_ipv6Neighbor_entry_t *neighbor;
	int ret,neighbor_valid_idx,nb_hash_idx,i;
	if(isLAN2WAN==1) //Only for LAN to WAN packet will hit binding table.
	{
		//Port/VLAN Binding
		if(pPktHdr->bindLookUpFinished==0)
		{
			pPktHdr->bindNextHopIdx=_rtk_rg_portBindingLookup(pPktHdr->pRxDesc->opts3.bit.src_port_num,0,pPktHdr->ctagVid);
			pPktHdr->bindLookUpFinished=1;
			if(pPktHdr->bindNextHopIdx!=FAIL)
			{
				DEBUG("binding hit!");
				pPktHdr->nexthopIdx=pPktHdr->bindNextHopIdx; //for Port/VLAN Bidning Route SMAC/DMAC
				pPktHdr->dmacL2Idx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.nhIdx; //for Port/VLAN Bidning Route DMAC
				pPktHdr->netifIdx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.ifIdx; //for Port/VLAN Bidning Route SMAC
				/* ext ip table decision */
				//pPktHdr->extipIdx=pPktHdr->netifIdx; /* for NAPT:DMAC, SMAC */			
				//DEBUG("pPktHdr->netifIdx = %d",pPktHdr->netifIdx);
				return RG_FWDENGINE_RET_DIRECT_TX;
			}
		}
		else if(pPktHdr->bindNextHopIdx!=FAIL) 
			return RG_FWDENGINE_RET_DIRECT_TX;
		
	}

	//Interface Routing
	ret=_rtk_rg_v6L3lookup(pPktHdr->pIpv6Dip);
	if(ret<0)return RG_FWDENGINE_RET_TO_PS;
	
	pPktHdr->dipL3Idx=ret;
	//DEBUG("ipv4=%x dipL3Idx=%d process=%d\n",pPktHdr->ipv4Dip,pPktHdr->dipL3Idx,rg_db.l3[pPktHdr->dipL3Idx].rtk_l3.process);
	DEBUG("the rout type is %x",rg_db.v6route[pPktHdr->dipL3Idx].rtk_v6route.type)
	switch(rg_db.v6route[pPktHdr->dipL3Idx].rtk_v6route.type)
	{
		/* neighbor table decision */
		case L34_IPV6_ROUTE_TYPE_LOCAL:
		{
			pPktHdr->netifIdx=rg_db.v6route[pPktHdr->dipL3Idx].rtk_v6route.nhOrIfidIdx; //for Normal Route SMAC
			ret=0;
			nb_hash_idx = _rtk_rg_IPv6NeighborHash(pPktHdr->pIpv6Dip+8, (unsigned char)pPktHdr->dipL3Idx);
			for(i=0;i<8;i++)
			{
				neighbor_valid_idx=(nb_hash_idx<<3)+i;
				//rtlglue_printf("the matchNeighbor idx = %d\n",matchNeighbor);
				DEBUG("the valid neighbor idx is %d",neighbor_valid_idx);
				neighbor = &rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor;
				//rtlglue_printf("the neighbor.ipv6RouteIdx = %d, matchEntry = %d\n",neighbor.ipv6RouteIdx,matchEntry);
				if(neighbor->valid && 
					(neighbor->ipv6RouteIdx == pPktHdr->dipL3Idx)&&
					(_rtk_rg_CompareIFID(pPktHdr->pIpv6Dip+8, neighbor->ipv6Ifid)))
				{
					//rtlglue_printf("HIT!!!!!\n");
					DEBUG("IPv6: Hit neighbor table!L2IDX = %d rt=%d",neighbor->l2Idx,pPktHdr->dipL3Idx);
					ret = 1;
					break;
				}
			}
			if(ret==0)	//neighbor lookup miss
			{
				DEBUG("before ret to PS in RDTL");
				return RG_FWDENGINE_RET_TO_PS;
			}
			pPktHdr->dipArpOrNBIdx=neighbor_valid_idx;
			pPktHdr->dmacL2Idx=neighbor->l2Idx;
			//DEBUG("Local netif=%d nb=%d l2=%d\n",pPktHdr->netifIdx,pPktHdr->dipArpOrNBIdx,pPktHdr->dmacL2Idx);		
			break;
		}
		/* nexthop table decision */
		case L34_IPV6_ROUTE_TYPE_GLOBAL:
		{
			pPktHdr->nexthopIdx=rg_db.v6route[pPktHdr->dipL3Idx].rtk_v6route.nhOrIfidIdx; //for Normal Route DMAC
			pPktHdr->dmacL2Idx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.nhIdx; //for Normal Route DMAC
			pPktHdr->netifIdx=rg_db.nexthop[pPktHdr->nexthopIdx].rtk_nexthop.ifIdx; //for Normal Route SMAC
			//DEBUG("Global netif=%d nh=%d l2=%d\n",pPktHdr->netifIdx,pPktHdr->nexthopIdx,pPktHdr->dmacL2Idx);
			break;
		}
		case L34_IPV6_ROUTE_TYPE_TRAP:
			return RG_FWDENGINE_RET_TO_PS;
		default:	//L34_IPV6_ROUTE_TYPE_DROP
			return RG_FWDENGINE_RET_DROP;
	}
	
	return RG_FWDENGINE_RET_DIRECT_TX;
}
#endif

void dump_packet(u8 *pkt,u32 size,char *memo);

/* force: Force to use the wishPort. For inbound connection, we don't modify the wishPort number. */
int _rtk_rg_naptExtPortGetAndUse(int force,int isTcp,uint16 wishPort)
{
	int wishIdx;
	uint32 wishBitValue;
	int i;

	i=wishPort;

	while(1)
	{
		wishIdx=i>>5; // =wishPort/32
		wishBitValue=1<<(i&0x1f);
		if(isTcp)
		{
			if(((rg_db.naptTcpExternPortUsed[wishIdx]&wishBitValue)==0) || force)
			{
				rg_db.naptTcpExternPortUsedRefCount[wishPort]++;
				rg_db.naptTcpExternPortUsed[wishIdx]|=wishBitValue;
				return i;
			}			
		}
		else
		{
			if(((rg_db.naptUdpExternPortUsed[wishIdx]&wishBitValue)==0) || force)
			{
				rg_db.naptUdpExternPortUsedRefCount[wishPort]++;
				rg_db.naptUdpExternPortUsed[wishIdx]|=wishBitValue;
				return i;
			}
		}
		i++;
		i&=0xffff;
		if(i==wishPort) break;		
	}	
	return FAIL;
}

#ifdef CONFIG_ROME_NAPT_SHORTCUT
void _rtk_rg_naptShortcutUpdate(rtk_rg_pktHdr_t *pPktHdr,int dir,int naptIdx, int isNapt)
{
	int isTcp=0;

	if(pPktHdr->aclDecision.aclHit) return;

	if(pPktHdr->tagif&TCP_TAGIF)
		isTcp=1;
	//Check for ALG
	if(isNapt && pPktHdr->algAction==RG_ALG_ACT_TO_FWDENGINE)
		//((_rtk_rg_algCheckEnable(isTcp,pPktHdr->dport)==SUCCESS) || (_rtk_rg_algCheckEnable(isTcp,pPktHdr->sport)==SUCCESS)	 ||	//check ALG serverInWan outbound and inbound port
		//(_rtk_rg_algSrvInLanCheckEnable(isTcp,pPktHdr->sport)==SUCCESS) || (_rtk_rg_algSrvInLanCheckEnable(isTcp,pPktHdr->dport)==SUCCESS)))	//check ALG serverInLan outbound and inbound port
	{
		DEBUG("ALG enable port...don't update shortCut!");
		return;
	}

	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].sip=pPktHdr->ipv4Sip; //SIP
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].dip=pPktHdr->ipv4Dip; //DIP
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].sport=pPktHdr->sport; //SPORT
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].dport=pPktHdr->dport; //DPORT
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].isTcp=isTcp;
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].new_lut_idx=pPktHdr->dmacL2Idx; //DMAC
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].new_intf_idx=pPktHdr->netifIdx; //SMAC
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].new_eip_idx=pPktHdr->extipIdx; //for outbound sip
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].naptIdx=naptIdx;
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].isNapt=isNapt;
	rg_db.naptShortCut[rg_db.naptShortcutEndIdx].direction=dir;
	DEBUG("[Update SC:%d] dir:%s da=%d sa_intf=%d eipIdx=%d naptIdx=%d\n",rg_db.naptShortcutEndIdx,(dir==NAPT_DIRECTION_INBOUND)?"IN":"OUT",pPktHdr->dmacL2Idx,pPktHdr->netifIdx,pPktHdr->extipIdx,naptIdx);
	DEBUG("isTcp:%d src:0x%x:%d dst:0x%x:%d",isTcp,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
	rg_db.naptShortcutEndIdx=(++rg_db.naptShortcutEndIdx)&(MAX_NAPT_SHORTCUT_SIZE-1);
}
#endif

int _rtk_rg_getNaptOutFreeList(rtk_rg_table_naptOut_linkList_t	**pNaptOutFreeList)
{
	if(rg_db.pNaptOutFreeListHead==NULL) return FAIL;
	*pNaptOutFreeList=rg_db.pNaptOutFreeListHead;
	rg_db.pNaptOutFreeListHead=rg_db.pNaptOutFreeListHead->pNext;
	(*pNaptOutFreeList)->pNext=NULL;
	//DEBUG("FreeList=%d(%p)\n",(*pNaptOutFreeList)->idx,*pNaptOutFreeList);
	return SUCCESS;
}

int _rtk_rg_getNaptInFreeList(rtk_rg_table_naptIn_linkList_t	**pNaptInFreeList)
{
	if(rg_db.pNaptInFreeListHead==NULL) return FAIL;
	*pNaptInFreeList=rg_db.pNaptInFreeListHead;
	rg_db.pNaptInFreeListHead=rg_db.pNaptInFreeListHead->pNext;
	(*pNaptInFreeList)->pNext=NULL;
//	DEBUG("FreeList=%d(%x)\n",(*pNaptInFreeList)->idx,*pNaptInFreeList);
	return SUCCESS;
}

int _rtk_rg_addNaptOutHashList(int naptHashOutIdx,int *pNaptOutIdx)
{
	rtk_rg_table_naptOut_linkList_t *pNaptFreeOutList;	
	if(_rtk_rg_getNaptOutFreeList(&pNaptFreeOutList)==FAIL) return FAIL;

	if(rg_db.pNaptOutHashListHead[naptHashOutIdx]==NULL)
	{
		rg_db.pNaptOutHashListHead[naptHashOutIdx]=pNaptFreeOutList;
	}
	else
	{		
		pNaptFreeOutList->pNext=rg_db.pNaptOutHashListHead[naptHashOutIdx];
		rg_db.pNaptOutHashListHead[naptHashOutIdx]=pNaptFreeOutList;	
	}
	*pNaptOutIdx=pNaptFreeOutList->idx;	
	rg_db.naptOut[*pNaptOutIdx].hashIdx=naptHashOutIdx;
	return SUCCESS;
}

int _rtk_rg_addNaptInHashList(int naptHashInIdx,int *pNaptInIdx)
{
	rtk_rg_table_naptIn_linkList_t *pNaptFreeInList;	
	if(_rtk_rg_getNaptInFreeList(&pNaptFreeInList)==FAIL) return FAIL;

	if(rg_db.pNaptInHashListHead[naptHashInIdx]==NULL)
	{
		rg_db.pNaptInHashListHead[naptHashInIdx]=pNaptFreeInList;
	}
	else
	{		
		pNaptFreeInList->pNext=rg_db.pNaptInHashListHead[naptHashInIdx];
		rg_db.pNaptInHashListHead[naptHashInIdx]=pNaptFreeInList;	
	}
	*pNaptInIdx=pNaptFreeInList->idx;
	rg_db.naptIn[*pNaptInIdx].hashIdx=naptHashInIdx;
	return SUCCESS;
}

int _rtk_rg_freeFragOutList(int fragIdx, rtk_rg_ipv4_fragment_out_t *pReleaseOutList)
{
	//remove from hashIdx list
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	if(rg_db.pFragOutHashListHead[fragIdx]==pReleaseOutList) 	//head need delete, move to next
		rg_db.pFragOutHashListHead[fragIdx]=pReleaseOutList->pNext;
	else
		pReleaseOutList->pPrev->pNext=pReleaseOutList->pNext;		//redirect free->prev's pointer pNext to next of free	

	if(pReleaseOutList->pNext!=NULL)		//last one
		pReleaseOutList->pNext->pPrev=pReleaseOutList->pPrev;		//redirect free->next's pointer pPrev to previous of free

	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);
	
	//add to free list		
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragFreeLock);
	pReleaseOutList->pNext=rg_db.pFragOutFreeListHead;
	rg_db.pFragOutFreeListHead=pReleaseOutList;
	//DEBUG("###### free out list..%p",pReleaseOutList);
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragFreeLock);
				
	/*rtk_rg_ipv4_fragment_out_t *pPreDelOutList,*pDelOutList;
	
	pPreDelOutList=rg_db.pFragOutHashListHead[fragIdx];
	pDelOutList=pPreDelOutList;
	
	if(naptOutIdx==FAIL)		//free ICMP frag list
	{
		while(pDelOutList!=NULL)
		{
			if(pDelOutList->pktInfo.pICMPCtrlFlow==pICMPCtrlFlow)
			{
				//remove from hashIdx list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragLock);
				if(pPreDelOutList==pDelOutList) 	//head need delete
					rg_db.pFragOutHashListHead[fragIdx]=pDelOutList->pNext;
				else
					pPreDelOutList->pNext=pDelOutList->pNext;
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragLock);
				
				//add to free list		
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragFreeLock);
				pDelOutList->pNext=rg_db.pFragOutFreeListHead;
				rg_db.pFragOutFreeListHead=pDelOutList;
				//DEBUG("###### free it..%p",pDelOutList);
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragFreeLock);
				break;
			}
			pPreDelOutList=pDelOutList;
			pDelOutList=pDelOutList->pNext;
		}		
	}
	else		//free napt frag list
	{
		while(pDelOutList!=NULL)
		{
			if(pDelOutList->NaptOutboundEntryIndex==naptOutIdx)
			{
				//remove from hashIdx list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragLock);
				if(pPreDelOutList==pDelOutList) 	//head need delete
					rg_db.pFragOutHashListHead[fragIdx]=pDelOutList->pNext;
				else
					pPreDelOutList->pNext=pDelOutList->pNext;
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragLock);
				
				//add to free list		
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragFreeLock);
				pDelOutList->pNext=rg_db.pFragOutFreeListHead;
				rg_db.pFragOutFreeListHead=pDelOutList;
				//DEBUG("###### free it..%p",pDelOutList);
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragFreeLock);
				break;
			}
			pPreDelOutList=pDelOutList;
			pDelOutList=pDelOutList->pNext;
		}
	}*/
	
	//DEBUG("###### delete me..%d",fragIdx);
	//rg_db.ipv4FragmentOutTable[fragIdx].valid=0;
	return SUCCESS;
}

int _rtk_rg_freeFragInList(int fragIdx, rtk_rg_ipv4_fragment_in_t *pReleaseInList)
{
	//remove from hashIdx list
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	if(rg_db.pFragInHashListHead[fragIdx]==pReleaseInList) 	//head need delete, move to next
		rg_db.pFragInHashListHead[fragIdx]=pReleaseInList->pNext;
	else
		pReleaseInList->pPrev->pNext=pReleaseInList->pNext;		//redirect free->prev's pointer pNext to next of free	

	if(pReleaseInList->pNext!=NULL)		//last one
		pReleaseInList->pNext->pPrev=pReleaseInList->pPrev;		//redirect free->next's pointer pPrev to previous of free

	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);
	
	//add to free list		
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragFreeLock);
	pReleaseInList->pNext=rg_db.pFragInFreeListHead;
	rg_db.pFragInFreeListHead=pReleaseInList;
	//DEBUG("###### free in list..%p",pReleaseInList);
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragFreeLock);
	/*rtk_rg_ipv4_fragment_in_t *pPreDelInList,*pDelInList;

	pPreDelInList=rg_db.pFragInHashListHead[fragIdx];
	pDelInList=pPreDelInList;
	if(naptOutIdx==FAIL)		//free ICMP frag list
	{
		while(pDelInList!=NULL)
		{
			if(pDelInList->pktInfo.pICMPCtrlFlow==pICMPCtrlFlow)
			{
				//remove from hashIdx list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragLock);			
				if(pPreDelInList==pDelInList)		//head need delete
					rg_db.pFragInHashListHead[fragIdx]=pDelInList->pNext;
				else
					pPreDelInList->pNext=pDelInList->pNext;
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragLock);

				//add to free list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragFreeLock);
				pDelInList->pNext=rg_db.pFragInFreeListHead;
				rg_db.pFragInFreeListHead=pDelInList;				
				//DEBUG("###### delete inbound frag list..%d",fragIdx);
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragFreeLock);
				break;
			}
			pPreDelInList=pDelInList;
			pDelInList=pDelInList->pNext;
		}		
	}
	else		//free napt frag list
	{
		while(pDelInList!=NULL)
		{
			if(pDelInList->NaptOutboundEntryIndex==naptOutIdx)
			{
				//remove from hashIdx list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragLock);			
				if(pPreDelInList==pDelInList)		//head need delete
					rg_db.pFragInHashListHead[fragIdx]=pDelInList->pNext;
				else
					pPreDelInList->pNext=pDelInList->pNext;
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragLock);

				//add to free list
				//------------------ Critical Section start -----------------------//
				rg_lock(&rg_kernel.ipv4FragFreeLock);
				pDelInList->pNext=rg_db.pFragInFreeListHead;
				rg_db.pFragInFreeListHead=pDelInList;				
				//DEBUG("###### delete inbound frag list..%d",fragIdx);
				//------------------ Critical Section End -----------------------//
				rg_unlock(&rg_kernel.ipv4FragFreeLock);
				break;
			}
			pPreDelInList=pDelInList;
			pDelInList=pDelInList->pNext;
		}		
	}*/
	//DEBUG("###### delete me..%d",fragIdx);
	//rg_db.ipv4FragmentOutTable[fragIdx].valid=0;
	return SUCCESS;
}

void _rtk_rg_freeAllFragOutList(void)
{
	int i;
	
	for(i=0;i<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;i++)
	{
		while(rg_db.pFragOutHashListHead[i]!=NULL)
		{
			//travel all list
			_rtk_rg_freeFragOutList(i,rg_db.pFragOutHashListHead[i]);
		}
	}
}

void _rtk_rg_freeAllFragInList(void)
{
	int i;
	
	for(i=0;i<MAX_NAPT_IN_HW_TABLE_SIZE>>2;i++)
	{
		while(rg_db.pFragInHashListHead[i]!=NULL)
		{
			//travel all list
			_rtk_rg_freeFragInList(i,rg_db.pFragInHashListHead[i]);
		}
	}
}

int _rtk_rg_getFragOutFreeList(rtk_rg_ipv4_fragment_out_t **pFragOutFreeList)
{
	if(rg_db.pFragOutFreeListHead==NULL)
	{
		//Clear all hashHead
		//mibdump_frag();
		DEBUG("============================== start clear OUT ======================");
		_rtk_rg_freeAllFragOutList();
		//mibdump_frag();
	}
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragFreeLock);
	*pFragOutFreeList=rg_db.pFragOutFreeListHead;
	rg_db.pFragOutFreeListHead=(*pFragOutFreeList)->pNext;
	(*pFragOutFreeList)->pNext=NULL;
	(*pFragOutFreeList)->pPrev=NULL;
	//DEBUG("get Free out List=%p\n",*pFragOutFreeList);
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragFreeLock);
	return SUCCESS;
}

int _rtk_rg_getFragInFreeList(rtk_rg_ipv4_fragment_in_t **pFragInFreeList)
{
	if(rg_db.pFragInFreeListHead==NULL)
	{
		//Clear all hashHead
		//mibdump_frag();
		DEBUG("============================== start clear IN ======================");
		_rtk_rg_freeAllFragInList();
		//mibdump_frag();
	}
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragFreeLock);
	*pFragInFreeList=rg_db.pFragInFreeListHead;
	rg_db.pFragInFreeListHead=(*pFragInFreeList)->pNext;
	(*pFragInFreeList)->pNext=NULL;
	(*pFragInFreeList)->pPrev=NULL;
	//DEBUG("get Free in List=%p\n",*pFragInFreeList);	
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragFreeLock);
	return SUCCESS;
}


int _rtk_rg_naptTcpUdpOutFreeEntryGet(rtk_rg_algAction_t algAction, int8 isTcp, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort)
{
	int naptHashOutIdx;
	int naptOutIdx;
	int i;
	
	//found a free naptOut entry				
	naptHashOutIdx=_rtk_rg_naptTcpUdpOutHashIndex(isTcp,srcAddr,srcPort,destAddr,destPort);
	naptOutIdx=naptHashOutIdx<<2;

	//Check for ALG
	if(algAction==RG_ALG_ACT_TO_FWDENGINE)
	{
		//DEBUG("Add to pure software..naptHashOutIdx=%d %p",naptHashOutIdx,rg_db.pNaptOutFreeListHead);
		goto PURESW;
	}

	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
#if 1	
		if(rg_db.naptOut[i].rtk_naptOut.valid==0) 
		{
			naptOutIdx=i;
			return naptOutIdx;
		}
#endif		
	}
	if(i==naptOutIdx+4)
	{
PURESW:
		//FIXME("NAPT 4-ways is full - found link list");	
			
		//found a free naptOut from link list		
		if(_rtk_rg_addNaptOutHashList(naptHashOutIdx,&naptOutIdx)==SUCCESS)
		{
			//DEBUG("%s naptOutIdx=%d\n",__FUNCTION__,naptOutIdx);
			return naptOutIdx;
		}
		else
			return FAIL;
	}
	return FAIL;
}

int _rtk_rg_naptTcpUdpInFreeEntryGet(int8 isTcp,int extIp,int extPort)
{
	int naptHashInIdx;
	int naptInIdx;
	int i;

	naptHashInIdx=_rtk_rg_naptTcpUdpInHashIndex(isTcp,extIp,extPort);
	naptInIdx=naptHashInIdx<<2;
	for(i=naptInIdx;i<naptInIdx+4;i++)
	{
		if(rg_db.naptIn[i].rtk_naptIn.valid==NAPT_IN_TYPE_INVALID) 
		{
			naptInIdx=i;
			return naptInIdx;
		}
	}
	
	//FIXME("NAPTR 4-ways is full - found link list");			
	
	//found a free naptIn from link list		
	if(_rtk_rg_addNaptInHashList(naptHashInIdx,&naptInIdx)==SUCCESS)
	{
		DEBUG("%s naptInIdx=%d\n",__FUNCTION__,naptInIdx);
		return naptInIdx;
	}
	else
		return FAIL;
	

	return FAIL;
}

#if	defined(CONFIG_RG_NAPT_UPNP_SUPPORT)
int _rtk_rg_fwdEngine_upnpCheck(void *data,ipaddr_t *transIP,uint16 *transPort)
{
	int upnpIdx=0;
	int ret;
	rtk_rg_upnpConnection_t upnp;
	rtk_rg_pktHdr_t *pPktHdr=(rtk_rg_pktHdr_t *)data;

	DEBUG("UPNP Check...");
	memset(&upnp,0,sizeof(upnp));
	
	while(1)
	{
		ret = rtk_rg_upnpConnection_find(&upnp,&upnpIdx);
		if(ret!=RT_ERR_RG_OK) break;

		if(((upnp.is_tcp && (pPktHdr->tagif&TCP_TAGIF)) || (upnp.is_tcp==0 && (pPktHdr->tagif&UDP_TAGIF)))&&
			(upnp.gateway_port == pPktHdr->dport) &&
			(upnp.wan_intf_idx == pPktHdr->extipIdx) &&
			((upnp.limit_remote_ip==0) || (upnp.remote_ip==pPktHdr->ipv4Sip)) &&
			((upnp.limit_remote_port==0) || (upnp.remote_port==pPktHdr->sport)))
		{
			*transIP = upnp.local_ip;
			*transPort = upnp.local_port;
			DEBUG("hit. ==> [localIP:0x%x] [localPort:0x%x]\n",pPktHdr->ipv4Dip,pPktHdr->dport);

			if(upnp.type == UPNP_TYPE_ONESHOT)
					rtk_rg_upnpConnection_del(upnpIdx);

			return RT_ERR_RG_OK;
		}
		upnpIdx++;
	}
	DEBUG("not hit.\n");
	return RT_ERR_RG_FAILED;
}
#endif

#if	defined(CONFIG_RG_NAPT_VIRTUAL_SERVER_SUPPORT)
int _rtk_rg_fwdEngine_virtualServerCheck(void *data,ipaddr_t *transIP,uint16 *transPort)
{
	int vsIdx=0;
	int ret;
	int off;
	rtk_rg_virtualServer_t virtualServer;
	rtk_rg_pktHdr_t *pPktHdr=(rtk_rg_pktHdr_t *)data;

	DEBUG("Virtual Server Check...");

	memset(&virtualServer,0,sizeof(virtualServer));
	
	while(1)
	{
		ret = rtk_rg_virtualServer_find(&virtualServer,&vsIdx);
		if(ret!=RT_ERR_RG_OK) break;
		
		if(((virtualServer.is_tcp && (pPktHdr->tagif&TCP_TAGIF)) || (virtualServer.is_tcp==0 && (pPktHdr->tagif&UDP_TAGIF)))&&
			(virtualServer.wan_intf_idx == pPktHdr->extipIdx) &&
			((pPktHdr->dport>=virtualServer.gateway_port_start) && (pPktHdr->dport<(virtualServer.gateway_port_start+virtualServer.mappingPortRangeCnt))))
		{
			off=pPktHdr->dport-virtualServer.gateway_port_start;
			*transIP = virtualServer.local_ip;
			*transPort = virtualServer.local_port_start+off;
			DEBUG("hit. ==> [localIP:0x%x] [localPort:0x%x]\n",pPktHdr->ipv4Dip,pPktHdr->dport);
			return RT_ERR_RG_OK;
		}
		vsIdx++;
	}
	
	DEBUG("not hit.\n");
	return RT_ERR_RG_FAILED;
}
#endif

#if	defined(CONFIG_RG_NAPT_DMZ_SUPPORT)
int _rtk_rg_fwdEngine_dmzCheck(void *data,ipaddr_t *transIP,uint16 *transPort)
{
	rtk_rg_pktHdr_t *pPktHdr=(rtk_rg_pktHdr_t *)data;
	DEBUG("DMZ Check...");

	if(rg_db.dmzInfo[pPktHdr->extipIdx].enabled && (rg_db.dmzInfo[pPktHdr->extipIdx].mac_mapping_enabled==0))
	{
		*transIP = rg_db.dmzInfo[pPktHdr->extipIdx].private_ip;
		DEBUG("hit. ==> [localIP:0x%x] [localPort:0x%x]\n",pPktHdr->ipv4Dip,pPktHdr->dport);
	}
	else
	{
		DEBUG("not hit.\n");
		return RT_ERR_RG_FAILED;
	}

	return RT_ERR_RG_OK;
}
#endif

int _rtk_rg_fwdEngine_connType_lookup(rtk_rg_pktHdr_t *pPktHdr, ipaddr_t *transIP, int16 *transPort)
{
	if((pPktHdr==NULL) || (transIP==NULL) || (transPort==NULL)) return RT_ERR_RG_INVALID_PARAM;
	
	//Inbound connection table lookup, e.g. UPNP,virtual server & DMZ tables.
	if(((rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack!=NULL) && (rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack(pPktHdr,transIP,transPort)==RT_ERR_OK)) ||
		((rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack!=NULL) && (rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack(pPktHdr,transIP,transPort)==RT_ERR_OK)) ||
		((rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack!=NULL) && (rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack(pPktHdr,transIP,transPort)==RT_ERR_OK)))
		return RT_ERR_RG_OK;

	return RT_ERR_RG_FAILED;
}

int _rtk_rg_urlFilter(u8 *pData, u32 len, rtk_rg_pktHdr_t *pPktHdr)
{

	int i,index;
	char fqdn[512]={0}; //packet fqdn
	char *keyword; //urlfilter url_filter_string
	char *path;//urlfilter path_filter_string
	char keyword_fqdn[384];// urlfilter url_filter_string(128) + urlfilter path_filter_string(256)
	
	char *parseSuccess=NULL;	 
	char *url_head=NULL;
	char *url_tail=NULL;
	char *path_head=NULL;
	char *path_tail=NULL;
	char space = 0x20;
	char sep[3];
	int url_len = 0;
	int path_len = 0;
	int keyword_url_len = 0;
	int keyword_path_len = 0;

	rtk_rg_naptInfo_t naptInfo;
	int valid_idx;
	int ret;

	if(_rtk_rg_is_urlFilter_table_init())
		ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	if(pPktHdr->pL4Payload==NULL){
		DEBUG("[URLFILTER]urlFilter get a Empty payload packet \n");
		return RG_FWDENGINE_RET_DROP;
	}

	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.urlFilter_valid_entry[i]==-1){//use urlFilter_valid_entry search to promote efficiency
			break;
		}else{
			index = rg_db.systemGlobal.urlFilter_valid_entry[i]; //get the valid entry
			keyword= rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.url_filter_string;
			path = rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.path_filter_string;
			if(keyword==NULL || path==NULL){
				DEBUG("[URLFILTER]urlFilter get a NULL keyword or path!");
				continue;
			}
			//DEBUG("keyword: %s \n",keyword);

			/*parsing FQDN*/

			/*use for parsing Host:*/
			sep[0]=0x0d;
			sep[1]=0x0a;
			sep[2]='\0';

			if(pPktHdr->pL4Payload!=NULL){		
				path_head = strstr(pPktHdr->pL4Payload, "/");
			}else{
				//rtlglue_printf("pL4Payload is null\n");
			}
		
			if(path_head!=NULL){
				//rtlglue_printf("path_head:%s.\n",url_head);
				path_tail = strchr(path_head, space);
			}else{
				DEBUG("[URLFILTER]path_head is null\n");
			}
		
			if(path_tail!=NULL){
				//rtlglue_printf("path_tail:%s.\n",url_tail);
				url_head = strstr(path_tail, "Host:");
				if(url_head!=NULL){
					url_head = strchr(url_head, space);//cut the "Host"
					if(url_head!=NULL)
						url_head = &url_head[1];//cut the " "
				}
			}else{
				DEBUG("[URLFILTER]path_tail is null\n");
			}
		
			if(url_head!=NULL){
				//rtlglue_printf("path_head:%s.\n",path_head);
				url_tail = strstr(url_head, sep);
			}else{
				DEBUG("[URLFILTER]url_head is null\n");
			}
		
			if(url_tail!=NULL){
				//rtlglue_printf("url_tail:%s.\n",path_tail);
			}else{
				DEBUG("[URLFILTER]url_tail is null\n");
			}
		
			if(url_head!=NULL && url_tail!=NULL && path_head!=NULL && path_tail!=NULL){
				url_len = url_tail - url_head;
				path_len = path_tail - path_head;
				if((url_len+path_len)>510){ //fqdn max buff is 512 bytes
					//dump_packet(pData,len,"urlFilter Packet");
					DEBUG("[URLFILTER]fqdn too long url_len=%d  path_len=%d!!!, DROP!!!\n",url_len,path_len);
					return RG_FWDENGINE_RET_DROP;
				}else{
					strncpy(fqdn,url_head,url_len);
					strncpy((fqdn+url_len),path_head,path_len);
					fqdn[url_len+path_len]='\0';
					
				}
			}else{
				//dump_packet(pData,len,"urlFilter Packet");
				//rtlglue_printf("fqdn: can not parse, trap!!! \n");
			}
			
			DEBUG("[URLFILTER]fqdn:%s\n",fqdn);
			
			if(fqdn!=NULL){
				/*compare FQDN*/
				if(rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.path_exactly_match){	
					keyword_url_len = strlen(rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.url_filter_string);
					keyword_path_len = strlen(rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.path_filter_string);
					DEBUG("[URLFILTER]keyword_url_len=%d keyword_path_len=%d\n",keyword_url_len,keyword_path_len);
					strncpy(keyword_fqdn,rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.url_filter_string,keyword_url_len);
					strncpy((keyword_fqdn+keyword_url_len),rg_db.systemGlobal.urlFilter_table_entry[index].urlFilter.path_filter_string,keyword_path_len);
					keyword_fqdn[keyword_url_len+keyword_path_len]='\0';
					
					DEBUG("[URLFILTER]keyword_fqdn:%s \n",keyword_fqdn);
					
					if(strlen(fqdn)!=strlen(keyword_fqdn)){
						DEBUG("[URLFILTER]len not the same! fqdn_len=%d  keyword_len=%d\n\n\n",strlen(fqdn),strlen(keyword_fqdn));
						//return SUCCESS;
					}else if(strcmp(fqdn,keyword_fqdn)){
						/*strcmp return true if fqdn & keyword not the same */
						DEBUG("[URLFILTER]strcmp not the same! keyword=%s \n\n\n",keyword);
						//return SUCCESS;
					}else{
						bzero(&naptInfo,sizeof(naptInfo));
						valid_idx= -1;
						naptInfo.naptTuples.is_tcp=1;
						naptInfo.naptTuples.local_ip=pPktHdr->ipv4Sip;
						naptInfo.naptTuples.local_port=pPktHdr->sport;
						naptInfo.naptTuples.remote_ip=pPktHdr->ipv4Dip;
						naptInfo.naptTuples.remote_port=pPktHdr->dport;
						ret = rtk_rg_naptConnection_find(&naptInfo,&valid_idx);
						
						if(ret==RT_ERR_RG_OK){
							DEBUG("[URLFILTER]del napt[%d]: sip=0x%x dip=0x%x sport=%d dport=%d\n",valid_idx,pPktHdr->ipv4Sip,pPktHdr->ipv4Dip,pPktHdr->sport,pPktHdr->dport);
							assert_ok(rtk_rg_naptConnection_del(valid_idx));
						}
						
						DEBUG("[URLFILTER]URL PACKET HAS BEEN DROP!\n\n\n");
						
						return RG_FWDENGINE_RET_DROP;
					}
				}else{
					//keyword only
					DEBUG("[URLFILTER]keyword: %s \n",keyword);
					parseSuccess = strstr(fqdn, keyword);
					if(parseSuccess!=NULL){
						bzero(&naptInfo,sizeof(naptInfo));
						valid_idx= -1;
						naptInfo.naptTuples.is_tcp=1;
						naptInfo.naptTuples.local_ip=pPktHdr->ipv4Sip;
						naptInfo.naptTuples.local_port=pPktHdr->sport;
						naptInfo.naptTuples.remote_ip=pPktHdr->ipv4Dip;
						naptInfo.naptTuples.remote_port=pPktHdr->dport;
						ret = rtk_rg_naptConnection_find(&naptInfo,&valid_idx);
						
						if(ret==RT_ERR_RG_OK){
							DEBUG("[URLFILTER]del napt[%d]: sip=0x%x dip=0x%x sport=%d dport=%d\n",valid_idx,pPktHdr->ipv4Sip,pPktHdr->ipv4Dip,pPktHdr->sport,pPktHdr->dport);
							assert_ok(rtk_rg_naptConnection_del(valid_idx));
						}
						
						DEBUG("[URLFILTER]URL PACKET HAS BEEN DROP!\n\n\n");
						return RG_FWDENGINE_RET_DROP;
					}
				}
			}
		}
	}


	return SUCCESS;
}

#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
int _rtk_rg_fwdEngine_inbound_fillNaptInfo(int *outIdx, rtk_rg_pktHdr_t *pPktHdr, ipaddr_t transIP, uint16 transPort)
{
	int extPort;
	int naptOutIdx,naptInIdx=0;
	int isTCP=0;

	if(pPktHdr->tagif&TCP_TAGIF) isTCP=1;
	//found a free naptOut entry			
	naptOutIdx=_rtk_rg_naptTcpUdpOutFreeEntryGet(pPktHdr->algAction,isTCP,transIP,transPort,pPktHdr->ipv4Sip,pPktHdr->sport);
	assert(naptOutIdx!=FAIL);
	if(naptOutIdx==FAIL) return FAIL;

	//found a free ext Port.
	extPort=_rtk_rg_naptExtPortGetAndUse(TRUE,isTCP,ntohs(*pPktHdr->pDport));
	assert(extPort!=FAIL);
	if(extPort==FAIL) return FAIL;
	DEBUG("Found external port:%d\n",extPort);
	
	//found a free naptIn entry
	naptInIdx=_rtk_rg_naptTcpUdpInFreeEntryGet(isTCP,rg_db.extip[pPktHdr->extipIdx].rtk_extip.extIpAddr,extPort);
	assert(naptInIdx!=FAIL);
	if(naptInIdx==FAIL) return FAIL;

	//_rtk_rg_arpAndMacEntryAdd(pPktHdr->ipv4Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->pRxDesc->opts3.bit.src_port_num,&sipArpIdx,0);


	rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx=naptInIdx;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.valid=1;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.priValid=0;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.priValue=0;
	rg_db.naptOut[naptOutIdx].extPort=extPort;

	rg_db.naptIn[naptInIdx].rtk_naptIn.extIpIdx=pPktHdr->extipIdx;
	rg_db.naptIn[naptInIdx].rtk_naptIn.extPortLSB=extPort&0xff;
	rg_db.naptIn[naptInIdx].rtk_naptIn.intIp=transIP;
	rg_db.naptIn[naptInIdx].rtk_naptIn.intPort=transPort;
	rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp=isTCP;
	rg_db.naptIn[naptInIdx].rtk_naptIn.priId=0;
	rg_db.naptIn[naptInIdx].rtk_naptIn.priValid=0;
	rg_db.naptIn[naptInIdx].rtk_naptIn.remHash=_rtk_rg_NAPTRemoteHash_get(pPktHdr->ipv4Sip,pPktHdr->sport);
	rg_db.naptIn[naptInIdx].rtk_naptIn.valid=NAPT_IN_TYPE_PORT_RESTRICTED_CONE;
	rg_db.naptIn[naptInIdx].remoteIp=pPktHdr->ipv4Sip;
	rg_db.naptIn[naptInIdx].remotePort=pPktHdr->sport;
	rg_db.naptIn[naptInIdx].refCount=1;

	*outIdx=naptOutIdx;

	DEBUG("set napt in[%d] out[%d] software entry",naptInIdx,naptOutIdx);
	//Record valid NAPT entry
	rg_db.naptValidSet[naptOutIdx>>5] |= (0x1<<(naptOutIdx&31));

	return SUCCESS;
}
#endif

int _rtk_rg_fwdEngine_outbound_fillNaptInfo(int *outIdx, rtk_rg_pktHdr_t *pPktHdr)
{		
	int extPort;
	int naptOutIdx,naptInIdx;
//	int sipArpIdx;
	int isTCP=0;
	
	if(pPktHdr->tagif&TCP_TAGIF) isTCP=1;
	//found a free naptOut entry				
	naptOutIdx=_rtk_rg_naptTcpUdpOutFreeEntryGet(pPktHdr->algAction,isTCP,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
	assert(naptOutIdx!=FAIL);
	if(naptOutIdx==FAIL) return FAIL;

	//found a free ext Port.
	extPort=_rtk_rg_naptExtPortGetAndUse(FALSE,isTCP,pPktHdr->sport);
	assert(extPort!=FAIL);
	if(extPort==FAIL) return FAIL;	
	
	//found a free naptIn entry
	naptInIdx=_rtk_rg_naptTcpUdpInFreeEntryGet(isTCP,rg_db.extip[pPktHdr->extipIdx].rtk_extip.extIpAddr,extPort);
	assert(naptInIdx!=FAIL);
	if(naptInIdx==FAIL) return FAIL;

	//_rtk_rg_arpAndMacEntryAdd(pPktHdr->ipv4Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->pRxDesc->opts3.bit.src_port_num,&sipArpIdx,0,0,0);


	rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx=naptInIdx;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.valid=1;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.priValid=0;
	rg_db.naptOut[naptOutIdx].rtk_naptOut.priValue=0;
	rg_db.naptOut[naptOutIdx].extPort=extPort;

	rg_db.naptIn[naptInIdx].rtk_naptIn.extIpIdx=pPktHdr->extipIdx;
	rg_db.naptIn[naptInIdx].rtk_naptIn.extPortLSB=extPort&0xff;
	rg_db.naptIn[naptInIdx].rtk_naptIn.intIp=pPktHdr->ipv4Sip;
	rg_db.naptIn[naptInIdx].rtk_naptIn.intPort=pPktHdr->sport;
	rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp=isTCP;
	rg_db.naptIn[naptInIdx].rtk_naptIn.priId=0;
	rg_db.naptIn[naptInIdx].rtk_naptIn.priValid=0;
	rg_db.naptIn[naptInIdx].rtk_naptIn.remHash=_rtk_rg_NAPTRemoteHash_get(pPktHdr->ipv4Dip,pPktHdr->dport);
	rg_db.naptIn[naptInIdx].rtk_naptIn.valid=NAPT_IN_TYPE_PORT_RESTRICTED_CONE;
	rg_db.naptIn[naptInIdx].remoteIp=pPktHdr->ipv4Dip;
	rg_db.naptIn[naptInIdx].remotePort=pPktHdr->dport;
	rg_db.naptIn[naptInIdx].refCount = 1;
	DEBUG("Add napt ==> %x:%d<-->%x:%d<-->%x:%d\n",rg_db.naptIn[naptInIdx].rtk_naptIn.intIp,rg_db.naptIn[naptInIdx].rtk_naptIn.intPort,rg_db.extip[pPktHdr->extipIdx].rtk_extip.extIpAddr,extPort,rg_db.naptIn[naptInIdx].remoteIp,rg_db.naptIn[naptInIdx].remotePort);

	*outIdx=naptOutIdx;

	DEBUG("set napt in[%d] out[%d] software entry",naptInIdx,naptOutIdx);
	//Record valid NAPT entry
	rg_db.naptValidSet[naptOutIdx>>5] |= (0x1<<(naptOutIdx&31));

	return SUCCESS;
}	

inline uint16 _rtk_rg_fwdengine_L4checksumUpdate(uint16 ori_checksum, uint32 ori_sip, uint16 ori_sport, uint32 new_sip, uint16 new_sport)
{
	uint32 tmp_chksum;
	//uint16 ori_ttlProto,new_ttlProto;
	//DEBUG("ori chksum = %x",ori_checksum);

	if(((ori_sip&0xffff0000)^(new_sip&0xffff0000))!=0)
	{
		//DEBUG("1");
		tmp_chksum = ((~ori_checksum)&0xffff) + (((~ori_sip)&0xffff0000)>>16) + (new_sip>>16);
		//DEBUG("chksum = %x",tmp_chksum);
		tmp_chksum += (((~ori_sip)&0xffff) + (new_sip&0xffff));
		//DEBUG("chksum = %x",tmp_chksum);
	}
	else 
	{
		//DEBUG("2");
		tmp_chksum = ((~ori_checksum)&0xffff) + ((~ori_sip)&0xffff) + (new_sip&0xffff);
		//DEBUG("chksum = %x",tmp_chksum);
	}

	if(ori_sport!=new_sport)
	{
		//DEBUG("3");
		tmp_chksum += (((~ori_sport)&0xffff) + new_sport);
		//DEBUG("chksum = %x",tmp_chksum);
	}

	//L4Chksum didn't contain TTL field!!
	
	return ~((tmp_chksum>>16)+(tmp_chksum&0xffff));
}

inline uint16 _rtk_rg_fwdengine_L3checksumUpdate(uint16 ori_checksum, uint32 ori_sip, uint8 ori_ttl, uint8 ori_protocol, uint32 new_sip, uint8 new_ttl)
{
	uint32 tmp_chksum;
	uint16 ori_ttlProto,new_ttlProto;
	//DEBUG("ori chksum = %x, oriSip is %x, oriTTL is %x, oriProto is %x, newSip is %x, newTTL is %x",ori_checksum,ori_sip,ori_ttl,ori_protocol,new_sip,new_ttl);

	if(((ori_sip&0xffff0000)^(new_sip&0xffff0000))!=0)
	{
		//DEBUG("1");
		tmp_chksum = ((~ori_checksum)&0xffff) + ((~ori_sip)>>16) + (new_sip>>16);
		//DEBUG("chksum = %x",tmp_chksum);
		tmp_chksum += (((~ori_sip)&0xffff) + (new_sip&0xffff));
		//DEBUG("chksum = %x",tmp_chksum);
	}
	else
	{
		//DEBUG("2");
		tmp_chksum = ((~ori_checksum)&0xffff) + ((~ori_sip)&0xffff) + (new_sip&0xffff);
		//DEBUG("chksum = %x",tmp_chksum);
	}

	if(ori_ttl!=new_ttl)
	{
		//DEBUG("3");
		ori_ttlProto=(ori_ttl<<8)+ori_protocol;
		new_ttlProto=(new_ttl<<8)+ori_protocol;
		tmp_chksum += ((~ori_ttlProto)&0xffff);
		tmp_chksum += new_ttlProto;
		//DEBUG("chksum = %x",tmp_chksum);
	}
	
	return ~((tmp_chksum>>16)+(tmp_chksum&0xffff));
}

void _rtk_rg_addPPPoETag(rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb,int l3Modify,int l4Modify)
{
	int i;
	u16 len;					
	u16 sid;
	int total_len;
#ifdef CONFIG_APOLLO_MODEL					
#else
	//add pppoe here
	if((u32)skb->data-8<(u32)skb->head)
	{
		FIXME("no more free skb buff in header room");
	}
	else					
#endif											
	{
		if(pPktHdr->tagif&IPV6_TAGIF)
		{
			total_len=pPktHdr->l3Offset+pPktHdr->l3Len-2+pPktHdr->ipv6PayloadLen;	// 2 bytes: ether type
			//DEBUG("l3offset is %d, l3len is %d, payload is %d = total_len is %d",
				//pPktHdr->l3Offset,pPktHdr->l3Len,pPktHdr->ipv6PayloadLen,total_len);
			len=total_len-14+2;	// 2 bytes: ppp
			//DEBUG("ipv6 new total_len is %d, len is %d",total_len,len);
			//sid=rg_db.pppoe[pPktHdr->netifIdx].rtk_pppoe.sessionID;
		}
		else
		{
			//DEBUG("the l3offset is %d, l3len is %d",pPktHdr->l3Offset,pPktHdr->l3Len);
			total_len=pPktHdr->l3Offset+pPktHdr->l3Len;
			len=total_len-14+2; // 2 bytes: ppp
			//DEBUG("new total len is %d, len is %d",total_len,len);
			//sid=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.wan_intf.pppoe_info.after_dial.sessionId;
		}
		sid=rg_db.pppoe[pPktHdr->netifIdx].rtk_pppoe.sessionID;
		//DEBUG("SID is %d",sid);
#if 0
//#ifdef CONFIG_APOLLO_RLE0371
		//checksum re-cal
		if(l3Modify)
		{
			*pPktHdr->pIpv4Checksum=0;
			*pPktHdr->pIpv4Checksum=htons(inet_chksum(skb->data+pPktHdr->l3Offset,pPktHdr->l4Offset-pPktHdr->l3Offset));
		}

		if(l4Modify)
		{
			*pPktHdr->pL4Checksum=0;					
			*pPktHdr->pL4Checksum=htons(inet_chksum_pseudo(skb->data+pPktHdr->l4Offset,total_len-pPktHdr->l4Offset,ntohl(*pPktHdr->pIpv4Sip),ntohl(*pPktHdr->pIpv4Dip),pPktHdr->ipProtocol));
		}
#endif	
		//DEBUG("before add pppoe header l3off=%d l4off=%d len=%d, total_len=%d",pPktHdr->l3Offset,pPktHdr->l4Offset,skb->len,total_len);

		skb->data-=8;
		//skb->len=total_len+8;
		skb->len+=8;

		//DA,SA
		for(i=0;i<pPktHdr->l3Offset-2;i++)
			skb->data[i]=skb->data[i+8];
		
		//reset DMAC and SMAC pointer
		pPktHdr->pDmac=&skb->data[0];
		pPktHdr->pSmac=&skb->data[6];

		//reset tagif and L4 offset
		pPktHdr->tagif|=PPPOE_TAGIF;
		if(pPktHdr->l4Offset>0)
			pPktHdr->l4Offset+=8;

		//8864
		skb->data[pPktHdr->l3Offset-2]=0x88;
		skb->data[pPktHdr->l3Offset-1]=0x64;

		//Code
		skb->data[pPktHdr->l3Offset]=0x11;
		skb->data[pPktHdr->l3Offset+1]=0;
		
		//session id
		skb->data[pPktHdr->l3Offset+2]=sid>>8;
		skb->data[pPktHdr->l3Offset+3]=sid&0xff;

		//len
		skb->data[pPktHdr->l3Offset+4]=len>>8;
		skb->data[pPktHdr->l3Offset+5]=len&0xff;
		
		//ppp = IPv4:0x0021, IPv6:0x0057
		if(pPktHdr->tagif&IPV6_TAGIF)
		{
			skb->data[pPktHdr->l3Offset+6]=0x00;
			skb->data[pPktHdr->l3Offset+7]=0x57;
		}
		else
		{
			skb->data[pPktHdr->l3Offset+6]=0x00;
			skb->data[pPktHdr->l3Offset+7]=0x21;
		}

		//DEBUG("add pppoe header l3off=%d l4off=%d len=%d",pPktHdr->l3Offset,pPktHdr->l4Offset,skb->len);
		//dump_packet(skb->data,skb->len,"pppoe_add_after");
	}
}

void _rtk_rg_removePPPoETag(rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb)
{
	//remove pppoe header	
	int i;
	int total_len;

	if(pPktHdr->tagif&IPV6_TAGIF)
	{
		total_len=pPktHdr->l3Offset+pPktHdr->l3Len-2+pPktHdr->ipv6PayloadLen;	// 2 bytes: ether type
		skb->data[pPktHdr->l3Offset-2]=0x86;
		skb->data[pPktHdr->l3Offset-1]=0xdd;
	}
	else
	{
		total_len=pPktHdr->l3Offset+pPktHdr->l3Len;			
		skb->data[pPktHdr->l3Offset-2]=0x08;
		skb->data[pPktHdr->l3Offset-1]=0x00;
	}

	//DEBUG("%s...total len is %d",pPktHdr->tagif&IPV6_TAGIF?"IPV6":"IPv4",total_len);
	//remove pppoe header here
	//DEBUG("tot_len=%d l3off=%d l3len=%d l4off=%d",total_len,pPktHdr->l3Offset,pPktHdr->l3Len,pPktHdr->l4Offset);
	//dump_packet(skb->data,skb->len,"pppoe_del_before");	

	//copy new DA/SA
	for(i=pPktHdr->l3Offset-3;i>=8;i--)
		skb->data[i]=skb->data[i-8];			
	//use memcpy to accelerate this copy process
	//memcpy(skb->data+16,skb->data+8,pPktHdr->l3Offset-18);
	//memcpy(skb->data+8,skb->data,8);
	
	//FIXME for vlan tag
	skb->data+=8;
	skb->len=total_len-8;			

	//dump_packet(skb->data,skb->len,"pppoe_del_after");	

	//reset DMAC and SMAC pointer
	pPktHdr->pDmac=&skb->data[0];
	pPktHdr->pSmac=&skb->data[6];

	//reset tagif and L4 offset
	pPktHdr->tagif&=(~PPPOE_TAGIF);
	if(pPktHdr->l4Offset>0)
		pPktHdr->l4Offset-=8;

//#ifdef CONFIG_APOLLO_RLE0371
#if 0
	// PATCH: must clear padding data for checksum offload.
	for(i=skb->len;i<60;i++)
		skb->data[i]=0;
#endif

	//DEBUG("remove pppoe header skb->len=%d l4_len=%d",skb->len,total_len-pPktHdr->l4Offset);
	//dump_packet(skb->data,skb->len,"pppoe_del");				
}

int _rtk_rg_fwdEngine_shortCutNaptPacketModify(int direct, int naptIdx, rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb,int l3Modify,int l4Modify)
{
	int l2Idx,ori_ttl=0;
	
	//DEBUG("direct is %s",direct==NAPT_DIRECTION_OUTBOUND?"OUTBOUND":"INBOUND");	
	//TTL minus one

	if(pPktHdr->pIpv4TTL!=NULL)
	{
		ori_ttl=*pPktHdr->pIpv4TTL;
		*pPktHdr->pIpv4TTL-=1;
	}
	else
		dump_packet(skb->data,skb->len,"NULL packet..");

	// fool-proofing & debug
	if(l3Modify || l4Modify) assert(naptIdx<MAX_NAPT_OUT_SW_TABLE_SIZE);
	
	if(direct==NAPT_DIRECTION_OUTBOUND)
	{
		l2Idx=pPktHdr->dmacL2Idx;
		assert((l2Idx!=FAIL) && (pPktHdr->dmacL2Idx<MAX_LUT_SW_TABLE_SIZE));
		/*
		FIXME("l2idx=%d mac=%02x:%02x:%02x:%02x:%02x:%02x\n",l2Idx,rg_db.mac[l2Idx].macAddr.octet[0]
		,rg_db.mac[l2Idx].macAddr.octet[1],rg_db.mac[l2Idx].macAddr.octet[2]
		,rg_db.mac[l2Idx].macAddr.octet[3],rg_db.mac[l2Idx].macAddr.octet[4]
		,rg_db.mac[l2Idx].macAddr.octet[5]);
		*/
		
		// fill DA
		memcpy(pPktHdr->pDmac,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);
		
		//fill SA
		assert(pPktHdr->netifIdx<MAX_NETIF_SW_TABLE_SIZE);
		memcpy(pPktHdr->pSmac,rg_db.netif[pPktHdr->netifIdx].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN);

		//fill SIP
		if(l3Modify)
			*pPktHdr->pIpv4Sip=htonl(rg_db.extip[pPktHdr->extipIdx].rtk_extip.extIpAddr);

		//fill SPORT
		if(l4Modify)
			*pPktHdr->pSport=htons(rg_db.naptOut[naptIdx].extPort);

		//add pppoe header
		if(rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.is_wan==1 &&
			rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			_rtk_rg_addPPPoETag(pPktHdr,skb,l3Modify,l4Modify);
			
			//fragment checksum re-cal because GMAC cannot offload PPPoE packets
			if(pPktHdr->ipv4FragPacket)
				*pPktHdr->pIpv4Checksum=htons(_rtk_rg_fwdengine_L3checksumUpdate(*pPktHdr->pIpv4Checksum,pPktHdr->ipv4Sip,ori_ttl,pPktHdr->ipProtocol,ntohl(*pPktHdr->pIpv4Sip),*pPktHdr->pIpv4TTL));
		}

	}
	else //INBOUND
	{					
		ipaddr_t originalDIP;
		int16 originalDPort;

		originalDIP=pPktHdr->ipv4Dip;
		originalDPort=pPktHdr->dport;

		//fill DIP
		if(l3Modify)
		{
			*pPktHdr->pIpv4Dip=htonl(rg_db.naptIn[naptIdx].rtk_naptIn.intIp);
			pPktHdr->ipv4Dip=rg_db.naptIn[naptIdx].rtk_naptIn.intIp;
		}

		_rtk_rg_routingDecisionTablesLookup(pPktHdr,SIP_DIP_CLASS_NAPTR);

		//fill DPORT
		if(l4Modify)
			*pPktHdr->pDport=htons(rg_db.naptIn[naptIdx].rtk_naptIn.intPort);


		//fill DA
		l2Idx=pPktHdr->dmacL2Idx;
		assert((l2Idx!=FAIL) && (pPktHdr->dmacL2Idx<MAX_LUT_SW_TABLE_SIZE));
		//assert_ok(rtk_rg_macEntry_find(&mac,&l2Idx));
		//memcpy(pPktHdr->pDmac,mac.mac.octet,6);

		//Check DMAC
#if 0
		{
			/* For inbound connection, the very first SYN packet will be forwarding to internal host but the MAC of host may be unknownd. */
			int zeroDA[6] = {0};
			int result = 0;
			result = memcmp(&zeroDA[0],pPktHdr->pDmac,ETHER_ADDR_LEN);

			//DA is not valid MAC address.
			if(result==0)
			{
				DEBUG("MAC miss....send arp packet...\n");
				_rtk_rg_fwdengine_handleArpMiss(pPktHdr);
				return RT_ERR_RG_L2_ENTRY_NOT_FOUND;
			}
		}
#else
		if(rg_db.lut[l2Idx].valid==0)
		{
			DEBUG("MAC miss....send arp packet...\n");
			_rtk_rg_fwdengine_handleArpMiss(pPktHdr);
			return RT_ERR_RG_L2_ENTRY_NOT_FOUND;
		}
		else
			memcpy(pPktHdr->pDmac,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);
#endif
		//fill SA
		assert(pPktHdr->netifIdx<MAX_NETIF_SW_TABLE_SIZE);
		memcpy(pPktHdr->pSmac,rg_db.netif[pPktHdr->netifIdx].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN);

		if(pPktHdr->matchSC==0)
		{
			//DEBUG("pPktHdr->matchSC==0");
			if((pPktHdr->tagif&TCP_TAGIF) && (
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
				((pPktHdr->tcpFlags.syn==0)&&(pPktHdr->tcpFlags.ack==1)) ||
#endif
				((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==1))))
			{
				/* Shortcut need to use original dip of inbound packet to compare shortcut table entry. */
				pPktHdr->ipv4Dip=originalDIP;
				pPktHdr->dport=originalDPort;
#ifdef CONFIG_ROME_NAPT_SHORTCUT
				_rtk_rg_naptShortcutUpdate(pPktHdr,NAPT_DIRECTION_INBOUND,naptIdx,1);
#endif
			}
		}

		if(pPktHdr->etherType==0x8864)
			_rtk_rg_removePPPoETag(pPktHdr,skb);
	}

	return RT_ERR_RG_OK;
}

int _rtk_rg_fwdEngine_naptPacketModify(int direct, int naptIdx, rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb,int l3Modify,int l4Modify)
{	
	int ret;
	assert_ok(_rtk_rg_egressACLPatternCheck(direct,naptIdx,pPktHdr,skb,l3Modify,l4Modify));
	
	ret = _rtk_rg_egressACLAction(pPktHdr);
	if(ret == RG_FWDENGINE_RET_TO_PS || ret == RG_FWDENGINE_RET_DROP)
		return ret;
	
	//rtk_rg_macEntry_t mac;
	
	//DEBUG("pPktHdr->dmacL2Idx=%d\n",pPktHdr->dmacL2Idx);

	//Alg pre function check
	_rtk_rg_algForward(direct,0,skb,pPktHdr);

	//DEBUG("skb is %p, pkthdr is %p, ttl is %p",skb,pPktHdr,pPktHdr->pIpv4TTL);
	ret = _rtk_rg_fwdEngine_shortCutNaptPacketModify(direct,naptIdx,pPktHdr,skb,l3Modify,l4Modify);

	//Alg post function check
	_rtk_rg_algForward(direct,1,skb,pPktHdr);

	//dump_packet(skb->data,skb->len,"new");
	return RG_FWDENGINE_RET_CONTINUE;
}

int _rtk_rg_fwdEngine_ipv6ShortCutPacketModify(int direct, int intfType,rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb)
{	
	int l2Idx;
	
	*pPktHdr->pIPv6HopLimit-=1;

	if(direct==NAPT_DIRECTION_OUTBOUND)
	{
		l2Idx=pPktHdr->dmacL2Idx;
		assert(l2Idx!=FAIL);
		/*
		FIXME("l2idx=%d mac=%02x:%02x:%02x:%02x:%02x:%02x\n",l2Idx,rg_db.mac[l2Idx].macAddr.octet[0]
		,rg_db.mac[l2Idx].macAddr.octet[1],rg_db.mac[l2Idx].macAddr.octet[2]
		,rg_db.mac[l2Idx].macAddr.octet[3],rg_db.mac[l2Idx].macAddr.octet[4]
		,rg_db.mac[l2Idx].macAddr.octet[5]);
		*/
		
		// fill DA
		memcpy(pPktHdr->pDmac,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);

		//fill SA
		memcpy(pPktHdr->pSmac,rg_db.netif[pPktHdr->netifIdx].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN);

		//add pppoe header
		if(intfType==L34_NH_PPPOE)
			_rtk_rg_addPPPoETag(pPktHdr,skb,0,0);

	}
	else //INBOUND
	{					
		//fill DA
		l2Idx=pPktHdr->dmacL2Idx;
		assert(l2Idx!=FAIL);
		//assert_ok(rtk_rg_macEntry_find(&mac,&l2Idx));
		//memcpy(pPktHdr->pDmac,mac.mac.octet,6);
		memcpy(pPktHdr->pDmac,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);

		//fill SA
		memcpy(pPktHdr->pSmac,rg_db.netif[pPktHdr->netifIdx].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN);

		//remove pppoe header
		if(pPktHdr->etherType==0x8864)
			_rtk_rg_removePPPoETag(pPktHdr,skb);
	}

	return RT_ERR_RG_OK;
}

int _rtk_rg_fwdEngine_ipv6PacketModify(int direct, int intfType,rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb)
{
	int ret;

	ret = _rtk_rg_egressACLAction(pPktHdr);
	if(ret == RG_FWDENGINE_RET_TO_PS || ret == RG_FWDENGINE_RET_DROP)
		return ret;

	//Hop Limit minus one with checking
	if(*pPktHdr->pIPv6HopLimit<=1)
		return RG_FWDENGINE_RET_TO_PS;

	//Alg pre function check
	_rtk_rg_algForward(direct,0,skb,pPktHdr);
	
	_rtk_rg_fwdEngine_ipv6ShortCutPacketModify(direct,intfType,pPktHdr,skb);

	//Alg post function check
	_rtk_rg_algForward(direct,1,skb,pPktHdr);

	return RT_ERR_RG_OK;
}

const unsigned char fragHashTable[256] =
{
    98,  6, 85,150, 36, 23,112,164,135,207,169,  5, 26, 64,165,219, //  1
    61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115, //  2
    90,168,156,203,177,120,  2,190,188,  7,100,185,174,243,162, 10, //  3
   237, 18,253,225,  8,208,172,244,255,126,101, 79,145,235,228,121, //  4
   123,251, 67,250,161,  0,107, 97,241,111,181, 82,249, 33, 69, 55, //  5
    59,153, 29,  9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222, //  6
   197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186, //  7
    39,158,178,187,131,136,  1, 49, 50, 17,141, 91, 47,129, 60, 99, //  8
   154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254, //  9
   133,232,196,144,198,124, 53,  4,108, 74,223,234,134,230,157,139, // 10
   189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44, // 11
   183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12, // 12
   221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166, // 13
     3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117, // 14
   238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110, // 15
    43,119,224, 71,122,142, 42,160,104, 48,247,103, 15, 11,138,239  // 16
};

unsigned int _rtk_rg_hashFun_xPear(uint8 len, uint8 ipProto, uint32 srcAddr, uint32 destAddr, uint16 identification)
{
	unsigned char *x, ch, h, i;
	unsigned int hex;
	unsigned short hh[len>>1];

	ch=ipProto; 
	for (i=0; i<(len>>1); i++)
	{
		// standard Pearson hash (output is h)
		h=0;
		h=fragHashTable[h ^ ch];		//ipProtocol
		if(srcAddr!=0)		//inbound hash will assign srcAddr as 0
		{
			x=(uint8 *)&srcAddr;
			h=fragHashTable[h ^ *x];
			h=fragHashTable[h ^ *(x+1)];
			h=fragHashTable[h ^ *(x+2)];
			h=fragHashTable[h ^ *(x+3)];
		}
		x=(uint8 *)&destAddr;
		h=fragHashTable[h ^ *x];
		h=fragHashTable[h ^ *(x+1)];
		h=fragHashTable[h ^ *(x+2)];
		h=fragHashTable[h ^ *(x+3)];
		x=(uint8 *)&identification;
		h=fragHashTable[h ^ *(x)];
		h=fragHashTable[h ^ *(x+1)];
		
		hh[i]=h;	// store result
		ch=ch+1; // increment first data byte by 1
	}

	//FIXME("hh[0].a is %02x, hh[1].a is %02x",hh[0].a,hh[1].a);
	if(len==4)
		hex = ((hh[0]&0xff)<<8) + (hh[1]&0xff);		//16bits//hex = ((hh[0].a&0xff)<<8) + (hh[1].a&0xff);		//16bits
	else
		hex = hh[0]&0xff;		//8bits//hex = hh[0].a&0xff;		//8bits
    
	return hex;
}

uint32 _rtk_rg_ipv4FragInHashIndex(uint8 ipProto, uint32 destAddr, uint16 identification)
{
	return _rtk_rg_hashFun_xPear(2,ipProto,0,destAddr,identification);
}

uint32 _rtk_rg_ipv4FragOutHashIndex(uint8 ipProto, uint32 srcAddr, uint32 destAddr, uint16 identification) 
{
	return _rtk_rg_hashFun_xPear(4,ipProto,srcAddr,destAddr,identification)&0x1ff;
}

int _rtk_rg_fwdEngine_fragmentPacketQueuing(rtk_rg_naptDirection_t direction, struct re_private *cp, struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	int8 i,queueIdx;
	long queueTime,compareTime;
#ifdef __KERNEL__
	struct timespec timeNow;
#endif
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragQueueLock);

#ifdef __KERNEL__
	//queueTime=jiffies;
	timeNow=current_kernel_time();
	queueTime=(long)timeNow.tv_sec;
#else
	queueTime=time(NULL);
#endif
	DEBUG("the queueTime is %ld",queueTime);
	queueIdx=0;
	compareTime=0;

	//find the first valid one or smallest queue_time(oldest) to use
	for(i=0;i<MAX_IPV4_FRAGMENT_QUEUE_SIZE;i++)
	{
		if(rg_db.ipv4FragmentQueue[i].occupied==0)
		{
			compareTime = 0;
			queueIdx = i;
			rg_db.systemGlobal.ipv4FragmentQueueNum++;
			break;
		}
		else if(rg_db.ipv4FragmentQueue[i].queue_time < compareTime)
		{
			compareTime = rg_db.ipv4FragmentQueue[i].queue_time;
			queueIdx = i;
		}
	}	
#ifdef __KERNEL__
	//free the oldest one skb
	if(compareTime>0)
		dev_kfree_skb_any(rg_db.ipv4FragmentQueue[queueIdx].queue_skb);
#endif

	//insert new packet into this idx
	rg_db.ipv4FragmentQueue[queueIdx].queue_skb=skb;
	memcpy(&rg_db.ipv4FragmentQueue[queueIdx].queue_pktHdr,pPktHdr,sizeof(rtk_rg_pktHdr_t));
	memcpy(&rg_db.ipv4FragmentQueue[queueIdx].queue_rx_info,pPktHdr->pRxDesc,sizeof(struct rx_info));
	memcpy(&rg_db.ipv4FragmentQueue[queueIdx].queue_cp,cp,sizeof(struct re_private));
	rg_db.ipv4FragmentQueue[queueIdx].queue_time=queueTime;
	rg_db.ipv4FragmentQueue[queueIdx].occupied=1;
	rg_db.ipv4FragmentQueue[queueIdx].direction=direction;

	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragQueueLock);

	return RT_ERR_RG_OK;
}

int _rtk_rg_fwdEngine_fragmentQueueProcessing(int aclRet, rtk_rg_pktHdr_t *pPktHdr)
{
	int i,ret,totalQueueNum;
	//If we are the first fragment packet:
	//Check the queue to see if there is any packet has same identification and SIP,DIP
	//Loop if match:
	//	no need to recompute the L3 checksum, let HW do it
	//	Forward the packet
	//	Move the last one to the proceed queue position
	//	Queue number --
	totalQueueNum=rg_db.systemGlobal.ipv4FragmentQueueNum;
	for(i=0;i<MAX_IPV4_FRAGMENT_QUEUE_SIZE&&totalQueueNum>0;i++)
	{
		//DEBUG("rg_db.systemGlobal.ipv4FragmentQueueNum is %d",rg_db.systemGlobal.ipv4FragmentQueueNum);
		//DEBUG("rg_db.ipv4FragmentQueue[i].occupied = %d",rg_db.ipv4FragmentQueue[i].occupied);
		//DEBUG("*pktHdr.pIpv4Identification is %x",*pktHdr.pIpv4Identification);
		//DEBUG("*rg_db.ipv4FragmentQueue[i].queue_pktHdr.pIpv4Identification = %x",*rg_db.ipv4FragmentQueue[i].queue_pktHdr.pIpv4Identification);
		//------------------ Critical Section start -----------------------//
		rg_lock(&rg_kernel.ipv4FragQueueLock);
		if(rg_db.ipv4FragmentQueue[i].occupied)
		{
			totalQueueNum--;
			if(*(pPktHdr->pIpv4Identification)==*(rg_db.ipv4FragmentQueue[i].queue_pktHdr.pIpv4Identification))
			{
				//DEBUG("queue [%d] Match!!",i);

				//Check ACL action by first packet
				if(aclRet==RG_FWDENGINE_RET_TO_PS)
					re8670_rx_skb(&rg_db.ipv4FragmentQueue[i].queue_cp,rg_db.ipv4FragmentQueue[i].queue_skb,&rg_db.ipv4FragmentQueue[i].queue_rx_info);
				else if(aclRet==RG_FWDENGINE_RET_DROP)
					dev_kfree_skb_any(rg_db.ipv4FragmentQueue[i].queue_skb);
				else
				{
					ret = _rtk_rg_fwdEngine_naptPacketModify(rg_db.ipv4FragmentQueue[i].direction,FAIL,&rg_db.ipv4FragmentQueue[i].queue_pktHdr,rg_db.ipv4FragmentQueue[i].queue_skb,1,0);		//L4Modify is 0, so naptIdx is don't care
					//assert_ok(ret);
					//Check ACL action
					if(ret == RG_FWDENGINE_RET_TO_PS)
						re8670_rx_skb(&rg_db.ipv4FragmentQueue[i].queue_cp,rg_db.ipv4FragmentQueue[i].queue_skb,&rg_db.ipv4FragmentQueue[i].queue_rx_info);	
					else if(ret == RG_FWDENGINE_RET_DROP)
						dev_kfree_skb_any(rg_db.ipv4FragmentQueue[i].queue_skb);

					//DEBUG("send the queued fragment packet [%d]!",i);
#ifdef CONFIG_APOLLO_MODEL			
#else
					//turn on txInfo mask, otherwise value won't be add
					rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
					rg_kernel.txDescMask.opts1.bit.ipcs=1;
					rg_kernel.txDescMask.opts1.bit.l4cs=1;
					rg_kernel.txDescMask.opts1.bit.cputag_ipcs=1;
					rg_kernel.txDescMask.opts1.bit.cputag_l4cs=1;
					rg_kernel.txDesc.opts1.bit.ipcs=1;
					rg_kernel.txDesc.opts1.bit.l4cs=0;		//these queue fragment packets are IP-packet
					rg_kernel.txDesc.opts1.bit.cputag_ipcs=1;
					rg_kernel.txDesc.opts1.bit.cputag_l4cs=0;
					rg_kernel.txDesc.opts2.bit.tx_vlan_action = 0x2;		//removing
					
					_rtk_rg_fwdEngineDirectTx(rg_db.ipv4FragmentQueue[i].queue_skb,&rg_db.ipv4FragmentQueue[i].queue_pktHdr);
					//re8686_send_with_txInfo(rg_db.ipv4FragmentQueue[i].queue_skb,&txDesc,0);
					//re8686_send_with_txInfo_and_mask(rg_db.ipv4FragmentQueue[i].queue_skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
#endif
				}
				rg_db.systemGlobal.ipv4FragmentQueueNum--;
				rg_db.ipv4FragmentQueue[i].occupied = 0;
				rg_db.ipv4FragmentQueue[i].queue_time = 0;
			}
		}
		//------------------ Critical Section End -----------------------//
		rg_unlock(&rg_kernel.ipv4FragQueueLock);
	}

	return RT_ERR_RG_OK;
}

int _rtk_rg_fwdEngine_fragmentOutHashIndexLookup(int *hashIdx, rtk_rg_ipv4_fragment_out_t **pRetFragOutList, uint8 ipProto, ipaddr_t srcAddr, ipaddr_t destAddr, uint16 identification)
{
	int ret=FAIL;
	uint32 naptHashOutIdx;
	//int naptInIdx;
	rtk_rg_ipv4_fragment_out_t *pFragOutList,*pNextFragList;
	rtk_rg_pkthdr_tagif_t layer4Type;

	if(ipProto==0x6)
		layer4Type=TCP_TAGIF;
	else if(ipProto==0x11)
		layer4Type=UDP_TAGIF;
	else
		layer4Type=ICMP_TAGIF;
	naptHashOutIdx=_rtk_rg_ipv4FragOutHashIndex(ipProto,srcAddr,destAddr,identification);
	//FIXME("the frag lookup index is %d",naptHashOutIdx);
	*hashIdx=naptHashOutIdx;
	//naptOutIdx=naptHashOutIdx<<2;

/*	
	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
		if(rg_db.ipv4FragmentOutTable[i].valid && rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry->valid)
		{
			//DEBUG("get the napt index is %d",rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex);
			naptInIdx=rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry->hashIdx;
			//DEBUG("srcAddr is %x, the intIP is %x",srcAddr,rg_db.naptIn[naptInIdx].rtk_naptIn.intIp);
			//DEBUG("isTcp is %d, naptIn.isTcp is %d",isTcp,rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp);
			//DEBUG("id is %x, fragoutTable id is %x",identification,rg_db.ipv4FragmentOutTable[i].identification);
			if((srcAddr==rg_db.naptIn[naptInIdx].rtk_naptIn.intIp)&&
				(isTcp==rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp)&&
				(identification==rg_db.ipv4FragmentOutTable[i].identification))
			{
				ret=rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex;							
				*fragIdx = i;
				*aclAct = rg_db.ipv4FragmentOutTable[i].aclAction;
				//DEBUG("found naptOutIdx = %d, fragIdx = %d",ret,i);
				break;
			}
		}		
	}
*/
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	pFragOutList=rg_db.pFragOutHashListHead[naptHashOutIdx];
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);
	//FIXME("after lock");
	while(pFragOutList!=NULL)
	{				
		pNextFragList=pFragOutList->pNext;
		if(pFragOutList->layer4Type&layer4Type)
		{
			if(layer4Type&ICMP_TAGIF)
			{
				if(identification==pFragOutList->identification &&
					srcAddr==pFragOutList->pktInfo.icmp.intIp)
				{
					*pRetFragOutList=pFragOutList;
					pFragOutList->beginIdleTime=jiffies;
					pFragOutList->pktCount++;
					//*icmpCtrlFlow=pFragOutList->pktInfo.pICMPCtrlFlow;
					//DEBUG("found ICMPCtrlFlow = %p, ret=%d",*icmpCtrlFlow,ret);
					//FIXME("get ICMP out list!");
					break;
				}
			}
			else	//TCP or UDP
			{
				if(identification==pFragOutList->identification &&
					srcAddr==pFragOutList->pktInfo.napt.intIp)
				{
					*pRetFragOutList=pFragOutList;
					pFragOutList->beginIdleTime=jiffies;
					pFragOutList->pktCount++;
					ret=pFragOutList->pktInfo.napt.NaptOutboundEntryIndex;							
					//DEBUG("found naptOutIdx = %d",ret);
					//FIXME("get NAPT out list!");
					break;
				}			
			}
		}
		if(pFragOutList->beginIdleTime+FRAGMENT_LIST_TIMEOUT <= jiffies)		//too old
		{
			//FIXME("free old %s %p",pFragOutList->pktType==FRAG_TYPE_ICMP?"ICMP":"NAPT",pFragOutList);
			//free it
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.ipv4FragLock);
			_rtk_rg_freeFragOutList(naptHashOutIdx,pFragOutList);

			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.ipv4FragLock);
		}
		
		//------------------ Critical Section start -----------------------//
		//rg_lock(&rg_kernel.ipv4FragLock);
		pFragOutList=pNextFragList;
		//------------------ Critical Section start -----------------------//
		//rg_unlock(&rg_kernel.ipv4FragLock);
	}
//FIXME("before unlock");
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.ipv4FragLock);
	
	return ret;
}

int _rtk_rg_fwdEngine_fillOutFragmentInfo(rtk_rg_fwdEngineReturn_t fragAction, unsigned int realNaptIdx, rtk_rg_pktHdr_t *pPktHdr, rtk_rg_ipv4_fragment_out_t **pFragList)
{
	//int i;
	//int isTCP=0;
	uint32 naptHashOutIdx;
	//uint32 naptOutIdx;
	rtk_rg_ipv4_fragment_out_t *pFragFreeOutList;
	
	//if(pPktHdr->tagif&TCP_TAGIF) isTCP=1;
	
	//then fill the fragment table
	naptHashOutIdx=_rtk_rg_ipv4FragOutHashIndex(pPktHdr->ipProtocol,pPktHdr->ipv4Sip,pPktHdr->ipv4Dip,ntohs(*pPktHdr->pIpv4Identification));
	//DEBUG("the fill out hash index is %d",naptHashOutIdx);
	//naptOutIdx=naptHashOutIdx<<2;
/*	
	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
		if(rg_db.ipv4FragmentOutTable[i].valid==0)
		{
			//DEBUG("the add fragment table index is %d, napt index is %d",i,realNaptIdx);
			rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex=realNaptIdx;
			rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry=&rg_db.naptOut[realNaptIdx].rtk_naptOut;
			rg_db.ipv4FragmentOutTable[i].identification=ntohs(*(pPktHdr->pIpv4Identification));
			rg_db.ipv4FragmentOutTable[i].valid=1;
			*fragIdx=i;
			ret=SUCCESS;
			break;
		}
	}
*/
	_rtk_rg_getFragOutFreeList(&pFragFreeOutList);

	if(pPktHdr->tagif&ICMP_TAGIF)	//ICMP
	{
		DEBUG("record icmp ctrl flow id=%x, intIp=%x",pPktHdr->ICMPIdentifier,pPktHdr->ipv4Sip);
		//pFragFreeOutList->pktInfo.icmp.identification=pPktHdr->ICMPIdentifier;
		pFragFreeOutList->pktInfo.icmp.intIp=pPktHdr->ipv4Sip;
	}
	else if(pPktHdr->tagif&TCP_TAGIF || pPktHdr->tagif&UDP_TAGIF)		//TCP or UDP
	{
		DEBUG("the add fragment table hashIdx is %d, frag action is %d",naptHashOutIdx,fragAction);
		pFragFreeOutList->pktInfo.napt.intIp=pPktHdr->ipv4Sip;
		pFragFreeOutList->pktInfo.napt.NaptOutboundEntryIndex=realNaptIdx;
		//pFragFreeOutList->pktInfo.napt.pNaptOutboundEntry=&rg_db.naptOut[realNaptIdx].rtk_naptOut;
		//pFragFreeOutList->pktInfo.napt.identification=ntohs(*(pPktHdr->pIpv4Identification));
	}
	*pFragList = pFragFreeOutList;
	pFragFreeOutList->layer4Type=pPktHdr->tagif;
	pFragFreeOutList->fragAction=fragAction;
	pFragFreeOutList->beginIdleTime=jiffies;
	pFragFreeOutList->identification=ntohs(*(pPktHdr->pIpv4Identification));
	pFragFreeOutList->pktCount=0;
	pFragFreeOutList->queueCount=0;

	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	if(rg_db.pFragOutHashListHead[naptHashOutIdx]==NULL)
	{
		rg_db.pFragOutHashListHead[naptHashOutIdx]=pFragFreeOutList;
	}
	else
	{		
		rg_db.pFragOutHashListHead[naptHashOutIdx]->pPrev=pFragFreeOutList;
		pFragFreeOutList->pNext=rg_db.pFragOutHashListHead[naptHashOutIdx];
		rg_db.pFragOutHashListHead[naptHashOutIdx]=pFragFreeOutList;	
	}
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);

	return SUCCESS;	
}

int _rtk_rg_fwdEngine_fragmentInHashIndexLookup(int *hashIdx, rtk_rg_ipv4_fragment_in_t **pRetFragInList, uint8 ipProto, ipaddr_t srcAddr, ipaddr_t destAddr, uint16 identification)
{
	int ret=FAIL;
	uint32 naptHashInIdx;
	//int naptOutIdx;
	rtk_rg_ipv4_fragment_in_t *pFragInList,*pNextFragList;
	rtk_rg_pkthdr_tagif_t layer4Type;

	if(ipProto==0x6)
		layer4Type=TCP_TAGIF;
	else if(ipProto==0x11)
		layer4Type=UDP_TAGIF;
	else
		layer4Type=ICMP_TAGIF;
	
	naptHashInIdx=_rtk_rg_ipv4FragInHashIndex(ipProto,destAddr,identification);
	//DEBUG("the inbound frag lookup index is %d",naptHashInIdx);
	*hashIdx=naptHashInIdx;
	//naptOutIdx=naptHashOutIdx<<2;

/*	
	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
		if(rg_db.ipv4FragmentOutTable[i].valid && rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry->valid)
		{
			//DEBUG("get the napt index is %d",rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex);
			naptInIdx=rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry->hashIdx;
			//DEBUG("srcAddr is %x, the intIP is %x",srcAddr,rg_db.naptIn[naptInIdx].rtk_naptIn.intIp);
			//DEBUG("isTcp is %d, naptIn.isTcp is %d",isTcp,rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp);
			//DEBUG("id is %x, fragoutTable id is %x",identification,rg_db.ipv4FragmentOutTable[i].identification);
			if((srcAddr==rg_db.naptIn[naptInIdx].rtk_naptIn.intIp)&&
				(isTcp==rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp)&&
				(identification==rg_db.ipv4FragmentOutTable[i].identification))
			{
				ret=rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex;							
				*fragIdx = i;
				*aclAct = rg_db.ipv4FragmentOutTable[i].aclAction;
				//DEBUG("found naptOutIdx = %d, fragIdx = %d",ret,i);
				break;
			}
		}		
	}
*/	
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	pFragInList=rg_db.pFragInHashListHead[naptHashInIdx];
	//------------------ Critical Section start -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);
	
	while(pFragInList!=NULL)
	{
		pNextFragList=pFragInList->pNext;
		if(pFragInList->layer4Type&layer4Type)
		{
			if(pFragInList->layer4Type&ICMP_TAGIF)
			{
				if(identification==pFragInList->identification&&
					srcAddr==pFragInList->pktInfo.icmp.remoteIp)
				{
					*pRetFragInList=pFragInList;
					pFragInList->beginIdleTime=jiffies;
					pFragInList->pktCount++;
					//*icmpCtrlFlow=pFragInList->pktInfo.pICMPCtrlFlow;
					DEBUG("found ICMPCtrlFlow, ret= %d, action=%d",ret,pFragInList->fragAction);
					break;
				}
			}
			else //TCP or UDP
			{
				if(identification==pFragInList->identification &&
					srcAddr==pFragInList->pktInfo.napt.remoteIp)
				{
					*pRetFragInList=pFragInList;
					pFragInList->beginIdleTime=jiffies;
					pFragInList->pktCount++;
					ret=pFragInList->pktInfo.napt.NaptOutboundEntryIndex;
					DEBUG("found naptOutIdx = %d, action=%d",ret,pFragInList->fragAction);
					break;
				}
			}
		}
		if(pFragInList->beginIdleTime+FRAGMENT_LIST_TIMEOUT <= jiffies)		//too old
		{
			FIXME("free old %s %p",pFragInList->layer4Type&ICMP_TAGIF?"ICMP":"NAPT",pFragInList);
			//free it
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.ipv4FragLock);
			
			_rtk_rg_freeFragInList(naptHashInIdx,pFragInList);

			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.ipv4FragLock);
		}
		//------------------ Critical Section start -----------------------//
		//rg_lock(&rg_kernel.ipv4FragLock);
		pFragInList=pNextFragList;
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.ipv4FragLock);
	}
	
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.ipv4FragLock);
	
	return ret;
}

int _rtk_rg_fwdEngine_fillInFragmentInfo(rtk_rg_fwdEngineReturn_t fragAction, int realNaptOutIdx, rtk_rg_table_icmp_flow_t *icmpCtrlFlow, rtk_rg_pktHdr_t *pPktHdr, rtk_rg_ipv4_fragment_in_t **pFragList)
{
	//int i;
	uint32 naptHashInIdx;
	//uint32 naptOutIdx;
	rtk_rg_ipv4_fragment_in_t *pFragFreeInList;
	
	//then fill the fragment table
	naptHashInIdx=_rtk_rg_ipv4FragInHashIndex(pPktHdr->ipProtocol,pPktHdr->ipv4Dip,ntohs(*pPktHdr->pIpv4Identification));
	//DEBUG("the fill in hash index is %d",naptHashInIdx);
	//naptOutIdx=naptHashOutIdx<<2;
	
/*	
	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
		if(rg_db.ipv4FragmentOutTable[i].valid==0)
		{
			//DEBUG("the add fragment table index is %d, napt index is %d",i,realNaptIdx);
			rg_db.ipv4FragmentOutTable[i].NaptOutboundEntryIndex=realNaptIdx;
			rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry=&rg_db.naptOut[realNaptIdx].rtk_naptOut;
			rg_db.ipv4FragmentOutTable[i].identification=ntohs(*(pPktHdr->pIpv4Identification));
			rg_db.ipv4FragmentOutTable[i].valid=1;
			*fragIdx=i;
			ret=SUCCESS;
			break;
		}
	}
*/
	_rtk_rg_getFragInFreeList(&pFragFreeInList);

	DEBUG("fragAction is %d",fragAction);
	if(pPktHdr->tagif&ICMP_TAGIF)		//ICMP
	{
		DEBUG("record icmp ctrl flow id=%x, remoteIp=%x,internalIp=%x",pPktHdr->ICMPIdentifier,pPktHdr->ipv4Sip,(icmpCtrlFlow!=NULL?icmpCtrlFlow->internalIP:0x0));
		//pFragFreeInList->pktInfo.icmp.identification=pPktHdr->ICMPIdentifier;
		pFragFreeInList->pktInfo.icmp.remoteIp=pPktHdr->ipv4Sip;
		if(icmpCtrlFlow!=NULL)pFragFreeInList->pktInfo.icmp.intIp=icmpCtrlFlow->internalIP;
	}
	else if(pPktHdr->tagif&TCP_TAGIF || pPktHdr->tagif&UDP_TAGIF)		//TCP or UDP 
	{
		DEBUG("the add fragment table hashindex is %d, napt index is %d",naptHashInIdx,realNaptOutIdx);
		pFragFreeInList->pktInfo.napt.remoteIp=pPktHdr->ipv4Sip;
		//pFragFreeInList->pktInfo.napt.NaptInboundEntryIndex=realNaptInIdx;
		pFragFreeInList->pktInfo.napt.NaptOutboundEntryIndex=realNaptOutIdx;		//FAIL if napt is not exist
		//pFragFreeInList->pktInfo.napt.pNaptInboundEntry=&rg_db.naptIn[realNaptInIdx].rtk_naptIn;
	}
	*pFragList = pFragFreeInList;
	pFragFreeInList->layer4Type=pPktHdr->tagif;
	pFragFreeInList->fragAction=fragAction;
	pFragFreeInList->beginIdleTime=jiffies;
	pFragFreeInList->identification=ntohs(*(pPktHdr->pIpv4Identification));
	pFragFreeInList->pktCount=0;
	pFragFreeInList->queueCount=0;
	
	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.ipv4FragLock);
	if(rg_db.pFragInHashListHead[naptHashInIdx]==NULL)
	{
		rg_db.pFragInHashListHead[naptHashInIdx]=pFragFreeInList;
	}
	else
	{
		rg_db.pFragInHashListHead[naptHashInIdx]->pPrev=pFragFreeInList;
		pFragFreeInList->pNext=rg_db.pFragInHashListHead[naptHashInIdx];
		rg_db.pFragInHashListHead[naptHashInIdx]=pFragFreeInList;	
	}
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.ipv4FragLock);

	return SUCCESS;	
}


int _rtk_rg_fwdEngine_TCPOutboundConnectionTracking(rtk_rg_pktHdr_t *pPktHdr, int *pNaptOutIdx)
{
	int naptInIdx;
	int ret=0;
	
	//*pNaptOutIdx=_rtk_rg_naptTcpUdpOutHashIndexLookup(1,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
	*pNaptOutIdx=_rtk_rg_naptTcpUdpOutHashIndexLookupByPktHdr(1,pPktHdr);

	//NAPT 4-ways not found, and not SYN packet, trap to CPU handle
	if(!((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==0)))
	{
		assert(*pNaptOutIdx!=FAIL);
		if(*pNaptOutIdx==FAIL) return RG_FWDENGINE_RET_DROP;
	}

	if(rg_db.naptOut[*pNaptOutIdx].state==TCP_CONNECTED)
	{
		if(pPktHdr->tcpFlags.fin==1)
		{	
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			rg_db.naptOut[*pNaptOutIdx].state=FIN_RECV;
		}
		else if(pPktHdr->tcpFlags.reset==1)
		{
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			rg_db.naptOut[*pNaptOutIdx].state=RST_RECV;
		}	
	}
	else
	{
		if((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==0))
		{		
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n%p",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset,rg_db.pNaptOutFreeListHead);

			//Lookup and write to free SW NAPT entry
			if(*pNaptOutIdx==FAIL)
			{
	            ret = _rtk_rg_fwdEngine_outbound_fillNaptInfo(pNaptOutIdx,pPktHdr);
				//DEBUG("set napt out[%d] software entry",*pNaptOutIdx);
	            if(ret!=SUCCESS) return RG_FWDENGINE_RET_TO_PS;
				assert(rg_db.naptOut[*pNaptOutIdx].state<=SYN_RECV);
				rg_db.naptOut[*pNaptOutIdx].state=SYN_RECV;
			}
		}
		else if((pPktHdr->tcpFlags.syn==0)&&(pPktHdr->tcpFlags.ack==1))
		{
			
			if(rg_db.naptOut[*pNaptOutIdx].state==SYN_ACK_RECV)
			{				
				//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
				assert(rg_db.naptOut[*pNaptOutIdx].state==SYN_ACK_RECV);
				naptInIdx = rg_db.naptOut[*pNaptOutIdx].rtk_naptOut.hashIdx;
				
				ret = _rtk_rg_naptConnection_add(*pNaptOutIdx,&rg_db.naptOut[*pNaptOutIdx].rtk_naptOut,&rg_db.naptIn[naptInIdx].rtk_naptIn);
				assert_ok(ret);

				//DEBUG("### add TCP naptOut[%d],naptIn[%d] ###\n",*pNaptOutIdx,naptInIdx);
#ifdef CONFIG_ROME_NAPT_SHORTCUT
				_rtk_rg_naptShortcutUpdate(pPktHdr,NAPT_DIRECTION_OUTBOUND,*pNaptOutIdx,1);
#endif
			}
		}
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
		else if((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==1))
		{
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			if(rg_db.naptOut[*pNaptOutIdx].state==0) 
			{
				//DEBUG("loss syn naptOutIdx=%d",*pNaptOutIdx); 				
				return RG_FWDENGINE_RET_TO_PS;
			}
			if(rg_db.naptOut[*pNaptOutIdx].state<=SYN_ACK_RECV)
			{
				rg_db.naptOut[*pNaptOutIdx].state=SYN_ACK_RECV;
#ifdef CONFIG_ROME_NAPT_SHORTCUT
				_rtk_rg_naptShortcutUpdate(pPktHdr,NAPT_DIRECTION_OUTBOUND,*pNaptOutIdx,1);
#endif
			}
		}
#endif
	}
	return RG_FWDENGINE_RET_NAPT_OK;
}

int _rtk_rg_fwdEngine_TCPInboundConnectionTracking(rtk_rg_pktHdr_t *pPktHdr, int *pNaptOutIdx)
{
	int naptInIdx;
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
	int ret=0;
	ipaddr_t transIP=pPktHdr->ipv4Dip;
	uint16 transPort=pPktHdr->dport;
#endif

	*pNaptOutIdx=_rtk_rg_naptTcpUdpInHashIndexLookup(1,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);

	if(!((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==0)))
	{
		//NAPT 4-ways not found, and not SYN packet, trap to CPU handle
		assert(*pNaptOutIdx!=FAIL);
		if(*pNaptOutIdx==FAIL) return RG_FWDENGINE_RET_TO_PS;
	}

#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
	//There are no NAPT flows, lookup UPNP/virtual server/DMZ tables.
	if(*pNaptOutIdx==FAIL)
	{
		ret = _rtk_rg_fwdEngine_connType_lookup(pPktHdr,&transIP,&transPort);
		if(ret!=RT_ERR_RG_OK) return RG_FWDENGINE_RET_TO_PS;
	}
#endif

	if((*pNaptOutIdx!=FAIL) && (rg_db.naptOut[*pNaptOutIdx].state==TCP_CONNECTED))
	{
		if(pPktHdr->tcpFlags.fin==1)
		{	
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			rg_db.naptOut[*pNaptOutIdx].state=FIN_RECV;
		}
		else if(pPktHdr->tcpFlags.reset==1)
		{
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			rg_db.naptOut[*pNaptOutIdx].state=RST_RECV;
		}
	}
	else
	{
		if((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==1))
		{							
			assert(*pNaptOutIdx!=FAIL);
			naptInIdx=rg_db.naptOut[*pNaptOutIdx].rtk_naptOut.hashIdx;
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			if(rg_db.naptOut[*pNaptOutIdx].state==0) 
			{
				DEBUG("loss syn naptOutIdx=%d",*pNaptOutIdx); 				
				return RG_FWDENGINE_RET_TO_PS;
			}
			if(rg_db.naptOut[*pNaptOutIdx].state<=SYN_ACK_RECV)
			{
				rg_db.naptOut[*pNaptOutIdx].state=SYN_ACK_RECV;
			}
		}
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
		// Inbound SYN packet
		else if((pPktHdr->tcpFlags.syn==1)&&(pPktHdr->tcpFlags.ack==0))
		{	
			//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
			//Lookup and write to free SW NAPT entry
			if(*pNaptOutIdx==FAIL)
			{
	            ret = _rtk_rg_fwdEngine_inbound_fillNaptInfo(pNaptOutIdx,pPktHdr,transIP,transPort);

				//naptInIdx = rg_db.naptOut[*pNaptOutIdx].rtk_naptOut.hashIdx;
				//DEBUG("set napt in[%d] out[%d] software entry",naptInIdx,*pNaptOutIdx);
	            if(ret!=SUCCESS) return RG_FWDENGINE_RET_TO_PS;
				assert(rg_db.naptOut[*pNaptOutIdx].state<=SYN_RECV);
				rg_db.naptOut[*pNaptOutIdx].state=SYN_RECV;
			}
		}
		else if((pPktHdr->tcpFlags.syn==0)&&(pPktHdr->tcpFlags.ack==1))
		{
			
			assert(*pNaptOutIdx!=FAIL);
			if(rg_db.naptOut[*pNaptOutIdx].state==SYN_ACK_RECV)
			{				
				int ret = 0;
				//DEBUG("SYN:%d ACK:%d FIN:%d RST:%d\n",pPktHdr->tcpFlags.syn,pPktHdr->tcpFlags.ack,pPktHdr->tcpFlags.fin,pPktHdr->tcpFlags.reset);
				assert(rg_db.naptOut[*pNaptOutIdx].state==SYN_ACK_RECV);
				naptInIdx = rg_db.naptOut[*pNaptOutIdx].rtk_naptOut.hashIdx;

				ret = _rtk_rg_naptConnection_add(*pNaptOutIdx,&rg_db.naptOut[*pNaptOutIdx].rtk_naptOut,&rg_db.naptIn[naptInIdx].rtk_naptIn);
				assert_ok(ret);

				rtlglue_printf("### add TCP naptOut[%d],naptIn[%d] ###\n",*pNaptOutIdx,naptInIdx);
			}
		}
#endif
	}
	return RG_FWDENGINE_RET_NAPT_OK;
}

int _rtk_rg_fwdEngine_UDPOutboundConnectionTracking(rtk_rg_pktHdr_t *pPktHdr, int *retIdx)
{
	int naptOutIdx=0,naptInIdx=0;
	int ret=0;

	//naptOutIdx=_rtk_rg_naptTcpUdpOutHashIndexLookup(0,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
	naptOutIdx=_rtk_rg_naptTcpUdpOutHashIndexLookupByPktHdr(0,pPktHdr);
	if(naptOutIdx==FAIL)
	{
		//Lookup and set NAPT SW tables
		ret = _rtk_rg_fwdEngine_outbound_fillNaptInfo(&naptOutIdx,pPktHdr);
		if(ret!=SUCCESS) return RG_FWDENGINE_RET_TO_PS;
		rg_db.naptOut[naptOutIdx].state=UDP_FIRST;
		//DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
	}
	else if(rg_db.naptOut[naptOutIdx].state==UDP_FIRST)
	{
		rg_db.naptOut[naptOutIdx].state=UDP_SECOND;
		//DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
	}
	else if(rg_db.naptOut[naptOutIdx].state==UDP_SECOND)
	{
		rg_db.naptOut[naptOutIdx].state=UDP_CONNECTED;
		//DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
		//Add NAPT connection to ASIC
		naptInIdx = rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;
		ret = _rtk_rg_naptConnection_add(naptOutIdx,&rg_db.naptOut[naptOutIdx].rtk_naptOut,&rg_db.naptIn[naptInIdx].rtk_naptIn);
		assert_ok(ret);

		DEBUG("### add UDP naptOut[%d],naptIn[%d] extPort=%d ###\n",naptOutIdx,naptInIdx,rg_db.naptOut[naptOutIdx].extPort);
#ifdef CONFIG_ROME_NAPT_SHORTCUT
		_rtk_rg_naptShortcutUpdate(pPktHdr,NAPT_DIRECTION_OUTBOUND,naptOutIdx,1);	
#endif
	}

	*retIdx = naptOutIdx;

	return RG_FWDENGINE_RET_NAPT_OK;
}

int _rtk_rg_fwdEngine_ICMPOutboundControlFlowTracking(rtk_rg_pktHdr_t *pPktHdr,rtk_rg_table_icmp_flow_t **icmpCtrlList)
{
	rtk_rg_table_icmp_flow_t *pEntry;

	//create new control flow in link-list
	pEntry=&rg_db.pICMPCtrlFlowHead->icmpFlow;
	rg_db.pICMPCtrlFlowHead=rg_db.pICMPCtrlFlowHead->pNext;

	pEntry->valid=1;
	pEntry->internalIP=pPktHdr->ipv4Sip;
	pEntry->remoteIP=pPktHdr->ipv4Dip;
	pEntry->IPID=ntohs(*pPktHdr->pIpv4Identification);
	//pEntry->ICMPType=pPktHdr->ICMPType;
	//pEntry->ICMPCode=pPktHdr->ICMPCode;
	pEntry->ICMPID=pPktHdr->ICMPIdentifier;

	*icmpCtrlList=pEntry;
	//DEBUG("adding Outbound: sip=%08x, dip=%08x ipid=%x, type is %d, code is %d, identifier is %d",
		//pEntry->internalIP, pEntry->remoteIP, pEntry->IPID, pEntry->ICMPType, pEntry->ICMPCode, pEntry->ICMPID);

	return RG_FWDENGINE_RET_NAPT_OK;
}

rtk_rg_table_icmp_flow_t * _rtk_rg_fwdEngine_ICMPInboundControlFlowTracking(rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_table_icmp_linkList_t *pEntry;
	
	//lookup all link-list
	for(pEntry=rg_db.pICMPCtrlFlowHead->pPrev;pEntry!=rg_db.pICMPCtrlFlowHead;pEntry=pEntry->pPrev)
	{
		if(pEntry->icmpFlow.valid==1 && 
			pEntry->icmpFlow.ICMPID==pPktHdr->ICMPIdentifier &&
			pEntry->icmpFlow.remoteIP==pPktHdr->ipv4Sip)
		{
			//match
			return &pEntry->icmpFlow;
		}
	}

	//unmatch
	return NULL;
}
#if 0
rtk_rg_table_icmp_flow_t * _rtk_rg_fwdEngine_ICMPOutboundFragmentLookup(rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_table_icmp_linkList_t *pEntry;
	
	//lookup all link-list
	for(pEntry=rg_db.pICMPCtrlFlowHead->pPrev;pEntry!=rg_db.pICMPCtrlFlowHead;pEntry=pEntry->pPrev)
	{
		if(pEntry->icmpFlow.valid==1 && pEntry->icmpFlow.internalIP==pPktHdr->ipv4Sip && pEntry->icmpFlow.IPID==ntohs(*pPktHdr->pIpv4Identification))
		{
			//match
			return &pEntry->icmpFlow;
		}
	}

	//unmatch
	return NULL;
}

rtk_rg_table_icmp_flow_t * _rtk_rg_fwdEngine_ICMPInboundFragmentLookup(rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_table_icmp_linkList_t *pEntry;
	
	//lookup all link-list
	for(pEntry=rg_db.pICMPCtrlFlowHead->pPrev;pEntry!=rg_db.pICMPCtrlFlowHead;pEntry=pEntry->pPrev)
	{
		if(pEntry->icmpFlow.valid==1 && pEntry->icmpFlow.remoteIP==pPktHdr->ipv4Sip && pEntry->icmpFlow.inboundIPID==ntohs(*pPktHdr->pIpv4Identification))
		{
			//match
			return &pEntry->icmpFlow;
		}
	}

	//unmatch
	return NULL;
}
#endif

#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
int _rtk_rg_fwdEngine_UDPInboundConnectionTracking(rtk_rg_pktHdr_t *pPktHdr, int *retIdx)
{
	int naptOutIdx=0,naptInIdx=0;
	int ret=0;
	ipaddr_t transIP = pPktHdr->ipv4Dip;
	uint16 transPort = pPktHdr->dport;

	naptOutIdx=_rtk_rg_naptTcpUdpInHashIndexLookup(0,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
	if(naptOutIdx==FAIL)
	{
		ret = _rtk_rg_fwdEngine_connType_lookup(pPktHdr,&transIP,&transPort);
		if(ret!=RT_ERR_RG_OK) return RG_FWDENGINE_RET_TO_PS;

		//Lookup and set NAPT SW tables
		ret = _rtk_rg_fwdEngine_inbound_fillNaptInfo(&naptOutIdx,pPktHdr,transIP,transPort);
		if(ret!=SUCCESS) return RG_FWDENGINE_RET_TO_PS;
		rg_db.naptOut[naptOutIdx].state=UDP_FIRST;
		DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
	}
	else if(rg_db.naptOut[naptOutIdx].state==UDP_FIRST)
	{
		rg_db.naptOut[naptOutIdx].state=UDP_SECOND;
		DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
	}
	else if(rg_db.naptOut[naptOutIdx].state==UDP_SECOND)
	{
		rg_db.naptOut[naptOutIdx].state=UDP_CONNECTED;
		DEBUG("UDP state=%d extport=%d\n",rg_db.naptOut[naptOutIdx].state,rg_db.naptOut[naptOutIdx].extPort);
		//Add NAPT connection to ASIC
		naptInIdx = rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;
		ret = _rtk_rg_naptConnection_add(naptOutIdx,&rg_db.naptOut[naptOutIdx].rtk_naptOut,&rg_db.naptIn[naptInIdx].rtk_naptIn);
		DEBUG("### add UDP naptOut[%d],naptIn[%d] extPort=%d ###\n",naptOutIdx,naptInIdx,rg_db.naptOut[naptOutIdx].extPort);
		pPktHdr->ipv4Dip = rg_db.naptIn[naptInIdx].rtk_naptIn.intIp;
#ifdef CONFIG_ROME_NAPT_SHORTCUT
		_rtk_rg_naptShortcutUpdate(pPktHdr,NAPT_DIRECTION_INBOUND,naptInIdx,1);
#endif
		assert_ok(ret);
	}

	*retIdx = naptOutIdx;

	return RG_FWDENGINE_RET_NAPT_OK;
}
#endif

int _rtk_rg_fwdengine_handleArpMiss(rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_arp_request_t arpReq;
	ipaddr_t ipAddr;
	int ret;
	
	arpReq.finished=0;
	arpReq.gwMacReqCallBack=NULL;
	arpReq.reqIp=pPktHdr->ipv4Dip;
	
	ret=_rtk_rg_routingDecisionTablesLookup(pPktHdr,SIP_DIP_CLASS_ROUTING);
	if(ret!=RG_FWDENGINE_RET_CONTINUE) return ret;
	
	if(rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.is_wan==1)
	{
			ipAddr=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].p_wanStaticInfo->ip_addr;
	}
	else
	{
			ipAddr=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.lan_intf.ip_addr;
	}
	
	if(pPktHdr->ipv4Dip!=ipAddr) //skip gateway ip arp request
	{
			DEBUG("ARP [0x%x] Miss, Send ARP Request!",pPktHdr->ipv4Dip);
			_rtk_rg_arpGeneration(&rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo,ipAddr,&arpReq);
	}

	return RG_FWDENGINE_RET_L2FORWARDED;
}

int _rtk_rg_fwdengine_handleNeighborMiss(rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_neighbor_discovery_t neighborDisc;
	rtk_ipv6_addr_t ipAddr;
	int ret,netIfIdx;
	//DEBUG("handle neighbor miss!");
	//_rtk_rg_v6RoutingDecisionTablesLookup(pPktHdr,0 /*It's not LAN to WAN*/);
	ret=_rtk_rg_v6L3lookup(pPktHdr->pIpv6Dip);
	//DEBUG("the v6L3lookup ret is %d",ret);
	if(ret<0)return RG_FWDENGINE_RET_L2FORWARDED;

	if(rg_db.v6route[ret].rtk_v6route.type == L34_IPV6_ROUTE_TYPE_LOCAL)
	{
		netIfIdx=rg_db.v6route[ret].rtk_v6route.nhOrIfidIdx;
	}
	else if(rg_db.v6route[ret].rtk_v6route.type == L34_IPV6_ROUTE_TYPE_GLOBAL)
	{
		netIfIdx=rg_db.nexthop[rg_db.v6route[ret].rtk_v6route.nhOrIfidIdx].rtk_nexthop.ifIdx;
	}
	else
		return RG_FWDENGINE_RET_L2FORWARDED;
		
	if(rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo.is_wan==1)
	{
			memcpy(ipAddr.ipv6_addr,rg_db.systemGlobal.interfaceInfo[netIfIdx].p_wanStaticInfo->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
	}
	else
	{
			memcpy(ipAddr.ipv6_addr,rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo.lan_intf.ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
	}
	/*DEBUG("the ipaddr is %08x:%08x:%08x:%08x",*(unsigned int *)ipAddr.ipv6_addr,
		*(unsigned int *)(ipAddr.ipv6_addr+4),
		*(unsigned int *)(ipAddr.ipv6_addr+8),
		*(unsigned int *)(ipAddr.ipv6_addr+12));*/
	if(memcmp(pPktHdr->pIpv6Dip,ipAddr.ipv6_addr,IPV6_ADDR_LEN)>0)	//skip gateway ip neighbor discovery
	{
		neighborDisc.finished=0;
		neighborDisc.ipv6GwMacReqCallBack=NULL;
		memcpy(neighborDisc.reqIp.ipv6_addr,pPktHdr->pIpv6Dip,IPV6_ADDR_LEN);
		DEBUG("Neighbor [0x%08x:%08x:%08x:%08x] Miss, Send Neighbor Discovery!",
			*(unsigned int *)pPktHdr->pIpv6Dip,
			*(unsigned int *)(pPktHdr->pIpv6Dip+4),
			*(unsigned int *)(pPktHdr->pIpv6Dip+8),
			*(unsigned int *)(pPktHdr->pIpv6Dip+12));
		_rtk_rg_NDGeneration(&rg_db.systemGlobal.interfaceInfo[ret].storedInfo,ipAddr,&neighborDisc);
		return RG_FWDENGINE_RET_TO_PS;
	}

	return RG_FWDENGINE_RET_L2FORWARDED;
}

uint16 _rtk_rg_CompareByte(uint8 char_a, uint8 char_b, uint8 mask)
{
	uint8 tmp_a,tmp_b,res;
	tmp_a = char_a&mask;
	tmp_b = char_b&mask;
	res = tmp_a^tmp_b;
	if (res == 0)	//tmp_a == tmp_b
		return 1;
	else
		return 0;
}

int _rtk_rg_egressACLAction(rtk_rg_pktHdr_t *pPktHdr)
{
	int i;
	rtk_rg_aclFilterEntry_t* pChkRule;
	//for(i=(MAX_ACL_ENTRY_SIZE-1);i<0;i--){ //reverse setting action, because the miner rule the higher action priority.
	if(rg_db.systemGlobal.acl_SW_table_entry_size<=0){//no rule need to verify
		return RG_FWDENGINE_RET_CONTINUE;
	}

	for(i=0;i<MAX_ACL_ENTRY_SIZE;i++){
		if(pPktHdr->aclDecision.aclIgrHit[i]==ENABLED && pPktHdr->aclDecision.aclEgrHit[i]==ENABLED){
			pPktHdr->aclDecision.aclHit = ENABLED; //any rule hit, then can not add this flow to short cut.
			pChkRule = &(rg_db.systemGlobal.acl_SW_table_entry[i]);

			if(pChkRule->acl_filter.action_type==ACL_ACTION_TYPE_DROP){
				DEBUG(" ACL[%d] DROP",i);
				return RG_FWDENGINE_RET_DROP;	
			}else if(pChkRule->acl_filter.action_type==ACL_ACTION_TYPE_TRAP_TO_PS){
				DEBUG(" ACL[%d] TRAP_TO_PS",i);
				return RG_FWDENGINE_RET_TO_PS;
			}else if(pChkRule->acl_filter.action_type==ACL_ACTION_TYPE_PERMIT){
//				DEBUG(" ACL[%d] CONTINUE",i);
				return RG_FWDENGINE_RET_CONTINUE;
			}else{ //Qos Type
				pPktHdr ->aclDecision.action_type = ACL_ACTION_TYPE_QOS;
				
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_1P_REMARKING_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_1P_REMARKING_BIT)){//ACL_ACTION_1P_REMARKING_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_1P_REMARKING_BIT;
						pPktHdr->aclDecision.action_dot1p_remarking_pri = pChkRule->acl_filter.action_dot1p_remarking_pri;
						DEBUG(" ACL[%d] enable ACL_ACTION_1P_REMARKING_BIT",i);
					}
				}
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT || pPktHdr->aclDecision.qos_actions&ACL_ACTION_DSCP_REMARKING_BIT)){//both  ACL_ACTION_1P_REMARKING_BIT & ACL_ACTION_DSCP_REMARKING_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT;
						pPktHdr->aclDecision.action_ip_precedence_remarking_pri= pChkRule->acl_filter.action_ip_precedence_remarking_pri;
						DEBUG(" ACL[%d] enable ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT",i);
					}	
				}
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_DSCP_REMARKING_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT || pPktHdr->aclDecision.qos_actions&ACL_ACTION_DSCP_REMARKING_BIT)){//both  ACL_ACTION_1P_REMARKING_BIT & ACL_ACTION_DSCP_REMARKING_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_DSCP_REMARKING_BIT;
						pPktHdr->aclDecision.action_dscp_remarking_pri= pChkRule->acl_filter.action_dscp_remarking_pri;
						DEBUG(" ACL[%d] enable ACL_ACTION_DSCP_REMARKING_BIT",i);
					}
				}
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_QUEUE_ID_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_QUEUE_ID_BIT)){//ACL_ACTION_QUEUE_ID_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_QUEUE_ID_BIT;
						pPktHdr->aclDecision.action_queue_id= pChkRule->acl_filter.action_queue_id;
						DEBUG(" ACL[%d] enable ACL_ACTION_QUEUE_ID_BIT",i);
					}
				}
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_SHARE_METER_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_SHARE_METER_BIT)){//ACL_ACTION_QUEUE_ID_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_SHARE_METER_BIT;
						pPktHdr->aclDecision.action_share_meter= pChkRule->acl_filter.action_share_meter;
						DEBUG(" ACL[%d] enable ACL_ACTION_SHARE_METER_BIT",i);
					}
				}
				if(pChkRule->acl_filter.qos_actions & ACL_ACTION_STREAM_ID_OR_LLID_BIT){
					if(!(pPktHdr->aclDecision.qos_actions&ACL_ACTION_STREAM_ID_OR_LLID_BIT)){//ACL_ACTION_QUEUE_ID_BIT have not been set
						pPktHdr->aclDecision.qos_actions |= ACL_ACTION_STREAM_ID_OR_LLID_BIT;
						pPktHdr->aclDecision.action_stream_id_or_llid= pChkRule->acl_filter.action_stream_id_or_llid;
						DEBUG(" ACL[%d] enable ACL_ACTION_STREAM_ID_OR_LLID_BIT",i);
					}
				}
			}
		}
	}
#if 0	
	DEBUG("===ACL ACTION FINAL DECISION:===");
	DEBUG(" 1P_REMARKING[%s]: vid=%d",pPktHdr->aclDecision.qos_actions&ACL_ACTION_1P_REMARKING_BIT?"O":"X",pPktHdr->aclDecision.action_dot1p_remarking_pri);
	DEBUG(" IP_PRECEDENCE_REMARKING[%s]: ip_pre=0x%x",pPktHdr->aclDecision.qos_actions&ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT?"O":"X",pPktHdr->aclDecision.action_ip_precedence_remarking_pri);
	DEBUG(" DSCP_REMARKING[%s]: dscp=%d",pPktHdr->aclDecision.qos_actions&ACL_ACTION_DSCP_REMARKING_BIT?"O":"X",pPktHdr->aclDecision.action_dscp_remarking_pri);
	DEBUG(" QUEUE_ID[%s]: qid=%d",pPktHdr->aclDecision.qos_actions&ACL_ACTION_QUEUE_ID_BIT?"O":"X",pPktHdr->aclDecision.action_queue_id);
	DEBUG(" SHARE_METER[%s]: shmeter=%d",pPktHdr->aclDecision.qos_actions&ACL_ACTION_SHARE_METER_BIT?"O":"X",pPktHdr->aclDecision.action_share_meter);
	DEBUG(" STREAM_ID[%s]: sid=%d",pPktHdr->aclDecision.qos_actions&ACL_ACTION_STREAM_ID_OR_LLID_BIT?"O":"X",pPktHdr->aclDecision.action_stream_id_or_llid);
#endif

	return RG_FWDENGINE_RET_CONTINUE;
}


//int _rtk_rg_egressACLPatternCheck(rtk_rg_pktHdr_t *pPktHdr)
int _rtk_rg_egressACLPatternCheck(int direct, int naptIdx, rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb,int l3Modify,int l4Modify)
{
	int i;
	rtk_rg_aclFilterEntry_t* pChkRule;
	ipaddr_t sipModify,dipModify;
	uint16 sportModify,dportModify;
	
	if(rg_db.systemGlobal.acl_SW_table_entry_size<=0)//no rule need to verify
		return RT_ERR_RG_OK;

	//get original info
	sipModify = pPktHdr->ipv4Sip;
	dipModify = pPktHdr->ipv4Dip;
	sportModify = pPktHdr->sport; 
	dportModify = pPktHdr->dport;

	//get L34 info (translate SIP/DIP  SPORT/DPORT)
	if(direct==NAPT_DIRECTION_OUTBOUND)
	{

		//fill SIP
		if(l3Modify)
			sipModify=rg_db.extip[pPktHdr->extipIdx].rtk_extip.extIpAddr;

		//fill SPORT
		if(l4Modify)
			sportModify=rg_db.naptOut[naptIdx].extPort;

		// egress intf index has been set by _rtk_rg_layer34Forward()
		
	}
	else if(direct==NAPT_DIRECTION_INBOUND)//INBOUND
	{					
		//fill DIP
		if(l3Modify)
			dipModify=rg_db.naptIn[naptIdx].rtk_naptIn.intIp;
		
		//fill DPORT
		if(l4Modify)
			dportModify=rg_db.naptIn[naptIdx].rtk_naptIn.intPort;

		//set egress intf index to pktHdr
		_rtk_rg_routingDecisionTablesLookup(pPktHdr,SIP_DIP_CLASS_NAPTR);

	}else{
		//bridge mode! no need to modify
	}


	//check pattern	
	for(i=0;i<MAX_ACL_ENTRY_SIZE;i++){
		if(pPktHdr->aclDecision.aclIgrHit[i]==DISABLED)//ignor ingress not hit rules
			continue;
		//debug("CHECK EGRESS ACL[%d]:",i);
	
		pChkRule = &(rg_db.systemGlobal.acl_SW_table_entry[i]);

		if(pChkRule->acl_filter.filter_fields&EGRESS_INTF_BIT){
			//FIXME: is that enought by just judge SMAC?
			if(rg_db.netif[pChkRule->acl_filter.egress_intf_idx].rtk_netif.valid==DISABLED){
				ACL("EGRESS_INTF UNHIT");
				continue;
			}
			if(pChkRule->acl_filter.egress_intf_idx != pPktHdr->netifIdx){
				ACL("EGRESS_INTF UNHIT");
				continue;
			}	
		}
		if(pChkRule->acl_filter.filter_fields&EGRESS_IPV4_SIP_RANGE_BIT){
			if( !((pChkRule->acl_filter.egress_src_ipv4_addr_end >= sipModify ) &&
				(pChkRule->acl_filter.egress_src_ipv4_addr_start<= sipModify ))){
				//debug(" sipModify=0x%x",sipModify);
				//ACL("EGRESS_src_ipv4_addr_start=0x%x egress_src_ipv4_addr_end=0x%x",pChkRule->acl_filter.egress_src_ipv4_addr_start,pChkRule->acl_filter.egress_src_ipv4_addr_end);
				ACL("EGRESS_IPV4_SIP_RANGE UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&EGRESS_IPV4_DIP_RANGE_BIT){
			if( !((pChkRule->acl_filter.egress_dest_ipv4_addr_end >= dipModify ) &&
				(pChkRule->acl_filter.egress_dest_ipv4_addr_start<= dipModify))){
				//debug(" dipModify=0x%x",dipModify);
				//ACL("EGRESS_dest_ipv4_addr_start=0x%x egress_dest_ipv4_addr_end=0x%x",pChkRule->acl_filter.egress_dest_ipv4_addr_start,pChkRule->acl_filter.egress_dest_ipv4_addr_end);
				ACL("EGRESS_IPV4_DIP_RANGE UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&EGRESS_L4_SPORT_RANGE_BIT){
			if( !((pChkRule->acl_filter.egress_src_l4_port_end >= sportModify ) &&
				(pChkRule->acl_filter.egress_src_l4_port_start<= sportModify ))){			
				ACL("EGRESS_L4_SPORT_RANGE UNHIT");
				continue;
			}

		}
		if(pChkRule->acl_filter.filter_fields&EGRESS_L4_DPORT_RANGE_BIT){
			if( !((pChkRule->acl_filter.egress_dest_l4_port_end >= dportModify ) &&
				(pChkRule->acl_filter.egress_dest_l4_port_start<= dportModify))){
				ACL("EGRESS_L4_DPORT_RANGE UNHIT");
				continue;
			}
		}

		//record CF hit rule
		pPktHdr->aclDecision.aclEgrHit[i]=ENABLED;

	}

	return RT_ERR_RG_OK;

}


int _rtk_rg_ingressACLPatternCheck(rtk_rg_pktHdr_t *pPktHdr)
{

	int i;
	uint8 port;
	rtk_rg_aclFilterEntry_t* pChkRule;
	int ipv6_upper_range_check,ipv6_lowwer_range_check;
	int port_check;
	uint8 tos;

	if(rg_db.systemGlobal.acl_SW_table_entry_size<=0)//no rule need to verify
		return RT_ERR_RG_OK;

	for(i=0;i<MAX_ACL_ENTRY_SIZE;i++){
		//debug("CHECK INGRESS ACL[%d]:",i);
		
		pChkRule = &(rg_db.systemGlobal.acl_SW_table_entry[i]);


		if(pChkRule->acl_filter.filter_fields==0x0)//empty rule
			continue;

		if(pChkRule->acl_filter.action_type==ACL_ACTION_TYPE_TRAP)//trap to fwdEngine ignor
			continue;


		/*ingress rule check*/
		port_check = FAIL;
		if(pChkRule->acl_filter.filter_fields&INGRESS_PORT_BIT){
			for(port=0;port<RTK_RG_MAX_MAC_PORT;port++){
				if(pChkRule->acl_filter.ingress_port_mask.portmask & (1<<port)){
					if(port==pPktHdr->pRxDesc->opts3.bit.src_port_num){
						port_check = SUCCESS;
					}
				}
			}
			if(port_check!=SUCCESS){
				ACL("INGRESS_PORT UNHIT");
				continue;
			}
		}


		
		if(pChkRule->acl_filter.filter_fields&INGRESS_INTF_BIT){
			//FIXME:judge rule is same as HWNAT, however may be not enought for ever case.
			//check this intf exist
			if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].valid==DISABLED){
				ACL("INGRESS_INTF UNHIT");
				continue;
			}


			if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.is_wan==1){//wan intf
				if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){//bridge wan
					//check vlan
					if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==1){//vlan should tagged
						if(pPktHdr->ctagVid!=rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id){
							ACL("INGRESS_INTF UNHIT");
							continue;
						}
					}else{//vlan should untagged
						if(pPktHdr->tagif & CVLAN_TAGIF){
							ACL("INGRESS_INTF UNHIT");
							continue;
						}		
					}
				}else{//L34 Wan
					if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==1){//vlan should tagged
						//vid==gw_vid && da==gw_mac
						if(pPktHdr->ctagVid!=rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id){
							ACL("INGRESS_INTF UNHIT");
							continue;
						}
						if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[0]!=pPktHdr->pDmac[0] ||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[1]!=pPktHdr->pDmac[1]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[2]!=pPktHdr->pDmac[2]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[3]!=pPktHdr->pDmac[3]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[4]!=pPktHdr->pDmac[4]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[5]!=pPktHdr->pDmac[5]
							){
								ACL("INGRESS_INTF UNHIT");
								continue;
						}
					}else{//vlan should untagged & da==gw_mac
						if(pPktHdr->tagif & CVLAN_TAGIF){
							ACL("INGRESS_INTF UNHIT");
							continue;
						}	
						if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[0]!=pPktHdr->pDmac[0] ||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[1]!=pPktHdr->pDmac[1]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[2]!=pPktHdr->pDmac[2]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[3]!=pPktHdr->pDmac[3]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[4]!=pPktHdr->pDmac[4]||
							rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[5]!=pPktHdr->pDmac[5]
							){
								ACL("INGRESS_INTF UNHIT");
								continue;
						}
					}
					
				}

			}else{//lan intf
				//check spa is in lan_port_mask
				port_check = FAIL;
				for(port=0;port<RTK_RG_MAX_MAC_PORT;port++){
					if(rg_db.systemGlobal.interfaceInfo[pChkRule->acl_filter.ingress_intf_idx].storedInfo.lan_intf.port_mask.portmask & (1<<port)){
						if(port==pPktHdr->pRxDesc->opts3.bit.src_port_num){
							port_check = SUCCESS;
						}
					}
				}
				if(port_check!=SUCCESS){
					ACL("INGRESS_INTF UNHIT");
					continue;
				}
			}
		}
		
		if(pChkRule->acl_filter.filter_fields&INGRESS_ETHERTYPE_BIT){
			if(pChkRule->acl_filter.ingress_ethertype!=pPktHdr->etherType){
//				ACL("INGRESS_ETHERTYPE UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_CTAG_PRI_BIT){
			if(pChkRule->acl_filter.ingress_ctag_pri!=pPktHdr->ctagPri){
				ACL("INGRESS_CTAG_PRI UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_CTAG_VID_BIT){
			if(pChkRule->acl_filter.ingress_ctag_vid!=pPktHdr->ctagVid){
				ACL("INGRESS_CTAG_VID UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_SMAC_BIT){
			if(pChkRule->acl_filter.ingress_smac.octet[0]!=pPktHdr->pSmac[0] ||
				pChkRule->acl_filter.ingress_smac.octet[1]!=pPktHdr->pSmac[1]||
				pChkRule->acl_filter.ingress_smac.octet[2]!=pPktHdr->pSmac[2]||
				pChkRule->acl_filter.ingress_smac.octet[3]!=pPktHdr->pSmac[3]||
				pChkRule->acl_filter.ingress_smac.octet[4]!=pPktHdr->pSmac[4]||
				pChkRule->acl_filter.ingress_smac.octet[5]!=pPktHdr->pSmac[5]
				){
				//memDump(pPktHdr->pSmac,6,"pktHdr pSmac");
				//memDump(pChkRule->acl_filter.ingress_smac.octet,6,"acl Smac");
				ACL("INGRESS_SMAC UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_DMAC_BIT){
			if(pChkRule->acl_filter.ingress_dmac.octet[0]!=pPktHdr->pDmac[0] ||
				pChkRule->acl_filter.ingress_dmac.octet[1]!=pPktHdr->pDmac[1]||
				pChkRule->acl_filter.ingress_dmac.octet[2]!=pPktHdr->pDmac[2]||
				pChkRule->acl_filter.ingress_dmac.octet[3]!=pPktHdr->pDmac[3]||
				pChkRule->acl_filter.ingress_dmac.octet[4]!=pPktHdr->pDmac[4]||
				pChkRule->acl_filter.ingress_dmac.octet[5]!=pPktHdr->pDmac[5]
				){
				//memDump(pPktHdr->pDmac,6,"pktHdr pDmac");
				//memDump(pChkRule->acl_filter.ingress_dmac.octet,6,"acl Dmac");
				ACL("INGRESS_DMAC UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_DSCP_BIT){
			if(pPktHdr->pTos==NULL){//pkt without DSCP
				continue;
			}else{
				tos = *(pPktHdr->pTos);
				if(pChkRule->acl_filter.ingress_dscp!=(tos>>2)){
					ACL("INGRESS_DSCP UNHIT");
					continue;
				}
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_L4_TCP_BIT){
			if(	!(pPktHdr->tagif&TCP_TAGIF)){
				ACL("INGRESS_L4_TCP UNHIT");
				continue;
			}
		}
		if(pChkRule->acl_filter.filter_fields&INGRESS_L4_UDP_BIT){
			if(	!(pPktHdr->tagif&UDP_TAGIF)){
				ACL("INGRESS_L4_UDP UNHIT");
				continue;
			}
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_L4_ICMP_BIT){
			if(	!(pPktHdr->tagif&IGMP_TAGIF)){
				ACL("INGRESS_L4_ICMP UNHIT");
				continue;
			}
		}	
		if(pChkRule->acl_filter.filter_fields&INGRESS_IPV6_SIP_RANGE_BIT){
			ipv6_upper_range_check = -1;
			ipv6_lowwer_range_check = -1;
			if(pPktHdr->pIpv6Sip != NULL){
				ipv6_upper_range_check = memcmp(&(pChkRule->acl_filter.ingress_src_ipv6_addr_end[0]),pPktHdr->pIpv6Sip,16);
				ipv6_lowwer_range_check = memcmp(pPktHdr->pIpv6Sip,&(pChkRule->acl_filter.ingress_src_ipv6_addr_start[0]),16);
			}
			if(!(ipv6_upper_range_check>=0 && ipv6_lowwer_range_check>=0 )){
				ACL("INGRESS_IPV6_SIP_RANGE UNHIT");
				continue;
			}
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_IPV6_DIP_RANGE_BIT){
			ipv6_upper_range_check = -1;
			ipv6_lowwer_range_check = -1;
			if(pPktHdr->pIpv6Dip!=NULL){
				ipv6_upper_range_check = memcmp(&(pChkRule->acl_filter.ingress_dest_ipv6_addr_end[0]),pPktHdr->pIpv6Dip,16);
				ipv6_lowwer_range_check = memcmp(pPktHdr->pIpv6Dip,&(pChkRule->acl_filter.ingress_dest_ipv6_addr_start[0]),16);
			}
			if(!(ipv6_upper_range_check>=0 && ipv6_lowwer_range_check>=0 )){
				ACL("INGRESS_IPV6_DIP_RANGE UNHIT");
				continue;
			}
			
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_IPV4_SIP_RANGE_BIT){
			if( !((pChkRule->acl_filter.ingress_src_ipv4_addr_end >= pPktHdr->ipv4Sip ) &&
				(pChkRule->acl_filter.ingress_src_ipv4_addr_start<= pPktHdr->ipv4Sip ))){
				ACL("INGRESS_IPV4_SIP_RANGE UNHIT");
				continue;
			}
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_IPV4_DIP_RANGE_BIT){
			if( !((pChkRule->acl_filter.ingress_dest_ipv4_addr_end >= pPktHdr->ipv4Dip ) &&
				(pChkRule->acl_filter.ingress_dest_ipv4_addr_start<= pPktHdr->ipv4Dip ))){
				ACL("INGRESS_IPV4_DIP_RANGE UNHIT");
				continue;
			}
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_L4_SPORT_RANGE_BIT){
			if( !((pChkRule->acl_filter.ingress_src_l4_port_end >= pPktHdr->sport ) &&
				(pChkRule->acl_filter.ingress_src_l4_port_start<= pPktHdr->sport ))){
				ACL("INGRESS_L4_SPORT_RANGE UNHIT");
				continue;
			}
		}		
		if(pChkRule->acl_filter.filter_fields&INGRESS_L4_DPORT_RANGE_BIT){
			if( !((pChkRule->acl_filter.ingress_dest_l4_port_end >= pPktHdr->dport ) &&
				(pChkRule->acl_filter.ingress_dest_l4_port_start<= pPktHdr->dport ))){
				ACL("INGRESS_L4_DPORT_RANGE UNHIT");
				continue;
			}
		}	

		//record ACL hit rule
		pPktHdr->aclDecision.aclIgrHit[i]=ENABLED;

	}
	
	return RT_ERR_RG_OK;
}

int _rtk_rg_modifyPacketByACLAction(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	unsigned char tos;
	//1 FIXME: at the moment, we only care QoS actions.
	if(pPktHdr->aclDecision.action_type==ACL_ACTION_TYPE_QOS)
	{
		if((pPktHdr->aclDecision.qos_actions&ACL_ACTION_1P_REMARKING_BIT)>0)
		{
			//Check tx desc for vlan tag or not
			if(rg_kernel.txDescMask.opts2.bit.tx_vlan_action==0x3)
			{
				switch(rg_kernel.txDesc.opts2.bit.tx_vlan_action)
				{
					case 0x2:		//vlan remove, do not remarking 1p priority
						break;
					case 0x1:		//vlan insert, always do 1p remarking
						rg_kernel.txDescMask.opts2.bit.prio=0x7;
						rg_kernel.txDesc.opts2.bit.prio=pPktHdr->aclDecision.action_dot1p_remarking_pri&0x7;
						break;
					case 0x0:		//vlan intact or
					case 0x3:		//vlan remarking should check rx desc, if tagged, remarking 1p, otherwise don't remarking
						if(pPktHdr->pRxDesc->opts2.bit.ctagva==1)
						{
							if(rg_kernel.txDesc.opts3.bit.tx_portmask==0)	//hardware lookup, use cputag_pri to overlap hardware internal priority
							{
								rg_kernel.txDescMask.opts2.bit.aspri=0x1;
								rg_kernel.txDescMask.opts2.bit.cputag_pri=0x7;
								rg_kernel.txDesc.opts2.bit.aspri=0x1;
								rg_kernel.txDesc.opts2.bit.cputag_pri=pPktHdr->aclDecision.action_dot1p_remarking_pri&0x7;
							}
							else	//Direct TX
							{
								rg_kernel.txDescMask.opts2.bit.prio=0x7;
								rg_kernel.txDesc.opts2.bit.prio=pPktHdr->aclDecision.action_dot1p_remarking_pri&0x7;
							}
						}
						break;
					default:
						break;
				}
			}
		}
		if((pPktHdr->aclDecision.qos_actions&ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT)>0)
		{
			if(pPktHdr->pTos==NULL)goto ACL_RET;		//packet may not have IP header
			
			if(pPktHdr->tagif&IPV6_TAGIF)
			{
				//ip precedence is the MSB 3 bits of traffic class
				tos = pPktHdr->aclDecision.action_ip_precedence_remarking_pri<<0x1;
				tos |= (*pPktHdr->pTos)&0xf1;		//keep version 4 bits and original traffic class MSB 4th bit
				*pPktHdr->pTos=tos;
			}
			else
			{	
				tos = pPktHdr->aclDecision.action_ip_precedence_remarking_pri<<0x5;
				tos |= (*pPktHdr->pTos)&0x1f;		//keep 5 bits from LSB
				*pPktHdr->pTos=tos;		//remarking tos of packet
			}
		}
		else if((pPktHdr->aclDecision.qos_actions&ACL_ACTION_DSCP_REMARKING_BIT)>0)
		{
			if(pPktHdr->pTos==NULL)goto ACL_RET;		//packet may not have IP header
			
			if(pPktHdr->tagif&IPV6_TAGIF)
			{
				//dscp is the MSB 6 bits of traffic class
				tos = pPktHdr->aclDecision.action_dscp_remarking_pri>>0x2;	//dscp MSB 4 bits
				tos |= (*pPktHdr->pTos)&0xf0;		//keep version 4 bits
				*pPktHdr->pTos=tos;

				tos = (pPktHdr->aclDecision.action_dscp_remarking_pri&0x3)<<0x6;	//dscp LSB 2 bits
				tos |= (*(pPktHdr->pTos+1))&0x3f;		//keep original traffic label LSB 2 bits and flow label MSB 4 bits
				*(pPktHdr->pTos+1)=tos;
			}
			else
			{
				tos = pPktHdr->aclDecision.action_dscp_remarking_pri<<0x2;
				tos |= (*pPktHdr->pTos)&0x3;		//keep 2 bits from LSB
				*pPktHdr->pTos=tos;		//remarking tos of packet
			}
		}
	}
ACL_RET:
	
	return RT_ERR_RG_OK;
}


#ifdef __KERNEL__

int _rtk_rg_ipv6L34Forward(struct re_private *cp, struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	int i,ret=0;
	
	int8 defRouteEntry = -1;
	int8 matchEntry = -1;
	int8 gatewayIP = -1;
	//int8 compareBit;
	uint8 nb_hash_idx;
	//uint16 index;
	//uint16 max;
	uint16 res;
	uint16 matchNeighbor;
	uint32 /*mask=0,*/toWANIntf;
	
	rtk_enable_t pppoemode;
	rtk_l34_nexthop_entry_t *nexthopEntry;
	rtk_ipv6Routing_entry_t *entry;
	rtk_ipv6Neighbor_entry_t *neighbor;
	
	//check ingress PPPoE
	ASSERT_EQ(rtk_l34_globalState_get(L34_GLOBAL_PPPKEEP_STATE,&pppoemode),RT_ERR_OK);	
	if((pPktHdr->tagif&PPPOE_TAGIF) && (pppoemode==DISABLED))
	{
		for (i=0;i<MAX_PPPOE_SW_TABLE_SIZE;i++)
		{
			if(pPktHdr->sessionId == rg_db.pppoe[i].rtk_pppoe.sessionID)
				break;
		}
		if (i==MAX_PPPOE_SW_TABLE_SIZE)	//unmatch, trap
		{
			DEBUG("IPV6: PPPoE ID lookup miss, trap to CPU !!!!");
			return RG_FWDENGINE_RET_TO_PS;
		}
	}

	//do the normal Route
	/*for (i=0;i < MAX_IPV6_ROUTING_SW_TABLE_SIZE; i++)
	{
		entry=&rg_db.v6route[i].rtk_v6route;
		if (entry->valid == 1)
		{
			if(memcmp(pPktHdr->pIpv6Dip,entry->ipv6Addr.ipv6_addr,IPV6_ADDR_LEN)==0)	//completly match
			{
				gatewayIP=i;
				break;
			}
			max = entry->ipv6PrefixLen;
			res = 1;

			//check for default route
			if(max == 0)
			{
				//default route
				//MT_WATCH("%d, default route!",i);
				defRouteEntry = i;
				continue;
			}

			max = entry->ipv6PrefixLen>>3;

			//check destination address of prefix length
			for (index = 0; index < max;index++)
			{
				compareBit = pPktHdr->pIpv6Dip[index]^entry->ipv6Addr.ipv6_addr[index];
				if (compareBit != 0)
				{
					//unmatched...
					res = 0;
					//MT_WATCH("%d, unmatched!",i);
					break;
				}
			}
			//check for the last few bits
			if (res == 1)
			{
				max = entry->ipv6PrefixLen % 8;
				mask = 0xff00>>max;
				res = _rtk_rg_CompareByte(pPktHdr->pIpv6Dip[index], entry->ipv6Addr.ipv6_addr[index], mask&0xff);
			}
			
			if (res == 1)
			{	
				//MT_WATCH("Routing table %d matched!",i);
				matchEntry = i;
				break;
			}
		}
	}*/

	matchEntry=_rtk_rg_v6L3lookup(pPktHdr->pIpv6Dip);
	if(matchEntry>=0)
	{
		//Check gwteway IP
		if(memcmp(pPktHdr->pIpv6Dip,rg_db.v6route[matchEntry].rtk_v6route.ipv6Addr.ipv6_addr,IPV6_ADDR_LEN)==0)
			gatewayIP=matchEntry;
		//Check for default route
		else if(rg_db.v6route[matchEntry].rtk_v6route.ipv6PrefixLen==0)
			defRouteEntry=matchEntry;
	}
	
	//either packet-to-gateway or neighbor advertisement
	if(gatewayIP != -1 || pPktHdr->ICMPv6Type==0x88)	
	{
		_rtk_rg_neighborAgent(skb,pPktHdr);
		return RG_FWDENGINE_RET_TO_PS;
	}

	//if there is no entry matched and default route is valid
	if (defRouteEntry != -1)
	{
		//DEBUG("IPv6: Default routing..");
		matchEntry = defRouteEntry;
		goto IPV6_GLOBAL_ROUTING;
	}
	else if (matchEntry == -1)
	{
		//no entry matched, Trap
		//DEBUG("IPv6: routing table lookup failed, Trap to CPU!");
		return RG_FWDENGINE_RET_TO_PS;
	} 
	else
	{
		//hit! check process column
		switch (rg_db.v6route[matchEntry].rtk_v6route.type)
		{
			case L34_IPV6_ROUTE_TYPE_DROP:
				//sprintf(msg,"After bindForward, Drop!");
				//DEBUG("IPv6: Routing table said: Drop!");
				return RG_FWDENGINE_RET_DROP;
			case L34_IPV6_ROUTE_TYPE_TRAP:
				//sprintf(msg,"After bindForward, trap to CPU!");
				//DEBUG("IPv6: Routing table said: Trap to CPU!");
				return RG_FWDENGINE_RET_TO_PS;
			case L34_IPV6_ROUTE_TYPE_GLOBAL:
				/* Read NextHop */
				//DEBUG("IPv6: Global routing..");
				//since the entry should be the one we matched when we left the loop,
				//there is no need to get it out once again!
IPV6_GLOBAL_ROUTING:
				entry=&rg_db.v6route[matchEntry].rtk_v6route;		
				/* Read NextHop */
				nexthopEntry=&rg_db.nexthop[entry->nhOrIfidIdx].rtk_nexthop;
				pPktHdr->dmacL2Idx=nexthopEntry->nhIdx;
				pPktHdr->netifIdx=nexthopEntry->ifIdx;

				if(entry->rt2waninf==1)
					toWANIntf=NAPT_DIRECTION_OUTBOUND;
				else
					toWANIntf=NAPT_DIRECTION_INBOUND;
				
				ret = _rtk_rg_fwdEngine_ipv6PacketModify(toWANIntf,nexthopEntry->type,pPktHdr,skb);
				if(ret!=RT_ERR_RG_OK)
					return ret;		//RG_FWDENGINE_RET_TO_PS || RG_FWDENGINE_RET_DROP)
				//assert_ok(ret);
				return RG_FWDENGINE_RET_DIRECT_TX;
			case L34_IPV6_ROUTE_TYPE_LOCAL:
				//DEBUG("IPv6: Local routing..");
				//lookup for neighbor table
				res = 0;
				nb_hash_idx = _rtk_rg_IPv6NeighborHash(pPktHdr->pIpv6Dip+8, (uint8)matchEntry);
				
				for(i=0;i<8;i++)
				{
					matchNeighbor = (nb_hash_idx<<3)+i;
					//rtlglue_printf("the matchNeighbor idx = %d\n",matchNeighbor);
					//ASSERT_EQ(dal_apollomp_l34_ipv6NeighborTable_get(matchNeighbor, &neighbor), RT_ERR_OK);
					neighbor = &rg_db.v6neighbor[matchNeighbor].rtk_v6neighbor;
					//rtlglue_printf("the neighbor.ipv6RouteIdx = %d, matchEntry = %d\n",neighbor.ipv6RouteIdx,matchEntry);
					if(neighbor->valid && 
						(neighbor->ipv6RouteIdx == matchEntry)&&
						(_rtk_rg_CompareIFID(pPktHdr->pIpv6Dip+8, neighbor->ipv6Ifid)))
					{
						//rtlglue_printf("HIT!!!!!\n");
						//DEBUG("IPv6: Hit neighbor table!L2IDX = %d rt=%d",neighbor->l2Idx,matchEntry);
						res = 1;
						break;
					}
				}
				
				if (res == 0)
				{
					//if not hit, trap to cpu
					//DEBUG("IPv6: Neighbor table un-hit, Trap to CPU!");
					return RG_FWDENGINE_RET_TO_PS;
				}
				
				entry=&rg_db.v6route[matchEntry].rtk_v6route;		

				pPktHdr->dmacL2Idx=neighbor->l2Idx;
				pPktHdr->netifIdx=entry->nhOrIfidIdx&0x7;
				
				if(entry->rt2waninf==1)
					toWANIntf=NAPT_DIRECTION_OUTBOUND;
				else
					toWANIntf=NAPT_DIRECTION_INBOUND;

				ret = _rtk_rg_fwdEngine_ipv6PacketModify(toWANIntf,L34_NH_ETHER,pPktHdr,skb);
				//assert_ok(ret);
				if(ret!=RT_ERR_RG_OK)
					return ret;		//RG_FWDENGINE_RET_TO_PS || RG_FWDENGINE_RET_DROP)
				return RG_FWDENGINE_RET_DIRECT_TX;
			default:
				//we should not get here
				assert_ok(0);
				break;
		}				
	}

	return RG_FWDENGINE_RET_TO_PS;
}


int _rtk_rg_layer34Forward(struct re_private *cp, struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
//	int isL34=0;
	//int sipArpIdx;
	rtk_rg_ipClassification_t sipClass,dipClass;
	rtk_rg_sipDipClassification_t sipDipClass;
	//u8 *pData=skb->data;
	//u32 len=skb->len;
	int ret=0;
	uint32 naptOutIdx=0,naptInIdx=0;
	int fragIdx=FAIL,forwardLength;
	uint16 ori_srcPort;
	uint32 ori_srcIP;
	rtk_rg_table_icmp_flow_t *icmpCtrlFlow=NULL;
	rtk_rg_ipv4_fragment_out_t *pFragList=NULL;
	rtk_rg_ipv4_fragment_in_t *pFragInList=NULL;

	//DEBUG("L34 Input");
	TRACE("L34 Input with reason=%d\n",pPktHdr->pRxDesc->opts3.bit.reason);
	//if(pPktHdr->isGatewayPacket==0) return FAIL;

	if(pPktHdr->tagif&IPV6_TAGIF)
		return _rtk_rg_ipv6L34Forward(cp,skb,pPktHdr);

	if((pPktHdr->pIpv4TTL!=NULL) && (*pPktHdr->pIpv4TTL<=1)) return RG_FWDENGINE_RET_TO_PS;

	//pPktHdr->sipL3Idx=0;
	sipClass=_rtk_rg_sip_classification(pPktHdr->ipv4Sip,pPktHdr);
	dipClass=_rtk_rg_dip_classification(pPktHdr->ipv4Dip,pPktHdr);
	sipDipClass=rg_db.sipDipClass[sipClass][dipClass];

	//DEBUG("sipClass=%d,dipClass=%d,sipDipClass=%d\n",sipClass,dipClass,sipDipClass);

	if(pPktHdr->tagif&IPV4_TAGIF)
	{
		//dump_packet(skb->data,skb->len,"learn arp");
		//DEBUG("L34 learning ARP...");
		_rtk_rg_arpAndMacEntryAdd(pPktHdr->ipv4Sip,pPktHdr->sipL3Idx,pPktHdr->pSmac,pPktHdr->ingressPort,NULL,0,pPktHdr->internalVlanID,0,0);
	}

	if(sipDipClass==SIP_DIP_CLASS_NAPT)
	{
		//Check ALG
		ret=_rtk_rg_algFunctionCheck(NAPT_DIRECTION_OUTBOUND,pPktHdr);
		//DEBUG("the algFunction ret=%d",ret);
		if(ret==RG_FWDENGINE_RET_TO_PS)return ret;	//hit ALG and registered function is NULL
	
		ret=_rtk_rg_routingDecisionTablesLookup(pPktHdr,SIP_DIP_CLASS_NAPT);
		if(ret!=RG_FWDENGINE_RET_CONTINUE) return ret;
		//DEBUG("skb is %p, ppkthdr is %p, l3offset is %d, l3len is %d",skb,pPktHdr,pPktHdr->l3Offset,pPktHdr->l3Len);
		//DEBUG("before mf and fragoffset check:mf=%d,fragoffset=%d",pPktHdr->ipv4MoreFragment,pPktHdr->ipv4FragmentOffset);
		if(pPktHdr->ipv4FragPacket==0) //normal packet (non-fragment)
		{
#ifdef CONFIG_RG_NAPT_TCP_AUTO_LEARN
			if(pPktHdr->tagif&TCP_TAGIF)
			{
				ret = _rtk_rg_fwdEngine_TCPOutboundConnectionTracking(pPktHdr,&naptOutIdx);
				if(ret!=RG_FWDENGINE_RET_NAPT_OK)return ret;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;

//					dump_packet(skb->data,skb->len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,naptOutIdx, pPktHdr,skb,1,1);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//dump_packet(skb->data,skb->len,"new");
				return RG_FWDENGINE_RET_DIRECT_TX;				
			}
#endif

			//UDP auto-learning
			if(pPktHdr->tagif&UDP_TAGIF)
			{
				ret = _rtk_rg_fwdEngine_UDPOutboundConnectionTracking(pPktHdr, &naptOutIdx);
				if(ret!=RG_FWDENGINE_RET_NAPT_OK)return ret;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;

				//dump_packet(pData,len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,naptOutIdx,pPktHdr,skb,1,1);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//dump_packet(skb->data,skb->len,"new"); 		
				return RG_FWDENGINE_RET_DIRECT_TX;		
			}

			//ICMP forwarding
			if(pPktHdr->tagif&ICMP_TAGIF)
			{
				//DEBUG("before add ICMP control flow!! pPktHdr->ICMPOverMTU is %d",pPktHdr->ICMPOverMTU);
				_rtk_rg_fwdEngine_ICMPOutboundControlFlowTracking(pPktHdr,&icmpCtrlFlow);

				//Check MTU
				if(pPktHdr->ICMPOverMTU)return RG_FWDENGINE_RET_TO_PS;
				
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,FAIL,pPktHdr,skb,1,0);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//dump_packet(skb->data,skb->len,"new"); 	
				return RG_FWDENGINE_RET_DIRECT_TX;		
			}
		}

		//fwdEngine only support TCP,UDP,ICMP fragment now
		//first packet which has L4 header of these fragments
		else if(pPktHdr->ipv4MoreFragment && pPktHdr->ipv4FragmentOffset==0)
		{
			//Add software NAPT entry(TCP or UDP)
#ifdef CONFIG_RG_NAPT_TCP_AUTO_LEARN
			if(pPktHdr->tagif&TCP_TAGIF)
			{
				//Keep original information for L4 checksum recaculate
				ori_srcIP = ntohl(*pPktHdr->pIpv4Sip);
				ori_srcPort = ntohs(*pPktHdr->pSport);
	
				ret = _rtk_rg_fwdEngine_TCPOutboundConnectionTracking(pPktHdr,&naptOutIdx);

				//before add to fragment table, lookup for created frag list
				pFragList = NULL;
				_rtk_rg_fwdEngine_fragmentOutHashIndexLookup(&fragIdx,&pFragList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragList!=NULL)
				{
					if(pFragList->fragAction==RG_FWDENGINE_RET_DROP || pFragList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragList->fragAction;
					else
						pFragList->fragAction=ret;	//forward, drop, or to_ps
				}
				else				
					_rtk_rg_fwdEngine_fillOutFragmentInfo(ret,naptOutIdx,pPktHdr,&pFragList);

				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//dump_packet(pData,len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,naptOutIdx, pPktHdr,skb,1,1);
				pFragList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//Recaculate L4 checksum, let HW do L3 checksum
				*pPktHdr->pL4Checksum = htons(_rtk_rg_fwdengine_L4checksumUpdate(*pPktHdr->pL4Checksum,ori_srcIP,ori_srcPort,ntohl(*pPktHdr->pIpv4Sip),ntohs(*pPktHdr->pSport)));
				//dump_packet(pData,len,"new");
				return RG_FWDENGINE_RET_FRAGMENT_ONE;
							
			}
#endif
			//UDP auto-learning
			if(pPktHdr->tagif&UDP_TAGIF)
			{		
				//Keep original information for L4 checksum recaculate
				ori_srcIP = ntohl(*pPktHdr->pIpv4Sip);
				ori_srcPort = ntohs(*pPktHdr->pSport);
	
				ret = _rtk_rg_fwdEngine_UDPOutboundConnectionTracking(pPktHdr, &naptOutIdx);

				//before add to fragment table, lookup for created frag list
				pFragList = NULL;
				_rtk_rg_fwdEngine_fragmentOutHashIndexLookup(&fragIdx,&pFragList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragList!=NULL)
				{
					if(pFragList->fragAction==RG_FWDENGINE_RET_DROP || pFragList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragList->fragAction;
					else
						pFragList->fragAction=ret;	//forward, drop, or to_ps
				}
				else
					_rtk_rg_fwdEngine_fillOutFragmentInfo(ret,naptOutIdx,pPktHdr,&pFragList);

				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//dump_packet(pData,len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,naptOutIdx,pPktHdr,skb,1,1);
				pFragList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue

				//Recaculate L4 checksum, let HW do L3 checksum
				//DEBUG("the original checksum is %x",*pPktHdr->pL4Checksum);
				*pPktHdr->pL4Checksum = htons(_rtk_rg_fwdengine_L4checksumUpdate(*pPktHdr->pL4Checksum,ori_srcIP,ori_srcPort,ntohl(*pPktHdr->pIpv4Sip),ntohs(*pPktHdr->pSport)));
				//DEBUG("the new checksum is %x",*pPktHdr->pL4Checksum);
				//DEBUG("UDP first packet modify for fragment...");

				//dump_packet(pData,len,"new");			
				return RG_FWDENGINE_RET_FRAGMENT_ONE;		
			}		

			//ICMP forwarding
			if(pPktHdr->tagif&ICMP_TAGIF)
			{
				//Check MTU
				if(pPktHdr->ICMPOverMTU)return RG_FWDENGINE_RET_FRAG_ONE_PS;
				
				//DEBUG("before add ICMP control flow!!(first fragment packet)");

				//before add to fragment table, lookup for created frag list
				pFragList = NULL;
				_rtk_rg_fwdEngine_fragmentOutHashIndexLookup(&fragIdx,&pFragList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragList!=NULL)
				{
					if(pFragList->fragAction==RG_FWDENGINE_RET_DROP || pFragList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragList->fragAction;
					else
						pFragList->fragAction=RG_FWDENGINE_RET_NAPT_OK;	//forward, drop, or to_ps
				}
				else
				{
					ret = _rtk_rg_fwdEngine_ICMPOutboundControlFlowTracking(pPktHdr,&icmpCtrlFlow);
					_rtk_rg_fwdEngine_fillOutFragmentInfo(ret,FAIL,pPktHdr,&pFragList);
				}

				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,FAIL,pPktHdr,skb,1,0);
				pFragList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//dump_packet(skb->data,skb->len,"ICMP first fragment"); 	
				return RG_FWDENGINE_RET_FRAGMENT_ONE;		
			}
		}
		else if(pPktHdr->ipv4FragmentOffset>0)	//other fragments 
		{
			//DEBUG("i am non-first fragment packets...");
			//Check if we had already add software NAPT entry for the fragment packets
			naptOutIdx = FAIL;
			//icmpCtrlFlow = NULL;
			pFragList = NULL;
			//if(pPktHdr->tagif&TCP_TAGIF || pPktHdr->tagif&UDP_TAGIF)
			naptOutIdx = _rtk_rg_fwdEngine_fragmentOutHashIndexLookup(&fragIdx,&pFragList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
			/*else if(pPktHdr->tagif&ICMP_TAGIF)
			{
				//if IPID is match, direct forward as L3 packet!!
				
				icmpCtrlFlow = _rtk_rg_fwdEngine_ICMPOutboundFragmentLookup(pPktHdr);
				if(icmpCtrlFlow!=NULL)
				{
					//Check MTU
					if(pPktHdr->ICMPOverMTU)return RG_FWDENGINE_RET_TO_PS;
				
					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,0,pPktHdr,skb,1,0);
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					//dump_packet(skb->data,skb->len,"ICMP outbound(frag)"); 		
					return RG_FWDENGINE_RET_FRAGMENT;
				}
				//DEBUG("icmpCtrlFlow is NULL in system......queuing");
			}*/

			if(pFragList!=NULL)		//filled by first packet or other early fragment packets
			{
				//DEBUG("later fragment modify... fragIdx=%d",fragIdx);
				if(pPktHdr->ipv4MoreFragment == 0 && fragIdx != FAIL)		//we can not free fragList here, unless there are all packets had forwarded
				{
					//Check count and total fragment length
					forwardLength=pFragList->pktCount*(rg_db.netif[pPktHdr->netifIdx].rtk_netif.mtu);
					forwardLength-=pFragList->pktCount*20;		//each fragment's IP header length
					if(forwardLength>pPktHdr->ipv4FragmentOffset+pPktHdr->l3Len-20)
					{
						DEBUG("[outBound]fragments are all forwareded!! free the list %p in [%d]...",pFragList,fragIdx);
						_rtk_rg_freeFragOutList(fragIdx,pFragList);
					}
				}
						
				//Check frag action from first packet
				if(pFragList->fragAction==RG_FWDENGINE_RET_TO_PS || pFragList->fragAction==RG_FWDENGINE_RET_DROP)
					return pFragList->fragAction;
				if(pFragList->fragAction==RG_FWDENGINE_RET_QUEUE_FRAG)	//first packet not come in yet
					goto OUTBOUND_FRAG_QUEUE;

				if(naptOutIdx!=FAIL)	//TCP or UDP
				{
					//Modify the SIP and forward it 
					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,FAIL,pPktHdr,skb,1,0);
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					
					return RG_FWDENGINE_RET_FRAGMENT;
				}
				else		//ICMP
				{
					//Check MTU
					if(pPktHdr->ICMPOverMTU)return RG_FWDENGINE_RET_TO_PS;
				
					//DEBUG("later fragment modify... fragIdx=%d",fragIdx);
					//if(pPktHdr->ipv4MoreFragment == 0 && fragIdx != FAIL)		//we can not free fragList here, if the last one is coming before other frags....
						//_rtk_rg_freeFragOutList(fragIdx,pFragList);
				
					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_OUTBOUND,FAIL,pPktHdr,skb,1,0);
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					//dump_packet(skb->data,skb->len,"ICMP outbound(frag)"); 		
					return RG_FWDENGINE_RET_FRAGMENT;
				}
			}
			else
			{	
				//Otherwise we need to queue this packet for later proceed
				_rtk_rg_fwdEngine_fillOutFragmentInfo(RG_FWDENGINE_RET_QUEUE_FRAG,FAIL,pPktHdr,&pFragList);
				
				//DEBUG("queuing packet.....");
OUTBOUND_FRAG_QUEUE:				

				//Check if we already queue same identification for MAX_FRAGMENT_QUEUE_THRESHOLD times
				pFragList->queueCount++;
				if(pFragList->queueCount>=MAX_FRAGMENT_QUEUE_THRESHOLD)
				{
					//clear same identification in queue
					pFragList->queueCount=0;
					pFragList->fragAction=RG_FWDENGINE_RET_DROP;
					_rtk_rg_fwdEngine_fragmentQueueProcessing(pFragList->fragAction,pPktHdr);
					return RG_FWDENGINE_RET_DROP;
				}
				else
				{
					//Put the fragment packet into queue
					_rtk_rg_fwdEngine_fragmentPacketQueuing(NAPT_DIRECTION_OUTBOUND,cp,skb,pPktHdr);
					return RG_FWDENGINE_RET_QUEUE_FRAG;
				}
			}
		}
	}
	else if(sipDipClass==SIP_DIP_CLASS_NAPTR)
	{	
		//Check ALG
		ret=_rtk_rg_algFunctionCheck(NAPT_DIRECTION_INBOUND,pPktHdr);
		//DEBUG("the algFunction ret=%d",ret);
		if(ret==RG_FWDENGINE_RET_TO_PS)return ret;	//hit ALG and registered function is NULL
	
		// From which WAN?
		pPktHdr->extipIdx = _rtk_rg_eiplookup(pPktHdr->ipv4Dip);
		assert(pPktHdr->extipIdx!=-1); //For NAPTR packet, external ip table lookup will not miss.
		pPktHdr->netifIdx = pPktHdr->extipIdx;

#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK
		//Keep WAN interface index in CP structure
		/*for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].index==pPktHdr->netifIdx)
			{
				cp->wanInterfaceIdx=i;
				break;
			}
		}*/
		cp->wanInterfaceIdx=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].lan_or_wan_index;		
		//DEBUG("this packet came from WAN index %d (eth%d)",cp->wanInterfaceIdx,cp->wanInterfaceIdx+1);
#endif

		if(pPktHdr->ipv4FragPacket==0) //normal packet (non-fragment)
		{
#ifdef CONFIG_RG_NAPT_TCP_AUTO_LEARN
			if(pPktHdr->tagif&TCP_TAGIF)
			{			
				ret = _rtk_rg_fwdEngine_TCPInboundConnectionTracking(pPktHdr,&naptOutIdx);
				if(ret!=RG_FWDENGINE_RET_NAPT_OK)return ret;

				naptInIdx=rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;

				//packet modify and directTX
				//dump_packet(skb->data,skb->len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,naptInIdx,pPktHdr,skb,1,1);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//dump_packet(skb->data,skb->len,"new");
				return RG_FWDENGINE_RET_DIRECT_TX;
			}
#endif
			if(pPktHdr->tagif&UDP_TAGIF)
			{
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
				ret = _rtk_rg_fwdEngine_UDPInboundConnectionTracking(pPktHdr, &naptOutIdx);
				if(ret!=RG_FWDENGINE_RET_NAPT_OK)return ret;
#else
				naptOutIdx=_rtk_rg_naptTcpUdpInHashIndexLookup(0,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
				assert(naptOutIdx!=FAIL);
				if(naptOutIdx==FAIL) 
				{
					//dump_packet(pData,len,"outIdx=fail");
					return RG_FWDENGINE_RET_TO_PS;
				}
#endif
				naptInIdx=rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;
				
				//packet modify and directTX
				//dump_packet(pData,len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,naptInIdx,pPktHdr,skb,1,1);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//if(ret!=RT_ERR_RG_OK) return RG_FWDENGINE_RET_DROP;
				//dump_packet(pData,len,"new");
				return RG_FWDENGINE_RET_DIRECT_TX;
			}

			//ICMP link list lookup
			if(pPktHdr->tagif&ICMP_TAGIF)
			{
				//DEBUG("before ICMP flow lookup!");
				icmpCtrlFlow = _rtk_rg_fwdEngine_ICMPInboundControlFlowTracking(pPktHdr);

				if(icmpCtrlFlow==NULL)
				{
					//DEBUG("packet in NAPTR...before return to PS");
					//dump_packet(skb->data,skb->len,"return to PS");
					return RG_FWDENGINE_RET_TO_PS;
				}

				//1 FIXME:Here should check MTU size, too!!!
				
				//DEBUG("the internal IP from ICMP flow is %x",icmpCtrlFlow->internalIP);
				//Modify DIP
				*pPktHdr->pIpv4Dip=htonl(icmpCtrlFlow->internalIP);
				pPktHdr->ipv4Dip=icmpCtrlFlow->internalIP;

				//invalid this ctrl flow
				icmpCtrlFlow->valid=0;

				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,FAIL,pPktHdr,skb,0,0);
				if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
				//assert_ok(ret);
				//dump_packet(skb->data,skb->len,"ICMP inbound"); 		
				return RG_FWDENGINE_RET_DIRECT_TX;		
			}
		}
		else if(pPktHdr->ipv4MoreFragment && pPktHdr->ipv4FragmentOffset==0)	//first packet which has L4 header of these fragments
		{
#ifdef CONFIG_RG_NAPT_TCP_AUTO_LEARN
			if(pPktHdr->tagif&TCP_TAGIF)
			{				
				//Keep original information for L4 checksum recaculate
				ori_srcIP = ntohl(*pPktHdr->pIpv4Sip);
				ori_srcPort = ntohs(*pPktHdr->pSport);
	
				ret = _rtk_rg_fwdEngine_TCPInboundConnectionTracking(pPktHdr,&naptOutIdx);

				//before add to fragment table, look up for created frag list
				pFragInList = NULL;
				_rtk_rg_fwdEngine_fragmentInHashIndexLookup(&fragIdx,&pFragInList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragInList!=NULL)
				{
					if(pFragInList->fragAction==RG_FWDENGINE_RET_DROP || pFragInList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragInList->fragAction;
					else
						pFragList->fragAction=ret;	//forward, drop, or to_ps
				}
				else
					_rtk_rg_fwdEngine_fillInFragmentInfo(ret,naptOutIdx,NULL,pPktHdr,&pFragInList);
				
				if(ret!=RG_FWDENGINE_RET_NAPT_OK)return ret;

				naptInIdx=rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;				
					
				//packet modify and directTX
				//dump_packet(skb->data,skb->len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,naptInIdx,pPktHdr,skb,1,1);
				pFragInList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//Recaculate L4 checksum, let HW do L3 checksum
				*pPktHdr->pL4Checksum = htons(_rtk_rg_fwdengine_L4checksumUpdate(*pPktHdr->pL4Checksum,ori_srcIP,ori_srcPort,ntohl(*pPktHdr->pIpv4Sip),ntohs(*pPktHdr->pSport)));
					
				//dump_packet(skb->data,skb->len,"new");
				return RG_FWDENGINE_RET_FRAGMENT_ONE;
			}
#endif
			
			if(pPktHdr->tagif&UDP_TAGIF)
			{
				//Keep original information for L4 checksum recaculate
				ori_srcIP = ntohl(*pPktHdr->pIpv4Sip);
				ori_srcPort = ntohs(*pPktHdr->pSport);
	
#ifdef CONFIG_RG_NAPT_INBOUND_TRACKING
				ret = _rtk_rg_fwdEngine_UDPInboundConnectionTracking(pPktHdr, &naptOutIdx);
#else
				naptOutIdx=_rtk_rg_naptTcpUdpInHashIndexLookup(0,pPktHdr->ipv4Sip,pPktHdr->sport,pPktHdr->ipv4Dip,pPktHdr->dport);
				assert(naptOutIdx!=FAIL);
				if(naptOutIdx==FAIL) 
				{
					//dump_packet(pData,len,"outIdx=fail");
					ret = RG_FWDENGINE_RET_TO_PS;
				}
				else
					ret = RG_FWDENGINE_RET_NAPT_OK;
#endif

				//before add to fragment table, look up for created frag list
				pFragInList = NULL;
				_rtk_rg_fwdEngine_fragmentInHashIndexLookup(&fragIdx,&pFragInList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragInList!=NULL)
				{
					if(pFragInList->fragAction==RG_FWDENGINE_RET_DROP || pFragInList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragInList->fragAction;
					else
						pFragList->fragAction=ret;	//forward or to_ps
				}
				else
					_rtk_rg_fwdEngine_fillInFragmentInfo(ret,naptOutIdx,NULL,pPktHdr,&pFragInList);

				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue

				naptInIdx=rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;

				/* Packet forwarded by Forwarding Engine, reset idle time */
				rg_db.naptOut[naptOutIdx].idleSecs=0;
				
				//packet modify and directTX
				//dump_packet(pData,len,"org");
				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,naptInIdx,pPktHdr,skb,1,1);
				pFragInList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				//Recaculate L4 checksum, let HW do L3 checksum
				*pPktHdr->pL4Checksum = htons(_rtk_rg_fwdengine_L4checksumUpdate(*pPktHdr->pL4Checksum,ori_srcIP,ori_srcPort,ntohl(*pPktHdr->pIpv4Sip),ntohs(*pPktHdr->pSport)));
				
				//dump_packet(pData,len,"new");
				return RG_FWDENGINE_RET_FRAGMENT_ONE;
			}

			if(pPktHdr->tagif&ICMP_TAGIF)
			{
				DEBUG("before lookup ICMP control flow!!(first inbound fragment packet)");
				icmpCtrlFlow = _rtk_rg_fwdEngine_ICMPInboundControlFlowTracking(pPktHdr);

				if(icmpCtrlFlow==NULL)
				{
					DEBUG("ICMP flow lookup failed in NAPTR..ret=to_PS");
					//dump_packet(skb->data,skb->len,"first return to PS");
					ret = RG_FWDENGINE_RET_TO_PS;
				}
				else
				{
					//Keep IP identifier for other fragment packets
					icmpCtrlFlow->inboundIPID=ntohs(*pPktHdr->pIpv4Identification);
					ret = RG_FWDENGINE_RET_NAPT_OK;
				}

				//1 FIXME:Here should check MTU size, too!!!

				//before add to fragment table, look up for created frag list
				pFragInList = NULL;
				_rtk_rg_fwdEngine_fragmentInHashIndexLookup(&fragIdx,&pFragInList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
				if(pFragInList!=NULL)
				{
					if(pFragInList->fragAction==RG_FWDENGINE_RET_DROP || pFragInList->fragAction==RG_FWDENGINE_RET_TO_PS)
						return pFragInList->fragAction;
					else
						pFragList->fragAction=ret;	//forward  or to_ps
				}
				else
					_rtk_rg_fwdEngine_fillInFragmentInfo(ret,FAIL,icmpCtrlFlow,pPktHdr,&pFragInList);

				DEBUG("after fillInFragmentinfo. ret is %d",ret);
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue

				//Modify DIP
				*pPktHdr->pIpv4Dip=htonl(icmpCtrlFlow->internalIP);
				pPktHdr->ipv4Dip=icmpCtrlFlow->internalIP;

				ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,FAIL,pPktHdr,skb,0,0);
				pFragInList->fragAction=ret;
				if(ret==RG_FWDENGINE_RET_DROP)
					return RG_FWDENGINE_RET_FRAG_ONE_DROP;		//drop all same identification in queue
				else if(ret==RG_FWDENGINE_RET_TO_PS)
					return RG_FWDENGINE_RET_FRAG_ONE_PS;		//trap all same identification in queue
				
				return RG_FWDENGINE_RET_FRAGMENT_ONE;
			}
		}
		else if(pPktHdr->ipv4FragmentOffset>0)	//other fragments 
		{
			naptOutIdx = FAIL;
			//icmpCtrlFlow = NULL;
			pFragInList = NULL;
			//if(pPktHdr->tagif&TCP_TAGIF)
			naptOutIdx = _rtk_rg_fwdEngine_fragmentInHashIndexLookup(&fragIdx,&pFragInList,pPktHdr->ipProtocol,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
			/*else if(pPktHdr->tagif&UDP_TAGIF)
				naptOutIdx = _rtk_rg_fwdEngine_fragmentTcpUdpInHashIndexLookup(&fragIdx,&fragAct,0,pPktHdr->ipv4Sip, pPktHdr->ipv4Dip, ntohs(*pPktHdr->pIpv4Identification));
			else if(pPktHdr->tagif&ICMP_TAGIF)
			{
				icmpCtrlFlow = _rtk_rg_fwdEngine_ICMPInboundFragmentLookup(pPktHdr);
				if(icmpCtrlFlow!=NULL)
				{
					//DEBUG("find the inbound fragment flow!! inboundIPID is %d",icmpCtrlFlow->inboundIPID);
					//Modify DIP
					*pPktHdr->pIpv4Dip=htonl(icmpCtrlFlow->internalIP);
					pPktHdr->ipv4Dip=icmpCtrlFlow->internalIP;

					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,0,pPktHdr,skb,0,0);
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					//dump_packet(skb->data,skb->len,"ICMP fragment"); 	
					if(pPktHdr->ipv4MoreFragment==0)		//final fragment packet, invalid the ctrl flow
					{
						icmpCtrlFlow->valid=0;
						//DEBUG("ICMP inbound final one, invalid the ctrl flow...");
					}
					return RG_FWDENGINE_RET_FRAGMENT;
				}

				//1 FIXME:Here should check MTU size, too!!!

				if(pPktHdr->isGatewayPacket)		//fragment packet to gateway should goto protocol stack directly, not queuing
					return RG_FWDENGINE_RET_TO_PS;
				//DEBUG("inbound fragment ICMP flow is NULL.....queuing");
			}*/

			if(pFragInList!=NULL)		//filled by first packet or other early fragment packets
			{
				if(pPktHdr->ipv4MoreFragment == 0 && fragIdx != FAIL)		//we can not free fragList here, unless there are all packets had forwarded
				{
					//Check count and total fragment length
					forwardLength=pFragInList->pktCount*(rg_db.netif[pPktHdr->netifIdx].rtk_netif.mtu);
					forwardLength-=pFragInList->pktCount*20;		//each fragment's IP header length
					if(forwardLength>pPktHdr->ipv4FragmentOffset+pPktHdr->l3Len-20)
					{
						DEBUG("[inBound]fragments are all forwareded!! free the list %p in [%d]...",pFragInList,fragIdx);
						_rtk_rg_freeFragInList(fragIdx,pFragInList);
					}
				}
				
				DEBUG("Check for frag action:%d",pFragInList->fragAction);
				//Check frag action from first packet
				if(pFragInList->fragAction==RG_FWDENGINE_RET_TO_PS || pFragInList->fragAction==RG_FWDENGINE_RET_DROP)
					return pFragInList->fragAction;
				if(pFragInList->fragAction==RG_FWDENGINE_RET_QUEUE_FRAG)	//first packet not come in yet
					goto INBOUND_FRAG_QUEUE;
				DEBUG("normal forward!");
				if(naptOutIdx!=FAIL)		//TCP or UDP
				{
					naptInIdx=rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx;

					/* Packet forwarded by Forwarding Engine, reset idle time */
					rg_db.naptOut[naptOutIdx].idleSecs=0;

					//DEBUG("UDP last fragment modify...");
					//if(pPktHdr->ipv4MoreFragment == 0 && fragIdx != FAIL)		//we can not free fragList here, if the last one is coming before other frags....
						//_rtk_rg_freeFragInList(fragIdx,pFragInList);

					//Modify the DIP and forward it 
					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,naptInIdx,pPktHdr,skb,1,0);		//fragment packet without dport
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					
					return RG_FWDENGINE_RET_FRAGMENT;
				}
				else	//ICMP
				{
					DEBUG("find the inbound fragment flow!! inboundIPID is %d",pFragInList->identification);

					//Modify DIP
					*pPktHdr->pIpv4Dip=htonl(pFragInList->pktInfo.icmp.intIp);
					pPktHdr->ipv4Dip=pFragInList->pktInfo.icmp.intIp;
					
					//DEBUG("ICMP last fragment modify...");
					//if(pPktHdr->ipv4MoreFragment == 0 && fragIdx != FAIL)		//we can not free fragList here, if the last one is coming before other frags....
					//{
						//pFragInList->pktInfo.pICMPCtrlFlow->valid=0;
						//_rtk_rg_freeFragInList(fragIdx,pFragInList);
					//}

					ret = _rtk_rg_fwdEngine_naptPacketModify(NAPT_DIRECTION_INBOUND,FAIL,pPktHdr,skb,0,0);
					if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
					//assert_ok(ret);
					//dump_packet(skb->data,skb->len,"ICMP fragment"); 	
					
					return RG_FWDENGINE_RET_FRAGMENT;
				}
			}
			else
			{
				//1 FIXME:Here should check MTU size, too!!!

				//Otherwise we need to queue this packet for later proceed
				_rtk_rg_fwdEngine_fillInFragmentInfo(RG_FWDENGINE_RET_QUEUE_FRAG,FAIL,NULL,pPktHdr,&pFragInList);
				
INBOUND_FRAG_QUEUE:				
				DEBUG("queuing packet.....");
				//DEBUG("before to PS");
				if(pPktHdr->isGatewayPacket)		//fragment packet to gateway should goto protocol stack directly, not queuing
				{
					pFragInList->fragAction=RG_FWDENGINE_RET_TO_PS;
					return RG_FWDENGINE_RET_TO_PS;
				}
				//DEBUG("before queuing...");

				//Check if we already queue same identification for MAX_FRAGMENT_QUEUE_THRESHOLD times
				pFragInList->queueCount++;
				if(pFragInList->queueCount>=MAX_FRAGMENT_QUEUE_THRESHOLD)
				{
					//clear same identification in queue
					pFragInList->queueCount=0;
					pFragInList->fragAction=RG_FWDENGINE_RET_DROP;
					_rtk_rg_fwdEngine_fragmentQueueProcessing(pFragInList->fragAction,pPktHdr);
					return RG_FWDENGINE_RET_DROP;
				}
				else
				{
					//Put the fragment packet into queue
					_rtk_rg_fwdEngine_fragmentPacketQueuing(NAPT_DIRECTION_INBOUND,cp,skb,pPktHdr);
					return RG_FWDENGINE_RET_QUEUE_FRAG;
				}
			}
		}
	}
	else if(sipDipClass==SIP_DIP_CLASS_ROUTING)
	{
		ret = _rtk_rg_routingDecisionTablesLookup(pPktHdr,SIP_DIP_CLASS_ROUTING);
		if(ret!=RG_FWDENGINE_RET_CONTINUE) return ret;

		DEBUG("Routing...modify packet and update shortcut");
		//dump_packet(skb->data,skb->len,"org");
		ret = _rtk_rg_fwdEngine_naptPacketModify(0,0,pPktHdr,skb,0,0);
		//dump_packet(skb->data,skb->len,"new");
		if(ret!=RG_FWDENGINE_RET_CONTINUE)return ret;		//TO PS or DROP
		//assert_ok(ret);

#ifdef CONFIG_ROME_NAPT_SHORTCUT
		_rtk_rg_naptShortcutUpdate(pPktHdr,0,0,0);
#endif
		return RG_FWDENGINE_RET_DIRECT_TX;
	}

	return RG_FWDENGINE_RET_TO_PS;
}

static inline struct ipv6hdr* _rtk_rg_getIpv6Header(uint8 *macFrame)
{
	uint8 *ptr;
	struct ipv6hdr *ipv6h=NULL;
	
	ptr=macFrame+12;
	if(*(int16 *)(ptr)==(int16)htons(ETH_P_8021Q))
	{
		ptr=ptr+4;
	}

	/*it's not ipv6 packet*/
	if(*(int16 *)(ptr)!=(int16)htons(ETH_P_IPV6))
	{
		return NULL;
	}
	
	ipv6h=(struct ipv6hdr *)(ptr+2);

	return ipv6h;
}

static int _rtk_rg_getIpv6TransportProtocol(struct ipv6hdr* ipv6h)
{
	unsigned char *ptr=NULL;
	unsigned char *startPtr=NULL;
	unsigned char *lastPtr=NULL;
	unsigned char nextHeader=0;
	unsigned short extensionHdrLen=0;
	unsigned char  optionDataLen=0;
	unsigned char  optionType=0;
	unsigned int ipv6RAO=0;
	if(ipv6h==NULL)
	{
		return -1;
	}
	if(ipv6h->version!=6)
	{
		return -1;
	}
	startPtr= (unsigned char *)ipv6h;
	lastPtr=startPtr+sizeof(struct ipv6hdr)+(ipv6h->payload_len);
	nextHeader= ipv6h ->nexthdr;
	ptr=startPtr+sizeof(struct ipv6hdr);
	while(ptr<lastPtr)
	{
		switch(nextHeader) 
		{
			case HOP_BY_HOP_OPTIONS_HEADER:
				nextHeader=ptr[0];
				extensionHdrLen=((uint16)(ptr[1])+1)*8;
				ptr=ptr+2;
				while(ptr<(startPtr+extensionHdrLen+sizeof(struct ipv6hdr)))
				{
					optionType=ptr[0];
					if(optionType==0)
					{
						ptr=ptr+1;
						continue;
					}
					if(optionType==1)
					{
						optionDataLen=ptr[1];
						ptr=ptr+optionDataLen+2;
						continue;
					}
					if(ntohl(*(uint32 *)(ptr))==IPV6_ROUTER_ALTER_OPTION)
					{
						ipv6RAO=IPV6_ROUTER_ALTER_OPTION;
						ptr=ptr+4;	
						continue;
					}
					if((optionType!=0) && (optionType!=1))
					{
						optionDataLen=ptr[1];
						ptr=ptr+optionDataLen+2;
						continue;
					}
				}
				break;
			case ROUTING_HEADER:
				nextHeader=ptr[0];
				extensionHdrLen=((uint16)(ptr[1])+1)*8;			
                            ptr=ptr+extensionHdrLen;
				break;
			case FRAGMENT_HEADER:
				nextHeader=ptr[0];
				ptr=ptr+8;
				break;
			case DESTINATION_OPTION_HEADER:
				nextHeader=ptr[0];
				extensionHdrLen=((uint16)(ptr[1])+1)*8;
				ptr=ptr+extensionHdrLen;
				break;
			case ICMP_PROTOCOL:
				nextHeader=NO_NEXT_HEADER;
				if((ptr[0]==MLD_QUERY) ||(ptr[0]==MLDV1_REPORT) ||(ptr[0]==MLDV1_DONE) ||(ptr[0]==MLDV2_REPORT))
				{
					return ICMP_PROTOCOL;
				}
				break;
			case PIM_PROTOCOL:
				nextHeader=NO_NEXT_HEADER;
				if(IS_IPV6_PIM_ADDR(ipv6h->daddr.s6_addr32))
				{
					return PIM_PROTOCOL;
				}
				break;
			case MOSPF_PROTOCOL:
				nextHeader=NO_NEXT_HEADER;
				if(IS_IPV6_MOSPF_ADDR1(ipv6h->daddr.s6_addr32) || IS_IPV6_MOSPF_ADDR2(ipv6h->daddr.s6_addr32))
				{
					return MOSPF_PROTOCOL;
				}
				break;
			case IPPROTO_TCP:
				nextHeader=NO_NEXT_HEADER;
				return IPPROTO_TCP;
				break;
			case IPPROTO_UDP:
				nextHeader=NO_NEXT_HEADER;
				return IPPROTO_UDP;
				break;	
			default:		
				return -1;
				break;
		}
	}
	return -1;
}

static int rtk_rg_multicastRxCheck(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr, int pid)
{

	unsigned int  vlanRelayPortMask=0;
#ifdef CONFIG_RTL_IGMP_SNOOPING	
		if( (pPktHdr->pDmac[0]==0x01 && pPktHdr->pDmac[1]==0x00 && pPktHdr->pDmac[2]==0x5e) || 
			(((pPktHdr->pDmac[0]&1)==0)&&((pPktHdr->tagif&(PPPOE_TAGIF|IPV4_TAGIF))==(PPPOE_TAGIF|IPV4_TAGIF))&&(pPktHdr->ipv4Dip>=0xe0000000)))
		{
			if(rg_db.systemGlobal.initParam.igmpSnoopingEnable)
			{
				
				if(pPktHdr->tagif&IGMP_TAGIF) 
				{	
						rtl_igmpMldProcess(rg_db.systemGlobal.nicIgmpModuleIndex, skb->data, pid, &vlanRelayPortMask);
						//return SUCCESS;
						return FAIL;	//20130722:multicast should continue to broadcast module in fwdEngine, not directly return to protocol stack
				}
			}
			else
				return FAIL;
		}
		
		else if ((pPktHdr->pDmac[0]==0x33) && (pPktHdr->pDmac[1]==0x33) && (pPktHdr->pDmac[2]!=0xff))
		{	

			struct ipv6hdr *ipv6h=NULL;
			if(!rg_db.systemGlobal.initParam.igmpSnoopingEnable)
				return FAIL;

			ipv6h=_rtk_rg_getIpv6Header(skb->data);
			if(ipv6h!=NULL)
			{	
				unsigned int l4Protocol=0;
				l4Protocol=_rtk_rg_getIpv6TransportProtocol(ipv6h);
				/*udp or tcp packet*/
				if(l4Protocol==IPPROTO_ICMPV6) 
				{
					/*icmp packet*/		
					rtl_igmpMldProcess(rg_db.systemGlobal.nicIgmpModuleIndex, skb->data, pid, &vlanRelayPortMask);
					//return SUCCESS;
					return FAIL;	//20130722:multicast should continue to broadcast module in fwdEngine, not directly return to protocol stack
				}		
				/*else if((l4Protocol==IPPROTO_UDP) || (l4Protocol==IPPROTO_TCP))
				{			
					struct rtl_multicastDataInfo multicastDataInfo;
					struct rtl_multicastFwdInfo nicMCastFwdInfo;			
					
					multicastDataInfo.ipVersion=6;
					memcpy(&multicastDataInfo.sourceIp, &ipv6h->saddr, 16);
					memcpy(&multicastDataInfo.groupAddr, &ipv6h->daddr, 16);
				
					if(rtl_getMulticastDataFwdInfo(rg_db.systemGlobal.nicIgmpModuleIndex, &multicastDataInfo, &nicMCastFwdInfo)==SUCCESS)
					{
						struct tx_info txInfo,txInfoMask;
						struct sk_buff *skb2=NULL;
						skb2 = skb_clone(skb, GFP_ATOMIC);
						vlanRelayPortMask = (~(1<<pid)) & nicMCastFwdInfo.fwdPortMask & 0x3f ;
						
						memset(&txInfo,0,sizeof(struct tx_info));
						memset(&txInfoMask,0,sizeof(struct tx_info));			

						txInfoMask.opts2.bit.cputag=1;
						txInfo.opts2.bit.cputag=1;

						txInfoMask.opts1.bit.keep=1;
						txInfoMask.opts3.bit.tx_portmask=0x3f;
						txInfo.opts3.bit.tx_portmask= vlanRelayPortMask;
						FIXME("not include extension ports");
						TRACE("IPv6 Multicast DirectTX");

						re8686_send_with_txInfo_and_mask(skb2,&txInfo,0,&txInfoMask);			
						return SUCCESS;
					}				
				}*/
			}
		} 

#endif	

	
	return FAIL;
}

int _rtk_rg_fwdEngineDestinationLookup(rtk_rg_pktHdr_t *pPktHdr)
{
	//First use packet's vlan value to find SVL or IVL,
	//then use packet header's Destination Mac address to look up for the destination port
	rtk_fidMode_t fidMode;
	int hashedIdx,VLANId,FId,EFId,i,search_index;

	if(pPktHdr->dmacL2Idx==FAIL)
	{
		//Layer2 packet should not be here, they will forward by hardware lookup

		/*if(pPktHdr->netifIdx==FAIL)	//L2
		{
			//DEBUG("$$$$$$$$$$$$$$$$ in %s, the pkthdr internal VID is %d",__FUNCTION__,pPktHdr->internalVlanID);
			VLANId=pPktHdr->internalVlanID;
			fidMode=rg_db.vlan[VLANId].fidMode;
			FId=rg_db.vlan[VLANId].fid;
			EFId=0;		//FIXME:we did not store efid right now
		}
		else
		{*/
			if(rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.is_wan)
			{
				VLANId=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
				fidMode=rg_db.vlan[VLANId].fidMode;
				FId=rg_db.vlan[VLANId].fid;
				EFId=0;		//FIXME:we did not store efid right now
				//DEBUG("WAN!! pPktHdr->netifIdx is %d, VLANID is %d",pPktHdr->netifIdx,VLANId);
			}
			else
			{
				VLANId=rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo.lan_intf.intf_vlan_id;
				fidMode=rg_db.vlan[VLANId].fidMode;
				FId=rg_db.vlan[VLANId].fid;
				EFId=0;		//FIXME:we did not store efid right now
				//DEBUG("LAN!! pPktHdr->netifIdx is %d, VLANID is %d",pPktHdr->netifIdx,VLANId);
			}
		//}
		
		if(fidMode==VLAN_FID_IVL)
			hashedIdx=_rtk_rg_hash_mac_vid_efid(pPktHdr->pDmac,VLANId,EFId);
		else
			hashedIdx=_rtk_rg_hash_mac_fid_efid(pPktHdr->pDmac,FId,EFId);
		
		hashedIdx<<=2;
		for(i=0;i<4;i++)
		{
			search_index=hashedIdx+i;
			if(rg_db.lut[search_index].valid==0)
				continue;
			
			if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
				(memcmp(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pPktHdr->pDmac,ETHER_ADDR_LEN)==0))
			{
				if((fidMode==VLAN_FID_IVL && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==VLANId) || 
					(fidMode==VLAN_FID_SVL && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==FId))
				{
					//rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;
					//rg_kernel.txDescMask.opts3.bit.l34_keep=1;
					
					//rg_kernel.txDesc.opts3.bit.tx_portmask=0x1<<rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port;
					//rg_kernel.txDesc.opts3.bit.l34_keep=0;
					//DEBUG("lookup! to portmask %x",rg_kernel.txDesc.opts3.bit.tx_portmask);
					break;
				}
			}
		}
		if(i==4)
			return FAIL;
	}
	else
	{
		search_index=pPktHdr->dmacL2Idx;
		//DEBUG("searchIdx is pPktHdr->dmacL2Idx:%d",search_index);
	}
	//rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;					
	//rg_kernel.txDesc.opts3.bit.tx_portmask=0x1<<rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port;

	return search_index;	
}

int _rtk_rg_fwdEngineDMAC2CVIDTransfer(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	//Use packet header's Destination Mac address to look up for the vlanid and destination port
	int /*hashedIdx,i,*/VLANId,search_index=0;
	
	/*if(pPktHdr->dmacL2Idx==FAIL)
	{
		hashedIdx=_rtk_rg_hash_mac_fid_efid(pPktHdr->pDmac,FId,EFId);
		hashedIdx<<=2;
		for(i=0;i<4;i++)
		{
			search_index=hashedIdx+i;
			if(rg_db.lut[search_index].valid==0)
				continue;
			
			if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
				(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pPktHdr->pDmac,6)) && 
				rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==FId)
				break;
		}
		if(i==4)
			return RT_ERR_RG_FAILED;
		}
	else
	{
		search_index=pPktHdr->dmacL2Idx;
	}*/
	search_index=_rtk_rg_fwdEngineDestinationLookup(pPktHdr);
	if(search_index==FAIL)
	{
		//DEBUG("do hardware lookup");
		rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;	
		rg_kernel.txDescMask.opts2.bit.vidl=0xff;
		rg_kernel.txDescMask.opts2.bit.vidh=0xf;
		rg_kernel.txDescMask.opts2.bit.prio=0x0;

		rg_kernel.txDesc.opts2.bit.tx_vlan_action=0;
		rg_kernel.txDesc.opts2.bit.vidl=0;
		rg_kernel.txDesc.opts2.bit.vidh=0;
		rg_kernel.txDesc.opts3.bit.tx_portmask=0;
	}
	else
	{
		//Assign Port
		rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;	
		rg_kernel.txDesc.opts3.bit.tx_portmask=0x1<<rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port;

		VLANId=rg_kernel.txDesc.opts2.bit.vidh;
		VLANId<<=8;
		VLANId|=rg_kernel.txDesc.opts2.bit.vidl;
		//DEBUG("the VLANId decide in _rtk_rg_interfaceVlanTagged is %d",VLANId);
		if(rg_db.vlan[VLANId].fidMode==VLAN_FID_IVL)
		{
			//tag/untag by VLAN untag setting
			if(rg_db.vlan[VLANId].UntagPortmask.bits[0]&rg_kernel.txDesc.opts3.bit.tx_portmask)
			{
				//DEBUG("IVL:untagged!");
				rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
				rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing
			}
 			else
 			{
 				//DEBUG("IVL:tagged!");
 				rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
 				rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x3;		//1 FIXME: here use remarking tag, not add tag
 			}
 		}
 		else
 		{
 			//Get VLANId from LUT, it will be 0 if learned by untag!
 			VLANId=rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid;
 
 			//if(rg_db.vlan[VLANId].fidMode==VLAN_FID_SVL)
 			if(VLANId==0)
 			{
 				//DEBUG("dmac2cvid:untagged!");
 				rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
 				rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing				
			}
			else
			{
				//Assign VLAN by DMAC2CVID, and this VLAN should be the same with VLAN above
				//DEBUG("dmac2cvid:tagged!");
				rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
				//rg_kernel.txDescMask.opts2.bit.vidl=0xff;
				//rg_kernel.txDescMask.opts2.bit.vidh=0xf;
				//rg_kernel.txDescMask.opts2.bit.prio=0x0;

				rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x3;		//1 FIXME: here use remarking tag, not add tag
				//rg_kernel.txDesc.opts2.bit.vidl=(VLANId&0xff);
				//rg_kernel.txDesc.opts2.bit.vidh=((VLANId&0xf00)>>8);
				/*if(rg_db.vlan[VLANId].priorityEn==1)
				{
					rg_kernel.txDescMask.opts2.bit.prio=0x7;
					rg_kernel.txDesc.opts2.bit.prio=rg_db.vlan[VLANId].priority;
				}*/
			}
		}
#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION		
		if((rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port==RTK_RG_PORT_CPU)&&(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port==1)) // to EXT0
		{
			TRACE("send to WIFI0");


#if 1 //patch for wifi no L3/L4 checksum offload
{		
		int total_len;
		FIXME("to WIFI0 software L3/L4 checksum");		

		if(pPktHdr->tagif&(IPV4_TAGIF|IPV6_TAGIF))
		{
			if(pPktHdr->tagif&IPV6_TAGIF)
			{
				total_len=pPktHdr->l3Offset+pPktHdr->l3Len-2+pPktHdr->ipv6PayloadLen;	// 2 bytes: ether type
			}
			else
			{
				total_len=pPktHdr->l3Offset+pPktHdr->l3Len;
			}
		
			*pPktHdr->pIpv4Checksum=0;
			*pPktHdr->pIpv4Checksum=htons(inet_chksum(skb->data+pPktHdr->l3Offset,pPktHdr->l4Offset-pPktHdr->l3Offset));

			if(pPktHdr->tagif&(TCP_TAGIF|UDP_TAGIF))
			{
				*pPktHdr->pL4Checksum=0;					
				*pPktHdr->pL4Checksum=htons(inet_chksum_pseudo(skb->data+pPktHdr->l4Offset,total_len-pPktHdr->l4Offset,ntohl(*pPktHdr->pIpv4Sip),ntohl(*pPktHdr->pIpv4Dip),pPktHdr->ipProtocol));
			}			
		}
}
#endif
			//memDump(skb->data,skb->len,"to wifi0");
			if(_rtk_master_wlan_mbssid_tx(skb)==FAIL)
			{
				dev_kfree_skb_any(skb);
			}
			return FAIL;
		}
#endif
	}
	
	return RT_ERR_RG_OK;	
}

int _rtk_rg_fwdEngineDirectTx(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	//Layer2forward back to use hardward lookup, so this check is unnecessary anymote.
	//if(pPktHdr->netifIdx!=FAIL)	//Layer2 forward didn't decide interface index
		_rtk_rg_interfaceVlanIDPriority(&rg_db.systemGlobal.interfaceInfo[pPktHdr->netifIdx].storedInfo,&rg_kernel.txDesc,&rg_kernel.txDescMask);
	/*else
	{
		//decide vlan ID by internalVID
		rg_kernel.txDescMask.opts2.bit.vidl=0xff;
		rg_kernel.txDescMask.opts2.bit.vidh=0xf;
		rg_kernel.txDesc.opts2.bit.vidl=(pPktHdr->internalVlanID&0xff);
		rg_kernel.txDesc.opts2.bit.vidh=((pPktHdr->internalVlanID&0xf00)>>8);
	}*/

#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	//_rtk_rg_fwdEngineDestPortLookup(pPktHdr);
	if(_rtk_rg_fwdEngineDMAC2CVIDTransfer(skb,pPktHdr)==FAIL) return SUCCESS;
#endif
	return _rtk_rg_egressPacketSend(skb,pPktHdr);
	//return re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
}

#endif

void _rtk_rg_fwdEngineLIMDBC(rtk_rg_pktHdr_t *pPktHdr)
{
	int i;
	//initialize
	pPktHdr->isGatewayPacket=0;
	
	/* lookup DA in netIf table */
	for(i=0;i<6/*MAX_NETIF_SW_TABLE_SIZE*/;i++)		//in Apollo L34 document said that we check first 6 entries only
	{
		if(rg_db.netif[i].rtk_netif.valid == 1)
		{
			if(memcmp(pPktHdr->pDmac,rg_db.netif[i].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN)==0)
			{
				pPktHdr->isGatewayPacket=1;
				break;
			}
		}
	}
}

#if 1	

int _rtk_rg_layer2LutLearningCountShow(void)
{	
	int i;
	rtlglue_printf("srcPort    Count    Limit    exceedAction\n");
	for(i=0;i<RTK_RG_PORT_MAX;i++)
		rtlglue_printf("Port %d         %d     %d    %s\n",i,atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[i]),rg_db.systemGlobal.sourceAddrLearningLimitNumber[i],rg_db.systemGlobal.sourceAddrLearningAction[i]==SA_LEARN_EXCEED_ACTION_PERMIT?"Forward":"Drop");

	return 0;
}

int _rtk_rg_layer2LutLearning(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr,rtk_rg_port_idx_t spa)

{
	rtk_rg_macEntry_t macEntry;
	int ret,l2Idx,search_index,count=0;
	

	//rtk_rg_port_idx_t spa=pPktHdr->pRxDesc->opts3.bit.src_port_num;
	//DEBUG("spa=%d SMAC=%02x:%02x:%02x:%02x:%02x:%02x\n",spa,skb->data[6],skb->data[7],skb->data[8],skb->data[9],skb->data[10],skb->data[11]);


    memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));

	//DEBUG("@@@ port%d %02x:%02x:%02x:%02x:%02x:%02x @@@\n",pPktHdr->pRxDesc->opts3.bit.src_port_num,pPktHdr->pSmac[0],pPktHdr->pSmac[1],pPktHdr->pSmac[2],
		//pPktHdr->pSmac[3],pPktHdr->pSmac[4],pPktHdr->pSmac[5]);
	/*if(pPktHdr->pRxDesc->opts2.bit.ctagva==1)
	{		
		macEntry.vlan_id=((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xf)<<0x8)+((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xff00)>>0x8);
		macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
		//DEBUG("in layer2 agent, the vlan id from RX descriptor is %d\n",macEntry.vlan_id);
	}
	else if((pPktHdr->tagif&CVLAN_TAGIF)>0)
	{
		macEntry.vlan_id=pPktHdr->ctagVid;
		macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
		//DEBUG("in layer2 agent, the vlan id from CTAG is %d\n",macEntry.vlan_id);
	}
	else
	{
		//DEBUG("in layer2 agent, untag..");
		macEntry.vlan_id=0;		//untagged
		macEntry.fid=LAN_FID;
	}*/
	if(rg_db.vlan[pPktHdr->internalVlanID].valid==0)		//unknown 1Q vlan should't learn
	{
		DEBUG("internalVID=%d valid=%d\n",pPktHdr->internalVlanID,rg_db.vlan[pPktHdr->internalVlanID].valid);
		return FAIL;
	}
	macEntry.vlan_id=pPktHdr->internalVlanID;
	macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
	//DEBUG("the internalVlanID is %d, fid is %d",macEntry.vlan_id,macEntry.fid);
#if 1

	if(/*macEntry.vlan_id!=0 && */rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL)
	{
		macEntry.isIVL=1;
		l2Idx=_rtk_rg_hash_mac_vid_efid(pPktHdr->pSmac,macEntry.vlan_id,0);		//FIXME:EFID is 0 now
	}
	else
	{
ADD_SVL_LUT:
		count=0;
		macEntry.isIVL=0;
		if(pPktHdr->pRxDesc->opts2.bit.ctagva==0 && (pPktHdr->tagif&CVLAN_TAGIF)==0)
			macEntry.vlan_id=0;		//untag
		//if(rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<pPktHdr->pRxDesc->opts3.bit.src_port_num))macEntry.vlan_id=0;		//untag
		l2Idx=_rtk_rg_hash_mac_fid_efid(pPktHdr->pSmac,macEntry.fid,0);			//FIXME:EFID is 0 now
	}

	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty, just add
						
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
			(memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pPktHdr->pSmac,ETHER_ADDR_LEN)==0))
		{
			if(((macEntry.isIVL==1) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
			((macEntry.isIVL==0) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
			{
				if(pPktHdr->pRxDesc->opts3.bit.reason==192 /* SA learning limit */ || pPktHdr->pRxDesc->opts3.bit.reason==196 /* unknown SA action */)
				{
					//Force SA learning
					DEBUG("Forced SA learning...reason=%d",pPktHdr->pRxDesc->opts3.bit.reason);
					atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[spa]);
					break;
				}
				//DEBUG("match!!");
				/*
				//FIXME: here reserved for WiFi interface may also need to handle port-moving in the future.
				if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port==RTK_RG_MAC_PORT_CPU)
				{

				}
				else */
				if(	!(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC))
				{
					if((spa<RTK_RG_PORT_CPU && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port!=spa) || 
						(spa>=RTK_RG_PORT_CPU && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port!=(spa-RTK_RG_PORT_CPU)))
					{
						//Mac port-moving, update LUT table without change ARP_USED flag
						//------------------ Critical Section start -----------------------//
						//rg_lock(&rg_kernel.saLearningLimitLock);
						if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
							atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
						else
							atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port]);
						//------------------ Critical Section End -----------------------//
						//rg_unlock(&rg_kernel.saLearningLimitLock);
		
						macEntry.arp_used=((rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)>0)?1:0;
						DEBUG("the port is moving..arp used is %d\n",macEntry.arp_used);
						break;
					}
				}

				if(macEntry.isIVL)goto ADD_SVL_LUT;		//check SVL,too
				return SUCCESS;		//exist, do nothing
			}
		}

		count++; //search from next entry
	}while(count < 4);

	if(count==4)
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table

	//Check per port SA learning limit
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[spa]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[spa]))		//no way to learn
	{
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.saLearningLimitLock);
		DEBUG("Port %d SA learning limit is reached(%d)...action is %s!!",spa,rg_db.systemGlobal.sourceAddrLearningLimitNumber[spa],
			rg_db.systemGlobal.sourceAddrLearningAction[spa]==SA_LEARN_EXCEED_ACTION_PERMIT?"Permit and Forward":"Drop");
		if(rg_db.systemGlobal.sourceAddrLearningAction[spa]==SA_LEARN_EXCEED_ACTION_PERMIT)
			return SUCCESS;
		else
			return FAIL;
	}
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);

	//Replace the least recently used entry for new entry
	if(count==4)
	{
		search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx);
		if(search_index==-1)
		{
			FIXME("must add software LUT entry for LUT entry full.");
			return FAIL;
		}			
			
	}
	else
		search_index=l2Idx+count;
#else
	//test for rtk_l2_addr_get
	rtk_l2_ucastAddr_t l2Addr;
	memset(&l2Addr,0,sizeof(rtk_l2_ucastAddr_t));

	l2Addr.fid=LAN_FID;
	memcpy(&l2Addr.mac.octet,pPktHdr->pSmac,6);

	ret=rtk_l2_addr_get(&l2Addr);
	DEBUG("rtk_l2_addr_get ret=%x",ret);
	if(ret==RT_ERR_OK)return SUCCESS;		//exist

#endif
	memcpy(macEntry.mac.octet,pPktHdr->pSmac,ETHER_ADDR_LEN);
	//set SVL for lanIntf, patched in 201221203
	//macEntry.fid=LAN_FID;
	//macEntry.isIVL=0;
	macEntry.port_idx=spa;
	macEntry.static_entry=0;	//FIXME:here turn off static entry to enable hardware auto age-out and port-moving
	ret=rtk_rg_macEntry_add(&macEntry,&search_index);
	DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x SPA=%d ###\n",search_index,pPktHdr->pSmac[0],pPktHdr->pSmac[1],pPktHdr->pSmac[2],
		pPktHdr->pSmac[3],pPktHdr->pSmac[4],pPktHdr->pSmac[5],spa);
	assert_ok(ret);

	//add to SA learning count
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	atomic_inc(&rg_db.systemGlobal.sourceAddrLearningCount[spa]);
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);

	if(macEntry.isIVL)goto ADD_SVL_LUT;		//add SVL,too
	return SUCCESS;
}
 
int _rtk_rg_layer2Agent(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	int ret=0;

 
 	//Drop multicast SMAC here
 	if((pPktHdr->pSmac[0]&0x1)>0)
 	{
  		//dump_hs();
  		dump_packet(skb->data,skb->len,"strange multicast source mac");
  		DEBUG("drop strange multicast packet...");
  		return RG_FWDENGINE_RET_DROP;
 	}

 	ret=_rtk_rg_layer2LutLearning(skb,pPktHdr,pPktHdr->ingressPort);

	if(ret==SUCCESS)
		return RG_FWDENGINE_RET_L2FORWARDED;
	else
		return RG_FWDENGINE_RET_DROP;		//drop because sa learning limit action or unknown VLAN id 
}
#else
int _rtk_rg_layer2Agent(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	rtk_rg_macEntry_t macEntry;
	int ret,l2Idx,search_index,count=0;

	/* Handle multicast packet*/
#ifdef __KERNEL__
	if(rtk_rg_multicastRxCheck(skb,pPktHdr,pPktHdr->pRxDesc->opts3.bit.src_port_num)==SUCCESS)
		return RG_FWDENGINE_RET_TO_PS;
#endif

	//Drop multicast SMAC here
	if((pPktHdr->pSmac[0]&0x1)>0)
	{
		//dump_hs();
		dump_packet(skb->data,skb->len,"strange multicast source mac");
		DEBUG("drop strange multicast packet...");
		return RG_FWDENGINE_RET_DROP;
	}

	memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));

	//DEBUG("@@@ port%d %02x:%02x:%02x:%02x:%02x:%02x @@@\n",pPktHdr->pRxDesc->opts3.bit.src_port_num,pPktHdr->pSmac[0],pPktHdr->pSmac[1],pPktHdr->pSmac[2],
		//pPktHdr->pSmac[3],pPktHdr->pSmac[4],pPktHdr->pSmac[5]);
	/*if(pPktHdr->pRxDesc->opts2.bit.ctagva==1)
	{		
		macEntry.vlan_id=((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xf)<<0x8)+((pPktHdr->pRxDesc->opts2.bit.cvlan_tag&0xff00)>>0x8);
		macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
		//DEBUG("in layer2 agent, the vlan id from RX descriptor is %d\n",macEntry.vlan_id);
	}
	else if((pPktHdr->tagif&CVLAN_TAGIF)>0)
	{
		macEntry.vlan_id=pPktHdr->ctagVid;
		macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
		//DEBUG("in layer2 agent, the vlan id from CTAG is %d\n",macEntry.vlan_id);
	}
	else
	{
		//DEBUG("in layer2 agent, untag..");
		macEntry.vlan_id=0;		//untagged
		macEntry.fid=LAN_FID;
	}*/
	macEntry.vlan_id=pPktHdr->internalVlanID;
	macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
	//DEBUG("the internalVlanID is %d, fid is %d",macEntry.vlan_id,macEntry.fid);
#if 1

	if(/*macEntry.vlan_id!=0 && */rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL)
	{
		macEntry.isIVL=1;
		l2Idx=_rtk_rg_hash_mac_vid_efid(pPktHdr->pSmac,macEntry.vlan_id,0);		//FIXME:EFID is 0 now
	}
	else
	{
ADD_SVL_LUT:
		count=0;
		macEntry.isIVL=0;
		if(pPktHdr->pRxDesc->opts2.bit.ctagva==0 || (pPktHdr->tagif&CVLAN_TAGIF)==0)
			macEntry.vlan_id=0;		//untag
		//if(rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<pPktHdr->pRxDesc->opts3.bit.src_port_num))macEntry.vlan_id=0;		//untag
		l2Idx=_rtk_rg_hash_mac_fid_efid(pPktHdr->pSmac,macEntry.fid,0);			//FIXME:EFID is 0 now
	}
	
	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty, just add
						
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
			(memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pPktHdr->pSmac,6)==0))
		{
			if(((macEntry.isIVL==1) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
			((macEntry.isIVL==0) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
			{
				//DEBUG("match!!");
				/*
				//FIXME: here reserved for WiFi interface may also need to handle port-moving in the future.
				if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port==RTK_RG_MAC_PORT_CPU)
				{

				}
				else */if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port!=pPktHdr->pRxDesc->opts3.bit.src_port_num)
				{
					//Mac port-moving, update LUT table without change ARP_USED flag
					//------------------ Critical Section start -----------------------//
					//rg_lock(&rg_kernel.saLearningLimitLock);
					rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port]--;
					//------------------ Critical Section End -----------------------//
					//rg_unlock(&rg_kernel.saLearningLimitLock);
	
					macEntry.arp_used=((rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)>0)?1:0;
					//DEBUG("the port is moving..arp used is %d\n",macEntry.arp_used);
					break;
				}

				if(macEntry.isIVL)goto ADD_SVL_LUT;		//check SVL,too
				return RG_FWDENGINE_RET_L2FORWARDED;		//exist, do nothing
			}
		}

		count++; //search from next entry
	}while(count < 4);

	if(count==4)
	{
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table
		if(count==4)
			search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx);		//replace the least recently used entry for new entry
		else
			search_index=l2Idx+count;
	}

	//Check per port SA learning limit
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[pPktHdr->pRxDesc->opts3.bit.src_port_num]==rg_db.systemGlobal.sourceAddrLearningCount[pPktHdr->pRxDesc->opts3.bit.src_port_num])		//no way to learn
	{
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.saLearningLimitLock);
		DEBUG("Port %d SA learning limit is reached(%d)...action is %s!!",pPktHdr->pRxDesc->opts3.bit.src_port_num,rg_db.systemGlobal.sourceAddrLearningLimitNumber[pPktHdr->pRxDesc->opts3.bit.src_port_num],
			rg_db.systemGlobal.sourceAddrLearningAction[pPktHdr->pRxDesc->opts3.bit.src_port_num]==SA_LEARN_EXCEED_ACTION_PERMIT?"Permit and Forward":"Drop");
		if(rg_db.systemGlobal.sourceAddrLearningAction[pPktHdr->pRxDesc->opts3.bit.src_port_num]==SA_LEARN_EXCEED_ACTION_PERMIT)
			return RG_FWDENGINE_RET_L2FORWARDED;
		else
			return RG_FWDENGINE_RET_DROP;
	}
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);
#else
	//test for rtk_l2_addr_get
	rtk_l2_ucastAddr_t l2Addr;
	memset(&l2Addr,0,sizeof(rtk_l2_ucastAddr_t));

	l2Addr.fid=LAN_FID;
	memcpy(&l2Addr.mac.octet,pPktHdr->pSmac,6);

	ret=rtk_l2_addr_get(&l2Addr);
	DEBUG("rtk_l2_addr_get ret=%x",ret);
	if(ret==RT_ERR_OK)return SUCCESS;		//exist

#endif
	memcpy(macEntry.mac.octet,pPktHdr->pSmac,6);
	//set SVL for lanIntf, patched in 201221203
	//macEntry.fid=LAN_FID;
	//macEntry.isIVL=0;
	macEntry.port_idx=pPktHdr->pRxDesc->opts3.bit.src_port_num;
	macEntry.static_entry=0;	//FIXME:here turn off static entry to enable hardware auto age-out and port-moving
	ret=rtk_rg_macEntry_add(&macEntry,&search_index);
	DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",search_index,pPktHdr->pSmac[0],pPktHdr->pSmac[1],pPktHdr->pSmac[2],
		pPktHdr->pSmac[3],pPktHdr->pSmac[4],pPktHdr->pSmac[5]);
	assert_ok(ret);

	//add to SA learning count
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	rg_db.systemGlobal.sourceAddrLearningCount[pPktHdr->pRxDesc->opts3.bit.src_port_num]++;
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);

	if(macEntry.isIVL)goto ADD_SVL_LUT;		//add SVL,too
	return RG_FWDENGINE_RET_L2FORWARDED;
}
#endif

#ifdef __KERNEL__
int _rtk_rg_layer2Forward(struct re_private *cp, struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	int i,ret;
	//unsigned char WANIntfMask=0;

	/* Handle software Learning of LUT table*/
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	//if(pPktHdr->isGatewayPacket == 0)	//DA =\= Gateway MAC, do Layer2 agent
	//{
		ret=_rtk_rg_layer2Agent(skb,pPktHdr);
		if(ret!=RG_FWDENGINE_RET_L2FORWARDED)return ret;		//multicast packet may return to PS
	//}
#endif

	/* Handle multicast packet*/
#ifdef __KERNEL__
	if(rtk_rg_multicastRxCheck(skb,pPktHdr,pPktHdr->ingressPort)==SUCCESS)
		return RG_FWDENGINE_RET_TO_PS;
#endif

	/* Handle packet has special CPU trap reason*/
	switch(pPktHdr->pRxDesc->opts3.bit.reason)
	{
   	    case 0: //normal forward
   	    case 0x8:	//8: NAT/NAPT (Layer 4) Fragmented IPv4 packets
		case 0x9:	//9: Routed (Layer 3) Fragmented IPv4/v6 packets
		case 0x22: //34: NAPT lookup miss
   	    	break;
		case 0x6:	//6: PPPoE ID lookup miss for Layer 3/4 forwarding
			DEBUG("PPPoE ID lookup miss for L34 forwarding");
			break;
		case 0x15:	//21: ARP or Neighbor miss
			if((pPktHdr->tagif&IPV6_TAGIF)==0)
			{
				ret=_rtk_rg_fwdengine_handleArpMiss(pPktHdr);
				if(ret==RG_FWDENGINE_RET_TO_PS)return ret;
			}
			else
			{
				ret=_rtk_rg_fwdengine_handleNeighborMiss(pPktHdr);
				if(ret==RG_FWDENGINE_RET_TO_PS)return ret;
			}
			break;
		case 0xCC:	//204: unknown DA for unicast packet
			if(pPktHdr->isGatewayPacket==0)
			{

////=======lookup DMAC start==========
				int l2Idx;
				int count=0;
				
				if(rg_db.vlan[pPktHdr->internalVlanID].fidMode==VLAN_FID_IVL)
				{
					l2Idx=_rtk_rg_hash_mac_vid_efid(pPktHdr->pDmac,pPktHdr->internalVlanID,0);		//FIXME;current efid is always 0
				}
				else
				{
					l2Idx=_rtk_rg_hash_mac_fid_efid(pPktHdr->pDmac,rg_db.vlan[pPktHdr->internalVlanID].fid,0);		//FIXME;current efid is always 0
				}
				
				l2Idx<<=2;
				do
				{						
					if(rg_db.lut[l2Idx].rtk_lut.entryType==RTK_LUT_L2UC &&
						(!memcmp(&rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,pPktHdr->pDmac,ETHER_ADDR_LEN)))
					{
						if((rg_db.vlan[pPktHdr->internalVlanID].fidMode==VLAN_FID_IVL && rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.vid==pPktHdr->internalVlanID) ||
							(rg_db.vlan[pPktHdr->internalVlanID].fidMode==VLAN_FID_SVL && rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.fid==rg_db.vlan[pPktHdr->internalVlanID].fid))
						{
							rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.age=7;
							RTK_L2_ADDR_ADD(&rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry);
							DEBUG("unicast unknown DA %02x:%02x:%02x:%02x:%02x:%02x...force sync to HW-LUT",pPktHdr->pDmac[0],pPktHdr->pDmac[1],pPktHdr->pDmac[2],pPktHdr->pDmac[3],pPktHdr->pDmac[4],pPktHdr->pDmac[5]);
							break;
						}
					}
					else
					{
						count++; //search from next entry
						l2Idx++;
					}
				}
				while(count < 4);
				
				if(count<4) break;

////=======lookup DMAC End==========


				
				DEBUG("unicast unknown DA %02x:%02x:%02x:%02x:%02x:%02x...go to broadcast",pPktHdr->pDmac[0],pPktHdr->pDmac[1],pPktHdr->pDmac[2],pPktHdr->pDmac[3],pPktHdr->pDmac[4],pPktHdr->pDmac[5]);
				return RG_FWDENGINE_RET_BROADCAST;
			}
			break;
		default:
			TRACE("not be handled reason=%d\n",pPktHdr->pRxDesc->opts3.bit.reason);
			break;
	}

	/* Handle packet has special etherType*/
	switch(pPktHdr->etherType)
	{
//		case 0x8809:										/* pass OAM packet */
//#ifdef CONFIG_DUALBAND_CONCURRENT
//		case CONFIG_DEFAULT_IPC_SEND_ETHERTYPE:
//		case CONFIG_DEFAULT_IPC_RECV_ETHERTYPE:			
//		case CONFIG_DEFAULT_IPC_SIGNAL_ETHERTYPE:
//#endif			
//			return RG_FWDENGINE_RET_TO_PS;
		case 0x0806:										/* Handle ARP packet */
			//memDump(skb->data,skb->len,"rx 0806 data");
			_rtk_rg_arpAgent(skb->data,skb->len,pPktHdr);

			//if(pPktHdr->arpOpCode==1)		//ARP request to protocol stack, ARP reply will L2 forward
			//{
				//Check Lan interface IP address
				for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
				{
					if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->ip_addr==pPktHdr->ipv4Dip)
					{
						//DEBUG("ARP %s to LAN interface %d, return to PS",pPktHdr->arpOpCode==1?"request":"reply",rg_db.systemGlobal.lanIntfGroup[i].index);						
						return RG_FWDENGINE_RET_TO_PS;
					}
				}

				//Check Wan interface IP address
				for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
				{
					if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type!=RTK_RG_BRIDGE && 
						rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ip_addr==pPktHdr->ipv4Dip)
					{
						//DEBUG("ARP %s to WAN interface %d, return to PS",pPktHdr->arpOpCode==1?"request":"reply",rg_db.systemGlobal.wanIntfGroup[i].index);
						//dump_packet(skb->data,skb->len,"arp packet");
#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK
						cp->wanInterfaceIdx=i;
#endif				
						return RG_FWDENGINE_RET_TO_PS;	
					}
				}
			//}
			break;
		case 0x8863:										/* Handle PPPoE control packet */
			//if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)		/* Handle broadcast packet to routing WAN if PPPoE pass through is turn on*/
			//{
				//DEBUG("Pass through turn on!!");
				//Check DA for WAN interface, return to PS if match
				/*for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
				{
					if(memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.gmac.octet,pPktHdr->pDmac,6)==0)
					{
						//DEBUG("match WAN! to PS now");
						return RG_FWDENGINE_RET_TO_PS;
					}

					//get all WAN should be put in broadcast mask
					
				}*/
				if(pPktHdr->isGatewayPacket == 1)
				{
					//packet should be learned in layer2Agent before goto protocol stack!!
					//ret=_rtk_rg_layer2Agent(skb,pPktHdr);
					//if(ret!=RG_FWDENGINE_RET_L2FORWARDED)return ret;		//multicast packet may return to PS
					
					//DEBUG("8863 packet, before return to protocol stack");
					//dump_packet(skb->data,skb->len,"8863 packet");
#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK
					//Check Lan MAC address, actually this may not be needed, since PPPoE should on WAN interface only....
					/*for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
					{
						if(memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac.octet,pPktHdr->pDmac,6)==0)
						{
							cp->wanInterfaceIdx=0;
							goto RET_TO_PS;
						}
					}*/

					//Check Wan 
					for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
					{
						if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_PPPoE && 
							memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->gmac.octet,pPktHdr->pDmac,ETHER_ADDR_LEN)==0)
						{
							cp->wanInterfaceIdx=i;
							break;
						}
					}
//RET_TO_PS:
#endif
					//DEBUG("the cp->waninterfaceIdx is %d",cp->wanInterfaceIdx);
					return RG_FWDENGINE_RET_TO_PS;
				}
			
				if((pPktHdr->pDmac[0]&pPktHdr->pDmac[1]&pPktHdr->pDmac[2]&pPktHdr->pDmac[3]&pPktHdr->pDmac[4]&pPktHdr->pDmac[5])==0xff)
				{
					//DEBUG("layer2 broadcast packet");
					if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)		/* Handle broadcast packet to routing WAN if PPPoE pass through is turn on*/
						pPktHdr->internalVlanID=DEFAULT_CPU_VLAN;	//pass through between LAN and WAN interface
					return RG_FWDENGINE_RET_BROADCAST;
				} 
				else
				{
					//DEBUG("layer2 unicast packet...hardware lookup");
					goto layer2_return;		//unicast L2 packet should be forwarded as hardware lookup
				}
				//{
	 				//Unicast packet in PPPoE Passthrough, change VID and port from DMAC2CVID
					//DEBUG("the packet is PPPoE control unicast packet, just bridging..");
//#ifdef __KERNEL__
					//turn on txInfo mask, otherwise value won't be add
					/*rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
					rg_kernel.txDescMask.opts1.bit.ipcs=1;
					rg_kernel.txDescMask.opts1.bit.l4cs=1;
					rg_kernel.txDesc.opts1.bit.ipcs=1;
					rg_kernel.txDesc.opts1.bit.l4cs=1;
					rg_kernel.txDesc.opts2.bit.tx_vlan_action = 0x2;		//removing

					ret=_rtk_rg_fwdEngineDMAC2CVIDLookup(pPktHdr,LAN_FID,0);		//FIXME:we use only one FID right now
					if(ret!=RT_ERR_RG_OK)
						return RG_FWDENGINE_RET_TO_PS;

					re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);	
					return RG_FWDENGINE_RET_DIRECT_TX;*/

					
					/*ret=_rtk_rg_fwdEngineDirectTx(skb,pPktHdr);
					if(ret==RT_ERR_RG_OK)
						return RG_FWDENGINE_RET_DIRECT_TX;*/
//#endif	
				//}
			//}
			
			//Otherwise 8863 will be sended to protocol stack
			//return RG_FWDENGINE_RET_TO_PS;
		case 0x8864:									/* Handle PPPoE data packet */
			//Check layer2 packet(without IP header)
			if(pPktHdr->isGatewayPacket==1)
			{
				if((pPktHdr->tagif&(IPV4_TAGIF|IPV6_TAGIF))==0)
					return RG_FWDENGINE_RET_TO_PS;
				else
					return RG_FWDENGINE_RET_CONTINUE;	//continue to L34 forward
			}
			
			if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)		/* Handle broadcast packet to routing WAN if PPPoE pass through is turn on*/
			{
				if((pPktHdr->pDmac[0]&pPktHdr->pDmac[1]&pPktHdr->pDmac[2]&pPktHdr->pDmac[3]&pPktHdr->pDmac[4]&pPktHdr->pDmac[5])==0xff)
				{
					//DEBUG("broadcast packet");
					if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)		/* Handle broadcast packet to routing WAN if PPPoE pass through is turn on*/
						pPktHdr->internalVlanID=DEFAULT_CPU_VLAN;	//pass through between LAN and WAN interface
					return RG_FWDENGINE_RET_BROADCAST;
				} 
				else
				{
					//DEBUG("the packet is PPPoE data unicast packet to %02x:%02x:%02x:%02x:%02x:%02x, just bridging..",
						//pPktHdr->pDmac[0],pPktHdr->pDmac[1],pPktHdr->pDmac[2],pPktHdr->pDmac[3],pPktHdr->pDmac[4],pPktHdr->pDmac[5]);
					goto layer2_return;		//unicast L2 packet should be forwarded as hardware lookup
	 				//Unicast packet in PPPoE Passthrough, change VID and port from DMAC2CVID
					//dump_packet(skb->data,skb->len,"sc");
//#ifdef __KERNEL__
//					ret=_rtk_rg_fwdEngineDirectTx(skb,pPktHdr);
//					if(ret==RT_ERR_RG_OK)
//						return RG_FWDENGINE_RET_DIRECT_TX;
//#endif	
				}
			}
			break;
		case 0x0800:	//IPv4
		case 0x86dd:	//IPv6
			break;
		default:
			if(pPktHdr->isGatewayPacket) //unknown ethertype & DMAC=GMAC
				return RG_FWDENGINE_RET_TO_PS; 
			break;
	}

	//Continue to L34forward
	if(pPktHdr->isGatewayPacket) //known ethertype & DMAC=GMAC
		return RG_FWDENGINE_RET_CONTINUE;
	
	/* Handle broadcast packet or unknown DA unicast packet*/
	if((pPktHdr->pDmac[0]&1)==1)
	{
		return RG_FWDENGINE_RET_BROADCAST;
	}
	
layer2_return:
	TRACE("Layer2 Forward");
	return RG_FWDENGINE_RET_L2FORWARDED;
}

void _rtk_rg_extIngressPortDecision(rtk_rg_pktHdr_t *pPktHdr,rtk_rg_rxdesc_t	*pRxDesc)
{
	if(pRxDesc->opts3.bit.src_port_num==RTK_RG_MAC_PORT_CPU)
	{
		if(pRxDesc->opts3.bit.dst_port_mask==0x8) //from EXT0 (Using ACL modify EXT_SPA to DST_PMSK)
		{
			pPktHdr->ingressPort=RTK_RG_EXT_PORT0;
		}
#ifdef CONFIG_DUALBAND_CONCURRENT
		else if(pRxDesc->opts3.bit.dst_port_mask==0x10) //from EXT1 (Using ACL modify EXT_SPA to DST_PMSK)
		{
			pPktHdr->ingressPort=RTK_RG_EXT_PORT1;				
		}
#endif
		else
		{
			pPktHdr->ingressPort=RTK_RG_MAC_PORT_CPU;
		}		
	}
	else
	{
		pPktHdr->ingressPort=pRxDesc->opts3.bit.src_port_num;
	}
}

int _rtk_rg_wlanExtraDataPathDecision(rtk_rg_pktHdr_t *pPktHdr,rtk_rg_rxdesc_t *pRxDesc,struct sk_buff *skb)
{
#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION
		//check from which WLAN interface(root,vap0,vap1,vap2,vap3)
		if(pRxDesc==(rtk_rg_rxdesc_t *)&rg_kernel.rxInfoFromWLAN)
		{
			int i;
			pPktHdr->wlan_dev_idx=0;
			for(i=0;i<4;i++)
			{
				if(wlan_vap_netdev[i]==NULL) break;
				if(skb->dev==wlan_vap_netdev[i]) pPktHdr->wlan_dev_idx=i+1;	
			}			
			_rtk_rg_wlanMbssidLearning(&skb->data[6],pPktHdr->wlan_dev_idx);
		}

		if(pRxDesc->opts3.bit.src_port_num==RTK_RG_MAC_PORT_CPU) //from CPU,EXT0,EXT1
		{
	 //ingress_port_changed:
			if(pPktHdr->ingressPort==RTK_RG_EXT_PORT0) //from EXT0
			{
				if((skb->data[0]&1)==1) //from EXT0 BC/MC to-PS
				{
					TRACE("(from EXT0 BC/MC), into FwdEngine!"); //let IGMP packets learn to HW.
				}
				else //Unicsat
				{
					if(pRxDesc==(rtk_rg_rxdesc_t *)&rg_kernel.rxInfoFromWLAN) //from WIFI directly, not trap from GMAC trap
					{
						//HWLOOKUP
						TRACE("From EXT0 UC which from WIFI0, forward by HWLOOKUP.");
						return RG_FWDENGINE_RET_HWLOOKUP;
					}
					else
					{
						//if from GMAC and without trap reason, just send to Master; otherwise goto slow path
						if(pRxDesc->opts3.bit.reason==0)
						{
							if(_rtk_master_wlan_mbssid_tx(skb)==FAIL)
							{
								FIXME("DMAC not found!");
								return RG_FWDENGINE_RET_DROP;
							}
							return RG_FWDENGINE_RET_CONTINUE;
						}
					}
				}
				// from EXT0 UC normal forward
			}
#ifdef CONFIG_DUALBAND_CONCURRENT
			else if((pPktHdr->ingressPort==RTK_RG_EXT_PORT1)&&((skb->data[0]&1)==0))	//from EXT1 Unicast
			{
				if(pRxDesc->opts3.bit.reason==0)
				{
					int i;
					//normal forward to Master CPU				
					for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
					{
						if(memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac.octet,skb->data,ETHER_ADDR_LEN)==0)
						{
							// DMAC=GMAC to Protocol Stack
							TRACE("from (WIFI2)EXT1 UC to LAN_MAC to PS!\n");
							return RG_FWDENGINE_RET_TO_PS;
						}
					}
					if(memcmp(master_ipc_macAddr.octet,skb->data,ETHER_ADDR_LEN)==0)
					{
							// DMAC=GMAC to Protocol Stack
							TRACE("from (WIFI2)EXT1 UC to LAN_MAC to PS!\n");
							return RG_FWDENGINE_RET_TO_PS;
					}
					if(wlan_root_netdev!=NULL)
					{
						TRACE("from EXT1 UC to WIFI1(EXT0)!\n");
						if(_rtk_master_wlan_mbssid_tx(skb)==FAIL) return RG_FWDENGINE_RET_DROP;
						return RG_FWDENGINE_RET_CONTINUE; //free skb by wifi driver.
					}
					else
					{
						return RG_FWDENGINE_RET_DROP;
					}
					
				}
				else
				{				
					TRACE("from (WIFI2)EXT1 UC to Master-CPU(trap) into fwdEngine!\n"); 
				}
			}		
			else if((pPktHdr->ingressPort==RTK_RG_EXT_PORT1)&&((skb->data[0]&1)==1))	//from EXT1 MC/BC
			{
				if(skb->data[0]==0xff)
				{
					//_rtk_rg_layer2LutLearning(skb,pPktHdr,pPktHdr->ingressPort);			
					FIXME("from (WIFI2)EXT1 BC to Master-CPU goto FwdEngine, skb_len=%d!\n",skb->len);
					//return RG_FWDENGINE_RET_TO_PS;
				}
				else
				{
					FIXME("from (WIFI2)EXT1 MC to Master-CPU goto FwdEngine, skb_len=%d!\n",skb->len);					
				}
			} 
			else if(pRxDesc->opts3.bit.dst_port_mask==0x4) // to EXT1
			{
				FIXME("unknow from CPU to EXT1 packet, skb_len=%d!\n",skb->len);
				return RG_FWDENGINE_RET_DROP;
			}		
#endif
			else if(pRxDesc->opts3.bit.dst_port_mask==0x2) // to EXT0
			{
				if(wlan_root_netdev!=NULL)
				{	
					TRACE("CPU(GMAC1) to EXT0 packet, skb_len=%d!\n",skb->len);					
					if(_rtk_master_wlan_mbssid_tx(skb)==FAIL) return RG_FWDENGINE_RET_DROP;
					return RG_FWDENGINE_RET_CONTINUE; //free skb by wifi driver
				}
				else
				{
					return RG_FWDENGINE_RET_DROP;
				}
			}
	
			else  // from CPU to CPU
			{
				//dump_hs();
				//patch for ACL+CF rule will over-write acl(src ext-port  to dest-ext-portmask) rule.
				//trace SMAC is from which port? if from EXT port, modify ingress port and goto ingress_port_changed.
#if 0			
				if(memcmp(skb->data+6,"\x00\x11\x33\x55\x77\xcc",6)==0)
				{
					printk("*** from EXT1 modify ingress port to EXT1 ***");				
					pPktHdr->ingressPort=RTK_RG_EXT_PORT1;
					goto ingress_port_changed;
				}	
#endif		

				if(pRxDesc->opts3.bit.reason==207) //Unkown IPv6 MC
				{
					TRACE("From PS unkown MC DA trap to here, and will forward by FwdEngine\n");
					return FAIL; //goto slow path (flooding to each interface)
				}
				
				TRACE("CPU to CPU: drop by src port filter, skb_len=%d! SPA=%d dstPmsk=0x%x\n",skb->len,pRxDesc->opts3.bit.src_port_num,pRxDesc->opts3.bit.dst_port_mask);
				return RG_FWDENGINE_RET_DROP;
			}
		}
		else //NOT FROM CPU
		{
			if((skb->data[0]&1)==0) //Unicast
			{
				if(pRxDesc->opts3.bit.dst_port_mask==0x2)  //from Physical Port to EXT0 Unicast
				{
		//			_rtk_rg_layer2LutLearning(skb,&pPktHdr->,2); //SA Learnning
					if(wlan_root_netdev!=NULL)
					{
						TRACE("from Physical Port to EXT0 Unicast, send to WIFI1, skb_len=%d!\n",skb->len);
						if(_rtk_master_wlan_mbssid_tx(skb)==FAIL) return RG_FWDENGINE_RET_DROP;			
						return RG_FWDENGINE_RET_CONTINUE;
					}
					else
						return RG_FWDENGINE_RET_DROP;
					
				}
#ifdef CONFIG_DUALBAND_CONCURRENT
				else if(pRxDesc->opts3.bit.dst_port_mask==0x4) //from Physical Port to EXT1 Unicast
				{
					TRACE("To EXT1 UC packets which trap to master CPU must be forwarded by fwdEngine");
				}
#endif
			}
			else if(skb->data[0]==0xff) //Broadcast
			{
				TRACE("not from CPU Broadcast goto FwdEngine, skb_len=%d!\n",skb->len);
			}
			else if((skb->data[0]&1)==1) //Multicast
			{
#if 1			
				if((pRxDesc->opts3.bit.dst_port_mask&0x1)==1)
				{
					if(pRxDesc->opts3.bit.reason!=0)
					{
						TRACE("Unkown Trap Reason, goto to slow path.");
						return FAIL;
					}					
					else if((pPktHdr->tagif&IPV4_TAGIF)&&(pPktHdr->ipv4Dip>=0xe0000100)&&(pPktHdr->ipv4Dip<=0xeeffffff))
					{
						TRACE("Drop IPv4 multicast data to CPU packet...(just send to WIFI)");
						//don't do anything here
					}
					else if((pPktHdr->tagif&IPV6_TAGIF)&&(pPktHdr->pIpv6Dip[0]==0xff)&&((pPktHdr->pIpv6Dip[1]&0x0f)==0xe))
					{
						TRACE("Drop IPv6 multicast data to CPU packet...(just send to WIFI)");
						//don't do anything here
					}					
					else
					{
						TRACE("Unkown Multicast packets send to CPU and go to slow path.");
						return FAIL;
					}
				}
				
				if(pRxDesc->opts3.bit.dst_port_mask&0x6)  //from Physical Port to EXT1 or EXT0 Multicast, if DstPortMask have CPU Port, this packet will forward by normal path.
				{				
					TRACE("Multicast packets to EXT0 or EXT1 port will forward by master CPU.");
				
					if(memcmp(skb->data,"\x01\x80\xc2",3)==0)
					{
						FIXME("trap 01:80:C2:%02x:%02x:%02x",skb->data[3],skb->data[4],skb->data[5]);
						return RG_FWDENGINE_RET_TO_PS;
					}



#ifdef CONFIG_DUALBAND_CONCURRENT				

					if(pRxDesc->opts3.bit.dst_port_mask&0x4)
					{

						struct sk_buff *skb_new=NULL;
						if(pRxDesc->opts3.bit.dst_port_mask&0x6)
						{
							skb_new=skb_clone(skb,GFP_KERNEL);
							if(skb_new==NULL) return RG_FWDENGINE_RET_DROP;
						}
						else
						{
							skb_new=skb;
						}
					
						//send to WIFI2(EXT1)
						rg_kernel.txDescMask.opts1.dw=0;
						rg_kernel.txDescMask.opts2.dw=0;	
						rg_kernel.txDescMask.opts3.dw=0;
						//rg_kernel.txDescMask.opts4.dw=0;	//not used now!
						rg_kernel.txDesc.opts1.dw=0;
						rg_kernel.txDesc.opts2.dw=0;	
						rg_kernel.txDesc.opts3.dw=0;
	
						
						rg_kernel.txDescMask.opts1.bit.dislrn=1;					
						rg_kernel.txDescMask.opts2.bit.vidl=0xff;
						rg_kernel.txDescMask.opts2.bit.vidh=0xf;
						rg_kernel.txDescMask.opts2.bit.prio=0x7;
						rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f; //no cpu port
						rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0;// no-action
	
	
						rg_kernel.txDesc.opts1.bit.dislrn=1; // patch for reason 192			
						rg_kernel.txDesc.opts2.bit.vidl=CONFIG_DEFAULT_TO_SLAVE_GMAC_VID&0xff;
						rg_kernel.txDesc.opts2.bit.vidh=CONFIG_DEFAULT_TO_SLAVE_GMAC_VID>>8;
						rg_kernel.txDesc.opts2.bit.prio=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;		
						rg_kernel.txDesc.opts3.bit.tx_portmask=0; //HWLOOKUP (because: HW do not have extension port & CPU port bit)
						rg_kernel.txDesc.opts2.bit.tx_vlan_action=3;// remarking
						TRACE("Multicast from master CPU to WLAN2 by GMAC(VID=1,PRI=4,HWLOOKUP)");	
						_rtk_rg_egressPacketSend(skb_new,pPktHdr);				
						
					}	
#endif


					if(pRxDesc->opts3.bit.dst_port_mask&0x2)
					{
						if(wlan_root_netdev)
						{
							TRACE("Multicast to WIFI1(EXT0)");
							if(_rtk_master_wlan_mbssid_tx(skb)==FAIL) return RG_FWDENGINE_RET_DROP;
						}
						else
							return RG_FWDENGINE_RET_DROP;
					}

					return RG_FWDENGINE_RET_CONTINUE; 
				}
				else
					return RG_FWDENGINE_RET_DROP;
#endif	
			}
		}
	
#endif
	return FAIL;

}

int rtk_rg_fwdEngineInput(struct re_private *cp, struct sk_buff *skb, void *pRxDescPtr)
{
	int i,ret,aclRet/*,naptOutIdx=FAIL,fragIdx=FAIL,totalQueueNum*/;
	rtk_rg_pktHdr_t pktHdr;
	rtk_rg_rxdesc_t	*pRxDesc=(rtk_rg_rxdesc_t *)pRxDescPtr;
	rtk_rg_napt_shortcut_t *pNaptSc;
	//rtk_rg_route_shortcut_t *pRoutingSc;
	
	_rtk_rg_extIngressPortDecision(&pktHdr,pRxDesc);
	*(u32*)(skb->data+skb->len)=(u32)&pktHdr; //save point of pktHdr into end of skb data.(for trace filter debug)
	
	pktHdr.pRxDesc=pRxDesc;	
	_rtk_rg_packetParser(skb->data,skb->len,&pktHdr);	
	
	TRACE("==================== PACKET BOUNDARY ====================\n        FWD_RX_I[%x]: DA=%02x:%02x:%02x:%02x:%02x:%02x SA=%02x:%02x:%02x:%02x:%02x:%02x ETH=%04x len=%d (EXT)SPA=%d EXTMASK=0x%x\n        Reason=%d IntPri=%d ctagVa=%d(VID=%d)",
		(u32)skb&0xffff,
		skb->data[0],skb->data[1],skb->data[2],skb->data[3],skb->data[4],skb->data[5],
		skb->data[6],skb->data[7],skb->data[8],skb->data[9],skb->data[10],skb->data[11],
		((skb->data[12]<<8)|skb->data[13])&0xffff,
		skb->len,
		pktHdr.ingressPort,
		pRxDesc->opts3.bit.dst_port_mask,
		pRxDesc->opts3.bit.reason,		
		pRxDesc->opts3.bit.internal_priority,
		pRxDesc->opts2.bit.ctagva,
		((pRxDesc->opts2.bit.cvlan_tag&0xf)<<0x8)+((pRxDesc->opts2.bit.cvlan_tag&0xff00)>>0x8));

	if(pktHdr.tagif&IPV4_TAGIF)
	{
		TRACE("        Ingress Src=%d.%d.%d.%d(%d) Dst=%d.%d.%d.%d(%d) %s %s%s%s%s%s%s"
			,(pktHdr.ipv4Sip>>24)&0xff,(pktHdr.ipv4Sip>>16)&0xff,(pktHdr.ipv4Sip>>8)&0xff,(pktHdr.ipv4Sip)&0xff,pktHdr.sport
			,(pktHdr.ipv4Dip>>24)&0xff,(pktHdr.ipv4Dip>>16)&0xff,(pktHdr.ipv4Dip>>8)&0xff,(pktHdr.ipv4Dip)&0xff,pktHdr.dport
			,(pktHdr.tagif&TCP_TAGIF)?"TCP":((pktHdr.tagif&UDP_TAGIF)?"UDP":"OTHER")
			,(pktHdr.tagif&TCP_TAGIF)?"Flags:":""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.syn?"SYN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.ack?"ACK ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.push?"PSH ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.fin?"FIN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.reset?"RST ":""):""
			);
	}


	//if(pRxDesc->opts3.bit.reason==192) dump_hs();

	if(rg_db.systemGlobal.hwnat_enable == RTN_PS)
		return RG_FWDENGINE_RET_TO_PS;

	//dump_hs();
	//dump_packet(skb->data,skb->len,"dump");



	/* Handle VLAN decision */
	assert_ok(_rtk_rg_internalVlanDecision(&pktHdr,&pktHdr.internalVlanID,skb));	

	/* WIFI DATA PATH Decision */
#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION
//#if 0
	ret=_rtk_rg_wlanExtraDataPathDecision(&pktHdr,pRxDesc,skb);
	if(ret==RG_FWDENGINE_RET_HWLOOKUP) goto hardware_lookup;
	else if(ret==RG_FWDENGINE_RET_CONTINUE) return ret;	
	else if(ret==RG_FWDENGINE_RET_TO_PS) return ret;	
	else if(ret==RG_FWDENGINE_RET_DROP) return ret;

#endif
	
#ifdef CONFIG_ROME_NAPT_SHORTCUT
	pktHdr.matchSC=0;
	if(pktHdr.tagif&(IPV4_TAGIF) && (pktHdr.pRxDesc->opts3.bit.reason!=192 /* SA learning limit */) /*&&(pktHdr.httpFirstPacket==0)*/)
	{
		for(i=0;i<MAX_NAPT_SHORTCUT_SIZE;i++)
		{
			pNaptSc=&rg_db.naptShortCut[i];
			//DEBUG("SC: %d %x %d %x %d\n",pNaptSc->isTcp,pNaptSc->sip,pNaptSc->sport,pNaptSc->dip,pNaptSc->dport);
			//DEBUG("Packet: %x %x %d %x %d\n",pktHdr.tagif,pktHdr.ipv4Sip,pktHdr.sport,pktHdr.ipv4Dip,pktHdr.dport);

			if((pktHdr.tagif&TCP_TAGIF) && ((pktHdr.tcpFlags.syn==1) || (pktHdr.tcpFlags.fin==1) || (pktHdr.tcpFlags.reset==1) || (pNaptSc->isTcp==0))) continue;
			if((pktHdr.tagif&UDP_TAGIF) && (pNaptSc->isTcp==1)) continue;
			if(pktHdr.sport!=pNaptSc->sport) continue;
			if(pktHdr.ipv4Sip!=pNaptSc->sip) continue;
			if(pktHdr.dport!=pNaptSc->dport) continue;
			if(pktHdr.ipv4Dip!=pNaptSc->dip) continue;

			if(pNaptSc->isNapt)
			{
				//NAPT shortcut
				if(pktHdr.ipv4FragPacket==0)
				{
					if(((pktHdr.tagif&TCP_TAGIF)&&(pktHdr.tcpFlags.fin==0)&&(pktHdr.tcpFlags.reset==0))||(pktHdr.tagif&UDP_TAGIF))
					{
						if(rg_db.lut[pNaptSc->new_lut_idx].valid==0) break;
						DEBUG("Hit NAPT shortcut[%d].\n",i);
						pktHdr.matchSC=1;
						pktHdr.dmacL2Idx=pNaptSc->new_lut_idx;
						pktHdr.netifIdx=pNaptSc->new_intf_idx;
						pktHdr.extipIdx=pNaptSc->new_eip_idx;
						ret = _rtk_rg_fwdEngine_shortCutNaptPacketModify(pNaptSc->direction,pNaptSc->naptIdx,&pktHdr,skb,1,1);
						assert_ok(ret);
						//dump_packet(skb->data,skb->len,"sc");
						goto direct_tx;	
					}
				}
			}
			else
			{
				if(rg_db.lut[pNaptSc->new_lut_idx].valid==0) break;
				//Routing shortcut
				DEBUG("Hit NAPT shortcut[%d].\n",i);
				pktHdr.matchSC=1;
				pktHdr.dmacL2Idx=pNaptSc->new_lut_idx;
				pktHdr.netifIdx=pNaptSc->new_intf_idx;
				ret = _rtk_rg_fwdEngine_shortCutNaptPacketModify(0,0,&pktHdr,skb,0,0);
				assert_ok(ret);
				//dump_packet(skb->data,skb->len,"sc");
				goto direct_tx; 			
			}
		}
	}
#endif

/*#ifdef CONFIG_RG_ICMP_FORWARDING
		//Let fwdEngine to learn and forward ICMP packet.
#else
		//Trap all ICMP packet to protocol stack
		if(pktHdr.tagif&ICMP_TAGIF)
			return RG_FWDENGINE_RET_TO_PS;
#endif*/
	if(pktHdr.tagif&ICMP_TAGIF && rg_db.systemGlobal.ctrlPathByProtocolStack_ICMP==1)
		return RG_FWDENGINE_RET_TO_PS;

	//ALE action
	pktHdr.sipL3Idx=FAIL;
	pktHdr.dipL3Idx=FAIL;	
	pktHdr.netifIdx=FAIL;
	pktHdr.extipIdx=FAIL;
	pktHdr.dipArpOrNBIdx=FAIL;
	pktHdr.nexthopIdx=FAIL;	
	pktHdr.dmacL2Idx=FAIL;
	pktHdr.bindNextHopIdx=FAIL;


	/*clear acl igr/egr hit info & action*/
	bzero(&(pktHdr.aclDecision),sizeof(rtk_rg_aclHitAndAction_t));

	/*Ingress ACL check*/
	assert_ok(_rtk_rg_ingressACLPatternCheck(&pktHdr));


	/* Check the DMAC */
	_rtk_rg_fwdEngineLIMDBC(&pktHdr);


//============================= L2 forward ============================
	ret=_rtk_rg_layer2Forward(cp,skb,&pktHdr);

	//Do ACL egress check
	assert_ok(_rtk_rg_egressACLPatternCheck(2,0,&pktHdr,skb,0,0));	//direct=2 means bridge mode		
	aclRet = _rtk_rg_egressACLAction(&pktHdr);
	if(aclRet == RG_FWDENGINE_RET_TO_PS || aclRet == RG_FWDENGINE_RET_DROP)
		return aclRet;

	if(ret==RG_FWDENGINE_RET_L2FORWARDED)
	{
		goto hardware_lookup;//Let hardware do lookup works (destination port, VLAN, DMAC2CVID)
		//goto direct_tx;	//Because CPU learning limit action set to trap now(for dual wifi), layer2 forward should use directTX, otherwise this packet will always fail.
	}
	else if(ret==RG_FWDENGINE_RET_BROADCAST)
	{
		//DEBUG("broadcast or flooding unknown DA packet");
#ifdef __KERNEL__
		//DEBUG("before broadcast forward!! internal vlan is %d",pPktHdr->internalVlanID);
		if(rg_db.systemGlobal.ctrlPathByProtocolStack_broadcast==1)
			return RG_FWDENGINE_RET_TO_PS;

		//DirectTX or To_ProtocolStack or DROP
		return _rtk_rg_broadcastForwardWithPkthdr(&pktHdr,skb,pktHdr.internalVlanID,pktHdr.pRxDesc->opts3.bit.src_port_num,pktHdr.ingressPort);
#endif
	}
	else if(ret!=RG_FWDENGINE_RET_CONTINUE)
	{
		//ret==RG_FWDENGINE_RET_TO_PS || RG_FWDENGINE_RET_DROP
		return ret;		
	}

	//DA == Gateway MAC, do Layer34 forward
//============================= L34 forward ============================
	if(pktHdr.httpFirstPacket==1)
	{
		if(_rtk_rg_urlFilter(skb->data,skb->len,&pktHdr)!=SUCCESS) return RG_FWDENGINE_RET_DROP;
	}

	switch(pktHdr.ipProtocol)
	{
		case 0x2f:		//47: GRE packet
			ret=_rtk_rg_PPTP_GREModify(skb,&pktHdr);
			if(ret==RG_FWDENGINE_RET_DIRECT_TX)
				goto direct_tx;
			else 
				return ret;
		default:
			break;
	}

//	if(pktHdr.tagif&IPV4_TAGIF)
//		rtlglue_printf("sip=%x sport=%d\n",pktHdr.ipv4Sip,pktHdr.sport);

//	memset(&rxinfo,0,sizeof(rtk_rg_rxdesc_t));
//	data = RG_GLB_FWDENGINE_initParam.nicRxCallBack(net_buf,&rxinfo);

	ret=_rtk_rg_layer34Forward(cp,skb,&pktHdr);

	switch(ret)
	{
		case RG_FWDENGINE_RET_QUEUE_FRAG:
		case RG_FWDENGINE_RET_DROP:
		case RG_FWDENGINE_RET_TO_PS:
			return ret;
		case RG_FWDENGINE_RET_DIRECT_TX:
			goto direct_tx;
		case RG_FWDENGINE_RET_FRAG_ONE_PS:		//Hit ACL action TRAP
			aclRet=RG_FWDENGINE_RET_TO_PS;
			goto fragment_send;
		case RG_FWDENGINE_RET_FRAG_ONE_DROP:	//Hit ACL action DROP
			aclRet=RG_FWDENGINE_RET_DROP;
			goto fragment_send;
		case RG_FWDENGINE_RET_FRAGMENT_ONE:
			//DEBUG(" Fragment First Packet send!");
			aclRet=RG_FWDENGINE_RET_DIRECT_TX;
		case RG_FWDENGINE_RET_FRAGMENT:			
			goto fragment_send;
		case FAIL: //not l34
			//if(rg_db.systemGlobal.hwnat_enable == ENABLE)
			//{
				//DEBUG("layer34Forward fail...return to PS");
				return RG_FWDENGINE_RET_TO_PS;
			/*}
			else
			{	
				//FIXME: to WLAN packet don't use Direct_TX (it will send to CPU again, and reason is srcPortFilter)
				for(i=0; i<rg_db.systemGlobal.lanIntfTotalNum; i++)
				{
					if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->ip_addr == pktHdr.ipv4Dip)
					{
						return RG_FWDENGINE_RET_TO_PS;
					}
				}
				
				for(i=0; i<rg_db.systemGlobal.wanIntfTotalNum;i++)
				{
					//Bridge WAN won't be compared with
					if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo==NULL)
						continue;
					
				   	if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ip_addr==pktHdr.ipv4Dip)
				   	{
						return RG_FWDENGINE_RET_TO_PS;
				   	}
				}
				//DEBUG("before direct tx");
				goto direct_tx;
			}*/
		default:
			break;
	}	
	return RG_FWDENGINE_RET_TO_PS;

hardware_lookup:
#ifdef __KERNEL__
	//clear old value
	rg_kernel.txDescMask.opts1.dw=0;
	rg_kernel.txDescMask.opts2.dw=0;
	rg_kernel.txDescMask.opts3.dw=0;
	rg_kernel.txDesc.opts1.dw=0;
	rg_kernel.txDesc.opts2.dw=0;
	rg_kernel.txDesc.opts3.dw=0;

	//turn on txInfo mask, otherwise value won't be add

	rg_kernel.txDescMask.opts1.bit.ipcs=1;
	rg_kernel.txDescMask.opts1.bit.l4cs=1;
	rg_kernel.txDescMask.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDescMask.opts1.bit.cputag_l4cs=1;
	rg_kernel.txDescMask.opts1.bit.dislrn=1;
	rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
	rg_kernel.txDescMask.opts2.bit.vidl=0xff;
	rg_kernel.txDescMask.opts2.bit.vidh=0xf;
	rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;	
	
	rg_kernel.txDesc.opts1.bit.ipcs=1;
	rg_kernel.txDesc.opts1.bit.l4cs=1;
	rg_kernel.txDesc.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDesc.opts1.bit.cputag_l4cs=1;
	rg_kernel.txDesc.opts1.bit.dislrn=1;		//disable HW to check and learn SA

	if(pktHdr.ingressPort==RTK_RG_EXT_PORT0)
	{
		//rg_kernel.txDesc.opts2.bit.vidl=1;
		//rg_kernel.txDesc.opts2.bit.tx_vlan_action=3;
		rg_kernel.txDescMask.opts3.bit.extspa=0x7;
		rg_kernel.txDesc.opts3.bit.extspa=0x1;
	}
	/*DEBUG("before hardware lookup");
	dump_packet(skb->data,skb->len,"hardward lookup");
	DEBUG("txDesc.opts1 is %x, txDesc.opts2 is %x,txDesc.opts3 is %x",
		rg_kernel.txDesc.opts1.dw,rg_kernel.txDesc.opts2.dw,rg_kernel.txDesc.opts3.dw);
	DEBUG("txDescMask.opts1 is %x, txDescMask.opts2 is %x,txDescMask.opts3 is %x",
		rg_kernel.txDescMask.opts1.dw,rg_kernel.txDescMask.opts2.dw,rg_kernel.txDescMask.opts3.dw);*/
	_rtk_rg_egressPacketSend(skb,&pktHdr);
	//re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);

	if(pktHdr.tagif&IPV4_TAGIF)
	{
		TRACE("        Egress Src=%d.%d.%d.%d(%d) Dst=%d.%d.%d.%d(%d) %s %s%s%s%s%s%s"
			,(pktHdr.ipv4Sip>>24)&0xff,(pktHdr.ipv4Sip>>16)&0xff,(pktHdr.ipv4Sip>>8)&0xff,(pktHdr.ipv4Sip)&0xff,pktHdr.sport
			,(pktHdr.ipv4Dip>>24)&0xff,(pktHdr.ipv4Dip>>16)&0xff,(pktHdr.ipv4Dip>>8)&0xff,(pktHdr.ipv4Dip)&0xff,pktHdr.dport
			,(pktHdr.tagif&TCP_TAGIF)?"TCP":((pktHdr.tagif&UDP_TAGIF)?"UDP":"OTHER")
			,(pktHdr.tagif&TCP_TAGIF)?"Flags:":""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.syn?"SYN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.ack?"ACK ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.push?"PSH ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.fin?"FIN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.reset?"RST ":""):""
			);
	}


	TRACE("FWD_TX_O[%x]: HWLOOKUP to portmask=0x%x extspa=%d vlanAct=%d vid=%d pri=%d",((u32)skb)&0xffff,
	rg_kernel.txDesc.opts3.bit.tx_portmask,
	rg_kernel.txDesc.opts3.bit.extspa,
	rg_kernel.txDesc.opts2.bit.tx_vlan_action,
	(rg_kernel.txDesc.opts2.bit.vidh<<8)|rg_kernel.txDesc.opts2.bit.vidl,
	rg_kernel.txDesc.opts2.bit.prio);		
	
#endif
	return RG_FWDENGINE_RET_HWLOOKUP;

direct_tx:
#ifdef __KERNEL__

	//clear old value
	rg_kernel.txDescMask.opts1.dw=0;
	rg_kernel.txDescMask.opts2.dw=0;
	rg_kernel.txDescMask.opts3.dw=0;
	rg_kernel.txDesc.opts1.dw=0;
	rg_kernel.txDesc.opts2.dw=0;
	rg_kernel.txDesc.opts3.dw=0;

	//turn on txInfo mask, otherwise value won't be add
	rg_kernel.txDescMask.opts1.bit.ipcs=1;
	rg_kernel.txDescMask.opts1.bit.l4cs=1;
	rg_kernel.txDescMask.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDescMask.opts1.bit.cputag_l4cs=1;
	rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
	rg_kernel.txDescMask.opts3.bit.l34_keep=1;

	rg_kernel.txDesc.opts1.bit.ipcs=1;
	rg_kernel.txDesc.opts1.bit.l4cs=1;
	rg_kernel.txDesc.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDesc.opts1.bit.cputag_l4cs=1;
	rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing
	rg_kernel.txDesc.opts3.bit.l34_keep=1;			//ensure switch won't modify or filter packet
	
	_rtk_rg_fwdEngineDirectTx(skb,&pktHdr);

	if(pktHdr.tagif&IPV4_TAGIF)
	{
		TRACE("        Egress Src=%d.%d.%d.%d(%d) Dst=%d.%d.%d.%d(%d) %s %s%s%s%s%s%s"
			,(pktHdr.ipv4Sip>>24)&0xff,(pktHdr.ipv4Sip>>16)&0xff,(pktHdr.ipv4Sip>>8)&0xff,(pktHdr.ipv4Sip)&0xff,pktHdr.sport
			,(pktHdr.ipv4Dip>>24)&0xff,(pktHdr.ipv4Dip>>16)&0xff,(pktHdr.ipv4Dip>>8)&0xff,(pktHdr.ipv4Dip)&0xff,pktHdr.dport
			,(pktHdr.tagif&TCP_TAGIF)?"TCP":((pktHdr.tagif&UDP_TAGIF)?"UDP":"OTHER")
			,(pktHdr.tagif&TCP_TAGIF)?"Flags:":""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.syn?"SYN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.ack?"ACK ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.push?"PSH ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.fin?"FIN ":""):""
			,(pktHdr.tagif&TCP_TAGIF)?(pktHdr.tcpFlags.reset?"RST ":""):""
			);
	}

	
	TRACE("FWD_TX_O[%x]: DIRECT TX to portmask=0x%x extspa=%d vlanAct=%d vid=%d pri=%d",((u32)skb)&0xffff,
	rg_kernel.txDesc.opts3.bit.tx_portmask,
	rg_kernel.txDesc.opts3.bit.extspa,
	rg_kernel.txDesc.opts2.bit.tx_vlan_action,
	(rg_kernel.txDesc.opts2.bit.vidh<<8)|rg_kernel.txDesc.opts2.bit.vidl,
	rg_kernel.txDesc.opts2.bit.prio);

#endif
//	DEBUG("");

	return RG_FWDENGINE_RET_DIRECT_TX;
#if 0
dmac2cvid_tx:
#ifdef __KERNEL__
	//turn on txInfo mask, otherwise value won't be add
	rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
	rg_kernel.txDescMask.opts1.bit.ipcs=1;
	rg_kernel.txDescMask.opts1.bit.l4cs=1;
	rg_kernel.txDesc.opts1.bit.ipcs=1;
	rg_kernel.txDesc.opts1.bit.l4cs=1;
	rg_kernel.txDesc.opts2.bit.tx_vlan_action = 0x2;		//removing

	ret=_rtk_rg_fwdEngineDMAC2CVIDLookup(&pktHdr,LAN_FID,0);		//FIXME:we use only one FID right now
	if(ret!=RT_ERR_RG_OK)
		return RG_FWDENGINE_RET_TO_PS;

	re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);

#endif
//	DEBUG("");
	return RG_FWDENGINE_RET_DIRECT_TX;
#endif

fragment_send:
	if(ret==RG_FWDENGINE_RET_FRAGMENT || ret==RG_FWDENGINE_RET_FRAGMENT_ONE)
	{
#ifdef __KERNEL__
		//clear old value
		rg_kernel.txDescMask.opts1.dw=0;
		rg_kernel.txDescMask.opts2.dw=0;
		rg_kernel.txDescMask.opts3.dw=0;
		rg_kernel.txDesc.opts1.dw=0;
		rg_kernel.txDesc.opts2.dw=0;
		rg_kernel.txDesc.opts3.dw=0;
		
		//turn on txInfo mask, otherwise value won't be add
		rg_kernel.txDescMask.opts1.bit.ipcs=1;
		rg_kernel.txDescMask.opts1.bit.l4cs=1;
		rg_kernel.txDescMask.opts1.bit.cputag_ipcs=1;
		rg_kernel.txDescMask.opts1.bit.cputag_l4cs=1;
		rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
		rg_kernel.txDescMask.opts3.bit.l34_keep=1;
		
		rg_kernel.txDesc.opts1.bit.ipcs=1;
		rg_kernel.txDesc.opts1.bit.l4cs=0;		//these queue fragment packets are IP-packet
		rg_kernel.txDesc.opts1.bit.cputag_ipcs=1;
		rg_kernel.txDesc.opts1.bit.cputag_l4cs=0;
		rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing
		rg_kernel.txDesc.opts3.bit.l34_keep=1;		//6266 bug: L4CS will always offload, fragment have to use L34keep to disable both IP and L4 cs offload

//#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	//_rtk_rg_fwdEngineDestPortLookup(&pktHdr);
//#endif
		_rtk_rg_fwdEngineDirectTx(skb,&pktHdr);
		TRACE("FWD_TX_O[%x]: DIRECT TX to portmask=0x%x extspa=%d vlanAct=%d vid=%d pri=%d",((u32)skb)&0xffff,
			rg_kernel.txDesc.opts3.bit.tx_portmask,
			rg_kernel.txDesc.opts3.bit.extspa,
			rg_kernel.txDesc.opts2.bit.tx_vlan_action,
			(rg_kernel.txDesc.opts2.bit.vidh<<8)|rg_kernel.txDesc.opts2.bit.vidl,
			rg_kernel.txDesc.opts2.bit.prio);

	//re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
#endif	

		if(ret==RG_FWDENGINE_RET_FRAGMENT)		//just stop here
			return RG_FWDENGINE_RET_DIRECT_TX;
	}

	//Fragment_one, one_ps, one_drop will reach here

	//send or drop or trap these queued packet which idendification is same to pktHdr
	_rtk_rg_fwdEngine_fragmentQueueProcessing(aclRet,&pktHdr);
	
	return aclRet;
}




int fwdEngine_rx_skb (struct re_private *cp, struct sk_buff *skb,struct rx_info *pRxInfo)
{
	int rg_fwdengine_ret_code=0;

	if(pRxInfo==NULL) pRxInfo=&rg_kernel.rxInfoFromWLAN;

	rg_fwdengine_ret_code = rtk_rg_fwdEngineInput(cp,skb,(void*)pRxInfo);
	
	//Processing packets
	if(rg_fwdengine_ret_code == RG_FWDENGINE_RET_TO_PS)
	{
		TRACE("FWD_RX_O[%x]: To Protocol-Stack",(u32)skb&0xffff);
		return RE8670_RX_CONTINUE;
	}
	else if(rg_fwdengine_ret_code == RG_FWDENGINE_RET_DROP)
	{
		TRACE("FWD_RX_O[%x]: Drop",(u32)skb&0xffff);
		//printk("drop skb=%x skb->data=%x\n",(u32)skb,(u32)skb->data);
		return RE8670_RX_STOP;			//let NIC driver free the SKB for us
	}
	else
	{
		//TRACE("FWD_RX_O[%x]: Forward",(u32)skb&0xffff);
		return RE8670_RX_STOP_SKBNOFREE;		//the SKB had been sended or queued, kfree is no need
	}
}

void _rtk_rg_switchLinkChangeHandler(void)
{
	int i,ret;
	rtk_portmask_t linkDownPort;
	DEBUG("%s: get link-change event",__FUNCTION__);
	//Check and clear link-down indicator register

	ret=rtk_intr_linkdownStatus_get(&linkDownPort);
	if(ret!=RT_ERR_OK)goto CLR_INTR;
	ret=rtk_intr_linkdownStatus_clear();
	if(ret!=RT_ERR_OK)goto CLR_INTR;

	//Sync per-port LUT entry between hardware and software table
	DEBUG("the link-down portmask is %x",linkDownPort.bits[0]);
	if(linkDownPort.bits[0]==0x0)goto CLR_INTR;
	for(i=0;i<MAX_LUT_SW_TABLE_SIZE;i++)
	{
		if(rg_db.lut[i].valid && 
			rg_db.lut[i].rtk_lut.entryType==RTK_LUT_L2UC &&
			(rg_db.lut[i].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)==0 &&			//FIXME: this condition may not work if hw setting change
			(rg_db.lut[i].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0 &&		//FIXME: this condition may not work if hw setting change
			(linkDownPort.bits[0]&(0x1<<rg_db.lut[i].rtk_lut.entry.l2UcEntry.port)))
		{
			ret=rtk_l2_addr_get(&rg_db.lut[i].rtk_lut.entry.l2UcEntry);
			if(ret==RT_ERR_L2_ENTRY_NOTFOUND || rg_db.lut[i].rtk_lut.entry.l2UcEntry.age==0)
			{
				//Clear software entry since it's hardware homogeneity is gone
				//------------------ Critical Section start -----------------------//
				//rg_lock(&rg_kernel.saLearningLimitLock);
				if(rg_db.lut[i].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
					atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[i].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
				else
					atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[i].rtk_lut.entry.l2UcEntry.port]);
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
			
				rg_db.lut[i].valid=0;
			}
		}
	}
CLR_INTR:
	DEBUG("before clear the ISR state");
	//Clear ISR state
	rtk_intr_ims_clear(INTR_TYPE_LINK_CHANGE);
}
#endif

void _rtk_rg_alg_functions_registration(void)
{
	int i;
	//When the function is set to NULL, if the function is turn on, the packets will trap to protocol stack directly
	for(i=0;i<MAX_ALG_FUNCTIONS;i++)
	{
		rg_db.algTcpFunctionMapping[i].portNum=0;
		rg_db.algTcpFunctionMapping[i].registerFunction=NULL;
		rg_db.algUdpFunctionMapping[i].portNum=0;
		rg_db.algUdpFunctionMapping[i].registerFunction=NULL;
	}
	
	//TCP port
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_SIP_TCP].portNum=RTK_RG_ALG_SIP_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_H323_TCP].portNum=RTK_RG_ALG_H323_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_RTSP_TCP].portNum=RTK_RG_ALG_RTSP_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_FTP_TCP].portNum=RTK_RG_ALG_FTP_TCP_PORT;

	//TCP function
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_SIP_TCP].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_H323_TCP].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_RTSP_TCP].registerFunction=NULL;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_FTP_TCP].registerFunction=_rtk_rg_algRegFunc_TCP_FTP;
	
	//UDP port
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_SIP_UDP].portNum=RTK_RG_ALG_SIP_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_H323_UDP].portNum=RTK_RG_ALG_H323_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_RTSP_UDP].portNum=RTK_RG_ALG_RTSP_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_FTP_UDP].portNum=RTK_RG_ALG_FTP_UDP_PORT;
	
	//UDP function
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_SIP_UDP].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_H323_UDP].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_RTSP_UDP].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_FTP_UDP].registerFunction=NULL;

	//TCP port Server in Lan
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_SIP_TCP_SRV_IN_LAN].portNum=RTK_RG_ALG_SIP_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_H323_TCP_SRV_IN_LAN].portNum=RTK_RG_ALG_H323_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN].portNum=RTK_RG_ALG_RTSP_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_FTP_TCP_SRV_IN_LAN].portNum=RTK_RG_ALG_FTP_TCP_PORT;

	//TCP function Server in Lan
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_SIP_TCP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_H323_TCP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_FTP_TCP_SRV_IN_LAN].registerFunction=NULL;//_rtk_rg_algRegFunc_TCP_FTP;

	//UDP port Server in Lan
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_SIP_UDP_SRV_IN_LAN].portNum=RTK_RG_ALG_SIP_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_H323_UDP_SRV_IN_LAN].portNum=RTK_RG_ALG_H323_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN].portNum=RTK_RG_ALG_RTSP_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_FTP_UDP_SRV_IN_LAN].portNum=RTK_RG_ALG_FTP_UDP_PORT;

	//UDP function Server in Lan
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_SIP_UDP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_H323_UDP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_FTP_UDP_SRV_IN_LAN].registerFunction=NULL;

	//Pass through TCP port setting
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_PPTP_TCP_PASSTHROUGH].portNum=RTK_RG_ALG_PPTP_PASSTHROUGH_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_L2TP_TCP_PASSTHROUGH].portNum=RTK_RG_ALG_L2TP_PASSTHROUGH_TCP_PORT;
	rg_db.algTcpFunctionMapping[RTK_RG_ALG_IPSEC_TCP_PASSTHROUGH].portNum=RTK_RG_ALG_IPSEC_PASSTHROUGH_TCP_PORT;

	//Pass through TCP function setting
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_PPTP_TCP_PASSTHROUGH].registerFunction=_rtk_rg_algRegFunc_TCP_PPTP;		//GRE_Modify is not ready
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_L2TP_TCP_PASSTHROUGH].registerFunction=NULL;
	//rg_db.algTcpFunctionMapping[RTK_RG_ALG_IPSEC_TCP_PASSTHROUGH].registerFunction=NULL;

	//Pass through UDP port setting
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_PPTP_UDP_PASSTHROUGH].portNum=RTK_RG_ALG_PPTP_PASSTHROUGH_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_L2TP_UDP_PASSTHROUGH].portNum=RTK_RG_ALG_L2TP_PASSTHROUGH_UDP_PORT;
	rg_db.algUdpFunctionMapping[RTK_RG_ALG_IPSEC_UDP_PASSTHROUGH].portNum=RTK_RG_ALG_IPSEC_PASSTHROUGH_UDP_PORT;

	//Pass through UDP function setting
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_PPTP_UDP_PASSTHROUGH].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_L2TP_UDP_PASSTHROUGH].registerFunction=NULL;
	//rg_db.algUdpFunctionMapping[RTK_RG_ALG_IPSEC_UDP_PASSTHROUGH].registerFunction=NULL;
}

int _rtk_rg_fwdEngineGlobalVariableReset(void)
{
	int i,j;
	
	//initialize the IPv4 fragment table and queue
	rg_db.systemGlobal.ipv4FragmentQueueNum = 0;
	for(i=0;i<MAX_NAPT_OUT_SW_TABLE_SIZE;i++)
	{
		if(i<MAX_IPV4_FRAGMENT_QUEUE_SIZE)
		{
			bzero(&rg_db.ipv4FragmentQueue[i],sizeof(rtk_rg_fragment_queue_t));
			rg_db.ipv4FragmentQueue[i].queue_skb=NULL;
		}
		//bzero(&rg_db.ipv4FragmentOutTable[i],sizeof(rtk_rg_fragment_napt_out_t));
		//rg_db.ipv4FragmentOutTable[i].pNaptOutboundEntry=NULL;
	}

	//init napt Out free link list	
	rg_db.pNaptOutFreeListHead=&rg_db.naptOutFreeList[0];	
	
	for(i=MAX_NAPT_OUT_HW_TABLE_SIZE;i<MAX_NAPT_OUT_SW_TABLE_SIZE;i++)
	{
		rg_db.naptOutFreeList[i-MAX_NAPT_OUT_HW_TABLE_SIZE].idx=i;
		if(i+1<MAX_NAPT_OUT_SW_TABLE_SIZE)
			rg_db.naptOutFreeList[i-MAX_NAPT_OUT_HW_TABLE_SIZE].pNext=&rg_db.naptOutFreeList[i-MAX_NAPT_OUT_HW_TABLE_SIZE+1];
		else
			rg_db.naptOutFreeList[i-MAX_NAPT_OUT_HW_TABLE_SIZE].pNext=NULL;
	}

	//init napt out hash index link list
	for(i=0;i<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;i++)
		rg_db.pNaptOutHashListHead[i]=NULL;

	//init napt In free link list	
	rg_db.pNaptInFreeListHead=&rg_db.naptInFreeList[0];	

	for(i=MAX_NAPT_IN_HW_TABLE_SIZE;i<MAX_NAPT_IN_SW_TABLE_SIZE;i++)
	{
		rg_db.naptInFreeList[i-MAX_NAPT_IN_HW_TABLE_SIZE].idx=i;
		if(i+1<MAX_NAPT_IN_SW_TABLE_SIZE)
			rg_db.naptInFreeList[i-MAX_NAPT_IN_HW_TABLE_SIZE].pNext=&rg_db.naptInFreeList[i-MAX_NAPT_IN_HW_TABLE_SIZE+1];
		else
			rg_db.naptInFreeList[i-MAX_NAPT_IN_HW_TABLE_SIZE].pNext=NULL;
	}

	//init napt In hash index link list
	for(i=0;i<MAX_NAPT_IN_HW_TABLE_SIZE>>2;i++)
		rg_db.pNaptInHashListHead[i]=NULL;

	//init fragment Out free link list	
	rg_db.pFragOutFreeListHead=&rg_db.fragOutFreeList[0];	
	
	for(i=0;i<MAX_FRAG_OUT_FREE_TABLE_SIZE;i++)
	{
		memset(&rg_db.fragOutFreeList[i],0,sizeof(rtk_rg_ipv4_fragment_out_t));
		if(i+1<MAX_FRAG_OUT_FREE_TABLE_SIZE)
			rg_db.fragOutFreeList[i].pNext=&rg_db.fragOutFreeList[i+1];
		else
			rg_db.fragOutFreeList[i].pNext=NULL;
	}

	//init fragment out hash index link list
	for(i=0;i<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;i++)
		rg_db.pFragOutHashListHead[i]=NULL;

	//init fragment In free link list	
	rg_db.pFragInFreeListHead=&rg_db.fragInFreeList[0];	

	for(i=0;i<MAX_FRAG_IN_FREE_TABLE_SIZE;i++)
	{
		memset(&rg_db.fragInFreeList[i],0,sizeof(rtk_rg_ipv4_fragment_in_t));
		if(i+1<MAX_FRAG_IN_FREE_TABLE_SIZE)
			rg_db.fragInFreeList[i].pNext=&rg_db.fragInFreeList[i+1];
		else
			rg_db.fragInFreeList[i].pNext=NULL;
	}

	//init fragment In hash index link list
	for(i=0;i<MAX_NAPT_IN_HW_TABLE_SIZE>>2;i++)
		rg_db.pFragInHashListHead[i]=NULL;

	//init ICMP control flow head
	rg_db.pICMPCtrlFlowHead=&rg_db.icmpCtrlFlowLinkList[0];

	//init ICMP control flow free link list
	for(i=0;i<MAX_ICMPCTRLFLOW_SIZE;i++)
	{
		if(i==MAX_ICMPCTRLFLOW_SIZE-1)
			rg_db.icmpCtrlFlowLinkList[i].pNext=&rg_db.icmpCtrlFlowLinkList[0];		//ring-buffer
		else
			rg_db.icmpCtrlFlowLinkList[i].pNext=&rg_db.icmpCtrlFlowLinkList[i+1];

		if(i==0)
			rg_db.icmpCtrlFlowLinkList[i].pPrev=&rg_db.icmpCtrlFlowLinkList[MAX_ICMPCTRLFLOW_SIZE-1];
		else
			rg_db.icmpCtrlFlowLinkList[i].pPrev=&rg_db.icmpCtrlFlowLinkList[i-1];
	}

	//init Alg port-function mapping
	_rtk_rg_alg_functions_registration();

	//init FTP control flow head
	rg_db.pAlgFTPCtrlFlowHead=&rg_db.algFTPCtrlFlowList[0]; 

	//init FTP control flow free link list	
	for(i=0;i<MAX_FTP_CTRL_FLOW_SIZE;i++)
	{
		if(i==MAX_FTP_CTRL_FLOW_SIZE-1)
			rg_db.algFTPCtrlFlowList[i].pNext=&rg_db.algFTPCtrlFlowList[0];		//ring-buffer
		else
			rg_db.algFTPCtrlFlowList[i].pNext=&rg_db.algFTPCtrlFlowList[i+1];

		if(i==0)
			rg_db.algFTPCtrlFlowList[i].pPrev=&rg_db.algFTPCtrlFlowList[MAX_FTP_CTRL_FLOW_SIZE-1];
		else
			rg_db.algFTPCtrlFlowList[i].pPrev=&rg_db.algFTPCtrlFlowList[i-1];
	}

	//init PPTP link list
	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		rg_db.pPPTPGreOutboundHead[i]=&rg_db.pptpGreOutboundLinkList[i][0];
		
		for(j=0;j<MAX_PPTP_SESSION_SIZE;j++)
		{
			if(j==MAX_PPTP_SESSION_SIZE-1)
				rg_db.pptpGreOutboundLinkList[i][j].pNext=&rg_db.pptpGreOutboundLinkList[i][0];		//ring-buffer
			else
				rg_db.pptpGreOutboundLinkList[i][j].pNext=&rg_db.pptpGreOutboundLinkList[i][j+1];

			if(j==0)
				rg_db.pptpGreOutboundLinkList[i][j].pPrev=&rg_db.pptpGreOutboundLinkList[i][MAX_PPTP_SESSION_SIZE-1];
			else
				rg_db.pptpGreOutboundLinkList[i][j].pPrev=&rg_db.pptpGreOutboundLinkList[i][j-1];
		}
	}
	
	return RT_ERR_RG_OK;
}


/* This function is called by romeDriver & test case */
int rtk_rg_rome_driver_init(void)
{
	int err;
	
#ifdef __KERNEL__	
	struct proc_dir_entry *p;
#endif

	_rtk_rg_fwdEngineGlobalVariableReset();

#ifdef CONFIG_RTL_IGMP_SNOOPING
#if 0 //ysleu:Move to rtk_rg_initParam_set()
	//igmp init	
	for(i=0;i<RTK_RG_MAC_PORT_CPU;i++)
	{
		if(i==RTK_RG_MAC_PORT_PON) continue;
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV1,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV2,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV3,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_MLDV1,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_MLDV2,ACTION_TRAP2CPU),RT_ERR_OK);
	}
#endif
	{		
		struct rtl_mCastSnoopingGlobalConfig mCastSnoopingGlobalConfig;
		rtl_multicastDeviceInfo_t devInfo;
		
		/* init igmp snooping module */ 	
		memset(&mCastSnoopingGlobalConfig, 0, sizeof(struct rtl_mCastSnoopingGlobalConfig));
		mCastSnoopingGlobalConfig.maxGroupNum=256;
		mCastSnoopingGlobalConfig.maxSourceNum=300;
		mCastSnoopingGlobalConfig.hashTableSize=64;
		mCastSnoopingGlobalConfig.groupMemberAgingTime=260;
		mCastSnoopingGlobalConfig.lastMemberAgingTime=2;
		mCastSnoopingGlobalConfig.querierPresentInterval=260;  
		mCastSnoopingGlobalConfig.dvmrpRouterAgingTime=120;
		mCastSnoopingGlobalConfig.mospfRouterAgingTime=120;
		mCastSnoopingGlobalConfig.pimRouterAgingTime=120;	
		ASSERT_EQ(rtl_initMulticastSnooping(mCastSnoopingGlobalConfig),SUCCESS);
		
		/* register igmp snooping module */
		ASSERT_EQ(rtl_registerIgmpSnoopingModule(&rg_db.systemGlobal.nicIgmpModuleIndex),SUCCESS);		
		memset(&devInfo, 0 , sizeof(rtl_multicastDeviceInfo_t));
		strcpy(devInfo.devName, RG_IGMP_SNOOPING_MODULE_NAME);
		devInfo.portMask	= (1<<RTK_RG_MAC_PORT_MAX)-1;
		devInfo.swPortMask	= 0;		
		ASSERT_EQ(rtl_setIgmpSnoopingModuleDevInfo(rg_db.systemGlobal.nicIgmpModuleIndex, &devInfo),SUCCESS);	
	}
#endif


#ifdef __KERNEL__	
	//init IPv4 fragment lock
	/*sema_init(&rg_kernel.ipv4FragLock, 1);
	sema_init(&rg_kernel.ipv4FragFreeLock, 1);
	sema_init(&rg_kernel.ipv4FragQueueLock, 1);*/
	init_MUTEX(&rg_kernel.ipv4FragLock);
	init_MUTEX(&rg_kernel.ipv4FragFreeLock);
	init_MUTEX(&rg_kernel.ipv4FragQueueLock);
	//init IGMP snooping lock
	//sema_init(&rg_kernel.igmpsnoopingLock, 1);

#ifdef CONFIG_RTL8686NIC
/*
	// init DirectTx Descs
	memset(&rg_kernel.txDesc,0,sizeof(struct tx_info));
	memset(&rg_kernel.txDescMask,0,sizeof(struct tx_info));
	rg_kernel.txDescMask.opts2.bit.cputag=1;
	rg_kernel.txDesc.opts2.bit.cputag=1;

	rg_kernel.txDescMask.opts3.bit.l34_keep=1;
	rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;
#ifdef CONFIG_APOLLO_RLE0371
	//patch bug for: ARP tx data error, when directTX	
	rg_kernel.txDesc.opts3.bit.l34_keep=1;
	rg_kernel.txDesc.opts3.bit.tx_portmask=0;
#else
	rg_kernel.txDesc.opts3.bit.l34_keep=0;
	rg_kernel.txDesc.opts3.bit.tx_portmask=0;		//hardware auto look up
	//rg_kernel.txDesc.opts3.bit.tx_portmask=(1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT_RGMII);
#endif
	rg_kernel.txDescMask.opts1.bit.ipcs=1;
	rg_kernel.txDescMask.opts1.bit.l4cs=1;	
	rg_kernel.txDescMask.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDescMask.opts1.bit.cputag_l4cs=1;
	rg_kernel.txDesc.opts1.bit.ipcs=1;
	rg_kernel.txDesc.opts1.bit.l4cs=1;
	rg_kernel.txDesc.opts1.bit.cputag_ipcs=1;
	rg_kernel.txDesc.opts1.bit.cputag_l4cs=1;
*/
	/*register NIC rx handler*/
	err = drv_nic_register_rxhook(0x7f,RE8686_RXPRI_RG,fwdEngine_rx_skb);
	if(err < 0)
		return err;
#endif

	if(rg_kernel.proc_rg==NULL)
		rg_kernel.proc_rg = proc_mkdir("rg", NULL); 

#ifdef CONFIG_RTL_IGMP_SNOOPING
	p = create_proc_entry("igmpSnooping", 0644, rg_kernel.proc_rg);
	if (p){
		p->read_proc = (void *)igmp_show;
	}else{
		printk("create proc rg/igmpSnooping failed!\n");
	}
#endif
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	p = create_proc_entry("saLearningCount", 0644, rg_kernel.proc_rg);
	if (p){
		p->read_proc = (void *)_rtk_rg_layer2LutLearningCountShow;
	}else{
		printk("create proc rg/saLearningCount failed!\n");
	}
#endif
#endif
#if defined(CONFIG_DUALBAND_CONCURRENT)
	_rtk_rg_str2mac(CONFIG_DEFAULT_MASTER_IPC_MAC_ADDRESS,&master_ipc_macAddr);
#endif

	return SUCCESS;
}

#ifdef __KERNEL__
/*cp is the NIC private data*/

#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK 
extern int re8670_start_xmit (struct sk_buff *skb, struct net_device *dev);
int rtk_rg_fwdEngine_xmit (struct sk_buff *skb, struct net_device *dev)
{
	char *startCP,*endCP;
	unsigned char interfaceIdx;
	unsigned int internalVlanID;
	int ret;

	//Handle broadcast or multicast packet
	if(rg_db.systemGlobal.ctrlPathByProtocolStack_broadcast==1)
		goto BYPASS;

	if(((*(unsigned short *)skb->data)&(*(unsigned short *)(skb->data+1))&(*(unsigned short *)(skb->data+2)))==0xffff ||	//broadcast
		(skb->data[0]==0x01&&skb->data[1]==0x00&&skb->data[2]==0x5e) ||		//ipv4 multicast
		(*(unsigned short *)skb->data==0x3333))	 //ipv6 multicast
	{
		//Check interface
		startCP=dev->name+3;		//bypass "eth" characters
		interfaceIdx = simple_strtoul(startCP,&endCP,0);
		//Get VLAN setting for member set and untag set
		internalVlanID=rg_db.netif[interfaceIdx].rtk_netif.vlan_id;
		//Allocate skb for tagged and untagged set, if needed
		//DEBUG("dev name is %s, interface index is %d,vlanID is %d",dev->name,interfaceIdx,internalVlanID);
		//dump_packet(skb->data,skb->len,"SPS xmit packet");
		ret=_rtk_rg_broadcastForward(skb,internalVlanID,RTK_RG_MAC_PORT_CPU,0);	
		if(ret==RG_FWDENGINE_RET_DROP)
			dev_kfree_skb_any(skb);

		return 0;
	}
	else	//unicast packet transmit by hardware lookup
	{
		//DEBUG("unicast packet, just forward and let hardware do the job");
		/*if(skb->data[0]==0x68 &&
			skb->data[1]==0x05&&
			skb->data[2]==0xCA&&
			skb->data[3]==0x0F&&
			skb->data[4]==0x8B&&
			skb->data[5]==0x73)
			dump_packet(skb->data,skb->len,"send to server!!!");*/
BYPASS:
		return re8670_start_xmit(skb,dev);
	}
}
#endif

int __init rtk_rg_rome_driver_module_init(void)
{
	rtk_rg_rome_driver_init();
	return SUCCESS;
}


void __exit rtk_rg_rome_driver_module_exit(void)
{
	int i;
	//rtlglue_printf("%s\n",__func__);

	//Free all queue packet if any
	for(i=0;i<MAX_IPV4_FRAGMENT_QUEUE_SIZE;i++)
	{
		dev_kfree_skb_any(rg_db.ipv4FragmentQueue[i].queue_skb);
	}

	/*unregister NIC rx handler*/
	drv_nic_unregister_rxhook(0x7f,RE8686_RXPRI_RG,fwdEngine_rx_skb);
}

module_init(rtk_rg_rome_driver_module_init);
module_exit(rtk_rg_rome_driver_module_exit);
#endif
#endif //CONFIG_APOLLO_ROMEDRIVER


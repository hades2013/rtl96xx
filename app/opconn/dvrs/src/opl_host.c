/*
 * ===============================================================================
 * Source Name: opl_host.c
 *
 * General Description:
 *     This file mananges Opconn host interface driver related things, incl.
 * host sending and receiving, processing, counter get, etc.
 *
 * ===============================================================================
 * $Copyright: (c) 2007 OPulan Corp.
 * All Rights Reserved.$
 * ===============================================================================
 *
 * Revision History:
 *
 * Author                Date              Description of Changes
 * ----------------   ------------  ----------------------------------------------
 * rzhou              12/10/2007    initial
 * ----------------   ------------  ----------------------------------------------
 *
 * ===============================================================================
 */
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//#include <dal_lib.h>

#include "opl_driver.h"
#include "opl_host.h"
#include "opl_dma.h"
#include "opl_ethernet.h"
#include "multicast_control.h"

#include "opl_debug.h"
#include "rstp_in.h"
#include "rstp_bpdu.h"
#include "vos.h"

#include "lw_drv_pub.h"



hostCntS hostCnt = {0,0,0,0,0,0};

#define ETH_SLOW_PROTOCOL_TYPE_OAM	0x03	/* Slow Protocol subtype OAM */

UINT8 customerTpid[2]  ={0x81,0x00};
UINT8 hostDumpDataEn = 0;
#if defined(ONU_4PORT_AR8327)
static UINT8 atheros_4bytes_header_flag = 0;
static UINT32 atheros_portNum;
char  atherosTpid[2] = {0x91, 0x00};
#endif

INT16 eopl_socket_send(unsigned char* packet, UINT16 length);
INT16 eopl_l2_send(unsigned char* packet, UINT16 length);
extern DRV_RET_E l2_send_by_port(UCHAR *pMsg, UINT uiLen,l2_send_op *op);

#define DRV_OK  0


/* Host debug macro switch */
#if 1
#define OPCONN_HOST_DEBUG
#endif

#ifdef OPCONN_HOST_DEBUG

#define OPL_HOST_DEBUG(format, ...) \
do {\
	if((gbHostDbg)&&(gbLogDbg)) fprintf(stderr,  format, ##__VA_ARGS__ ); }while(0)

#else
	#define OPL_HOST_DEBUG(format, ...)

#endif

/* Dump packet content function */
void eopl_dump(const void *pkt, UINT16 len, const UINT8 *pCaption);

#define OPL_HOST_DUMP(addr, len) 	eopl_dump(addr, len, " Host Interface:")

/* Host Send Routine Mutex*/
VOS_MUTEX_t gHostSendMutex = PTHREAD_MUTEX_INITIALIZER;

/* Host Tx and Tx packet dump switch */
static INT32 host_dump_enable = OPL_TRUE;

extern UINT8 oam_dest_mac[6];

/* begin: add by jiangmingli for RSTP process */
UINT8 aucBpduMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
UINT8 aucIgmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};

BPDU_T g_stRcvBpdu;

int rstpdbg = 0;

int rstptrace = 0;


void showCntHost(void)
{

	printf("dmaRx  : %d\n", hostCnt.dmaRx);
	printf("dmaTx  : %d\n", hostCnt.dmaTx);
	printf("dmaTxF : %d\n", hostCnt.dmaTxFail);
	printf("OamRx  : %d\n", hostCnt.oamRx);
	printf("OamTx  : %d\n", hostCnt.oamTx);
	printf("OamTxF : %d\n", hostCnt.oamTxFail);


}


void ClearCntHost(void)
{
hostCnt.dmaRx = 0;
hostCnt.dmaTx = 0;
hostCnt.dmaTxFail = 0;
hostCnt.oamRx = 0;
hostCnt.oamTx = 0;
hostCnt.oamTxFail = 0;
}
	
void odmStpRcvBpdu(unsigned char ucPortid, unsigned short usVlanid, void *pkt, unsigned short len)
{
#ifndef ONU_1PORT
	DSA_TAG_FORMAT_6046_S *pstDsaTag = NULL;
       #ifndef ONU_4PORT_AR8327
	/* BPDU + CRC, 4 for CRC data */
	if (len > sizeof(BPDU_T))
	{
		opl_dump_data(pkt, (len + 1), 16);
		//OPL_HOST_DUMP(pkt, (len + 1));
		return;
	}
       #endif
	OPL_MEMCPY(&(g_stRcvBpdu.eth), (char *)pkt, len);

	opl_dump_data((unsigned char *)&g_stRcvBpdu, sizeof(BPDU_T), 16);

	/* only one rstp instance, must set vlanid equals 0 */
	usVlanid = 0;

	/* rstp receive machine process */
	STP_IN_rx_bpdu(usVlanid, ucPortid, &g_stRcvBpdu, len);
#else
    printf("odmStpRcvBpdu  \r\n");
#if 0
    if (len > MAX_PACKET_SIZE - (RTL_HEAD_OFF + 2))
    {
        return;
    }
    PrintH3cPacket(RXTX_ACT_RCV_STP, pkt, len);
#endif   
    	#if 0
	(void)RSTP_PutMbox(pkt, len, (int)usVlanid, (UINT32)ucPortid);
        #endif
#endif

	return;
}
/* end: add by jiangmingli for RSTP process */

/*
* eopl_pkt_process
*
* DESCRIPTION:
*   This function processes the received packets.
*
* INPUT: pkt, len
*   pkt    - packet pointer
*   len    - length of packet
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
#if 1
void eopl_pkt_process(void * pkt, UINT16 len)
{
	eth_header_none_tag_t *ptr_eth_oam_hdr;
	eth_header_with_sgl_tag_t *ptr_eth_hdr;
	ipv4_header_t *ptr_ipv4_hdr;
	UINT8 type = 0x0;
	host_inbound_hdr_t *pstInHeader = NULL;
	UINT8 ucPort;
    UINT32 index;
    char *pucBuf;
	UINT8 *pbuf;
	eth_header_raisecom_t *ptr_eth_raisecom_hdr;

    if (NULL == pkt || 0 == len)
        return ;

    /* begin added by jiangmingli for host debug */
    if (OPL_TRUE == g_bHostDsDbgEn)
    {
        //opl_dump_data(pkt,len,16);
        printf("\r\nRcv Pkt from PON/GE, len=%u\r\n",len);
        pucBuf = (char *)pkt;
        for(index = 0; index < len; index++)
        {
            if((index != 0) && (index%16 == 0))
            {
                printf("\r\n%02x ",(UINT8)pucBuf[index]);
            }
            else
            {
                printf("%02x ",(UINT8)pucBuf[index]);
            }
        }
        printf("\r\n");
        /* printf("%s,%d\n",__FUNCTION__,__LINE__); */
    }
    /* end added by jiangmingli for host debug */

 #if 0
	 /* For internal use, no parameter valid check */
	/*if(pulpkt[0] == 0 && pulpkt[1] == 0x0180c200 && *(UINT16 *)&pulpkt[2] == 0)
	{
		STP_IN_rx_bpdu(0,1,&pulpkt[1],len-4);
	}*/
 #endif

 	pbuf = (UINT8 *)pkt + sizeof(host_inbound_hdr_t);
	/* OAM packet processing */
	if(!memcmp(&oam_dest_mac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,ETH_MAC_ADDR_LEN))
	{
		if(pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1]){
          
            memmove(&pbuf[12], &pbuf[16], len-16);
            len = len - 4;
        }
		ptr_eth_oam_hdr =   (eth_header_none_tag_t *)((UINT8 *)pkt + sizeof(host_inbound_hdr_t));
		type = *(UINT8 *)((UINT8 *)pkt + sizeof(host_inbound_hdr_t) + sizeof(eth_header_none_tag_t));

		/* OAM packet processing */
		if((ETH_TYPE_SLOW_PROTOCOL == ptr_eth_oam_hdr->protocol)&&(ETH_SLOW_PROTOCOL_TYPE_OAM == type))
		{
			eopl_oam_pdu_receive((UINT8 *)pkt, len);
		}
		return;
	}

    #ifndef ONU_4PORT_AR8327
	/* IGMP packet processing */
	#if 0
	ptr_eth_hdr = (eth_header_with_sgl_tag_t *)((unsigned char *)pkt + sizeof(host_inbound_hdr_t));
	
	if(ETH_P_IP == ptr_eth_hdr->protocol)
	{
       	ptr_ipv4_hdr = (ipv4_header_t *)((unsigned char *)pkt + sizeof(host_inbound_hdr_t) + sizeof(eth_header_with_sgl_tag_t));
		 if((IPV4_PROTOCOL_IGMP == ptr_ipv4_hdr->protocol)&&(INET_IP_VER == ptr_ipv4_hdr->version))
       	{
			odmMulticastIgmpMsgHandler(pkt, len);
       	}
	}
	#else
	UINT32 vlan = 0;	

	pstInHeader = (host_inbound_hdr_t *)pkt;	 
	ucPort = pstInHeader->iport;

	if(!memcmp(&aucIgmpMac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,3))
	{
		if(pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1])
		{
			vlan = ((pbuf[14]&0xf) << 8)|pbuf[15];
			odmMulticastIgmpMsgHandler_MultiPort(
		            ucPort, (UINT16)vlan, 1, &pbuf[12+4], 
		            (UINT16)(len - 16 -sizeof(host_inbound_hdr_t)), (UINT8 *)pkt);
		}
		else {
			odmMulticastIgmpMsgHandler_MultiPort(
		            ucPort, 0, 0, &pbuf[12], 
		            (UINT16)(len - 12 -sizeof(host_inbound_hdr_t)), (UINT8 *)pkt);
		}
	}	

	/* RSTP packet processing */
	if(!memcmp(&aucBpduMac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,ETH_MAC_ADDR_LEN))
	{
		if (pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1])
		{
			vlan = (pbuf[14]&0xf)|pbuf[15];
			odmStpRcvBpdu(ucPort, vlan, &pbuf[16], len - sizeof(host_inbound_hdr_t) - 18);
		}
		else
		{
			odmStpRcvBpdu(ucPort, 0, &pbuf[12], len - sizeof(host_inbound_hdr_t) - 14);
		}
	}
	#endif
	
	#else
	UINT32 vlan = 0;
	UINT8 *pbuf;
	if(1 == atheros_4bytes_header_flag)
	{
	     pstInHeader = (host_inbound_hdr_t *)pkt;

	     pbuf = (UINT8 *)pkt + sizeof(host_inbound_hdr_t);
	     /* igmp handle for AR8327 */
	     if(!memcmp(&aucIgmpMac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,3))
	     {
	         pstInHeader->iport = atheros_portNum;
                pstInHeader->reserved = 0;

		  if(pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1])
		  {
		      vlan = (pbuf[14]&0xf)|pbuf[15];
                    odmMulticastIgmpMsgHandler_MultiPort(
                        (UINT8)atheros_portNum, (UINT16)vlan, 1, &pbuf[12+4], 
                        (UINT16)(len - 16 -sizeof(host_inbound_hdr_t)), (UINT8 *)pkt);
                }
                else {
                    odmMulticastIgmpMsgHandler_MultiPort(
                        (UINT8)atheros_portNum, 0, 0, &pbuf[12], 
                        (UINT16)(len - 12 -sizeof(host_inbound_hdr_t)), (UINT8 *)pkt);
		  }
	     }

	     /* rstp handle for AR8327*/
	     if (!memcmp(&rstp_dev_mac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)+6) ,ETH_MAC_ADDR_LEN))
            {
				abPortLoop[atheros_portNum -1] = 1;
            }
		 
	     if(!memcmp(&aucBpduMac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,ETH_MAC_ADDR_LEN))
            {   
                if (pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1])
		  {
		      vlan = (pbuf[14]&0xf)|pbuf[15];
		      odmStpRcvBpdu(atheros_portNum, vlan, &pbuf[16], len - sizeof(host_inbound_hdr_t) - 18);
		  }
		  else
		  {  
	  	      odmStpRcvBpdu(atheros_portNum, 0, &pbuf[12], len - sizeof(host_inbound_hdr_t) - 14);
                }
            }
	}
	#endif

 	/* begin: add by jiangmingli for RSTP process */
	#if 0
 	//#ifdef ONU_1PORT
 	if(!memcmp(&aucBpduMac[0], ((UINT8 *)pkt + sizeof(host_inbound_hdr_t)) ,ETH_MAC_ADDR_LEN))
	{
		UINT32 vlan = 0;
		UINT8 *pbuf;
		
		#if 0 //#ifdef ONU_1PORT
		return;
		#else
		pstInHeader = (host_inbound_hdr_t *)pkt;
		ucPort = pstInHeader->iport;

		/* check whether the packet comes from GE port, 0 - GE, 1 - PON, 2 - CPU */
		/*if (0 != ucPort)
		{
			return;
		}*/
		pbuf = (UINT8 *)pkt + sizeof(host_inbound_hdr_t);
		if (0 == ucPort)
		{
		  if(!memcmp(&rstp_dev_mac[0], &pbuf[6] ,ETH_MAC_ADDR_LEN))
		  {
		    abPortLoop[0] = 1;
		  }
		}
		
		//odmStpRcvBpdu((UINT8 *)pkt + sizeof(host_inbound_hdr_t), len - sizeof(host_inbound_hdr_t));
		if (pbuf[12] == customerTpid[0] && pbuf[13] == customerTpid[1])
		{
		  vlan = (pbuf[14]&0xf)|pbuf[15];
		  odmStpRcvBpdu(1, vlan, &pbuf[16], len - sizeof(host_inbound_hdr_t) - 18);
		}
		else
		{
	  	odmStpRcvBpdu(1, 0, &pbuf[12], len - sizeof(host_inbound_hdr_t) - 14);
    }
		return;
		#endif
	}
	#endif 
	/* end: add by jiangmingli for RSTP process */

   /* Other packet discarded */
}
#else
void eopl_pkt_process(void * pkt, UINT16 len)
{
}
#endif

/*
* eopl_host_send
*
* DESCRIPTION:
*   This function send packet from host interface.
*
* INPUT: pkt, len
*   pkt         - packet pointer
*   len         - length of packet
*
* OUTPUT: None
*
* RETURNS:
*   -1: failed; 0: succeed.
*
* SEE ALSO:
*/
INT16 eopl_host_send(void *pkt, UINT16 len)
{
    INT16 sts;

    host_outbound_hdr_t *ptr_pkt_hdr;
    char *pucBuf;
    UINT32 index;

    if (NULL == pkt || 0 == len)
        return -1;

    /* begin added by jiangmingli for host debug */
    if (OPL_TRUE == g_bHostDsDbgEn)
    {
        //opl_dump_data(pkt,len,16);
        printf("\r\nSend Pkt to PON, len=%u\r\n",len);
        pucBuf = (char *)pkt;
        for(index = 0; index < len; index++)
        {
            if((index != 0) && (index%16 == 0))
            {
                printf("\r\n%02x ",(UINT8)pucBuf[index]);
            }
            else
            {
                printf("%02x ",(UINT8)pucBuf[index]);
            }
        }
        printf("\r\n");
        /* printf("%s,%d\n",__FUNCTION__,__LINE__); */
    }
    /* end added by jiangmingli for host debug */

    ptr_pkt_hdr = (host_outbound_hdr_t *)pkt;

    /* If need to log packet */
    OPL_HOST_DEBUG("\r       Packet sent from Host interface:\n"
	                "\r       eport : %d\n\t"
	                "\r       icos : %d\n\t"
	                ,ptr_pkt_hdr->eport
	                ,ptr_pkt_hdr->icos
	                );
	  /*if need to dump packet */
    OPL_HOST_DUMP((void *)ptr_pkt_hdr, len);

    pthread_mutex_lock(&gHostSendMutex);
    /* BEGIN: Modified 2012/4/17 */
    /*eopl_dma_send() directly use dma send */
    //sts = eopl_socket_send((void *)ptr_pkt_hdr, len);
    sts = eopl_l2_send((void *)ptr_pkt_hdr, len);
    /* END:   Modified 2012/4/17 */

	if (sts != OPL_OK)
	{
		hostCnt.dmaTxFail++;
	}
    pthread_mutex_unlock(&gHostSendMutex);

    return sts;
}

/*
* eopl_host_recv
*
* DESCRIPTION:
*   This function receives packets from host interface.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS:
*   Always return 0.
*
* SEE ALSO:
*/
extern INT32 eopl_rx_pkt_dispatch(void *pBuff,UINT16 len);

UINT8 sendPktTestEnable = 0;
UINT8 sendToPonLink = 1;
UINT8 sendToGeLink  = 1;

INT32 eoplSendTest(UINT8 *pBuff,UINT32 len)
{
    host_outbound_hdr_t *ptr_pkt_hdr;

    ptr_pkt_hdr =(host_outbound_hdr_t *) &pBuff[3];

    if(sendToPonLink)
    {
        ptr_pkt_hdr->eport = 0;
        ptr_pkt_hdr->icos = 0;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 1;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 2;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 3;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 4;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 5;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 6;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 7;
        eopl_dma_send(&pBuff[3], (len-3));
    }

    if(sendToGeLink)
    {
        ptr_pkt_hdr->eport = 1;
        ptr_pkt_hdr->icos = 0;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 1;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 2;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 3;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 4;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 5;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 6;
        eopl_dma_send(&pBuff[3], (len-3));
        ptr_pkt_hdr->icos = 7;
        eopl_dma_send(&pBuff[3], (len-3));
    }

    return OPL_OK;
}

/*****************************************************************************
 
*****************************************************************************/
INT16 eopl_l2_send(unsigned char* packet, UINT16 length)
{
    l2_send_op l2_info;
    host_outbound_hdr_t *ptr_pkt_hdr = NULL;

    memset(&l2_info, 0, sizeof(l2_info));
    l2_info.usVid = 0;
   
    #if 1
    /*
       |p5
  +---------+
  | opl6750 |
  +---------+
       |
  +---------+
  | rtl8305 |
  +---------+
   |p1 ... |p4
  */
    ptr_pkt_hdr = (host_outbound_hdr_t *)packet;
	SetLgcMaskBitNoCheck(ptr_pkt_hdr->eport, &l2_info.portmask);
	#if 0
    /*send to pon*/
    /*Logic port mask begins with bit 0.*/
    if (1 == ptr_pkt_hdr->eport)
    {
        l2_info.portmask.pbits[0] |= (1U << 0);
    }
    /*send to gmac.
    Realtek header will be added in future.*/
    else
    {
        l2_info.portmask.pbits[0] |= (1U << 1);
    }
	#endif
    #else
    SetLgcMaskBitNoCheck(LOGIC_UPPON_PORT, &l2_info.portmask);
    #endif
   

    //printf("eopl_l2_send portmask=%x\n", l2_info.portmask.pbits[0]);
    
    /*exclude cpu head*/
    if(DRV_OK == l2_send_by_port(&packet[1], length-1, &l2_info)){
        return OPL_OK;
    }
    return -1;
}


/*****************************************************************************
 º¯ Êý Ãû  : eopl_socket_send
 
*****************************************************************************/
INT16 eopl_socket_send(unsigned char* packet, UINT16 length)
{
    int raw_socket;
    struct sockaddr_ll sll;
    struct ifreq ifstruct;

    raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_TYPE_SLOW_PROTOCOL));
    if (0 > raw_socket){
        printf("raw_socket socket create error!\n");
        return -1;
    }

    strcpy(ifstruct.ifr_name, "eth0");
    ioctl(raw_socket, SIOCGIFINDEX, &ifstruct);
    
    memset( &sll, 0, sizeof(sll) );
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifstruct.ifr_ifindex;
    sll.sll_protocol = htons(ETH_TYPE_SLOW_PROTOCOL );

    if(bind(raw_socket, (struct sockaddr *) &sll, sizeof(sll)) == -1 ){
        printf("\nraw_socket bind create error!\n");
        return -1;
    }

    /*exclude cpu head*/
    if (-1 == sendto(raw_socket, &packet[1],
                        length-1, 0,
                        &sll,
                        sizeof(struct sockaddr_ll)) )
    {
        printf("\nraw_socket send error!\n");
        return -1;
    }  
    return OPL_OK;
}

#define CPU_HEAD_LEN 4

INT16 eopl_host_socket_recv(int socket_fd)
{
    host_inbound_hdr_t *ptr_pkt_hdr;
    unsigned char  oam_rcv_pkt[2048];
    unsigned char * pBuff = oam_rcv_pkt;
	host_inbound_hdr_t *pstInHeader = (host_inbound_hdr_t *)pBuff;
    
    static INT32 usLen;
    INT32 counter=0;

	struct msghdr msg;
    struct iovec iov;
    struct mw_l2_ctl_s l2_ctl;

    /*reserve opl cpu head*/
    pBuff[0]=0;
    pBuff[1]=0;
    pBuff[2]=0;
    pBuff[3]=1;

	memset(&msg, 0, sizeof(msg));
	memset(&iov, 0, sizeof(iov));
	memset(&l2_ctl, 0, sizeof(l2_ctl));
    iov.iov_base = (void *)&pBuff[CPU_HEAD_LEN];
    iov.iov_len = 2048-CPU_HEAD_LEN;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;   
    msg.msg_control = &l2_ctl;
    msg.msg_controllen = sizeof(l2_ctl);

    //while ((usLen = recv(socket_fd, &pBuff[CPU_HEAD_LEN], 2048-CPU_HEAD_LEN, 0)) > 0)    
    while((usLen = recvmsg(socket_fd, &msg, 0)) > 0)    
    {
        usLen+=CPU_HEAD_LEN;/*reserve opl cpu head*/
        
		pstInHeader->iport = l2_ctl.sll_port;
	    pstInHeader->reserved = 0;
        
        /* valid dma packet */
        hostCnt.dmaRx ++;
    
        if(sendPktTestEnable)
        {
            eoplSendTest((unsigned char *)pBuff,usLen);
        }
        
        opl_dump_data((unsigned char *)pBuff,usLen,16);

     /* To handle the packet */
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
        eopl_pkt_process((void *)pBuff, usLen);
#elif defined(ONU_4PORT_AR8327)
        atheros_4bytes_header_flag = 0;
        ATHEROS_HEADER_FRAME_t *pInHeader = (ATHEROS_HEADER_FRAME_t *)pBuff;
        if(0 == OPL_MEMCMP(&(pInHeader->head_type), atherosTpid, 2))
        {  
            usLen = usLen - 4;
         atheros_4bytes_header_flag = 1;
         atheros_portNum = pInHeader->portNum;
         memcpy((char *)&(pBuff[16]), (char *)&(pBuff[16+4]), usLen -16-4);
        }
        else if (0 == OPL_MEMCMP(&(pInHeader->head_type), customerTpid, 2)){
            pInHeader = (ATHEROS_HEADER_FRAME_t *)((unsigned char *)pBuff+4);
            if (0 == OPL_MEMCMP(&(pInHeader->head_type), atherosTpid, 2)){
                usLen = usLen - 4;
                atheros_4bytes_header_flag = 1;
                atheros_portNum = pInHeader->portNum;
                memcpy((char *)&(pBuff[16]), (char *)&(pBuff[16+8]), usLen -16-8);
            }
        }
        
        eopl_pkt_process((void *)pBuff, usLen);
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
        eopl_rx_pkt_dispatch(pBuff, usLen);
#endif
        ptr_pkt_hdr = (host_inbound_hdr_t *)pBuff;

        /* If need to log packet */
        OPL_HOST_DEBUG("\r       Packet received from Host interface:\n"
                       "\r       Iport : %d\n\t"
                       ,ptr_pkt_hdr->iport);

        /*if need to dump packet */
        OPL_HOST_DUMP((void *)pBuff, usLen);
        counter++;
        if (counter > 32)
          break;
		
		msg.msg_control = &l2_ctl;
		msg.msg_controllen = sizeof(l2_ctl);
    }

    return 0;
}


INT16 eopl_host_recv(void)
{
    host_inbound_hdr_t *ptr_pkt_hdr;
    static void *pBuff;
    static INT32 usLen;
    INT32 counter=0;
    
    /* Begin DMA 0 receive routine */
    #ifdef OPL_DMA0_ETH
    while ((usLen = eopl_dma_recv((volatile INT8 **)&pBuff, 0)) > 0)
    {
    #else
    while((usLen = eopl_dma_recv((volatile INT8 **)&pBuff, 0)) > 0)
    {
        /* End DMA 0 receive routine */
        eopl_dma_recv((volatile INT8 **)&pBuff, 1);
    #endif
	/* valid dma packet */
	hostCnt.dmaRx ++;
	
        if(sendPktTestEnable)
        {
            eoplSendTest((unsigned char *)pBuff,usLen);
        }

		opl_dump_data((unsigned char *)pBuff,usLen,16);

  	 /* To handle the packet */
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
        eopl_pkt_process((void *)pBuff, usLen);
#elif defined(ONU_4PORT_AR8327)
        atheros_4bytes_header_flag = 0;
        ATHEROS_HEADER_FRAME_t *pInHeader = (ATHEROS_HEADER_FRAME_t *)pBuff;
        if(0 == OPL_MEMCMP(&(pInHeader->head_type), atherosTpid, 2))
        {  
            usLen = usLen - 4;
	     atheros_4bytes_header_flag = 1;
	     atheros_portNum = pInHeader->portNum;
	     memcpy((char *)&(pBuff[16]), (char *)&(pBuff[16+4]), usLen -16-4);
        }
		else if (0 == OPL_MEMCMP(&(pInHeader->head_type), customerTpid, 2)){
			pInHeader = (ATHEROS_HEADER_FRAME_t *)((unsigned char *)pBuff+4);
			if (0 == OPL_MEMCMP(&(pInHeader->head_type), atherosTpid, 2)){
				usLen = usLen - 4;
	     		atheros_4bytes_header_flag = 1;
	     		atheros_portNum = pInHeader->portNum;
	     		memcpy((char *)&(pBuff[16]), (char *)&(pBuff[16+8]), usLen -16-8);
			}
		}
		
        eopl_pkt_process((void *)pBuff, usLen);
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
        eopl_rx_pkt_dispatch(pBuff, usLen);
#endif
        ptr_pkt_hdr = (host_inbound_hdr_t *)pBuff;

        /* If need to log packet */
        OPL_HOST_DEBUG("\r       Packet received from Host interface:\n"
	                   "\r       Iport : %d\n\t"
	                   ,ptr_pkt_hdr->iport);

        /*if need to dump packet */
        OPL_HOST_DUMP((void *)pBuff, usLen);
        counter++;
        if (counter > 32)
          break;
    }

    return 0;
}




/*
* eopl_host_dump_enable
*
* DESCRIPTION:
*   This function enable packet dump switch.
*
* INPUT: enable
*    enable    - TRUE: enable; FALSE:disable.
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_host_dump_enable(INT32 enable)
{
    host_dump_enable = enable;
}

#if defined(ONU_4PORT_AR8327)
UINT8 eopl_host_atheros_headflag_get()
{
    return atheros_4bytes_header_flag;
}

UINT32 eopl_host_atheros_portnum_get()
{
    return atheros_portNum;
}
#endif

/*
* eopl_dump
*
* DESCRIPTION:
*   This function dump the contents of packets in Hex.
*
* INPUT: pkt, len, pCaption
*    pkt    - packet pointer
*    len    - length of packet
*  pCaption - caption of dump
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_dump(const void *pkt, UINT16 len, const UINT8 *pCaption)
{
    UINT16 i;
    UINT8 *ch;

/*
    if (OPL_NULL == pkt)
        return;
*/
    if(!host_dump_enable) return;

    OPL_HOST_DEBUG("Pkt addr = 0x%x , len = %d \n",(UINT32)pkt, len);

	for(i=0; i<len; i++)
	{
		ch = (char *)pkt+i;

		if (0 == (i%16))
		{
		    OPL_HOST_DEBUG("\n	0x%x : ",(UINT32)(pkt+i));
		}
		OPL_HOST_DEBUG("%02x",*ch);
		OPL_HOST_DEBUG(" ");

		/* Don't print all of the big packet */
		if (i >=1518) break;
	}
	OPL_HOST_DEBUG("\n\n");
}

INT16 eopl_send_to_uplink(UINT16 portNum,void *pkt,UINT16 len)
{
	char buff[2048];
	host_outbound_hdr_t *ptr_pkt_hdr = (host_outbound_hdr_t*)&buff[0];

    if (NULL == pkt || 0 == len)
        return -1;

	ptr_pkt_hdr->reserved = 0x00;
	ptr_pkt_hdr->eport = LOGIC_PON_PORT;
	ptr_pkt_hdr->icos  = 0x7;

	memcpy(&buff[1],(char *)pkt,len);

	opl_dump_data(buff,len+1,16);

	return eopl_host_send(buff,len+1);
}

INT16 eopl_send_to_downlink_one(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
	char buff[2048];
	host_outbound_hdr_t *ptr_pkt_hdr = (host_outbound_hdr_t*)&buff[0];

    if (STP_PKT_TYPE == pktType)
    {
        ptr_pkt_hdr->reserved = 0x00;
    	ptr_pkt_hdr->eport = portNum;
    	ptr_pkt_hdr->icos  = 0x1;
    }
    else{
    	ptr_pkt_hdr->reserved = 0x00;
    	ptr_pkt_hdr->eport = portNum;
    	ptr_pkt_hdr->icos  = 0x7;
    }

	memcpy(&buff[1],(char *)pkt,len);

	opl_dump_data(buff,len+1,16);

	return eopl_host_send(buff,len+1);
}

INT16 eopl_send_to_down_link_6045(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
	char buff[2048];
	host_outbound_hdr_t *ptr_pkt_hdr = (host_outbound_hdr_t*)&buff[0];
	GT_FROM_CPU_DSA_TAG_RSV_t *pRsvDsaTagPkt = (GT_FROM_CPU_DSA_TAG_RSV_t *)&buff[1];
	GT_FROM_CPU_DSA_TAG_IGMP_t *pIgmpDsaTagPkt =(GT_FROM_CPU_DSA_TAG_IGMP_t *) &buff[1];
	char *pPkt = (char * )pkt;
    UINT32 length;
    UINT32 vlanMode;

	ptr_pkt_hdr->reserved = 0x00;
	ptr_pkt_hdr->eport = 0;
	ptr_pkt_hdr->icos  = 0x7;

    dalPortVlanModeGet(portNum,&vlanMode);

    switch(pktType)
    {
        case OAM_PKT_TYPE:
            break;
        case STP_PKT_TYPE:
            memcpy(&buff[1],pPkt,12);
        	pRsvDsaTagPkt->dir = 0x01;
    		pRsvDsaTagPkt->trg_tagged = withTag;
			pRsvDsaTagPkt->trg_dev = 0x00;
			pRsvDsaTagPkt->cfi = 0x00;
    		pRsvDsaTagPkt->trg_port = portNum - 1;
    		pRsvDsaTagPkt->pad1 = 0x00;
    		pRsvDsaTagPkt->pad2 = 0x00;
            /*reserved mac packet always be not tagged when egressing out from uni port*/
            memcpy(&buff[17],pPkt+12,len-12);
            length = len + 1+4;/*opconn host dma header,ether type tag,dsa tag*/
            break;
        case IGMP_PKT_TYPE:
            memcpy(&buff[1],pPkt,12);

            if(vlanMode == VLAN_TRANSPARENT && withTag)
            {
                memcpy(&buff[17],pPkt+12,len-12);
            	length = len+1+4;
        		pIgmpDsaTagPkt->dir = 0x01;
        		pIgmpDsaTagPkt->trg_tagged = withTag;
				pIgmpDsaTagPkt->trg_dev = 0x00;
				pIgmpDsaTagPkt->cfi = 0x00;
        		pIgmpDsaTagPkt->trg_port = portNum - 1;
        		pIgmpDsaTagPkt->pad1 = 0x00;
        		pIgmpDsaTagPkt->pad2 = 0x00;
            }else
            {
                memcpy(&buff[13],pPkt+12,len-12);
            	length = len+1;
        		pIgmpDsaTagPkt->dir = 0x01;
        		pIgmpDsaTagPkt->trg_tagged = withTag;
				pIgmpDsaTagPkt->trg_dev = 0x00;
        		pIgmpDsaTagPkt->trg_port = portNum - 1;
				pIgmpDsaTagPkt->cfi = 0x00;
        		pIgmpDsaTagPkt->pad1 = 0x00;
        		pIgmpDsaTagPkt->pad2 = 0x00;
            }

            break;
        default:
            break;
    }

    opl_dump_data(buff,len+5,16);

    return eopl_host_send(buff,length);
}

INT16 eopl_send_to_down_link_6046(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
	char buff[2048];
	host_outbound_hdr_t *ptr_pkt_hdr = (host_outbound_hdr_t*)&buff[0];
	GT_ETHER_FROM_CPU_DSA_TAG_RSV_t *pRsvDsaTagPkt = (GT_ETHER_FROM_CPU_DSA_TAG_RSV_t *)&buff[1];
	GT_ETHER_FROM_CPU_DSA_TAG_IGMP_t *pIgmpDsaTagPkt =( GT_ETHER_FROM_CPU_DSA_TAG_IGMP_t *)&buff[1];
	char *pPkt = (char * )pkt;
    UINT32 length;
    UINT32 vlanMode;

	ptr_pkt_hdr->reserved = 0x00;
	ptr_pkt_hdr->eport = 0;
	ptr_pkt_hdr->icos  = 0x7;

    dalPortVlanModeGet(portNum,&vlanMode);

    switch(pktType)
    {
        case OAM_PKT_TYPE:
            break;
        case STP_PKT_TYPE:
            memcpy(&buff[1],pPkt,12);
        	pRsvDsaTagPkt->etherType = 0x9100;
        	pRsvDsaTagPkt->reserved = 0x00;
        	pRsvDsaTagPkt->dir = 0x01;
    		pRsvDsaTagPkt->trg_tagged = withTag;
			pRsvDsaTagPkt->trg_dev = 0x00;
			pRsvDsaTagPkt->cfi = 0x00;
    		pRsvDsaTagPkt->trg_port = portNum - 1;
    		pRsvDsaTagPkt->pad1 = 0x00;
    		pRsvDsaTagPkt->pad2 = 0x00;
            /*reserved mac packet always be not tagged when egressing out from uni port*/
            memcpy(&buff[21],pPkt+12,len-12);
            length = len + 1+4+4;/*opconn host dma header,ether type tag,dsa tag*/
            break;
        case IGMP_PKT_TYPE:
            memcpy(&buff[1],pPkt,12);
        	pIgmpDsaTagPkt->etherType = 0x9100;
        	pIgmpDsaTagPkt->reserved = 0x00;

            if(vlanMode == VLAN_TRANSPARENT && withTag)
            {
                memcpy(&buff[21],pPkt+12,len-12);
            	length = len+1+4+4;
        		pIgmpDsaTagPkt->dir = 0x01;
        		pIgmpDsaTagPkt->trg_tagged = withTag;
				pIgmpDsaTagPkt->trg_dev = 0x00;
				pIgmpDsaTagPkt->cfi = 0x00;
        		pIgmpDsaTagPkt->trg_port = portNum - 1;
        		pIgmpDsaTagPkt->pad1 = 0x00;
        		pIgmpDsaTagPkt->pad2 = 0x00;
            }else
            {
                memcpy(&buff[21],pPkt+12,len-12);
            	length = len+1+4+4;
        		pIgmpDsaTagPkt->dir = 0x01;
        		pIgmpDsaTagPkt->trg_tagged = withTag;
				pIgmpDsaTagPkt->trg_dev = 0x00;
        		pIgmpDsaTagPkt->trg_port = portNum - 1;
				pIgmpDsaTagPkt->cfi = 0x00;
        		pIgmpDsaTagPkt->pad1 = 0x00;
        		pIgmpDsaTagPkt->pad2 = 0x00;
            }

            break;
        default:
            break;
    }

    opl_dump_data(buff,len+5,16);

    return eopl_host_send(buff,length);
}

INT16 eopl_send_to_down_link_atheros(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
	char buff[1600];
	ATHEROS_HEADER_FRAME_t *pOutHeader = (ATHEROS_HEADER_FRAME_t*)&buff[0];
    UINT32 length;
    UINT32 index;

#if defined(ONU_4PORT_AR8306)
    pOutHeader->fromCpu = 1;
    pOutHeader->ver = 2;
    pOutHeader->priority = 3;
    pOutHeader->type = 0;
    pOutHeader->broadcast = 0;
    pOutHeader->portNum = portNum;
    pOutHeader->ver2 = 2;
#elif defined(ONU_4PORT_AR8228)
    pOutHeader->fromCpu = 1;
    pOutHeader->ver = 2;
    pOutHeader->priority = 3;
    pOutHeader->type = 0;
    //VOS_BIT_SET(pOutHeader->portNum, portNum);
    pOutHeader->portNum = 1<<portNum;
#endif

    switch(pktType)
    {
        case OAM_PKT_TYPE:
            break;
        case STP_PKT_TYPE:
        case IGMP_PKT_TYPE:
            memcpy(&buff[0],pkt,12);
            /*reserved mac packet always be not tagged when egressing out from uni port*/
            memcpy(&buff[14],pkt+12,len-12);
            length = len + 2; /*opconn host dma header,ether type tag,dsa tag*/
            break;

        default:
            break;
    }

    opl_dump_data(buff,len+2,16);

    /* begin added by jiangmingli for host debug */
    if (OPL_TRUE == g_bHostUsDbgEn)
    {
        //opl_dump_data(buff,length,16);
        printf("\r\nSend Pkt to atheros, len=%u\r\n",length);
        for(index = 0; index < length; index++)
        {
            if((index != 0) && (index%16 == 0))
            {
                printf("\r\n%02x ",(UINT8)buff[index]);
            }
            else
            {
                printf("%02x ",(UINT8)buff[index]);
            }
        }
        printf("\r\n");
        /* printf("%s,%d\n",__FUNCTION__,__LINE__); */
    }
    /* end added by jiangmingli for host debug */

    return syscall(NR_TEST_SETDATA, &buff[0], length);
}

#if defined(ONU_4PORT_AR8327)
INT16 eopl_send_to_down_link_atheros_8327(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
	char buff[2048];
	ATHEROS_HEADER_OUTBOUND_FRAME_t *pOutHeader = (ATHEROS_HEADER_OUTBOUND_FRAME_t*)&buff[0];
       UINT32 length;
       UINT32 index;

       //memcpy(&buff[1],(char *)pkt,len);
       memcpy(&buff[1],pkt,12);
       /*reserved mac packet always be not tagged when egressing out from uni port*/
	//memset(&buff[13],0, 4);
       memcpy(&buff[17],pkt+12,len-12);
       length = len + 5; /*opconn host dma header,ether type tag,dsa tag*/
       //length = len + 1;

	   pOutHeader->fromCpu = 1;
		
    if (STP_PKT_TYPE == pktType)
    {  
        pOutHeader->reserved = 0x00;
    	pOutHeader->eport = 0;
    	pOutHeader->icos  = 0x1;

		pOutHeader->head_type = 0x9100;
		pOutHeader->ver = 2;
       pOutHeader->priority = 1;
       pOutHeader->type = 0;
       pOutHeader->portNum = 1<<portNum;
    }
    else{
    	pOutHeader->reserved = 0x00;
    	pOutHeader->eport = 0;
    	pOutHeader->icos  = 0x7;

	pOutHeader->head_type = 0x9100;
	pOutHeader->ver = 2;
       pOutHeader->priority = 7;
       pOutHeader->type = 0;
       pOutHeader->portNum = 1<<portNum;
    }

	opl_dump_data(buff,len+1,16);

    /* begin added by jiangmingli for host debug */
    if (OPL_TRUE == g_bHostUsDbgEn)
    {
        printf("\r\nSend Pkt to atheros 8327, len=%u\r\n",length);
        for(index = 0; index < length; index++)
        {
            if((index != 0) && (index%16 == 0))
            {
                printf("\r\n%02x ",(UINT8)buff[index]);
            }
            else
            {
                printf("%02x ",(UINT8)buff[index]);
            }
        }
        printf("\r\n");
    }
    /* end added by jiangmingli for host debug */

	return eopl_host_send(buff,len+1);
}
#endif

INT16 eopl_send_to_down_link(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len)
{
    if (NULL == pkt || 0 == len)
        return -1;
    #if defined(ONU_1PORT)
	  return eopl_send_to_downlink_one(portNum,withTag,pktType,pkt,len);
	#elif defined(ONU_4PORT_88E6045)
		return eopl_send_to_down_link_6045(portNum,withTag,pktType,pkt,len);
	#elif defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
		return eopl_send_to_down_link_6046(portNum,withTag,pktType,pkt,len);
    #elif defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)
		return eopl_send_to_down_link_atheros(portNum,withTag,pktType,pkt,len);
    #elif defined(ONU_4PORT_AR8327)
	       return eopl_send_to_down_link_atheros_8327(portNum,withTag,pktType,pkt,len);
    #endif
}

INT32 eopl_rx_pkt_process(UINT32 pktType,UINT8 portNum,UINT16 vlan,UINT8 withTag,UINT8 *pOrgPkt,UINT8 *payLoad,UINT16 len)
{
	opl_host_printf(("pkt type = %d.\n",pktType));
	opl_host_printf(("port num = %d.\n",portNum));
	opl_host_printf(("vlan     = %04x.\n",vlan));
	opl_host_printf(("with tag = %d.\n",withTag));
	opl_host_printf(("len      = %d.\n",len));
	opl_dump_data(payLoad,len,16);

    switch(pktType)
    {
        case OAM_PKT_TYPE:
			eopl_oam_pdu_receive((UINT8 *)pOrgPkt, len+16);
            break;
        case IGMP_PKT_TYPE:
            odmMulticastIgmpMsgHandler_MultiPort(
              portNum, vlan, withTag, payLoad, len, pOrgPkt);
            break;
        case STP_PKT_TYPE:
			odmStpRcvBpdu(portNum, vlan, payLoad, len);
            break;
        default:
            break;
    }

	return OPL_OK;
}

INT32 eopl_rx_pkt_dispatch_6045(void *pBuff,UINT16 len)
{
	char *pPkt = (char *)pBuff;
	host_inbound_hdr_t *pInHeader = (host_inbound_hdr_t *)pBuff;
	GT_TO_CPU_DSA_TAG_RSV_t *pRsvDsaTagPkt = (GT_TO_CPU_DSA_TAG_RSV_t *)&pPkt[4];
	GT_TO_CPU_DSA_TAG_IGMP_t *pIgmpDsaTagPkt = (GT_TO_CPU_DSA_TAG_IGMP_t *)&pPkt[4];

	UINT8 *payLoad = NULL;
	UINT32 portNum;
	UINT32 withTag;
	UINT32 pktType;
	UINT32 vlan = 0;
	UINT32 hdrLen = 0;
	char oamMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x02};
	char stpMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
	char igmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
    UINT32 vlanMode;

	portNum = pInHeader->iport;

	if(1 == portNum)
	{
		portNum = 0;
		withTag = 0;
		vlan 	= 0;
		payLoad = &pPkt[4];
		hdrLen  = 4+12;

		if(!(memcmp(&pPkt[4],oamMac,6)))
		{
			pktType = OAM_PKT_TYPE;
		}else if(!(memcmp(&pPkt[4],stpMac,6)))
		{
			pktType = STP_PKT_TYPE;
		}
		else if(!(memcmp(&pPkt[4],igmpMac,3)))
		{
			pktType = IGMP_PKT_TYPE;
		}else
		{
			opl_host_printf(("unknown pkt.\n"));
		}
	}else
	{
		if(!(memcmp(pRsvDsaTagPkt->da,oamMac,6)))
		{
			pktType = OAM_PKT_TYPE;
			portNum = pRsvDsaTagPkt->src_port + 1;
			withTag = pRsvDsaTagPkt->src_tagged;
			vlan	= pRsvDsaTagPkt->vid;
			payLoad = pRsvDsaTagPkt->payLoad;
			hdrLen  = 4+4+12;
            /*always from uplink port,no need check*/
		}else if(!(memcmp(pRsvDsaTagPkt->da,stpMac,6)))
		{
			pktType = STP_PKT_TYPE;
			portNum = pRsvDsaTagPkt->src_port + 1;
			withTag = pRsvDsaTagPkt->src_tagged;
			vlan	= pRsvDsaTagPkt->vid;
			payLoad = pRsvDsaTagPkt->payLoad;

            dalPortVlanModeGet(portNum,&vlanMode);
            if(vlanMode == VLAN_TRANSPARENT)
            {
                if(payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
                {
                    hdrLen  = 4+4+12+4;
					withTag = OPL_TRUE;
					vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
                    payLoad = &payLoad[4];
                }else
                {
                    hdrLen  = 4+4+12;
                }
            }

		}
		else if(!(memcmp(pRsvDsaTagPkt->da,igmpMac,3)))
		{
			pktType = IGMP_PKT_TYPE;
			portNum = pIgmpDsaTagPkt->src_port + 1;
			withTag = pIgmpDsaTagPkt->src_tagged;
			vlan	= pIgmpDsaTagPkt->vid;
			payLoad = pIgmpDsaTagPkt->payLoad;

            dalPortVlanModeGet(portNum,&vlanMode);
            if(vlanMode == VLAN_TRANSPARENT)
            {
                if(payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
                {
                    hdrLen  = 4+4+12+4;
					withTag = OPL_TRUE;
					vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
                    payLoad = &payLoad[4];
                }else
                {
                    hdrLen  = 4+4+12;
                }
            }
		}else
		{
			opl_host_printf(("unknown pkt.\n"));
		}
	}


	opl_dump_data(pPkt,len,16);

	return eopl_rx_pkt_process(pktType,portNum,vlan,withTag,(UINT8 *)pBuff,payLoad,len-hdrLen);
}


INT32 eopl_rx_pkt_dispatch_6046(void *pBuff,UINT16 len)
{
  char buff[2048];
 	char *pPkt = (char *)pBuff;
	host_inbound_hdr_t *pInHeader = (host_inbound_hdr_t *)pBuff;
	GT_ETHER_TO_CPU_DSA_TAG_RSV_t *pRsvDsaTagPkt =(GT_ETHER_TO_CPU_DSA_TAG_RSV_t *) &pPkt[4];
	GT_ETHER_TO_CPU_DSA_TAG_IGMP_t *pIgmpDsaTagPkt =(GT_ETHER_TO_CPU_DSA_TAG_IGMP_t *) &pPkt[4];

	UINT8 *payLoad = NULL;
	UINT32 portNum;
	UINT32 withTag;
	UINT32 pktType;
	UINT32 vlan = 0;
	UINT32 hdrLen = 0;
	char oamMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x02};
	char stpMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
	char igmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
    UINT32 vlanMode;

	portNum = pInHeader->iport;

	if(1 == portNum)
	{
		portNum = 0;
		withTag = 0;
		vlan 	= 0;
		payLoad = &pPkt[4];
		hdrLen  = 4+12;

		if(!(memcmp(&pPkt[4],oamMac,6)))
		{
			pktType = OAM_PKT_TYPE;
		}else if(!(memcmp(&pPkt[4],stpMac,6)))
		{
			pktType = STP_PKT_TYPE;
      /*payLoad = &payLoad[12];
      if (payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
      {
          withTag = TRUE;
          vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
          hdrLen = 12+4+4;
          payLoad = &payLoad[4];
      }
      else
      {
          hdrLen = 12+4;
      }*/
		}
		else if(!(memcmp(&pPkt[4],igmpMac,3)))
		{
			pktType = IGMP_PKT_TYPE;
      /*payLoad = &payLoad[12];
      if (payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
      {
          withTag = TRUE;
          vlan  = (payLoad[2]&0xf)<<8|payLoad[3];
          hdrLen = 12+4+4;
          payLoad = &payLoad[4];
      }
      else
      {
          hdrLen = 12+4;
      }*/
      return odmMulticastIgmpMsgHandler(pPkt, len);
		}else
		{
			opl_host_printf(("unknown pkt.\n"));
		}
	}else
	{
		if(!(memcmp(pRsvDsaTagPkt->da,oamMac,6)))
		{
			pktType = OAM_PKT_TYPE;
			portNum = pRsvDsaTagPkt->src_port + 1;
			withTag = pRsvDsaTagPkt->src_tagged;
			vlan	= pRsvDsaTagPkt->vid;
			payLoad = pRsvDsaTagPkt->payLoad;
			hdrLen  = 4+4+4+12;
            /*always from uplink port,no need check*/
		}else if(!(memcmp(pRsvDsaTagPkt->da,stpMac,6)))
		{
			pktType = STP_PKT_TYPE;
			portNum = pRsvDsaTagPkt->src_port + 1;
			withTag = pRsvDsaTagPkt->src_tagged;
			vlan	= pRsvDsaTagPkt->vid;
			payLoad = pRsvDsaTagPkt->payLoad;

      if(payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
      {
          hdrLen  = 4+4+4+4+12+4;
          withTag = OPL_TRUE;
          vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
          payLoad = &payLoad[4];
      }else
      {
          hdrLen  = 4+4+4+4+12;
      }
		}
		else if(!(memcmp(pRsvDsaTagPkt->da,igmpMac,3)))
		{
			pktType = IGMP_PKT_TYPE;
			portNum = pIgmpDsaTagPkt->src_port + 1;
			withTag = pIgmpDsaTagPkt->src_tagged;
			vlan	= pIgmpDsaTagPkt->vid;
			payLoad = pIgmpDsaTagPkt->payLoad;

      vosMemCpy(buff, pBuff, 16);
      vosMemCpy(buff+16, pIgmpDsaTagPkt->payLoad, len-28);
      if(payLoad[0] == customerTpid[0] && payLoad[1] == customerTpid[1])
      {
          hdrLen  = 4+4+4+4+12+4;
          withTag = OPL_TRUE;
          vlan 	= (payLoad[2]&0xf)<<8|payLoad[3];
          payLoad = &payLoad[4];
      }else
      {
          hdrLen  = 4+4+4+4+12;
      }
      pBuff = buff;
		}else
		{
			opl_host_printf(("unknown pkt.\n"));
		}
	}


	opl_dump_data(pPkt,len,16);

	return eopl_rx_pkt_process(pktType,portNum,vlan,withTag,(UINT8 *)pBuff,payLoad,len-hdrLen);
}

INT32 eopl_rx_pkt_dispatch(void *pBuff,UINT16 len)
{
    opl_dump_data((UINT8 *)pBuff,len,16);

    #if defined(ONU_4PORT_88E6045)
		return eopl_rx_pkt_dispatch_6045(pBuff,len);
	#elif defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
		return eopl_rx_pkt_dispatch_6046(pBuff,len);
    #endif
}

#ifdef OPCONN_HOST_DEBUG

static volatile UINT8 host_bnd_test[2048] =
{
    /* Outbound & ountbound header*/
    0x00,
    /* Ethernet header */
    0x00, 0x00, 0xc2, 0x00, 0x00, 0x01,  /* Destionation MAC Address */
    0x00, 0x05, 0x12, 0x34, 0x56, 0x78,  /* Source MAC Address */

	//0x00, 0x00,                          /* Vlan tag type */
    //0x00, 0x00,                          /* Vlan tag value */

    0x08, 0x00,                          /* Ethernet type IPV4 0x0800 */

    /* IPv4 header */
    0x46, 0x00,                          /* IP verion = 4, ihl = 6, tos =0x00, to be verfied */
    0x00, 0x20,                          /* Total length of IP packet, 24 (ip header) + 8 (igmp msg) = 32 */
    0x00, 0x00,                          /* Identifier */
    0x00, 0x00,                          /* Fragment offset */
    0x01,                                /* Time to live */
    0x00,                                /* Protocol, IGMP = 2 */
    0x00, 0x00,                          /* checksum, to be calculated before sent */
    0x00, 0x00, 0x00, 0x00,              /* Source IP Address */
    0x00, 0x00, 0x00, 0x00,              /* Destination IP Address */

    /* IP router alert option */
    0x94, 0x04,                          /* Flag = 1; class =0; option =20; value =0: Router shall examine packet */
    0x00, 0x00,                          /* Fragment offset */

    /* IGMP message */
    0x16,                                /* type, default: IGMP Ver. 2 membership report */
    0x00,                                /* max response time, default: 100(10 seconds) */
    0x00,                                /* IP-style checksum, to be calculated before sent */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */

    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */
    0x00, 0x00, 0x00, 0x00,              /* Group address, default: 224.0.0.1 */

};

/*
* eopl_host_send_test
*
* DESCRIPTION:
*   This function test to send packets.
*
* INPUT:
*			port     - port ID;
*			icos     - ICOS;
*			type     - 0: fixed with input data and dataLen;
*							1: fixed with 1 byte data;
*							2:random data.
*							3: increased with 0x0;
*							4:decreased with 0xFF;
*
*			pktlen   - real packet length to send;
*         count    - count to send.
*			data     - data pointer;
*         datalen - data ength;
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_host_send_test (
	UINT32 port ,
	UINT8 icos,
	UINT8 type,
	UINT16 pktLen,
	UINT32 count,
	UINT8 *data,
	UINT16 dataLen)
{
    UINT32 iPort, iIcos;
    UINT32 i,j;
    UINT16 len;
    UINT32 *rand_data;
	INT16 sts;
	UINT8 ucData;

    host_outbound_hdr_t *ptr_pkt_hdr;

	/* pointer to packet content */
    ptr_pkt_hdr = (host_outbound_hdr_t *)&host_bnd_test[0];

	/* Count to sent, if zero, send 65535 times */
	if(count == 0) count = 65535;

	 for(i=0;i<count;i++)
    {
		iPort =port;
	   if(iPort>2)  port = rand()%2;

		iIcos = icos;
		if(iIcos >7)  icos = rand()%8;

		len  = pktLen;
		if(len>1518)	len  = rand()%1518;

		switch (type)
		{
			case 0:
				/* Set the packet content */
				if((NULL!= data)&&(dataLen<=1518))
				{
					memcpy((void *)&host_bnd_test[1], data, dataLen);
				}else
				{
					return;
				}
				break;

			case 1:
				if(NULL!= data)
				{
					memset((void *)&host_bnd_test[1], *(UINT8 *)data, len);
				}else
				{
					return;
				}
				break;

			case 3:
				ucData = 0x0;
				 for(j=0;j<len;j++)
				 {
				 	 host_bnd_test[1+ j] = ucData;
					 ucData++;
					 if(0xFF==ucData) ucData = 0x0;
				 }
				break;

			case 4:
				ucData = 0xFF;
				 for(j=0;j<len;j++)
				 {
				 	 host_bnd_test[1+ j] = ucData;
					 ucData--;
					 if(0x0 == ucData)	ucData = 0xFF;
				 }
				break;

			case 2:
			default:
				 /* Do not cover the fist 1 bytes */
			 	 rand_data = (INT32 *)&host_bnd_test[1];
				 for(j=0;j<(len/4);j++)
				 {
				    *rand_data =  rand();
				    rand_data++;
				 }
				break;
		}

	    /* If need to log packet */
		OPL_HOST_DEBUG("\r       Packet sent from Host interface:\n"
		                "\r       eport : %d\n\t"
		                "\r       icos : %d\n\t"
		                ,ptr_pkt_hdr->eport
		                ,ptr_pkt_hdr->icos);

		 ptr_pkt_hdr->eport = iPort; /*0 GE Port; 1: Pon port. */
		 ptr_pkt_hdr->icos  = iIcos; /* ICos 0*/
		 ptr_pkt_hdr->reserved   = 0; /* reserved bits */

		 /*if need to dump packet, only dump the first 20 bytes */
	     OPL_HOST_DUMP((void *)ptr_pkt_hdr, (len +1));

	     sts = eopl_dma_send((void *)ptr_pkt_hdr, (len +1));
		 if (sts != OPL_OK)
		 {
		      OPL_HOST_DEBUG("\r Packet sent from Host interface failed!\n");
		 }
    }

}

#endif /* OPCONN_HOST_DEBUG */

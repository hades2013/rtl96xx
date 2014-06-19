
#ifdef CONFIG_RG_DEBUG

#include <rtk_rg_liteRomeDriver.h>
#include <rtk_rg_internal.h>
#include <rtk_rg_fwdEngine.h>
#include <rtk_rg_define.h>
#include <rtk_rg_struct.h>
#include <rtk_rg_debug.h>

#include <rtk/acl.h>
#include <rtk/classify.h>
#include <rtk/l2.h>
#include <rtk/l34.h>
#include <rtk/qos.h>
#include <rtk/svlan.h>

#include <linux/proc_fs.h>


#include <ioal/mem32.h>


#ifdef CONFIG_APOLLO_RLE0371
#include <dal/apollo/raw/apollo_raw_hwmisc.h>
#else
#include <dal/apollomp/raw/apollomp_raw_hwmisc.h>
#endif


#if 1

#ifdef CONFIG_APOLLO_RLE0371
#define DIAG_UTIL_CHIP_TYPE APOLLO_CHIP_ID
#else
#define DIAG_UTIL_CHIP_TYPE APOLLOMP_CHIP_ID
#endif

//#define diag_util_inet_mactoa mactoa
int8 *diag_util_inet_mactoa (const uint8 *mac)
{
        static int8 str[6*sizeof "123"];

    if (NULL == mac)
    {
        sprintf(str,"NULL");
        return str;
    }

    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return str;
} /* end of diag_util_mac2str */

/*IPv4 address to string*/
int8 *diag_util_inet_ntoa(uint32 ina)
{	
	static int8 buf[4*sizeof "123"];    
	sprintf(buf, "%d.%d.%d.%d", ((ina>>24)&0xff), ((ina>>16)&0xff), ((ina>>8)&0xff), (ina&0xff));	
	return (buf);
}


#define diag_util_printf(fmt, args...)    printk( fmt, ## args)
#include <diag_display.h>

#endif




/*internal utils*/
#define inet_ntoa_r(x,y) _inet_ntoa_r(x,y)
#define inet_ntoa(x) _inet_ntoa(x)


static inline int8 *_ui8tod( uint8 n, int8 *p )
{
	if( n > 99 ) *p++ = (n/100) + '0';
	if( n >  9 ) *p++ = ((n/10)%10) + '0';
	*p++ = (n%10) + '0';
	return p;
}

int8 *_inet_ntoa_r(ipaddr_t ipaddr, 	int8 *p)
{
	uint8 *ucp = (unsigned char *)&ipaddr;
	assert(p!=NULL);
	p = _ui8tod( ucp[0] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[1] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[2] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[3] & 0xFF, p);
	*p++ = '\0';
	return (p);
}

int8 *_inet_ntoa(rtk_ip_addr_t ina)
{
	static int8 buf[4*sizeof "123"];
	int8 *p = buf;
	uint8 *ucp = (unsigned char *)&ina;

	p = _ui8tod( ucp[0] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[1] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[2] & 0xFF, p);
	*p++ = '.';
	p = _ui8tod( ucp[3] & 0xFF, p);
	*p++ = '\0';

	return (buf);
}



/*internal function*/
void hs_displayL4Hsb_S(rtk_l34_hsb_t * hsbWatch)
{
    rtk_ip_addr_t addr;
    // rtlglue_printf("L4HSB(");
    rtlglue_printf("\tspa:%d\n",hsbWatch->spa);
    rtlglue_printf("\tda:%02x-%02x-%02x-%02x-%02x-%02x",hsbWatch->dmac.octet[0],hsbWatch->dmac.octet[1],hsbWatch->dmac.octet[2],hsbWatch->dmac.octet[3],hsbWatch->dmac.octet[4],hsbWatch->dmac.octet[5]);
    addr =ntohl( hsbWatch->sip);
#ifdef CONFIG_APOLLO_TESTING
    rtlglue_printf("\tsip:%s(hex:%08x)   ",inet_ntoa(addr),hsbWatch->sip);
#else
    rtlglue_printf("\tsip:%08x ",hsbWatch->sip);
#endif
    rtlglue_printf("\tsprt:%d (hex:%x)\n ",(int)hsbWatch->sport_icmpid_chksum,hsbWatch->sport_icmpid_chksum);
    rtlglue_printf("\tdip:");

    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[0]));
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[1]));
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[2]));
    rtlglue_printf("%02x",hsbWatch->dip.ipv6_addr[3]);
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[4]));
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[5]));
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[6]));
    rtlglue_printf("%02x",hsbWatch->dip.ipv6_addr[7]);
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[8]));
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[9]));
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[10]));
    rtlglue_printf("%02x",hsbWatch->dip.ipv6_addr[11]);
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[12]));
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[13]));
    rtlglue_printf(":");
    rtlglue_printf("%02x",(hsbWatch->dip.ipv6_addr[14]));
    rtlglue_printf("%02x",hsbWatch->dip.ipv6_addr[15]);

    rtlglue_printf("\tdprt:%d(hex:%08x)\n",hsbWatch->dport_l4chksum,hsbWatch->dport_l4chksum);

    rtlglue_printf("\ttype:%d",hsbWatch->type);

    rtlglue_printf("\tttlst:0x%x\n",hsbWatch->ttls);
    rtlglue_printf("\tpppoetagif:%d\tpppoeId:%d\n",hsbWatch->pppoe_if,hsbWatch->pppoe_id);
    rtlglue_printf("\tctagif:%d",hsbWatch->cvlan_if);
    rtlglue_printf("\tvid :%d\n",hsbWatch->cvid);
    rtlglue_printf("\tstagif:%d",hsbWatch->svlan_if);
    rtlglue_printf("\tlen:%d\n",hsbWatch->len);

    rtlglue_printf("\tudp_nocs:%d",hsbWatch->udp_no_chksum);
    rtlglue_printf("\ttcpflg:%x\n",hsbWatch->tcp_flag);
    rtlglue_printf("\tparsfail:%d",hsbWatch->parse_fail);
    rtlglue_printf("\tdirtx:%d\n",hsbWatch->cpu_direct_tx);
    rtlglue_printf("\tl3csok:%d\tl4csok:%d\n",hsbWatch->l3_chksum_ok,hsbWatch->l4_chksum_ok);
    rtlglue_printf("\tipmf:%d\tipfragif:%d\n",hsbWatch->ipmf,hsbWatch->ipfrag_s);
    rtlglue_printf("\tisFromWan:%d\n",hsbWatch->isFromWan);

//    rtlglue_printf("\tl2bridge:%d\n)\n",hsbWatch->extl2);
    rtlglue_printf("\tl2bridge:%d\n",hsbWatch->l2bridge);

}

void hs_displayL4Hsa_S(rtk_l34_hsa_t *hsaWatch)
{
    rtk_ip_addr_t addr;
//   rtlglue_printf(("L4HSA("));
    addr =ntohl( hsaWatch->ip);
#ifdef CONFIG_APOLLO_TESTING
	rtlglue_printf("\ttrip:%s(hex:%08x)",inet_ntoa(addr),hsaWatch->ip);
#else
    rtlglue_printf("\ttrip:%08x",hsaWatch->ip);
#endif
    rtlglue_printf("\tprt:%d\n",hsaWatch->port);
    rtlglue_printf("\tl3cs:0x%x",hsaWatch->l3_chksum);
    rtlglue_printf("\tl4cs:0x%x\n",hsaWatch->l4_chksum);
    rtlglue_printf("\tmacidx:%d\n",hsaWatch->nexthop_mac_idx);
    rtlglue_printf("\tpppif:%d",hsaWatch->pppoe_if);
    rtlglue_printf("\tpppid:%d\n",hsaWatch->pppid_idx);
    rtlglue_printf("\tdvid:%d(0x%x)",hsaWatch->dvid,hsaWatch->dvid);
    rtlglue_printf("\tdestination interface :%d\n",hsaWatch->difid);

    rtlglue_printf("\tfrag:%d\n",hsaWatch->frag);
    rtlglue_printf("\tpriority:%d\tvalid:%d\n",hsaWatch->l4_pri_sel,hsaWatch->l4_pri_valid);
    rtlglue_printf("\taction:%d\n",hsaWatch->action);
    rtlglue_printf("\tInternal NETIF:%d",hsaWatch->interVlanIf);
    rtlglue_printf("\tl2tr:%d",hsaWatch->l2trans);
    rtlglue_printf("\tl34tr:%d\n",hsaWatch->l34trans);
    rtlglue_printf("\tbindvidtrans:%d\n",hsaWatch->bindVidTrans);
    rtlglue_printf("\treason:%d\n",hsaWatch->reason);

//    rtlglue_printf(")\n");

}




#define COLOR_Y "\033[1;33m"
#define COLOR_NM "\033[0m"
#define COLOR_H "\033[1;37;41m"
#define COLOR_G "\033[1;32m"


extern void _diag_debug_hsb_display(rtk_hsb_t *hsaDatb);
extern void _diag_debug_hsa_display(rtk_hsa_t *hsaData);
extern void _diag_debug_hsd_display(rtk_hsa_debug_t *hsaDatd);


int32 dump_tcpudp (void)
{    
#if 1
	uint32 idx=0, entry=0, retval=0;
    rtk_l34_naptOutbound_entry_t asic_outtcpudp;
    rtk_l34_naptInbound_entry_t asic_intcpudp;
	char *napt_state[9]={"INVALID","SYN_RECV","UDP_FIRST","SYN_ACK_RECV","UDP_SECOND","TCP_CONNECTED","UDP_CONNECTED","FIN_RECV","RST_RECV"};

    rtlglue_printf(">>ASIC NAPT TCP/UDP Table:\n");
    rtlglue_printf(" ---------------- Outbound ----------------\n");
    for(idx=0; idx<MAX_NAPT_OUT_HW_TABLE_SIZE; idx++)
    {
        retval = rtk_l34_naptOutboundTable_get(idx, &asic_outtcpudp);
        if (retval == FAIL)
            continue;

        if (asic_outtcpudp.valid == 1)
        {
            rtlglue_printf("  [%4d] INIDX(%d) priValid(%d) priority(%d) - extPort(0x%x) state(%s) idle(%d)\n",
                           idx,
                           asic_outtcpudp.hashIdx,
                           asic_outtcpudp.priValid,
                           asic_outtcpudp.priValue,
                           rg_db.naptOut[idx].extPort,
                           napt_state[rg_db.naptOut[idx].state], 
                           rg_db.naptOut[idx].idleSecs
                           );
            entry++;
        }
    }

    entry=0;
    rtlglue_printf(" ---------------- Inbound ----------------\n");
    for(idx=0; idx<MAX_NAPT_OUT_HW_TABLE_SIZE; idx++)
    {
        retval = rtk_l34_naptInboundTable_get(idx, &asic_intcpudp);
        if (retval == FAIL)
            continue;

        if (asic_intcpudp.valid != 0)
        {
            rtlglue_printf("  [%4d] %d.%d.%d.%d:%d V(%d), IPIDX(%d} REMHASH(%d) EPLSB(0x%x) TCP(%d) PRI_EN(%d) PRI(%d) - remote(%d.%d.%d.%d:%d) idle(%d)\n",
                           idx,
                           asic_intcpudp.intIp>>24, (asic_intcpudp.intIp&0x00ff0000) >> 16,
                           (asic_intcpudp.intIp&0x0000ff00)>>8, asic_intcpudp.intIp&0x000000ff,
                           asic_intcpudp.intPort,
                           asic_intcpudp.valid, asic_intcpudp.extIpIdx,
                           asic_intcpudp.remHash,
                           asic_intcpudp.extPortLSB, asic_intcpudp.isTcp, asic_intcpudp.priValid, asic_intcpudp.priId,
                           rg_db.naptIn[idx].remoteIp>>24, (rg_db.naptIn[idx].remoteIp&0x00ff0000) >> 16,
                           (rg_db.naptIn[idx].remoteIp&0x0000ff00)>>8, rg_db.naptIn[idx].remoteIp&0x000000ff,
                           rg_db.naptIn[idx].remotePort,
                           rg_db.naptIn[idx].idleSecs
                           );
            entry++;
        }
    }
    rtlglue_printf("Total entry: %d\n", entry);
#endif
    return SUCCESS;
}

int32 dump_napt_entry(int outIdx,int outHashIdx)
{
	char *napt_state[9]={"INVALID","SYN_RECV","UDP_FIRST","SYN_ACK_RECV","UDP_SECOND","TCP_CONNECTED","UDP_CONNECTED","FIN_RECV","RST_RECV"};		

	if(rg_db.naptOut[outIdx].rtk_naptOut.valid == 1)
	{
		int inIdx;
		inIdx=rg_db.naptOut[outIdx].rtk_naptOut.hashIdx;
		if(rg_db.naptIn[inIdx].rtk_naptIn.valid !=0)
		{
			uint32 extip=rg_db.extip[rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx].rtk_extip.extIpAddr;
			int inHash=_rtk_rg_naptTcpUdpInHashIndex(rg_db.naptIn[inIdx].rtk_naptIn.isTcp,extip,rg_db.naptOut[outIdx].extPort);
			//rtlglue_printf("[O:%04d(%03d),I:%04d(%03d)] [%s %03d.%03d.%03d.%03d:%05d<->%03d.%03d.%03d.%03d:%05d<->%03d.%03d.%03d.%03d:%05d] priEnOut(%d) priOut(%d) V(%d) IPIDX(%d) REMHASH(%d) EPLSB(0x%x) priEnIn(%d) priIn(%d)\n",
			rtlglue_printf("[O:%04d(%03d),I:%04d(%03d)] %s %03d.%03d.%03d.%03d:%05d[PRI:%d(%s)] <-> %03d.%03d.%03d.%03d:%05d(%d) <-> %03d.%03d.%03d.%03d:%05d[PRI:%d(%s)] %s IDLE:%d\n",				
						   outIdx,outHashIdx,
						   inIdx,inHash,						   
						   (rg_db.naptIn[inIdx].rtk_naptIn.isTcp==1)?"TCP":"UDP", 
						   rg_db.naptIn[inIdx].rtk_naptIn.intIp>>24, (rg_db.naptIn[inIdx].rtk_naptIn.intIp&0x00ff0000) >> 16,
						   (rg_db.naptIn[inIdx].rtk_naptIn.intIp&0x0000ff00)>>8, rg_db.naptIn[inIdx].rtk_naptIn.intIp&0x000000ff,
						   rg_db.naptIn[inIdx].rtk_naptIn.intPort,
						   rg_db.naptIn[inIdx].rtk_naptIn.priId,
						   (rg_db.naptIn[inIdx].rtk_naptIn.priValid==1)?"on":"off",
							extip>>24,(extip&0x00ff0000) >> 16,
							(extip&0x0000ff00)>>8, extip&0x000000ff,
						   rg_db.naptOut[outIdx].extPort,
							(rg_db.naptIn[inIdx].rtk_naptIn.isTcp==1)?rg_db.naptTcpExternPortUsedRefCount[rg_db.naptOut[outIdx].extPort]:rg_db.naptUdpExternPortUsedRefCount[rg_db.naptOut[outIdx].extPort],
						   rg_db.naptIn[inIdx].remoteIp>>24,(rg_db.naptIn[inIdx].remoteIp&0x00ff0000) >> 16,
						   (rg_db.naptIn[inIdx].remoteIp&0x0000ff00)>>8, rg_db.naptIn[inIdx].remoteIp&0x000000ff,
						   rg_db.naptIn[inIdx].remotePort,
							rg_db.naptOut[outIdx].rtk_naptOut.priValue,
							(rg_db.naptOut[outIdx].rtk_naptOut.priValid==1)?"on":"off",
						   
						   napt_state[rg_db.naptOut[outIdx].state], 
						   rg_db.naptOut[outIdx].idleSecs
						   /*
						   rg_db.naptOut[outIdx].rtk_naptOut.priValid, rg_db.naptOut[idx].rtk_naptOut.priValue,
						   rg_db.naptIn[inIdx].rtk_naptIn.valid,
						   rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx,
						   rg_db.naptIn[inIdx].rtk_naptIn.remHash,
						   rg_db.naptIn[inIdx].rtk_naptIn.extPortLSB,							   
						   rg_db.naptIn[inIdx].rtk_naptIn.priValid, rg_db.naptIn[inIdx].rtk_naptIn.priId
						   */
						   );
		}
	
	}
	return SUCCESS;
}

int32 dump_napt(void)
{
    uint32 idx=0;
	rtk_rg_table_naptOut_linkList_t	*pNaptOutLinkList;
	
    rtlglue_printf(">>ASIC NAPT OUT TCP/UDP Table:\n");
    for(idx=0; idx<MAX_NAPT_OUT_HW_TABLE_SIZE; idx++)
    {
		int outIdx=idx;
		dump_napt_entry(outIdx,outIdx>>2);
    }
	
	rtlglue_printf(">>SOFTWARE LINK LIST NAPT OUT TCP/UDP Table:\n");

	for(idx=0;idx<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;idx++)
	{
		pNaptOutLinkList=rg_db.pNaptOutHashListHead[idx];
		while(pNaptOutLinkList!=NULL)
		{
			dump_napt_entry(pNaptOutLinkList->idx,idx);
			pNaptOutLinkList=pNaptOutLinkList->pNext;
		}
	}

    return SUCCESS;
}

int32 mibdump_frag(void)
{
    uint32 idx=0;
	rtk_rg_ipv4_fragment_out_t *pFragOut;
	rtk_rg_ipv4_fragment_in_t *pFragIn;
	
    rtlglue_printf(">>SOFTWARE Fragment Link-List:\n");

	//count free out list number
	idx=0;
	pFragOut=rg_db.pFragOutFreeListHead;
	while(pFragOut!=NULL)
	{
		idx++;
		pFragOut=pFragOut->pNext;
	}
	rtlglue_printf("free Frag Out list has %d elements\n",idx);

	//display used hash head
	for(idx=0;idx<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;idx++)
	{
		if(rg_db.pFragOutHashListHead[idx]!=NULL)
		{
			rtlglue_printf("pFragOutHashListHead[%d]->%p(%s,idle:%ld,act:%d)",idx,rg_db.pFragOutHashListHead[idx],
				rg_db.pFragOutHashListHead[idx]->layer4Type&ICMP_TAGIF?"ICMP":"NAPT",
				(jiffies-rg_db.pFragOutHashListHead[idx]->beginIdleTime)/100,rg_db.pFragOutHashListHead[idx]->fragAction);
			//travel all list
			pFragOut=rg_db.pFragOutHashListHead[idx]->pNext;
			while(pFragOut!=NULL)
			{
				rtlglue_printf("->%p(%s,idle:%ld,act:%d)",pFragOut,pFragOut->layer4Type&ICMP_TAGIF?"ICMP":"NAPT",
					(jiffies-pFragOut->beginIdleTime)/100,rg_db.pFragOutHashListHead[idx]->fragAction);
				pFragOut=pFragOut->pNext;
			}
			rtlglue_printf("->NULL\n");
		}
	}

	rtlglue_printf("===================================\n");

	//count free in list number
	idx=0;
	pFragIn=rg_db.pFragInFreeListHead;
	while(pFragIn!=NULL)
	{
		idx++;
		pFragIn=pFragIn->pNext;
	}
	rtlglue_printf("free Frag In list has %d elements\n",idx);

	//display used hash head
	for(idx=0;idx<MAX_NAPT_IN_HW_TABLE_SIZE>>2;idx++)
	{
		if(rg_db.pFragInHashListHead[idx]!=NULL)
		{
			rtlglue_printf("pFragInHashListHead[%d]->%p(%s,idle:%ld,act:%d)",idx,rg_db.pFragInHashListHead[idx],
				rg_db.pFragInHashListHead[idx]->layer4Type&ICMP_TAGIF?"ICMP":"NAPT",
				(jiffies-rg_db.pFragInHashListHead[idx]->beginIdleTime)/100,rg_db.pFragInHashListHead[idx]->fragAction);
			//travel all list
			pFragIn=rg_db.pFragInHashListHead[idx]->pNext;
			while(pFragIn!=NULL)
			{
				rtlglue_printf("->%p(%s,idle:%ld,act:%d)",pFragIn,pFragIn->layer4Type&ICMP_TAGIF?"ICMP":"NAPT",
					(jiffies-pFragIn->beginIdleTime)/100,rg_db.pFragInHashListHead[idx]->fragAction);
				pFragIn=pFragIn->pNext;
			}
			rtlglue_printf("->NULL\n");
		}
	}
	
    return SUCCESS;
}


int32 dump_netif (void)
{ 
#if 1
	uint8	*mac;
    int32	i, j;
    rtk_portmask_t mbr,untag;

    rtlglue_printf(">>ASIC Netif Table:\n\n");

    for(i=0; i<MAX_NETIF_HW_TABLE_SIZE; i++)
    {
    	rtk_l34_netif_entry_t intf;
        int retval  = 0;
		memset(&intf,0,sizeof(rtk_l34_netif_entry_t));
		retval = rtk_l34_netifTable_get(i,&intf);
        if (retval == FAIL)
            continue;

        if (intf.valid)
        {
            mac = (uint8 *)&intf.gateway_mac.octet[0];
            rtlglue_printf("  [%d]-vid[%d] %02x:%02x:%02x:%02x:%02x:%02x",
                           i, intf.vlan_id, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            rtlglue_printf("  L3/4 HW acc %s ",
                           intf.enable_rounting==TRUE? "enabled": "disabled" );
            rtlglue_printf("\n      %d MAC Addresses, MTU %d Bytes", intf.mac_mask, intf.mtu);
            mbr.bits[0]=0;
            untag.bits[0]=0;
            retval = rtk_vlan_port_get(intf.vlan_id,&mbr,&untag);
            ASSERT_EQ(retval,RT_ERR_OK);
            rtlglue_printf("\n		Untag member ports:");
            for(j=0; j<RTK_RG_MAX_MAC_PORT; j++)
                if(untag.bits[0] & (1<<j))
                    rtlglue_printf("%d ", j);
            rtlglue_printf("\n		Active member ports:");
            for(j=0; j<RTK_RG_MAX_MAC_PORT; j++)
                if(mbr.bits[0] & (1<<j))
                    rtlglue_printf("%d ", j);
            mbr.bits[0]=0;
            retval = rtk_vlan_extPort_get(intf.vlan_id,&mbr);
            ASSERT_EQ(retval,RT_ERR_OK);
            for(j=1; j<RTK_RG_MAX_EXT_PORT; j++)
                if(mbr.bits[0] & (1<<j))
                    rtlglue_printf("EXT%d ", j);
            rtlglue_printf("\n\n");
        }
    }
#endif

    return SUCCESS;
}
int32 dump_pppoe (void)
{
	rtk_l34_pppoe_entry_t pppEntry;
	int32	i=0,retval=0;

	rtlglue_printf(">>PPPOE Table:\n");
	for(i=0; i<MAX_PPPOE_HW_TABLE_SIZE; i++)
	{
		memset(&pppEntry,0,sizeof(rtk_l34_pppoe_entry_t));
		retval = rtk_l34_pppoeTable_get(i,  &pppEntry);
		if (retval == FAIL)
			continue;
		rtlglue_printf("  [%d]	sessionID(%d)\n", i, pppEntry.sessionID);
	}

#if 0
    rtl865x_tblAsicDrv_pppoeParam_t asic_pppoe;
    int32	i=0,retval=0;

    rtlglue_printf(">>PPPOE Table:\n");
    for(i=0; i<MAX_PPPOE_HW_TABLE_SIZE; i++)
    {
        memset(&asic_pppoe,0,sizeof(asic_pppoe));
        retval = apollo_getAsicPppoe(i,  &asic_pppoe);
        if (retval == FAILED)
            continue;
        rtlglue_printf("  [%d]  sessionID(%d)  ageSec(%d)\n", i, asic_pppoe.sessionId, asic_pppoe.age);
    }
#endif
    return SUCCESS;
}
int32 dump_nexthop (void)
{
	rtk_l34_nexthop_entry_t nextHopEntry;
	uint32 idx, refcnt, rt_flag, retval;

	uint8	isPppoe=0;
	uint8  	dvid=0;
	uint8  	pppoeIdx=0;
	uint16 	nextHopRow=0;
	uint16 	nextHopColumn=0;
	uint8  	pppoekeep=0;
	uint8 	extIntIpIdx=0;


	rtlglue_printf(">>ASIC Next Hop Table:\n");

	for(idx=0; idx<MAX_NEXTHOP_HW_TABLE_SIZE; idx++)
	{
		refcnt = rt_flag = 0;
		retval = rtk_l34_nexthopTable_get(idx, &nextHopEntry);
		ASSERT_EQ(retval,RT_ERR_OK);


		dvid = nextHopEntry.ifIdx;
		nextHopRow = nextHopEntry.nhIdx>>2;
		nextHopColumn = nextHopEntry.nhIdx&0x3;
		pppoeIdx = nextHopEntry.pppoeIdx;
		isPppoe = nextHopEntry.type;
		pppoekeep = nextHopEntry.keepPppoe;

		if((nextHopRow==0) && (nextHopColumn==0) && (isPppoe==0) && (extIntIpIdx==0) && (dvid==0) && (pppoeIdx==0)) 
			continue;

		rtlglue_printf("  [%d]\ttype(%s%s) IFIdx(%d) pppoeIdx(%d) nextHop(%d,%d)\n", idx,
					   (isPppoe==TRUE? "pppoe": "ethernet"),(pppoekeep?", Keep":", Non-Keep"),
					   dvid, pppoeIdx, (nextHopRow),nextHopColumn);

	}

#if 0
    rtl865x_tblAsicDrv_nextHopParam_t asic_nxthop;

    uint32 idx, refcnt, rt_flag, retval;

    rtlglue_printf(">>ASIC Next Hop Table:\n");

    for(idx=0; idx<MAX_NEXTHOP_HW_TABLE_SIZE; idx++)
    {
        refcnt = rt_flag = 0;
        retval = apollo_getAsicNextHopTable(idx, &asic_nxthop);
        ASSERT_EQ(retval,RT_ERR_OK);


        if((asic_nxthop.nextHopRow==0) && (asic_nxthop.nextHopColumn==0) && (asic_nxthop.isPppoe==0) && (asic_nxthop.extIntIpIdx==0) && (asic_nxthop.dvid==0) && (asic_nxthop.pppoeIdx==0)) continue;

        rtlglue_printf("  [%d]\ttype(%s%s) IFIdx(%d) pppoeIdx(%d) nextHop(%d,%d)\n", idx,
                       (asic_nxthop.isPppoe==TRUE? "pppoe": "ethernet"),(asic_nxthop.pppoekeep?", Keep":", Non-Keep"),
                       asic_nxthop.dvid, asic_nxthop.pppoeIdx, (asic_nxthop.nextHopRow),asic_nxthop.nextHopColumn);

    }
#endif
    return SUCCESS;
}
int32 dump_arp (void)
{
#if 1
	rtk_l34_routing_entry_t asic_l3;
	rtk_l34_arp_entry_t asic_arp;
	rtk_rg_arp_linkList_t *pSoftwareArp;

	uint32	i, j, retval;
	ipaddr_t ipAddr;
	int8 ipBuf[sizeof"255.255.255.255"];

	rtlglue_printf(">>Arp Table:\n");
	for(i=0; i<MAX_ARP_HW_TABLE_SIZE; i++)
	{
		retval = rtk_l34_arpTable_get(i,  &asic_arp);
		if ((retval == FAIL) || asic_arp.valid!=1)
			continue;
		{
			for(j=0; j<MAX_L3_HW_TABLE_SIZE; j++)
			{
				memset(&asic_l3,0,sizeof(rtk_l34_arp_entry_t));
				retval = rtk_l34_routingTable_get(j, &asic_l3);
				if ( retval == FAIL || asic_l3.process!= 2 || asic_l3.valid!=1)
					continue;
				if(asic_l3.arpStart <= (i>>2) &&  (i>>2) <= asic_l3.arpEnd)
				{
					ipAddr= (i-(asic_l3.arpStart<<2))+(asic_l3.ipAddr & (~((1<<(31-asic_l3.ipMask))-1)));			
					inet_ntoa_r(ntohl(ipAddr), ipBuf);
					if(rg_db.arp[i].staticEntry)
						rtlglue_printf("  [%3d] : %-16s (STATIC) : ",i, ipBuf);
					else
						rtlglue_printf("  [%3d] : %-16s (DYNAMIC) : ",i, ipBuf);
					rtlglue_printf(" -> L2:%d (Idle %d Secs)\n", asic_arp.nhIdx,rg_db.arp[i].idleSecs);
					break;
				}
			}
		}

	}

#if 0
	rtlglue_printf(">> ARP valid:\n");
	for(i=0;i<=(512/32);i++) rtlglue_printf("[%08x]",rg_db.arpValidSet[i]);
	rtlglue_printf("\n");
#endif
	
	rtlglue_printf(">>Arp Software Table:\n");
	for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
	{
		if(!list_empty(&rg_db.softwareArpTableHead[i]))
		{
			rtlglue_printf("  [%3d] : ",i);
			list_for_each_entry(pSoftwareArp,&rg_db.softwareArpTableHead[i],arp_list)
			{
				inet_ntoa_r(ntohl(rg_db.arp[pSoftwareArp->idx].ipv4Addr), ipBuf);
				rtlglue_printf("->%-16s([%d],l2:%d,idle:%d%s) ",ipBuf,pSoftwareArp->idx,rg_db.arp[pSoftwareArp->idx].rtk_arp.nhIdx,rg_db.arp[pSoftwareArp->idx].idleSecs,rg_db.arp[pSoftwareArp->idx].staticEntry==1?",S":"");
			}
			rtlglue_printf("\n");
		}
	}
#endif
    return SUCCESS;
}
int32 dump_ip (void)
{
#if 1
	rtk_l34_ext_intip_entry_t asic_ip;
    int32	i;
    int8 intIpBuf[sizeof"255.255.255.255"];
    int8 extIpBuf[sizeof"255.255.255.255"];

    rtlglue_printf(">>IP Table:\n");
    for(i=0; i<MAX_EXTIP_HW_TABLE_SIZE; i++)
    {
        int retval = 0;
		memset(&asic_ip,0,sizeof(rtk_l34_ext_intip_entry_t));
		retval = rtk_l34_extIntIPTable_get(i,&asic_ip);
        if ( retval != RT_ERR_OK)
        {
            rtlglue_printf("  [%d] (Invalid) --> %x.\n", i,retval);
            continue;
        }
        else if (asic_ip.valid==0)
        {
            rtlglue_printf("  [%d] (Invalid)\n", i);
            continue;
        }
        else
        {
            inet_ntoa_r(ntohl(asic_ip.intIpAddr), intIpBuf);
            inet_ntoa_r(ntohl(asic_ip.extIpAddr),extIpBuf);
            rtlglue_printf("  [%d] intip(%-14s) extip(%-14s) type(%s) nhIdx(%d) PriValid(%d) Priority(%d)\n",
                           i, intIpBuf,extIpBuf,
                           ((asic_ip.type&0x2)!=0? "LP" : ((asic_ip.type&0x1)!=0 ? "NAT" : "NAPT")), asic_ip.nhIdx, asic_ip.prival, asic_ip.pri);
        }
    }
#endif
    return SUCCESS;
}
int32 dump_l3 (void)
{

#if 1
	rtk_l34_routing_entry_t asic_l3;
	uint32 idx, retval;
	int netIdx;
	int8 *str[4] = { "CPU", "DROP", "ARP", "NxtHop"};
	int8 *nhstr[3] = {"PER-PACKET","PER-SESSION","PER-SIP"};
	int8 *strNetType[2] = { "WAN", "LAN"};
#ifdef CONFIG_APOLLO_RLE0371
#else
	int8 *strRouteType[2] = { "RTLAN", "RTWAN"};
#endif


	rtlglue_printf(">>L3 Routing Table:\n");
	for(idx=0; idx<MAX_L3_HW_TABLE_SIZE; idx++)
	{
	    retval = rtk_l34_routingTable_get(idx, &asic_l3);
	    if ( retval != RT_ERR_OK)
	    {
	        rtlglue_printf("[%d]  (Invalid)\n", idx);
	        continue;
	    }
	    else
	    {
	        netIdx = asic_l3.internal;
			if(asic_l3.ipAddr!=0) asic_l3.ipMask++;
#ifdef CONFIG_APOLLO_RLE0371
			if(asic_l3.valid==1)
			{
				rtlglue_printf("[%d] Valid %d.%d.%d.%d/%d %s\n", idx, (asic_l3.ipAddr>>24)&0xff,
				(asic_l3.ipAddr>>16)&0xff, (asic_l3.ipAddr>>8)&0xff, (asic_l3.ipAddr&0xff),
				asic_l3.ipMask,strNetType[netIdx]);
			}
			else
			{
				rtlglue_printf("[%d]  (Invalid)\n", idx);
	        	continue;
			}
#else
			if(asic_l3.valid==1)
			{
		        rtlglue_printf("[%d] Valid %d.%d.%d.%d/%d %s %s\n", idx, (asic_l3.ipAddr>>24)&0xff,
		        (asic_l3.ipAddr>>16)&0xff, (asic_l3.ipAddr>>8)&0xff, (asic_l3.ipAddr&0xff),
		        asic_l3.ipMask,strNetType[netIdx],strRouteType[asic_l3.rt2waninf]);
			}
			else
			{
				rtlglue_printf("[%d]  (Invalid)\n", idx);
	        	continue;
			}

#endif
		    
		    switch(asic_l3.process)
		    {
		    case 0x00:	/* CPU */
		        rtlglue_printf("            [%s PROCESS] \n\n", str[asic_l3.process]);
		        break;

		    case 0x01:	/* DROP */
		        rtlglue_printf("            [%s PROCESS] \n\n", str[asic_l3.process]);
		        break;

		    case 0x02:	/* ARP */
		        rtlglue_printf("            [%s PROCESS]: NETIF(%d) ARPSTA(%d) ARPEND(%d) \n\n", str[asic_l3.process],asic_l3.netifIdx,asic_l3.arpStart, asic_l3.arpEnd);
		        break;

		    case 0x03:	/* NextHop */
		        rtlglue_printf("            [%s PROCESS]: NHSTA(%d) NHNUM(%d) NHNXT(%d) NHALGO(%s) IPDOMAIN(%d)\n\n", str[asic_l3.process],asic_l3.nhStart,
		                       asic_l3.nhNum, asic_l3.nhNxt, nhstr[asic_l3.nhAlgo], asic_l3.ipDomain);
		        break;
		    default:
		        assert(0);
		    }
		}
	}
#endif 
 
    return SUCCESS;
}
int32 dump_l4hs (void)
{
	int retval = 0;
	rtk_l34_hsb_t hsb_r;
	rtk_l34_hsa_t hsa_r;
	memset(&hsb_r,0,sizeof(rtk_l34_hsb_t));
	memset(&hsa_r,0,sizeof(rtk_l34_hsa_t));
	retval = rtk_l34_hsbData_get(&hsb_r);
	ASSERT_EQ(retval,RT_ERR_OK);
	retval = rtk_l34_hsaData_get(&hsa_r);
	ASSERT_EQ(retval,RT_ERR_OK);

	rtlglue_printf("---- "COLOR_Y "[L4HSB:]" COLOR_NM "------------------------------------\n");
	hs_displayL4Hsb_S(&hsb_r);
	rtlglue_printf("----------------------------------------------\n");
	rtlglue_printf("---- "COLOR_Y "[L4HSA:]" COLOR_NM "------------------------------------\n");
	hs_displayL4Hsa_S(&hsa_r);
	rtlglue_printf("----------------------------------------------\n");
	
#if 0
	l4_hsb_param_t hsb_r;
	l4_hsa_param_t hsa_r;
	memset((void*)&hsb_r,0,sizeof(hsb_r));
	memset((void*)&hsa_r,0,sizeof(hsa_r));

	apollo_virtualMacGetL4Hsb( &hsb_r );
	apollo_virtualMacGetL4Hsa( &hsa_r );

	rtlglue_printf("---- "COLOR_Y "[L4HSB:]" COLOR_NM "------------------------------------\n");
	hs_displayL4Hsb_S(&hsb_r);
	rtlglue_printf("----------------------------------------------\n");
	rtlglue_printf("---- "COLOR_Y "[L4HSA:]" COLOR_NM "------------------------------------\n");
	hs_displayL4Hsa_S(&hsa_r);
	rtlglue_printf("----------------------------------------------\n");
#endif
    return SUCCESS;
}
int32 dump_hs (void)
{
#if 1
    int32 ret=0;
    rtk_hsb_t rawHsb;
    rtk_hsa_t rawHsa;
    rtk_hsa_debug_t rawHsd;
    memset((void*)&rawHsb,0,sizeof(rawHsb));
    memset((void*)&rawHsa,0,sizeof(rawHsa));
    memset((void*)&rawHsd,0,sizeof(rawHsd));

#ifdef CONFIG_APOLLO_RLE0371
    ret = apollo_raw_hsbData_get(&rawHsb);
    ASSERT_EQ(ret,RT_ERR_OK);
    //convertAsicHsbToSoftware(&rawHsb,&hsb_r);
    ret = apollo_raw_hsaData_get(&rawHsa);
    ASSERT_EQ(ret,RT_ERR_OK);
    //convertAsicHsaToSoftware(&rawHsa,&hsa_r);
    ret = apollo_raw_hsdData_get(&rawHsd);
    ASSERT_EQ(ret,RT_ERR_OK);
#else
    ret = apollomp_raw_hsbData_get(&rawHsb);
    ASSERT_EQ(ret,RT_ERR_OK);
    //convertAsicHsbToSoftware(&rawHsb,&hsb_r);
    ret = apollomp_raw_hsaData_get(&rawHsa);
    ASSERT_EQ(ret,RT_ERR_OK);
    //convertAsicHsaToSoftware(&rawHsa,&hsa_r);
    ret = apollomp_raw_hsdData_get(&rawHsd);
    ASSERT_EQ(ret,RT_ERR_OK);
#endif
    rtlglue_printf("---- "COLOR_Y "[HSB:]" COLOR_NM "------------------------------------\n");
    _diag_debug_hsb_display(&rawHsb);
    rtlglue_printf("----------------------------------------------\n");
    rtlglue_printf("---- "COLOR_Y "[HSA:]" COLOR_NM "------------------------------------\n");
    _diag_debug_hsa_display(&rawHsa);
    rtlglue_printf("----------------------------------------------\n");
    rtlglue_printf("---- "COLOR_Y "[HSD:]" COLOR_NM "------------------------------------\n");
    _diag_debug_hsd_display(&rawHsd);
    rtlglue_printf("----------------------------------------------\n");
#endif
    return SUCCESS;

}
int32 dump_lut(int idx)
{
    rtk_l2_addr_table_t	data;
    int in_idx=idx;
    int out_idx=in_idx;
    int ret;
    ret=rtk_l2_nextValidEntry_get(&out_idx,&data);
    if(ret!=RT_ERR_OK) rtlglue_printf("rtk_l2_nextValidEntry_get Error\n");

    if(in_idx==out_idx)
    {
        rtlglue_printf("--------------- LUT TABLE (%d)----------------\n",idx);
        rtlglue_printf("LUT idx=%d\n",out_idx);
        if(data.entryType==RTK_LUT_L2UC)
        {
            rtlglue_printf("[P1] mac=%02x:%02x:%02x:%02x:%02x:%02x cvid=%d l3lookup=%d ivl=%d\n"
                           ,data.entry.l2UcEntry.mac.octet[0]
                           ,data.entry.l2UcEntry.mac.octet[1]
                           ,data.entry.l2UcEntry.mac.octet[2]
                           ,data.entry.l2UcEntry.mac.octet[3]
                           ,data.entry.l2UcEntry.mac.octet[4]
                           ,data.entry.l2UcEntry.mac.octet[5]
                           ,data.entry.l2UcEntry.vid
                           ,0
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)?1:0);

            rtlglue_printf("efid=%d fid=%d sapri_en=%d spa=%d age=%d auth1x=%d sablock=%d\n"
                           ,data.entry.l2UcEntry.efid
                           ,data.entry.l2UcEntry.fid
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_FWD_PRI)?1:0
                           ,data.entry.l2UcEntry.port
                           ,data.entry.l2UcEntry.age
                           ,data.entry.l2UcEntry.auth
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0);

            rtlglue_printf("dablock=%d ext_spa=%d arp_used=%d lutpri_en=%d lutpri=%d fwdpri_en=%d notsalearn=%d \n"
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0
                           ,data.entry.l2UcEntry.ext_port
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)?1:0
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_LOOKUP_PRI)?1:0
                           ,data.entry.l2UcEntry.priority
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_FWD_PRI)?1:0
                           ,(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0);

        }
        else if(data.entryType==RTK_LUT_L2MC)
        {
            rtlglue_printf("[P2] mac=%02x:%02x:%02x:%02x:%02x:%02x ivl=%d vid=%d fid=%d l3lookup=%d ivl=%d\n"
                           ,data.entry.l2McEntry.mac.octet[0]
                           ,data.entry.l2McEntry.mac.octet[1]
                           ,data.entry.l2McEntry.mac.octet[2]
                           ,data.entry.l2McEntry.mac.octet[3]
                           ,data.entry.l2McEntry.mac.octet[4]
                           ,data.entry.l2McEntry.mac.octet[5]
                           ,(data.entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_IVL)?1:0
                           ,data.entry.l2McEntry.vid
                           ,data.entry.l2McEntry.fid
                           ,0
                           ,(data.entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_IVL)?1:0);

            rtlglue_printf("mbr=0x%x extmbr=0x%x\n"
                           ,data.entry.l2McEntry.portmask.bits[0]
                           ,data.entry.l2McEntry.ext_portmask.bits[0]);

            rtlglue_printf("lutpri=%d fwdpri_en=%d\n"
                           ,data.entry.l2McEntry.priority
                           ,(data.entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_FWD_PRI)?1:0);

        }
        else if(data.entryType==RTK_LUT_L3MC)
        {

            if(!(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_DIP_ONLY))
            {
                rtlglue_printf("[P3] gip=%d.%d.%d.%d\n",((data.entry.ipmcEntry.dip>>24)&0xff)|0xe0,(data.entry.ipmcEntry.dip>>16)&0xff,(data.entry.ipmcEntry.dip>>8)&0xff,(data.entry.ipmcEntry.dip)&0xff);
                rtlglue_printf("sip=%d.%d.%d.%d vid=%d\n",(data.entry.ipmcEntry.sip>>24)&0xff,(data.entry.ipmcEntry.sip>>16)&0xff,(data.entry.ipmcEntry.sip>>8)&0xff,(data.entry.ipmcEntry.sip)&0xff,data.entry.ipmcEntry.vid);
                rtlglue_printf("mbr=0x%x extmbr=0x%x\n",data.entry.ipmcEntry.portmask.bits[0],data.entry.ipmcEntry.ext_portmask.bits[0]);
                rtlglue_printf("lutpri=%d fwdpri_en=%d\n"
                               ,data.entry.ipmcEntry.priority
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FWD_PRI)?1:0);
            }
            else
            {
                rtlglue_printf("[P4] gip=%d.%d.%d.%d\n",((data.entry.ipmcEntry.dip>>24)&0xff)|0xe0,(data.entry.ipmcEntry.dip>>16)&0xff,(data.entry.ipmcEntry.dip>>8)&0xff,(data.entry.ipmcEntry.dip)&0xff);
                rtlglue_printf("mbr=0x%x extmbr=0x%x l3trans=0x%x\n",data.entry.ipmcEntry.portmask.bits[0],data.entry.ipmcEntry.ext_portmask.bits[0],data.entry.ipmcEntry.l3_trans_index);
                rtlglue_printf("lutpri=%d fwdpri_en=%d dip_only=%d ext_fr=%d wan_sa=%d notsalearn=%d\n"
                               ,data.entry.ipmcEntry.priority
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FWD_PRI)?1:0
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_DIP_ONLY)?1:0
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE)?1:0
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN)?1:0
                               ,(data.entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_STATIC)?1:0
                              );
            }

        }
    }


	return SUCCESS;
}
int32 dump_lut_table (void)
{
    int i;
    for(i=0; i<MAX_LUT_HW_TABLE_SIZE; i++)
    {
        dump_lut(i);
    }
    return SUCCESS;
}
int32 mibdump_lut(int idx)
{
    rtk_l2_addr_table_t	*pdata;

    if(rg_db.lut[idx].valid==1)
    {
    	pdata=&rg_db.lut[idx].rtk_lut;
        rtlglue_printf("--------------- LUT TABLE (%d)----------------\n",idx);
        rtlglue_printf("LUT idx=%d\n",idx);
        if(pdata->entryType==RTK_LUT_L2UC)
        {
            rtlglue_printf("[P1] mac=%02x:%02x:%02x:%02x:%02x:%02x cvid=%d l3lookup=%d ivl=%d\n"
                           ,pdata->entry.l2UcEntry.mac.octet[0]
                           ,pdata->entry.l2UcEntry.mac.octet[1]
                           ,pdata->entry.l2UcEntry.mac.octet[2]
                           ,pdata->entry.l2UcEntry.mac.octet[3]
                           ,pdata->entry.l2UcEntry.mac.octet[4]
                           ,pdata->entry.l2UcEntry.mac.octet[5]
                           ,pdata->entry.l2UcEntry.vid
                           ,0
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)?1:0);

            rtlglue_printf("efid=%d fid=%d sapri_en=%d spa=%d age=%d auth1x=%d sablock=%d\n"
                           ,pdata->entry.l2UcEntry.efid
                           ,pdata->entry.l2UcEntry.fid
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_FWD_PRI)?1:0
                           ,pdata->entry.l2UcEntry.port
                           ,pdata->entry.l2UcEntry.age
                           ,pdata->entry.l2UcEntry.auth
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0);

            rtlglue_printf("dablock=%d ext_spa=%d arp_used=%d lutpri_en=%d lutpri=%d fwdpri_en=%d notsalearn=%d \n"
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0
                           ,pdata->entry.l2UcEntry.ext_port
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)?1:0
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_LOOKUP_PRI)?1:0
                           ,pdata->entry.l2UcEntry.priority
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_FWD_PRI)?1:0
                           ,(pdata->entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0);

        }
        else if(pdata->entryType==RTK_LUT_L2MC)
        {
            rtlglue_printf("[P2] mac=%02x:%02x:%02x:%02x:%02x:%02x ivl=%d vid=%d fid=%d l3lookup=%d ivl=%d\n"
                           ,pdata->entry.l2McEntry.mac.octet[0]
                           ,pdata->entry.l2McEntry.mac.octet[1]
                           ,pdata->entry.l2McEntry.mac.octet[2]
                           ,pdata->entry.l2McEntry.mac.octet[3]
                           ,pdata->entry.l2McEntry.mac.octet[4]
                           ,pdata->entry.l2McEntry.mac.octet[5]
                           ,(pdata->entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_IVL)?1:0
                           ,pdata->entry.l2McEntry.vid
                           ,pdata->entry.l2McEntry.fid
                           ,0
                           ,(pdata->entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_IVL)?1:0);

            rtlglue_printf("mbr=0x%x extmbr=0x%x\n"
                           ,pdata->entry.l2McEntry.portmask.bits[0]
                           ,pdata->entry.l2McEntry.ext_portmask.bits[0]);

            rtlglue_printf("lutpri=%d fwdpri_en=%d\n"
                           ,pdata->entry.l2McEntry.priority
                           ,(pdata->entry.l2McEntry.flags&RTK_L2_MCAST_FLAG_FWD_PRI)?1:0);

        }
        else if(pdata->entryType==RTK_LUT_L3MC)
        {

            if(!(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_DIP_ONLY))
            {
                rtlglue_printf("[P3] gip=%d.%d.%d.%d\n",((pdata->entry.ipmcEntry.dip>>24)&0xff)|0xe0,(pdata->entry.ipmcEntry.dip>>16)&0xff,(pdata->entry.ipmcEntry.dip>>8)&0xff,(pdata->entry.ipmcEntry.dip)&0xff);
                rtlglue_printf("sip=%d.%d.%d.%d vid=%d\n",(pdata->entry.ipmcEntry.sip>>24)&0xff,(pdata->entry.ipmcEntry.sip>>16)&0xff,(pdata->entry.ipmcEntry.sip>>8)&0xff,(pdata->entry.ipmcEntry.sip)&0xff,pdata->entry.ipmcEntry.vid);
                rtlglue_printf("mbr=0x%x extmbr=0x%x\n",pdata->entry.ipmcEntry.portmask.bits[0],pdata->entry.ipmcEntry.ext_portmask.bits[0]);
                rtlglue_printf("lutpri=%d fwdpri_en=%d\n"
                               ,pdata->entry.ipmcEntry.priority
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FWD_PRI)?1:0);
            }
            else
            {
                rtlglue_printf("[P4] gip=%d.%d.%d.%d\n",((pdata->entry.ipmcEntry.dip>>24)&0xff)|0xe0,(pdata->entry.ipmcEntry.dip>>16)&0xff,(pdata->entry.ipmcEntry.dip>>8)&0xff,(pdata->entry.ipmcEntry.dip)&0xff);
                rtlglue_printf("mbr=0x%x extmbr=0x%x l3trans=0x%x\n",pdata->entry.ipmcEntry.portmask.bits[0],pdata->entry.ipmcEntry.ext_portmask.bits[0],pdata->entry.ipmcEntry.l3_trans_index);
                rtlglue_printf("lutpri=%d fwdpri_en=%d dip_only=%d ext_fr=%d wan_sa=%d notsalearn=%d\n"
                               ,pdata->entry.ipmcEntry.priority
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FWD_PRI)?1:0
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_DIP_ONLY)?1:0
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE)?1:0
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN)?1:0
                               ,(pdata->entry.ipmcEntry.flags&RTK_L2_IPMCAST_FLAG_STATIC)?1:0
                              );
            }

        }
    }


	return SUCCESS;
}
int32 mibdump_lut_table (void)
{
    int i;
    for(i=0; i<MAX_LUT_HW_TABLE_SIZE; i++)
    {
        mibdump_lut(i);
    }
    return SUCCESS;
}

int32 dump_ipmc_group (void)
{
	int i;
    rtlglue_printf("GIP\tPMSK\n");

	for(i=0;i<MAX_IPMCGRP_HW_TABLE_SIZE;i++)
	{
		if(rg_db.ipmcgrp[i].valid==TRUE)
		{
			rtlglue_printf("[%02d] %d.%d.%d.%d\t0x%x\n",i,(rg_db.ipmcgrp[i].groupIp>>24)&0xff
				,(rg_db.ipmcgrp[i].groupIp>>16)&0xff
				,(rg_db.ipmcgrp[i].groupIp>>8)&0xff
				,(rg_db.ipmcgrp[i].groupIp)&0xff
				,rg_db.ipmcgrp[i].portMsk.bits[0]);
		}		
	}

    return SUCCESS;
}
int32 dump_ipmc_routing (void)
{
	int i;
	rtk_l34_ipmcTrans_entry_t ipmc;

	rtlglue_printf("Idx\tNetif\tSIP_T_EN\tEXTIP_IDX\tPPPOE_ACT\tPPPOE_IDX\n");

	for(i=0; i<16; i++)
	{
		rtk_l34_ipmcTransTable_get(i,&ipmc);
//		rtlglue_printf("%02d\t%d\t%d\t\t%02d\t\t%d\t\t%d\n",i,ipmc.netifIdx,ipmc.sipTransEnable,ipmc.extipIdx,ipmc.isPppoeIf,ipmc.pppoeIdx);
		rtlglue_printf("%02d\t%d\t%d\t\t%02d\t\t%d\t\t%d\n",i,ipmc.netifIdx,ipmc.sipTransEnable,ipmc.extipIdx,ipmc.pppoeAct,ipmc.pppoeIdx);
	}
    return SUCCESS;
}
int32 dump_vlan_table (void)
{

	int i,j;
	rtk_portmask_t mbr, untag, ext;
	rtk_fid_t fid;
	rtk_fidMode_t ivlsvl;
	rtk_pri_t pri;
	rtk_enable_t en;

	rtlglue_printf(">>ASIC VLAN Table:\n\n");

	for(i=0; i<MAX_VLAN_HW_TABLE_SIZE; i++)
	{
		// clean up
		mbr.bits[0] = 0;
		untag.bits[0] = 0;
		ext.bits[0] = 0;
		fid = 0;
		ivlsvl = 0;
		pri = 0;
		en = 0;

		rtk_vlan_port_get(i, &mbr, &untag);
		if(mbr.bits[0]==0) continue;	// ignore unconfigured entry

		rtlglue_printf(" -- VID[%d] --", i);
#if 1
		rtlglue_printf("\n\tMember Ports:0x%x",mbr.bits[0]);
		rtk_vlan_extPort_get(i, &mbr);
		rtlglue_printf("\n\tExtension Member Ports:0x%x",mbr.bits[0]);
		rtlglue_printf("\n\tUntag Member Ports:0x%x",untag.bits[0]);

#else
		rtlglue_printf("\n\tMember Ports: ");
		if(mbr.bits[0]==0) rtlglue_printf("X");
		else
		{
			for(j=0; j<(MAX_APOLLO_PORT); j++)
			{
				if(mbr.bits[0] & (1<<j))  rtlglue_printf("%d ", j);
			}
		}
		rtlglue_printf("\n\tUntag Member Ports: ");
		if(untag.bits[0]==0) rtlglue_printf("X");
		else
		{
			for(j=0; j<(MAX_APOLLO_PORT); j++)
			{
				if(untag.bits[0] & (1<<j))	 rtlglue_printf("%d ", j);
			}
		}
#endif
		rtk_vlan_fid_get(i, &fid);
		rtk_vlan_fidMode_get(i, &ivlsvl);
		rtlglue_printf("\n\tFID: %d,\tIVL_SVL: %s\n", fid, (ivlsvl==VLAN_FID_IVL)?"IVL":"SVL");    //0:SVL, 1:IVL but VLAN_FID_IVL = 0, VLAN_FID_SVL = 1
		rtk_vlan_priority_get(i, &pri);
		rtk_vlan_priorityEnable_get(i, &en);
		rtlglue_printf("\tBased Priority: %s, %d\n", (en==ENABLED)?"enable":"disable", pri);
		rtk_vlan_extPort_get(i, &ext);
		rtlglue_printf("\tExtension Ports: ");
		if(ext.bits[0]==0) rtlglue_printf("X");
		else
		{
			for(j=0; j<(RTK_RG_MAX_EXT_PORT); j++)
			{
				if(ext.bits[0] & (1<<j))   rtlglue_printf("%d ", j);
			}
		}
		rtlglue_printf("\n");
	}

    return SUCCESS;
}
int32 dump_vlan_mbr (void)
{
	int i;
	uint32 val1, val2;
	rtlglue_printf(">>ASIC VLAN Member Configurations:\n\n");

#ifdef CONFIG_APOLLO_RLE0371
	for(i=0; i<32; i++)
	{
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, MBRf, &val2);
		if(val2==0) continue;

		rtlglue_printf(" -- IDX[%d] --\n", i);
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, EVIDf, &val1);
		rtlglue_printf("\tCVID: %d\n", val1);
		rtlglue_printf("\tMember Ports Mask: 0x%x\n", val2);
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, EXT_MBRf, &val1);
		rtlglue_printf("\tExtension Ports Mask: 0x%x\n", val1);
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, VBPRIf, &val1);
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, VBPENf, &val2);
		rtlglue_printf("\tBased Priority: %s, %d\n",
					   val2?"enable":"disable", val1);
		reg_array_field_read(VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, FID_MSTIf, &val1);
		rtlglue_printf("\tFID: %d\n", val1);
	}
#else
	for(i=0; i<32; i++)
	{
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_MBRf, &val2);
		if(val2==0) continue;

		rtlglue_printf(" -- IDX[%d] --\n", i);
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_EVIDf, &val1);
		rtlglue_printf("\tCVID: %d\n", val1);
		rtlglue_printf("\tMember Ports Mask: 0x%x\n", val2);
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_EXT_MBRf, &val1);
		rtlglue_printf("\tExtension Ports Mask: 0x%x\n", val1);
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_VBPRIf, &val1);
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_VBPENf, &val2);
		rtlglue_printf("\tBased Priority: %s, %d\n",
					   val2?"enable":"disable", val1);
		reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_FID_MSTIf, &val1);
		rtlglue_printf("\tFID: %d\n", val1);
	}
#endif
    return SUCCESS;
}
int32 dump_svlan_mbr (void)
{
    int i,j;
    rtk_svlan_memberCfg_t ent= {0};
    int res = 0, count = 0;
    rtlglue_printf(">>ASIC SVLAN Member Configurations:\n\n");

    for(i=0; i<MAX_VLAN_HW_TABLE_SIZE; i++)
    {
        ent.svid = i;
        res = rtk_svlan_memberPortEntry_get(&ent);
        if((res!=RT_ERR_OK) || (ent.memberport.bits[0] == 0)) continue;
        else
        {
            rtlglue_printf(" -- COUNT[%d] --\n", ++count);	// can't get real index here with rtk API
            rtlglue_printf("\tSVID: %d", ent.svid);
            rtlglue_printf("\tS-Priority: %d\n", ent.priority);

            rtlglue_printf("\tMember Ports: ");
            if(ent.memberport.bits[0]==0) rtlglue_printf("X");
            else
            {
                for(j=0; j<(RTK_RG_MAX_MAC_PORT+RTK_RG_MAX_EXT_PORT); j++)
                {
                    if(ent.memberport.bits[0] & (1<<j))  rtlglue_printf("%d ", j);
                }
            }
            rtlglue_printf("\n\tUntag Member Ports: ");
            if(ent.untagport.bits[0]==0) rtlglue_printf("X");
            else
            {
                for(j=0; j<(RTK_RG_MAX_MAC_PORT+RTK_RG_MAX_EXT_PORT); j++)
                {
                    if(ent.untagport.bits[0] & (1<<j))  rtlglue_printf("%d ", j);
                }
            }
            rtlglue_printf("\n\tForce FID: %s, %d\n", ent.fiden?"enabled":"disabled", ent.fid);
            rtlglue_printf("\tEnhanced FID: %s, %d\n",ent.efiden?"enabled":"disabled", ent.efid);
        }
    }

    return SUCCESS;
}
int32 dump_svlan_c2s (void)
{   
	rtlglue_printf("fix me");
#if 0
	// need too much time in FPGA
    /*    rtk_vlan_t svid=0, cvid;
        rtk_port_t port, pmsk=0;
        int res=0, count=0;
        for(cvid=0; cvid<(RTL865XC_VLAN_NUMBER*2-1); cvid++)
        {
            pmsk = 0;
            svid = 0;
            for(port=0; port<MAX_APOLLO_PORT; port++)
            {
                res=rtk_svlan_c2s_get(cvid, port, &svid);
                if(res==RT_ERR_OK) pmsk|=1<<port;
            }
            if(pmsk!=0)
            {
                rtlglue_printf(" -- COUNT[%d] --\n", count++);
                rtlglue_printf("\tEVID: %d\n", cvid);
                rtlglue_printf("\tC2SENPMSK: 0x%x\n", pmsk);
                rtlglue_printf("\tSVID: %d\n", svid);
            }
        }*/
    int i;
    uint32 val;
    rtlglue_printf(">>ASIC SVLAN CVID/PVID to SVID Tables:\n\n");

    for(i=0; i<MAX_C2S_HW_TABLE_SIZE; i++)
    {
        ioal_mem32_read(0x014000+i*0x4,&val);
        if(((val>>0x6)&0x7F)==0) continue;
        rtlglue_printf(" -- IDX[%d] --\n", i);
        rtlglue_printf("\tSVIDX: 0x%x\n", (val)&0x1F);
        rtlglue_printf("\tEVID: 0x%x\n", (val>>0xD)&0x1FFF);
        rtlglue_printf("\tC2SENPMSK: 0x%x\n", (val>>0x6)&0x7F);
    }
#endif	
    return SUCCESS;
}
int32 dump_svlan_mc2s (void)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
	int i;
	uint32 val;
	rtlglue_printf(">>ASIC SVLAN Multicast to SVLAN Tables:\n\n");

	//SVLAN_MC2S
	for(i=0; i<SVLANMC2STBL_SIZE; i++)
	{
		reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_VALIDf, &val);
		if(val==0) continue;

		rtlglue_printf(" -- IDX[%d] --\n", i);
		//rtlglue_printf("\tValid: yes\n");
		reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_FORMATf, &val);
		rtlglue_printf("\tFormat: %s\n", (val!=0)?"DIP":"DMAC");
		reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_SVIDXf, &val);
		rtlglue_printf("\tSVIDX: 0x%x\n", val);
		reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_DATAf, &val);
		rtlglue_printf("\tData: 0x%x\n", val);
		reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, i, APOLLOMP_MASKf, &val);
		rtlglue_printf("\tMask: 0x%x\n", val);
	}
#endif
    return SUCCESS;
}
int32 dump_svlan_sp2c (void)
{
    rtk_vlan_t svid, cvid;
    rtk_port_t port;
    int res=0, count=0;
    for(svid=0; svid<MAX_VLAN_HW_TABLE_SIZE; svid++)
    {
        cvid = 0;
        for(port=0; port<RTK_RG_MAX_MAC_PORT; port++)
        {
            res=rtk_svlan_sp2c_get(svid, port, &cvid);
            if(res==RT_ERR_OK)
            {
                rtlglue_printf(" -- COUNT[%d] --\n", count++);
                rtlglue_printf("\tSVID: %d\n", svid);
                rtlglue_printf("\tDestination Port: %d\n", port);
                rtlglue_printf("\tExtened VID: %d\n", cvid);
            }
        }
    }

    return SUCCESS;
}
int32 dump_acl_template(void)
{
    int i;
    rtk_acl_template_t aclTemplate;

#ifdef CONFIG_APOLLO_RLE0371
#else
	int j;
    uint32 val;
    rtlglue_printf("\n----ACL TEMPLATE(ASIC VALUE by ASIC API)-----\n");
    for(i=0; i<4; i++)
    {
    	rtlglue_printf("\tTEMPLATE[%d]:",i);
        for(j=0; j<8; j++)
        {
		reg_array_field_read(APOLLOMP_ACL_TEMPLATE_CTRLr, i,j, APOLLOMP_FIELDf, &val);
			rtlglue_printf("[%d:0x%x]",j,val&0x7f);
        }
		rtlglue_printf("\n");
    }
#endif
    rtlglue_printf("--------------- ACL TEMPLATES(API VALUE) ----------------\n");
    for(i=0; i<4; i++)
    {
        memset(&aclTemplate,0,sizeof(aclTemplate));
        aclTemplate.index=i;
        rtk_acl_template_get(&aclTemplate);
        rtlglue_printf("\tTEMPLATE[%d]: [0:0x%x][1:0x%x][2:0x%x][3:0x%x][4:0x%x][5:0x%x][6:0x%x][7:0x%x]\n",i,aclTemplate.fieldType[0],aclTemplate.fieldType[1],aclTemplate.fieldType[2],aclTemplate.fieldType[3],aclTemplate.fieldType[4],aclTemplate.fieldType[5],aclTemplate.fieldType[6],aclTemplate.fieldType[7]);
    }
    return SUCCESS;
}



int32 dump_acl_vidRangeTable(void)
{
    int i;
    rtk_acl_rangeCheck_vid_t vidRangeEntry;

    rtlglue_printf("------------ ACL VID RANGE TABLES -------------\n");
    for(i=0; i<8; i++)
    {
        memset(&vidRangeEntry,0,sizeof(vidRangeEntry));
        vidRangeEntry.index=i;
        rtk_acl_vidRange_get(&vidRangeEntry);
        rtlglue_printf("\tVIDRANGE[%d] upper:%d lower:%d type:0x%x\n",i,vidRangeEntry.upperVid,vidRangeEntry.lowerVid,vidRangeEntry.type);
    }
    return SUCCESS;
}
int32 dump_acl_ipRangeTable(void)
{
    int i;
    rtk_acl_rangeCheck_ip_t ipRangeEntry;
    rtlglue_printf("------------ ACL IP RANGE TABLES -------------\n");
    for(i=0; i<8; i++)
    {
        memset(&ipRangeEntry,0,sizeof(ipRangeEntry));
        ipRangeEntry.index=i;
        rtk_acl_ipRange_get(&ipRangeEntry);
        rtlglue_printf("\tIPRANGE[%d] upper:0x%x lower:0x%x type:0x%x\n",i,ipRangeEntry.upperIp,ipRangeEntry.lowerIp,ipRangeEntry.type);
    }
    return SUCCESS;
}
int32 dump_acl_portRangeTable(void)
{
    int i;
    rtk_acl_rangeCheck_l4Port_t portRangeEntry;
    rtlglue_printf("------------ ACL PORT RANGE TABLES -------------\n");
    for(i=0; i<16; i++)
    {
        memset(&portRangeEntry,0,sizeof(portRangeEntry));
        portRangeEntry.index=i;
        rtk_acl_portRange_get(&portRangeEntry);
        rtlglue_printf("\tPORTRANGE[%d] upper:%d lower:%d type:0x%x\n",i,portRangeEntry.upper_bound,portRangeEntry.lower_bound,portRangeEntry.type);
    }
    return SUCCESS;
}

int32 dump_acl_pktlenRangeTable(void)
{
    int i;

    rtk_acl_rangeCheck_pktLength_t pktlenRangeEntry;
    rtlglue_printf("------------ ACL PKTLEN RANGE TABLES -------------\n");
    for(i=0; i<8; i++)
    {
        memset(&pktlenRangeEntry,0,sizeof(pktlenRangeEntry));
        pktlenRangeEntry.index=i;
        rtk_acl_packetLengthRange_get(&pktlenRangeEntry);
        rtlglue_printf("\tPKTLENRANGE[%d] upper:%d lower:%d type:0x%x\n",i,pktlenRangeEntry.upper_bound,pktlenRangeEntry.lower_bound,pktlenRangeEntry.type);
    }
    return SUCCESS;
}

int32 dump_rg_acl(void){
	int i;
	rtk_rg_aclFilterAndQos_t *aclPara;
	rtk_rg_aclFilterAndQos_t acl_parameter;

	rtlglue_printf("acl_SW_table_entry_size:%d\n",rg_db.systemGlobal.acl_SW_table_entry_size);
	
	for(i=0;i<MAX_ACL_ENTRY_SIZE;i++){
		if(i>=rg_db.systemGlobal.acl_SW_table_entry_size){
			break;
		}else{
			aclPara = &(rg_db.systemGlobal.acl_SW_table_entry[i].acl_filter);
			rtlglue_printf("========================RG_ACL[%d]===========================\n",i);
			rtlglue_printf("##hw_acl_start:%d(continue:%d) hw_cf_start:%d(continue:%d) \n"
				,rg_db.systemGlobal.acl_SW_table_entry[i].hw_aclEntry_start
				,rg_db.systemGlobal.acl_SW_table_entry[i].hw_aclEntry_size
				,rg_db.systemGlobal.acl_SW_table_entry[i].hw_cfEntry_start
				,rg_db.systemGlobal.acl_SW_table_entry[i].hw_cfEntry_size);
			rtlglue_printf("##ACL_SIP4_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_IPTABLE_IPV4SIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_IPTABLE_IPV4SIP_INDEX]);
			rtlglue_printf("##ACL_DIP4_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_IPTABLE_IPV4DIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_IPTABLE_IPV4DIP_INDEX]);
			rtlglue_printf("##ACL_SIP6_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_IPTABLE_IPV6SIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_IPTABLE_IPV6SIP_INDEX]);
			rtlglue_printf("##ACL_DIP6_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_IPTABLE_IPV6DIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_IPTABLE_IPV6DIP_INDEX]);
			rtlglue_printf("##ACL_SPORT_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_PORTTABLE_SPORT_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_PORTTABLE_SPORT_INDEX]);
			rtlglue_printf("##ACL_DPORT_RANGE[%s]:%d  \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&ACL_USED_PORTTABLE_DPORT_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[ACL_USED_PORTTABLE_DPORT_INDEX]);
			rtlglue_printf("##CF_SIP4_RANGE[%s]:%d \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&CF_USED_IPTABLE_IPV4SIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[CF_USED_IPTABLE_IPV4SIP_INDEX]);
			rtlglue_printf("##CF_DIP4_RANGE[%s]:%d \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&CF_USED_IPTABLE_IPV4DIP_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[CF_USED_IPTABLE_IPV4DIP_INDEX]);
			rtlglue_printf("##CF_SPORT_RANGE[%s]:%d \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&CF_USED_PORTTABLE_SPORT_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[CF_USED_PORTTABLE_SPORT_INDEX]);
			rtlglue_printf("##CF_DPORT_RANGE[%s]:%d \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&CF_USED_PORTTABLE_DPORT_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[CF_USED_PORTTABLE_DPORT_INDEX]);
			rtlglue_printf("##CF_DSCP[%s]:%d \n",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table&CF_USED_DSCPTABLE_INDEX?"O":"X",rg_db.systemGlobal.acl_SW_table_entry[i].hw_used_table_index[CF_USED_DSCPTABLE_INDEX]);


			rtlglue_printf("Patterns: \n");	
			rtlglue_printf("filter_fields:0x%x\n", aclPara->filter_fields);	
			rtlglue_printf("ingress_port_mask:0x%x\n", aclPara->ingress_port_mask.portmask);	
			rtlglue_printf("ingress_dscp:     %d\n", aclPara->ingress_dscp);			
			rtlglue_printf("ingress_intf_idx: %d\n", aclPara->ingress_intf_idx);	
			rtlglue_printf("egress_intf_idx:  %d\n", aclPara->egress_intf_idx);	
			rtlglue_printf("ingress_ethertype:0x%x\n", aclPara->ingress_ethertype);			
			rtlglue_printf("ingress_ctag_vid: %d\n", aclPara->ingress_ctag_vid);	
			rtlglue_printf("ingress_ctag_pri: %d\n", aclPara->ingress_ctag_pri);	
			rtlglue_printf("ingress_smac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
							aclPara->ingress_smac.octet[0],
							aclPara->ingress_smac.octet[1],
					aclPara->ingress_smac.octet[2],
					aclPara->ingress_smac.octet[3],
					aclPara->ingress_smac.octet[4],
					aclPara->ingress_smac.octet[5]);			
			rtlglue_printf("ingress_dmac: %02X:%02X:%02X:%02X:%02X:%02X\n",
					aclPara->ingress_dmac.octet[0],
					aclPara->ingress_dmac.octet[1],
					aclPara->ingress_dmac.octet[2],
					aclPara->ingress_dmac.octet[3],
					aclPara->ingress_dmac.octet[4],
					aclPara->ingress_dmac.octet[5]);

			acl_parameter = rg_db.systemGlobal.acl_SW_table_entry[i].acl_filter;
			rtlglue_printf("ingress_sip_low_bound: %s  ", inet_ntoa(acl_parameter.ingress_src_ipv4_addr_start));
			rtlglue_printf("ingress_sip_up_bound:  %s \n", inet_ntoa(acl_parameter.ingress_src_ipv4_addr_end));

			rtlglue_printf("ingress_dip_low_bound: %s  ", inet_ntoa(acl_parameter.ingress_dest_ipv4_addr_start));
			rtlglue_printf("ingress_dip_up_bound:  %s \n", inet_ntoa(acl_parameter.ingress_dest_ipv4_addr_end));


			rtlglue_printf("ingress_src_ipv6_addr_start: %02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[0],acl_parameter.ingress_src_ipv6_addr_start[1]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[2],acl_parameter.ingress_src_ipv6_addr_start[3]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[4],acl_parameter.ingress_src_ipv6_addr_start[5]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[6],acl_parameter.ingress_src_ipv6_addr_start[7]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[8],acl_parameter.ingress_src_ipv6_addr_start[9]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[10],acl_parameter.ingress_src_ipv6_addr_start[11]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[12],acl_parameter.ingress_src_ipv6_addr_start[13]);
			rtlglue_printf(":%02X%02X: \n",acl_parameter.ingress_src_ipv6_addr_start[14],acl_parameter.ingress_src_ipv6_addr_start[15]);

			rtlglue_printf("ingress_src_ipv6_addr_end: %02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[0],acl_parameter.ingress_src_ipv6_addr_end[1]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[2],acl_parameter.ingress_src_ipv6_addr_end[3]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[4],acl_parameter.ingress_src_ipv6_addr_end[5]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[6],acl_parameter.ingress_src_ipv6_addr_end[7]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[8],acl_parameter.ingress_src_ipv6_addr_end[9]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[10],acl_parameter.ingress_src_ipv6_addr_end[11]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[12],acl_parameter.ingress_src_ipv6_addr_end[13]);
			rtlglue_printf(":%02X%02X: \n",acl_parameter.ingress_src_ipv6_addr_end[14],acl_parameter.ingress_src_ipv6_addr_end[15]);

			rtlglue_printf("ingress_dest_ipv6_addr_start: %02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[0],acl_parameter.ingress_dest_ipv6_addr_start[1]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[2],acl_parameter.ingress_dest_ipv6_addr_start[3]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[4],acl_parameter.ingress_dest_ipv6_addr_start[5]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[6],acl_parameter.ingress_dest_ipv6_addr_start[7]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[8],acl_parameter.ingress_dest_ipv6_addr_start[9]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[10],acl_parameter.ingress_dest_ipv6_addr_start[11]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[12],acl_parameter.ingress_dest_ipv6_addr_start[13]);
			rtlglue_printf(":%02X%02X: \n",acl_parameter.ingress_dest_ipv6_addr_start[14],acl_parameter.ingress_dest_ipv6_addr_start[15]);

			rtlglue_printf("ingress_dest_ipv6_addr_end: %02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[0],acl_parameter.ingress_dest_ipv6_addr_end[1]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[2],acl_parameter.ingress_dest_ipv6_addr_end[3]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[4],acl_parameter.ingress_dest_ipv6_addr_end[5]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[6],acl_parameter.ingress_dest_ipv6_addr_end[7]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[8],acl_parameter.ingress_dest_ipv6_addr_end[9]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[10],acl_parameter.ingress_dest_ipv6_addr_end[11]);
			rtlglue_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[12],acl_parameter.ingress_dest_ipv6_addr_end[13]);
			rtlglue_printf(":%02X%02X: \n",acl_parameter.ingress_dest_ipv6_addr_end[14],acl_parameter.ingress_dest_ipv6_addr_end[15]);


			rtlglue_printf("ingress_src_l4_port_low_bound: %d  ", acl_parameter.ingress_src_l4_port_start);
			rtlglue_printf("ingress_src_l4_port_up_bound:  %d \n", acl_parameter.ingress_src_l4_port_end);

			rtlglue_printf("ingress_dest_l4_port_low_bound: %d  ", acl_parameter.ingress_dest_l4_port_start);
			rtlglue_printf("ingress_dest_l4_port_up_bound:  %d \n", acl_parameter.ingress_dest_l4_port_end);


			rtlglue_printf("egress_sip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.egress_src_ipv4_addr_start));
			rtlglue_printf("egress_sip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.egress_src_ipv4_addr_end));

			rtlglue_printf("egress_dip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.egress_dest_ipv4_addr_start));
			rtlglue_printf("egress_dip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.egress_dest_ipv4_addr_end));

			rtlglue_printf("egress_src_l4_port_low_bound: %d  ", acl_parameter.egress_src_l4_port_start);
			rtlglue_printf("egress_src_l4_port_up_bound:  %d \n", acl_parameter.egress_src_l4_port_end);

			rtlglue_printf("egress_dest_l4_port_low_bound: %d  ", acl_parameter.egress_dest_l4_port_start);
			rtlglue_printf("egress_dest_l4_port_up_bound:  %d \n", acl_parameter.egress_dest_l4_port_end);

			if(acl_parameter.filter_fields & INGRESS_L4_TCP_BIT){
				rtlglue_printf("l4-protocal: tcp \n");
			}else if(acl_parameter.filter_fields & INGRESS_L4_UDP_BIT){
				rtlglue_printf("l4-protocal: udp \n");
			}else if(acl_parameter.filter_fields & INGRESS_L4_ICMP_BIT){
				rtlglue_printf("l4-protocal: icmp \n");
			}else{
				rtlglue_printf("l4-protocal: not care \n");
			}


			rtlglue_printf("\n");
			rtlglue_printf("Actions: \n");
			switch(acl_parameter.action_type){
				case ACL_ACTION_TYPE_DROP:
					rtlglue_printf("action_type: ACL_ACTION_TYPE_DROP \n");
					break;
				case ACL_ACTION_TYPE_PERMIT:
					rtlglue_printf("action type: ACL_ACTION_TYPE_PERMIT \n");
					break;
				case ACL_ACTION_TYPE_TRAP:
					rtlglue_printf("action type: ACL_ACTION_TYPE_TRAP \n");
					break;
				case ACL_ACTION_TYPE_TRAP_TO_PS:
					rtlglue_printf("action type: ACL_ACTION_TYPE_TRAP_TO_PS \n");
					break;
				case ACL_ACTION_TYPE_QOS:
					rtlglue_printf("action type: ACL_ACTION_TYPE_QOS \n");
					rtlglue_printf("qos_actions_bits: 0x%x\n",acl_parameter.qos_actions);
					rtlglue_printf("dot1p_remarking_pri: %d \n",acl_parameter.action_dot1p_remarking_pri);
					rtlglue_printf("ip_precedence_remarking_pri: %d\n",acl_parameter.action_ip_precedence_remarking_pri);
					rtlglue_printf("dot1p_remarking_pri: %d\n",acl_parameter.action_dscp_remarking_pri);
					rtlglue_printf("queue_id: %d\n",acl_parameter.action_queue_id);
					rtlglue_printf("share_meter: %d\n",acl_parameter.action_share_meter);		
					break;
				default:
					break;
			}
	

		}



	}

	return SUCCESS;
}


int32 dump_acl(void)
{
    int i,j;

#if 1//def FORCE_PROBE_APOLLOMP
    uint32 val;
#endif
    rtk_acl_ingress_entry_t aclRule;

    rtlglue_printf("--------------- ACL TABLES ----------------\n");
    for(i=0; i<64; i++)
    {
        memset(&aclRule,0,sizeof(aclRule));
        aclRule.index=i;
        rtk_acl_igrRuleEntry_get(&aclRule);
        if(aclRule.valid)
        {
            rtlglue_printf("\t--- ACL TABLE[%d] ---\n",i);
            rtlglue_printf("\tvalid:%x\n",aclRule.valid);
            for(j=0; j<8; j++)
            {
                rtlglue_printf("\tfield[%d]:%x  fielf_mask[%d]:%x\n",j,aclRule.readField.fieldRaw[j].value,j,aclRule.readField.fieldRaw[j].mask);

            }
            rtlglue_printf("\tactive portmask:0x%x\n",aclRule.activePorts.bits[0]);
            rtlglue_printf("\tTag_care:0b%d%d%d%d%d%d%d\n \tTag_mask:0b%d%d%d%d%d%d%d\n",
                           aclRule.careTag.tags[6].value,aclRule.careTag.tags[5].value,aclRule.careTag.tags[4].value,aclRule.careTag.tags[3].value,aclRule.careTag.tags[2].value,aclRule.careTag.tags[1].value,aclRule.careTag.tags[0].value,
                           aclRule.careTag.tags[6].mask,aclRule.careTag.tags[5].mask,aclRule.careTag.tags[4].mask,aclRule.careTag.tags[3].mask,aclRule.careTag.tags[2].mask,aclRule.careTag.tags[1].mask,aclRule.careTag.tags[0].mask);
            rtlglue_printf("\ttemplateIdx:%x\n",aclRule.templateIdx);
            rtlglue_printf("\taction bits:[INT:%d][FWD:%d][LOG:%d][PRI:%d][SVLAN:%d][CVLAN:%d] \n",aclRule.act.enableAct[5],aclRule.act.enableAct[4],aclRule.act.enableAct[3],aclRule.act.enableAct[2],aclRule.act.enableAct[1],aclRule.act.enableAct[0]);
            rtlglue_printf("\t[CACT:%x] cvid:%d dot1p:%x\n",aclRule.act.cvlanAct.act,aclRule.act.cvlanAct.cvid,aclRule.act.cvlanAct.dot1p);

#ifdef CONFIG_APOLLO_RLE0371
            rtlglue_printf("\t[SACT:%x] svid:%d dot1p:%x dscp:%d \n",aclRule.act.svlanAct.act,aclRule.act.svlanAct.svid,aclRule.act.svlanAct.dot1p,aclRule.act.svlanAct.dscp);
#else
			rtlglue_printf("\t[SACT:%x] svid:%d dot1p:%x dscp:%d nexthop:%x\n",aclRule.act.svlanAct.act,aclRule.act.svlanAct.svid,aclRule.act.svlanAct.dot1p,aclRule.act.svlanAct.dscp,aclRule.act.svlanAct.nexthop);
#endif

            rtlglue_printf("\t[FWDACT:%x] portMask:0x%x\n",aclRule.act.forwardAct.act,aclRule.act.forwardAct.portMask.bits[0]);
#ifdef CONFIG_APOLLO_RLE0371
            rtlglue_printf("\t[PRIACT:%x] aclPri:%x dot1p:%x dscp:%d \n",aclRule.act.priAct.act,aclRule.act.priAct.aclPri,aclRule.act.priAct.dot1p,aclRule.act.priAct.dscp);
#else
	        rtlglue_printf("\t[PRIACT:%x] aclPri:%x dot1p:%x dscp:%d nexthop:%x\n",aclRule.act.priAct.act,aclRule.act.priAct.aclPri,aclRule.act.priAct.dot1p,aclRule.act.priAct.dscp,aclRule.act.priAct.nexthop);
#endif
            rtlglue_printf("\t[LOGACT:%x] meteridx:%d\n",aclRule.act.logAct.act, aclRule.act.logAct.meter);
            rtlglue_printf("\t[INTACT:%x] EXTACT:%x ACLLATCH:%x INTERRUPT:%x index(stream_id or llid):0x%x, pmask:0x%x\n",aclRule.act.aclInterrupt,aclRule.act.extendAct.act,aclRule.act.aclLatch, aclRule.act.aclInterrupt,aclRule.act.extendAct.index,aclRule.act.extendAct.portMask.bits[0]);
            rtlglue_printf("\t[ACL_LATCH:%x] \n",aclRule.act.aclLatch);
        }
    }


#if 1//def FORCE_PROBE_APOLLOMP
	//use the ASIC API
    rtlglue_printf("--------------- ACL HIT OINFO----------------\n");
    for(i=0; i<6; i++)
    {
        reg_array_field_read(APOLLOMP_STAT_ACL_REASONr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_ACL_HIT_INFOf, &val);
        switch(i)
        {
        case 0:
            rtlglue_printf("[CACT:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;
        case 1:
            rtlglue_printf("[SACT:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;
        case 2:
            rtlglue_printf("[PRI:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;
        case 3:
            rtlglue_printf("[POLICE:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;
        case 4:
            rtlglue_printf("[INT:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;
        case 5:
            rtlglue_printf("[FWD:%s]: hit rule %d\n",val&0x80?"O":"X",val&0x7f);
            break;

        }
    }
	
#endif  

    return SUCCESS;
}

int32 dump_cf(void)
{
    int i;
#if 1
//#ifdef FORCE_PROBE_APOLLOMP
    uint32 val;
#endif

    rtk_dscp_t dscp_entry;
    int dscp=0;
    rtk_classify_cfg_t classifyCfg;
    rtk_classify_rangeCheck_l4Port_t prtRangeEntry;
    rtk_classify_rangeCheck_ip_t ipRangeEntry;


    rtlglue_printf("------------ CF PORT RANGE TABLE -------------\n");
    for(i=0; i<8; i++)
    {
        memset(&prtRangeEntry,0,sizeof(prtRangeEntry));
        prtRangeEntry.index=i;
        rtk_classify_portRange_get(&prtRangeEntry);
        rtlglue_printf("\tPORTRANGE[%d] upper:%d lower:%d type:0x%x\n",i,prtRangeEntry.upperPort,prtRangeEntry.lowerPort,prtRangeEntry.type);

    }

    rtlglue_printf("------------ CF IP RANGE TABLE -------------\n");
    for(i=0; i<8; i++)
    {
        memset(&ipRangeEntry,0,sizeof(ipRangeEntry));
        ipRangeEntry.index=i;
        rtk_classify_ipRange_get(&ipRangeEntry);		
        rtlglue_printf("\tIPRANGE[%d] upper:0x%x lower:0x%x type:0x%x\n",i,ipRangeEntry.upperIp,ipRangeEntry.lowerIp,ipRangeEntry.type);
    }

	rtlglue_printf("------------ CF DSCP TABLE -------------\n");
    for(i=0; i<8; i++)
    {
		rtk_classify_cfPri2Dscp_get(i,&dscp_entry);
        rtlglue_printf("\tDSCP[%d]:0x%x\n",i,dscp_entry);
    }

    rtlglue_printf("------------ CF TABLE -------------\n");
    for(i=0; i<64; i++)
    {
        memset(&classifyCfg,0,sizeof(classifyCfg));
        classifyCfg.index=i;
        rtk_classify_cfgEntry_get(&classifyCfg);
        if(classifyCfg.valid)
        {
            rtlglue_printf("\t--- CF TABLE[%d] ---\n",i);
            rtlglue_printf("\tvalid:%x\n",classifyCfg.valid);

            rtlglue_printf("\tfield[2]:%x mask:%x\n",classifyCfg.field.readField.dataFieldRaw[2],classifyCfg.field.readField.careFieldRaw[2]);
            rtlglue_printf("\tfield[1]:%x mask:%x\n",classifyCfg.field.readField.dataFieldRaw[1],classifyCfg.field.readField.careFieldRaw[1]);
            rtlglue_printf("\tfield[0]:%x mask:%x\n",classifyCfg.field.readField.dataFieldRaw[0],classifyCfg.field.readField.careFieldRaw[0]);

            //parse PortRange
            if((classifyCfg.field.readField.dataFieldRaw[2]>>12)&0x8)
            {
                rtlglue_printf("\tIP RANGE[ENABLE]:%d\t",(classifyCfg.field.readField.dataFieldRaw[2]>>12)&0x7);
                rtlglue_printf("\tIP_RANGE_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>12)&0xf);

            }
            else
            {
                rtlglue_printf("\tIP RANGE[DISABLE]\t");
                rtlglue_printf("\tIP_RANGE_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>12)&0xf);
            }

            //parse IpRange
            if((classifyCfg.field.readField.dataFieldRaw[2]>>8)&0x8)
            {
                rtlglue_printf("\tPORT RANGE[ENABLE]:%d\t",(classifyCfg.field.readField.dataFieldRaw[2]>>8)&0x7);
                rtlglue_printf("\tPORT_RANGE_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>8)&0xf);
            }
            else
            {
                rtlglue_printf("\tPORT RANGE[DISABLE]\t");
                rtlglue_printf("\tPORT_RANGE_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>8)&0xf);
            }

            //parse Acl Hit
            if((classifyCfg.field.readField.dataFieldRaw[2]>>0)&0x80)
            {
                rtlglue_printf("\tACL HIT[ENABLE]:%d\t",(classifyCfg.field.readField.dataFieldRaw[2]>>0)&0x7f);
                rtlglue_printf("\tACL_HIT_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>0)&0xff);
            }
            else
            {
                rtlglue_printf("\tACL HIT[DISABLE]\t");
                rtlglue_printf("\tACL_HIT_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[2]>>0)&0xff);
            }

            //parse U/D
            rtlglue_printf("\tDIRECTION:%x (%s)\n",classifyCfg.direction,classifyCfg.direction?"DOWNSTREAM":"UPSTREAM");

            //parse Wan Interface
            rtlglue_printf("\tWAN_IF:%d\t",(classifyCfg.field.readField.dataFieldRaw[1]>>12)&0x7);
            rtlglue_printf("\tWAN_IF_MASK:%x\n",(classifyCfg.field.readField.careFieldRaw[1]>>12)&0x7);


            //parse IPv6_MC IPv4_MC MLD IGMP
            rtlglue_printf("\tIPv6_MC:%d IPv4_MC:%d MLD:%d IGMP:%d \t",
                           (classifyCfg.field.readField.dataFieldRaw[1]>>11)&0x1,
                           (classifyCfg.field.readField.dataFieldRaw[1]>>10)&0x1,
                           (classifyCfg.field.readField.dataFieldRaw[1]>>9)&0x1,
                           (classifyCfg.field.readField.dataFieldRaw[1]>>8)&0x1);
            rtlglue_printf("\tIPv6_MC_MSK:%d IPv4_MC_MSK:%d MLD_MSK:%d IGMP_MSK:%d \n",
                           (classifyCfg.field.readField.careFieldRaw[1]>>11)&0x1,
                           (classifyCfg.field.readField.careFieldRaw[1]>>10)&0x1,
                           (classifyCfg.field.readField.careFieldRaw[1]>>9)&0x1,
                           (classifyCfg.field.readField.careFieldRaw[1]>>8)&0x1);

            //parse DEI
            rtlglue_printf("\tDEI:%d\t",(classifyCfg.field.readField.dataFieldRaw[1]>>7)&0x1);
            rtlglue_printf("\tDEI_MSK:%d\n",(classifyCfg.field.readField.careFieldRaw[1]>>7)&0x1);

            //parse VID
            rtlglue_printf("\tVID:%d\t",(((classifyCfg.field.readField.dataFieldRaw[1]>>0)&0x7f)<<5) | ((classifyCfg.field.readField.dataFieldRaw[0])>>11 &0x1f));
            rtlglue_printf("\tVID_MSK:%d\n",(((classifyCfg.field.readField.careFieldRaw[1]>>0)&0x7f)<<5) | ((classifyCfg.field.readField.careFieldRaw[0])>>11 &0x1f));

            //parse PRI
            rtlglue_printf("\tPRI:%d\t",(classifyCfg.field.readField.dataFieldRaw[0]>>8)&0x7);
            rtlglue_printf("\tPRI:%d\n",(classifyCfg.field.readField.careFieldRaw[0]>>8)&0x7);

            //parse INTER_PRI
            rtlglue_printf("\tINTER_PRI:%d\t",(classifyCfg.field.readField.dataFieldRaw[0]>>5)&0x7);
            rtlglue_printf("\tINTER_PRI:%d\n",(classifyCfg.field.readField.careFieldRaw[0]>>5)&0x7);

            //parse STAG, CTAG
            rtlglue_printf("\tSTAG_IF:%d CTAG_IF:%d\t",(classifyCfg.field.readField.dataFieldRaw[0]>>4)&0x1,(classifyCfg.field.readField.dataFieldRaw[0]>>3)&0x1);
            rtlglue_printf("\tSTAG_IF_MSK:%d CTAG_IF_MSK:%d\n",(classifyCfg.field.readField.careFieldRaw[0]>>4)&0x1,(classifyCfg.field.readField.careFieldRaw[0]>>3)&0x1);

            //parse UNI
            rtlglue_printf("\tUNI:%d\t",(classifyCfg.field.readField.dataFieldRaw[0]>>0)&0x7);
            rtlglue_printf("\tUNI_MSK:%d\n",(classifyCfg.field.readField.careFieldRaw[0]>>0)&0x7);


            if(classifyCfg.direction) //downstream
            {
                rtk_classify_cfPri2Dscp_get(classifyCfg.act.dsAct.cfPri, &dscp);
                rtlglue_printf("\t[CSACT] SACT:0x%x SVID_ACT:0x%x SPRI_ACT:0x%x  SVID:%d SPRI:%d \n",classifyCfg.act.dsAct.csAct,classifyCfg.act.dsAct.csVidAct,classifyCfg.act.dsAct.csPriAct,classifyCfg.act.dsAct.sTagVid,classifyCfg.act.dsAct.sTagPri);
                rtlglue_printf("\t[CACT] CACT:0x%x CVID_ACT:0x%x CPRI_ACT:0x%x VID:%d PRI:%d \n",classifyCfg.act.dsAct.cAct,classifyCfg.act.dsAct.cVidAct,classifyCfg.act.dsAct.cPriAct,classifyCfg.act.dsAct.cTagVid,classifyCfg.act.dsAct.cTagPri);
                rtlglue_printf("\t[CFPRI] CFPRI_ACT:0x%x CFPRI:%d\n",classifyCfg.act.dsAct.interPriAct,classifyCfg.act.dsAct.cfPri);
                rtlglue_printf("\t[DSCP] DSCP_ACT:0x%x DSCP:%d \n",classifyCfg.act.dsAct.dscp,dscp);
                rtlglue_printf("\t[UNI] UNI_ACT:0x%x UNI_MASK:0x%x\n",classifyCfg.act.dsAct.uniAct,classifyCfg.act.dsAct.uniMask.bits[0]);
            }
            else
            {
                rtk_classify_cfPri2Dscp_get(classifyCfg.act.usAct.cfPri, &dscp);
                rtlglue_printf("\t[CSACT] SACT:0x%x SVID_ACT:0x%x SPRI_ACT:0x%x SVID:%d SPRI:%d \n",classifyCfg.act.usAct.csAct,classifyCfg.act.usAct.csVidAct,classifyCfg.act.usAct.csPriAct,classifyCfg.act.usAct.sTagVid,classifyCfg.act.usAct.sTagPri);
                rtlglue_printf("\t[CACT] CACT:0x%x CVID_ACT:0x%x CPRI_ACT:0x%x VID:%d PRI:%d \n",classifyCfg.act.usAct.cAct,classifyCfg.act.usAct.cVidAct,classifyCfg.act.usAct.cPriAct,classifyCfg.act.usAct.cTagVid,classifyCfg.act.usAct.cTagPri);
                rtlglue_printf("\t[CFPRI] CFPRI_ACT:0x%x CFPRI:%d\n",classifyCfg.act.usAct.interPriAct,classifyCfg.act.usAct.cfPri);
                rtlglue_printf("\t[DSCP] DSCP_ACT:0x%x DSCP:%d \n",classifyCfg.act.usAct.dscp,dscp);
                rtlglue_printf("\t[DROP] DROP_ACT:%x\n",classifyCfg.act.usAct.drop);
                rtlglue_printf("\t[SID] SID_ACT:%x SidQid=%d \n",classifyCfg.act.usAct.sidQidAct,classifyCfg.act.usAct.sidQid);
                rtlglue_printf("\t[LOG] LOG_ACT:%x\n",classifyCfg.act.usAct.log);
            }
        }
    }

#if 1
//#ifdef FORCE_PROBE_APOLLOMP

	rtlglue_printf("--------------- CF HIT INFO----------------\n");
    for(i=0; i<2; i++)
    {
        reg_array_field_read(APOLLOMP_STAT_CF_REASONr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_CF_HIT_INFOf, &val);
        switch(i)
        {
        case 0:
			if(val&0x200)
            	rtlglue_printf("[RULE(64-511):%s]: hit rule %d\n",val&0x200?"O":"X",val&0x1ff);
			else
				rtlglue_printf("[RULE(64-511):%s]: unhit\n",val&0x200?"O":"X");
			break;
        case 1:
			if(val&0x200)
            	rtlglue_printf("[RULE(0-63):%s]: hit rule %d\n",val&0x200?"O":"X",val&0x1ff);
			else
				rtlglue_printf("[RULE(0-63):%s]: unhit\n",val&0x200?"O":"X");
			break;
        }
    }
#endif

    return SUCCESS;
}

int32 dump_reg (void)
{

#ifdef CONFIG_APOLLO_MODEL
	rtlglue_printf("X86 Model do not support register dump!!\n");
#else
	//reg_array_field_read(APOLLOMP_XXXr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_XXXf, &val);
		int32 retv;
		uint32 val,val2;
		//rtk_portmask_t pmsk;
		//rtk_vlan_protoVlanCfg_t ppbCfg;
		int i,j;
#ifdef CONFIG_APOLLO_RLE0371
#else
		//rtk_vlan_protoGroup_t protoGroup;
#endif
	
#ifdef CONFIG_APOLLO_RLE0371
 		//LUT regs
		rtlglue_printf("\n-----LUT-----\n");
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_UNKN_UC_DA_CTRLr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNKN_UC_DA_BEHAVE[%d]:0x%x\n",i,val);
		}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_UNKN_UC_FLOODr, i, REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_LUT_UNKN_UC_FLOOD[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_BC_FLOODr, i, REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_LUT_BC_FLOOD[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_UNKN_SA_CTRLr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNKN_SA_BEHAVE[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_UNMATCHED_SA_CTRLr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNMATCHED_SA_BEHAVE[%d]:0x%x\n",i,val);
		}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(L2_SRC_PORT_PERMITr, i, REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_L2_SRC_PORT_PERMIT[%d]:0x%x\n",i,(val>>i)&0x1);
		}
		for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
		{
			reg_array_field_read(L2_SRC_EXT_PERMITr, REG_ARRAY_INDEX_NONE,i, ENf, &val);
			rtlglue_printf("reg_L2_SRC_EXT_PERMIT[%d]:0x%x\n",i,(val>>(i-1))&0x1);
		}
			
		//Port Security regs
		rtlglue_printf("\n-----Port Security-----\n");
			
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_LRN_LIMITNOr, i, REG_ARRAY_INDEX_NONE, NUMf, &val);
			rtlglue_printf("reg_LUT_LRN_LIMITNO[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_LEARN_OVER_CTRLr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_LUT_LEARN_OVER_ACT[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_DIS_AGEr, i, REG_ARRAY_INDEX_NONE, DIS_AGEf, &val);
			rtlglue_printf("reg_LUT_DISABLE_AGE[%d]: %d\n",i,val);
		}
			
			
		reg_field_read(LUT_CFGr,  LUT_IPMC_HASHf, &val);
		rtlglue_printf("reg_LUT_IPMC_HASH:0x%x\n",val);
			
		reg_field_read(LUT_CFGr,  LUT_IPMC_LOOKUP_OPf, &val);
			
		rtlglue_printf("reg_LUT_IPMC_LOOKUP_OP:0x%x\n",val);
	
		//Multicast
		rtlglue_printf("\n-----Multicast-----\n");
	
		reg_field_read(L34_GLB_CFGr,  L34_GLOBAL_CFGf, &val);
		rtlglue_printf("reg_L34_Global_Enable: 0x%x\n",val);
			
		//Unknown Multicast Control
		rtlglue_printf("\n-----Unknown Multicast Control-----\n");
			
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(UNKN_IP4_MCr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNKN_IP4_MC_ACT[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(UNKN_IP6_MCr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNKN_IP6_MC_ACT[%d]:0x%x\n",i,val);
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(UNKN_L2_MCr, i, REG_ARRAY_INDEX_NONE, ACTf, &val);
			rtlglue_printf("reg_UNKN_L2_MC_ACT[%d]:0x%x\n",i,val);
		}
			
			
		rtlglue_printf("reg_UNKNOWN_MCAST_TRAP_PRIORITY: not found\n");
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(LUT_UNKN_MC_FLOODr, i, REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_LUT_UNKN_MC_FLOOD[%d]:0x%x\n",i,val);
		}
			
			
		// QOS-Ingress Priority regs
		rtlglue_printf("\n----- QOS-Ingress Priority-----\n");
			
		for(i=0; i<8; i++)
		{
			reg_array_field_read(QOS_1Q_PRI_REMAPr,REG_ARRAY_INDEX_NONE,i, INTPRI_1Qf, &val);
			rtlglue_printf("reg_DOT1Q_PRI_PRIORITY[%d]:0x%x\n",i,val);//QOS_1Q_PRI_REMAP
		}
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(QOS_PB_PRIr, i, REG_ARRAY_INDEX_NONE, INTPRI_PBf, &val);
			rtlglue_printf("reg_QOS_PORT_PRIORITY[%d]:0x%x\n",i,val);//QOS_PB_PRI
		}
			
		reg_field_read(PRI_SEL_TBL_CTRLr,	PORT_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_PORT_WEIGHT:0x%x\n",val); //PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	DOT1Q_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_1Q_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	DSCP_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_DSCP_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	ACL_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_ACL_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	CVLAN_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_CVLAN_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	LUTFWD_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_LUTFWD_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	SA_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_SA_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRLr,	SVLAN_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_SVLAN_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(PRI_SEL_TBL_CTRL2r,  L4_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_L4_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL2
			
			
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(QOS_PORT_QMAP_CTRLr,i,REG_ARRAY_INDEX_NONE, IDXf, &val);
			rtlglue_printf("reg_PORT_QTABLE_INDEX[%d]:0x%x\n",i,val);//QOS_PRI_REMAP_IN_CPU
		}
	
		for(i=0; i<4; i++)
		{
			for(j=0; j<8; j++)
			{
				reg_array_field_read(QOS_INTPRI_TO_QIDr,i,j, PRI_TO_QIDf, &val);
				rtlglue_printf("reg_QOS_PRIORITY_TO_QID_TABLE[%d][%d]:0x%x\n",i,j,val);//QOS_INTPRI_TO_QID
			}
		}
			
		// QOS-Remarking
		rtlglue_printf("\n----- QOS-Remarking-----\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(RMK_DOT1Q_RMK_EN_CTRLr,i,REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_REMARKING_1Q_PORT_ENABLE[%d]:0x%x\n",i,val);//RMK_DOT1Q_RMK_EN_CTRL
		}
			
//ioal_mem32_read(0x0231CC,&val);
		for(i=0; i<8; i++)
		{
			reg_array_field_read(RMK_1Q_CTRLr,REG_ARRAY_INDEX_NONE,i, INTPRI_1Qf, &val);
			rtlglue_printf("reg_INTPRI_1QPRI[%d]:0x%x\n",i,val); //RMK_1Q_CTRL
		}
			
		//Qos
	for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
        {
                reg_array_field_read(RMK_DSCP_RMK_EN_CTRLr,i,REG_ARRAY_INDEX_NONE, ENf, &val);
                rtlglue_printf("reg_RMK_DSCP_RMK_EN_CTRL[%d]:0x%x\n",i,val);
        }

        for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
        {
                reg_array_field_read(RMK_P_DSCP_SELr,REG_ARRAY_INDEX_NONE,i, SELf, &val);
                rtlglue_printf("reg_RMK_DSCP_CFG_SEL[%d]:0x%x\n",i,val);
        }

        for(i=0; i<64; i++)
        {
                reg_array_field_read(QOS_DSCP_REMAPr,REG_ARRAY_INDEX_NONE,i, INTPRI_DSCPf, &val);
                rtlglue_printf("reg_QOS_DSCP_REMAP[%d]:%d\n",i,val);
        }

        for(i=0; i<8; i++)
        {
                reg_array_field_read(RMK_DSCP_INT_PRI_CTRLr,REG_ARRAY_INDEX_NONE,i, INTPRI_DSCPf, &val);
                rtlglue_printf("reg_RMK_DSCP_INT_PRI_CTRL[%d]:0x%x\n",i,val);
        }

        for(i=0; i<64; i++)
        {
                reg_array_field_read(RMK_DSCP_CTRLr,REG_ARRAY_INDEX_NONE,i, INTPRI_DSCPf, &val);
                rtlglue_printf("reg_RMK_DSCP_CTRL[%d]:0x%x\n",i,val);
        }
#else
		//LUT regs
		rtlglue_printf("\n-----LUT-----\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_UNKN_UC_DA_CTRLr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNKN_UC_DA_BEHAVE[%d]:0x%x\n",i,val);
		}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_UNKN_UC_FLOODr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_LUT_UNKN_UC_FLOOD[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_BC_FLOODr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_LUT_BC_FLOOD[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_UNKN_SA_CTRLr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNKN_SA_BEHAVE[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_UNMATCHED_SA_CTRLr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNMATCHED_SA_BEHAVE[%d]:0x%x\n",i,val);
		}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_L2_SRC_PORT_PERMITr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_L2_SRC_PORT_PERMIT[%d]:0x%x\n",i,(val>>i)&0x1);
		}
		for(i=0; i<RTK_RG_MAX_EXT_PORT-1; i++)
		{
			reg_array_field_read(APOLLOMP_L2_SRC_EXT_PERMITr, REG_ARRAY_INDEX_NONE,i, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_L2_SRC_EXT_PERMIT[%d]:0x%x\n",i,(val>>(i-1))&0x1);
		}
	
		//Port Security regs
		rtlglue_printf("\n-----Port Security-----\n");
	
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_LRN_LIMITNOr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_NUMf, &val);
			rtlglue_printf("reg_LUT_LRN_LIMITNO[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_LEARN_OVER_CTRLr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_LUT_LEARN_OVER_ACT[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_AGEOUT_CTRLr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_AGEOUT_OUTf, &val);
			rtlglue_printf("reg_LUT_DISABLE_AGE[%d]: %d\n",i,val);
		}
	
	
		reg_field_read(APOLLOMP_LUT_CFGr,  APOLLOMP_LUT_IPMC_HASHf, &val);
		rtlglue_printf("reg_LUT_IPMC_HASH:0x%x\n",val);
	
		reg_field_read(APOLLOMP_LUT_CFGr,  APOLLOMP_LUT_IPMC_LOOKUP_OPf, &val);
	
		rtlglue_printf("reg_LUT_IPMC_LOOKUP_OP:0x%x\n",val);
	
		//Multicast
		rtlglue_printf("\n-----Multicast-----\n");
	
		reg_field_read(APOLLOMP_L34_GLB_CFGr,  APOLLOMP_L34_GLOBAL_CFGf, &val);
		rtlglue_printf("reg_L34_Global_Enable: 0x%x\n",val);
	
		reg_field_read(APOLLOMP_L34_IPMC_TTL_CFGr,	APOLLOMP_IP_MCST_TTL_1f, &val);
		rtlglue_printf("reg_IP_MULTICAST_ROUTE_TTL_1: %d\n",val);
	
		//Unknown Multicast Control
		rtlglue_printf("\n-----Unknown Multicast Control-----\n");
	
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_UNKN_IP4_MCr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNKN_IP4_MC_ACT[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_UNKN_IP6_MCr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNKN_IP6_MC_ACT[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_UNKN_L2_MCr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &val);
			rtlglue_printf("reg_UNKN_L2_MC_ACT[%d]:0x%x\n",i,val);
		}
	
	
		rtlglue_printf("reg_UNKNOWN_MCAST_TRAP_PRIORITY: not found\n");
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_LUT_UNKN_MC_FLOODr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_LUT_UNKN_MC_FLOOD[%d]:0x%x\n",i,val);
		}
	
	
		// QOS-Ingress Priority regs
		rtlglue_printf("\n----- QOS-Ingress Priority-----\n");
	
		for(i=0; i<8; i++)
		{
			reg_array_field_read(APOLLOMP_QOS_1Q_PRI_REMAPr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_INTPRI_1Qf, &val);
			rtlglue_printf("reg_DOT1Q_PRI_PRIORITY[%d]:0x%x\n",i,val);//QOS_1Q_PRI_REMAP
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_QOS_PB_PRIr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_INTPRI_PBf, &val);
			rtlglue_printf("reg_QOS_PORT_PRIORITY[%d]:0x%x\n",i,val);//QOS_PB_PRI
		}
	
		//QOS_DSCP_REMAP
		for(i=0; i<64; i++)
		{
			reg_array_field_read(APOLLOMP_QOS_DSCP_REMAPr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_INTPRI_DSCPf, &val);
			rtlglue_printf("reg_QOS_DSCP_REMAP[%d]:%d\n",i,val);
		}
	
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_PORT_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_PORT_WEIGHT:0x%x\n",val); //PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_DOT1Q_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_1Q_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_DSCP_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_DSCP_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_ACL_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_ACL_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_CVLAN_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_CVLAN_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_LUTFWD_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_LUTFWD_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_SA_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_SA_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr,	APOLLOMP_SVLAN_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_SVLAN_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL
		reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRL2r,  APOLLOMP_L4_WEIGHTf, &val);
		rtlglue_printf("reg_QOS_L4_WEIGHT:0x%x\n",val);//PRI_SEL_TBL_CTRL2
	
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_QOS_PORT_QMAP_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_IDXf, &val);
			rtlglue_printf("reg_PORT_QTABLE_INDEX[%d]:0x%x\n",i,val);//QOS_PRI_REMAP_IN_CPU
		}
	
		for(i=0; i<4; i++)
		{
			for(j=0; j<8; j++)
			{
				reg_array_field_read(APOLLOMP_QOS_INTPRI_TO_QIDr,i,j, APOLLOMP_PRI_TO_QIDf, &val);
				rtlglue_printf("reg_QOS_PRIORITY_TO_QID_TABLE[%d][%d]:0x%x\n",i,j,val);//QOS_INTPRI_TO_QID
			}
		}
	
		// QOS-Remarking
		rtlglue_printf("\n----- QOS-Remarking-----\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_DOT1Q_RMK_EN_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_REMARKING_1Q_PORT_ENABLE[%d]:0x%x\n",i,val);//RMK_DOT1Q_RMK_EN_CTRL
		}
	
		ioal_mem32_read(0x0231CC,&val);
		for(i=0; i<8; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_1Q_CTRLr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_INTPRI_1Qf, &val);
			rtlglue_printf("reg_INTPRI_1QPRI[%d]:0x%x\n",i,val); //RMK_1Q_CTRL
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_DSCP_RMK_EN_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_RMK_DSCP_RMK_EN_CTRL[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_P_DSCP_SELr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_SELf, &val);
			rtlglue_printf("reg_RMK_DSCP_CFG_SEL[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<8; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_DSCP_INT_PRI_CTRLr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_INTPRI_DSCPf, &val);
			rtlglue_printf("reg_RMK_DSCP_INT_PRI_CTRL[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<64; i++)
		{
			reg_array_field_read(APOLLOMP_RMK_DSCP_CTRLr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_INTPRI_DSCPf, &val);
			rtlglue_printf("reg_RMK_DSCP_CTRL[%d]:0x%x\n",i,val);
		}
#endif
	
	//ACL regs
#ifdef CONFIG_APOLLO_RLE0371
		rtlglue_printf("\n----- ACL-----\n");
		retv = reg_field_read(ACL_CFGr, MODEf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_ACL_MODE:0x%x\n", val); /*0: 8 field template mode	 1: 4 field template + 3 field template */

		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(ACL_ENr,i,REG_ARRAY_INDEX_NONE, ENf, &val);
			rtlglue_printf("reg_ACL_EN[%d]:0x%x\n",i,val);	/*per port enable/disable ACL*/
		}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(ACL_PERMITr,i,REG_ARRAY_INDEX_NONE, PERMITf, &val);
			rtlglue_printf("reg_ACL_PERMIT[%d]:0x%x\n",i,val);	/*per port permit/drop frame while ACL rule unhit*/
		}
		
		retv = reg_field_read(CF_CFGr, CF_US_PERMITf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_CF_US_PERMIT:0x%x\n", val); /*per port permit/drop frame while CF rule unhit*/

#else
		rtlglue_printf("\n----- ACL-----\n");
		//ioal_mem32_read(0x01530C,&val);
		//rtlglue_printf("reg_ACL_MODE:0x%x\n",val&0x1); /*0: 8 field template mode   1: 4 field template + 3 field template */
		retv = reg_field_read(APOLLOMP_ACL_CFGr, APOLLOMP_MODEf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_ACL_MODE:0x%x\n", val); /*0: 8 field template mode	 1: 4 field template + 3 field template */
	
	
		//ioal_mem32_read(0x015104,&val);
		//for(i=0; i<MAX_APOLLO_PORT; i++)
		//{
		//	  rtlglue_printf("reg_ACL_EN[%d]:0x%x\n",i,(val>>i)&0x1);/*per port enable/disable ACL*/
		//}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_ACL_ENr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			rtlglue_printf("reg_ACL_EN[%d]:0x%x\n",i,val);	/*per port enable/disable ACL*/
		}
	
		//ioal_mem32_read(0x015108,&val);
		//for(i=0; i<MAX_APOLLO_PORT; i++)
		//{
		//	  rtlglue_printf("reg_ACL_PERMIT[%d]:0x%x\n",i,(val>>i)&0x1);/*per port permit/drop frame while ACL rule unhit*/
		//}
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_ACL_PERMITr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_PERMITf, &val);
			rtlglue_printf("reg_ACL_PERMIT[%d]:0x%x\n",i,val);	/*per port permit/drop frame while ACL rule unhit*/
		}
	
	
		//check hit reason
		//ioal_mem32_read(0x01C0CC,&val);
		//rtlglue_printf("STAT_ACL_REASON(0x01C0CC):0x%x\n",val);
		for(i=0; i<6; i++)
		{
			reg_array_field_read(APOLLOMP_STAT_ACL_REASONr,REG_ARRAY_INDEX_NONE,i, APOLLOMP_ACL_HIT_INFOf, &val);
			rtlglue_printf("STAT_ACL_REASON[%d]:0x%x\n",i,val);
		}
	
		//ioal_mem32_read(0x01C0D0,&val_1);
		//rtlglue_printf("STAT_ACL_REASON(0x01C0D0):0x%x\n",val_1);
	
	//Classification regs
		rtlglue_printf("\n----- Classification-----\n");
		//ioal_mem32_read(0x0150E0,&val);
		//rtlglue_printf("reg_CF_SEL_PON_EN:0x%x\n",(val&0x4)>>2);	/*enable/disable PON port as CF port*/
		retv = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_PON_ENf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_CF_SEL_PON_EN:0x%x\n", val);	/*enable/disable PON port as CF port*/
	
		//ioal_mem32_read(0x0150E0,&val);
		//rtlglue_printf("reg_CF_SEL_RGMII_EN:0x%x\n",(val&0x8)>>3);/*enable/disable RGMII port as CF port*/
		retv = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_RGMII_ENf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_CF_SEL_RGMII_EN:0x%x\n", val);	/*enable/disable RGMII port as CF port*/
	
	
		//ioal_mem32_read(0x0150E0,&val);
		//rtlglue_printf("reg_CF_PERMIT:0x%x\n",(val&0x3)); /*per port permit/drop frame while CF rule unhit*/
		retv = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_US_PERMITf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_CF_US_PERMIT:0x%x\n", val); /*per port permit/drop frame while CF rule unhit*/
#endif
	
	
	//vlan
		rtlglue_printf("\n-----vlan-----\n");
#ifdef CONFIG_APOLLO_RLE0371
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(VLAN_PB_EFIDENr, i, REG_ARRAY_INDEX_NONE, PBFIDENf, &val);
			reg_array_field_read(VLAN_PB_EFIDr, i, REG_ARRAY_INDEX_NONE, PBFIDf, &val2);
			//assert(rtk_vlan_portFid_get(i, &val, &val2) == RT_ERR_OK);
			rtlglue_printf("reg_PORTn_PBFIDEN[port%d]:0x%x,%s\n",i,val2,
				val==0?"DISABLED":"ENABLED");
		}

		for(i=0; i<=6; i++)
		{
			reg_array_field_read(VLAN_EGRESS_TAGr, i, REG_ARRAY_INDEX_NONE, EGRESS_MODEf, &val);
			//assert(rtk_vlan_tagMode_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_EGRESS_MODE[port%d]:0x%x\n",i,val);
		}

		for(i=0; i<=6; i++)
		{
			//bzero(pmsk.bits,sizeof(pmsk.bits));
			reg_array_field_read(VLAN_EGRESS_KEEPr, i, REG_ARRAY_INDEX_NONE, MBRf, &val);
			//assert(rtk_vlan_portEgrTagKeepType_get(i, &pmsk, &val2)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_EGRESS_PORTn_VLAN_KEEP[port%d]:0x%x\n",i,val);
		}
	
		reg_array_field_read(VLAN_CTRLr, REG_ARRAY_INDEX_NONE,REG_ARRAY_INDEX_NONE, VLAN_FILTERINGf, &val);
		//assert(rtk_vlan_vlanFunctionEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_FILTERING:0x%x\n",val);
		reg_array_field_read(VLAN_CTRLr, REG_ARRAY_INDEX_NONE,REG_ARRAY_INDEX_NONE, TRANSPARENT_ENf, &val);
		//assert(rtk_vlan_transparentEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_TRANSPARENT_EN:0x%x\n",val);
		reg_array_field_read(VLAN_CTRLr, REG_ARRAY_INDEX_NONE,REG_ARRAY_INDEX_NONE, CFI_KEEPf, &val);
		//assert(rtk_vlan_cfiKeepEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_CFI_KEEP:0x%x\n",val);
		reg_array_field_read(VLAN_CTRLr, REG_ARRAY_INDEX_NONE,REG_ARRAY_INDEX_NONE, VID_0_TYPEf, &val);
		//assert(rtk_vlan_reservedVidAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_VID0_TYPE:0x%x\n",val);
		reg_array_field_read(VLAN_CTRLr, REG_ARRAY_INDEX_NONE,REG_ARRAY_INDEX_NONE, VID_4095_TYPEf, &val2);
		rtlglue_printf("reg_VLAN_VID4095_TYPE:0x%x\n",val2);
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(VLAN_PORT_ACCEPT_FRAME_TYPEr, i,REG_ARRAY_INDEX_NONE, FRAME_TYPEf, &val);
			//assert(rtk_vlan_portAcceptFrameType_get(i, &val)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORT_ACCEPT_FRAME_TYPE[port%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(VLAN_INGRESSr, i,REG_ARRAY_INDEX_NONE, INGRESSf, &val);
			//assert(rtk_vlan_portIgrFilterEnable_get(i, &val)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_INGRESS[port%d]:0x%x\n",i,val);
		}

	// port-based vlan
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(VLAN_PB_VIDXr, i,REG_ARRAY_INDEX_NONE, VIDXf, &val);
			//assert(rtk_vlan_portPvid_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_VIDX[port%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<5; i++)
		{
			reg_array_field_read(VLAN_EXT_VIDXr, i,REG_ARRAY_INDEX_NONE, VIDXf, &val);
			//assert(rtk_vlan_extPortPvid_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_EXTn_VIDX[extPort%d]:0x%x\n",i,val);
		}
	
	// port-and-protocol-based vlan
	
		for(i=0; i<=3; i++)
		{
			
			reg_array_field_read(VLAN_PPB_VLAN_VALr, REG_ARRAY_INDEX_NONE,i, FRAME_TYPEf, &val);
			//bzero(&protoGroup,sizeof(protoGroup));
			//assert(rtk_vlan_protoGroup_get(i, &protoGroup)== RT_ERR_OK);
			//rtlglue_printf("reg_VLAN_PPBn_FRAME_TYPE[%d]:0x%x\n",i,protoGroup.frametype);
			rtlglue_printf("reg_VLAN_PPBn_FRAME_TYPE[%d]:0x%x\n",i,val);
			reg_array_field_read(VLAN_PPB_VLAN_VALr, REG_ARRAY_INDEX_NONE,i, ETHER_TYPEf, &val);
			//rtlglue_printf("reg_VLAN_PPBn_ETHERTYPE[%d]:0x%x\n",i,protoGroup.framevalue);
			rtlglue_printf("reg_VLAN_PPBn_ETHERTYPE[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<=6; i++) 	//port
		{
			rtlglue_printf("Port %d:\n",i);
			for(j=0; j<=3; j++)
			{
				reg_array_field_read(VLAN_PORT_PPB_VLANr, i,j, VALIDf, &val);
				if(val==0)continue;	//invalid will pass
				//assert(rtk_vlan_portProtoVlan_get(i,j,&ppbCfg)== RT_ERR_OK);
				//rtlglue_printf("	reg_VLAN_PPBn_VALID[%d]:0x%x\n",j,ppbCfg.valid);
				rtlglue_printf("	reg_VLAN_PPBn_VALID[%d]:0x%x\n",j,val);
				reg_array_field_read(VLAN_PORT_PPB_VLANr, i,j, PPB_VIDXf, &val);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_VIDX[%d]:0x%x\n",j,ppbCfg.vid);
				rtlglue_printf("	reg_VLAN_PPBm_PORTn_VIDX[%d]:0x%x\n",j,val);
				reg_array_field_read(VLAN_PORT_PPB_VLANr, i,j, PPB_PRIf, &val);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_PRIORITY[%d]:0x%x\n",j,ppbCfg.pri);
				rtlglue_printf("	reg_VLAN_PPBm_PORTn_PRIORITY[%d]:0x%x\n",j,val);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_DEI[%d]:0x%x\n",j,ppbCfg.dei);
			}
		}
		
#else
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_PB_FIDENr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_PBFIDENf, &val);
			reg_array_field_read(APOLLOMP_VLAN_PB_FIDr, i, REG_ARRAY_INDEX_NONE, APOLLOMP_PBFIDf, &val2);
			//assert(rtk_vlan_portFid_get(i, &val, &val2) == RT_ERR_OK);
			rtlglue_printf("reg_PORTn_PBFIDEN[port%d]:0x%x,%s\n",i,val2,
				val==0?"DISABLED":"ENABLED");
		}
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_EGRESS_TAGr, i,REG_ARRAY_INDEX_NONE, APOLLOMP_EGRESS_MODEf, &val);
			//assert(rtk_vlan_tagMode_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_EGRESS_MODE[port%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<=6; i++)
		{
			//bzero(pmsk.bits,sizeof(pmsk.bits));
			reg_array_field_read(APOLLOMP_VLAN_EGRESS_KEEPr, i,REG_ARRAY_INDEX_NONE, APOLLOMP_MBRf, &val);
			//assert(rtk_vlan_portEgrTagKeepType_get(i, &pmsk, &val2)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_EGRESS_PORTn_VLAN_KEEP[port%d]:0x%x\n",i,val);
		}
	
		reg_field_read(APOLLOMP_VLAN_CTRLr, APOLLOMP_VLAN_FILTERINGf, &val);
		//assert(rtk_vlan_vlanFunctionEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_FILTERING:0x%x\n",val);
		reg_field_read(APOLLOMP_VLAN_CTRLr, APOLLOMP_TRANSPARENT_ENf, &val);
		//assert(rtk_vlan_transparentEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_TRANSPARENT_EN:0x%x\n",val);
		reg_field_read(APOLLOMP_VLAN_CTRLr, APOLLOMP_CFI_KEEPf, &val);
		//assert(rtk_vlan_cfiKeepEnable_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_CFI_KEEP:0x%x\n",val);
		reg_field_read(APOLLOMP_VLAN_CTRLr, APOLLOMP_VID_0_TYPEf, &val);
		//assert(rtk_vlan_reservedVidAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VLAN_VID0_TYPE:0x%x\n",val);
		reg_field_read(APOLLOMP_VLAN_CTRLr, APOLLOMP_VID_4095_TYPEf, &val2);
		rtlglue_printf("reg_VLAN_VID4095_TYPE:0x%x\n",val2);
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_PORT_ACCEPT_FRAME_TYPEr, i,REG_ARRAY_INDEX_NONE, APOLLOMP_FRAME_TYPEf, &val);
			//assert(rtk_vlan_portAcceptFrameType_get(i, &val)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORT_ACCEPT_FRAME_TYPE[port%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_INGRESSr, i,REG_ARRAY_INDEX_NONE, APOLLOMP_INGRESSf, &val);
			//assert(rtk_vlan_portIgrFilterEnable_get(i, &val)== RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_INGRESS[port%d]:0x%x\n",i,val);
		}
	
	// port-based vlan
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_PB_VIDXr, i,REG_ARRAY_INDEX_NONE, APOLLOMP_VIDXf, &val);
			//assert(rtk_vlan_portPvid_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_PORTn_VIDX[port%d]:0x%x\n",i,val);
		}

		//ext-port0 is cpu port, too
		reg_array_field_read(APOLLOMP_VLAN_PB_VIDXr, 6,REG_ARRAY_INDEX_NONE, APOLLOMP_VIDXf, &val);
		//assert(rtk_vlan_portPvid_get(i, &val) == RT_ERR_OK);
		rtlglue_printf("reg_VLAN_EXTn_VIDX[extPort0]:0x%x\n",val);
		for(i=0; i<5; i++)
		{
			reg_array_field_read(APOLLOMP_VLAN_EXT_VIDXr, REG_ARRAY_INDEX_NONE,i, APOLLOMP_VIDXf, &val);
			//assert(rtk_vlan_extPortPvid_get(i, &val) == RT_ERR_OK);
			rtlglue_printf("reg_VLAN_EXTn_VIDX[extPort%d]:0x%x\n",i+1,val);
		}
	
	// port-and-protocol-based vlan
	
		for(i=0; i<=3; i++)
		{
			
			reg_array_field_read(APOLLOMP_VLAN_PPB_VLAN_VALr, REG_ARRAY_INDEX_NONE,i, APOLLOMP_FRAME_TYPEf, &val);
			//bzero(&protoGroup,sizeof(protoGroup));
			//assert(rtk_vlan_protoGroup_get(i, &protoGroup)== RT_ERR_OK);
			//rtlglue_printf("reg_VLAN_PPBn_FRAME_TYPE[%d]:0x%x\n",i,protoGroup.frametype);
			rtlglue_printf("reg_VLAN_PPBn_FRAME_TYPE[%d]:0x%x\n",i,val);
			reg_array_field_read(APOLLOMP_VLAN_PPB_VLAN_VALr, REG_ARRAY_INDEX_NONE,i, APOLLOMP_ETHER_TYPEf, &val);
			//rtlglue_printf("reg_VLAN_PPBn_ETHERTYPE[%d]:0x%x\n",i,protoGroup.framevalue);
			rtlglue_printf("reg_VLAN_PPBn_ETHERTYPE[%d]:0x%x\n",i,val);
		}
	
		for(i=0; i<=6; i++) 	//port
		{
			rtlglue_printf("Port %d:\n",i);
			for(j=0; j<=3; j++)
			{
				reg_array_field_read(APOLLOMP_VLAN_PORT_PPB_VLANr, i,j, APOLLOMP_VALIDf, &val);
				//assert(rtk_vlan_portProtoVlan_get(i,j,&ppbCfg)== RT_ERR_OK);
				//rtlglue_printf("	reg_VLAN_PPBn_VALID[%d]:0x%x\n",j,ppbCfg.valid);
				if(val==0)continue;
				rtlglue_printf("	reg_VLAN_PPBn_VALID[%d]:0x%x\n",j,val);
				reg_array_field_read(APOLLOMP_VLAN_PORT_PPB_VLANr, i,j, APOLLOMP_PPB_VIDXf, &val);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_VIDX[%d]:0x%x\n",j,ppbCfg.vid);
				rtlglue_printf("	reg_VLAN_PPBm_PORTn_VIDX[%d]:0x%x\n",j,val);
				reg_array_field_read(APOLLOMP_VLAN_PORT_PPB_VLANr, i,j, APOLLOMP_PPB_PRIf, &val);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_PRIORITY[%d]:0x%x\n",j,ppbCfg.pri);
				//rtlglue_printf("	reg_VLAN_PPBm_PORTn_DEI[%d]:0x%x\n",j,ppbCfg.dei);
				rtlglue_printf("	reg_VLAN_PPBm_PORTn_PRIORITY[%d]:0x%x\n",j,val);
				rtlglue_printf("	reg_VLAN_PPBm_PORTn_DEI[%d]:0x%x\n",j,0);
			}
		}
#endif

	// svlan
		rtlglue_printf("\n-----svlan-----\n");
#ifdef CONFIG_APOLLO_RLE0371
		reg_field_read(SVLAN_CFGr, VS_TPIDf, &val);
		//assert(rtk_svlan_tpidEntry_get(0,&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_TPID:0x%x\n",val);//SVLAN_CFG

		reg_field_read(SVLAN_CTRLr, VS_PRIf, &val);
		//assert(rtk_svlan_trapPri_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_TRAP_PRI:0x%x\n", val);//VS_TRAP_PRI
	
		rtlglue_printf("reg_VS_PMSK: ");//SVLAN_UPLINK_PMSK
		for(i=0,j=0; i<=6; i++)
		{
			reg_array_field_read(SVLAN_UPLINK_PMSKr,REG_ARRAY_INDEX_NONE,i, ENf, &val);
			//assert(rtk_svlan_servicePort_get(i,&val)== RT_ERR_OK);
			if(val==1)
			{
				j=1;
				rtlglue_printf("%d ",i);
			}
		}
		if(j==0) rtlglue_printf("X\n");
		else rtlglue_printf("\n");//SVLAN_UPLINK_PMSK
	
		reg_field_read(SVLAN_CTRLr, VS_SPRISELf, &val);
		//assert(rtk_svlan_priorityRef_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_SPRISEL:0x%x\n",val);//SVLAN_CTRL
	
		reg_field_read(SVLAN_CTRLr, VS_UNTAGf, &val);
		//assert(rtk_svlan_untagAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VS_UNTAG:0x%x\n",val);//SVLAN_CTRL
	
		reg_field_read(SVLAN_CTRLr, VS_UNTAG_SVIDXf, &val2);
		rtlglue_printf("reg_VS_UNTAG_SVIDX:0x%x\n",val2);//SVLAN_CTRL
	
		reg_field_read(SVLAN_CTRLr, VS_UNMATf, &val);
		//assert(rtk_svlan_unmatchAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VS_UNMAT:0x%x\n",val);//SVLAN_CTRL
	
		reg_field_read(SVLAN_CTRLr, VS_UNMAT_SVIDXf, &val2);
		rtlglue_printf("reg_VS_UNMAT_SVIDX:0x%x\n",val2);//SVLAN_CTRL
	
		reg_field_read(SVLAN_CTRLr, VS_CFI_KEEPf, &val);
		//assert(rtk_svlan_deiKeepState_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_CFI_KEEP:0x%x\n",val);//SVLAN_CTRL
	
		for(i=0; i<6; i++)	 //VS_PORTn_SVIDX[0]~[4]
		{
			reg_array_field_read(SVLAN_P_SVIDXr,REG_ARRAY_INDEX_NONE,i, SVIDXf, &val);
			//ASSERT(rtk_svlan_portSvid_get(i,&val)== RT_ERR_OK);
			//val2 = rtk_svlan_portSvid_get(i,&val);
			//if(val2==RT_ERR_SVLAN_INVALID)
				//rtlglue_printf("Port-based SVALN is invalid\n");
			//else if(val2==RT_ERR_OK)
				rtlglue_printf("reg_VS_PORTn_SVIDX[%d]:0x%x\n",i,val);//SVLAN_P_SVIDX
		}
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(SVLAN_EP_DMAC_CTRLr,i,REG_ARRAY_INDEX_NONE, ENf, &val);
			//assert(rtk_svlan_dmacVidSelState_get(i,&val)== RT_ERR_OK);
			rtlglue_printf("reg_VS_PORTn_DMACVIDSE[%d]:0x%x\n", i, val);
		}
	
#else
		reg_field_read(APOLLOMP_SVLAN_CFGr, APOLLOMP_VS_TPIDf, &val);
		//assert(rtk_svlan_tpidEntry_get(0,&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_TPID:0x%x\n",val);//SVLAN_CFG
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_TYPEf, &val);
		//assert(rtk_svlan_lookupType_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_LOOK_UP_TYPE:0x%x\n", val);//SVLAN_LOOK_UP_TYPE
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_PRIf, &val);
		//assert(rtk_svlan_trapPri_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_TRAP_PRI:0x%x\n", val);//VS_TRAP_PRI
	
		rtlglue_printf("reg_VS_PMSK: ");//SVLAN_UPLINK_PMSK
		for(i=0,j=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_SVLAN_UPLINK_PMSKr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			//assert(rtk_svlan_servicePort_get(i,&val)== RT_ERR_OK);
			if(val==1)
			{
				j=1;
				rtlglue_printf("%d ",i);
			}
		}
		if(j==0) rtlglue_printf("X\n");
		else rtlglue_printf("\n");//SVLAN_UPLINK_PMSK

		reg_field_read(APOLLOMP_SVLAN_CTRLr,APOLLOMP_FORCED_DMACVIDSELf,&val);
		//assert(rtk_svlan_dmacVidSelForcedState_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_FORCED_DMACVIDSEL:0x%x\n", val);
	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SPRISELf, &val);
		//assert(rtk_svlan_priorityRef_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_SPRISEL:0x%x\n",val);//SVLAN_CTRL
	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAGf, &val);
		//assert(rtk_svlan_untagAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VS_UNTAG:0x%x\n",val);//SVLAN_CTRL	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAG_SVIDXf, &val2);
		rtlglue_printf("reg_VS_UNTAG_SVIDX:0x%x\n",val2);//SVLAN_CTRL
	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMATf, &val);
		//assert(rtk_svlan_unmatchAction_get(&val,&val2)== RT_ERR_OK);
		rtlglue_printf("reg_VS_UNMAT:0x%x\n",val);//SVLAN_CTRL	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMAT_SVIDXf, &val2);
		rtlglue_printf("reg_VS_UNMAT_SVIDX:0x%x\n",val2);//SVLAN_CTRL
		
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SP2C_UNMATf, &val);
		//assert(rtk_svlan_sp2cUnmatchCtagging_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_SP2C_UNMAT:0x%x\n",val);//SVLAN_CTRL
	
		reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_DEI_KEEPf, &val);
		//assert(rtk_svlan_deiKeepState_get(&val)== RT_ERR_OK);
		rtlglue_printf("reg_VS_DEI_KEEP:0x%x\n",val);//SVLAN_CTRL
	
		for(i=0; i<6; i++)	 //VS_PORTn_SVIDX[0]~[4]
		{
			reg_array_field_read(APOLLOMP_SVLAN_P_SVIDXr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &val);
			//ASSERT(rtk_svlan_portSvid_get(i,&val)== RT_ERR_OK);
			//val2 = rtk_svlan_portSvid_get(i,&val);
			//if(val2==RT_ERR_SVLAN_INVALID)
				//rtlglue_printf("Port-based SVALN is invalid\n");
			//else if(val2==RT_ERR_OK)
				rtlglue_printf("reg_VS_PORTn_SVIDX[%d]:0x%x\n",i,val);//SVLAN_P_SVIDX
		}
	
		for(i=0; i<=6; i++)
		{
			reg_array_field_read(APOLLOMP_SVLAN_EP_DMAC_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val);
			//assert(rtk_svlan_dmacVidSelState_get(i,&val)== RT_ERR_OK);
			rtlglue_printf("reg_VS_PORTn_DMACVIDSE[%d]:0x%x\n", i, val);
		}

#endif

#ifdef CONFIG_APOLLO_RLE0371
#else
	rtlglue_printf("\n-----binding-----\n");
	
	//Binding
		retv = reg_field_read(APOLLOMP_V6_BD_CTLr, APOLLOMP_PB_ENf, &val);
		//retv = rtk_l34_globalState_get(L34_GLOBAL_BIND_STATE,&val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_V6_BD_CTL:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L2L3f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L2L3, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L2L3:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L2L34f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L2L34, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L2L34:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L3L2f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L3L2, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L3L2:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L3L34f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L3L34, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L3L34:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L34L2f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L34L2, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L34L2:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L34L3f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L34L3, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L34L3:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_UNMATCHED_L3L3f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_UNMATCHED_L3L3, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_UNMATCHED_L3L3:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_CUSTOMIZED_L2f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_CUSTOMIZED_L2, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_CUSTOMIZED_L2:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_CUSTOMIZED_L3f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_CUSTOMIZED_L3, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_CUSTOMIZED_L3:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_BD_CFGr, APOLLOMP_WAN_BINDING_CUSTOMIZED_L34f, &val);
		//retv = rtk_l34_bindingAction_get(L34_BIND_CUSTOMIZED_L34, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_BINDING_WAN_BINDING_CUSTOMIZED_L34:0x%x\n", val);
#endif

#ifdef CONFIG_APOLLO_RLE0371
#else	
	// IPv6
		rtlglue_printf("\n-----IPv6-----\n");
	
		//retv = reg_field_read(APOLLOMP_V6_BD_CTLr, APOLLOMP_PB_ENf, &val);		//FIXME:no RTK api
		//assert(retv == RT_ERR_OK);
		//rtlglue_printf("reg_IPV6_PB_EN:0x%x\n", val);		//binding control bit
	
		retv = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_TTL_1ENABLEf, &val);	//FIXME:no RTK api
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_IPV6_TTL_1_EN:0x%x\n", val);		//TTL-1 control bit
#endif

#ifdef CONFIG_APOLLO_RLE0371
#else	
	// Port Isolation
		rtlglue_printf("\n-----Port Isolation-----\n");
	
		retv = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_CTAG_SELf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_ISOLATION_CTAG_SEL:0x%x\n", val);
	
		retv = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_L34_SELf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_ISOLATION_L34_SEL:0x%x\n", val);
	
		rtlglue_printf("\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_PISO_P_MODE0_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &val);
			rtlglue_printf("reg_ISOLATION_PORT_MODE0[%d]:0x%x\n",i,val);
		}
	
		rtlglue_printf("\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_PISO_P_MODE1_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &val);
			rtlglue_printf("reg_ISOLATION_PORT_MODE1[%d]:0x%x\n",i,val);
		}
	
		rtlglue_printf("\n");
	
		for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_PISO_EXT_MODE0_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &val);
			rtlglue_printf("reg_ISOLATION_EXTPORT_MODE0[%d]:0x%x\n",i,val);
		}
	
		rtlglue_printf("\n");
	
		for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
		{
			reg_array_field_read(APOLLOMP_PISO_EXT_MODE1_CTRLr,i,REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &val);
			rtlglue_printf("reg_ISOLATION_EXTPORT_MODE1[%d]:0x%x\n",i,val);
		}
	
		//rtlglue_printf("reg_ISOLATION_EFID[MAX_APOLLO_PORT]:0x%x\n");
	
	
	// ALE34
		rtlglue_printf("\n-----ALE34-----\n");
		//rtlglue_printf("reg_NAT_CTRL:0x%x\n");
	
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP0f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P0]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP1f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P1]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP2f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P2]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP3f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P3]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP4f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P4]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFPr, APOLLOMP_INTP5f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_PORT_NETIF_MAPPING[P5]:0x%x\n",val);
	
		rtlglue_printf("\n");
	
		retv = reg_field_read(APOLLOMP_NIFEPr, APOLLOMP_INTEXTP0f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_EXTPORT_NETIF_MAPPING[EP0]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFEPr, APOLLOMP_INTEXTP1f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_EXTPORT_NETIF_MAPPING[EP1]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFEPr, APOLLOMP_INTEXTP2f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_EXTPORT_NETIF_MAPPING[EP2]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFEPr, APOLLOMP_INTEXTP3f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_EXTPORT_NETIF_MAPPING[EP3]:0x%x\n",val);
		retv = reg_field_read(APOLLOMP_NIFEPr, APOLLOMP_INTEXTP4f, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_EXTPORT_NETIF_MAPPING[EP4]:0x%x\n",val);
	
		rtlglue_printf("\n");
	
		retv = reg_field_read(APOLLOMP_HSBA_CTRLr, APOLLOMP_TST_LOG_MDf, &val);
		assert(retv == RT_ERR_OK);
		rtlglue_printf("reg_NAT_HSBA_TST_LOG_MODE:0x%x\n",val);
	
		rtlglue_printf("\n");
	
		//rtlglue_printf("reg_VS_FIDEN:0x%x\n");
		//rtlglue_printf("reg_VS_FID:0x%x\n");
	
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{
			for(j=0; j<8; j++)
			{
				reg_array_field_read(APOLLOMP_L34_PORT_TO_WANr,i,j, APOLLOMP_PORT_TO_WAN_PERMITf, &val);
				rtlglue_printf("reg_L34_PORT[%d]_TO_WAN[%d]:0x%x\n",i,j,val);
			}
			rtlglue_printf("\n");
		}
	
		for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
		{
			for(j=0; j<8; j++)
			{
				reg_array_field_read(APOLLOMP_L34_EXTPORT_TO_WANr,i,j, APOLLOMP_EXTPORT_TO_WAN_PERMITf, &val);
				rtlglue_printf("reg_L34_EXTPORT[%d]_TO_WAN[%d]:0x%x\n",i,j,val);
			}
			rtlglue_printf("\n");
		}
	
		rtlglue_printf("\n");
	
		for(j=0; j<8; j++)
		{
			for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
			{
				reg_array_field_read(APOLLOMP_L34_WAN_TO_PORTr,i,j, APOLLOMP_WAN_TO_PORT_PERMITf, &val);
				rtlglue_printf("reg_L34_WAN[%d]_TO_PORT[%d]:0x%x\n",j,i,val);
			}
			rtlglue_printf("\n");
		}
	
		rtlglue_printf("\n");
	
		for(j=0; j<8; j++)
		{
			for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
			{
				reg_array_field_read(APOLLOMP_L34_WAN_TO_EXTPORTr,i,j, APOLLOMP_WAN_TO_EXTPORT_PERMITf, &val);
				rtlglue_printf("reg_L34_WAN[%d]_TO_EXTPORT[%d]:0x%x\n",j,i,val);
			}
			rtlglue_printf("\n");
		}
	
#endif
	
#endif
		return SUCCESS;
	
}

int32 dump_l34_bind_table (void)
{
#ifndef CONFIG_APOLLO_RLE0371
	rtk_binding_entry_t entry;
	uint32 idx;

	rtlglue_printf(">>ASIC L34 Binding Table:\n");
	for(idx=0; idx<MAX_BIND_HW_TABLE_SIZE; idx++)
	{
		if (rtk_l34_bindingTable_get(idx, &entry)!= RT_ERR_OK)
			continue;
		rtlglue_printf("  [%d]	pmask(%02x) epmask(%02x) vid(%d) wt_idx(%d) bindPtl (%d)\n",
					   idx,
					   entry.portMask.bits[0],
					   entry.extPortMask.bits[0],
					   entry.vidLan,
					   entry.wanTypeIdx,
					   entry.bindProto
					  );
	}
#endif
    return SUCCESS;
}
int32 dump_l34_wantype_table (void)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
	rtk_wanType_entry_t entry;
	uint32 idx;

	rtlglue_printf(">>ASIC L34 WAN Type Table:\n");
	for(idx=0; idx<MAX_WANTYPE_HW_TABLE_SIZE; idx++)
	{
		if (rtk_l34_wanTypeTable_get(idx, &entry)!= RT_ERR_OK)
			continue;
		switch(entry.wanType)
		{         
			case L34_WAN_TYPE_L2_BRIDGE:
			case L34_WAN_TYPE_L3_ROUTE:
				rtlglue_printf("  [%d]	wanType(%s) nexthopIdx(%d)\n",
					   idx,
					   entry.wanType==L34_WAN_TYPE_L2_BRIDGE?"L2_BRIDGE":"L3_ROUTE",
					   entry.nhIdx
					  );
				break;
			case L34_WAN_TYPE_L34NAT_ROUTE:
			case L34_WAN_TYPE_L34_CUSTOMIZED:
				rtlglue_printf("  [%d]	wanType(%s) nexthopIdx(%d)\n",
					   idx,
					   entry.wanType==L34_WAN_TYPE_L34NAT_ROUTE?"L34NAT_ROUTE":"L34_CUSTOMIZED",
					   entry.nhIdx
					  );
				break;
			default:
				break;
		}
	}
#endif
    return SUCCESS;
}
int32 dump_ipv6_route_table (void)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
	rtk_ipv6Routing_entry_t entry;
	uint32 idx;

	rtlglue_printf(">>ASIC IPv6 Routing Table:\n");
	for(idx=0; idx<MAX_IPV6_ROUTING_HW_TABLE_SIZE; idx++)
	{
		if (rtk_l34_ipv6RoutingTable_get(idx, &entry) != RT_ERR_OK)
			continue;
		if(entry.valid!=TRUE)
			continue;

		switch(entry.type)
		{
			case L34_IPV6_ROUTE_TYPE_TRAP:
				rtlglue_printf("  [%d]	valid type(TRAP)",
					   idx);
				break;
    		case L34_IPV6_ROUTE_TYPE_DROP:
				rtlglue_printf("  [%d]	valid type(DROP)",
									   idx);
				break;
    		case L34_IPV6_ROUTE_TYPE_LOCAL:
				rtlglue_printf("  [%d]	valid type(LOCAL) DNET_IF(%d)",
									   idx,entry.nhOrIfidIdx);
				break;
    		case L34_IPV6_ROUTE_TYPE_GLOBAL:
				rtlglue_printf("  [%d]	valid type(GLOBAL) NEXTHOP(%d)",
									   idx,entry.nhOrIfidIdx);
				break;
			default:
				break;
		}
		rtlglue_printf(" prefixLen(%d) DIP(%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x) %s\n",
					   entry.ipv6PrefixLen,
					   entry.ipv6Addr.ipv6_addr[0],entry.ipv6Addr.ipv6_addr[1],entry.ipv6Addr.ipv6_addr[2],entry.ipv6Addr.ipv6_addr[3],
					   entry.ipv6Addr.ipv6_addr[4],entry.ipv6Addr.ipv6_addr[5],entry.ipv6Addr.ipv6_addr[6],entry.ipv6Addr.ipv6_addr[7],
					   entry.ipv6Addr.ipv6_addr[8],entry.ipv6Addr.ipv6_addr[9],entry.ipv6Addr.ipv6_addr[10],entry.ipv6Addr.ipv6_addr[11],
					   entry.ipv6Addr.ipv6_addr[12],entry.ipv6Addr.ipv6_addr[13],entry.ipv6Addr.ipv6_addr[14],entry.ipv6Addr.ipv6_addr[15],
					   (entry.rt2waninf==TRUE? "RT2WAN": "RT2LAN")
					  );
	}
#endif

    return SUCCESS;
}
int32 dump_ipv6_neighbor_table (void)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
	rtk_ipv6Neighbor_entry_t entry;
	uint32 idx;

	rtlglue_printf(">>ASIC IPv6 Neighbor Table:\n");
	for(idx=0; idx<MAX_IPV6_NEIGHBOR_HW_TABLE_SIZE; idx++)
	{
		if (rtk_l34_ipv6NeighborTable_get(idx, &entry) != RT_ERR_OK)
			continue;
		if (entry.valid==FALSE)
			continue;
		rtlglue_printf("  [%d]	valid L2Idx(%d) routingIdx(%d) InterfaceID(%02x%02x:%02x%02x:%02x%02x:%02x%02x)\n",
					   idx, entry.l2Idx,
					   entry.ipv6RouteIdx,
#if 0
					   (uint32)((entry.ipv6Ifid&0xff00000000000000)>>56),
					   (uint32)((entry.ipv6Ifid&0xff000000000000)>>48),
					   (uint32)((entry.ipv6Ifid&0xff0000000000)>>40),
					   (uint32)((entry.ipv6Ifid&0xff00000000)>>32),
#else
					   (uint32)((entry.ipv6Ifid>>56)&0xff),
					   (uint32)((entry.ipv6Ifid>>48)&0xff),
					   (uint32)((entry.ipv6Ifid>>40)&0xff),
					   (uint32)((entry.ipv6Ifid>>32)&0xff),
#endif
					   (uint32)((entry.ipv6Ifid&0xff000000)>>24),
					   (uint32)((entry.ipv6Ifid&0xff0000)>>16),
					   (uint32)((entry.ipv6Ifid&0xff00)>>8),
					   (uint32)((entry.ipv6Ifid&0xff))
					  );
	}
#endif

    return SUCCESS;
}

int32 mibdump_ipv6_neighbor_table (void)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
	rtk_ipv6Neighbor_entry_t *entry;
	uint32 idx;

	rtlglue_printf(">>ASIC IPv6 Neighbor Table:\n");
	for(idx=0; idx<MAX_IPV6_NEIGHBOR_HW_TABLE_SIZE; idx++)
	{
		entry=&rg_db.v6neighbor[idx].rtk_v6neighbor;
		if (entry->valid==FALSE)
			continue;
		rtlglue_printf("  [%d]	valid L2Idx(%d) routingIdx(%d) InterfaceID(%02x%02x:%02x%02x:%02x%02x:%02x%02x)\n",
					   idx, entry->l2Idx,
					   entry->ipv6RouteIdx,
#if 0
					   (uint32)((entry.ipv6Ifid&0xff00000000000000)>>56),
					   (uint32)((entry.ipv6Ifid&0xff000000000000)>>48),
					   (uint32)((entry.ipv6Ifid&0xff0000000000)>>40),
					   (uint32)((entry.ipv6Ifid&0xff00000000)>>32),
#else
					   (uint32)((entry->ipv6Ifid>>56)&0xff),
					   (uint32)((entry->ipv6Ifid>>48)&0xff),
					   (uint32)((entry->ipv6Ifid>>40)&0xff),
					   (uint32)((entry->ipv6Ifid>>32)&0xff),
#endif
					   (uint32)((entry->ipv6Ifid&0xff000000)>>24),
					   (uint32)((entry->ipv6Ifid&0xff0000)>>16),
					   (uint32)((entry->ipv6Ifid&0xff00)>>8),
					   (uint32)((entry->ipv6Ifid&0xff))
					  );
	}
#endif

    return SUCCESS;
}

int32 dump_piso (void)
{
	int i,j,ret=0;
	rtk_portmask_t mbr= {{0}},extmbr= {{0}};
	rtlglue_printf(">>Port Isolation:\n");
	for(j=0; j<RTK_PORT_ISO_CFG_END; j++)
	{
		rtlglue_printf("  - mode %d - \n", j);
		for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
		{

		 rtlglue_printf("\t[Phy port %d:]", i);
		 ret = rtk_port_isolationEntry_get(j,i,&mbr,&extmbr);
		 rtlglue_printf("\tPhy Port Mask:0x%x",mbr.bits[0]);
		 rtlglue_printf("\tExtension Port Mask:0x%x\n",extmbr.bits[0]);

		}
		for(i=0; i<RTK_RG_MAX_EXT_PORT; i++)
		{

		 rtlglue_printf("\t[Ext. port %d:]", i);
		 ret = rtk_port_isolationEntryExt_get(j,i,&mbr,&extmbr);
		 rtlglue_printf("\tPhy Port Mask:0x%x",mbr.bits[0]);
		 rtlglue_printf("\tExtension Port Mask:0x%x\n",extmbr.bits[0]);

		}
	}

    return SUCCESS;
}

int32 dump_4kVlan(void)
{

	int i,ret=0;
	rtk_portmask_t mbr,untagset;
	rtlglue_printf(">>ASIC VLAN Table:\n\n");
	for(i=0; i<MAX_VLAN_HW_TABLE_SIZE; i++)
	{
		mbr.bits[0]=0;
		untagset.bits[0]=0;
		ret = rtk_vlan_port_get(i,&mbr,&untagset);
		if(mbr.bits[0] == 0) continue;

		rtlglue_printf("  VID[%d] ", i);

#if 1
		rtlglue_printf("\n\tMember Ports:0x%x",mbr.bits[0]);
		rtk_vlan_extPort_get(i, &mbr);
		rtlglue_printf("\n\tExtension Member Ports:0x%x",mbr.bits[0]);
		rtlglue_printf("\n\tUntag Member Ports:0x%x\n",untagset.bits[0]);

#else

		rtlglue_printf("\n\tmember ports:");
		for(j=0; j<32; j++)
			if(mbr.bits[0] & (1<<j))
				rtlglue_printf("%d ", j);
		rtlglue_printf("\t\tUntag member ports:");
		for(j=0; j<32; j++)
			if(untagset.bits[0] & (1<<j))
				rtlglue_printf("%d ", j);

		rtlglue_printf("\t\tVID:\t%d\n",i);
#endif

	}

	return SUCCESS;
}


int32 dump_wlan_mbssid(void)
{
	int i;
	printk("IDX\tMAC\t\t\tINTF\n");
	for(i=rg_db.wlanMbssidHeadIdx;i<rg_db.wlanMbssidHeadIdx+MAX_WLAN_MBSSID_SW_TABLE_SIZE;i++)
	{
		int idx=i%MAX_WLAN_MBSSID_SW_TABLE_SIZE;
		if(memcmp(rg_db.wlanMbssid[idx].mac.octet,"\x0\x0\x0\x0\x0\x0",6)==0) continue;
		printk("%02d\t%02X:%02X:%02X:%02X:%02X:%02X\t%d\n",idx,
			rg_db.wlanMbssid[idx].mac.octet[0],
			rg_db.wlanMbssid[idx].mac.octet[1],
			rg_db.wlanMbssid[idx].mac.octet[2],
			rg_db.wlanMbssid[idx].mac.octet[3],
			rg_db.wlanMbssid[idx].mac.octet[4],
			rg_db.wlanMbssid[idx].mac.octet[5],
			rg_db.wlanMbssid[idx].wlan_dev_idx);		

	}
	return SUCCESS;
}




void dump_packet(u8 *pkt,u32 size,char *memo)
{
    int off;
    u8 protocol=0;
    int i;
    int pppoeif=0;
    for(i=0; i<78; i++)
        rtlglue_printf("=");
    rtlglue_printf("\n");

    if(size==0)
    {
        rtlglue_printf("%s\npacket_length=0\n",memo);
        return;
    }

    memDump(pkt,size,memo);
    rtlglue_printf("\n" COLOR_Y "DA" COLOR_NM ":[%02X-%02X-%02X-%02X-%02X-%02X]\t" COLOR_Y "SA" COLOR_NM ":[%02X-%02X-%02X-%02X-%02X-%02X]\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5]
                   ,pkt[6],pkt[7],pkt[8],pkt[9],pkt[10],pkt[11]);
    off=12;
    if((pkt[off]==0x88)&&(pkt[off+1]==0x99))
    {

        if(((pkt[off+8]==0x88)&&(pkt[off+9]==0xa8))||((pkt[off+8]==0x81)&&(pkt[off+9]==0x00))||((pkt[off+8]==0x88)&&((pkt[off+9]==0x63)||(pkt[off+9]==0x64)))||
                ((pkt[off+8]==0x86)&&(pkt[off+9]==0xdd))||((pkt[off]==0x08)&&(pkt[off+1]==0x00)))
        {
            //TO CPU
            rtlglue_printf("CPU:[" COLOR_Y "Protocol" COLOR_NM "=%d][" COLOR_Y "Res" COLOR_NM "=0x%x][" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "TTL_1" COLOR_NM "=0x%x][" COLOR_Y "L3R" COLOR_NM "=%d][" COLOR_Y "ORG" COLOR_NM "=%d][" COLOR_Y "SPA" COLOR_NM "=%d][" COLOR_Y "EPMSK" COLOR_NM "=0x%x]\n"
                           ,pkt[off+2],pkt[off+3],pkt[off+4]>>5,pkt[off+4]&0x1f
                           ,pkt[off+5]>>7,(pkt[off+5]>>6)&1,pkt[off+5]&7,pkt[off+7]&0x3f);
            off+=8;
        }
        else
        {
            //FROM CPU
            rtlglue_printf("CPU:[" COLOR_Y "Proto" COLOR_NM "=%d][" COLOR_Y "L3CS" COLOR_NM "=%d][" COLOR_Y "L4CS" COLOR_NM "=%d][" COLOR_Y "TxPortMask" COLOR_NM "=0x%x][" COLOR_Y "EFID_EN" COLOR_NM "=%d][" COLOR_Y "EFID" COLOR_NM "=%d][" COLOR_Y "Priority" COLOR_NM "=%d]\n"
                           ,pkt[off+2],(pkt[off+3]>>7)&1,(pkt[off+3]>>6)&1,pkt[off+3]&0x3f,pkt[off+4]>>5,(pkt[off+4]>>3)&3,pkt[off+4]&7);
            rtlglue_printf("    [" COLOR_Y "Keep" COLOR_NM "=%d][" COLOR_Y "VSEL" COLOR_NM "=%d][" COLOR_Y "DisLrn" COLOR_NM "=%d][" COLOR_Y "PSEL" COLOR_NM "=%d][" COLOR_Y "Rsv1" COLOR_NM "=%d][" COLOR_Y "Rsv0" COLOR_NM "=%d][" COLOR_Y "L34Keep" COLOR_NM "=%d][" COLOR_Y "QSEL" COLOR_NM "=%d]\n"
                           ,pkt[off+5]>>7,(pkt[off+5]>>6)&1,(pkt[off+5]>>5)&1,(pkt[off+5]>>4)&1,(pkt[off+5]>>3)&1,(pkt[off+5]>>2)&1,(pkt[off+5]>>1)&1,pkt[off+5]&1);
            rtlglue_printf("    [" COLOR_Y "ExtSPA" COLOR_NM "=%d][" COLOR_Y "PPPoEAct" COLOR_NM "=%d][" COLOR_Y "PPPoEIdx" COLOR_NM "=%d][" COLOR_Y "L2BR" COLOR_NM "=%d][" COLOR_Y "QID" COLOR_NM "=%d]\n"
                           ,(pkt[off+6]>>5)&7,(pkt[off+6]>>3)&3,pkt[off+6]&7,(pkt[off+7]>>7)&1,pkt[off+6]&0x7f);
            off+=12;
        }
    }

    if((pkt[off]==0x88)&&(pkt[off+1]==0xa8))
    {
        rtlglue_printf("SVLAN:[" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "DEI" COLOR_NM "=%d][" COLOR_Y "VID" COLOR_NM "=%d]\n",pkt[off+2]>>5,(pkt[off+2]>>4)&1,((pkt[off+2]&0xf)<<8)|(pkt[off+3]));
        off+=4;
    }

    if((pkt[off]==0x81)&&(pkt[off+1]==0x00))
    {
        rtlglue_printf("CVLAN:[" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "CFI" COLOR_NM "=%d][" COLOR_Y "VID" COLOR_NM "=%d]\n",pkt[off+2]>>5,(pkt[off+2]>>4)&1,((pkt[off+2]&0xf)<<8)|(pkt[off+3]));
        off+=4;
    }

    if((pkt[off]==0x88)&&((pkt[off+1]==0x63)||(pkt[off+1]==0x64))) //PPPoE
    {
        rtlglue_printf("PPPoE:[" COLOR_Y "Code" COLOR_NM "=0x%02x][" COLOR_Y "SessionID" COLOR_NM "=0x%04x]\n",
                       pkt[off+3],((u32)pkt[off+4]<<8)|pkt[off+5]);
        off+=8;
        pppoeif=1;
    }

    if(((pkt[off]==0x86)&&(pkt[off+1]==0xdd)) || ((pkt[off]==0x00)&&(pkt[off+1]==0x57)))		//IPv6 or IPv6 with PPPoE
    {
        rtlglue_printf("IPv6:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "TC" COLOR_NM "=%02x][" COLOR_Y "FL" COLOR_NM "=%02x%02x%x][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "HopLimit" COLOR_NM "=%d]\n"
                       ,pkt[off+2]>>4, (pkt[off+2]&0xf)+(pkt[off+3]>>4), (pkt[off+3]&0xf)+(pkt[off+4]>>4), (pkt[off+4]&0xf)+(pkt[off+5]>>4), (pkt[off+5]&0xf), (pkt[off+6]<<8)+pkt[off+7], pkt[off+8], pkt[off+9]);
        rtlglue_printf("     [" COLOR_Y "SIP" COLOR_NM "=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]\n"
                       ,pkt[off+10], pkt[off+11], pkt[off+12], pkt[off+13], pkt[off+14], pkt[off+15], pkt[off+16], pkt[off+17]
                       ,pkt[off+18], pkt[off+19], pkt[off+20], pkt[off+21], pkt[off+22], pkt[off+23], pkt[off+24], pkt[off+25]);
        rtlglue_printf("     [" COLOR_Y "DIP" COLOR_NM "=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]\n"
                       ,pkt[off+26], pkt[off+27], pkt[off+28], pkt[off+29], pkt[off+30], pkt[off+31], pkt[off+32], pkt[off+33]
                       ,pkt[off+34], pkt[off+35], pkt[off+36], pkt[off+37], pkt[off+38], pkt[off+39], pkt[off+40], pkt[off+41]);

        protocol=pkt[off+8];
        if(protocol==0)	//hop-by-hop
        {
            rtlglue_printf("Hop-By-Hop:[" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "Length" COLOR_NM "=%d]\n"
                           ,pkt[off+42], pkt[off+43]);
            rtlglue_printf("          [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x]\n"
                           ,pkt[off+44], pkt[off+45], pkt[off+46], pkt[off+47], pkt[off+48], pkt[off+49]);
            for(i=0; i<pkt[off+43]; i++)
            {
                rtlglue_printf("         [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x %02x %02x]\n"
                               ,pkt[off+50+i*8], pkt[off+51+i*8], pkt[off+52+i*8], pkt[off+53+i*8]
                               ,pkt[off+54+i*8], pkt[off+55+i*8], pkt[off+56+i*8], pkt[off+57+i*8]);
            }

            protocol=pkt[off+42];
            off+=(50+pkt[off+43]*8);
        }
        else
            off+=42;
    }

    if(((pkt[off]==0x08)&&(pkt[off+1]==0x00))||((pkt[off]==0x00)&&(pkt[off+1]==0x21)))
    {
        rtlglue_printf("IPv4:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "HLen" COLOR_NM "=%d][" COLOR_Y "TOS" COLOR_NM "=%d(DSCP=%d)][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "ID" COLOR_NM "=%d][" COLOR_Y "R" COLOR_NM "=%d," COLOR_Y "DF" COLOR_NM "=%d," COLOR_Y "MF" COLOR_NM "=%d]\n"
                       ,pkt[off+2]>>4,(pkt[off+2]&0xf)*4,pkt[off+3],pkt[off+3]>>2,(pkt[off+4]<<8)|pkt[off+5],(pkt[off+6]<<8)|pkt[off+7]
                       ,(pkt[off+8]>>7)&1,(pkt[off+8]>>6)&1,(pkt[off+8]>>5)&1);
        rtlglue_printf("     [" COLOR_Y "FrgOff" COLOR_NM "=%d][" COLOR_Y "TTL" COLOR_NM "=%d][" COLOR_Y "PROTO" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n"
                       ,((pkt[off+8]&0x1f)<<8)|pkt[off+9],pkt[off+10],pkt[off+11],(pkt[off+12]<<8)|pkt[off+13]);
        rtlglue_printf("     [" COLOR_Y "SIP" COLOR_NM "=%d.%d.%d.%d][" COLOR_Y "DIP" COLOR_NM "=%d.%d.%d.%d]\n"
                       ,pkt[off+14],pkt[off+15],pkt[off+16],pkt[off+17],pkt[off+18],pkt[off+19],pkt[off+20],pkt[off+21]);

        protocol=pkt[off+11];
        off+=(pkt[off+2]&0xf)*4+2;
    }

    if(protocol==0x6) //TCP
    {
        rtlglue_printf("TCP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Seq" COLOR_NM "=0x%x][" COLOR_Y "Ack" COLOR_NM "=0x%x][" COLOR_Y "HLen" COLOR_NM "=%d]\n"
                       ,(pkt[off]<<8)|(pkt[off+1]),(pkt[off+2]<<8)|(pkt[off+3]),(pkt[off+4]<<24)|(pkt[off+5]<<16)|(pkt[off+6]<<8)|(pkt[off+7]<<0)
                       ,(pkt[off+8]<<24)|(pkt[off+9]<<16)|(pkt[off+10]<<8)|(pkt[off+11]<<0),pkt[off+12]>>4<<2);
        rtlglue_printf("    [" COLOR_Y "URG" COLOR_NM "=%d][" COLOR_Y "ACK" COLOR_NM "=%d][" COLOR_Y "PSH" COLOR_NM "=%d][" COLOR_Y "RST" COLOR_NM "=%d][" COLOR_Y "SYN" COLOR_NM "=%d][" COLOR_Y "FIN" COLOR_NM "=%d][" COLOR_Y "Win" COLOR_NM "=%d]\n"
                       ,(pkt[off+13]>>5)&1,(pkt[off+13]>>4)&1,(pkt[off+13]>>3)&1,(pkt[off+13]>>2)&1,(pkt[off+13]>>1)&1,(pkt[off+13]>>0)&1
                       ,(pkt[off+14]<<8)|pkt[off+15]);
        rtlglue_printf("    [" COLOR_Y "CHM" COLOR_NM "=0x%x][" COLOR_Y "Urg" COLOR_NM "=0x%x]\n",(pkt[off+16]<<8)|(pkt[off+17]<<0),(pkt[off+18]<<8)|(pkt[off+19]<<0));
    }
    else if(protocol==0x11) //UDP
    {
        rtlglue_printf("UDP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n",(pkt[off]<<8)|(pkt[off+1]),(pkt[off+2]<<8)|(pkt[off+3])
                       ,(pkt[off+4]<<8)|(pkt[off+5]),(pkt[off+6]<<8)|(pkt[off+7]));

    }
}
void dump_compare_packet(uint8 *output,uint8 *expect,int out_size,int expect_size)
{
    int off;
    u8 protocol=0;
    int i,j,pppoeif=0;
    uint8 *pkt=NULL;
    char tmpstr[32];
    int size=max(out_size,expect_size);


    for(j=1; j<3; j++)
    {
        //if(j==0) {pkt=input; sprintf(tmpstr,"INPUT");}
        if(j==1)
        {
            pkt=output;
            sprintf(tmpstr,"OUTPUT(size=%d)",out_size);
        }
        if(j==2)
        {
            pkt=expect;
            sprintf(tmpstr,"EXPECT(size=%d)",expect_size);
        }

        for(i=0; i<78; i++)
            rtlglue_printf("=");

        rtlglue_printf("\n");

        rtlglue_printf("%s\n",tmpstr);
        if(size==0) return;

        for(i=0; i<size; i++)
        {
            int diff=0;

            if(j==1)
            {
                if((i>=out_size)||(out_size<=4))
                    break;//goto skip_dump;
            }
            if(j==2)
            {
                if((i>=expect_size)||(expect_size<=4))
                    break;//goto skip_dump;
            }
            if((i%16)==0) rtlglue_printf("%08x ",(unsigned int)(unsigned long int)(&pkt[i]));
            //if((input[i]!=output[i])||(output[i]!=expect[i])) diff=1;
            if(i<min(out_size,expect_size-4))
                if(output[i]!=expect[i]) diff=1;
            rtlglue_printf("%s%02x%s ",(diff==1)?COLOR_H:"",(u8)pkt[i],(diff==1)?COLOR_NM:"");
            if((i%16)==7) rtlglue_printf("  ");
            if((i%16)==15) rtlglue_printf("\n");
        }
        if(i%16!=15)
            rtlglue_printf("\n");

        rtlglue_printf(COLOR_Y "DA" COLOR_NM ":[%02X-%02X-%02X-%02X-%02X-%02X]\t" COLOR_Y "SA" COLOR_NM ":[%02X-%02X-%02X-%02X-%02X-%02X]\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5]
                       ,pkt[6],pkt[7],pkt[8],pkt[9],pkt[10],pkt[11]);
        off=12;
        if((pkt[off]==0x88)&&(pkt[off+1]==0x99))
        {
            rtlglue_printf("CPU:[" COLOR_Y "Protocol" COLOR_NM "=%d][" COLOR_Y "Res" COLOR_NM "=0x%x][" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "TTL_1" COLOR_NM "=0x%x][" COLOR_Y "L3R" COLOR_NM "=%d][" COLOR_Y "ORG" COLOR_NM "=%d][" COLOR_Y "SPA" COLOR_NM "=%d][" COLOR_Y "EPMSK" COLOR_NM "=0x%x]\n"
                           ,pkt[off+2],pkt[off+3],pkt[off+4]>>5,pkt[off+4]&0x1f
                           ,pkt[off+5]>>7,(pkt[off+5]>>6)&1,pkt[off+5]&7,pkt[off+7]&0x3f);
            off+=8;
        }

        if((pkt[off]==0x88)&&(pkt[off+1]==0xa8))
        {
            rtlglue_printf("SVLAN:[" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "DEI" COLOR_NM "=%d][" COLOR_Y "VID" COLOR_NM "=%d]\n",pkt[off+2]>>5,(pkt[off+2]>>4)&1,((pkt[off+2]&0xf)<<8)|(pkt[off+3]));
            off+=4;
        }

        if((pkt[off]==0x81)&&(pkt[off+1]==0x00))
        {
            rtlglue_printf("CVLAN:[" COLOR_Y "Pri" COLOR_NM "=%d][" COLOR_Y "CFI" COLOR_NM "=%d][" COLOR_Y "VID" COLOR_NM "=%d]\n",pkt[off+2]>>5,(pkt[off+2]>>4)&1,((pkt[off+2]&0xf)<<8)|(pkt[off+3]));
            off+=4;
        }

        if((pkt[off]==0x88)&&((pkt[off+1]==0x63)||(pkt[off+1]==0x64))) //PPPoE
        {
            rtlglue_printf("PPPoE:[" COLOR_Y "Code" COLOR_NM "=0x%02x][" COLOR_Y "SessionID" COLOR_NM "=0x%04x]\n",
                           pkt[off+3],(pkt[off+4]<<8)|pkt[off+5]);
            off+=8;
            pppoeif=1;
        }

        if(((pkt[off]==0x86)&&(pkt[off+1]==0xdd)) || ((pkt[off]==0x00)&&(pkt[off+1]==0x57)))		//IPv6 or IPv6 with PPPoE
        {
            rtlglue_printf("IPv6:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "TC" COLOR_NM "=%02x][" COLOR_Y "FL" COLOR_NM "=%02x%02x%x][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "HopLimit" COLOR_NM "=%d]\n"
                           ,pkt[off+2]>>4, (pkt[off+2]&0xf)+(pkt[off+3]>>4), (pkt[off+3]&0xf)+(pkt[off+4]>>4), (pkt[off+4]&0xf)+(pkt[off+5]>>4), (pkt[off+5]&0xf), (pkt[off+6]<<8)+pkt[off+7], pkt[off+8], pkt[off+9]);
            rtlglue_printf("     [" COLOR_Y "SIP" COLOR_NM "=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]\n"
                           ,pkt[off+10], pkt[off+11], pkt[off+12], pkt[off+13], pkt[off+14], pkt[off+15], pkt[off+16], pkt[off+17]
                           ,pkt[off+18], pkt[off+19], pkt[off+20], pkt[off+21], pkt[off+22], pkt[off+23], pkt[off+24], pkt[off+25]);
            rtlglue_printf("     [" COLOR_Y "DIP" COLOR_NM "=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]\n"
                           ,pkt[off+26], pkt[off+27], pkt[off+28], pkt[off+29], pkt[off+30], pkt[off+31], pkt[off+32], pkt[off+33]
                           ,pkt[off+34], pkt[off+35], pkt[off+36], pkt[off+37], pkt[off+38], pkt[off+39], pkt[off+40], pkt[off+41]);

            protocol=pkt[off+8];
            if(protocol==0)	//hop-by-hop
            {
                rtlglue_printf("Hop-By-Hop:[" COLOR_Y "NxHdr" COLOR_NM "=%d][" COLOR_Y "Length" COLOR_NM "=%d]\n"
                               ,pkt[off+42], pkt[off+43]);
                rtlglue_printf("          [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x]\n"
                               ,pkt[off+44], pkt[off+45], pkt[off+46], pkt[off+47], pkt[off+48], pkt[off+49]);
                for(i=0; i<pkt[off+43]; i++)
                {
                    rtlglue_printf("         [" COLOR_Y "Option" COLOR_NM "=%02x %02x %02x %02x %02x %02x %02x %02x]\n"
                                   ,pkt[off+50+i*8], pkt[off+51+i*8], pkt[off+52+i*8], pkt[off+53+i*8]
                                   ,pkt[off+54+i*8], pkt[off+55+i*8], pkt[off+56+i*8], pkt[off+57+i*8]);
                }

                protocol=pkt[off+42];
                off+=(50+pkt[off+43]*8);
            }
            else
                off+=42;
        }
        //rtlglue_printf("###############the pkt off before IPV4 is %02x off+1 is %02x, pppoeif is %d\n",pkt[off],pkt[off+1],pppoeif);
        if(((pkt[off]==0x08)&&(pkt[off+1]==0x00)&&(pppoeif==0))||((pkt[off]==0x00)&&(pkt[off+1]==0x21)&&(pppoeif==1)))
        {
            rtlglue_printf("IPv4:[" COLOR_Y "Ver" COLOR_NM "=%d][" COLOR_Y "HLen" COLOR_NM "=%d][" COLOR_Y "TOS" COLOR_NM "=%d(DSCP=%d)][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "ID" COLOR_NM "=%d][" COLOR_Y "R" COLOR_NM "=%d," COLOR_Y "DF" COLOR_NM "=%d," COLOR_Y "MF" COLOR_NM "=%d]\n"
                           ,pkt[off+2]>>4,(pkt[off+2]&0xf)*4,pkt[off+3],pkt[off+3]>>2,(pkt[off+4]<<8)|pkt[off+5],(pkt[off+6]<<8)|pkt[off+7]
                           ,(pkt[off+8]>>7)&1,(pkt[off+8]>>6)&1,(pkt[off+8]>>5)&1);
            rtlglue_printf("	 [" COLOR_Y "FrgOff" COLOR_NM "=%d][" COLOR_Y "TTL" COLOR_NM "=%d][" COLOR_Y "PROTO" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n"
                           ,((pkt[off+8]&0x1f)<<8)|pkt[off+9],pkt[off+10],pkt[off+11],(pkt[off+12]<<8)|pkt[off+13]);
            rtlglue_printf("	 [" COLOR_Y "SIP" COLOR_NM "=%d.%d.%d.%d][" COLOR_Y "DIP" COLOR_NM "=%d.%d.%d.%d]\n"
                           ,pkt[off+14],pkt[off+15],pkt[off+16],pkt[off+17],pkt[off+18],pkt[off+19],pkt[off+20],pkt[off+21]);

            protocol=pkt[off+11];
            off+=(pkt[off+2]&0xf)*4+2;
        }

        if(protocol==0x6) //TCP
        {
            rtlglue_printf("TCP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Seq" COLOR_NM "=0x%x][" COLOR_Y "Ack" COLOR_NM "=0x%x][" COLOR_Y "HLen" COLOR_NM "=%d]\n"
                           ,(pkt[off]<<8)|(pkt[off+1]),(pkt[off+2]<<8)|(pkt[off+3]),(pkt[off+4]<<24)|(pkt[off+5]<<16)|(pkt[off+6]<<8)|(pkt[off+7]<<0)
                           ,(pkt[off+8]<<24)|(pkt[off+9]<<16)|(pkt[off+10]<<8)|(pkt[off+11]<<0),pkt[off+12]>>4<<2);
            rtlglue_printf("	[" COLOR_Y "URG" COLOR_NM "=%d][" COLOR_Y "ACK" COLOR_NM "=%d][" COLOR_Y "PSH" COLOR_NM "=%d][" COLOR_Y "RST" COLOR_NM "=%d][" COLOR_Y "SYN" COLOR_NM "=%d][" COLOR_Y "FIN" COLOR_NM "=%d][" COLOR_Y "Win" COLOR_NM "=%d]\n"
                           ,(pkt[off+13]>>5)&1,(pkt[off+13]>>4)&1,(pkt[off+13]>>3)&1,(pkt[off+13]>>2)&1,(pkt[off+13]>>1)&1,(pkt[off+13]>>0)&1
                           ,(pkt[off+14]<<8)|pkt[off+15]);
            rtlglue_printf("	[" COLOR_Y "CHM" COLOR_NM "=0x%x][" COLOR_Y "Urg" COLOR_NM "=0x%x]\n",(pkt[off+16]<<8)|(pkt[off+17]<<0),(pkt[off+18]<<8)|(pkt[off+19]<<0));
        }
        else if(protocol==0x11) //UDP
        {
            rtlglue_printf("UDP:[" COLOR_Y "SPort" COLOR_NM "=%d][" COLOR_Y "DPort" COLOR_NM "=%d][" COLOR_Y "Len" COLOR_NM "=%d][" COLOR_Y "CHM" COLOR_NM "=0x%x]\n",(pkt[off]<<8)|(pkt[off+1]),(pkt[off+2]<<8)|(pkt[off+3])
                           ,(pkt[off+4]<<8)|(pkt[off+5]),(pkt[off+6]<<8)|(pkt[off+7]));
        }
//skip_dump:
        //      ;
    }
}

void rg_proc_init()
{
	struct proc_dir_entry *p;
	struct proc_dir_entry *proc_root=NULL,*proc_root_mib=NULL;;
	proc_root = proc_mkdir("dump", NULL);	
	proc_root_mib = proc_mkdir("mib", NULL);	

	/* SPI delay */
	(*(volatile unsigned int   *)((unsigned int)0xb8009008))=0xff000000;

	p = create_proc_entry("l4", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_napt;
	}
	
	p = create_proc_entry("napt", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_tcpudp;	
	}	
	p = create_proc_entry("netif", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_netif;		
	}
	p = create_proc_entry("pppoe", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_pppoe;		
	}
	p = create_proc_entry("nh", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_nexthop;		
	}	
	p = create_proc_entry("nexthop", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_nexthop;		
	}
	p = create_proc_entry("arp", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_arp;		
	}
	p = create_proc_entry("ip", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_ip;		
	}
	p = create_proc_entry("l3", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_l3;		
	}
	p = create_proc_entry("l4hs", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_l4hs;		
	}
	p = create_proc_entry("hs", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_hs;		
	}

	p = create_proc_entry("lut", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_lut_table;
	}

	p = create_proc_entry("l2", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_lut_table;
	}

	p = create_proc_entry("ipmcgrp", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_ipmc_group;
	}
	p = create_proc_entry("ipmc", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_ipmc_routing;
	}	

	p = create_proc_entry("vlan", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_vlan_table;
		
	}
	p = create_proc_entry("vlanmbr", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_vlan_mbr;		
	}    
	p = create_proc_entry("svlan", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_svlan_mbr;		
	}
	p = create_proc_entry("c2s", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_svlan_c2s;		
	}
	p = create_proc_entry("mc2s", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_svlan_mc2s;		
	}
	p = create_proc_entry("sp2c", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_svlan_sp2c;		
	}    
	p = create_proc_entry("acl", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl;		
	}
	p = create_proc_entry("acl_rg", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_rg_acl;		
	}	
	p = create_proc_entry("acl_template", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl_template;		
	}
	p = create_proc_entry("acl_vidrange", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl_vidRangeTable;		
	}

	p = create_proc_entry("acl_iprange", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl_ipRangeTable;		
	}
		p = create_proc_entry("acl_portrange", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl_portRangeTable;		
	}
		p = create_proc_entry("acl_pktlenrange", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_acl_pktlenRangeTable;
		
	}
	p = create_proc_entry("cf", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_cf;		
	}
	p = create_proc_entry("reg", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_reg;		
	}

	p = create_proc_entry("bind", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_l34_bind_table;		
	}

	p = create_proc_entry("wantype", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_l34_wantype_table;		
	}

	p = create_proc_entry("v6route", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_ipv6_route_table;
		
	}

	p = create_proc_entry("neighbor", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_ipv6_neighbor_table;		
	}
	
	p = create_proc_entry("portIso", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_piso;		
	}

	//WLAN dump
	p = create_proc_entry("mbssid", S_IRUGO, proc_root);
	if (p){
		p->read_proc = (void *)dump_wlan_mbssid;
	}


	//MIB dump
	p = create_proc_entry("lut", S_IRUGO, proc_root_mib);
	if (p){
		p->read_proc = (void *)mibdump_lut_table;
	}
	p = create_proc_entry("mib_neighbor", S_IRUGO, proc_root_mib);
	if (p){
		p->read_proc = (void *)mibdump_ipv6_neighbor_table;
	}
	p = create_proc_entry("frag", S_IRUGO, proc_root_mib);
	if (p){
		p->read_proc = (void *)mibdump_frag;
	}



	return;
}


int _rtk_rg_trace_filter_compare(struct sk_buff *skb)
{
	int trace=1,i;
	rtk_rg_pktHdr_t *pPktHdr;	
	pPktHdr=(rtk_rg_pktHdr_t *)(*(u32 *)(skb->data+skb->len));
	if((((u32)pPktHdr)&0xf0000000)!=0x80000000)
	{
		if((u32)pPktHdr!=0) //skip ARP,NB which send by fwdEngine
		{
			printk("FIXME: PKTHDR isn't put at end of skb.\n");
		}			
		return 1;
	}
	
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SPA)
	{
		if(rg_kernel.trace_filter.spa!=pPktHdr->ingressPort) trace=0;
	}
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_DA)
	{
		for(i=0;i<6;i++)
		{
			if((rg_kernel.trace_filter.dmac.octet[i]&rg_kernel.trace_filter.dmac_mask.octet[i])!=
				(skb->data[i]&rg_kernel.trace_filter.dmac_mask.octet[i])) 
					trace=0;
		}
	}
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SA)
	{
		for(i=0;i<6;i++)
		{
			if((rg_kernel.trace_filter.smac.octet[i]&rg_kernel.trace_filter.smac_mask.octet[i])!=
				(skb->data[i+6]&rg_kernel.trace_filter.smac_mask.octet[i])) 
					trace=0;
		}
	}
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_ETH)
	{
		if(rg_kernel.trace_filter.ethertype!=pPktHdr->etherType) trace=0;
	}
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SIP)
	{
		if(rg_kernel.trace_filter.sip!=pPktHdr->ipv4Sip) trace=0;
	}
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_DIP)
	{
		if(rg_kernel.trace_filter.dip!=pPktHdr->ipv4Dip) trace=0;
	}
	return trace;
}

#endif


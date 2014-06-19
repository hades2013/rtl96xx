


#include "cmd_eoc.h"
#include "eoc_utils.h"
#include "lw_drv_pub.h"
#include "lw_type.h"

#include <lw_config_api.h>
#include <lw_config_oid.h>
#include <lw_if_pub.h>
#include <lw_drv_pub.h>
#include <lw_drv_req.h>


/*
interface [INTERFACE] // enter 
duplex [auto|full|half]
speed [auto|full|half]
flow-control [on | off]
priority [1-7]
rate-limitation [ingreass | egress] []
vlan-port access
vlan-port trunk pvid VID allowed VSTR
vlan-port hybrid pvid VID tagged VSTR untagged VSTR

show interface IF
show interface IF statistics
show interface IF vlan-port

interface  IFNAME
interface  IFNAME


*/


static switch_interface_t interface_in_view;

#define IFNAME_ETH0  "eth0"
#define IFNAME_ETH1  "eth1"
#define IFNAME_CAB0  "cab0"
#define IFNAME_CAB1  "cab1"


#define INTERFACE_SUPPORT_LIST IFNAME_ETH0,IFNAME_ETH1,IFNAME_CAB0,IFNAME_CAB1


#define CABLE_PORT_ASSERT(pif) do {\
								if (strstr(pif->ifname, "cab")){\
									vty_out(vty, "%% Cable port do not support this feature%s", VTY_NEWLINE);\
									return CMD_SUCCESS;\
								}\
							}while(0) 


static char *interface_fancy_mode(int mode)
{
	switch(mode) {
	case LINK_TYPE_ACCESS:
		return "Access";
	case LINK_TYPE_TRUNK:
		return "Trunk";
	case LINK_TYPE_HYBRID:
		return "Hybrid";
	}
	return "Unknown";
}




static char * interface_name_parse(const char *in, const char *type)
{
	//static char *ifnames[] = {INTERFACE_SUPPORT_LIST};
	
	static char *ifeth[] = {IFNAME_ETH0, IFNAME_ETH1};
	static char *ifcab[] = {IFNAME_CAB0, IFNAME_CAB1};	
	char *ifid[]={"1/1", "1/2", NULL};
	int i;

	for (i = 0; ifid[i] != NULL; i ++){
		if (!strcmp(in, ifid[i])){
			if (!strcmp(type, "ethernet")){
				return ifeth[i];	
			}else if (!strcmp(type, "cable")){
				return ifcab[i];
			}
		}
	}
	return NULL;
}
#define VTY_MAX_VLAN_STRING_SIZE_PER_LINE 50

static void vty_out_vlans(struct vty* vty, const char *prefix, switch_vlan_group_t *vg)
{
	int i;
	char buffer[4094*4];
	char *cp, *prev, *p;
	char bfix[256];

	for (i = 0; (i < sizeof(bfix) - 1) && (i < strlen(prefix)); i ++){
		bfix[i] = ' ';
	}
	bfix[i] = '\0';

	vlan_group_sprintf(buffer, vg);
	p = prev = buffer;
	while((cp = strchr(p, ',')) != NULL){
		if (cp - prev > VTY_MAX_VLAN_STRING_SIZE_PER_LINE){
			*cp = 0;
			vty_out(vty, "%s %s%s", (prev == buffer) ? prefix : bfix, prev, VTY_NEWLINE);
			prev = cp + 1;
		}
		p = cp + 1;
	}
	// don't forget the last line
	vty_out(vty, "%s %s%s", (prev == buffer) ?  prefix : bfix, prev, VTY_NEWLINE);
}


#if 0 //add by zhouguanhua 2013/5/31

static void vty_show_interface(struct vty* vty, switch_interface_t *info, switch_interface_status_t *status)
{
	vty_out(vty, "  INTERFACE : %s   Link %s%s", interface_fancy_name(info->ifname),LINK_STR(status->link), VTY_NEWLINE);
	vty_out(vty, "-----------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, " Shutdown  : %-6s   Auto-Neg  : %-6s%s", YESNO_STR(!info->enabled), ONOFF_STR(info->autoneg), VTY_NEWLINE);
    vty_out(vty, " Speed     : %-6s   Duplex    : %-6s%s", SPEED_STR(status->speed), DUPLEX_STR(status->duplex), VTY_NEWLINE);
    vty_out(vty, " Flow-Ctrl : %-6s   Priority  : %-6s%s", ONOFF_STR(info->flowctrl), PRIORITY_STR(info->priority), VTY_NEWLINE);	
    vty_out(vty, " Rate Limitation%s", VTY_NEWLINE);
    vty_out(vty, " Ingress   : %-6s   Egress    : %-6s%s", RATE_STR(info->ingress_limit), RATE_STR(info->egress_limit), VTY_NEWLINE);	
	vty_out(vty, "-----------------------------------------%s", VTY_NEWLINE);

}


static void vty_show_interface_statistics(struct vty* vty, switch_interface_mib_t *info)
{
	switch_interface_stats_t stats;
	switch_mibs2stats(info->mibs, &stats);
	
	vty_out(vty, "  INTERFACE : %s%s", interface_fancy_name(info->ifname), VTY_NEWLINE);
	vty_out(vty, "-----------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, " Rx Bytes       : %s%s", BYTES_STR(stats.in_octets), VTY_NEWLINE);	
	vty_out(vty, " Rx Packets     : %u%s", stats.in_pkts, VTY_NEWLINE); 
	vty_out(vty, " Rx Non-Unicast : %u%s", stats.in_mcast, VTY_NEWLINE); 
	vty_out(vty, " Rx Error       : %u%s", stats.in_error, VTY_NEWLINE); 	
	vty_out(vty, " Tx Bytes       : %s%s", BYTES_STR(stats.in_octets), VTY_NEWLINE); 
	vty_out(vty, " Tx Packets     : %u%s", stats.out_pkts,  VTY_NEWLINE); 	
	vty_out(vty, " Tx Non-Unicast : %u%s", stats.out_mcast, VTY_NEWLINE); 
	vty_out(vty, " Tx Drops       : %u%s", stats.out_drops, VTY_NEWLINE); 	
	vty_out(vty, "-----------------------------------------%s", VTY_NEWLINE);

}

#endif


static void vty_show_interface_switchport(struct vty* vty, switch_vlan_interface_t *info, int portnum)
{
	int i;
	switch_vlan_group_t *vg, *vgt = NULL, *vgu = NULL;
	char cliname[128];	;	
	IF_GetCliName(vty->ifindex,cliname,sizeof(cliname));
		vty_out(vty, "Port : %s%s",cliname, VTY_NEWLINE);
		vty_out(vty, "-------------------------------------------------------%s", VTY_NEWLINE);
		vty_out(vty, "Port Mode            : %s%s", interface_fancy_mode(info->mode), VTY_NEWLINE);	
		vty_out(vty, "Port Pvid            : %d%s", info->pvid, VTY_NEWLINE);	
/*		vty_out(vty, " Premitted VLAN  : 1-4094%s", VTY_NEWLINE);
  vty_out_vlans(vty, " Premitted VLAN  : ", info->trunk_hybrid);		
		vty_out(vty, " Tagged VLAN     : %s", VTY_NEWLINE);
  vty_out_vlans(vty, " Tagged VLAN     : ", vgt);		
		vty_out(vty, " Untagged VLAN   : %s", VTY_NEWLINE);
  vty_out_vlans(vty, " Untagged VLAN   : ", vgu);	
*/			

	switch(info->mode) {
		
	case LINK_TYPE_TRUNK:
		if(0 == info->count) {
			vty_out(vty, "Premitted Vlan          : 1-4094%s", VTY_NEWLINE);
		}
		else {
			vty_out_vlans(vty, "Premitted Vlan        : ", info->trunk_hybrid);			
		}
		break;
	case LINK_TYPE_HYBRID:
		vg=info->trunk_hybrid;
		for (i = 0; i < info->count; i ++){
			if (vlan_tagged(vg->flag)){
				vgt = vg;
			}else {
				vgu = vg;
			}
			vg = vlan_group_to_next(vg);
		}			
		
		if(vgt == NULL) {
			vty_out(vty, "Tagged Vlan     : %s", VTY_NEWLINE);
		}
		else {
 			 vty_out_vlans(vty, "Tagged Vlan     : ", vgt);		
		}
		if(vgu == NULL) {
			vty_out(vty, "Untagged Vlan          : %s", VTY_NEWLINE);
		}
		else {
			  vty_out_vlans(vty, "Untagged Vlan       : ", vgu);		
		}		

		break;
	}	
	vty_ShowPortProperty(vty, portnum);
	vty_out(vty, "-------------------------------------------------------%s", VTY_NEWLINE);
}



DEFUN(	interface,
       		interface_cmd,
       		"interface "CLI_GIGAPORTPREFIX" <1-2>",
       		"Select an interface to configure\n"
       		"Gigabit ethernet interface to configure\n"
       		"GigabitEthernet device number\n"
       		)
       		
{
	int ret;
	char szifname[64];
	ifindex_t ifindex=0;
	int portnum;
//	ifname = interface_name_parse(argv[1], argv[0]);
	//printf("argv[0] %s\n",argv[0]);
	portnum=strtoul(argv[0], NULL, 0);
	if (strstr(self->string, CLI_CABLE))
	{
		portnum+=FE_PORT_NO;
		portnum+=GE_PORT_NO;
	}
	if (!VALID_PORT(portnum)){
		vty_out(vty, "%% Invalid interface name%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,portnum);
	IF_GetByIFName(szifname,&ifindex);
	
	//VTY_IPC_ASSERT(ret);

//	vty->index = &interface_in_view;
	vty->ifindex=ifindex;
	vty->node = INTERFACE_NODE;
	
	return CMD_SUCCESS;
}
ALIAS (	interface,
			interface_cable_cmd,
			"interface "CLI_CABLE" <1-1>",
       		"Select an interface to configure\n"
       		"cable interface to configure\n"
       		"cable port number\n"
		);

#if 0

DEFUN (	show_interface,
			show_interface_cmd,
			"show interface (ethernet|cable) IFNAME",
			SHOW_STR
			STR(interface)
			STR(ethernet)
			STR(cable)
			STR(ifname)
		)
{
	int ret;
	char *ifname;
	switch_interface_status_t status;
	switch_interface_t ifcfg;
	switch_interface_t *pif = (switch_interface_t *)vty->index;	

	if (strstr(self->string, "interface")){
		ifname = interface_name_parse(argv[1], argv[0]);
	}else {
		ifname = pif->ifname;
	}

	if (ifname == NULL){
		vty_out(vty, "%% Invalid interface name%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	memset(&ifcfg, 0, sizeof(ifcfg));
	safe_strncpy(ifcfg.ifname, ifname, sizeof(ifcfg.ifname));

	ret = ipc_get_interface(&ifcfg);
	
	VTY_IPC_ASSERT(ret);

	ret = ipc_get_interface_status(ifcfg.ifname, &status);
	VTY_IPC_ASSERT(ret);
	vty_show_interface(vty, &ifcfg, &status);
		
	return CMD_SUCCESS;
}

#if 0
ALIAS (	show_interface,
			show_current_cmd,
			"show",
			SHOW_STR
		);
#endif

#endif

/*Begin add by huangmingjian 2013-09-03 for cli port mode*/
int vty_ShowPortMibCounter(struct vty* vty, UINT32 lport)
{	
    int ret = 0;
	UINT64 ullValue = 0;
	int i = 0;
	char cliname[64] = {0};

	UINT64 rx_Unicastpkt = 0;
	UINT64 rx_Multicastpkt = 0;
	UINT64 rx_Broadcastpkt = 0;
	UINT64 tx_Unicastpkt = 0;
	UINT64 tx_Broadcastpkt = 0;
	UINT64 tx_Multicastpkt = 0;
	UINT64 rx_Pkt = 0;
	UINT64 tx_Pkt = 0;
	UINT64 rx_Octets = 0;
	UINT64 tx_Octets = 0;
	UINT64 rx_Pausepkt = 0;
	UINT64 rx_Errpkt = 0;
	UINT64 tx_Pausepkt = 0;
	UINT64 tx_Errpkt = 0;
	
	if(FALSE == IsValidLgcPort(lport))
    {
        return CMD_WARNING;
    }

	IF_GetCliName(vty->ifindex, cliname, sizeof(cliname));
	vty_out(vty, "Port : %s%s", cliname, VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------%s", VTY_NEWLINE);
	
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxUnicastPkts, &rx_Unicastpkt)))
		DBG_PRINTF("Get RxUnicastPkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxMulticastPkts, &rx_Multicastpkt)))
		DBG_PRINTF("Get RxMulticastPkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxBroadcastPkts , &rx_Broadcastpkt)))
		DBG_PRINTF("Get RxBroadcastPkts fail"); 
	rx_Pkt = rx_Unicastpkt + rx_Multicastpkt + rx_Broadcastpkt;
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxUnicastPkts, &tx_Unicastpkt)))
		DBG_PRINTF("Get TxUnicastPkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxMulticastPkts , &tx_Multicastpkt)))
		DBG_PRINTF("Get TxMulticastPkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxBroadcastPkts, &tx_Broadcastpkt)))
		DBG_PRINTF("Get TxBroadcastPkts fail"); 
	tx_Pkt = tx_Unicastpkt + tx_Multicastpkt + tx_Broadcastpkt;
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxOctets, &rx_Octets)))
		DBG_PRINTF("Get RxOctets fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxOctets, &tx_Octets)))
		DBG_PRINTF("Get TxOctets fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxPausePkts, &rx_Pausepkt)))
		DBG_PRINTF("Get RxPausePkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, RxErrPkts, &rx_Errpkt)))
		DBG_PRINTF("Get RxErrPkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxPausePkts, &tx_Pausepkt)))
		DBG_PRINTF("Get TxPausePkts fail"); 
	if(DRV_OK != (ret = Ioctl_GetPortStatisticsByPort(lport, TxErrPkts, &tx_Errpkt)))
		DBG_PRINTF("Get TxErrPkts fail"); 

	vty_out(vty, "%-16s 		   : %llu%s", "rx Pkt", rx_Pkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Pkt", tx_Pkt, VTY_NEWLINE);
	vty_out(vty, "%-16s 		   : %llu%s", "rx Octets", rx_Octets, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Octets", tx_Octets, VTY_NEWLINE);	
	vty_out(vty, "%-16s 		   : %llu%s", "rx Unicastpkt", rx_Unicastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "rx Multicastpkt", rx_Multicastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "rx Broadcastpkt", rx_Broadcastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "rx Pausepkt", rx_Pausepkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "rx Errpkt", rx_Errpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Unicastpkt", tx_Unicastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Multicastpkt", tx_Multicastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Broadcastpkt", tx_Broadcastpkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Pausepkt", tx_Pausepkt, VTY_NEWLINE); 
	vty_out(vty, "%-16s 		   : %llu%s", "tx Errpkt", tx_Errpkt, VTY_NEWLINE); 		
	vty_out(vty, "-------------------------------------------------------%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}



int vty_ShowPortProperty(struct vty* vty, UINT32 lport)
{
	int ret = 0;
    int arrayidx = 0;
	ipc_port_ack_t *pack = NULL;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
    unsigned int linkduplex  = 0;
    unsigned int linkspeed = 0;
    unsigned int linkstate = 0;
	unsigned int speed_duplex = 0;
	unsigned int mdix = 0;

	char *linkstate_str[] =    {"10M/HDX", "100M/HDX", "1000M/HDX", "Unknown", "10M/FDX", "100M/FDX",
							    "1000M/FDX", "Unlink"};
	char *speed_duplex_str[] = {"10M/HDX", "100M/HDX", "1000M/HDX", "AUTOM/HDX", 
							    "10M/FDX", "100M/FDX", "1000M/FDX", "AUTOM/FDX",
							    "10M/AUTO", "100M/AUTO", "1000M/AUTO", "Auto-Negotiation", "Unknown"};
	char *enable_str[] =       {"Disabled", "Enabled"};
	char *open_str[] =         {"Close", "Open"};
	char *mdix_str[] =         {"ACROSS", "AUTO", "NORMAL"};
	
    if(FALSE == IsValidLgcPort(lport))
    {
        return CMD_WARNING;
    }
    arrayidx = lport-1;
	
	pack = ipc_port_req(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        memcpy(port_cfg_val, pack->port_cfg_val, sizeof(PORT_CFG_VALUE_S)*LOGIC_PORT_NO);
		ret = IPC_STATUS_OK;
	}
    else if (pack)
    {
		ret = pack->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(pack) 
    {   
        free(pack);     
    }
    
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get port cfg faild");
        return 0;
    }
   
    Ioctl_GetPortCurrentLink(lport, &linkstate);
    Ioctl_GetPortDuplex(lport, &linkduplex);
    Ioctl_GetPortSpeed(lport, &linkspeed);

	linkstate = (0 == linkstate?7:(linkspeed+(linkduplex<<2)));
	speed_duplex = port_cfg_val[arrayidx].stSpeed + (port_cfg_val[arrayidx].stDuplex<<2);
	speed_duplex = (speed_duplex>11?12:speed_duplex);
	mdix = port_cfg_val[arrayidx].ulMdix;
	mdix = mdix>2?1:mdix;

	vty_out(vty, "Description          : %s%s", port_cfg_val[arrayidx].stIfDesc, VTY_NEWLINE);	
	vty_out(vty, "Link State           : %s%s", linkstate_str[linkstate], VTY_NEWLINE);
	vty_out(vty, "Speed/Duplex         : %s%s", speed_duplex_str[speed_duplex], VTY_NEWLINE);
	vty_out(vty, "Priority         	   : %d%s", port_cfg_val[arrayidx].ulPriority, VTY_NEWLINE);
	vty_out(vty, "Flow Control         : %s%s", enable_str[port_cfg_val[arrayidx].ulFlowCtl], VTY_NEWLINE);
	vty_out(vty, "PHY         		   : %s%s", open_str[port_cfg_val[arrayidx].ulIfEnable], VTY_NEWLINE);
	vty_out(vty, "MDIX         		   : %s%s", mdix_str[mdix], VTY_NEWLINE);
	vty_out(vty, "Isolate         	   : %s%s", enable_str[port_cfg_val[arrayidx].ulIsolateEn], VTY_NEWLINE);

	return CMD_SUCCESS;
}

/*End add by huangmingjian 2013-09-03 for cli port mode*/


DEFUN (	show_interface_statistics,
			show_interface_statistics_cmd,
			"show interface statistics "CLI_GIGAPORTPREFIX" <1-2>",
			SHOW_STR
			"Select an interface to configure\n"
			STR(statistics)			
			"Gigabit ethernet interface to configure\n"					
			"GigabitEthernet device number\n"
		)
{
	int ret;
	char *ifname;
	int portnum=0;
	char szifname[64];

	//printf("self->string=%s argv[0] %s\n",self->string,argv[0]);
	if (strstr(self->string, "interface"))
	{
		if(strstr(self->string, CLI_CABLE))
		{
			portnum = 3;/*logic port 3 is cable port*/
		}
		else
		{
			portnum=strtoul(argv[0], NULL, 0);
		}
		
		if (!VALID_PORT(portnum))
		{
			vty_out(vty, "%% Invalid interface name%s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
	
		sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,portnum);
		ifname=szifname;		
		IF_GetByIFName(szifname,&vty->ifindex);	
	
	}else 
	{
	//	ifname = pif->ifname;
		IF_GetAttr(vty->ifindex,IF_NAME,szifname,sizeof(szifname));
	}
	
	if (strstr(self->string, "statistics")){		
		//ret = ipc_get_interface_statistics(ifname, &stats);
		//VTY_IPC_ASSERT(ret);
		//vty_show_interface_statistics(vty, &stats);
		vty_ShowPortMibCounter(vty, portnum);
	}else if (strstr(self->string, "switchport")){
		
		switch_vlan_mode_t vmode;
		ipc_clt_vlan_interface_config_ack_t *pack;
		switch_vlan_interface_t req;

		ret = ipc_get_vlan_mode(ipc_fd,&vmode);	
		VTY_IPC_ASSERT(ret);
		
		if (vmode.mode == VLAN_MODE_TRANSPARENT){
			vty_out(vty, "System VLAN is in transparent mode, please use 'show vlan all' to get detail%s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}		
		memset(&req, 0, sizeof(req));
		safe_strncpy(req.ifname, ifname, sizeof(req.ifname));	
		pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, &req, 1);
		if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count == 1)) {
			vty_show_interface_switchport(vty, pack->vlanifs, portnum);	
			ret = IPC_STATUS_OK;
		}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
			ret = pack->hdr.status;
		}else {
			ret = IPC_STATUS_FAIL;
		}

		if(pack) free(pack);
	
	//	VTY_IPC_ASSERT(ret);
	}

	return CMD_SUCCESS;

}
/*
DEFUN (	show_interface_statistics,
			show_statistics_cmd,
			"show statistics",
			STR(show)
			STR(statistics)
		);

*/
ALIAS (	show_interface_statistics,
			show_interface_switchport_cmd,
			"show interface switchport "CLI_GIGAPORTPREFIX" <1-2>",
			STR(show)
			"Interface status and configuration\n"
			"Set switching mode characteristics\n"
			"GigabitEthernet interface to show\n"					
			"GigabitEthernet port number\n"
		);
ALIAS (	show_interface_statistics,
			show_interface_cableport_cmd,
			"show interface switchport "CLI_CABLE" <1-1>",
			STR(show)
			"Interface status and configuration\n"
			"Set switching mode characteristics\n"
			"cable interface to show \n"					
			"cable port number\n"
		);


ALIAS (	show_interface_statistics,
			show_switchport_cmd,
			"show switchport",
			STR(show)
			"Set switching mode characteristics\n"
		);

#if 0
/*
interface [INTERFACE] // enter 
duplex [auto|full|half]
speed [auto|full|half]
flow-control [on | off]
priority [1-7]
rate-limitation [ingreass | egress] []
vlan-port access
vlan-port trunk pvid VID allowed VSTR
vlan-port hybrid pvid VID tagged VSTR untagged VSTR

show interface IF
show interface IF statistics
show interface IF vlan-port

interface  IFNAME
interface  IFNAME


*/

DEFUN (	ifshutdown,
			shutdown_cmd,
			"shutdown",
			STR(shutdown)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret;
	int enable = 1;	

	CABLE_PORT_ASSERT(pif);	
	
	if (strstr(self->string, "no")){
		enable = 0;
	}
	if (enable != pif->enabled){
		pif->enabled = enable;
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;
}

ALIAS(	ifshutdown, 
			no_shutdown_cmd, 
			"no shutdown", 
			NO_STR 
			STR(shutdown));



DEFUN (	auto_neg,
			auto_neg_cmd,
			"auto-neg",
			STR(auto_neg)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret;
	int enable = 1;	

	CABLE_PORT_ASSERT(pif);

	if (strstr(self->string, "no")){
		enable = 0;
	}
	if (enable != pif->autoneg){
		pif->autoneg = enable;
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;
}


ALIAS(	auto_neg, 
			no_auto_neg_cmd, 
			"no auto-neg", 
			NO_STR 
			STR(auto_neg));


DEFUN (	duplex,
			duplex_cmd,
			"duplex (full|half)",
			STR(duplex)
			STR(full_half)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret, v, change = 0;

	CABLE_PORT_ASSERT(pif);
	
	v = !strcmp(argv[0], "full") ? ETH_DUPLEX_FULL : ETH_DUPLEX_HALF;

	if (pif->autoneg){
		pif->autoneg = 0;		
		pif->speed = ETH_SPEED_1000;
		change = 1;
	}

	if (change || (pif->duplex != v)){
		pif->duplex = v;
		
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}

	return CMD_SUCCESS;
}


DEFUN (	speed,
			speed_cmd,
			"speed (10|100|1000)",
			STR(speed)
			STR(speed_10)
			STR(speed_100)
			STR(speed_1000)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret, v = ETH_SPEED_1000, change = 0;

	CABLE_PORT_ASSERT(pif);

	if (!strcmp(argv[0], "10")){
		v = ETH_SPEED_10;
	}else if (!strcmp(argv[0], "100")){
		v = ETH_SPEED_100;
	}else if(!strcmp(argv[0], "1000")){
		v = ETH_SPEED_1000;
	}

	if (pif->autoneg){
		pif->autoneg = 0;		
		pif->duplex = ETH_DUPLEX_FULL;
		change = 1;
	}

	if (change || (pif->speed != v)){
		pif->speed = v;
		
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}

	return CMD_SUCCESS;	
}


DEFUN (	flow_control,
			flow_control_cmd,
			"flow_control",
			STR(flow_control)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret;
	int enable = 1;	

	CABLE_PORT_ASSERT(pif);

	if (strstr(self->string, "no")){
		enable = 0;
	}
	if (enable != pif->flowctrl){
		pif->flowctrl = enable;
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;
}


ALIAS(	flow_control, 
			no_flow_control_cmd, 
			"no flow_control", 
			NO_STR 
			STR(flow_control));



DEFUN (	bandwidth,
			bandwidth_ingress_cmd,
			"bandwidth ingress <1-1000>",
			STR(bandwidth)
			STR(ingress)
			STR(rate)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret, change = 0, ingress = 0;
	unsigned long rate;

	CABLE_PORT_ASSERT(pif);

	if (strstr(self->string, "no")){
		rate = 0;
	}else {
		rate = strtoul(argv[0], NULL, 0);
	}

	if (strstr(self->string, "ingress")){
		ingress = 1;
	}

	if (ingress && (pif->ingress_limit != rate)){
		pif->ingress_limit = rate;
		change = 1;
	}else if (!ingress && (pif->egress_limit != rate)){
		pif->egress_limit = rate;
		change = 1;
	}
	
	if (change){
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;

}

ALIAS (	bandwidth,
			bandwidth_egress_cmd,
			"bandwidth egress <1-1000>",
			STR(bandwidth)
			STR(egress)
			STR(rate)
		);

ALIAS(	bandwidth, 
			no_bandwidth_ingress_cmd, 
			"no bandwidth ingress", 
			NO_STR 
			STR(bandwidth)
			STR(ingress));

ALIAS(	bandwidth, 
			no_bandwidth_egress_cmd, 
			"no bandwidth egress", 
			NO_STR 
			STR(bandwidth)
			STR(egress));



DEFUN (	priority,
			priority_cmd,
			"priority <0-7>",
			STR(priority)
			STR(priority_value)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret, pri;	

	CABLE_PORT_ASSERT(pif);

	if (strstr(self->string, "no")){
		pri = ETH_PRIORITY_OFF;
	}else {
		pri = strtoul(argv[0], NULL, 0);
	}
	if (pri != pif->priority){
		pif->flowctrl = pri;
		ret = ipc_set_interface(pif);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;
}


ALIAS(	priority, 
			no_priority_cmd, 
			"no priority", 
			NO_STR 
			STR(priority));


DEFUN (	statistics_reset,
			statistics_reset_cmd,
			"statistics reset",
			STR(statistics)
			STR(reset)
		)
{
	switch_interface_t *pif = (switch_interface_t *)vty->index;
	int ret;	

	ret = ipc_reset_interface_statistics(pif->ifname);
	
	VTY_IPC_ASSERT(ret);
		
	return CMD_SUCCESS;
}
#endif

DEFUN (	switchport_access,
			switchport_access_cmd,
			"switchport access pvid <1-4094>",
			STR(switchport)
			STR(access)
			STR(pvid)
			STR(VLANID)	
		)
{
	//switch_interface_t *pif = (switch_interface_t *)vty->index;
	switch_vlan_interface_t vlanif;
	switch_vlan_mode_t vmode;
	int i, ret, submit = 0;	
	char portname[64];
	UINT16 pvid = 0;
	
	IF_GetAttr(vty->ifindex,IF_NAME,portname,sizeof(portname));
	ret = ipc_get_vlan_mode(ipc_fd,&vmode);	
	VTY_IPC_ASSERT(ret);
	
	if (vmode.mode == VLAN_MODE_TRANSPARENT){

		#if 0
		if (strstr(pif->ifname, "cab")){
			vty_out(vty, "Cable port do not support in VLAN transparent mode%s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
		
		for (i = 0; i < EOC_MAX_PORT_NUMS; i ++){
			if (!strcmp(vmode.ports[i].ifname, pif->ifname)){
				if (vmode.ports[i].type != ETH_SWITCHPORT_ACCESS){
					vmode.ports[i].type = ETH_SWITCHPORT_ACCESS;
					submit = 1;
				}
			}
		}
		if (submit == 1){
			ret = ipc_set_vlan_mode(&vmode);
			VTY_IPC_ASSERT(ret);
		}
		#endif
		vty_out(vty, "port do not support in VLAN transparent mode%s", VTY_NEWLINE);			
		return CMD_SUCCESS;
	}	

	// dot1q mode

	/*Begin add by huangmingjian for pvid check 2013-09-05*/
	pvid = strtoul(argv[0], NULL, 0); 
	if(!vlan_exist(pvid, &ret))
	{
		vty_out(vty, "Pvid %d do not exist!%s", pvid, VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	/*End add by huangmingjian for pvid check 2013-09-05*/
	
	memset(&vlanif, 0, sizeof(vlanif));
	safe_strncpy(vlanif.ifname, portname, sizeof(vlanif.ifname));
	vlanif.mode = LINK_TYPE_ACCESS;
//	vlanif.pvid = 1;// it will be set by pvid
	vlanif.pvid = pvid;
	ret = ipc_set_interface_switchport(&vlanif);
	
	VTY_IPC_ASSERT(ret);
		
	return CMD_SUCCESS;
}



DEFUN(	switchport_trunk,
			switchport_trunk_cmd,
			"switchport trunk pvid <1-4094> allowed (all|VLANLIST)",
			STR(switchport)
			STR(trunk)
			STR(pvid)
			STR(VLANID)	
			STR(allowed)
			STR(all)
			STR(vlanlist)
		)
{
	//switch_interface_t *pif = (switch_interface_t *)vty->index;
	switch_vlan_interface_t *vlanif;
	switch_vlan_group_t *vg;
	int ret;
	switch_vlan_mode_t vmode;
	char portname[64];
	int i, submit = 0;	
	ret = ipc_get_vlan_mode(ipc_fd,&vmode);	
	UINT16 pvid = 0;
	VTY_IPC_ASSERT(ret);
	
	if (vmode.mode == VLAN_MODE_TRANSPARENT){
		#if 0
		if (strstr(pif->ifname, "cab")){
			vty_out(vty, "Cable port do not support in VLAN transparent mode%s", VTY_NEWLINE);
			return CMD_SUCCESS;
		}
		vty_out(vty, "System VLAN is in transparent mode, neglects trunk parameters%s", VTY_NEWLINE);
		
		for (i = 0; i < EOC_MAX_PORT_NUMS; i ++){
			if (!strcmp(vmode.ports[i].ifname, pif->ifname)){
				if (vmode.ports[i].type != ETH_SWITCHPORT_TRUNK){
					vmode.ports[i].type = ETH_SWITCHPORT_TRUNK;
					submit = 1;
				}
			}
		}
		if (submit == 1){
			ret = ipc_set_vlan_mode(&vmode);
			VTY_IPC_ASSERT(ret);
		}
		#endif
		vty_out(vty, "port do not support in VLAN transparent mode%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}	

	/*Begin add by huangmingjian for pvid check 2013-09-05*/
	pvid = strtoul(argv[0], NULL, 0); 
	if(!vlan_exist(pvid, &ret))
	{
		vty_out(vty, "Pvid %d do not exist!%s", pvid, VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	/*End add by huangmingjian for pvid check 2013-09-05*/
	
	if (!strcmp(argv[1], "all")){
		vg = NULL;
	}else if (vlan_string_check(argv[1])){
		vg = vlan_group_new_from_string_simple(argv[1]);
		VTY_NOMEM_ASSERT(vg);
		if (vg) vg->flag = VLAN_FLAG_TAG;
	}else {
		vty_out(vty, "Invalid VLAN ID string%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	if (!all_vlan_exist(vg, &ret)){
		VTY_IPC_ASSERT(ret);
		vty_out(vty, "One or more ID in VLAN list do not exist%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	IF_GetAttr(vty->ifindex,IF_NAME,portname,sizeof(portname));
	vlanif = vlan_interface_new(portname, LINK_TYPE_TRUNK, vg, NULL);
	
	if (vg) free(vg);

	VTY_NOMEM_ASSERT(vlanif);

	vlanif->pvid = pvid;
	ret = ipc_set_interface_switchport(vlanif);

	if (vlanif) free(vlanif);	
	
	VTY_IPC_ASSERT(ret);
		
	return CMD_SUCCESS;	
}
		

DEFUN(	switchport_hybrid,
			switchport_hybrid_cmd,
			"switchport hybrid pvid <1-4094> tagged (none|VLANS) untagged (none|VLANS)",
			STR(switchport)
			STR(hybrid)
			STR(pvid)
			STR(VLANID)	
			STR(tagged)
			STR(none)			
			STR(vlanlist)
			STR(untagged)
			STR(none)			
			STR(vlanlist)
		)
{
	//switch_interface_t *pif = (switch_interface_t *)vty->index;
	switch_vlan_interface_t *vlanif;
	switch_vlan_group_t *vg_tag, *vg_untag;
	switch_vlan_mode_t vmode;	
	int ret;	
	char portname[64];
	UINT16 pvid = 0;
	
	IF_GetAttr(vty->ifindex,IF_NAME,portname,sizeof(portname));
	ret = ipc_get_vlan_mode(ipc_fd,&vmode);	
	VTY_IPC_ASSERT(ret);
	
	if (vmode.mode == VLAN_MODE_TRANSPARENT){
		vty_out(vty, "Port do not support in VLAN transparent mode%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	/*Begin add by huangmingjian for pvid check 2013-09-05*/
	pvid = strtoul(argv[0], NULL, 0); 
	if(!vlan_exist(pvid, &ret))
	{
		vty_out(vty, "Pvid %d do not exist!%s", pvid, VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	/*End add by huangmingjian for pvid check 2013-09-05*/
	
	if (!strcmp(argv[1], "none")){
		vg_tag = NULL;
	}else if (vlan_string_check(argv[1])){
		vg_tag = vlan_group_new_from_string_simple(argv[1]);
		VTY_NOMEM_ASSERT(vg_tag);
		if (vg_tag) vg_tag->flag = VLAN_FLAG_TAG;
	}else {
		vty_out(vty, "Invalid VLAN ID string%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if (!all_vlan_exist(vg_tag, &ret)){
		VTY_IPC_ASSERT(ret);		
		vty_out(vty, "One or more ID in VLAN list do not exist%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	if (!strcmp(argv[2], "none")){
		vg_untag = NULL;
	}else if (vlan_string_check(argv[2])){
		vg_untag = vlan_group_new_from_string_simple(argv[2]);
		VTY_NOMEM_ASSERT(vg_untag);
	}else {
		vty_out(vty, "Invalid VLAN ID string%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	if (!all_vlan_exist(vg_untag, &ret)){
		VTY_IPC_ASSERT(ret);		
		vty_out(vty, "One or more ID in VLAN list do not exist%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	if (any_vlan_in_groups(vg_tag, vg_untag, 1)){
		vty_out(vty, "One or more ID exist in tagged group and untagged group%s", VTY_NEWLINE);
		return CMD_SUCCESS;		
	}
	
	
	
	vlanif = vlan_interface_new(portname, LINK_TYPE_HYBRID, vg_tag, vg_untag);
	
	if (vg_tag) free(vg_tag);
	if (vg_untag) free(vg_untag);

	VTY_NOMEM_ASSERT(vlanif);
	
	vlanif->pvid = pvid;	

	ret = ipc_set_interface_switchport(vlanif);

	if (vlanif) free(vlanif);

	
	VTY_IPC_ASSERT(ret);
		
	return CMD_SUCCESS;	

}
		
//#endif  //add by zhouguanhua 2013/5/31

static struct cmd_node interface_node = {INTERFACE_NODE, NULL, NULL, 1, };
static struct cmd_node vlan_interface_node = {VLAN_INTERFACE_NODE, NULL, NULL, 1 };

static char *cmd_interface_node_prompt(struct vty *vty)
{
	//switch_interface_t *ifp = (switch_interface_t *)vty->index;
	static char prompt[40];	
	char cliname[128];	;	
	IF_GetCliName(vty->ifindex,cliname,sizeof(cliname));
	sprintf(prompt, "%%s(config-%s)# ", cliname);
	return prompt;
}

static int cmd_interface_config_write(struct vty *vty) 
{
	return vty_out(vty, "!%s", VTY_NEWLINE);
}

/*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
static char *cmd_vlan_interface_node_prompt(struct vty *vty)
{
    static char prompt[40];
    sprintf(prompt, "%%s(Vlan-interface%d)# ", vty->para.para_integer);
    return prompt;
}

DEFUN (vlan_interface,
        access_vlan_interface_cmd,
        "interface vlan-interface <1-4094>",
        INTERFACE_DESC_STR
        "VLAN interface\n"
        "VLAN interface\n")
{
	char szifname[64];//added by wanghuanyu for 129
	
    #if 0 //add by zhouguanhua 2013/5/31
    sys_network_t info;
    int iRet = IPC_STATUS_OK;

    memset(&info, 0x0, sizeof(sys_network_t));  
    iRet = ipc_get_sys_networking(&info);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 

    if(info.m_vlan != strtoul(argv[0], NULL, 0))
    {
        vty_out(vty, " ERROR: Interface-vlan should be same with management-vlan!%s", VTY_NEWLINE);
        return CMD_WARNING;  
    }
    vty->para.para_integer = strtoul(argv[0], NULL, 0);
    vty->node = VLAN_INTERFACE_NODE;
    return CMD_SUCCESS;
   #endif //add by zhouguanhua 2013/5/31
   sys_mvlan_t info;
   int iRet = IPC_STATUS_OK;
   memset(&info, 0x0, sizeof(sys_mvlan_t));  
   iRet = get_sys_mvlan_value(&info);
   if(IPC_STATUS_OK != iRet)
   {
       vty_out(vty, " ERROR: get system mvlan failed!%s", VTY_NEWLINE);
       return CMD_WARNING;        
   } 
   if(info.m_vlan != strtoul(argv[0], NULL, 0))
   {
       vty_out(vty, " ERROR: Interface-vlan should be same with management-vlan!%s", VTY_NEWLINE);
       return CMD_WARNING;  
   }
   /*begin modified by wanghuanyu */
	vty->para.para_integer = strtoul(argv[0], NULL, 0);
	sprintf(szifname,"%s%d",IF_L3VLAN_NAMEPREFIX,vty->para.para_integer);
	IF_GetByIFName(szifname,&(vty->ifindex));
	vty->node = VLAN_INTERFACE_NODE;
	/*end modified by wanghuanyu */
   return CMD_SUCCESS;
}
/*****************************************************************
    Function:management_vlan_set
    Description:func to set management vlan
    Author:liaohongjun
    Date:2012/11/2
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (management_vlan_set,
        management_vlan_set_cmd,
        "management-vlan <1-4094>",
        "Specify management-vlan\n"
        "802.1Q VLAN ID\n")
{
#if 0 //add by zhouguanhua 2013/5/31
    int iRet = IPC_STATUS_OK;
    int iVlanId = 0;
    int i = 0;
    ipc_network_t ipc_set;
    sys_network_t info;

    iVlanId = strtoul(argv[0], NULL, 0);
    memset(&info, 0x0, sizeof(sys_network_t));  
    iRet = ipc_get_sys_networking(&info);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    
    memset(&ipc_set, 0, sizeof(ipc_set));          
    if(strstr(self->string, "no")) 
    {
        if(DEFAULT_MANAGEMENT_VLAN == info.m_vlan)
        {
            vty_out(vty, " INFO: the management vlan has already been default set!%s", VTY_NEWLINE); /*modify by zhouguanhua 2012/12/6 of EPN204QID0010*/           
            return CMD_SUCCESS;
        }
        else
        {
            ipc_set.m_vlan = DEFAULT_MANAGEMENT_VLAN;
            ipc_set.vlan_interface = DEFAULT_MANAGEMENT_VLAN;
        }
    }
    else
    {
        if(info.m_vlan == iVlanId)
        {
            vty_out(vty, " INFO: the management vlan has already been set!%s", VTY_NEWLINE); /*modify by zhouguanhua 2012/12/6 of EPN204QID0010*/
            return CMD_SUCCESS;
        }
        else
        {
            ipc_set.m_vlan = iVlanId;         
            ipc_set.vlan_interface = iVlanId;
        }        
    }
    
    ipc_set.ip_proto = info.ip_proto;
    memcpy(&ipc_set.netif, &info.netif, sizeof(ipc_set.netif));
    safe_strncpy(ipc_set.hostname, info.hostname, sizeof(ipc_set.hostname));
    safe_strncpy(ipc_set.interface_state, info.interface_state, sizeof(ipc_set.interface_state));

	/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	ipc_set.loopback_enable = info.loopback_enable;
	ipc_set.loop_update = info.loop_update;
	/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
	
    iRet = ipc_set_sys_networking(&ipc_set);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }  
    
    return CMD_SUCCESS;
#endif //add by zhouguanhua 2013/5/31

    int iRet = IPC_STATUS_OK;
    int iVlanId = DEFAULT_MANAGEMENT_VLAN;
   
    sys_mvlan_t ipc_set;
    sys_mvlan_t info;

	/*Begin modified by huangmingjian for pvid check 2013-09-05*/
    memset(&info, 0x0, sizeof(sys_mvlan_t));  

    iRet = get_sys_mvlan_value(&info);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system mvlan failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    
    if(strstr(self->string, "no")) 
    {
        if(DEFAULT_MANAGEMENT_VLAN == info.m_vlan)
        {
            vty_out(vty, " INFO: the management vlan has already been default set!%s", VTY_NEWLINE); /*modify by zhouguanhua 2012/12/6 of EPN204QID0010*/           
            return CMD_SUCCESS;
        }
        else
        {
            ipc_set.m_vlan = DEFAULT_MANAGEMENT_VLAN;
            ipc_set.vlan_interface = DEFAULT_MANAGEMENT_VLAN;
        }
    }
    else
    {	
    	iVlanId = strtoul(argv[0], NULL, 0); 
        if(info.m_vlan == iVlanId)
        {        
            vty_out(vty, " INFO: the management vlan has already been set!%s", VTY_NEWLINE); /*modify by zhouguanhua 2012/12/6 of EPN204QID0010*/
            return CMD_SUCCESS;
        }
        else
        {
        	/*begin added by wanghuanyu for 215*/
        	#ifdef SWITCH_STYLE
			if(!vlan_exist(iVlanId, &iRet))
			{
				vty_out(vty, "Vlan Id %d do not exist!%s", iVlanId, VTY_NEWLINE);
				return CMD_SUCCESS;
			}
			#endif
			/*end added by wanghuanyu for 215*/
            ipc_set.m_vlan = iVlanId;         
            ipc_set.vlan_interface = iVlanId;
        }        
    }
	/*End modified by huangmingjian for pvid check 2013-09-05*/

    iRet = ipc_set_sys_mvlan(&ipc_set); 
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }  
    return CMD_SUCCESS;
}
ALIAS(management_vlan_set, 
        no_management_vlan_cmd, 
        "no management-vlan", 
        NO_DESC_STR 
        "Specify management-vlan\n");


/*loid command start*/
DEFUN (epon_loid_set,
        epon_loid_cmd,
        "epon loid LOID<1-24> [PASSWORD<1-12>]",
        "Set Logical ONU Identifier \n"
        "Logical ONU Identifier \n")
{
    int iRet = IPC_STATUS_OK;
	unsigned char val[25] = {0};
	unsigned ii = 0;
	sys_epon_t epon_info;
	
	memset(&epon_info, 0, sizeof(sys_epon_t));
	
	if(strlen(argv[0]) > 24)
	{	
		vty_out(vty, "Loid is too long, make sure that it is between 1-24 characters%s",VTY_NEWLINE);
		return CMD_WARNING;
		
	}
	if(2 == argc)
	{
		if(strlen(argv[1]) > 12)
		{	
			vty_out(vty, "Passwd is too long, make sure that it is between 1-12 characters%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
    sprintf(val, argv[0], strlen(argv[0]));

	for(ii=0; ii<strlen(val); ii++)
	{
		if(((val[ii]<='z') && (val[ii]>='a')) ||
		((val[ii]<='Z') && (val[ii]>='A')) ||
		((val[ii]<='9') && (val[ii]>='0')))
		{
			// input correct
		}
		else
		{
			vty_out(vty, "LOID is not correct, should be <a-z>,<A-Z>,<0-9>%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	memcpy(epon_info.epon_loid, val, 25);
	
	if(2 == argc)
	{
		memset(val, 0, sizeof(val));
		sprintf(val, argv[1], strlen(argv[1]));
		for(ii=0; ii<strlen(val); ii++)
		{
			if(((val[ii]<='z') && (val[ii]>='a')) ||
			((val[ii]<='Z') && (val[ii]>='A')) ||
			((val[ii]<='9') && (val[ii]>='0')))
			{
				// input correct
			}
			else
			{
				vty_out(vty, "Password is not correct, should be <a-z>,<A-Z>,<0-9>%s",VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		memcpy(epon_info.epon_passwd, val, 12);
	}
	

	if(IPC_STATUS_OK != ipc_set_sys_epon(&epon_info))
	{
		/*set error*/
		return CMD_WARNING;
	}
	
    return CMD_SUCCESS;
}

DEFUN (show_epon_loid,
          show_epon_loid_cmd,
          "show epon loid",
          "Display onu loid and password(if exist)\n"
          "Display onu loid and password(if exist)\n")
{
	int ret = 0;
	unsigned char val[25] = {0};

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_EPON_LOID, (void *)val, "", sizeof(val))) < 0)
    {
       return CMD_WARNING;
    }
    vty_out(vty, "Loid\t\t:\t%s%s", val, VTY_NEWLINE);

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_EPON_PASSWD, (void *)val, "", sizeof(val))) < 0)
    {
       return CMD_WARNING;
    }
	if(val[0] !=0)
	{
		vty_out(vty, "Password\t:\t%s%s", val, VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}
/*loid command end*/

#ifdef CONFIG_LOOPD

/*****************************************************************
    Function:loopback_enable
    Description: func to enable or disable loopback 
    Author:huangmingjian
    Date:2012/11/28
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (loopback_enable,
		loopback_enable_cmd,
		"loopback-detection (enable|disable)",
		"Loopback-detection\n"
		"Enable function\n"
		"Disable function\n")
{

#define ENABLE  1
#define DISABLE 0
	
	int iRet = IPC_STATUS_OK;
    int iLoopback = 0;
    sys_loop_t ipc_set;
    sys_loop_t info;

	
    if(strstr(ENABLE_STR, (char *)argv[0]))
    {
        iLoopback = ENABLE;
    }
    else if(strstr(DISABLE_STR, (char *)argv[0])) 
    {
        iLoopback = DISABLE;
    }
    else 
    {
        vty_out(vty, " ERROR: set loopback paras error!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    memset(&info, 0x0, sizeof(sys_loop_t));  
    iRet = ipc_get_sys_loop(&info);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system loop failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    memset(&ipc_set, 0, sizeof(ipc_set));          
    if(iLoopback == info.loopback_enable)
    {  
    	if(DISABLE == iLoopback)
    	{
    		vty_out(vty, " INFO: loopback-detection is disabled!%s", VTY_NEWLINE);
    	}
		else
		{
			vty_out(vty, " INFO: loopback-detection is enabled!%s", VTY_NEWLINE);
		}
       return CMD_SUCCESS;
    }
    else
    {
       ipc_set.loopback_enable = iLoopback;
    }
	ipc_set.loop_update = info.loop_update;/*Add by huangmingjian 2013-09-23 for Bug 161*/
	
    iRet = ipc_set_sys_loop(&ipc_set);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    return CMD_SUCCESS;
}
#endif

/*Add by zhouguanhua 2013/6/7*/
/*****************************************************************
    Function:logging_enble
    Description: func to enable or disable log 
    Author:zhouguanhua
    Date:2013/6/7
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (logging_enble,
		logging_enable_cmd,
		"logging enable",
		"Commands of configure logging\n"
		"Enable function\n"
		)
{
#define ENABLE  1
#define DISABLE 0
	
	int iRet = IPC_STATUS_OK;
    int logback = 0;
    sys_syslog_t info;    

    memset(&info, 0x0, sizeof(sys_syslog_t));  
    iRet = get_syslog_value(&info,0);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system log failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 

    if(strstr(self->string, "no")) 
    {
        if(DISABLE == info.syslog_en)
        {
            vty_out(vty, " INFO: the syslog has already been default set (disable)!%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }
        else
        {
            logback=DISABLE;
        }
    }
    else
    {

            logback = ENABLE;
    }
  
    info.syslog_en = logback;      
    	
    iRet = ipc_set_syslog(&info);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: set system log failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    return CMD_SUCCESS;

}

ALIAS (logging_enble,
        no_logging_enable_cmd,
        "no logging enable",
        NO_DESC_STR
        "Commands of configure logging\n"
        "Enable function\n");


DEFUN (logging_save_severity,
        logging_save_severity_cmd,
        "logging buffered severity <0-7>",
        "Commands of configure logging\n"
        "logging messages save to RAM\n"
        "Specify logging level\n")
{

   int iRet = IPC_STATUS_OK;
   int iVlanId = 0;
   
   sys_syslog_t info;
   
   iVlanId = strtoul(argv[0], NULL, 0);
   
   memset(&info, 0x0, sizeof(sys_syslog_t));  
   
   iRet = get_syslog_value(&info,0);
   if(IPC_STATUS_OK != iRet)
   {
       vty_out(vty, " ERROR: get system log failed!%s", VTY_NEWLINE);
       return CMD_WARNING;        
   } 

   info.syslog_save_degree=iVlanId;
      
   iRet = ipc_set_syslog(&info); 
   if(IPC_STATUS_OK != iRet)    
   {
       vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
       return CMD_WARNING;        
   }  
   
   return CMD_SUCCESS;
}

DEFUN (logging_host,
        logging_host_cmd,
        "logging host A.B.C.D severity <0-7>", 
        "Commands of configure logging\n"
        "Setting of logging host configuration\n"
        "IP Address format is A.B.C.D (where the log messages send)\n"
        "Specify logging level\n"
        "Specify logging level\n")
{

   int iRet = IPC_STATUS_OK;
   sys_syslog_t info;
   struct in_addr host_ip;
   char defaultIp[]="0.0.0.0";  
   memset(&info, 0x0, sizeof(sys_syslog_t));     
   iRet = get_syslog_value(&info,0);
   if(IPC_STATUS_OK != iRet)
   {
       vty_out(vty, " ERROR: get system log failed!%s", VTY_NEWLINE);
       return CMD_WARNING;        
   } 

   if(strstr(self->string, "no")) 
   {
        memset(info.syslog_ip,0,sizeof(info.syslog_ip));
        strcpy(info.syslog_ip, defaultIp);
   }
   else
   {
       inet_aton(argv[0], &host_ip); 
       if ((inet_aton(argv[0], &host_ip) == 0) || !valid_ip(host_ip))
       {
           vty_out(vty, " ERROR: invalid IP address : %s%s", argv[0], VTY_NEWLINE);
           return CMD_WARNING;
       } 
       safe_strncpy(info.syslog_ip, argv[0], sizeof(info.syslog_ip));

       if (argc > 1)
       {
           info.syslog_degree = strtoul(argv[1], NULL, 0);
       }
   }
  
   iRet = ipc_set_syslog(&info); 
   if(IPC_STATUS_OK != iRet)    
   {
       vty_out(vty, " ERROR: set system log failed!%s", VTY_NEWLINE);
       return CMD_WARNING;        
   }  
   
   return CMD_SUCCESS;
}
ALIAS (logging_host,
        no_logging_host_cmd,
        "no logging host",
        NO_DESC_STR
        "Commands of configure logging\n"
        "No send logging messages to remote host\n");

DEFUN (show_logging,
        show_log_cmd,
        "show logging",
        "Show syslog\n"
        "syslog\n"       
        )
{
    int iRet = IPC_STATUS_OK;
    sys_syslog_t info;
    char *level[]={"Emergency","Alert","Critical","Error","Warning","Notice","Informational","Debug"};  

    memset(&info, 0x0, sizeof(sys_syslog_t));     
    iRet = get_syslog_value(&info,0);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system log failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 

    vty_out(vty, "logging: %s%s",(info.syslog_en == 1) ? "enabled" : "disabled", VTY_NEWLINE);
    vty_out(vty, "Log host: %s", VTY_NEWLINE);
    vty_out(vty, "    ip: %s%s",info.syslog_ip, VTY_NEWLINE);
    vty_out(vty, "    level: %d(%s) Setting of logging host%s",info.syslog_degree,level[info.syslog_degree], VTY_NEWLINE);
    vty_out(vty, "Log buffer: %s", VTY_NEWLINE);
    vty_out(vty, "    level: %d(%s) Save to RAM%s",info.syslog_save_degree,level[info.syslog_save_degree], VTY_NEWLINE);
    return CMD_SUCCESS;
}
/*END by zhouguanhua 2013/6/7*/


/*****************************************************************
    Function:shutdown_vlan_interface
    Description:shutdown vlan interface
    Author:liaohongjun
    Date:2012/9/18
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (shutdown_vlan_interface,
        shutdown_vlan_interface_cmd,
        "shutdown",
        SHUTDOWN_DESC_STR)
{
	/*begin adde by wanghuanyu for 129*/
	if(strstr(self->string, "no")) 
    { 
		ipc_shutdown_intf(ipc_fd,vty->ifindex,UNDOSHUTDOWN);
	}
	else
	{
		ipc_shutdown_intf(ipc_fd,vty->ifindex,SHUTDOWN);
	}
	if(vty->address && strcmp(vty->address, "Console")) //telnet??
    {
        vty->status = VTY_CLOSE;
    }
	return CMD_SUCCESS;
	/*begin adde by wanghuanyu for 129*/
#if 0 //add by zhouguanhua 2013/6/8

    int iRet = IPC_STATUS_OK;
    ipc_network_t ipc_set;
    sys_network_t info;

    memset(&info, 0x0, sizeof(sys_network_t));  
    iRet = ipc_get_sys_networking(&info);
    if(IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
    
    memset(&ipc_set, 0, sizeof(ipc_set));  
    safe_strncpy(ipc_set.hostname, info.hostname, sizeof(ipc_set.hostname));
    if(strstr(self->string, "no")) 
    {  
        if(!strcmp(info.interface_state, INTERFACE_STATE_UP)) 
        {
            vty_out(vty, " INFO: interface current state is UP%s", VTY_NEWLINE);            
            return CMD_SUCCESS;            
        }
        else
        {
            safe_strncpy(ipc_set.interface_state, INTERFACE_STATE_UP, sizeof(ipc_set.interface_state));  
        }
    }
    else
    {
        if(!strcmp(info.interface_state, INTERFACE_STATE_DOWN)) 
        {
            vty_out(vty, " INFO: interface current state is DOWN%s", VTY_NEWLINE);            
            return CMD_SUCCESS;            
        }
        else
        {
            safe_strncpy(ipc_set.interface_state, INTERFACE_STATE_DOWN, sizeof(ipc_set.interface_state)); 
        }
    }
   
    ipc_set.ip_proto = info.ip_proto;
    #if  0//add by zhouguanhua 2013/5/31
    ipc_set.m_vlan = info.m_vlan;
    ipc_set.vlan_interface = info.vlan_interface;

	/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	ipc_set.loopback_enable = info.loopback_enable;
	ipc_set.loop_update = info.loop_update;
	/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
	#endif  //add by zhouguanhua 2013/5/31
    iRet = ipc_set_sys_networking(&ipc_set);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }    
     //modify by zhouguanhua 2012-12-3 of EPN204QID0008
     /* Close connection. */
    if(vty->address && strcmp(vty->address, "Console")) //telnet??
    {
        vty->status = VTY_CLOSE;
    }
    //end modify by zhouguanhua 2012-12-3 of EPN204QID0008
    
    return CMD_SUCCESS;
    #endif
}
ALIAS(shutdown_vlan_interface, 
        no_shutdown_vlan_interface_cmd, 
        "no shutdown", 
        NO_DESC_STR 
        SHUTDOWN_DESC_STR);


/*****************************************************************
    Function:ip_address
    Description:set system ip  address
    Author:liaohongjun
    Date:2012/9/18
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/

DEFUN (ip_address,
        ip_address_cmd,
        "ip address A.B.C.D A.B.C.D [A.B.C.D]",
        "Commands of configure interface IP address\n"
        "Specify IP address\n"
        "IP address with format A.B.C.D\n"
        "IP mask with format A.B.C.D\n"
        "Gateway address with format A.B.C.D\n")
{
    struct in_addr ip, msk, gw;
    int has_gw = 0, ret, i;
    sys_network_t info;
    ipc_network_t ipc_set;

    memset(&info, 0, sizeof(info));
    ret = get_sys_networking_value(&info,0);
    if(IPC_STATUS_OK != ret)
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    
    if(IP_PROTO_DYNAMIC== info.ip_proto)
    {
        vty_out(vty, " INFO: can't specify ip address in ip dynamic mode!%s", VTY_NEWLINE);
        return CMD_SUCCESS; 
    }
    
    inet_aton(argv[0], &ip);
    inet_aton(argv[1], &msk);
    if (argc > 2)
    {
        has_gw = 1;
        inet_aton(argv[2], &gw);
    }

    if ((inet_aton(argv[0], &ip) == 0) || !valid_ip(ip))
    {
        vty_out(vty, " ERROR: invalid IP address : %s%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }
    if ((inet_aton(argv[1], &msk) == 0) || !valid_subnet_mask(ip, msk))
    {
        vty_out(vty, " ERROR: invalid IP address : %s or invalid Subnet mask : %s%s", argv[0], argv[1], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (has_gw)
    {
        if ((inet_aton(argv[2], &gw) == 0) || !valid_gateway(ip, msk, gw))
        {
            vty_out(vty, " ERROR: invalid gateway address : %s%s", argv[2], VTY_NEWLINE);
            return CMD_WARNING; 
        }
    }
    //modify by zhouguanhua 2012-12-3 of EPN204QID0008
    if((ip.s_addr==info.netif.ip.s_addr)&&( msk.s_addr==info.netif.subnet.s_addr)&&(0==has_gw))
    {
         return CMD_SUCCESS;  //ижии??ж╠?IP??Maskoик?-ид'IP??Mask?идик???2???DDижии?? 
    }
    if((ip.s_addr==info.netif.ip.s_addr)&&( msk.s_addr==info.netif.subnet.s_addr)
        &&(1==has_gw)&&(gw.s_addr==info.netif.gateway.s_addr))
    {
         return CMD_SUCCESS;  //ижии??ж╠?IP??Mask??DNSoик?-ид'IP??Mask??DNS?идик???2???DDижии?? 
    }
   //end modify by zhouguanhua 2012-12-3 of EPN204QID0008

    memset(&ipc_set, 0, sizeof(ipc_network_t));

    safe_strncpy(ipc_set.hostname, info.hostname, sizeof(ipc_set.hostname));
    safe_strncpy(ipc_set.interface_state, info.interface_state, sizeof(ipc_set.interface_state));
    ipc_set.ip_proto = IP_PROTO_STATIC;
    ipc_set.netif.ip = ip;
    ipc_set.netif.subnet = msk;
    #if  0//add by zhouguanhua 2013/5/31
	/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	ipc_set.loopback_enable = info.loopback_enable;
	ipc_set.loop_update = info.loop_update;
	/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
	#endif  //add by zhouguanhua 2013/5/31
    if (has_gw)
    {
        ipc_set.netif.gateway = gw;
    }
    else 
    {
        ipc_set.netif.gateway.s_addr = info.netif.gateway.s_addr;//modify by zhouguanhua 2012-12-3,ижии???a??ии??ж╠
    }

    for (i = 0; i < MAX_DNS_NUM; i ++)
    {
        ipc_set.netif.dns[i] = info.netif.dns[i];
    }
    #if  0//add by zhouguanhua 2013/5/31
    ipc_set.m_vlan = info.m_vlan;
    ipc_set.vlan_interface = info.vlan_interface;
    #endif  //add by zhouguanhua 2013/5/31
    ret = ipc_set_sys_networking(&ipc_set);
    if(IPC_STATUS_OK != ret)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    //modify by zhouguanhua 2012-12-3 of EPN204QID0008
     /* Close connection. */
    if(vty->address && strcmp(vty->address, "Console")) //telnet??
    {
        vty->status = VTY_CLOSE;
        vty_out(vty, "Set Ip success ! please login again%s", VTY_NEWLINE);
    }
    //end modify by zhouguanhua 2012-12-3 of EPN204QID0008

    return CMD_SUCCESS;
}

/*****************************************************************
    Function:ip_dynamic
    Description:dynamic get ip  address form dhcp server
    Author:liaohongjun
    Date:2012/9/18
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (ip_dynamic,
        ip_dynamic_cmd,
        "ip dynamic",
        "Commands of configure interface's IP address\n"
        "Dynamic get IP address from DHCP server\n")
{

    int ret = IPC_STATUS_FAIL;
    sys_network_t info;
    ipc_network_t ipc_set;

    memset(&info, 0, sizeof(sys_network_t));
    //ret = ipc_get_sys_networking(&info);//disable by zhouguanhua 2013/5/31
    ret = get_sys_networking_value(&info,0);
    if(IPC_STATUS_OK != ret)    
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

    memset(&ipc_set, 0, sizeof(ipc_network_t));
    safe_strncpy(ipc_set.hostname, info.hostname, sizeof(ipc_set.hostname));
    safe_strncpy(ipc_set.interface_state, info.interface_state, sizeof(ipc_set.interface_state)); 
    ipc_set.ip_proto = IP_PROTO_DYNAMIC;
    if(strstr(self->string, "no"))
    {
        if(IP_PROTO_STATIC == info.ip_proto)
        {
            vty_out(vty, " INFO: ip protocol is static!%s", VTY_NEWLINE);
            return CMD_SUCCESS;              
        }
        else
        {
            ipc_set.action = ACTION_RELEASE; 
        }
    }
    else
    {
        if(IP_PROTO_DYNAMIC == info.ip_proto)
        {
            vty_out(vty, " INFO: ip protocol is dynamic!%s", VTY_NEWLINE);
            return CMD_SUCCESS;              
        }
        else
        {
            ipc_set.action = ACTION_RENEW; 
        }
    }
   #if  0//add by zhouguanhua 2013/5/31
    ipc_set.m_vlan = info.m_vlan;
    ipc_set.vlan_interface = info.vlan_interface;

	/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	ipc_set.loopback_enable = info.loopback_enable;
	ipc_set.loop_update = info.loop_update;
	/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
	#endif  //add by zhouguanhua 2013/5/31
    ret = ipc_set_sys_networking(&ipc_set);
    if(IPC_STATUS_OK != ret)    
    {
        vty_out(vty, " ERROR: set system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    //modify by zhouguanhua 2012-12-3 of EPN204QID0008
     /* Close connection. */
    if(vty->address && strcmp(vty->address, "Console")) //telnet??
    {
        vty->status = VTY_CLOSE;
        vty_out(vty, "Set Ip success ! please login again%s", VTY_NEWLINE);
    }
    //end modify by zhouguanhua 2012-12-3 of EPN204QID0008

    return CMD_SUCCESS;
}
ALIAS(ip_dynamic, 
        no_ip_dynamic_cmd, 
        "no ip dynamic", 
        NO_DESC_STR 
        "Commands of configure interface's IP address\n"
        "Dynamic get IP address from DHCP server\n");


#if 0
DEFUN (dns_server,
		dns_server_cmd,
		"dns-server A.B.C.D [A.B.C.D]",
		STR(dns_server)
		STR(dns1)
		STR(dns2))
{
	struct in_addr dns, dns1;
	int  ret, has_dns1;
	sys_network_t info;
	ipc_network_t ipc_set;

	inet_aton(argv[0], &dns);

	if (argc > 1){
		has_dns1 = 1;
		inet_aton(argv[1], &dns1);		
	}

	if (!valid_ip(dns)){
		vty_out(vty, "Invalid DNS Server IP address : %s%s", inet_ntoa(dns), VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if (has_dns1 && !valid_ip(dns1)){
		vty_out(vty, "Invalid DNS Server IP address : %s%s", inet_ntoa(dns1), VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	
	ret = ipc_get_sys_networking(&info);
	VTY_IPC_ASSERT(ret);

	memset(&ipc_set, 0, sizeof(ipc_set));
	safe_strncpy(ipc_set.hostname, info.hostname, sizeof(ipc_set.hostname));

	ipc_set.ip_proto = info.ip_proto;
	ipc_set.action = ACTION_NONE;

	if (ipc_set.ip_proto == IP_PROTO_DYNAMIC){
		memset(&ipc_set.netif, 0, sizeof(ipc_set.netif));
	}else {
		memcpy(&ipc_set.netif, &info.netif, sizeof(ipc_set.netif));
	}

	// now replace dns
	ipc_set.netif.dns[0] = dns;
	if (has_dns1){
		ipc_set.netif.dns[1] = dns1;
	}
	
	ret = ipc_set_sys_networking(&ipc_set);
	VTY_IPC_ASSERT(ret);
	
	return CMD_SUCCESS;
}
#endif
/*****************************************************************
    Function:show_vlan_interface
    Description:show infomation of vlan interface
    Author:liaohongjun
    Date:2012/9/18
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (show_vlan_interface,
        show_vlan_interface_cmd,
        "show interface vlan-interface <1-4094>",
        SHOW_DESC_STR
        "Interface\n"
        "VLAN interface\n"
        "VLAN interface\n")
{
    int iRet = IPC_STATUS_FAIL;
    sys_network_t info;
   // ipc_network_t ipc_set;
    unsigned int iVlanId = 0;
    sys_mvlan_t  info_mvlan;
	ifindex_t ifindex=0;
	int ifstate=0;
	char ifname[64] = {0};

    iVlanId = strtoul(argv[0], NULL, 0);
    memset(&info, 0, sizeof(sys_network_t));
    iRet = ipc_get_sys_networking(&info);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    //modify by zhouguanhua 2013/6/3
    if(IPC_STATUS_OK != get_sys_mvlan_value(&info_mvlan))    
    {
        vty_out(vty, " ERROR: get system mvlan failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
	
    if(info_mvlan.vlan_interface != iVlanId)
    {
        vty_out(vty, " ERROR: Vlan-interface %d does not exist!%s", iVlanId, VTY_NEWLINE);
        return CMD_WARNING;
    }
    //end modify by zhouguanhua 2013/6/8

	/*Begin modified by huangmingjian 2013/08/30*/
    vty_out(vty, "Vlan-interface %d current state is ", iVlanId, VTY_NEWLINE);   
	sprintf(ifname,IF_L3VLAN_NAMEPREFIX"%d",info_mvlan.vlan_interface);
	IF_GetByIFName(ifname,&ifindex);
	IF_GetAttr(ifindex, IF_STATE, &ifstate, sizeof(ifstate));
	
	if(IF_UP(ifstate))
	{
		vty_out(vty, "%s%s", "UP", VTY_NEWLINE);
	}
	else
 	{
 		vty_out(vty, "%s%s", "DOWN", VTY_NEWLINE);
 	}
  	/*End modified by huangmingjian 2013/08/30*/
	 
    vty_out(vty, "Hardware Address is %s%s", mac2str(&info.mac), VTY_NEWLINE);
    vty_out(vty, "IP PROTO is %s%s", (info.ip_proto == IP_PROTO_STATIC) ? "Static" : "Dynamic", VTY_NEWLINE);
    vty_out(vty, "Internet Address is %s%s", inet_ntoa(info.netif.ip), VTY_NEWLINE);
    vty_out(vty, "Subnet Mask is %s%s", inet_ntoa(info.netif.subnet), VTY_NEWLINE);
    vty_out(vty, "Gateway is %s %s%s", inet_ntoa(info.netif.gateway), VTY_NEWLINE, VTY_NEWLINE);

    return CMD_SUCCESS;
}
/*****************************************************************
    Function:show_ip_interface
    Description:show infomation of ip interface
    Author:liaohongjun
    Date:2012/9/18
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (show_ip_interface,
        show_ip_interface_cmd,
        "show ip interface",
        SHOW_DESC_STR   
        "Commands of configure interface's IP address\n"
        "Interface\n")
{
    int iRet = IPC_STATUS_FAIL;
    sys_network_t info;
  //  ipc_network_t ipc_set;
    char *dhcp_status;
    sys_mvlan_t info_mvlan;
    ifindex_t ifindex;
	char ifname[64];
	int ifstate=0;
# define DHCP_STATUS_STR_STOP 	"Stoped"
# define DHCP_STATUS_STR_DOING 	"Fetching"
# define DHCP_STATUS_STR_OK 	"Success"
# define DHCP_STATUS_STR_FAILED "Failed"

    memset(&info, 0, sizeof(sys_network_t));
    iRet = ipc_get_sys_networking(&info);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    /*modify by zhouguanhua 2013/6/3*/  
    if(IPC_STATUS_OK != get_sys_mvlan_value(&info_mvlan))
    {
        vty_out(vty, " ERROR: get system mvlan failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    } 
     /*end modify by zhouguanhua 2013/6/3*/ 


     vty_out(vty, "Vlan-interface %d current state is ", info_mvlan.vlan_interface, VTY_NEWLINE); 
	 sprintf(ifname,IF_L3VLAN_NAMEPREFIX"%d",info_mvlan.vlan_interface);
	 IF_GetByIFName(ifname,&ifindex);
	 IF_GetAttr(ifindex,IF_STATE,&ifstate,sizeof(ifstate));
	if(IF_UP(ifstate))
	{
		vty_out(vty, "%s%s", "UP", VTY_NEWLINE);
	}
	else
 	{
 		vty_out(vty, "%s%s", "DOWN", VTY_NEWLINE);
 	}
	#if 0
    if(!strcmp(info.interface_state, INTERFACE_STATE_UP))
    {
        vty_out(vty, "%s%s", "UP", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%s%s", "DOWN", VTY_NEWLINE);
    }
    #endif
    vty_out(vty, "Mac Address     : %s%s", mac2str(&info.mac), VTY_NEWLINE);
    vty_out(vty, "IP PROTO        : %s%s", (info.ip_proto == IP_PROTO_STATIC) ? "Static" : "Dynamic", VTY_NEWLINE);
    vty_out(vty, "IP Address      : %s%s", inet_ntoa(info.netif.ip), VTY_NEWLINE);
    vty_out(vty, "Subnet Mask     : %s%s", inet_ntoa(info.netif.subnet), VTY_NEWLINE);
    vty_out(vty, "Gateway         : %s%s", inet_ntoa(info.netif.gateway), VTY_NEWLINE);

    if (info.ip_proto == IP_PROTO_DYNAMIC)
    {
        if (info.ip_proto == IP_PROTO_STATIC)
        {
            dhcp_status = DHCP_STATUS_STR_STOP;
        }
        else if (info.netif.ip.s_addr == 0)
        {
            dhcp_status = DHCP_STATUS_STR_DOING;
        }
        else 
        {
            dhcp_status = DHCP_STATUS_STR_OK;
        }
        vty_out(vty, " DHCP Status    : %s%s", dhcp_status, VTY_NEWLINE);
        vty_out(vty, " Lease Time     : %u%s", info.lease_time, VTY_NEWLINE);			
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}

/*end added by liaohongjun 2012/9/18 of EPN104QID0019*/

#ifdef CONFIG_LOOPD
/*****************************************************************
    Function:show_loopback_detection
    Description:show uni infomation of loopback detection
    Author:huangmingjian
    Date:2012-11-29
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (show_loopback_detection,
        show_loopback_detection_cmd,
        "show loopback-detection",
        SHOW_DESC_STR   
        "Lookback detection function\n")
{
	# define LOOP_DISAABLE  0
	
    int iRet = IPC_STATUS_FAIL;
	char *STATUS_STRING[] = {"disabled", "blocking", "listening", "forwarding"};
	unsigned char i = 0;
	unsigned char loop_flag = 0;
    sys_loop_t info;
	
    memset(&info, 0, sizeof(sys_loop_t));
    iRet = ipc_get_sys_loop(&info);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: get system loop failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

	if(LOOP_DISAABLE == info.loopback_enable)
	{
		vty_out(vty, "Loopback-detection function is disabled%s", VTY_NEWLINE);
	}
	else
	{	
		vty_out(vty, "Loopback-detection function is enabled%s", VTY_NEWLINE);
		
		for(i=0; i<MAX_PORT_NUM; i++)
		{
			if(!strcmp("blocking", STATUS_STRING[info.loopback_unis_status[i]]))
			{	
				loop_flag = 1;
				vty_out(vty, "Port %d has been detected existing loopback link status%s", i+1, VTY_NEWLINE);
			}
			if(!strcmp("listening", STATUS_STRING[info.loopback_unis_status[i]]))
			{	
				loop_flag = 1;
				vty_out(vty, "Port %d has been detected in listening status%s", i+1, VTY_NEWLINE);
			}
		}
		if(0 == loop_flag)
		{
			vty_out(vty, "No port has been detected existing loopback link status%s", VTY_NEWLINE);
		}
	}
	
    vty_out(vty, "%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}
#endif

/*****************************************************************
    Function:mac_address_timer_aging
    Description:set mac address timer aging
    Author:huangmingjian
    Date:2013-09-07
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/


DEFUN (mac_address_timer_aging,
        mac_address_timer_aging_cmd,
        "mac-address timer aging <45-458>",
        "Configure MAC address\n"   
        "Configure timer\n"
		"Global aging time\n"
		"Global aging time (second)")
{
	unsigned int aging_time = MAC_AGINGTIME_MIN;
    ipc_mac_setting_ack_t *pack = NULL;
	mac_setting_t info;
	
	if(strstr(self->string, "no")) 
	{
		aging_time = MAC_AGINGTIME_DISABLE;
	}
	else
	{	
		aging_time = strtoul(argv[0], NULL, 0);
	}

	memset(&info, 0, sizeof(info));
	info.aging_time = aging_time;
    pack = ipc_mac_setting_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW , &info);
 	if(!pack || (pack->hdr.status != IPC_STATUS_OK)) 
	{	
		vty_out(vty, "Set MAC aging time fail!%s", VTY_NEWLINE);
 	}
		
    if(pack)
    {
    	free(pack);
    }

    return CMD_SUCCESS;
}


ALIAS (mac_address_timer_aging,
	mac_address_timer_no_aging_cmd,
	"mac-address timer no-aging",
	"Configure MAC address\n"	
	"Configure timer\n"
	" Disable aging\n");


/*****************************************************************
    Function:show_mac_address_aging_time
    Description:get mac address timer aging
    Author:huangmingjian
    Date:2013-09-07
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/


DEFUN (show_mac_address_aging_time,
        show_mac_address_aging_time_cmd,
        "show mac-address aging-time",
        SHOW_DESC_STR   
        "Configure MAC address\n"
		"Global aging time\n")
{
    ipc_mac_setting_ack_t *pack = NULL;
    unsigned int aging_time = 0;
	
    pack = ipc_mac_setting_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		aging_time = pack->mac_setting.aging_time;
 	}
	else 
	{
		vty_out(vty, "Get MAC aging time fail!%s", VTY_NEWLINE);
	    if(pack)
	    	free(pack);
		return CMD_WARNING;
	}
		
    if(pack)
    	free(pack);
	
	if(MAC_AGINGTIME_DISABLE == aging_time)
	{
		vty_out(vty, "Mac address aging time: no aging%s", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "Mac address aging time: %d(s)%s", aging_time, VTY_NEWLINE);
	}
	
    return CMD_SUCCESS;
}

/*Begin add by linguobin 2013-12-09*/
#if defined(CONFIG_ONU_COMPATIBLE)


DEFUN_HIDDEN (compatible_common_mode,
        compatible_common_mode_cmd,
        "compatible",  
        "Compatible mode\n"
)
{
    pid_t  pid = 0;
    int val = ENABLE;
	int ret = CMD_SUCCESS;
    ipc_compatible_setting_ack_t *pack = NULL;
    compatible_setting_t sys;
    memset(&sys, 0, sizeof(sys));

 	if(strstr(self->string, "no")) 
    {	
		val = DISABLE;
 	}
	else 
	{
		val = ENABLE;
	}

    ret = oam_ipc_get_sys_compatible(ipc_fd,&sys);
    if(ret != IPC_STATUS_OK)
    {
        vty_out(vty, "Get compatible mode failed!%s", VTY_NEWLINE); 
        return CMD_WARNING;        
    }
 
    if(val == sys.enable)
    {
        vty_out(vty, "Compatible mode has been set to %s!%s", sys.enable?ENABLE_STR:DISABLE_STR, VTY_NEWLINE);  		
        return CMD_SUCCESS;
    }
    
	sys.enable = val;
    pack = ipc_compatible_setting_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW , &sys);
 	if(!pack || (pack->hdr.status != IPC_STATUS_OK)) 
	{	
		vty_out(vty, "Set compatible mode fail!%s", VTY_NEWLINE);
        if(pack)
        {
        	free(pack);
        }
        return CMD_WARNING; 
 	}
		
    if(pack)
    {
    	free(pack);
    }
    if((pid = get_pid_by_name("eponoamd")) > 0)
    {
        if(-1 == kill(pid, SIGUSR1))
        {   
        	ret = CMD_WARNING;
        }          
    }
	else
	{
		ret = CMD_WARNING;
	}
	vty_out(vty, "Set compatibility mode %s successfully, please save and reboot system to take effect!%s", sys.enable?ENABLE_STR:DISABLE_STR, VTY_NEWLINE);

	return ret;
}
ALIAS_HIDDEN(compatible_common_mode,
	no_compatible_common_mode_cmd,
	"no compatible",
	NO_DESC_STR
	"Compatible mode\n"	
);
#endif
/*End add by linguobin 2013-12-09*/

/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
DEFUN_HIDDEN (pontest,
	        pontest_cmd,
	        "pontest <0-1>",  
	        "0 means start calibration,1 means end calibration\n")
{
	UINT32 cfgFTOverFLAG = strtoul(argv[0], NULL, 0);

	if(DRV_OK != Ioctl_SetPonftoverflag(cfgFTOverFLAG))
	{
        vty_out(vty, "Set pontest failed!%s", VTY_NEWLINE); 
		return CMD_WARNING;
	}
	if(0 == cfgFTOverFLAG)
	{
		vty_out(vty, "Setting 0 OK, it should be set to 1 again, or laser dosen't work normaly after reboot system!%s", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "Set pontest to 1 successfully!%s", VTY_NEWLINE);
	}
    return CMD_SUCCESS;
}


DEFUN_HIDDEN (apcsetval,
	        apcsetval_cmd,
	        "apcsetval <0-255>",  
	        "Register value\n")
{
	UINT32 cfgApcVal = strtoul(argv[0], NULL, 0);
	
	if(DRV_OK != Ioctl_SetApcset(cfgApcVal))
	{
        vty_out(vty, "Set apcsetval to register failed!%s", VTY_NEWLINE); 
		return CMD_WARNING;
	}
	
	vty_out(vty, "Set apcsetval(%d) to register successfully!%s", cfgApcVal, VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN_HIDDEN (modsetval,
	        modsetval_cmd,
	        "modsetval <0-255>",  
	        "Register value\n")
{
	UINT32 cfgModVal = strtoul(argv[0], NULL, 0);

	if(DRV_OK != Ioctl_SetImodset(cfgModVal))
	{
        vty_out(vty, "Set modsetval to register failed!%s", VTY_NEWLINE); 
		return CMD_WARNING;
	}
	
	vty_out(vty, "Set modsetval(%d) to register successfully!%s", cfgModVal, VTY_NEWLINE);
    return CMD_SUCCESS;
}


DEFUN_HIDDEN (laseron,
		laseron_cmd,
		"laser (normal|on|off)",
		"Laser\n"
		"Nomal laser\n"
		"Enable laser\n"
		"Disalbe laser\n")
{
	UINT32 action = LASER_NAOMAL;	
	UINT32 readData = LASER_NAOMAL;
	char *action_info[] = {"normal", "on", "off"};
	UINT32 laserOnFlag = LASEON_TEST_DISABLE;  

	if(strstr("normal", (char *)argv[0]))
	{
		action = LASER_NAOMAL;
	}
	else if(strstr("on", (char *)argv[0])) 
	{
		action = LASER_FORCE_ON;
	}
	else if(strstr("off", (char *)argv[0]))
	{
		action = LASER_FORCE_OFF;
	}
	else
	{
		vty_out(vty, "ERROR: set laser paras error!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(DRV_OK != Ioctl_GetLaser(&readData))
	{
		vty_out(vty, "ERROR: get laser paras error!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(readData != action)
	{
		if(DRV_OK != Ioctl_SetLaser(action))
		{
			vty_out(vty, "ERROR: set laser paras error!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	if(LASER_FORCE_ON == action)
	{	
		laserOnFlag = LASEON_TEST_ENABLE;
	}
	if(DRV_OK !=Ioctl_SetLaserOnTestFlag(laserOnFlag))
	{
		vty_out(vty, "Set laserOnTestFlag enable failed.%s", VTY_NEWLINE);
		return CMD_WARNING;	
	}
	
	vty_out(vty, "Set laser %s successfully.%s",  action_info[action], VTY_NEWLINE); 

	return CMD_SUCCESS;

}

DEFUN_HIDDEN (show_laser_parameter,
					show_laser_parameter_cmd,
					"show laser-parameter)",
					SHOW_DESC_STR
					"laser parameter\n")
{
	int ponTestFlag = 0;
	int setAction = 0;
	int apcVal = 0;
	int modVal = 0;

	char *action_info[] = {"normal", "on", "off"};
	
	if(0 != Ioctl_GetPonftoverflag(&ponTestFlag))
	{
		vty_out(vty, "ERROR: get pontest error!%s", VTY_NEWLINE);
	}


	if(0 != Ioctl_GetApcset(&apcVal))
	{
		vty_out(vty, "ERROR: get apcsetval error!%s", VTY_NEWLINE);
	}

	if(0 != Ioctl_GetImodset(&modVal))
	{
		vty_out(vty, "ERROR: get modsetval error!%s", VTY_NEWLINE);
	}

	if(0 != Ioctl_GetLaser(&setAction) || setAction<LASER_NAOMAL || setAction> LASER_FORCE_OFF)
	{
		vty_out(vty, "ERROR: get laser status error!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    vty_out(vty, "pontest     	: %d%s", ponTestFlag, VTY_NEWLINE);
    vty_out(vty, "apcsetval   	: %d%s", apcVal, VTY_NEWLINE);
    vty_out(vty, "modsetval   	: %d%s", modVal, VTY_NEWLINE);
    vty_out(vty, "laser status    : %s%s", action_info[setAction], VTY_NEWLINE);

	return CMD_SUCCESS;

}
#endif
/*Begin add by huangmingjian 2014-01-13*/


void cmd_interface_init(void)
{
    /*begin modified by liaohongjun 2012/9/18 of EPN104QID0019*/
    /*vlan interface init*/
    vlan_interface_node.prompt_func = cmd_vlan_interface_node_prompt;
    install_node(&vlan_interface_node, NULL);  
    install_default(VLAN_INTERFACE_NODE);
    install_element(CONFIG_NODE, &access_vlan_interface_cmd);
    install_element(CONFIG_NODE, &management_vlan_set_cmd);
#ifdef CONFIG_LOOPD	
	install_element(CONFIG_NODE, &loopback_enable_cmd);    /*Add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
#endif
	install_element(CONFIG_NODE, &no_management_vlan_cmd);
	//added 2013-08-06
	install_element(CONFIG_NODE, &epon_loid_cmd);
    /*add by zhouguanhua 2013/6/7*/
    install_element(CONFIG_NODE, &logging_enable_cmd);
   install_element(CONFIG_NODE, &logging_save_severity_cmd);
    install_element(CONFIG_NODE, &logging_host_cmd);
    install_element(CONFIG_NODE, &no_logging_host_cmd);
    install_element(CONFIG_NODE, &no_logging_enable_cmd);
    install_element(CONFIG_NODE, &show_log_cmd);
    /*add by zhouguanhua 2013/6/7*/
    install_element(VLAN_INTERFACE_NODE, &shutdown_vlan_interface_cmd);
    install_element(VLAN_INTERFACE_NODE, &no_shutdown_vlan_interface_cmd); 
    install_element(VLAN_INTERFACE_NODE, &ip_address_cmd);
    install_element(VLAN_INTERFACE_NODE, &ip_dynamic_cmd); 
    install_element(VLAN_INTERFACE_NODE, &no_ip_dynamic_cmd);     
    

    /*interface init*/

   
    interface_node.prompt_func = cmd_interface_node_prompt;
    install_node(&interface_node, cmd_interface_config_write);    
    install_default(INTERFACE_NODE);

	#ifdef SWITCH_STYLE
	install_element(CONFIG_NODE, &interface_cmd);	
	install_element(CONFIG_NODE, &interface_cable_cmd);
	install_element(ENABLE_NODE, &show_interface_statistics_cmd);
    install_element(ENABLE_NODE, &show_interface_switchport_cmd);
	install_element(ENABLE_NODE, &show_interface_cableport_cmd);
	
	install_element(CONFIG_NODE, &show_interface_statistics_cmd);
    install_element(CONFIG_NODE, &show_interface_switchport_cmd); 
	install_element(CONFIG_NODE, &show_interface_cableport_cmd);
    install_element(INTERFACE_NODE, &show_switchport_cmd);
	#endif
	#if (CMD_CURRENT_SUPPORT) 
	 install_element(VLAN_INTERFACE_NODE, &dns_server_cmd);     
    install_element(ENABLE_NODE, &show_interface_cmd);
	
 

    install_element(CONFIG_NODE, &show_interface_cmd);

    install_element(INTERFACE_NODE, &show_current_cmd);
    install_element(INTERFACE_NODE, &interface_cmd);
    install_element(INTERFACE_NODE, &show_statistics_cmd);
    install_element(INTERFACE_NODE, &show_switchport_cmd);
    install_element(INTERFACE_NODE, &shutdown_cmd); 
    install_element(INTERFACE_NODE, &no_shutdown_cmd); 
    install_element(INTERFACE_NODE, &auto_neg_cmd);
    install_element(INTERFACE_NODE, &no_auto_neg_cmd); 
    install_element(INTERFACE_NODE, &duplex_cmd);
    install_element(INTERFACE_NODE, &speed_cmd);
    install_element(INTERFACE_NODE, &flow_control_cmd);
    install_element(INTERFACE_NODE, &no_flow_control_cmd); 
    install_element(INTERFACE_NODE, &bandwidth_ingress_cmd);
    install_element(INTERFACE_NODE, &bandwidth_egress_cmd);
    install_element(INTERFACE_NODE, &no_bandwidth_ingress_cmd); 
    install_element(INTERFACE_NODE, &no_bandwidth_egress_cmd); 
    install_element(INTERFACE_NODE, &priority_cmd);
    install_element(INTERFACE_NODE, &no_priority_cmd); 
    install_element(INTERFACE_NODE, &statistics_reset_cmd);
#endif
#ifdef SWITCH_STYLE
    install_element(INTERFACE_NODE, &switchport_access_cmd);
    install_element(INTERFACE_NODE, &switchport_trunk_cmd);
    install_element(INTERFACE_NODE, &switchport_hybrid_cmd);
	/*Begin add by huangmingjian 2013-09-07 for mac aging time*/
    install_element(CONFIG_NODE, &mac_address_timer_aging_cmd);
    install_element(CONFIG_NODE, &mac_address_timer_no_aging_cmd);
    install_element(CONFIG_NODE, &show_mac_address_aging_time_cmd);
	/*Begin add by huangmingjian 2013-09-07  for mac aging time*/
#endif
    /*end modified by liaohongjun 2012/9/18 of EPN104QID0019*/
#if defined(CONFIG_ONU_COMPATIBLE)
    install_element(VIEW_NODE, &compatible_common_mode_cmd);
    install_element(VIEW_NODE, &no_compatible_common_mode_cmd);
#endif
 
/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
	install_element(VIEW_NODE, &pontest_cmd);
	install_element(VIEW_NODE, &apcsetval_cmd);
	install_element(VIEW_NODE, &modsetval_cmd);
	install_element(VIEW_NODE, &laseron_cmd);
	install_element(VIEW_NODE, &show_laser_parameter_cmd);
#endif
/*Begin add by huangmingjian 2014-01-13*/

}







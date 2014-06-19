/*
 * demo_interface.c
 *
 *  Created on: Oct 30, 2010
 *      Author: ryan
 */

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "vty.h"
#include <ipc_client.h>
//#include <switch.h>
//#include <vlan_utils.h>
#include <debug.h>

extern int ipc_fd;

// switch_interface_t interfaces[MAX_CLT_ETH_PORTS];
static char *ifnames[MAX_CLT_ETH_PORTS];
static int ifcount = 0;
static switch_interface_t current_interface;

#define	SIMPLE_SET_INTERFACE(member, value) do { \
		((switch_interface_t *)vty->index)->member = (value); \
		set_interface_config((switch_interface_t *)vty->index); } while(0)

#define	GET_CURRENT_CONFIG(member) (((switch_interface_t *)vty->index)->member)

char *if_fancy_name(char *ifname)
{
	if(!strcmp(ifname, "eth0")) return "Ethernet 1/1";
	if(!strcmp(ifname, "eth1")) return "Ethernet 1/2";
	if(!strcmp(ifname, "cab0")) return "Cable 1/1";
	if(!strcmp(ifname, "cab1")) return "Cable 1/2";
	return "";
}

char *if_fancy_mode(int mode)
{
	switch(mode) {
	case ETH_SWITCHPORT_ACCESS:
		return "Access";
	case ETH_SWITCHPORT_TRUNK:
		return "Trunk";
	case ETH_SWITCHPORT_HYBRID:
		return "Hybrid";
	}
	return "Unknown";
}

char *interface_prompt(struct vty *vty)
{
	switch_interface_t *ifp = (switch_interface_t *)vty->index;
	static char prompt[40];
	sprintf(prompt, "config-if(%s)# ", if_fancy_name(ifp->ifname));
	return prompt;
}

static struct cmd_node interface_node =
		{ INTERFACE_NODE, NULL, interface_prompt, 1, };

static int demo_interface_config_write(struct vty *vty) 
{   
	vty_out(vty, "!%s", VTY_NEWLINE);	
}

int get_interface_config(const char *arg, const char *cmd, switch_interface_t *ifp)
{
	ipc_clt_interface_config_ack_t *pack;
	int slot, ifindex, ret, i;
	char ifname[IFNAMSIZ];

	ret = if_arg_decode2((char *)arg, &slot, &ifindex);
	sprintf(ifname, "%s%d", strstr(cmd, "ethernet") ? "eth" : "cab", ifindex - 1);
	for(i = 0; i < ifcount; i ++) {
		if(!strcmp(ifname, ifnames[i])) {
			ifindex = i;
			break;
		}
	}
	if(i >= ifcount) return -1;

	memset(ifp, 0, sizeof(switch_interface_t));
	strcpy(ifp->ifname, ifnames[ifindex]);
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, ifp, 1);
	ret = -1;
	if(pack && pack->hdr.status == IPC_STATUS_OK && pack->count == 1) {
		memcpy(ifp, &pack->cfg[0], offsetof(switch_interface_t, trunk_hybrid_array));
		if(ifp->trunk_hybrid) free(ifp->trunk_hybrid);
		if(pack->cfg[0].count > 0) {
			ifp->trunk_hybrid = (switch_vlan_t *)malloc(pack->cfg[0].count * sizeof(switch_vlan_t));
			DBG_ASSERT(ifp->trunk_hybrid, "Memory allocate fail");
			if(!ifp->count) goto out;
			memcpy(ifp->trunk_hybrid, pack->cfg[0].trunk_hybrid_array, sizeof(switch_vlan_t) * pack->cfg[0].count);
		}
		else
			ifp->trunk_hybrid = NULL;
		ret = 0;
	}

out:
	if(pack) free(pack);
	return ret;
}

int set_interface_config(switch_interface_t *ifp)
{
	ipc_clt_interface_config_ack_t *pack;
	int ret = -1;

	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, ifp, 1);
	DBG_ASSERT(pack, "IPC NOACK");
	if(pack){
		DBG_ASSERT(pack->hdr.status == IPC_STATUS_OK, "%s", ipc_strerror(pack->hdr.status));
		ret = pack->hdr.status;
		free(pack);
	}
	return ret;
}

DEFUN (interface,
       interface_eth_cmd,
       "interface ethernet IFNAME",
       "Select an interface to configure\n"
       "Ethernet interface\n"
       "Interface's name\n")
{
	int ret;

	ret = get_interface_config(argv[0], self->string, &current_interface);
	if(ret < 0) {
		vty_out(vty, "Fail to get interface status%s", VTY_NEWLINE);
		goto out;
	}

	vty->index = &current_interface;
	vty->node = INTERFACE_NODE;
out:
	return CMD_SUCCESS;
}

ALIAS(interface,
		interface_cab_cmd,
       "interface cable IFNAME",
       "Select an interface to configure\n"
       "Cable interface\n"
       "Interface's name\n")

DEFUN (interface_switchport,
		interface_switchport_cmd,
		"switchport mode (access|hybrid|trunk)",
		"Specify switch port characteristics\n"
		"Specify switch port mode\n"
		"Access switch port\n"
		"Hybrid switch port\n"
		"Trunk switch port\n")
{
	SIMPLE_SET_INTERFACE(mode, switchport_atoi(argv[0]));
	return CMD_SUCCESS;
}

static void mode_err(struct vty *vty, char *mode)
{
	vty_out(vty, "Switchport %s is not on %s mode%s", if_fancy_name(GET_CURRENT_CONFIG(ifname)), mode, VTY_NEWLINE);
}

DEFUN(interface_switchport_access,
		interface_switchport_access_cmd,
		"switchport access vlan <1-4094>",
		"Specify switch port characteristics\n"
		"Set access mode characteristics of the interface\n"
		"Set Vlan when interface is in access mode\n"
		"Vlan ID <1-4094>\n")
{
	if(strstr(self->string, "access") && (GET_CURRENT_CONFIG(mode) != ETH_SWITCHPORT_ACCESS)) {
		mode_err(vty, "ACCESS");
		return CMD_WARNING;
	}
	if(strstr(self->string, "trunk") && (GET_CURRENT_CONFIG(mode) != ETH_SWITCHPORT_TRUNK)) {
		mode_err(vty, "TRUNK");
		return CMD_WARNING;
	}
	if(strstr(self->string, "hybrid") && (GET_CURRENT_CONFIG(mode) != ETH_SWITCHPORT_ACCESS)) {
		mode_err(vty, "HYBRID");
		return CMD_WARNING;
	}

	SIMPLE_SET_INTERFACE(pvid, strtoul(argv[0], NULL, 0));
	return CMD_SUCCESS;
}
ALIAS(interface_switchport_access,
		interface_switchport_trunk_native_cmd,
		"switchport trunk native vlan <1-4094>",
		"Specify switch port characteristics\n"
		"Set trunk mode characteristics of the interface\n"
		"Set native characteristics\n"
		"Set native Vlan\n"
		"Vlan ID <1-4094>\n")
ALIAS(interface_switchport_access,
		interface_switchport_hybrid_native_cmd,
		"switchport hybrid native vlan <1-4094>",
		"Specify switch port characteristics\n"
		"Set hybrid mode characteristics of the interface\n"
		"Set native characteristics\n"
		"Set native Vlan\n"
		"Vlan ID <1-4094>\n")


DEFUN(interface_shutdown,
		interface_shutdown_cmd,
		"shutdown",
		"Shutdown the selected interface\n")
{
	SIMPLE_SET_INTERFACE(enabled, strstr(self->string, "no") ? 1 : 0);
	return CMD_SUCCESS;
}
ALIAS(interface_shutdown, interface_no_shutdown_cmd, "no shutdown", NO_STR "Shutdown the selected interface\n")

DEFUN(interface_flow,
		interface_flow_cmd,
		"flow-control",
		"Enable flow control of selected interface\n")
{
	SIMPLE_SET_INTERFACE(flowctrl, strstr(self->string, "no") ? 0 : 1);
	return CMD_SUCCESS;
}
ALIAS(interface_flow, interface_no_flow_cmd, "no flow-control", NO_STR "Disable flow control of selected interface\n")

DEFUN(interface_duplex,
		interface_duplex_cmd,
		"half-duplex",
		"Set half duplex mode of selected interface\n")
{
	SIMPLE_SET_INTERFACE(duplex, strstr(self->string, "no") ? 0 : 1);
	return CMD_SUCCESS;
}
ALIAS(interface_duplex, interface_no_duplex_cmd, "no half-duplex", NO_STR "Set full-duplex mode of selected interface\n")

DEFUN(interface_autoneg,
		interface_autoneg_cmd,
		"auto-negotiation",
		"Enable auto-negotiation of selected interface\n")
{
	SIMPLE_SET_INTERFACE(autoneg, strstr(self->string, "no") ? 0 : 1);
	return CMD_SUCCESS;
}
ALIAS(interface_autoneg, interface_no_autoneg_cmd, "no auto-negotiation", NO_STR "Disable auto-negotiation of selected interface\n")

DEFUN(interface_pri,
		interface_pri_cmd,
		"priority <0-7>",
		"Set priority of selected interface\n")
{
	SIMPLE_SET_INTERFACE(priority, strtoul(argv[0], NULL, 0));
	return CMD_SUCCESS;
}

DEFUN(interface_spd,
		interface_spd_cmd,
		"port-speed (10|100|1000)",
		"Set port-speed of selected interface\n"
		"Speed is 10Mbps\n"
		"Speed is 100Mbps\n"
		"Speed is 1000Mbps\n")
{
	SIMPLE_SET_INTERFACE(speed, speed_atoi(argv[0]));
	return CMD_SUCCESS;
}

DEFUN(interface_bw,
		interface_bw_in_cmd,
		"bandwidth ingress <1-1000000>",
		"Set bandwidth control of selected interface\n"
		"Set ingress bandwidth control\n"
		"Bandwidth value in Kbps\n")
{
	if(!strstr(self->string, "ingress"))
		SIMPLE_SET_INTERFACE(ingress_limit, strstr(self->string, "no") ? 0 : strtoul(argv[0], NULL, 0));
	else
		SIMPLE_SET_INTERFACE(egress_limit, strstr(self->string, "no") ? 0 : strtoul(argv[0], NULL, 0));
	return CMD_SUCCESS;
}
ALIAS(interface_bw,
		interface_bw_en_cmd,
		"bandwidth egress <1-1000000>",
		"Set bandwidth control of selected interface\n"
		"Set egress bandwidth control\n"
		"Bandwidth value in Kbps\n")
ALIAS(interface_bw,
		interface_no_bw_in_cmd,
		"no bandwidth ingress",
		NO_STR
		"Disable bandwidth control of selected interface\n"
		"Disable ingress bandwidth control\n")

ALIAS(interface_bw,
		interface_no_bw_en_cmd,
		"no bandwidth egress",
		NO_STR
		"Disable bandwidth control of selected interface\n"
		"Disable egress bandwidth control\n")

DEFUN(show_if_info,
		show_if_eth_cmd,
		"show interface ethernet IFNAME",
		SHOW_STR
		"Display interface information\n"
		"Ethernet interface\n"
		"Interface name")
{
	switch_interface_t ifr;
	int ret;

	ret = get_interface_config(argv[0], self->string, &ifr);
	if(ret < 0) {
		vty_out(vty, "Fail to get interface status%s", VTY_NEWLINE);
		goto out;
	}

out:
	return CMD_SUCCESS;
}

void demo_if_init(void) {
	ipc_clt_interface_config_ack_t *pack = NULL;
	int i;

	/* Install interface node. */
	install_node(&interface_node, demo_interface_config_write);

	// Get interface names
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		DBG_PRINTF("IF COUNT: %d", pack->count);
		for(i = 0; i < pack->count; i ++) {
			ifnames[i] = strdup(pack->cfg[i].ifname);
			DBG_PRINTF("IFNAME[%d]: %s", i, pack->cfg[i].ifname);
		}
		ifcount = pack->count;
	}
	else {
		DBG_ASSERT(0, "IPC fail: %s", pack == NULL ? "NO ACK" : ipc_strerror(pack->hdr.status));
	}
	if(pack) free(pack);

	/* Install commands. */
	//install_default(INTERFACE_NODE);
/*
	install_element(CONFIG_NODE, &interface_eth_cmd);
	install_element(CONFIG_NODE, &interface_cab_cmd);
	install_element(INTERFACE_NODE, &interface_eth_cmd);
	install_element(INTERFACE_NODE, &interface_cab_cmd);
	install_element(INTERFACE_NODE, &show_if_eth_cmd);
	install_element(INTERFACE_NODE, &interface_switchport_cmd);
	install_element(INTERFACE_NODE, &interface_shutdown_cmd);
	install_element(INTERFACE_NODE, &interface_no_shutdown_cmd);
	install_element(INTERFACE_NODE, &interface_flow_cmd);
	install_element(INTERFACE_NODE, &interface_no_flow_cmd);
	install_element(INTERFACE_NODE, &interface_duplex_cmd);
	install_element(INTERFACE_NODE, &interface_no_duplex_cmd);
	install_element(INTERFACE_NODE, &interface_autoneg_cmd);
	install_element(INTERFACE_NODE, &interface_no_autoneg_cmd);
	install_element(INTERFACE_NODE, &interface_pri_cmd);
	install_element(INTERFACE_NODE, &interface_bw_in_cmd);
	install_element(INTERFACE_NODE, &interface_bw_en_cmd);
	install_element(INTERFACE_NODE, &interface_no_bw_in_cmd);
	install_element(INTERFACE_NODE, &interface_no_bw_en_cmd);
	install_element(INTERFACE_NODE, &interface_spd_cmd);
	install_element(INTERFACE_NODE, &interface_switchport_access_cmd);
	install_element(INTERFACE_NODE, &interface_switchport_trunk_native_cmd);
	install_element(INTERFACE_NODE, &interface_switchport_hybrid_native_cmd); */


//	install_element(CONFIG_NODE, &no_interface_cmd);
/*	install_element(INTERFACE_NODE, &interface_desc_cmd);
	install_element(INTERFACE_NODE, &no_interface_desc_cmd);
	install_element(INTERFACE_NODE, &show_address_cmd);*/
}

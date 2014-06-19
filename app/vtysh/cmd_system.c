/*
	System info 
	Author : Einsn
	Date : 2011-05-03
*/

#include "cmd_eoc.h"
#include "eoc_utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


/*
	show system 
	show system security
	show system networking
	
	system-config
	ip dynamic
	ip A.B.C.D A.B.C.D [A.B.C.D]
	dns-server A.B.C.D [A.B.C.D]

	management-security vlan <1-4094>
	management-security https
	management-security ssh
	management-security telnet

	no management-security https
	no management-security ssh
	no management-security telnet

	reboot
	factory-defaults
	save
*/


/*
	show system 
*/

/*begin modified by liaohongjun 2012/9/18 of EPN104QID0019*/
#if 0
DEFUN (show_system_info,
		show_system_info_cmd,
		"show system",
		SHOW_STR
		STR(system))
{
	int ret, i;
	sys_info_t info;
	ret = ipc_get_sys_info(&info);
	VTY_IPC_ASSERT(ret);
	
	vty_out(vty, "      SYSTEM INFORMATION   %s", VTY_NEWLINE);
	vty_out(vty, "----------------------------------------------------%s", VTY_NEWLINE);	
	vty_out(vty, " Model            : %s%s", PROD_MODEL, VTY_NEWLINE);
	vty_out(vty, " MAC Address      : %s%s", mac2str(&info.sysmac), VTY_NEWLINE);
	vty_out(vty, " Hardware Version : %s%s", info.hw_version, VTY_NEWLINE);	
	vty_out(vty, " Software Version : %s%s", info.sw_version, VTY_NEWLINE);
	vty_out(vty, " Built Time       : %s%s", info.buildtime, VTY_NEWLINE);
	vty_out(vty, " Runtime          : %s%s", runtime_str(info.uptime), VTY_NEWLINE);

	for (i = 0; i < MAX_CLT_CHANNEL; i ++){
		vty_out(vty, " Cable%d MAC       : %s%s", i + 1, mac2str(&info.cabmac[i]), VTY_NEWLINE);
	}
	vty_out(vty, "----------------------------------------------------%s", VTY_NEWLINE);		
	return CMD_SUCCESS;
}



DEFUN (show_system_security,
		show_system_security_cmd,
		"show security",
		SHOW_STR
		STR(security))
{
	int ret;
	sys_security_t info;
	ret = ipc_get_sys_security(&info);
	VTY_IPC_ASSERT(ret);
	/*Begin modified by huangmingjian 2012/08/20 for EPN104QID0007*/ 
	vty_out(vty, "      system security   %s", VTY_NEWLINE);
	vty_out(vty, "Management VLAN : %u%s", info.mvlan, VTY_NEWLINE);
	vty_out(vty, "HTTPS           : %s%s", ENABLE_TO_STR(info.https_en), VTY_NEWLINE);
	vty_out(vty, "SSH             : %s%s", ENABLE_TO_STR(info.ssh_en), VTY_NEWLINE);
	vty_out(vty, "TELNET          : %s%s", ENABLE_TO_STR(info.telnet_en), VTY_NEWLINE);
	/*End modified by huangmingjian 2012/08/20 for EPN104QID0007 */
	return CMD_SUCCESS;
}


DEFUN (show_system_networking,
		show_system_networking_cmd,
		"show networking",
		SHOW_STR
		"Displays system networking info\n")
{
	int ret, i;
	sys_network_t info;
	char *dhcp_status;

# define DHCP_STATUS_STR_STOP 	"Stoped"
# define DHCP_STATUS_STR_DOING 	"Fetching"
# define DHCP_STATUS_STR_OK 	"Success"
# define DHCP_STATUS_STR_FAILED "Failed"

	
	ret = ipc_get_sys_networking(&info);
	VTY_IPC_ASSERT(ret);
	/*Begin modified by huangmingjian 2012/08/20 for EPN104QID0007*/  
	vty_out(vty, "      system info   %s", VTY_NEWLINE);
	vty_out(vty, "Mac Address     : %s%s", mac2str(&info.mac), VTY_NEWLINE);
	vty_out(vty, "Hostname    	  : %s%s", info.hostname, VTY_NEWLINE);
	vty_out(vty, "IP PROTO        : %s%s", (info.ip_proto == IP_PROTO_STATIC) ? "Static" : "Dynamic", VTY_NEWLINE);
	vty_out(vty, "IP Address      : %s%s", inet_ntoa(info.netif.ip), VTY_NEWLINE);
	vty_out(vty, "Subnet Mask     : %s%s", inet_ntoa(info.netif.subnet), VTY_NEWLINE);
	vty_out(vty, "Gateway         : %s%s", inet_ntoa(info.netif.gateway), VTY_NEWLINE);
	vty_out(vty, "DNS Server      : ");
    /*End modified by huangmingjian 2012/08/20 for EPN104QID0007 */
	for (i = 0; i < MAX_DNS_NUM; i ++){
		if (info.netif.dns[i].s_addr > 0){
			vty_out(vty, "%s ", inet_ntoa(info.netif.dns[i]));
		}
	}
	vty_out(vty, "%s", VTY_NEWLINE);
	if (info.ip_proto == IP_PROTO_DYNAMIC){
		if (info.ip_proto == IP_PROTO_STATIC){
			dhcp_status = DHCP_STATUS_STR_STOP;
		}else if (info.netif.ip.s_addr == 0){
			dhcp_status = DHCP_STATUS_STR_DOING;
		}else {
			dhcp_status = DHCP_STATUS_STR_OK;
		}
	vty_out(vty, " DHCP Status    : %s%s", dhcp_status, VTY_NEWLINE);
	vty_out(vty, " Lease Time     : %u%s", info.lease_time, VTY_NEWLINE);			
	}
	return CMD_SUCCESS;
}
#endif
/*end modified by liaohongjun 2012/9/18 of EPN104QID0019*/


DEFUN (system_config,
		system_config_cmd,
		"system-config",
		"System and management configuration\n")
{
	vty->node = SYSCONFIG_NODE;
	return CMD_SUCCESS;
}


/*
management-security vlan <1-4094>
management-security https
management-security ssh
management-security telnet

no management-security https
no management-security ssh
no management-security telnet
*/


DEFUN (management_security,
		management_security_vlan_cmd,
		"mgmt-security vlan <1-4094>",
		STR(mgmt_security)
		STR(vlan)
		STR(vlan_id)
		)
{
	int enable = 1, flag = 0, mvid = 0, ret = 0, change = 0;
	sys_security_t info;
	int mask = 0;
	
# define  SEC_VLAN 0x01
# define  SEC_SSH 0x02
# define  SEC_HTTPS 0x04
# define  SEC_TELNET 0x08

	if (strstr(self->string, "no")){
		enable = 0;
	}

	if (strstr(self->string, "vlan")){
		flag |= SEC_VLAN;
		mvid = strtoul(argv[0], NULL, 0);
		if (!vlan_exist(mvid, &ret)){
			VTY_IPC_ASSERT(ret);
			vty_out(vty, "VLAN %d does not exists, please create it first.%s", mvid, VTY_NEWLINE);			
			return CMD_SUCCESS;
		}
	} else 	if (strstr(self->string, "https")){
		flag |= SEC_HTTPS;
	} else if (strstr(self->string, "ssh")){
		flag |= SEC_SSH;
	} else if (strstr(self->string, "telnet")){
		flag |= SEC_TELNET;
	}

/*
	ret = ipc_get_sys_security(&info)

	VTY_IPC_ASSERT(ret);
*/

	if ((flag & SEC_VLAN)/* && (mvid != info.mvlan)*/){
		info.mvlan = mvid;
		change = 1;
		mask = SYS_MVLAN;
	}else if ((flag & SEC_HTTPS)/* && (enable != info.https_en)*/){
		info.https_en = enable;
		change = 1;
		mask = SYS_HTTPS;
	}else if ((flag & SEC_SSH) /*&& (enable != info.ssh_en )*/){
		info.ssh_en = enable;
		change = 1;
		mask = SYS_SSH;
	}else if ((flag & SEC_TELNET)/* && (enable != info.telnet_en )*/){
		info.telnet_en = enable;
		change = 1;
		mask = SYS_TELNET;
	}

	if (change){
		ret = ipc_set_sys_security(&info, mask);
		VTY_IPC_ASSERT(ret);
	}
	
	return CMD_SUCCESS;
}


ALIAS (management_security,
		management_security_https_cmd,
		"mgmt-security https",
		STR(mgmt_security)
		STR(https)
		);


ALIAS (management_security,
		no_management_security_https_cmd,
		"no mgmt-security https",
		NO_STR
		STR(mgmt_security)
		STR(https)
		);

ALIAS (management_security,
		management_security_ssh_cmd,
		"mgmt-security ssh",
		STR(mgmt_security)
		STR(ssh)
		);


ALIAS (management_security,
		no_management_security_ssh_cmd,
		"no mgmt-security ssh",
		NO_STR
		STR(mgmt_security)
		STR(ssh)
		);

ALIAS (management_security,
		management_security_telnet_cmd,
		"mgmt-security telnet",
		STR(mgmt_security)
		STR(telnet)
		);


ALIAS (management_security,
		no_management_security_telnet_cmd,
		"no mgmt-security telnet",
		NO_STR
		STR(mgmt_security)
		STR(telnet)
		);




DEFUN (reboot,
		reboot_cmd,
		"reboot",
		"Reboot system\n")
{
	int ret;
	pid_t i;
	char *cmd[] = {"reboot", NULL};
		
	/*Begin modified by sunmingliang for bug 199*/
	int reboot_fd = -1;
	if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
	{
		/*should not go here*/
		perror("read lock failed");
		vty_out(vty, "Reboot system failed %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
	{
		/*system is going to reboot, so no need to release lock*/
		vty_out(vty, "Reboot system failed, system is performing upgrade %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ret = ipc_system_request(IPC_SYS_REBOOT);
	if (ret != IPC_STATUS_OK){	
		_eval(cmd, NULL, 0, &i);
	}
	vty_out(vty, "Reboot system...%s", VTY_NEWLINE);
	return CMD_SUCCESS;
}

DEFUN (factory_defaults,
		factory_defaults_cmd,
		"factory-defaults",
		"Restore all configuration to factory default values\n")
{
	int ret;
    /*Begin modified by sunmingliang for bug 199*/
	int reboot_fd = -1;
	if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
	{
		/*should not go here*/
		perror("read lock failed");
		vty_out(vty, "Reboot system failed %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
	{
		/*system is going to reboot, so no need to release lock*/
		vty_out(vty, "restoring failed, system is performing upgrade %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*End modified by sunmingliang for bug 199*/
	vty_out(vty, "Now restoring system configuration to factory default values...%s", VTY_NEWLINE);
	ret = ipc_system_request(IPC_SYS_RESTORE_AND_REBOOT);
	VTY_IPC_ASSERT(ret);
    
    if(IPC_STATUS_OK == ret)
    {
    	vty_out(vty, "Default configuration will take affect after reboot %s", VTY_NEWLINE);
        vty_out(vty, "System rebooting...%s", VTY_NEWLINE);
    }
	return CMD_SUCCESS;
}

DEFUN (save,
		save_cmd,
		"save",
		"Store all running config to non-variable flash\n")
{
	int ret;
	vty_out(vty, "Please wait while saving configuration to flash memory...%s", VTY_NEWLINE);	
	ret = ipc_system_request(IPC_SYS_NVRAM_COMMIT);
	VTY_IPC_ASSERT(ret);

    if(IPC_STATUS_OK == ret)
    {
        vty_out(vty, "System running config saved to flash memory successfully%s", VTY_NEWLINE);
   
}
	return CMD_SUCCESS;
}



static struct cmd_node sysconfig_node = {SYSCONFIG_NODE, "%s(system-config)# ", NULL, 1, };///*Modified by huangmingjian 2012/09/21 for EPN104QID0046*/ 


static int cmd_system_config_write(struct vty *vty) 
{ 
	return vty_out(vty, "!%s", VTY_NEWLINE);

}


void cmd_systemconfig_init(void)
{

	install_node(&sysconfig_node, cmd_system_config_write);
	install_default(SYSCONFIG_NODE);
    install_element(SYSCONFIG_NODE, &factory_defaults_cmd); 
	install_element(ENABLE_NODE, &system_config_cmd); 
	/*Begin modified by huangmingjian 2012/08/21 for EPN104QID0046*/
    install_element(SYSCONFIG_NODE, &hostname_cmd);
    install_element(SYSCONFIG_NODE, &no_hostname_cmd);
	install_element(SYSCONFIG_NODE, &no_super_cmd);	
	install_element(SYSCONFIG_NODE, &super_password_cmd);
    /*end modified by huangmingjian 2012/08/21for EPN104QID0046*/  

    #if (CMD_CURRENT_SUPPORT)   
	install_element(VIEW_NODE, &show_system_info_cmd);
	install_element(ENABLE_NODE, &show_system_info_cmd);
	install_element(ENABLE_NODE, &show_system_security_cmd);
	install_element(ENABLE_NODE, &show_system_networking_cmd);
	install_element(CONFIG_NODE, &show_system_info_cmd);
	install_element(CONFIG_NODE, &show_system_security_cmd);
	install_element(CONFIG_NODE, &show_system_networking_cmd);
	install_element(SYSCONFIG_NODE, &show_system_info_cmd);
	install_element(SYSCONFIG_NODE, &show_system_security_cmd);
	install_element(SYSCONFIG_NODE, &show_system_networking_cmd);  
	install_element(SYSCONFIG_NODE, &management_security_vlan_cmd);
	install_element(SYSCONFIG_NODE, &management_security_https_cmd);
	install_element(SYSCONFIG_NODE, &no_management_security_https_cmd);
	install_element(SYSCONFIG_NODE, &management_security_ssh_cmd);
	install_element(SYSCONFIG_NODE, &no_management_security_ssh_cmd);
	install_element(SYSCONFIG_NODE, &management_security_telnet_cmd);
	install_element(SYSCONFIG_NODE, &no_management_security_telnet_cmd);
	#endif
}




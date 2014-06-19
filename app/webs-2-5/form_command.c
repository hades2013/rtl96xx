/*
 * goform.c
 *
 *  Created on: Jan 11, 2011
 *      Author: root
 */
#include	"utils.h"
#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include 	"webs_nvram.h"
#include	<sysmon.h>
#include	<debug.h>
#include	<ipc_client.h>
#include 	<hex_utils.h>
#include 	<str_utils.h>
    //#include 	<vlan_utils.h>
    //#include	<switch.h>
#include 	<config.h>
#include 	<valids.h>
#include    "lw_drv_pub.h"
#include	<lw_if_pub.h>
#include	<lw_config_oid.h>
#include    <systemlog.h>		//add by leijinbao 2013/9/17 for USER_LOG
#include	<lw_drv_req.h>

/*Begin modified for bug 199 by sunmingliang*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
/*End modified for bug 199 by sunmingliang*/
#include	"asp_variable.h"
/*add by leijinbao 2013/12/19 for mac*/
#include 	"cipher.h"
/*end by leijinbao 2013/12/19 for mac*/

#define	MALLOC_CHK(var, type, count) do { \
	var = (type *)malloc(sizeof(type) * count); \
	DBG_ASSERT(var, "Memory allocate fail"); \
	if(!var) return 0; \
	} while(0)

extern int ipc_fd;
extern int Mac_firstidx;
extern int Mac_port;
extern int Mac_port_firstidx;
extern int Mac_port_num;
extern int port_msk;
extern uint8_t is_timepage_post;
	

struct cgi_handle_t {
	char *cmd;
	int (*request_handle)(webs_t wp, cgi_result_t *result);
};

int countnum = 0;
int getValueByKey(webs_t req,char *keyword, char *getVal, unsigned int  len)
{	
//	int ret= 0; 
//	int valid = 0;		
	char *cp = NULL;	
	char svar[1024] = {0};	  
	char *sval = NULL;	
   // request * req; 
	if(NULL == keyword||req==NULL)
	{
		return FALSE;
	}
	memset(getVal,0,len);
	while(1)	
	{		
		 sprintf(svar, "SET%d", countnum); 				 
		 sval = websGetVar(req, svar, NULL);
		 if(!sval)		 
		 {		   
			  break;
		 }		 
	 //    BOA_PRINTF("SET%d = %s", count, sval);
		 cp = strchr(sval, '=');	 
		 if (!cp)		 
		 {			 
			 break; 
		 }		 
		 *cp = 0;
		 cp++;
		 countnum++;	
		 if(0 == strcmp(keyword, sval))
		 {						  
			 safe_strncpy(getVal, cp, len);
			 return TRUE;		 
		 }		   
	}	

	return FALSE;
}


#if 0 //add by zhouguanhua
static int port_spd(char *spd)
{
	if(!strcmp(spd, "10")) return CNU_ETHER_SPD_10;
	if(!strcmp(spd, "100")) return CNU_ETHER_SPD_100;
	if(!strcmp(spd, "1000")) return CNU_ETHER_SPD_1000;
	return CNU_ETHER_SPD_100;
}

static int port_dup(char *dup)
{
	if(!strcmp(dup, "half")) return CNU_ETHER_DUP_HALF;
	return CNU_ETHER_DUP_FULL;
}
#endif  //add by zhouguanhua

/*
author:huangmingjian
date:2014-04-08
U-Boot 2011.12.4234-svn4230
The perfect mac encrypt is th svn version r4230, so when the onu have the boot that svn version is < 4230, but the 
app >=  4230,  if we set the mac which include encrypt product name, then the system will away down.So we need 
to check it. 
*/
int svn_version_check(void)
{
	char ubootv[64] = {0};  
	unsigned int svn_num = 0;
	char *pstr = NULL; 
	
	if(GetBootVersion(ubootv, sizeof(ubootv)) < 0)
	{	
		return -1;	
	}
	pstr = strstr(ubootv, "svn");
	pstr +=3;
	svn_num = strtol(pstr, NULL, 10);
	if(svn_num < SNV_VERSION_MAC_PRODUCT_ENCRYPT)
	{
		return -1; 
	}
	
	return 0;
}

int sys_setdebug_handle(webs_t wp, cgi_result_t *result)
{
    
	int count;
	char *cp, svar[10], *sval;
	count = 0;
	/*modify by leijinbao 2014/05/06 for mac&product Encryption bug 565*/
	char macbuf[64];
	char *cp1 = NULL;
	/*end by leijinbao 2014/05/06 for mac&product Encryption bug 565*/
	while(1) 
	{
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);	
		count ++;		
		if(!sval) break;	
				
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!cp[0])continue;
		
		if (!strcmp(sval, "sys_mac"))
		{
			/*Begin add by huangmingjian 2014-05-09 for Bug 575*/
			if(svn_version_check()<0)
			{
				SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);  
				return 0;  
			}
			/*End add by huangmingjian 2014-05-09 for Bug 575*/
                  
			if(1 == IsStringN63Cipher(cp))
            {	
				/*modify by leijinbao 2014/05/06 for mac&product Encryption bug 565*/
				memset(macbuf,0,sizeof(macbuf));
				DecryptN63Password(cp,macbuf);
				cp1 = strchr(macbuf, '/');
				if(cp1)
				{
					cp1++;
					if(strcmp(CONFIG_PRODUCT_NAME,cp1))
					{
						SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);  
						return 0;
					}
					
					/*Begin modified by huangmingjian 2014/04/04 for Bug 500*/
	            	Ioctl_SetMacWriteFlag(1);
		           	write_date_file(UFILE_ETHADDR,cp,strlen(cp)+1);
					Ioctl_SetMacWriteFlag(0);
					/*End modified by huangmingjian 2014/04/04 for Bug 500*/
				}
				else
				{
					SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);  
					return 0;
				}
				/*end by leijinbao 2014/05/06 for mac&product Encryption bug 565*/
				break;
			}
			else
			{
				SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);  
				return 0;
			 }
	    }
        else if (0 == strcmp(sval,"SN"))
        {            
		     if(SN_MAX_SIZE != strlen(cp))
            {
				 SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);	
				 return 0; 
            }
			write_date_file(UFILE_SN,cp,strlen(cp)+1);
            break;
        }
        else 
        {		  
		    SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);  
			return 0; 
        }
	}
	SET_RESULT(result, MSG_OK, CGI_OK, 0, 0);
    return 0; 
}
/*End by zhouguanhua 2013/5/15 for bug id 52*/


int sys_config_handle(webs_t wp, cgi_result_t *result)
{
	int count, access = 0;
	char *cp, svar[10], *sval;
	/*Begin modified for bug 199 by sunmingliang*/
	ipc_system_ack_t *pack = NULL;	
	int reboot_fd = -1;
	
	count = 0;
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);	
		count ++;		
		if(!sval) break;	
		//DBG_PRINTF("SET%d = %s\n", count, sval);			
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!cp[0])continue;
		if (!strcmp(sval, "config")){
			if (!strcmp(cp, "save")){
				access = IPC_SYS_NVRAM_COMMIT;
				break; // currently only support one at once
			}else if (!strcmp(cp, "restore")){
				access = IPC_SYS_RESTORE_AND_REBOOT;			
				if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
				{
					/*should not go here*/
					printf("File open error\n");
					SET_RESULT(result, MSG_FAIL, CGI_IPC_FAIL, 0, 0);
					return 0;
				}
				
				if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
				{
					/*system is going to reboot, so no need to release lock*/
					SET_RESULT(result, MSG_FAIL, CGI_IPC_FAIL, 0, 0);
					return 0;
				}
				break;
			}/*else if (!strcmp(cp, "export")){
				access = IPC_SYS_CONFIG_BACKUP;
				break;
			}else if (!strcmp(cp, "import")){
				//1TODO:
				break;
			}*/else if (!strcmp(cp, "reboot")){
				access = IPC_SYS_REBOOT;
				if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
				{
					/*should not go here*/
					printf("File open error\n");
					SET_RESULT(result, MSG_FAIL, CGI_IPC_FAIL, 0, 0);
					return 0;
				}
				printf("Open file %s \n", REBOOT_LOCK);
				if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
				{
					/*system is going to reboot, so no need to release lock*/
					SET_RESULT(result, MSG_FAIL, CGI_IPC_FAIL, 0, 0);
					return 0;
				}
				break;
			}else if (!strcmp(cp, "syslog")){
				access = IPC_SYS_LOG_CLEAR;
				break;
			}
		}
	}	

	if (access == 0){
		DBG_ASSERT(access, "Invalid Form Command");
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
		return 0;	
	}

	pack = ipc_system_req(ipc_fd, access, 0);
	if (pack) {
		if (pack->hdr.status != IPC_STATUS_OK){
			SET_RESULT(result, MSG_FAIL, CGI_IPC_FAIL, 0, 0);
		}else {
			switch(access)
			{
			case IPC_SYS_NVRAM_COMMIT:
				SET_RESULT(result, MSG_OK, CGI_OK, 0, 0);					
				break;
			case IPC_SYS_REBOOT:
				SET_RESULT(result, MSG_REBOOT, CGI_OK, 0, 25);					
				break;
			case IPC_SYS_RESTORE_AND_REBOOT:
				SET_RESULT(result, MSG_RESTORE, CGI_OK, 0, 30);					
				break;
			case IPC_SYS_LOG_CLEAR:
				SET_RESULT(result, MSG_OK, CGI_OK, 1, 0);					
			break;				
			//case IPC_SYS_CONFIG_BACKUP:
			//	SET_RESULT(result, MSG_OK, CGI_OK, 1, 0);					
			//	break;
			default:
				SET_RESULT(result, MSG_UNKNOWN, CGI_OK, 0, 0);	
				break;
			}		
		}
		free(pack);
	}else {
		SET_RESULT(result, MSG_FAIL, CGI_IPC_TIMEOUT, 0, 0);
	}
	return 0;
}


#if 0 //add by zhouguanhua
/*
CMD value=VLAN_MODE>
SET2 value="vlan_mode=0">
SET3 value="port_type=cab0;access">
SET3 value="port_type=cab1;trunk">
*/



static int vlan_mode_prop_handle(webs_t wp, cgi_result_t *result)
{
	int count, ifnum, items;
	char *cp, svar[10], *sval, *vars[10];
	char tmp[128];
//	char *cmdvar;
	switch_vlan_mode_t vlan_mode;

	ipc_clt_vlan_mode_ack_t *pack = NULL;

	// Step 1, Get CMD TYPE

	// Step 2, Get valid command strings
	// Step 3, parse vlans in command strings
	
	memset(&vlan_mode, 0, sizeof(vlan_mode));
	count = 0;
	ifnum = 0;
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		//DBG_PRINTF("SET%d = %s\n", count, sval);			
		safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(tmp, '=');
		if (!cp) break;
		*cp = 0; cp ++;

		if (!strcmp(tmp, "vlan_mode")){
			vlan_mode.mode = strtoul(cp, NULL, 0);
		}else if ((ifnum < EOC_MAX_PORT_NUMS) && !strcmp(tmp, "port_type")){
			items = split_string(cp, ';', vars);
			if (items > 1){
				safe_strncpy(vlan_mode.ports[ifnum].ifname, vars[0], sizeof(vlan_mode.ports[ifnum].ifname));
				vlan_mode.ports[ifnum].type = switchport_atoi(vars[1]);
				ifnum ++;
			}
		}
		count ++;
	}	

	if ((vlan_mode.mode == VLAN_MODE_TRANSPARENT) && (ifnum == 0)){
		DBG_ASSERT(ifnum, "Invalid Form Command");
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
		return 0;	
	}

	// Step 4, set ipc 
	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &vlan_mode);

	IPC_RESULT(pack, result);
	
	// free all memory
	if (pack) free(pack);

	return 0;
}




/*
CMD value=VLAN_DEL>
SET2 value="vlan_del=0;cab0,cab1,eth0,eth1;1-5,32,54,74,100-105">
CMD value=VLAN_NEW>
SET2 value="vlan_add=0;cab0,cab1,eth0,eth1;1-5,32,54,74,100-105">
*/



static int vlan_prop_handle(webs_t wp, cgi_result_t *result)
{
	int act = IPC_CONFIG_NEW,  i;	
	int count;
	char *cp, svar[10], *sval;
	char tmp[4096 + 16];
	char *cmdvar;
	switch_vlan_group_t *groups[20], *vg;
	int vgcnt, vgsize;

	ipc_clt_vlan_config_ack_t *pack = NULL;
	switch_vlan_group_t *vlan_groups = NULL;

	// Step 1, Get CMD TYPE
	sval = websGetVar(wp, "CMD", NULL);
	if (sval != NULL){
		if (!strcmp(sval, "VLAN_DEL")){
			act = IPC_CONFIG_DEL;
		}else if (!strcmp(sval, "VLAN_NEW")){
			act = IPC_CONFIG_NEW;
		}
	}	

	// Step 2, Get valid command strings
	count = 0;
	vgcnt = 0;
	vgsize = 0;
	while(1) {
		sprintf(svar, "SET%d", count);
					
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		//DBG_PRINTF("SET%d = %s\n", count, sval);			
		safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(tmp, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (act == IPC_CONFIG_DEL){
			cmdvar = "vlan_del";
		}else if (act == IPC_CONFIG_NEW){
			cmdvar = "vlan_add";
		}
		if ((vgcnt < sizeof(groups)/sizeof(groups[0])) && !strcmp(tmp, cmdvar)){
			groups[vgcnt] = vlan_group_new_from_string(cp);
			if (groups[vgcnt]){
				vgsize += sizeof_vlan_group(groups[vgcnt]->count);
				vgcnt ++;
			}
		}			
		count ++;
	}	

	if (vgcnt == 0){
		DBG_PRINTF("Invalid Form Command: Could not find valid vlan string");
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
		return 0;	
	}

	// Step 3, parse vlans in command strings

	vlan_groups = (switch_vlan_group_t *)malloc(vgsize);
	if (!vlan_groups){
		DBG_ASSERT(vlan_groups, "Memory allocate fail"); 
		// free all groups
		for (i = 0; i < vgcnt; i ++){
			if (groups[i]) free(groups[i]);
		}		
		return 0;
	}
	vg = vlan_groups;
	for (i = 0; i < vgcnt; i ++){
		memcpy(vg, groups[i], sizeof_vlan_group(groups[i]->count));
		vg = vlan_group_to_next(vg);
	}

	// Step 4, set ipc 
	pack = ipc_clt_vlan_config(ipc_fd, act, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, vlan_groups, vgcnt);

	IPC_RESULT(pack, result);

	// free all memory
	for (i = 0; i < vgcnt; i ++){
		if (groups[i]) free(groups[i]);
	}	
	if (vlan_groups) free(vlan_groups);
	if (pack) free(pack);

	return 0;
}

/*
SET2 value="vlanif=cab0;access;1;;">
SET3 value="vlanif=eth1;access;1;;">
SET2 value="vlanif=eth0;trunk;1;1000,1002-1009,1011,1013,1014,1015;">
*/
static 	switch_vlan_interface_t *vlan_interface_parse(const char *vistr)
{
	int items, pvid, mode;
	switch_vlan_group_t *vlan0, *vlan1; // for trunk use vlan0 only , for hybrid vlan0 is tag and vlan1 is untag 
	switch_vlan_interface_t * vlanif = NULL;
	char *vp = strdup(vistr);
	char *vars[10]; 
	if (vp == NULL) return NULL;
	items = split_string(vp, ';', vars);
	if (items >= 5){
			mode = switchport_atoi(vars[1]);
			pvid = strtoul(vars[2], NULL, 0);
			vlan0 = NULL;
			vlan1 = NULL;
			if (mode == ETH_SWITCHPORT_TRUNK){
				vlan0 = vlan_group_new_from_string_simple(vars[3]);
				if (vlan0) vlan0->flag = VLAN_FLAG_TAG;
			}else if(mode == ETH_SWITCHPORT_HYBRID){
				// get tag vlans 
				vlan0 = vlan_group_new_from_string_simple(vars[3]);
				if (vlan0) vlan0->flag = VLAN_FLAG_TAG;
				// get untag vlans
				vlan1 = vlan_group_new_from_string_simple(vars[4]);
			}
			vlanif = vlan_interface_new(vars[0], mode, vlan0, vlan1);
			if (vlanif) vlanif->pvid = pvid;
			if (vlan0) free(vlan0);
			if (vlan1) free(vlan1);
	}
	free(vp);
	return vlanif;
}


static int vlanif_prop_handle(webs_t wp, cgi_result_t *result)
{
	int i;	
	int count;
	char *cp, svar[10], *sval;
	char tmp[4096 + 16];
	switch_vlan_interface_t *vlanifs[4], *vi;
	int vicnt, visize;

	ipc_clt_vlan_interface_config_ack_t *pack = NULL;
	switch_vlan_interface_t *vlan_ifs = NULL;

	// Step 1, Get CMD TYPE
	// Only 	has IPC_CONFIG_SET in web pages

	// Step 2, Get valid command strings
	count = 0;
	vicnt = 0;
	visize = 0;
	while(1) {
		sprintf(svar, "SET%d", count);
					
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		//DBG_PRINTF("SET%d = %s\n", count, sval);			
		safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(tmp, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if ((vicnt < sizeof(vlanifs)/sizeof(vlanifs[0])) && !strcmp(tmp, "vlanif")){
			vlanifs[vicnt] = vlan_interface_parse(cp);
			if (vlanifs[vicnt]){
				visize += sizeof_vlan_interface(vlanifs[vicnt]->trunk_hybrid, vlanifs[vicnt]->count);
				vicnt ++;
			}
		}			
		count ++;
	}	

	if (vicnt == 0){
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;	
	}

	// Step 3, parse vlans in command strings

	vlan_ifs = (switch_vlan_interface_t *)malloc(visize);
	if (!vlan_ifs){
		DBG_ASSERT(vlan_ifs, "Memory allocate fail"); 
		// free all groups
		for (i = 0; i < vicnt; i ++){
			if (vlanifs[i]) free(vlanifs[i]);
		}		
		return 0;
	}
	vi = vlan_ifs;
	for (i = 0; i < vicnt; i ++){
		memcpy(vi, vlanifs[i], sizeof_vlan_interface(vlanifs[i]->trunk_hybrid, vlanifs[i]->count));
		vi = vlan_interface_to_next(vi);
	}

	// Step 4, set ipc 
	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, vlan_ifs, vicnt);

	IPC_RESULT(pack, result);
	
	// free all memory
	for (i = 0; i < vicnt; i ++){
		if (vlanifs[i]) free(vlanifs[i]);
	}		
	if (vlan_ifs) free(vlan_ifs);
	if (pack) free(pack);

	return 0;
}


int tmpl_get_config(char *s_tmpl, char **s_tmpl_port, int port_num, char *s_tmpl_misc, ipc_service_template_t *tmpl) // ram_id in order :by einsn
{
	char  *vars[20];
	int i, items;

	// part 1 gen
	items = split_string(s_tmpl, ';', vars);
	if(items < 3) return -1;

	tmpl->tmpl_id = strtoul(vars[0], NULL, 0);
	tmpl->device_id = strtoul(vars[1], NULL, 0);
	safe_strncpy(tmpl->tmpl_name, vars[2], sizeof(tmpl->tmpl_name));
	if(items > 3) safe_strncpy(tmpl->tmpl_desc, vars[3], sizeof(tmpl->tmpl_desc));

	if (port_num < 1) return -1;
	
	tmpl->lsw_cfg.ports = port_num; // should be updated in master later


	// part 2 cab port
	items = split_string(s_tmpl_port[0], ';', vars);
	if(items < 10) return -1;	
	tmpl->lsw_cfg.cab_port.speed = port_spd(vars[1]);
	tmpl->lsw_cfg.cab_port.duplex = !strcmp(vars[2], "full");
	tmpl->lsw_cfg.cab_port.priority = strtoul(vars[3], NULL, 0);
	tmpl->lsw_cfg.cab_port.flowctrl = !strcmp(vars[4], "on");
	tmpl->lsw_cfg.cab_port.ingress_rate_limit = strtoul(vars[5], NULL, 0);
	tmpl->lsw_cfg.cab_port.egress_rate_limit = strtoul(vars[6], NULL, 0);
	tmpl->lsw_cfg.cab_port.tagged = !strcmp(vars[7], "on");
	tmpl->lsw_cfg.cab_port.pvid = strtoul(vars[8], NULL, 0);
	tmpl->lsw_cfg.cab_port.disabled = !strcmp(vars[9], "dis");

	// prt 3 eth ports


	// Template configuration for each ETHERNET port!!!
	for (i = 0; i < port_num - 1; i++) {
		items = split_string(s_tmpl_port[i + 1], ';', vars);
		if (items < 10) return -1;
		tmpl->lsw_cfg.eth_ports[i].autoneg = !strcmp(vars[1], "auto");
		tmpl->lsw_cfg.eth_ports[i].speed = port_spd(vars[1]);
		tmpl->lsw_cfg.eth_ports[i].duplex = port_dup(vars[2]);
		tmpl->lsw_cfg.eth_ports[i].priority = strtoul(vars[3], NULL, 0);
		tmpl->lsw_cfg.eth_ports[i].flowctrl	= !strcmp(vars[4], "on");
		tmpl->lsw_cfg.eth_ports[i].ingress_rate_limit = strtoul(vars[5], NULL, 0);
		tmpl->lsw_cfg.eth_ports[i].egress_rate_limit = strtoul(vars[6], NULL, 0);
		tmpl->lsw_cfg.eth_ports[i].tagged = !strcmp(vars[7], "on");
		tmpl->lsw_cfg.eth_ports[i].pvid = strtoul(vars[8], NULL, 0);
		tmpl->lsw_cfg.eth_ports[i].disabled = !strcmp(vars[9],	"dis");
	}

	// part 4 misc
	items = split_string(s_tmpl_misc, ';', vars);
	if (items < 4) return -1; 
	tmpl->cable_rate_up = strtoul(vars[0], NULL, 0);
	tmpl->cable_rate_dn = strtoul(vars[1], NULL, 0);
	tmpl->lsw_cfg.broadcast_storm = !strcmp(vars[2], "on");
	tmpl->mac_limit = strtoul(vars[3], NULL, 0);
	return 1;
}


int tmpl_prop_handle(webs_t wp, cgi_result_t *result)
{
	int act = IPC_CONFIG_SET, i;	
	int count = 0, tmpl_id = -1;
	char *cp, svar[10], *sval;
	ipc_service_template_ack_t *pack = NULL;
	ipc_service_template_t tmpl, *ptmpl = NULL;
	char tmp[256 + 16];
	int del[EOC_MAX_TMPL_NUMS];
	int del_cnt = 0;
	char *tmpl_s, *tmpl_p[CNU_MAX_ETH_PORTS + 1], *tmpl_m;
	char ctmpl_s[16], ctmpl_p[CNU_MAX_ETH_PORTS + 1][16], ctmpl_m[16];
	int port_num;

	sval = websGetVar(wp, "CMD", NULL);
	if (sval != NULL){
		if (!strcmp(sval, "TMPL_DEL")){
			act = IPC_CONFIG_DEL;
		}else if (!strcmp(sval, "TMPL_NEW")){
			act = IPC_CONFIG_NEW;
		}
	}	
	count = 0;

	// for new and set
	// find the tmpl id
	// SET7=tmpl_id=5
	memset(del, 0, sizeof(del));

	while(1) {
		sprintf(svar, "SET%d", count);
					
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
		safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(tmp, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (act == IPC_CONFIG_DEL){
			if ((del_cnt < EOC_MAX_TMPL_NUMS) && !strcmp(tmp, "tmpl_del")){
				del[del_cnt ++] = strtoul(cp, NULL, 0);
			}			
		}else {
			if (!strcmp(tmp, "tmpl_id")){
				tmpl_id = strtoul(cp, NULL, 0);
				// only accept one template
				break;
			}
		}
		count ++;
	}	

	if (act == IPC_CONFIG_SET || act == IPC_CONFIG_NEW) {
		if (tmpl_id < 0){ // invalid tmpl string
			DBG_PRINTF("Invalid Tmpl Form Command.");
			SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
			return 0;
		}
		//return 0;
		// set all empty
		sprintf(ctmpl_s, "tmpl_%d", tmpl_id); 
		tmpl_s = NULL;
		for (i = 0; i < CNU_MAX_ETH_PORTS + 1; i ++){
			sprintf(ctmpl_p[i], "tmpl_%d_%d", tmpl_id, i);			
			tmpl_p[i] = NULL;
		}
		sprintf(ctmpl_m, "tmpl_%d_misc", tmpl_id);
		tmpl_m = NULL;
		// find the variables
		count = 0;
		port_num = 0;
		while(1){
			sprintf(svar, "SET%d", count);
			count ++;
			sval = websGetVar(wp, svar, NULL);
			if(!sval) break;
			cp = strchr(sval, '=');
			if (!cp) break;
			*cp = 0; cp ++;
			if (!cp[0]) continue;
			if (!tmpl_s && !strcmp(sval, ctmpl_s)){
				tmpl_s = cp;
			}
			if (!tmpl_m && !strcmp(sval, ctmpl_m)){
				tmpl_m = cp;
			}
			for (i = 0; i < CNU_MAX_ETH_PORTS + 1; i ++){
				if (!tmpl_p[i] && !strcmp(sval, ctmpl_p[i])){
					tmpl_p[i] = cp;
					port_num ++;
				}				
			}
			/*
			if (port_num < CNU_MAX_ETH_PORTS + 1){
				if (!strcmp(sval, ctmpl_p[port_num])){
					tmpl_p[port_num] = cp;
					port_num ++;
				}
			}*/
		}
		// check if all ready
		if (!tmpl_s || !tmpl_m || (port_num < 1)){
			DBG_PRINTF("Invalid Form Command: lost variables");
			SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
			return 0;
		}
		for (i = 0; i < port_num; i ++){
			if (!tmpl_p[i]){
				DBG_PRINTF("Invalid Form Command: lost variables");
				SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);				
				return 0;
			}
		}	
		
		// now all need variables ready
		memset(&tmpl, 0, sizeof(tmpl));
		if (tmpl_get_config(tmpl_s, tmpl_p, port_num, tmpl_m, &tmpl) < 0){
			DBG_PRINTF("Invalid Form Command: get variables failed");
			SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);			
			return 0;			
		}
		// update tmpl_id
		if (tmpl.tmpl_id != tmpl_id){
			DBG_PRINTF("Invalid Form Command: tmpl_id unmatch");
			SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
			return 0;						
		}
		pack = ipc_service_template(ipc_fd, act, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &tmpl, 1);
		
	}else if ((act == IPC_CONFIG_DEL) && (del_cnt > 0)){
			MALLOC_CHK(ptmpl, ipc_service_template_t, del_cnt);
			memset(ptmpl, 0, sizeof(*ptmpl) * del_cnt);
			for (i = 0; i < del_cnt; i ++){
				ptmpl[i].tmpl_id = del[i];
				//DBG_PRINTF("GET DEL:%d\n", ptmpl[i].tmpl_id);				
			}
			pack = ipc_service_template(ipc_fd, act, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, ptmpl, del_cnt);
			if (ptmpl) free(ptmpl);			
	}

	IPC_RESULT(pack, result);

	if(pack) free(pack);
	
	return 0;
}




int user_prop_handle(webs_t wp, cgi_result_t *result)
{

	int act = IPC_CONFIG_SET, ret;	
	int count = 0, vc = 0, items;
	char *cp, svar[10], *sval, *vars[10];
	ipc_cnu_user_config_ack_t *pack = NULL;
	cnu_user_config_t *user_entry = NULL;

	sval = websGetVar(wp, "CMD", NULL);
	if (sval != NULL){
		if (!strcmp(sval, "USER_DEL")){
			act = IPC_CONFIG_DEL;
		}else if (!strcmp(sval, "USER_NEW")){
			act = IPC_CONFIG_NEW;
		}
	}	
	count = 0;
	// Count number of new users
	while(1) {
		sprintf(svar, "SET%d", count);
		if(!websGetVar(wp, svar, NULL)) break;
		count ++;
	}
	// Allocate memory 
	MALLOC_CHK(user_entry, cnu_user_config_t, count);
	// Fill USERs
	count = 0;
	vc = 0;
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
		//DBG_PRINTF("SET%d=%s\n", count, sval);
		
		cp = strchr(sval, '=');
		DBG_ASSERT(cp, "Invalid SET string: %s", sval);
		if(cp) {
			*(cp ++) = 0;

			items = split_string(cp, ';', vars);
					
			if (act == IPC_CONFIG_SET || act == IPC_CONFIG_NEW){
				if ((items < 6) || !strcmp(sval, "user_del")){
					//DBG_PRINTF("Invalid SET string: SET%d", count);
					count ++;
					continue;
				}
				//00:22:03:44:55:66;1;1;blk;username;desc">
				user_entry[vc].device_id = strtoul(vars[2], NULL, 0);
				user_entry[vc].tmpl_id = strtoul(vars[1], NULL, 0);
				user_entry[vc].auth = !strcmp(vars[3], "acc") ? CNU_AUTH_ACCEPT : CNU_AUTH_DENY;
				safe_strncpy(user_entry[vc].name, vars[4], sizeof(user_entry[vc].name));
				safe_strncpy(user_entry[vc].desc, vars[5], sizeof(user_entry[vc].desc));				
			}else if ((act == IPC_CONFIG_DEL) && strcmp(sval, "user_del")){
				count ++;
				continue;				
			}

			ret = hexencode(user_entry[vc].mac.octet, sizeof(ethernet_addr_t), vars[0]);
			if (ret != sizeof(ethernet_addr_t)){
				DBG_PRINTF("Invalid SET string: SET%d", count);
				count ++;
				continue;
			}	
			
			/*
			DBG_PRINTF("DEC%d.mac=%s,%d, %d,%d,%s,%s\n", count, 
				ether_addr_str(tmp, &user_entry[count].mac),
				user_entry[count].device_id,
				user_entry[count].tmpl_id,
				user_entry[count].auth,
				user_entry[count].name,
				user_entry[count].desc);
				*/
			vc ++;
		count ++;
	}
	}
	if(vc > 0) {
		pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_MAC, act, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, user_entry, vc);
		IPC_RESULT(pack, result);		
	}else {
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
	}

	if(user_entry) free(user_entry);

	if(pack) free(pack);
	return 0;

}


#endif  //add by zhouguanhua

/*

<INPUT type=hidden name=CMD value=SYS_MANIP>
<INPUT type=hidden name=GO value=http://192.168.0.100/index.asp>
<input type=hidden name=SET0 value="sys_ip=192.168.0.100">
<input type=hidden name=SET1 value="sys_nm=255.255.255.0">
<input type=hidden name=SET2 value="sys_gw=192.168.0.1">
<input type=hidden name=SET3 value="ip_proto=static">
<input type=hidden name=SET4 value="hostname=CLT502">
<input type=hidden name=SET5 value="sys_dns=202.96.125.24">
<input type=hidden name=SET6 value="sys_dns=34.45.25.14">
*/


static int sys_ip_get_config(int *valid, char *sval, char *cp, ipc_network_t *info, int *dhcp_action)
{	
#	define IPF_IP  0x01
#	define IPF_NM  0x02
#	define IPF_GW  0x04
#	define IPF_PR  0x08
#	define IPF_HN  0x10
#if 0 //add by zhouguanhua 2013/6/16
#	define IPF_DNS1 0x20
#	define IPF_DNS2 0x40
#endif  //add by zhouguanhua 2013/6/16
#	define IPF_DHCP 0x80

# define VF_IP  (0x9f)

	if (!(*valid & IPF_IP) && !strcmp("sys_ip", sval)){
		*valid |= IPF_IP;
		if (!inet_aton(cp, &info->netif.ip)){
			return 0;
		}
	}else if (!(*valid & IPF_NM) && !strcmp("sys_nm", sval)){
		*valid |= IPF_NM;
		if (!inet_aton(cp, &info->netif.subnet)){
			return 0;
		}
	}else if (!(*valid & IPF_GW) && !strcmp("sys_gw", sval)){
		*valid |= IPF_GW;
		if (!inet_aton(cp, &info->netif.gateway)){
			return 0;
		}
	}else if (!(*valid & IPF_PR) && !strcmp("ip_proto", sval)){
		*valid |= IPF_PR;
		info->ip_proto = !strcmp("static", cp) ? IP_PROTO_STATIC : IP_PROTO_DYNAMIC;
	}else if (!(*valid & IPF_HN) && !strcmp("hostname", sval)){
		*valid |= IPF_HN;
		safe_strncpy(info->hostname, cp, sizeof(info->hostname));
	}
	#if 0 //add by zhouguanhua 2013/6/16
	else if (!(*valid & IPF_DNS1) && !strcmp("sys_dns", sval)){
		*valid |= IPF_DNS1;
		if (!inet_aton(cp, &info->netif.dns[0])){
			return 0;
		}
	}
	else if (!(*valid & IPF_DNS2) && !strcmp("sys_dns", sval)){
		*valid |= IPF_DNS2;
		if (!inet_aton(cp, &info->netif.dns[1])){
			return 0;
		}
	}
	#endif  //add by zhouguanhua 2013/6/16
	else if (!(*valid & IPF_DHCP) && !strcmp("dhcp_action", sval)){
		*valid |= IPF_DHCP;
		if (dhcp_action && !strcmp(cp, "renew")){
			*dhcp_action = ACTION_RENEW;
		}else if (dhcp_action && !strcmp(cp, "release")){
			*dhcp_action = ACTION_RELEASE;
		}else{ // none
			*dhcp_action = 0;
		}
	}
	return VF_IP;
}

/*
SET0 value="vlan_man_vid=1">
SET1 value="web_ssl_enabled=0">
SET2 value="sys_ssh_enabled=0">
SET3 value="sys_ssh_port=22">
SET4 value="sys_telnet_enabled=0">
SET5 value="sys_telnet_port=23">
*/

static int sys_security_get_config(int *valid, char *sval, char *cp, sys_security_t *info)
{
		
#	define SECF_MVID  0x01
#	define SECF_SSLEN  0x02
#	define SECF_SSHEN  0x04
#	define SECF_SSHPT  0x08
#	define SECF_TLNEN  0x10
#	define SECF_TLNPT  0x20

# define VF_SECURITY  0x3f 

	if (!(*valid & SECF_MVID) && !strcmp("vlan_man_vid", sval)){
		*valid |= SECF_MVID;
		 info->mvlan = strtoul(cp, NULL, 0);
		 if (!info->mvlan || info->mvlan > 4094){
			return 0;
		 }
	}else if (!(*valid & SECF_SSLEN) && !strcmp("web_ssl_enabled", sval)){
		*valid |= SECF_SSLEN;
		info->https_en = strtoul(cp, NULL, 0);
	}else if (!(*valid & SECF_SSHEN) && !strcmp("sys_ssh_enabled", sval)){
		*valid |= SECF_SSHEN;
		info->ssh_en = strtoul(cp, NULL, 0);
	}else if (!(*valid & SECF_SSHPT) && !strcmp("sys_ssh_port", sval)){
		*valid |= SECF_SSHPT;
		info->ssh_port = strtoul(cp, NULL, 0);
	}else if (!(*valid & SECF_TLNEN) && !strcmp("sys_telnet_enabled", sval)){
		*valid |= SECF_TLNEN;
		info->telnet_en = strtoul(cp, NULL, 0);
	}else if (!(*valid & SECF_TLNPT) && !strcmp("sys_telnet_port", sval)){
		*valid |= SECF_TLNPT;
		info->telnet_port = strtoul(cp, NULL, 0);
	}
	return VF_SECURITY;
}

/*
SET0 value="snmp_enabled=1">
SET1 value="snmp_community_ro=public">
SET2 value="snmp_community_rw=private">
SET3 value="snmp_trap_enabled=0">
SET4 value="snmp_trap_ip=192.168.0.103">
SET5 value="snmp_trap_community=public">
SET6 value="snmp_trust_host=10128944">
*/

static int sys_snmp_get_config(int *valid, char *sval, char *cp, sys_snmp_t *info)
{
/*Begin modified by huangmingjian 2012/12/19:snmp*/	
#	define SNMPF_EN  						0x0001
#	define SNMPF_ENGINEID   				0x0002
#	define SNMP_MAX_PACKET_LEN 				0x0004
#	define SNMP_LOCATION   					0x0008
#	define SNMP_CONTACT   					0x0010
#	define SNMP_VERSION   					0x0020
#	define SNMP_TRUST_HOST   				0x0040
#	define SNMP_COMMUNITY_CURRENT_COUNT  	0x0080
#	define SNMP_COMMUNITY   				0x0100
#	define SNMP_ACCESS_MODE   				0x0200	 
#	define SNMP_ADD_COMM_WHICH_NUM   		0x0800	
#	define SNMP_DEL_COMM   					0x0400	
int which_num = 0;
/*
# define VF_SNMP 0x7f
*/
	if(!(*valid & SNMP_DEL_COMM) && !strcmp("snmp_del_comm_which_num", sval))
	{
		*valid |= SNMP_DEL_COMM;
		info->snmp_del_which_num = strtoul(cp, NULL, 0);
	}

	else if (!(*valid & SNMPF_EN) && !strcmp("snmp_enabled", sval))
	{
		*valid |= SNMPF_EN;
		info->snmp_en = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMPF_ENGINEID) && !strcmp("snmp_engineID", sval))
	{
		*valid |= SNMPF_ENGINEID;
		safe_strncpy(info->snmp_engineID, cp, sizeof(info->snmp_engineID));
	}
	else if (!(*valid & SNMP_MAX_PACKET_LEN) && !strcmp("snmp_max_packet_len", sval))
	{
		*valid |= SNMP_MAX_PACKET_LEN;
		info->snmp_max_packet_len = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_LOCATION) && !strcmp("snmp_location", sval))
	{
		*valid |= SNMP_LOCATION;
		safe_strncpy(info->snmp_location, cp, sizeof(info->snmp_location));
	}
	else if (!(*valid & SNMP_CONTACT) && !strcmp("snmp_contact", sval))
	{
		*valid |= SNMP_CONTACT;
		safe_strncpy(info->snmp_contact, cp, sizeof(info->snmp_contact));
	}
	else if (!(*valid & SNMP_VERSION) && !strcmp("snmp_version", sval))
	{
		*valid |= SNMP_VERSION;
		info->snmp_version = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_TRUST_HOST) && !strcmp("snmp_trust_host", sval))
	{
		*valid |= SNMP_TRUST_HOST;
		safe_strncpy(info->snmp_trust_host, cp, sizeof(info->snmp_trust_host));
	}
	else if (!(*valid & SNMP_COMMUNITY_CURRENT_COUNT) && !strcmp("snmp_comm_count", sval))
	{
		*valid |= SNMP_COMMUNITY_CURRENT_COUNT;
		info->snmp_comm_current_count = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_ADD_COMM_WHICH_NUM) && !strcmp("snmp_add_comm_which_num", sval))
	{
		*valid |= SNMP_ADD_COMM_WHICH_NUM;
		info->snmp_add_which_num = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_COMMUNITY) && !strcmp("snmp_add_comm_name", sval))
	{
		*valid |= SNMP_COMMUNITY;
		which_num = info->snmp_add_which_num;
		safe_strncpy(info->snmp_comm[which_num].snmp_comm_name, cp, sizeof(info->snmp_comm[which_num].snmp_comm_name));
	}
	else if (!(*valid & SNMP_ACCESS_MODE) && !strcmp("snmp_access_mode", sval))
	{
		*valid |= SNMP_ACCESS_MODE;
		which_num = info->snmp_add_which_num;
		info->snmp_comm[which_num].snmp_access_mode = strtoul(cp, NULL, 0);
		DBG_PRINTF("access_mode:%d", info->snmp_comm[which_num].snmp_access_mode);
	}
	return valid;
/*End modified by huangmingjian 2012/12/19:snmp*/ 
}
/*****************************************************************
Function:sys_snmp_trap_get_config
Description:the function for dealing with system_snmp_trap.asp
Author:zhouguanhua
Date:2012/12/20
INPUT:     
OUTPUT:         
Return:                          
Others:        
*****************************************************************/
static int sys_snmp_trap_get_config(int *valid, char *sval, char *cp, sys_snmp_t *info)
{

#	define SNMP_TRAP_EN  			0x0001
#	define SNMP_TRAP_IP             0x0002
#	define SNMP_TRAP_COM            0x0004
#	define SNMP_TRAP_COMUN  		0x0008
#	define SNMP_TRAP_VERS  		    0x0010
#	define SNMP_TRAP_SAFE   		0x0020
#	define SNMP_TRAP_LAST_SAVE_SAFE_NUM 0x0040

# 	define VF_SNMP_TRAP 0x007f
	if (!(*valid & SNMP_TRAP_EN) && !strcmp("snmp_trap_en", sval))
	{
		*valid |= SNMP_TRAP_EN;
		info->snmp_trap_en = strtoul(cp, NULL, 0);
	}
    else if (!(*valid & SNMP_TRAP_IP) && !strcmp("snmp_trap_ip", sval))
    {
        *valid |= SNMP_TRAP_IP;
        if (!inet_aton(cp, &info->snmp_trap_ip))
        {
			return 0;
        } 
		}		
	else if (!(*valid & SNMP_TRAP_COM) && !strcmp("snmp_trap_com", sval))
	{
		*valid |= SNMP_TRAP_COM;
		info->snmp_trap_com = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_TRAP_COMUN) && !strcmp("snmp_trap_community", sval))
	{
		*valid |= SNMP_TRAP_COMUN;
		safe_strncpy(info->snmp_trap_community, cp, sizeof(info->snmp_trap_community));
	}
	else if (!(*valid & SNMP_TRAP_VERS) && !strcmp("snmp_trap_version", sval))
	{
		*valid |= SNMP_TRAP_VERS;
		info->snmp_trap_version = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_TRAP_SAFE) && !strcmp("snmp_trap_safe", sval))
	{
		*valid |= SNMP_TRAP_SAFE;
		info->snmp_trap_safe = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SNMP_TRAP_LAST_SAVE_SAFE_NUM) && !strcmp("snmp_last_save_safe_num", sval))
	{
		*valid |= SNMP_TRAP_LAST_SAVE_SAFE_NUM;
		info->snmp_last_save_safe_num = strtoul(cp, NULL, 0);
	}
	return VF_SNMP_TRAP;
}
/*****************************************************************
Function:sys_snmp_add_user_get_config
Description:the function for dealing with system_snmp_user.asp
Author:zhouguanhua
Date:2012/12/20
INPUT:     
OUTPUT:         
Return:                          
Others:        
*****************************************************************/
static int sys_snmp_add_user_get_config(int *valid, char *sval, char *cp, sys_snmp_t *info)
{

#	define SNMP_USER_NAME  			0x0001
#	define SNMP_USER_SAFE           0x0002
#	define SNMP_USER_INGROUP        0x0004
#	define SNMP_USER_PASSWORD  		0x0008
#	define SNMP_USER_ENPASSWORD     0x0010
#	define SNMP_USER_DELE           0x0020
#	define SNMP_USER_COUNT          0x0040       
#	define SNMP_USER_WHICH_NUM      0x0080  


# 	define VF_SNMP_TRAP 0x00ff

    if (!(*valid & SNMP_USER_DELE) && !strcmp("snmp_del_user_which_num", sval))
	{
		*valid |= SNMP_USER_DELE;
		info->snmp_del_which_num = strtoul(cp, NULL, 0);
	}
    else if (!(*valid & SNMP_USER_WHICH_NUM) && !strcmp("snmp_add_group_which_num", sval))
    {
        *valid |= SNMP_USER_WHICH_NUM;
        info->snmp_add_which_num = strtoul(cp, NULL, 0);
    }

	 else if (!(*valid & SNMP_USER_COUNT) && !strcmp("snmp_count", sval))
	{
		*valid |= SNMP_USER_COUNT;
		info->snmp_user_current_count = strtoul(cp, NULL, 0);
	} 
    else if (!(*valid & SNMP_USER_NAME) && !strcmp("snmp_user_name", sval))
	{
		*valid |= SNMP_USER_NAME;
		safe_strncpy(info->snmp_user[info->snmp_add_which_num].snmp_user_name, cp, sizeof(info->snmp_user[info->snmp_add_which_num].snmp_user_name));
        safe_strncpy(info->snmp_trust_host, cp, sizeof(info->snmp_trust_host));
	}

    else if (!(*valid & SNMP_USER_SAFE) && !strcmp("snmp_user_security", sval))
    {
        *valid |= SNMP_USER_SAFE;
        info->snmp_user[info->snmp_add_which_num].snmp_user_security = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & SNMP_USER_INGROUP) && !strcmp("snmp_user_ingroup", sval))
	{
		*valid |= SNMP_USER_INGROUP;
		safe_strncpy(info->snmp_user[info->snmp_add_which_num].snmp_user_group, cp, sizeof(info->snmp_user[info->snmp_add_which_num].snmp_user_group));
	}

	else if (!(*valid & SNMP_USER_PASSWORD) && !strcmp("snmp_user_password", sval))
	{
		*valid |= SNMP_USER_PASSWORD;
		safe_strncpy(info->snmp_user[info->snmp_add_which_num].snmp_user_password, cp, sizeof(info->snmp_user[info->snmp_add_which_num].snmp_user_password));
	}

	else if (!(*valid & SNMP_USER_ENPASSWORD) && !strcmp("snmp_user_enpassword", sval))
	{
		*valid |= SNMP_USER_ENPASSWORD;
		safe_strncpy(info->snmp_user[info->snmp_add_which_num].snmp_user_enpassword, cp, sizeof(info->snmp_user[info->snmp_add_which_num].snmp_user_enpassword));
	}           

	return VF_SNMP_TRAP;
}



static int sys_ip_prop_handle(webs_t wp, cgi_result_t *result)
{
	int ret, valid, count;
	ipc_sys_network_ack_t *pack = NULL;
    ipc_sys_network_ack_t *pack_tmp = NULL;
	ipc_network_t info;
	char *cp, svar[10], *sval;	
	int dhcp_action = 0;

	valid = 0;
	count = 0;
	memset(&info, 0 , sizeof(info));
	
	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = sys_ip_get_config(&valid, sval, cp, &info, &dhcp_action))){
			break;
		}
		if ((valid & ret) == ret){ // already get all need values
			break;
		}		
		count ++;
	}	

	if ((valid & ret) != ret){
		DBG_PRINTF("Invalid Form IP Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}

	if (info.ip_proto == IP_PROTO_DYNAMIC){
		memset(&info.netif, 0, sizeof(info.netif));
		info.action = dhcp_action;
	}
	pack_tmp = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
    if(pack_tmp &&(IPC_STATUS_OK  == pack_tmp->hdr.status))
    {
        #if 0
        memcpy(info.interface_state, &pack_tmp->network.interface_state, sizeof(info.interface_state));
        
        info.m_vlan = pack_tmp->network.m_vlan;
        info.vlan_interface = pack_tmp->network.vlan_interface;

		/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
		info.loopback_enable = pack_tmp->network.loopback_enable;
		info.loop_update = pack_tmp->network.loop_update;
		/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
		#endif

	}
    else
    {   
		DBG_PRINTF("Get sys networking failed!\n");
        if(pack_tmp) free(pack_tmp);
        return 0;
    }
	if(pack_tmp) free(pack_tmp);
    
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);

	IPC_RESULT(pack, result);

	if(pack) free(pack);
	
	return 0;
}

static int sys_mvlan_get_config(int *valid, char *sval, char *cp, sys_mvlan_t *info)
{
#define M_VLAN  0x01
#define VF_VLAN  0x01 
     if (!(*valid & M_VLAN) && !strcmp("mvlan", sval))
     {
		*valid |= M_VLAN;
		info->m_vlan = strtoul(cp, NULL, 0);       	
        info->vlan_interface = info->m_vlan;
        DBG_PRINTF("m_vlan:%d\n",info->m_vlan);
	 }
	return VF_VLAN;
}

/*****************************************************************
Function:sys_mvlan_handle
Description:the function for dealing with mvaln.asp  
Author:huangmingjian
Date:2012/11/13
INPUT:wp  *result       
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/

static int sys_mvlan_handle(webs_t wp, cgi_result_t *result)
{
	#if 0 //add by zhouguanhua 2013/5/31
	int count = 0;
	ipc_sys_mvlan_ack_t *pack = NULL;
  //  ipc_sys_network_ack_t *pack_tmp = NULL;
	sys_mvlan_t info;
	char *cp = NULL;
	char svar[10] = {0};
	char *sval = NULL;	
	
	memset(&info, 0 , sizeof(info));
	
	// load values from web
	while(1)
	{
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) 
		{
			break;
		}
		DBG_PRINTF("SET%d = %s ", count, sval);			
		cp = strchr(sval, '=');
		if (!cp) 
		{
			break;
		}
		*cp = 0; 
		cp++;
		if(!strcmp("mvlan", sval))
		{
			info.m_vlan = strtol(cp, NULL, 0);	
			info.vlan_interface = info.m_vlan;	
		}
		count++;
	}	

	pack_tmp = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
    if(pack_tmp &&(IPC_STATUS_OK  == pack_tmp->hdr.status))
    {	
    	info.ip_proto = pack_tmp->network.ip_proto;
        memcpy(info.interface_state, pack_tmp->network.interface_state, sizeof(info.interface_state));
		memcpy(&info.netif, &pack_tmp->network.netif, sizeof(info.netif));
		memcpy(info.hostname, pack_tmp->network.hostname, sizeof(info.hostname));

		/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
		info.loopback_enable = pack_tmp->network.loopback_enable;
		info.loop_update = pack_tmp->network.loop_update;
		/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */
    }
    else
    {   
		DBG_PRINTF("Get sys networking failed!\n");
        if(pack_tmp)
		{
			free(pack_tmp);
		}
        return -1;
    }
	if(pack_tmp) 
	{
		free(pack_tmp);
	}
    
	pack = ipc_sys_mvlan(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack, result);
	if(pack) 
	{
		free(pack);
	}
	return 0;		
    #endif //add by zhouguanhua 2013/5/31
	int ret, valid, count;
	ipc_sys_mvlan_ack_t *pack = NULL;
	sys_mvlan_t info;
	char *cp, svar[10], *sval;	
	valid = 0;
	count = 0;
	memset(&info, 0 , sizeof(info));	
	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	

		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = sys_mvlan_get_config(&valid, sval,  cp, &info))){
			break;
		}
		if ((valid & ret) == ret){ // already get all need values
			break;
		}
		count ++;
	}

	if ((valid & ret) != ret){
		DBG_PRINTF("Invalid Form mvlan Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}
	pack = ipc_sys_mvlan(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);

	IPC_RESULT(pack, result);
	if(pack)
    {
	    free(pack);
	}
	return 0;	


}


static int sys_loop_get_config(int *valid, char *sval, char *cp, sys_loop_t *info)
{
#define LOOP_EN  0x01
#define LOOP_SAMPLE  0x02
#define VF_LOOP  0x03 

	if (!(*valid & LOOP_EN) && !strcmp("loopback", sval))
	{
		*valid |= LOOP_EN;
		 info->loopback_enable = strtoul(cp, NULL, 0);
	}else if (!(*valid & LOOP_SAMPLE) && !strcmp("sample", sval)){
		*valid |= LOOP_SAMPLE;
		info->loop_update = strtoul(cp, NULL, 0);
	}	
	return VF_LOOP;
}

/*****************************************************************
Function:sys_loopback_handle
Description:the function for dealing with loopback_detection.asp
Author:huangmingjian
Date:2012/11/27
INPUT:wp  *result       
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/

static int sys_loopback_handle(webs_t wp, cgi_result_t *result)
{
	#if 0
	int count = 0;
	ipc_sys_network_ack_t *pack = NULL;
    ipc_sys_network_ack_t *pack_tmp = NULL;
	ipc_network_t info;
	char *cp = NULL;
	char svar[10] = {0};
	char *sval = NULL;	
	
	memset(&info, 0 , sizeof(info));
	
	while(1)
	{
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) 
		{
			break;
		}
		//DBG_PRINTF("SET%d = %s ", count, sval);	
		cp = strchr(sval, '=');
		if (!cp) 
		{
			break;
		}
		*cp = 0; 
		cp++;
		if(!strcmp("loopback", sval))
		{
			info.loopback_enable = strtol(cp, NULL, 0);	
		}
		if(!strcmp("sample", sval))
		{
			info.loop_update = strtol(cp, NULL, 0);	
		}
		count++;
	}	
		
	pack_tmp = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
    if(pack_tmp &&(IPC_STATUS_OK  == pack_tmp->hdr.status))
    {	
    	info.ip_proto = pack_tmp->network.ip_proto;
        memcpy(info.interface_state, pack_tmp->network.interface_state, sizeof(info.interface_state));
		memcpy(&info.netif, &pack_tmp->network.netif, sizeof(info.netif));
		memcpy(info.hostname, pack_tmp->network.hostname, sizeof(info.hostname));
		info.m_vlan =  pack_tmp->network.m_vlan;	
		info.vlan_interface = pack_tmp->network.vlan_interface;	
    }
    else
    {   
		DBG_PRINTF("Get sys networking failed!\n");
        if(pack_tmp)
		{
			free(pack_tmp);
		}
        return -1;
    }

	if(pack_tmp) 
	{
		free(pack_tmp);
	}
    
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack, result);
	if(pack) 
	{
		free(pack);
	}
	return 0;	
	#endif
	int ret, valid, count;
	ipc_sys_loop_ack_t *pack = NULL;
	sys_loop_t info;
	char *cp, svar[10], *sval;	
	valid = 0;
	count = 0;
	
	memset(&info, 0 , sizeof(info));	
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = sys_loop_get_config(&valid, sval,  cp, &info)))
		{
			break;
        }
		if ((valid & ret) == ret){ // already get all need values
			break;
		}
		count ++;
	}

	if ((valid & ret) != ret){
		DBG_PRINTF("Invalid Form Loop Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}
	pack = ipc_sys_loop(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
	IPC_RESULT(pack, result);
	if(pack) free(pack);
	return 0;		
}

static int sys_security_prop_handle(webs_t wp, cgi_result_t *result)
{
	// only support static mode
	int ret, valid, count;
	ipc_sys_mvlan_ack_t *mvlan_pack = NULL;
	ipc_sys_security_ack_t *pack = NULL;
	sys_mvlan_t mvlan_info;
	sys_security_t info;
	char *cp, svar[10], *sval;	
	char value[16] = {0};
	char tel_en_old[16] = {0};
	pid_t pid = 0;

	valid = 0;
	count = 0;
	memset(&mvlan_info, 0 , sizeof(mvlan_info));	
	memset(&info, 0 , sizeof(info));	
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		DBG_PRINTF("SET%d = %s\n", count, sval);			
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = sys_security_get_config(&valid, sval,  cp, &info))){
			break;
		}
		if ((valid & ret) == ret){ // already get all need values
			break;
		}
		count ++;
	}

	if ((valid & ret) != ret)
	{
		DBG_PRINTF("Invalid Form Security Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}

	/*mvlan*/
	mvlan_info.m_vlan = mvlan_info.vlan_interface = info.mvlan;
	mvlan_pack = ipc_sys_mvlan(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &mvlan_info);
	IPC_RESULT(mvlan_pack, result);
	if(mvlan_pack)
    {
	    free(mvlan_pack);
	}

	/*telnet*/
	strcpy(value, info.telnet_en==1?ENABLE_STR:DISABLE_STR);
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_TELNET_SERVICE_ENABLE, (void *)tel_en_old, "", sizeof(value))) < 0)
	{
		DBG_ASSERT(0, "Get TELNET_SERVICE_ENABLE faild");
		return -1;
	}
	
	if(strcmp(tel_en_old, value))
	{	
		if(1 == info.telnet_en)
		{
			if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_TELNET_SERVICE_ENABLE, ENABLE_STR) != 0)
		    {
		        return -1;   
		    }
		}
		else
		{
    		if((pid = get_pid_by_name("vtysh")) > 0)
    		{   
                if(-1 == kill(pid, SIGUSR2))
                {   
                    DBG_PRINTF(" Web disabled vtysh fail, pid:%d\n", pid);
                    return -1;
                } 
    		}
		}
	}

	
	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info, 0);
	IPC_RESULT(pack, result);
	if(pack) 
	{
		free(pack);
	}

	return 0;	
}


static int sys_snmp_prop_handle(webs_t wp, cgi_result_t *result)
{
	/*Begin add by huangmingjian 2012/12/19:snmp*/ 
	ipc_sys_snmp_ack_t *pack = NULL;
	sys_snmp_t info;
	char *cp, svar[10], *sval;	

	# define IS_SNMP_DEL_COMM_SET   		 0x0400	
	# define IS_SNMP_AGENT_SET 	 			 0x007f
	# define IS_SNMP_AGENT_AND_ADD_COMM_SET	 0x0bff
	int ret= 0;
	int valid = 0;
	int count = 0;
	memset(&info, 0 , sizeof(info));	
	// load values from web
	while(1)
	{
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		DBG_PRINTF("SET%d = %s", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp)
		{
			break;
	}
		*cp = 0; 
		cp++;
		if (!(ret = sys_snmp_get_config(&valid, sval, cp, &info)))
		{
			break;
		}		
		count ++;
	}	

	DBG_PRINTF("valid:%04X\n", valid);
	if(IS_SNMP_DEL_COMM_SET == valid)
	{	
		info.snmp_set_flag |= SNMP_DEL_COMM_FLAG;
	}
	else if(IS_SNMP_AGENT_SET == valid)
	{
		info.snmp_set_flag |= SNMP_AGENT_FLAG;
	}
	else if(IS_SNMP_AGENT_AND_ADD_COMM_SET == valid)
	{
		info.snmp_set_flag |= SNMP_AGENT_FLAG;
		info.snmp_set_flag |= SNMP_ADD_COMM_FLAG;
	}
	else
	{
		DBG_PRINTF("Invalid Form SNMP Command:%04X\n", valid);
		return 0;
	}
	pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
	IPC_RESULT(pack, result);
	if(pack)
	{
		free(pack);
	}
	return 0;	
	/*End add by huangmingjian 2012/12/19:snmp*/ 
}
/*****************************************************************
Function:sys_snmp_trap_prop_handle
Description:the function for dealing with system_snmp_trap.asp
Author:zhouguanhua
Date:2012/12/20
INPUT:wp  *result       
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/
static int sys_snmp_trap_prop_handle(webs_t wp, cgi_result_t *result)
{       
    ipc_sys_snmp_ack_t *pack = NULL;
    sys_snmp_t info;
    char *cp, svar[10], *sval;  
        
    int ret= 0;
    int valid = 0;
    int count = 0;
    memset(&info, 0 , sizeof(info));       
        // load values from web
        while(1)
        {
            sprintf(svar, "SET%d", count);          
            sval = websGetVar(wp, svar, NULL);      
            if(!sval) break;    
            DBG_PRINTF("SET%d = %s", count, sval);          
            cp = strchr(sval, '=');
            if (!cp)
            {
                break;
            }
            *cp = 0; 
            cp++;
            if (!(ret = sys_snmp_trap_get_config(&valid, sval, cp, &info)))
            {
                break;
            }
            if ((valid & ret) == ret)   /*already get all need values*/
            { 
                break;
            }       
            count++;
        }   
        if ((valid & ret) != ret)
        {
            DBG_PRINTF("Invalid Form SNMP_TRAP Command:%04X\n", valid);
            SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);       
            return 0;
        }
    DBG_PRINTF("valid:%04X\n", valid);
    info.snmp_set_flag |= SNMP_TRAP_FLAG;

    pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
    IPC_RESULT(pack, result);
        
    if(pack)
    {
    free(pack);
    }
            
    return 0;   
}

/*****************************************************************
Function:sys_snmp_user_prop_handle
Description:the function for dealing with system_snmp_trap.asp
Author:zhouguanhua
Date:2012/12/20
INPUT:wp  *result       
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/
static int sys_snmp_user_prop_handle(webs_t wp, cgi_result_t *result)
{       
    ipc_sys_snmp_ack_t *pack = NULL;
    sys_snmp_t info;
    char *cp, svar[33], *sval;  

# define IS_SNMP_ADD_USER            0x00df
# define IS_SNMP_DELE_USER           0x0020

        
    int ret= 0;
    int valid = 0;
    int count = 0;
    memset(&info, 0 , sizeof(info));       
        // load values from web
    while(1)
    {
            sprintf(svar, "SET%d", count);          
            sval = websGetVar(wp, svar, NULL);      
            if(!sval) break;    
            DBG_PRINTF("SET%d = %s", count, sval);          
            cp = strchr(sval, '=');
            if (!cp)
            {
                break;
            }
            *cp = 0; 
            cp++;
            if (!(ret = sys_snmp_add_user_get_config(&valid, sval, cp, &info)))
            {
                break;
            }
            count++;
     }  

    DBG_PRINTF("valid:%04X\n", valid);
    if(IS_SNMP_ADD_USER == valid)
    {   
        info.snmp_set_flag |= SNMP_ADD_USER_FLAG;            
    }
    else if(IS_SNMP_DELE_USER == valid)
    {
            info.snmp_set_flag |= SNMP_DEL_USER_FLAG;         
    }
    else
    {
        DBG_PRINTF("Invalid Form SNMP add user:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
     }
        
    pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW , &info);
    IPC_RESULT(pack, result);
        
    if(pack)
    {
        free(pack);
    }
        
    return 0;   
}

/*****************************************************************
    Function:sys_snmp_group_handle
    Description: system snmp group handle
    Author:huangmingjian
    Date:2012/12/23
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/

static int  sys_snmp_group_handle(webs_t wp, cgi_result_t *result)
{
	ipc_sys_snmp_ack_t *pack = NULL;
	sys_snmp_t info;
	char *cp  = NULL;	
	char *sval = NULL;	
	char svar[10] ={0}; 
	char flag_which_set = 0;
//	int ret= 0;
	int count = 0;
	# define SNMP_DEL_GROUP_SET		1 
	memset(&info, 0 , sizeof(info));	
	while(1)
	{
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		DBG_PRINTF("SET%d = %s", count, sval);			
		cp = strchr(sval, '=');
		if (!cp)
		{
			break;
		}
		*cp = 0; 
		cp++; 
		if(!strcmp("snmp_del_group_which_num", sval))
		{	
			flag_which_set = SNMP_DEL_GROUP_SET;
			info.snmp_del_which_num = strtoul(cp, NULL, 0);
		}
		else if(!strcmp("snmp_group_count", sval))
		{
			info.snmp_group_current_count = strtoul(cp, NULL, 0);
		}
		else if(!strcmp("snmp_add_group_which_num", sval)) /*must fetch first before group name security and mode*/
		{
			info.snmp_add_which_num = strtoul(cp, NULL, 0);
		}
		else if(!strcmp("snmp_group_name", sval))
		{
			safe_strncpy(info.snmp_group[info.snmp_add_which_num].snmp_group_name, cp, sizeof(info.snmp_group[info.snmp_add_which_num].snmp_group_name));
		}
		else if(!strcmp("snmp_group_security", sval))
		{
			info.snmp_group[info.snmp_add_which_num].snmp_group_security = strtoul(cp, NULL, 0);
		}
		else if(!strcmp("snmp_group_mode", sval))
		{
			info.snmp_group[info.snmp_add_which_num].snmp_group_mode = strtoul(cp, NULL, 0);
		}
		else
		{
			DBG_PRINTF("get value from web error!");
			return -1;	
		}
		count++;
	}
	if(SNMP_DEL_GROUP_SET == flag_which_set)
	{
		info.snmp_set_flag |= SNMP_DEL_GROUP_FLAG;
	}
	else
	{
		info.snmp_set_flag |= SNMP_ADD_GROUP_FLAG;
	}
	pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
	IPC_RESULT(pack, result);
	if(pack)
	{
		free(pack);
	}
	
	return 0;	

}
#if 0 //add by zhouguanhua
/*****************************************************************
    Function:sys_snmp_catvinfo_handle
    Description: system snmp catvinfo handle
    Author:huangmingjian
    Date:2012/12/23
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
static int sys_snmp_catvinfo_handle(webs_t wp, cgi_result_t *result)
{
	ipc_sys_snmp_ack_t *pack = NULL;
	sys_snmp_t info;
	char *cp  = NULL;	
	char *sval = NULL;	
	char svar[10] ={0}; 
	int count = 0;
	memset(&info, 0 , sizeof(info));	
	while(1)
	{
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
		DBG_PRINTF("SET%d = %s", count, sval);			
		cp = strchr(sval, '=');
		if (!cp)
		{
			break;
		}
		*cp = 0; 
		cp++; 
		if(!strcmp("dev_status_update", sval))
		{	
			info.snmp_catvinfo.dev_status_update = strtoul(cp, NULL, 0);
		}
		else if(!strcmp("dev_status_output_level_atten", sval)) 
		{
			info.snmp_catvinfo.output_level_atten = strtoul(cp, NULL, 0);
		}
		else
		{
			DBG_PRINTF("get value from web error!");
			if(pack)
			{
				free(pack);
			}
			return -1;	
		}
		count++;
	}
	DBG_PRINTF("hadle:%d", info.snmp_catvinfo.output_level_atten);
	info.snmp_set_flag |= SNMP_CATVINFO_FLAG;
	pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
	IPC_RESULT(pack, result);
	if(pack)
	{
		free(pack);
	}
	
	return 0;	
}

#endif //add by zhouguanhua
/*****************************************************************
Function:sys_syslog_config
Description:the function for dealing with system_syslog_set.asp
Author:zhouguanhua
Date:2012/12/20
INPUT:     
OUTPUT:         
Return:                          
Others:        
*****************************************************************/
static int sys_syslog_config(int *valid, char *sval, char *cp, sys_syslog_t *info)
{

#	define SYSLOG_EN  			 0x0001
#	define SYSLOG_IP             0x0002
#	define SYSLOG_DEGREE         0x0004
#	define SYSLOG_SAVE_DEGREE         0x0008
#ifdef CONFIG_PRODUCT_5500
#	define SYSLOG_PORT			 0x0010			/*add by leijinbao 2013/8/21*/
# 	define VF_SYSLOG 0x001f
#else
#define VF_SYSLOG 0x000f
#endif
	if (!(*valid & SYSLOG_EN) && !strcmp("syslog_en", sval))
	{
		*valid |= SYSLOG_EN;
		info->syslog_en = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SYSLOG_IP) && !strcmp("syslog_ip", sval))
	{
		*valid |= SYSLOG_IP;
		safe_strncpy(info->syslog_ip, cp, sizeof(info->syslog_ip));
	}
	else if (!(*valid & SYSLOG_DEGREE) && !strcmp("syslog_degree", sval))
	{
		*valid |= SYSLOG_DEGREE;
		info->syslog_degree = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & SYSLOG_SAVE_DEGREE) && !strcmp("syslog_save_degree", sval))
	{
		*valid |= SYSLOG_SAVE_DEGREE;
		info->syslog_save_degree = strtoul(cp, NULL, 0);
	}
	/*add by leijinbao 2013/8/21 for 5500*/
#ifdef CONFIG_PRODUCT_5500
	if (!(*valid & SYSLOG_PORT) && !strcmp("syslog_port", sval))
	{
		*valid |= SYSLOG_PORT;
		info->syslog_port = strtoul(cp, NULL, 0);
	}
#endif
	return VF_SYSLOG;
}
/*****************************************************************
Function:sys_syslog_handle
Description:the function for dealing with system_syslog_set.asp
Author:zhouguanhua
Date:2013/4/7
INPUT:wp  *result       
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/
static int sys_syslog_handle(webs_t wp, cgi_result_t *result)
{       
    ipc_syslog_ack_t *pack = NULL;
    sys_syslog_t info;
	char buf[1024];
    memset(&info, 0 , sizeof(info));   

        // load values from web
        #if 0
        while(1)
        {
            sprintf(svar, "SET%d", count);          
            sval = websGetVar(wp, svar, NULL);      
            if(!sval) break;    
            DBG_PRINTF("SET%d = %s", count, sval);          
            cp = strchr(sval, '=');
            if (!cp)
            {
                break;
            }
            *cp = 0; 
            cp++;
            if (!(ret = sys_syslog_config(&valid, sval, cp, &info)))
            {
                break;
            }
            if ((valid & ret) == ret)   /*already get all need values*/
            { 
                break;
            }       
            count++;
        }   
        if ((valid & ret) != ret)
        {
            DBG_PRINTF("Invalid Form Syslog Command:%04X\n", valid);
            SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);       
            return 0;
        }
   		DBG_PRINTF("valid:%04X\n", valid);
		#endif
	memset(buf,0,sizeof(buf));
	getValueByKey(wp,"syslog_en",buf,sizeof(buf));
	info.syslog_en = strtoul(buf,NULL,0);

	memset(buf,0,sizeof(buf));
	getValueByKey(wp,"syslog_save_degree",buf,sizeof(buf));
	info.syslog_save_degree = strtoul(buf,NULL,0);

	memset(buf,0,sizeof(buf));
	getValueByKey(wp,"syslog_ip",buf,sizeof(buf));
	safe_strncpy(info.syslog_ip, buf, sizeof(info.syslog_ip));
	
	memset(buf,0,sizeof(buf));
	getValueByKey(wp,"syslog_degree",buf,sizeof(buf));
	info.syslog_degree = strtoul(buf,NULL,0);

	memset(buf,0,sizeof(buf));
	getValueByKey(wp,"syslog_port",buf,sizeof(buf));
	info.syslog_port = strtoul(buf,NULL,0);
	countnum = 0;

    pack = ipc_sys_syslog(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
    IPC_RESULT(pack, result);
        
    if(pack)
    {
    free(pack);
    }
            
    return 0;   

}


static int sys_admin_prop_handle(webs_t wp, cgi_result_t *result)
{
	// only support static mode
	int valid, count;
	ipc_sys_admin_ack_t *pack = NULL;
	sys_admin_t info;
	char *cp, svar[10], *sval, *vars[8];	
	char tmp[128];	
	int ucnt = 0, items;

	valid = 0;
	count = 0;
	memset(&info, 0 , sizeof(info));

	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
		safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(tmp, '=');
		if (!cp) break;
		*cp = 0; cp ++;

		if (!strcmp(tmp, "idle_time")){
			info.idle_time = strtoul(cp, NULL, 0);
		}else if ((ucnt < sizeof(info.users)/sizeof(info.users[0])) && !strcmp(tmp, "user")){
			items = split_string(cp, ';', vars);
			if ((items > 1) && vars[0][0]){
				//safe_strncpy(info.users[ucnt].old_name, vars[0], sizeof(info.users[ucnt].old_name));				
				//info.users[ucnt].enable = strtoul(vars[1], NULL, 0);
				//if (info.users[ucnt].enable){
					safe_strncpy(info.users[ucnt].name, vars[0], sizeof(info.users[ucnt].name));
					safe_strncpy(info.users[ucnt].pwd, vars[1], sizeof(info.users[ucnt].pwd));
					//printf("info.users[%d].name = %s\n",ucnt,info.users[ucnt].name);
					
					//printf("info.users[%d].pwd = %s\n",ucnt,info.users[ucnt].pwd);
				//}
				ucnt ++;
			}
		}
		count ++;
	}

	info.count = ucnt;
	
	pack = ipc_sys_admin(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);

	IPC_RESULT(pack, result);

	if(pack) free(pack);
	
	return 0;	
}

#if 0 //add by zhouguanhua

/*
<SET0 value="eth_cfg=eth0;3;1;0;1000;full;0;3;524288;0;0;1">
<SET1 value="eth_cfg=eth1;4;1;1;100;full;1;off;0;4194304;0;1">
*/
static int sys_ethernet_parse_config(const char *cp, switch_interface_t *ifp)
{
	int items, ret = 0; 
	char *vars[20];	
	char *str= strdup(cp);
#define bool_value(x)  (((x) > 0) ? 1 : 0)
	
	items = split_string(str, ';', vars);
	if (items >= 12){
		safe_strncpy(ifp->ifname, vars[0], sizeof(ifp->ifname));
		ifp->phy = bool_value(strtoul(vars[1], NULL, 0));
		ifp->enabled = bool_value(strtoul(vars[2], NULL, 0));
		ifp->autoneg = bool_value(strtoul(vars[3], NULL, 0));
		ifp->speed = speed_atoi(vars[4]);
		ifp->duplex = !strcmp(vars[5], "full") ? ETH_DUPLEX_FULL : ETH_DUPLEX_HALF;
		ifp->flowctrl = bool_value(strtoul(vars[6], NULL, 0)); 
		if (!strcmp(vars[7], "off")){
			ifp->priority = ETH_PRIORITY_OFF;
		}else {
			ifp->priority = strtoul(vars[7], NULL, 0);
		}
		ifp->ingress_limit = strtoul(vars[8], NULL, 0); 
		ifp->egress_limit = strtoul(vars[9], NULL, 0);
		ifp->ingress_filter = strtoul(vars[10], NULL, 0);		
		ifp->pvid = strtoul(vars[11], NULL, 0);

		ret = 1;
	}
	free(str);	
	return ret;
}


static int sys_ethernet_prop_handle(webs_t wp, cgi_result_t *result)
{
	// only support static mode
	int valid, count;
	ipc_clt_interface_config_ack_t *pack = NULL;
	switch_interface_t *ifps;
	char *cp, svar[10], *sval;	

	ifps = (switch_interface_t *)malloc(sizeof(switch_interface_t) * MAX_CLT_ETH_PORTS);
	if (ifps == NULL){
		DBG_ASSERT(0, "Malloc failed");
		return 0;
	}

	valid = 0;
	count = 0;

	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (sys_ethernet_parse_config(cp, &ifps[valid])){
			valid ++;
		}
		count ++;
	}

	if ( valid > 0){// not valid set string 
		pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, ifps, valid);
		IPC_RESULT(pack, result);		
		if(pack) free(pack);
	}else {
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
	}

	free(ifps);
	
	return 0;	
}



static int sys_interface_mibs_prop_handle(webs_t wp, cgi_result_t *result)
{
	int valid, count;
	ipc_clt_interface_mibs_ack_t *pack = NULL;
	char *cp, svar[10], *sval;	

	valid = 0;
	count = 0;

	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!strcmp(sval, "reset") && !strcmp(cp, "all")){
			valid ++;
			break;
		}
		count ++;
	}

	if ( valid > 0){// not valid set string 
		pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_RESET,  NULL, 0 );
		IPC_RESULT_NOWAIT(pack, result);
		if(pack) free(pack);
	}else {
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);
	}
	
	return 0;	
}

#endif  //add by zhouguanhua
/*
<FORM name=OUT action=goform/command method=POST>
<INPUT type=hidden name=CMD value=SYS_TIME>
<INPUT type=hidden name=GO value=system_time.asp>
<input type=hidden name=SET0 value="time_zone=44">
<input type=hidden name=SET1 value="time_sync=1">
<input type=hidden name=SET2 value="time_server=pool.ntp.org">
<input type=hidden name=SET3 value="time_utc=1304263100">
</FORM>
*/

static int sys_time_get_config(int *valid, char *sval, char *cp, sys_time_t *info)
{
		
#	define TM_ZONE  0x01
#	define TM_SYNC  0x02
#	define TM_SERVER  0x04
#	define TM_UTC  0x08

# define VF_TIME  0x0f 

	if (!(*valid & TM_ZONE) && !strcmp("time_zone", sval)){
		*valid |= TM_ZONE;
		 info->zone = strtoul(cp, NULL, 0);
		 if (!valid_time_zone(info->zone)){
			DBG_ASSERT(0, "Invalid Time zone");
			return 0;
		 }
	}else if (!(*valid & TM_SYNC) && !strcmp("time_sync", sval)){
		*valid |= TM_SYNC;
		info->ntp_en = (strtoul(cp, NULL, 0) > 0) ? 1 : 0;
		
	}else if (!(*valid & TM_SERVER) && !strcmp("time_server", sval)){
		*valid |= TM_SERVER;
		if (cp[0]){
			safe_strncpy(info->server, cp, sizeof(info->server));
		}
	}else if (!(*valid & TM_UTC) && !strcmp("time_utc", sval)){
		*valid |= TM_UTC;
		info->time = strtoul(cp, NULL, 0);
	}
	return VF_TIME;
}


static int sys_time_handle(webs_t wp, cgi_result_t *result)
{
	// only support static mode
	int ret, valid, count;
	ipc_sys_time_ack_t *pack = NULL;
	sys_time_t info;
	char *cp, svar[10], *sval;	

	valid = 0;
	count = 0;
	memset(&info, 0 , sizeof(info));	
	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);			
		sval = websGetVar(wp, svar, NULL);		
		if(!sval) break;	
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = sys_time_get_config(&valid, sval,  cp, &info))){
			break;
		}
		if ((valid & ret) == ret){ // already get all need values
			break;
		}
		count ++;
	}

	if ((valid & ret) != ret){
		DBG_PRINTF("Invalid Form Time Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}

	pack = ipc_sys_time(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &info);
	IPC_RESULT(pack, result);
	if(pack) free(pack);
	is_timepage_post = 1; /*Add by huangmingjian 2013/9/06 for logout when setting time on web*/
	
	return 0;	

}

/*
<FORM name=OUT action=goform/command method=POST>
<INPUT type=hidden name=CMD value=CAB_PROP>
<INPUT type=hidden name=GO value=port_cable_prop.asp>
<input type=hidden name=SET0 value="anonymous=1">
<input type=hidden name=SET1 value="refresh=30">
<input type=hidden name=SET2 value="snid_cab0=1">
<input type=hidden name=SET3 value="snid_cab1=1">
</FORM>

*/
static int cable_prop_get_config(int *valid, char *sval, char *cp, ipc_clt_cable_param_t *req)
{
	int tmp, i;
	char cc[24];
	
#	define CP_ANONYMOUS  0x01
#	define CP_REFRESH    0x02
#	define CP_SNID		  0x04

# define VF_CAB_PROP  0x07 

	if (!(*valid & CP_ANONYMOUS) && !strcmp("anonymous", sval)){
		*valid |= CP_ANONYMOUS;
		tmp = (strtoul(cp, NULL, 0) > 0) ? 1 : 0;
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			req->clts[i].anonymous_control = tmp;	
		}
	}else if (!(*valid & CP_REFRESH) && !strcmp("refresh", sval)){
		*valid |= CP_REFRESH;
		tmp = strtoul(cp, NULL, 0);
		if (!valid_network_refresh_period(tmp)){
			DBG_ASSERT(0, "Invalid value for refresh interval");
			return 0;
		}
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			req->clts[i].network_refresh_period = tmp;	
		}
	}else if (!(*valid & CP_SNID) && !strstr("snid_cab", sval)){

		// parse the one to be set
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			if (req->clts[i].snid > 0) continue;
			
			sprintf(cc, "snid_cab%d", i);
			if (!strcmp(cc, sval)){
				tmp = strtoul(cp, NULL, 0);
				if (!valid_snid(tmp)){
					DBG_ASSERT(0, "Invalid value for snid");
					return 0;
				}
				req->clts[i].snid = tmp;
				break;
			}
		}
		tmp = 0;
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			if (req->clts[i].snid > 0){
				tmp ++;
			}
		}

		if (tmp == MAX_CLT_CHANNEL){
			*valid |= CP_SNID;
		}
	}
	
	return VF_CAB_PROP;
}


static int cable_prop_handle(webs_t wp, cgi_result_t *result)
{
	int ret, valid, count;
	ipc_clt_cable_param_ack_t *pack = NULL;
	ipc_clt_cable_param_t req;
	char *cp, svar[10], *sval;	

	valid = 0;
	count = 0;
	memset(&req, 0 , sizeof(req));
	
	// load values from web
	while(1) {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval) break;
	//	DBG_PRINTF("SET%d = %s\n", count, sval);			
	//	safe_strncpy(tmp, sval, sizeof(tmp));
		cp = strchr(sval, '=');
		if (!cp) break;
		*cp = 0; cp ++;
		if (!(ret = cable_prop_get_config(&valid, sval, cp, &req))){
			break;
		}
		if ((valid & ret) == ret){ // already get all need values
			break;
		}		
		count ++;
	}	


	if ((valid & ret) != ret){
		DBG_PRINTF("Invalid Form Cable Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}
	
	pack = ipc_clt_cable_param(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &req);
	IPC_RESULT(pack, result);	
	if(pack) free(pack);
	return 0;	
}
/*Begin added by feihuaxin for qos 2013-7-10*/
static int sys_Qosmode_get_config(int *valid, char *sval, char *cp, unsigned int *req)
{
    #define QOS_MODE  0x01
    #define CHK_FLAG  0x01

    if (!(*valid & QOS_MODE) && !strcmp("qosmode", sval))
    {
        *valid |= QOS_MODE;
        *req = strtoul(cp, NULL, 0);
        if ((QOS_COS != *req)&&(QOS_DSCP != *req))
        {
            DBG_ASSERT(0, "Invalid QOS mode");
            return 0;
        }
    }
	return CHK_FLAG;
}

static int sys_Qosmode_handle(webs_t wp, cgi_result_t *result)
{
	ipc_sys_qos_ack_t *pack = NULL;
    unsigned int  req = 0;
    int ret = 0;
    int valid = 0;
    int count = 0;
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  

	while(1) 
    {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval)
        {      
            break;
        }
		cp = strchr(sval, '=');
		if (!cp)
        {   
            break;
        }
		*cp = 0;
        cp ++;
		if (!(ret = sys_Qosmode_get_config(&valid, sval, cp, &req)))
        {
			break;
		}
		if ((valid & ret) == ret)
        {
			break;
		}		
		count ++;
	}	

	if ((valid & ret) != ret)
    {
		DBG_PRINTF("Invalid Form Qos Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);		
		return 0;
	}

	pack = ipc_qos_trustmode(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, req);
	IPC_RESULT(pack, result);
	if(pack)
    {
	    free(pack);
	}
	return 0;

}
static int sys_Qosweight_getconfig(int *valid, char *sval, char *cp, QueueMode_S *req)
{
    #define QOS_SCHEDULE  0x01
    #define QOS_WEIGHT1   0x02
    #define QOS_WEIGHT2   0x04
    #define QOS_WEIGHT3   0x08
    #define QOS_WEIGHT4   0x10   
    #define QOS_CHK       0x1f 

    if (!(*valid & QOS_SCHEDULE) && !strcmp("schedule", sval))
    {
        *valid |= QOS_SCHEDULE;
        req->QueueMode = strtoul(cp, NULL, 0);
        if ((QUEUE_MODE_WRR != req->QueueMode) && (QUEUE_MODE_HQ_WRR != req->QueueMode))
        {
            DBG_ASSERT(0, "Invalid queue mode");
            return 0;
        }
    }
    else if (!(*valid & QOS_WEIGHT1) && !strcmp("queweight1", sval))
    {
        *valid |= QOS_WEIGHT1;
        req->Weight[0] = strtoul(cp, NULL, 0);
        
    }
    else if (!(*valid & QOS_WEIGHT2) && !strcmp("queweight2", sval))
    {
        *valid |= QOS_WEIGHT2;
        req->Weight[1] = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & QOS_WEIGHT3) && !strcmp("queweight3", sval))
    {
        *valid |= QOS_WEIGHT3;
        req->Weight[2] = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & QOS_WEIGHT4) && !strcmp("queweight4", sval))
    {
        *valid |= QOS_WEIGHT4;
        req->Weight[3] = strtoul(cp, NULL, 0);
    }
    
    return QOS_CHK;

}

static int sys_Qosweight_handle(webs_t wp, cgi_result_t *result)
{
	ipc_sys_qosqueuemod_ack_t *pack = NULL;
    QueueMode_S req;
    int ret = 0;
    int valid = 0;
    int count = 0;
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    memset(&req, 0, sizeof(req));

	while(1) 
    {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval)
        {      
            break;
        }
		cp = strchr(sval, '=');
		if (!cp)
        {   
            break;
        }
		*cp = 0;
        cp ++;
		if (!(ret = sys_Qosweight_getconfig(&valid, sval, cp, &req)))
        {
			break;
		}
		if ((valid & ret) == ret)
        {
			break;
		}		
		count ++;
	}	

	if ((valid & ret) != ret)
    {
		DBG_PRINTF("Invalid Form QosWeight Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);	
		return 0;
	}

	pack = ipc_qos_queuemode(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &req);
	IPC_RESULT(pack, result);
	if(pack)
    {
	    free(pack);
	}
	return 0;


}
/*End added by feihuaxin for qos 2013-7-10*/

static int sys_mac_dis_handle(webs_t wp, cgi_result_t *result)
{
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    int mac_idx = 0;

    sprintf(svar, "SET%d", 0);
    sval = websGetVar(wp, svar, NULL);
    if(!sval)
    {      
		DBG_PRINTF("get post of mac display error\n");
        return 0;
    }
    cp = strchr(sval, '=');
    if (!cp)
    {   
		DBG_PRINTF("Invalid Form content of mac display\n");
        return 0;
    }
    *cp = 0;
    cp ++;

    if(!strcmp("req_mac_idx", sval))
    {
        mac_idx = strtoul(cp, NULL, 0);
    }
    
    Mac_firstidx = mac_idx;

	return 0;	

}

static int sys_mac_port_getconfig(int *valid, char *sval, char *cp, int *mac_port, int *mac_idx)
{
#define MAC_PORT  0x01
#define MAC_IDX   0x02
#define MAC_CHK   0x03

    if (!(*valid & MAC_PORT) && !strcmp("portselect", sval))
    {
        *valid |= MAC_PORT;
        *mac_port = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & MAC_IDX) && !strcmp("req_mac_idx", sval))
    {
        *valid |= MAC_IDX;
        *mac_idx = strtoul(cp, NULL, 0);
        
    }
    
    return MAC_CHK;
}

static int sys_mac_port_dis_handle(webs_t wp, cgi_result_t *result)
{
#if 0
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    int mac_idx = 0;
    printf("sys_mac_port_dis_handle\n");

    sprintf(svar, "SET%d", 0);
    sval = websGetVar(wp, svar, NULL);
    if(!sval)
    {      
		DBG_PRINTF("get post of mac port display error\n");
        return 0;
    }
    cp = strchr(sval, '=');
    if (!cp)
    {   
		DBG_PRINTF("Invalid Form content of mac port display\n");
        return 0;
    }
    *cp = 0;
    cp ++;

    if(!strcmp("req_mac_idx", sval))
    {
        mac_idx = strtoul(cp, NULL, 0);
    }
    printf("Mac_port_firstidx:%d\n",Mac_port_firstidx);
    Mac_port_firstidx = mac_idx;

	return 0;	
#endif

    int ret = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    int mac_port = 0;
    int mac_idx = 0;

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_mac_port_getconfig(&valid, sval, cp, &mac_port, &mac_idx)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count ++;
    }   

    if ((valid & ret) != ret)
    {
        DBG_PRINTF("Invalid Form MAC_PORT_DIS Command:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

    Mac_port = mac_port; 
    Mac_port_firstidx = mac_idx;

    return 0;
}


static int sys_mac_port_handle(webs_t wp, cgi_result_t *result)
{
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    int mac_port = 0;

    sprintf(svar, "SET%d", 0);
    sval = websGetVar(wp, svar, NULL);
    if(!sval)
    {      
		DBG_PRINTF("get post of mac port num error\n");
        return 0;
    }
    cp = strchr(sval, '=');
    if (!cp)
    {   
		DBG_PRINTF("Invalid Form content of mac port num\n");
        return 0;
    }
    *cp = 0;
    cp ++;

    if(!strcmp("portselect", sval))
    {
        mac_port = strtoul(cp, NULL, 0);
    }

    Mac_port = mac_port;

	return 0;	

}



/*****************************************************************
  Function:        sys_mac_setting_getconfig
  Description:     sys_mac_setting_getconfig
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int sys_mac_setting_getconfig(int *valid, char *sval, char *cp, mac_setting_t *req)
{
	#define AGING_TIME_EN   0x01
	#define AGING_TIME    	0x02
	#define ALL_MAST        (AGING_TIME | AGING_TIME) 
	int en = 0;


	if (!(*valid & AGING_TIME_EN) && !strcmp("agint_time_en", sval))
    {
        *valid |= AGING_TIME_EN;
		en = strtoul(cp, NULL, 0);
		if(!en)
		{	
			req->aging_time = 0;
			*valid |= AGING_TIME;
			return ALL_MAST;
		}
    }
	else if (!(*valid & AGING_TIME) && !strcmp("agint_time", sval))
    {
        *valid |= AGING_TIME;
		req->aging_time = strtoul(cp, NULL, 0);
    }
    
    return ALL_MAST;
}


/*****************************************************************
  Function:        sys_mac_setting_handle
  Description:     sys_mac_setting_handle
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int sys_mac_setting_handle(webs_t wp, cgi_result_t *result)
{
    ipc_mac_setting_ack_t *pack = NULL;
    mac_setting_t info;
    int ret = 0;
    int i = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  

	memset(&info, 0, sizeof(info));

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
		DBG_PRINTF("SET%d = %s", count, sval);			
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_mac_setting_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count ++;
    }   

    if ((valid & ret) != ret)
    {
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }
	
    pack = ipc_mac_setting_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW , &info);;
	IPC_RESULT(pack, result);
    if(pack)
    {
    	free(pack);
    }

	return 1;	

}

static int sys_portset_getconfig(int *valid, char *sval, char *cp, PORT_CFG_VALUE_S *req, int *portmsk)
{
#define PORT_MASK    0x01
#define PORT_DES     0x02
#define PORT_SPEED   0x04
#define PORT_DUX     0x08
#define PORT_ADMIN   0x10   
#define PORT_FC      0x20   
#define PORT_PRI     0x40
#define PORT_MIDX    0x80
#define PORT_ISOLATE 0x100 

#define PORT_CHK      0x1ff 

    if (!(*valid & PORT_MASK) && !strcmp("portmask", sval))
    {
        *valid |= PORT_MASK;
        *portmsk = strtoul(cp, NULL, 0);
        if(*portmsk == 0)
        {
            DBG_ASSERT(0, "Invalid portmask");
            return 0;
        }
    }
    else if (!(*valid & PORT_DES) && !strcmp("description", sval))
    {
        *valid |= PORT_DES;
        safe_strncpy(req->stIfDesc, cp, sizeof(req->stIfDesc)); //?
    }
    else if (!(*valid & PORT_SPEED) && !strcmp("speed", sval))
    {
        *valid |= PORT_SPEED;
        req->stSpeed = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_DUX) && !strcmp("duplex", sval))
    {
        *valid |= PORT_DUX;
        req->stDuplex = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_ADMIN) && !strcmp("admin", sval))
    {
        *valid |= PORT_ADMIN;
        req->ulIfEnable = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_FC) && !strcmp("fc", sval))
    {
        *valid |= PORT_FC;
        req->ulFlowCtl = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_PRI) && !strcmp("priority", sval))
    {
        *valid |= PORT_PRI;
        req->ulPriority = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_MIDX) && !strcmp("MDIX", sval))
    {
        *valid |= PORT_MIDX;
        req->ulMdix = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & PORT_ISOLATE) && !strcmp("isolate", sval))
    {
        *valid |= PORT_ISOLATE;
        req->ulIsolateEn = strtoul(cp, NULL, 0);
    }
    
    return PORT_CHK;
}
int sys_getPortCurCfg(PORT_CFG_VALUE_S * port_cur)
{
	int ret = 0;
    int arrayidx = 0;
	ipc_port_ack_t *pack = NULL;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
    
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
        return 0;
    }
    
    for(arrayidx = 0; arrayidx < LOGIC_PORT_NO; arrayidx++)
    {
        safe_strncpy(port_cur[arrayidx].stIfDesc, port_cfg_val[arrayidx].stIfDesc, sizeof(port_cur[arrayidx].stIfDesc));
        port_cur[arrayidx].stSpeed = port_cfg_val[arrayidx].stSpeed;
        port_cur[arrayidx].stDuplex = port_cfg_val[arrayidx].stDuplex;
        port_cur[arrayidx].ulIfEnable = port_cfg_val[arrayidx].ulIfEnable;
        port_cur[arrayidx].ulFlowCtl = port_cfg_val[arrayidx].ulFlowCtl;
        port_cur[arrayidx].ulMdix = port_cfg_val[arrayidx].ulMdix;
        port_cur[arrayidx].ulIsolateEn = port_cfg_val[arrayidx].ulIsolateEn;
        port_cur[arrayidx].ulPriority  = port_cfg_val[arrayidx].ulPriority;
    }

    return 1;

}
static int sys_port_handle(webs_t wp, cgi_result_t *result)
{
    ipc_port_ack_t *pack = NULL;
    PORT_CFG_VALUE_S req;
    int portmsk = 0;
    int ret = 0;
    int i = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    PORT_CFG_VALUE_S port_cur[LOGIC_PORT_NO] = {0};

    memset(&req, 0, sizeof(req));

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_portset_getconfig(&valid, sval, cp, &req, &portmsk)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count ++;
    }   

    if ((valid & ret) != ret)
    {
        DBG_PRINTF("Invalid Form portset Command:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

    /*handle the port cfg according the port mask*/
    if(1 != sys_getPortCurCfg(port_cur))
    {
        DBG_PRINTF("Get port current cfg error.\n");
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

    sleep(1);
    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(portmsk & (1<<i))
        {
            memcpy(&(port_cur[i]), &req, sizeof(PORT_CFG_VALUE_S));
        }
    }

    pack = ipc_port_req(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &port_cur);
    IPC_RESULT(pack, result);
    if(pack)
    {
        free(pack);
    }

    return 0;
}

static int sys_portmsk_handle(webs_t wp, cgi_result_t *result)
{
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    int portmsk = 0;

    sprintf(svar, "SET%d", 0);
    sval = websGetVar(wp, svar, NULL);
    if(!sval)
    {      
		DBG_PRINTF("get post of port mask error\n");
        return 0;
    }
    cp = strchr(sval, '=');
    if (!cp)
    {   
		DBG_PRINTF("Invalid Form content of port mask num\n");
        return 0;
    }
    *cp = 0;
    cp ++;

    if(!strcmp("portmask", sval))
    {
        portmsk = strtoul(cp, NULL, 0);
    }

    port_msk = portmsk;

	return 0;
}

extern void websAuthAddUser(char *name, char *pwd, uint32_t ipaddr, uint32_t timeout);
extern void websAuthDelUser(uint32_t ipaddr);

/*Begin modify by jiangzhenhua for EPN104QID0044  2012/9/24*/
enum LOGIN_ERR{ERR_NONE,ERR_NAME,ERR_UNKNOW};
int log_msg=ERR_NONE;
int websLoginSubmit(webs_t wp)
{
	char *usr=NULL, *pwd=NULL;
	struct in_addr in;
	sys_admin_t info;
	ipc_sys_admin_ack_t *pack;
	int i, valid = 0;

	log_msg=ERR_NONE;
	usr = websGetVar(wp, T("uname"), NULL);
	pwd = websGetVar(wp, T("pws"), NULL);
	//DBG_PRINTF("'%s','%s'",usr,pwd);
	if (!strcmp(usr, "") || !strcmp(pwd, "")){
		//websRedirect(wp, "prelogin.asp");
		log_msg = ERR_NAME;
    
        websRedirect(wp, "login.asp");

		return 0;
	}
	
	pack = ipc_sys_admin(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		memcpy(&info, &pack->admin, sizeof(info));
		valid = 1;
	}
	if (pack) free(pack);

	if (!valid){
		//websRedirect(wp, "prelogin.asp");
		log_msg = ERR_UNKNOW;
        
        websRedirect(wp, "login.asp");

		return 0;
	}		

	for (i = 0; i < info.count; i ++){
		if (!info.users[i].enable){
			continue;
		}
		if (usr && !strcmp(usr, info.users[i].name)){
			if (pwd && !strcmp(pwd, info.users[i].pwd)){
				inet_aton(wp->ipaddr, &in);
				websAuthAddUser(usr, pwd, in.s_addr, info.idle_time);
				websRedirect(wp, "index.asp");
				USER_LOG(LangId,WEB,LOG_NOTICE,M_LOGIN,usr,wp->ipaddr);  //add by leijinbao 2013/09/17 for login log
				return 1;
			}
		}			
	}
	websAuthDelUser(in.s_addr);
	log_msg = ERR_NAME;
	USER_LOG(LangId,WEB,LOG_ERR,M_USERNAMER,"","");  //add by leijinbao 2013/09/17 for login log
    websRedirect(wp, "login.asp");
	return 0;
}
/*End modify by jiangzhenhua for EPN104QID0044  2012/9/24*/

extern int sys_vlanmode_handle(webs_t wp, cgi_result_t *result);
extern int vlan_prop_handle(webs_t wp, cgi_result_t *result);
extern int vlanif_prop_handle(webs_t wp, cgi_result_t *result);
extern int access_vlan_handle(webs_t wp, cgi_result_t *result);
extern int sys_stormrate_handle(webs_t wp, cgi_result_t *result);
extern 	int sys_portmib_handle(webs_t wp, cgi_result_t *result);
extern int sys_linerate_handle(webs_t wp, cgi_result_t *result);
extern int sys_mirrorset_handle(webs_t wp, cgi_result_t *result);
#ifdef CONFIG_PRODUCT_5500
extern int sys_pon_count_reset_handle(webs_t wp, cgi_result_t *result);
#endif
#ifdef CONFIG_CATVCOM	
extern int asp_set_catv(webs_t wp, cgi_result_t *result);
#endif									
#ifdef CONFIG_WLAN_AP	
extern int asp_set_wlan_basic(webs_t wp, cgi_result_t *result);
extern int asp_set_wlan_security(webs_t wp, cgi_result_t *result);
extern int asp_set_wlan_advance(webs_t wp, cgi_result_t *result);
extern int asp_set_wlan_access_ctr(webs_t wp, cgi_result_t *result);
#endif



struct cgi_handle_t cgi_handle[] = {
//	{"TMPL_PROP",	tmpl_prop_handle},
//	{"TMPL_NEW",	tmpl_prop_handle},
//	{"TMPL_DEL",	tmpl_prop_handle},
//	{"USER_PROP",	user_prop_handle},
//	{"USER_NEW",	user_prop_handle},
//	{"USER_DEL",	user_prop_handle},
//	{"VLAN_NEW",	vlan_prop_handle},
//	{"VLAN_DEL",	vlan_prop_handle},
//	{"VLANIF",		vlanif_prop_handle},
//	{"VLAN_MODE", 	vlan_mode_prop_handle},
	{"CAB_PROP",	cable_prop_handle},
	{"SYS_MANIP", 	sys_ip_prop_handle},
	{"SYS_MVLAN", 	sys_mvlan_handle},   /*Add by huangmingjian 2012/11/13 for EPN104QID0082*/ 
#ifdef CONFIG_LOOPD    	
	{"SYS_LOOPBACK",	sys_loopback_handle},	 /*Add by huangmingjian 2012/11/27 for EPN104QID0083*/ 
#endif	
	{"SYS_MANSEC", 	sys_security_prop_handle},
	{"SNMP_BASIC",		sys_snmp_prop_handle},
	{"SNMP_GROUP",   sys_snmp_group_handle},
 	{"SNMP_TRAP",	sys_snmp_trap_prop_handle},
 	{"SNMP_USER",	sys_snmp_user_prop_handle},	
//	{"SNMP_CATVINFO",   sys_snmp_catvinfo_handle},
	{"SYS_ADMIN", 	sys_admin_prop_handle },
//	{"SYS_ETH", 	sys_ethernet_prop_handle},
//	{"IF_MIBS", 	sys_interface_mibs_prop_handle},
	{"SYS_TIME",	sys_time_handle},
	{"CONFIG",		sys_config_handle},
	{"SYSLOG",		sys_syslog_handle},
	{"SET_DEBUG",   sys_setdebug_handle},//Add by zhouguanhua 2013/5/15 for bug id 49
	{"SYS_QOSMODE", sys_Qosmode_handle},
	{"SYS_QOSWEIGHT", sys_Qosweight_handle},
	{"MAC_DIS", sys_mac_dis_handle},
	{"MAC_PORT_DIS", sys_mac_port_dis_handle},
	{"MAC_PORT", sys_mac_port_handle},
	{"MAC_SETTING", sys_mac_setting_handle},
	{"PORT_SET", sys_port_handle},
	{"PORT_MASK", sys_portmsk_handle},
	{"VLAN_MODE",sys_vlanmode_handle},
	{"VLAN_NEW",	vlan_prop_handle},
	{"VLAN_DEL",	vlan_prop_handle},
	{"VLANIF",vlanif_prop_handle},
#ifdef CONFIG_PRODUCT_5500
	{"VLAN_ACCESS",access_vlan_handle},
#endif	
	{"STORM_RATE",sys_stormrate_handle},
	{"IF_MIBS",sys_portmib_handle},
	{"LINE_RATE",sys_linerate_handle},
	{"PORT_MIRROR",sys_mirrorset_handle},
#ifdef CONFIG_PRODUCT_5500
	{"PON_COUNT_RESET",sys_pon_count_reset_handle},
#endif
#ifdef CONFIG_CATVCOM	
	{"CATVINFO", asp_set_catv},
#endif	
#ifdef CONFIG_WLAN_AP	
	{"WLAN_AP_BASIC", asp_set_wlan_basic},
	{"SSID_LIST", asp_set_wlan_security},
	{"WLAN_AP_ADVANCE", asp_set_wlan_advance},
	{"MAC_FILTER", asp_set_wlan_access_ctr},
#endif
	{NULL,				NULL}
};

/**/
extern userInfo_t websUser[MAX_USER_NUM];
void form_command(webs_t wp, char_t *path, char_t *query)
{
	int i, got = 0;
	char *cmd, *go, goto_url[128];
	cgi_result_t result;

	result.msg = MSG_UNKNOWN;
	result.delay = 0;
	result.nowait = 0;
	result.error = CGI_OK;

	go = websGetVar(wp, T("GO"), "index.asp");
	cmd = websGetVar(wp, T("CMD"), NULL);
	
	if(!cmd) {
		DBG_PRINTF("Invalid command, URL:%s", wp->url);
		result.msg = MSG_FAIL;
		result.error = CGI_NOCMD;
		goto do_cmd;
	}

	if (cmd && !strcmp(cmd, "LOGIN")){
		websLoginSubmit(wp);
		return ;
	}
	
	
	/*Begin add by huangmingjian 2013/9/06 for logout when setting time on web*/
	time_t t = time(NULL);
	struct in_addr in;
	inet_aton(wp->ipaddr, &in);
	uint32_t host = in.s_addr;

	for(i = 0; i < MAX_USER_NUM; i ++)
	{
		if(0==i)
		{
			/*no user logins but can post , this happan when reboot system on cli and post on web, so dont call any handle*/
			if((0 == websUser[i].logout_time) && (0==websUser[i].ip_addr)) 
			{
				USER_LOG(LangId,WEB,LOG_NOTICE,M_LOGOUT,"",wp->ipaddr); //add by leijinbao 2013/11/21 for 281
				goto do_cmd;
			}
		}
		
		/*someone logins but post on web when timeout, so dont call any handle*/
		if ((websUser[i].ip_addr == host) && (websUser[i].logout_time < t))
		{
			USER_LOG(LangId,WEB,LOG_NOTICE,M_LOGOUT,"",wp->ipaddr);  // //add by leijinbao 2013/11/21 for 281
			goto do_cmd;
		}
	}
	/*Begin add by huangmingjian 2013/9/06 for logout when setting time on web*/
				

	for(i = 0; cgi_handle[i].cmd; i ++) {
		if(!strcmp(cmd, cgi_handle[i].cmd)) {
			got = 1;
			if(cgi_handle[i].request_handle){
				cgi_handle[i].request_handle(wp, &result);
			}
			break;
		}
	}

	if (got == 0){
		DBG_PRINTF("Unsupported CMD[%s]", cmd);
		result.msg = MSG_FAIL;
		result.error = CGI_UNKNOWN_CMD;
		goto do_cmd;
	}
	
	if (result.nowait == 1){
		websRedirect(wp, go);
		return;
	}

do_cmd:	
	if (!strcmp(result.msg, MSG_FAIL)){
        go = "index.asp";   
	}

    sprintf(goto_url, "/do_cmd.asp?DELAY=%d&ERR=%d&MSG=%s&GO=%s", result.delay, result.error, result.msg, go);
	websRedirect(wp, goto_url);
}




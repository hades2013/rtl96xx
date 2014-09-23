/*
 * aspfunc.c
 *
 *  Created on: Jan 10, 2011
 *      Author: root
 */

#include "uemf.h"
#include "wsIntrn.h"
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <debug.h>
#include <config.h>
#include <syslog.h>

#include <cable.h>
#include <ipc_client.h>
//#include	<switch.h>
#include <hex_utils.h>
//#include <vlan_utils.h>
#include <str_utils.h>
#include "webs_nvram.h"
#include "asp_variable.h"
#include "pdt_config.h"
#include <ipc_protocol.h>
#include "memshare.h"
#include <lw_config_oid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "shutils.h"
#include <time_zone.h>
#include <lw_config_api.h>
#include "lw_drv_req.h"
/*add by leijinbao 2013/12/19 for mac*/
#include 	"cipher.h"
/*end by leijinbao 2013/12/19 for mac*/


#define	DEFAULT_NV_USER			";0;0;acc;;;"
#define	DEFAULT_NV_TMPL_DESC	"0;0;;"
#define	DEFAULT_NV_TMPL_PORT	"'0;100;full;0;off;0;0;on;1;ena',\n\
'1;auto;auto;0;off;0;0;off;1;ena',\n\
'2;auto;auto;0;off;0;0;off;1;ena',\n\
'3;auto;auto;0;off;0;0;off;1;ena',\n\
'4;auto;auto;0;off;0;0;off;1;ena'"

#define	DEFAULT_NV_TMPL_MISC	"0;0;off;0"

//#define WEBS_CONFIG_FILE 		"config.bin"
#define WEBS_SYSLOG_FILE 		"syslog.txt"


extern int ipc_fd;



static const char *cnu_auth_string(int auth)
{
	static const char *str[] = {"any", "any", "acc", "blk"};

	if (auth < sizeof(str)/sizeof(char *)){
		return str[auth];
	}
	return str[0];
}


char *websQueryString(webs_t wp, char *varname, char *val, int size)
{
	char *cp = strstr(wp->query, varname), *ep;
	if(!cp) return NULL;
	while(*cp && *cp != '=') cp ++;
	if(*cp) {
		cp ++;
		strncpy(val, cp, size - 1);
		ep = val;
		while(*ep && *ep != '&') ep ++;
		*ep = 0;
		return val;
	}
	else
		return NULL;
}
int get_if_mac(char *ifname, ethernet_addr_t *mac)
{
	int sock, ret;
	struct ifreq ifr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	DBG_ASSERT(sock >= 0, "create socket: %s", strerror(errno));
	if(sock < 0) return -1;
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	DBG_ASSERT(ret >= 0, "ioctl: %s", strerror(errno));
	if(ret < 0) return -1;
	memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(ethernet_addr_t));
	close(sock);
	return 0;
}
#if 0 //zhouguanhua
static char *vlan_member_string(uint32_t member)
{
	static char buf[80];

	buf[0] = 0;
	if(member & (1 << CLT0_PORT_NUMBER)) strcat(buf, "cab0,");
	if(member & (1 << CLT1_PORT_NUMBER)) strcat(buf, "cab1,");
	if(member & (1 << ETH0_PORT_NUMBER)) strcat(buf, "eth0,");
	if(member & (1 << ETH1_PORT_NUMBER)) strcat(buf, "eth1,");

	// delete the last ','
	if (strlen(buf)){
		buf[strlen(buf) - 1] = 0;	
	}
	return buf;
}


/*
char ifname[IFNAMSIZ];
uint8_t phy;
uint8_t enabled;
uint8_t autoneg;
uint8_t speed;
uint8_t duplex;
uint8_t flowctrl;
uint8_t priority;
uint32_t ingress_limit;
uint32_t egress_limit;
uint8_t ingress_filter;
uint16_t pvid;// read only

eth0;1;0;1;1000;full;1;off;0;0;0;4094

*/

static int asp_get_ethernet_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i, vlen = 0, count;
	ipc_clt_interface_config_ack_t *pack = NULL;
	switch_interface_t *ifp, *ifps;
	char buffer[512];
	char *ifname[] = {"eth0", "eth1"};

	count = sizeof(ifname)/sizeof(char *);

	ifps = (switch_interface_t *)malloc(sizeof(switch_interface_t) * count);
	if (ifps == NULL){
		DBG_ASSERT(0, "Malloc failed");
		return 0;
	}
	for (i = 0; i < count; i ++){
		ifp = &ifps[i];
		safe_strncpy(ifp->ifname, ifname[i], sizeof(ifp->ifname));
	}

	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, ifps, count);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		for(i = 0; i < pack->count; i ++) {
			ifp = &pack->ifs[i];
			vlen = sprintf(buffer, "%s;%d;%d;%d", ifp->ifname, ifp->phy, ifp->enabled, ifp->autoneg);
			vlen += sprintf(buffer + vlen, ";%s;%s;%d", speed_itoa(ifp->speed), ifp->duplex ? "full" : "half", ifp->flowctrl);
			if (ifp->priority == ETH_PRIORITY_OFF){
				vlen += sprintf(buffer + vlen, ";%s", "off");
			}else {
				vlen += sprintf(buffer + vlen, ";%d", ifp->priority);
			}			
			vlen += sprintf(buffer + vlen, ";%u;%u;%d;%d", ifp->ingress_limit, ifp->egress_limit, ifp->ingress_filter, ifp->pvid);
			wlen += websWrite(wp, T("'%s'%s"),buffer, (i == pack->count - 1) ? "" : ",\n");
		}
	}
	if (ifps) free(ifps);
	if(pack) free(pack);

	return wlen;
}


static int asp_get_vlan_interface_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i, v, vlen = 0;
	ipc_clt_vlan_interface_config_ack_t *pack = NULL;
	switch_vlan_interface_t* vlanif;
	switch_vlan_group_t *vlan_group;
//	char vs[24];
	char buffer[128];
	char tagbuf[4096*4];
	char untagbuf[4096*4];

	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		vlanif = pack->vlanifs;
		for(i = 0; i < pack->count; i ++) {
			/* we will print out something like that
			'eth0;access;1;;',
			'eth1;trunk;1;;',
			'cab0;trunk;1;201,202,203;',
			'cab1;hybrid;1;303,304;123,85' ; 
			// for hybrid , the last second part is always for tag vlans.
			// and the last part is for untag vlans
			*/
			vlen = sprintf(buffer, "%s;%s;%d;", vlanif->ifname,  switchport_itoa(vlanif->mode), vlanif->pvid);

			// get buffer tag
			tagbuf[0] = 0;
			untagbuf[0] = 0;

			if (vlanif->count && (vlanif->mode == ETH_SWITCHPORT_TRUNK)){
				vlan_group = vlanif->trunk_hybrid;
				vlan_group_sprintf(tagbuf, vlan_group);
			}else if (vlanif->count && (vlanif->mode == ETH_SWITCHPORT_HYBRID)){
				vlan_group = vlanif->trunk_hybrid;
				for (v = 0; v < vlanif->count; v ++){
					if (vlan_tagged(vlan_group->flag)){
						vlan_group_sprintf(tagbuf, vlan_group);
					}else {
						vlan_group_sprintf(untagbuf, vlan_group);
					}
					vlan_group = vlan_group_to_next(vlan_group);
				}
			}
			
			//buffer[vlen] = 0;
			wlen += websWrite(wp, T("'%s%s;%s'%s"),buffer, tagbuf, untagbuf, (i == pack->count - 1) ? "" : ",\n");
			vlanif = vlan_interface_to_next(vlanif);
		}
	}
	if(pack) free(pack);
	return wlen;
}



static int asp_get_vlan_mode_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i,vlen = 0;
	ipc_clt_vlan_mode_ack_t *pack = NULL;
	char buffer[512];

	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {

		vlen = sprintf(buffer, "'%d','%d'", pack->vlan_mode.mode, pack->vlan_mode.mvlan);
		
		for(i = 0; i < EOC_MAX_PORT_NUMS; i ++) {
			if (pack->vlan_mode.ports[i].ifname[0]){
				vlen += sprintf(buffer + vlen, ",'%s;%s'", 
					pack->vlan_mode.ports[i].ifname, switchport_itoa(pack->vlan_mode.ports[i].type));
			}else {
				break;
			}
		}
		//DBG_PRINTF("vmstring:%s", buffer);		
		wlen = websWrite(wp, T("%s"),buffer);
	}
	if(pack) free(pack);
	return wlen;
}



static int asp_get_vlan_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i, vlen = 0;
	ipc_clt_vlan_config_ack_t *pack = NULL;
	switch_vlan_group_t *vlan_group;
//	char vs[24];
	char buffer[4096*4];

	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		vlan_group = &pack->vlan_groups[0];
		for(i = 0; i < pack->count; i ++) {
			vlen = sprintf(buffer, "%d;%s;", i, vlan_member_string(vlan_group->member));
			vlen += vlan_group_sprintf(buffer + vlen, vlan_group);
			//buffer[vlen] = 0;
			wlen += websWrite(wp, T("'%s'%s"),buffer, (i == pack->count - 1) ? "" : ",\n");
			vlan_group = vlan_group_to_next(vlan_group);
		}
	}
	if(pack) free(pack);
	return wlen;
}

static int asp_get_user_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i;
	ipc_cnu_user_config_ack_t *pack = NULL;
	cnu_user_config_t *user;
	char macstr[64];

	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_NONE, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		user = pack->user;
		for(i = 0; i < pack->count; i ++, user ++) {
			if(user->valid == USER_DELETED) continue;
			// user_0 = XX:XX:XX:XX:XX:XX;TMP_ID;DEV_ID;ACCESS;NAME;DESC
			hexstring(macstr, sizeof(macstr), user->mac.octet, sizeof(ethernet_addr_t));
			wlen += websWrite(wp, T("'%s;%d;%d;%s;%s;%s'%s"),
					macstr,
					user->tmpl_id,
					user->device_id,
					user->auth == CNU_AUTH_ACCEPT ? "acc" : "blk",
					user->name,
					user->desc,
					(i == pack->count - 1) ? "" : ",\n");
		}
	}
	if(pack) free(pack);
	return wlen;
}
#endif //add by zhouguanhua

static char *port_map_string(char *str, uint8_t *map, int ports)
{
	int i, len = 0;
	for(i = 0; i < ports; i ++) {
		len += sprintf(str + len, "%d", map[i]);
		if(i < ports - 1) len += sprintf(str + len, ",");
	}
	return str;
}

static int asp_get_devinfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i;
	ipc_supported_device_ack_t *pack = NULL;
	char pm[20];

	pack = ipc_supported_device(ipc_fd);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		for(i = 0; i < pack->count; i ++) {
			port_map_string(pm, pack->devinfo[i].port_map, pack->devinfo[i].ports);
			wlen += websWrite(wp, T("'%s;%d;%d;%s'%s"),
					pack->devinfo[i].alias,
					pack->devinfo[i].ports,
					pack->devinfo[i].switch_chip_id,
					pm,
					(i == pack->count - 1) ? "" : ",\n");
		}
	}
	if(pack) free(pack);
	return wlen;
}

#if 0 //add by zhouguanhua
static int asp_get_templates(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i;
	ipc_service_template_ack_t *pack = NULL;
	ipc_service_template_t tmpl;
	char query_id[32];
	char *port_speeds[] = {"10", "100", "1000"};

	if(argc < 1) {
		DBG_ASSERT(0, "Invalid asp argument");
		return 0;
	}
	else {
		if(!strcasecmp(argv[0], "brief")) {
			pack = ipc_service_template(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
			if(pack && pack->hdr.status == IPC_STATUS_OK) {
				for(i = 0; i < pack->count; i ++)
					wlen += websWrite(wp, T("'%d;%d;%s;%s'%s"),
							pack->templates[i].tmpl_id,
							pack->templates[i].device_id,
							pack->templates[i].tmpl_name,
							pack->templates[i].tmpl_desc,
							(i == pack->count - 1) ? "" : ",\n");
			}
			if (pack) free(pack);
			pack = NULL;
		}
		else {
			if(argc < 2) {
				DBG_ASSERT(0, "Invalid asp argument");
				return 0;
			}
			else {
				if(websQueryString(wp, argv[1], query_id, 32)) {
					tmpl.tmpl_id = strtoul(query_id, NULL, 0);
					pack = ipc_service_template(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, &tmpl, 1);
					if(pack && pack->hdr.status == IPC_STATUS_OK && pack->count == 1) {
						wlen += websWrite(wp, T("var template_desc='%d;%d;%s;%s';\n"),
								pack->templates[0].tmpl_id,
								pack->templates[0].device_id,
								pack->templates[0].tmpl_name,
								pack->templates[0].tmpl_desc);
						wlen += websWrite(wp, T("var template_misc='%d;%d;%s;%d';\n"),
								pack->templates[0].cable_rate_up,
								pack->templates[0].cable_rate_dn,
								pack->templates[0].lsw_cfg.broadcast_storm ? "on" : "off",
								pack->templates[0].mac_limit);
						wlen += websWrite(wp, T("var template_port=Array(\n"));
						wlen += websWrite(wp, T("'0;%s;%s;%d;%s;%d;%d;%s;%d;%s',\n"),
								pack->templates[0].lsw_cfg.cab_port.autoneg ? "auto" :
										(pack->templates[0].lsw_cfg.cab_port.speed <= 2 ? port_speeds[pack->templates[0].lsw_cfg.cab_port.speed] : "100"),
								pack->templates[0].lsw_cfg.cab_port.autoneg ? "auto" :
										(pack->templates[0].lsw_cfg.cab_port.duplex ? "full" : "half"),
								pack->templates[0].lsw_cfg.cab_port.priority,
								pack->templates[0].lsw_cfg.cab_port.flowctrl ? "on" : "off",
								pack->templates[0].lsw_cfg.cab_port.ingress_rate_limit,
								pack->templates[0].lsw_cfg.cab_port.egress_rate_limit,
								pack->templates[0].lsw_cfg.cab_port.tagged ? "on" : "off",
								pack->templates[0].lsw_cfg.cab_port.pvid,
								pack->templates[0].lsw_cfg.cab_port.disabled ? "dis" : "ena"
								);
						for(i = 0; i < pack->templates[0].lsw_cfg.ports - 1; i ++) {
							wlen += websWrite(wp, T("'%d;%s;%s;%d;%s;%d;%d;%s;%d;%s'%s"),
									i + 1,
									pack->templates[0].lsw_cfg.eth_ports[i].autoneg ? "auto" :
											(pack->templates[0].lsw_cfg.eth_ports[i].speed <= 2 ? port_speeds[pack->templates[0].lsw_cfg.eth_ports[i].speed] : "100"),
									pack->templates[0].lsw_cfg.eth_ports[i].autoneg ? "auto" :
											(pack->templates[0].lsw_cfg.eth_ports[i].duplex ? "full" : "half"),
									pack->templates[0].lsw_cfg.eth_ports[i].priority,
									pack->templates[0].lsw_cfg.eth_ports[i].flowctrl ? "on" : "off",
									pack->templates[0].lsw_cfg.eth_ports[i].ingress_rate_limit,
									pack->templates[0].lsw_cfg.eth_ports[i].egress_rate_limit,
									pack->templates[0].lsw_cfg.eth_ports[i].tagged ? "on" : "off",
									pack->templates[0].lsw_cfg.eth_ports[i].pvid,
									pack->templates[0].lsw_cfg.eth_ports[i].disabled ? "dis" : "ena",
									(i == pack->templates[0].lsw_cfg.ports - 2) ? "" : ",\n");
						}
						wlen += websWrite(wp, T(");\n"));
					}
					if (pack) free(pack);					
					pack = NULL;
				}
				else {
					// Creating new template
					wlen += websWrite(wp, T("var template_desc = '%s';\n"), DEFAULT_NV_TMPL_DESC);
					wlen += websWrite(wp, T("var template_misc = '%s';\n"), DEFAULT_NV_TMPL_MISC);
					wlen += websWrite(wp, T("var template_port = Array(%s);\n"), DEFAULT_NV_TMPL_PORT);
				}
			}
		}
	}
	return wlen;
}
#endif //add by zhouguanhua

/*begin add by leijinbao for EPN104NQID275*/
#if !defined(CONFIG_PRODUCT_5500)
static int asp_get_syslog(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[320];
	int wlen = 0;
	int ret= 0;
	char *ptr = NULL;
	char *str = NULL;

	/*add by zhouguanhua 2013/6/7*/
	if (access(SYSLOG_FILE_PATH, F_OK) != 0)
	{
		return 0;	 
	}
	/*end add by zhouguanhua 2013/6/7*/

	fp = fopen(SYSLOG_FILE_PATH, "r");
	if(fp) 
	{
		fgets(line, sizeof(line), fp);
		while(!feof(fp)) 
		{
			ptr = strchr(line,'	');
			if(ptr)
			{
				str = strchr(ptr,':');
				*str++ = ';';
				replace_char(line, '	', ';');
				replace_char(line, 0xa, '.');	   
				wlen += websWrite(wp, T("'%s'"), line);
			}
			memset(line, 0, sizeof(line));
			fgets(line, sizeof(line), fp);
			if(ptr && !feof(fp)) 
			{	
				wlen += websWrite(wp, T(","));
			}
		}
		fclose(fp);
	}
	else 
	{
		DBG_PRINTF("Fail to open log file!");
	}
	return wlen;
}

#else
//end by leijinbao for 275

static int asp_get_syslog(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[320];
	int wlen = 0;
	int ret= 0;
	char *ptr = NULL;
	char *str = NULL;
	#if 0
	/*add by leijinbao 2013/8/22*/
	char syslog_name[64] = {0};
	char cmd[64] = {0};
	sprintf(syslog_name, "%s_syslog.bin", CONFIG_PRODUCT_NAME);
	sprintf(cmd,"cp -r /var/log/messages  /www/web_asp/%s",syslog_name);
	ret = system(cmd);
	if(-1 == ret || 127 == ret)
	{	
		DBG_PRINTF("error!\n");
		return -1;
	}
	/*end by leijinbao 2013/8/22*/
	#endif
    /*add by zhouguanhua 2013/6/7*/
	if (access(SYSLOG_FILE_PATH, F_OK) != 0)
	{
	    return 0;    
	}
    /*end add by zhouguanhua 2013/6/7*/
	fp = fopen(SYSLOG_FILE_PATH, "r");
	if(fp) {
		wlen += websWrite(wp, T("'"));
		while(!feof(fp)) {
			fgets(line, sizeof(line), fp);
			ptr = strchr(line,'	');
			if(ptr)
			{
				//*ptr++ = ';'
				str = strchr(ptr,':');
				*str++ = ';';
				replace_char(line, '	', ';');
				replace_char(line, 0xa, ';');      
				if(!feof(fp)) 
				{
					wlen += websWrite(wp, T("%s"), line);
				}
			}
		}
		wlen += websWrite(wp, T("'"));
		fclose(fp);
	}
	else {
		DBG_PRINTF("Fail to open log file!");
	}
	return wlen;
}
#endif

int get_syslog_value(sys_syslog_t *sys)
{
	int invalid = 0;
	char val[64];  
    if(sys == NULL)
    {
        return IPC_STATUS_FAIL;
    }
	/* syslog_en */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get syslog_en  faild");
        return IPC_STATUS_FAIL;
    }

	if (val[0])
    {
		sys->syslog_en = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }
    /*syslog_ip*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_IP, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_IP faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		strncpy(sys->syslog_ip, val, sizeof(sys->syslog_ip));
	}else {
		invalid ++;
	}	
    
	
	/*syslog_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_DEGREE faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		sys->syslog_degree = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
		
	/*syslog_save_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_SAVE_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_SAVE_DEGREE faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		sys->syslog_save_degree  = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	/*syslog_port  add by leijinbao 2013/8/21*/


	memset(val, 0, sizeof(val));
	if(cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_PORT, (void *)val, "", sizeof(val)) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_PORT faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		sys->syslog_port= strtoul(val, NULL, 0);
	}else {
		invalid ++;
		//sys->syslog_port= 514;
	}

	/*end by leijinbao 2013/8/21*/
	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid LOG INFO");
		return IPC_STATUS_FAIL;
	}  	
	return IPC_STATUS_OK;
}
/*****************************************************************
Function:asp_syslog_set
Description:the function for getting value for sys_syslog_set.asp 
Author:zhouguanhua
Date:2013/4/5
INPUT:eid  wp  argc  argv     
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/

static int asp_syslog_set(int eid, webs_t wp, int argc, char_t **argv)
{
    sys_syslog_t info;
    int ret = 0; 
    memset(&info,0,sizeof(info));
	if(get_syslog_value(&info) ==IPC_STATUS_OK)
	{
		ret = websWrite(wp, T("'%u;%s;%u;%u'"), 
                            info.syslog_en, 
                            info.syslog_ip,
                            info.syslog_degree,
                            info.syslog_save_degree); 

	}
	return ret;
}


/*
static int webs_put_file(webs_t wp, char *path)
{
	FILE *fp;
	char buffer[WEBS_BUFSIZE];
	int wlen = 0, rlen;

	fp = fopen(path, "r");
	if(fp) {
		while(!feof(fp)) {
			rlen = fread(buffer, sizeof(unsigned char), sizeof(buffer), fp);
			if(!feof(fp) && (rlen > 0)) {
				websWriteBlock(wp, buffer, rlen);
			}
			wlen += rlen;
		}
		fclose(fp);
	}
	else {
		DBG_PRINTF("Fail to open file : %s!", path);
	}
	return wlen;
}


static int asp_get_files(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen;
	ipc_system_ack_t *pack;
	if(argc < 1) {
		DBG_ASSERT(0, "Invalid asp argument");
		return 0;
	}
	else {
		if(!strcasecmp(argv[0], WEBS_CONFIG_FILE)) {	
			pack = ipc_system_req(ipc_fd, IPC_CONFIG_GET, 0);
			if (pack && (pack->status == IPC_STATUS_OK)){
				wlen = webs_put_file(wp, SYS_CONFIG_TMP_FILE);
			}
			if (pack) free(pack);
		}else if (!strcasecmp(argv[0], WEBS_SYSLOG_FILE)){
			wlen = webs_put_file(wp, SYSLOG_FILE_PATH);
		}
	}
	return wlen;
}
*/
int asp_get_config_file_ready(void)
{
	int ret = 0;
	ipc_system_ack_t *pack;
	pack = ipc_system_req(ipc_fd, IPC_SYS_CONFIG_BACKUP, 0);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)){
		ret = 1;
	}

	if (pack) free(pack);
	
	return ret;
}

static int asp_cnu_info(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_cnu_info_ack_t *pinfo = NULL;
	ethernet_addr_t cnu_mac;
	char mac[24];
	int ret, wlen = 0;


	if ((argc < 1) || (argv[0] == NULL)){
		DBG_ASSERT(0, "Invalid arguments");
		return 0;
	}

	if(!websQueryString(wp, argv[0], mac, sizeof(mac))) {
		DBG_ASSERT(0, "Invalid query string");
		return 0;
	}

	ret = hexencode(cnu_mac.octet, sizeof(cnu_mac.octet), mac);
	if(ret != sizeof(ethernet_addr_t)) {
		DBG_ASSERT(0, "Invalid MAC address");
		return 0;
	}

	pinfo = ipc_cnu_info(ipc_fd, FILTER_TYPE_MAC, 0, 0, &cnu_mac);

	if(pinfo && (pinfo->hdr.status == IPC_STATUS_OK)) {
//		hexdump(cnu_stat, 0, sizeof(ipc_cnu_status_ack_t));
		wlen += websWrite(wp, T("'%d;%d;%s;%d;%d;%d;%s;%d;%s;%s;%d;%d;%lu'"),
				pinfo->cnu_info.clt,			// Mater Channel ID
				0,		// STA ID
				mac,	// STA Mac Address
				pinfo->cnu_info.tei,	// STA TEI
				pinfo->cnu_info.avgtx,	// TX Rate
				pinfo->cnu_info.avgrx,	// RX Rate
				pinfo->cnu_info.alias,	// HFID
				pinfo->cnu_info.link,					// Link status
				cnu_auth_string(pinfo->cnu_info.auth),		// Auth status
				pinfo->cnu_info.username,						// Alias
				pinfo->cnu_info.device_id,	// device id
				pinfo->cnu_info.template_id,
				time(NULL) - pinfo->cnu_info.online_tm	// Online time
				);
	}

	if(pinfo) free(pinfo);
	return wlen;
}


static int asp_cnu_mibs(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_cnu_mib_ack_t *pmib = NULL;
	ethernet_addr_t cnu_mac;
	char mac[24];
	int ret, wlen = 0, port;


	if ((argc < 1) || (argv[0] == NULL)){
		DBG_ASSERT(0, "Invalid arguments");
		return 0;
	}

	if(!websQueryString(wp, argv[0], mac, sizeof(mac))) {
		DBG_ASSERT(0, "Invalid query string");
		return 0;
	}

	ret = hexencode(cnu_mac.octet, sizeof(cnu_mac.octet), mac);
	if(ret != sizeof(ethernet_addr_t)) {
		DBG_ASSERT(0, "Invalid MAC address");
		return 0;
	}

	pmib = ipc_cnu_mib(ipc_fd, 0, FILTER_TYPE_MAC, 0, 0, &cnu_mac);
	if(pmib && (pmib->hdr.status == IPC_STATUS_OK)) {
		for(port = 0; port < pmib->ports; port++) {
			wlen += websWrite(wp, T("'%d;%d;%s;%lu;%lu;%lu;%lu'%s"),
					port,
					pmib->cnu_mib[port].link,
					pmib->cnu_mib[port].spd ? "100M" : "10M",
					pmib->cnu_mib[port].txpacket,
					pmib->cnu_mib[port].rxpacket,
					0,
					0, (port == pmib->ports - 1) ? "" : ",\n"
					);
		}
	}else {
		ipc_cnu_port_mib_t cnu_mib[2];
		int port_num = 2;
		memset(cnu_mib, 0, sizeof(cnu_mib));
		
		for(port = 0; port < port_num; port++) {
			wlen += websWrite(wp, T("'%d;%d;%s;%lu;%lu;%lu;%lu'%s"),
					port,
					pmib->cnu_mib[port].link,
					pmib->cnu_mib[port].spd ? "100M" : "10M",
					pmib->cnu_mib[port].txpacket,
					pmib->cnu_mib[port].rxpacket,
					0,
					0, (port == port_num - 1) ? "" : ",\n"
					);
		}	
	}
	if(pmib) free(pmib);
	return wlen;
}



static int asp_cnu_link_stats(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_cnu_link_stats_ack_t *pack = NULL;
	ethernet_addr_t cnu_mac;
	char mac[24], stat_str[128];
	int ret, wlen = 0;
		


	if ((argc < 1) || (argv[0] == NULL)){
		DBG_ASSERT(0, "Invalid arguments");
		return 0;
	}

	if(!websQueryString(wp, argv[0], mac, sizeof(mac))) {
		DBG_ASSERT(0, "Invalid query string");
		return 0;
	}

	ret = hexencode(cnu_mac.octet, sizeof(cnu_mac.octet), mac);
	if(ret != sizeof(ethernet_addr_t)) {
		DBG_ASSERT(0, "Invalid MAC address");
		return 0;
	}

	pack = ipc_cnu_link_stats(ipc_fd, FILTER_TYPE_MAC, 0, 0, &cnu_mac);

	if(pack && (pack->hdr.status == IPC_STATUS_OK)) {
		sprintf(stat_str, T("'%6.3f%%;%6.3f;%6.3fdB;%6.3f%%;%6.3f%%;%6.2fdB;%6.2f;%llu;%llu;%llu;%llu;%u'"),
			pack->stats.pre_fec, 
			pack->stats.bits_carrier,
			pack->stats.snr_carrier,
			pack->stats.tx_pbs_err,
			pack->stats.rx_pbs_err,
			pack->stats.avg_att,
			pack->stats.avg_txpwr,
			pack->stats.txpkt,
			pack->stats.txerr,
			pack->stats.rxpkt,
			pack->stats.rxerr,
			pack->stats.valid
		);
		
		wlen += websWrite(wp, T("%s"), stat_str);
	}

	if(pack) free(pack);
	return wlen;
}



static int asp_clt_cable_param(int eid, webs_t wp, int argc, char_t **argv)
{
	
	ipc_clt_cable_param_t *p;
	ipc_clt_cable_param_ack_t *pack;
	int i, wlen = 0;
	pack = ipc_clt_cable_param(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		p = &pack->cab_param;
		wlen = websWrite(wp, T("'%d',"), MAX_CLT_CHANNEL);
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			wlen += websWrite(wp, T("'%d;%d;%d'%s"), p->clts[i].snid, p->clts[i].network_refresh_period, 
				p->clts[i].anonymous_control, (i == MAX_CLT_CHANNEL - 1) ? "" : ",\n");					
		}
	}
	if(pack) free(pack);
	
	return wlen;	
}

/*begin modified by liaohongjun 2012/9/10*/
#if 0
static int asp_client_query(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_network_info_ack_t *nwinfo;
	int clt, update = 0;
	int i, wlen = 0;

	if(argc < 1) {
		DBG_ASSERT(1, "Invalid argument.");
		return 0;
	}
	if(!strcmp(argv[0], "ALL"))
		clt = 0xff;
	else
		clt = strtoul(argv[0], NULL, 0);

	if(argc > 1) {
		update = strtoul(argv[1], NULL, 0);
	}

	nwinfo = ipc_network_info(ipc_fd, clt, update);

	if(nwinfo && (nwinfo->hdr.status == IPC_STATUS_OK)) {
		for(i = 0; i < nwinfo->count; i ++) {
			wlen += websWrite(wp, (i == 0) ? T("\n") : T(",\n"));
			wlen += websWrite(wp, T("'%d;%d;%s;%d;%d;%d;%s;%d;%s;%s;%d;%d;%d'"),
					nwinfo->cnus[i].clt, i,	mac2str(&nwinfo->cnus[i].mac),
					nwinfo->cnus[i].tei,
					nwinfo->cnus[i].avgtx,
					nwinfo->cnus[i].avgrx,
					nwinfo->cnus[i].alias,
					nwinfo->cnus[i].link,
					cnu_auth_string(nwinfo->cnus[i].auth),
					nwinfo->cnus[i].username,
					nwinfo->cnus[i].device_id,
					nwinfo->cnus[i].template_id,
					time(NULL) - nwinfo->cnus[i].online_tm
					);
		}
	}
	if(nwinfo) free(nwinfo);
	return wlen;
}
#endif
static int asp_client_query(int eid, webs_t wp, int argc, char_t **argv)
{
	int clt, update = 0;
	int i, wlen = 0;

	if(argc < 1) {
		DBG_ASSERT(1, "Invalid argument.");
		return 0;
	}
	if(!strcmp(argv[0], "ALL"))
		clt = 0xff;
	else
		clt = strtoul(argv[0], NULL, 0);

	if(argc > 1) {
		update = strtoul(argv[1], NULL, 0);
	}

	for(i = 0; i < 1; i ++) {
		wlen += websWrite(wp, (i == 0) ? T("\n") : T(",\n"));  
	}
	return wlen;
}
/*end modified by liaohongjun 2012/9/10*/
static int asp_fs_dir(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}

static int asp_map_name(int eid, webs_t wp, int argc, char_t **argv)
{
	//return websWrite(wp, T("addCfg('%s','%s','%s');\n"), argv[0], argv[1], webs_nvram_safe_get(argv[1]));
	return 0;
}
#if 0 //add by zhouguanhua

static int asp_query(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}

static int asp_get_link_status(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_clt_interface_status_ack_t *pack;
	char *spd_string[] = {"10", "100", "1000"};
	int i, ret = 0;

	pack = ipc_clt_interface_status(ipc_fd, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		for(i = 0; i < pack->count; i ++) {
			ret = websWrite(wp, T("'%s;%s;%s;%s'%s"),
					pack->status[i].ifname,
					pack->status[i].link == 1 ? "UP" : "DOWN",
					pack->status[i].speed <= 2 ? spd_string[pack->status[i].speed] : "Unknow",
					pack->status[i].duplex == 1 ? "FULL" : "HALF",
					(i == pack->count - 1) ? "" : ",\n"				
					);			
		}
	}
	if(pack) free(pack);
	
	return ret;
}



static int asp_get_port_mibs(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_clt_interface_mibs_ack_t *pack;
	switch_interface_stats_t stats;
	int i, ret = 0;
	char in_oct[128], out_oct[128];

	pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_GET, NULL, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		for(i = 0; i < pack->count; i ++) {
			//fprintf(stderr, "%s:\n", pack->ifmibs[i].ifname);
			//switch_port_mibs_dump(pack->ifmibs[i].mibs);
			switch_mibs2stats(pack->ifmibs[i].mibs, &stats);
			//switch_port_stats_dump(&stats);

			/*
			Important: websWrite do not support long long options
			*/
			sprintf(in_oct, "%llu", stats.in_octets);
			sprintf(out_oct, "%llu", stats.out_octets);			
			
			ret = websWrite(wp, T("'%s;%lu;%lu;%lu;%s;%lu;%lu;%lu;%s'%s"),
					pack->ifmibs[i].ifname,
					stats.in_pkts, stats.in_mcast, stats.in_error, in_oct,
					stats.out_pkts, stats.out_mcast, stats.out_drops,out_oct,
					(i == pack->count - 1) ? "" : ",\n");
		}
	}
	if(pack) free(pack);
	
	return ret;
}
#endif  //add by zhouguanhua




int get_info_val(sys_info_t *info)
{
    char val[BUF_SIZE256] = {0};
    version_t sys_version = {version_string, version_count, version_date};

    if(info == NULL)
    {
        return IPC_STATUS_FAIL;
    }
     /* mac */ 
	if ((get_if_mac(SYS_NETIF_NAME, &(info->sysmac))) < 0)
	{
        DBG_PRINTF("get sys mac error");
        return IPC_STATUS_FAIL;
	}     
    
	//version_date
#if defined(PRIV_PRODUCT_NAME_SUPPORT)	
	safe_strncpy(info->sw_version,	CONFIG_RELEASE_VER_PRIV, sizeof(info->sw_version));
#else
	safe_strncpy(info->sw_version,  sys_version.toString(), sizeof(info->sw_version));
#endif

    //buildtime	
	safe_strncpy(info->buildtime,sys_version.toDate() , sizeof(info->buildtime));

    //HWREV
    memset(val,0,sizeof(val));
    if(GetHardVersion(val,sizeof(val)) !=-1)
	{
        safe_strncpy(info->hw_version,val,sizeof(info->hw_version));     
    }
    else
    {
        DBG_PRINTF("get hw version error");
        return IPC_STATUS_FAIL;
	}
    //uptime
    info->uptime = sys_os_get_uptime();  

    //mvlan
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get mvlan id  faild");
        return IPC_STATUS_FAIL;	
    }
	if (val[0])
    {
		info->mvlan = strtoul(val, NULL, 0);       
	}
    return IPC_STATUS_OK;
}
/*Add by zhouguanhua 2013/5/3 for bug id 36*/
static int asp_debug_info(int eid, webs_t wp, int argc, char_t **argv)
{
	sys_info_t *info;
	ipc_sys_info_ack_t *pack;
	int ret = 0;
	pack = ipc_sys_info(ipc_fd);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		info = &pack->info;		
		ret = websWrite(wp, T("'%s','%s','%s','%s','%s','%s','%s'"),				
				info->hw_version,       
				info->bootload_version, 
				info->sw_version,      
				info->sw_inter_version, 
				info->kernel_version,   
				info->kernel_buildtime, 
				info->buildtime         
				);	
	}
	if(pack) free(pack);	
	return ret;
}
/*End add by zhouguanhua 2013/5/3 for bug id 36*/
/*Add by zhouguanhua 2013/5/15 for bug id 52*/
static int asp_setdebug_info(int eid, webs_t wp, int argc, char_t **argv)
{
    int ret = 0;
	#define MAX_BUILDTIME_SIZE 120

    char tmp_var[MAX_BUILDTIME_SIZE];
	/*add by leijinbao 2014/05/06 for product Encryption bug 565*/
    char szClearText[64];
	char *cp = NULL;
	/*end by leijinbao 2014/05/06 for product Encryption bug 565*/
	
    memset(tmp_var,0,sizeof(tmp_var)); 
	/*begin by leijinbao 2013/12/23 for mac Encryption*/
	memset(szClearText,0,sizeof(szClearText)); 
    if( GetEthaddr(tmp_var,sizeof(tmp_var)) !=-1)
    {
      if(1 == IsStringN63Cipher(tmp_var))
	    {
			DecryptN63Password(tmp_var,szClearText);
		/*add by leijinbao 2014/05/06 for product Encryption bug 565*/
			cp = strchr(szClearText, '/');
			if(cp)
			{
				*cp = '\0';
			}			
		/*end by leijinbao 2014/05/06 for product Encryption bug 565*/
			ret = websWrite(wp, T("'%s',"),szClearText);  
	    }
	  else{
			ret = websWrite(wp, T("'%s',"),tmp_var);
	  }
    }
    else
    {
        ret = websWrite(wp, T("'%s',"),"");   
    }

    memset(tmp_var,0,sizeof(tmp_var));
    if(GetSN(tmp_var,sizeof(tmp_var)) !=-1)
    {
        ret += websWrite(wp, T("'%s'"),tmp_var);	
    }
    else
    {
        ret += websWrite(wp, T("'%s'"),"");	   
    }  	   				
	return ret;
}
/*End add by zhouguanhua 2013/5/15 for bug id 52*/

/*begin add by feihuaxin for cpu info 2013-8-23*/
#define NCPUSTATES 4
#define NMEMSTATS 6

static long cp_time[NCPUSTATES];
static long cp_old[NCPUSTATES];
static long cp_diff[NCPUSTATES];
static int cpu_states[NCPUSTATES];
static int memory_stats[NMEMSTATS];


struct system_info
{
    int    last_pid;
    double load_avg[3];
    int    p_total;
    int    p_active;     /* number of procs considered "active" */
    int    *procstates;
    int    *cpustates;
    int    *memory;
};

static inline char *
skip_ws(const char *p)
{
    while (isspace(*p)) p++;
    return (char *)p;
}
    
static inline char *
skip_token(const char *p)
{
    while (isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    return (char *)p;
}

long percentages(cnt, out, new, old, diffs)

int cnt;
int *out;
register long *new;
register long *old;
long *diffs;

{
    register int i;
    register long change;
    register long total_change;
    register long *dp;
    long half_total;

    /* initialization */
    total_change = 0;
    dp = diffs;

    /* calculate changes for each state and the overall change */
    for (i = 0; i < cnt; i++)
    {
	if ((change = *new - *old) < 0)
	{
	    /* this only happens when the counter wraps */
	    change = (int)
		((unsigned long)*new-(unsigned long)*old);
	}
	total_change += (*dp++ = change);
	*old++ = *new++;
    }

    /* avoid divide by zero potential */
    if (total_change == 0)
    {
	total_change = 1;
    }

    /* calculate percentages based on overall change, rounding up */
    half_total = total_change / 2l;
    for (i = 0; i < cnt; i++)
    {
	*out++ = (int)((*diffs++ * 1000 + half_total) / total_change);
    }

    /* return the total in case the caller wants to use it */
    return(total_change);
}

void
get_system_info(info)
    struct system_info *info;
{
    char buffer[4096+1];
    int fd, len;
    char *p;
    int i;

    /* get load averages */
    {
	fd = open("/proc/loadavg", O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';

	info->load_avg[0] = strtod(buffer, &p);
	info->load_avg[1] = strtod(p, &p);
	info->load_avg[2] = strtod(p, &p);
	p = skip_token(p);			/* skip running/tasks */
	p = skip_ws(p);
	if (*p)
	    info->last_pid = atoi(p);
	else
	    info->last_pid = -1;
    }

    /* get the cpu time info */
    {
	fd = open("/proc/stat", O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';

	p = skip_token(buffer);			/* "cpu" */
	cp_time[0] = strtoul(p, &p, 0);
	
	cp_time[1] = strtoul(p, &p, 0);
	cp_time[2] = strtoul(p, &p, 0);
	cp_time[3] = strtoul(p, &p, 0);

	/* convert cp_time counts to percentages */
	percentages(4, cpu_states, cp_time, cp_old, cp_diff);
    }
	
    /* get system wide memory usage */
    {
	char *p;

	fd = open("/proc/meminfo", O_RDONLY);
	len = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	buffer[len] = '\0';

	/* be prepared for extra columns to appear be seeking
	   to ends of lines */
	
	p = buffer;
	p = skip_token(p);
	memory_stats[0] = strtoul(p, &p, 10); /* total memory */
	
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[1] = strtoul(p, &p, 10); /* free memory */
	
	
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[2] = strtoul(p, &p, 10); /* buffer memory */
	
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[3] = strtoul(p, &p, 10); /* cached memory */
	
	for(i = 0; i< 8 ;i++) {
		p++;
		p = strchr(p, '\n');
	}
	
	p = skip_token(p);
	memory_stats[4] = strtoul(p, &p, 10); /* total swap */
	
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[5] = strtoul(p, &p, 10); /* free swap */
	
    }

    /* set arrays and strings */
    info->cpustates = cpu_states;
    info->memory = memory_stats;
}

/*end add by feihuaxin for cpu info 2013-8-23*/


static int asp_sys_info(int eid, webs_t wp, int argc, char_t **argv)
{
    #if 0
	sys_info_t *info;
	ipc_sys_info_ack_t *pack;
    /*  
	char macstr0[24], macstr1[24],macstr2[24];
	int ret = 0;
	pack = ipc_sys_info(ipc_fd);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		//'uptime,mvlan, sw, hw, cab0mac, cab1mac, sysmac, buildtime
		info = &pack->info;
		strcpy(macstr0, mac2str(&info->cabmac[0]));
		strcpy(macstr1, mac2str(&info->cabmac[1]));
		strcpy(macstr2, mac2str(&info->sysmac));		
		ret = websWrite(wp, T("'%lu','%u','%s','%s','%s','%s', '%s', '%s'"),
				info->uptime,
				info->mvlan,
				info->sw_version,
				info->hw_version,
				macstr0, macstr1, macstr2,
				info->buildtime
				);
	}*/
	#endif
	sys_info_t info;
	//char val[BUF_SIZE256] = {0};
    char macstr2[24];
    int ret = 0;
    char var1[BUF_SIZE_16];
    char var2[BUF_SIZE_16];

    memset(&info, 0 , sizeof(info));

    if(IPC_STATUS_OK != get_info_val(&info))
    {
       return ret; 
    }
    strcpy(macstr2, mac2str(&info.sysmac));


    struct system_info sysinfo;
    memset(cp_time, 0, NCPUSTATES);
    memset(cp_old, 0, NCPUSTATES);
    memset(cp_diff, 0, NCPUSTATES);
    memset(memory_stats, 0, NMEMSTATS);
    memset(&sysinfo, 0, sizeof(sysinfo));
    get_system_info(&sysinfo);
    sprintf(var1, "%.1f", (float)sysinfo.cpustates[0]/10);
    sprintf(var2, "%.1f", (((float)sysinfo.memory[0]-(float)sysinfo.memory[1])/(float)sysinfo.memory[0])*100);

    ret = websWrite(wp, T("'%lu','%u','%s','%s','%s', '%s','%s','%s'"),
            info.uptime,
            info.mvlan,  
            info.sw_version,
            info.hw_version,
            macstr2,
            info.buildtime,
            var1,
            var2
            );
            	
	return ret;
}

int get_ntp_syn_state(void)
{
    FILE *fd;	
	int sysn = 0;
	char buffer[2]; 
    if (access(SYS_NTPCLIENT_OK_FILE, F_OK) != 0)
	{
	    return 0;    
	}
	fd = fopen(SYS_NTPCLIENT_OK_FILE, "r");
	if (fd == NULL)
	{
	    DBG_PRINTF("ntpclient fail to sysn\n");
    }
	else
	{
		if(fgets(buffer, 2, fd))
		{
			sysn = strtoul(buffer, NULL, 0);
        }
		fclose(fd);
	}
	if (sysn > 0){
		return 1;
	}
	else
	{
	    return 0;   
	}
}

static int asp_sys_time(int eid, webs_t wp, int argc, char_t **argv)
{
    #if 0
	sys_time_t *t;
	struct tm tm;
	ipc_sys_time_ack_t *pack;
	int ret = 0;
	pack = ipc_sys_time(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		t = &pack->time;
		gmtime_r(&t->time, &tm);
		//fprintf(stderr, "Time:%lu Get:%s\n", t->time, asctime(&tm));
		ret = websWrite(wp, T("'%d/%d/%d', '%d:%d:%d', '%d;%d;%s','%d\'"),//modify by zhouguanhua  2012-9-25 EPN104QID0060
			tm.tm_mon + 1,tm.tm_mday,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec,
			t->zone, t->ntp_en, t->server,t->ntp_ok);//Modify by zhouguanhua to add ntp_ok//modify by zhouguanhua
	}
	if(pack) free(pack);	
	return ret;
    #endif
    sys_time_t sys;
    struct tm tm;
    int ret = 0;
    char val[BUF_SIZE256] = {0};
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_SERVER, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP server faild");
		return 0;
	}
	if (val[0])
	{
		strncpy(sys.server, val, sizeof(sys.server));
	}
	else 
	{
		DBG_ASSERT(0, "Get NTP server faild");
		return 0;
	}
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_ZONE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP zone faild");
		return 0;
	}
	if (val[0])
	{
		sys.zone = strtoul(val, NULL, 0);
	}
	else 
	{
		DBG_ASSERT(0, "Get NTP server faild");
		return 0;
	}
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_TYPE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP type faild");
		return 0;
	}
	if (val[0])
	{
		sys.ntp_en = strtoul(val, NULL, 0);
	}
	else 
	{
		DBG_ASSERT(0, "Get NTP server faild");
		return 0;
	}	
	sys.time=local_time(time(NULL), sys.zone);	
    gmtime_r(&sys.time, &tm);
    sys.ntp_ok=get_ntp_syn_state();
    ret = websWrite(wp, T("'%d/%d/%d', '%d:%d:%d', '%d;%d;%s','%d\'"),//modify by zhouguanhua  2012-9-25 EPN104QID0060
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_year+1900,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec,
                sys.zone, 
                sys.ntp_en, 
                sys.server,
                sys.ntp_ok);
	return ret;
}

static int asp_sys_security(int eid, webs_t wp, int argc, char_t **argv)
{
	sys_security_t *info;
	ipc_sys_security_ack_t *pack;
	
	int ret = 0;
	int ssh_support = 0;
#ifdef CONFIG_SSHD
	ssh_support = 1;
#endif
	
	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {		
		info = &pack->security;
		/*mvlan https, ssh, sshp, telnet, telnetp*/
		ret = websWrite(wp, T("'%u','%d','%d','%u','%d','%u','%s','%s','%d'"),
				info->mvlan,
				info->https_en,
				info->ssh_en,
				info->ssh_port,
				info->telnet_en,
				info->telnet_port,
				"",
				"",
				ssh_support);	
	}
	
	if(pack) free(pack);	
	return ret;
}

 int  get_sys_snmp_value(sys_snmp_t *sys)
{
	int invalid = 0;
	uint8_t i = 0;
	char val[1024]; 

	 if(sys == NULL)
     {
         return IPC_STATUS_FAIL;
     }
    char *snmp_security_str[] = {"noauthnopriv", "authnopriv", "authpriv" ,""};
	/*Begin add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/
	/*agent */
    /*snmp_enabled*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_en  faild");
    }
	if(!strcmp(val, "enable"))
	{
		sys->snmp_en = 1;
	}
	else if(!strcmp(val, "disable"))
	{
		sys->snmp_en = 0;
	}
	else
	{
		invalid++;
	}
	
    /*snmp_engineID*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENGINEID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_engineID  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_engineID, val, sizeof(sys->snmp_engineID));
    }
    else
    {
        invalid++;
    }
    /*snmp_max_packet_len*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_MAX_PACKET_LEN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_max_packet_len  faild");
    }
	if (val[0])
	{
		sys->snmp_max_packet_len = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	/*snmp_location*/
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_LOCATION, (void *)sys->snmp_location, "", sizeof(sys->snmp_location))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_location  faild");
    }

    /*snmp_contact*/
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_CONTACT, (void *)sys->snmp_contact, "", sizeof(sys->snmp_contact))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_contact  faild");
    }

    /*snmp_version*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_version  faild");
    }
	if (val[0])
	{
		sys->snmp_version = strtoul(val, NULL, 0);
        if(NULL != strstr(val, "V1"))
        {
            sys->snmp_version= SNMP_V1_VAL;
        }
        else if(NULL != strstr(val, "V2c"))
        {
            sys->snmp_version= SNMP_V2c_VAL;
        }
        else if(NULL != strstr(val, "V3"))
        {
            sys->snmp_version= SNMP_V3_VAL;
        }		
	}
	else 
	{
		invalid++;
	}
    
	/*snmp_trust_host*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRUST_HOST, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trust_host  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_trust_host, val, sizeof(sys->snmp_trust_host));
    }
    else
    {
        invalid++;
    }
	
	/*add community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_comm_current_count  faild");
    }
	if (val[0])
	{
		sys->snmp_comm_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_COMM_COUNT; i++)
	{	
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_NAME+i+1, (void *)sys->snmp_comm[i].snmp_comm_name, "", sizeof(sys->snmp_comm[i].snmp_comm_name))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_comm_name  faild");
            return IPC_STATUS_FAIL;
        }

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ACCESS_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_access_mode  faild");
            return IPC_STATUS_FAIL;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RO;
		}
		else 
		{
		    sys->snmp_comm[i].snmp_access_mode=SNMP_ACESS_MODE_NULL;//NULL
		}
	}

	
	/*add*/
	sys->snmp_add_which_num = NO_ADD_NUM; 
	/*del */
	sys->snmp_del_which_num = NO_DEL_NUM; 

	sys->snmp_set_flag = 0;
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/


	/*trap*/
    //TRAP add by zhouguanhua 2012/12/20  
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_EN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_en  faild");
    }

	if (val[0]){
		sys->snmp_trap_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_IP, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_ip  faild");
    }

	if (!val[0] || !inet_aton(val, &sys->snmp_trap_ip)){
		invalid ++;		
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_com  faild");
    }
    /*snmp_trap_com*/

	if (val[0]){
        sys->snmp_trap_com = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    /*snmp_trap_community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COMMUNITY, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_community  faild");
    }

	if (val[0]){
		strncpy(sys->snmp_trap_community, val, sizeof(sys->snmp_trap_community));
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_version  faild");
    }

	if (val[0]){
		sys->snmp_trap_version = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_SAFE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_safe  faild");
    }

	if (val[0]){
        sys->snmp_trap_safe = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_LAST_SAVE_SAFE_NUM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_last_save_safe_num  faild");
    }

	if (val[0]){
        sys->snmp_last_save_safe_num = strtoul(val, NULL, 0);
	}else {
		invalid ++;
    }
     //end add by zhouguanhua 2012/12/20
	/*Begin add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/
	/*add group*/

     memset(val, 0, sizeof(val));  
     if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
     {
         DBG_ASSERT(0, "Get snmp_group_current_count  faild");
     }

	if (val[0])
	{
		sys->snmp_group_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_GROUP_COUNT; i++)
	{	
         if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_NAME+i+1, (void *)sys->snmp_group[i].snmp_group_name, "", sizeof(sys->snmp_group[i].snmp_group_name))) < 0)
         {
             DBG_ASSERT(0, "Get snmp_group_current_count  faild");
             return IPC_STATUS_FAIL;
         }		

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_current_count  faild");
            return IPC_STATUS_FAIL;
        }
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHENCRYPT;
		}
		else
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NULL;
		}

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_mode  faild");
            return IPC_STATUS_FAIL;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RO;
		}
		else
		{   
		    sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_NULL;
		}
	}	
	
	/*End add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/
	/*add user*/
	//add by zhouguanhua 2012/12/24
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_user_current_count  faild");
    }

	if (val[0])
	{
		sys->snmp_user_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	for(i=0; i< MAX_SNMP_USER_COUNT; i++)
	{	
        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_NMAE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_name, val, sizeof(sys->snmp_user[i].snmp_user_name));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_GROUP+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_group  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_group, val, sizeof(sys->snmp_user[i].snmp_user_group));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_PASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_password  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_password, val, sizeof(sys->snmp_user[i].snmp_user_password));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENPASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_enpassword  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_enpassword, val, sizeof(sys->snmp_user[i].snmp_user_enpassword));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return IPC_STATUS_FAIL;
        }	
        
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHENCRYPT;
		}
		else 
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NULL;
		}


	}	
	//end add by zhouguanhua 2012/12/24	
	


	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid SYS SNMP INFO");
		return IPC_STATUS_FAIL;
	}
	
	return IPC_STATUS_OK;
}

static int asp_sys_snmp_basic(int eid, webs_t wp, int argc, char_t **argv)
{
    sys_snmp_t sys,*info = NULL;
    int ret = 0;
    int i = 0;
    memset(&sys,0,sizeof(sys));
    if(get_sys_snmp_value(&sys) ==IPC_STATUS_OK)
    {       
        info=&sys;
        ret = websWrite(wp, T("'%u;%s;%lu;%s;%s;%u;%s;%u"), 
                            info->snmp_en, 
                            info->snmp_engineID,
                            info->snmp_max_packet_len,
                            info->snmp_location,
                            info->snmp_contact,
                            info->snmp_version,
                            info->snmp_trust_host,
                            info->snmp_comm_current_count); 
        for (i = 0; i < MAX_SNMP_COMM_COUNT; i++)
        {
            ret += websWrite(wp, T(";%s;%u"), 
                info->snmp_comm[i].snmp_comm_name,
                info->snmp_comm[i].snmp_access_mode);   
        }
        ret += websWrite(wp, T("'")); 
    }
    else
    {   
        DBG_PRINTF("ipc_sys_snmp get error!");
        ret = -1;
    }
    return ret;
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/
}
/*****************************************************************
    Function:asp_sys_snmp_trap
    Description: 
    Author:zhouguanhua
    Date:2012/12/20
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
static int asp_sys_snmp_trap(int eid, webs_t wp, int argc, char_t **argv)
{
    sys_snmp_t sys,*info = NULL;
    int ret = 0;
    int i = 0;
    memset(&sys,0,sizeof(sys));
    if(get_sys_snmp_value(&sys) ==IPC_STATUS_OK)
    {       
        info=&sys;
        ret = websWrite(wp, T("'%u;%s;%1u;%u"), 
                            info->snmp_trap_en, 
                            inet_ntoa(info->snmp_trap_ip),
                            info->snmp_trap_com,
                            info->snmp_trap_version);                              
        for (i = 0; i < MAX_SNMP_COMM_COUNT; i++)
        {
            ret += websWrite(wp, T(";%s"), 
            info->snmp_comm[i].snmp_comm_name); 
        }
        for (i = 0; i < MAX_SNMP_USER_COUNT; i ++)
        {
            ret += websWrite(wp, T(";%s;%u"), 
                            info->snmp_user[i].snmp_user_name,
                            info->snmp_user[i].snmp_user_security); 
        }  
        ret += websWrite(wp, T(";%u"),info->snmp_last_save_safe_num);
        ret += websWrite(wp, T("'")); 
    }
    else
    {   
        DBG_PRINTF("ipc_sys_snmp_trap get error!");
        ret = -1;
     }  
    return ret;
}
//
/*****************************************************************
    Function:asp_sys_snmp_user
    Description: 
    Author:zhouguanhua
    Date:2012/12/24
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
static int asp_sys_snmp_user(int eid, webs_t wp, int argc, char_t **argv)
{
    sys_snmp_t sys,*info = NULL;
    int ret = 0;
    int i = 0;
    memset(&sys,0,sizeof(sys));
    if(get_sys_snmp_value(&sys) ==IPC_STATUS_OK)
    {       
        info=&sys;
        ret = websWrite(wp, T("'"));                        
        for (i = 0; i < MAX_SNMP_GROUP_COUNT; i ++)
        {
            ret += websWrite(wp, T("%s;"), 
            info->snmp_group[i].snmp_group_name);  
        }
        for (i = 0; i < MAX_SNMP_GROUP_COUNT; i ++)
        {
            ret += websWrite(wp, T("%u;"), 
            info->snmp_group[i].snmp_group_security);  
        }
        ret += websWrite(wp, T("%u"),info->snmp_user_current_count);
        for (i = 0; i < MAX_SNMP_USER_COUNT; i ++)
        {
            ret += websWrite(wp, T(";%s;%s;%u"), 
            info->snmp_user[i].snmp_user_name,
            info->snmp_user[i].snmp_user_group,
            info->snmp_user[i].snmp_user_security); 
        }     
        ret += websWrite(wp, T("'"));   
    }
    else
    {   
        DBG_PRINTF("ipc_sys_snmp_user get error!");
        ret = -1;
    }     
    return ret;
}

/*****************************************************************
    Function:asp_sys_snmp_group
    Description: system snmp group send data to web
    Author:huangmingjian
    Date:2012/12/23
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/

static int asp_sys_snmp_group(int eid, webs_t wp, int argc, char_t **argv)
{
    sys_snmp_t sys,*info = NULL;
    int ret = 0;
    int i = 0;
    memset(&sys,0,sizeof(sys));
    if(get_sys_snmp_value(&sys) ==IPC_STATUS_OK)
    {       
        info=&sys;
		
		ret = websWrite(wp, T("'%u"), info->snmp_group_current_count);	
		for (i = 0; i < MAX_SNMP_GROUP_COUNT; i ++)
		{
			ret += websWrite(wp, T(";%s;%u;%u"), 
				info->snmp_group[i].snmp_group_name,
				info->snmp_group[i].snmp_group_security,
				info->snmp_group[i].snmp_group_mode);	
		}
		ret += websWrite(wp, T("','")); 	

		for (i = 0; i < MAX_SNMP_USER_COUNT; i ++)
        {
            ret += websWrite(wp, T("%s;%s;"), 
            info->snmp_user[i].snmp_user_name,
            info->snmp_user[i].snmp_user_group); 
        }
		ret += websWrite(wp, T("'")); 	
	}
	else
	{	
		DBG_PRINTF("asp_sys_snmp_group get error!");
		ret = -1;
	}
		
	return ret;
}
#if 0
/*****************************************************************
    Function:asp_sys_snmp_dev_status
    Description: system snmp device status send data to web
    Author:huangmingjian
    Date:2012/12/24
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/

static int asp_sys_snmp_catvinfo(int eid, webs_t wp, int argc, char_t **argv)
{	
	sys_admin_t *info_admin = NULL;
	ipc_sys_admin_ack_t *pack_admin = NULL;
	sys_snmp_t *info = NULL;
	ipc_sys_snmp_ack_t *pack = NULL;
	int ret = 0;
	
	pack_admin = ipc_sys_admin(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack_admin && pack_admin->hdr.status == IPC_STATUS_OK) 
	{
		info_admin = &pack_admin->admin;
	}
	else
	{
		DBG_PRINTF("ipc_sys_admin error\n");
		if(pack_admin) 
		{
			free(pack_admin);
		}
		return -1;
	}

	pack = ipc_sys_snmp(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
	{		
		info = &pack->snmp;

		ret = websWrite(wp, T("'%u;%lu;%s;%s;%s;%s;%s;%u'"), 
				info->snmp_catvinfo.dev_status_update,
				info_admin->idle_time,
				info->snmp_catvinfo.soft_version,
		
				info->snmp_catvinfo.input_laser_power,
				info->snmp_catvinfo.output_level,
				info->snmp_catvinfo.temper,
				info->snmp_catvinfo.power_12v,
				info->snmp_catvinfo.output_level_atten);
	}
	else
	{	
		DBG_PRINTF("asp_sys_snmp_dev_status get error!");
		if(pack) 
		{
			free(pack);
		}
		return -1;
	}
	
	if(pack) 
	{
		free(pack);
	}	
	if(pack_admin) 
	{
		free(pack_admin);
	}
	return ret;	
}
#endif

int get_sys_admin(sys_admin_t *info)
{
    char val[BUF_SIZE256] = {0};
    
    if(info == NULL)
    {
        return IPC_STATUS_FAIL;
    }
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_NAME+1, (void *)val, "", sizeof(val))) < 0)
    {              
        DBG_ASSERT(0, "Get user web name faild");
        return IPC_STATUS_FAIL;
    }
    if (val[0])
    {    
        safe_strncpy(info->users[0].name, val, sizeof(info->users[0].name));
    }
    else 
    {
        return IPC_STATUS_FAIL;
    }

    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_PASSWORD+1, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get user web password faild");
        return IPC_STATUS_FAIL;
    }
    if (val[0])
    {
        safe_strncpy(info->users[0].pwd, val, sizeof(info->users[0].pwd));
    }
    else 
    {
        return IPC_STATUS_FAIL;
    }
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_USERNAME, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get web username faild");
        return IPC_STATUS_FAIL;
    }
    if (val[0]){
        safe_strncpy(info->users[1].name, val, sizeof(info->users[1].name));
    }
    else 
    {
        return IPC_STATUS_FAIL;
    }
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_PASSWORD, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get web password faild");
        return IPC_STATUS_FAIL;
    }
    if (val[0])
    {
        safe_strncpy(info->users[1].pwd, val, sizeof(info->users[1].pwd));
    }
    else 
    {
        return IPC_STATUS_FAIL;
    }
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_AUX_ENABLE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get web aux enable faild");
        return IPC_STATUS_FAIL;
    }
    if (!strcmp(val, "enable"))
    {
        info->users[1].enable = 1;
    }
    else 
    {
        info->users[1].enable = 0;
    }
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_IDLETIME, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get web idletime faild");
        return IPC_STATUS_FAIL;
    }
    if (val[0])
    {
        info->idle_time = strtoul(val, NULL, 0);
    }
    else 
    {
        return IPC_STATUS_FAIL;
    }  
    info->users[0].enable = 1;
    info->count = 2;   

    return IPC_STATUS_OK;
}
static int asp_sys_admin(int eid, webs_t wp, int argc, char_t **argv)
{
   #if 0
	sys_admin_t *info;
	ipc_sys_admin_ack_t *pack;
	int ret = 0, i;

	pack = ipc_sys_admin(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		info = &pack->admin;
		ret = websWrite(wp, T("'%lu'"), info->idle_time);	
		for (i = 0; i < info->count; i ++){
			ret += websWrite(wp, T(",'%d;%s;%s'"), info->users[i].enable, info->users[i].name, info->users[i].pwd);	
		}
	}
	
	if(pack) free(pack);	
	return ret;
	#endif
    sys_admin_t info;
	int ret = 0, i;
    memset(&info,0,sizeof(info));
    if(get_sys_admin(&info) == IPC_STATUS_OK )
    {    
    	ret = websWrite(wp, T("'%lu'"), info.idle_time);	
    	for (i = 0; i < info.count; i ++)
    	{	    
    		ret += websWrite(wp, T(",'%d;%s;%s'"), info.users[i].enable, info.users[i].name,info.users[i].pwd);	
    	}
    }
	return ret;
}

/*
CLT502;00:11:22:33:44:55;static',
'ok;192.168.0.100;255.255.255.0;192.168.0.1;202.96.125.24;34.45.25.14;0.0.0.0',
'doing;0.0.0.0;0.0.0.0;0.0.0.0;202.96.125.24;34.45.25.14;0.0.0.0;86400;local'

*/
	/*
	struct in_addr ip;
	struct in_addr subnet;
	struct in_addr broadcast;
	struct in_addr gateway;
	struct in_addr dns[MAX_DNS_NUM];
	
		char hostname[MAX_HOSTNAME_SIZE];
		char ifname[IFNAMSIZ];	
		ethernet_addr_t mac;	
		uint8_t ip_proto;
		netif_param_t netif;
		netif_param_t netif_static;
		uint32_t lease_time; // for dhcp	
		char domain[MAX_DOMAIN_SIZE];			
	*/	


static int asp_sys_networking(int eid, webs_t wp, int argc, char_t **argv)
{
	sys_network_t *info;
	ipc_sys_network_ack_t *pack;
	char *dhcp_status;
	
# define DHCP_STATUS_STR_STOP 	"stop"
# define DHCP_STATUS_STR_DOING 	"doing"
# define DHCP_STATUS_STR_OK 	"ok"
# define DHCP_STATUS_STR_FAILED "failed"

	int i, ret = 0, vlen = 0;
	char buffer[64], ip[24], nm[24], gw[24];
				
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		info = &pack->network;
		/*Begin modified by huangmingjian 2012/11/14 for EPN104QID0082*/ 
		ret = websWrite(wp, T("'%s;%s;%s',"),
				info->hostname,
				mac2str(&info->mac),
				(info->ip_proto == IP_PROTO_STATIC) ? "static" : "dhcp");
		//		info->m_vlan);	
		/*End modified by huangmingjian 2012/11/14 for EPN104QID0082 */
		strcpy(ip, inet_ntoa(info->netif_static.ip));
		strcpy(nm, inet_ntoa(info->netif_static.subnet));
		strcpy(gw, inet_ntoa(info->netif_static.gateway));
        /*begin modified by zhouguanhua 2012/8/31*/       
        #if 0 
		vlen = 0;
		for (i = 0; i < MAX_DNS_NUM; i ++){
			vlen +=sprintf(buffer + vlen, ";%s", inet_ntoa(info->netif_static.dns[i]));
		}
        #endif
        /*end modified by zhouguanhua 2012/8/31*/
		ret = websWrite(wp, T("'%s;%s;%s;%s%s',"), "ok", ip, nm, gw, buffer);

		if (info->ip_proto == IP_PROTO_STATIC){
			memset(&info->netif, 0, sizeof(info->netif));
			dhcp_status = DHCP_STATUS_STR_STOP;
		}else if (info->netif.ip.s_addr == 0){
			dhcp_status = DHCP_STATUS_STR_DOING;
		}else {
			dhcp_status = DHCP_STATUS_STR_OK;
		}

		strcpy(ip, inet_ntoa(info->netif.ip));
		strcpy(nm, inet_ntoa(info->netif.subnet));
		strcpy(gw, inet_ntoa(info->netif.gateway));
		 /*begin modified by zhouguanhua 2012/8/31*/
		 
		/*Begin modified by huangmingjian 2013/03/05 for EPN204QID0029*/
		if(((info->netif.dns[1].s_addr & 0xff000000) >> 24) > 223)
		{	
			info->netif.dns[1].s_addr = 0x0;
		}
		/*Begin modified by huangmingjian 2013/03/05 for EPN204QID0029*/
        #if 0
		vlen = 0;
		for (i = 0; i < MAX_DNS_NUM; i ++){
			vlen +=sprintf(buffer + vlen, ";%s", inet_ntoa(info->netif.dns[i]));
		}
        #endif
		ret = websWrite(wp, T("'%s;%s;%s;%s%s;%lu;%s'"), dhcp_status, ip, nm, gw, buffer, info->lease_time, info->domain);
         /*end modified by zhouguanhua 2012/8/31*/

	}
	if(pack) free(pack);	
	return ret;
}
#if 0 //add by zhouguanhua 2013/6/5
static int asp_sys_networking(int eid, webs_t wp, int argc, char_t **argv)
{
	sys_network_t info;
	char *dhcp_status;
	int i, ret = 0, vlen = 0;
	char buffer[64], ip[24], nm[24], gw[24];
	char val[BUF_SIZE_256] = {0};
    ifindex_t ifindex;
	int items = 0; 
    char *vars[10] = {0};
	//netif = &info->netif_static;
	
# define DHCP_STATUS_STR_STOP 	"stop"
# define DHCP_STATUS_STR_DOING 	"doing"
# define DHCP_STATUS_STR_OK 	"ok"
# define DHCP_STATUS_STR_FAILED "failed"
# define dbg_str(x, v) DBG_ASSERT(0, "Invalid @ "#x" = %s", v)

    memset(&info, 0 , sizeof(info));
   
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get Hostname faild");
        return 0;
    }    
    safe_strncpy(info.hostname, val, sizeof(info.hostname));
    
    if (!info.hostname[0])
    {
        DBG_ASSERT(0, "Empty Hostname, Set to none");
        safe_strncpy(info.hostname, "none", sizeof(info.hostname));     
    }
     /*mac*/
    if ((get_if_mac(SYS_NETIF_NAME, &(info.mac))) < 0){
        DBG_ASSERT(0,"get sys mac error");
        return 0;
    }     
  
    /*l3vlan1 oid*/
    if(IF_GetByIFName("l3vlan1",&ifindex) !=IF_OK)
    {
        DBG_ASSERT(0, "Get ifname faild"); 
        return 0;
    }
    /* IP proto */     
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_DIAL_TYPE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get IP PROTO faild");
       return 0;
    }
    if (val[0])
    {
        info.ip_proto = !strcmp(val, "static") ? IP_PROTO_STATIC : IP_PROTO_DYNAMIC;
    }
    else 
    {
        dbg_str(info.ip_proto, val);
        return 0;
    } 

    /* ipaddress */
    if(IF_OK !=IF_GetAttr(ifindex,IF_IP,&info.netif.ip.s_addr,sizeof(info.netif.ip.s_addr)))
    {               
      DBG_ASSERT(0, "Get IP faild");
      return 0;
    }  

    /*ipmask*/
    if(IF_OK !=IF_GetAttr(ifindex,IF_MASK,&info.netif.subnet.s_addr,sizeof(info.netif.subnet.s_addr)))
    {
        DBG_ASSERT(0, "Get NETMASK faild");
        return 0;
    }
    /*gateway*/
    if(IF_OK !=IF_GetAttr(ifindex,IF_GATEWAY,&info.netif.gateway.s_addr,sizeof(info.netif.gateway.s_addr)))
    {               
      DBG_ASSERT(0, "Get gateway faild");
      return 0;
    }
       
    /*static masterdns*/    
    memset(val, 0, sizeof(val)); 
    if((cfg_getval(ifindex, CONFIG_IP_MSTDNS, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get DNS faild");        
        return 0;
    }
	if (val[0])
	{
    	items = split_string(val, ',', vars);
    	items = (items > MAX_DNS_NUM) ? MAX_DNS_NUM : items;
    	for (i = 0 ; i < items; i ++)
    	{
    		inet_aton(vars[i], &info.netif.dns[i]);
    	}
	}

	ret = websWrite(wp, T("'%s;%s;%s;%u',"),
		                info.hostname,
		                mac2str(&info.mac),
		                (info.ip_proto == IP_PROTO_STATIC) ? "static" : "dhcp"
		                );
    
    /*static*/
    strcpy(ip, inet_ntoa(info.netif.ip));
    strcpy(nm, inet_ntoa(info.netif.subnet));
    strcpy(gw, inet_ntoa(info.netif.gateway));

	vlen = 0;
	for (i = 0; i < MAX_DNS_NUM; i ++)
	{
		vlen +=sprintf(buffer + vlen, ";%s", inet_ntoa(info.netif_static.dns[i]));
	}
    ret = websWrite(wp, T("'%s;%s;%s;%s%s',"), "ok", ip, nm, gw, buffer);


    /*Dynamic*/
    if (info.ip_proto == IP_PROTO_STATIC)
    {
        memset(&info.netif, 0, sizeof(info.netif));
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
    
    strcpy(ip, inet_ntoa(info.netif.ip));
    strcpy(nm, inet_ntoa(info.netif.subnet));
    strcpy(gw, inet_ntoa(info.netif.gateway));

    /*get Dynamic DNS IF_DNSPRI_DYN*/
    /*
    if(dhcp_status == DHCP_STATUS_STR_OK)
    {
        if(IF_OK !=IF_GetAttr(ifindex,IF_DNSPRI_DYN,&info.netif.dns[0],sizeof(info.netif.dns[0])))
        {               
          DBG_ASSERT(0, "Get DNS faild"); 
          return 0;
        }   
    }*/
    
    vlen = 0;
    for (i = 0; i < MAX_DNS_NUM; i ++){
        vlen +=sprintf(buffer + vlen, ";%s", inet_ntoa(info.netif.dns[i]));
    }
    
    ret = websWrite(wp, T("'%s;%s;%s;%s%s;%lu;%s'"), dhcp_status, ip, nm, gw, 
                    buffer, info.lease_time, info.domain);
	return ret;
}
#endif //add by zhouguanhua 2013/6/5

 int get_sys_mvlan_value(sys_mvlan_t *sys)
{
	int invalid = 0;
     char val[10];  
     if(sys == NULL)
     {
         return IPC_STATUS_FAIL;
     }
	/* vlan_manage_vid */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get mvlan id  faild");
         invalid ++;
    }

	if (val[0])
    {
		sys->m_vlan = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }
	/* vlan_interface*/	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_INTERFACE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get vlan_interface faild");
         invalid ++;
    }
	if (val[0])
    {
		sys->vlan_interface = strtoul(val, NULL, 0);       
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid mvlan INFO");
         return IPC_STATUS_FAIL;
	}
     return IPC_STATUS_OK;
}


/*****************************************************************
Function:asp_sys_mvlan
Description:the function for getting value for mvaln.asp  
Author:huangmingjian
Date:2012/11/13
INPUT:eid  wp  argc  argv     
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/

static int asp_sys_mvlan(int eid, webs_t wp, int argc, char_t **argv)
{
#if 0
	sys_network_t *info;
	ipc_sys_network_ack_t *pack;
	int ret = 0;
	
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		info = &pack->network;
		ret = websWrite(wp, T("'%u'"), info->m_vlan);	
	}
	else
	{
		DBG_PRINTF("ipc_sys_networking error\n");
		if(pack) 
		{
			free(pack);
		}
		return -1;
	}
	
	if(pack) 
	{
		free(pack);
	}	
	return ret;
#endif
    sys_mvlan_t info;
    int ret = 0;
    memset(&info,0,sizeof(info));
	if(get_sys_mvlan_value(&info) ==IPC_STATUS_OK)
	{
		ret = websWrite(wp, T("'%u'"), info.m_vlan);	    
	}
	return ret;
}

#if 0
int get_sys_loopback(sys_loop_t *sys)
{
 	int invalid = 0;
	char val[32];  

    if(sys == NULL)
    {
        return -1;
    }
	/* loop_enabled */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_LOOP_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get loop_enabled  faild");
    }

	if (val[0])
    {
		sys->loopback_enable = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }

	/* loop_update*/	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_LOOP_UPDATE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get loop_update faild");
    }
	if (val[0])
    {
		sys->loop_update = strtoul(val, NULL, 0);       
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid LOOP INFO");
		return -1;
	}
	return 1;
}
#endif
/*****************************************************************
Function:asp_sys_loopback
Description:the function for getting value for loopback_detection.asp 
Author:huangmingjian
Date:2012/11/27
INPUT:eid  wp  argc  argv     
OUTPUT:         
Return:
    -1    //error  
             
     0    //OK
Others:	       
*****************************************************************/

static int asp_sys_loopback(int eid, webs_t wp, int argc, char_t **argv)
{
 #if 0
	sys_network_t *info;
	ipc_sys_network_ack_t *pack;
	int ret = 0;

	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		info = &pack->network;
		ret = websWrite(wp, T("'%u;%u;%u;%u;%u;%u'"), info->loopback_enable,
			info->loopback_unis_status[0], info->loopback_unis_status[1],
			info->loopback_unis_status[2], info->loopback_unis_status[3],
			info->loop_update);
	}
	else
	{
		DBG_PRINTF("ipc_sys_networking error\n");
		if(pack) 
		{
			free(pack);
		}
		return -1;
	}
		
	if(pack) 
	{
		free(pack);
	}
	return ret;
	#endif
	sys_loop_t *info;
	ipc_sys_loop_ack_t *pack;
	int ret = 0;

	pack = ipc_sys_loop(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		info = &pack->loop;
		ret = websWrite(wp, T("'%u;%u;%u;%u;%u;%u'"),
		    info->loopback_enable,
			info->loopback_unis_status[0],
			info->loopback_unis_status[1],
			info->loopback_unis_status[2],
			info->loopback_unis_status[3],
			info->loop_update);	
	}
	else
	{
		DBG_PRINTF("ipc_sys_loop error\n");
		if(pack) 
		{
			free(pack);
		}
		return -1;
	}
	if(pack) 
	{
		free(pack);
}
	return ret;
}



/*
static int asp_cnu_names(int eid, webs_t wp, int argc, char_t **argv)
{
	char v[] = 
		"dev_names['CNU204N']='CNU204N';\n"		
		"dev_names['CNU204']='CNU204';\n"		
		"dev_names['CNU101']='CNU101';\n"
		"dev_names['CNU104']='CNU104';\n"
		"dev_names['STB103']='STB103';\n"
		"dev_names['CB201']='CB201';\n"
		"dev_names['CB203']='CB203';\n";
	return websWrite(wp, T("%s"), v);
}

*/

static int asp_get_web_const(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_system_ack_t *pack;
	int nvram_changes = 0;
	int sys_upgrade = 0;
	
	pack = ipc_system_req(ipc_fd, IPC_SYS_STATUS, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)){
		
		if (pack->status & IPC_SYS_STATUS_NVRAM_CHANGE){
			nvram_changes = 1;
		}
		if (pack->status & IPC_SYS_STATUS_UPGRADING){
			sys_upgrade = 1;
		}
	}
	if (pack) free(pack);
	
	return websWrite(wp, T("'%s', '%d', '%d'"), PROD_MODEL, nvram_changes, sys_upgrade);	
}


static int asp_get_upgrade_state(int eid, webs_t wp, int argc, char_t **argv)
{
	ipc_system_ack_t *pack;
	int state = UFILE_STATE_IDLE;
	int error = 0;
	int process = 0;
	char *state_str;
	
	pack = ipc_system_req(ipc_fd, IPC_SYS_UPGRADE_STATUS, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)){
		state = pack->process;
		error = pack->error;
		process = pack->percentage;
 	}
	if (pack) free(pack);

	switch(state)
	{
		case UFILE_STATE_DONE:
			state_str = "ok";
			break;
		case UFILE_STATE_FAILED:
			state_str = "fail";
			break;
		case UFILE_STATE_BOOT_ERASE:
			state_str = "e_boot";
			break;
		case UFILE_STATE_BOOT_WRITE:
		case UFILE_STATE_BOOT_VERIFY:
			state_str = "w_boot";
			break;
		case UFILE_STATE_ROOTFS_ERASE:
			state_str = "e_rootfs";
			break;			
		case UFILE_STATE_ROOTFS_WRITE:
		case UFILE_STATE_ROOTFS_VERIFY:
			state_str = "w_rootfs";
			break;
		case UFILE_STATE_KERNEL_ERASE:
			state_str = "e_kernel";
			break;			
		case UFILE_STATE_KERNEL_WRITE:
		case UFILE_STATE_KERNEL_VERIFY:			
			state_str = "w_kernel";
			break;
		default:			
			state_str = "unknown";
			break;
	}
	
	return websWrite(wp, T("'%s', '%d', '%d'"), state_str, process, error);	
}
/*Begin add by jiangzhenhua for EPN104QID0044  2012/9/24*/

extern int log_msg;
static int asp_sys_login(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp,"'%d'", log_msg);
	log_msg=0;
	return 1;
}
/*End add by jiangzhenhua for EPN104QID0044  2012/9/24*/

static int asp_sys_bakup_cfg_name(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"'%s.config'", PRODUCT_NAME);
	return 1;
}

static int asp_sys_qosconfig(int eid, webs_t wp, int argc, char_t **argv)
{
#if 0
    char val[BUF_SIZE32] = {0};
    char qosmode = 0;
    QueueMode_S qos_queuemode;
    
    memset(&qos_queuemode, 0, sizeof(QueueMode_S));
	memset(val, 0, sizeof(val));    
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_QOSMODE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS mode faild");
		return 0;
	}
	if (val[0])
	{
		qosmode = strtoul(val, NULL, 0);
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS mode faild");
		return 0;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_QUEUEMODE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queuemode faild");
		return 0;
	}
	if (val[0])
	{
		qos_queuemode.QueueMode = strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queuemode faild");
		return 0;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT1, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight1 faild");
		return 0;
	}
	if (val[0])
	{
		qos_queuemode.Weight[0]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight1 faild");
		return 0;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT2, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight2 faild");
		return 0;
	}
	if (val[0])
	{
		qos_queuemode.Weight[1]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight2 faild");
		return 0;
	}


    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT3, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight3 faild");
		return 0;
	}
	if (val[0])
	{
		qos_queuemode.Weight[2]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight3 faild");
		return 0;
	}

    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT4, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight4 faild");
		return 0;
	}
	if (val[0])
	{
		qos_queuemode.Weight[3]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight4 faild");
		return 0;
	}

    websWrite(wp, T("'%d','%d','%d;%d;%d;%d'"), qosmode, qos_queuemode.QueueMode,
        qos_queuemode.Weight[0], qos_queuemode.Weight[1],qos_queuemode.Weight[2],
        qos_queuemode.Weight[3]);
#else
    char qosmode = 0;
	int ret = 0;
    QueueMode_S qos_queuemode;
	ipc_sys_qos_ack_t *pack = NULL;
    ipc_sys_qosqueuemod_ack_t *rvpkt = NULL;
    mac_dump_ucast_tab_t  tmpbuf;

    
	pack = ipc_qos_trustmode(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        qosmode = pack->uiQosTrustmode;
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
        DBG_ASSERT(0, "Get QOS mode faild");
        return 0;
    }

    memset(&qos_queuemode, 0, sizeof(QueueMode_S));
    ret = 0;

  	rvpkt = ipc_qos_queuemode(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (rvpkt && rvpkt->hdr.status == IPC_STATUS_OK) 
    {
        qos_queuemode.QueueMode = rvpkt->Qosqueuemode.QueueMode;
        qos_queuemode.Weight[0] = rvpkt->Qosqueuemode.Weight[0];
        qos_queuemode.Weight[1] = rvpkt->Qosqueuemode.Weight[1];
        qos_queuemode.Weight[2] = rvpkt->Qosqueuemode.Weight[2];
        qos_queuemode.Weight[3] = rvpkt->Qosqueuemode.Weight[3];
		ret = IPC_STATUS_OK;
	}
    else if (rvpkt)
    {
		ret = rvpkt->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(rvpkt) 
    {   
        free(rvpkt);     
    }
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get QOS queuemod faild");
        return 0;
    }      

    websWrite(wp, T("'%d','%d','%d;%d;%d;%d'"), qosmode, qos_queuemode.QueueMode,
        qos_queuemode.Weight[0], qos_queuemode.Weight[1],qos_queuemode.Weight[2],
        qos_queuemode.Weight[3]);
//
#if 0
    int cnt = 0;
    int m = 0;
    mac_dump_ucast_tab_t* MacBuff = NULL;

    MacBuff = (mac_dump_ucast_tab_t *) malloc(sizeof(mac_dump_ucast_tab_t)*2000);
    memset(MacBuff, 0, sizeof(mac_dump_ucast_tab_t)*2000);
    
    Ioctl_GetMacDumpUcast(MacBuff, &cnt);
    
    for(m = 0; m < cnt; m++)
    {
        tmpbuf.mac_addr[0]= MacBuff->mac_addr[0];
        tmpbuf.mac_addr[1]= MacBuff->mac_addr[1];
        tmpbuf.mac_addr[2]= MacBuff->mac_addr[2];
        tmpbuf.mac_addr[3]= MacBuff->mac_addr[3];
        tmpbuf.mac_addr[4]= MacBuff->mac_addr[4];
        tmpbuf.mac_addr[5]= MacBuff->mac_addr[5];
        tmpbuf.vid = MacBuff->vid;
        tmpbuf.lport = MacBuff->lport;

        printf("mac%d  mac: %x:%x:%x:%x:%x:%x ,port:%d vid:%d\n",
        m+1,   
        tmpbuf.mac_addr[0],
        tmpbuf.mac_addr[1],
        tmpbuf.mac_addr[2],
        tmpbuf.mac_addr[3],
        tmpbuf.mac_addr[4],
        tmpbuf.mac_addr[5],
        tmpbuf.lport,
        tmpbuf.vid
        );
        MacBuff++;
    }
    
    if(MacBuff)
    {
        free(MacBuff);
    }
#endif
//
	return 1;
    
#endif
}

int Mac_firstidx = 1;
int Mac_num = 0;
int Mac_port = 1;
int Mac_port_firstidx = 1;
int Mac_port_num = 0;
int port_msk = 0;


static int asp_get_mac(int eid, webs_t wp, int argc, char_t **argv)
{
    int cnt = 0;
    int m = 0;
    int tmpcnt = 0;
    int Tcnt = 15;
    unsigned char OneItemflag = 0;
    mac_dump_ucast_tab_t* MacBuff = NULL;
    mac_dump_ucast_tab_t* MacValid = NULL;
    mac_dump_ucast_tab_t* MacDis = NULL;  
	mac_dump_ucast_tab_t* pMacBuff = NULL;
	mac_dump_ucast_tab_t* pMacValid = NULL;
    MacBuff = (mac_dump_ucast_tab_t *) malloc(sizeof(mac_dump_ucast_tab_t)*2048);
    memset(MacBuff, 0, sizeof(mac_dump_ucast_tab_t)*2048);
	pMacBuff=MacBuff;
    /*for there wil have mac of port 7*/
    MacValid = (mac_dump_ucast_tab_t *) malloc(sizeof(mac_dump_ucast_tab_t)*2048);
    memset(MacValid, 0, sizeof(mac_dump_ucast_tab_t)*2048);
    MacDis = MacValid;
    pMacValid=MacValid;
    Ioctl_GetMacDumpUcast(MacBuff, &cnt);
    
    tmpcnt = cnt;

    for(m = 0; m < 2048; m++)
    {
        if((MacBuff->lport >= 1) && (MacBuff->lport <= 4))
        {
            memcpy(MacValid, MacBuff, sizeof(mac_dump_ucast_tab_t));
            MacValid++;
        }
        else
        {
            cnt--;
        }

        MacBuff++;
        tmpcnt--;
        
        if(0 == tmpcnt)
        {
            break;
        }
    }

    if(pMacBuff)
    {
        free(pMacBuff);
    }
    Mac_num = cnt;

    if(0 == Mac_num)
    {
        if(pMacValid)
        {
            free(pMacValid);
        }
        
        DBG_ASSERT(0, "Get MAC faild\n");
        return 0;
    }

    /* parameter check*/
    if(Mac_firstidx > Mac_num)
    {
        /*get first page*/
        Mac_firstidx = 1;
    }

    MacDis += (Mac_firstidx - 1);

    /*first or last page consideration*/
    if(Mac_firstidx  == Mac_num)
    {
        OneItemflag = 1;
    }
    else if (Mac_firstidx + 15 > Mac_num)
    {
        Tcnt = Mac_num - Mac_firstidx + 1;
    }
    else
    {
        Tcnt = 15;
    }
    
    if(1 != OneItemflag)
    {
        for(m = 0; m < Tcnt-1; m++)
        {
            websWrite(wp, T("'%02x%02x-%02x%02x-%02x%02x','%d','%d','%d','%d',"), 
            MacDis->mac_addr[0],MacDis->mac_addr[1],
            MacDis->mac_addr[2],MacDis->mac_addr[3],
            MacDis->mac_addr[4],MacDis->mac_addr[5],
            0,
            MacDis->vid,
            MacDis->lport,
            0
            );
            MacDis++;
        }
    }
    
    websWrite(wp, T("'%02x%02x-%02x%02x-%02x%02x','%d','%d','%d','%d'"), 
    MacDis->mac_addr[0],MacDis->mac_addr[1],
    MacDis->mac_addr[2],MacDis->mac_addr[3],
    MacDis->mac_addr[4],MacDis->mac_addr[5],
    0,
    MacDis->vid,
    MacDis->lport,
    0
    );
    
    if(pMacValid)
    {
        free(pMacValid);
    }

    return 1;

}

static int asp_get_mac_para(int eid, webs_t wp, int argc, char_t **argv)
{
    if(Mac_num == 0)
    {
        asp_get_mac(0, 0, 0, NULL);
    }
    
    websWrite(wp, T("'%d','%d'"), Mac_num, Mac_firstidx);
    Mac_firstidx = 1;
    Mac_num = 0;

    return 1;
}

/*****************************************************************
  Function:        asp_get_mac_setting
  Description:     asp_get_mac_setting
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int asp_get_mac_setting(int eid, webs_t wp, int argc, char_t **argv)
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
		DBG_PRINTF("Get MAC aging time fail!");
	    if(pack)
	    	free(pack);
		return 0;
	}
		
    if(pack)
    	free(pack);
    websWrite(wp, T("'%d;%d'"), (aging_time?1:0), aging_time);

    return 1;
}




static int asp_get_portmac(int eid, webs_t wp, int argc, char_t **argv)
{
    int cnt = 0;
    int m = 0;
    int tmpcnt = 0;
    int Tcnt = 15;
    unsigned char OneItemflag = 0;
    mac_dump_ucast_tab_t* MacBuff = NULL;
    mac_dump_ucast_tab_t* MacValid = NULL;
    mac_dump_ucast_tab_t* MacDis = NULL;  
	 mac_dump_ucast_tab_t* pMacBuff = NULL;
    mac_dump_ucast_tab_t* pMacValid = NULL;
    MacBuff = (mac_dump_ucast_tab_t *) malloc(sizeof(mac_dump_ucast_tab_t)*2048);
    memset(MacBuff, 0, sizeof(mac_dump_ucast_tab_t)*2048);
	pMacBuff=MacBuff;
    /*for there wil have mac of port 7*/
    MacValid = (mac_dump_ucast_tab_t *) malloc(sizeof(mac_dump_ucast_tab_t)*2048);
    memset(MacValid, 0, sizeof(mac_dump_ucast_tab_t)*2048);
    MacDis = MacValid;
	pMacValid=MacValid;
    Ioctl_GetMacDumpUcast(MacBuff, &cnt);
    
    tmpcnt = cnt;

    for(m = 0; m < 2048; m++)
    {
        if(MacBuff->lport == Mac_port)
        {
            memcpy(MacValid, MacBuff, sizeof(mac_dump_ucast_tab_t));
            MacValid++;
        }
        else
        {
            cnt--;
        }

        MacBuff++;
        tmpcnt--;
        
        if(0 == tmpcnt)
        {
            break;
        }
    }

    if(pMacBuff)
    {
        free(pMacBuff);
    }
    Mac_port_num = cnt;

    /* parameter check*/
    if(Mac_port_firstidx > Mac_port_num)
    {
        /*get first page*/
        Mac_port_firstidx = 1;
    }
    
    MacDis += (Mac_port_firstidx - 1);

    if (Mac_port_firstidx == Mac_port_num)
    {
        OneItemflag = 1;
    }
    else if(Mac_port_firstidx + 15 > Mac_port_num)
    {
        Tcnt = Mac_port_num - Mac_port_firstidx + 1;
    }
    else
    {
        Tcnt = 15;
    }
    
    if(1 != OneItemflag)
    {
        for(m = 0; m < Tcnt-1; m++)
        {
        
            websWrite(wp, T("'%02x%02x-%02x%02x-%02x%02x','%d','%d','%d','%d',"), 
            MacDis->mac_addr[0],MacDis->mac_addr[1],
            MacDis->mac_addr[2],MacDis->mac_addr[3],
            MacDis->mac_addr[4],MacDis->mac_addr[5],
            0,
            MacDis->vid,
            MacDis->lport,
            0
            );
            MacDis++;
        }
    }
    
    websWrite(wp, T("'%02x%02x-%02x%02x-%02x%02x','%d','%d','%d','%d'"), 
    MacDis->mac_addr[0],MacDis->mac_addr[1],
    MacDis->mac_addr[2],MacDis->mac_addr[3],
    MacDis->mac_addr[4],MacDis->mac_addr[5],
    0,
    MacDis->vid,
    MacDis->lport,
    0
    );
    
    if(pMacValid)
    {
        free(pMacValid);
    }
    
    return 1;

}

static int asp_get_port_mac_para(int eid, webs_t wp, int argc, char_t **argv)
{
    #if 0   /*no mac will be possible*/
    if(Mac_port_num == 0)
    {
        asp_get_portmac(0, 0, 0, NULL);
    }
    #endif 
    websWrite(wp, T("'%d','%d','%d'"), Mac_port_num, Mac_port_firstidx, Mac_port);
    Mac_port_firstidx = 1;
    Mac_port_num = 0;
    Mac_port = 1;

    return 1;
}

static int asp_get_port_state(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    int arrayidx = 0;
	ipc_port_ack_t *pack = NULL;
    port_num_t lport = 0;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
    unsigned int linkduplex  = 0;
    unsigned int linkspeed = 0;
    unsigned int linkstate = 0;

    
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
    
    for(arrayidx = 0; arrayidx < LOGIC_PORT_NO - 1; arrayidx++)
    {
        lport = arrayidx + 1;
        
        Ioctl_GetPortCurrentLink(lport, &linkstate);
        Ioctl_GetPortDuplex(lport, &linkduplex);
        Ioctl_GetPortSpeed(lport, &linkspeed);
        websWrite(wp, T("'%s','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d',"), port_cfg_val[arrayidx].stIfDesc,
        port_cfg_val[arrayidx].ulIfEnable, port_cfg_val[arrayidx].stSpeed,
        port_cfg_val[arrayidx].stDuplex, port_cfg_val[arrayidx].ulFlowCtl,
        port_cfg_val[arrayidx].ulIsolateEn, port_cfg_val[arrayidx].ulMdix,
        port_cfg_val[arrayidx].ulPriority,linkspeed, linkduplex,linkstate);
    }
    
    lport++;
    Ioctl_GetPortCurrentLink(lport, &linkstate);
    Ioctl_GetPortDuplex(lport, &linkduplex);
    Ioctl_GetPortSpeed(lport, &linkspeed);
    websWrite(wp, T("'%s','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d'"), port_cfg_val[arrayidx].stIfDesc,
    port_cfg_val[arrayidx].ulIfEnable, port_cfg_val[arrayidx].stSpeed,
    port_cfg_val[arrayidx].stDuplex, port_cfg_val[arrayidx].ulFlowCtl,
    port_cfg_val[arrayidx].ulIsolateEn, port_cfg_val[arrayidx].ulMdix,
    port_cfg_val[arrayidx].ulPriority, linkspeed, linkduplex, linkstate);

    return 1;
}
static int asp_get_single_port_state(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;
    unsigned char cnt = 0;
    unsigned char arrayidx =0;
    PORT_CFG_VALUE_S port_cur[LOGIC_PORT_NO] = {0};

    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(port_msk & (1<<i))
        {
            cnt++;
            arrayidx = i;
        }
    }

    if(1 != cnt)
    {
         websWrite(wp, T("'%d','0','0','0','0','0','0','0','0'"),port_msk);
         port_msk = 0;
         return 1;
    }

    if(1 != sys_getPortCurCfg(port_cur))
    {
        DBG_PRINTF("Get port current cfg error.\n");
        websWrite(wp, T("'0','0','0','0','0','0','0','0','0'"));
        port_msk = 0;
        return 1;
    }
    //portmask, portdes, enable  speed duplex  fc IsolateEn  midx  pri
    websWrite(wp, T("'%d','%s','%d','%d','%d','%d','%d','%d','%d'"), port_msk,
    port_cur[arrayidx].stIfDesc, port_cur[arrayidx].ulIfEnable, 
    port_cur[arrayidx].stSpeed, port_cur[arrayidx].stDuplex, 
    port_cur[arrayidx].ulFlowCtl, port_cur[arrayidx].ulIsolateEn,
    port_cur[arrayidx].ulMdix, port_cur[arrayidx].ulPriority);
    port_msk = 0;
    
    return 1;
}
static int asp_get_copyright(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp,T("\"%s\""),CONFIG_COPYRIGHT_STRING );
	 return 0;
}
static int asp_get_model(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp,T("\"%s\""), CONFIG_PRODUCT_NAME);
	 return 0;
}

extern int asp_get_vlan_options(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_vlan_config(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_vlan_interface_config(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_stormctl_config(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_linerate(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_single_linerate(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_PortMibCounter(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CONFIG_PRODUCT_5500
extern int asp_get_PonStatus(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_PonMibCounter(int eid, webs_t wp, int argc, char_t **argv);
#endif
extern int asp_get_single_stormctl(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_portmirror(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_portnamelist(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_portlogic_num(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CONFIG_CATVCOM	
extern int asp_get_catv(int eid, webs_t wp, int argc, char_t **argv);
#endif							
#ifdef CONFIG_WLAN_AP	
extern int asp_get_wlan_basic(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_wlan_security(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_wlan_mac_list(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_wlan_advance(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_wlan_access_ctr(int eid, webs_t wp, int argc, char_t **argv);
extern int asp_get_wlan_client(int eid, webs_t wp, int argc, char_t **argv);
#endif


static const struct {
	char *name;
	int (*fn)(int ejid, webs_t wp, int argc, char_t **argv);
}
asp_func_table[] = {
		{"CGI_MAP_NAME",		asp_map_name},/*to be del*/	
		//{"CGI_GET_DHCP_INFO",	asp_get_dhcp_info},
		{"CGI_GET_VARIABLE",	asp_get_variable},
		{"CGI_GET_WEB_CONST",	asp_get_web_const},
		{"CGI_GET_UPGRADE_STATE", asp_get_upgrade_state},
	//	{"CGI_QUERY",			asp_query},/*to be del*/	
	//	{"CGI_GET_LINK_STATUS",	asp_get_link_status},		
	//	{"CGI_GET_PORT_MIBS",	asp_get_port_mibs},
		//{"CGI_QUERY_LINK_STATUS",	asp_query_link_status},/*to be del*/
		{"CGI_CLIENT_QUERY",	asp_client_query},
		//{"CGI_FS_DIR",			asp_fs_dir},/*to be del*/	
		//{"CGI_STA_CABLE_QUERY",	asp_sta_cable_query},
		//{"CGI_TEMPLATE_QUERY",	asp_template_query},
		//{"CGI_USER_QUERY",		asp_user_query},/*to be del*/
		{"CGI_GET_SYSLOG",		asp_get_syslog},
		{"CGI_SYS_LOG_SET",     asp_syslog_set},
		//{"CGI_CNU_STATUS",		asp_cnu_status},
		{"CGI_CNU_INFO",    	asp_cnu_info},
		{"CGI_CNU_MIBS",    	asp_cnu_mibs},		
		{"CGI_CNU_LINK_STATS",  asp_cnu_link_stats},
		{"CGI_CLT_CABLE_PARAM",	asp_clt_cable_param},	
	//	{"CGI_GET_ETH_CONFIG",	asp_get_ethernet_config},	
	//	{"CGI_GET_VLANIF_CONFIG",		asp_get_vlan_interface_config},		
	//	{"CGI_GET_VLAN_CONFIG",			asp_get_vlan_config},
	//	{"CGI_GET_VLAN_MODE_CONFIG", 	asp_get_vlan_mode_config},
	//	{"CGI_GET_USER_CONFIG", asp_get_user_config},
		{"CGI_GET_DEVINFO", 	asp_get_devinfo},
	//	{"CGI_GET_TEMPLATES",	asp_get_templates},		
		{"CGI_SYS_INFO",		asp_sys_info},
		{"CGI_SYS_TIME",		asp_sys_time},
		{"CGI_SYS_SECURITY",	asp_sys_security},
		{"CGI_SYS_SNMP_BASIC",  asp_sys_snmp_basic},	
		{"CGI_SYS_SNMP_GROUP",	asp_sys_snmp_group},
		{"CGI_SYS_SNMP_TRAP",	asp_sys_snmp_trap},	//add by zhouguanhua 2012/12/20 
		{"CGI_SYS_SNMP_USER",	asp_sys_snmp_user},
		//{"CGI_SYS_SNMP_CATVINFO",	asp_sys_snmp_catvinfo},
		{"CGI_SYS_ADMIN",		asp_sys_admin}, 	
		{"CGI_SYS_IP",			asp_sys_networking},
        {"CGI_MVLAN",           asp_sys_mvlan},  /*Add by huangmingjian 2012/11/13 for EPN104QID0082*/ 
    #ifdef CONFIG_LOOPD    
    	{"CGI_LOOPBACK",		asp_sys_loopback},  /*Add by huangmingjian 2012/11/27 for EPN104QID0083*/ 
	#endif
		//{"CGI_GET_FILES",		asp_get_files},	
		{"CGI_GET_LOGIN_ERR", 	asp_sys_login},
        {"CGI_GET_BAKUP_CFG_NAME",asp_sys_bakup_cfg_name},
        {"CGI_SYS_DEBUG",       asp_debug_info},//add by zhouguanhua 2013/5/3 
        {"CGI_SYS_SETDEBUG",    asp_setdebug_info}, //add by zhouguanhua 2012/12/20 
        {"CGI_GET_SYS_QOS",     asp_sys_qosconfig},
        {"CGI_GET_MAC",         asp_get_mac},
        {"CGI_GET_MAC_PARA",    asp_get_mac_para},
		{"CGI_GET_MAC_SETTING", asp_get_mac_setting},
        {"CGI_GET_PORTMAC",     asp_get_portmac},
        {"CGI_GET_PORTMAC_PARA",asp_get_port_mac_para},
        {"CGI_GET_PORT_CFG",    asp_get_port_state},
        {"CGI_GET_SINGLE_PORT", asp_get_single_port_state},
        {"CGI_GET_VLAN_OPTIONS",asp_get_vlan_options},
        {"CGI_GET_VLAN_CONFIG",asp_get_vlan_config},
        {"CGI_GET_VLANIF_CONFIG",asp_get_vlan_interface_config}, 
        {"CGI_GET_SINGLE_STORMCTL",asp_get_single_stormctl}, 
        {"CGI_GET_STORMCTL_CONFIG",asp_get_stormctl_config}, 
        {"CGI_GET_LINERATE", asp_get_linerate},         
        {"CGI_GET_SINGLE_LINERATE",asp_get_single_linerate},
		{"CGI_GET_IF_MIB",asp_get_PortMibCounter},
		#ifdef CONFIG_PRODUCT_5500
		{"CGI_PON_STATUS",asp_get_PonStatus},
		{"CGI_GET_PON_MIB",asp_get_PonMibCounter},
		#endif
		{"CGI_GET_MIRROR",asp_get_portmirror},
        {"CGI_GET_PORT_LOGICNUM",asp_get_portlogic_num},
        {"CGI_GET_PORT_NAME",asp_get_portnamelist},
#ifdef CONFIG_CATVCOM	
		{"CGI_SYS_CATVINFO",asp_get_catv},
#endif				
#ifdef CONFIG_WLAN_AP	
		{"CGI_WLAN_BASIC",asp_get_wlan_basic},
		{"CGI_SSID_LIST",asp_get_wlan_security},	
		{"CGI_WLAN_AP_ADVANCE",asp_get_wlan_advance},
		{"CGI_MAC_FILTER",asp_get_wlan_access_ctr},
		{"CGI_MAC_LIST",asp_get_wlan_mac_list}, 
		{"CGI_CLIENT_LIST",asp_get_wlan_client}, 		
#endif
		{"CGI_GET_COPYRIGHT",asp_get_copyright},
		{"CGI_GET_MODEL",asp_get_model},
		{NULL,					NULL},
};


void websAspFuncDefine(void)
{
	int i;
	for(i = 0; asp_func_table[i].name; i ++)
		websAspDefine(asp_func_table[i].name, asp_func_table[i].fn);
	//webs_nvram_init();
}


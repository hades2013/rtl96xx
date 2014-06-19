
#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include	<debug.h>
#include 	<config.h>
#include	<syslog.h>

#include	<cable.h>
#include	<ipc_client.h>
    //#include	<switch.h>
#include	<hex_utils.h>
    //#include	<vlan_utils.h>
#include	<str_utils.h>
#include 	"webs_nvram.h"
#include	"asp_variable.h"
#include    "pdt_config.h"
#include 	<ipc_protocol.h>
#include 	"memshare.h"
#include    <lw_config_oid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "shutils.h"
#include <time_zone.h>
#include <lw_config_api.h>
#include <lw_drv_pub.h>
#include <lw_drv_req.h>
#include <vlan_utils.h>
#include "asp_variable.h"

extern int ipc_fd;

int asp_get_vlan_options(int eid, webs_t wp, int argc, char_t **argv)
{
	 //int mvid; 
	char buf[BUF_SIZE1024*4];
	char mvid[BUF_SIZE_64];
	char vlanmode[BUF_SIZE_64];
	char vlan_type[BUF_SIZE_64];
	char szifname[BUF_SIZE_64];
	char szPseduname[BUF_SIZE_64];
	ifindex_t ifindex;
	unsigned int port;
	// cfg_getval(int ifindex,unsigned int oid,void * val,void * default_val,unsigned retlen)
	cfg_getval(IF_ROOT_IFINDEX,CONFIG_MVLAN_VID,mvid,"1",sizeof(mvid));
	cfg_getval(IF_ROOT_IFINDEX,CONFIG_VLAN_MODE,vlanmode,"0",sizeof(vlanmode));
	sprintf(buf,"'%s',",vlanmode);
	sprintf(buf+strlen(buf),"'%s'",mvid);
	LgcPortFor(port)
	{
		cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_LINKTYPE_ETH+port,vlan_type,"access",sizeof(vlan_type));
		sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,port);
        IF_GetByIFName(szifname,&ifindex);
		IF_GetWebName(ifindex,szPseduname,sizeof(szPseduname)); 
		sprintf(buf+strlen(buf),",'%s;%s'",szPseduname,vlan_type);
	}
	return websWrite(wp, T("%s"),buf);
}
extern int getValueByKey(webs_t req,char *keyword, char *getVal, unsigned int len);
#if 0
typedef struct cgi_result{
	char *msg;
	int error;	
	int nowait;
	int delay; 
}
cgi_result_t;


#define CGI_OK  0
#define CGI_ERR_ARGS  1
#define CGI_IPC_FAIL   2
#define CGI_IPC_TIMEOUT 3	
#define CGI_NOMEM    4
#define CGI_NOCMD  5
#define CGI_UNKNOWN_CMD  6

#define MSG_REBOOT "reboot"
#define MSG_UNKNOWN "unknown"
#define MSG_FAIL  "fail"
#define MSG_UPGRADE "upgrade"
#define MSG_RESTORE "restore"
#define MSG_OK "ok"




#define SET_RESULT(res, m, e, n, d)  do {\
														(res)->msg = (m); \
														(res)->error = (e); \
														(res)->nowait = (n); \
														(res)->delay = (d); \
													}while(0)

#define IPC_RESULT(pack, res) do { \
		if (pack){ \
			if (pack->hdr.status == IPC_STATUS_OK){ \
				SET_RESULT(res, MSG_OK, CGI_OK, 0, 0); \
			}else { \
				SET_RESULT(res, MSG_FAIL, CGI_IPC_FAIL, 0, 0); \
			} \
		}else { \
			SET_RESULT(res, MSG_FAIL, CGI_IPC_TIMEOUT, 0, 0); \
		} \
	}while(0)
#endif
int sys_vlanmode_handle(webs_t wp, cgi_result_t *result)
{
	char buf[BUF_SIZE_64]={0};
	switch_vlan_mode_t vmode;
	ipc_clt_vlan_mode_ack_t *pack=NULL;

	memset(&vmode, 0, sizeof(vmode));
	getValueByKey(wp,"vlan_mode",buf,sizeof(buf));
	//printf("buf=%s\n",buf);
	vmode.mode=strtoul(buf,NULL,0);
	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &vmode);
	IPC_RESULT(pack, result);
	if(pack) free(pack);

	/*Add by huangmingjian 2013-09-18*/
	/*when transparent -->802.1q, we set GE2 to trunk pvid=1,permit all, set CAB1 to trunk pvid=1,permit all*/
	if(VLAN_MODE_8021Q == vmode.mode) 	
	{
		ipc_clt_vlan_interface_config_ack_t *pack_new = NULL;
		switch_vlan_interface_t *vlan_ifs = NULL;
		char ifname[BUF_SIZE64] = {0};
		int mode = LINK_TYPE_TRUNK;
		int pvid = 1;
		
		IF_Userstr2ifame("GE2", ifname, sizeof(ifname)); 
		vlan_ifs = vlan_interface_new(ifname, mode, NULL, NULL);
		vlan_ifs->pvid = pvid;
		pack_new = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, vlan_ifs, 1);
		if (vlan_ifs) free(vlan_ifs);
		if (pack_new) free(pack_new);
		
		IF_Userstr2ifame("CAB1", ifname, sizeof(ifname)); 
		vlan_ifs = vlan_interface_new(ifname, mode, NULL, NULL);
		vlan_ifs->pvid = pvid;
		pack_new = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, vlan_ifs, 1);
		if (vlan_ifs) free(vlan_ifs);
		if (pack_new) free(pack_new);
	}
	/*End by huangmingjian 2013-09-18*/
	
	return 0;	
}
static char *vlan_member_string(logic_pmask_t member)
{
	static char buf[80];
	char tmpbuf[256];
	buf[0] = 0;
	port_num_t lport;
	int i=0;
	ifindex_t ifindex;
	char szifname[BUF_SIZE_64];
	char szPseduname[BUF_SIZE_64];
	
	LgcPortFor(lport)
	{
	
		if(TstLgcMaskBit(lport,&member)==TRUE)
		{

			sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
			IF_GetByIFName(szifname,&ifindex);
			IF_GetWebName(ifindex,szPseduname,sizeof(szPseduname)); 
			if(i==0)
			sprintf(tmpbuf,"%s",szPseduname);
			else
			sprintf(tmpbuf,",%s",szPseduname);			
			strcat(buf, tmpbuf);
			i++;
		}
	}

	
	return buf;
}
int asp_get_vlan_config(int eid, webs_t wp, int argc, char_t **argv)
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
int asp_get_vlan_interface_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int wlen = 0, i, v, vlen = 0;
	ipc_clt_vlan_interface_config_ack_t *pack = NULL;
	switch_vlan_interface_t* vlanif;
	switch_vlan_group_t *vlan_group;
//	char vs[24];
	char buffer[128];
	char tagbuf[4096*4];
	char untagbuf[4096*4];
	ifindex_t ifindex;
	char szifname[BUF_SIZE_64];
	char szPseduname[BUF_SIZE_64];
	
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
			IF_GetByIFName(vlanif->ifname,&ifindex);
			IF_GetWebName(ifindex,szPseduname,sizeof(szPseduname)); 
			vlen = sprintf(buffer, "%s;%s;%d;", szPseduname,  switchport_itoa(vlanif->mode), vlanif->pvid);

			// get buffer tag
			tagbuf[0] = 0;
			untagbuf[0] = 0;

			if (vlanif->count && (vlanif->mode == LINK_TYPE_TRUNK)){
				vlan_group = vlanif->trunk_hybrid;
				vlan_group_sprintf(tagbuf, vlan_group);
			}else if (vlanif->count && (vlanif->mode == LINK_TYPE_HYBRID)){
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
int vlan_prop_handle(webs_t wp, cgi_result_t *result)
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
	char ifname[BUF_SIZE64];
	if (vp == NULL) return NULL;
	items = split_string(vp, ';', vars);
	if (items >= 5){
			mode = switchport_atoi(vars[1]);
			pvid = strtoul(vars[2], NULL, 0);
			vlan0 = NULL;
			vlan1 = NULL;
			if (mode == LINK_TYPE_TRUNK){
				vlan0 = vlan_group_new_from_string_simple(vars[3]);
				if (vlan0) vlan0->flag = VLAN_FLAG_TAG;
			}else if(mode == LINK_TYPE_HYBRID){
				// get tag vlans 
				vlan0 = vlan_group_new_from_string_simple(vars[3]);
				if (vlan0) vlan0->flag = VLAN_FLAG_TAG;
				// get untag vlans
				vlan1 = vlan_group_new_from_string_simple(vars[4]);
			}
			IF_Userstr2ifame(vars[0],ifname,sizeof(ifname));
			vlanif = vlan_interface_new(ifname, mode, vlan0, vlan1);
			if (vlanif) vlanif->pvid = pvid;
			if (vlan0) free(vlan0);
			if (vlan1) free(vlan1);
	}
	free(vp);
	return vlanif;
}

int vlanif_prop_handle(webs_t wp, cgi_result_t *result)
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



/*****************************************************************
    Function:access_vlan_handle
    Description: access_vlan_handle
    Author:huangmingjian
    Date:2013/09/14
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/


int access_vlan_handle(webs_t wp, cgi_result_t *result)
{
    int valid = 0;
    int count = 0;
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
	switch_vlan_interface_t vlanif;
	unsigned int uiIfindex = 0;
	ipc_clt_vlan_interface_config_ack_t *pack = NULL;

	memset(&vlanif, 0 ,sizeof(switch_vlan_interface_t));
	while(1) 
    {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval)
        {      
            break;
        }
		DBG_PRINTF("%s", sval);
		cp = strchr(sval, '=');
		if (!cp)
        {   
            break;
        }
		*cp = 0;
        cp ++;

		if (!strcmp("port", sval))
		{	
			IF_Userstr2ifame(cp, vlanif.ifname, sizeof(vlanif.ifname));
		}
		else if(!strcmp("access_pid", sval))
		{	
			vlanif.pvid = strtoul(cp, NULL, 0);
		}
		else
		{
			break;
		}
		
		count++;
	}	

	if (count!= 2)
    {
		DBG_PRINTF("Invalid Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);	
		return 0;
	}

	vlanif.mode = LINK_TYPE_ACCESS;
	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &vlanif, 1);
	IPC_RESULT(pack, result);
	if(pack) 
	{
		free(pack);
	}	
	
	return 0;
}



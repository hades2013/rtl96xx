

#include "cmd_eoc.h"
#include "eoc_utils.h"
#include <lw_type.h>

#include <lw_drv_pub.h>
#include <ipc_protocol.h>


extern switch_vlan_group_t *vlan_group_to_next(switch_vlan_group_t *vg);
extern switch_vlan_group_t *vlan_group_new_from_string_simple(const char* vstr);
extern int vlan_group_sprintf(char *buffer, switch_vlan_group_t *vlan_group);
#define VTY_MAX_VLAN_STRING_SIZE_PER_LINE 50

/*

vlan [VSTR] // create VLAN
no vlan [VSTR] // delete VLAN
show vlan [ALL | VSTR]
*/
 //add by zhouguanhua 2013/5/31
static char *vlan_member_string(logic_pmask_t member)
{
	static char buf[80];
	char tmpbuf[256];
	buf[0] = 0;
	port_num_t lport;
	int i=0;
	LgcPortFor(lport)
	{
	
		if(TstLgcMaskBit(lport,&member)==TRUE)
		{
			if(i==0)
			{
				if(PT_TP((PORT_TYPE((port_num_t)lport)))==TP_CABLE)
				{
					sprintf(tmpbuf,"%s%d",CLI_CABLE,(lport-FE_PORT_NO-GE_PORT_NO));
				}
				else 
				{
					if(PT_SP((PORT_TYPE((port_num_t)lport)))==SP_1000)
					sprintf(tmpbuf,"%s%d",CLI_GIGAPORTPREFIX,lport);
					else
					sprintf(tmpbuf,"%s%d",CLI_FASTPORTPREFIX,lport);
				}
				
			}
			else
			{
				if(PT_TP((PORT_TYPE((port_num_t)lport)))==TP_CABLE)
				{
					sprintf(tmpbuf,",%s%d",CLI_CABLE,(lport-FE_PORT_NO-GE_PORT_NO));
				}
				else
				{
					if(PT_SP((PORT_TYPE((port_num_t)lport)))==SP_1000)
					sprintf(tmpbuf,",%s%d",CLI_GIGAPORTPREFIX,lport);
					else
					sprintf(tmpbuf,",%s%d",CLI_FASTPORTPREFIX,lport);	
				}
			}
			strcat(buf, tmpbuf);
			i++;
		}
	}

	
	return buf;
}


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


void vty_show_transparent_vlan(struct vty* vty, switch_vlan_mode_t *vmode)
{
	vty_out(vty, "	     Vlan information	 %s", VTY_NEWLINE);
	vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, " Vlan in transparent mode %s", VTY_NEWLINE);	
	vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);	
}


void vty_show_dot1q_vlans(struct vty* vty, switch_vlan_group_t *vlan_groups, int count)
{
	int i;
	switch_vlan_group_t *vg, *vg_unexist = NULL;
	
//	vty_out(vty, "	     VLAN INFORMATION	 %s", VTY_NEWLINE);
	vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
	vg = vlan_groups;
	
	for(i = 0; i < count; i ++) {
		if (LgcMaskNotNull(&(vg->member))==TRUE) {
	   	  vty_out_vlans(vty, "Vlan       :", vg); 		
			vty_out(vty, "Ports      : %s%s", vlan_member_string(vg->member), VTY_NEWLINE);
			//vty_out(vty, " VLAN alias : %s%s",  "default", VTY_NEWLINE);
			vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
		}else {
			vg_unexist = vg;
		}
		vg = vlan_group_to_next(vg);
	}
	
	if (vg_unexist){
//		vty_out(vty, "Vlan :%s", VTY_NEWLINE);
		vty_out_vlans(vty, "Vlan      :  ", vg_unexist);
		vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
	}
	vty_out(vty, " Total %d Vlan Groups%s", vg_unexist ? count - 1 : count , VTY_NEWLINE);

}




DEFUN (	vlan,
			vlan_cmd,
			"vlan VLANLIST",
			STR(vlan)
			STR(vlanlist)
		)
{

	int del = 0, ret;
	switch_vlan_group_t *vg;
	switch_vlan_group_t *vlan1_vg;


	if (strstr(self->string, "no")){
		del = 1;
	}

	if (!vlan_string_check(argv[0])){
	   vty_out(vty, "Invalid VLAN list string%s", VTY_NEWLINE);
	   return CMD_SUCCESS;
   }

	/*Begin modified by huangmingjian 2013/9/06 for add or del vlan checking*/
	ipc_clt_vlan_config_ack_t *pack = NULL;
	int include_vlan1 = 0;
	
	vg = vlan_group_new_from_string_simple(argv[0]);
	VTY_NOMEM_ASSERT(vg);

	vlan1_vg = vlan_group_new_from_string_simple("1");
	VTY_NOMEM_ASSERT(vg);
	
	if(1 == any_vlan_in_groups(vlan1_vg, vg, 1))
	{
		include_vlan1 = 1;
	}
	if(vlan1_vg) free(vlan1_vg);
	
	if(del)
	{
		if(include_vlan1)
		{
			vty_out(vty, "Delete vlan 1 is not allowed!%s", VTY_NEWLINE);
			if(vg) free(vg);
			return CMD_SUCCESS; 
		}
		
		if (!all_vlan_exist(vg, &ret))
		{
			VTY_IPC_ASSERT(ret);
			vty_out(vty, "One or more ID in VLAN list does(do) not exist!%s", VTY_NEWLINE);
			if(vg) free(vg);
			return CMD_SUCCESS;
		}
	}
	else
	{
		pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL, 0);
		if (pack && (pack->hdr.status == IPC_STATUS_OK)) 
		{	
			if(1 == any_vlan_in_groups(vg, pack->vlan_groups, pack->count))
			{
				vty_out(vty, "One or more ID in VLAN list has(have) exist!%s", VTY_NEWLINE);
				if(pack) free(pack);
				if(vg) free(vg);
				return CMD_SUCCESS; 
			}
		}
		else 
		{
			vty_out(vty, "Get vlans fail!%s", VTY_NEWLINE);
		}
		if(pack) free(pack);
	}
	/*End add by huangmingjian 2013/9/06 for add or del vlan checking*/

	ret = ipc_set_vlan(ipc_fd,del ? IPC_CONFIG_DEL : IPC_CONFIG_NEW, vg);
	if(vg) free(vg);

	VTY_IPC_ASSERT(ret);

	return CMD_SUCCESS;
}



ALIAS (	vlan,
			no_vlan_cmd,
			"no vlan VLANLIST",
			STR(no)
			STR(vlan)
			STR(vlanlist)
		);



DEFUN (	show_vlan,
			show_vlan_cmd,
			"show vlan (all | VLANLIST)",
			STR(show)
			STR(vlan)
			STR(all)
			STR(vlanlist)
		)
{
	int ret, vvalid = 0;
	ipc_clt_vlan_config_ack_t *pack;
	switch_vlan_group_t *vg;
	switch_vlan_mode_t vmode;

	if (!strcmp(argv[0], "all")){
		vg = NULL;
	}else if (!vlan_string_check(argv[0])){
		vty_out(vty, "Invalid VLAN list string%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}else {
		vvalid = 1;
	}

	ret = ipc_get_vlan_mode(ipc_fd,&vmode);	
	VTY_IPC_ASSERT(ret);
	
	if (vmode.mode == VLAN_MODE_TRANSPARENT){
		vty_show_transparent_vlan(vty, &vmode);
		return CMD_SUCCESS;
	}

	
	if (vvalid){
		vg = vlan_group_new_from_string_simple(argv[0]);
		VTY_NOMEM_ASSERT(vg);
	}
	
	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, vg, vg ? 1 : 0);
	//printf("pack->count %d\n",pack->count);
	if (vg) free(vg);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		vty_show_dot1q_vlans(vty, pack->vlan_groups, pack->count);
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);
	
	VTY_IPC_ASSERT(ret);

	return CMD_SUCCESS;
}



DEFUN (	transparent_vlan,
			transparent_vlan_cmd,
			"transparent-vlan",
			STR(transparent-vlan)
		)
{
	int ret;
	switch_vlan_mode_t vmode;
	
	memset(&vmode, 0, sizeof(vmode));

	if (strstr(self->string, "no")){
		vmode.mode = VLAN_MODE_8021Q;
	}else {
		vmode.mode = VLAN_MODE_TRANSPARENT;
	}

	ret = ipc_set_vlan_mode(ipc_fd,&vmode);
	VTY_IPC_ASSERT(ret);
	
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

	return CMD_SUCCESS;
}

ALIAS (	transparent_vlan,
			no_transparent_vlan_cmd,
			"no transparent-vlan",
			NO_STR
			STR(transparent-vlan)
		);


 //add by zhouguanhua 2013/5/31


void cmd_vlan_init(void)
{
	#ifdef SWITCH_STYLE
	install_element(CONFIG_NODE, &show_vlan_cmd);
	install_element(CONFIG_NODE, &no_vlan_cmd);
	install_element(CONFIG_NODE, &vlan_cmd);
	install_element(CONFIG_NODE, &transparent_vlan_cmd);	
	install_element(CONFIG_NODE, &no_transparent_vlan_cmd);
	install_element(ENABLE_NODE, &show_vlan_cmd);
	#endif
}





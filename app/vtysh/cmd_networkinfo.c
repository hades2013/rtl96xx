

#include "cmd_eoc.h"
#include "eoc_utils.h"

/*

show cnuinfo cnu 1/2
show cnuinfo cnu 1 brief
show cnuinfo cnu 2 brief
show cnuinfo cnu all   networkinfo

show cnuinfo cnu 1/2 link-stats
show cnuinfo cnu 1/2 port-stats

*/
int parse_cnu_id(const char *str, int *clt, int *cnu)
{
	int ret = 0;
	const char *cp;
	char *p, *s;

	cp = str;
	while((*cp != '/') && (*cp != '\0')){
		if (!isdigit(*cp)){
			return 0;			
		}
		cp ++;
	}
	cp ++;
	while(*cp != '\0'){
		if (!isdigit(*cp)){
			return 0;			
		}
		cp ++;		
	}

	s = strdup(str);
	
	if ((p = strchr(s, '/'))){
		*p = '\0';
		p ++;
		*clt = strtoul(s, NULL, 0);
		*cnu = strtoul(p, NULL, 0);

		if (in_range(*clt, 1, MAX_CLT_CHANNEL) 
			&& in_range(*cnu, 1, MAX_CNU_PER_CLT)){	
			ret = 1;
		}
	}
	free(s);
	
	return ret;
}

static char *ether_addr_string(char *buf, ethernet_addr_t *e)
{
	sprintf(buf, "%02X%02X%02X-%02X%02X%02X", e->octet[0], e->octet[1], e->octet[2], e->octet[3], e->octet[4], e->octet[5]);
	return buf;
}

static char *online_tm_string(time_t t)
{
	static char buf[40];
	time_t sec = time(NULL) - t;
	sprintf(buf, "%ldd %02ld:%02ld:%02ld", sec / (24 * 3600), (sec % (24 * 3600)) / 3600, (sec % 3600) / 60, sec % 60);
	return buf;
}



void vty_show_networkinfo(struct vty* vty, ipc_cnu_info_t *cnus, int count)
{
	int i;
	char macstr[24];
	ipc_cnu_info_t *pcnu;
	
	vty_out(vty, " CNU   MAC ADDRESS    TEI  MODEL           LINK     TX(M)  RX(M)  ONLINE TIME%s", VTY_NEWLINE);
	vty_out(vty, " ----  -------------  ---  --------------  -------  -----  -----  --------------%s", VTY_NEWLINE);

	//			   1/1  00B052-80000A CB203-A       UP   14
	for(i = 0; i < count; i ++) {
		pcnu = &cnus[i];
		vty_out(vty, " %d/%-2d  %s   %-2d  %-14s  %-7s  %-5d  %-5d  %s%s",
				pcnu->clt,
				pcnu->index,
				ether_addr_string(macstr, &pcnu->mac),
				pcnu->tei,
				pcnu->alias,
				CNU_LINK_STR(pcnu->link),
				pcnu->avgtx, pcnu->avgrx,
				pcnu->link == CNU_LINK_UP ? online_tm_string(pcnu->online_tm) : "N/A",
				VTY_NEWLINE);
	}
	vty_out(vty, " -------------------------------------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, " Total %d CNUs listed%s", count, VTY_NEWLINE);	
}



void vty_show_cnu_link_stats(struct vty* vty, ipc_cnu_info_t *info, cable_link_stats_t* link)
{
	int vlink = 0, vtone = 0;
	vlink = (link->valid & 0x01) ? 1 : 0;
	vtone = (link->valid & 0x02) ? 1 : 0;

	vty_out(vty, "  CNU LINK STATISTICS %d/%d%s", info->clt, info->index, VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, " Averaged Pre-FEC Bit Error Rate : %s%s", vlink ? FLOAT_STR(link->pre_fec, "%") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged Bits/Carrier           : %s%s", vtone ? FLOAT_STR(link->bits_carrier,"") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged SNR/Carrier            : %s%s", vtone ? FLOAT_STR(link->snr_carrier,"") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged Tx CRC Error Rate      : %s%s", vlink ? FLOAT_STR(link->tx_pbs_err,"%") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged Rx CRC Error Rate      : %s%s", vlink ? FLOAT_STR(link->rx_pbs_err,"%") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged Attenuation            : %s%s", vtone ? FLOATT_STR(link->avg_att," dB") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Averaged Output Power           : %s%s", vtone ? FLOAT_STR(link->avg_txpwr," dBuV") : "N/A", VTY_NEWLINE);
	vty_out(vty, " Total Tx Packets                : %llu%s", link->txpkt, VTY_NEWLINE);
	vty_out(vty, " Total Tx Failed                 : %llu%s", link->txerr, VTY_NEWLINE);
	vty_out(vty, " Total Rx Packets                : %llu%s", link->rxpkt, VTY_NEWLINE);	
	vty_out(vty, " Total Rx Failed                 : %llu%s", link->rxerr, VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------------%s", VTY_NEWLINE);	
}


void vty_show_cnu_port_stats(struct vty* vty, ipc_cnu_info_t *info, ipc_cnu_port_mib_t *pmib, int pnum)
{
	int i;
	char name[24];
	vty_out(vty, "  CNU PORT STATISTICS %d/%d:%s", info->clt, info->index, VTY_NEWLINE);
	vty_out(vty, " PORT  LINK SPEED DUPLEX TX PACKET  RX PACKET  RX DROP    RX CRC     RX FRAG%s", VTY_NEWLINE);
	vty_out(vty, " ----- ---- ----- ------ ---------- ---------- ---------- ---------- ----------%s", VTY_NEWLINE);
	
	for(i = 0; i < pnum; i ++) {
		if (i == 0){
			strcpy(name, "Cable");
		}else {
			sprintf(name, "Eth%d", i);
		}
		vty_out(vty, " %-5s %-4s %-5s  %-5s %-10d %-10d %-10d %-10d %-10d%s",
				name,
				pmib[i].link ? "UP" : "DOWN",
				pmib[i].link ? (pmib[i].spd ? "100M" : "10M") : "N/A",
				pmib[i].link ? (pmib[i].duplex ? "Full" : "Half") : "N/A",
				pmib[i].txpacket,
				pmib[i].rxpacket,
				pmib[i].rxdrop,
				pmib[i].rxcrc,
				pmib[i].rxfrag,
				VTY_NEWLINE);
	}
	vty_out(vty, " ------------------------------------------------------------------------------%s", VTY_NEWLINE);
}

void vty_show_cnu_info(struct vty* vty, ipc_cnu_info_t *info)
{
	char macstr[24];
	vty_out(vty, "  CNU INFO %d/%d%s", info->clt, info->index, VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, " MAC Address              : %s%s", ether_addr_string(macstr, &info->mac), VTY_NEWLINE);
	vty_out(vty, " Model Number             : %s%s", info->alias, VTY_NEWLINE);
	vty_out(vty, " Firmware Version         : %s%s", info->version, VTY_NEWLINE);
	vty_out(vty, " Authentication Status    : %s%s", CNU_AUTH_STR(info->auth), VTY_NEWLINE);
	vty_out(vty, " Link Status              : %s%s", CNU_LINK_STR(info->link), VTY_NEWLINE);
	vty_out(vty, " Average TX Rate          : %d Mbps%s", info->avgtx, VTY_NEWLINE);
	vty_out(vty, " Average RX Rate          : %d Mbps%s", info->avgrx, VTY_NEWLINE);
	vty_out(vty, " Device ID                : %s%s", CNU_DEVID_STR(info->device_id), VTY_NEWLINE);
	vty_out(vty, " Template ID              : %s%s", CNU_TMPLID_STR(info->template_id), VTY_NEWLINE);
	vty_out(vty, " User Name                : %s%s", info->username, VTY_NEWLINE);	
	vty_out(vty, " Online Time              : %s%s", info->link == CNU_LINK_UP ? online_tm_string(info->online_tm) : "N/A", VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------------%s", VTY_NEWLINE);	
}




DEFUN (	show_cnuinfo_clt,
			show_cnuinfo_clt_cmd,
			"show cnuinfo clt (all|<1-2>)",
			SHOW_STR
			STR(cnuinfo)
			STR(clt)
			STR(all)
			STR(clt_id)
		)
{
	ipc_network_info_ack_t *pack;
	int ret;
	int id;
	if (!strcmp(argv[0], "all")){
		id = 0xff;
	}else {
		id = strtoul(argv[0], NULL, 0);
	}
	pack = ipc_network_info(ipc_fd, id, 0);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		vty_show_networkinfo(vty, pack->cnus, pack->count);
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



DEFUN (	show_cnuinfo_cnu,
			show_cnuinfo_cnu_id_cmd,
			"show cnuinfo cnu CNUID",
			SHOW_STR
			STR(cnuinfo)
			STR(cnu)
			STR(cnuid)
		)
{
	int ret, clt, cnu, status;
	ipc_cnu_info_t info;
		
	ret = parse_cnu_id(argv[0], &clt, &cnu);

	if (!ret){
		vty_out(vty, "%% Invalid CNU ID%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	ret = ipc_get_cnu_info(clt, cnu, &info, &status);
	VTY_IPC_ASSERT(ret);
	VTY_CNU_ASSERT(status);

	if (strstr(self->string, "link-stats")){
		cable_link_stats_t link;
		ret = ipc_get_cnu_link_stats(clt, cnu, &link, &status);
		VTY_IPC_ASSERT(ret);
		VTY_CNU_ASSERT(status);
		vty_show_cnu_link_stats(vty, &info, &link);
		return CMD_SUCCESS;
	}else if (strstr(self->string, "port-stats")){
		ipc_cnu_port_mib_t pmib[CNU_MAX_ETH_PORTS + 1];
		int num;
		ret = ipc_get_cnu_port_stats(clt, cnu, pmib, &num, &status);
		VTY_IPC_ASSERT(ret);
		VTY_CNU_ASSERT(status);
		vty_show_cnu_port_stats(vty, &info, pmib, num);
		return CMD_SUCCESS;		
	}
	vty_show_cnu_info(vty, &info);
	
	return CMD_SUCCESS;
}


ALIAS (	show_cnuinfo_cnu,
			show_cnuinfo_cnu_link_stats_cmd,
			"show cnuinfo link-stats cnu CNUID",
			SHOW_STR
			STR(cnuinfo)
			STR(link-stats)				
			STR(cnu)
			STR(cnuid)	
		);

ALIAS (	show_cnuinfo_cnu,
			show_cnuinfo_cnu_port_stats_cmd,
			"show cnuinfo port-stats cnu CNUID",
			SHOW_STR
			STR(cnuinfo)
			STR(port-stats)				
			STR(cnu)
			STR(cnuid)	
		);


void cmd_networkinfo_init(void)
{/*
	install_element(ENABLE_NODE, &show_cnuinfo_clt_cmd);
	install_element(ENABLE_NODE, &show_cnuinfo_cnu_id_cmd);
	install_element(ENABLE_NODE, &show_cnuinfo_cnu_link_stats_cmd);
	install_element(ENABLE_NODE, &show_cnuinfo_cnu_port_stats_cmd);
	
	install_element(CONFIG_NODE, &show_cnuinfo_clt_cmd);
	install_element(CONFIG_NODE, &show_cnuinfo_cnu_id_cmd);
	install_element(CONFIG_NODE, &show_cnuinfo_cnu_link_stats_cmd);	
	install_element(CONFIG_NODE, &show_cnuinfo_cnu_port_stats_cmd);*/
}





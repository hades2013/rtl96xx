/*
 * cnu.c
 *
 *  Created on: Mar 5, 2011
 *      Author: root
 */

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "vty.h"
#include <ipc_client.h>
#include <cable.h>
#include <debug.h>

extern int ipc_fd;





DEFUN(show_clt,
		show_clt_cmd,
		"show clt <1-2>",
		SHOW_STR
		"Show CLT information\n"
		"CLT index number\n")
{
	return CMD_SUCCESS;
}

DEFUN(show_cnu,
		show_cnu_cmd,
		"show cnu clt (<1-2>|ALL)",
		SHOW_STR
		"Display CNU information\n"
		"Display CNU information of specific CLT\n"
		"The CLT identify number\n"
		"All CLTs\n")
{
	int clt = 0xff, i;
	ipc_network_info_ack_t *pack = NULL;
	ipc_cnu_info_t *pcnu;
	char macstr[64];

	if(strcasecmp(argv[0], "all")) {
		clt = strtoul(argv[0], NULL, 0);
		if(!CLT_ID_RANGE_CHECK(clt)) {
			vty_out(vty, "ERROR: Invalid CLT index!%s", VTY_NEWLINE);
			goto out;
		}
	}

	pack = ipc_network_info(ipc_fd, clt, 0);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		if(pack->count > 0) {
			vty_out(vty, "CNU  MAC ADDRESS   MODEL        LINK CONFIG STATUS   TX(M) RX(M) ONLINE TIME%s", VTY_NEWLINE);
			vty_out(vty, "---  ------------- ------------ ---- --------------- ----- ----- --------------%s", VTY_NEWLINE);
//			              1/1  00B052-80000A CB203-A     UP   14
			for(i = 0; i < pack->count; i ++) {
				pcnu = &pack->cnus[i];
				vty_out(vty, "%d/%-2d %s %-12s %-4s %-15s %5d %5d %s%s",
						pcnu->clt,
						pcnu->index,
						ether_addr_string(macstr, &pcnu->mac),
						pcnu->hfid,
						pcnu->link == CNU_LINK_STATUS_UP ? "UP" : "DOWN",
						cnu_state_string[pcnu->state].short_state_string,
						pcnu->avgtx, pcnu->avgrx,
						pcnu->link == CNU_LINK_STATUS_UP ? online_tm_string(pcnu->online_tm) : "N/A",
						VTY_NEWLINE);
			}
			vty_out(vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE);
			vty_out(vty, "Total %d CNUs listed.%s", pack->count, VTY_NEWLINE);
		}
		else {
			vty_out(vty, "No CNU found.%s", VTY_NEWLINE);
		}
		goto out;
	}

	vty_out(vty, "ERROR: IPC communication fail!%s", VTY_NEWLINE);

out:
	if(pack) free(pack);
	return CMD_SUCCESS;
}

DEFUN(show_cnu_info,
		show_cnu_info_cmd,
		"show cnu properties WORD",
		SHOW_STR
		"Display CNU information\n"
		"Display CNU properties\n"
		"Display information of specific CNU (CLT ID/CNU ID)"
		)
{
	char *cp;
	int clt, cnu, ipcerr, i, ports;
	ipc_cnu_info_ack_t *pack = NULL;
	char macstr[64];

	cp = strchr(argv[0], '/');
	if(!cp) {
		vty_out(vty, "ERROR: Invalid CNU expression, must be CLT ID/CNU ID!%s", VTY_NEWLINE);
		goto out;
	}
	*cp = 0;
	cp ++;
	clt = strtoul(argv[0], NULL, 0);
	cnu = strtoul(cp, NULL, 0);
	if(!CLT_ID_RANGE_CHECK(clt)) {
		vty_out(vty, "ERROR: Invalid CLT identify number!%s", VTY_NEWLINE);
		goto out;
	}
	if(!CNU_ID_RANGE_CHECK(cnu)) {
		vty_out(vty, "ERROR: Invalid CNU identify number!%s", VTY_NEWLINE);
		goto out;
	}

	pack = ipc_cnu_info(ipc_fd, FILTER_TYPE_ID, clt, cnu, NULL);
	if(!pack) {
		vty_out(vty, "ERROR: IPC communication fail!%s", VTY_NEWLINE);
		goto out;
	}
	ipcerr = pack->hdr.status;
	if(ipcerr != IPC_STATUS_OK) {
		switch(ipcerr) {
		case IPC_STATUS_NOCLT:
			vty_out(vty, "ERROR: CLT device is not available%s", VTY_NEWLINE);
			break;
		case IPC_STATUS_NOCNU:
			vty_out(vty, "ERROR: CNU device is not available%s", VTY_NEWLINE);
			break;
		default:
			vty_out(vty, "ERROR: %s%s", ipc_strerror(ipcerr), VTY_NEWLINE);
		}
		goto out;
	}
	vty_out(vty, "Information of CNU %d/%d:%s", clt, cnu, VTY_NEWLINE);
	vty_out(vty, "-------------------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, "MAC address              : %s%s", ether_addr_string(macstr, &pack->cnu_info.mac), VTY_NEWLINE);
	vty_out(vty, "Model number             : %s%s", pack->cnu_info.hfid, VTY_NEWLINE);
	vty_out(vty, "Device alias             : %s%s", pack->cnu_info.alias, VTY_NEWLINE);
	vty_out(vty, "Firmware version         : %s%s", pack->cnu_info.version, VTY_NEWLINE);
	vty_out(vty, "Authentication privilege : %s%s", auth_state_string[pack->cnu_info.auth], VTY_NEWLINE);
	vty_out(vty, "Link status              : %s%s", pack->cnu_info.link == CNU_LINK_STATUS_UP ? "UP" : "DOWN", VTY_NEWLINE);
	vty_out(vty, "Configuration status     : %s%s", cnu_state_string[pack->cnu_info.state].fancy_state_string, VTY_NEWLINE);
	vty_out(vty, "Average TX rate          : %dMbps%s", pack->cnu_info.avgtx, VTY_NEWLINE);
	vty_out(vty, "Average RX rate          : %dMbps%s", pack->cnu_info.avgrx, VTY_NEWLINE);
	vty_out(vty, "Online time              : %s%s", pack->cnu_info.link == CNU_LINK_STATUS_UP ? online_tm_string(pack->cnu_info.online_tm) : "N/A",
			VTY_NEWLINE);
out:
	if(pack) free(pack);
	return CMD_SUCCESS;
}

DEFUN(show_cnu_stat,
		show_cnu_stat_cmd,
		"show cnu statistics WORD",
		SHOW_STR
		"Display CNU information\n"
		"Display CNU statistics\n"
		"Display statistics of specific CNU (CLT ID/CNU ID)"
		)
{
	char *cp;
	int clt, cnu, ipcerr, i, ports;
	ipc_cnu_mib_ack_t *pack = NULL;
	char macstr[64];

	cp = strchr(argv[0], '/');
	if(!cp) {
		vty_out(vty, "ERROR: Invalid CNU expression, must be CLT ID/CNU ID!%s", VTY_NEWLINE);
		goto out;
	}
	*cp = 0;
	cp ++;
	clt = strtoul(argv[0], NULL, 0);
	cnu = strtoul(cp, NULL, 0);
	if(!CLT_ID_RANGE_CHECK(clt)) {
		vty_out(vty, "ERROR: Invalid CLT identify number!%s", VTY_NEWLINE);
		goto out;
	}
	if(!CNU_ID_RANGE_CHECK(cnu)) {
		vty_out(vty, "ERROR: Invalid CNU identify number!%s", VTY_NEWLINE);
		goto out;
	}

	pack = ipc_cnu_mib(ipc_fd, 0, FILTER_TYPE_ID, clt, cnu, NULL);
	if(!pack) {
		vty_out(vty, "ERROR: IPC communication fail!%s", VTY_NEWLINE);
		goto out;
	}
	ipcerr = pack->hdr.status;
	if(ipcerr != IPC_STATUS_OK) {
		switch(ipcerr) {
		case IPC_STATUS_NOCLT:
			vty_out(vty, "ERROR: CLT device is not available%s", VTY_NEWLINE);
			break;
		case IPC_STATUS_NOCNU:
			vty_out(vty, "ERROR: CNU device is not available%s", VTY_NEWLINE);
			break;
		case IPC_STATUS_CNUDOWN:
			vty_out(vty, "ERROR: CNU is not link up%s", VTY_NEWLINE);
		default:
			vty_out(vty, "ERROR: %s%s", ipc_strerror(ipcerr), VTY_NEWLINE);
		}
		goto out;
	}
	vty_out(vty, "Statistics of CNU %d/%d:%s", clt, cnu, VTY_NEWLINE);
	ports = pack->ports;
	vty_out(vty, "PORT  LINK SPEED DUPLEX TX PACKET  RX PACKET  RX DROP    RX CRC     RX FRAG%s", VTY_NEWLINE);
	vty_out(vty, "----- ---- ----- ------ ---------- ---------- ---------- ---------- ----------%s", VTY_NEWLINE);
	for(i = 0; i < ports; i ++) {
		if(i == 0)
			strcpy(macstr, "cable");
		else
			sprintf(macstr, "eth%d", i);
		vty_out(vty, "%-5s %4s %5s  %5s %10d %10d %10d %10d %10d%s",
				macstr,
				pack->cnu_mib[i].link ? "UP" : "DOWN",
				pack->cnu_mib[i].link ? (pack->cnu_mib[i].spd ? "100M" : "10M") : "N/A",
				pack->cnu_mib[i].link ? (pack->cnu_mib[i].duplex ? "FULL" : "HALF") : "N/A",
				pack->cnu_mib[i].txpacket,
				pack->cnu_mib[i].rxpacket,
				pack->cnu_mib[i].rxdrop,
				pack->cnu_mib[i].rxcrc,
				pack->cnu_mib[i].rxfrag,
				VTY_NEWLINE);
	}
out:
	if(pack) free(pack);
	return CMD_SUCCESS;
}

DEFUN(clean_cnu_stat,
		clean_cnu_stat_cmd,
		"clean cnu statistics WORD",
		"Clean statistics or records\n"
		"Clean statistics or records of CNU\n"
		"Clean statistics of CNU\n"
		"Clean statistics of specific CNU (CLT ID/CNU ID)\n"
		)
{
	char *cp;
	int clt, cnu, ipcerr, i, ports;
	ipc_cnu_mib_ack_t *pack = NULL;

	cp = strchr(argv[0], '/');
	if(!cp) {
		vty_out(vty, "ERROR: Invalid CNU expression, must be CLT ID/CNU ID!%s", VTY_NEWLINE);
		goto out;
	}
	*cp = 0;
	cp ++;
	clt = strtoul(argv[0], NULL, 0);
	cnu = strtoul(cp, NULL, 0);
	if(!CLT_ID_RANGE_CHECK(clt)) {
		vty_out(vty, "ERROR: Invalid CLT identify number!%s", VTY_NEWLINE);
		goto out;
	}
	if(!CNU_ID_RANGE_CHECK(cnu)) {
		vty_out(vty, "ERROR: Invalid CNU identify number!%s", VTY_NEWLINE);
		goto out;
	}

	pack = ipc_cnu_mib(ipc_fd, 1, FILTER_TYPE_ID, clt, cnu, NULL);
	if(!pack) {
		vty_out(vty, "ERROR: IPC communication fail!%s", VTY_NEWLINE);
		goto out;
	}
	ipcerr = pack->hdr.status;
	if(ipcerr != IPC_STATUS_OK) {
		switch(ipcerr) {
		case IPC_STATUS_NOCLT:
			vty_out(vty, "ERROR: CLT device is not available%s", VTY_NEWLINE);
			break;
		case IPC_STATUS_NOCNU:
			vty_out(vty, "ERROR: CNU device is not available%s", VTY_NEWLINE);
			break;
		case IPC_STATUS_CNUDOWN:
			vty_out(vty, "ERROR: CNU is not link up%s", VTY_NEWLINE);
		default:
			vty_out(vty, "ERROR: %s%s", ipc_strerror(ipcerr), VTY_NEWLINE);
		}
		goto out;
	}
	out:
		if(pack) free(pack);
		return CMD_SUCCESS;
}

void cnu_cmd_init(void)
{/*
	install_element(ENABLE_NODE, &show_cnu_cmd);
	install_element(ENABLE_NODE, &show_cnu_info_cmd);
	install_element(ENABLE_NODE, &show_cnu_stat_cmd);
	install_element(ENABLE_NODE, &clean_cnu_stat_cmd);*/
}

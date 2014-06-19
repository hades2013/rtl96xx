/*
 * superuser.c
 *
 *  Created on: Mar 4, 2011
 *      Author: root
 */

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "vty.h"
#include <ipc_client.h>

#define	MME_STR		"MME commands\n"

extern int ipc_fd;

static struct cmd_node su_node =
		{ SU_NODE, "%s! ", NULL, 1, };

static int su_config_write(struct vty *vty) 
{ 
	vty_out(vty, "!%s", VTY_NEWLINE);
}

DEFUN(superuser,
		superuser_cmd,
		"su",
		"Enter super user mode\n")
{
	vty->node = SU_NODE;
	return CMD_SUCCESS;
}

DEFUN(mme_stat,
		mme_stat_cmd,
		"mme statistics",
		MME_STR
		"MME statistics\n")
{
	uint8_t clean;
	ipc_mme_stat_ack_t *pack = NULL;

	clean = strstr(self->string, "clean") != NULL;
	pack = ipc_mme_stat(ipc_fd, clean);
	if(pack) {
		if(!clean) {
			vty_out(vty, "MME packet sent         : %d%s", pack->send, VTY_NEWLINE);
			vty_out(vty, "MME packet received     : %d%s", pack->recv, VTY_NEWLINE);
			vty_out(vty, "MME packet acknowledged : %d%s", pack->ack, VTY_NEWLINE);
			vty_out(vty, "MME packet failed       : %d%s", pack->fail, VTY_NEWLINE);
			vty_out(vty, "MME queues in use       : %d%s", pack->queues, VTY_NEWLINE);
			vty_out(vty, "MME queues usage peak   : %d%s", pack->queues_peak, VTY_NEWLINE);
		}
		free(pack);
	}
	else {
		vty_out(vty, "IPC connection failed.%s", VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}

ALIAS(mme_stat,
		mme_stat_clean_cmd,
		"mme statistics clean",
		MME_STR
		"MME statistics\n"
		"Clean statistics\n")

void su_cmd_init(void) {
	/* Install super user node. */
	install_node(&su_node, su_config_write);

	/* Install commands. */
//	install_element(ENABLE_NODE, &superuser_cmd);
	install_element(SU_NODE, &config_list_cmd);
	install_element(SU_NODE, &config_exit_cmd);
	install_element(SU_NODE, &config_quit_cmd);
	install_element(SU_NODE, &config_help_cmd);
	install_element(SU_NODE, &mme_stat_cmd);
	install_element(SU_NODE, &mme_stat_clean_cmd);
}

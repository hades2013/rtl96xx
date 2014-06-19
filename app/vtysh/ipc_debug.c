/*
 * ipc_debug.c
 *
 *  Created on: Jan 6, 2011
 *      Author: root
 */

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "vty.h"
#include <ipc_client.h>

extern int ipc_fd;

DEFUN(ipc_test1,
		ipc_test1_cmd,
		"ipc send WORD",
		"IPC test command\n"
		"Send string to server\n"
		"Word to send")
{
	int ret;
	if(ipc_fd < 0) {
		vty_out(vty, "invalid IPC FD!%s", VTY_NEWLINE);
	}
	else {
		vty_out(vty, "Sending [%s]%s", argv[0], VTY_NEWLINE);
		ret = send(ipc_fd, argv[0], strlen(argv[0]) + 1, 0);
		vty_out(vty, "%d bytes sent%s", ret, VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}

DEFUN(ipc_test2,
		ipc_test2_cmd,
		"ipc2 send WORD",
		"IPC test 2 command\n"
		"Send string to server\n"
		"Word to send")
{
	int ret = 0;
	return CMD_SUCCESS;

}

void ipc_debug_init(void) {
	install_element(VIEW_NODE, &ipc_test1_cmd);
	install_element(VIEW_NODE, &ipc_test2_cmd);
}

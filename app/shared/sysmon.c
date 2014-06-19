/*
 * sysmon.c
 *
 *  Created on: Jan 11, 2011
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>

#include "debug.h"

#include "cable.h"
#include "ipc_client.h"
#include "sysmon.h"

int sysmon_cmd(int ipc_fd, uint32_t cmd)
{
	ipc_sysmon_req_t req;
	ipc_sysmon_ack_t *ack;
	int ret = -1;

	req.hdr.ipc_type = IPC_SYSMON_CMD;
	req.hdr.msg_len = sizeof(uint32_t);
	req.cmd = cmd;
	ipc_send_request(ipc_fd, &req);
	ack = (ipc_sysmon_ack_t *)ipc_recv_acknowledge(ipc_fd);
	DBG_ASSERT(ack, "sysmon_cmd fail.");
	if(ack) {
		DBG_PRINTF("sysmon_cmd result: %d", ack->result);
		ret = ack->result;
		free(ack);
	}
	return ret;
}

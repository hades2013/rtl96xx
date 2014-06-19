/*
 * ipc_client.c
 *
 *  Created on: Jan 5, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
#include <linux/stddef.h>
#include <stddef.h>


#include <basetype.h>
#include <debug.h>
#include "ipc_client.h"
#include "ipc_protocol.h"

cnu_state_string_t cnu_state_string[] = {
		{"Unknow state", 				"Unknow state   "},
		{"Identifying",					"Identifying    "},
		{"Query software version",		"Query Version  "},
		{"Reading PIB header",			"Read PIB header"},
		{"Authenticating",				"Authenticating "},
		{"Initializing configuration",	"Init Config    "},
		{"Reading PIB blocks",			"Reading PIB    "},
		{"Writing PIB blocks",			"Writing PIB    "},
		{"Committing PIB",				"Committing PIB "},
		{"Configuring LAN Switch",		"Config LSW     "},
		{"Starting MDIO Block",			"MDIO BLK Start "},
		{"Writing MDIO Block",			"MDIO BLK Write "},
		{"Committing MDIO Block",		"MDIO BLK Commit"},
		{"Configuring MIB",				"Config MIB     "},
		{"Configure Done, Link is up",	"Config Done    "},
		{"Link Lost",					"Link Lost      "},
		{"Link Down",					"Link Down      "},
		{"Reading MDIO data",			"MDIO Reading   "},
		{"Writing MDIO data",			"MDIO Writing   "},
};

int ipc_client_connect(char *process_name)
{
	int sock, size;
	struct sockaddr_un un;
	struct timeval tv;

//	DBG_PRINTF("%s trying connect to ipc", process_name);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0) {
		printf("Fail to create IPC server socket: %s\n", strerror(errno));
		return -1;
	}


	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "/tmp/%s.%d", process_name, getpid());
	unlink(un.sun_path);

	size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	if(bind(sock, (struct sockaddr *)&un, size) < 0) {
		printf("Fail to bind IPC server socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, IPC_SERVER_PATH);

	size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	if(connect(sock, (struct sockaddr *)&un, size) < 0) {
		printf("Fail to connect IPC server: %s\n", strerror(errno));
		return -1;
	}

//	tv.tv_sec = 0;
//	tv.tv_usec = 10 * 1000;	// Timeout 10ms

	/* DEBUG ONLY! */
	tv.tv_sec = 100;
	tv.tv_usec = 0;
	if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		printf("Fail to set IPC socket timeout: %s\n", strerror(errno));
		return -1;
	}

	return sock;
}

void ipc_send_request(int ipc_fd, void *msg)
{
	ipc_request_hdr_t *req = (ipc_request_hdr_t *)msg;
	send(ipc_fd, req, sizeof(ipc_request_hdr_t) + req->msg_len, 0);
}

ipc_acknowledge_hdr_t * ipc_recv_acknowledge(int ipc_fd)
{
	ipc_acknowledge_hdr_t ack, *nack = NULL;
	int recv_len, ret, msize;
	struct timeval tv;
	fd_set rfds;

	FD_ZERO(&rfds);
	FD_SET(ipc_fd, &rfds);
	/*modify by guguiyuan 2012-9-25*/
	tv.tv_sec = 60;// what about 5 minutes?
	tv.tv_usec = 0;

	ret = select(ipc_fd + 1, &rfds, NULL, NULL, &tv);
	if (ret > 0) {
		if (FD_ISSET(ipc_fd, &rfds)) {
			recv_len = recv(ipc_fd, &ack, sizeof(ipc_acknowledge_hdr_t), MSG_PEEK);
			if (recv_len < sizeof(ipc_acknowledge_hdr_t)) {
				DBG_ASSERT(0, "IPC receive fail: %d:%s", recv_len, strerror(errno));
				return NULL;
			}
//			DBG_PRINTF("RECV: %d", offsetof(ipc_acknowledge_t, content));
//			hexdump(ack, 0, 16);
//			DBG_PRINTF("IPC message length: %d", ack->msg_len);
			msize = sizeof(ipc_acknowledge_hdr_t) + ack.msg_len;
			nack = (ipc_acknowledge_hdr_t *)malloc(msize);
			if (nack == NULL) {
				DBG_ASSERT(0, "Fail to allocate memory for IPC content, size: %d", msize);
				return NULL;
			}
			recv_len = recv(ipc_fd, nack, msize, 0);
			if (recv_len < msize) {
				free(nack);
				DBG_ASSERT(0, "Fail to read IPC message content: %d", recv_len);
				return NULL;
			}
			return nack;
		}
	}
	DBG_PRINTF("IPC ack timeout!");
	return NULL;
}

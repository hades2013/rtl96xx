/*
 * ipc_server.c
 *
 *  Created on: Jan 5, 2011
 *      Author: root
 */
#include "master.h"
#include "ipc.h"
#include <ipc_protocol.h>

#define	IPC_QLEN			64

ipc_entry_t *ipc_entry[IPC_MAX_CLIENT_ENTRY] = { NULL };

int ipc_socket_create_listen(void)
{
	int sock, size;
	struct sockaddr_un un;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0) {
		DBG_PRINTF("Fail to create IPC server socket: %s", strerror(errno));
		return -1;
	}

	unlink(IPC_SERVER_PATH);

	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, IPC_SERVER_PATH);

	size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	if(bind(sock, (struct sockaddr *)&un, size) < 0) {
		DBG_PRINTF("Fail to bind IPC server socket: %s", strerror(errno));
		return -1;
	}

	if(listen(sock, IPC_QLEN) < 0) {
		DBG_PRINTF("Fail to listen IPC server socket: %s", strerror(errno));
		return -1;
	}

	return sock;
}

ipc_entry_t *ipc_entry_new(void)
{
	int i;
	for(i = 0; i < IPC_MAX_CLIENT_ENTRY; i ++) {
		if(ipc_entry[i] == NULL) break;
	}
	DBG_ASSERT(i < IPC_MAX_CLIENT_ENTRY, "IPC entries full!");
	if(i < IPC_MAX_CLIENT_ENTRY) {
		ipc_entry[i] = (ipc_entry_t *)malloc(sizeof(ipc_entry_t));
		return ipc_entry[i];
	}
	return NULL;
}

int ipc_socket_accept(int server_socket)
{
	int fd;
	struct sockaddr_un un;
	char *cp, *pp;
	ipc_entry_t *client;
	socklen_t size;	
    
    /*begin added by liaohongjun 2012/6/25*/
    size = sizeof(un);
    /*end added by liaohongjun 2012/6/25*/
	fd = accept(server_socket, (struct sockaddr *)&un, &size);
	if(fd < 0) {
		DBG_PRINTF("accept error: %s", strerror(errno));
		return -1;
	}

	size -= offsetof(struct sockaddr_un, sun_path);
	un.sun_path[size] = 0;

	unlink(un.sun_path);

	cp = strchr(un.sun_path, '.');
	if(cp == NULL) {
		DBG_PRINTF("Invalid client address format %s", un.sun_path);
		return -1;
	}
	*cp = 0;

	pp = strrchr(un.sun_path, '/');
	if(pp == NULL)
		pp = un.sun_path;
	else
		pp ++;

	client = ipc_entry_new();
	if(client == NULL) return -1;

	client->fd = fd;
	client->process_name = strdup(pp);
	client->pid = strtoul(cp + 1, NULL, 0);

#ifdef IPC_PACKET_DEBUG
	DBG_PRINTF("Accepted client FD=%d NAME=%s PID=%d", client->fd, client->process_name, client->pid);
#endif
	return 0;
}

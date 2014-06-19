/*
 * ipc.h
 *
 *  Created on: Jan 5, 2011
 *      Author: root
 */

#ifndef IPC_H_
#define IPC_H_

#define	IPC_MAX_CLIENT_ENTRY	64

typedef struct {
	int		fd;			// Client FD
	char	*process_name;
	pid_t	pid;
}
ipc_entry_t;

int ipc_socket_create_listen(void);
int ipc_socket_accept(int server_socket);
void ipc_packet_process(ipc_entry_t *ipc_entry);

#endif /* IPC_H_ */

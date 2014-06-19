/*
 * signal_process.c
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#include "master.h"
#include "ipc.h"
#include <stdio.h>

extern int master_raw_socket;
extern int master_ipc_socket;
extern int signal_pipe[2];


extern ipc_entry_t *ipc_entry[IPC_MAX_CLIENT_ENTRY];

extern pid_t pid_vtysh;


void free_all(void)
{
	close(master_raw_socket);
	close(master_ipc_socket);
	close(signal_pipe[0]);
	close(signal_pipe[1]);	
}


void free_all_quit(void)
{
	//int i;
	close(master_raw_socket);
//	for(i = 0; i < IPC_MAX_CLIENT_ENTRY; i ++) {
//		if(ipc_entry[i]->fd >= 0) close(ipc_entry[i]->fd);
//	}
	close(master_ipc_socket);
	if(pid_vtysh) kill(pid_vtysh, SIGTERM);
	printf("Master exit by signal!\n\n");
	exit(0);
}
typedef struct Cyg_Mempool_Status_t {
    const UINT8 *arenabase; // base address of entire pool
    UINT32   arenasize;      // total size of entire pool
    UINT32   freeblocks;     // number of chunks free for use
    UINT32   totalallocated; // total allocated space in bytes
    UINT32   totalfree;      // total space in bytes not in use
    UINT32   blocksize;      // block size if fixed block
    UINT32   maxfree;        // size of largest unused block
    INT8    waiting;        // are there any threads waiting for memory?
    const UINT8 *origbase;  // address of original region used when pool
                                // created
    UINT32   origsize;       // size of original region used when pool
    INT8    maxoverhead;

}Cyg_Mempool_Status;
extern int pool_status(Cyg_Mempool_Status *stStatus);

void signal_process(int sig){	
	switch(sig) {
	case SIGUSR1:
		{
			FILE *fp=NULL;
			Cyg_Mempool_Status stStatus;
			pool_status(&stStatus);			
			fp=fopen("/dev/console","w");			
			if(fp!=NULL)
			{			
				fprintf(fp,"Mem Start Addr: %x\n", stStatus.arenabase);
				fprintf(fp,"Mem Total Size: %d\n", stStatus.arenasize);
				fprintf(fp,"Mem Free Blocks: %d\n", stStatus.freeblocks);
				fprintf(fp,"Mem Total alloc: %d\n", stStatus.totalallocated);
				fprintf(fp,"Mem Total Free: %d\n", stStatus.totalfree);
				fprintf(fp,"Mem Max Free: %d\n", stStatus.maxfree);
				fflush(fp);
				fclose(fp);				
			}
			else
			{				
				printf("Mem Start Addr: %x\n", stStatus.arenabase);
				printf("Mem Total Size: %d\n", stStatus.arenasize);
				printf("Mem Free Blocks: %d\n", stStatus.freeblocks);
				printf("Mem Total alloc: %d\n", stStatus.totalallocated);
				printf("Mem Total Free: %d\n", stStatus.totalfree);
				printf("Mem Max Free  : %d\n", stStatus.maxfree);				
			}			
		}
		 break;
	case SIGTERM:
	case SIGKILL:
	case SIGSEGV:
	case SIGSTOP:
	case SIGABRT:
	case SIGQUIT:
	case SIGINT:
		free_all_quit();
	}
}


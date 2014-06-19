/*
 * main.c
 *
 *  Created on: Dec 29, 2010
 *      Author: Ryan
 */

#include "master.h"
#include "signal_process.h"
#include "ipc.h"
//#include "raw_packet.h"
#include "ipc_packet.h"
//#include "cltmon.h"
//#include "queue.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <lw_config_api.h>
#include <lw_config_oid.h>
#include <timer.h>
#include <sys/wait.h>



#define FIFO_SERVER "/tmp/myfifo"
#define BUFSIZE_16 	16
#define FILE_TELL_VTYSH "/tmp/tell_vtysh"

#define	DEBUG_PATH_PREFIX	"/tmp/"

#define	MAIN_TIMEOUT_SEC		0
#define	MAIN_TIMEOUT_USEC		(500 * 1000)

char *master_ifname	= "eth0";
int master_ifindex;

ethernet_addr_t	master_ifmac;

int master_raw_socket = -1;
int master_ipc_socket = -1;

extern ipc_entry_t *ipc_entry[IPC_MAX_CLIENT_ENTRY];

//extern int raw_packet_debug;
extern int ipc_debug;

time_t main_tick = 0;

int signal_pipe[2];
extern int dhcpc_event_handler(int argc, char *argv[]);



static void signal_handler(int sig)
{
	int ret;
	ret = send(signal_pipe[1], &sig, sizeof(sig), MSG_DONTWAIT);
	//DBG_ASSERT(ret >= 0, "Could not send signal: %d", sig);
}
void sigchild (int sig)
{
     pid_t pid;
    (void)sig;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
             ;
    }
    pid = pid; //for pclint
    return ;
}

static void signal_setup(void)
{
	socketpair(AF_UNIX, SOCK_STREAM, 0, signal_pipe);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGKILL, signal_handler);
//	signal(SIGSEGV, signal_handler);
	signal(SIGSTOP, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGCHLD, sigchild);	
}

void main_loop(void)
{
	struct timeval tv;
	fd_set rfds;
	int maxfd, ret, sig, i;

	while(1) {
		tv.tv_sec = MAIN_TIMEOUT_SEC;
		tv.tv_usec = MAIN_TIMEOUT_USEC;

		FD_ZERO(&rfds);
		if(master_raw_socket >= 0) FD_SET(master_raw_socket, &rfds);
		if(master_ipc_socket >= 0) FD_SET(master_ipc_socket, &rfds);
		FD_SET(signal_pipe[0], &rfds);

		maxfd = signal_pipe[0] > master_raw_socket ? signal_pipe[0] : master_raw_socket;
		maxfd = master_ipc_socket > maxfd ? master_ipc_socket : maxfd;

		for(i = 0; i < IPC_MAX_CLIENT_ENTRY; i ++) {
			if(ipc_entry[i] == NULL) continue;
			if(ipc_entry[i]->fd < 0) {
                /* begin: add by soar pn:stormtokpps */
                free(ipc_entry[i]->process_name);
                free(ipc_entry[i]);
                /* end: add by soar pn:stormtokpps */
				ipc_entry[i] = NULL;
				continue;
			}
			FD_SET(ipc_entry[i]->fd, &rfds);
			if(ipc_entry[i]->fd > maxfd) maxfd = ipc_entry[i]->fd;
		}

		ret = select(maxfd + 1, &rfds, NULL, NULL, &tv);

		if(ret > 0) {
			if(FD_ISSET(signal_pipe[0], &rfds)) {
				if(read(signal_pipe[0], &sig, sizeof(sig)) >= 0) {
#ifdef SIGNAL_DEBUG
					DBG_PRINTF("Receive signal: %d", sig);
#endif
					signal_process(sig);
					continue;
				}
			}
			if(FD_ISSET(master_raw_socket, &rfds)) {
#ifdef RAW_PACKET_DEBUG
//				if(raw_packet_debug) DBG_PRINTF("Receive from raw socket");
#endif
			//	raw_packet_process();
				continue;
			}
			if(FD_ISSET(master_ipc_socket, &rfds)) {
#ifdef IPC_PACKET_DEBUG
				DBG_PRINTF("IPC client request...");
#endif
				ipc_socket_accept(master_ipc_socket);
				continue;
			}

			for(i = 0; i < IPC_MAX_CLIENT_ENTRY; i ++) {
				if(ipc_entry[i] == NULL) continue;
				if(ipc_entry[i]->fd < 0) continue;
				if(FD_ISSET(ipc_entry[i]->fd, &rfds)) {
					ipc_packet_process(ipc_entry[i]);
					break;
				}
			}
			continue;
		}

		timer_scheduler();

	}
}
/*****Begin add Func for feed_wtd by shipeng 2012-7-11*****/
void writepid_to_wtd()
{
	int fifo_fd;
	unsigned char buf_fd[BUFSIZE_16];
	
	memset(buf_fd, '\0', BUFSIZE_16);
	sprintf(buf_fd, "/proc/%d", getpid());/*get the pid of master*/

	if((mkfifo(FIFO_SERVER, O_CREAT | O_EXCL | O_RDWR) < 0) && (errno != EEXIST))/*create fifo_server*/
	{
		DBG_PRINTF("cannot create FIFO_SERVER\n");
	}
	if((fifo_fd = open(FIFO_SERVER, O_RDWR, 0)) == -1)
	{
	    DBG_PRINTF("open FIFO_SERVER error\n");
		exit(1);
	}
	write(fifo_fd, buf_fd, BUFSIZE_16);/*write pid into fifo_server*/
	
	system("/usr/sbin/app_watchdog &");/*run the app_watchdog in the background*/
}
/*****End add Func for feed_wtd by shipeng 2012-7-11*****/

/*****************************************************************
    Function:tell_vtysh_byfile
    Description: This function creat file to tell vtysh that we have started,which can 
                      make sure IPC connection exactly
    Author:liaohongjun
    Date:2012/8/9
    Input:       
    Output:         
    Return:
=================================================
    Note:add for QID0004
*****************************************************************/
static void tell_vtysh_byfile(void)
{
    int fd;
    unsigned char buf[BUFSIZE_16];

    memset(buf, 0x0, BUFSIZE_16);
    sprintf(buf, "/proc/%d", getpid());
    
    /*if file exist,delect it*/
    if(0 == access(FILE_TELL_VTYSH, F_OK))
    {
        unlink(FILE_TELL_VTYSH);
    }

    if((fd = open(FILE_TELL_VTYSH, O_WRONLY | O_CREAT, 0)) == -1)
    {
        DBG_PRINTF("open file %s error",FILE_TELL_VTYSH);
        return;
    }
    /*write the pid info of master to the file,then close it*/
    (void)write(fd, buf, BUFSIZE_16);
    close(fd);
    return;
}

extern int sys_ifm_mon_start(void);
extern void sys_ifm_init(void);
extern void sys_notifier_init(void);
extern int switch_apply(void);
extern int switch_config_init(void);

int main(int argc, char **argv)
{
	int ret;
	
	if(argc > 1) {
		if(!strncmp(argv[1], "pd", 2)) {
			//raw_packet_debug = 1;
		}else if(!strcmp(argv[1], "ipc")){
			ipc_debug = 1;
		}else if (!strcmp(argv[1], "dhcpc")){
			dhcpc_event_handler(argc - 1, &argv[1]);
		}else if (!strcmp(argv[1], "d"))
		{			
			daemon(NULL,0);
			writepid_to_wtd();/*****add Func for feed_wtd by shipeng 2013-8-27*****/
		}
		else
		{
			exit(0);
		}
	}
	else
	writepid_to_wtd();/*****add Func for feed_wtd by shipeng 2012-7-11*****/
	
	
	signal_setup();
	/*for new cfg*/
	Cfginit(MASTER);
	
	CfgStart();
	
	sys_ifm_init();
	
    sys_notifier_init();      
   

	master_ipc_socket = ipc_socket_create_listen();
	DBG_ASSERT(master_ipc_socket >= 0, "Fail to open IPC socket!");
	if(master_ipc_socket < 0) return -1;

	master_raw_socket = create_raw_socket(SYS_NETIF_NAME, &master_ifindex);
	DBG_ASSERT(master_raw_socket >= 0, "Fail to open raw socket on master interface!");
	if(master_raw_socket < 0) return -1;
	
	timer_init();
	

    /*start ifm monitor*/
	
	sys_ifm_finish();
	switch_config_init();
	sys_config_init();	
	switch_apply();
	sys_config_apply(); 
	sys_ifm_mon_start();
    tell_vtysh_byfile();
  
	main_loop();
	// Never reach
	return 0;
}

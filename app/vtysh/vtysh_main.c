/*
 * vtysh_main.c
 *
 *  Created on: Sep 22, 2010
 *      Author: ryan
 */

#include <zebra.h>
#include <cable.h>
//#include <switch.h>

#include "thread.h"
#include "vty.h"
#include "sigevent.h"
#include "log.h"
#include "filter.h"
#include "demo_interface.h"
#include <termios.h>
#include <paths.h>
#include <debug.h>

#include <ipc_client.h>
#include <ipc_protocol.h>
#include "cmd_eoc.h"
#include "memshare.h"
#include <lw_config_api.h>
#include <lw_config_oid.h>
#include <lw_config.h>

extern struct vty *vty_services[];

struct thread_master *master;

static struct termios oldt; // Original terminal settings
int tty_fd;

int ipc_fd = -1;
#ifdef CONFIG_CATVCOM
CATV_WORK_INFO_S *gst_CATVInfo = NULL;
#endif
// For debug only, restore the TTY terminal settings than exit
void restore_terminal_and_exit(void)
{
	printf("\nExit vtysh...\n");
	tcsetattr(tty_fd, TCSANOW, &oldt);
	exit(0);
}


/* SIGHUP handler. */
static void sighup(void) {
	zlog_info("SIGHUP received");
	//  rip_clean ();
	//  rip_reset ();
	zlog_info("vtysh restarting!");

	//  /* Reload config file. */
	//  vty_read_config (config_file, config_default);
	//
	//  /* Create VTY's socket */
	//  vty_serv_sock (vty_addr, vty_port, RIP_VTYSH_PATH);

	/* Try to return to normal operation. */
}

/* SIGINT handler. */
static void sigint(void) {
	zlog_notice("Terminating on signal");

	//  if (! retain_mode)
	//    rip_clean ();

	restore_terminal_and_exit();
}

/* SIGUSR1 handler. */
static void sigusr1(void) {
	zlog_rotate(NULL);
}

/*used by raisecom.c and web, disabled telnet*/
static void telnet_disable()
{
    int i =0;
    struct vty *vty_tmp = NULL;

    int iRet = IPC_STATUS_OK;
    
    iRet = ipc_cfg_set_telnetService("telnet_service_enabled", DISABLE_STR);;
    if (IPC_STATUS_OK != iRet)
    {
        return;        
    }

	/*close current connection*/
    for (i = 0; i < vector_active (vtyvec); i++)
    {
        if ((vty_tmp = vector_slot (vtyvec, i)) != NULL)
        {
            if(strcmp(vty_tmp->address, "Console"))
            {
                vty_tmp->status = VTY_CLOSE;
                shutdown(vty_tmp->fd, SHUT_RDWR);
            }
        }
    }
}


static void sigusr2(void)
{
	telnet_disable();
}
void sigchild ()
{
     pid_t pid;


   	
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
             ;
    }
    pid = pid; //for pclint

	
	//close(0);
	//close(1);
	//close(2);
	//setsid();
			  /* Open the new terminal device and setup stdout, stderr for the process
				   * so they point to the supplied terminal */
	tty_fd = console_init();
	vty_create(tty_fd, NULL); // Open CONSOLE vty
    return ;
}

static struct quagga_signal_t vtysh_signals[] = { { .signal = SIGHUP,
		.handler = &sighup, }, { .signal = SIGUSR1, .handler = &sigusr1, }, {
		.signal = SIGINT, .handler = &sigint, }, { .signal = SIGTERM,
		.handler = &sigint, }, { .signal = SIGUSR2, .handler = &sigusr2, },{ .signal = SIGCHLD, .handler = &sigchild, }};

char config_default[] = SYSCONFDIR "vtysh.conf";

static void set_term1( int fd )
{
    struct termios tty;

    tcgetattr( fd, &oldt );
	memcpy(&tty, &oldt, sizeof(struct termios));

    /*
     * set control chars
     */
    tty.c_cc[VINTR] = 3;	/* C-c */
    tty.c_cc[VQUIT] = 28;	/* C-\ */
    tty.c_cc[VERASE] = 127;	/* C-? */
    tty.c_cc[VKILL] = 21;	/* C-u */
    tty.c_cc[VEOF] = 4;		/* C-d */
    tty.c_cc[VSTART] = 17;	/* C-q */
    tty.c_cc[VSTOP] = 19;	/* C-s */
    tty.c_cc[VSUSP] = 26;	/* C-z */

    /*
     * use line dicipline 0
     */
    tty.c_line = 0;

    /*
     * Make it be sane
     */
    tty.c_cflag &= CBAUD | CBAUDEX | CSIZE | CSTOPB | PARENB | PARODD;
    tty.c_cflag |= CREAD | HUPCL | CLOCAL;

    /*
     * input modes
     */
    tty.c_iflag = ICRNL | IXON | IXOFF;

    /*
     * output modes
     */
    tty.c_oflag = OPOST | ONLCR;

    /*
     * local modes
     */
    tty.c_lflag = ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

	tcsetattr( fd, TCSANOW, &tty );
}

int console_init(void)
{
    int fd;
	char devname[256];
    /*
     * Clean up
     */
    strcpy(devname, ttyname(0));
    ioctl( 0, TIOCNOTTY, 0 );
    close( 0 );
    close( 1 );
    close( 2 );
    setsid(  );
	#if 0
	if(isatty(g_pstConsole->nReadFd))
	   {
		   strcpy(g_szTtyname, ttyname(g_pstConsole->nReadFd));
		   if(strstr(g_szTtyname,"ttyS"))
	#endif	
    /*
     * Reopen console
     */
    if(strstr(devname,"ttyS")||strstr(devname,"console"))
	{
		
		if( ( fd = open( _PATH_CONSOLE, O_RDWR ) ) < 0 )
		{
		/*
		 * Avoid debug messages is redirected to socket packet if no exist a
		 * UART chip, added by honor, 2003-12-04
		 */
		( void )open( "/dev/null", O_RDONLY );
		( void )open( "/dev/null", O_WRONLY );
		( void )open( "/dev/null", O_WRONLY );
		perror( _PATH_CONSOLE );
		return errno;
		}
	}
	else
	{
		if( ( fd = open( devname, O_RDWR ) ) < 0 )
		{
		/*
		 * Avoid debug messages is redirected to socket packet if no exist a
		 * UART chip, added by honor, 2003-12-04
		 */
		( void )open( "/dev/null", O_RDONLY );
		( void )open( "/dev/null", O_WRONLY );
		( void )open( "/dev/null", O_WRONLY );
		perror( devname );
		return errno;
		}
	}
    dup2( fd, 0 );
    dup2( fd, 1 );
    dup2( fd, 2 );

    ioctl( 0, TIOCSCTTY, 1 );
    tcsetpgrp( 0, getpgrp(  ) );
    set_term1( 0 );

    return 0;
}

/*begin added by liaohongjun 2012/9/7*/
void open_new_terminal(void)
{
    int fd;
    
    if( ( fd = open(_PATH_CONSOLE, O_RDWR ) ) < 0 )
    {
        /*
             * Avoid debug messages is redirected to socket packet if no exist a
             * UART chip, added by honor, 2003-12-04
             */
        (void)open( "/dev/null", O_RDONLY );
        (void)open( "/dev/null", O_WRONLY );
        (void)open( "/dev/null", O_WRONLY );
        perror(_PATH_CONSOLE);
        return errno;
    }
    /* Setup stdout, stderr for the new process so they point to the supplied terminal */
    dup2( fd, 0 );
    dup2( fd, 1 );
    dup2( fd, 2 );
    
    return 0;
}


void set_term(void)
{
        struct termios tty;

        tcgetattr(STDIN_FILENO, &tty);

        /* set control chars */
        tty.c_cc[VINTR] = 3;    /* C-c */
        tty.c_cc[VQUIT] = 28;   /* C-\ */
        tty.c_cc[VERASE] = 127; /* C-? */
        tty.c_cc[VKILL] = 21;   /* C-u */
//      tty.c_cc[VEOF] = 4;     /* C-d */
        tty.c_cc[VSTART] = 17;  /* C-q */
        tty.c_cc[VSTOP] = 19;   /* C-s */
        tty.c_cc[VSUSP] = 26;   /* C-z */

        /* use line dicipline 0 */
        tty.c_line = 0;

        /* Make it be sane */
        tty.c_cflag &= CBAUD | CBAUDEX | CSIZE | CSTOPB | PARENB | PARODD;
        tty.c_cflag |= CREAD | HUPCL | CLOCAL;


        /* input modes */
        tty.c_iflag = ICRNL | IXON | IXOFF;

        /* output modes */
        tty.c_oflag = OPOST | ONLCR;

        /* local modes */
        tty.c_lflag =
                ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN;

        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}


void enter_shell(void)
{
    pid_t pid = 0;
    unsigned char *argv[] = {(unsigned char *)"/bin/sh", NULL};
        struct termios old;
    //char *pttyname = NULL;
    //int fd = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    pid = fork();
    if(0 == pid)
    {
        /* now we are in son process */
        setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
        ioctl(0, TIOCNOTTY, 0 );
        close(0);
        close(1);
        close(2);

        /* Create a new session and make ourself the process  group leader */
        setsid();
        /* Open the new terminal device */      
        open_new_terminal();
        tcsetpgrp(0, getpid());
        /*Begin add by liaohongjun 2012/09/29 for EPN104QID0067*/       
		set_term();
        /*End add by liaohongjun 2012/09/29 for EPN104QID0067*/
        /* Make sure the terminal will act fairly normal for us */
        ioctl(0, TIOCSCTTY, 1);
        execvp((char *)argv[0], (char**)argv);

        exit(0);
    }
    else if( pid > 0)
    {
		#if 0
		waitpid(pid, NULL, 0);


		close(0);
		close(1);
		close(2);
		setsid();
				  /* Open the new terminal device and setup stdout, stderr for the process
					   * so they point to the supplied terminal */
		open_new_terminal();
		tcsetpgrp(0, getpid());
		dup(0);
		dup(0);
		tcsetattr(STDIN_FILENO, TCSANOW, &old);
		ioctl(0, TIOCSCTTY, 1);
		#endif
    }
    return;
}
/*end added by liaohongjun 2012/9/7*/
static void vtysh_main(char *config_file) {
	//int ret;
	//struct termios newt;
	char devname[256];
	struct thread thread;
	//int fd;

	/* First of all we need logging init. */
	zlog_default = openzlog("vtysh", ZLOG_DEFAULT,
			LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

	master = thread_master_create();
	signal_init(master, Q_SIGC(vtysh_signals), vtysh_signals);

	ipc_fd = ipc_client_connect("vtysh");
//	DBG_PRINTF("IPC connected FD=%d", ipc_fd);

    Cfginit(SLAVER);
	cmd_init(1);
	cmd_systemconfig_init();
	cmd_interface_init	();
	
	cmd_vlan_init();	
	cmd_networkinfo_init();
	
// disabled by Einsn	
//	access_list_init(); 

	vty_init(master);
// disabled by Einsn
//	memory_init();

	sort_node();

	//vty_read_config (config_file, config_default);

#if 0
#if 0
	close(0);
	close(1);
	close(2);
#endif
	tty_fd = open("/dev/tty", O_NONBLOCK | O_RDWR);
	DBG_ASSERT(tty_fd >= 0, "Fail to open tty device: %s", strerror(errno));

	if (tty_fd >= 0) {
#if 0
		dup2(0, fd);
		dup2(1, fd);
		dup2(2, fd);
#endif
		tcgetattr(tty_fd, &oldt);
		memcpy(&newt, &oldt, sizeof(struct termios));
		newt.c_lflag &= ~ICANON;
		newt.c_lflag &= ~ECHO;
		newt.c_cc[VMIN] = 1;
		newt.c_cc[VTIME] = 0;
		newt.c_lflag &= ~ISIG;
		ret = tcsetattr(tty_fd, TCSANOW, &newt);
		if (ret != 0) {
			perror("tcsetattr");
		}
	}
#endif

	tty_fd = console_init();
	vty_create(tty_fd, NULL); // Open CONSOLE vty
	strcpy(devname, ttyname(tty_fd));
    if(strstr(devname,"ttyS")||strstr(devname,"console"))
	vty_serv_sock(NULL, 23, NULL); // Open TELNET server on ALL interface, port 23
	
#ifdef CONFIG_CATVCOM
	/*Begin add by huangmingjian 2013/03/13 for EPN204QID0033*/ 
	if(NULL == (gst_CATVInfo = CATV_WorkInfoShmInit()))
    {
        DBG_PRINTF("CATV shared memory init error!");      
    }
	/*End add by huangmingjian 2013/03/13 for EPN204QID0033*/ 
#endif 
	/* Execute each thread. */
	while (thread_fetch(master, &thread))
		thread_call(&thread);
}

/*****************************************************************
    Function:need_wait_master
    Description: This function read file that created by master.if we check succeed ,
                      we consider that master has started successfully. this is to make 
                      sure IPC connection exactly later
    Author:liaohongjun
    Date:2012/8/9
    Input:       
    Output:       
    Return:
              0    //file exist 
              1    //file not  exist
=================================================
    Note:add for QID0004
*****************************************************************/
#define FILE_TELL_VTYSH "/tmp/tell_vtysh"
static int need_wait_master(void)
{
  //  int fd = 0;

    /*check file created by master*/
    if(0 != access(FILE_TELL_VTYSH, F_OK))
    {
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    /*begin modified by liaohongjun 2012/8/9 of QID0004*/
    //printf("VTYSH started.\n");
    /*make sure master has started*/
    while(need_wait_master());
    /*end modified by liaohongjun 2012/8/9 of QID0004*/
	if(argc > 1) {
		vtysh_main(argv[1]);
	}
	else {
		vtysh_main(NULL);
	}
	return 0;
}

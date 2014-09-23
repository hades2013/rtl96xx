/*
 * shutils.c
 *
 *  Created on: Feb 18, 2011
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <shutils.h>

/* Linux specific headers */
#include <error.h>
#include <termios.h>
#include <sys/time.h>
#include <net/ethernet.h>
#include <dirent.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <ctype.h>
#include <net/if_arp.h>
#include <lw_config.h>
#include "build_time.h"
#include <time_zone.h>
/*
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @param	path	NULL, ">output", or ">>output"
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @param	ppid	NULL to wait for child termination or pointer to pid
 * @return	return value of executed command or errno
 */
int
_eval(char *const argv[], char *path, int timeout, int *ppid)
{
	pid_t pid;
	int status;
	int fd;
	int flags;
	int sig;

	switch (pid = fork()) {
	case -1:	/* error */
		perror("fork");
		return errno;
	case 0:		/* child */
		/* Reset signal handlers set for parent process */
		for (sig = 0; sig < (_NSIG-1); sig++)
			signal(sig, SIG_DFL);
#if 1
		/* Clean up */
		ioctl(0, TIOCNOTTY, 0);
		close(STDIN_FILENO);
		setsid();

		/* Redirect stdout to <path> */
		if (path) {
			flags = O_WRONLY | O_CREAT;
			if (!strncmp(path, ">>", 2)) {
				/* append to <path> */
				flags |= O_APPEND;
				path += 2;
			} else if (!strncmp(path, ">", 1)) {
				/* overwrite <path> */
				flags |= O_TRUNC;
				path += 1;
			}
			if ((fd = open(path, flags, 0644)) < 0)
				perror(path);
			else {
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
		}
#endif

		/* execute command */
		setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
		alarm(timeout);
//		fcntl(1, F_SETFD, FD_CLOEXEC);
		execvp(argv[0], argv);
		perror(argv[0]);
		exit(errno);
	default:	/* parent */
		if (ppid) {
			*ppid = pid;
			return 0;
		} else {
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid");
				return errno;
			}
			if (WIFEXITED(status))
				return WEXITSTATUS(status);
			else
				return status;
		}
	}
}

/*begin added by liaohongjun 2012/7/31 of QID0002*/
void * xmalloc (size_t size)
{
    register void *value = malloc(size);
    if (0 == value)
    {
        perror("virtual memory exhausted");
    }

    return value;
}

void * xrealloc(void *ptr, size_t size)
{
    register void *value = realloc(ptr, size);
    if (0 == value)
    {
        perror("virtual memory exhausted");
    }

    return value;
}

/*****************************************************************
    Function:get_pid_by_name
    Description: This function gets the process id of a process. It opens the /proc
                     directory and then checks the entries in the directory. If the name 
                     of the process exists, then the pid is obtained.
    Author:liaohongjun
    Date:2012/7/31
    Input:char *pname       
    Output:         
    Return:retval
=================================================
    Note:
*****************************************************************/
/*begin modified by wanghuanyu for 241*/
pid_t get_pid_by_name(char *pname)
{
    DIR *dir = NULL;
    FILE *status = NULL;
    struct dirent *next;
    long *pidList = NULL;
    int i = 0;
    pid_t retval = 0;
    char *pidName = NULL;
    char filename[READ_BUF_SIZE] = {0};
    char buffer[READ_BUF_SIZE] = {0};
    char name[READ_BUF_SIZE] = {0};
    
    if(NULL == pname)
    {
        perror("paras error!");
        return 0;
    }
    
    pidName = pname;
    dir = opendir("/proc");
    if (!dir)
    {
        printf("Cannot open /proc");
        return 0;
    }

    while ((next = readdir(dir))!= NULL)
    {
        /* Must skip ".." since that is outside /proc */
        status = NULL;
        if (strcmp(next->d_name, "..") == 0)
        {
            continue;
        }

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))
        {
            continue;
        }

        memset(filename, 0x0, sizeof(filename));
        sprintf(filename, "/proc/%s/status", next->d_name);    
        if (! (status = fopen(filename, "r")) )
        {
            continue;
        }
        memset(buffer, 0x0, sizeof(buffer));
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL)
        {
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
        memset(name, 0x0, sizeof(name));
        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, pidName) == 0)
        {
            pidList=(long *)xrealloc(pidList, sizeof(long) * (i+2));
            if(pidList)
            {
                pidList[i++]=strtol(next->d_name, NULL, 0);				
            }
            else
            {
            	closedir(dir);
                return 0;
            }
        }
    }
    if (pidList)
    {
        pidList[i]=0;
    }
    else
    {
        pidList=(long *)xrealloc( pidList, sizeof(long));
        if(pidList)
        {
            pidList[0]=-1;
        }
        else
        {	closedir(dir);
            return 0;
        }        
    }

    retval = (pid_t)pidList[0];
    free(pidList);
	closedir(dir);
    return retval;
}
/*end added by liaohongjun 2012/7/31 of QID0002*/

int setip( unsigned char *ifname, unsigned int ip )           
{
        unsigned int socketfd = 0;                                   
        struct ifreq ifr;                                   
        struct sockaddr_in *addr_in;
        int err = 0;                                        
        int i = 0;                                          

        socketfd = socket ( AF_INET, SOCK_DGRAM, 0 );
        if ( socketfd < 0 )
        {      
                return -1;
        }


        for ( i=0; i<IFNAMSIZ; i++ )
        {
                ifr.ifr_name[i] = 0;
        }

        strcpy(ifr.ifr_name, (char*)ifname );

        /*  Set the device IP Address */
        addr_in = (struct sockaddr_in *)&(ifr.ifr_ifru.ifru_addr);
        memset(addr_in, 0, sizeof(*addr_in));
        addr_in->sin_family = AF_INET;
        addr_in->sin_port = 0;
        addr_in->sin_addr.s_addr = ip;

        err = ioctl(socketfd, SIOCSIFADDR, &ifr);
        close(socketfd);
        return err;

}
int setnetmask(unsigned char *ifname, unsigned int netmask )
{       
        unsigned int socketfd;
        struct ifreq ifr;
        struct sockaddr_in *addr_in;
        int err = 0;
        int i = 0;
        
        socketfd = socket ( AF_INET, SOCK_DGRAM, 0 );
        if ( socketfd < 0 ) 
        {          
                return -1;
        }
        
        for ( i=0; i<IFNAMSIZ; i++ )
        {
                ifr.ifr_name[i] = 0;
        }

        strcpy(ifr.ifr_name, (char*)ifname );
        
        /*  Set the device Netmask */
        addr_in = (struct sockaddr_in *)&(ifr.ifr_ifru.ifru_netmask);
        memset(addr_in, 0, sizeof(*addr_in));
        addr_in->sin_family = AF_INET;
        addr_in->sin_port = 0;
        addr_in->sin_addr.s_addr = netmask;

        err = ioctl(socketfd, SIOCSIFNETMASK, &ifr);
        close(socketfd);
        return err;
}
#define IPROUTE         "/bin/ip"
#define DEFAULT_RT_PRIORITY    70
struct ip_route_param {
    const char * command; /* add or del */
    const char * table;
    const char * proto;   /* static */
    char * dev;
    char * via;
    char dest[24];  /* It's always calculated, so leave it an array rather than pointer */
#if 0
    char dev[IFNAMESIZE];   /* what about using char * looks like ? */
    char via[16];
    char dest[24];    /* destination */
#endif
};

static inline int generate_route_cmd(char * cmd, struct ip_route_param * route)
{
   
    return sprintf(cmd, "%s route %s %s via %s dev %s proto %s metric %d"/*" table %s"*/,
            IPROUTE, route->command, route->dest, route->via, route->dev, route->proto, DEFAULT_RT_PRIORITY);
    
}

int do_route(struct ip_route_param * route)
{
      if (NULL == route)
    {   return -1;
    }
#ifndef CONFIG_multiwan
    char    buf[512];
        
    generate_route_cmd(buf, route);
    system(buf);
#endif          
    return 0;   
}       

int del_default_rt(char * dev_name, unsigned int gw)
{       
    struct  ip_route_param  route;
    char    via[32];
        
    route.dev = dev_name;
    route.command = "del";
    route.proto = "static";
    strcpy(route.dest, "default"); 
    route.table = "main";
    inet_ntop(AF_INET, &gw, via, INET_ADDRSTRLEN);
    route.via = via;
    return do_route(&route);
}
int set_default_rt(char * dev_name, unsigned int gw)
{
    struct  ip_route_param  route;
    char    via[32];
    
    route.dev = dev_name;
    route.command = "add";
    route.proto = "static";
    strcpy(route.dest, "default");
    route.table = "main";
    inet_ntop(AF_INET, &gw, via, INET_ADDRSTRLEN);
    route.via = via;
    return do_route(&route);
}
int set_mtu(char * dev_name, int mtu)
{
    char    command[64];

    sprintf(command, "ifconfig %s mtu %d", dev_name, mtu);
    system(command);
    return 0;
}
int setflag(char* dev_name, int flag, int set /*1 for set and 0 for clear */)
{
    int socketfd = 0;
    struct ifreq ifr;

    socketfd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socketfd < 0 )
    {       
        return -1;
    }
    strcpy(ifr.ifr_name, dev_name);
    if (ioctl(socketfd, SIOCGIFFLAGS, &ifr) < 0) {        
        close(socketfd);
        return -1;
    }
    if (set)
        ifr.ifr_flags |= (short)flag;
    else
        ifr.ifr_flags &= (short)(~(unsigned short)(flag));

    if (ioctl(socketfd, SIOCSIFFLAGS, &ifr) < 0) {       
        close(socketfd);
        return -1;
    }
    close(socketfd);

    return 0;

}

int setup(char * dev_name)
{   
    return setflag(dev_name, IFF_UP | IFF_RUNNING, 1);
}
int getmac(char *ifname,  char *mac )
{
        int socketfd;
        struct ifreq ifr;
        int ret, i;

        socketfd = socket ( AF_INET, SOCK_DGRAM, 0 );
        if ( socketfd < 0 )
        {

       // CM_DBG_ERR("%s\n",strerror(errno));
                return -1;
        }

        for ( i=0; i<IFNAMSIZ; i++ )
        {
                ifr.ifr_name[i]=0;
    }
        strcpy(ifr.ifr_name, (char*)ifname );


        ifr.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;

        /* Get the device  MAC */
        ret=ioctl(socketfd, SIOCGIFHWADDR, &ifr);
        if ( ret == -1 )
        {
            close(socketfd);
            return -1;
        }
        else
        {
        for(i=0; i<6; i++ )
        {
                mac[i] = ifr.ifr_ifru.ifru_hwaddr.sa_data[i];
        }
        close(socketfd);
    
        }
    
        return 0;
}   

int getmac_string(char *ifname,  char *macstr)
{   
        unsigned char mac[6] = {0};
        if(0 == getmac(ifname,(char *)mac))
        {
                sprintf ((char *)macstr, "%02X:%02X:%02X:%02X:%02X:%02X",(unsigned int)mac[0],(unsigned int)mac[1],(unsigned int)mac[2],(unsigned int)mac[3],(unsigned int)mac[4],(unsigned int)mac[5]);
            return 0;
        }
        else
        {

                return -1;
        }

}

int setdown(char * dev_name)
{
    return setflag(dev_name, IFF_UP, 0);
}

/*begin add by zhouguanhua 2013/6/5*/
UINT32 sys_os_get_uptime(void)
{
	FILE *fp;
	char val[64];
	fp = fopen("/proc/uptime", "r");
	if (fp == NULL){
		return 0;
	}
	fgets(val, sizeof(val), fp);
	fclose(fp);
	return strtoul(val, NULL, 0);
}

CHAR *version_string(void)
{
	static char ver[64];
	sprintf(ver, "%s", CONFIG_RELEASE_VER);
	return ver;
}

UINT32 version_count(void)
{
	return SYS_BUILD_COUNT;
}

CHAR *version_date(void)
{
	static char ver[64];
	time_t bt = SYS_BUILD_TIME;	
	sprintf(ver, "%s", ctime(&bt));
	ver[strlen(ver) - 1] = 0;
	return ver;
}

/*end add by zhouguanhua 2013/6/5*/

/*add by leijinbao 2013/8/22*/
int replace_char(char *src, int old, int new)
{
	char *ptr = NULL;

	if(!src)
	{
		return FALSE;
	}
	
	ptr = strchr(src, old);
	while(1)
	{	
		if(ptr)
		{
			*ptr = new;			
		}
		else
		{
			break;
		}
		ptr = strchr(ptr, old);
	}
	return TRUE;
}

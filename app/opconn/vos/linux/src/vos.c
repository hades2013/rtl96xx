/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos.c
*
* DESCRIPTION: 
*	
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <drv_wtd_user.h>

VOS_MUTEX_t g_pstAppAccessLock;

/*******************************************************************************
*
* vosAppInit:	 Initialize the VOS abstraction layer for LINUX
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0 - success
*   -x - failed
*
* SEE ALSO: 
*/
int vosAppInit(void)
{
    char		start_cwd[256];
    vosCurrentDirGet(start_cwd, sizeof (start_cwd));
    vosHomeDirSet(start_cwd);

    if (VOS_MUTEX_OK != vosMutexCreate(&g_pstAppAccessLock))
    { 
        printf("vosAppInit: create apps access mutex failed.\r\n");
        return -1;
    } 

    vosConsoleInit();
    return(0);
}

int vosAppDestroy(void)
{
    vosConsoleDestroy();
    vosHomeDirRemove();
    return(0);
}

int vosCoreInit(void)
{    
    vosUDelay(0);	/* Cause vosUDelay() to self-calibrate */
    vosThreadInit();
    vosThreadMainSet(vosThreadSelf());
    vosTerminalInit();
    return 0;
}

int vosCoreDestroy(void)
{
    return(0);
}

/*******************************************************************************
*
* vosShell:	 Fork a standard O/S shell.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosShell(void)
{
    setenv("PS1", "OPULAN / #", 1);
    char *s = getenv("SHELL");
    vosSystem(s ? s : "/bin/sh");
}


/*******************************************************************************
*
* vosSystem:	Fork current process to execute a command
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSystem(const char *cmd)
{
    int stat;
    pid_t pid;
    struct sigaction sa, savintr, savequit;
    sigset_t saveblock;
    if (cmd == NULL)
        return(1);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigemptyset(&savintr.sa_mask);
    sigemptyset(&savequit.sa_mask);
    sigaction(SIGINT, &sa, &savintr);
    sigaction(SIGQUIT, &sa, &savequit);
    sigaddset(&sa.sa_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sa.sa_mask, &saveblock);
    if ((pid = fork()) == 0) {
        sigaction(SIGINT, &savintr, (struct sigaction *)0);
        sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
        sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    }
    if (pid == -1) {
        stat = -1; /* errno comes from fork() */
    } else {
        while(1) {
            while(waitpid(pid, &stat, 0) < 0)
                if (errno != EINTR){
                    stat = -1;
                    break;
                }
            if(WIFSTOPPED(stat))
                continue;
            if(WIFEXITED(stat))
                break;
            if(WIFSIGNALED(stat))
                break;
        }
    }
    sigaction(SIGINT, &savintr, (struct sigaction *)0);
    sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
    sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
    return(stat);
}



/*******************************************************************************
*
* vosRandom:	Get random number. 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Random number between 0 and OP_VOS_RAND_MAX (2147483647)
*
* SEE ALSO: 
*/
int vosRandom(void)
{
    return (int)(random() & OP_VOS_RAND_MAX);
}

/*******************************************************************************
*
* vosReboot:	 Reboot the system
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosReboot(void)
{
    /*begin modified by liaohongjun 2012/7/30 of QID0002*/
    if(DRV_WTD_REBOOT_OK != Drv_Watchdog_Reboot())
    {
        system("reboot");    
    }
    /*end modified by liaohongjun 2012/7/30 of QID0002*/
}

STATUS vosHWAddrGet(UINT8 *ifName, UINT8 *macAddr)
{
    struct ifreq ifreq;
    INT32 sock;

    if (NULL == macAddr || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        vosPrintk("open sock error.\n");
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);
    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        /* begin modifieded by liaohongjun of QID0017*/
        VOS_DBG("get hardware address error.\n");
        /* end modifieded by liaohongjun of QID0017*/
        close(sock);
        return ERROR;
    }
    macAddr[0] = ifreq.ifr_hwaddr.sa_data[0];
    macAddr[1] = ifreq.ifr_hwaddr.sa_data[1];
    macAddr[2] = ifreq.ifr_hwaddr.sa_data[2];
    macAddr[3] = ifreq.ifr_hwaddr.sa_data[3];
    macAddr[4] = ifreq.ifr_hwaddr.sa_data[4];
    macAddr[5] = ifreq.ifr_hwaddr.sa_data[5];
    close(sock);
    return OK;
}

STATUS vosHWAddrSet(UINT8 *ifName, UINT8 *macAddr)
{
    struct ifreq ifreq;
    INT32 sock;

    if (NULL == macAddr || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);
    ifreq.ifr_hwaddr.sa_data[0] = macAddr[0];
    ifreq.ifr_hwaddr.sa_data[1] = macAddr[1];
    ifreq.ifr_hwaddr.sa_data[2] = macAddr[2];
    ifreq.ifr_hwaddr.sa_data[3] = macAddr[3];
    ifreq.ifr_hwaddr.sa_data[4] = macAddr[4];
    ifreq.ifr_hwaddr.sa_data[5] = macAddr[5];
    if (ioctl(sock, SIOCSIFHWADDR, &ifreq) < 0)
    {
        /*vosPrintk("set hardware address error.\n");*/
        close(sock);
        return ERROR;
    }
    close(sock);
    return OK;
}

STATUS vosIPAddrGet(UINT8 *ifName, char *ipAddr)
{
    struct ifreq ifreq;
    INT32 sock;
    struct sockaddr_in *psLocalAddr;
    char *tmpIpAddr = NULL;

    if (NULL == ipAddr || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);

    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0)
    {
        /*vosPrintk("get ip address error.\n");*/
        close(sock);
        return ERROR;
    }
    psLocalAddr = (struct sockaddr_in *)&ifreq.ifr_addr;
    tmpIpAddr = (char *)inet_ntoa(psLocalAddr->sin_addr);
    strcpy(ipAddr, tmpIpAddr);
    close(sock);
    return OK;
}

STATUS vosIPAddrSet(UINT8 *ifName, char *ipAddr)
{
    struct ifreq ifreq;
    INT32 sock;
    struct sockaddr_in localAddr;

    if (NULL == ipAddr || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);

    vosMemSet(&localAddr, 0x00, sizeof(struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    inet_aton(ipAddr, &localAddr.sin_addr);

    memcpy(&ifreq.ifr_addr, &localAddr, sizeof(struct sockaddr));

    if (ioctl(sock, SIOCSIFADDR, &ifreq) < 0)
    {
        /*vosPrintk("set ip address error.\n");*/
        close(sock);
        return ERROR;
    }
    close(sock);
    return OK;
}


STATUS vosNetMaskGet(UINT8 *ifName, char *netMask)
{
    struct ifreq ifreq;
    INT32 sock;
    struct sockaddr_in *psLocalAddr;
    char *tmpIpAddr = NULL;

    if (NULL == netMask || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);

    if (ioctl(sock, SIOCGIFNETMASK, &ifreq) < 0)
    {
        /*vosPrintk("get net mask error.\n");*/
        close(sock);
        return ERROR;
    }
    psLocalAddr = (struct sockaddr_in *)&ifreq.ifr_addr;
    tmpIpAddr = (char *)inet_ntoa(psLocalAddr->sin_addr);
    strcpy(netMask, tmpIpAddr);
    close(sock);
    return OK;
}

STATUS vosNetMaskSet(UINT8 *ifName, char *netMask)
{
    struct ifreq ifreq;
    INT32 sock;
    struct sockaddr_in localAddr;

    if (NULL == netMask || NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, &ifName[0]);

    vosMemSet(&localAddr, 0x00, sizeof(struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    inet_aton(netMask, &localAddr.sin_addr);

    memcpy(&ifreq.ifr_netmask, &localAddr, sizeof(struct sockaddr));

    if (ioctl(sock, SIOCSIFNETMASK, &ifreq) < 0)
    {
        /*vosPrintk("set net mask error.\n");*/
        close(sock);
        return ERROR;
    }
    close(sock);
    return OK;
}

int vosDigitToHexChar(int digit)
{
    digit &= 0xf;
    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}

int vosMacCharToDigit(char c)
{
	int retVal;
	if(c >= 'a' && c <= 'f')
		{
			retVal = c - 'a' + 10;
		}
	else if(c >= 'A' && c <= 'F')
		{
			retVal = c - 'A' + 10;
		}
	else
			retVal = c - '0';
	return retVal;
}

STATUS vosMacToStr(unsigned char *pMac, unsigned char *pMacStr)
{
    unsigned char chTemp;
    int i;
   /* if (strlen(pMac) < 6)
    {
		vosPrintf(pstEnv->nWriteFd, " Mac address syntax error!\r\n");
    	return ERROR;
    }*/

    for (i = 0; i < 6; i++)
    {
    	chTemp = pMac[i];
    	pMacStr[i*3] = vosDigitToHexChar(chTemp / 16);
    	pMacStr[i*3 + 1] = vosDigitToHexChar(chTemp % 16);
    	if (5 == i)
    		pMacStr[i*3 + 2] = '\0';
    	else
    		pMacStr[i*3 + 2] = ':';
    }

    return TRUE;
}

STATUS vosStrToMac(char *pchMacSrc,char *pchMac)
{
    int i = 0;
    int j = 0;
    int nMacSrcLen = 0;
    int nFlagOfOneByte = 0;

    
    if (pchMacSrc == NULL || pchMac == NULL) 
    {
        return ERROR;
    }
    
    nMacSrcLen = strlen(pchMacSrc);
    
    if (nMacSrcLen == 0) 
    {
        return ERROR;
    }
    for (i = 0;i < nMacSrcLen;i++)
    if (pchMacSrc[i] == ':' 
        || (pchMacSrc[i] >= '0' && pchMacSrc[i] <= '9')
        || (pchMacSrc[i] >= 'a' && pchMacSrc[i] <= 'f')
        || (pchMacSrc[i] >= 'A' && pchMacSrc[i] <= 'F'))
    {
        continue;
    }
    else
    {
        break;
    }
    if (i < nMacSrcLen) 
    {
        return ERROR;
    }
    
    nMacSrcLen = 0;/*nMacSrcLen is to supervisor if number of dot is less than 3*/
    
    for (i = 0; i < 6; i++) 
    {
         pchMac[i] = 0;
    }
        
    for (i = 0;i < 6;i++)
    {
        nFlagOfOneByte = 0;
        if (pchMacSrc[0] == ':' ) 
        {
            return ERROR;
        }
		if (pchMacSrc[1] == ':' ) 
        {
            return ERROR;
        }
        
        /*wjzhao add it 10.11.01, determine mac address format is XX:XX:XX:XX:XX:XX*/
        if (pchMacSrc[j] == ':' && (pchMacSrc[j+1] == '\0' || pchMacSrc[j+2] == '\0' ||
			(pchMacSrc[j+1] != NULL && pchMacSrc[j+1] == ':') || 
			(pchMacSrc[j+2] != NULL && pchMacSrc[j+2] == ':'))) 
        {
            return ERROR;
        }
        /*-----------------------------------*/
        
        if(pchMacSrc[j]== '\0'&& nMacSrcLen<6)
        {
            return ERROR;
        }
        
        if (pchMacSrc[j] == ':' ) 
        {
            j++;
        }
        while (pchMacSrc[j] != ':' && pchMacSrc[j] != '\0')
        {
            if (pchMacSrc[j] >= 'a' && pchMacSrc[j] <= 'f' ||
                pchMacSrc[j] >= 'A' && pchMacSrc[j] <= 'F')
            {
                pchMac[i] = pchMac[i]*16+(tolower(pchMacSrc[j])-'a' +10);
            }
            else
            {
                pchMac[i] = pchMac[i]*16+(pchMacSrc[j]-'0');
            }

            j++;
            

            if (nFlagOfOneByte ++ > 1)
            {
                return ERROR;
            }
        }
        nMacSrcLen++;
     }
    if (pchMacSrc[j] != '\0') 
    {
        return ERROR;
    }

    if (nMacSrcLen < 6) 
    {
        return ERROR;
    }
    
    return OK;
}
STATUS vosCpuMtuGet(UINT8 *ifName, UINT32 *ifMtu)
{
    struct ifreq ifreq;
    INT32 sock;

    if (NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, ifName);

    if (ioctl(sock, SIOCGIFMTU, &ifreq) < 0)
    {
        /*vosPrintk("get ip address error.\n");*/
        close(sock);
        return ERROR;
    }
    *ifMtu = ifreq.ifr_mtu;
    close(sock);
    return OK;
}

STATUS vosCpuMtuSet(UINT8 *ifName, UINT32 ifMtu)
{
    struct ifreq ifreq;
    INT32 sock;

    if (NULL == ifName)
    {
        return ERROR;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*vosPrintk("open sock error.\n");*/
        return ERROR;
    }
    strcpy(ifreq.ifr_name, ifName);
	ifreq.ifr_mtu = ifMtu;

    if (ioctl(sock, SIOCSIFMTU, &ifreq) < 0)
    {
        /*vosPrintk("get ip address error.\n");*/
        close(sock);
        return ERROR;
    }
    close(sock);
    return OK;
}


/** @}*/
/** @}*/

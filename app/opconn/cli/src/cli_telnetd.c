
/* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_telnetd.c#1 $ 
* 
* Simple telnet server 
* Bjorn Wesen, Axis Communications AB (bjornw@axis.com) 
* 
* This file is distributed under the Gnu Public License (GPL), 
* please see the file LICENSE for further information. 
* 
* --------------------------------------------------------------------------- 
* (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN 
**************************************************************************** 
* 
* The telnetd manpage says it all: 
* 
*   Telnetd operates by allocating a pseudo-terminal device (see pty(4))  for 
*   a client, then creating a login process which has the slave side of the 
*   pseudo-terminal as stdin, stdout, and stderr. Telnetd manipulates the 
*   master side of the pseudo-terminal, implementing the telnet protocol and 
*   passing characters between the remote client and the login process. 
*
* Vladimir Oleynik <dzo@simtreas.ru> 2001 
*     Set process group corrections, initial busybox port 
*/

/*************************************************************************
*
* FILENAME:  telnetd.c
*
* DESCRIPTION:
*
*
* Date Created: Apr 22, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_telnetd.c#1 $
* $Log:$
*
*
**************************************************************************/

#ifdef DEBUG
#define TELCMDS
#define TELOPTS
#endif

#include <sys/ioctl.h>

#include <vos.h>
#include <userLib.h>
#include "errors.h"
#include "cli_engine.h"
#include "cli_session.h"

#include "cli_telnet.h"

#define DEFAULT_TIMEOUT 3000

/***** data allocation *****/

static int g_nMaxFd;
static int g_nMasterFd;
static fd_set g_stRdfdset;

#pragma pack(1)

typedef struct OPT_INIT_s
{
    unsigned char   option;
    unsigned char   action;
} OPT_INIT_t;

#pragma pack()

static OPT_INIT_t requiredOptions[] =
{
    { TELOPT_ECHO ,  DO},
    { TELOPT_NAWS ,  DO},
    { TELOPT_LFLOW,  DO},
    { TELOPT_ECHO ,  WILL},
    { TELOPT_SGA  ,  WILL}
};


static unsigned char g_ucInitialized = 0;
static VOS_THREAD_t g_pstTelnetdTaskId;

extern STATUS cliPrintVersion(ENV_t *pstEnv, int nVerbose);

static int cliTelnetdIACRecv(ENV_t *pstEnv)
{
    unsigned char buf[4] = {0};
    unsigned char subOption[10] = {0};
    int recvLen = 0;
    VOS_TIME_t start;

    if (!pstEnv)
        return -1;

    start = vosTimeGet();
    do {
        recvLen += vosSafeRead(pstEnv->nReadFd, buf, 3);
        if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
        {
            return -1;
        }
    } while (recvLen < 3);

    if (buf[0] == IAC)
    {
        if (buf[1] == WILL && buf[2] == TELOPT_NAWS)
        {
            /* wait until receive the SB TELOPT_NAWS */
            recvLen = 0;
            do {
                start = vosTimeGet();
                recvLen += vosSafeRead(pstEnv->nReadFd, subOption, 9);
                if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
                {
                    return -1;
                }
            } while (recvLen < 9);
            /*
             * IAC -> SB -> TELOPT_NAWS -> 4-byte -> IAC -> SE
             */
            if (subOption[0] == IAC &&
                subOption[1] == SB &&
                subOption[2] == TELOPT_NAWS)
            {
                pstEnv->ulTermWidth = (subOption[3] << 8) | subOption[4];
                pstEnv->ulTermHeight = (subOption[5] << 8) | subOption[6];
#ifdef DEBUG
                vosPrintk("Receive NAWS width = %d, height = %d\r\n",
                    pstEnv->ulTermWidth, pstEnv->ulTermHeight);
#endif
                /* ioctl(pstEnv->nWriteFd, TIOCSWINSZ, (char *)&ws); */
            }
        } else {
            /* skip other IAC */
#ifdef DEBUG
            vosPrintk("Receive IAC %s,%s\r\n",
                TELCMD(buf[1]), TELOPT(buf[2]));
#endif
        }
    }
}


static int cliTelnetdIACSend(ENV_t *pstEnv, unsigned char command, int option)
{
    unsigned char b[4] = {0};
    if (pstEnv)
    {
        b[0] = IAC;
        b[1] = command;
        b[2] = option;
        vosSafeWrite(pstEnv->nWriteFd,b,3);
    }
}

static int cliTelnetdMaxFdUpdate(void *key, void *data)
{
    if (data)
    {
        if (g_nMaxFd < ((ENV_t *)data)->nReadFd)
            g_nMaxFd = ((ENV_t *)data)->nReadFd;
        if (g_nMaxFd < ((ENV_t *)data)->nWriteFd)
            g_nMaxFd = ((ENV_t *)data)->nWriteFd;
        if (g_nMaxFd < ((ENV_t *)data)->nErrFd)
            g_nMaxFd = ((ENV_t *)data)->nErrFd;
    }
}

static int cliTelnetdMaxFdUpdateForAllSessions()
{
    g_nMaxFd = 0;
    vosHashForEachDo(cliSessionTableGet(), cliTelnetdMaxFdUpdate);
}

static int cliTelnetdSelect(int sock, long timeout)
{
    if (sock < 0)
        return -1;

    if (g_nMaxFd < sock)
    {
        g_nMaxFd = sock;
    }

    fd_set fdset = g_stRdfdset;

    struct timeval tval;
    tval.tv_sec  = timeout / 1000;
    tval.tv_usec = (timeout - tval.tv_sec * 1000) * 1000;
    return select(g_nMaxFd + 1, &fdset, NULL, NULL, &tval);
}


static void * cliTelnetdRecvThread(int sock)
{
    ENV_t *pstEnv;
    int idx;
    OPT_INIT_t *pInit = requiredOptions;
    VOS_THREAD_t pstSessionTaskID = vosThreadSelf();

    pstEnv = cliSessionAdd(pstSessionTaskID, FALSE);
    if (pstEnv == NULL) {
        FD_CLR(sock, &g_stRdfdset);
        vosClose(sock);
        vosThreadExit(0);
        return (void *)NULL;
    }

    if (g_nMaxFd < sock) {
        g_nMaxFd = sock;
    }

    FD_SET(sock, &g_stRdfdset);

    pstEnv->nReadFd = sock;
    pstEnv->nWriteFd = sock;
    pstEnv->nErrFd = sock;

    /* set default terminal size */
    pstEnv->ulTermWidth = 80;
    pstEnv->ulTermHeight = 25;
    pstEnv->tmode = TERMINAL_MODE_VT100_N;

    //vosNonBlockingOn(sock);

    vosSocketPeerNameGet(sock, &pstEnv->clientAddress);

    /* Make the telnet client understand we will echo characters so it
     * should not do it locally. We don't tell the client to run linemode,
     * because we want to handle line editing and tab completion and other
     * stuff that requires char-by-char support. */

    for (idx = 0; idx < DIM(requiredOptions); idx++, pInit++)
    {
        cliTelnetdIACSend(pstEnv, pInit->action, pInit->option);
        cliTelnetdIACRecv(pstEnv);
    }

    vosFPuts(pstEnv->nWriteFd, "\r\r\n");

    cliPrintVersion(pstEnv, TRUE);

    /* start loop to handle command. */
    cliCmdProcess(pstEnv);

    cliTelnetdMaxFdUpdateForAllSessions();
    FD_CLR(sock, &g_stRdfdset);
    vosClose(sock);
    cliSessionDelete(pstSessionTaskID);
    /* vosPrintk("cliTelnetdRecvThread exit\r\n");  */
    vosThreadExit(0);
    return (void *)NULL;
}


static void * cliTelnetdMainThread(void)
{
    int selRet;
    int newFd;
    struct sockaddr_in clientAddr;
    VOS_THREAD_t recvThreadId;
    char clientThreadName[32];

    FD_ZERO(&g_stRdfdset);

    FD_SET(g_nMasterFd, &g_stRdfdset);

    if (g_nMasterFd > g_nMaxFd)
        g_nMaxFd = g_nMasterFd;

    for (;;)
    {
        selRet = cliTelnetdSelect(g_nMasterFd, DEFAULT_TIMEOUT);
        if (selRet <= 0)
            continue;

        newFd = vosSocketAccept(g_nMasterFd, &clientAddr);
        if (newFd < 0) {
            continue;
        }
        vosSprintf(clientThreadName, "tTelnetd%d", newFd);
        recvThreadId = vosThreadCreate(clientThreadName, OP_VOS_THREAD_STKSZ*32, 121,
                    (void *)cliTelnetdRecvThread, (void *)newFd);
        if (recvThreadId == NULL) {
            vosSocketClose(newFd);
            continue;
        }
    }
}

uint32 cliTelnetdInit(void)
{
    int ret;

    if (g_ucInitialized) {
        return SYS_TELNETD_ALREADY_INITIALIZED;
    }

    ret = vosSocketTcpSvrOpen(&g_nMasterFd, odmSysCfgTelnetPortGet());

    if (ret < 0) {
        return SYS_TELNETD_CREATE_SOCKET_FAILED;
    }

    g_pstTelnetdTaskId = vosThreadCreate("tTelnetd", OP_VOS_THREAD_STKSZ, 120,
                        (void *)cliTelnetdMainThread, (void *)NULL);
    if (g_pstTelnetdTaskId == NULL) {
        vosSocketClose(g_nMasterFd);
        return SYS_TELNETD_CREATE_FAILED;
    }

    g_ucInitialized = 1;

    return NO_ERROR;
}

UINT32 cliSessionClose(ENV_t *pstEnv)
{
    if (!pstEnv)
        return INVALID_32;

    if (pstEnv->pstMainTaskId != vosThreadMainGet()) {
        vosThreadDestroy(pstEnv->pstCommandTaskId);
        vosThreadDestroy(pstEnv->pstMainTaskId);
        cliTelnetdMaxFdUpdateForAllSessions();
        FD_CLR(pstEnv->nReadFd, &g_stRdfdset);
        vosClose(pstEnv->nReadFd);
        cliSessionDelete(pstEnv->pstMainTaskId);
    }

    return NO_ERROR;
}


static int cliTelnetdRecvThreadDestroy(void *key, void *value)
{
    char *k = key;
    ENV_t *v = value;

    cliSessionClose(v);

    return 0;
}


uint32 cliTelnetdShutdown(void)
{
    uint32 shutdownStart, current;

    if (!g_ucInitialized) {
        return SYS_TELNETD_NOT_INITIALIZED;
    }

    vosHashForEachDo(cliSessionTableGet(), cliTelnetdRecvThreadDestroy);

    g_ucInitialized = 0;

    if (g_pstTelnetdTaskId != NULL) {
        vosSocketClose(g_nMasterFd);
        vosThreadDestroy(g_pstTelnetdTaskId);
    }

    return NO_ERROR;
}


/*****************************************************************************
   日期        姓名             描述
  --------------------------------------------------------------------------

*****************************************************************************/


#include <sys/ioctl.h>
#include <vos.h>
#include "errors.h"
//#include "rstp_def.h"
//#include "rstp_func.h"
#include "../h/cli_engine.h"
#include "oam.h"

#define DEFAULT_TIMEOUT 3000
#define OK 0
#if 0
/*lint -save -e572 */
#define HLTOBUF(buf, x)\
    do {\
        *((buf) + 0) = (UCHAR)(((ULONG)(x) & 0xFF000000) >> 24);\
        *((buf) + 1) = (UCHAR)(((ULONG)(x) & 0x00FF0000) >> 16);\
        *((buf) + 2) = (UCHAR)(((ULONG)(x) & 0x0000FF00) >> 8 );\
        *((buf) + 3) = (UCHAR)(((ULONG)(x) & 0x000000FF) >> 0 );\
    } while (0)

#define BUFTOHL(buf, x)  \
    do { x =  (buf)[3] + \
          ((buf)[2] << 8) + \
          ((buf)[1] << 16) + \
          ((buf)[0] << 24);  \
    } while (0)
#endif
#define CMD_OK      0
#define CMD_ERROR   1
/*don't send to back msg*/
#define CMD_NO_SEND 2

#define MEM_COPY(pDest, pSrc, ulLen)  memcpy((VOID *)(pDest), (VOID *)(pSrc), (INT)(ulLen))

typedef int (*command_func)(UCHAR *pCmdBuf, UCHAR *pRetBuf);
/***** data allocation *****/
extern ULONG Drv_GetOpticsPower(FLOAT *pReceivPower, FLOAT *pTransPower);
extern ULONG EOAM_SendPonTrapInfo(IN UCHAR ucPonState, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendLogInfo (IN UCHAR *pucLogInof, IN USHORT usInfoLength, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendUniTrapInfo (IN UCHAR ucUniNo, IN UCHAR ucUniState, IN UCHAR ucDebugLevel);
extern int vdprintf(int fd, const char *format, va_list ap);

extern UINT8 g_bLightAlways;
extern BOOL_T g_bEoamPktPrintEnable;
extern UINT8 g_ucLOSInt;

static int g_nMaxFd;
static int g_nMasterFd;
static fd_set g_stRdfdset;

static unsigned char g_ucInitialized = 0;
static VOS_THREAD_t g_pstDebugCliTaskId;
#define MAX_PRINT_BUF (4096 * 2)
#define MAX_SEND_INFO_SIZE      128
char *g_szPrintBuf = NULL;

int g_iIsPrintf = 0;

extern ENV_t *g_pstConsole;
extern CMD_KEY_PARA_LINK_t *g_psRootOfForest;

int CMD_OpticsPower(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_StpStatus(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_OplCmdProcess(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_PonAlwaysLaserTrapSend(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_PonAlwaysLaserTestSta(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_PortUp(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_PortDown(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_EoamPrint(UCHAR *pCmdBuf, UCHAR *pRetBuf);
int CMD_OamStatus(UCHAR *pCmdBuf, UCHAR *pRetBuf);

void print2console(char *fmt, ...);

/* 扩展命令 */
/*lint --e{751, 749}*/
typedef enum tagEXT_CMD_ENTRY
{
    EXT_CMD_ENTRY_OPTIC = 0,
    EXT_CMD_ENTRY_STPSTATUS,
    EXT_CMD_ENTRY_OPL,
    EXT_CMD_ENTRY_PON_TRAP_SEND,
    EXT_CMD_ENTRY_PON_ALWAYS_STAT,
    EXT_CMD_ENTRY_PORT_UP,
    EXT_CMD_ENTRY_PORT_DOWN,
    EXT_CMD_ENTRY_EOAM_PRINT,
    EXT_CMD_ENTRY_OAM_STATUS,
    EXT_CMD_ENTRY_BUTT
} EXT_CMD_ENTRY_E;

/* 显示命令列表 */
static CHAR *m_apCmd[EXT_CMD_ENTRY_BUTT] =
{
    "OpticsPower",
    "StpStatus",
    "Opl",
    "PonAlwaysLaserTrapSend",  /* 长发光trap发送命令 */
    "PonAlwaysLaserTestState",  /* PON是否在长发光测试状态 */
    "PortUp",
    "PortDown",
    "EoamPrint",
    "OamState"
};

/*注册的函数返回值要注意，如果返回buf长度，不能是CMD_NO_SEND， CMD_ERROR, CMD_OK*/
static command_func m_apfCmd[EXT_CMD_ENTRY_BUTT] =
{
    CMD_OpticsPower,
    CMD_StpStatus,
    CMD_OplCmdProcess,
    CMD_PonAlwaysLaserTrapSend,
    CMD_PonAlwaysLaserTestSta,
    CMD_PortUp,
    CMD_PortDown,
    CMD_EoamPrint,
    CMD_OamStatus
};

/*****************************************************************************
 
*****************************************************************************/
static ULONG GET_INT32U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return CMD_ERROR;
    }

    return (pucData[usOffset] << 24) | (pucData[usOffset + 1] << 16) | (pucData[usOffset + 2] << 8) | (pucData[usOffset + 3]);
}

/*****************************************************************************
 
*****************************************************************************/
static FLOAT GET_FLOAT_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    FLOAT fData = 0;
    ULONG ulData = 0;

    if(NULL == pucData)
    {
        return (FLOAT)CMD_ERROR;
    }

    ulData = GET_INT32U_FROM_PKT(pucData, usOffset);
    MEM_COPY((VOID *)&fData, (VOID *)&ulData, sizeof(FLOAT));
    return fData;
}

/*****************************************************************************
 
*****************************************************************************/
static USHORT GET_INT16U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return (USHORT)CMD_ERROR;
    }

    return (pucData[usOffset] << 8) | (pucData[usOffset + 1]);
}

/*****************************************************************************
 
*****************************************************************************/
static UCHAR GET_INT8U_FROM_PKT(UCHAR *pucData, USHORT usOffset)
{
    if(NULL == pucData)
    {
        return (UCHAR)CMD_ERROR;
    }

    return pucData[usOffset];
}

/*****************************************************************************
 
*****************************************************************************/
static VOID SET_INT32U_TO_PKT(UCHAR *pucData, USHORT usOffset, ULONG ulData)
{
    if(NULL == pucData)
    {
        return;
    }

    pucData[usOffset]     = (ulData & 0xFF000000) >> 24;
    pucData[usOffset + 1] = (ulData & 0x00FF0000) >> 16;
    pucData[usOffset + 2] = (ulData & 0x0000FF00) >> 8;
    pucData[usOffset + 3] = (ulData & 0x000000FF);

    return;
}

/*****************************************************************************

*****************************************************************************/
static VOID SET_FLOAT_TO_PKT(UCHAR *pucData, USHORT usOffset, FLOAT fData)
{
    ULONG ulData = 0;

    if(NULL == pucData)
    {
        return;
    }

    MEM_COPY((VOID *)&ulData, (VOID *)&fData, sizeof(FLOAT));
    SET_INT32U_TO_PKT(pucData, usOffset, ulData);
    return;
}

/*****************************************************************************
 
*****************************************************************************/
static VOID SET_INT16U_TO_PKT(UCHAR *pucData, USHORT usOffset, USHORT usData)
{
    if(NULL == pucData)
    {
        return;
    }

    pucData[usOffset]     = (usData & 0xFF00) >> 8;
    pucData[usOffset + 1] = (usData & 0x00FF);

    return;
}


/*****************************************************************************

*****************************************************************************/
static VOID SET_INT8U_TO_PKT(UCHAR *pucData, USHORT usOffset, UCHAR ucData)
{
    if(NULL == pucData)
    {
        return;
    }

    pucData[usOffset] = ucData;

    return;
}

int CMD_OpticsPower(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    float fReceivePower = 0;
    float fTransmitPower = 0;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    if(CMD_ERROR == Drv_GetOpticsPower(&fReceivePower, &fTransmitPower))
    {
        return ERROR;
    }
#if 0
    USHORT usOffset = 0;
    /* cli printf float always 0, so return string*/
    SET_FLOAT_TO_PKT(pRetBuf, usOffset, fReceivePower);
    usOffset += sizeof(float);
    SET_FLOAT_TO_PKT(pRetBuf, usOffset, fTransmitPower);
    usOffset += sizeof(float);
    return usOffset;
#else
    return sprintf((CHAR *)pRetBuf, " Receive : %.2fdBm\r\n Transmit: %.2fdBm\r\n", fReceivePower, fTransmitPower);
#endif


}

int CMD_StpStatus(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    ULONG ulStpEnable;
   // ULONG aulStpHwStatus[RSTP_MAX_PORT_NUM];
   // ULONG aulStpProStatus[RSTP_MAX_PORT_NUM];
    USHORT usOffset = 0;
    int i = 0;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }
    #if 0
    (void)CFG_GetStpEnable(&ulStpEnable);
    SET_INT32U_TO_PKT(pRetBuf, usOffset, ulStpEnable);
    usOffset += sizeof(ULONG);
    if (DISABLE == ulStpEnable)
    {
        return usOffset;
    }

    for (i = 0; i < RSTP_MAX_PORT_NUM; i++)
    {
        aulStpHwStatus[i] = RSTP_Comm_GetPortRole((ULONG)i);
        aulStpProStatus[i] = RSTP_Comm_GetPortStatus((ULONG)i);
    }

    for(i = 0; i < RSTP_MAX_PORT_NUM; i++)
    {
        SET_INT32U_TO_PKT(pRetBuf, usOffset, aulStpHwStatus[i]);
        usOffset += sizeof(ULONG);
    }

    for(i = 0; i < RSTP_MAX_PORT_NUM; i++)
    {
        SET_INT32U_TO_PKT(pRetBuf, usOffset, aulStpProStatus[i]);
        usOffset += sizeof(ULONG);
    }
    #endif
    return usOffset;
}

int CMD_PonAlwaysLaserTrapSend(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    //UCHAR ucPonState = 0;
    //UCHAR ucDebugLevel = 0;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }
    #if 0
    if (CMD_OK != EOAM_SendPonTrapInfo(1, 2))
    {
        return ERROR;
    }
	
    #endif
    return CMD_OK;
}

int CMD_PonAlwaysLaserTestSta(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    USHORT usOffset = 0;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    SET_INT32U_TO_PKT(pRetBuf, usOffset, g_bLightAlways);
    usOffset += sizeof(UINT);

    return usOffset;
}

int CMD_PortUp(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    char pInfo[MAX_SEND_INFO_SIZE] = {0};
    unsigned int iPortNo;
    int iBufLen;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    sscanf((CHAR *)pCmdBuf, "PortUp %d", &iPortNo);

    iBufLen = vosSprintf(pInfo, "Ethernet 0/%d is link up.", iPortNo);
#if 0
    (VOID)EOAM_SendLogInfo((UCHAR *)pInfo, (USHORT)iBufLen, IC_LEVEL_WARN);
    (VOID)EOAM_SendUniTrapInfo((UCHAR)iPortNo, (UCHAR)STATUS_UP, IC_LEVEL_WARN);
#endif
    return CMD_NO_SEND;
}

int CMD_PortDown(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    char pInfo[MAX_SEND_INFO_SIZE] = {0};
    unsigned int iPortNo;
    int iBufLen;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    sscanf((CHAR *)pCmdBuf, "PortDown %d", &iPortNo);

    iBufLen = vosSprintf(pInfo, "Ethernet 0/%d is link down.", iPortNo);
   #if 0
    (VOID)EOAM_SendLogInfo((UCHAR *)pInfo, (USHORT)iBufLen, IC_LEVEL_WARN);
    (VOID)EOAM_SendUniTrapInfo((UCHAR)iPortNo, (UCHAR)STATUS_DOWN, IC_LEVEL_WARN);
   #endif
    return CMD_NO_SEND;

}

int CMD_EoamPrint(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }
	#if 0
    if (!vosStrCmp((CHAR *)pCmdBuf, "EoamPrint off"))
    {
        g_bEoamPktPrintEnable = BOOL_FALSE;
    }
    else
    {
        g_bEoamPktPrintEnable = BOOL_TRUE;
    }
    #endif
    return CMD_NO_SEND;

}

int CMD_OamStatus(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    UINT8 oamStatus = 0;
    ULONG ulStatus = FALSE;
    USHORT usOffset = 0;

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    if (g_ucLOSInt)
    {
        ulStatus = FALSE;
    }
    else
    {
        (VOID)OamStatusGet(&oamStatus);
        ulStatus = (EX_DISCOVERY_SEND_ANY == oamStatus)? TRUE : FALSE;
    }

    SET_INT32U_TO_PKT(pRetBuf, usOffset, ulStatus);
    usOffset += sizeof(UINT);

    return usOffset;

}

int CMD_OplCmdProcess(UCHAR *pCmdBuf, UCHAR *pRetBuf)
{
    int iRet = 0;
    char *pcSubCmd = NULL;
    int i = 0;
    char szParamBuf[MAX_POOL_LENGTH + 1 ] = {0};
    char szBuf[128];

    if ((NULL == pRetBuf) || (NULL == pCmdBuf))
    {
        return ERROR;
    }

    pcSubCmd = vosStrStr((CHAR *)pCmdBuf, "Opl");
    if (NULL == pcSubCmd)
    {
        return ERROR;
    }

    pcSubCmd++;

    while((NULL != pcSubCmd) && ('\0' != *pcSubCmd))
    {
        if('|' == pcSubCmd[i])
        {
            szParamBuf[i] = ' ';
        }
        else
        {
            szParamBuf[i] = pcSubCmd[i];
        }

        pcSubCmd++;
        i++;
    }

    szParamBuf[i] = '\n';
    sprintf(szBuf, "echo cmd %s >> debugOpl", szParamBuf);
    vosSystem(szBuf);

    strncpy(g_pstConsole->command->achCmdPool, szParamBuf, MAX_POOL_LENGTH);
    g_pstConsole->command->achCmdPool[MAX_POOL_LENGTH] = 0;

    g_iIsPrintf = g_iIsPrintf;
    memset(g_szPrintBuf, 0, sizeof(g_szPrintBuf));
    g_iIsPrintf = 1;
    cliCmdInterpreter(g_pstConsole, g_psRootOfForest, NULL);
    g_iIsPrintf = 0;
    sprintf(szBuf, "echo cmd %s >> debugOpl", g_szPrintBuf);
    vosSystem(szBuf);

    iRet = vosStrLen(g_szPrintBuf);

    if ((0 == iRet) || (MAX_PRINT_BUF <= iRet))
    {
        return ERROR;
    }

    strncpy((CHAR *)pRetBuf, g_szPrintBuf, MAX_PRINT_BUF);
    pRetBuf[MAX_PRINT_BUF - 1] = 0;

    return iRet;
}

int recvBuf(int sock, UCHAR *pBuf, int iBufLen)
{

    VOS_TIME_t start;
    int recvLen = 0;
    int ilen = 0;

    if ((NULL == pBuf) || (0 == iBufLen))
    {
        return  -1;
    }

    /* recv length*/
    ilen = sizeof(int);
    start = vosTimeGet();
    do
    {
        recvLen += vosSafeRead(sock, (void *)(pBuf + recvLen), (uint32)(ilen - recvLen));
        if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
        {
            return -1;
        }

    } while (recvLen < ilen);

    //BUFTOHL(pBuf, ilen);
    ilen = (int)GET_INT32U_FROM_PKT(pBuf, 0);

    if (ilen > iBufLen)
    {
        return -1;
    }

    recvLen = 0;
    /* recv buf*/
    start = vosTimeGet();
    do
    {
        recvLen += vosSafeRead(sock, (void *)(pBuf + recvLen), (uint32)(ilen - recvLen));
        if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
        {
            return -1;
        }
    } while (recvLen < ilen);

    return recvLen;
}

int sendBuf(int sock, UCHAR *pBuf, int iBufLen)
{
    int sendLen = 0;
    int ilen = 0;
    UCHAR szBuf[128] = {0};
    VOS_TIME_t start;

    if ((NULL == pBuf) || (0 == iBufLen))
    {
        return  -1;
    }

    //HLTOBUF(szBuf, iBufLen);
    SET_INT32U_TO_PKT(szBuf, 0, (uint32)iBufLen);
    ilen = sizeof(int);
    start = vosTimeGet();
    do
    {
        sendLen += vosSafeWrite(sock, (void *)(szBuf + sendLen), (uint32)(ilen - sendLen));
        if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
        {
            return -1;
        }

    } while (sendLen < ilen);

    ilen = iBufLen;
    sendLen = 0;
    start = vosTimeGet();
    do
    {
        sendLen += vosSafeWrite(sock, (void *)(pBuf + sendLen), (uint32)(ilen - sendLen));
        if ((vosTimeGet() - start) >= (DEFAULT_TIMEOUT / 1000))
        {
            return -1;
        }

    } while (sendLen < ilen);

    return sendLen;
}
static void cliCmdDebugMainThread(void)
{
    int selRet;
    int newFd;
    struct sockaddr_in clientAddr;
    char szRecvBuf[128];
    char szCmd[128];
    UCHAR *szSendBuf = NULL;
    struct linger so_linger;
    so_linger.l_onoff = TRUE;
    so_linger.l_linger = 0;

    szSendBuf = (UCHAR *)malloc(MAX_PRINT_BUF + 1);
    if( NULL == szSendBuf)
    {
        return;
    }

    vosMemSet(szSendBuf, 0, MAX_PRINT_BUF + 1);

    int i = 0;
    int iRet = 0;
    USHORT usBufOffset = sizeof(ULONG);

    for (;;)
    {
        newFd = vosSocketAccept(g_nMasterFd, &clientAddr);
        if (newFd < 0)
        {
            continue;
        }
        setsockopt(newFd, SOL_SOCKET,  SO_LINGER, (char *)&so_linger, sizeof(so_linger));

        /* recv buf*/
        memset(szRecvBuf, 0, sizeof(szRecvBuf));
        if(-1 == recvBuf(newFd, (UCHAR *)szRecvBuf, sizeof(szRecvBuf)))
        {
            vosClose(newFd);
            continue;
        }

        /* find cmd key */
        vosMemSet(szCmd, 0, sizeof(szCmd));
        i = 0;
        while((szRecvBuf[i] != '\0') && (szRecvBuf[i] != ' '))
        {
            szCmd[i] = szRecvBuf[i];
            i++;
        }

        i = 0;
        for(i = 0; i < EXT_CMD_ENTRY_BUTT; i++)
        {
            if (!vosStrCmp(szCmd, m_apCmd[i]))
            {
                vosMemSet(szSendBuf, 0, MAX_PRINT_BUF + 1);
                iRet = m_apfCmd[i]((UCHAR *)szRecvBuf, szSendBuf + usBufOffset);
                break;
            }
        }

        if(CMD_ERROR == iRet)
        {
            SET_INT32U_TO_PKT(szSendBuf, 0, CMD_ERROR);
        }
        else if(CMD_OK == iRet)
        {
            SET_INT32U_TO_PKT(szSendBuf, 0, CMD_OK);
        }
        else if(CMD_NO_SEND == iRet)
        {
            vosClose(newFd);
            continue;
        }

        if(-1 == sendBuf(newFd, szSendBuf, iRet + usBufOffset))
        {
            vosClose(newFd);
            continue;
        }
        vosClose(newFd);

    }

}

uint getOpconMagicPort()
{
    return 9988;
}
#if 0
uint getCliMagicPort()
{
    return 8899;
}
#endif
uint32 cliCmdDebugInit(void)
{
    int ret;
    struct timeval timeout = {3, 0};
    struct linger so_linger;
    so_linger.l_onoff = TRUE;
    so_linger.l_linger = 0;

    if (g_ucInitialized) {
        return SYS_TELNETD_ALREADY_INITIALIZED;
    }

    ret = vosSocketTcpSvrOpen(&g_nMasterFd, (uint16)getOpconMagicPort());

    if (ret < 0) {
        return SYS_TELNETD_CREATE_SOCKET_FAILED;
    }

    setsockopt(g_nMasterFd, SOL_SOCKET,  SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(g_nMasterFd, SOL_SOCKET,  SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(g_nMasterFd, SOL_SOCKET,  SO_LINGER, (char *)&so_linger, sizeof(so_linger));

    /*lint --e{611}*/
    g_pstDebugCliTaskId = vosThreadCreate("CmdDebug", OP_VOS_THREAD_STKSZ, 120,
                        (void *)cliCmdDebugMainThread, (void *)NULL);
    if (g_pstDebugCliTaskId == NULL) {
        vosSocketClose(g_nMasterFd);
        return SYS_TELNETD_CREATE_FAILED;
    }

    g_szPrintBuf = (char *)malloc(MAX_PRINT_BUF + 1);
    if (NULL == g_szPrintBuf)
    {
        return SYS_TELNETD_CREATE_FAILED;
    }

    memset(g_szPrintBuf, 0, MAX_PRINT_BUF + 1);
    return NO_ERROR;
}
#if 0
int cliCmdDebugConnect(void)
{
    int connect_fd;
    int  rc;
    struct timeval timeout = {3, 0};

    connect_fd = vosSocketTcpCreate();
    if (-1 == connect_fd)
    {
        return -1;
    }

    rc = vosSocketConnect(connect_fd, "127.0.0.1", (uint16)getCliMagicPort());
    if (-1 == rc)
    {
        return -1;
    }

    setsockopt(connect_fd, SOL_SOCKET,  SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(connect_fd, SOL_SOCKET,  SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    return connect_fd;
}
#endif
void print2console(char *fmt, ...)
{
    int fd;
    if ((fd = open("/dev/console", O_WRONLY | O_NOCTTY | O_NONBLOCK)) >= 0)
    {
        /*lint --e{530}*/
        va_list arguments;
        va_start(arguments, fmt);
        /*lint --e{830}*/
        vdprintf(fd, fmt, arguments);
        va_end(arguments);
        close(fd);
    }

    return ;
}


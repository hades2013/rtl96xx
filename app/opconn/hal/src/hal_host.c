/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_host.c
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_host.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/filter.h>


#include <vos.h>
#include <hal.h>
#include <opl_host.h>

#ifdef OPL_DMA0_ETH
    #define PKT_BUFF_MODE 
#endif

#define HOST_DMA_COUNTER_NUM    11

static VOS_THREAD_t g_pstHostThreadId = NULL;

typedef struct HOST_COUNTER_s
{
	INT32 	id;
	UINT8	*name;
	UINT64 	counter;
} HOST_COUNTER_t;

enum {
    HAL_API_HOST_CNT_CPDMA_RX_INT, 
    HAL_API_HOST_CNT_CPDMA_TX_INT, 
    HAL_API_HOST_CNT_RXBD_FULL_INT,
    HAL_API_HOST_CNT_QUEUE_FULL_INT, 
    HAL_API_HOST_CNT_PLC_DROP_INT, 
    HAL_API_HOST_CNT_CPTM_RD_ERR_INT, 
    HAL_API_HOST_CNT_FIFO_EPD_DROP_INT,
    HAL_API_HOST_CNT_TX_LONG_PKT_INT, 
    HAL_API_HOST_CNT_LL_FULL_INT,
    HAL_API_HOST_CNT_FIFO_PPD_ERR_INT, 
    HAL_API_HOST_CNT_ENQ_LEN_ERR_INT,
    HAL_API_HOST_CNT_NUM
};

HOST_COUNTER_t g_stHostDmaCounter[HAL_API_HOST_CNT_NUM+1] = 
{
    { HAL_API_HOST_CNT_CPDMA_RX_INT, "CPDMA_RX_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_CPDMA_TX_INT, "CPDMA_TX_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_RXBD_FULL_INT, "RXBD_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_QUEUE_FULL_INT, "QUEUE_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_PLC_DROP_INT, "PLC_DROP_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_CPTM_RD_ERR_INT, "CPTM_RD_ERR_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_FIFO_EPD_DROP_INT, "FIFO_EPD_DROP_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_TX_LONG_PKT_INT, "TX_LONG_PKT_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_LL_FULL_INT, "LL_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_FIFO_PPD_ERR_INT, "FIFO_PPD_ERR_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_ENQ_LEN_ERR_INT, "ENQ_LEN_ERR_INT", OPL_ZERO },
    { -1 , NULL, OPL_ZERO }
};

struct sock_filter opconn_bpf_filter [] = {
    /* OAM*/
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 12),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x88, 0, 2), 
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 13),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x09, 16, 0), 
	/* IGMP */
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 0),    
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x01, 0, 15),   
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 1),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x00, 0, 2),
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 2),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x5e, 10, 0),
    /* RSTP */
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 1),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x80, 0, 9),
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 2),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0xc2, 0, 7), 
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 3),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x00, 0, 5),  
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 4),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x00, 0, 3),
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 5),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x00, 0, 1),                        
    /* If we passed all the tests, ask for the whole packet. */
    BPF_STMT(BPF_RET+BPF_K,0x7fffffff),
    /* Otherwise, drop it. */
    BPF_STMT(BPF_RET+BPF_K,0),
};

STATIC VOID setup_socket_filter(INT s)
{
        struct sock_fprog p;

        /* Set up the bpf filter program structure.
         * This is defined in bpf.c */
        p.len = sizeof(opconn_bpf_filter)/sizeof (struct sock_filter);
        p.filter = opconn_bpf_filter;

        setsockopt( s, SOL_SOCKET, SO_ATTACH_FILTER, &p, sizeof(p));

    return;
}



static void halHostSocketThread(void)
{
    int raw_socket;
    struct sockaddr_ll sll;
    struct ifreq ifstruct;

    //raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_TYPE_SLOW_PROTOCOL));
    raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (0 > raw_socket){
        printf("raw_socket socket create error!\n");
        return;
    }

    strcpy(ifstruct.ifr_name, "eth0");
    ioctl(raw_socket, SIOCGIFINDEX, &ifstruct);

	setup_socket_filter(raw_socket);
    
    memset( &sll, 0, sizeof(sll) );
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifstruct.ifr_ifindex;
    //sll.sll_protocol = htons(ETH_TYPE_SLOW_PROTOCOL );
    sll.sll_protocol = htons(ETH_P_ALL);

    if(bind(raw_socket, (struct sockaddr *) &sll, sizeof(sll)) == -1 ){
        printf("\nraw_socket bind create error!\n");
        return;
    }
    /* begin modifieded by liaohongjun of QID0017*/
    VOS_DBG("socket init ok raw_socket=%d\n", raw_socket);
    /* end modifieded by liaohongjun of QID0017*/
    while (1){
        eopl_host_socket_recv(raw_socket);
		vosUSleep(1000);
    } 

    vosThreadExit(0);
}


static void halHostThread(void)
{
	UINT32 intStatus = OPL_ZERO;
    UINT32 index = 0;

    enum { DMA0_RX_MASK = 0x0001 };

#ifndef PKT_BUFF_MODE
    oplRegWrite(REG_DMA0_INT_EN,0x7ff);
    enable_host_dma0_interrupt();
#endif

	while (1)
	{
        wait_for_host_dma0_interrupt();
#ifndef PKT_BUFF_MODE
		oplRegRead(REG_DMA0_INT, &intStatus);
        
        for (index = 0; index < HOST_DMA_COUNTER_NUM; index++)
        {
            if (intStatus&(1<<index))
            {
                g_stHostDmaCounter[index].counter++;
            }
        }
#endif

#ifndef OPL_DMA0_ETH
        if (DMA0_RX_MASK & intStatus)
#endif
		{
			eopl_host_recv();
		}

#ifndef PKT_BUFF_MODE
        enable_host_dma0_interrupt();
#endif
	}

	vosThreadExit(0);
}

void halHostReceive()
{
    eopl_host_recv();
}

void halHostSend(void *pParam)
{
    HAL_API_HOST_SEND_t *pHostParam = (HAL_API_HOST_SEND_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_host_send(pHostParam->pvPacket, pHostParam->usPacketLen);
}


void halHostSendToUplink(void *pParam)
{
    HAL_API_HOST_SEND_TO_UPLINK_t *pHostParam = (HAL_API_HOST_SEND_TO_UPLINK_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_send_to_uplink(
        pHostParam->usPortNum, 
        pHostParam->pvPacket, 
        pHostParam->usPacketLen);
}


void halHostSendToDownlink(void *pParam)
{
    HAL_API_HOST_SEND_TO_DOWNLINK_t *pHostParam = (HAL_API_HOST_SEND_TO_DOWNLINK_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_send_to_down_link(
        pHostParam->usPortNum, 
        pHostParam->usWithTag, 
        (UINT8)pHostParam->stPacketType, 
        pHostParam->pvPacket, 
        pHostParam->usPacketLen);
}

void halHostDumpEnable(void *pParam)
{
    HAL_API_HOST_DUMP_EN_t *pHostParam = (HAL_API_HOST_DUMP_EN_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    eopl_host_dump_enable(pHostParam->ulEnable);
}


void halHostCounterClear(void)
{
    UINT32 index = 0;
    for (index = 0; index < HOST_DMA_COUNTER_NUM; index++)
    {
        g_stHostDmaCounter[index].counter = OPL_ZERO;
    }
}

void halHostCounterShow(void *pParam)
{
    HAL_API_HOST_COUNTER_SHOW_t *pHostParam = (HAL_API_HOST_COUNTER_SHOW_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (0 == pHostParam->ulFd)
    {
        pHostParam->ulFd = OP_STDOUT_FILENO;
    }

	INT32 index = OPL_ZERO;

	vosPrintf(pHostParam->ulFd, "%-32s %-12s\r\n", " Description", " Counter");
	for(index = OPL_ZERO; g_stHostDmaCounter[index].id != -1; index++)
	{
		vosPrintf(pHostParam->ulFd, "-------------------------------- ------------\r\n");
		vosPrintf(pHostParam->ulFd, "%-32s %-12d\r\n",
			g_stHostDmaCounter[index].name,
			g_stHostDmaCounter[index].counter);
    }
}


STATUS halHostThreadInit(void)
{
    int ret;

    if (g_pstHostThreadId) {
        return ERROR;
    }

    
    /*halHostThread()use interrupt recv*/
    g_pstHostThreadId = vosThreadCreate("tHost", OP_VOS_THREAD_STKSZ, 20,
                        (void *)halHostSocketThread, (void *)NULL);
    
    if (g_pstHostThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS halHostThreadShutdown(void)
{
    if (!g_pstHostThreadId) {
        return ERROR;
    }

    if (g_pstHostThreadId != NULL) {
        vosThreadDestroy(g_pstHostThreadId);
    }

    return OK;
}



/*******************************************************************************
*
* halHostInit:	 to initialize host hal module
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	OK      - initialize successfully.
*   other   - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halHostInit(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register host API */

    stApiUnit.apiId = HAL_API_HOST_RECEIVE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostReceive;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSend;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_UPLINK;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSendToUplink;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_DOWNLINK;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSendToDownlink;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_DUMP_EN;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostDumpEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_CLEAR;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostCounterClear;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_SHOW;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostCounterShow;
    ulRet += halDrvApiRegister(&stApiUnit);

    halHostThreadInit();

    return ulRet;
}

/*******************************************************************************
*
* halHostDestroy:	 
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
OPL_RESULT halHostDestroy()
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    halHostThreadShutdown();

#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    halDma1ThreadShutdown();
#endif

    stApiUnit.apiFunc = NULL;

    /* deregister host API */

    stApiUnit.apiId = HAL_API_HOST_RECEIVE;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_UPLINK;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_DOWNLINK;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_DUMP_EN;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_CLEAR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_SHOW;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
}



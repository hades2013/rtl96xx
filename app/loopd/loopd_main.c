
/*****************************************************************************

      File name:loopd_main.c
      Description:process to do loop detect
      Author:liaohongjun
      Date:
              2012/11/30
==========================================================
      Note:file added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************************/

#ifdef  __cplusplus
    extern "C"{
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/filter.h>
#include <semaphore.h>

#include "loopd.h"
#include "memshare.h"

sem_t g_iLoopdTimeSem;
extern PORT_LOOPD_STATE_S *gstLoopdState;

/*Begin modified by huangmingjian 2013-12-31*/
/*We should check the untag and tag packets at the same time as vlan accelerating*/
struct sock_filter loopd_bpf_filter [] = {
    /* rtl loop detect frame*/
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 12),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x90, 0, 2), 
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 13),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x01, 4, 0),  
	
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 16),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x90, 0, 3), 
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 17),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x01, 0, 1), 
	
    /* If we passed all the tests, ask for the whole packet. */
    BPF_STMT(BPF_RET+BPF_K,0x7fffffff),
    /* Otherwise, drop it. */
    BPF_STMT(BPF_RET+BPF_K,0),
	
	#if 0
    /* rtl loop detect frame*/
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 12),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x90, 0, 3), 
	BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 13),
	BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K,0x01, 0, 1),                         
    /* If we passed all the tests, ask for the whole packet. */
    BPF_STMT(BPF_RET+BPF_K,0x7fffffff),
    /* Otherwise, drop it. */
    BPF_STMT(BPF_RET+BPF_K,0),	
	#endif
};
/*End modified by huangmingjian 2013-12-31*/

static void LoopdSetupSocketFilter(int s)
{
    struct sock_fprog p;

    /* Set up the bpf filter program structure.
     * This is defined in bpf.c */
    p.len = sizeof(loopd_bpf_filter)/sizeof (struct sock_filter);
    p.filter = loopd_bpf_filter;

    setsockopt( s, SOL_SOCKET, SO_ATTACH_FILTER, &p, sizeof(p));

    return;
}
STATIC VOID dbg_print_pkt(CHAR *pkt,INT len)
{
    int i = 0;
    
    for (i=0; i<len; i++)
    {
      if (0 == (i%16))
      {
        printf("\n  0x%02x : ",(pkt+i));
      }
      printf("%02x", *(UINT8 *)(pkt+i));
      printf(" ");
    }
    printf("\n\n");

}

static int LoopdRawSocketCreate(void)
{
    int raw_socket;
    struct sockaddr_ll sll;
    struct ifreq ifstruct;

    raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    //raw_socket = socket(PF_PACKET, SOCK_RAW, htons(0x9001));
    if (0 > raw_socket)
    {
        printf("raw_socket socket create error!\n");
        return RTK_PROTO_ERR_FAILED;
    }

    strcpy(ifstruct.ifr_name, "eth0");
    ioctl(raw_socket, SIOCGIFINDEX, &ifstruct);

	LoopdSetupSocketFilter(raw_socket);
    
    memset( &sll, 0, sizeof(sll) );
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifstruct.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);
    
    if(bind(raw_socket, (struct sockaddr *) &sll, sizeof(sll)) == -1 )
    {
        printf("\nraw_socket bind create error!\n");
        return RTK_PROTO_ERR_FAILED;
    }

    return raw_socket;
}

STATIC INT16 LoopdHostSocketRecv(INT socket_fd)
{
    unsigned char  rcv_pkt[PKT_BUFFER_SIZE];
    unsigned char *pBuff = rcv_pkt;
    pktBuf_t pktBuf;
    eth_hdr_t *pEthHdr = NULL;
	struct msghdr msg;
    struct iovec iov;
    struct mw_l2_ctl_s l2_ctl;
    STATIC INT32 usLen;
    INT32 counter=0;
    
    memset(&msg, 0, sizeof(msg));
    memset(&iov, 0, sizeof(iov));
    memset(&l2_ctl, 0, sizeof(l2_ctl));
    iov.iov_base = (void *)&pBuff[0];
    iov.iov_len = PKT_BUFFER_SIZE;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;   
    msg.msg_control = &l2_ctl;
    msg.msg_controllen = sizeof(l2_ctl); 
    
    while((usLen = recvmsg(socket_fd, &msg, 0)) > 0)    
    {       
        /*remove TPID*/
		if(pBuff[12] == 0x81 && pBuff[13] == 0x00){
            memmove(&pBuff[12], &pBuff[16], usLen-16);
            usLen = usLen - 4;
        } 
        
        pEthHdr = (eth_hdr_t *)pBuff;
        if (ETH_TYPE_LOOPD == (ntohs(pEthHdr->ether_type)))
        {
            memset(&pktBuf, 0x0, sizeof(pktBuf_t));
            pktBuf.pkt_head = pBuff;
            pktBuf.l2 = pBuff;
            pktBuf.l3 = pktBuf.l2 + sizeof(eth_hdr_t);
            pktBuf.spa = l2_ctl.sll_port;
            //printf("loopd rcv rlpp: \n");
            //dbg_print_pkt(pktBuf.pkt_head, usLen);
            if(RTK_PROTO_ERR_OK != rlpp_pkt_process(&pktBuf, usLen))
            {
                return -1;
            }
        }
        
        counter++;
        if (counter > 32)
          break;
		
		msg.msg_control = &l2_ctl;
		msg.msg_controllen = sizeof(l2_ctl);    
    }

    return 0;
}

STATIC VOID sigterm_handler(int sig)
{
    if(SIGTERM != sig)
    {
        return;
    }
    /*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
  //  (VOID)rlpp_setState(FALSE);
   // (VOID)shmdt((VOID *)gstLoopdState);
    /*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/
    exit(0);
}

int main(int argc,char *argv[])
{
    int iRet = 0;
    int iRawSocket = 0;
	struct itimerval stTime;
	struct sigaction stSigAction;
    
    sem_init (&g_iLoopdTimeSem, 0, 0);
    
	signal(SIGTERM, sigterm_handler);
	memset(&stSigAction, 0, sizeof(stSigAction));
	stSigAction.sa_handler = RlppHelloTimerHandle;
	iRet = sigaction (SIGALRM, &stSigAction, 0);
	if(0 != iRet)
	{
		printf("signal action set for timer unit error!\n");
		return -1;
	}

	memset(&stTime, 0, sizeof(stTime));
	stTime.it_value.tv_sec = 1;
	stTime.it_value.tv_usec = 0;
	stTime.it_interval = stTime.it_value;
	iRet = setitimer (ITIMER_REAL, &stTime, 0);
	if(0 != iRet)
	{
		printf("timer unit start error!\n");
		return -1;
	}
    
    (VOID)rlpp_init();
    iRawSocket = LoopdRawSocketCreate();
    if (iRawSocket < 0){
        printf("iRawSocket create error!\n");
        return -1;
    }

    while(1)
    {
        sem_wait(&g_iLoopdTimeSem);
        (void)LoopdHostSocketRecv(iRawSocket);
        usleep(1000);
    }
    
    return 0;
}


#ifdef  __cplusplus
}
#endif


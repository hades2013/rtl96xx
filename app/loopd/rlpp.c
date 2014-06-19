/*****************************************************************************

      File name:rlpp.c
      Description:provide funcs of loop detecte
      Author:liaohongjun
      Date:
              2012/11/30
==========================================================
      Note: file added by liaohongjun 2012/11/30 of EPN104QID0084
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
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

//#include "str_utils.h"
#include "loopd.h"
#include "memshare.h"

STATIC UINT8 p_tx_buffer[PKT_BUFFER_SIZE]; 
PORT_LOOPD_STATE_S *gstLoopdState = NULL;

static rlpp_switch_id_t    sid;
static UINT8               gen_rlppu;
static UINT8               max_hopcount;
static logic_pmask_t       rlpp_en_pmsk;
rlpp_port_var_t            port_var[LOGIC_PORT_NO + 1]; //0 is not use,logic port start from 1
static UINT8               hello_timer;
static UINT8               rlpp_en;
static UINT8               rlpp_sys_loop_state;
UINT8                      prevState[LOGIC_PORT_NO + 1] = {RLPP_FORWARDING_STATE};
char rlpp_srcmac[MAC_ADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x00};

#define RLPP_MSTI    (0)

static void GenerateRlppPkt();
static INT32 rlppSentPkt2OnePort(port_num_t lgcPort);

STATIC INT sys_get_if_mac(CHAR *ifname, CHAR *mac)
{
	INT sock, ret;
	struct ifreq ifr;

    if(!mac || !ifname)
    {
        return -1;
    }
    
	sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("[%s]create socket: %s",__FUNCTION__, strerror(errno));
        return -1;
    }

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	if(ret < 0) 
    {
        printf("[%s]ioctl: %s",__FUNCTION__, strerror(errno));
        return -1;
    }

	memcpy(mac, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
	close(sock);
	return 0;
}

INT32 sal_port_flush_set(port_num_t logicPort)
{
    logicPort = 1;

    return 0;
}

VOID dumpPortVector(rlpp_port_vector_t *pv)
{
    UINT8 *ptr;
	UINT8 i;

	ptr = (UINT8*)pv;

	for(i = 0; i < sizeof(rlpp_port_vector_t);i++)
	{
	    printf("%02X ", (UINT16)*(ptr + i));
		if ( i % 16 == 15 )
		{
			printf("\n");
		}
	}
    printf("\n\n");	
}

INT32 rlpp_portVar_init(port_num_t logicPort)
{
    UINT32 uiAdmEn = DISABLE;
    memset(&port_var[logicPort], 0x00, sizeof(rlpp_port_var_t));
    port_var[logicPort].port_vector.swppri = 0xFF;
    port_var[logicPort].slen               = TRUE;
    port_var[logicPort].ppri               = 0xFF;
    port_var[logicPort].hc                 = 1;
    port_var[logicPort].port_state         = RLPP_FORWARDING_STATE;

    /*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
    prevState[logicPort] = RLPP_FORWARDING_STATE;
    (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_FORWARDING);
    (VOID)Ioctl_GetPortEnable(logicPort, &uiAdmEn);
    if(ENABLE == uiAdmEn)
    {
        gstLoopdState->port[logicPort].curstate = PORT_STATE_FORWARDING;
        
    }
    else
    {
        gstLoopdState->port[logicPort].curstate = PORT_STATE_DISABLED;
    }
    /*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/
    return RTK_PROTO_ERR_OK;
}

INT32 rlpp_init(VOID)
{
    port_num_t      logicPort = 0;
    char sys_mac[MAC_ADDR_LEN]={0};
    
    (void)sys_get_if_mac("eth0", sys_mac); 
	memcpy(rlpp_srcmac, sys_mac, MAC_ADDR_LEN);
    memcpy(sid.swmac, sys_mac, MAC_ADDR_LEN);
    memset(&sid.swpri, 0x07, RLPP_SWPRI_LEN);

    gen_rlppu    = TRUE;
	rlpp_en      = TRUE;
    rlpp_sys_loop_state = FALSE;
    SetLgcMaskAll(&rlpp_en_pmsk);

    if(NULL == (gstLoopdState = LoopdPortCurStateShmInit()))
    {
        RLPP_DBG("loopd port state shared memory init error!");        
        return RTK_PROTO_ERR_FAILED;
    }
    /* port variable */
    for(logicPort = 1; logicPort < (MAX_PORT_NUM + 1); logicPort++)
    {
        (VOID)rlpp_portVar_init(logicPort);
    }

    /*delete acl of drop loopd frames*/
    if(DRV_OK != Ioctl_AclDropLoopdDelete())
    {
        return RTK_PROTO_ERR_FAILED;
    }
    
	return RTK_PROTO_ERR_OK;
}
INT32 rlpp_setState(UINT8 enable)
{
    /*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
    port_num_t      lgcPort = 0;
    rlpp_en = enable;

    if ( rlpp_en == FALSE )
    {
        rlpp_init();
        rlpp_en = FALSE;
        for(lgcPort = 1; lgcPort < (MAX_PORT_NUM + 1); lgcPort++)
        {
            gstLoopdState->port[lgcPort].curstate = PORT_STATE_DISABLED;     
        }   
        /*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/
    }
	else
	{
            rlpp_init();
            hello_timer = RLPP_HELLO_TIME - 1;
	}

	return RTK_PROTO_ERR_OK;	
}

INT32 rlpp_getPortState(port_num_t logicPort, rlpp_port_state_t *state)
{
    *state = port_var[logicPort - 1].port_state;

	return RTK_PROTO_ERR_OK;
}

void _rlpp_switch_state_update()
{
    port_num_t logicPort = 0;
    UINT32 uiAdmEnable = DISABLE;
        
    for(logicPort = 1; logicPort < (MAX_PORT_NUM + 1);logicPort++)
    {
        /*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
        (VOID)Ioctl_GetPortEnable(logicPort, &uiAdmEnable);   
        if(ENABLE == uiAdmEnable)
        {
            /* loop is newly discovered */
            if ( prevState[logicPort] == RLPP_FORWARDING_STATE &&
    			 port_var[logicPort].port_state != RLPP_FORWARDING_STATE )
            {  
#if RLPP_DBG_EN                                    
                RLPP_DBG("set logicport %d to PORT_STATE_BLOCKING",logicPort);
#endif      
                gstLoopdState->port[logicPort].curstate = PORT_STATE_BLOCKING;
                (VOID)Ioctl_AclDropLoopdCreateAddPort(logicPort); 
                (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_BLOCKING);  
            }
    		/* loop is newly removed */
            else if ( prevState[logicPort] != RLPP_FORWARDING_STATE &&
    			 port_var[logicPort].port_state == RLPP_FORWARDING_STATE )
            {
#if RLPP_DBG_EN                                                
                RLPP_DBG("set logicport %d to PORT_STATE_FORWARDING",logicPort);
#endif
                gstLoopdState->port[logicPort].curstate = PORT_STATE_FORWARDING;
                (VOID)Ioctl_AclDropLoopdCreateRemovePort(logicPort);            
                (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_FORWARDING);
                GenerateRlppPkt();
                //rlppSentPkt2OnePort(logicPort);           
            }
			/*Begin add by liaohongjun 2013/03/01 of EPN204QID0030*/
			else 			
			{
				if(PORT_STATE_DISABLED == gstLoopdState->port[logicPort].curstate)
				{
					gstLoopdState->port[logicPort].curstate = PORT_STATE_FORWARDING;
				}
			}
			/*End add by liaohongjun 2013/03/01 of EPN204QID0030*/
        }
        else
        {
            (VOID)Ioctl_AclDropLoopdCreateRemovePort(logicPort);            
            (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_FORWARDING);            
            gstLoopdState->port[logicPort].curstate = PORT_STATE_DISABLED;
        }
        /*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/
        /* update state */
		prevState[logicPort] = port_var[logicPort].port_state;
    }
}

static UINT8 _rlpp_port_own_best_pv()
{
    port_num_t              best_port = 0xFF, logicPort = 0;
    rlpp_port_vector_t best_vector;

    memset(&best_vector, 0xFF, sizeof(rlpp_port_vector_t));
    for(logicPort = 1; logicPort < (MAX_PORT_NUM + 1); logicPort++)
    {    
        if ( FALSE == port_var[logicPort].pv_ag_enable || FALSE == TstLgcMaskBit(logicPort, &rlpp_en_pmsk))
            continue;

#if 0
        dumpPortVector(&best_vector);
        dumpPortVector(&port_var[logicPort].port_vector);
#endif
        
        if ( memcmp(&port_var[logicPort].port_vector, &best_vector, sizeof(rlpp_port_vector_t)) < 0)
        {
            best_port = logicPort;
            memcpy(&best_vector, &port_var[logicPort].port_vector, sizeof(rlpp_port_vector_t));
        }
    }

    return best_port;
}   

static void _port_update_process()
{
    port_num_t              logicPort, best_port;
    rlpp_port_vector_t pv;
    UINT16             tmpHopCount;
    
    if ( TRUE == gen_rlppu )
    {
        memcpy(&pv.rid, &sid, sizeof(rlpp_switch_id_t));
        pv.hopcount = 0;
        memcpy(&pv.sid, &sid, sizeof(rlpp_switch_id_t));        
        
        for(logicPort = 1; logicPort < (MAX_PORT_NUM + 1); logicPort++)
        {
            if ( FALSE == TstLgcMaskBit(logicPort, &rlpp_en_pmsk) )
            {
                port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                continue;
            }
        
            if ( FALSE == port_var[logicPort].selfloop )
            {
                pv.swppri = port_var[logicPort].ppri;
                pv.swport = logicPort;

                if ( port_var[logicPort].pv_ag_enable )
                {  
                    if ( memcmp(&pv, &port_var[logicPort].port_vector, sizeof(rlpp_port_vector_t)) < 0 )
                    {    
                        port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                    }
                    else if ( memcmp(&pv, &port_var[logicPort].port_vector, sizeof(rlpp_port_vector_t)) > 0 )
                    {
                        port_var[logicPort].port_state = RLPP_BLOCKING_STATE;
						if ( rlpp_sys_loop_state == FALSE )
						{
						    rlpp_sys_loop_state = TRUE;

                            /* flush port */
                            //sal_port_flush_set(logicPort);
						}
                    }
                    else
                    {                    
                        /* exception ?? */
#if RLPP_DBG_EN         
                        RLPP_DBG("Line %d - exception");
#endif
                    }
                }
                else
                {
                    port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                }
            } 
        }
    }
    else
    {
    
        best_port = _rlpp_port_own_best_pv();
#if RLPP_DBG_EN                        
        RLPP_DBG("point D - %u", (UINT16)best_port);
#endif
        memcpy(&pv.rid, &port_var[best_port].port_vector.rid, sizeof(rlpp_switch_id_t));
        memcpy(&pv.sid, &sid, sizeof(rlpp_switch_id_t));        
        pv.hopcount = port_var[best_port].port_vector.hopcount;
        
        for(logicPort = 1; logicPort < (MAX_PORT_NUM + 1); logicPort++)
        {
            if ( FALSE == TstLgcMaskBit(logicPort, &rlpp_en_pmsk) )
            {
                port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                continue;
            }

            if ( FALSE == port_var[logicPort].selfloop )
            {
                if ( logicPort == best_port )
                    port_var[best_port].port_state= RLPP_FORWARDING_STATE;
                else
                {
                    port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                    if( TRUE == port_var[logicPort].pv_ag_enable )
                    {
                        tmpHopCount = pv.hopcount;
                        pv.hopcount += port_var[logicPort].hc;
                        pv.swppri = port_var[logicPort].ppri;
                        pv.swport = logicPort;

                        if ( memcmp(&pv, &port_var[logicPort].port_vector, sizeof(rlpp_port_vector_t)) >= 0 )
                       	{
                            port_var[logicPort].port_state = RLPP_BLOCKING_STATE;        
                            if ( rlpp_sys_loop_state == FALSE )
                            {
                                rlpp_sys_loop_state = TRUE;
                                /* flush port */
                                //sal_port_flush_set(logicPort);
                            }
                        }
                        pv.hopcount = tmpHopCount;                    
                    }
                }
            }
        }
    }    
}

void _forward_process(port_num_t best_port, pktBuf_t *pBuf)
{
    port_num_t              logicPort;
    rlpp_payload_t     *pRlppPayload;
    l2_send_op l2_info;
	UINT8              *l2Ptr;
    //char rlpp_srcmac[MAC_ADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x01};
	UINT8              i;

    memset(&l2_info, 0, sizeof(l2_info));
    l2_info.usVid = 0;

#if 0
    for(i=0;i<60;i++)
    {
        RLPP_PRINT("%02X ", (UINT16)*(pBuf->l2 + i));
	    if ( i % 16 == 15 )
		    RLPP_PRINT("\n");
    }
    RLPP_PRINT("\n");
#endif
    /* DA, SA are the same as received RLPP packet */
	/* These two field need not to modify */

    l2Ptr = pBuf->l2;

    for ( logicPort = 1; logicPort < (LOGIC_PORT_NO + 1); logicPort++)
    {    
        if ((port_var[logicPort].port_state == RLPP_FORWARDING_STATE ||
			 port_var[logicPort].port_state == RLPP_LISTENING_STATE) &&
			(logicPort != best_port) && (TstLgcMaskBit(logicPort, &rlpp_en_pmsk) != 0)
		   )
        {        
            pBuf->l2 = l2Ptr;

   	        /* set DA */
            memset(pBuf->l2, 0xFF, MAC_ADDR_LEN);
			/* copy swmac to SA */
	        //memcpy(pBuf->l2 + MAC_ADDR_LEN, sid.swmac, MAC_ADDR_LEN);
	        memcpy(pBuf->l2 + MAC_ADDR_LEN, rlpp_srcmac, MAC_ADDR_LEN);
            
            *(pBuf->l2 + MAC_ADDR_LEN * 2)     = 0x90;
            *(pBuf->l2 + MAC_ADDR_LEN * 2 + 1) = 0x01;	

            pRlppPayload = (rlpp_payload_t*)(pBuf->l2 + sizeof(eth_hdr_t));
            pRlppPayload->proto_id = RTK_PROTO_ID_RLPP;

            memcpy(&pRlppPayload->pkt_port_vector.rid, &port_var[best_port].port_vector.rid, sizeof(rlpp_switch_id_t));
            memcpy(&pRlppPayload->pkt_port_vector.sid, &sid, sizeof(rlpp_switch_id_t));

            pRlppPayload->pkt_port_vector.hopcount = port_var[best_port].port_vector.hopcount + port_var[logicPort].hc;
            pRlppPayload->pkt_port_vector.swppri = port_var[logicPort].port_vector.swppri;
            pRlppPayload->pkt_port_vector.swport = logicPort;		

            pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
            ClrLgcMaskAll(&(pBuf->tx_dst_pmsk));
            SetLgcMaskBitNoCheck(logicPort, &(pBuf->tx_dst_pmsk));            
            pBuf->l2fmt = L2FMT_ETH;
            pBuf->l3fmt = L3FMT_NONE;
            pBuf->pkt_head = pBuf->l2;
            ClrLgcMaskAll(&l2_info.portmask);
            SetLgcMaskBitNoCheck(logicPort, &l2_info.portmask);            
            l2_send_by_port(pBuf->pkt_head, RLPP_PKT_LEN, &l2_info);
        }
    }

    /* kill warning message */
    i = 0;
}

INT32 rlpp_up(pktBuf_t *pBuf)
{
    port_num_t rx_port =0, best_port = 0;
    DRV_RET_E    iRet = DRV_OK;
    l2_send_op l2_info;
    rlpp_payload_t   *rlpp_payload = NULL;
    UINT32 uiAdmEn = DISABLE;
    
    if(NULL == pBuf)
    {
        return RTK_PROTO_ERR_FAILED;
    }    
    
    if ( rlpp_en == FALSE )
		return RTK_PROTO_ERR_OK;

    memset(&l2_info, 0, sizeof(l2_info));
    l2_info.usVid = 0;    
    rx_port = pBuf->spa;
#if RLPP_DBG_EN
    //RLPP_DBG("rx_port = %d", (UINT16)rx_port);
#endif
#if RLPP_DBG_EN
    //dbg_print_pkt((char *)pBuf->pkt_head, 64);
#endif

    if ( RLPP_DISABLE_STATE == port_var[rx_port].port_state )
    {
        //RLPP_DBG();
        return RTK_PROTO_ERR_OK;
    }
    if (FALSE != TstLgcMaskBit(rx_port, &rlpp_en_pmsk) && TRUE == rlpp_en )
    {
        rlpp_payload = (rlpp_payload_t*)pBuf->l3;
        /* hop count is over */
        if ( rlpp_payload->pkt_port_vector.hopcount >= RLPP_HOPCOUNT_MAX)
        {
            return RTK_PROTO_ERR_OK;
        }

#if RLPP_DBG_EN        
        RLPP_DBG("rx_port = %d, rlpp_payload->pkt_port_vector.swport = %d", (UINT16)rx_port, rlpp_payload->pkt_port_vector.swport);
#endif
        /* self loop */
        if ( memcmp(&sid, &rlpp_payload->pkt_port_vector.sid, sizeof(rlpp_switch_id_t)) == 0 && rx_port == rlpp_payload->pkt_port_vector.swport)
        {  
            if ( port_var[rx_port].slen )
            {
#if RLPP_DBG_EN                        
                RLPP_DBG("self loop detect,set lgcport %d to PORT_STATE_BLOCKING",rx_port);
#endif
                port_var[rx_port].pv_sl_enable = TRUE;
                port_var[rx_port].sl_ag = 0;
                port_var[rx_port].selfloop = TRUE;
                port_var[rx_port].port_state = RLPP_DISABLE_STATE;
                gstLoopdState->port[rx_port].curstate = PORT_STATE_BLOCKING;
                iRet = Ioctl_AclDropLoopdCreateAddPort(rx_port);
                iRet |= Ioctl_SetRStpPortState(rx_port, PORT_STATE_BLOCKING);              

                if(DRV_OK != iRet)
                {
                    return RTK_PROTO_ERR_FAILED;
                }
                _rlpp_switch_state_update();
            }
            return RTK_PROTO_ERR_OK;
        }

        /* receive better port vector */
        if ( memcmp(&rlpp_payload->pkt_port_vector.rid, &sid, sizeof(rlpp_switch_id_t)) < 0)
        {
            gen_rlppu = FALSE;
        }

        /* if switch is root and port vector of received is not better, return */
        if( TRUE == port_var[rx_port].pv_ag_enable )
        {
            if ( memcmp(&port_var[rx_port].port_vector, &rlpp_payload->pkt_port_vector, sizeof(rlpp_port_vector_t)) < 0)
            {
                return RTK_PROTO_ERR_OK;
            }
        }

        memcpy(&port_var[rx_port].port_vector, &rlpp_payload->pkt_port_vector, sizeof(rlpp_port_vector_t));

        port_var[rx_port].pv_ag_enable = TRUE;
        port_var[rx_port].pv_ag = 0;
        _rlpp_switch_state_update();
        _port_update_process();
		//_rlpp_switch_state_update();

        if ( gen_rlppu == FALSE)
        {
            best_port = _rlpp_port_own_best_pv();
            /* Received RLPPU is not the best one */
            if ( best_port != rx_port)
            {
                return RTK_PROTO_ERR_OK;
            }
        }
        else
        {
            return RTK_PROTO_ERR_OK;
        }
#if RLPP_DBG_EN        
        RLPP_DBG("_forward_process");
#endif
        _forward_process(best_port, pBuf);
    }
    else
    {
#if RLPP_DBG_EN                
        RLPP_DBG("normal forwarding");
#endif
        /* normal forwarding */
        /* write type to cover received cpu tag */
        *(pBuf->l3 - 2) = 0x90;
	      *(pBuf->l3 - 1) = 0x01;

	      /* move SA */
		    memcpy(pBuf->l3 - 2 - MAC_ADDR_LEN, pBuf->l2 + MAC_ADDR_LEN, MAC_ADDR_LEN);

        /* set DA as BCAST ADDR */
        memset(pBuf->l3 - sizeof(eth_hdr_t), 0xFF, MAC_ADDR_LEN);
		pBuf->l2 = pBuf->l3 - sizeof(eth_hdr_t);
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        SetLgcMaskAll(&l2_info.portmask); 
        ClrLgcMaskBit(rx_port, &l2_info.portmask);
        ClrLgcMaskBit(LOGIC_PON_PORT, &l2_info.portmask);//don't forward to pon port 
        SetLgcMaskAll(&(pBuf->tx_dst_pmsk));
	    ClrLgcMaskBit(rx_port, &(pBuf->tx_dst_pmsk));
        ClrLgcMaskBit(LOGIC_PON_PORT, &(pBuf->tx_dst_pmsk));
	    pBuf->l2fmt = L2FMT_ETH;
        pBuf->l3fmt = L3FMT_NONE;
        pBuf->pkt_head = pBuf->l2;
        l2_send_by_port(pBuf->pkt_head, RLPP_PKT_LEN, &l2_info);
    }
    
    return RTK_PROTO_ERR_OK;
}

static void GenerateRlppPkt()
{
    port_num_t logicPort; 
    l2_send_op l2_info;
    pktBuf_t        pBuf;
    rlpp_payload_t  *pRlppPayload;
    //UINT8 rlpp_srcmac[MAC_ADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x01};
    UINT8           *l2Ptr;

    memset(&pBuf, 0x0, sizeof(pktBuf_t));
    memset(p_tx_buffer, 0x0, sizeof(p_tx_buffer));
    memset(&l2_info, 0x0, sizeof(l2_info));
    l2_info.usVid = 0;

    pBuf.pkt_head = p_tx_buffer;
    pBuf.l2 = pBuf.pkt_head;
    /* set Type/Length */
    *(pBuf.l2 + MAC_ADDR_LEN * 2) = 0x90;
    *(pBuf.l2 + MAC_ADDR_LEN * 2 + 1) = 0x01;
			
    pRlppPayload = (rlpp_payload_t*)(pBuf.l2 + sizeof(eth_hdr_t));

    /* set protocol ID */
    pRlppPayload->proto_id = RTK_PROTO_ID_RLPP;

    /* set RID and SID */
    memcpy(&pRlppPayload->pkt_port_vector.rid, &sid, sizeof(rlpp_switch_id_t));
    memcpy(&pRlppPayload->pkt_port_vector.sid, &sid, sizeof(rlpp_switch_id_t));

    /* set hopcount */
    pRlppPayload->pkt_port_vector.hopcount = 0;

    l2Ptr = pBuf.l2;

    for ( logicPort = 1; logicPort < (LOGIC_PORT_NO+1); logicPort++ )
    {
        ClrLgcMaskAll(&l2_info.portmask);
        SetLgcMaskBitNoCheck(logicPort, &l2_info.portmask);
        pBuf.l2 = l2Ptr;

        /* set DA */
        memset(pBuf.l2, 0xFF, MAC_ADDR_LEN);
        /* copy swmac to SA */
		//memcpy(pBuf.l2 + MAC_ADDR_LEN, sid.swmac, MAC_ADDR_LEN);
		memcpy(pBuf.l2 + MAC_ADDR_LEN, rlpp_srcmac, MAC_ADDR_LEN);
        if (port_var[logicPort].port_state == RLPP_FORWARDING_STATE || 
            port_var[logicPort].port_state == RLPP_LISTENING_STATE)
        {
            pRlppPayload->pkt_port_vector.swppri = port_var[logicPort].ppri;
            pRlppPayload->pkt_port_vector.swport = logicPort;

            pBuf.tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
            ClrLgcMaskAll(&pBuf.tx_dst_pmsk);
            SetLgcMaskBitNoCheck(logicPort, &pBuf.tx_dst_pmsk);
            pBuf.l2fmt = L2FMT_ETH;
            pBuf.l3fmt = L3FMT_NONE;
            pBuf.pkt_head = pBuf.l2;
            (VOID)l2_send_by_port(pBuf.pkt_head, RLPP_PKT_LEN, &l2_info);
        }
    }
}

static INT32 rlppSentPkt2OnePort(port_num_t lgcPort)
{
    port_num_t logicPort; 
    l2_send_op l2_info;
    pktBuf_t        pBuf;
    rlpp_payload_t  *pRlppPayload = NULL;
    //UINT8 rlpp_srcmac[MAC_ADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x01};
    UINT8           *l2Ptr = NULL;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return RTK_PROTO_ERR_FAILED;
    }
    memset(&pBuf, 0x0, sizeof(pktBuf_t));
    memset(p_tx_buffer, 0x0, sizeof(p_tx_buffer));
    memset(&l2_info, 0x0, sizeof(l2_info));
    l2_info.usVid = 0;

    pBuf.pkt_head = p_tx_buffer;
    pBuf.l2 = pBuf.pkt_head;
    /* set Type/Length */
    *(pBuf.l2 + MAC_ADDR_LEN * 2) = 0x90;
    *(pBuf.l2 + MAC_ADDR_LEN * 2 + 1) = 0x01;
			
    pRlppPayload = (rlpp_payload_t*)(pBuf.l2 + sizeof(eth_hdr_t));

    /* set protocol ID */
    pRlppPayload->proto_id = RTK_PROTO_ID_RLPP;

    /* set RID and SID */
    memcpy(&pRlppPayload->pkt_port_vector.rid, &sid, sizeof(rlpp_switch_id_t));
    memcpy(&pRlppPayload->pkt_port_vector.sid, &sid, sizeof(rlpp_switch_id_t));

    /* set hopcount */
    pRlppPayload->pkt_port_vector.hopcount = 0;

    l2Ptr = pBuf.l2;
    
    pBuf.l2 = l2Ptr;
    /* set DA */
    memset(pBuf.l2, 0xFF, MAC_ADDR_LEN);
    /* copy swmac to SA */
	//memcpy(pBuf.l2 + MAC_ADDR_LEN, sid.swmac, MAC_ADDR_LEN);
	memcpy(pBuf.l2 + MAC_ADDR_LEN, rlpp_srcmac, MAC_ADDR_LEN);
    if (port_var[lgcPort].port_state == RLPP_FORWARDING_STATE || 
        port_var[lgcPort].port_state == RLPP_LISTENING_STATE)
    {
        pRlppPayload->pkt_port_vector.swppri = port_var[logicPort].ppri;
        pRlppPayload->pkt_port_vector.swport = logicPort;

        pBuf.tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        ClrLgcMaskAll(&l2_info.portmask);
        SetLgcMaskBitNoCheck(lgcPort, &l2_info.portmask);        
        ClrLgcMaskAll(&pBuf.tx_dst_pmsk);
        SetLgcMaskBitNoCheck(lgcPort, &pBuf.tx_dst_pmsk);
        pBuf.l2fmt = L2FMT_ETH;
        pBuf.l3fmt = L3FMT_NONE;
        pBuf.pkt_head = pBuf.l2;
        (VOID)l2_send_by_port(pBuf.pkt_head, RLPP_PKT_LEN, &l2_info);
    }

    return RTK_PROTO_ERR_OK;
}

extern sem_t g_iLoopdTimeSem;
void RlppHelloTimerHandle(int signal)
{
    port_num_t      logicPort;
    port_num_t      best_port;
    UINT8           loopFree;
    UINT32 uiAdmEn = DISABLE;
    
    if(SIGALRM != signal)
    {
        sem_post(&g_iLoopdTimeSem);
        return;
    }
    
    hello_timer += 1;      

    /* Hello timeout */
    if ( hello_timer >= RLPP_HELLO_TIME )
    {   
        hello_timer = 0;
        if ( gen_rlppu == TRUE )
        {
            GenerateRlppPkt();
        } 
    }
    /* update SL_AG and PV_AG and hello timer */    
    for ( logicPort = 1; logicPort < (LOGIC_PORT_NO+1); logicPort++ )
    {
        if ( TRUE == port_var[logicPort].pv_ag_enable )
            port_var[logicPort].pv_ag++;
        if ( TRUE == port_var[logicPort].pv_sl_enable)
            port_var[logicPort].sl_ag++;

    }

    for ( logicPort = 1; logicPort < (LOGIC_PORT_NO+1); logicPort++ )
    {
        (VOID)Ioctl_GetPortEnable(logicPort, &uiAdmEn);	
		if(ENABLE != uiAdmEn)
		{
			gstLoopdState->port[logicPort].curstate = PORT_STATE_DISABLED;
		}
        //printf("%s %d %d %d %d\n",__FUNCTION__,__LINE__,logicPort ,prevState[logicPort],port_var[logicPort].port_state);
        /* selfloop timeout */
        else if( TRUE == port_var[logicPort].slen )
        {
            if ( TRUE == port_var[logicPort].selfloop && RLPP_SL_TIME == port_var[logicPort].sl_ag )
            {
                /*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
                port_var[logicPort].sl_ag = 0;
                //(VOID)Ioctl_GetPortEnable(logicPort, &uiAdmEn);
                if(ENABLE == uiAdmEn)
                {
                    if ( RLPP_DISABLE_STATE == port_var[logicPort].port_state )
                    {
#if RLPP_DBG_EN                                    
                        RLPP_DBG("set logicport %d to PORT_STATE_LEARNING",logicPort);
#endif
                        port_var[logicPort].port_state = RLPP_LISTENING_STATE;
                        gstLoopdState->port[logicPort].curstate = PORT_STATE_LEARNING;
                        (VOID)Ioctl_AclDropLoopdCreateAddPort(logicPort);
                        (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_BLOCKING);                                    
                        port_var[logicPort].sl_ag = 0;

                    }
                    else
                    {
#if RLPP_DBG_EN                                                        
                        RLPP_DBG("set logicport %d to PORT_STATE_FORWARDING",logicPort); 
#endif
                        port_var[logicPort].selfloop = FALSE;
                        port_var[logicPort].pv_sl_enable = FALSE;
                        port_var[logicPort].port_state = RLPP_FORWARDING_STATE;
                        gstLoopdState->port[logicPort].curstate = PORT_STATE_FORWARDING;
                        (VOID)Ioctl_AclDropLoopdCreateRemovePort(logicPort);
                        (VOID)Ioctl_SetRStpPortState(logicPort, PORT_STATE_FORWARDING);                   
                        //GenerateRlppPkt();                     
                    }
                }
            }
            else
            {	
				if(PORT_STATE_DISABLED == gstLoopdState->port[logicPort].curstate)
				{
					gstLoopdState->port[logicPort].curstate = PORT_STATE_FORWARDING;
				}
            }
        }

        
        /* port timer timeout */
        if ( TRUE == port_var[logicPort].pv_ag_enable && 
			 port_var[logicPort].pv_ag == RLPP_PV_TIME)
        {
            port_var[logicPort].pv_ag_enable = FALSE;
            port_var[logicPort].pv_ag = 0;
            /*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/
            best_port = _rlpp_port_own_best_pv();
            /* all port vectors are invalid */
            if ( 0xFF == best_port )
            {
                gen_rlppu = TRUE;
            }
            else
            {               
                if ( memcmp(&sid, &port_var[best_port].port_vector.rid, sizeof(rlpp_switch_id_t)) <= 0 )
               	{
                    gen_rlppu = TRUE;
                }
                else
                {
                    gen_rlppu = FALSE;
                }
            }            
            _port_update_process();
            _rlpp_switch_state_update();			
        }
    }

	if ( TRUE == rlpp_sys_loop_state )
	{
        loopFree = TRUE;	
	    for(logicPort = 1; logicPort < (LOGIC_PORT_NO+1); logicPort++ )
        {
            if ( port_var[logicPort].port_state != RLPP_FORWARDING_STATE )
           	{
           	    loopFree = FALSE;
				break;
            }
        }

		if ( loopFree == TRUE )
		{
		    rlpp_sys_loop_state = FALSE;
		}
	} 
    sem_post(&g_iLoopdTimeSem);
}

INT32 rlpp_pkt_process(pktBuf_t *pktBuf, UINT16 len)
{   
    rlpp_payload_t  *pRlppPayload = NULL;
    INT32 iRet = RTK_PROTO_ERR_OK;

    if(NULL == pktBuf)
    {
        return RTK_PROTO_ERR_FAILED;
    }
    
    pRlppPayload = (rlpp_payload_t *)(pktBuf->l2 + sizeof(eth_hdr_t));
    if(RTK_PROTO_ID_RLPP == pRlppPayload->proto_id)
    {
        iRet = rlpp_up(pktBuf);
    }

    return iRet;
}


#ifdef  __cplusplus
}
#endif



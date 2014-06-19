/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 41092 $
 * $Date: 2013-07-11 22:15:02 +0800 (Thu, 11 Jul 2013) $
 *
 * Purpose : Definie the device driver that communication between
 *           kernel space and user space
 *
 * Feature : Provide kernel space APIs and actual data exchange
 *           source
 *
 */

/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <net/sock.h> 
#include <net/netlink.h> 
#include <linux/skbuff.h>

#include "re8686.h"
#include "pkt_redirect.h"
#include <rtk/epon.h>

/* 
 * Symbol Definition 
 */
/* The data base typedef only used in this file */
/* For user space application */
typedef struct pr_userAppDb_s {
    unsigned short uid;        /* User ID */
    int            pid;        /* Process ID */
    unsigned short mtu;        /* Maximum data size */
    struct pr_userAppDb_s *prev;
    struct pr_userAppDb_s *next;
} pr_userAppDb_t;

/* For kernel space application */
typedef struct pr_kernelAppDb_s {
    unsigned short uid;        /* User ID - assigned by kernel/user program itself */
    void (*appCallback)(unsigned short dataLen, unsigned char *data); /* Kernel application call back function */
    struct pr_kernelAppDb_s *prev;
    struct pr_kernelAppDb_s *next;
} pr_kernelAppDb_t;

/*  
 * Data Declaration  
 */
static struct sock *session_sock;
static pr_userAppDb_t *userAppDb;
static pr_kernelAppDb_t *kernelAppDb;

/*  
 * Function Declaration  
 */

/* ------------------------------------------------------------------
 * Device driver kernel part internal APIs
 */
static int pkt_redirect_userApp_add(pr_regUserApp_t *regData)
{
    pr_userAppDb_t *userApp, *searchPtr;

    userApp = (pr_userAppDb_t *) kmalloc(sizeof(pr_userAppDb_t), GFP_KERNEL);
    if(NULL == userApp)
    {
        return -1;
    }

    userApp->uid = regData->ownUid;
    userApp->pid = regData->ownPid;
    userApp->mtu = regData->mtu;
    userApp->prev = NULL;
    userApp->next = NULL;

    if(NULL == userAppDb)
    {
        userAppDb = userApp;
        userApp->prev = NULL;
    }
    else
    {
        searchPtr = userAppDb;
        while(NULL != searchPtr)
        {
            if(searchPtr->uid == regData->ownUid)
            {
                /* Update MTU and PID */
                searchPtr->pid = regData->ownPid;
                searchPtr->mtu = regData->mtu;
                kfree(userApp);

                return 0;
            }
	     if( NULL == searchPtr->next)
	     {
	     	  break;
	     }
            searchPtr = searchPtr->next;
        }
        searchPtr->next = userApp;
        userApp->prev = searchPtr;
        userApp->next = NULL;
    }

    return 0;
}

static pr_userAppDb_t *pkt_redirect_userApp_get(unsigned short uid)
{
    pr_userAppDb_t *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            return searchPtr;
        }
        searchPtr = searchPtr->next;
    }

    return NULL;
}

static int pkt_redirect_userApp_del(pr_regUserApp_t *regData)
{
    pr_userAppDb_t *userApp, *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        if((searchPtr->uid == regData->ownUid) &&
            (searchPtr->pid == regData->ownPid))
        {
            userApp = searchPtr;
            if(searchPtr->prev == NULL && searchPtr->next == NULL)
            {
                userAppDb = NULL;
            }
            else
            {
                if(searchPtr->prev != NULL)
                {
                    searchPtr->prev->next = searchPtr->next;
                }
                if(searchPtr->next != NULL)
                {
                    searchPtr->next->prev = searchPtr->prev;
                }
                if(userAppDb == searchPtr)
                {
                    userAppDb = searchPtr->next;
                }
                userApp->prev = NULL;
                userApp->next = NULL;
            }
            kfree(userApp);

            return 0;
        }
        searchPtr = searchPtr->next;
    }

    return -1;
}

static int pkt_redirect_userApp_delAll(void)
{
    pr_userAppDb_t *userApp, *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        userApp = searchPtr;
        searchPtr = searchPtr->next;
        kfree(userApp);
    }
    userAppDb = NULL;

    return 0;
}

static int pkt_redirect_kernelApp_add(unsigned short uid, void (*appCallback)(unsigned short dataLen, unsigned char *data))
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    kernelApp = (pr_kernelAppDb_t *) kmalloc(sizeof(pr_kernelAppDb_t), GFP_KERNEL);

    if(NULL == kernelApp)
    {
        return -1;
    }

    kernelApp->uid = uid;
    kernelApp->appCallback = appCallback;
    kernelApp->prev = NULL;
    kernelApp->next = NULL;
    if(NULL == kernelAppDb)
    {
        kernelAppDb = kernelApp;
        kernelApp->prev = NULL;
    }
    else
    {
        searchPtr = kernelAppDb;
        while(NULL != searchPtr)
        {
            if(searchPtr->uid == uid)
            {
                /* Update callback function only */
                searchPtr->appCallback = appCallback;
                kfree(kernelApp);

                return 0;
            }
	     if( NULL == searchPtr->next)
	     {
	     	  break;
	     }
            searchPtr = searchPtr->next;
        }
        searchPtr->next = kernelApp;
        kernelApp->prev = searchPtr;
        kernelApp->next = NULL;
    }

    return 0;
}

static pr_kernelAppDb_t *pkt_redirect_kernelApp_get(unsigned short uid)
{
    pr_kernelAppDb_t *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            return searchPtr;
        }
        searchPtr = searchPtr->next;
    }

    return NULL;
}

static int pkt_redirect_kernelApp_del(unsigned short uid)
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            kernelApp = searchPtr;
            if(searchPtr->prev == NULL && searchPtr->next == NULL)
            {
                kernelAppDb = NULL;
            }
            else
            {
                if(searchPtr->prev != NULL)
                {
                    searchPtr->prev->next = searchPtr->next;
                }
                if(searchPtr->next != NULL)
                {
                    searchPtr->next->prev = searchPtr->prev;
                }
                if(kernelAppDb == searchPtr)
                {
                    kernelAppDb = searchPtr->next;
                }
                kernelApp->prev = NULL;
                kernelApp->next = NULL;
            }
            kfree(kernelApp);

            return 0;
        }
        searchPtr = searchPtr->next;
    }

    return -1;
}

static int pkt_redirect_kernelApp_delAll(void)
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        kernelApp = searchPtr;
        searchPtr = searchPtr->next;
        kfree(kernelApp);
    }
    kernelAppDb = NULL;

    return 0;
}

static int pkt_redirect_send2User(
    pr_userAppDb_t *userApp,
    unsigned short dataLen,
    unsigned char *payload)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len;
    void *data;
    int ret;

    if((NULL == userApp) || (NULL == payload))
    {
        return -1;
    }

    len = NLMSG_SPACE(dataLen);
    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb)
    {
        return -3;
    }
    nlh = NLMSG_PUT(skb, userApp->pid, 0, 0, dataLen);
    nlh->nlmsg_flags = 0;
    data = NLMSG_DATA(nlh);
    memcpy(data, payload, dataLen);
    NETLINK_CB(skb).pid = 0; /* from kernel */ 
    NETLINK_CB(skb).dst_group = 0; /* unicast */

    ret = netlink_unicast(session_sock, skb, userApp->pid, MSG_DONTWAIT);
    if (ret < 0) 
    { 
        printk("send failed %d\n", ret);
        return -4;
    } 
    return 0;
     
    nlmsg_failure: /* Used by NLMSG_PUT */ 
    if (skb)
    {
        kfree_skb(skb);
    }
    return -5;
}

static void pkt_redirect_recv(struct sk_buff *skb)
{
    int ret;
    u_int uid, pid, seq, sid;
    unsigned int dataLen;
    void *data;
    struct nlmsghdr *nlh;
    pr_identifier_t *prId;
    pr_regUserApp_t *regData;
    pr_data_t *sendData;

    nlh = (struct nlmsghdr *)skb->data;
    pid = NETLINK_CREDS(skb)->pid;
    uid = NETLINK_CREDS(skb)->uid;
    sid = NETLINK_CB(skb).sid;
    seq = nlh->nlmsg_seq;
    data = NLMSG_DATA(nlh);
    dataLen = nlh->nlmsg_len;
    //printk("recv skb from user space uid:%d pid:%d seq:%d,sid:%d\n", uid, pid, seq, sid);

    prId = (pr_identifier_t *) data;
    //printk("OpType:%u pid:%d uid:%d\n", prId->prOpType, prId->pid, prId->uid);

    /* Check the packet prOpType to decide operation */
    switch(prId->prOpType)
    {
    case PKT_REDIRECT_OPTYPE_SEND_MSG:
        sendData = (pr_data_t *) data;
        //printk("PKT_REDIRECT_OPTYPE_SEND\n");
        //printk("uid: %u\n", sendData->uid);
        //printk("isUser: %d\n", sendData->isUser);
        //printk("flag: %u\n", sendData->flag);
        //printk("len: %u\n", sendData->dataLen);

        pkt_redirect_kernelApp_sendPkt(sendData->uid, sendData->isUser, sendData->dataLen, PR_DATA(sendData));
        break;
    case PKT_REDIRECT_OPTYPE_REGISTER:
        /* Register new */
        regData = (pr_regUserApp_t *) data;
        if(PKT_REDIRECT_REGTYPE_REGISTER == regData->regType)
        {
            ret = pkt_redirect_userApp_add(regData);
            //printk("PKT_REDIRECT_REGTYPE_REGISTER: %d\n", ret);
        }
        else if(PKT_REDIRECT_REGTYPE_DEREGISTER == regData->regType)
        {
            ret = pkt_redirect_userApp_del(regData);
            //printk("PKT_REDIRECT_REGTYPE_DEREGISTER: %d\n", ret);
        }

#if 0
        {
            pr_userAppDb_t *userApp, *searchPtru;
            pr_kernelAppDb_t *kernelApp, *searchPtrk;

            searchPtru = userAppDb;
            printk("userApp\n");
            while(NULL != searchPtru)
            {
                userApp = searchPtru;
                searchPtru = searchPtru->next;
                printk("uid: %u\n", userApp->uid);
                printk("pid: %d\n", userApp->pid);
                printk("mtu: %u\n", userApp->mtu);
            }

            searchPtrk = kernelAppDb;
            printk("kernelApp\n");
            while(NULL != searchPtrk)
            {
                kernelApp = searchPtrk;
                searchPtrk = searchPtrk->next;
                printk("uid: %u\n", kernelApp->uid);
                printk("CB: 0x%08x\n", (unsigned int) kernelApp->appCallback);
            }
        }
#endif
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------
 * Kernel space APIs for other kernel modules
 */
int pkt_redirect_kernelApp_reg(
    unsigned short uid,
    void (*appCallback)(unsigned short dataLen, unsigned char *data))
{
    return pkt_redirect_kernelApp_add(uid, appCallback);
}

int pkt_redirect_kernelApp_dereg(
    unsigned short uid)
{
    return pkt_redirect_kernelApp_del(uid);
}

/* Sender have to prepare the data and free after send */
int pkt_redirect_kernelApp_sendPkt(
    unsigned short dstUid,
    int            isUser,
    unsigned short dataLen,
    unsigned char *data)
{
    pr_kernelAppDb_t *kernelApp;
    pr_userAppDb_t *userApp;

    if(0 == isUser)
    {
        /* Kernel app -> kernel app */
        kernelApp = pkt_redirect_kernelApp_get(dstUid);
        if(NULL == kernelApp)
        {
            /* No such UID registered */
            return -1;
        }
        kernelApp->appCallback(dataLen, data);
    }
    else
    {
        /* Kernel app -> user app */
        userApp = pkt_redirect_userApp_get(dstUid);
        if(NULL == userApp)
        {
            /* No such UID/PID registered */
            return -1;
        }
        if(dataLen > userApp->mtu)
        {
            /* User app can't receive such large amount of data */
            return -2;
        }
        pkt_redirect_send2User(userApp, dataLen, data);
    }

    return 0;
}

#if defined(FPGA_DEFINED)
    #define RTL8690X_EPON_PORT_MASK  0xFF
#else
    #define RTL8690X_EPON_PORT_MASK  0x10
#endif
/*for debug oam please change to 0x3F*/

#ifdef CONFIG_EPON_FEATURE
void epon_oam_pkt_tx(unsigned short dataLen, unsigned char *data)
{
    struct tx_info txInfo;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    memset(&txInfo,0x0,sizeof(struct tx_info));
    txInfo.opts2.bit.cputag = 1;
    txInfo.opts2.bit.aspri = 1;
    txInfo.opts2.bit.cputag_pri = 7;

    txInfo.opts3.bit.tx_portmask = RTL8690X_EPON_PORT_MASK;
    re8686_tx_with_Info(data, dataLen, &txInfo);
}

void epon_oam_dyingGasp_tx(unsigned short dataLen, unsigned char *data)
{
    struct tx_info txInfo;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    memset(&txInfo,0x0,sizeof(struct tx_info));
    txInfo.opts1.bit.cputag_psel = 1;
    txInfo.opts2.bit.cputag = 1;
    txInfo.opts3.bit.tx_dst_stream_id = 0x7f;
    txInfo.opts3.bit.tx_portmask = RTL8690X_EPON_PORT_MASK;
    re8686_tx_with_Info(data, dataLen, &txInfo);
}

static int epon_register_pkt_accept_check(struct sk_buff *skb)
{
    rtk_epon_regReq_t regEntry;    
	#if 0
    printk("\n receive register packet mac [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x] flag:%d\n",
                    skb->data[0],
                    skb->data[1],
                    skb->data[2],
                    skb->data[3],
                    skb->data[4],
                    skb->data[5],
                    skb->data[22]);
	#endif				
    rtk_epon_registerReq_get(&regEntry);
	#if 0
    printk("\n local mac addr [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x]\n",
            regEntry.mac.octet[0],
            regEntry.mac.octet[1],
            regEntry.mac.octet[2],
            regEntry.mac.octet[3],
            regEntry.mac.octet[4],
            regEntry.mac.octet[5]);
    #endif
    if(skb->data[0]==0xFF && 
       skb->data[1]==0xFF &&
       skb->data[2]==0xFF &&
       skb->data[3]==0xFF &&
       skb->data[4]==0xFF &&
       skb->data[5]==0xFF)    
    {
        return 1;
    }
    
    if(skb->data[0]==regEntry.mac.octet[0] && 
       skb->data[1]==regEntry.mac.octet[1] &&
       skb->data[2]==regEntry.mac.octet[2] &&
       skb->data[3]==regEntry.mac.octet[3] &&
       skb->data[4]==regEntry.mac.octet[4] &&
       skb->data[5]==regEntry.mac.octet[5])    
    {
        return 1;
    }

    return 0;
    
}


static unsigned int mpcp_cnt=0;
static unsigned int inValidLaserCnt=0;
static unsigned int lastInValidLaserCnt=0;
static unsigned int errorCnt=0;
static unsigned int gateRxExpire=0;
static unsigned int mpcp_disc_cnt=0;

int epon_oam_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
    int ret;
    unsigned char chLlidIdx; 
    
    /* Filter only spa = PON and OAM frames */
    if((skb->data[12] == 0x88) && (skb->data[13] == 0x09) && (skb->data[14] == 0x03))
    {
        /* concate the LLID index to the end of the packet buffer */
        chLlidIdx = pRxInfo->opts2.bit.pon_stream_id;
        skb_put(skb, sizeof(chLlidIdx));
        *((unsigned char *)(skb->data + (skb->len - sizeof(chLlidIdx)))) = chLlidIdx;
        ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, skb->len, skb->data);
        if(ret)
        {
            printk("send to user app (%d) fail (%d)\n", PR_USER_UID_EPONOAM, ret);
        }
        /* Left the skb to be free by caller */
        return RE8670_RX_STOP;
    }


    if((skb->data[12] == 0x88) && (skb->data[13] == 0x08)&& (skb->data[14] == 0x00) &&  (skb->data[15] == 0x05))
    {
        unsigned int gateType;
		#if 0
        printk("\n receive register packet mac [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x] flag:%d\n",
                        skb->data[0],
                        skb->data[1],
                        skb->data[2],
                        skb->data[3],
                        skb->data[4],
                        skb->data[5],
                        skb->data[22]);
		#endif				
		if(epon_register_pkt_accept_check(skb)==0)
        {
            //printk("\n this register is not for this onu!!\n");
            return RE8670_RX_STOP;    
        }				
        
        if(skb->data[22] == 2)/*flag == deregister */
        {
            /*receive deregister packet*/    
            rtk_epon_llid_entry_t llidEntry;

            /*disable LLID*/            
            llidEntry.llidIdx = 0;
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            rtk_epon_llid_entry_set(&llidEntry);
        }
        else/*register packet*/
        {
            /*change gate trap type to normal*/
            gateType = (*((volatile unsigned int *)(0xBB036060)));
            gateType = gateType&0xFFFFFFE7;
            gateType = gateType | 0x10;
            (*((volatile unsigned int *)(0xBB036060))) = gateType;
            errorCnt = 0; 
        }
        return RE8670_RX_STOP;
        
    }    
    
    /*filter mpcp gate message*/
    if((skb->data[12] == 0x88) && (skb->data[13] == 0x08)&& (skb->data[14] == 0x00) &&  (skb->data[15] == 0x02))
    {
        unsigned int localtime,firstLocalTime;
        unsigned int mpcpGateStar;
        unsigned int benOn;
        unsigned int startTime=0,tmpData1,tmpData2;
        unsigned int laserPhase;

        if(skb->data[20] == 0x09)
        {
            rtk_epon_llid_entry_t llidEntry;
            mpcp_disc_cnt++;
#if 0
            if(mpcp_disc_cnt%10000 == 0)
                printk("\n mpcp discovery gate rx:(%d) \n",mpcp_disc_cnt);    
#endif
            /*disable LLID*/            
            llidEntry.llidIdx = 0;
            ret = rtk_epon_llid_entry_get(&llidEntry);
            /*register ok do not need to handle discovery gate*/
            if(llidEntry.valid==ENABLED)
                return RE8670_RX_STOP;
        }
        
        
        /*gate grant list start time*/        
        (*((volatile unsigned int *)(0xBB036018)))= 0x2081;
        tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
        startTime = tmpData1 & 0x00FFFFFF;
        (*((volatile unsigned int *)(0xBB036018)))= 0x2082;
        tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
        startTime = startTime+((tmpData2&0x000000FF)<<24);

        localtime = (*((volatile unsigned int *)(0xBB036230)));
        firstLocalTime = localtime;
        mpcp_cnt ++;
        
        memcpy(&mpcpGateStar,&(skb->data[21]),4); 

#if 1
        (*((volatile unsigned int *)(0xBB036018)))= 0x208B;
        benOn = (*((volatile unsigned int *)(0xBB03601C)));
        if(benOn & 0x00040000)
        {
            localtime = (*((volatile unsigned int *)(0xBB036230)));
            if(((startTime - localtime)< 0x0fffffff) && ((startTime - localtime) > 1))
            {
                inValidLaserCnt++;
                errorCnt++;
                      
            }
        }
        
        if(((mpcpGateStar - localtime) > 0x0fffffff))
        {
            gateRxExpire++;    
        }
#endif
        if(errorCnt >=5)
        {
            errorCnt=0;
            laserPhase = (*((volatile unsigned int *)(0xBB022a34)));
            if(laserPhase & 0x00008000)
            {
                laserPhase = laserPhase&0xFFFF7FFF;
                (*((volatile unsigned int *)(0xBB022a34))) = laserPhase;
            }
            else
            {
                laserPhase = laserPhase|0x00008000;
                (*((volatile unsigned int *)(0xBB022a34))) = laserPhase;
            }
            //printk("\n=====================================\n");
            //printk("\n laser phase change!!\n");
            //printk("\n=====================================\n");
        }

#if 0        
        if(mpcp_cnt%10000 == 0)        
        {
            #if 0
            printk("\n packet timestamp:%2.2x %2.2x %2.2x %2.2x\n",skb->data[16],
                                                                skb->data[17],
                                                                skb->data[18],
                                                                skb->data[19]);
            printk("\n local time: 0x%8.8x/0x%8.8x   startTime:0x%8.8x\n",localtime,firstLocalTime,startTime); 
            printk("\n packet gate start:%2.2x  Ben:0x%8.8x\n",mpcpGateStar,benOn);
            printk("\n packet gate len:%2.2x %2.2x\n",skb->data[25],skb->data[26]);
            #endif

            if((inValidLaserCnt-lastInValidLaserCnt)!=0)
                printk("\n [Ivalid Laser CNT]:%d  (%d)\n",inValidLaserCnt,(inValidLaserCnt-lastInValidLaserCnt));
            lastInValidLaserCnt=inValidLaserCnt;
            printk("\n gateRxExpire cnt:%d\n",gateRxExpire);


        }
#endif
        return RE8670_RX_STOP;
    }


    return RE8670_RX_CONTINUE;
}
#endif


int __init pkt_redirect_init(void)
{
    extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);

    /* Create netlink for communicate between kernel and user protocol */
    session_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, 0, pkt_redirect_recv, NULL, THIS_MODULE);
    pkt_redirect_userApp_delAll();
    pkt_redirect_kernelApp_delAll();
    userAppDb = NULL;
    kernelAppDb = NULL;
    if(NULL == session_sock)
    {
        printk("pkt_redirect_init failed!\n");
    }
    else
    {
        printk("pkt_redirect_init complete\n");
    }

#ifdef CONFIG_EPON_FEATURE
    printk("Register EPON for pkt_redirect module\n");
    /* For packet redirect to user space protocol */
    pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GMAC, epon_oam_pkt_tx);
    pkt_redirect_kernelApp_reg(PR_KERNEL_UID_EPONDYINGGASP, epon_oam_dyingGasp_tx);
    /* Hook on PON port only */
    drv_nic_register_rxhook(RTL8690X_EPON_PORT_MASK, RE8686_RXPRI_OAM, epon_oam_pkt_rx);
#endif

    return 0;
}

void __exit pkt_redirect_exit(void)
{
    pkt_redirect_userApp_delAll();
    pkt_redirect_kernelApp_delAll();
    sock_release(session_sock->sk_socket);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek kernel/user packet redirect module");
MODULE_AUTHOR("Elliot Cheng <elliot.cheng@realtek.com>");
EXPORT_SYMBOL(pkt_redirect_kernelApp_reg);
EXPORT_SYMBOL(pkt_redirect_kernelApp_dereg);
EXPORT_SYMBOL(pkt_redirect_kernelApp_sendPkt);

module_init(pkt_redirect_init);
module_exit(pkt_redirect_exit);


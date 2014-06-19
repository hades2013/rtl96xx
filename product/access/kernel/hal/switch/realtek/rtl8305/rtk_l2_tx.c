/*****************************************************************************
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <linux/autoconf.h>
#include <lw_type.h>
#include <linux/smp_lock.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#ifdef CHIPSET_RTL8328L
#include <drv/nic/nic.h>
#include <drv/swcore/sys.h>
#endif
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/if_vlan.h>
#include "hal_common.h"
#ifdef CHIPSET_OPL6750
#include "opl_drv.h"
#endif

#ifdef CHIPSET_RTL8328L
#define PKTBUF_ALLOC(size)      kmalloc(size, GFP_ATOMIC)
#define PKTBUF_FREE(pktbuf)     kfree(pktbuf)
#endif
#define RESERVE_SPACE           32
#define MAX_FRAME_LEN           1518
#define MIN_FRAME_LEN           60

/* attention, if you modify it, you should modify the same define in onu_voip_fe.c*/
#define OPL_CPU_IPORT_GBE     0     /*Gmac*/
#define OPL_CPU_IPORT_PON     1     /*Pon*/ 
#define OPL_CPU_IPORT_INVALID 0xFF


#define MAX_TABLE_ENTRY_COUNT       64
#define MAC_TABLE_AGING_TICK        1

/*未加lock保护*/
typedef struct tagETH_MAC_TABLE_ENTRY
{
    atomic_t   ucAge;      /* 0表示该表项未被占用。当一个表项被添加，该值为255，每隔一定时间就减1 */
    unsigned int    enPort;    
    unsigned short  usVlan;
    unsigned char   aucMac[6];  /* MAC地址作为该软件TABLE的索引 */
} ETH_MAC_TABLE_ENTRY_S;

static ETH_MAC_TABLE_ENTRY_S m_astDrvMacTable[MAX_TABLE_ENTRY_COUNT];
struct timer_list mac_tab_timer;
/*lint --e{551}*/
static spinlock_t mac_tab_lock=SPIN_LOCK_UNLOCKED;

#ifdef CHIPSET_RTL8328L
extern UINT32 _qosIntPriToDp[QOS_MAX_INTERNAL_PRIORITY];

/* Function Name:
 *      l2_tx_callback
 * Description:
 *      l2 packet TX callback function 
 * Input:
 *      unit    - unit id
 *      pPacket - pointer buffer to the packet 
 *      pCookie - cookie data buffer
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */ 
static void l2_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    struct sk_buff *pSkb;
    (void)unit;
    (void)pCookie;
    
    if (NULL == pPacket)
    {        
        goto _exit;
    }

    pSkb = (struct sk_buff *)(pPacket->buf_id);
    if (NULL == pSkb)
    {   
    	PKTBUF_FREE(pPacket);
        goto _exit;
    } 
    
    dev_kfree_skb_irq(pSkb);

    PKTBUF_FREE(pPacket);
_exit:      
    return;
} /* end of rtnic_tx_callback */

/* Function Name:
 *      k_l2_send_by_port
 * Description:
 *      k interface of send l2 packet
 * Input:
 *      pSkb    - send data
 *      op - send control option  
 *  
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */ 

DRV_RET_E k_l2_send_by_port(struct sk_buff *pSkb, l2_send_op *op)
{    
    drv_nic_pkt_t *pPacket;    
    port_num_t lport=0;
    phyid_t phyid = 0;
    
    if(NULL==pSkb || NULL==op)
    {
        return DRV_ERR_PARA;
    }
	if(pSkb->len>MAX_FRAME_LEN)
	{
		dev_kfree_skb(pSkb);
		return DRV_L2_SEND_ERR;
	}
	if((op->ucPriority!=L2_NO_PRIORITY)&&(op->ucPriority>QOS_MAX_INTERNAL_PRIORITY))
	{
		dev_kfree_skb(pSkb);
		return DRV_L2_SEND_ERR;
	}
        
    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (NULL == pPacket)
    {
        dev_kfree_skb(pSkb);
        return DRV_L2_SEND_ERR;
    }
    
    if(op->usVid!=0)
    {
        pSkb=__vlan_put_tag(pSkb,op->usVid);
    }
    
    /* setting the CPU tx tag */    
    pPacket->as_txtag = 1;
    
    /*convert port mask first,now we just copy it from user layer*/ 
   
    pPacket->tx_tag.dst_port_mask=0;
    IfLgcMaskSet(&op->portmask, lport)
    {
        phyid=PortLogic2PhyID(lport);   
        pPacket->tx_tag.dst_port_mask|=((0x1<<PORTID(phyid)));
    }  
    pPacket->tx_tag.as_dst_port_mask=1;   
    if(op->ucPriority!=L2_NO_PRIORITY)
    {
        pPacket->tx_tag.as_priority=1;
		pPacket->tx_tag.as_port_remark=1;
        pPacket->tx_tag.priority=op->ucPriority;
		pPacket->tx_tag.drop_precedence=_qosIntPriToDp[op->ucPriority];
    }  
	else
	{
		pPacket->tx_tag.as_priority=0;
	}
    pPacket->tx_tag.l2_recalculate=1;    
    pPacket->tx_tag.l3_recalculate=0;   
    pPacket->tx_tag.l4_recalculate=0;   
    pPacket->tx_tag.flags=0;
    if(op->ucLogicEnable==L2_LOGIC_ENABLE)
    {
        /* set tag status/qos/acl/vlan check/mac1x/attck prevent/vlan filter*/
        pPacket->tx_tag.flags|=0x3EFF;
    }
    else
    {
        /*only set tag status bit*/
        pPacket->tx_tag.flags|=0x80;    
    }    
   
    /* raw packet */
    pPacket->buf_id = (void *)pSkb;
    pPacket->head = pSkb->head;
    pPacket->data = pSkb->data;
    pPacket->tail = pSkb->tail;
    pPacket->end = pSkb->end;
    pPacket->length = pSkb->len;  
    pPacket->next = NULL;    
    if (RT_ERR_OK != drv_nic_pkt_tx(op->ucDevid, pPacket,l2_tx_callback, NULL))
    {
        PKTBUF_FREE(pPacket);
        dev_kfree_skb(pSkb);    
        return DRV_L2_SEND_ERR;
    }      
    return DRV_OK;    
}
#endif

extern int onu_start_xmit_iport(struct sk_buff *skb, int opl_cpu_iport);

/*****************************************************************************
 函 数 名  : skb_push_qtag
 输入参数  : struct sk_buff *skb  
             UINT16 usVid, 0 表示不需要添加tag头
             UINT8 ucPriority     附带的优先级信息
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :

*****************************************************************************/
void skb_push_qtag(struct sk_buff *pSkb, UINT16 usVid, UINT8 ucPriority)
{
    if(usVid){
        /*push switch header*/
        skb_push(pSkb, 4); //pSkb->data -=4
        memmove(pSkb->data, 
                pSkb->data + 4, 
                (2 * MAC_ADDR_LEN));
        *(pSkb->data + RTL_HEADER_OFF) = 0x81;
        *(pSkb->data + RTL_HEADER_OFF + 1) = 0x00;
        *(pSkb->data + RTL_HEADER_OFF + 2) = (uint8_t)(((usVid >> 8) & 0xF) | (ucPriority << 5));
        *(pSkb->data + RTL_HEADER_OFF + 3) = usVid & 0xFF;
    }    
}

/*****************************************************************************
 函 数 名  : check_add_del_rtl_head
 输入参数  : UINT8 * pBuf  
             INT32 len, 报文长度
 输出参数  : UINT32* rtl_pport_mask   switch head中的port id信息     
 返 回 值  : pull buf的长度
                  0: 未发现
                  其它，sizeof(stRtlHeader)   


*****************************************************************************/
INT32 check_add_del_rtl_head(UINT8 * pBuf, INT32 len, UINT32* rtl_pport)
{
    UINT8 *pbufTmp = NULL;
    RTL_HEADER_S stRtlHeader;
    INT32 rtl_size = sizeof(stRtlHeader);

    if ((NULL == pBuf) ||
        (60 > len) ||
        (NULL == rtl_pport))
    {
        return 0;
    }

    *rtl_pport = OPL_CPU_IPORT_INVALID;

    #if (1 == SWITCH_HEADER_POSITION_BEHIND_SRC_MAC)
    /*Realtek header is behind the src mac of the frame.*/
    /*Skip 1q vlan tag.*/
    if ((0x81 == pBuf[12]) && (0x00 == pBuf[13]))
    {
        pbufTmp = pBuf + 16;
        len -= 4;
    }
    else
    {
        pbufTmp = pBuf + 12;
    }

    if((0x88 == pbufTmp[0]) && (0x99 == pbufTmp[1])){
        memcpy((uint8_t *)&stRtlHeader, pbufTmp, (size_t)rtl_size);
        *rtl_pport = (UINT32)stRtlHeader.usPPortMask;

        memcpy(pbufTmp, pbufTmp+rtl_size, (size_t)((len - rtl_size) - 12));

        return rtl_size;
    }
    #else
    /*Realtek header is at the end of the frame.*/
    pbufTmp = pBuf + len - rtl_size;
    if((0x88 == pbufTmp[0]) && (0x99 == pbufTmp[1])){
        memcpy((uint8_t *)&stRtlHeader, pbufTmp, (size_t)rtl_size);
        *rtl_pport = (UINT32)stRtlHeader.usPPortMask;
        return rtl_size;
    }
    #endif
    
    return 0;
}


/* Function Name:
 *      Hal_L2send
 * Description:
 *      ioctl interface of l2 send
 * Input:
 *      pSkb    - send data
 *      op - send control option  
 *  
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */ 
DRV_RET_E Hal_L2send(unsigned char *pMsg,UINT uiLen,l2_send_op *pOp)
{
#ifdef CHIPSET_RTL8305
    UINT32  uiLPort;
    //UINT32  uiChipType;
    phyid_t phyid;
    struct sk_buff *pSkb=NULL; 
    RTL_HEADER_S stRtlHeader;
    int opl_cpu_iport;
    
    if((NULL == pMsg) || (0 == uiLen) || (MAX_FRAME_LEN < uiLen) || (NULL == pOp))
    {
        return DRV_ERR_PARA;
    }

    pSkb = alloc_skb(uiLen+RESERVE_SPACE,GFP_KERNEL);
    if(NULL == pSkb)
    {
        return DRV_L2_SEND_ERR;
    }
    skb_reserve(pSkb, RESERVE_SPACE);

    /*copy frame to address pointed by skb->tail and move skb->tail.*/
    if(copy_from_user(skb_put(pSkb,uiLen),pMsg,uiLen))
    {
        dev_kfree_skb(pSkb);
        return DRV_L2_SEND_ERR;
    }

    /*If vid in packet equals 0 or 0xFFF, remove the vlan tag.*/
    if ((0x81 == pSkb->data[12]) && (0x00 ==  pSkb->data[13]))
    {
        UINT16 usVlanId;
        usVlanId = ((pSkb->data[14] & 0xF) << 8) + pSkb->data[15];
        if((0 == usVlanId) || (0xFFF == usVlanId))
        {
            memmove(pSkb->data+15, pSkb->data+11, 12);

            skb_pull(pSkb, 4);//pSkb->data +=4
            
            uiLen = (uiLen >= (MIN_FRAME_LEN + 4))? (uiLen - 4) : MIN_FRAME_LEN;
        }
    }

    if((LgcMaskNotNull(&(pOp->portmask))) || (TstLgcMaskBitNoCheck((LOGIC_UPPON_PORT), (&(pOp->portmask))))){
        /*The logic port mask begins with bit 0.*/
        IfLgcMaskSetNoCheck(&(pOp->portmask), uiLPort)
        {
            /*add vid*/
            skb_push_qtag(pSkb, pOp->usVid, pOp->ucPriority);

			if (LOGIC_PON_PORT == uiLPort)
            {
                opl_cpu_iport = OPL_CPU_IPORT_PON;
            }
            /*Send to gmac with realtek header.*/
            else
            {   
                /*opl6750 port*/
                opl_cpu_iport = OPL_CPU_IPORT_GBE;

                /*should fill rtl head header*/
                //phyid = port_convert_for_tx(CHIP_TYPE_RTL8305, uiLPort);
                phyid = PortLogic2PhyID(uiLPort);
                memset(&stRtlHeader, 0, sizeof(stRtlHeader));
                stRtlHeader.usEthType = 0x8899;
				stRtlHeader.ucProtocol = 0x04;
                stRtlHeader.usPPortMask = (UINT16)(1U << phyid);
                //stRtlHeader.usPPortMask = (UINT16)phyid;


                #if (1 == SWITCH_HEADER_POSITION_BEHIND_SRC_MAC)
                /*push switch header behind src mac.*/
                skb_push(pSkb, sizeof(stRtlHeader)); //pSkb->data -=sizeof(stRtlHeader)
                memmove(pSkb->data, 
                        pSkb->data + sizeof(stRtlHeader), 
                        RTL_HEADER_OFF);
                memcpy(pSkb->data + RTL_HEADER_OFF, &stRtlHeader, sizeof(stRtlHeader));
                #else
                /*push switch header befor crc.*/
				skb_push(pSkb, sizeof(stRtlHeader));
				memmove(pSkb->data, 
                        pSkb->data + sizeof(stRtlHeader), 
                        uiLen);
                memcpy(pSkb->data + uiLen, &stRtlHeader, sizeof(stRtlHeader));
                #endif
            } 

            /*send frame*/
            if (onu_start_xmit_iport(pSkb, opl_cpu_iport))
            {
                dev_kfree_skb(pSkb);
                return DRV_L2_SEND_ERR;
            }
        }
    }
    else{
        /*如果 lport mask为空，那么就向Gbe接口，发送一个普通的以太网报文，不加realtek head*/
        /*add vid*/
        skb_push_qtag(pSkb, pOp->usVid, pOp->ucPriority);

        /*send frame*/
        if (onu_start_xmit_iport(pSkb, OPL_CPU_IPORT_GBE))
        {
            dev_kfree_skb(pSkb);
            return DRV_L2_SEND_ERR;
        }
    }
    return DRV_OK;
#endif
    
}
#if  0
{
#ifdef CHIPSET_RTL8328L
    struct sk_buff *pSkb=NULL;    
    
    if(pMsg==NULL || 0 ==uiLen || NULL == pOp)
    {
        return DRV_ERR_PARA;
    }
    pSkb=alloc_skb(uiLen+RESERVE_SPACE,GFP_KERNEL);
    if(pSkb==NULL)
    {
        return DRV_L2_SEND_ERR;
    }   
    skb_reserve(pSkb, CPU_SPACE);//for cpu tag      
 
    if(copy_from_user(skb_put(pSkb,uiLen),pMsg,uiLen))
    {
        dev_kfree_skb(pSkb);
        return DRV_L2_SEND_ERR;
    }     
    pSkb->nh.raw = pSkb->data+ETH_HLEN;
    pSkb->mac.raw =pSkb->data;  
    return k_l2_send_by_port(pSkb,pOp);
#else
#ifdef CHIPSET_OPL6750 && CHIPSET_RTL8305
//extern int set_ipmux_dma0_tx_buf_data(char *appbuf, int len);
extern int dev_queue_xmit(struct sk_buff *skb);

    UINT32  uiLPort;
    phyid_t phyid;
    struct sk_buff *pSkb=NULL; 
    OPL_HEADER_EGRESS_S stOplHeader;
    RTL_HEADER_S stRtlHeader;
    
    if((pMsg == NULL) || (0 ==uiLen) || (MAX_FRAME_LEN < uiLen) || (NULL == pOp))
    {
        return DRV_ERR_PARA;
    }

    pSkb=alloc_skb(uiLen+RESERVE_SPACE,GFP_KERNEL);
    if(pSkb==NULL)
    {
        return DRV_L2_SEND_ERR;
    }

    /*
    skb->data: points to the frame's beginning excepting cpu header.
    skb->tail: points to the frame's end. It points to the same address at beginning.
    */
    /*Skip opl6750 header and now skb->data points to beginning of frame.*/
    skb_reserve(pSkb, CPU_SPACE);

    /*copy frame to address pointed by skb->tail and move skb->tail.*/
    if(copy_from_user(skb_put(pSkb,uiLen),pMsg,uiLen))
    {
        dev_kfree_skb(pSkb);
        return DRV_L2_SEND_ERR;
    }

    /*If vid in packet equals 0 or 0xFFF, remove the vlan tag.*/
    if ((0x81 == pSkb->data[12]) && (0x00 ==  pSkb->data[13]))
    {
        UINT16 usVlanId;
        usVlanId = ((pSkb->data[14] & 0xF) << 8) + pSkb->data[15];
        if((0 == usVlanId) || (0xFFF == usVlanId))
        {
            memmove(pSkb->data + 12, pSkb->data + 16, uiLen - 16);
            //pSkb->data[uiLen - 4] = 0;
            //pSkb->data[uiLen - 3] = 0;
            //pSkb->data[uiLen - 2] = 0;
            //pSkb->data[uiLen - 1] = 0;
            pSkb->tail -= 4;
	        pSkb->len  -= 4;
            
            uiLen = (uiLen >= (MIN_FRAME_LEN + 4))? (uiLen - 4) : MIN_FRAME_LEN;
        }
    }
    
    memset(&stOplHeader, 0, sizeof(stOplHeader));
    memset(&stRtlHeader, 0, sizeof(stRtlHeader));
    
    IfLgcMaskSet(&(pOp->portmask), uiLPort)
    {
        /*Send to pon*/
        if (LOGIC_PON_PORT == uiLPort)
        {
            //set_ipmux_dma0_tx_buf_data();
            
            /*fill cpu header*/
            stOplHeader.eport = 1;
            //memcpy(pSkb->head, &stOplHeader, sizeof(stOplHeader));
        }
        /*Send to uni*/
        else
        {
            /*fill cpu header*/
            stOplHeader.eport = 0;
            //memcpy(pSkb->head, &stOplHeader, sizeof(stOplHeader));

            /*initiate switch header*/
            phyid = PortLogic2PhyID(uiLPort);
            stRtlHeader.usPPortMask |= (UINT16)(1U << phyid);
        } 
    }

    /*fill cpu header*/
    memcpy(pSkb->head, &stOplHeader, sizeof(stOplHeader));
    
    /*fill switch header*/
    memmove(pSkb->data + RTL_HEADER_OFF + sizeof(stRtlHeader), 
            pSkb->data + RTL_HEADER_OFF, 
            uiLen - RTL_HEADER_OFF);
    memcpy(pSkb->data + RTL_HEADER_OFF, &stRtlHeader, sizeof(stRtlHeader));
    (void)skb_put(pSkb,sizeof(stRtlHeader));

    /*send frame*/
    if (NET_XMIT_SUCCESS != dev_queue_xmit(pSkb))
    {
        dev_kfree_skb(pSkb);
        return DRV_L2_SEND_ERR;
    }

    dev_kfree_skb(pSkb);
    return DRV_OK;
#endif
#endif
}
#endif /* #if 0 */

void Hal_test_L2send(struct sk_buff *skb)
{
    l2_send_op Op;
    l2_send_op* pOp= &Op;
    static int test_flag=0;

    test_flag++;
    Op.portmask.pbits[0] = (1 << test_flag);
    Op.usVid = 0xF34;
    Op.ucPriority = 6;

    Hal_L2send(skb->data, skb->len, pOp);
}

static void Drv_MT_Age(unsigned long flag)
{
    int i = 0;
    (void)flag;
    
    spin_lock_bh(&mac_tab_lock);

    for (i = 0; i < MAX_TABLE_ENTRY_COUNT; i++)
    {
        if(atomic_read(&m_astDrvMacTable[i].ucAge)){
            atomic_dec(&m_astDrvMacTable[i].ucAge);
        }
    }

    spin_unlock_bh(&mac_tab_lock);

    mod_timer(&mac_tab_timer, jiffies + MAC_TABLE_AGING_TICK* HZ);

    return;
}

void Drv_MT_Clear(void)
{
    int i = 0;
    
    spin_lock_bh(&mac_tab_lock);
    for (i = 0; i < MAX_TABLE_ENTRY_COUNT; i++)
    {
        /* 如果表项的age大于0，则将其设置为1，让其很快自动老化 */
        /* 不设置为0是因为这里没有信号量保护，避免设置为0后又被减1，从而变成255 */
        if(atomic_read(&m_astDrvMacTable[i].ucAge)){
            atomic_set(&m_astDrvMacTable[i].ucAge, 1);
        }
        else{
            atomic_set(&m_astDrvMacTable[i].ucAge, 0);
        }
    }
    spin_unlock_bh(&mac_tab_lock);
    
    return;
}

int Drv_MT_LookForEntry(unsigned char aucMac[6])
{
    int i = 0;

    if (NULL == aucMac)
    {
        return -1;
    }

    for (i = 0; i < MAX_TABLE_ENTRY_COUNT; i++)
    {
        if (   (m_astDrvMacTable[i].aucMac[0] == aucMac[0]) 
            && (m_astDrvMacTable[i].aucMac[1] == aucMac[1]) 
            && (m_astDrvMacTable[i].aucMac[2] == aucMac[2]) 
            && (m_astDrvMacTable[i].aucMac[3] == aucMac[3]) 
            && (m_astDrvMacTable[i].aucMac[4] == aucMac[4]) 
            && (m_astDrvMacTable[i].aucMac[5] == aucMac[5]))
        {
            
            if (atomic_read(&m_astDrvMacTable[i].ucAge) > 0)
            {
                return i;
            }
        }
    }

    return -1;
}

/*****************************************************************************
 函 数 名  : Drv_MT_GetPortByMac
 输入参数  : unsigned char aucMac[6]               
 输出参数  : *pVid, 
 返 回 值  : int opl_cpu_iport, 
                  OPL_CPU_IPORT_INVALID表示没有找到,此时*pVid也没有意义
 调用函数  : 
 被调函数  : 
 
 修改历史      :

*****************************************************************************/
int Drv_MT_GetPortByMac(unsigned char aucMac[6], uint16_t* pVid)
{
    int i = 0;

    if (NULL == aucMac)
    {
        return OPL_CPU_IPORT_INVALID;
    }

    for (i = 0; i < MAX_TABLE_ENTRY_COUNT; i++)
    {
        if (   (m_astDrvMacTable[i].aucMac[0] == aucMac[0]) 
            && (m_astDrvMacTable[i].aucMac[1] == aucMac[1]) 
            && (m_astDrvMacTable[i].aucMac[2] == aucMac[2]) 
            && (m_astDrvMacTable[i].aucMac[3] == aucMac[3]) 
            && (m_astDrvMacTable[i].aucMac[4] == aucMac[4]) 
            && (m_astDrvMacTable[i].aucMac[5] == aucMac[5]))
        {
            if (atomic_read(&m_astDrvMacTable[i].ucAge) > 0)
            {
                * pVid = m_astDrvMacTable[i].usVlan;
                return (int)m_astDrvMacTable[i].enPort;
            }
        }
    }

    return OPL_CPU_IPORT_INVALID;
}

int Drv_MT_LookForEmptyOrOldestEntry(void)
{
    int i = 0;
    int min_index = 0;    

    for (i = 0; i < MAX_TABLE_ENTRY_COUNT; i++)
    {
        if (0 == atomic_read(&m_astDrvMacTable[i].ucAge))
        {
            return i;
        }
        if (atomic_read(&m_astDrvMacTable[i].ucAge) < atomic_read(&m_astDrvMacTable[i].ucAge))
        {
            min_index = i;
        }
    }

    return min_index;
}

void Drv_MT_AddEntry(unsigned char* pucPkt, int enPort)
{
    int iIndex = -1;
    unsigned char* pucSrcMac = NULL;

    if (NULL == pucPkt)
    {
        return;
    }

    pucSrcMac = pucPkt + 6;
    if((0xFF == pucSrcMac[0]) && (0xFF == pucSrcMac[1]) && (0xFF == pucSrcMac[2]) && \
        (0xFF == pucSrcMac[3]) && (0xFF == pucSrcMac[4]) && (0xFF == pucSrcMac[5]))
    {
        /*广播不学习，组播学习*/
        return;
    }

    iIndex = Drv_MT_LookForEntry(pucSrcMac);
    if ((-1 == iIndex) || (MAX_TABLE_ENTRY_COUNT <= iIndex))
    {
        iIndex = Drv_MT_LookForEmptyOrOldestEntry();
        if ((-1 == iIndex) || (iIndex >= MAX_TABLE_ENTRY_COUNT))
        {
            return;
        }
    }

    spin_lock_bh(&mac_tab_lock);
    atomic_set(&m_astDrvMacTable[iIndex].ucAge, 0xFF);//aging time
        
    m_astDrvMacTable[iIndex].enPort = (uint32_t)enPort;
    if ((0x81 == pucPkt[12]) && (0x00 == pucPkt[13]))
    {
        m_astDrvMacTable[iIndex].usVlan = pucPkt[14] << 8 | pucPkt[15];
    }
    else
    {
        m_astDrvMacTable[iIndex].usVlan = 0;
    }
    
    m_astDrvMacTable[iIndex].aucMac[0] = pucSrcMac[0];
    m_astDrvMacTable[iIndex].aucMac[1] = pucSrcMac[1];
    m_astDrvMacTable[iIndex].aucMac[2] = pucSrcMac[2];
    m_astDrvMacTable[iIndex].aucMac[3] = pucSrcMac[3];
    m_astDrvMacTable[iIndex].aucMac[4] = pucSrcMac[4];
    m_astDrvMacTable[iIndex].aucMac[5] = pucSrcMac[5];
    spin_unlock_bh(&mac_tab_lock);    

    return;
}

int zj_debug(struct seq_file *m, void *v)
{
    int ii;
    uint8_t* mac;

    (void)v;
    
    for ( ii = 0 ; ii < MAX_TABLE_ENTRY_COUNT; ii++ )
    {
        if(atomic_read(&m_astDrvMacTable[ii].ucAge)){
            mac = m_astDrvMacTable[ii].aucMac;
            seq_printf(m, "Index=%d, ucAge=%d,mac=%02x:%02x:%02x:%02x:%02x:%02x, enPort=%d,usVlan=%d\n", \
                ii, atomic_read(&m_astDrvMacTable[ii].ucAge), \
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], \
                m_astDrvMacTable[ii].enPort, m_astDrvMacTable[ii].usVlan);
        }
    }
    
    return 0;
}

int __init rtl_l2_init(void)
{
    spin_lock_init(&mac_tab_lock);
    Drv_MT_Clear();
    
    init_timer(&mac_tab_timer);
	mac_tab_timer.function = Drv_MT_Age;
	mac_tab_timer.data = 0;
	mod_timer(&mac_tab_timer, jiffies + (MAC_TABLE_AGING_TICK * HZ));

    return 0;
}

module_init(rtl_l2_init);

#ifdef  __cplusplus
}
#endif


/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
#include "lw_drv_pub.h"
#if defined(__KERNEL__)
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <unistd.h>
#endif
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
/*��ȡbitNo���ڵ�˫�ֺţ���0��ʼ*/
#define GET_WORD_NO(bitNo)  ((bitNo)/PORT_MASK_WORD_WIDTH)
/*��bitNoת������˫�ֱ�ʾ��mask*/
#define BIT_TO_MASK(bitNo)  (1U<<((bitNo) % PORT_MASK_WORD_WIDTH))
/*��ȡmask�еĵ�word��˫��*/
#define GET_MASK_WORD(mask,word)   (((sysWord_t *)(&(mask)))[(word)])
/*����mask����op�������������bma��*/
#define MASK_OPERATION(bma, bmb, op)	do { \
		UINT32  _w; \
		for (_w = 0; _w < PORT_MASK_WORD_MAX; _w++) { \
			GET_MASK_WORD(bma, _w) op GET_MASK_WORD(bmb, _w); \
		} \
	} while (0)

#define SET_MASK_BIT(bitNo,mask)   do{\
    GET_MASK_WORD((mask),GET_WORD_NO(bitNo)) |= BIT_TO_MASK(bitNo);\
}while(0)

#define CLR_MASK_BIT(bitNo,mask)   do{\
    GET_MASK_WORD((mask),GET_WORD_NO(bitNo)) &= ~(BIT_TO_MASK(bitNo));\
}while(0)

#define TST_MASK_BIT(bitNo,mask) \
    ((GET_MASK_WORD((mask),GET_WORD_NO(bitNo)) & BIT_TO_MASK(bitNo)) != 0)

#if 0    
#define CLR_MASK_ALL(mask)   do{\
    memset(&(mask),0,sizeof(mask));\
}while(0)
#endif

/*****************************************************************************


*****************************************************************************/
BOOL IsValidLgcPort(port_num_t ucLogicPort)
{
#if 1
    if ((LOGIC_CPU_PORT != ucLogicPort) && 
        (LOGIC_PON_PORT != ucLogicPort) &&
        ((ucLogicPort > LOGIC_PORT_NO)||(ucLogicPort < 1))) 
#else
    if ((ucLogicPort > LOGIC_PORT_NO)||(ucLogicPort < 1)) 
#endif
    {
        return FALSE;
    }
    else{
        return TRUE;
    }
}

/*****************************************************************************

*****************************************************************************/
void SetLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr)
{
    if (!IsValidLgcPort(ucLogicPort)) {
        return;
    }
    if(NULL == lgcPMask_ptr) {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    SET_MASK_BIT((UINT32)(ucLogicPort-1),*lgcPMask_ptr);
}

void SetLgcMaskBitNoCheck(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr)
{
    if(NULL == lgcPMask_ptr) {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    SET_MASK_BIT((UINT32)(ucLogicPort-1),*lgcPMask_ptr);
}

/*****************************************************************************

*****************************************************************************/
void ClrLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr)
{
    if (!IsValidLgcPort(ucLogicPort)) {
        return;
    }
    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    CLR_MASK_BIT((UINT32)(ucLogicPort-1),*lgcPMask_ptr);
}

/*****************************************************************************

*****************************************************************************/
void ClrLgcMaskAll(logic_pmask_t *lgcPMask_ptr)
{
    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    memset(lgcPMask_ptr,0,sizeof(logic_pmask_t));
}

/*****************************************************************************

*****************************************************************************/
void SetLgcMaskAll(logic_pmask_t *lgcPMask_ptr)
{
    port_num_t lport;
    
    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    memset(lgcPMask_ptr,0x00,sizeof(logic_pmask_t));
    LgcPortFor(lport)
    {
        SetLgcMaskBit(lport, lgcPMask_ptr);
    }
}

/*****************************************************************************

*****************************************************************************/
void ClrLgcMaskNotUsed(logic_pmask_t *lgcPMask_ptr)
{
    UINT32 i;
    
    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }

    for(i=0; i<LOGIC_MASK_WIDTH; i++)
    {
        if(FALSE == IsValidLgcPort(i+1))
        {
            CLR_MASK_BIT(i, (*lgcPMask_ptr));
        }
    }
}

/*****************************************************************************

*****************************************************************************/
BOOL TstLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr)
{
    if (!IsValidLgcPort(ucLogicPort)) {
        return FALSE;
    }
    if(NULL == lgcPMask_ptr) { 
        ASSERT(NULL != lgcPMask_ptr);
        return FALSE;
    }

    if(TST_MASK_BIT((UINT32)(ucLogicPort-1), *lgcPMask_ptr)){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

BOOL TstLgcMaskBitNoCheck(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr)
{
    if(NULL == lgcPMask_ptr) { 
        ASSERT(NULL != lgcPMask_ptr);
        return FALSE;
    }

    if(TST_MASK_BIT((UINT32)(ucLogicPort-1), *lgcPMask_ptr)){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

/*��uint32���͵�portmask ת���logic_pmask_t
  pUint32PortMask lport=1 ����bit-1
  logic_pmask_t lport=1������bit-0
*/
BOOL TransUnit32MaskToLgcMask(unsigned int* pUint32PortMask, logic_pmask_t *lgcPMask_ptr)
{
    
    lgcPMask_ptr->pbits[0] = *pUint32PortMask >> 1;

    return TRUE;
}

/*****************************************************************************

*****************************************************************************/
BOOL LgcMaskNotNull(logic_pmask_t *lgcPMask_ptr)
{
    UINT32 lport;
    
    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return FALSE;
    }
    LgcPortFor(lport)
    {
        if(TST_MASK_BIT((lport-1), *lgcPMask_ptr)){
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************

*****************************************************************************/
BOOL LgcMaskIsFull(logic_pmask_t *lgcPMask_ptr)
{
    UINT32 lport;

    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return FALSE;
    }

    LgcPortFor(lport)
    {
        if(!TST_MASK_BIT((lport-1), *lgcPMask_ptr)){
            return FALSE;
        }
    }

    return  TRUE;
}

/*****************************************************************************

*****************************************************************************/
void LgcMaskCopy(logic_pmask_t *dstMsk,logic_pmask_t *srcMsk)
{
    if((NULL == dstMsk) || (NULL == srcMsk))
    {
        ASSERT(NULL != dstMsk);
        ASSERT(NULL != srcMsk);
        return;
    }
    memcpy(dstMsk,srcMsk,sizeof(logic_pmask_t));
}

/*****************************************************************************

*****************************************************************************/
void LgcMaskAnd(logic_pmask_t *mska,logic_pmask_t *mskb)
{
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    MASK_OPERATION(*mska, *mskb, &=);
}

/*****************************************************************************

*****************************************************************************/
void LgcMaskOr(logic_pmask_t *mska,logic_pmask_t *mskb)
{
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    MASK_OPERATION(*mska, *mskb, |=);
}

/*****************************************************************************

*****************************************************************************/
void LgcMaskXor(logic_pmask_t *mska,logic_pmask_t *mskb)
{
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    MASK_OPERATION(*mska, *mskb, ^=);
}

/*****************************************************************************

*****************************************************************************/
void LgcMaskRemove(logic_pmask_t *mska,logic_pmask_t *mskb)
{
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    MASK_OPERATION(*mska, *mskb, &= ~);
}

/*****************************************************************************
*****************************************************************************/
void LgcMaskNegate(logic_pmask_t *mska,logic_pmask_t *mskb)
{
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    MASK_OPERATION(*mska, *mskb, = ~);
}

/*****************************************************************************
  ������: GetLgcMinPort
  ��  ��: logic_pmask_t *lgcPMask_ptr   -   �߼�����
  ��  ��: ��
  ��  ��: port_num_t    -   ��С�˿ں�
          0xFF          -   ������û�ж˿ڴ���
 

*****************************************************************************/
port_num_t GetLgcMinPort(logic_pmask_t *lgcPMask_ptr)
{
    port_num_t lport;
    UINT32 portNum;

    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return INVALID_PORT;
    }

    portNum = 0;
    IfLgcMaskSet(lgcPMask_ptr,lport)
    {
        portNum ++;
        break;
    }

    if(portNum != 0)
    {
        return lport;
    }
    else
    {
        return INVALID_PORT;
    }
}

/*****************************************************************************
  ������: GetLgcMskPortNum
  ��  ��: logic_pmask_t *lgcPMask_ptr   -   �߼�����
  ��  ��: ��
  ��  ��: INT32 num                     -   �����еĶ˿���
 

*****************************************************************************/
INT32 GetLgcMskPortNum(logic_pmask_t *lgcPMask_ptr)
{
    INT32 num;
    port_num_t lport;

    if(NULL == lgcPMask_ptr)
    {
        ASSERT(NULL != lgcPMask_ptr);
        return 0;
    }

    num = 0;
    IfLgcMaskSet(lgcPMask_ptr, lport)
    {
        num ++;
    }
    return num;
}


/*****************************************************************************
 �� �� ��  : TstLgcMaskAndIsNull
 �������  : mska
                            mskb    
 �������  : ��
 �� �� ֵ  : TRUE: Ϊ0
                        FALSE:��������Ϊ0
 ���ú���  : 
 ��������  : 
 

*****************************************************************************/
BOOL TstLgcMaskAndIsNull( logic_pmask_t mska, logic_pmask_t mskb)
{
    logic_pmask_t mska1,mskb1;
    UINT32 lport;
    
    LgcMaskCopy(&mska1,&mska);
    LgcMaskCopy(&mskb1,&mskb);
    MASK_OPERATION(mska1,mskb1,&=);
    
    LgcPortFor(lport)
    {
        if(TST_MASK_BIT((lport-1), mska1))
        {
            return FALSE;
        }
    }
    return TRUE;   
}

/*****************************************************************************
 �� �� ��  : TstLgcMaskRemoveIsNull
 ��������  : ����mska&(~makb)�Ƿ�Ϊ0
 �������  : mska
                            mskb    
 �������  : ��
 �� �� ֵ  : TRUE: Ϊ0
                        FALSE:��������Ϊ0
 ���ú���  : 
 ��������  : 
 

*****************************************************************************/
BOOL TstLgcMaskRemoveIsNull( logic_pmask_t mska, logic_pmask_t mskb)
{
    logic_pmask_t mska1,mskb1;
    UINT32 lport;
    
    LgcMaskCopy(&mska1,&mska);
    LgcMaskCopy(&mskb1,&mskb);
    MASK_OPERATION(mska1,mskb1,&=~);
    
    LgcPortFor(lport)
    {
        if(TST_MASK_BIT((lport-1), mska1))
        {
            return FALSE;
        }
    }
    return TRUE;   
}

/*****************************************************************************
    Func Name: PortUser2Logic
  Description: ���û��˿�ת��Ϊ�߼��˿�
        Input: user_port_num_t stUsrPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
port_num_t PortUser2Logic(user_port_num_t stUsrPort)
{
    port_num_t lport = 0;
    
    if(!IsValidUserPort(stUsrPort))
    {
        ASSERT(IsValidUserPort(stUsrPort));
        return INVALID_PORT;
    }

    LgcPortFor(lport)
    {
        if((DEV_NO(PORT_USERPORT(lport)) == DEV_NO(stUsrPort)) && \
            (SLOT_NO(PORT_USERPORT(lport)) == SLOT_NO(stUsrPort)) && \
            (PORT_NO(PORT_USERPORT(lport)) == PORT_NO(stUsrPort)))
        {
            return lport;
        }

        if(IS_COMBO_PORT(lport))
        {
            if((DEV_NO(PORT_COMBO_SEC_USERPORT(lport)) == DEV_NO(stUsrPort)) && \
                (SLOT_NO(PORT_COMBO_SEC_USERPORT(lport)) == SLOT_NO(stUsrPort)) && \
                (PORT_NO(PORT_COMBO_SEC_USERPORT(lport)) == PORT_NO(stUsrPort)))
            {
                return lport;
            }
        }
    }
    return INVALID_PORT;
}

/*****************************************************************************
    Func Name: IsComboFiberUserPort
  Description: �ж��û��˿��Ƿ�Ϊ���
        Input: user_port_num_t stUsrPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL IsComboFiberUserPort(user_port_num_t stUsrPort)
{
    BOOL lport = 0;
    
    if(!IsValidUserPort(stUsrPort))
    {
        ASSERT(IsValidUserPort(stUsrPort));
        return FALSE;
    }

    LgcPortFor(lport)
    {
        if(IS_COMBO_PORT(lport))
        {
            if((DEV_NO(PORT_COMBO_SEC_USERPORT(lport)) == DEV_NO(stUsrPort)) && \
                (SLOT_NO(PORT_COMBO_SEC_USERPORT(lport)) == SLOT_NO(stUsrPort)) && \
                (PORT_NO(PORT_COMBO_SEC_USERPORT(lport)) == PORT_NO(stUsrPort)))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*****************************************************************************
    Func Name: PortLogic2User
  Description: ���߼��˿�ת��Ϊ�û��˿�
        Input: port_num_t lport  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
user_port_num_t PortLogic2User(port_num_t lport)
{
    if (!IsValidLgcPort(lport)) {
        ASSERT(IsValidLgcPort(lport));
        return INVALID_PORT;
    }

    return PORT_USERPORT(lport);
}

/*****************************************************************************
    Func Name: PortLogic2ComboSecUser
  Description: ���߼��˿�ת��ΪCombo�ĵڶ����û��˿�
        Input: port_num_t lport  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
user_port_num_t PortLogic2ComboSecUser(port_num_t lport)
{
    if (!IsValidLgcPort(lport)) {
        ASSERT(IsValidLgcPort(lport));
        return INVALID_PORT;
    }

    if(!IS_COMBO_PORT(lport))
    {
        return INVALID_PORT;
    }

    return PORT_COMBO_SEC_USERPORT(lport);
}

/*****************************************************************************
    Func Name: IsValidUserPort
  Description: �ж��Ƿ��ǺϷ����û��˿�
        Input: user_port_num_t stUsrPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL IsValidUserPort(user_port_num_t stUsrPort)
{
    /*�ж����֧�ֵ��豸��*/
    if(DEV_NO(stUsrPort) > MAX_DEVICE_NUM)
    {
        return FALSE;
    }
    
    if(SLOT_NO(stUsrPort) >= MAX_SLOT_NUM)
    {
        return FALSE;
    }

    if((PORT_NO(stUsrPort) > USER_PORT_MAX_NUM(DEV_NO(stUsrPort), SLOT_NO(stUsrPort))) || \
        (PORT_NO(stUsrPort) == 0))
    {
        return FALSE;
    }

    return TRUE;
}

#if !defined(__KERNEL__)
#if CONFIG_WDT
#define WTD_DEVNAME "/dev/mw_wtd_dev"
#define IOC_WTD_COMMIT (WDIOC_KEEPALIVE)

void Drv_updateWatchdog(void)
{
	int fd,ret;	

	fd = open ( WTD_DEVNAME, O_RDWR );
	if( fd <0 )
	{
		return;
	}

	/* enable wtd device */
	ret = ioctl (fd ,(unsigned long )IOC_WTD_COMMIT,0);
    
	if( ret < 0 )
	{
		close(fd);
		return;
	}

	close(fd);
	return;
}


#endif
#endif

#ifdef  __cplusplus
}
#endif


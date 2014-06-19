/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h> 
#include "lw_drv_pub.h"
#include "hal_common.h"
#include <linux/string.h>
#ifdef CHIPSET_RTL8328
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/rt_type.h>
#endif
#ifdef UT_TEST
#include <stdio.h>
#include <string.h>
#endif

/*****************************************************************************
    Func Name: IsValidPhyID
  Description: 判断是否是合法的PHYID
        Input: phyid_t uiPhyID  
       Output: 
       Return: TRUE / FALSE
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL IsValidPhyID(phyid_t uiPhyID)
{
    UINT32 uiPortId = PORTID(uiPhyID);
    UINT32 uiChipID = CHIPID(uiPhyID);

    if(uiChipID >= MAX_CHIP_NUM)
    {
        return FALSE;
    }

    if(uiPortId >= g_szChipMaxPort[uiChipID])
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*****************************************************************************
    Func Name: PortLogic2PhyID
  Description: 将逻辑端口号转换为phyid
        Input: port_num_t ucLogicPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
phyid_t PortLogic2PhyID(port_num_t ucLogicPort)
{
    if (!IsValidLgcPort(ucLogicPort)) {
        ASSERT(IsValidLgcPort(ucLogicPort));
        return INVALID_PORT;
    }
    return PORT_PHYID(ucLogicPort);
}

/*****************************************************************************
    Func Name: PortPhyID2Logic
  Description: 将phyid转换为逻辑端口号
        Input: port_num_t ucPhyId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
port_num_t PortPhyID2Logic(phyid_t uiPhyId)
{
    port_num_t lport = INVALID_PORT;
    UINT32 ulChipId = 0;
    UINT32 ulid = 0;
    UINT32 ulPortStart = 0;
    
    if (!IsValidPhyID(uiPhyId)) {
        ASSERT(IsValidPhyID(uiPhyId));
        return INVALID_PORT;
    }

    ulChipId = CHIPID(uiPhyId);
    for(ulid=0; ulid<ulChipId; ulid++)
    {
        ulPortStart += g_szChipMaxPort[ulid]; 
    }
    
    lport = TOLGCPORT(ulPortStart + PORTID(uiPhyId));
    if(STACK_PORT == lport)
    {
        return INVALID_PORT;       
    }
    else
    {
        return lport;
    }
}


/*****************************************************************************
    Func Name: SetPhyMaskBit
  Description: 根据phyid设置物理掩码的某一位为1
        Input: phyid_t uiPhyId            
                phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void SetPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr)
{
    if (!IsValidPhyID(uiPhyId)) {
        ASSERT(IsValidPhyID(uiPhyId));
        return;
    }
    if(NULL == phyPMask_ptr) {
        ASSERT(NULL != phyPMask_ptr);
        return;
    }

    RTK_PORTMASK_PORT_SET(phyPMask_ptr->pmask[CHIPID(uiPhyId)], PORTID(uiPhyId));

}

/*****************************************************************************
    Func Name: ClrPhyMaskBit
  Description: 根据phyid清除物理掩码的某一位
        Input: phyid_t uiPhyId            
                phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void ClrPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr)
{
    if (!IsValidPhyID(uiPhyId)) {
        ASSERT(IsValidPhyID(uiPhyId));
        return;
    }
    if(NULL == phyPMask_ptr) {
        ASSERT(NULL != phyPMask_ptr);
        return;
    }

    RTK_PORTMASK_PORT_CLEAR(phyPMask_ptr->pmask[CHIPID(uiPhyId)], PORTID(uiPhyId));
}


/*****************************************************************************
    Func Name: ClrPhyMaskAll
  Description: 清除物理掩码的所有位
        Input: phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void ClrPhyMaskAll(phy_pmask_t *phyPMask_ptr)
{
    UINT32 chipId = 0;
    
    if(NULL == phyPMask_ptr) {
        ASSERT(NULL != phyPMask_ptr);
        return;
    }

    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_RESET(CHIPNMASK(chipId, phyPMask_ptr)); 
    }
}

/*****************************************************************************
    Func Name: SetPhyMaskAll
  Description: 设置所有有效的物理端口进入mask
        Input: phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: mask中包含CPU端口
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void SetPhyMaskAll(phy_pmask_t *phyPMask_ptr)
{
    port_num_t pport = 0;
    UINT32 chipId = 0;
    phyid_t uiPhyId = 0;
    
    if(NULL == phyPMask_ptr) {
        ASSERT(NULL != phyPMask_ptr);
        return;
    }

    memset(phyPMask_ptr, 0x00, sizeof(phy_pmask_t));
    PhyPortFor(chipId, pport)
    {
        uiPhyId = TOPHYID(chipId, pport);
        SetPhyMaskBit(uiPhyId, phyPMask_ptr);
    }
    PhyMaskAddCpuPort(phyPMask_ptr);
}

/*****************************************************************************
    Func Name: TstPhyMaskBit
  Description: 检查物理掩码的某一位是否为1
        Input: phyid_t uiPhyId            
                phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL TstPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr)
{
    if (!IsValidPhyID(uiPhyId)) {
        ASSERT(IsValidPhyID(uiPhyId));
        return FALSE;
    }
    if(NULL == phyPMask_ptr) {
        ASSERT(NULL != phyPMask_ptr);
        return FALSE;
    }

    if(RTK_PORTMASK_IS_PORT_SET(phyPMask_ptr->pmask[CHIPID(uiPhyId)], PORTID(uiPhyId)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
    Func Name: MaskLogic2Phy
  Description: 将逻辑掩码转换为物理掩码
        Input: logic_pmask_t *lgcPMask_ptr  
                phy_pmask_t *phyPMask_ptr    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void MaskLogic2Phy(logic_pmask_t *lgcPMask_ptr, phy_pmask_t *phyPMask_ptr)
{
    port_num_t lPortNo;
    phyid_t uiPhyId;

    if((NULL == lgcPMask_ptr) || (NULL == phyPMask_ptr))
    {
        ASSERT(NULL != phyPMask_ptr);
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }
    
    ClrPhyMaskAll(phyPMask_ptr);
    
    LgcPortFor(lPortNo)
    {
        if(TstLgcMaskBit(lPortNo, lgcPMask_ptr))
        {
            uiPhyId = PortLogic2PhyID(lPortNo);
            if(uiPhyId == INVALID_PORT)
            {
                continue;
            }
            SetPhyMaskBit(uiPhyId ,phyPMask_ptr);
        }
    }
    
    /*begin modified by liaohongjun 2012/7/30 of QID0001*/
    #if defined(CONFIG_PRODUCT_EPN104)
    /*end modified by liaohongjun 2012/7/30 of QID0001*/
    if (TstLgcMaskBit(LOGIC_PON_PORT, lgcPMask_ptr))
    {
        uiPhyId = PortLogic2PhyID(LOGIC_PON_PORT);
        if(uiPhyId != INVALID_PORT)
        {
            SetPhyMaskBit(uiPhyId ,phyPMask_ptr);
        }
    }

    if (TstLgcMaskBit(LOGIC_CPU_PORT, lgcPMask_ptr))
    {
        uiPhyId = PortLogic2PhyID(LOGIC_CPU_PORT);
        if(uiPhyId != INVALID_PORT)
        {
            SetPhyMaskBit(uiPhyId ,phyPMask_ptr);
        }
    }
    #endif
}

/*****************************************************************************
    Func Name: MaskPhy2Logic
  Description: 将物理掩码转换为逻辑掩码
        Input: phy_pmask_t *phyPMask_ptr    
                logic_pmask_t *lgcPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void MaskPhy2Logic(phy_pmask_t *phyPMask_ptr, logic_pmask_t *lgcPMask_ptr)
{
    port_num_t lPortNo = 0;
    phyid_t uiPhyId = 0;

    if((NULL == lgcPMask_ptr) || (NULL == phyPMask_ptr))
    {
        ASSERT(NULL != phyPMask_ptr);
        ASSERT(NULL != lgcPMask_ptr);
        return;
    }
    
    ClrLgcMaskAll(lgcPMask_ptr);
    
    LgcPortFor(lPortNo)
    {
        uiPhyId = PortLogic2PhyID(lPortNo);
        if(uiPhyId == INVALID_PORT)
        {
            continue;
        }
        if(TstPhyMaskBit(uiPhyId, phyPMask_ptr))
        {
            SetLgcMaskBit(lPortNo, lgcPMask_ptr);
        }
    }

    /*begin modified by liaohongjun 2012/7/30 of QID0001*/
    #if defined(CONFIG_PRODUCT_EPN104)
    /*end modified by liaohongjun 2012/7/30 of QID0001*/
    uiPhyId = PortLogic2PhyID(LOGIC_PON_PORT);
    if(uiPhyId != INVALID_PORT)
    {
        if(TstPhyMaskBit(uiPhyId, phyPMask_ptr))
        {
            SetLgcMaskBit(LOGIC_PON_PORT, lgcPMask_ptr);
        }
    }
    
    uiPhyId = PortLogic2PhyID(LOGIC_CPU_PORT);
    if(uiPhyId != INVALID_PORT)
    {
        if(TstPhyMaskBit(uiPhyId, phyPMask_ptr))
        {
            SetLgcMaskBit(LOGIC_CPU_PORT, lgcPMask_ptr);
        }
    }
    #endif

}

#ifdef CHIPSET_RTL8305
UINT32 bitop_numberOfSetBitsInArray(UINT32 *pArray, UINT32 arraySize)
{
    UINT32  index;
    UINT32  value;
    UINT32  numOfBits;
    
    if (NULL == pArray)
    {/* if null pointer, return 0 bits */
        return 0;
    }
    
    numOfBits = 0;
    
    for (index = 0; index < arraySize; index++)
    {
        value = pArray[index];
        
        value = value - ((value >> 1) & 0x55555555);
        value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
        numOfBits =+ (((((value + (value >> 4)) & 0xF0F0F0F)) * 0x1010101) >> 24);
    }
    
    return numOfBits;
}
#endif

/*****************************************************************************
    Func Name: GetPhyMskPortNum
  Description: 获取物理端口mask中的端口数目
        Input: phy_pmask_t *phyPMask_ptr  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
INT32 GetPhyMskPortNum(phy_pmask_t *phyPMask_ptr)
{
    INT32 num = 0;
    UINT32 chipId = 0;
    
    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        num += (INT32)RTK_PORTMASK_GET_PORT_COUNT(CHIPNMASK(chipId, phyPMask_ptr));
    }
    
    return num;
}

/*****************************************************************************
    Func Name: PhyMaskCopy


  Description: 将srcMsk的值赋给dstMsk
        Input: phy_pmask_t *dstMsk  
                phy_pmask_t *srcMsk  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskCopy(phy_pmask_t *dstMsk,phy_pmask_t *srcMsk)
{
    if((NULL == dstMsk) || (NULL == srcMsk))
    {
        ASSERT(NULL != dstMsk);
        ASSERT(NULL != srcMsk);
        return;
    }
    memcpy(dstMsk,srcMsk,sizeof(phy_pmask_t));
}

/*****************************************************************************
    Func Name: PhyMaskAnd


  Description: 将mska和mskb做与操作
        Input: phy_pmask_t *mska  
                phy_pmask_t *mskb  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskAnd(phy_pmask_t *mska,phy_pmask_t *mskb)
{
    UINT32 chipId = 0;
    
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }

    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_AND(CHIPNMASK(chipId, mska), CHIPNMASK(chipId, mskb));
    }
}

/*****************************************************************************
    Func Name: PhyMaskOr


  Description: 将mska和mskb做或操作
        Input: phy_pmask_t *mska  
                phy_pmask_t *mskb  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskOr(phy_pmask_t *mska,phy_pmask_t *mskb)
{
    UINT32 chipId = 0;
    
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_OR(CHIPNMASK(chipId, mska), CHIPNMASK(chipId, mskb));
    }
}

/*****************************************************************************
    Func Name: PhyMaskXor


  Description: 将mska和mskb做XOR操作
        Input: phy_pmask_t *mska  
                phy_pmask_t *mskb  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskXor(phy_pmask_t *mska,phy_pmask_t *mskb)
{
    UINT32 chipId = 0;
    
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_XOR(CHIPNMASK(chipId, mska), CHIPNMASK(chipId, mskb));
    }
}

/*****************************************************************************
    Func Name: PhyMaskRemove


  Description: 将mska和mskb做REMOVE操作
        Input: phy_pmask_t *mska  
                phy_pmask_t *mskb  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskRemove(phy_pmask_t *mska,phy_pmask_t *mskb)
{
    UINT32 chipId = 0;
    
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }
    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_REMOVE(CHIPNMASK(chipId, mska), CHIPNMASK(chipId, mskb));
    }
}

/*****************************************************************************
    Func Name: PhyMaskNegate


  Description: 将mska和mskb做取反操作
        Input: phy_pmask_t *mska  
                phy_pmask_t *mskb  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskNegate(phy_pmask_t *mska,phy_pmask_t *mskb)
{
    UINT32 chipId = 0;
    
    if((NULL == mska) || (NULL == mskb))
    {
        ASSERT(NULL != mska);
        ASSERT(NULL != mskb);
        return;
    }

    for(chipId=0; chipId<CHIP_NO; chipId++)
    {
        RTK_PORTMASK_REVERT(CHIPNMASK(chipId, mskb));
    }
    PhyMaskCopy(mska, mskb);
}

/*****************************************************************************
    Func Name: PhyMaskAddCpuPort


  Description: 向物理掩码中添加CPU端口
        Input: phy_pmask_t *mska  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskAddCpuPort(phy_pmask_t *mska)
{
    phyid_t uiPhyId = 0;
    
    if(NULL == mska)
    {
        ASSERT(NULL != mska);
        return;
    }
    uiPhyId = PORT_PHYID(LOGIC_CPU_PORT);

    SetPhyMaskBit(uiPhyId, mska);
}

/*****************************************************************************
    Func Name: PhyMaskRemoveCpuPort


  Description: 从物理掩码中去掉CPU口
        Input: phy_pmask_t *mska  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskRemoveCpuPort(phy_pmask_t *mska)
{
    phyid_t uiPhyId = 0;
    
    if(NULL == mska)
    {
        ASSERT(NULL != mska);
        return;
    }
    uiPhyId = PORT_PHYID(LOGIC_CPU_PORT);

    ClrPhyMaskBit(uiPhyId, mska);
}

/*****************************************************************************
    Func Name: PhyGetStackPort


  Description: 获取堆叠口
        Input: UINT32 ulUnit        
                UINT32 ulStackIndex  
       Output: 
       Return: 
      Caution: ulStackIndex从1开始
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
phyid_t PhyGetStackPort(UINT32 ulUnit, UINT32 ulStackIndex)
{
    port_num_t pport = 0;
    UINT32 iMatchCnt = 0;
    UINT32 ulChipId = 0;
    UINT32 ulPortStart = 0;

    for(ulChipId=0; ulChipId<ulUnit; ulChipId++)
    {
        ulPortStart += g_szChipMaxPort[ulChipId]; 
    }

    for(pport=0; pport<g_szChipMaxPort[ulUnit]; pport++)
    {
        if(STACK_PORT == TOLGCPORT(ulPortStart+pport))
        {
            iMatchCnt++;
            if(ulStackIndex == iMatchCnt)
            {
                return TOPHYID(ulUnit, pport);
            }
        }
    }    
    
    return INVALID_PORT;
}

/*****************************************************************************
    Func Name: IsStackPort


  Description: 检查是否为堆叠口
        Input: phyid_t uiPhyId
               
       Output: 
       Return: TRUE
               FALSE
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL IsStackPort(phyid_t uiPhyId)
{
    if (!IsValidPhyID(uiPhyId)) {
        ASSERT(IsValidPhyID(uiPhyId));
        return FALSE;
    }

    if(STACK_PORT == PortPhyID2Logic(uiPhyId))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}


/*****************************************************************************
    Func Name: PhyMaskAddStackPort


  Description: 向掩码中添加堆叠口
        Input: phy_pmask_t *mska  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void PhyMaskAddStackPort(phy_pmask_t *mska)
{
    UINT32 chipId = 0;
    UINT32 ulChipId = 0;
    UINT32 ulPortStart = 0;
    port_num_t pport = 0;
    port_num_t lport = 0;

    if(NULL == mska)
    {
        ASSERT(NULL != mska);
        return;
    }

    PhyPortFor(chipId, pport)
    {
        ulPortStart = 0;
        for(ulChipId=0; ulChipId<chipId; ulChipId++)
        {
            ulPortStart += g_szChipMaxPort[ulChipId]; 
        }
        lport = TOLGCPORT(ulPortStart + pport);
        if(STACK_PORT == lport)
        {
            SetPhyMaskBit(TOPHYID(chipId, pport), mska);
        }
    }
}

/*****************************************************************************
    Func Name: ErrorSdk2Drv


  Description: 将SDK中的错误转换为DRV层的错误
        Input: INT32 iRtkError  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E ErrorSdk2Drv(INT32 iRtkError)
{
    switch (iRtkError)
    {
        case RT_ERR_FAILED:
            return DRV_SDK_GEN_ERROR;
        case RT_ERR_OK:
            return DRV_OK;
        case RT_ERR_INPUT:
            return DRV_SDK_INVALID_PARA;
        case RT_ERR_UNIT_ID:
            return DRV_SDK_INVALID_UNIT;
        case RT_ERR_PORT_ID:
            return DRV_SDK_INVALID_PORTID;
        case RT_ERR_PORT_MASK:
            return DRV_SDK_INVALID_MASK;
        case RT_ERR_PORT_LINKDOWN:
            return DRV_SDK_PORT_LINKDOWN;
        case RT_ERR_ENTRY_INDEX:
            return DRV_SDK_INVALID_ENTRY_INDEX;
        case RT_ERR_NULL_POINTER:
            return DRV_SDK_NULL_POINTER;
        case RT_ERR_QUEUE_ID:
            return DRV_SDK_INVALID_QUEUE_ID;
        case RT_ERR_QUEUE_NUM:
            return DRV_SDK_INVALID_QUEUE_NUM;
        case RT_ERR_BUSYWAIT_TIMEOUT:
            return DRV_SDK_BUSYWAIT_TIMEOUT;
        case RT_ERR_MAC:
            return DRV_SDK_INVALID_MAC;
        case RT_ERR_OUT_OF_RANGE:
            return DRV_SDK_OUT_OF_RANGE;
        case RT_ERR_CHIP_NOT_SUPPORTED:
            return DRV_SDK_CHIP_NOT_SUPPORTED;
        case RT_ERR_SMI:
            return DRV_SDK_INVALID_SMI;
        case RT_ERR_NOT_INIT:
            return DRV_SDK_NOT_INIT;
        case RT_ERR_CHIP_NOT_FOUND:
            return DRV_SDK_CHIP_NOT_FOUND;
        case RT_ERR_NOT_ALLOWED:
            return DRV_SDK_NOT_ALLOWED;
        case RT_ERR_DRIVER_NOT_FOUND:
            return DRV_SDK_DRIVER_NOT_FOUND;
        case RT_ERR_SEM_LOCK_FAILED:
            return DRV_SDK_SEM_LOCK_FAILED;
        case RT_ERR_SEM_UNLOCK_FAILED:
            return DRV_SDK_SEM_UNLOCK_FAILED;
#ifdef CHIPSET_RTL8328
        case RT_ERR_THREAD_EXIST:
            return DRV_SDK_THREAD_EXIST;
        case RT_ERR_THREAD_CREATE_FAILED:
            return DRV_SDK_THREAD_CREATE_FAILED;
        case RT_ERR_FWD_ACTION:
            return DRV_SDK_INVALID_FWD_ACTION;
        case RT_ERR_IPV4_ADDRESS:
            return DRV_SDK_INVALID_IPV4_ADDRESS;
        case RT_ERR_IPV6_ADDRESS:
            return DRV_SDK_INVALID_IPV6_ADDRESS;
        case RT_ERR_PRIORITY:
            return DRV_SDK_INVALID_PRIORITY;
        case RT_ERR_FID:
            return DRV_SDK_INVALID_FID;
        case RT_ERR_ENTRY_NOTFOUND:
            return DRV_SDK_ENTRY_NOTFOUND;
        case RT_ERR_DROP_PRECEDENCE:
            return DRV_SDK_DROP_PRECEDENCE;
        case RT_ERR_NOT_FINISH:
            return DRV_SDK_NOT_FINISH;
        case RT_ERR_TIMEOUT:
            return DRV_SDK_TIMEOUT;
        case RT_ERR_REG_ARRAY_INDEX_1:
            return DRV_SDK_INVALID_REG_ARRAY_INDEX_1;
        case RT_ERR_REG_ARRAY_INDEX_2:
            return DRV_SDK_INVALID_REG_ARRAY_INDEX_2;
        case RT_ERR_ETHER_TYPE:
            return DRV_SDK_INVALID_ETHER_TYPE;
        case RT_ERR_PHY_FIBER_LINKUP:
            return DRV_SDK_ERR_PHY_FIBER_LINKUP;
#endif
        default:
            ASSERT(iRtkError == RT_ERR_OK);
            return DRV_SDK_GEN_ERROR;
    }
}


#ifdef  __cplusplus
}
#endif


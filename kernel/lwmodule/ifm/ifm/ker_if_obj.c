/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
//#include <mw_dbg.h>
#include <lw_if_pub.h>
#include <lw_if_type.h>
#include <lw_if_obj.h>
#include <linux/slab.h>
#include <linux/string.h>
#if  defined(UT_TEST) || defined(CONFIG_GCOV_ALL)
#include <kmalloc_ut_stub.h>
#endif
#ifdef UT_TEST_X86
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "malloc_ut_stub.h"
#include <mw_dbg.h>
#define kmalloc(a,b) malloc(a)
#define kfree      free
#define printk     printf
#endif
#include <lw_config.h>

typedef UINT Pool_Word_t;
#define POOL_WORD_WIDTH  (8 * sizeof(Pool_Word_t))
#define IF_INDEX_NUM    (IF_INDEX_MASK+1)

#define IF_MAX_COUNT(_type, _domon)  g_apIfTable[(UINT32)FixCounterType((_type))][(_domon)].uiMax
#define IF_COUNT(_type, _domon)      g_apIfTable[(UINT32)FixCounterType((_type))][(_domon)].uiCount

STATIC IF_REGISTER_BLOCK * g_apIfReg[IF_SUB_TYPE_COUNT];  
STATIC Pool_Word_t g_szIfObjBitPool[IF_SUB_TYPE_COUNT][IF_DOMAIN_ALL][(IF_INDEX_NUM+POOL_WORD_WIDTH-1)/POOL_WORD_WIDTH];
STATIC IF_HEAD_S  g_apIfTable[IF_SUB_TYPE_COUNT][IF_DOMAIN_ALL];


STATIC IF_SUB_TYPE_E FixCounterType(IF_SUB_TYPE_E eSubType);


/*****************************************************************************
    Func Name: GetIfPriAttr
                                                                            
*****************************************************************************/
IF_RET_E GetIfPriAttr(ifindex_t uiIfindex, IF_ATTR_E iAttrId, VOID * pValue ,UINT iLen)
{ 
     IF_SUB_TYPE_E uiSubType ;
    
     uiSubType =(IF_SUB_TYPE_E) IF_SUB_TYPE(uiIfindex);
     if(!VALID_SUB_TYPE(uiSubType))
     {
        return IF_INVALID_IDX;
     }
     if((NULL == g_apIfReg[uiSubType]) || (NULL == pValue))
     {
         return IF_ERR_PARAM;
     }
     if(NULL == g_apIfReg[uiSubType]->attribute)
     {
        return IF_NFUND_REG_FUNC;
     }
     
     return g_apIfReg[uiSubType]->attribute(uiIfindex,iAttrId,pValue,iLen);
     
}

/*****************************************************************************
    Func Name: IF_SetPriAttr
                                                                            
*****************************************************************************/
IF_RET_E SetPriAttr(IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue ,IN UINT uiLen)
{
     IF_SUB_TYPE_E uiSubType ;
     uiSubType =(IF_SUB_TYPE_E) IF_SUB_TYPE(uiIfindex);
     if(uiSubType >=IF_SUB_PHY_ALL)
     {
        return IF_ERR_PARAM;
     }
     if((NULL == g_apIfReg[uiSubType]) || (NULL == pValue))
     {
         return IF_ERR_PARAM;
     }
     if(NULL == g_apIfReg[uiSubType]->setattr)
     {
        return IF_ERR_PARAM;
     }
     return g_apIfReg[uiSubType]->setattr(uiIfindex,iAttrId,pValue,uiLen);
}
/*****************************************************************************
    Func Name: GetFirstIfindex
*****************************************************************************/
IF_RET_E GetFirstIfindexbyDomain( IN IF_SUB_TYPE_E enSubType, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfindex )
{
    IF_OBJ_S* pstTemp =NULL;
    INT i = 0;
    if(IF_SUB_PHY_ALL == enSubType)
    {  
        for(pstTemp = g_apIfTable[i][enDomain].pstNext;i < IF_OBJ_SUB_TYPE_MAX;++i,pstTemp = g_apIfTable[i][enDomain].pstNext)
        {
            if(NULL == pstTemp)
            {
                continue;  
            }
            *puiIfindex = pstTemp->uiIfIndex;
            return IF_OK;
        }
        return IF_NFOUND;
    }  
    else
    {
        pstTemp = g_apIfTable[enSubType][enDomain].pstNext;
        if(NULL == pstTemp)
        {
            return IF_NFOUND;
        }
        *puiIfindex = pstTemp->uiIfIndex;
        return IF_OK;
    }
}


/*****************************************************************************
    Func Name: K_IF_Register
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
VOID K_IF_Register(UINT uiSubType,IF_REGISTER_BLOCK *pstIfRegBlock)
{    
    if((VALID_SUB_TYPE(uiSubType))&&(NULL != pstIfRegBlock))
    {
        g_apIfReg[uiSubType] = pstIfRegBlock;      
    }
}

/*****************************************************************************
    Func Name: K_IF_UnRegister
*****************************************************************************/
VOID K_IF_UnRegister(UINT uiSubType)
{
    if(VALID_SUB_TYPE(uiSubType))
    {
        g_apIfReg[uiSubType] = NULL;
    }
}
/*****************************************************************************
    Func Name: InitObjBitPool
                                                                            
*****************************************************************************/
VOID InitObjBitPool(void)
{
    memset(g_szIfObjBitPool,0,sizeof(g_szIfObjBitPool));
}
/*****************************************************************************
    Func Name: InitIFRegisterTbl()
                                                                            
*****************************************************************************/
VOID InitIFRegisterTbl (VOID)
{
    memset(&g_apIfReg[0],0,sizeof(g_apIfReg));    
}
/*****************************************************************************
    Func Name: Init_if_data(IF_SUB_TYPE_E enType,UINT uiMax)
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
STATIC IF_RET_E Init_if_data(IF_SUB_TYPE_E enType,UINT uiMax)
{
    UINT enDomain;
   
    for(enDomain=0;enDomain<IF_DOMAIN_ALL;enDomain++)
    {
        IF_MAX_COUNT(enType, enDomain) = uiMax;
    }    
    return IF_OK;
}
/*****************************************************************************
    Func Name: InitIFTbl()
*****************************************************************************/
VOID InitIFTbl(VOID)
{
    memset(&g_apIfTable[0][0],0,sizeof(g_apIfTable));  
    Init_if_data(IF_SUB_ETH,IF_ETH_MAX);
    Init_if_data(IF_SUB_L3VLAN,IF_L3VLAN_MAX);
    Init_if_data(IF_SUB_ROOT,IF_ROOT_MAX);
//    Init_if_data(IF_SUB_VLANRANGE1,IF_VLAN_MAX);
    Init_if_data(IF_SUB_ETHPORT,IF_ETHPORT_MAX);
  //  Init_if_data(IF_SUB_AGGR,IF_AGGRGROUP_MAX);
}
/*****************************************************************************
    Func Name: AllocFromObjBitPool
                                                                            
*****************************************************************************/
STATIC INT AllocFromObjBitPool(ifindex_t uiIfindex)
{
    UINT uiSubType = 0;
    UINT uiIndex = 0;
    Pool_Word_t setBit = 0;
    IF_DOMAIN_E enDomain;

    uiSubType = IF_SUB_TYPE(uiIfindex);
    
    uiIndex = IF_INDEX(uiIfindex);
    enDomain = IF_DOMAIN(uiIfindex);
    setBit = (Pool_Word_t)(0x1<<(uiIndex%POOL_WORD_WIDTH));
    if(0 == (g_szIfObjBitPool[uiSubType][enDomain][uiIndex/POOL_WORD_WIDTH] & setBit))
    {
        g_szIfObjBitPool[uiSubType][enDomain][uiIndex/POOL_WORD_WIDTH] |= setBit;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
    Func Name: FreeToObjBitPool
                                                                            
*****************************************************************************/
STATIC INT FreeToObjBitPool(ifindex_t uiIfindex)
{
    UINT uiSubType = 0;
    UINT uiIndex = 0;
    Pool_Word_t setBit = 0;
    IF_DOMAIN_E enDomain;

    uiSubType = IF_SUB_TYPE(uiIfindex);

    uiIndex = IF_INDEX(uiIfindex);
    enDomain =IF_DOMAIN(uiIfindex);

    setBit = (Pool_Word_t)(0x1<<(uiIndex%POOL_WORD_WIDTH));
    if(g_szIfObjBitPool[uiSubType][enDomain][uiIndex/POOL_WORD_WIDTH] & setBit)
    {
        g_szIfObjBitPool[uiSubType][enDomain][uiIndex/POOL_WORD_WIDTH] &= ~setBit;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
    Func Name: TestObjBitPookl
*****************************************************************************/
STATIC INT TestObjBitPool(ifindex_t uiIfindex)
{
    UINT uiSubType = 0;
    UINT uiIndex = 0;
    IF_DOMAIN_E enDomain;

    uiSubType = IF_SUB_TYPE(uiIfindex);

    uiIndex = IF_INDEX(uiIfindex);
    enDomain =IF_DOMAIN(uiIfindex);
    if(0 == (g_szIfObjBitPool[uiSubType][enDomain][uiIndex/POOL_WORD_WIDTH] & (Pool_Word_t)(0x1<<(uiIndex%POOL_WORD_WIDTH))))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*****************************************************************************
    Func Name: AllocIfObj
                                                                            
*****************************************************************************/
IF_OBJ_S * AllocIfObj(VOID)
{
    IF_OBJ_S * psBuf=NULL;
    
    psBuf = (IF_OBJ_S*)kmalloc(sizeof(IF_OBJ_S),GFP_KERNEL);
    if(NULL == psBuf)
    {
        return NULL;
    }
    memset(psBuf,0,sizeof(IF_OBJ_S));
    return  psBuf;
}

/*****************************************************************************
    Func Name: FreeIfObj
*****************************************************************************/
VOID FreeIfObj(IF_OBJ_S * pstObj)
{   
    if(NULL != pstObj)
    {
        kfree(pstObj);
    }
}

/*****************************************************************************
    Func Name: AddObjToList
*****************************************************************************/
IF_RET_E AddObjToList(IF_OBJ_S * pstObj)
{
    IF_SUB_TYPE_E uiSubtype = IF_SUB_ROOT;
    IF_OBJ_S * pstPre = NULL;
    IF_OBJ_S * pstSearch = NULL;
    IF_DOMAIN_E enDomain;
    uiSubtype = IF_SUB_TYPE(pstObj->uiIfIndex) ;
    enDomain = IF_DOMAIN(pstObj->uiIfIndex);

    if(IF_COUNT(uiSubtype, enDomain) >= IF_MAX_COUNT(uiSubtype, enDomain))
    {
        return IF_ALREADY_MAX_NUM;
    }
    
    if(FALSE == AllocFromObjBitPool(pstObj->uiIfIndex))
    {
        IFM_DBG_ERR("%s fail\n",__FUNCTION__);
        return IF_ALREADY_EXIST;
    }

    if(NULL == g_apIfTable[uiSubtype][enDomain].pstNext)
    {
         g_apIfTable[uiSubtype][enDomain].pstNext = pstObj;
         IF_COUNT(uiSubtype, enDomain)++;
         return IF_OK;
    }

    for(pstSearch=g_apIfTable[uiSubtype][enDomain].pstNext; \
        pstSearch != NULL; \
        pstPre=pstSearch,pstSearch=pstSearch->pstNext)
    {   
        if(IF_INDEX(pstSearch->uiIfIndex) > IF_INDEX(pstObj->uiIfIndex))
        {
            if(pstPre==NULL)
            {
                g_apIfTable[uiSubtype][enDomain].pstNext=pstObj;
            }
            else
            {
                pstPre->pstNext = pstObj;
            }
            pstObj->pstNext = pstSearch;     
            IF_COUNT(uiSubtype, enDomain)++;
            return  IF_OK;
        }
        else if(IF_INDEX(pstSearch->uiIfIndex) == IF_INDEX(pstObj->uiIfIndex))
        {
            IFM_DBG_ERR("%s, interface exist\n",__FUNCTION__);
            return  IF_ERR_PANIC;
        }
    }

    if(pstPre)
    {
        pstPre->pstNext = pstObj;
        IF_COUNT(uiSubtype, enDomain)++;
        return  IF_OK;
    }
    else
    {
        return  IF_ERR_PANIC;
    }
}

/*****************************************************************************
    Func Name: RemoveObjFromList
                                                                            
*****************************************************************************/
IF_RET_E RemoveObjFromList(IF_OBJ_S * pstObj)
{
    IF_OBJ_S *  pstPre = NULL;
    IF_OBJ_S * pstDel = NULL;
    IF_SUB_TYPE_E uiSubtype = IF_SUB_ROOT;
    IF_DOMAIN_E enDomain;
    
    uiSubtype = IF_SUB_TYPE(pstObj->uiIfIndex);
    enDomain = IF_DOMAIN(pstObj->uiIfIndex);
    if(FALSE == FreeToObjBitPool(pstObj->uiIfIndex))
    {
        return IF_NFOUND;
    }
    
    for(pstPre=NULL,pstDel=g_apIfTable[uiSubtype][enDomain].pstNext; \
        NULL != pstDel; \
        pstPre=pstDel,pstDel=pstDel->pstNext)
    {
        if(IF_INDEX(pstDel->uiIfIndex) == IF_INDEX(pstObj->uiIfIndex))
        {
            if(pstPre==NULL)
            {
                g_apIfTable[uiSubtype][enDomain].pstNext=pstDel->pstNext;
            }
            else
            {
                pstPre->pstNext = pstDel->pstNext;
            }
            IF_COUNT(uiSubtype, enDomain)--;
            return IF_OK;
        }
        else if(IF_INDEX(pstObj->uiIfIndex) < IF_INDEX(pstDel->uiIfIndex))
        {
            return IF_ERR_PANIC;
        }
    }
    return IF_ERR_PANIC;
}

/*****************************************************************************
    Func Name: GetObjByIfindex
                                                                            
*****************************************************************************/
IF_OBJ_S * GetObjByIfindex(ifindex_t uiIfindex)
{
    UINT uiSubtype = 0;
    IF_OBJ_S *  pstGet = NULL;
    IF_DOMAIN_E enDomain;
    
    uiSubtype = IF_SUB_TYPE(uiIfindex);
    enDomain = IF_DOMAIN(uiIfindex);
    for(pstGet=g_apIfTable[uiSubtype][enDomain].pstNext; NULL!=pstGet; pstGet=pstGet->pstNext)
    {
        if(IF_INDEX(pstGet->uiIfIndex) == IF_INDEX(uiIfindex))
        {            
            return pstGet;
        }
    }
    return NULL;
}

/*****************************************************************************
    Func Name: GetObjByPseudoName
                                                                            
*****************************************************************************/
IF_OBJ_S * GetObjByPseudoNameByDomain(CHAR * pcPseudo_name,IF_DOMAIN_E enDomain)
{
    UINT uiSubtype = 0;
    IF_OBJ_S *  pstGet = NULL;
    
    if( NULL == pcPseudo_name)
    {
        return NULL;
    }
    for(uiSubtype=0; uiSubtype<IF_OBJ_SUB_TYPE_MAX; uiSubtype++)
    {        
        for(pstGet=g_apIfTable[uiSubtype][enDomain].pstNext; NULL!=pstGet; pstGet=pstGet->pstNext)
        {
            if(0 == strcmp((CONST CHAR *)pstGet->ucPseudoName,(CONST CHAR *)pcPseudo_name))
            {            
                return pstGet;
            }
        }   
    }
    return NULL;
}
/*****************************************************************************
    Func Name: GetObjByIfName
                                                                            
*****************************************************************************/
IF_OBJ_S * GetObjByIfNameBYDomain(CHAR * pcIf_name,IF_DOMAIN_E enDomain)
{
    UINT uiSubtype = 0;
    IF_OBJ_S *  pstGet = NULL;
    
    if( NULL == pcIf_name)
    {
        return NULL;
    }
    for(uiSubtype=0; uiSubtype<IF_OBJ_SUB_TYPE_MAX; uiSubtype++)
    {        
        for(pstGet=g_apIfTable[uiSubtype][enDomain].pstNext; NULL!=pstGet; pstGet=pstGet->pstNext)
        {
            if(0 == strcmp((CONST CHAR *)pstGet->ucIfName,(CONST CHAR *)pcIf_name))
            {            
                return pstGet;
            }
        }   
    }
    return NULL;
}
/*****************************************************************************
    Func Name: GetNextObj
                                                                            
*****************************************************************************/
IF_OBJ_S * GetNextObj(IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t uiIfindex)
{
    UINT uiSubtype = 0;
    IF_OBJ_S *  pstGet = NULL;
    IF_DOMAIN_E enDomain;
    
    if ( IF_INVALID_IFINDEX == uiIfindex || enSubType > IF_SUB_PHY_ALL)
    {
        return NULL ;
    }
    enDomain=IF_DOMAIN(uiIfindex);
    if( IF_SUB_PHY_ALL != enSubType)
    {        
        for(pstGet=g_apIfTable[enSubType][enDomain].pstNext; NULL!=pstGet; pstGet=pstGet->pstNext)
        {
            if(pstGet->uiIfIndex == uiIfindex)
            {   
                return pstGet->pstNext;
            }
        }   
    }
    else /*enSubType == IF_SUB_PHY_ALL*/
    {
        uiSubtype = IF_SUB_TYPE(uiIfindex);
        if(!VALID_SUB_TYPE(uiSubtype))
        {
            return NULL ;
        }       
       
        for(pstGet=g_apIfTable[uiSubtype][enDomain].pstNext; NULL!=pstGet; pstGet=pstGet->pstNext)
        {
            if((pstGet->uiIfIndex == uiIfindex)&&(pstGet->pstNext))
            {            
                return pstGet->pstNext;
            }
        }   
        uiSubtype++;
        while(uiSubtype<IF_SUB_PHY_ALL)
        {
            if( g_apIfTable[uiSubtype][enDomain].pstNext)
            {
                pstGet=g_apIfTable[uiSubtype][enDomain].pstNext; 
                return pstGet;
            }
            uiSubtype++;
        }
    }  
    return pstGet ;
}


/*****************************************************************************
    Func Name: TestIfObjExist
                                                                            
*****************************************************************************/
INT TestIfObjExist(ifindex_t uiIfindex)
{
    if(FALSE == TestObjBitPool(uiIfindex))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*****************************************************************************
    Func Name: GetFreeIfindex
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
ifindex_t GetFreeIfindex(IF_SUB_TYPE_E enSubType,IF_DOMAIN_E enDomain)
{
    ifindex_t uiIfindex = 0;
    UINT uiIndex = 0;

    for(uiIndex=0; uiIndex<IF_INDEX_NUM; uiIndex++)
    {
        uiIfindex = IF_DOMAIN_IFINDEX(enSubType,enDomain, uiIndex);
        if(FALSE == TestObjBitPool(uiIfindex))
        {
            return uiIfindex;
        }
    }
    
    return IF_INVALID_IFINDEX;
}

/*****************************************************************************
    Func Name: FixCounterType
                                                                            
*****************************************************************************/
STATIC IF_SUB_TYPE_E FixCounterType(IF_SUB_TYPE_E eSubType)
{
    switch (eSubType)   
    {
        default:
            return eSubType;
    }
}

/*****************************************************************************
    Func Name: InitIfData()
                                                                            
*****************************************************************************/
IF_RET_E InitIfData(ifindex_t ifindex,VOID *pData)
{    
    if((NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)])&& \
    (NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)]->init))       
    {  
        if(NULL==pData)
        {
            return IF_ERR_PARAM;
        }
        return g_apIfReg[IF_SUB_TYPE(ifindex)]->init(ifindex,pData);
    }
    return IF_OK;
}
/*****************************************************************************
    Func Name: DestroyIfData()
                                                                            
*****************************************************************************/
IF_RET_E DestroyIfData(ifindex_t ifindex)
{
    if((NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)])&& \
    (NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)]->destroy)) 
    {
        return g_apIfReg[IF_SUB_TYPE(ifindex)]->destroy(ifindex);
    }
    return IF_OK;
}

/*****************************************************************************
    Func Name: MallocIfData()
                                                                            
*****************************************************************************/

VOID * MallocIfData(IF_SUB_TYPE_E enSubType)
{
     if((NULL!=g_apIfReg[enSubType])&& \
        (NULL!=g_apIfReg[enSubType]->alloc))
     {        
        return g_apIfReg[enSubType]->alloc(enSubType);
     }
    
    return NULL;
}
/*****************************************************************************
    Func Name: FreeIfData()
                                                                            
*****************************************************************************/

VOID FreeIfData(ifindex_t ifindex,CONST VOID * pData)
{
    if((NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)])&& \
        (NULL!=g_apIfReg[IF_SUB_TYPE(ifindex)]->free)&&(NULL!=pData))
    {        
        g_apIfReg[IF_SUB_TYPE(ifindex)]->free(pData);
    }    
}

/*****************************************************************************
    Func Name: SetIfPseudoName
                                                                            
*****************************************************************************/
VOID SetIfPseudoName(IF_OBJ_S *pstIfObj, CHAR * pcPseudo_name)
{
    if(NULL == pstIfObj || NULL == pcPseudo_name)
    {
        return ;
    }    
    memcpy(pstIfObj->ucPseudoName ,pcPseudo_name,IFNAMESIZE); 
    return;
}

/*****************************************************************************
    Func Name: SetIfName
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
VOID SetIfName(IF_OBJ_S *pstIfObj, CHAR * pcIf_name)
{
    if(NULL == pstIfObj || NULL == pcIf_name)
    {
        return ;
    }    
    memcpy(pstIfObj->ucIfName ,pcIf_name,IFNAMESIZE); 
    return;
}

/*****************************************************************************
    Func Name: SetIfPhysicalId
                                                                            
*****************************************************************************/
VOID SetIfPhysicalId(IF_OBJ_S *pstIfObj, INT iId)
{   
    SetPriAttr(pstIfObj->uiIfIndex, IF_PHYID, &iId, sizeof(iId));
    return;
}

#ifdef  __cplusplus
}
#endif


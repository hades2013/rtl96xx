/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include <lw_if_pub.h>
#include <lw_if_ethport.h>
#include <linux/slab.h>
#include <lw_if_obj.h>
DEFINE_IF_REGISTER(stETHPort);

/*****************************************************************************
    Func Name: IF_EthportInit
                                                                            
*****************************************************************************/
IF_RET_E IF_EthportInit(IN ifindex_t uiIfindex,INOUT VOID *pvPara)
{
    UCHAR ucTmp[MACLEN]={"00:00:00:00:00:00"};  
    
    if(NULL == pvPara) 
    {
        return IF_ERR_PARAM;
    }
    if( IF_SUB_ETHPORT != IF_SUB_TYPE(uiIfindex))
    {
        return IF_ERR_TYPE;
    }
    ((ETHPORT_S *)pvPara)->enState = ETHPORT_DOWN;
    ((ETHPORT_S *)pvPara)->enDuplex = DUPLEX_AUTO;
    ((ETHPORT_S *)pvPara)->enSpeed = SPEED_AUTO;
    ((ETHPORT_S *)pvPara)->uiIsFiber = PORT_TYPE_IS_COPPER;
    ((ETHPORT_S *)pvPara)->uiPortid =(UINT)IF_LOGIF_PHYID;
    memcpy(((ETHPORT_S *)pvPara)->ucMac,ucTmp,MACLEN); 
    ((ETHPORT_S *)pvPara)->enFiberInfoFlag = FIBER_INFO_INVALID;
    ((ETHPORT_S *)pvPara)->pucFiberInfo = NULL;
    //((ETHPORT_S *)pvPara)->usPortStatus = PORT_EXIST;
    ((ETHPORT_S *)pvPara)->ulBelongToPA = BOOL_FALSE;
    ((ETHPORT_S *)pvPara)->ulMasterPort = BOOL_FALSE;
    ((ETHPORT_S *)pvPara)->ulAggGroupID = 0;
    ((ETHPORT_S *)pvPara)->ulUseResource = 0xffffffff;
    ((ETHPORT_S *)pvPara)->ulBlockStatus = 0;
    ((ETHPORT_S *)pvPara)->ulPortModeInAgg = 0;
    ((ETHPORT_S *)pvPara)->ulPAmode = BOOL_FALSE;
    ((ETHPORT_S *)pvPara)->usLinkStatus = PORT_LINK_DOWN;
    ((ETHPORT_S *)pvPara)->usDuplexSet = PORT_DUPLEX_AUTO;
    ((ETHPORT_S *)pvPara)->usActualDuplex = PORT_DUPLEX_FULL; 
   // ((ETHPORT_S *)pvPara)->ulStpState = 0xffffffff;
   //// ((ETHPORT_S *)pvPara)->ulLacpState = MSTP_STATE_FORWARDING;
   // ((ETHPORT_S *)pvPara)->ulDot1xState = MSTP_STATE_FORWARDING;
    ((ETHPORT_S *)pvPara)->usSpeedSet = PORT_SPEED_AUTO; 
    ((ETHPORT_S *)pvPara)->usActualSpeed = INF_PORT_SPEED_100M;
    ((ETHPORT_S *)pvPara)->ulPortDefaultMdi = 0;
    ((ETHPORT_S *)pvPara)->ulPortDefaultDuplex = PORT_DUPLEX_AUTO;
    ((ETHPORT_S *)pvPara)->ulPortDefaultSpeed = INF_PORT_SPEED_100M;
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_EthportAlloc
                                                                            
*****************************************************************************/

VOID * IF_EthportAlloc(IF_SUB_TYPE_E enIftype)
{     
    if( IF_SUB_ETHPORT != enIftype)
    {
        return NULL;
    }   
    return kmalloc(sizeof(ETHPORT_S),GFP_KERNEL);    
}
/*****************************************************************************
    Func Name: IF_EthportFree
                                                                            
*****************************************************************************/

VOID  IF_EthportFree(CONST VOID * pData)
{    
    if( NULL == pData)
    {
        return ;
    }   

    if(NULL != ((ETHPORT_S*)pData)->pucFiberInfo)
    {
        kfree(((ETHPORT_S*)pData)->pucFiberInfo);
    }
    
    kfree(pData);    
}
/*****************************************************************************
    Func Name: IF_EthportGetAttr()
                                                                            
*****************************************************************************/

IF_RET_E  IF_EthportGetAttr(ifindex_t uiIfindex,INT iAttrId, VOID * pValue ,UINT iLen)
{
    IF_OBJ_S * pstObj=NULL;
    
    if(iAttrId >= IF_ATTR_ALL || NULL == pValue)
    {
        return IF_ERR_PARAM;
    }
    if(IF_SUB_TYPE(uiIfindex)!=IF_SUB_ETHPORT)
    {
        return IF_ERR_TYPE;
    }
    pstObj = GetObjByIfindex(uiIfindex);
    if(NULL == pstObj)
    {
        return IF_NFOUND;
    }
    if(pstObj->pstData == NULL)
    {
        return IF_INVALID_DATA;
    }
    switch(iAttrId)
    {
        case IF_PHYID:
        {
            if(iLen<sizeof((((ETHPORT_S*)(pstObj->pstData))->uiPortid)))
            {
                return IF_ERR_PARAM; 
            }
            *(UINT *)pValue=((ETHPORT_S*)(pstObj->pstData))->uiPortid;
            break;
        }
        case IF_DUPLEX:
        {
            if(iLen<sizeof(DUPLEX_E))
            {
                return IF_ERR_PARAM; 
            }
            *(DUPLEX_E*)pValue=((ETHPORT_S*)(pstObj->pstData))->enDuplex;
            break;
        }
        case IF_STATE:
        {
            if(iLen<sizeof(PORTSTATE_E))
            {
                return IF_ERR_PARAM; 
            }
            *(PORTSTATE_E*)pValue=((ETHPORT_S*)(pstObj->pstData))->enState;
            break;
        }
        case IF_SPEED:
        {
            if(iLen<sizeof(SPEED_E))
            {
                return IF_ERR_PARAM; 
            }
            *(SPEED_E*)pValue=((ETHPORT_S*)(pstObj->pstData))->enSpeed;
            break;
        }
        case IF_TYPE:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM; 
            }
            *(UINT*)pValue=((ETHPORT_S*)(pstObj->pstData))->uiIsFiber;
            break;
        }
        case IF_MAC:
        {
            if(iLen<MACLEN)
            {
                return IF_ERR_PARAM; 
            }
            strcpy((CHAR *)pValue,(CONST CHAR *)(((ETHPORT_S*)(pstObj->pstData))->ucMac));
            break;
        }
        case IF_FIBER_INFO_FLAG:
        {
            if(iLen<sizeof(FIBERINFOFLAG_E))
            {
                return IF_ERR_FIBER_INFO_FLAG;
            }
            *((FIBERINFOFLAG_E*)pValue) = ((ETHPORT_S*)(pstObj->pstData))->enFiberInfoFlag;
            break;
        }
        case IF_FIBER_INFO:
        {
            if(iLen > MAX_FIBER_INFO_LEN)
            {
                return IF_ERR_FIBER_INFO;
            }
            if(NULL == ((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo)
            {
                return IF_ERR_FIBER_INFO;
            }
            memcpy((CHAR *)pValue, (CONST CHAR*)(((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo), iLen);
            break;
        }
        default:
        {
            return IF_ERR_ATTRID;
        }
    }
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_EthportSetAttr()
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

IF_RET_E IF_EthportSetAttr(ifindex_t uiIfindex, INT iAttrId,VOID * pValue,UINT uiLen )
{
    IF_OBJ_S * pstObj=NULL;
    UCHAR    * pucTemp = NULL;
    if(iAttrId >= IF_ATTR_ALL || NULL == pValue)
    {
        return IF_ERR_PARAM;
    }
    if(IF_SUB_TYPE(uiIfindex)!=IF_SUB_ETHPORT)
    {
        return IF_ERR_PARAM;
    }
    pstObj=GetObjByIfindex( uiIfindex);
    if(NULL == pstObj)
    {
        return IF_ERR_PARAM;
    }
    if(pstObj->pstData == NULL)
    {
        return IF_ERR_PARAM;
    }
    switch(iAttrId)
    {
        case IF_PHYID:
        {    
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PHYID;
            }
            ((ETHPORT_S*)(pstObj->pstData))->uiPortid=*((UINT *)pValue);
            break;
        }
        case IF_DUPLEX:
        {
            if(uiLen<sizeof(DUPLEX_E))
            {
                return IF_ERR_DUPLEX;
            }
            ((ETHPORT_S*)(pstObj->pstData))->enDuplex =*((DUPLEX_E *)pValue);
            break;
        }
        case IF_SPEED:
        {
            if(uiLen<sizeof(SPEED_E))
            {
                return IF_ERR_SPEED;
            }
            ((ETHPORT_S*)(pstObj->pstData))->enSpeed = *((SPEED_E *)pValue);
            break;
        }
        case IF_STATE:
        {
            if(uiLen<sizeof(PORTSTATE_E))
            {
                return IF_ERR_STATE;
            }
            ((ETHPORT_S*)(pstObj->pstData))->enState = *((PORTSTATE_E*)pValue);
            break;
        }
        case IF_TYPE:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM; 
            }
            ((ETHPORT_S*)(pstObj->pstData))->uiIsFiber = *(UINT*)pValue;
            break;
        }
        case IF_MAC:
        {     
            if(strlen((const char*)pValue)!=(MACLEN-1))
            {
               return  IF_ERR_MAC;
            }
            strcpy((CHAR *)(((ETHPORT_S*)(pstObj->pstData))->ucMac),(CONST CHAR*)pValue);
            break;
        }
        case IF_FIBER_INFO_FLAG:
        {
            if(uiLen<sizeof(FIBERINFOFLAG_E))
            {
                return IF_ERR_FIBER_INFO_FLAG;
            }
            ((ETHPORT_S*)(pstObj->pstData))->enFiberInfoFlag = *((FIBERINFOFLAG_E*)pValue);
            break;
        }
        case IF_FIBER_INFO:
        {
            if(uiLen > MAX_FIBER_INFO_LEN)
            {
                return IF_ERR_FIBER_INFO;
            }
            if(NULL == ((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo)
            {
                pucTemp = (UCHAR *)kmalloc(MAX_FIBER_INFO_LEN,GFP_KERNEL);
                if(NULL == pucTemp)
                {
                    return IF_ERR_FIBER_INFO;
                }
                ((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo = pucTemp;
            }
            memset(((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo, 0, MAX_FIBER_INFO_LEN);
            memcpy((CHAR *)(((ETHPORT_S*)(pstObj->pstData))->pucFiberInfo),(CONST CHAR*)pValue, uiLen);
            break;
        }
        default:
        {
            return IF_ERR_ATTRID;
        }
    }
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_EthportRegister()
*****************************************************************************/
VOID IF_EthportRegister(VOID)
{    
    memset(&stETHPort,0,sizeof(IF_REGISTER_BLOCK));
    stETHPort.init=IF_EthportInit;
    stETHPort.alloc=IF_EthportAlloc;
    stETHPort.free =IF_EthportFree;
    stETHPort.attribute=IF_EthportGetAttr;
    stETHPort.setattr=IF_EthportSetAttr;
    K_IF_Register(IF_SUB_ETHPORT,&stETHPort);
    printk("Registered Ethport \n");
    return ;    
}
/*****************************************************************************
    Func Name: IF_EthportUnRegist()
*****************************************************************************/
VOID IF_EthportUnRegister(VOID)
{   
    K_IF_UnRegister(IF_SUB_ETHPORT);
    printk("UnRegistered ethport\n");
    return ;
}
#ifdef __cplusplus
}
#endif


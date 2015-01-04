/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include <lw_if_pub.h>
#include <lw_if_l3if.h>
#include <linux/slab.h>
#include <lw_if_obj.h>
#ifdef UT_TEST_X86
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "malloc_ut_stub.h"
#define kmalloc(a,b) malloc(a)
#define kfree(a)   free((void *)a) 
#define printk printf
#endif
/*****************************************************************************
    Func Name: IF_L3IfInit
                                                                            
*****************************************************************************/
IF_RET_E IF_L3IfInit(IN ifindex_t uiIfindex,INOUT VOID *pvPara)
{
    if(NULL == pvPara) 
    {
        return IF_ERR_PARAM;
    }
    if( IF_SUB_ETH != IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_L3VLAN!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_IPSEC!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_ETHSUB!=IF_SUB_TYPE(uiIfindex))
    {
        return IF_ERR_TYPE;
    }
    ((L3IF_S *)pvPara)->uiIpAddr=0;
    ((L3IF_S *)pvPara)->uiMask=0;
    ((L3IF_S *)pvPara)->uiGateWay=0;
    ((L3IF_S *)pvPara)->szLinkName[0]=0;
    ((L3IF_S *)pvPara)->szNetName[0]=0;
    ((L3IF_S *)pvPara)->uiLinkState=0; 
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_L3IfAlloc
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
VOID * IF_L3IfAlloc(IF_SUB_TYPE_E enIftype)
{
    VOID *p=NULL;
    if( IF_SUB_ETH != enIftype
        &&IF_SUB_L3VLAN!= enIftype
        &&IF_SUB_IPSEC!= enIftype
        &&IF_SUB_ETHSUB!=enIftype)
    {
        return NULL;
    }
    p=kmalloc(sizeof(L3IF_S),GFP_KERNEL);
    if(p==NULL)
    {
        return p;
    }
    else
    {
        memset(p,0,sizeof(L3IF_S));
    }
    return p;
}

/*****************************************************************************
    Func Name: IF_L3IfFree
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

VOID  IF_L3IfFree(CONST VOID * pData)
{
    if( NULL == pData)
    {
        return ;
    }   
    kfree(pData);
}
/*****************************************************************************
    Func Name: IF_L3IfGetAttr()
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

IF_RET_E  IF_L3IfGetAttr(ifindex_t uiIfindex,INT iAttrId, VOID * pValue ,UINT iLen)
{
    IF_OBJ_S * pstObj=NULL;
    
    if(iAttrId >= IF_ATTR_ALL || NULL == pValue)
    {
        return IF_ERR_PARAM;
    }
    if( IF_SUB_ETH != IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_L3VLAN!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_IPSEC!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_ETHSUB!=IF_SUB_TYPE(uiIfindex))
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
            if(iLen<sizeof((((L3IF_S*)(pstObj->pstData))->uiPhyid)))
            {
                return IF_ERR_PARAM; 
            }
            memcpy(pValue,&(((L3IF_S*)(pstObj->pstData))->uiPhyid),sizeof(UINT));
            break;
        }
        case IF_IP:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiIpAddr;
        }
        break;
        case IF_MASK:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiMask;
        }
        break; 
        case IF_GATEWAY:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiGateWay;
        }
        break;
        case IF_DNSPRI_DYN:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiDnsMst;
        }
        break;
        case IF_DNSSLV_DYN:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiDnsSlv;
        }
        break;
        case IF_WORKMODE:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiWorkMode;                       
        }
        break;
        case IF_MTU:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiMtu;
        }    
        break;
        case IF_LINKNAME:
        {
            if(iLen<IFNAMESIZE)
            {
                return IF_ERR_PARAM;
            }
            memcpy(pValue,((L3IF_S *)pstObj->pstData)->szLinkName,IFNAMESIZE);
        }
        break;
        case IF_NETNAME:
        {
            if(iLen<IFNAMESIZE)
            {
                return IF_ERR_PARAM;
            }
            memcpy(pValue,((L3IF_S *)pstObj->pstData)->szNetName,IFNAMESIZE);
        }
        break;   
        case IF_MAC:
        {
            if(iLen<MACADDRSIZE)
            {
                return IF_ERR_PARAM;
            }
            memcpy(pValue,((L3IF_S *)pstObj->pstData)->szMacaddr,MACADDRSIZE);
        }
        break;
        case IF_MONITOR_STATE:
        {
            if(iLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            *(UINT *)pValue=((L3IF_S *)pstObj->pstData)->uiLinkState;
        }
        break;
        default:
        {
            return IF_ERR_ATTRID;
        }  
        
    }    
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_L3IfSetAttr()
                                                                            
*****************************************************************************/

IF_RET_E IF_L3IfSetAttr(ifindex_t uiIfindex, INT iAttrId,VOID * pValue,UINT uiLen )
{
    IF_OBJ_S * pstObj=NULL;
    if(iAttrId >= IF_ATTR_ALL || NULL == pValue)
    {
        return IF_ERR_PARAM;
    }
    if( IF_SUB_ETH != IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_L3VLAN!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_IPSEC!= IF_SUB_TYPE(uiIfindex)
        &&IF_SUB_ETHSUB!=IF_SUB_TYPE(uiIfindex))
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
            ((L3IF_S*)(pstObj->pstData))->uiPhyid=*((UINT *)pValue);
            break;
        }
        case IF_IP:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiIpAddr=*(UINT *)pValue;
        }
        break;
        case IF_MTU:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiMtu=*(UINT *)pValue;
        }
        break;
        case IF_MASK:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiMask=*(UINT *)pValue;
        }
        break;
        case IF_GATEWAY:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiGateWay=*(UINT *)pValue;
        }    
        break;
        case IF_DNSSLV_DYN:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiDnsSlv=*(UINT *)pValue;
        }
        break;
        case IF_DNSPRI_DYN:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiDnsMst=*(UINT *)pValue;
        }
        break;
        case IF_WORKMODE:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiWorkMode=*(UINT *)pValue;            
        }
        break;
        case IF_LINKNAME:
        {
            if(uiLen>IFNAMESIZE)
            {
                 return IF_ERR_PARAM;
            }
            memset(((L3IF_S *)pstObj->pstData)->szLinkName,0,IFNAMESIZE);
            memcpy(((L3IF_S *)pstObj->pstData)->szLinkName,pValue,uiLen);
        }
        break;
        case IF_NETNAME:
        {
            if(uiLen>IFNAMESIZE)
            {
                 return IF_ERR_PARAM;
            }
            memset(((L3IF_S *)pstObj->pstData)->szNetName,0,IFNAMESIZE);
            memcpy(((L3IF_S *)pstObj->pstData)->szNetName,pValue,uiLen);
        }
        break;
        case IF_MAC:
        {
            if(uiLen>MACADDRSIZE)
            {
                 return IF_ERR_PARAM;
            }
            memset(((L3IF_S *)pstObj->pstData)->szMacaddr,0,MACADDRSIZE);
            memcpy(((L3IF_S *)pstObj->pstData)->szMacaddr,pValue,uiLen);
        }
        break;
        case IF_MONITOR_STATE:
        {
            if(uiLen<sizeof(UINT))
            {
                return IF_ERR_PARAM;
            }
            ((L3IF_S *)pstObj->pstData)->uiLinkState=*(UINT *)pValue;
        }
        break;
        default:
        {
            return IF_ERR_ATTRID;
        }
    }    
    return IF_OK;
}



#ifdef  __cplusplus
}
#endif


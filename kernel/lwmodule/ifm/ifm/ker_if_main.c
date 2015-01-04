/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
/*lint -save -e10 */
#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <lw_type.h>
//#include <mw_msg.h>
//#include <mw_dbg.h>
#include <lw_if_pub.h>
#include <lw_if_type.h>
#include <lw_if_obj.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
//#include <asm/semaphore.h>
#include <net/netlink.h>
#include <lw_if_ethport.h>
#include <lw_if_l3if.h>
#include <linux/version.h>

/*lint -restore */
extern VOID IF_EthportRegister(VOID);
extern VOID IF_EthportUnRegister(VOID);
extern VOID IF_EthRegister(VOID);
extern VOID IF_EthUnRegister(VOID);

STATIC IF_RET_E DeleteIntfCore( ifindex_t uiIfIndex );
//#define  CONFIG_GCOV_ALL 0
#if  defined(UT_TEST) 
#include <kmalloc_ut_stub.h>
#include <ifobj_ut_stub.h>
#include <skbuff_ut_stub.h>
#endif
struct sock * psIFMnl;
 /*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
DEFINE_RWLOCK(if_lock);
/*lint -restore*/
STATIC SYS_SETUP_STATE_E gSetstate;

/*IFM module kernel debug switch*/
int ifm_kdbg = 0; /*debug开关变量*/


/*****************************************************************************
    Func Name: CreateIntfCore
*****************************************************************************/
STATIC IF_RET_E CreateIntfCore( IN IF_INFO_S * pstInfo, OUT ifindex_t *puiIfIndex)
{
    ifindex_t uiIndex = 0;
    IF_OBJ_S *pstObj = NULL;
    VOID * pData = NULL;
    IF_RET_E rv = IF_OK;
    IF_DOMAIN_E enDomain;

    enDomain=IF_DOMAIN(pstInfo->uiParentIfindex);
    if (IF_DOMAIN_IFINDEX(IF_SUB_ROOT,enDomain,0)!= pstInfo->uiParentIfindex)
    {
        if(FALSE == TestIfObjExist(pstInfo->uiParentIfindex))
        {
            IFM_DBG_ERR(" parent %02x does not exist\n",pstInfo->uiParentIfindex);
            return IF_NO_PARENT;
        }
    }

    if (!VALID_SUB_TYPE(pstInfo->enSubType))
    {
        IFM_DBG_ERR(" invalid if type %d\n",pstInfo->enSubType);
        return IF_ERR_TYPE;
    }


    if ((strlen(pstInfo->szPseudoName)!=0)&&(NULL != GetObjByPseudoNameByDomain(pstInfo->szPseudoName,enDomain)))
    {
        IFM_DBG_ERR(" if %s exist %d\n",pstInfo->szPseudoName,__LINE__);
        return IF_ERR_PDNAME_USED;
    }
    if ((strlen(pstInfo->szIfName)!=0)&&(NULL != GetObjByIfNameBYDomain(pstInfo->szIfName,enDomain)))
    {
        IFM_DBG_ERR(" if %s exist %d\n",pstInfo->szIfName,__LINE__);
        return IF_ERR_NAME_USED;
    }
    if (pstInfo->uiSpecIndex != IF_INVALID_IFINDEX) 
    {
        uiIndex = pstInfo->uiSpecIndex;
        if (TRUE == TestIfObjExist(uiIndex)) 
        {
            IFM_DBG_ERR(" ifindex %02x exist\n",pstInfo->uiSpecIndex);
            return IF_ALREADY_EXIST;
        }
    }
    else 
    {
        uiIndex = GetFreeIfindex(pstInfo->enSubType,IF_DOMAIN(pstInfo->uiParentIfindex));
        if (IF_INVALID_IFINDEX == uiIndex)
        {
            IFM_DBG_ERR(" get free inex fail\n");
            return IF_NO_FREE_INDEX;
        }
    }

    pstObj = AllocIfObj();
    if(NULL == pstObj)
    {
        IFM_DBG_ERR(" get ifobj fail\n");
        return IF_ERR_NO_MEM;
    }
    
    pstObj->uiIfIndex = uiIndex;
    pstObj->iChildCnt = 0;

    rv = AddObjToList(pstObj);
    if(IF_OK != rv)
    {
        FreeIfObj(pstObj);
        IFM_DBG_ERR(" add ifobj fail\n");
        return rv;
    }

    pData = (VOID *)MallocIfData(pstInfo->enSubType);

    pstObj->pstData = pData;

    rv = InitIfData(uiIndex,pData);
    if(IF_OK != rv)
    {
        RemoveObjFromList(pstObj);
        FreeIfData(uiIndex,pData);
        FreeIfObj(pstObj);        
        IFM_DBG_ERR(" init ifdata fail\n");
        return rv;
    }
    SetIfPseudoName(pstObj,pstInfo->szPseudoName);
    SetIfPhysicalId(pstObj,pstInfo->iPhysicalId);
    SetIfName(pstObj,pstInfo->szIfName);
    IFM_DBG_INFO("if %s is created\n",pstInfo->szIfName);
    /*设置父子关系*/
    pstObj->pstParent = GetObjByIfindex(pstInfo->uiParentIfindex);
    
    if(NULL!=pstObj->pstParent&&pstObj->pstParent!=pstObj)
    {
        pstObj->pstParent->iChildCnt++;
    }   
    
    *puiIfIndex = pstObj->uiIfIndex;

    return IF_OK;    
}

/*****************************************************************************
    Func Name: K_IF_CreatInterface
                                                                            
*****************************************************************************/
IF_RET_E K_IF_CreatInterface( IN IF_INFO_S * pstInfo, OUT ifindex_t * puiIfIndex )
{
    IF_RET_E rv = IF_ERR_PARAM;
    
    if (NULL == pstInfo)
    {
        return IF_ERR_PARAM;
    }
    if (NULL == puiIfIndex)
    {
        return IF_ERR_PARAM;
    }
    
    /*获取接口链表写锁*/
    write_lock_bh(&if_lock);
    rv = CreateIntfCore(pstInfo,puiIfIndex);
    /*释放写锁*/
    write_unlock_bh(&if_lock);
   
    return rv;
}


/*****************************************************************************
    Func Name: K_IF_DeleteInterface
                                                                            
*****************************************************************************/
IF_RET_E K_IF_DeleteInterface( IN ifindex_t uiIfIndex )
{
    IF_RET_E rv = IF_OK;
    
    if(!VALID_IFINDEX(uiIfIndex))
    {
        IFM_DBG_ERR("%s fail,invalid ifindex (%d)of %02x\n",__FUNCTION__,IF_SUB_TYPE(uiIfIndex),uiIfIndex);
        return IF_ERR_PARAM;
    }
    rv = K_IF_TestIfExist(uiIfIndex);
    if(IF_OK != rv)
    {
        IFM_DBG_ERR("%s fail,%02x does not exist\n",__FUNCTION__,uiIfIndex);
        return rv;
    }
    write_lock_bh(&if_lock);
    rv = DeleteIntfCore(uiIfIndex);
    write_unlock_bh(&if_lock);
    
    return rv;
}

/*****************************************************************************
    Func Name: DeleteIntfCore
                                                                            
*****************************************************************************/
STATIC IF_RET_E DeleteIntfCore( ifindex_t uiIfIndex )
{
    IF_OBJ_S * pstObj = NULL;
    IF_RET_E rv = IF_OK;
    UINT uiFlag=0;
    pstObj = GetObjByIfindex(uiIfIndex);
    if(NULL == pstObj)
    {
        return IF_NFOUND;
    }
    uiFlag=IF_GET_FLAG(uiIfIndex);   
    if((uiFlag!=IF_FORCE_DELETE_FLAG)&&(0 != pstObj->iChildCnt))
    {  
        IFM_DBG_ERR("DestroyIfData fail,%02x has child\n",uiIfIndex);
        return IF_CHILD_EXIST;
    }

    rv = DestroyIfData(uiIfIndex);
    if(IF_OK != rv)
    {
        IFM_DBG_ERR("DestroyIfData fail");
        return rv;
    }   
    rv = RemoveObjFromList(pstObj);
    if(IF_OK != rv)
    {
        IFM_DBG_ERR("RemoveObjFromList fail");
        return rv;
    }
    
    FreeIfData(uiIfIndex,pstObj->pstData);
    
    if((uiFlag!=IF_FORCE_DELETE_FLAG)&&(NULL != pstObj->pstParent))
    {
        if(pstObj->pstParent->iChildCnt > 0)
        {
            pstObj->pstParent->iChildCnt--;
        }
    }    
    FreeIfObj(pstObj);
    return IF_OK;
    
}
/*****************************************************************************
    Func Name: IF_GetAttr
                                                                            
*****************************************************************************/

IF_RET_E K_IF_GetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, INOUT VOID * pValue ,IN UINT iLen)
{
    IF_OBJ_S * pstObj = NULL;
  //  IF_SUB_TYPE_E enTemp = IF_SUB_ROOT;
    IF_RET_E rv = IF_OK;
    
    /*入参检查*/ 
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }
    if(!VALID_IF_ATTR_ID(iAttrId))  
    {
        return IF_ERR_ATTRID;
    }
    if(NULL == pValue)
    {
        return IF_ERR_PARAM;
    }    

    /*获取接口实体*/
    read_lock_bh(&if_lock);
    pstObj = GetObjByIfindex(uiIfindex);  
    if(NULL == pstObj)
    {
        read_unlock_bh(&if_lock);
        return IF_NFOUND;
    }
    
    /*数据选择填充*/
    switch(iAttrId)
    {
        #if 0
        #endif
        case IF_REFCNT:
        if(iLen < sizeof(UINT))
        {
            read_unlock_bh(&if_lock);               
            return IF_SHORT_BUFF_SIZE;
        }   
        *(UINT*)pValue =(UINT)(pstObj->iChildCnt);
        break;
        case IF_PARENT:
        {
            if(NULL == pstObj->pstParent)
            {
                read_unlock_bh(&if_lock);
                return IF_NO_PARENT;

            }
            if(iLen < sizeof(ifindex_t))
            {
                read_unlock_bh(&if_lock);
                return IF_SHORT_BUFF_SIZE;
            }
            memcpy(pValue,&(pstObj->pstParent->uiIfIndex),sizeof(ifindex_t));
            break;
        }
        case IF_PSEUDONAME:
        {
            if(iLen < IFNAMESIZE)
            {
                read_unlock_bh(&if_lock);               
                return IF_SHORT_BUFF_SIZE;
            }         
            memcpy(pValue,pstObj->ucPseudoName,IFNAMESIZE);
            break;
        }
        case IF_NAME:
        {      
            if(iLen < IFNAMESIZE)
            {
                read_unlock_bh(&if_lock);
                return IF_SHORT_BUFF_SIZE;
            }
            memcpy(pValue,pstObj->ucIfName,IFNAMESIZE);
            break;
        }
        case IF_ATTRENABLE:
        {
            if(iLen < sizeof(IF_ENABLE_E))
            {
                read_unlock_bh(&if_lock);
                return IF_SHORT_BUFF_SIZE;
            }
            *(IF_ENABLE_E*)pValue=pstObj->enEnable;
            break;
        }
        case IF_STATE:
        {
            if(iLen < sizeof(IF_STATUS_E))
            {
                read_unlock_bh(&if_lock);
                return IF_SHORT_BUFF_SIZE;
            }
            *(IF_STATUS_E*)pValue=pstObj->enStatus;
            break;
        }
        default:
        {
            rv = GetIfPriAttr( uiIfindex,iAttrId,pValue,iLen);
            if(IF_OK != rv)
            {            
                read_unlock_bh(&if_lock);
                return rv;
            }
            else
            {
                break;
            }           
        }
    }    
    read_unlock_bh(&if_lock);
    return IF_OK;
}
/*****************************************************************************
    Func Name: K_IF_SetAttr
*****************************************************************************/
IF_RET_E K_IF_SetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue,IN UINT uiLen )
{
    IF_OBJ_S * ifobj=NULL;
    IF_RET_E iRet=IF_ERR_PARAM;
    /*检查入参*/
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }    
    if(!VALID_IF_ATTR_ID(iAttrId))  
    {
        return IF_ERR_ATTRID;
    }
    if(NULL == pValue)
    {        
        return IF_ERR_PARAM;
    } 
    /*根据uiIfindex获取obj*/
    write_lock_bh(&if_lock);
    ifobj=GetObjByIfindex(uiIfindex);
    if(NULL == ifobj)
    {       
        write_unlock_bh(&if_lock);
        return IF_ERR_PARAM;
    }
    /*通过iAttrId获取入参的类型*/
    switch(iAttrId)
    {
        case IF_PSEUDONAME:
            if(uiLen>IFNAMESIZE)
            {
                write_unlock_bh(&if_lock);
                return IF_ERR_PARAM;
            }
            memset(ifobj->ucPseudoName,0,IFNAMESIZE);
            memcpy((CHAR *)(ifobj->ucPseudoName),pValue,uiLen);
            iRet=IF_OK;
            break; 
        case IF_ATTRENABLE:
            {
                if(uiLen<sizeof(IF_ENABLE_E))
                {
                    write_unlock_bh(&if_lock);
                    return IF_ERR_PARAM;
                }
                ifobj->enEnable=*(IF_ENABLE_E*)pValue;
                iRet=IF_OK;                
            }
            break;
        case IF_STATE:
            {
                if(uiLen<sizeof(IF_STATUS_E))
                {
                    write_unlock_bh(&if_lock);
                    return IF_ERR_PARAM;
                }
                ifobj->enStatus=*(IF_STATUS_E*)pValue;
                iRet=IF_OK;
            }
            break;
        default:
            {               
                iRet=SetPriAttr(uiIfindex,iAttrId,pValue,uiLen);
                break;
            }
           
    }    
    write_unlock_bh(&if_lock);
    return iRet;
}
/*****************************************************************************
    Func Name: K_IF_GetByPseudoName
------------------------------------------------------------------------------
*****************************************************************************/

IF_RET_E K_IF_GetByPseudoNameDomain( IN CHAR * name,IN IF_DOMAIN_E enDomain, OUT ifindex_t * puiIfIndex )
{
    IF_OBJ_S * pstObj = NULL;
    
    if( NULL == name || NULL == puiIfIndex)
    {
        return IF_ERR_PARAM;
    }
    read_lock_bh(&if_lock);
    pstObj=GetObjByPseudoNameByDomain(name,enDomain);
    if(NULL != pstObj)
    {
        *puiIfIndex = pstObj->uiIfIndex;        
    }
    else
    {   
        *puiIfIndex=IF_INVALID_IFINDEX;       
         read_unlock_bh(&if_lock);
         return IF_NFOUND;        
    }
    read_unlock_bh(&if_lock);
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_GetByIFName
                                                                            
*****************************************************************************/
IF_RET_E K_IF_GetByIFNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex )
{
    IF_OBJ_S * pstObj=NULL;
    
    if((NULL == name)||(NULL == puiIfIndex))
    {
        return IF_ERR_PARAM;
    }
    if(strlen(name) >= IFNAMESIZE)
    {
        return IF_ERR_PARAM;
    }
    
    read_lock_bh(&if_lock);
    pstObj = GetObjByIfNameBYDomain(name,enDomain);
    if(NULL != pstObj)
    {
        *puiIfIndex = pstObj->uiIfIndex;
    }
    else
    {
        *puiIfIndex = IF_INVALID_IFINDEX;
        read_unlock_bh(&if_lock);        
        return IF_NFOUND;
    }
    read_unlock_bh(&if_lock);    
    return IF_OK;
}

/*****************************************************************************
    Func Name: K_IF_GetNextIfindex
                                                                            
*****************************************************************************/
IF_RET_E K_IF_GetNextIfindex( IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t * puiIfindex )
{
    IF_OBJ_S * pstObj = NULL;
    
    if ( NULL == puiIfindex || enSubType > IF_SUB_PHY_ALL)
    {
        if(puiIfindex)
        {
            *puiIfindex = IF_INVALID_IFINDEX;
        }
        return IF_ERR_PARAM;
    }
    read_lock_bh(&if_lock);
    pstObj=GetNextObj(enSubType,*puiIfindex);
    if(NULL != pstObj)
    {
        *puiIfindex=pstObj->uiIfIndex;          
    }
    else
    {   
        *puiIfindex=IF_INVALID_IFINDEX;       
         read_unlock_bh(&if_lock);
         return IF_NFOUND;        
    }
    read_unlock_bh(&if_lock);
    return IF_OK;    
}

/*****************************************************************************
    Func Name: IF_GetFirstIfindex
                                                                            
*****************************************************************************/
IF_RET_E K_IF_GetFirstIfindexByDomain( IN IF_SUB_TYPE_E enSubType, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfindex )
{
    IF_RET_E enRet;
    /*检查入参*/
    if ( NULL == puiIfindex || enSubType > IF_SUB_PHY_ALL)
    {
        return IF_ERR_PARAM;
    }
    read_lock_bh(&if_lock);
    enRet = GetFirstIfindexbyDomain(enSubType,enDomain,puiIfindex);
    read_unlock_bh(&if_lock);

    return enRet;
}


/*****************************************************************************
    Func Name: IF_ModuleInit
*****************************************************************************/
int  IF_ModuleInit(VOID)
{
    InitObjBitPool();
    InitIFRegisterTbl();
    InitIFTbl();
    IF_EthportRegister();  
    IF_L3vlanRegister();
    IF_EthRegister();
    IFM_DBG_INFO("Init Ifm Module...OK\n");
    return 0;    
}

/*****************************************************************************
    Func Name: IF_ModuleExit
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
 VOID  IF_ModuleExit(VOID) 
{
    IF_EthUnRegister();
    IF_L3vlanUnRegister();
    IF_EthportUnRegister();

    return;
}

/*****************************************************************************
    Func Name: K_IF_TestIfExist
*****************************************************************************/
IF_RET_E K_IF_TestIfExist(IN ifindex_t uiIfindex)
{
    read_lock_bh(&if_lock);    
    if(true == TestIfObjExist(uiIfindex))
    {
        read_unlock_bh(&if_lock);  
        return IF_OK;
    }
    else
    {
        read_unlock_bh(&if_lock);  
        return IF_NFOUND;
    }
}
/*****************************************************************************
    Func Name: K_IF_AttachParent()
*****************************************************************************/
IF_RET_E K_IF_AttachParent(IN ifindex_t uiIfindex,IN ifindex_t uiPifindex)
{   
    IF_OBJ_S *pstObj=NULL;
    IF_OBJ_S *pstPobj=NULL;
    if(!VALID_IFINDEX(uiIfindex)||!VALID_IFINDEX(uiPifindex))
    {
        return IF_INVALID_IDX;
    }
    write_lock_bh(&if_lock);
    pstObj = GetObjByIfindex(uiIfindex);   
    if(pstObj == NULL)
    {
        write_unlock_bh(&if_lock);
        return IF_NFOUND;
    }
    pstPobj=GetObjByIfindex(uiPifindex); 
    if(pstPobj == NULL)
    {
        write_unlock_bh(&if_lock);
        return IF_NFOUND;
    }
    if(pstObj->pstParent!=NULL)
    {
        if(pstObj->pstParent->iChildCnt>0)
        {
            pstObj->pstParent->iChildCnt--;
        }
    }
    pstObj->pstParent=pstPobj;
    pstPobj->iChildCnt++;
    write_unlock_bh(&if_lock);
    return IF_OK;
}

/****************************************************************
wanghuanyu
*****************************************************************/
IF_RET_E Ifm_Ioctl(IFM_REQ_S *pUsrreq)
{
    IF_RET_E ret;
    IF_GETINDEX_PAYLOAD_S stGet;
    void * value=NULL;
    ifindex_t uiIfIndex;
 //   printk("%s %d\n",__FUNCTION__,__LINE__);
    if(NULL == pUsrreq)
    {
        return IF_ERR_PANIC;
    }
    switch(pUsrreq->cmd)
    {
        case IF_CMD_CREATE:   
           // printk("%s %d\n",__FUNCTION__,__LINE__);
            ret = K_IF_CreatInterface(&(pUsrreq->m_ifm_creatpld),&(pUsrreq->m_ifm_createretpld.uiIfindex));
           // printk("%s %d %d %02x\n",__FUNCTION__,__LINE__,ret,pUsrreq->m_ifm_creatpld.uiParentIfindex);
           // printk("%s %d\n",__FUNCTION__,__LINE__);
            pUsrreq->err=ret;
            break;
        case IF_CMD_DELETE:
            ret = K_IF_DeleteInterface(pUsrreq->m_ifm_delpld.ifindex);
            pUsrreq->err=ret;
            break;
        case IF_CMD_GETATTR:
            value=kmalloc(pUsrreq->m_ifm_getattrpld.uiLen,GFP_KERNEL);
            if(value==NULL)            
                return IF_ERR_PANIC;
            
            ret = K_IF_GetAttr(pUsrreq->m_ifm_getattrpld.uiIfindex,pUsrreq->m_ifm_getattrpld.enAttrId,value,pUsrreq->m_ifm_getattrpld.uiLen);
           // pUsrreq.
            copy_to_user(pUsrreq->m_ifm_getattrretpld,value,pUsrreq->m_ifm_getattrpld.uiLen);
            kfree(value);
            pUsrreq->err=ret; 
            break;
        case IF_CMD_ATTACH:
           // pUsrreq.m_ifm_attach.
            pUsrreq->err=K_IF_AttachParent(pUsrreq->m_ifm_attach.uiIfindex,pUsrreq->m_ifm_attach.uiPifindex);
         //   stRet.uiPayloadlen = 0;
            ret=pUsrreq->err;
            break;
        case IF_CMD_GETBYIFNAME:            
            ret = K_IF_GetByIFNameDomain(pUsrreq->m_ifm_getpld.szIfName,pUsrreq->m_ifm_getpld.enDomain,&uiIfIndex);
            pUsrreq->m_ifm_getindexpld.uiIfindex=uiIfIndex;
            pUsrreq->err=ret;
            break;
        case IF_CMD_GETBYPSNAME:
            ret = K_IF_GetByPseudoNameDomain(pUsrreq->m_ifm_getpld.szPseudoName,pUsrreq->m_ifm_getpld.enDomain,&uiIfIndex);
            pUsrreq->m_ifm_getindexpld.uiIfindex=uiIfIndex;
            pUsrreq->err=ret;
            break;
        case IF_CMD_GETFIRSTINDEX:            
            ret = K_IF_GetFirstIfindexByDomain(pUsrreq->m_ifm_getindexpld.enType,pUsrreq->m_ifm_getindexpld.enDomain,&uiIfIndex);
            pUsrreq->m_ifm_getindexpld.uiIfindex=uiIfIndex;
            pUsrreq->err=ret;
            break;
        case IF_CMD_GETNEXTINDEX:            
             uiIfIndex=pUsrreq->m_ifm_getindexpld.uiIfindex;
            ret = K_IF_GetNextIfindex(pUsrreq->m_ifm_getindexpld.enType,&uiIfIndex);                       
            pUsrreq->m_ifm_getindexpld.uiIfindex=uiIfIndex;
            pUsrreq->err=ret;
            break;
        case IF_CMD_GETSYSSTATE:
            read_lock_bh(&if_lock);
          //  gSetstate=((IF_SYSSTATE_PAYLOAD_S*)((UCHAR*)pstMsg+sizeof(MW_MSG_S)))->enState;
            pUsrreq->m_ifm_getsysstate.enState=gSetstate;
            pUsrreq->err= IF_OK;
            ret=IF_OK;
            read_unlock_bh(&if_lock);
            break;
        case IF_CMD_TEST:
            ret=K_IF_TestIfExist(pUsrreq->m_ifm_testpld.ifindex);
            pUsrreq->err=ret;
            break;
        case IF_CMD_SETATTR:
            value=kmalloc(pUsrreq->m_ifm_setattrpld.uiLen,GFP_KERNEL);
            if(value==NULL) 
            {
                return IF_ERR_PANIC;
            }
            if( copy_from_user(value, pUsrreq->m_ifm_setattrpld.data.paddr, pUsrreq->m_ifm_setattrpld.uiLen))
            {
                kfree(value);
                return IF_ERR_PANIC;
            }
            ret=K_IF_SetAttr(pUsrreq->m_ifm_setattrpld.uiIfindex,pUsrreq->m_ifm_setattrpld.enAttrId,value,pUsrreq->m_ifm_setattrpld.uiLen);
            pUsrreq->err=ret;
            kfree(value);
            break;
        case IF_CMD_SETSYSSTATE:
            write_lock_bh(&if_lock);            
            gSetstate=pUsrreq->m_ifm_sysstatepld.enState;
            pUsrreq->err= IF_OK;
            ret=IF_OK;
            write_unlock_bh(&if_lock);
            break;
        default:
            return IF_ERR_PANIC;
        }
    return ret;
}

#if  defined(UT_TEST) 
EXPORT_SYMBOL(CreateIntfCore);
EXPORT_SYMBOL(DeleteIntfCore);
EXPORT_SYMBOL(GetObjByIfindex);
//EXPORT_SYMBOL(RemoveObjFromList);
EXPORT_SYMBOL(g_addobj_error);
EXPORT_SYMBOL(g_addobj_count);
EXPORT_SYMBOL(reset_addobj_stub);
EXPORT_SYMBOL(addobj_ut);
EXPORT_SYMBOL(IF_Append_Data);
EXPORT_SYMBOL(IF_SanityCheck);
EXPORT_SYMBOL(__IF_DispatchRequest);
EXPORT_SYMBOL(g_initifdata_error);
EXPORT_SYMBOL(g_initifdata_count);
EXPORT_SYMBOL(InitIfData_ut);
EXPORT_SYMBOL(reset_initifdata_stub);
EXPORT_SYMBOL(g_destroyifdata_error);   
EXPORT_SYMBOL(g_destroyifdata_count);
EXPORT_SYMBOL(reset_destroyifdata_stub);
EXPORT_SYMBOL(DestroyIfData_ut);
EXPORT_SYMBOL(g_removeobjfromlist_error);
EXPORT_SYMBOL(g_removeobjfromlist_count);
EXPORT_SYMBOL(reset_removeobjfromlist_stub);
EXPORT_SYMBOL(RemoveObjFromList_ut);

#endif
/*lint -save -e578 -e808 -e508 -e832 -e31 -e10 -e40 -e522 -e533 -e550 -e830*/
//EXPORT_SYMBOL(test_option);
EXPORT_SYMBOL(K_IF_CreatInterface);
EXPORT_SYMBOL(K_IF_DeleteInterface);
EXPORT_SYMBOL(K_IF_GetAttr);
EXPORT_SYMBOL(K_IF_GetByIFNameDomain);
EXPORT_SYMBOL(K_IF_GetByPseudoNameDomain);
EXPORT_SYMBOL(K_IF_GetNextIfindex);
EXPORT_SYMBOL(K_IF_GetFirstIfindexByDomain);
EXPORT_SYMBOL(K_IF_SetAttr);
EXPORT_SYMBOL(K_IF_AttachParent);
/*lint -restore */
#if  defined(UT_TEST) 
module_init(IF_ModuleInit);
module_exit(IF_ModuleExit);
#else
//arch_initcall(IF_ModuleInit);
#endif

#ifdef  __cplusplus
}
#endif

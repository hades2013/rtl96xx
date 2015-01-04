/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
//#include <mw_msg.h>
#include <stdlib.h>
#include <lw_if_pub.h>
#include <lw_if_ethport.h>
#include <lw_if_api.h>
#include <lw_if_obj.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>
//#include <mw_dbg.h>
//#include <cfg_pub.h>
//#include <mp_proc.h>

#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <lw_drv_req.h>
#include "lw_config_api.h" 
#include <unistd.h>
#include <if_notifier.h>

IF_RET_E Ifm_IoctlCmd( IFM_REQ_S *pifm_req )
{
    INT32 fd = 0;
    INT32 ret = 0;    
    CHAR tmpdevname[32];

    if(NULL==pifm_req)
    {
        return IF_ERR_PARAM;
    }
    memset(tmpdevname, 0, sizeof(tmpdevname));
    sprintf(tmpdevname, "/dev/");
    strcat(tmpdevname, "ifm");

    fd = open( tmpdevname, O_RDWR );
    if( fd <0 )
    {
    	return IF_ERR_PANIC;
    }

    ret = ioctl(fd, SIOCDRVPRIVATE, pifm_req);

    close(fd);

    if (0 != ret)
    {
        return IF_ERR_PANIC;
    }

    return pifm_req->err;
}

/*****************************************************************************
                                                                              
*****************************************************************************/
IF_RET_E IF_CreatInterface( IN IF_INFO_S * pstInfo, OUT ifindex_t * puiIfIndex )
{
    
  //  UCHAR *pstMsg = NULL;
  //  INT iQueueId = 0;
 //   CHAR * pcResult = NULL;
  //  UINT uiReslen = 0;
    INT iRet = IF_ERR_PARAM;
    ifindex_t uiIdx = 0;
    IF_RET_E rv = IF_OK;
    IFM_REQ_S ifm_req;

    memset(&ifm_req,0,sizeof(ifm_req));
    if(NULL == pstInfo)
    {
        return IF_ERR_PARAM;
    }
	
    if(NULL == puiIfIndex)
    {
        return IF_ERR_PARAM;
    }
	
	if(isMaster()!=MASTER)
    {
        return IF_ERR_PANIC;
    }
   // printf("%s %d %02x\n",__FUNCTION__,__LINE__,ifm_req.m_ifm_createretpld.uiIfindex);
    ifm_req.cmd=IF_CMD_CREATE;
    ifm_req.m_ifm_creatpld.iPhysicalId=pstInfo->iPhysicalId;
    ifm_req.m_ifm_creatpld.enSubType=pstInfo->enSubType;
    ifm_req.m_ifm_creatpld.uiParentIfindex=pstInfo->uiParentIfindex;
    ifm_req.m_ifm_creatpld.uiSpecIndex=pstInfo->uiSpecIndex;
    memcpy(ifm_req.m_ifm_creatpld.szPseudoName,pstInfo->szPseudoName,IFNAMESIZE);
    memcpy(ifm_req.m_ifm_creatpld.szIfName,pstInfo->szIfName,IFNAMESIZE);
    iRet=Ifm_IoctlCmd(&ifm_req);
    if(IF_OK != iRet)
    {
        return (IF_RET_E)iRet;
    }
    else
    {
        uiIdx=ifm_req.m_ifm_createretpld.uiIfindex;
        //printf("%s %d %02x\n",__FUNCTION__,__LINE__,ifm_req.m_ifm_createretpld.uiIfindex);
        if(ifm_call_notifiers(uiIdx, IF_NOTIFY_INTFADD, NULL))  
        {            
            return (IF_RET_E)IF_NOTIFY_ERR;   
        }
    }   

    rv = IF_InitInterface(uiIdx);
    if (IF_OK == rv)
    {
        if(ifm_call_notifiers(uiIdx, IF_NOTIFY_ALREADY_INIT, NULL))  
        {      
            //IFM_DBG_ERR("Raise event IF_EVENT_INTFADD failed.\n");   
            return (IF_RET_E)IF_NOTIFY_ERR;   
        }
        *puiIfIndex = uiIdx;
        return IF_OK;
    }
    else
    {
        IF_DeleteInterface(uiIdx);
        return rv;
    }    
}


/*****************************************************************************
                                                                             
*****************************************************************************/
IF_RET_E IF_DeleteInterface( IN ifindex_t uiIfIndex )
{
   // UCHAR*pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
  //  INT iQueueId = 0;
    IFM_REQ_S ifm_req;
    /*检查入参*/
    if(IF_INVALID_IFINDEX == uiIfIndex)
    {
        return IF_ERR_PARAM;
    } 
	
    if(isMaster()!=MASTER)
    {
        return IF_ERR_PANIC;
    }    
	
    iRet=IF_Destroy(uiIfIndex);
    if(IF_OK!=iRet)
    {
    	
        return IF_DESTROY_ERR;
    }
    ifm_req.cmd=IF_CMD_DELETE;
    ifm_req.m_ifm_delpld.ifindex=uiIfIndex;
    iRet=Ifm_IoctlCmd(&ifm_req);
    return (IF_RET_E)iRet;
   
}
/*****************************************************************************
                                                                            
*****************************************************************************/
IF_RET_E IF_GetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E enAttrId, INOUT VOID * pValue ,IN UINT uiLen)
{
    UCHAR*pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
   // INT iQueueId = 0;
  //  UINT uiReslen = 0;
  //  CHAR *result = NULL; 
    IFM_REQ_S ifm_req;
    /*检查入参*/
    if(IF_INVALID_IFINDEX == uiIfindex || NULL == pValue )
    {
        return IF_ERR_PARAM;
    } 
    pstMsg=(UCHAR *)malloc(uiLen);
    ifm_req.cmd=IF_CMD_GETATTR;
    ifm_req.m_ifm_getattrpld.enAttrId=enAttrId;
    ifm_req.m_ifm_getattrpld.uiIfindex=uiIfindex;
    ifm_req.m_ifm_getattrpld.uiLen=uiLen;
    ifm_req.m_ifm_getattrretpld=pstMsg;    
    iRet=Ifm_IoctlCmd(&ifm_req);
    if(iRet==IF_OK)
    memcpy(pValue,ifm_req.m_ifm_getattrretpld,uiLen);
    free(pstMsg);
    return (IF_RET_E)iRet;
}

/*****************************************************************************
                                                                               
*****************************************************************************/
IF_RET_E IF_GetByPseudoNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex )
{
  //  UCHAR*pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
   // INT iQueueId = 0;
    //UINT uiReslen = 0;
   // CHAR *result = NULL;
    IFM_REQ_S ifm_req;
    
    if( NULL == name || NULL == puiIfIndex)
    {
        return IF_ERR_PARAM;      
    }
    if(strlen(name)>=IFNAMESIZE)
    {
        return IF_ERR_PARAM;
    }
    ifm_req.cmd=IF_CMD_GETBYPSNAME;
    strcpy(ifm_req.m_ifm_getpld.szPseudoName,name);
    ifm_req.m_ifm_getpld.enDomain=enDomain;
    ifm_req.cmd=IF_CMD_GETBYPSNAME;
    iRet=Ifm_IoctlCmd(&ifm_req);
    *puiIfIndex=ifm_req.m_ifm_getindexpld.uiIfindex;
    return (IF_RET_E)iRet;   
    
}
/*****************************************************************************
 
                                                                            
*****************************************************************************/
IF_RET_E IF_GetByIFNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex )
{
 //   UCHAR*pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
//    INT iQueueId = 0;
  //  UINT uiReslen = 0;
 //   CHAR *result = NULL;
    IFM_REQ_S ifm_req;
    if( NULL == name || NULL == puiIfIndex)
    {
        IFM_DBG_ERR("invalid parameters\n");
        return IF_ERR_PARAM;      
    }
    if(strlen(name)>=IFNAMESIZE)
    {
        IFM_DBG_ERR("invalid parameters\n");
        return IF_ERR_PARAM;
    }
    strcpy(ifm_req.m_ifm_getpld.szIfName,name);
    ifm_req.m_ifm_getpld.enDomain=enDomain;
    ifm_req.cmd=IF_CMD_GETBYIFNAME;
    iRet=Ifm_IoctlCmd(&ifm_req);
    *puiIfIndex=ifm_req.m_ifm_getindexpld.uiIfindex;
    return (IF_RET_E)iRet;    
}


/*****************************************************************************
                                                                                
*****************************************************************************/
IF_RET_E IF_GetFirstIfindexByDomain( IN IF_SUB_TYPE_E enSubType, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfindex )
{
  //  UCHAR *pstMsg = NULL;
  //  INT iQueueId = 0;
    INT iRet = IF_ERR_PARAM;
 //   UINT uiReslen = 0;
//    CHAR *pcResult = NULL;
    IFM_REQ_S ifm_req;
    /*检查入参*/
    if ( NULL == puiIfindex || enSubType > IF_SUB_PHY_ALL)
    {
        return IF_ERR_PARAM;
    }
    ifm_req.m_ifm_getindexpld.enType=enSubType;
    ifm_req.m_ifm_getindexpld.enDomain=enDomain;
    ifm_req.cmd=IF_CMD_GETFIRSTINDEX;
 //   ifm_req.m_ifm_getindexpld.
    iRet=Ifm_IoctlCmd(&ifm_req);
    *puiIfindex=ifm_req.m_ifm_getindexpld.uiIfindex;
    return (IF_RET_E)iRet;   
}

/*****************************************************************************
                                                                               
*****************************************************************************/
IF_RET_E IF_GetNextIfindex( IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t * puiIfindex )
{
//    UCHAR *pstMsg = NULL;
 //   INT iQueueId = 0;
    INT iRet = IF_ERR_PARAM;
   // UINT uiReslen = 0;
   // CHAR *pcResult = NULL;
    IFM_REQ_S ifm_req;
    
    /*检查入参*/
    if ( NULL == puiIfindex || enSubType > IF_SUB_PHY_ALL)
    {
        return IF_ERR_PARAM;
    }
    
    ifm_req.m_ifm_getindexpld.enType=enSubType;
    ifm_req.m_ifm_getindexpld.uiIfindex=*puiIfindex;
    ifm_req.cmd=IF_CMD_GETNEXTINDEX;
    iRet=Ifm_IoctlCmd(&ifm_req);
    *puiIfindex=ifm_req.m_ifm_getindexpld.uiIfindex;
    return (IF_RET_E)iRet;      
}


/*****************************************************************************
                                                                              
*****************************************************************************/
IF_RET_E IF_SetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue,IN UINT uiLen )
{
  //  IF_SET_ATTR_PAYLOAD_S stSet;
 //   UCHAR *pstMsg = NULL;
 //   INT iQueueId = 0;
    INT iRet = IF_ERR_PARAM;  
    IFM_REQ_S ifm_req;
    //IF_ATTR_S * pstIfattr;   

   // ifm_req.
    ifm_req.m_ifm_setattrpld.enAttrId=iAttrId;
    ifm_req.m_ifm_setattrpld.uiIfindex=uiIfindex;
    ifm_req.m_ifm_setattrpld.uiLen=uiLen;
    ifm_req.m_ifm_setattrpld.data.paddr=malloc(uiLen+1);
    memcpy(ifm_req.m_ifm_setattrpld.data.paddr,pValue,uiLen);
    ifm_req.cmd=IF_CMD_SETATTR;    
    iRet=Ifm_IoctlCmd(&ifm_req);     
    free(ifm_req.m_ifm_setattrpld.data.paddr);
    return (IF_RET_E)(iRet);
}
/*****************************************************************************
                                                                              
*****************************************************************************/
IF_RET_E IF_TestIfExist(IN ifindex_t uiIfindex)
{
//    UCHAR *pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
//    INT iQueueId = 0;
    IFM_REQ_S ifm_req;
    /*检查入参*/
    if(IF_INVALID_IFINDEX == uiIfindex)
    {
        return IF_INVALID_IDX;
    } 
    ifm_req.cmd=IF_CMD_TEST;
    ifm_req.m_ifm_testpld.ifindex=uiIfindex;
    iRet=Ifm_IoctlCmd(&ifm_req);
    return (IF_RET_E)iRet;   
     
}
/*****************************************************************************
                                                                                
*****************************************************************************/

IF_RET_E IF_SetSysState(SYS_SETUP_STATE_E enState)
{
   // UCHAR *pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
  //  INT iQueueId = 0;
    IFM_REQ_S ifm_req;
    ifm_req.cmd=IF_CMD_SETSYSSTATE;
    ifm_req.m_ifm_sysstatepld.enState=enState;
    iRet=Ifm_IoctlCmd(&ifm_req);   
    return (IF_RET_E)iRet;      
}
IF_RET_E IF_GetSysState(SYS_SETUP_STATE_E *enState)
{
  //  UCHAR *pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
  //  INT iQueueId = 0;
   // UINT uiReslen = 0;
  //  CHAR *pcResult = NULL;
    IFM_REQ_S ifm_req;
    
    if(NULL == enState)
    {
        return IF_ERR_PARAM;
    }
    ifm_req.cmd=IF_CMD_GETSYSSTATE;
    iRet=Ifm_IoctlCmd(&ifm_req); 
    *enState=ifm_req.m_ifm_getsysstate.enState;
    
    return (IF_RET_E)iRet;   
}
/*****************************************************************************
                                                                               
*****************************************************************************/

IF_RET_E IF_AttachParent(IN ifindex_t uiIfindex,IN ifindex_t uiPifindex)
{
   // UCHAR *pstMsg = NULL;
    INT iRet = IF_ERR_PARAM;
  //  INT iQueueId = 0;
    IFM_REQ_S ifm_req;

    ifm_req.cmd=IF_CMD_ATTACH;
    ifm_req.m_ifm_attach.uiIfindex=uiIfindex;
    ifm_req.m_ifm_attach.uiPifindex=uiPifindex;
    iRet=Ifm_IoctlCmd(&ifm_req); 
    return (IF_RET_E)iRet;
}

/*****************************************************************************
                                                                               
*****************************************************************************/
INT32 IF_ExistInterfaceInit(void)
{
    INT32 eSubType = 0;
    ifindex_t ifIdx = 0;
    INT iRet;
   
    SYS_SETUP_STATE_E enState;
    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_ERR_STATE;
    }	
    for(eSubType=IF_SUB_ROOT; eSubType<IF_OBJ_SUB_TYPE_MAX; eSubType++)
    {
        if(IF_OK != IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)eSubType, IF_DOMAIN_NORMAL, &ifIdx))
        {
            continue;
        }
        do{
            if(enState>=SYS_SETUP_IFINIT)
            {
                ifm_call_notifiers(ifIdx, IF_NOTIFY_INTFADD, NULL);       
            }
            iRet = IF_InitInterface(ifIdx);			
            if(IF_OK != iRet)
            {
                IFM_DBG_ERR("IF_ExistInterfaceInit interface[%x] init fail %d\n",ifIdx,iRet);
            }
        }while(IF_OK == IF_GetNextIfindex((IF_SUB_TYPE_E)eSubType, &ifIdx));
    } 
   
    return IF_OK;
}


#ifdef  __cplusplus
}
#endif
 

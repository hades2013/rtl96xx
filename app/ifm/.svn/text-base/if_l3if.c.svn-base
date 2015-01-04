/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>    
#include "lw_if_pub.h" 
#include "lw_if_type.h"
#include "lw_if_l3if.h"
#include "lw_drv_pub.h"  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pdt_config.h"

#include "lw_config_api.h" 
#include "lw_config_oid.h" 



//#define IF_L3VLAN_NAMEPREFIX "l3vlan"
#define IF_L3VLAN_BASE   "eth0"
//#define IF_ETH_NAMEPREFIX "eth"
extern BOOL Drv_IsLinkUpPort(port_num_t lport);


//extern int cfg_getval(int ifindex,unsigned int oid,void *val,void* default_val,unsigned retlen);
//extern INT master_cfg_setval(IN ifindex_t ifindex, IN UINT uiCmoid,IN VOID * pVal);
/*****************************************************************************
    Func Name: IF_L2Up
                                                                            
*****************************************************************************/
#if  0
IF_RET_E IF_L2Up(IN ifindex_t uiIfindex)
{   
    CHAR szIfname[IFNAMESIZE];
    UINT uiVid;
    logic_pmask_t pstPortMask;
    INT iRet;
    INT ihasupport=0;
    memset(szIfname,0,sizeof(szIfname));
    port_num_t lport = 0;
    
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_NAME,szIfname,sizeof(szIfname)))
    {
        return IF_ERR_GETATTR;
    }   
    if(IF_SUB_TYPE(uiIfindex)==IF_SUB_L3VLAN)
    {
        sscanf(szIfname,IF_L3VLAN_NAMEPREFIX"%u",&uiVid);
        iRet=Drv_GetVlanMember((UINT16)uiVid,&pstPortMask);
        if(DRV_OK!=iRet)
        {
             return IF_ERR_PANIC;
        }
        iRet=Drv_MultiGetPortLinkState(&pstPortMask);
        if(DRV_OK!=iRet)
        {
             return IF_ERR_PANIC;
        }
        LgcPortFor(lport)
        {         
            if((TRUE == TstLgcMaskBit(lport, &pstPortMask)))
            {           
                ihasupport=1;
                break;
                
            }        
        }
        if(1==ihasupport)
        {
            return IF_OK;
        }
        else
        {
            return IF_ERR_PANIC;
        }   
    }
    else if(IF_SUB_TYPE(uiIfindex)==IF_SUB_ETH)
    {
        if(IF_OK!=IF_GetAttr(uiIfindex,IF_PHYID,&uiVid,sizeof(uiVid)))
        {
            return IF_ERR_GETATTR;
        }   
            
        if((TRUE == Drv_IsLinkUpPort(uiVid)))
        {         
            return IF_OK;
        }  
        else
        {
            return IF_ERR_PANIC;
        }
    }
    else
    {
         return IF_ERR_PANIC;
    }   
}
#endif
extern int setup(char * dev_name);

/*****************************************************************************
    Func Name: IF_L3Enable
                                                                            
*****************************************************************************/

IF_RET_E IF_L3Enable(IN ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;
    UCHAR    buf[BUF_SIZE128];   
  //  UCHAR    szCommand[BUF_SIZE128];
    IF_ENABLE_E uiEnable=IF_MOD_ENABLE;
    /*获取接口link设备*/
    
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME, buf, sizeof(buf));
    if(IF_OK!=iRet)
    {
        return IF_ERR_LINKNAME;
    }
     /*up device*/
    iRet=setup((CHAR*)buf);
    if(0!=iRet)
    {
        return IF_ERR_ENABLE;
    }
    /*更新enable*/
    iRet=IF_SetAttr(uiIfindex, IF_ATTRENABLE, &uiEnable, sizeof(uiEnable));
    if(iRet!=IF_OK)
    {        
        return IF_ERR_ENABLE;
    }   
     /*保存cfg*/
    return IF_OK;
}

/*****************************************************************************
    Func Name: IF_L3vlanInit
                                                                            
*****************************************************************************/

/*STATIC */IF_RET_E IF_L3vlanInit(IN ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;    
    CHAR    buf[BUF_SIZE128];
    ifindex_t uipIfindex;
    INT      vid=0;
    UCHAR    szCommand[BUF_SIZE128];
    IF_ENABLE_E enEnable=IF_MOD_DISABLE;
    SYS_SETUP_STATE_E enState; 
    CHAR szMacaddr[MACADDRSIZE];
    DRV_RET_E eDrvRet = DRV_OK;
    UINT  iWorkMode=IF_WORKMODE_ROUTER;
    
    CHAR szIfname[IFNAMESIZE] = {0};

    if(IF_OK!=IF_GetAttr(uiIfindex,IF_NAME,szIfname,sizeof(szIfname)))
    {
        return IF_ERR_GETATTR;
    }
    sscanf(szIfname,IF_L3VLAN_NAMEPREFIX"%u",&vid);
   /*配置数据初始化*/
    if(isMaster()!=MASTER)
    {
        return IF_INIT_ERR;
    }
    else
    {
        iRet=cfg_getval(uiIfindex,CONFIG_IF_ENABLE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(IF_MOD_DISABLE));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_ENABLE, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_ENABLE error\n");
                return IF_INIT_ERR;
            }        
        }        
        if(strcmp(buf,ENUMTOSTR(IF_MOD_DISABLE))==0)
        {           
            enEnable=IF_MOD_DISABLE;
        }
        else
        {
            enEnable=IF_MOD_ENABLE;
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_DESC,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"Vlan-Interface%u Interface",vid);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_DESC, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_DESC error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_TYPE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",IF_L3VLAN_NAMEPREFIX);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_TYPE, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_TYPE error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_NAME,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s%u",IF_L3VLAN_NAMEPREFIX,vid);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_NAME, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_NAME error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_DIAL_TYPE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(CT_STATIC));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_DIAL_TYPE, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_DIAL_TYPE error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_ADDR,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","0.0.0.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_ADDR, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_ADDR error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_MASK,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","255.255.255.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_MASK, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_MASK error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_GATEWAY,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","0.0.0.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_GATEWAY,(UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_GATEWAY error\n");
                return IF_INIT_ERR;
            }        
        }          
        iRet=cfg_getval(uiIfindex,CONFIG_IP_MTU,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%u",1500);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_MTU,(UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_MTU error\n");
                return IF_INIT_ERR;
            }        
        } 
    }
    if(IF_DOMAIN(uiIfindex)!=IF_DOMAIN_NORMAL)
    {
       return IF_OK;
    } 

    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_INIT_ERR;
    }

    if(enState>=SYS_SETUP_CFGINIT)
    {
        /*获取父接口索引*/  
        iRet=IF_GetByIFName(IF_L3VLAN_BASE,&uipIfindex);
        if(IF_OK!=iRet)
        {
            IFM_DBG_ERR("get %02x IF_PARENT error\n",uiIfindex);
            return IF_INIT_ERR;
        }  
        /*重建父子关系*/
        iRet=IF_AttachParent(uiIfindex, uipIfindex);
        if(IF_OK!=iRet)
        {
            IFM_DBG_ERR("attach %02x IF_PARENT error\n",uiIfindex);
            return IF_INIT_ERR;
        }   
         /*<SYS_SETUP_IFINIT返回*/
        if(enState < SYS_SETUP_IFINIT)
        {       
            return IF_OK;
        }
        /*获取父接口link设备*/   
        iRet=IF_GetAttr(uipIfindex, IF_LINKNAME, buf, sizeof(buf));
        if(IF_OK!=iRet)
        {        
            return IF_INIT_ERR;
        }    

         /*添加vlan netdevice*/
        sprintf((CHAR*)szCommand,"vconfig add %s %d",buf,vid);
       // printf("szCommand=%s\n",szCommand);
        system((CONST CHAR*)szCommand);
        /*设置txlen*/
        memset(szCommand,0,sizeof(szCommand));
        sprintf((CHAR*)szCommand,"ip link set %s.%d txqueuelen 1000",buf,vid);
        system((CONST CHAR*)szCommand);
        /*设置接口的link设备*/
        memset(szCommand,0,sizeof(szCommand));
        sprintf((CHAR*)szCommand,"%s.%d",buf,vid);
        iRet=IF_SetAttr(uiIfindex, IF_LINKNAME, (VOID*)szCommand, strlen((CONST CHAR*)szCommand));
        if(iRet!=IF_OK)
        {
            return IF_INIT_ERR;
        }
        iRet=IF_SetAttr(uiIfindex, IF_NETNAME, (VOID*)szCommand, strlen((CONST CHAR*)szCommand));
        if(iRet!=IF_OK)
        {
            return IF_INIT_ERR;
        }
        memset(szMacaddr,0,sizeof(szMacaddr));
        iRet = getmac_string((CHAR*)szCommand, szMacaddr);
        if(IF_OK != iRet)
        {
            return IF_INIT_ERR;
        }
        iRet=IF_SetAttr(uiIfindex, IF_MAC, (VOID*)szMacaddr, strlen((CONST CHAR*)szMacaddr));
        if(IF_OK != iRet)
        {
            return IF_INIT_ERR;
        }
        iRet = IF_SetAttr(uiIfindex, IF_WORKMODE, &iWorkMode, sizeof(iWorkMode));
        if(IF_OK != iRet)
        {
            return IF_INIT_ERR;
        }
        if(enEnable==IF_MOD_ENABLE)
        {
            IF_Enable(uiIfindex);
        }
        
        eDrvRet = Drv_VlanIfCreate((vlan_id_t)vid);
        if(DRV_OK == eDrvRet)
        {
            return IF_OK;
        }
        else if(DRV_ERR_VLAN_NOT_EXIST == eDrvRet)
        {
            return IF_VLAN_NOT_EXIST;
        }
        else if(DRV_ERR_VLAN_IF_EXIST == eDrvRet)
        {
            return IF_VLAN_IF_ALREADY_EXIST;
        }
        else
        {
            return IF_INIT_ERR;
        }
    }

    return IF_OK;
   
}
/*****************************************************************************
    Func Name: IF_EthInit                                                                            
*****************************************************************************/

IF_RET_E IF_EthInit(IN ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;    
    CHAR    buf[BUF_SIZE128];    
    IF_ENABLE_E enEnable=IF_MOD_DISABLE;
    SYS_SETUP_STATE_E enState; 
    CHAR    szCommand[BUF_SIZE128];
    CHAR szMacaddr[MACADDRSIZE];
    UINT  iWorkMode=IF_WORKMODE_ROUTER;
   /*配置数据初始化*/
    if(isMaster()!=MASTER)
    {
        return IF_INIT_ERR;
    }
    else
    {
        iRet=cfg_getval(uiIfindex,CONFIG_IF_ENABLE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(IF_MOD_DISABLE));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_ENABLE, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_ENABLE error\n");
                return IF_INIT_ERR;
            }        
        }
        //sscanf(buf,"%u",&enEnable);
        if(strcmp(buf,ENUMTOSTR(IF_MOD_DISABLE))==0)
        {           
            enEnable=IF_MOD_DISABLE;
        }
        else
        {
            enEnable=IF_MOD_ENABLE;
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_DESC,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",IF_ETH_NAMEPREFIX);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_DESC, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_DESC error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_TYPE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",IF_ETH_NAMEPREFIX);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_TYPE, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_TYPE error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_NAME,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s%u",IF_ETH_NAMEPREFIX,IF_INDEX(uiIfindex));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_NAME, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_NAME error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_DIAL_TYPE,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(CT_STATIC));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_DIAL_TYPE, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_DIAL_TYPE error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_ADDR,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","0.0.0.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_ADDR, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_ADDR error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_MASK,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","255.255.255.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_MASK, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_MASK error\n");
                return IF_INIT_ERR;
            }        
        } 
        iRet=cfg_getval(uiIfindex,CONFIG_IP_GATEWAY,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s","0.0.0.0");  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_GATEWAY,(UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_GATEWAY error\n");
                return IF_INIT_ERR;
            }        
        } 
       
        iRet=cfg_getval(uiIfindex,CONFIG_IP_MTU,buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%u",1500);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IP_MTU,(UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IP_MTU error\n");
                return IF_INIT_ERR;
            }        
        } 
    }
    if(IF_DOMAIN(uiIfindex)!=IF_DOMAIN_NORMAL)
    {
        return IF_OK;
    }
    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_INIT_ERR;
    }
    if(enState < SYS_SETUP_IFINIT)
    {
        return IF_OK;
    }
   
    if(enEnable==IF_MOD_ENABLE)
    {
        IF_Enable(uiIfindex);        
    }   
    memset(szCommand,0,sizeof(szCommand));
    iRet = IF_GetAttr(uiIfindex, IF_NAME, szCommand,sizeof(szCommand));
    if(IF_OK != iRet)
    {
        return IF_INIT_ERR;
    }
    iRet = IF_SetAttr(uiIfindex, IF_LINKNAME, (VOID*)szCommand, strlen((CONST CHAR*)szCommand));
    if(IF_OK != iRet)
    {
        return IF_INIT_ERR;
    }
    iRet = IF_SetAttr(uiIfindex, IF_NETNAME, (VOID*)szCommand, strlen((CONST CHAR*)szCommand));
    if(IF_OK != iRet)
    {
        return IF_INIT_ERR;
    }
    memset(szMacaddr,0,sizeof(szMacaddr));
    iRet = getmac_string(szCommand,szMacaddr);
    if(iRet!=IF_OK)
    {
        return IF_INIT_ERR;
    }
    iRet = IF_SetAttr(uiIfindex, IF_MAC, (VOID*)szMacaddr, strlen((CONST CHAR*)szMacaddr));
    if(IF_OK != iRet)
    {
        return IF_INIT_ERR;
    }
    iRet = IF_SetAttr(uiIfindex, IF_WORKMODE, &iWorkMode, sizeof(iWorkMode));
    if(IF_OK != iRet)
    {
        return IF_INIT_ERR;
    }
    return IF_OK;
}
/*****************************************************************************
    Func Name: IF_L3vlanDestroy
                                                                            
*****************************************************************************/

/*STATIC*/ IF_RET_E IF_L3vlanDestroy(IN ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;
    UCHAR    buf[BUF_SIZE128];   
    UCHAR    szCommand[BUF_SIZE128];
    CHAR     szIfname[IFNAMESIZE];
    UINT32   uiVid = 0;
    /*获取接口link设备*/
    
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME, buf, sizeof(buf));
    if(IF_OK!=iRet)
    { /*netdevice没有生成,仅仅返回*/
        return IF_OK;
    }
    sprintf((CHAR*)szCommand,"vconfig rem %s",buf);  
    system((CONST CHAR*)szCommand);
    memset(szIfname,0,sizeof(szIfname));
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_NAME,szIfname,sizeof(szIfname)))
    {
        return IF_ERR_GETATTR;
    }
    sscanf(szIfname,IF_L3VLAN_NAMEPREFIX"%u",&uiVid);
    Drv_VlanIfDel((vlan_id_t)uiVid);
    /*发送删除事件.其中对应配置的删除,在事件处理之中删除?????*/
    return IF_OK;    
}
/*****************************************************************************
    Func Name: IF_L3Disable
                                                                            
*****************************************************************************/

IF_RET_E IF_L3Disable(IN ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;
    UCHAR    buf[BUF_SIZE128];   
   // UCHAR    szCommand[BUF_SIZE128];
    IF_ENABLE_E uiEnable=IF_MOD_DISABLE;
    /*获取接口link设备*/
    
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME, buf, sizeof(buf));
    if(IF_OK!=iRet)
    {
        return IF_ERR_LINKNAME;
    }
    /*down device*/
    iRet=setdown((CHAR*)buf);
    if(0!=iRet)
    {
        return IF_ERR_ENABLE;
    }
    /*更新enable*/
    iRet=IF_SetAttr(uiIfindex, IF_ATTRENABLE, &uiEnable, sizeof(uiEnable));
    if(iRet!=IF_OK)
    {
        return IF_ERR_ENABLE;
    } 
    return IF_OK;
}


#if 0

/*****************************************************************************
    Func Name: IF_L3IfMonitor
 Date Created: 2011/9/26
       Author: feihuaxin
  Description: 虚接口状态轮询
        Input: VOID
       Output: 
       Return: IF_RET_E  
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
IF_RET_E IF_L3IfMonitor(VOID)
{
	INT iRet = 0;
	ifindex_t uiIfindex = 0;
	UINT32 uiVid = 0;
    UINT32 uiStatus=0;
	IF_STATUS_E logstate = MPST_MAX; 
    CHAR szBuf[BUF_SIZE128]={0};   
    //CHAR szName[IFNAMESIZE];
    CHAR szIfname[IFNAMESIZE];
    CHAR szPseduname[BUF_SIZE128];
    INT iQueId = -1;
    STA_EV_INFO_S stEvInfo;

    /*创建二层事件发送队列*/
    if(IF_OK != IF_StaEv_CreateSndQue(&iQueId))
    {
        //IFM_DBG_ERR("L3if create event queue error!\n");
    }
    
	iRet = IF_GetFirstIfindex(IF_SUB_L3VLAN,&uiIfindex);
	while(IF_OK == iRet)
	{
       (VOID)CFG_Get(uiIfindex,CFG_IF_ENABLE,szBuf,sizeof(szBuf));        
        if(strcmp(szBuf,ENUMTOSTR(IF_MOD_ENABLE))==0)
        {
    		memset(szIfname,0,sizeof(szIfname));
    		if(IF_OK!=IF_GetAttr(uiIfindex,IF_NAME,szIfname,sizeof(szIfname)))
            {
                iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);
    		    continue; 
            }
            sscanf(szIfname,IF_L3VLAN_NAMEPREFIX"%u",&uiVid);
    		Drv_GetVlanStatus((vlan_id_t)uiVid, &uiStatus);
    		if(UP == uiStatus)
    		{
    			/*查看虚接口状态*/
    			IF_GetAttr(uiIfindex, IF_STATE, &logstate, sizeof(IF_STATUS_E));
    			/*虚接口UP，不做处理*/
    			if(IF_UP(logstate))
    		    {
    	    		iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);
    				continue; 
    			}

                /*直接先设置端口状态，防止重复打印*/
                logstate = MPST_NOIPGET;
                IF_SetAttr(uiIfindex, IF_STATE, &logstate, sizeof(logstate));

                memset(szPseduname,0,sizeof(szPseduname));
                IF_GetCliName(uiIfindex,szPseduname,sizeof(szPseduname));
                IC_SendTrap(LOGM_IFNET, LOG_NOTICE, "%s %s%s", 
                "LINK_UPDOWN: OID:1.3.6.1.6.3.1.1.5.3,"
                ,szPseduname,
                " is UP, ifAdminStatus is 1, ifOperStatus is 1.\n");     
    			/*虚接口enable,非UP状态，发送UP事件*/

                if(-1 != iQueId)
                {
                    memset(&stEvInfo, 0x00, sizeof(STA_EV_INFO_S));
                    stEvInfo.enState = STA_EVENT_L3IF_UP;
                    IF_StaEv_Write(iQueId, uiIfindex, STA_EVENT_L3IF_UP, &stEvInfo); 
                }
               	
    		}
    		else
    		{
    			/*查看虚接口状态*/
    			(VOID)IF_GetAttr(uiIfindex, IF_STATE, &logstate, sizeof(IF_STATUS_E));
    			/*虚接口DOWN，不做处理*/
    			if(!IF_UP(logstate))
    		    {
    				iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);
    				continue; 
    			}

                /*直接先设置端口状态，防止重复打印*/
                logstate = MPST_PHYLINKDOWN;
                IF_SetAttr(uiIfindex, IF_STATE, &logstate, sizeof(logstate));

                /*添加LOG信息*/ 
                memset(szPseduname,0,sizeof(szPseduname));
                IF_GetCliName(uiIfindex,szPseduname,sizeof(szPseduname));
                IC_SendTrap(LOGM_IFNET, LOG_NOTICE, "%s %s%s", 
                "LINK_UPDOWN: OID:1.3.6.1.6.3.1.1.5.4,"
                ,szPseduname,
                " is DOWN, ifAdminStatus is 2, ifOperStatus is 2.\n");             
    			/*虚接口UP，发送DOWN事件*/
                if(-1 != iQueId)
                {
                    memset(&stEvInfo, 0x00, sizeof(STA_EV_INFO_S));
                    stEvInfo.enState = STA_EVENT_L3IF_DOWN;
                    IF_StaEv_Write(iQueId, uiIfindex, STA_EVENT_L3IF_DOWN, &stEvInfo); 
                }
                      
    		}
        }
        iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);        
	}

    if(IF_OK != IF_StaEv_DeleteQue(iQueId))
    {
        //IFM_DBG_ERR("L3if delete event queue error!\n");
    }
    
	return IF_OK;
}
#endif
#ifdef  __cplusplus
}
#endif


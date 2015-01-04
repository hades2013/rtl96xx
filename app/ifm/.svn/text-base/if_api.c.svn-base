/*****************************************************************************
                                                                              
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <stdio.h>
#include <string.h>
#include <lw_type.h>
#include "lw_if_pub.h"
#include "lw_if_ethport.h"
#include "lw_if_api.h"
#include "lw_if_type.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "mw_msg.h"
#include "lw_config_api.h" 
#include "lw_config_oid.h" 
#include "if_notifier.h" 

//#define IF_L3VLAN_NAMEPREFIX "l3vlan"

extern IF_RET_E IF_L3Enable(IN ifindex_t uiIfindex);
extern IF_RET_E IF_L3Disable(IN ifindex_t uiIfindex);
extern IF_RET_E IF_L3vlanDestroy(IN ifindex_t uiIfindex);
extern IF_RET_E IF_L3vlanInit(IN ifindex_t uiIfindex);
//extern IF_RET_E IF_EthPortInit(ifindex_t uiIfindex);
extern IF_RET_E IF_EthInit(IN ifindex_t uiIfindex);


//extern INT MP_CFG_Set(IN ifindex_t ifindex, IN UINT uiCmoid,IN VOID * pVal);
extern int master_cfg_setval(int ifindex,unsigned int oid,void *val);

/*****************************************************************************
                                                                            
*****************************************************************************/
IF_RET_E IF_InitInterface(IN ifindex_t uiIfindex)
{
   // SYS_SETUP_STATE_E enState; 
    /*检查ifindex是否合法*/
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }
    
    /*检查接口是否存在*/
    if(IF_OK != IF_TestIfExist(uiIfindex))
    {
        return IF_NFOUND;
    }
   /*如果当前状态是SYS_SETUP_CFGINIT返回*/    
    switch (IF_SUB_TYPE(uiIfindex))
    {
        case IF_SUB_ETH:
            return IF_EthInit(uiIfindex);
        case IF_SUB_ETHPORT:           
            return IF_EthPortInit(uiIfindex);
        case IF_SUB_L3VLAN:
            return IF_L3vlanInit(uiIfindex);          
        default:           
           // return IF_OK;
           break;
    }
    
    return IF_OK;
}
extern int setip( unsigned char *ifname, unsigned int ip ) ;          
extern int setnetmask(unsigned char *ifname, unsigned int netmask );

IF_RET_E IF_SetIpMask(IN ifindex_t uiIfindex,IN CHAR *pIpstr,IN CHAR *pMskstr)
{
    UINT ip=0;
    UINT mask=0;
    struct in_addr stIp,stMask;
    INT iRet=0;
    CHAR buf[BUF_SIZE128];
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }
    if(pIpstr==NULL || pMskstr==NULL)
    {
        return IF_ERR_PARAM;
    }
    /*获取link device*/
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME,buf, sizeof(buf));
    if(IF_OK!=iRet)
    {
        return IF_ERR_LINKNAME;
    }
    inet_aton(pIpstr,&stIp);
    inet_aton(pMskstr,&stMask);
    ip=stIp.s_addr;
    mask=stMask.s_addr;
    /*设置ip*/
    iRet=setip((UCHAR*)buf,ip);
    if(iRet!=0)
    {
        return IF_SETIP_ERR;
    }
    /*设置mask*/
    if(0 != strcmp((const CHAR *)pIpstr,"0.0.0.0"))
    {
        iRet=setnetmask((UCHAR*)buf,mask);
        if(iRet!=0)
        {    
            return IF_SETMASK_ERR;
        }
    }
    iRet=IF_SetAttr(uiIfindex, IF_IP, &ip, sizeof(ip));
    if(iRet!=IF_OK)
    {
        return IF_SETIP_ERR;
    }
    iRet=IF_SetAttr(uiIfindex, IF_MASK, &mask, sizeof(mask));
    if(iRet!=IF_OK)
    {
        return IF_SETMASK_ERR;
    }
    /*保存cfg*/
    if(IF_GET_FLAG(uiIfindex)!=IF_NO_COMMIT_FLAG)
    {
       
        iRet=master_cfg_setval(uiIfindex,CONFIG_IP_ADDR, (UCHAR*)pIpstr);
        if(iRet!=CFG_OK)
        {
            return IF_SETIP_ERR;
        }
        iRet=master_cfg_setval(uiIfindex,CONFIG_IP_MASK, (UCHAR*)pMskstr);
        if(iRet!=CFG_OK)
        {
            return IF_SETMASK_ERR;
        }        
    }
    return IF_OK;
}
IF_RET_E IF_SetGateWay(IN ifindex_t uiIfindex,IN CHAR *pGwstr)
{
    UINT gw=0;
    struct in_addr stGw;
    INT iRet=0;
    CHAR buf[BUF_SIZE128];
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }
    if(pGwstr==NULL)
    {
        return IF_ERR_PARAM;
    }
    /*获取link device*/
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME,buf, sizeof(buf));
    if(IF_OK!=iRet)
    {
        return IF_ERR_LINKNAME;
    }
    /*获取old gw*/
    iRet=IF_GetAttr(uiIfindex, IF_GATEWAY,&gw, sizeof(gw));
    if(IF_OK!=iRet)
    {
        return IF_ERR_GATEWAY;
    }
    /*删除old default route*/
    if(gw!=0)
    {
        iRet=del_default_rt(buf,gw);
        if(0!=iRet)
        {
            return IF_ERR_GATEWAY;
        }
    }
    inet_aton(pGwstr,&stGw);
    gw=stGw.s_addr;
     /*设置新的default route*/
    if(0!=strcmp(IP_ZERO,pGwstr))
    {
        iRet=set_default_rt(buf,gw);
        if(0!=iRet)
        {
            return IF_ERR_GATEWAY;
        }
    }
    /*更新接口gw*/
    iRet=IF_SetAttr(uiIfindex, IF_GATEWAY, &gw, sizeof(gw));
    if(iRet!=IF_OK)
    {
        return IF_ERR_GATEWAY;
    }
     /*保存cfg*/
    if(IF_GET_FLAG(uiIfindex)!=IF_NO_COMMIT_FLAG)
    {      
        {
            iRet=master_cfg_setval(uiIfindex, CONFIG_IP_GATEWAY, (UCHAR*)pGwstr);
            if(iRet!=CFG_OK)
            {
                return IF_ERR_GATEWAY;
            }        
        }
    }
    return IF_OK;
}
IF_RET_E IF_SetMtu(IN ifindex_t uiIfindex,IN UINT uiMtu)
{
    INT iRet=0;
    CHAR buf[BUF_SIZE128];
    
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }    
    /*获取link device*/
    iRet=IF_GetAttr(uiIfindex, IF_LINKNAME,buf, sizeof(buf));
    if(IF_OK!=iRet)
    {
        return IF_ERR_LINKNAME;
    }
    iRet=set_mtu(buf,(INT)uiMtu);
    if(0!=iRet)
    {
        return IF_ERR_MTU;
    }
      /*更新接口gw*/
    iRet=IF_SetAttr(uiIfindex, IF_MTU, &uiMtu, sizeof(uiMtu));
    if(IF_OK!=iRet)
    {
        return IF_ERR_MTU;
    }  
      /*保存cfg*/
    memset(buf,0,sizeof(buf));
    sprintf(buf,"%u",uiMtu);
    if(IF_GET_FLAG(uiIfindex)!=IF_NO_COMMIT_FLAG)
    {   
        iRet=master_cfg_setval(uiIfindex, CONFIG_IP_MTU, (UCHAR*)buf);
        if(iRet!=CFG_OK)
        {
            return IF_ERR_GATEWAY;
        }         
    }
    return IF_OK;
}
IF_RET_E IF_Enable(IN ifindex_t uiIfindex)
{
    IF_RET_E iRet=IF_OK;
    IF_ENABLE_E enEnable=IF_MOD_DISABLE;
    SYS_SETUP_STATE_E enState;
     /*检查ifindex是否合法*/
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    } 
    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_INIT_ERR;
    }
    if(enState < SYS_SETUP_IFINIT)
    {
        return IF_OK;
    }
    if(IF_DOMAIN(uiIfindex)!=IF_DOMAIN_NORMAL)
    {
        return IF_OK;
    }

    /*检查接口状态*/
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_ATTRENABLE,&enEnable, sizeof(IF_ENABLE_E)))
    {    
         return IF_ERR_GETATTR;
    }
    if(IF_MOD_ENABLE == enEnable )
    {
        return IF_OK;
    }   
    switch (IF_SUB_TYPE(uiIfindex))
    {
        case IF_SUB_ETHPORT:
            break;
        case IF_SUB_L3VLAN:
            iRet=IF_L3Enable(uiIfindex);
            break;        
        default:           
           break;
    }
    if(IF_OK!=ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTFENABLE, NULL))  
    {          
        return (IF_RET_E)IF_NOTIFY_ERR;   
    }
    return iRet;
}
IF_RET_E IF_GetCliName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize)
{
    UINT uiIfid=0;
    CHAR szName[IFNAMESIZE];
    CHAR szPsedoame[BUF_SIZE128];
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    } 
    if(uiSize<IFNAMESIZE)
    {
        return IF_ERR_PARAM;
    }
    memset(szName,0,sizeof(szName));
    memset(szPsedoame,0,sizeof(szPsedoame));
    if(IF_OK!=IF_GetAttr(uiIfindex, IF_NAME, szName, sizeof(szName)))
    {
        return IF_ERR_PANIC;
    }
    switch(IF_SUB_TYPE(uiIfindex))
    {
        case IF_SUB_L3VLAN:
        {
            sscanf(szName,IF_L3VLAN_NAMEPREFIX"%u",&uiIfid);
            sprintf(szPsedoame,"Vlan-interface%u",uiIfid);
            if(strlen(szPsedoame)+1>uiSize)
            {
                return IF_ERR_PARAM;
            }
            strcpy(pszName,szPsedoame);
            break;
        }
		case IF_SUB_ETHPORT:
        {
            sscanf(szName,IF_ETHPORT_NAMEPREFIX"%u",&uiIfid);
			if(PT_TP((PORT_TYPE((port_num_t)uiIfid)))==TP_CABLE)
			{
				//printf("WEB_CABLEPORTPREFIX %d",(uiIfid-FE_PORT_NO-GE_PORT_NO));
				sprintf(szPsedoame,"%s%u",CLI_CABLE,(uiIfid-FE_PORT_NO-GE_PORT_NO));
			}
			else
			{
				if(PT_SP((PORT_TYPE((port_num_t)uiIfid)))==SP_1000)
	            sprintf(szPsedoame,"%s%u",CLI_GIGAPORTPREFIX,uiIfid-FE_PORT_NO);
				else
				sprintf(szPsedoame,"%s%u",CLI_FASTPORTPREFIX,uiIfid);	
			}
            if(strlen(szPsedoame)+1>uiSize)
            {
                return IF_ERR_PARAM;
            }
            strcpy(pszName,szPsedoame);
            break;
        }
        default:
        {
            strcpy(pszName,szName);
            break;
        }           
    }
    return IF_OK;
}
IF_RET_E IF_Disable(IN ifindex_t uiIfindex)
{
    IF_RET_E iRet=IF_OK;
    IF_ENABLE_E enEnable=IF_MOD_DISABLE;
    SYS_SETUP_STATE_E enState;
    IF_STATUS_E enIfState=IFM_INIT; 
   // CHAR szName[IFNAMESIZE];
    CHAR szPseduname[BUF_SIZE128];
     /*检查ifindex是否合法*/
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    } 
    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_INIT_ERR;
    }
    if(enState < SYS_SETUP_IFINIT)
    {
        return IF_OK;
    }
    if(IF_DOMAIN(uiIfindex)!=IF_DOMAIN_NORMAL)
    {
        return IF_OK;
    }
    /*检查接口状态*/
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_ATTRENABLE,&enEnable, sizeof(IF_ENABLE_E)))
    { 
        return IF_ERR_GETATTR;
    }    
    if(IF_MOD_DISABLE==enEnable)
    {
        return IF_OK;
    } 
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_STATE,&enIfState, sizeof(IF_STATUS_E)))
    { 
        return IF_ERR_GETATTR;
    }    
    if(enIfState!=IFM_PHYLINKDOWN)
    {
        if(IF_OK!=ifm_call_notifiers(uiIfindex, IF_NOTIFY_PHYDOWN, NULL)) 
        {
            return IF_ERR_PANIC;
        }

        memset(szPseduname,0,sizeof(szPseduname));  
        IF_GetCliName(uiIfindex,szPseduname,sizeof(szPseduname));   
		printf("%s is down.. \n",szPseduname);
    } 
    if(IF_OK!=ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTFDISABLE, NULL)) 
    {      
        //IFM_DBG_ERR("Raise event IF_EVENT_INTFDISABLE failed.\n");   
        return (IF_RET_E)IF_NOTIFY_ERR;   
    }
    switch (IF_SUB_TYPE(uiIfindex))
    {  
        case IF_SUB_ETH:
        case IF_SUB_L3VLAN:
            iRet=IF_L3Disable(uiIfindex);
            break;      
        default:           
           break;
    }
    return iRet;
}
IF_RET_E IF_Destroy(IN ifindex_t uiIfindex)
{
    IF_RET_E iRet=IF_OK;
    SYS_SETUP_STATE_E enState;
    IF_ENABLE_E enEnable=IF_MOD_DISABLE;
    
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    }   
    if(IF_OK!=IF_GetSysState(&enState))
    {
        return IF_INIT_ERR;
    }
    if(enState < SYS_SETUP_IFINIT)
    {
        if(ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTF_ALREADYREM, NULL))  
        {      
            return (IF_RET_E)IF_NOTIFY_ERR;   
        }
        return IF_OK;
    }
    if(IF_DOMAIN(uiIfindex)!=IF_DOMAIN_NORMAL)
    {
        if(ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTF_ALREADYREM, NULL))  
        {      
            return (IF_RET_E)IF_NOTIFY_ERR;   
        }
        return IF_OK;
    }   
  
    if(IF_OK!=IF_GetAttr(uiIfindex,IF_ATTRENABLE,&enEnable, sizeof(IF_ENABLE_E)))
    { 
        return IF_ERR_GETATTR;
    }
    if(IF_MOD_ENABLE == enEnable )
    {
        iRet=IF_Disable(uiIfindex);
        if(IF_OK!=iRet)
        {
            return iRet;
        }
    }
   
    if(ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTFREM, NULL))  
    {      
        //IFM_DBG_ERR("Raise event IF_EVENT_INTFREM failed.\n");   
        return (IF_RET_E)IF_NOTIFY_ERR;   
    }
    switch (IF_SUB_TYPE(uiIfindex))
    {  
        case IF_SUB_L3VLAN:
            iRet=IF_L3vlanDestroy(uiIfindex);
            break;            
        default:           
           break;
    }
    if(ifm_call_notifiers(uiIfindex, IF_NOTIFY_INTF_ALREADYREM, NULL))  
    {         
        return (IF_RET_E)IF_NOTIFY_ERR;   
    }
    return iRet;
}

IF_RET_E IF_GetWebName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize)
{
    UINT uiIfid=0;
    CHAR szName[IFNAMESIZE];
    CHAR szPsedoame[BUF_SIZE128];
    if(!VALID_IFINDEX(uiIfindex))
    {
        return IF_INVALID_IDX;
    } 
    if(uiSize<IFNAMESIZE)
    {
        return IF_ERR_PARAM;
    }
    memset(szName,0,sizeof(szName));
    memset(szPsedoame,0,sizeof(szPsedoame));
    if(IF_OK!=IF_GetAttr(uiIfindex, IF_NAME, szName, sizeof(szName)))
    {
        return IF_ERR_PANIC;
    }
    switch(IF_SUB_TYPE(uiIfindex))
    {
        case IF_SUB_L3VLAN:
        {
            sscanf(szName,IF_L3VLAN_NAMEPREFIX"%u",&uiIfid);
            sprintf(szPsedoame,"Vlan-interface%u",uiIfid);
            if(strlen(szPsedoame)+1>uiSize)
            {
                return IF_ERR_PARAM;
            }
            strcpy(pszName,szPsedoame);
            break;
        }
		case IF_SUB_ETHPORT:
        {
            sscanf(szName,IF_ETHPORT_NAMEPREFIX"%u",&uiIfid);
			if(PT_TP((PORT_TYPE((port_num_t)uiIfid)))==TP_CABLE)
			{
				// printf("WEB_CABLEPORTPREFIX %d",(uiIfid-FE_PORT_NO-GE_PORT_NO));
				 sprintf(szPsedoame,"%s%u",WEB_CABLEPORTPREFIX,(uiIfid-FE_PORT_NO-GE_PORT_NO));
			}
			else
			{
				if(PT_SP((PORT_TYPE((port_num_t)uiIfid)))==SP_1000)
	            sprintf(szPsedoame,"%s%u",WEB_GIGAPORTPREFIX,uiIfid-FE_PORT_NO);
				else
				sprintf(szPsedoame,"%s%u",WEB_FASTPORTPREFIX,uiIfid);	
			}
            if(strlen(szPsedoame)+1>uiSize)
            {
                return IF_ERR_PARAM;
            }
            strcpy(pszName,szPsedoame);
            break;
        }
        default:
        {
            strcpy(pszName,szName);
            break;
        }           
    }
    return IF_OK;
}
IF_RET_E IF_Userstr2ifame(INOUT CHAR *str,INOUT CHAR *pszName,IN UINT uiSize)
{
	int port=0;
	char buf[BUF_SIZE1024];
	if(str==NULL||pszName==NULL)
	{
		return IF_ERR_PARAM;
	}
	if(strstr(str,"Ethernet")!=NULL)
	sscanf(str,"Ethernet%d",&port);	
	else if(strstr(str,CLI_GIGAPORTPREFIX)!=NULL)
	{
		sscanf(str,CLI_GIGAPORTPREFIX"%d",&port);
		port+=FE_PORT_NO;
	}
	else if(strstr(str,CLI_FASTPORTPREFIX)!=NULL)
	sscanf(str,CLI_FASTPORTPREFIX"%d",&port);
	else if(strstr(str,WEB_GIGAPORTPREFIX)!=NULL)
	{
		sscanf(str,WEB_GIGAPORTPREFIX"%d",&port);
		port+=FE_PORT_NO;
	}
	else if(strstr(str,WEB_FASTPORTPREFIX)!=NULL)
	sscanf(str,WEB_FASTPORTPREFIX"%d",&port);
	else if(strstr(str,WEB_CABLEPORTPREFIX)!=NULL)
	{
		sscanf(str,WEB_CABLEPORTPREFIX"%d",&port);
		port+=FE_PORT_NO;
		port+=GE_PORT_NO;
		
	}
	else if(strstr(str,CLI_CABLE)!=NULL)
	{
		sscanf(str,CLI_CABLE"%d",&port);
		port+=FE_PORT_NO;
		port+=GE_PORT_NO;
	}    
	if(!VALID_PORT(port))
	{
		return IF_ERR_PARAM;
	}
	sprintf(buf,IF_ETHPORT_NAMEPREFIX"%d",port);
	if(strlen(buf)>=uiSize)
	{
		return IF_ERR_PARAM;
	}
	strcpy(pszName,buf);
	return IF_OK;
}
#ifdef  __cplusplus
}
#endif


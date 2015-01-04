/*****************************************************************************
                                                                              
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>    
#include "lw_if_pub.h"  
#include "lw_if_type.h"
#include "lw_if_ethport.h"
#include "lw_if_api.h"
#include "lw_drv_pub.h" 
#include <string.h>
//#define IF_ETHPORT_NAMEPREFIX "ethport"

#include "lw_config_api.h" 
#include "lw_config_oid.h" 

//extern int cfg_getval(int ifindex,unsigned int oid,void *val,void* default_val,unsigned retlen);
//extern INT master_cfg_setval(IN ifindex_t ifindex, IN UINT uiCmoid,IN VOID * pVal);

IF_RET_E IF_EthPortInit(ifindex_t uiIfindex)
{
    INT iRet=IF_INIT_ERR;
    port_num_t uiPort;
    UCHAR    buf[100]; 
    SYS_SETUP_STATE_E enState; 

   /*配置数据初始化*/
    if(isMaster()!=MASTER)
    {
        return IF_INIT_ERR;
    }
    else
    {
         iRet=cfg_getval(uiIfindex,CONFIG_IF_DESC,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",IF_ETHPORT_NAMEPREFIX);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_DESC, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_DESC error.\n");
                return IF_INIT_ERR;
            }        
        }
        
        iRet=cfg_getval(uiIfindex,CONFIG_IF_TYPE,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",IF_ETHPORT_NAMEPREFIX);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_TYPE,(UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_TYPE error\n");
                return IF_INIT_ERR;
            }        
        }
       iRet=cfg_getval(uiIfindex,CONFIG_IF_NAME,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s%u",IF_ETHPORT_NAMEPREFIX,IF_INDEX(uiIfindex));  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_NAME, (UCHAR*)buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_IF_NAME error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_PORT_SPEED,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(PORT_SPEED_AUTO));   
            iRet=master_cfg_setval(uiIfindex,CONFIG_PORT_SPEED, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_PORT_SPEED error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_PORT_DUPLEX,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
            sprintf((CHAR*)buf,"%s",ENUMTOSTR(PORT_DUPLEX_AUTO));    
            iRet=master_cfg_setval(uiIfindex,CONFIG_PORT_DUPLEX, buf);
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_PORT_DUPLEX error\n");
                return IF_INIT_ERR;
            }        
        }
        iRet=cfg_getval(uiIfindex,CONFIG_IF_ENABLE,(CHAR*)buf,NULL,sizeof(buf));
        if(iRet!=CFG_OK)
        {     
            memset(buf,0,sizeof(buf));
          //  sprintf((CHAR*)buf,"%u",IF_MOD_ENABLE);  
            iRet=master_cfg_setval(uiIfindex,CONFIG_IF_ENABLE, (UCHAR*)ENUMTOSTR(IF_MOD_ENABLE));
            if(iRet!=CFG_OK)
            {
                IFM_DBG_ERR("set CFG_PORT_ENABLE error\n");
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
    
   /*驱动数据初始化*/
    iRet=IF_GetAttr(uiIfindex, IF_PHYID, &uiPort, sizeof(port_num_t));
    if(iRet==IF_OK)
    {
        iRet=Drv_PortInit(uiPort);
        if(DRV_OK == iRet)
        {
            iRet=IF_OK;
        }
        else
        {  
            IFM_DBG_ERR("Drv_PortInit [%d] error\n",uiPort);
            iRet=IF_INIT_ERR;
        }
    }    
    return (IF_RET_E)iRet;
}



    
#ifdef  __cplusplus
}
#endif


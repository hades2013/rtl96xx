#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <timer.h>
#include <if_notifier.h>
#include <stdio.h>
#include <string.h>
#include <systemlog.h>    //add by leijinbao 2013/9/17 for USER_LOG

#define PORT_MON_INTERVAL   1
#define VLANIF_MON_INERVAL   1

static logic_pmask_t stOldUpPMask;

static int timer_handle_port_monitor_check(timer_element_t *timer, void *data)
{
    logic_pmask_t stNewUpPMask;
    port_num_t lport = 0;
    ifindex_t ifindex;
    Drv_GetLinkUpPorts(&stNewUpPMask);
    unsigned char szifname[BUF_SIZE1024]; 
	/*begin modified by wanghuanyu for 129*/
	unsigned char szPseduname[BUF_SIZE1024]; 
    //printf("mask=%02x\n",stNewUpPMask);
    LgcPortFor(lport)
    {

        
        sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
        IF_GetByIFName(szifname,&ifindex);
		 IF_GetCliName(ifindex,szPseduname,sizeof(szPseduname)); 
        if((TRUE == TstLgcMaskBit(lport, &stOldUpPMask))&&(FALSE == TstLgcMaskBit(lport, &stNewUpPMask)))
        {
            printf("%s is down \n",szPseduname);
			USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_LINKDOWN,szPseduname,"");  //add by leijinbao 2013/09/17 for log
            ifm_call_notifiers(ifindex,IF_NOTIFY_PHYDOWN,NULL);
            ClrLgcMaskBit(lport,&stOldUpPMask);
        }
        if((FALSE == TstLgcMaskBit(lport, &stOldUpPMask))&&(TRUE == TstLgcMaskBit(lport, &stNewUpPMask)))
        {
             printf("%s is up \n",szPseduname);
			 USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_LINKUP,szPseduname,"");  //add by leijinbao 2013/09/17 for log
             ifm_call_notifiers(ifindex,IF_NOTIFY_PHYUP,NULL);
             SetLgcMaskBit(lport,&stOldUpPMask);
        }
    }
		/*end modified by wanghuanyu for 129*/
    return 0;
}
static int timer_handle_vlanif_monitor_check(timer_element_t *timer, void *data)
{
    INT iRet = 0;
	ifindex_t uiIfindex = 0;
	UINT32 uiVid = 0;
    UINT32 uiStatus=0;
	IF_STATUS_E logstate = IFM_MAX; 
    CHAR szBuf[BUF_SIZE128]={0};   
    //CHAR szName[IFNAMESIZE];
    CHAR szIfname[IFNAMESIZE];
    CHAR szPseduname[BUF_SIZE128];
    
    
	iRet = IF_GetFirstIfindex(IF_SUB_L3VLAN,&uiIfindex);
	while(IF_OK == iRet)
	{
       (VOID)cfg_getval(uiIfindex,CONFIG_IF_ENABLE,szBuf,NULL,sizeof(szBuf));        
        if(strcmp(szBuf,ENUMTOSTR(IF_MOD_ENABLE))==0)
        {
    		memset(szIfname,0,sizeof(szIfname));
    		if(IF_OK!=IF_GetAttr(uiIfindex,IF_NAME,szIfname,sizeof(szIfname)))
            {
                iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);
    		    continue; 
            }
            sscanf(szIfname,IF_L3VLAN_NAMEPREFIX"%u",&uiVid);
			
			#ifdef ONU_STYLE
    		Drv_GetVlanStatus((vlan_id_t)0, &uiStatus);
			#else
		    Drv_GetVlanStatus((vlan_id_t)uiVid, &uiStatus);	
			#endif
			//printf("%s is %d.. \n",szPseduname,uiStatus);
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
                logstate = IFM_NOIPGET;
                IF_SetAttr(uiIfindex, IF_STATE, &logstate, sizeof(logstate));

                memset(szPseduname,0,sizeof(szPseduname));
                IF_GetCliName(uiIfindex,szPseduname,sizeof(szPseduname));
                ifm_call_notifiers(uiIfindex,IF_NOTIFY_PHYUP,NULL);
				printf("%s is up.. \n",szPseduname);
				USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_LINKUP,szPseduname,"");  //add by leijinbao 2013/09/17 for log
               	
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
                logstate = IFM_PHYLINKDOWN;
                IF_SetAttr(uiIfindex, IF_STATE, &logstate, sizeof(logstate));

                /*添加LOG信息*/ 
                memset(szPseduname,0,sizeof(szPseduname));
                IF_GetCliName(uiIfindex,szPseduname,sizeof(szPseduname));
				
                ifm_call_notifiers(uiIfindex,IF_NOTIFY_PHYDOWN,NULL);
				printf("%s is down.. \n",szPseduname);
				USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_LINKDOWN,szPseduname,"");  //add by leijinbao 2013/09/17 for log
                      
    		}
        }
        iRet = IF_GetNextIfindex(IF_SUB_L3VLAN,&uiIfindex);        
	}
    
	return 0;
}

int sys_ifm_mon_start(void)
{
    ClrLgcMaskAll(&stOldUpPMask);
    /*scan each ethport  up or down*/
    timer_register(PORT_MON_INTERVAL, 1, (timer_func_t)timer_handle_port_monitor_check, NULL, NULL, "port monitor");
    timer_register(PORT_MON_INTERVAL, 1, (timer_func_t)timer_handle_vlanif_monitor_check, NULL, NULL, "port monitor");
    return 0;
}
void sys_tmpif_init(void)
{
	IF_INFO_S stInfo;
    IF_RET_E ret;
    ifindex_t ifindex;
    
    memset(&stInfo,0,sizeof(IF_INFO_S));
   
    strcpy(stInfo.szIfName,IF_TMP_NAME);
    strcpy(stInfo.szPseudoName,IF_TMP_NAME);
    stInfo.enSubType=IF_SUB_ROOT;
    stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_NORMAL,0);
    stInfo.uiSpecIndex=IF_TEMP_IFINDEX;
    
    ret=IF_CreatInterface(&stInfo,&ifindex);
    if(ret!=IF_OK)
    {
        printf("fatal error ,create tmproot if failed %d\n",ret);
    }
	return ;
}
extern void network_init(void);

void sys_ifm_init(void)
{
    sys_tmpif_init();  
	IF_SetSysState(SYS_SETUP_CFGINIT);
	IF_ExistInterfaceInit();	
		
    return ;
}
void sys_ifm_finish(void)
{
 
    IF_SetSysState(SYS_SETUP_IFINIT);
	IF_ExistInterfaceInit();
	IF_SetSysState(SYS_SETUP_FINISH);
    return ;
}

extern void ifm_state_init(void);
extern void  master_ifcfg_init(void);

void sys_notifier_init(void)
{
    ifm_state_init();
	network_init();
	master_ifcfg_init();
	
    return ;
}


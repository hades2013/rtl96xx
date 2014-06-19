#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
#include <shutils.h>
#include <str_utils.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time_zone.h>
#include "utils.h"
#include "sys_upgrade.h"
#include <drv_wtd_user.h>
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include <if_notifier.h>
#include "lw_drv_req.h"

#define IF_ETHPORT_IFINDEX  IF_IFINDEX(IF_SUB_ETHPORT,0)


int get_spdlx_by_new_speed(PortSpeedValue_E enPortSpeed, int* spdlx_val)
{
    int duplex_tmp;

    duplex_tmp = (*spdlx_val % 3);

    switch (enPortSpeed)
    {
        case PORT_SPEED_10 :
            *spdlx_val = (0*3) + duplex_tmp;
            break;
        case PORT_SPEED_100 :
            *spdlx_val = (1*3) + duplex_tmp;
            break;
        case PORT_SPEED_1000 :
            *spdlx_val = (2*3) + duplex_tmp;
            break;
        case PORT_SPEED_AUTO :
            *spdlx_val = (3*3) + duplex_tmp;
            break;
        default:
            return -1;
    }

    return 0;
}

int get_spdlx_by_new_duplex(PortDuplexValue_E enPortDuplex, int* spdlx_val)
{
    int speed_tmp;

    speed_tmp = (*spdlx_val / 3);

    switch (enPortDuplex)
    {
        case PORT_DUPLEX_AUTO :
            *spdlx_val = (speed_tmp*3) + 2;
            break;
        case PORT_DUPLEX_FULL :
            *spdlx_val = (speed_tmp*3) + 1;
            break;
        case PORT_DUPLEX_HALF :
            *spdlx_val = (speed_tmp*3) + 0;
            break;
        default:
            return -1;
    }

    return 0;

}

INT32 SetPortSpeed(UINT32 lport, PortSpeedValue_E enPortSpeed)
{
    int spdlx_val;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }

    if(DRV_OK != Ioctl_GetSpdlxSet(lport, &spdlx_val)) {
        return ERROR;
    }

    if(-1 == get_spdlx_by_new_speed(enPortSpeed, &spdlx_val)) {
        return ERROR;
    }

    if(DRV_OK == Ioctl_SetPortSpdlx(lport, spdlx_val)){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }

}

INT32 SetPortDuplex(UINT32 lport, PortDuplexValue_E enPortDuplex)
{
    int spdlx_val;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }

    if(DRV_OK != Ioctl_GetSpdlxSet(lport, &spdlx_val)) {
        return ERROR;
    }

    if(-1 == get_spdlx_by_new_duplex(enPortDuplex, &spdlx_val)) {
        return ERROR;
    }

    if(DRV_OK == Ioctl_SetPortSpdlx(lport, spdlx_val)){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
}


void sys_port_load(sys_port_t *sys)
{
    port_num_t lport = 0;
    int arryidx  = 0;    
    char val[BUF_SIZE32] = {0};
    int ifindex = 0;
    char szifname[BUF_SIZE32] = {0};
    
    LgcPortFor(lport)
    { 
         arryidx = lport - 1;

		 memset(val, 0, sizeof(val));
		 memset(szifname, 0, sizeof(szifname));
         sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
         IF_GetByIFName(szifname,&ifindex);
        if((cfg_getval(ifindex, CONFIG_ETHPORT_DESCRIPTION, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT description faild");
            return;
        }
        strncpy(sys->port_cfg_val[arryidx].stIfDesc, val, MAX_PORT_DESC_LEN);
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_ENABLED, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT enable  faild");
            return;
        }

        if(NULL != strstr(val, "enable"))
        {
            sys->port_cfg_val[arryidx].ulIfEnable = 1;
        }
        else
        {
            sys->port_cfg_val[arryidx].ulIfEnable = 0;
        }
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_SPD, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT speed  faild");
            return;
        }

        if(!strcmp("10", val))
        {
            sys->port_cfg_val[arryidx].stSpeed = 0;
        }
        else if (!strcmp("100", val))
        {
            sys->port_cfg_val[arryidx].stSpeed = 1;
        }
        else if (!strcmp("1000", val))
        {
            sys->port_cfg_val[arryidx].stSpeed = 2;
        }
        else
        {   
            /*auto */
            sys->port_cfg_val[arryidx].stSpeed = 3;
        }
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_DUP, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT duplex  faild");
            return;
        }

        if(!strcmp("half", val))
        {
            sys->port_cfg_val[arryidx].stDuplex = 0;
        }
        else if (!strcmp("full", val))
        {
            sys->port_cfg_val[arryidx].stDuplex = 1;
        }
        else
        {
            sys->port_cfg_val[arryidx].stDuplex = 2;
        }
   
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_PRI, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT priority faild");
            return;
        }
        sys->port_cfg_val[arryidx].ulPriority = strtoul(val, NULL, 0);
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_FC, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT flowcontrl  faild");
            return;
        }

        if(NULL != strstr(val, "enable"))
        {
            sys->port_cfg_val[arryidx].ulFlowCtl = 1;
        }
        else
        {
            sys->port_cfg_val[arryidx].ulFlowCtl = 0;
        }

        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_MDIX, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT MDIX  faild");
            return;
        }

        if(NULL != strstr(val, "across"))
        {
            sys->port_cfg_val[arryidx].ulMdix = 0;
        }
        else if(NULL != strstr(val, "normal"))
        {
            sys->port_cfg_val[arryidx].ulMdix = 2;
        }
        else
        {   
            /*auto */   
            sys->port_cfg_val[arryidx].ulMdix = 1;
        }

        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_ISOLATE, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT isolate  faild");
            return;
        }

        if(NULL != strstr(val, "enable"))
        {
            sys->port_cfg_val[arryidx].ulIsolateEn = 1;
        }
        else
        {
            sys->port_cfg_val[arryidx].ulIsolateEn = 0;
        }
        
    }
    #if 0
    //for test feihuaxin
    LgcPortFor(lport)
    {  
        arryidx = lport - 1;
        sys->port_cfg_val[arryidx].stDuplex = 1;
        sys->port_cfg_val[arryidx].stSpeed = 1;
        sys->port_cfg_val[arryidx].ulIfEnable = 1;
        sys->port_cfg_val[arryidx].ulFlowCtl = 1;
        sys->port_cfg_val[arryidx].ulPriority = 1;
        sys->port_cfg_val[arryidx].ulMdix = 0; //auto
        memcpy(sys->port_cfg_val[arryidx].stIfDesc, "none", 10);
    }
    #endif
    return;
}
void sys_port_apply(void)
{
	port_num_t lport = 0;
    char arryidx = 0;
	logic_pmask_t lPortMask;

	LgcPortFor(lport)
	{  
		arryidx = lport - 1;
		Ioctl_SetPortEnable(lport, sys_port.port_cfg_val[arryidx].ulIfEnable);
		SetPortDuplex(lport, sys_port.port_cfg_val[arryidx].stDuplex);
		SetPortSpeed(lport, sys_port.port_cfg_val[arryidx].stSpeed);
		Ioctl_SetPortPause(lport, sys_port.port_cfg_val[arryidx].ulFlowCtl, sys_port.port_cfg_val[arryidx].ulFlowCtl);
		Ioctl_SetPortMdix(lport, sys_port.port_cfg_val[arryidx].ulMdix);
		Ioctl_SetPortPriority(lport,sys_port.port_cfg_val[arryidx].ulPriority);
	
		if(sys_port.port_cfg_val[arryidx].ulIsolateEn)
		{
			SetLgcMaskBit(lport,&lPortMask);
		}
	}
	Ioctl_SetPortIsolate(lPortMask);

}


INT sys_port_update(PORT_CFG_VALUE_S* port_cfg,long long * updatPortMsk)
{
	sys_qos_t *sys = &sys_port;
	int update = 0;
    int i = 0;

    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if((sys_port.port_cfg_val[i].stSpeed != port_cfg[i].stSpeed ) || 
            (sys_port.port_cfg_val[i].stDuplex != port_cfg[i].stDuplex ) ||
            (sys_port.port_cfg_val[i].ulIfEnable != port_cfg[i].ulIfEnable ) ||
            (sys_port.port_cfg_val[i].ulFlowCtl != port_cfg[i].ulFlowCtl ))
        {
            *updatPortMsk += 1<<i;
        }
    }

    memcpy(sys_port.port_cfg_val, port_cfg, sizeof(PORT_CFG_VALUE_S)*LOGIC_PORT_NO);
   
    update ++;
    set_update(port);
    
	return update;
}

INT sys_port_commit(unsigned char apply_options)
{
    char val[30] = {0};
    sys_port_t *sys = &sys_port;
    port_num_t lport = 0;
    int arryidx  = 0; 
	ifindex_t ifindex = 0;
	char szifname[BUF_SIZE_64] = {0};

    if (test_update(port))
    {
        LgcPortFor(lport)
        {
            arryidx = lport - 1;
			memset(szifname, 0, sizeof(szifname));
			memset(szifname, 0, sizeof(szifname));
			sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
			if(0 != IF_GetByIFName(szifname,&ifindex))
			{
				return -1;  
			}
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_DESCRIPTION, sys->port_cfg_val[arryidx].stIfDesc))
            {
                return -1;  
            }
            
            memset(val, 0, sizeof(val));
            sprintf(val, "%s", sys->port_cfg_val[arryidx].ulIfEnable ? "enable" : "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_ENABLED, val))
            {
                return -1;  
            }
           
            memset(val, 0, sizeof(val));
            if(0 == sys->port_cfg_val[arryidx].stSpeed)
            {
                sprintf(val, "%s", "10");
            }
            else if (1 == sys->port_cfg_val[arryidx].stSpeed)
            {
                sprintf(val, "%s", "100");
            }
            else if (2 == sys->port_cfg_val[arryidx].stSpeed)
            {
                sprintf(val, "%s", "1000");
            }
            else
            {
                sprintf(val, "%s", "auto");
            }
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_SPD, val))
            {
                return -1;  
            }

            
            memset(val, 0, sizeof(val));
            if(0 == sys->port_cfg_val[arryidx].stDuplex)
            {
                sprintf(val, "%s", "half");
            }
            else if (1 == sys->port_cfg_val[arryidx].stDuplex)
            {
                sprintf(val, "%s", "full");
            }
            else
            {
                sprintf(val, "%s", "auto");
            }
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_DUP, val))
            {
                return -1;  
            }

            memset(val, 0, sizeof(val));
            sprintf(val, "%d", sys->port_cfg_val[arryidx].ulPriority);
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_PRI, val))
            {
                return -1;  
            }

            memset(val, 0, sizeof(val));
            sprintf(val, "%s", sys->port_cfg_val[arryidx].ulFlowCtl ? "enable" : "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_FC, val))
            {
                return -1;  
            }

            memset(val, 0, sizeof(val));
            if(0 == sys->port_cfg_val[arryidx].ulMdix)
            {
                sprintf(val, "%s", "across");
            }
            else if (2 == sys->port_cfg_val[arryidx].ulMdix)
            {
                sprintf(val, "%s", "nomal");
            }
            else
            {
                sprintf(val, "%s", "auto");
            }
            
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_MDIX, val))
            {
                return -1;  
            }

            memset(val, 0, sizeof(val));
            sprintf(val, "%s", sys->port_cfg_val[arryidx].ulIsolateEn ? "enable" : "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_ISOLATE, val))
            {
                return -1;  
            }
        }
       
        clr_update(port);
    }
    
    return 1;
}

void sys_stormctl_apply(void)
{
	int i = 0;
	port_num_t lport = 0;
	logic_pmask_t lPortMask;
	STORM_CTLRATE_S pstStorm = {0};
	for(i = 0; i < LOGIC_PORT_NO; i++)
	{		
		lport = i + 1;
	    ClrLgcMaskAll(&lPortMask);
        SetLgcMaskBit(lport, &lPortMask);
		
		pstStorm.rateType = STORM_RATE_PERCENT;
		
		if(0 == sys_storm.stormclt[i].uucasten)
		{
			/*if value is 100% close it*/
			pstStorm.value = 100;
			Ioctl_SetStormCtrlPort(STORM_DLF_CTRL, &lPortMask, &pstStorm);
		}
		else
		{
			pstStorm.value = sys_storm.stormclt[i].ratepercent;
			Ioctl_SetStormCtrlPort(STORM_DLF_CTRL, &lPortMask, &pstStorm);
		}
		
		if(0 == sys_storm.stormclt[i].umcasten)
		{
			/*if value is 100% close it*/
			pstStorm.value = 100;
			Ioctl_SetStormCtrlPort(STORM_MCAST_CTRL, &lPortMask, &pstStorm);
		}
		else
		{
			pstStorm.value = sys_storm.stormclt[i].ratepercent;
			Ioctl_SetStormCtrlPort(STORM_MCAST_CTRL, &lPortMask, &pstStorm);
		}		
		
		if(0 == sys_storm.stormclt[i].bcasten)
		{
			/*if value is 100% close it*/
			pstStorm.value = 100;
			Ioctl_SetStormCtrlPort(STORM_BCAST_CTRL, &lPortMask, &pstStorm);
		}
		else
		{
			pstStorm.value = sys_storm.stormclt[i].ratepercent;
			Ioctl_SetStormCtrlPort(STORM_BCAST_CTRL, &lPortMask, &pstStorm);
		}
	}
}

void sys_stormctl_load(sys_stormctl_t *sys)
{
    port_num_t lport = 0;
    int arryidx  = 0;    
    char val[BUF_SIZE32] = {0};
    int ifindex = 0;
    char szifname[BUF_SIZE32] = {0};
    
    LgcPortFor(lport)
    { 
        arryidx = lport - 1;

        memset(val, 0, sizeof(val));
        memset(szifname, 0, sizeof(szifname));
        sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
        IF_GetByIFName(szifname,&ifindex);
        if((cfg_getval(ifindex, CONFIG_ETHPORT_UCAST_STORMCTL_EN, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT ucast storm enable faild");
            return;
        }
        
       if(!strcmp(val, "disable"))
       {
           sys->stormclt[arryidx].uucasten = 0;
       }
       else
       {
           sys->stormclt[arryidx].uucasten = 1;
       }
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_MCAST_STORMCTL_EN, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT mcast storm enable  faild");
            return;
        }
        if(!strcmp(val, "disable"))
        {
            sys->stormclt[arryidx].umcasten = 0;
        }
        else
        {
            sys->stormclt[arryidx].umcasten = 1;
        }

        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_BCAST_STORMCTL_EN, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT bcast storm enable  faild");
            return;
        }
        
        if(!strcmp(val, "disable"))
        {
            sys->stormclt[arryidx].bcasten = 0;
        }
        else
        {
            sys->stormclt[arryidx].bcasten = 1;
        }
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_STORMCTL_PERCENT, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT storm percent faild");
            return;
        }
        sys->stormclt[arryidx].ratepercent = strtoul(val, NULL, 0);
    }

    return;

}


INT sys_stormctl_update(STROM_CTL_S *stormctl, long long *updatPortMsk)
{
	sys_stormctl_t *sys = &sys_storm;
	int update = 0;
    int i = 0;

    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
		if(sys->stormclt[i].uucasten != stormctl[i].uucasten ||
			sys->stormclt[i].umcasten != stormctl[i].umcasten ||
			sys->stormclt[i].bcasten != stormctl[i].bcasten ||
			sys->stormclt[i].ratepercent != stormctl[i].ratepercent)
        {
            *updatPortMsk += 1<<i;
		    update ++;
        }
    }

    if(update > 0)
    {
		memcpy(sys->stormclt, stormctl, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
		set_update(storm);
	}

	return update;
}

INT sys_stormctl_commit(unsigned char apply_options)
{
    char val[30] = {0};
    sys_stormctl_t *sys = &sys_storm;
    port_num_t lport = 0;
	int arryidx  = 0; 
	ifindex_t ifindex = 0;
	char szifname[BUF_SIZE_64] = {0};
    

    if (test_update(storm))
    {
        LgcPortFor(lport)
        {       
            memset(szifname, 0, sizeof(szifname));
            sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
            if(0 != IF_GetByIFName(szifname,&ifindex))
            {
                return -1;  
            }
            
            memset(val, 0, sizeof(val));
            arryidx = lport - 1;
        
            sprintf(val, "%s", sys->stormclt[arryidx].uucasten ? "enable": "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_UCAST_STORMCTL_EN, val))
            {
                return -1;  
            }
            
            memset(val, 0, sizeof(val));
            sprintf(val, "%s", sys->stormclt[arryidx].umcasten ? "enable": "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_MCAST_STORMCTL_EN, val))
            {
                return -1;  
            }

            memset(val, 0, sizeof(val));
            sprintf(val, "%s", sys->stormclt[arryidx].bcasten ? "enable": "disable");
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_BCAST_STORMCTL_EN, val))
            {
                return -1;  
            }  

            memset(val, 0, sizeof(val));
            sprintf(val, "%d", sys->stormclt[arryidx].ratepercent);
            if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_STORMCTL_PERCENT, val))
            {
                return -1;  
            }  
        }

        clr_update(storm);
    }
    
	return 1;
}




typedef enum tagPORT_Direct
{
    DIRECT_IN,
    DIRECT_OUT,
    DIRECT_BOTH,
    DIRECT_END_OAM
}PORT_DIRECT_E;


void sys_linerate_load(sys_linerate_t *sys)
{
    port_num_t lport = 0;
    int arryidx  = 0;    
    char val[BUF_SIZE32] = {0};
    int ifindex = 0;
    char szifname[BUF_SIZE32] = {0};
 #if 1  
    LgcPortFor(lport)
    { 
         arryidx = lport - 1;

		 memset(val, 0, sizeof(val));
		 memset(szifname, 0, sizeof(szifname));
         sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
         IF_GetByIFName(szifname,&ifindex);
        if((cfg_getval(ifindex, CONFIG_ETHPORT_INRATE, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT inrate faild");
            return;
        }
		
        sys->port_linerate_val[arryidx].ulRateIn = strtoul(val, NULL, 0);
        
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_ETHPORT_OUTRATE, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get ETHPORT enable  faild");
            return;
        }
        sys->port_linerate_val[arryidx].ulRateOut = strtoul(val, NULL, 0);
        
    }
#endif
#if 0
    //for test feihuaxin
    LgcPortFor(lport)
    {  
        arryidx = lport - 1;
		sys->port_linerate_val[arryidx].ulRateIn = 64;
		sys->port_linerate_val[arryidx].ulRateOut = 128;
    }
#endif
    return;

}

void sys_linerate_apply(void)
{
    char arryidx = 0;
    port_num_t lport = 0;

	LgcPortFor(lport)
	{  
		arryidx = lport - 1;
		Setportlinerate(lport, DRV_RATELIMIT_RX, sys_linerate.port_linerate_val[arryidx].ulRateIn);
		Setportlinerate(lport, DRV_RATELIMIT_TX, sys_linerate.port_linerate_val[arryidx].ulRateOut);
	}

	return;
}

INT sys_linerate_update(PORT_LINERATE_VALUES* port_cfg,long long * updatPortMsk)
{
	int update = 0;
    int i = 0;

    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
		if(sys_linerate.port_linerate_val[i].ulRateIn != port_cfg[i].ulRateIn ||
			sys_linerate.port_linerate_val[i].ulRateOut != port_cfg[i].ulRateOut)
        {
            *updatPortMsk += 1<<i;
		    update ++;
        }
    }

	if(update > 0)
	{
		memcpy(sys_linerate.port_linerate_val, port_cfg, sizeof(PORT_LINERATE_VALUES)*LOGIC_PORT_NO);
		set_update(linerate);
	}
    
	return update;

}

int Setportlinerate(UINT32 lport,  UINT32 uiDirection, UINT32 uiLineRate)
{
	logic_pmask_t pmask;

	memset(&pmask, 0, sizeof(pmask));
	SetLgcMaskBit(lport, &pmask);

    /*rate unit 1kbps feihuaxin*/
	if(DRV_OK != Ioctl_SetRateLimitPort(uiDirection, pmask, uiLineRate))
	{
		return ERROR;
	}

	return NO_ERROR;
}

int sys_linerate_commit(unsigned char apply_options)
{
#if 0
	char val[30] = {0};
	sys_linerate_t *sys = &sys_linerate;
	port_num_t lport = 0;
	int arryidx  = 0; 
	ifindex_t ifindex = 0;
	char szifname[BUF_SIZE_64] = {0};
	
	 if (test_update(linerate))
	 {
		 LgcPortFor(lport)
		 {		 
		     memset(szifname, 0, sizeof(szifname));
		     sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
			 if(0 != IF_GetByIFName(szifname,&ifindex))
		 	 {
				 return -1;  
			 }
			 
			 memset(val, 0, sizeof(val));
			 arryidx = lport - 1;

			 sprintf(val, "%d", sys->port_linerate_val[arryidx].ulRateIn);
			 if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_INRATE, val))
			 {
				 return -1;  
			 }
			 
			 memset(val, 0, sizeof(val));
			 sprintf(val, "%d", sys->port_linerate_val[arryidx].ulRateOut);
			 if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_OUTRATE, val))
			 {
				 return -1;  
			 }			
		 }
		
		 clr_update(linerate);
	 }
#endif
	 return 1;

}


INT sys_mirror_update(MIRROR_GROUP_S * portmirror)
{
	int update = 0;
	int i = 0;
    int lport = 0;
    
    if(sys_mirror.port_mirror_val.ulPortMonitor != portmirror->ulPortMonitor)
	{
		update++;
	}
	/*check right  ??*/
	if(update == 0)
	{
		for(i = 0; i < LOGIC_PORT_NO; i++)
		{
		    lport = i+1;
			if(TstLgcMaskBit(lport, &(sys_mirror.port_mirror_val.stMirrPortInList)) !=
                TstLgcMaskBit(lport,&(portmirror->stMirrPortInList)))
			{
				update++;
				break;
			}
		}
	}

	if(update == 0)
	{
		for(i = 0; i < LOGIC_PORT_NO; i++)
		{
            lport = i+1;
            if(TstLgcMaskBit(lport, &(sys_mirror.port_mirror_val.stMirrPortOutList)) !=
                TstLgcMaskBit(lport,&(portmirror->stMirrPortOutList)))
			{
				update++;
				break;
			}
		}
	}

	if(update > 0)
	{
		memcpy(&sys_mirror.port_mirror_val, portmirror, sizeof(MIRROR_GROUP_S));
		set_update(mirror);
	}
	
	return update;

}

int sys_mirror_commit(unsigned char apply_options)
{
	char val[30] = {0};
	sys_mirror_t *sys = &sys_mirror;
	port_num_t lport = 0;
	ifindex_t ifindex = 0;
	char mirrorin_flag = 0;
	char mirrorout_flag = 0;
	char szifname[BUF_SIZE_64] = {0};
	
	 if (test_update(mirror))
	 {
		 LgcPortFor(lport)
		 {		 
			memset(szifname, 0, sizeof(szifname));
			sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
			if(0 != IF_GetByIFName(szifname,&ifindex))
			{
				return -1;  
			}

			memset(val, 0, sizeof(val));
			if(lport == sys->port_mirror_val.ulPortMonitor)
			{
				sprintf(val, "monitorport");
				if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_MIRRORMOD, val))
				{
					return -1;	
				}  
			}
			else
			{
                mirrorin_flag = 0;
                mirrorout_flag = 0;
                if(TstLgcMaskBit(lport, &(sys->port_mirror_val.stMirrPortInList)))
                {
                    mirrorin_flag = 1;
                }

                if(TstLgcMaskBit(lport, &(sys->port_mirror_val.stMirrPortOutList)))
                {
                    mirrorout_flag = 1;
                }

				if(mirrorin_flag && mirrorout_flag)
				{
					sprintf(val, "both");
				}
				else if(mirrorin_flag)
				{
					sprintf(val, "mirrorin");
				}
				else if(mirrorout_flag)
				{
					sprintf(val, "mirrorout");
				}
				else
				{
					sprintf(val, "nomirror");
				}
					
				if(0 !=master_cfg_setval(ifindex, CONFIG_ETHPORT_MIRRORMOD, val))
				{
					return -1;	
				}  
			}

		 }
		
		 clr_update(mirror);
	 }

     return 1; 
}
void sys_mirror_load(sys_mirror_t *sys)
{
	   port_num_t lport = 0;
	   char val[BUF_SIZE32] = {0};
	   ifindex_t ifindex = 0;
	   char szifname[BUF_SIZE32] = {0};
	   
	   LgcPortFor(lport)
	   { 
			memset(val, 0, sizeof(val));
			memset(szifname, 0, sizeof(szifname));
			sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
			IF_GetByIFName(szifname,&ifindex);
			if((cfg_getval(ifindex, CONFIG_ETHPORT_MIRRORMOD, (void *)val, "", sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get ETHPORT mirrormod faild");
				return;
			}

			if(!strcmp(val, "monitorport"))
			{
				sys->port_mirror_val.ulPortMonitor = lport;
				ClrLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortInList);
				ClrLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortOutList);
			}
			else if(!strcmp(val, "mirrorin"))
			{
				SetLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortInList);
				ClrLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortOutList);
			}
			else if(!strcmp(val, "mirrorout"))
			{
				SetLgcMaskBit(lport,&sys->port_mirror_val.stMirrPortOutList);
				ClrLgcMaskBit(lport,&sys->port_mirror_val.stMirrPortInList);
			}
			else if(!strcmp(val, "both"))
			{
				SetLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortInList);
				SetLgcMaskBit(lport,&sys->port_mirror_val.stMirrPortOutList);
			}
			else
			{
				ClrLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortInList);
				ClrLgcMaskBit(lport, &sys->port_mirror_val.stMirrPortOutList);
			}
	   }

	   return;
}
void sys_mirror_apply(void)
{
	sys_mirror_t *sys = &sys_mirror;

	Ioctl_SetMirrorGroup(sys->port_mirror_val.ulPortMonitor, 
	sys->port_mirror_val.stMirrPortInList, sys->port_mirror_val.stMirrPortOutList
	);
}


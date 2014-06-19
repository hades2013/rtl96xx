/*******************************************************************************
 Copyright (c) 2013, Hangzhou LEPEI Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                            
  Project Code: lware
  Module Name: 
  Date Created: 2013-10-15
  Author: huangmingjian 
  Description: 

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD  

*******************************************************************************/	
	



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
#include <lw_drv_req.h>
#include <lw_drv_pub.h>
#include <memshare.h>


#ifdef CONFIG_CATVCOM	

CATV_WORK_INFO_S *gst_CATVWorkInfo = NULL;


/*****************************************************************
    Function:catv_init
    Description: scatv share memory init
    Author:huangmingjian
    Date:2013/01/11
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
void catv_init(void)
{
	catvinfo_t *sys = &sys_catv;
	startup_process(CATV_STARTUP_CMD);
	if(NULL == (gst_CATVWorkInfo = CATV_WorkInfoShmInit()))
    {
        DBG_PRINTF("CATV shared memory init error!");
        return;        
    }	
    gst_CATVWorkInfo->catv_connet= VAL_UINT8_ERROR;
    gst_CATVWorkInfo->rf_contrl= VAL_UINT8_ERROR;
    gst_CATVWorkInfo->rf_offset= VAL_UINT8_ERROR;
	gst_CATVWorkInfo->temper.val_int = VAL_UINT8_ERROR;
	gst_CATVWorkInfo->temper.val_dec= VAL_UINT8_ERROR;                
    gst_CATVWorkInfo->power_12v = VAL_INT_ERROR;
    gst_CATVWorkInfo->input_laser_power = VAL_INT_ERROR;
    gst_CATVWorkInfo->output_level = VAL_INT_ERROR; 
}



/*****************************************************************
  Function:        sys_catv_load
  Description:    sys_catv_load
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

void sys_catv_load(catvinfo_t *sys) 
{
	int invalid = 0;
	char val[BUF_SIZE_64] = {0};

	if(sys == NULL)
	{
		DBG_ASSERT(0, "Sys is NULL!");
		return;
	}
	memset(sys, 0, sizeof(catvinfo_t));
	
	/*dev status*/
	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_CATV_UPDATE, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_CATV_UPDATE faild!");
	}
	if (val[0])	
		sys->dev_status_update = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	return;
}


/*****************************************************************
  Function:        catv_init_apply
  Description:    catv_init_apply
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/


void catv_init_apply(catvinfo_t *sys) 
{
	if(sys == NULL)
	{
		DBG_ASSERT(0, "Sys is NULL!");
		return;
	}
	return;
}



/*****************************************************************
  Function:        sys_catv_update
  Description:    sys_catv_update
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_catv_update(catvinfo_t *cfg)  
{
	catvinfo_t *sys = &sys_catv;
	int update = 0;

	sys->module_mask = cfg->module_mask; 
	sys->para_mask = 0;
	/*dev status*/
	if(sys->module_mask & DEV_STATUS)
	{
		if (sys->dev_status_update != cfg->dev_status_update)
		{
			sys->dev_status_update = cfg->dev_status_update;
			sys->para_mask |= PM_UPDATE;
			update++;
		}
	}

	DBG_PRINTF("catv update:%d, 0x%08x, module_mask:0x%02x", update, sys->para_mask, sys->module_mask);
	
	return update;
}


/*****************************************************************
  Function:        sys_catv_ap_apply
  Description:    sys_catv_ap_apply
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int sys_catv_apply(int apply_options)   
{
	catvinfo_t *sys = &sys_catv;
	
	/*dev status*/
	if(sys->module_mask & DEV_STATUS)
	{
		if(sys->para_mask & PM_UPDATE)
		{
			;
		}
	}

	return 0;
}

/*****************************************************************
  Function:        sys_catv_commit
  Description:    sys_catv_commit
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/


int sys_catv_commit(int apply_options) 
{	
	catvinfo_t *sys = &sys_catv;
	char buf[128] = {0};
	
	/*dev status*/
	if(sys->module_mask & DEV_STATUS)
	{
		if(sys->para_mask & PM_UPDATE)
		{	
			sprintf(buf, "%d", sys->dev_status_update);
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_CATV_UPDATE, (void *)buf))
            {	
				DBG_PRINTF("Set CONFIG_CATV_UPDATE faild!");
                return -1;  
            }				
		}
	}
	return 0;
}
#endif


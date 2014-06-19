/*
=============================================================================
     File Name: odm_rstp.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	2008/10/23		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include "defs.h"

//#include "bridgemac.h"
#include "odm_port.h"

//#include "dal_rstp.h"
#include "hal.h"
#include "hal_rstp.h"
#include "uid_stp.h"
#include "rstp_in.h"
#include "odm_rstp.h"

/*******************************************************************************
* rstp_enable
*
* DESCRIPTION:
*  		enable or disable rstp
*
*	INPUTS:
*		enable		0 - disable rstp
					1 - enable rstp
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpEnable (int enable)
{
	int iExist;
	char *pcKeyValueTemp = NULL;
	int bCfgEnable;
	int iRet;
	
	/* check whether the rstp cfg file exists */
	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_VALUE_MODE);
	if (0 == iExist)
	{
		/* check whether the rstp mode is already set */
		pcKeyValueTemp = vosConfigValueGet(CFGFILE_RSTP,
												      RSTP_SECTION_BRD,
												      RSTP_BRD_KEY_VALUE_MODE,
												      RSTP_VALUE_DISABLE);
		
		if (0 == memcmp(RSTP_VALUE_DISABLE, pcKeyValueTemp, strlen(RSTP_VALUE_DISABLE)))
		{
			bCfgEnable = 0;	/* rstp is disabled in rstp cfg file */
		}
		else
		{
			bCfgEnable = 1;	/* rstp is enabled in rstp cfg file */
		}

		if (bCfgEnable == enable)
		{
			return NO_ERROR;
		}
	}
	
	if (1 == enable)
	{
		
		/* set bridge rstp enable */
		iRet = cli_enable();
		if (0 != iRet)
		{
			return RSTP_BRD_ENABLE_ERROR;
		}

		/* set rstp global enabled in rstp cfg file */
		iRet = vosConfigValueSet(CFGFILE_RSTP,
										    RSTP_SECTION_BRD,
									 		RSTP_BRD_KEY_VALUE_MODE,
									 		RSTP_VALUE_ENABLE);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_BRD_MODE_SET_ERROR;
		}

	}
	else
	{
		
		/* set bridge rstp disable */
		iRet = cli_disable();
		if (0 != iRet)
		{
			return RSTP_BRD_DISABLE_ERROR;
		}

		/* set rstp global disabled in rstp cfg file */
		iRet = vosConfigValueSet(CFGFILE_RSTP,
									 		RSTP_SECTION_BRD,
									 		RSTP_BRD_KEY_VALUE_MODE,
									 		RSTP_VALUE_DISABLE);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_BRD_MODE_SET_ERROR;
		}

	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_info_show
*
* DESCRIPTION:
*  		show rstp state machine info
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
void odmRstpInfoShow(int lFd)
{
	char str[10];
	int i;
	
	/* show rstp instance cfg */
	bridge_conf_read();
	vosPrintf(lFd, "Rstp bridge config\r\n");
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp enable", uid_cfg.stp_enabled);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp bridge_priority", uid_cfg.bridge_priority);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp diameter", uid_cfg.diameter);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp max_age", uid_cfg.max_age);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp hello_time", uid_cfg.hello_time);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp forward_delay", uid_cfg.forward_delay);
	vosPrintf(lFd, "    %-20s : %d\r\n", "rstp force_version", uid_cfg.force_version);

	for (i = 1; i <= NUMBER_OF_PORTS; i++)
	{
		sprintf(str,"port %d",i);
		vosPrintf(lFd, "Rstp %s config\r\n", str);
		vosPrintf(lFd, "    %-20s : %d\r\n", "port enable", port_cfg[i-1].enable);
		vosPrintf(lFd, "    %-20s : %d\r\n", "port priority", port_cfg[i-1].priority);
		vosPrintf(lFd, "    %-20s : %d\r\n", "port p2plink", port_cfg[i-1].p2plink);
		vosPrintf(lFd, "    %-20s : %d\r\n", "port path_cost", port_cfg[i-1].path_cost);
		vosPrintf(lFd, "    %-20s : %d\r\n", "port edge_port", port_cfg[i-1].edge_port);
	}

	return;

}

/*******************************************************************************
* rstp_conf_show
*
* DESCRIPTION:
*  		show rstp configure file info
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
void odmRstpConfShow (int lFd)
{
	int iExist;

	/* show rstp cfg file */
	iExist = vosConfigModuleIsExisted(CFGFILE_RSTP);
	if (0 == iExist)
	{
		vosConfigShowByModule(CFGFILE_RSTP, lFd);
	}
	else
	{
		vosPrintf(lFd, "No rstp configure\r\n");
	}

	return;
}

/*******************************************************************************
* rstp_set_hello
*
* DESCRIPTION:
*  		show rstp hello time
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetHello(unsigned int data)
{
	int iExist;
	unsigned int ulCfgHelloTime;
	int iRet;
	
	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_HELLOTIME);

	if (0 == iExist)
	{
		ulCfgHelloTime = vosConfigUInt32Get(CFGFILE_RSTP,
												       RSTP_SECTION_BRD,
												       RSTP_BRD_KEY_U32_HELLOTIME,
												       RSTP_BRD_DEFAULT_HELLOTIME);

		if (ulCfgHelloTime == data)
		{
			return NO_ERROR;
		}
	}

	iRet = cli_br_hellotime(data);
	if (0 != iRet)
	{
		return RSTP_BRD_HELLOTIME_SET_ERROR;
	}	

	iRet = vosConfigUInt32Set(CFGFILE_RSTP,
									     RSTP_SECTION_BRD,
									     RSTP_BRD_KEY_U32_HELLOTIME,
									     data);
	if (0 != iRet)
	{
		return RSTP_CFG_FILE_BRD_HELLOTIME_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_forward
*
* DESCRIPTION:
*  		show rstp forward delay time
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetForward(unsigned int data)
{
	int iExist;
	unsigned int ulCfgFwdDelayTime; 
	int iRet;

	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_FWDDELAY);
	
	if (0 == iExist)
	{
		ulCfgFwdDelayTime = vosConfigUInt32Get(CFGFILE_RSTP,
													      RSTP_SECTION_BRD,
													      RSTP_BRD_KEY_U32_FWDDELAY,
													      RSTP_BRD_DEFAULT_FWDDELAYTIME);

		if (ulCfgFwdDelayTime == data)
		{
			return NO_ERROR;
		}
	}
	
	iRet = cli_br_fdelay(data);
	if (0 != iRet)
	{
		return RSTP_BRD_FWDDELAY_SET_ERROR;
	}	

	iRet = vosConfigUInt32Set(CFGFILE_RSTP,
									     RSTP_SECTION_BRD,
									     RSTP_BRD_KEY_U32_FWDDELAY,
									     data);
	if (0 != iRet)
	{
		return RSTP_CFG_FILE_BRD_FWDDELAY_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_maxage
*
* DESCRIPTION:
*  		show rstp max age
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetMaxage(unsigned int data)
{
	int iExist;
	unsigned int ulCfgMaxAge; 
	int iRet;

	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_MAXAGE);
	
	if (0 == iExist)
	{
		ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
											      	RSTP_SECTION_BRD,
											      	RSTP_BRD_KEY_U32_MAXAGE,
											      	RSTP_BRD_DEFAULT_MAXAGE);

		if (ulCfgMaxAge == data)
		{
			return NO_ERROR;
		}
	}
	
	iRet = cli_br_maxage(data);
	if (0 != iRet)
	{
		return RSTP_BRD_MAXAGE_SET_ERROR;
	}	

	iRet = vosConfigUInt32Set(CFGFILE_RSTP,
								  		 RSTP_SECTION_BRD,
								  		 RSTP_BRD_KEY_U32_MAXAGE,
								  		 data);
	if (0 != iRet)
	{
		return RSTP_CFG_FILE_BRD_MAXAGE_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_forver
*
* DESCRIPTION:
*  		show rstp force version
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetForver(unsigned int data)
{
	int iExist;
	unsigned int ulCfgForceVer;
	int iRet;

	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_VERSION);
	
	if (0 == iExist)
	{
		ulCfgForceVer = vosConfigUInt32Get(CFGFILE_RSTP,
											      	  RSTP_SECTION_BRD,
											      	  RSTP_BRD_KEY_U32_VERSION,
											      	  RSTP_BRD_DEFAULT_FORCEVERSION);

		if (ulCfgForceVer == data)
		{
			return NO_ERROR;
		}
	}
	
	iRet = cli_br_fvers(data);
	if (0 != iRet)
	{
		return RSTP_BRD_VERSION_SET_ERROR;
	}	

	iRet = vosConfigUInt32Set(CFGFILE_RSTP,
								  		 RSTP_SECTION_BRD,
								  		 RSTP_BRD_KEY_U32_VERSION,
								  		 data);
	if (0 != iRet)
	{
		return RSTP_CFG_FILE_BRD_VERSION_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_priority
*
* DESCRIPTION:
*  		show rstp bridge priority
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetPriority(unsigned int data)
{
	int iExist;
	unsigned int ulCfgBrdPri;
	int iRet;

	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_PRI);
	
	if (0 == iExist)
	{
		ulCfgBrdPri = vosConfigUInt32Get(CFGFILE_RSTP,
											      	RSTP_SECTION_BRD,
											      	RSTP_BRD_KEY_U32_PRI,
											      	RSTP_BRD_DEFAULT_PRI);

		if (ulCfgBrdPri == data)
		{
			return NO_ERROR;
		}
	}

	iRet = cli_br_prio(data * 4096);
	if (0 != iRet)
	{
		return RSTP_BRD_PRI_SET_ERROR;
	}	

	iRet = vosConfigUInt32Set(CFGFILE_RSTP,
								  		 RSTP_SECTION_BRD,
								  		 RSTP_BRD_KEY_U32_PRI,
								  		 data);
	if (0 != iRet)
	{
		return RSTP_CFG_FILE_BRD_PRI_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_diameter
*
* DESCRIPTION:
*  		show rstp bridge diameter
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetDiameter(unsigned int data)
{
	int iExist;
	unsigned int ulCfgHelloTime;
	unsigned int ulCfgFwdDelayTime;
	unsigned int ulCfgMaxAge;
	unsigned int ulHelloTime;
	unsigned int ulFwdDelayTime;
	unsigned int ulMaxAge;
	int bUpdateHelloTime = 1;
	int bUpdateFwdDelayTime = 1;
	int bUpdateMaxAge = 1;
	int iRet;

	/* calculate forward delay time and maxage */
	ulHelloTime = RSTP_BRD_DEFAULT_HELLOTIME;
	ulFwdDelayTime = data * ulHelloTime + 1;
	ulMaxAge = (data + 3) * ulHelloTime;

	/* get hellotime cfg value */
	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_HELLOTIME);
	
	if (0 == iExist)
	{
		ulCfgHelloTime = vosConfigUInt32Get(CFGFILE_RSTP,
												       RSTP_SECTION_BRD,
												       RSTP_BRD_KEY_U32_HELLOTIME,
												       RSTP_BRD_DEFAULT_HELLOTIME);

		if (ulHelloTime == ulCfgHelloTime)
		{
			bUpdateHelloTime = 0;
		}

	}

	/* get forward delay time cfg value */
	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_FWDDELAY);

	if (0 == iExist)
	{
		ulCfgFwdDelayTime = vosConfigUInt32Get(CFGFILE_RSTP,
												       	  RSTP_SECTION_BRD,
												       	  RSTP_BRD_KEY_U32_FWDDELAY,
												       	  RSTP_BRD_DEFAULT_FWDDELAYTIME);

		if (ulFwdDelayTime == ulCfgFwdDelayTime)
		{
			bUpdateFwdDelayTime = 0;
		}
	}

	/* get max age cfg value */
	iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
											   RSTP_SECTION_BRD,
											   RSTP_BRD_KEY_U32_MAXAGE);

	if (0 == iExist)
	{
		ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
												       	  RSTP_SECTION_BRD,
												       	  RSTP_BRD_KEY_U32_MAXAGE,
												       	  RSTP_BRD_DEFAULT_MAXAGE);

		if (ulMaxAge == ulCfgMaxAge)
		{
			bUpdateMaxAge = 0;
		}
	}

	/* update state machine */
	if ((0 != bUpdateHelloTime) || (0 != bUpdateFwdDelayTime) || (0 != bUpdateMaxAge))
	{
		iRet = cli_br_diameter(RSTP_BRD_DEFAULT_HELLOTIME, data);
		if (0 != iRet)
		{
			return RSTP_BRD_HELLOTIME_SET_ERROR;
		}
	}

	/* update configure of diameter */
	if (data != vosConfigUInt32Get(CFGFILE_RSTP,
								 RSTP_SECTION_BRD,
				       	  		 RSTP_BRD_KEY_U32_DIAMETER,
				       	  		 0)){
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
					       	  		 RSTP_SECTION_BRD,
					       	  		 RSTP_BRD_KEY_U32_DIAMETER,
					       	  		 data);
		if (0 != iRet){
			return RSTP_CFG_FILE_BRD_DIAMETER_SET_ERROR;
		}
	}
			
	/* update configure of hellotime */
	if (0 != bUpdateHelloTime)
	{
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
							       	  		 RSTP_SECTION_BRD,
							       	  		 RSTP_BRD_KEY_U32_HELLOTIME,
							       	  		 ulHelloTime);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_BRD_HELLOTIME_SET_ERROR;
		}
	}

	/* update configure of forward delay time */
	if (0 != bUpdateFwdDelayTime)
	{		
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
							       	  		 RSTP_SECTION_BRD,
							       	   		 RSTP_BRD_KEY_U32_FWDDELAY,
							       	  		 ulFwdDelayTime);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_BRD_FWDDELAY_SET_ERROR;
		}
	}

	/* update configure of max age */
	if (0 != bUpdateMaxAge)
	{
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
							       	  		 RSTP_SECTION_BRD,
							       	  		 RSTP_BRD_KEY_U32_MAXAGE,
							       	  		 ulMaxAge);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_BRD_MAXAGE_SET_ERROR;
		}
	}
	
	return NO_ERROR;
}

/*******************************************************************************
* rstp_port_enable
*
* DESCRIPTION:
*  		port enable or disable rstp
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpPortEnable(unsigned long ulPortBitmap, int enable)	
{
	int bNonRstp;
	char *pcRstpMode = NULL;
	char str[10];
	int i;
	unsigned char ucPortBit;
	int iExist;
	char *pcKeyValueTemp = NULL;
	int bCfgEnable;
	UINT8 ucPortState;
	int iRet = 0;

	if (1 == enable)
	{
		pcRstpMode = vosConfigValueGet(CFGFILE_RSTP,
									   			  RSTP_SECTION_BRD,
									   			  RSTP_BRD_KEY_VALUE_MODE,
									   			  RSTP_VALUE_DISABLE);
		if (0 != strcmp(RSTP_VALUE_ENABLE, pcRstpMode))
		{  
			return RSTP_BRD_NOT_ENABLE_WARNING;
		}
	}

	/* update rstp state machine */
	if (0 == enable)
	{
		bNonRstp = 1;
	}
	else
	{
		bNonRstp = 0;
	}
	//iRet = cli_prt_non_stp(port, enable);
	iRet = cli_multiport_non_rstp(ulPortBitmap, bNonRstp);
	if (0 != iRet)
	{
		return RSTP_PORT_MODE_SET_ERROR;
	}	
	
	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		ucPortBit = 1 << (i - ODM_START_PORT_NUN);
		if (0 != (ulPortBitmap & ucPortBit))
		{
			sprintf(str,"PORT %d", i);
		}
		else
		{
			continue;
		}
		
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   str,
												   RSTP_PORT_KEY_VALUE_MODE);
		if (0 == iExist)
		{
			pcKeyValueTemp = vosConfigValueGet(CFGFILE_RSTP,
											      		  str,
											          	  RSTP_PORT_KEY_VALUE_MODE,
											      		  RSTP_VALUE_DISABLE);
			
			if (0 == memcmp(RSTP_VALUE_DISABLE, pcKeyValueTemp, strlen(RSTP_VALUE_DISABLE)))
			{
				bCfgEnable = 0;
			}
			else
			{
				bCfgEnable = 1;
			}

			if (bCfgEnable == enable)
			{
				continue;
			}
		}
		
		if (1 == enable)
		{
			iRet = vosConfigValueSet(CFGFILE_RSTP,
										 		str,
										 		RSTP_PORT_KEY_VALUE_MODE,
										 		RSTP_VALUE_ENABLE);
		}
		else
		{
			iRet = vosConfigValueSet(CFGFILE_RSTP,
										 		str,
										 		RSTP_PORT_KEY_VALUE_MODE,
										 		RSTP_VALUE_DISABLE);
		}

		if (0 != iRet)
		{
			return RSTP_CFG_FILE_PORT_MODE_SET_ERROR;
		}	
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_edgeport_enable
*
* DESCRIPTION:
*  		set port edge-port or non-edge-port
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpEdgePortEnable(unsigned long ulPortBitmap, int enable)	
{
	char str[10];
	int i;
	unsigned char ucPortBit;
	int iExist;
	char *pcKeyValueTemp = NULL;
	int bCfgEnable;
	int iRet = 0;

	/* update rstp state machine */
	//iRet = cli_prt_edge(port, enable);
	iRet = cli_multiport_edge(ulPortBitmap, enable);
	if (0 != iRet)
	{
		return RSTP_PORT_EDGE_SET_ERROR;
	}	

	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		ucPortBit = 1 << (i - ODM_START_PORT_NUN);
		if (0 != (ulPortBitmap & ucPortBit))
		{
			sprintf(str,"PORT %d", i);
		}
		else
		{
			continue;
		}
		
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   str,
												   RSTP_PORT_KEY_VALUE_EDGE);
		if (0 == iExist)
		{
			pcKeyValueTemp = vosConfigValueGet(CFGFILE_RSTP,
											      		  str,
											          	  RSTP_PORT_KEY_VALUE_EDGE,
											      		  RSTP_VALUE_DISABLE);
			
			if (0 == memcmp(RSTP_VALUE_DISABLE, pcKeyValueTemp, strlen(RSTP_VALUE_DISABLE)))
			{
				bCfgEnable = 0;
			}
			else
			{
				bCfgEnable = 1;
			}

			if (bCfgEnable == enable)
			{
				continue;
			}
		}
		
		if (1 == enable)
		{			
			iRet = vosConfigValueSet(CFGFILE_RSTP,
										 		str,
										 		RSTP_PORT_KEY_VALUE_EDGE,
										 		RSTP_VALUE_ENABLE);
		}
		else
		{			
			iRet = vosConfigValueSet(CFGFILE_RSTP,
										 		str,
										 		RSTP_PORT_KEY_VALUE_EDGE,
										 		RSTP_VALUE_DISABLE);
		}

		if (0 != iRet)
		{
			return RSTP_CFG_FILE_PORT_EDGE_SET_ERROR;
		}	
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_p2plink_mode
*
* DESCRIPTION:
*  		set port p2p type
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpP2plinkMode(unsigned long ulPortBitmap, int data)
{
	char str[10];
	int i;
	unsigned char ucPortBit;
	int iExist;
	int iP2pMode;
	int bCfgEnable;
	int iRet = 0;

	/* update rstp state machine */
	//iRet = cli_prt_p2p(port, data);
	iRet = cli_multiport_p2p(ulPortBitmap, data);
	if (0 != iRet)
	{
		return RSTP_PORT_P2P_SET_ERROR;
	}	

	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		ucPortBit = 1 << (i - ODM_START_PORT_NUN);
		if (0 != (ulPortBitmap & ucPortBit))
		{
			sprintf(str,"PORT %d", i);
		}
		else
		{
			continue;
		}
		
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   str,
												   RSTP_PORT_KEY_U32_P2P);
		if (0 == iExist)
		{
			iP2pMode = vosConfigUInt32Get(CFGFILE_RSTP,
									      		     str,
									          	     RSTP_PORT_KEY_U32_P2P,
									      		     RSTP_PORT_DEFAULT_P2P_MODE);
			if (iP2pMode == data)
			{
				continue;
			}
		}
				
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
				  					 		 str,
				 					 		 RSTP_PORT_KEY_U32_P2P,
									 		 data);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_PORT_P2P_SET_ERROR;
		}	
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_pathcost
*
* DESCRIPTION:
*  		set port path cost
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetPathCost(unsigned long ulPortBitmap, unsigned int data)
{
	char str[10];
	int i;
	unsigned char ucPortBit;
	int iExist;
	unsigned int ulCfgPortPri;
	int iRet = 0;

	/* update rstp state machine */
	//iRet = cli_prt_pcost(port, data);
	iRet = cli_multiport_pcost(ulPortBitmap, data);
	if (0 != iRet)
	{
		return RSTP_PORT_PATH_COST_SET_ERROR;
	}	

	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		ucPortBit = 1 << (i - ODM_START_PORT_NUN);
		if (0 != (ulPortBitmap & ucPortBit))
		{
			sprintf(str,"PORT %d", i);
		}
		else
		{
			continue;
		}
		
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   str,
												   RSTP_PORT_KEY_U32_PCOST);
		if (0 == iExist)
		{
			ulCfgPortPri = vosConfigUInt32Get(CFGFILE_RSTP,
														 str,
														 RSTP_PORT_KEY_U32_PCOST,
														 RSTP_PORT_DEFAULT_PATH_COST);
			

			if (ulCfgPortPri == data)
			{
				continue;
			}
		}
		
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
											  str,
											  RSTP_PORT_KEY_U32_PCOST,
											  data);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_PORT_PATH_COST_SET_ERROR;
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* rstp_set_portpriority
*
* DESCRIPTION:
*  		set port priority
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpSetPortPriority(unsigned long ulPortBitmap, unsigned int data)
{
	char str[10];
	int i;
	unsigned char ucPortBit;
	int iExist;
	unsigned int ulCfgPortPri;
	int iRet = 0;

	/* update rstp state machine */
	//iRet = cli_prt_prio(port, data);
	iRet = cli_multiport_prio(ulPortBitmap, data * 16);
	if (0 != iRet)
	{
		return RSTP_PORT_PRI_SET_ERROR;
	}	

	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		ucPortBit = 1 << (i - ODM_START_PORT_NUN);
		if (0 != (ulPortBitmap & ucPortBit))
		{
			sprintf(str,"PORT %d", i);
		}
		else
		{
			continue;
		}
		
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   str,
												   RSTP_PORT_KEY_U32_PRI);
		if (0 == iExist)
		{
			ulCfgPortPri = vosConfigUInt32Get(CFGFILE_RSTP,
											      		 str,
											          	 RSTP_PORT_KEY_U32_PRI,
											      		 RSTP_PORT_DEFAULT_PRI);
			
			if (ulCfgPortPri == data)
			{
				continue;
			}
		}
		
		iRet = vosConfigUInt32Set(CFGFILE_RSTP,
									 		 str,
									 		 RSTP_PORT_KEY_U32_PRI,
									 		 data);
		if (0 != iRet)
		{
			return RSTP_CFG_FILE_PORT_PRI_SET_ERROR;
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmRstpInit
*
* DESCRIPTION:
*  		initial rstp module
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpInit(void)
{
	int iExist;
	char *szData;
	UINT32 ulData;
	UID_STP_CFG_T uid_cfg;
	UINT16 i;
	char str[10];
	UID_STP_PORT_CFG_T uid_port_cfg;
	STATUS iRet;
    UINT32  enableFlag;
    UINT32  loopHoldTime;

    UINT8 ucIcos;
    UINT8 ucMacCtrl;
    /* begin added by jiangmingli for tw_bug 2066 */
    for (i = 0; i < NUMBER_OF_PORTS; i++)
    {
        abPortLoop[i] = 0;
		rstpwarningstate[i] = 0;
        /* init loop detection info */
        odmLoopDetectionGet(i+1, &enableFlag);
        bOnePortLoopDetectEn[i] = enableFlag;
        odmLoopDetectionHoldDownTimeGet(i+1, &loopHoldTime);
        bOnePortLoopWhile[i] = loopHoldTime;
        bOnePortLooptimer[i] = loopHoldTime;
    }
    /* end added by jiangmingli for tw_bug 2066 */
            
	/* create rstp instance and thread */
	iRet = rstp_init();
	if (0 != iRet)
	{
		return RSTP_INIT_ERROR;
	}

	/* check config file */
	iExist = vosConfigModuleIsExisted(CFGFILE_RSTP);
	if (0 != iExist)
	{
        odmRstpPktHandleModeSet(0); /* set to enable */
        #if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
        iRet = odmRstpEnable(OPL_TRUE);
        if (NO_ERROR != iRet)
        {
            OPL_LOG_TRACE();
            printf("Enable rstp failed!\n");
            return iRet;
        }

        odmRstpPortEnable(0x0000000F, OPL_TRUE);
        #endif
		return NO_ERROR;
	}

	uid_cfg.field_mask = 0;

	/* recover rstp bridge configuration */
	iExist = vosConfigSectionIsExisted(CFGFILE_RSTP, RSTP_SECTION_BRD);
	if (0 == iExist)
	{		
		/* set rstp mode */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_VALUE_MODE);
		if (0 == iExist)
		{

			szData = vosConfigValueGet(CFGFILE_RSTP,
										      	  RSTP_SECTION_BRD,
										          RSTP_BRD_KEY_VALUE_MODE,
										      	  RSTP_VALUE_DISABLE);

			if (0 == memcmp(RSTP_VALUE_DISABLE, szData, strlen(RSTP_VALUE_DISABLE)))
			{
				uid_cfg.field_mask |= BR_CFG_STATE;
				uid_cfg.stp_enabled = STP_DISABLED;
			}
			else
			{
				uid_cfg.field_mask |= BR_CFG_STATE;
				uid_cfg.stp_enabled = STP_ENABLED;
			}
		}

		/* set rstp hellotime */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_U32_HELLOTIME);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
										      	   RSTP_SECTION_BRD,
										           RSTP_BRD_KEY_U32_HELLOTIME,
										      	   RSTP_BRD_DEFAULT_HELLOTIME);

			uid_cfg.field_mask |= BR_CFG_HELLO;
			uid_cfg.hello_time = ulData;
		}

		/* set rstp forward-delay time */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_U32_FWDDELAY);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
										      	   RSTP_SECTION_BRD,
										           RSTP_BRD_KEY_U32_FWDDELAY,
										      	   RSTP_BRD_DEFAULT_FWDDELAYTIME);

			uid_cfg.field_mask |= BR_CFG_DELAY;
			uid_cfg.forward_delay = ulData;
		}

		/* set rstp max age */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_U32_MAXAGE);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
										      	   RSTP_SECTION_BRD,
										           RSTP_BRD_KEY_U32_MAXAGE,
										      	   RSTP_BRD_DEFAULT_MAXAGE);

			uid_cfg.field_mask |= BR_CFG_AGE;
			uid_cfg.max_age = ulData;
		}

		/* set rstp force-version */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_U32_VERSION);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
										      	   RSTP_SECTION_BRD,
										           RSTP_BRD_KEY_U32_VERSION,
										      	   RSTP_BRD_DEFAULT_FORCEVERSION);

			uid_cfg.field_mask |= BR_CFG_FORCE_VER;
			uid_cfg.force_version = ulData;
		}

		/* set rstp bridge priority */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
												   RSTP_SECTION_BRD,
												   RSTP_BRD_KEY_U32_PRI);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
										      	   RSTP_SECTION_BRD,
										           RSTP_BRD_KEY_U32_PRI,
										      	   RSTP_BRD_DEFAULT_PRI);

			uid_cfg.field_mask |= BR_CFG_PRIO;
			uid_cfg.bridge_priority = ulData * 4096;
		}

		/* set bridge cfg to state machine */
		iRet = STP_IN_stpm_set_cfg (0, NULL, &uid_cfg);
		if (OPL_OK != iRet)
		{
			OPL_LOG_TRACE();
			return RSTP_BRD_CFG_RECOVER_ERROR;
		}
        /* begin added by jiangmingli for zte */
        /* set rstp packet handle mode */
		iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
									   RSTP_SECTION_BRD,
									   RSTP_BRD_KEY_U32_HANDLE_MODE);
		if (0 == iExist)
		{
			ulData = vosConfigUInt32Get(CFGFILE_RSTP,
							      	    RSTP_SECTION_BRD,
							            RSTP_BRD_KEY_U32_HANDLE_MODE,
							      	    RSTP_BRD_DEFAULT_HANDLE_MODE);

            odmRstpPktHandleModeSet(ulData);
		}
        /* end added by jiangmingli for zte */
	}
	
	/* recover rstp port configuration */
	for (i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++)
	{
		sprintf(str,"PORT %d", i);
		
		iExist = vosConfigSectionIsExisted(CFGFILE_RSTP, str);
		if (0 == iExist)
		{
			memset(&uid_port_cfg, 0, sizeof(UID_STP_PORT_CFG_T));
			uid_port_cfg.port_bmp.part[0] = 1 << (i - ODM_START_PORT_NUN);
			
			/* set rstp port mode */
			iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
													   str,
													   RSTP_PORT_KEY_VALUE_MODE);
			if (0 == iExist)
			{

				szData = vosConfigValueGet(CFGFILE_RSTP,
											      	  str,
											          RSTP_PORT_KEY_VALUE_MODE,
											      	  RSTP_VALUE_DISABLE);

				if (0 == memcmp(RSTP_VALUE_DISABLE, szData, strlen(RSTP_VALUE_DISABLE)))
				{
					uid_port_cfg.field_mask |= PT_CFG_NON_STP;
					uid_port_cfg.admin_non_stp = 1;
				}
				else
				{
					uid_port_cfg.field_mask |= PT_CFG_NON_STP;
					uid_port_cfg.admin_non_stp = 0;
				}
			}

			/* set rstp port admin-edge */
			iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
													   str,
													   RSTP_PORT_KEY_VALUE_EDGE);
			if (0 == iExist)
			{
				szData = vosConfigValueGet(CFGFILE_RSTP,
											      	  str,
													  RSTP_PORT_KEY_VALUE_EDGE,
											      	  RSTP_VALUE_DISABLE);

				if (0 == memcmp(RSTP_VALUE_DISABLE, szData, strlen(RSTP_VALUE_DISABLE)))
				{
					uid_port_cfg.field_mask |= PT_CFG_EDGE;
					uid_port_cfg.admin_edge = 0;
				}
				else
				{
					uid_port_cfg.field_mask |= PT_CFG_EDGE;
					uid_port_cfg.admin_edge = 1;
				}
			}

			/* set rstp port p2p */
			iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
													   str,
													   RSTP_PORT_KEY_U32_P2P);
			if (0 == iExist)
			{
				ulData = vosConfigUInt32Get(CFGFILE_RSTP,
											      	   str,
													   RSTP_PORT_KEY_U32_P2P,
											      	   RSTP_PORT_DEFAULT_P2P_MODE);

				uid_port_cfg.field_mask |= PT_CFG_P2P;
				uid_port_cfg.admin_point2point = ulData;
			}

			/* set rstp port priority */
			iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
													   str,
													   RSTP_PORT_KEY_U32_PRI);
			if (0 == iExist)
			{
				ulData = vosConfigUInt32Get(CFGFILE_RSTP,
											      	   str,
													   RSTP_PORT_KEY_U32_PRI,
											      	   RSTP_PORT_DEFAULT_PRI);

				uid_port_cfg.field_mask |= PT_CFG_PRIO;
				uid_port_cfg.port_priority = ulData;
			}

			/* set rstp port p2p */
			iExist = vosConfigKeyIsExisted(CFGFILE_RSTP,
													   str,
													   RSTP_PORT_KEY_U32_PCOST);
			if (0 == iExist)
			{
				ulData = vosConfigUInt32Get(CFGFILE_RSTP,
											      	   str,
													   RSTP_PORT_KEY_U32_PCOST,
											      	   RSTP_PORT_DEFAULT_PATH_COST);

				uid_port_cfg.field_mask |= PT_CFG_COST;
				uid_port_cfg.admin_port_path_cost = ulData * 16;
			}

			/* set bridge cfg to state machine */
			iRet = STP_IN_set_port_cfg (0, &uid_port_cfg);
			if (OPL_OK != iRet)
			{
				OPL_LOG_TRACE();
				return RSTP_PORT_CFG_RECOVER_ERROR;
			}
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmRstpDestroy
*
* DESCRIPTION:
*  		destroy rstp instance and thread
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpDestroy(void)
{
	/* delete rstp instance and thread */
	bridge_shutdown();

	return 0;
}

/*******************************************************************************
* odmRstpBrdInfoGet
*
* DESCRIPTION:
*  		get rstp bridge info from state machine
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpBrdInfoGet(RSTP_INFO_ELEMENT_E enType, INT32 *pulBrdInfo)
{
	if (NULL == pulBrdInfo)
	{
		return ERR_NULL_POINTER;
	}
	
	switch (enType)
	{
		/* get bridge mode */
		case RSTP_BRD_MODE:
		{
			*pulBrdInfo = uid_cfg.stp_enabled;
			break;
		}
		/* get bridge priority */
		case RSTP_BRD_PRI:
		{
			*pulBrdInfo = uid_cfg.bridge_priority;
			break;
		}
		/* get bridge max age */
		case RSTP_BRD_MAXAGE:
		{
			*pulBrdInfo = uid_cfg.max_age;
			break;
		}
		/* get bridge hello time */
		case RSTP_BRD_HELLO:
		{
			*pulBrdInfo = uid_cfg.hello_time;
			break;
		}
		/* get bridge forward delay time */
		case RSTP_BRD_FWDDELAY:
		{
			*pulBrdInfo = uid_cfg.forward_delay;
			break;
		}
		/* get bridge force version */
		case RSTP_BRD_VER:
		{
			*pulBrdInfo = uid_cfg.force_version;
			break;
		}
		/* get bridge diameter */
		case RSTP_BRD_DIAMETER:
		{
			*pulBrdInfo = uid_cfg.diameter;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmRstpPortInfoGet
*
* DESCRIPTION:
*  		get rstp port info from state machine
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpPortInfoGet(UINT32 ulPortId, RSTP_INFO_ELEMENT_E enType, INT32 *pulPortInfo)
{
	if ((ulPortId < 1) || (ulPortId > ODM_NUM_OF_PORTS))
	{
		return ERR_INVALID_PARAMETERS;
	}
	
	if (NULL == pulPortInfo)
	{
		return ERR_NULL_POINTER;
	}

	switch (enType)
	{
		/* get port mode */
		case RSTP_PORT_MODE:
		{
			*pulPortInfo = port_cfg[ulPortId].enable;
			break;
		}
		/* get port path cost */
		case RSTP_PORT_PCOST:
		{
			*pulPortInfo = port_cfg[ulPortId].path_cost;
			break;
		}
		/* get port priority */
		case RSTP_PORT_PRI:
		{
			*pulPortInfo = port_cfg[ulPortId].priority;
			break;
		}
		/* get port edge state */
		case RSTP_PORT_EDGE:
		{
			*pulPortInfo = port_cfg[ulPortId].edge_port;
			break;
		}
		/* get port p2p state */
		case RSTP_PORT_P2P:
		{
			*pulPortInfo = port_cfg[ulPortId].p2plink;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmRstpPortTransStatGet
*
* DESCRIPTION:
*  		get rstp port transmition state
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpPortTransStatGet(UINT32 ulPortId, UINT8 *pucPortState)
{
    OPL_API_DATA_t stData;
    HAL_API_RSTP_PORT_STATE_GET_S stPara;
	OPL_STATUS iRet;

    #if 0
	iRet = dalRstpPortStateGet(ulPortId, pucPortState);
    #endif
    stPara.ulPortId = ulPortId;
    stPara.pucPortState = pucPortState;
    stData.apiId = HAL_API_RSTP_PORT_STATE_GET;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		return RSTP_PORT_STAT_GET_ERROR;
	}

	return NO_ERROR;
}

/* begin added by jiangmingli for zte */
/*******************************************************************************
* odmRstpPktHandleModeSet
*
* DESCRIPTION:
*  		set rstp handle mode
*
*	INPUTS:
*           ulMode: 0 --- trap to cpu
*                       1 --- forward
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpPktHandleModeSet(UINT32 ulMode)
{
    UINT8 ucMacNum;
    UINT8 ucIcos;
    UINT8 ucMacCtrl;
    STATUS iRet;
    UINT8 stpMac[6] = {0x01,0x80,0xC2,0x00,0x00,0x00};

    ucMacNum = 0;
    
    iRet = brgRsvMacCtrlHwRead(ucMacNum, &ucIcos, &ucMacCtrl);
    if (NO_ERROR != iRet)
    {
        return iRet;
    }
    switch (ulMode)
    {
        case 0: /* trap to cpu */
        {
            if (1 == ucMacCtrl)
            {
                return NO_ERROR;
            }

            brgRsvMacCtrlHwWrite(ucMacNum , ucIcos, 1);
            #if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            dalAclRstpAdd(0);
            #endif

            break;
        }
        case 1: /* forward */
        {
            if (2 == ucMacCtrl)
            {
                return NO_ERROR;
            }

            brgRsvMacCtrlHwWrite(ucMacNum , ucIcos, 2);
            #if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            dalAclRstpAdd(1);
            #endif

            break;
        }
        default:
        {
            return ERR_INVALID_PARAMETERS;
        }
    }

    
    iRet = vosConfigUInt32Set(CFGFILE_RSTP,
						      RSTP_SECTION_BRD,
						      RSTP_BRD_KEY_U32_HANDLE_MODE,
						      ulMode);

    return NO_ERROR;
}

/*******************************************************************************
* odmRstpPktHandleModeGet
*
* DESCRIPTION:
*  		get rstp handle mode
*
*	INPUTS:
*           pulMode: 0 --- trap to cpu
*                         1 --- forward
*                         0xffffffff --- not support
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		
* 	SEE ALSO: 
*/
STATUS odmRstpPktHandleModeGet(UINT32 *pulMode)
{
    UINT8 ucMacNum;
    UINT8 ucIcos;
    UINT8 ucMacCtrl;
    STATUS iRet;

    ucMacNum = 0;
    
    iRet = brgRsvMacCtrlHwRead(ucMacNum, &ucIcos, &ucMacCtrl);
    if (NO_ERROR != iRet)
    {
        return iRet;
    }

    if (1 == ucMacCtrl)
    {
        *pulMode = 0;
    }
    else if (2 == ucMacCtrl)
    {
        *pulMode = 1;
    }
    else
    {
        *pulMode = 0xffffffff;
    }

    return NO_ERROR;
}
/* end added by jiangmingli for zte */

/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_cmd_storm.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 19, 2008
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_storm.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"

#include "odm_storm.h"
#include "odm_port.h"

#if defined(ONU_1PORT)
    #define STORM_CONTROL_SWITCH_PARAM_INDEX 2
#else
    #define STORM_CONTROL_SWITCH_PARAM_INDEX 3
#endif

/*****************************************************************************
 *cliCmdStormCtrlRateSet  - set storm type and status
 *DESCRIPTION
 *.
 *Input:N/A.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/
STATUS cliCmdStormCtrlFrameSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	int retVal;
	char   portbuf[255];	  
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId = 1;

	int stormType = 0;
	int status = 0;

	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

#ifdef ONU_1PORT
    stormType= 2; /*psPara[STORM_CONTROL_SWITCH_PARAM_INDEX-1].u;*/ /*only support broadcast*/
    status = psPara[1].u;
#else
    stormType= psPara[2].u-1;
    status = psPara[3].u;
#endif

	if (1 == status)
	{
		status = TRUE;
	}
	else if(2 == status)
	{
		status = FALSE;
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput storm status error.\r\n");
		return ERROR;
	}
       
	if (psPara[0].u == 1)
	{
#ifdef ONU_1PORT
       /* vosPrintf(pstEnv->nWriteFd, "\r\nodmStormCtrlFrameSet called, stormType:%d.\r\n", stormType); */
        retVal = odmStormCtrlFrameSet(portId, stormType, status);
        if(retVal != OK)
        {
        	vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
        	return retVal;
        }	
#else
		vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
		vosStrCpy(portbuf,psPara[1].p);
		retVal = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);

		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
			return ERROR;
		}
		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			if(portArry[portId] == 1)
			{
				retVal = odmStormCtrlFrameSet(portId, stormType, status);
				if(retVal != OK)
				{
					vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
					return retVal;
				}									
			}
		}
#endif
	}
	else if (psPara[0].u == 2)
	{
	       #if defined(ONU_4PORT_AR8327)
		retVal = odmStormCtrlFrameSet(0, stormType, status);
		#else
		retVal = odmStormCtrlFrameSet(6, stormType, status);
		#endif

		if(retVal != OK)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
			return retVal;
		}
	}
	
	return OK;
}
	



/*****************************************************************************
 *cliCmdStormCtrlRateSet  - set storm rate limit
 *DESCRIPTION
 *.
 *Input:N/A.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/
STATUS cliCmdStormCtrlRateSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	int retVal;
	char   portbuf[255];	  
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId = 1;

	int stormType = 0;
	int status = 0;

	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

	if (psPara[0].u == 1)
	{
    	#ifdef ONU_1PORT
     	retVal = odmStormCtrlRateSet(portId, psPara[STORM_CONTROL_SWITCH_PARAM_INDEX-1].u);
    	if(retVal != OK)
    	{
    		vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
    		return retVal;
    	}	
    	#else
		vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
		vosStrCpy(portbuf,psPara[1].p);
		retVal = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);

		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
			return ERROR;
		}

		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			if(portArry[portId] == 1)
			{
				retVal = odmStormCtrlRateSet(portId, psPara[STORM_CONTROL_SWITCH_PARAM_INDEX-1].u);
				if(retVal != OK)
				{
					vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
					return retVal;
				}									
			}
		}
		#endif
	}
	else if (psPara[0].u == 2)
	{
	       #if defined(ONU_4PORT_AR8327)
		retVal = odmStormCtrlRateSet(0, psPara[STORM_CONTROL_SWITCH_PARAM_INDEX-1].u);
		#else
		retVal = odmStormCtrlRateSet(6, psPara[STORM_CONTROL_SWITCH_PARAM_INDEX-1].u);
		#endif

		if(retVal != OK)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nset storm failed.\r\n");
			return retVal;
		}
	}
	
	return OK;
}


/*****************************************************************************
 *cliCmdStormShow - show storm config info 
 *DESCRIPTION
 *.
 *Input:N/A.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/

#ifdef ONU_1PORT
STATUS cliCmdStormShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
   	UINT32 stormType;
	UINT32 stormStatus;
	UINT32 ratelimit;
	UINT32 portId,i;
	UINT32 retVal = OK;
	UINT32 statusStorm[2]={0};
   	UINT8 *cliStatusString[] = {"Disable","Enable",NULL};
	UINT8 *cliTypeString[] = {"Broadcast  ", "Ratelimt   ", NULL};
	UINT8  cliRateString[2][32] = {"32kbps", "32kbps"};
        
    cliShowStart(pstEnv);
    vosPrintf(pstEnv->nWriteFd, "   %-9s%-9s\t%-9s\r\n",
        "",
        "Upstream",
        "Downstream");
    vosPrintf(pstEnv->nWriteFd, "----------- --------  \t-------- \r\n");	

    portId = 1;
    for (i =0; i < 2; i++){
  		retVal = odmStormCtrlFrameGet(portId,2,&stormStatus);
		if(OK == retVal){
			statusStorm[i] = stormStatus;   
		}
    		
        retVal = odmStormCtrlRateGet(portId,&ratelimit);
    	if(OK == retVal){
    		if (1 != ratelimit){
    		    vosSprintf(cliRateString[i], "%dkbps ", ratelimit);
    		}
    	}
    	portId = 6;
    }

	vosPrintf(pstEnv->nWriteFd, "%-10s %-12s%-12s\r\n",
    	cliTypeString[0],
    	cliStatusString[statusStorm[0]],
    	cliStatusString[statusStorm[1]]);  
    vosPrintf(pstEnv->nWriteFd, "%-10s %-12s%-12s\r\n",
    	cliTypeString[1],
    	cliRateString[0],
    	cliRateString[1]);  
    	
    cliShowEnd(pstEnv);

	return OK;
 }
#elif defined(ONU_4PORT_AR8327)
STATUS cliCmdStormShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	UINT32 stormType;
	UINT32 stormStatus;
	UINT32 ratelimit;
	UINT32 portId;
	UINT32 statusStorm[5][3]={0};
	UINT32 retVal = OK;
	UINT8 *cliStatusString[] = {"Disable","Enable",NULL};
	UINT8 *cliTypeString[] = {	"Unicast    ","Multicast  ","Broadcast  ",NULL};
	UINT8  cliRateString[5][32] = {"32kbps", "32kbps"};

	cliShowStart(pstEnv);

	vosPrintf(pstEnv->nWriteFd, "   %-9s %-9s %-9s %-9s %-9s %-9s\r\n",
		"",
		"down",
		"port1",
		"port2",
		"port3",
		"port4");
	vosPrintf(pstEnv->nWriteFd, "----------- --------  --------  -------- -------- --------\r\n");	
	for (portId=0;portId<=ODM_NUM_OF_PORTS;portId++)
	{				
		for(stormType =0; stormType<3; stormType++)
		{
			retVal = odmStormCtrlFrameGet(portId,stormType,&stormStatus);
			if(OK != retVal)
			{
				return retVal;
			}
			statusStorm[portId][stormType] = stormStatus;    
		}

		retVal = odmStormCtrlRateGet(portId,&ratelimit);
		if(OK == retVal){
    		if (1 != ratelimit){
    		    vosSprintf(cliRateString[portId], "%dkbps ", ratelimit);
    		}
    	}
	}

	for(stormType =0 ;stormType<3 ;stormType++)
	{
		vosPrintf(pstEnv->nWriteFd, "%-9s %-9s %-9s %-9s %-9s %-9s\r\n",
			cliTypeString[stormType],cliStatusString[statusStorm[0][stormType]],cliStatusString[statusStorm[1][stormType]],
			cliStatusString[statusStorm[2][stormType]],cliStatusString[statusStorm[3][stormType]],cliStatusString[statusStorm[4][stormType]]);  
	}

	vosPrintf(pstEnv->nWriteFd, "%-9s %-9s %-9s %-9s %-9s %-9s\r\n",
		"RateLimit  ",cliRateString[0],cliRateString[1],
		cliRateString[2],cliRateString[3],
		cliRateString[4]);
	cliShowEnd(pstEnv);

	return OK;
}
 #else
STATUS cliCmdStormShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	UINT32 stormType;
	UINT32 stormStatus;
	UINT32 ratelimit;
	UINT32 portId;
	UINT32 statusStorm[6][3]={0};
	UINT32 rateStorm[6] ={0};
	UINT32 retVal = OK;
	UINT8 *cliStatusString[] = {"Disable","Enable",NULL};
	UINT8 *cliTypeString[] = {	"Unicast    ","Multicast  ","Broadcast  ",NULL};
	UINT8 *cliRateString[] = {	"1KPPS","2KPPS","4KPPS","8KPPS","16KPPS","32KPPS","64KPPS","128KPPS","256KPPS","512KPPS","1024KPPS",NULL};

	cliShowStart(pstEnv);

	vosPrintf(pstEnv->nWriteFd, "   %-9s %-9s %-9s %-9s %-9s %-9s\r\n",
		"",
		"port1",
		"port2",
		"port3",
		"port4",
		"down");
	vosPrintf(pstEnv->nWriteFd, "----------- --------  --------  -------- -------- --------\r\n");	
	for (portId=ODM_START_PORT_NUN;portId<=6;portId++)
	{
		if (5 == portId)
		{
			continue;
		}			
			
		for(stormType =0; stormType<3; stormType++)
		{
			retVal = odmStormCtrlFrameGet(portId,stormType,&stormStatus);
			if(OK != retVal)
			{
				return retVal;
			}
			statusStorm[portId-1][stormType] = stormStatus;    
		}

		retVal = odmStormCtrlRateGet(portId,&ratelimit);
		if(OK != retVal)
		{
			return retVal;
		}
		rateStorm[portId-1] = ratelimit;
	}

	for(stormType =0 ;stormType<3 ;stormType++)
	{
		vosPrintf(pstEnv->nWriteFd, "%-9s %-9s %-9s %-9s %-9s %-9s\r\n",
			cliTypeString[stormType],cliStatusString[statusStorm[0][stormType]],cliStatusString[statusStorm[1][stormType]],
			cliStatusString[statusStorm[2][stormType]],cliStatusString[statusStorm[3][stormType]],cliStatusString[statusStorm[5][stormType]]);  
	}

	vosPrintf(pstEnv->nWriteFd, "%-9s %-9s %-9s %-9s %-9s %-9s\r\n",
		"RateLimit  ",cliRateString[rateStorm[0]-1],cliRateString[rateStorm[1]-1],
		cliRateString[rateStorm[2]-1],cliRateString[rateStorm[3]-1],
		cliRateString[rateStorm[5]-1]);
	cliShowEnd(pstEnv);

	return OK;
}
#endif




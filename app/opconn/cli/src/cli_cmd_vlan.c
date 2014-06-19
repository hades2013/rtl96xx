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
* FILENAME:  cli_cmd_vlan.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_vlan.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"

#include "mc_control.h"
//wfxu #include "dal_vlan.h"
#include "odm_port.h"
#include "odm_vlan.h"
#include "opconn_usr_ioctrl.h"


STATUS cliCmdLocalSwitchEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT8 i;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    for (i = ODM_START_PORT_NUN;i <= ODM_NUM_OF_PORTS;i++)
    {
        odmSetLocalSwitch(i,pstPt->i-1);
    }
    return OK;
}
STATUS cliCmdCreateVlanName(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    ret = odmCreateVlanName(pstPt->p);
    if (ret == VLAN_NAME_EXIST_ERR)
    {
        vosPrintf(pstEnv->nWriteFd,"%%Vlan name is existed!\r\n");
        return ERROR;
    }
    return OK;
}
STATUS cliCmdDeleteVlanName(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    ret = odmDeleteVlanName(pstPt->p);
    switch (ret)
    {
        case VLAN_NAME_EXIST_ERR:
                vosPrintf(pstEnv->nWriteFd,"%%This Vlan name is not existed! \r\n");
                return ERROR;
        case  VLAN_GERNEL_ERROR:
                vosPrintf(pstEnv->nWriteFd,"%%This Vlan name has add tag! \r\n");
                return ERROR;
        default:
               break;
    }

    return OK;
}
STATUS cliCmdClearVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

     if (NULL == pstEnv)
    {
        return ERROR;
    }
    odmFlushVlanAll();
    return OK;
}
STATUS cliCmdSetVlanTag(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT16 vid;

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }
    vid = pstPt[1].u;
/*
    vid = strtoul(pstPt[1].p,NULL,0);
    if (vid > 0xfff)
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error vlan value(0-4095)\r\n");
        return ERROR;
    }
*/
    ret = odmSetVlanNameTag(pstPt[0].p,vid);
    switch (ret)
    {
        case VLAN_ID_EXIST_ERR:
             vosPrintf(pstEnv->nWriteFd,"%%This tag is existed! \r\n");
             return ERROR;
        case VLAN_NAME_EXIST_ERR:
             vosPrintf(pstEnv->nWriteFd,"%%Vlan name is not existed! \r\n");
             return ERROR;
        case VLAN_NAME_ID_MAPERROR:
             vosPrintf(pstEnv->nWriteFd,"%%This Vlan has add tag! \r\n");
             return ERROR;
        case NO_ERROR :
              break;
		default:
			 vosPrintf(pstEnv->nWriteFd,"%%General error,cannot create successful! \r\n");
             return ERROR;
    }
    return OK;
}
STATUS cliCmdAddVlanPort(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT32  lport;
    UINT8  portnum;
    UINT16 vid;
    VLAN_EGRESS_TYPE_E type;

    char   portbuf[255];
    UINT32 portArry[ODM_NUM_OF_PORTS + 1];

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }

    vosStrCpy(portbuf,pstPt[0].p);
    ret = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);
	if(-1 == ret)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
		return ERROR;
	}
    if (odmGetVlanNameVid(pstPt[1].p,&vid) != NO_ERROR)
    {
        vosPrintf(pstEnv->nWriteFd,"%%Vlan name has no added tag! \r\n");
        return ERROR;
    }
	for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
	{
	    if(1 != portArry[portnum])
        {
            continue;
        }
        if (pstPt[2].i == 1) type = TAG;
        else if (pstPt[2].i == 2) type = UNTAG;
        else if (pstPt[2].i == 3) type = UNMODIFY;
        else return ERROR;
        if (odmAddVlanPort(portnum,vid,type) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd,"%%Add vlan member failed:! \r\n");
            return ERROR;
        }
	}
    return OK;
}
STATUS cliCmdDelVlanPort(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT32  lport;
    UINT8  portnum;
    UINT16 vid;

    char   portbuf[255];
    UINT32 portArry[ODM_NUM_OF_PORTS + 1];

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }

    vosStrCpy(portbuf,pstPt[0].p);
    ret = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);
	if(-1 == ret)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
		return ERROR;
	}
	if (NO_ERROR != odmCheckVlanNameExist(pstPt[1].p))
	{
	    vosPrintf(pstEnv->nWriteFd, "%%Vlan name is not existed!\r\n",portbuf);
		return ERROR;
	}

    if (NO_ERROR != odmGetVlanNameVid(pstPt[1].p,&vid) )
    {
        vosPrintf(pstEnv->nWriteFd, "%%Vlan name has not added tag!\r\n",portbuf);
        return ERROR;
    }
	for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
	{
	    if(1 != portArry[portnum])
        {
            continue;
        }

        ret = odmMoveVlanPort(portnum,vid);
        if (VLAN_GERNEL_ERROR == ret)
        {
            vosPrintf(pstEnv->nWriteFd, "%%Port [%d] is not vlan member!\r\n",portnum);
        }
	}
    return OK;
}
STATUS cliCmdShowVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT8  portnum;
    UINT16 vid;

    UINT8  mode = 0;
    UINT32 portlist = 0;

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }
    if (pstPt[0].i == 1)
    {
        odmShowAllVlan(pstEnv->nWriteFd,1);
    }
    else
    {
        if (odmCheckVlanNameExist(pstPt[1].p) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd,"%%Vlan name is not existed!\r\n");
            return ERROR;
        }
        if (odmGetVlanNameVid(pstPt[1].p,&vid) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd,"%%Vlan name has no added tag! \r\n");
            return ERROR;
        }

        cliShowStart(pstEnv);
        vosPrintf(pstEnv->nWriteFd, "VLAN Information:\r\n");
        vosPrintf(pstEnv->nWriteFd, "  %-20s : %s\r\n","VLAN Name",pstPt[1].p);
        vosPrintf(pstEnv->nWriteFd, "  %-20s : %d\r\n","VLAN ID",vid);

        vosPrintf(pstEnv->nWriteFd, "\r\n");
        vosPrintf(pstEnv->nWriteFd, "   Port   Type\r\n");
        vosPrintf(pstEnv->nWriteFd, "  ------ ------------\r\n");
        odmGetVlanPortList(vid,&portlist);

        for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    	{
    	    if (utilStrStartWith(pstPt[1].p,"mc_") == 1)
    	    {
    	        odmMulticastTagstripGet(portnum,&mode);
    	        if (MC_TAG_OPER_MODE_STRIP == mode)
    	        {
    	            vosPrintf(pstEnv->nWriteFd, "   %5d  %s\r\n",portnum,"untag");
    	        }
    	        else
    	        {
    	            vosPrintf(pstEnv->nWriteFd, "   %5d  %s\r\n",portnum,"tag");
    	        }
    	    }
    	    else
    	    {
        	    if ((portlist&(1<<((portnum-1)*3))) > 0)
        	    {
            	    switch ((portlist>>((portnum-1)*3 +1))&0x3)
            	    {
                	    case TAG:
                    	    vosPrintf(pstEnv->nWriteFd, "   %5d  %s\r\n",portnum,"tag");
                    	    break;
                        case UNTAG:
                    	    vosPrintf(pstEnv->nWriteFd, "   %5d  %s\r\n",portnum,"untag");
                    	    break;
                        case UNMODIFY:
                    	    vosPrintf(pstEnv->nWriteFd, "   %5d  %s\r\n",portnum,"unmodify");
                    	    break;
                    	 default :
                            cliShowEnd(pstEnv);
                    	    return ERROR;

            	    }
        	    }
    	    }
    	}
        cliShowEnd(pstEnv);
    }
    return OK;
}
STATUS cliCmdShowVlanlist(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT32  lport[20];
    UINT8  portnum;
    UINT16 vid;

     if (NULL == pstEnv)
    {
        return ERROR;
    }
    cliShowStart(pstEnv);
    odmShowAllVlan(pstEnv->nWriteFd,0);
    cliShowEnd(pstEnv);
    return NO_ERROR;
}
STATUS cliCmdShowVlanAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT32  lport[20];
    UINT8  portnum;
    UINT16 vid;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    odmShowAllVlan(pstEnv->nWriteFd,1);
    return NO_ERROR;
}
STATUS cliCmdShowVlanEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;

    UINT8  step;
    UINT32  lport[20];
    UINT8  portnum;
    UINT16 vid;

    if (NULL == pstEnv) 
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
    //dalShowChipVlanEntry();
    cliShowEnd(pstEnv);

    return OK;
}

STATUS cliCmdVlanAggAction(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
  int ret;

  UINT8 portnum;
  UINT16 targetVlan, aggregatedVlan;
  UINT32 vlanMode;

  char   portbuf[255];
  UINT32 portArry[ODM_NUM_OF_PORTS + 1];

  if ((NULL == pstEnv) || (NULL == pstPt))
  {
      return ERROR;
  }

  vosStrCpy(portbuf,pstPt[1].p);
  ret = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);
  if(-1 == ret)
  {
      vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
      return ERROR;
  }

  targetVlan = pstPt[2].u;
  aggregatedVlan = pstPt[3].u;

  for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
  {
      if(1 != portArry[portnum])
      {
          continue;
      }
#if 1
	ret = odmPortVlanModeGet(portnum,&vlanMode);
	if(OK != ret)
	{
    	vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",ret);
    	return ret;
	}

	if(ODM_VLAN_AGGREGATION != vlanMode)
	{
    	vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in vlan aggregation mode.please set to aggregation mode first.\r\n",portnum);
    	return PORT_NOT_AT_VLAN_TRUNK_MODE;
	}
#endif
      if(pstPt[0].u==1) { /* create */
        ret = odmVlanAggAdd(portnum,targetVlan,aggregatedVlan);
        if(ret!=OPL_OK) {
          vosPrintf(pstEnv->nWriteFd,"%%Failed to add entry for VLAN aggregation\r\n");
          return ret;
        }
      }
      if(pstPt[0].u==2) { /* delete */
        ret = odmVlanAggDel(portnum,targetVlan,aggregatedVlan);
        if(ret!=OPL_OK) {
          vosPrintf(pstEnv->nWriteFd,"%%Failed to delete entry for VLAN aggregation\r\n");
          return ret;
        }
      }

  }
  return OK;
}
#if 0
static inline int
clStringGetByMac(unsigned char* mac_string, unsigned char* mac) {
  if (mac == NULL || mac_string == NULL) {
    return -1;
  }

  vosSprintf(mac_string,
             "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0],
             mac[1],
             mac[2],
             mac[3],
             mac[4],
             mac[5]);

  return 0;
}
#endif
STATUS cliCmdShowVlanAggAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    VLAN_AGG_DS_t *e;
    int i,j;
    char mac_string[128];

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    ret = 0;
    e = NULL;
    vosMemSet(mac_string, 0, 128);

    vosPrintf(pstEnv->nWriteFd, "VLAN aggregation information:\r\n");

	for(j=1; j<ODM_NUM_OF_PORTS+1; j++) {
		vosPrintf(pstEnv->nWriteFd, "  %-20s : %d\r\n","Egress port",j);
		e = odmVlanAggGet();
		if(e) {
			for(i=1; i<ODM_MAX_NUM_OF_AGGRE_ENTRY; i++) {
			  e++;
			  if(e->egressPort==j) {
			    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n","Target VLAN",e->targetVlan);
			    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n","Aggregated VLAN",e->aggedVlan);
			    clStringGetByMac(mac_string, e->aggedMac);
			    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n","Mac address",mac_string);
			  }
			}
		}
	}
    

   return NO_ERROR;
}

STATUS cliCmdShowVlanAgg(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    VLAN_AGG_DS_t *e;
    int i;
    char mac_string[128];

 	UINT8 portnum;
  	char   portbuf[255];
  	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 vlanMode;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    ret = 0;
    e = NULL;
    vosMemSet(mac_string, 0, 128);

    vosStrCpy(portbuf,pstPt[0].p);
    ret = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);
    if(-1 == ret)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
        return ERROR;
    }

    vosPrintf(pstEnv->nWriteFd, "VLAN aggregation information:\r\n");

    e = odmVlanAggGet();

  if(e) {
    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
      if(1 != portArry[portnum])
      {
          continue;
      }

#if 1
	  ret = odmPortVlanModeGet(portnum,&vlanMode);
	  if(OK != ret)
	  {
		  vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",ret);
		  return ret;
	  }

	  if(ODM_VLAN_AGGREGATION != vlanMode)
	  {
		  vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in vlan aggregation mode.please set to aggregation mode first.\r\n",portnum);
		  return PORT_NOT_AT_VLAN_TRUNK_MODE;
	  }
#endif

      vosPrintf(pstEnv->nWriteFd, "  %-20s : %d\r\n","Egress port",portnum);
      e = odmVlanAggGet();
      
      for(i=1; i<ODM_MAX_NUM_OF_AGGRE_ENTRY; i++) {
        e++;
        if(e->egressPort==portnum) {
          vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n","Target VLAN",e->targetVlan);
          vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n","Aggregated VLAN",e->aggedVlan);
          clStringGetByMac(mac_string, e->aggedMac);
          vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n","Mac address",mac_string);
        }
      }
    }
  }

   return NO_ERROR;
}

STATUS cliCmdQinQMode(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 mode;
	
	if(NULL == pstEnv) 
	{
		return ERROR;
	}
	if (1 == pstPara[0].i)
    {
        mode = 0; /*base mode*/
    }
    else
    {
        mode = 1; /*flexible mode*/
    }
	
	retVal = dalVlanQinQModeSet(mode);	
	if(OPL_OK != retVal)
	{
		printf("%s %d QinQ mode = %d  %d\n",__FUNCTION__,__LINE__,mode,pstPara[0].i);
		return OPL_ERROR;
	}
	if(0 == mode)
	{
		printf("configure QinQ mode to base mode succesfully! \n");
	}
	else
	{
		printf("configure QinQ mode to flexible mode succesfully! \n");
	}
	return retVal;
}

STATUS cliCmdQinQBase(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 mode;
	
	if(NULL == pstEnv) 
	{
		return ERROR;
	}
	if (1 == pstPara[0].i) /*add 1 ;delete 2*/
    {
		retVal = dalVlanQinQBaseAdd(pstPara[1].u,pstPara[2].u,pstPara[3].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d QinQ add base vlan %d remarkPri %d insertPri %d failed !\n",__FUNCTION__,__LINE__,pstPara[1].u,pstPara[2].u,pstPara[3].u);
			return OPL_ERROR;
		}
		printf("Add QinQ vlan  %d remarkPri %d insertPri %d  succesffuly! \n",pstPara[1].u,pstPara[2].u,pstPara[3].u);
    }
    else
    {
		retVal = dalVlanQinQBaseDel(pstPara[1].u,pstPara[2].u,pstPara[3].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d Delete QinQ vlan %d remarkPri %d insertPri %d  failed !\n",__FUNCTION__,__LINE__,pstPara[1].u,pstPara[2].u,pstPara[3].u);
			return OPL_ERROR;
		}
		printf("Delete QinQ base vlan  %d remarkPri %d insertPri %d  succesffuly! \n",pstPara[1].u,pstPara[2].u,pstPara[3].u);		

    }

	return retVal;
}


STATUS cliCmdQinQTransparent(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 mode;
	
	if(NULL == pstEnv) 
	{
		return ERROR;
	}
	
	if (1 == pstPara[0].i)
    {
		retVal = dalVlanTransparentAdd(pstPara[1].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d add QinQ vlan = %d  failed ! \n",__FUNCTION__,__LINE__,pstPara[1].u);
			return OPL_ERROR;
		}
		printf("Add QinQ transparent vlan  %d succesffuly! \n",pstPara[1].u);
		
    }
    else
    {
		retVal = dalVlanTransparentDel(pstPara[1].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d Delete QinQ vlan = %d  failed !\n",__FUNCTION__,__LINE__,pstPara[1].u);
			return OPL_ERROR;
		}
		printf("Delete QinQ transparent vlan  %d succesffuly! \n",pstPara[1].u);

    }

	return retVal;
}


STATUS cliCmdQinQFlexible(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 mode;
	
	if(NULL == pstEnv) 
	{
		return ERROR;
	}
	
	if (1 == pstPara[0].i)
    {
		retVal = dalVlanQinQFlexibleAdd(pstPara[1].u,pstPara[2].u,pstPara[3].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d add QinQ svlan = %d  cvlan= %d sPriority=%d failed !\n",__FUNCTION__,__LINE__,pstPara[1].u,pstPara[2].u,pstPara[3].u);
			return OPL_ERROR;
		}
		printf("Add QinQ flexible svlan %d  cvlan %d sPriority %d successfully !\n",pstPara[1].u,pstPara[2].u,pstPara[3].u);
		
    }
    else
    {
		retVal = dalVlanQinQFlexibleDel(pstPara[1].u,pstPara[2].u,pstPara[3].u);	
		if(OPL_OK != retVal)
		{
			printf("%s %d delete QinQ svlan = %d  cvlan= %d  sPriority= %d failed !\n",__FUNCTION__,__LINE__,pstPara[1].u,pstPara[2].u,pstPara[3].u);
			return OPL_ERROR;
		}
		printf("Delete QinQ flexible svlan %d  cvlan %d sPriority %d successfully !\n",pstPara[1].u,pstPara[2].u,pstPara[3].u);

    }

	return retVal;
}




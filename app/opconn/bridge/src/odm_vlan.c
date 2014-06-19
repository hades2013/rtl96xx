/*
=============================================================================
     File Name: brg_vlan.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved

NOTES: opconn ONU chip not support 802.1q vlan function.
so implement by switch chip.
Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
wangliang 			2008/8/29		Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/

#include "defs.h"
#include "log.h"
#include "mc_control.h"
#include "hal.h"
#include "odm_port.h"
#include "odm_vlan.h"
#include "stats.h"

#define MODULE MOD_VLAN

#define VLAN_TRACE() OP_DEBUG(DEBUG_LEVEL_INFO, "\n%s,%s,%d!\r\n",__FILE__,__FUNCTION__,__LINE__)


/*******************************************************************************
* odmSetLocalSwitch
*
* DESCRIPTION:
*  		this function support set the local siwtch enable/disable.
*
*	INPUTS:
          lport:user logic port.
          enable  :elable/disable
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS odmSetLocalSwitch
(
    UINT8  lport,
    UINT8  enable
) {
    OPL_STATUS ret;
    UINT32 localswitchflag;
	OPL_API_DATA_t        stData;
    HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t stvlan;

	if(odmPortRangeCheck(lport))
	{
		return PORT_INVALID;
	}
    stvlan.port = lport;
    stvlan.enable = enable;
    stData.apiId = HAL_API_VLAN_LOCAL_SWITCH_SET;
    stData.length= sizeof(HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t);
    stData.param = (void *)&stvlan;
    ret = halAppApiCallSync(&stData);
    ret += stvlan.ret;

    if(ret == OPL_OK)
    {
        /*port local switch flag bit map*/
        localswitchflag = vosConfigUInt32Get(VLAN_CONFIG_NAME,
            LOCAL_SWITCH_SEC, LOCAL_SWITCH_KEY, 0);
        if (TRUE == enable)
        {
            localswitchflag |= (1<<lport);
            vosConfigUInt32Set(VLAN_CONFIG_NAME,LOCAL_SWITCH_SEC,LOCAL_SWITCH_KEY,localswitchflag);
        }
        else
        {
            localswitchflag &= ~(1<<lport);
            vosConfigUInt32Set(VLAN_CONFIG_NAME,LOCAL_SWITCH_SEC,LOCAL_SWITCH_KEY,localswitchflag);
        }
    }
    return OPL_OK;
}

/*******************************************************************************
* odmGetLocalSwitch
*
* DESCRIPTION:
*  		this function support Get the local siwtch enable/disable.
*
*	INPUTS:
          lport:user logic port.
          enable  :elable/disable
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS odmGetLocalSwitch
(
    UINT8  lport,
    UINT8  *enable
)
{
    UINT32 localswitchflag;
    if (enable == NULL)
    {
        return ERR_NULL_POINTER;
    }
    if(odmPortRangeCheck(lport))
	{
		return PORT_INVALID;
	}
    /* default local switch disable */
    localswitchflag = vosConfigUInt32Get(VLAN_CONFIG_NAME,
        LOCAL_SWITCH_SEC, LOCAL_SWITCH_KEY, 0);
    *enable = ((localswitchflag >> lport) & 0x1);
    return OPL_OK;
}
/*******************************************************************************
* odmCheckVlanNameExist
*
* DESCRIPTION:
*  		this function check this vlan name is exist.
*
*	INPUTS:
          name:vlan name.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmCheckVlanNameExist
(
    char *name
)
{
    if (NULL == name)
    {
        return ERR_NULL_POINTER;
    }
    return (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,name));
}
/*******************************************************************************
* odmCheckVlanIDExist
*
* DESCRIPTION:
*  		this function check this vlan name is exist.
*
*	INPUTS:
          name:vlan name.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmCheckVlanIDExist
(
    UINT16 vid
)
{
    char section[16] = {0};

    vosSprintf(section, VLAN_SESSION, vid);
    /*if this vid has been designated.cannot designate again*/
    if (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,section) != 0)
    {
        return VLAN_ID_EXIST_ERR;
    }
    return NO_ERROR;
}
/*******************************************************************************
* odmCreateVlanName
*
* DESCRIPTION:
*  		this function support create vlan name.
*
*	INPUTS:
          name:vlan name.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmCreateVlanName
(
    char *name
)
{
    int status;
    if (NULL == name)
    {
        return ERR_NULL_POINTER;
    }
    status = odmCheckVlanNameExist(name);
    if (NO_ERROR == status)
    {
        return VLAN_NAME_EXIST_ERR;
    }
    if (vosConfigSectionCreate(VLAN_CONFIG_NAME,name) != NO_ERROR)
    {
        return ERR_CONFIG_SET_FAIL;
    }
    vosConfigValueSet(VLAN_CONFIG_NAME,name,VLAN_NAME,name);
    return NO_ERROR;
}
/*******************************************************************************
* odmCreateVlanID
*
* DESCRIPTION:
*  		this function support create a vlan.
*
*	INPUTS:
          vid:vlan id.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmCreateVlanID
(
    UINT16 vid
)
{
	int ret;
	int port_index;

	char section[16] = {0};
	OPL_API_DATA_t        stData;
    HAL_VLAN_CREATE_PAR_t stvlan;

    vosSprintf(section, VLAN_SESSION, vid);

    /*if this vid has been designated.cannot designate again*/
    if (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,section) == 0)
    {
        VLAN_TRACE();
        return VLAN_ID_EXIST_ERR;
    }
    stvlan.vid = vid;
    stData.apiId = HAL_API_VLAN_CREATE;
    stData.length= sizeof(HAL_VLAN_CREATE_PAR_t);
    stData.param = (void *)&stvlan;
    ret = halAppApiCallSync(&stData);
    ret += stvlan.ret;

    if (ret != NO_ERROR)
    {
        VLAN_TRACE();
        return ret;
    }
    return NO_ERROR;
}
/*******************************************************************************
* odmSetVlanNameTag
*
* DESCRIPTION:
*  		this function support set a vlan tag to vlan name.
*
*	INPUTS:
          name:vlan name.
*		  vid :vlan id.
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmSetVlanNameTag
(
    char  *name,
    UINT16 vid
)
{
    int status;
    UINT32 port;
    UINT32 pvid = 1;

    char section[16] = {0};

    if (NULL == name)
    {
        return ERR_NULL_POINTER;
    }
    status = odmCheckVlanNameExist(name);
    if (NO_ERROR != status)
    {
        return VLAN_NAME_EXIST_ERR;
    }
    if (vosConfigUInt32Get(VLAN_CONFIG_NAME,name,VLAN_ID_KEY,0) != 0)
    {
        VLAN_TRACE();
        return VLAN_NAME_ID_MAPERROR;
    }
    vosSprintf(section, VLAN_SESSION, vid);
    /*if this vid has been designated.cannot designate again*/
    if (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,section) == 0)
    {
        VLAN_TRACE();
        return VLAN_ID_EXIST_ERR;
    }
    for (port = ODM_START_PORT_NUN;port <= ODM_NUM_OF_PORTS;port++)
    {
        odmPortDefaultVlanGet(port,&pvid);
        if (pvid == vid)
        {
            VLAN_TRACE();
            return VLAN_GERNEL_ERROR;
        }
    }
    /*create a vlan to switch chip*/
    if (odmCreateVlanID(vid) != NO_ERROR)
    {
        return VLAN_GERNEL_ERROR;
    }
    if (vosConfigUInt32Set(VLAN_CONFIG_NAME,name, VLAN_ID_KEY,vid) != NO_ERROR)
    {
        return ERR_CONFIG_SET_FAIL;
    }
    vosConfigSectionCreate(VLAN_CONFIG_NAME,section);
    return NO_ERROR;
}
/*******************************************************************************
* odmDeleteVlanID
*
* DESCRIPTION:
*  		this function support delete a vlan.
*
*	INPUTS:
          vid:vlan id.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmDeleteVlanID
(
    UINT16 vid
)
{
	int ret;
	char section[16] = {0};

	OPL_API_DATA_t        stData;
    HAL_VLAN_CREATE_PAR_t stvlan;

    vosSprintf(section, VLAN_SESSION, vid);
    if (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,section) != 0)
    {
        VLAN_TRACE();
        return VLAN_ID_EXIST_ERR;
    }
    stvlan.vid = vid;
    stData.apiId = HAL_API_VLAN_DELETE;
    stData.length= sizeof(HAL_VLAN_CREATE_PAR_t);
    stData.param = (void *)&stvlan;
    ret = halAppApiCallSync(&stData);
    ret += stvlan.ret;

    if (ret != NO_ERROR)
    {
        VLAN_TRACE();
        return ret;
    }


   if (vosConfigSectionDelete(VLAN_CONFIG_NAME,section) != NO_ERROR)
   {
        return ERR_CONFIG_SET_FAIL;
   }

    return NO_ERROR;
}
/*******************************************************************************
* odmDeleteVlanName
*
* DESCRIPTION:
*  		this function support delete vlan name.
*
*	INPUTS:
          name:vlan name.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmDeleteVlanName
(
    char *name
)
{
    UINT16 vid = 0;
    int    status;

    if (NULL == name)
    {
        return ERR_NULL_POINTER;
    }
    if (NO_ERROR != odmCheckVlanNameExist(name))
    {
        VLAN_TRACE();
        return VLAN_NAME_EXIST_ERR;
    }

    vid = vosConfigUInt32Get(VLAN_CONFIG_NAME,name,VLAN_ID_KEY,0);
    if (vid != 0)
    {
        status =  odmDeleteVlanID(vid);
        if ((NO_ERROR != status) &&(VLAN_ID_EXIST_ERR != status))
        {
            VLAN_TRACE();
            return VLAN_GERNEL_ERROR;
        }
    }

    vosConfigSectionDelete(VLAN_CONFIG_NAME,name);
    return NO_ERROR;
}
/*******************************************************************************
* odmGetVlanNameVid
*
* DESCRIPTION:
*  		this function support get vlan id according to vlan name.
*
*	INPUTS:
          name:vlan name.
*
*	OUTPUTS:
          vid  :vlan Id
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmGetVlanNameVid
(
    char  *name,
    UINT16 *vid
)
{
    UINT16 tmp = 0;
    if ((name == NULL)||(vid == NULL))
    {
        return ERR_NULL_POINTER;
    }
    tmp = vosConfigUInt32Get(VLAN_CONFIG_NAME,name,VLAN_ID_KEY,0);
    if ( tmp == 0)
    {
        return VLAN_GERNEL_ERROR;
    }
    *vid = tmp;

    return NO_ERROR;
}
/*******************************************************************************
* odmGetVlanIDName
*
* DESCRIPTION:
*  		this function support get vlan name according to vlan id.
*
*	INPUTS:
          vid  :vlan Id
*
*	OUTPUTS:
          name:vlan name.

*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			others vale is failed.
* 	SEE ALSO:
*/
int odmGetVlanIDName
(
    UINT16 vid,
    char  *name
)
{
    UINT16 tmp = 0;
    UINT8  count;
    UINT8  roop;
    UINT16 vlan;

    char   *session = NULL;
    if ((name == NULL)||(vid == 0))
    {
        return ERR_NULL_POINTER;
    }
    if (NO_ERROR != odmCheckVlanIDExist(vid))
    {
        return VLAN_ID_EXIST_ERR;
    }

    count = vosConfigSectionCount(VLAN_CONFIG_NAME);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(VLAN_CONFIG_NAME,roop,&session) == NO_ERROR)
        {
            vlan = vosConfigUInt32Get(VLAN_CONFIG_NAME,session,VLAN_ID_KEY,0xffff);
            if (vlan == vid)
            {
                vosStrCpy(name,session);
                return NO_ERROR;
            }
        }
    }

    return VLAN_GERNEL_ERROR;
}
/*******************************************************************************
* odmAddVlanPort
*
* DESCRIPTION:
*  		this function support add a port to vlan.
*
*	INPUTS:
          lport:user logic port.
          vid:vlan id.
          type:port vlan egress type.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
int odmAddVlanPort
(
    UINT8  lport,
    UINT16 vid,
    VLAN_EGRESS_TYPE_E type
)
{

	int    ret;

    UINT32 bitmap = 0;

	char section[16] = {0};

	OPL_API_DATA_t       stData;
    HAL_VLAN_MEMBER_ADD_PAR_t stvlan;

	if(odmPortRangeCheck(lport))
	{
		return PORT_INVALID;
	}
	vosSprintf(section, VLAN_SESSION, vid);
    bitmap  = vosConfigUInt32Get(VLAN_CONFIG_NAME,section,PORT_MAP_KEY,0);
    if ((bitmap&(1<<((lport - 1)*3)))&&(type == ((bitmap>>((lport-1)*3 +1))&0x3)))
    {
        VLAN_TRACE();
        return NO_ERROR;
    }
    /*add parameter*/
    stvlan.port  = lport;
    stvlan.vid   = vid;
    stvlan.type  = type;
    stData.apiId = HAL_API_VLAN_MEMBERADD;
    stData.length = sizeof(HAL_VLAN_MEMBER_ADD_PAR_t);
    stData.param = (void *)&stvlan;
    ret = halAppApiCallSync(&stData);
    ret += stvlan.ret;

    if (ret != NO_ERROR)
    {
        VLAN_TRACE();
        return ret;
    }
    /*updata local vlan--port map info*/

    bitmap |= (1<<((lport - 1)*3));
    bitmap &= ~(3<<((lport - 1)*3 + 1));
    bitmap |= (type<<((lport - 1)*3 + 1));
    if (vosConfigUInt32Set(VLAN_CONFIG_NAME,section, PORT_MAP_KEY,bitmap) != NO_ERROR)
    {
        VLAN_TRACE();
        return ERR_CONFIG_SET_FAIL;
    }

    return NO_ERROR;
}
/*******************************************************************************
* odmMoveVlanPort
*
* DESCRIPTION:
*  		this function support move a port to vlan.
*
*	INPUTS:
          lport:user logic port.
          vid:vlan id.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
int odmMoveVlanPort
(
    UINT8  lport,
    UINT16 vid
)
{
	int   ret;

    UINT32 bitmap = 0;

    char section[16] = {0};

    OPL_API_DATA_t       stData;
    HAL_VLAN_MEMBER_DEL_PAR_t stvlan;


    if(odmPortRangeCheck(lport))
	{
		return PORT_INVALID;
	}
	vosSprintf(section, VLAN_SESSION, vid);
    if (vosConfigSectionIsExisted(VLAN_CONFIG_NAME,section) != 0)
    {
        VLAN_TRACE();
        return VLAN_ID_EXIST_ERR;
    }
    bitmap  = vosConfigUInt32Get(VLAN_CONFIG_NAME,section,PORT_MAP_KEY,0);
    if ((bitmap&(1<<((lport - 1)*3))) == 0)
    {
        VLAN_TRACE();
        return VLAN_GERNEL_ERROR;
    }
    /*add parameter*/
    stvlan.port  = lport;
    stvlan.vid   = vid;
    stData.apiId = HAL_API_VLAN_MEMBERDEL;
    stData.length = sizeof(HAL_VLAN_MEMBER_DEL_PAR_t);
    stData.param = (void *)&stvlan;
    ret = halAppApiCallSync(&stData);
    ret += stvlan.ret;

    if (ret != NO_ERROR)
    {
        VLAN_TRACE();
        return ret;
    }

    bitmap &= ~(7<<((lport - 1)*3));
    if (vosConfigUInt32Set(VLAN_CONFIG_NAME,section, PORT_MAP_KEY,bitmap) != NO_ERROR)
    {
        VLAN_TRACE();
        return ERR_CONFIG_SET_FAIL;
    }

    return NO_ERROR;
}
/*******************************************************************************
* odmGetVlanPortList
*
* DESCRIPTION:
*  		this function support get a vlan member port list.
*
*	INPUTS:
          lport:user logic port.
*
*	OUTPUTS:
          portlist:vlan member port list.
*
* 	RETURNS:
* 			NO_ERROR if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
int odmGetVlanPortList
(
    UINT16 vid,
    UINT32 *portlist
)
{
    char section[16] = {0};

    if (portlist == NULL)
    {
        return ERR_NULL_POINTER;
    }
    vosSprintf(section, VLAN_SESSION, vid);

    *portlist = vosConfigUInt32Get(VLAN_CONFIG_NAME,section,PORT_MAP_KEY,0);

    return NO_ERROR;
}
/*******************************************************************************
* odmFlushVlanAll
*
* DESCRIPTION:
*  		this function support flush all vlan .
*
*	INPUTS:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
int odmFlushVlanAll
(
    void
)
{
    int ret;
    OPL_API_DATA_t       stData;
    HAL_VLAN_FLUSH_PAR_t stvlan;

    stData.apiId = HAL_API_VLAN_FLUSH;
    stData.length = sizeof(HAL_VLAN_FLUSH_PAR_t);
    stData.param = (void *)&stvlan;

    ret  = halAppApiCallSync(&stData);
    ret += stvlan.ret;
    if (ret != NO_ERROR)
    {
        VLAN_TRACE();
        return ret;
    }
    if (vosConfigModuleDelete(VLAN_CONFIG_NAME) != NO_ERROR)
    {
        VLAN_TRACE();
        return ERR_CONFIG_SET_FAIL;
    }

    return NO_ERROR;
}

void odmShowAllVlan
(
    int fd,
    int flag
)
{
    UINT16  count;
    UINT16  roop;
    UINT16 vid ;
    UINT8  portnum;
    UINT8  mode = 0;

    UINT32 portlist = 0;

    char   *session = NULL;
    if (1 != flag)
    {
        vosPrintf(fd,"name      |   id\r\n");
        vosPrintf(fd,"----------------\r\n");
    }
    /*set the config according to the database save*/
    count = vosConfigSectionCount(VLAN_CONFIG_NAME);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(VLAN_CONFIG_NAME,roop,&session) == 0)
        {
            vid = vosConfigUInt32Get(VLAN_CONFIG_NAME,session,VLAN_ID_KEY,0xffff);
            if (vid < 0xffff)
            {

                if (1 == flag)
                {
                    vosPrintf(fd, "VLAN Information:\r\n");
                    vosPrintf(fd, "  %-20s : %s\r\n","VLAN Name",session);
                    vosPrintf(fd, "  %-20s : %d\r\n","VLAN ID",vid);

                    vosPrintf(fd, "\r\n");
                    vosPrintf(fd, "   Port   Type\r\n");
                    vosPrintf(fd, "  ------ ------------\r\n");
                    odmGetVlanPortList(vid,&portlist);

                    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
                	{
                	    if (utilStrStartWith(session,"mc_") == 1)
                	    {
                	        odmMulticastTagstripGet(portnum,&mode);
                	        if (MC_TAG_OPER_MODE_STRIP == mode)
                	        {
                	            vosPrintf(fd, "   %5d  %s\r\n",portnum,"untag");
                	        }
                	        else
                	        {
                	            vosPrintf(fd, "   %5d  %s\r\n",portnum,"tag");
                	        }
                	    }
                	    else
                	    {
                    	    if ((portlist&(1<<((portnum-1)*3))) > 0)
                    	    {
                        	    switch ((portlist>>((portnum-1)*3 +1))&0x3)
                        	    {
                            	    case TAG:
                                	    vosPrintf(fd, "   %5d  %s\r\n",portnum,"tag");
                                	    break;
                                    case UNTAG:
                                	    vosPrintf(fd, "   %5d  %s\r\n",portnum,"untag");
                                	    break;
                                    case UNMODIFY:
                                	    vosPrintf(fd, "   %5d  %s\r\n",portnum,"unmodify");
                                	    break;
                                	 default :
                                	    break;

                        	    }
                    	    }
                	    }
                	}
                	vosPrintf(fd, "\r\n");
            	}
            	else
            	{
            	    vosPrintf(fd,"%-11s  %d\r\n",session,vid);
            	}

            }
            else
            {
                if ((NULL != vosConfigValueGet(VLAN_CONFIG_NAME,session,VLAN_NAME,NULL))&&(1 != flag))
                    vosPrintf(fd,"%-11s  %s\r\n",session,"---");
            }
        }
    }
    return;
}

int odmGetVlanListInfo(UINT32 *vlanNum, vlan_info_t *vlanInfo)
{
	UINT32 vlan_index;
	UINT8 port_index;
	UINT16 vid;
	UINT32 portlist;
	UINT8 mode;
	UINT32 tag;
	UINT16 vlan_Num = 0;
	char * session = NULL;
	UINT32 count;

	count = vosConfigSectionCount(VLAN_CONFIG_NAME);
	for (vlan_index = 0; vlan_index < count; vlan_index++)
	{
        if (vosConfigSectionGetByIndex(VLAN_CONFIG_NAME, vlan_index, &session) == 0)
        {
            vid = vosConfigUInt32Get(VLAN_CONFIG_NAME, session, VLAN_ID_KEY, 0xffff);
            if (vid < 0xffff)
            {
            	vlanInfo->port = 0;
				vlanInfo->vid  = vid;
				vosMemSet(vlanInfo->member, 0xF, sizeof(vlanInfo->member));

                odmGetVlanPortList(vid, &portlist);

                for(port_index = ODM_START_PORT_NUN; port_index <= ODM_NUM_OF_PORTS; port_index++)
            	{
            	    if ((portlist&(1<<((port_index-1)*3))) > 0)
            	    {
            	    	tag = (portlist>>((port_index-1)*3 +1))&0x3;
                	    switch (tag)
                	    {
                    	    case TAG:
                            case UNTAG:
                            case UNMODIFY:
								vlanInfo->member[port_index - 1] = tag;
								break;

                        	default:
                        	    break;
                	    }
            	    }
                }

				vlanInfo++;
				vlan_Num++;
            }
		}
	}

	*vlanNum = vlan_Num;
}



/*
 * Add for vlan N:1 aggregation
 *
 */
typedef struct VLAN_AGG_MAC_AGE_INFO_s{
  OPL_CNT_ADD_t	ruleCnt;
  UINT32 time;
  UINT16 ruleId;
}VLAN_AGG_MAC_AGE_INFO_t;
INT32 vlanAggMacFull;
INT32 vlanAggTimeRunning;
INT32 vlanAggInited[ODM_NUM_OF_PORTS];
static TIMER_OBJ_t * g_pstAggMacAgingTimer;
VLAN_AGG_DS_t vlanAggArray[ODM_MAX_NUM_OF_AGGRE_ENTRY];
VLAN_AGG_MAC_AGE_INFO_t vlanAggMacAgeInfo[ODM_MAX_NUM_OF_AGGRE_MAC];

OPL_STATUS odmVlanAggDelByPort(UINT16 egressPort);
static int odmAggMacAgingTimer(void);

OPL_STATUS odmVlanAggInit(UINT32  portId)
{
	if (NULL == g_pstAggMacAgingTimer){
		vlanAggMacFull = 0;
		memset(vlanAggMacAgeInfo, 0, sizeof(VLAN_AGG_MAC_AGE_INFO_t)*ODM_MAX_NUM_OF_AGGRE_MAC);
   		memset(vlanAggArray, 0, sizeof(VLAN_AGG_DS_t)*ODM_MAX_NUM_OF_AGGRE_ENTRY);
#ifndef ONU_4PORT_AR8327
		g_pstAggMacAgingTimer = vosTimerGetFree();
		if (NULL == g_pstAggMacAgingTimer){
			printf("%s fails to get timer.\n", __FUNCTION__);
			return OPL_ERROR;
		}
		
		vosTimerStart(g_pstAggMacAgingTimer, REOCURRING_TIMER, 
		    1000, (FUNCPTR)odmAggMacAgingTimer,1,2,3,4,5,6,7,8);
#endif
	}  
	
	vlanAggInited[portId-1] = 1;

	OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggInit(%d) finished (timer %x)\n", portId,g_pstAggMacAgingTimer);
	
    return OPL_OK;
}

OPL_STATUS odmVlanAggUninit(UINT32  portId)
{
	UINT32 i,enable = TRUE;
	UINT32 vlanMode = 0;

	if (!vlanAggInited[portId-1]){
		return OPL_OK;
	}

	vlanAggInited[portId-1] = 0;

#ifndef ONU_4PORT_AR8327
	enable = 1;
	for(i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++){
		if (i != portId){
			odmPortVlanModeGet(i, &vlanMode);
		    if (vlanMode == ODM_VLAN_AGGREGATION){
				enable = 0;
				break;
		    }
		}
	}

	if (enable && g_pstAggMacAgingTimer){
	    vosTimerCancel(g_pstAggMacAgingTimer);
	    g_pstAggMacAgingTimer = NULL;
	}  

	while(vlanAggTimeRunning){
		vosUDelay(100);
	}

	odmVlanAggDelByPort(portId);
	
#ifdef ONU_4PORT_AR8228
    dalArlMacPortLockCpuEn(portId, FALSE);
#endif

	//#if defined(ONU_4PORT_AR8228)
   odmPortLearningEnableGet(portId, &enable);
   if(enable){
   		odmPortLearningEnableSet(portId, enable);
   }
//#endif

#endif

	OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggUninit(%d) finished (timer %x)\n", portId,g_pstAggMacAgingTimer);

    return OPL_OK;
}

int odmAggMacAgingTimer(void)
{
    INT32 i;
	UINT32 ruleId;
	UINT32 agingTime = 1200;
	OPL_CNT_t oplCounter[DAL_CNT_CLEHIT_NUM];

	dalCounterGet(DAL_CNT_CLEHIT_NUM, oplCounter, 0, DAL_CNT_CLEHIT_NUM/2);
	dalCounterGet(DAL_CNT_CLEHIT_NUM, oplCounter+(DAL_CNT_CLEHIT_NUM/2), DAL_CNT_CLEHIT_NUM/2, DAL_CNT_CLEHIT_NUM/2);

	vlanAggTimeRunning = 1;

	odmFdbAgingTimeGet(&agingTime);

#ifdef ONU_4PORT_AR8228
	if (agingTime < 7){
		agingTime = 0;
	}
#endif
	if (agingTime == 0){
		vlanAggTimeRunning = 0;
		return 0;
	}

	for (i=1; i<ODM_MAX_NUM_OF_AGGRE_MAC; i++){
		if (vlanAggArray[i].used){
			ruleId = vlanAggMacAgeInfo[i].ruleId;
			if (vosMemCmp(&(oplCounter[ruleId].stAdded), &(vlanAggMacAgeInfo[i].ruleCnt), sizeof(OPL_CNT_ADD_t))==0){				
				vlanAggMacAgeInfo[i].time = vlanAggMacAgeInfo[i].time + 1;
				//printf("rule %d cnt time %d.\n", ruleId, vlanAggMacAgeInfo[i].time);
				if (vlanAggMacAgeInfo[i].time >= agingTime){
					OP_DEBUG(DEBUG_LEVEL_INFO,"port %d mac[5] %x old.\n", vlanAggArray[i].egressPort,vlanAggArray[i].aggedMac[5]);
					dalVttVlanAggDel(vlanAggArray[i].egressPort, vlanAggArray[i].targetVlan, vlanAggArray[i].aggedVlan, vlanAggArray[i].aggedMac);
#ifdef ONU_4PORT_AR8228
			        dalVoipClsDelEntry(vlanAggMacAgeInfo[i].ruleId);
#endif
			        memset(&vlanAggArray[i], 0, sizeof(VLAN_AGG_DS_t));
					memset(&vlanAggMacAgeInfo[i], 0, sizeof(VLAN_AGG_MAC_AGE_INFO_t));
					if (vlanAggMacFull){
						OP_DEBUG(DEBUG_LEVEL_INFO,"timer enable: agg mac to cpu\n");
						falVlanAggMacCpuEn();
						vlanAggMacFull = 0;
					}
				}
			}
			else {
				vlanAggMacAgeInfo[i].time = 0;
				vosMemCpy(&(vlanAggMacAgeInfo[i].ruleCnt), &(oplCounter[ruleId].stAdded), sizeof(OPL_CNT_ADD_t));

			}
		}
	}

	vlanAggTimeRunning = 0;

	return 0;
}


OPL_STATUS falVlanAggMacCpuEn()
{
	UINT32 i, vlanMode;
		
	for(i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS; i++){
		odmPortVlanModeGet(i, &vlanMode);
	    if (vlanMode != ODM_VLAN_AGGREGATION){
		    continue;
	    }
#if defined(ONU_4PORT_AR8228)
		dalArlMacPortLockCpuEn(i, TRUE);
#elif defined(ONU_1PORT)
		odmPortLearningEnableSet(i, FALSE);
#endif
	}

	return OPL_OK;
}

/* 0x0 transparent, 0x1 tag, 0x2 translation, 0x3 N:1 aggregation, 0x4 trunk */
//static UINT8 vlanModeOld = 0;

OPL_STATUS falVlanAggAdd(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,
    UINT8 *aggedMac, UINT16 tmpVlan)
{
  INT32 i,full;
  UINT16 index, ruleId;
  //int flg = 0;
  UINT32 defaultVlan = 0;
  OPL_STATUS ret;
  CLS_CONFIG_INFO_t clsconfiginfo;
  UINT8  fullmacMask[MAC_LENGTH] = {
	  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
	};

  if (!vlanAggInited[egressPort-1]){
  	 return OPL_ERROR;
  }

  index = 0;
  if (aggedMac == NULL)
  {
    for (i=ODM_MAX_NUM_OF_AGGRE_ENTRY-1; i>ODM_MAX_NUM_OF_AGGRE_MAC; i--)
    {
      if (vlanAggArray[i].used)
      {
        if ((vlanAggArray[i].egressPort==egressPort)
         &&(vlanAggArray[i].targetVlan==targetVlan)
         &&(vlanAggArray[i].aggedVlan==aggedVlan))
        {
          //index = i; /* entry exist */
          return OPL_OK;
        }
      }
	  else if (index == 0)
	  {
	  	index = i;
	  }
	 
    }
  }
  else
  {
  	targetVlan = 0;
	for(i=ODM_MAX_NUM_OF_AGGRE_ENTRY-1; i>ODM_MAX_NUM_OF_AGGRE_MAC; i--){
		if(vlanAggArray[i].used) {        
	        if((vlanAggArray[i].egressPort==egressPort)
	          &&(vlanAggArray[i].aggedVlan==aggedVlan)) 
	        {
	          targetVlan = vlanAggArray[i].targetVlan;
			  break;
	        }
		}
	}
	/* target vlan is invalid */
	if (targetVlan == 0)
	{
#ifdef ONU_4PORT_AR8228 
		odmPortDefaultVlanGet(egressPort, &defaultVlan);
		if (aggedVlan == defaultVlan){
			return dalSoftArlMacAdd(egressPort, aggedVlan, aggedMac);
		}
#endif
		return OPL_ERROR;
	}
	
	full = 1;
    for(i=1; i<ODM_MAX_NUM_OF_AGGRE_MAC; i++) {
      if(vlanAggArray[i].used) {        
        if((vlanAggArray[i].egressPort==egressPort)
          &&(vlanAggArray[i].aggedVlan==aggedVlan)) 
        {
          //targetVlan = vlanAggArray[i].targetVlan;
          if ((vlanAggArray[i].aggedMac[0] == aggedMac[0])
            &&(vlanAggArray[i].aggedMac[1] == aggedMac[1])
            &&(vlanAggArray[i].aggedMac[2] == aggedMac[2])
            &&(vlanAggArray[i].aggedMac[3] == aggedMac[3])
            &&(vlanAggArray[i].aggedMac[4] == aggedMac[4])
            &&(vlanAggArray[i].aggedMac[5] == aggedMac[5]))
          {
            //index = i; /* entry exist */
            return OPL_OK;
          }
          //flg = 1;
        }
      }
	  else {
	  	full = 0;
		if (index == 0){
	  	    index = i;
	    }
	  }
    }

	if (full){ /* full.*/
		if (!vlanAggMacFull){
			OP_DEBUG(DEBUG_LEVEL_INFO,"agg mac full, enable mac dropped.\n");
#if defined(ONU_4PORT_AR8228)
			odmPortLockDropEnable(egressPort, TRUE);
#elif defined(ONU_1PORT)
			odmPortLearningEnableSet(egressPort, TRUE);
#endif
			vlanAggMacFull = full;
		}
	    return OPL_ERROR;
	}	
  }

  /* record is not exist, search free record */
 /* if(((index==0) && (flg==1)) || (flg==2)) {
    for(i=1; i<ODM_MAX_NUM_OF_AGGRE_MAC; i++) {
      if(vlanAggArray[i].used==OPL_FALSE) {
        index = i;
        break;
      }
    }
  }*/

  if(index!=0) {

    if(aggedMac!=NULL) {
      vlanAggArray[index].aggedMac[0] = aggedMac[0];
      vlanAggArray[index].aggedMac[1] = aggedMac[1];
      vlanAggArray[index].aggedMac[2] = aggedMac[2];
      vlanAggArray[index].aggedMac[3] = aggedMac[3];
      vlanAggArray[index].aggedMac[4] = aggedMac[4];
      vlanAggArray[index].aggedMac[5] = aggedMac[5];
    }

	//#ifdef ONU_1PORT    
    ret = dalVttVlanAggAdd(egressPort, targetVlan, aggedVlan, vlanAggArray[index].aggedMac, &ruleId);
    //#endif

	if (OPL_OK == ret){

		vlanAggArray[index].egressPort=egressPort;
	    vlanAggArray[index].targetVlan=targetVlan;
	    vlanAggArray[index].aggedVlan=aggedVlan;

		if(tmpVlan!=0) {
	      vlanAggArray[index].tmpVlan=tmpVlan;
	    }

        if (aggedMac != NULL){
#if defined(ONU_4PORT_AR8228)
			vosMemSet(&clsconfiginfo, 0, sizeof(CLS_CONFIG_INFO_t));
			clsconfiginfo.srcMacFlag = 1;
			vosMemCpy(clsconfiginfo.srcMac.lowRange, aggedMac, MAC_LENGTH);
			vosMemCpy(clsconfiginfo.srcMac.highRange,fullmacMask,MAC_LENGTH);
			clsconfiginfo.bingress = 0;
			clsconfiginfo.ingressmask = 1;
			clsconfiginfo.t_act = DONOT_DROP;
			dalVoipClsRuleCtcAdd(&clsconfiginfo,&(vlanAggMacAgeInfo[index].ruleId));
#elif defined(ONU_1PORT)
			vlanAggMacAgeInfo[index].ruleId = ruleId;
#endif
        }

		vlanAggArray[index].used = OPL_TRUE;

	}
#if defined(ONU_4PORT_AR8228)
	else if (DAL_VTT_ACL_RULE_FULL == ret){
		if (!vlanAggMacFull){
			OP_DEBUG(DEBUG_LEVEL_INFO,"agg mac full2, enable mac dropped.\n");
			odmPortLockDropEnable(egressPort, TRUE);
			odmPortLearningEnableSet(egressPort, TRUE);
			vlanAggMacFull = 1;
		}
	    return OPL_ERROR;
	}
#endif
    
    return ret;
  }
  return OPL_ERROR;
}

OPL_STATUS falVlanAggDel(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,
    UINT8 *aggedMac, UINT16 tmpVlan)
{
  int i;
  UINT16 index;
  UINT32 vlanMode;

  index = 0;

  for(i=1; i<ODM_MAX_NUM_OF_AGGRE_ENTRY; i++) {
    if(vlanAggArray[i].used) {
      if((vlanAggArray[i].egressPort==egressPort)
        &&(vlanAggArray[i].targetVlan==targetVlan)
        &&(vlanAggArray[i].aggedVlan==aggedVlan)) {
        index = i; /* entry exist */
       // #ifdef ONU_1PORT
        dalVttVlanAggDel(egressPort, targetVlan, aggedVlan, vlanAggArray[index].aggedMac);
       // #endif
        if(i < ODM_MAX_NUM_OF_AGGRE_MAC){
#ifdef ONU_4PORT_AR8228
			dalVoipClsDelEntry(vlanAggMacAgeInfo[i].ruleId);
#endif
			memset(&vlanAggMacAgeInfo[i], 0, sizeof(VLAN_AGG_MAC_AGE_INFO_t));
			if (vlanAggMacFull){
				vlanAggMacFull = 0;
				if (vlanAggInited[egressPort-1]){ /* no configure port lock.*/
					OP_DEBUG(DEBUG_LEVEL_INFO,"agg mac del, enable mac to cpu.\n");
					falVlanAggMacCpuEn();
				}
			}
        }
        memset(&vlanAggArray[index], 0, sizeof(VLAN_AGG_DS_t));
      }
    }
  }

  return OPL_ERROR;
}

char *clStrDel(char *s, const char *del)
{
  char *tmp;
  int len;

  if((s == NULL) || (del == NULL))
    return NULL;

  tmp = NULL;
  len = 0;

  tmp = strstr(s, del);

  if(tmp==NULL)
    return NULL;

  len = strlen(del);

  strcpy(tmp, tmp+len);

  return s;
}

#define SMALL_BUF_LEN                    256
#define PORT_TO_STRING_LEN                    512
OPL_STATUS odmVlanAggAdd(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan)
{
  INT8 section[256];
  INT8 vlan_string[512];
  UINT32 len;
  OPL_STATUS ret;
  INT8* get;
  INT8 vlanid_string[6];

  OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggAdd:egressPort=%d,targetVlan=%d,aggedVlan=%d\n", egressPort,targetVlan,aggedVlan);

  if(egressPort>DAL_NUM_OF_PORTS) {
    return ERR_INVALID_PARAMETERS;
  }

  if (odmVlanRangeCheck(targetVlan) != OK || odmVlanRangeCheck(aggedVlan) != OK)
  {
    return ERR_INVALID_PARAMETERS;
  }

  vosMemSet(section, 0, 256);
  vosMemSet(vlan_string, 0, 512);
  vosMemSet(vlanid_string, 0, 6);
  len = 0;
  get = NULL;

  vosSprintf(section, "%s%d-%d", VLAN_AGGREGATION_TARGED_ID, targetVlan, egressPort);
  vosSprintf(vlanid_string, "%d,", aggedVlan);

  get = vosConfigValueGet(VLAN_CONFIG_NAME, section, VLAN_AGGREGATION_AGGED_ID, NULL);

  if (get) {
    vosStrCpy(vlan_string, get);
  }

  get = strstr(vlan_string, vlanid_string);
  if(get!=NULL) { /* find the vlan id */
  	 if (egressPort == vosConfigUInt32Get(VLAN_CONFIG_NAME, section, VLAN_AGGREGATION_ENGRESS_ID, 0xff)){
  		return OPL_OK;
 	 }
  }

  ret = falVlanAggAdd(egressPort,targetVlan,aggedVlan, NULL, 0);
  if (OPL_OK != ret){
  	printf("falVlanAggAdd fail %d.\n", ret);
  	return ret;
  }

  strcat(vlan_string, vlanid_string);

  if (OPL_ERROR == vosConfigValueSet(VLAN_CONFIG_NAME,
                                     section,
                                     VLAN_AGGREGATION_AGGED_ID,
                                     vlan_string)) {
    return ERR_VARNORESOURCE;
  }

  vosConfigUInt32Set(VLAN_CONFIG_NAME, section, VLAN_AGGREGATION_ENGRESS_ID, egressPort);

  return OPL_OK;
}


OPL_STATUS odmVlanAggDel(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan)
{
  INT8 section[256];
  INT8 vlan_string[512];
  UINT32 len;
  INT8* get;
  INT8 vlanid_string[6];

  OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggDel:egressPort=%d,targetVlan=%d,aggedVlan=%d\n", egressPort,targetVlan,aggedVlan);

  if(egressPort>DAL_NUM_OF_PORTS) {
    return ERR_INVALID_PARAMETERS;
  }
  if((targetVlan>4095)||(aggedVlan>4095)) {
    return ERR_INVALID_PARAMETERS;
  }

  vosMemSet(section, 0, 256);
  vosMemSet(vlan_string, 0, 512);
  vosMemSet(vlanid_string, 0, 6);
  len = 0;
  get = NULL;

  vosSprintf(section, "%s%d-%d", VLAN_AGGREGATION_TARGED_ID, targetVlan, egressPort);
  vosSprintf(vlanid_string, "%d,", aggedVlan);

  get = vosConfigValueGet(VLAN_CONFIG_NAME, section, VLAN_AGGREGATION_AGGED_ID, NULL);

  if (get) {
    vosStrCpy(vlan_string, get);
  }

  get = strstr(vlan_string, vlanid_string);
  if(get==NULL) { /* do not find the vlan id */
    return OPL_OK;
  }

  if (egressPort != vosConfigUInt32Get(VLAN_CONFIG_NAME, section, VLAN_AGGREGATION_ENGRESS_ID, 0xff)){
  	return OPL_OK;
  }

  clStrDel(vlan_string, vlanid_string);

  if (OPL_ERROR == vosConfigValueSet(VLAN_CONFIG_NAME,
                                     section,
                                     VLAN_AGGREGATION_AGGED_ID,
                                     vlan_string)) {
    return ERR_VARNORESOURCE;
  }

  falVlanAggDel(egressPort,targetVlan,aggedVlan, NULL, 0);

  return OPL_OK;
}

OPL_STATUS odmVlanAggMacDelByPort(UINT16 egressPort)
{
  int i;
  UINT8 aggedMac[6];

  //OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggMacDelByPort:egressPort=%d,DAL_NUM_OF_PORTS=%d\n", egressPort,DAL_NUM_OF_PORTS);

  if(egressPort>DAL_NUM_OF_PORTS) {
    return ERR_INVALID_PARAMETERS;
  }

  vosMemSet(aggedMac, 0, 6);

  for(i=1; i<ODM_MAX_NUM_OF_AGGRE_MAC; i++) {
    if(vlanAggArray[i].used) {
      if(vlanAggArray[i].egressPort==egressPort) {
		dalVttVlanAggDel(egressPort, vlanAggArray[i].targetVlan, vlanAggArray[i].aggedVlan, vlanAggArray[i].aggedMac);
		dalVoipClsDelEntry(vlanAggMacAgeInfo[i].ruleId);
        memset(&vlanAggArray[i], 0, sizeof(VLAN_AGG_DS_t));
		memset(&vlanAggMacAgeInfo[i], 0, sizeof(VLAN_AGG_MAC_AGE_INFO_t));
      }
    }
  }

  return OPL_OK;
}
  
OPL_STATUS odmVlanAggDelByPort(UINT16 egressPort)
{
  int i;

  OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggDelByPort:egressPort=%d\n", egressPort);

  if(egressPort>DAL_NUM_OF_PORTS) {
    return ERR_INVALID_PARAMETERS;
  }

  odmVlanAggMacDelByPort(egressPort);
  
  for(i=1; i<ODM_MAX_NUM_OF_AGGRE_ENTRY; i++) {
    if(vlanAggArray[i].used) {
      if(vlanAggArray[i].egressPort==egressPort) {
        odmVlanAggDel(egressPort, vlanAggArray[i].targetVlan, vlanAggArray[i].aggedVlan);
      }
    }
  }

  return OPL_OK;
}


VLAN_AGG_DS_t* odmVlanAggGet()
{
  return vlanAggArray;
}

OPL_STATUS odmVlanAggLearnAction(UINT16 egressPort, UINT16 aggedVlan, UINT8 *aggedMac)
{
  int i;
  UINT16 targetVlan;

  if(aggedMac==NULL)
    return OPL_ERROR;

  targetVlan = 0;

  //printf("-----------------------in odmVlanAggLearnAction\n");
  OP_DEBUG(DEBUG_LEVEL_INFO,"odmVlanAggLearnAction egressPort=%d, Vlan=%d, Mac[4]Mac[5]=%02x:%02x\n", egressPort, aggedVlan, aggedMac[4], aggedMac[5]);
  falVlanAggAdd(egressPort,targetVlan,aggedVlan, aggedMac, 0);
  #if 0
  for(i=1; i<256; i++) {
    if(vlanAggArray[i].used) {
      if((vlanAggArray[i].egressPort==egressPort)
         &&(vlanAggArray[i].aggedVlan==aggedVlan)) {
        vlanAggArray[i].aggedMac[0] = aggedMac[0];
        vlanAggArray[i].aggedMac[1] = aggedMac[1];
        vlanAggArray[i].aggedMac[2] = aggedMac[2];
        vlanAggArray[i].aggedMac[3] = aggedMac[3];
        vlanAggArray[i].aggedMac[4] = aggedMac[4];
        vlanAggArray[i].aggedMac[5] = aggedMac[5];
         targetVlan = vlanAggArray[i].targetVlan;
        break;
      }
    }
  }

  if(i==256) {
    //printf("do not find the N:1 VLAN aggregation entry.\n");
    return -1;
  }

  //printf("targetVlan=%d\n", targetVlan);

#ifdef ONU_1PORT
  dalVttVlanAggAdd(egressPort, targetVlan, aggedVlan, aggedMac);
#endif
  #endif
}


int odmVlanInit
(
    void
)
{
    int    ret=0;
    UINT8  count;
    UINT8  roop;
    UINT16 vid ;
    UINT32 portlist;

    UINT8  portnum;
    UINT8  cos;
    UINT16  ruleid;
	char *ip;
	UINT32 u32ip;
	//CLASSIFY_ITEM_t ClassItemInfo;
	//CLASSIFY_PARAMETER_t *pParameter;
    char str_ip[20] = "";
	int i;
    char   *session = NULL;
    char   *value   = NULL;

    char   vlanname[20];
	char   targeId[20];

	OPL_API_DATA_t       stData;
    HAL_VLAN_CREATE_PAR_t stCreatVlan;
    HAL_VLAN_MEMBER_ADD_PAR_t stvlan;
    HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t stLocalSwitchVlan;
    HAL_VLAN_EGRESS_TYPE_E type;
    HAL_VLAN_FLUSH_PAR_t stVlanFlush;

	UINT32 vlanMode, targedId;
	char *str, *str1;
	
    /*set the config according to the database save*/
    count = vosConfigSectionCount(VLAN_CONFIG_NAME);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(VLAN_CONFIG_NAME,roop,&session) == 0)
        {
            vid = vosConfigUInt32Get(VLAN_CONFIG_NAME,session,VLAN_ID_KEY,0xffff);
            if (vid < 0xffff)
            {

                stCreatVlan.vid = vid;
                stData.apiId = HAL_API_VLAN_CREATE;
                stData.length= sizeof(HAL_VLAN_CREATE_PAR_t);
                stData.param = (void *)&stCreatVlan;
                ret = halAppApiCallSync(&stData);
                ret += stCreatVlan.ret;
                if (ret != NO_ERROR)
                {
                    OP_ALARM("Create vlan (%d) failed", vid);
                    VLAN_TRACE();
                    continue;
                }

                vosMemSet(vlanname,0,20);
                vosSprintf(vlanname, VLAN_SESSION, vid);
                portlist  = vosConfigUInt32Get(VLAN_CONFIG_NAME,vlanname,PORT_MAP_KEY,0);
				if(portlist!=0)
				{
                	for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
	            	{
	            	    if ((portlist&(1<<((portnum-1)*3))) > 0)
	            	    {

	            	        type = ((portlist>>((portnum-1)*3 +1))&0x3);
	        	            stvlan.port  = portnum;
	                        stvlan.vid   = vid;
	                        stvlan.type  = type;
	                        stData.apiId = HAL_API_VLAN_MEMBERADD;
	                        stData.length = sizeof(HAL_VLAN_MEMBER_ADD_PAR_t);
	                        stData.param = (void *)&stvlan;
	                        ret = halAppApiCallSync(&stData);
	                        ret += stvlan.ret;

	                        if (ret != NO_ERROR)
	                        {
	                            OP_ALARM("Add vlan port failed");
	                            VLAN_TRACE();
	                            return ret;
	                        }

	            	    }
	            	}
				}

            }

			portnum = vosConfigUInt32Get(VLAN_CONFIG_NAME, session, VLAN_AGGREGATION_ENGRESS_ID, 0xFF);
			if (portnum != 0xFF){
				vlanMode = 0;
				odmPortVlanModeGet(portnum, &vlanMode);
				if (ODM_VLAN_AGGREGATION == vlanMode){
					odmVlanAggInit(portnum);
					str = vosStrStr(session, VLAN_AGGREGATION_TARGED_ID);
					if (str){
						str += vosStrLen(VLAN_AGGREGATION_TARGED_ID);
						str1 = vosStrStr(str, "-");
						memset(targeId, 0, sizeof(targeId));
						memcpy(targeId, str, str1-str);
						targedId = vosStrToUInt(targeId, NULL);
						str = vosConfigValueGet(VLAN_CONFIG_NAME,session,VLAN_AGGREGATION_AGGED_ID, NULL);
		                while (str && *str!='\0'){
							str1 = vosStrStr(str, ",");
							*str1 = '\0';
							odmVlanAggAdd(portnum, targedId, vosStrToUInt(str, NULL));
							str = str1+1;
		                }
					}
					
				}
			}		
        }
    }

    /*local switch */
    UINT32 localswith;
    UINT8  enable;

    localswith = vosConfigUInt32Get(VLAN_CONFIG_NAME,
        LOCAL_SWITCH_SEC, LOCAL_SWITCH_KEY, 0);

    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
        enable = ((localswith >> portnum) & 0x1); /*enable all localSwitch*/
        stLocalSwitchVlan.port = portnum;
        stLocalSwitchVlan.enable = enable;
        stData.apiId = HAL_API_VLAN_LOCAL_SWITCH_SET;
        stData.length= sizeof(HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t);
        stData.param = (void *)&stLocalSwitchVlan;
        ret = halAppApiCallSync(&stData);
        ret += stLocalSwitchVlan.ret;
        if (ret != NO_ERROR)
        {
            OP_ALARM("Set local switch failed");
            VLAN_TRACE();
            return ret;
        }
    }

    return NO_ERROR;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/

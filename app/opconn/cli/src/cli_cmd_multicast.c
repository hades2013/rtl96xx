/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
* used only with Opulan semi-conductor products.
*
* FILENAME:  cli_cmd_multicast.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_multicast.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"

#include "log.h"

#include "odm_port.h"
#include "mc_control.h"
//wfxu #include "dal_multicast.h"
//wfxu #include "dal_vtt.h"
#include "opconn_usr_ioctrl.h"

#define printf(fmt, ...)

extern struct multicast_igmp_cfg igmp_cfg;

STATUS  cliCmdMulticastFastleaveSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    return odmMulticastFastleaveModeSet(pstPara->i - 1);
}

extern uint32 mcPktPrintDebug;
STATUS  cliCmdMulticastDebugSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    mcPktPrintDebug = pstPara->i - 1;

    return 0;
}

STATUS  cliCmdMulticastHostTimeoutSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    odmMulticastHostAgingTimeSet(pstPara->u);

    return OK;
}

STATUS  cliCmdMulticastLastQueryIntervalSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    return odmMcControlBlockLastQueryIntervalSet(pstPara->u);
}

STATUS  cliCmdMulticastLastQueryCountSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }
    return odmMcControlBlockLastQueryCountSet(pstPara->u);
}

STATUS  cliCmdMulticastIgmpConfig(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    if(pstPara[0].u == 1) { /* enable */
      return odmMcControlBlockRecvEnSet(OPL_ENABLE);
    }
    if(pstPara[0].u == 2) { /* disable */
      return odmMcControlBlockRecvEnSet(OPL_DISABLE);
    }
}

STATUS  cliCmdMulticastModeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    if((pstPara->i - 1)==0) { /* snooping */
      odmMulticastSwitchSet(MC_MODE_SNOOPING);
    }
    if((pstPara->i - 1)==1) { /* ctc */
      odmMulticastSwitchSet(MC_MODE_CTC);
    }

    return 0;
}

STATUS  cliCmdMulticastControlTypeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    int ret;

    if (NULL == pstEnv || NULL == pstPara) {
        return ERROR;
    }

    odmMulticastControlTypeSet(pstPara->i - 1);

    return 0;
}

STATUS  cliCmdMulticastMulticastGroupCreate(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  uint16 vlanId;
  uint8 mac[6];

  vosMemSet(mac, 0, 6);

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    vosPrintf(pstEnv->nWriteFd, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }
  if(!(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)) {
    vosPrintf(pstEnv->nWriteFd, "Not in GDA MAC or GDA MAC + Vlan ID mode.\r\n");
    return OPL_ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  vlanId = pstPara[3].u;
  ret = clMacGetByString(mac, pstPara[4].p);
  if(ret!=0) {
    vosPrintf(pstEnv->nWriteFd, "Mac address error.\r\n");
    return ret;
  }

  /* check dest mac: 01 00 5e ... */
  if(!(mac[0]==0x01 && mac[1]==0x00 && mac[2]==0x5e)) {
    vosPrintf(pstEnv->nWriteFd, "Mac address error.\r\n");
    return ERROR;
  }

  if(vlanId >= 4096) {
    vosPrintf(pstEnv->nWriteFd, "Vlan id error.\r\nvlan_id=%d\r\n");
    return MULTICAST_IGMP_PARA_ERROR;
  }

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID)
  {
      UINT8	dal_vtt_num;
      UINT32 vlanAddNum = 0;

      if(pstPara[0].i == 2) {
        vlanAddNum = ODM_NUM_OF_PORTS;
      }
      else {
        vlanAddNum = 1;
      }
	  ret = dalVttNumExistGet(&dal_vtt_num);
	  if(ret!= OPL_OK) {
	    return ret;
	  }	  
      if(dal_vtt_num+vlanAddNum>16) {
        ret = OPL_FULL;
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
  }
#elif defined(ONU_1PORT)
  UINT32 addNum = 0;
  UINT8 numOfRecords = 0;
  UINT8 numOfRule = 0;
  UINT8 numOfMvlan = 0;
  UINT16 vlanIdArray[4096];
  
  if(pstPara[0].i == 2) {
    addNum = ODM_NUM_OF_PORTS;
  }
  else {
    addNum = 1;
  }
      
  dalMulticastNumofRuleGet(&numOfRule);
  if(numOfRule+addNum>32) {
    ret = OPL_FULL;
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }
  
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
    {
	  odmMulticastVlanGet(1, vlanIdArray, &numOfMvlan);
	  if(numOfRule+addNum*numOfMvlan>MC_MAX_GROUP_NUM) {
	    ret = OPL_FULL;
	    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
	  }
    }
    
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)
    {
	  dalVttMulticastNumOfRecordGet(&numOfRecords);
	  if(mcVlanMapExist(1, vlanId) == OPL_FALSE)
	    numOfRecords++;
      
	  if(numOfRecords>DAL_PORT_MCAST_ENTRY_NUM) {
	    ret = OPL_FULL;
	    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
	  }
    }
#endif

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      ret = odmMulticastControlEntryAdd(i, vlanId, mac);
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastGroupDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  uint16 vlanId;
  uint8 mac[6];

  vosMemSet(mac, 0, 6);

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    vosPrintf(pstEnv->nWriteFd, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }
  if(!(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)) {
    vosPrintf(pstEnv->nWriteFd, "Not in GDA MAC or GDA MAC + Vlan ID mode.\r\n");
    return OPL_ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  vlanId = pstPara[3].u;
  ret = clMacGetByString(mac, pstPara[4].p);
  if(ret!=0) {
    vosPrintf(pstEnv->nWriteFd, "Mac address error.\r\n");
    return ret;
  }

  /* check dest mac: 01 00 5e ... */
  if(!(mac[0]==0x01 && mac[1]==0x00 && mac[2]==0x5e)) {
    vosPrintf(pstEnv->nWriteFd, "Mac address error.\r\n");
    return ERROR;
  }

  if(vlanId >= 4096) {
    vosPrintf(pstEnv->nWriteFd, "Vlan id error.\r\nvlan_id=%d\r\n");
    return MULTICAST_IGMP_PARA_ERROR;
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      ret = odmMulticastControlEntryDelete(i, vlanId, mac);
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastGroupClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  uint16 vlanId;

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
    return ERROR;
  }

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    vosPrintf(pstEnv->nWriteFd, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }

  if(pstPara[1].i == 1) { /* vlan name */
    vlanId = pstPara[2].u;
    ret = odmMulticastControlEntryDeleteByVlan(vlanId);
    if(ret!=0) {
      return ret;
    }
  }
  if(pstPara[1].i == 2) { /* all */
    ret = odmMulticastControlEntryClear();
    if(ret!=0) {
      return ret;
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastGroupCreateByIp(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  uint16 vlanId;
  uint32 ip;
  uint8 mac[6];

  ip = 0;
  vlanId = 0;
  vosMemSet(mac, 0, 6);

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
    return ERROR;
  }

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    vosPrintf(pstEnv->nWriteFd, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }
  if(odmMulticastControlTypeGet()!=MC_CTL_GDA_GDA_IP_VID) {
    vosPrintf(pstEnv->nWriteFd, "Not in GDA IP + Vlan ID mode.\r\n");
    return OPL_ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  vlanId = pstPara[3].u;
  ip = pstPara[4].u;
  *(uint32 *)&mac[2] = ip;

  if(vlanId >= 4096) {
    vosPrintf(pstEnv->nWriteFd, "Vlan id error.\r\nvlan_id=%d\r\n");
    return MULTICAST_IGMP_PARA_ERROR;
  }

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID)
  {
      UINT8	dal_vtt_num;
      UINT32 vlanAddNum = 0;

      if(pstPara[0].i == 2) {
        vlanAddNum = ODM_NUM_OF_PORTS;
      }
      else {
        vlanAddNum = 1;
      }
	  ret = dalVttNumExistGet(&dal_vtt_num);
	  if(ret!= OPL_OK) {
	    return ret;
	  }	

      if(dal_vtt_num+vlanAddNum>16) {
        ret = OPL_FULL;
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
  }
#elif defined(ONU_1PORT)
  UINT32 addNum = 0;
  UINT8 numOfRecords = 0;
  UINT8 numOfRule = 0;
  UINT8 numOfMvlan = 0;
  UINT16 vlanIdArray[4096];
  
  if(pstPara[0].i == 2) {
    addNum = ODM_NUM_OF_PORTS;
  }
  else {
    addNum = 1;
  }
      
  dalMulticastNumofRuleGet(&numOfRule);
  if(numOfRule+addNum>MC_MAX_GROUP_NUM) {
    ret = OPL_FULL;
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }

  dalVttMulticastNumOfRecordGet(&numOfRecords);
  if(mcVlanMapExist(1, vlanId) == OPL_FALSE)
    numOfRecords++;
  
  if(numOfRecords>DAL_PORT_MCAST_ENTRY_NUM) {
    ret = OPL_FULL;
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }
#endif

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      ret = odmMulticastControlEntryAdd(i, vlanId, mac);
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastGroupDeleteByIp(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  uint16 vlanId;
  uint32 ip;
  uint8 mac[6];

  ip = 0;
  vlanId = 0;
  vosMemSet(mac, 0, 6);

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
    return ERROR;
  }

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    vosPrintf(pstEnv->nWriteFd, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }
  if(odmMulticastControlTypeGet()!=MC_CTL_GDA_GDA_IP_VID) {
    vosPrintf(pstEnv->nWriteFd, "Not in GDA IP + Vlan ID mode.\r\n");
    return OPL_ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  vlanId = pstPara[3].u;
  ip = pstPara[4].u;
  *(uint32 *)&mac[2] = ip;

  if(vlanId >= 4096) {
    vosPrintf(pstEnv->nWriteFd, "Vlan id error.\r\nvlan_id=%d\r\n");
    return MULTICAST_IGMP_PARA_ERROR;
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      ret = odmMulticastControlEntryDelete(i, vlanId, mac);
    }
  }

  return NO_ERROR;
}

#define OP_MAX_VLAN_NUMBER      4096

STATUS cliCmdMulticastMulticastVlanSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  ////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	  UINT8	dal_vtt_num;
      UINT32 vlanAddNum;
      vlanAddNum = 0;
      if(pstPara[0].i == 2) {
        vlanAddNum = ODM_NUM_OF_PORTS;
      }
      else {
        vlanAddNum = 1;
      }
	  ret = dalVttNumExistGet(&dal_vtt_num);
	  if(ret!= OPL_OK) {
	    return ret;
	  }		  
      if(dal_vtt_num+vlanAddNum>16) {
        ret = OPL_FULL;
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
#elif defined(ONU_1PORT)
    UINT32 vlanAddNum = 0;
    UINT8 numOfRecords = 0;
    multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
    UINT8 numOfRule = 0;    
    UINT16 numOfEntries = 0;
    
      if(pstPara[0].i == 2) {
        vlanAddNum = ODM_NUM_OF_PORTS;
      }
      else {
        vlanAddNum = 1;
      }
      
      dalVttMulticastNumOfRecordGet(&numOfRecords);
      if(numOfRecords+vlanAddNum>DAL_PORT_MCAST_ENTRY_NUM) {
        ret = OPL_FULL;
	 RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
      
      if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
      {
          dalMulticastNumofRuleGet(&numOfRule);
              
          odmMulticastControlEntryGet(ctl_entry_list, &numOfEntries);
          
          if(numOfRule+vlanAddNum*numOfEntries>MC_MAX_GROUP_NUM) {
            ret = OPL_FULL;
            RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
          }
      }
#endif
    ////20100327_XFAN_ADD_END

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastVlanAdd(i, pstPara[3].u);
    }
  }

  return NO_ERROR;
}


STATUS  cliCmdMulticastMulticastVlanDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastVlanDelete(i, pstPara[3].u);
    }
  }

  return NO_ERROR;
}


STATUS  cliCmdMulticastMulticastVlanClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastVlanClear(i);
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastPortConfigShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }
  cliShowStart(pstEnv);
  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      mcPortConfigShow(pstEnv->nWriteFd, i);
    }
  }
  cliShowEnd(pstEnv);

  return NO_ERROR;
}

STATUS  cliCmdMulticastCfgShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int i;

  if (NULL == pstEnv || NULL != pstPara) {
    return ERROR;
  }

  cliShowStart(pstEnv);

  mcControlBlockShow(pstEnv->nWriteFd);

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    mcPortConfigShow(pstEnv->nWriteFd, i);
  }

  cliShowEnd(pstEnv);
  return NO_ERROR;
}

STATUS  cliCmdMulticastVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  uint16 vlan_id;
  int ret;

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(pstPara[0].i== 2) {/* all */
  	cliShowStart(pstEnv);
    mcSnoopingGroupShowByVlan(pstEnv->nWriteFd, 0);
  	cliShowEnd(pstEnv);
  }

  if(pstPara[0].i == 1) {
  	cliShowStart(pstEnv);
    printf("pstPara[1].u=%d\n", pstPara[1].u);
    printf("pstPara[1].u=%d\n", pstPara[2].u);
    mcSnoopingGroupShowByVlan(pstEnv->nWriteFd, pstPara[1].u);
  	cliShowEnd(pstEnv);
  }

  return NO_ERROR;
}


STATUS  cliCmdMulticastStats(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  if (NULL == pstEnv || NULL != pstPara) {
    return ERROR;
  }

  cliShowStart(pstEnv);
  mcControlStatsShow(pstEnv->nWriteFd);
  cliShowEnd(pstEnv);
  return NO_ERROR;
}

STATUS  cliCmdMulticastGroupLimitAll(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
#if 0
  if (NULL == pstEnv || NULL != pstPara) {
    return ERROR;
  }

  cliShowStart(pstEnv);
  mcCtrlIgmpGroupLimitShowAll(pstEnv->nWriteFd);
  cliShowEnd(pstEnv);
#endif
  return NO_ERROR;
}


STATUS  cliCmdMulticastConfigEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
#if 0
  //printf("pstEnv=%x,pstPara=%x,pstEnv->nWriteFd=%x\n", pstEnv, pstPara, pstEnv->nWriteFd);
  if (NULL == pstEnv || NULL != pstPara) {
    return ERROR;
  }
  //printf("%s %d\n", __FUNCTION__, __LINE__);
  //multicast_igmp_config_enable();
#endif
  return NO_ERROR;
}


STATUS  cliCmdMulticastMulticastTagSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastTagstripSet(i, pstPara[3].i-1);
     }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastTagSetTranslation(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  uint32 i;
  UINT8 port_list_buff[128];
  uint8 tagOper;

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];
  tagOper = 0;

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  ////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	UINT8	dal_vtt_num;
    UINT32 vlanAddNum;
    vlanAddNum = 0;
    if(pstPara[0].i == 2) {
      vlanAddNum = ODM_NUM_OF_PORTS;
    }
    else {
      vlanAddNum = 1;
    }
	ret = dalVttNumExistGet(&dal_vtt_num);
	if(ret!= OPL_OK) {
	return ret;
	}		
    if(dal_vtt_num+vlanAddNum>16) {
      ret = OPL_FULL;
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
#elif defined(ONU_1PORT)
    UINT32 vlanAddNum = 0;
    UINT8 numOfRecords = 0;

    if(pstPara[0].i == 2) {
      vlanAddNum = ODM_NUM_OF_PORTS;
    }
    else {
      vlanAddNum = 1;
    }
        
    dalVttMulticastNumOfRecordGet(&numOfRecords);
    if(numOfRecords+vlanAddNum>4) {
      ret = OPL_FULL;
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
#endif
  ////20100327_XFAN_ADD_END

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastTagstripGet(i, &tagOper);
      if(tagOper==MC_TAG_OPER_MODE_TRANSLATION) {
        odmMulticastTagTranslationAdd(i, pstPara[3].u, pstPara[4].u);
      }
      else {
        vosPrintf(pstEnv->nWriteFd, "\r\nPort is not in vlan translation mode.\r\nport=%d.\r\n", i);
      }
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastMulticastGroupLimit(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  int ret;
  int i;
  UINT8 port_list_buff[128];

  UINT32 port_num;
  UINT32 port_array[ODM_NUM_OF_PORTS+1];

  if (NULL == pstEnv || NULL == pstPara) {
    return ERROR;
  }

  if(NULL == pstPara[0].p)
  {
  return ERROR;
  }

  if(pstPara[0].i == 1) { /* portlist */
    vosStrCpy(port_list_buff, pstPara[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
    if(port_array[0] == 1) {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort num shuld start at 1.\r\n");
      return ERROR;
    }

  }
  if(pstPara[0].i == 2) { /* all */
    for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }

  for(i = 1; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMulticastGroupMaxNumSet(i, pstPara[3].u);
    }
  }

  return NO_ERROR;
}

STATUS  cliCmdMulticastIgmpTest(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
  /* checkMem(); */
  return NO_ERROR;
}



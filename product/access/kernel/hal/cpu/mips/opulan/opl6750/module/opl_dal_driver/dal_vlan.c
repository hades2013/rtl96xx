/*
=============================================================================
     File Name: dal_mac.c

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
wangliang 			2008/8/19		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/

#include "dal_lib.h"

#define QINQ_RULE_NUM_MAX  128
QINQ_FlEXIBLE_VLAN_US_t vlanQinqFlexibleUsInfo[QINQ_RULE_NUM_MAX];
QINQ_TRANSPARENT_VLAN_t vlanQinqTransparentInfo[QINQ_RULE_NUM_MAX];
QINQ_BASE_VLAN_t vlanQinqBaseInfo[QINQ_RULE_NUM_MAX];
UINT8 qinqFlexibleNum = 0;
UINT8 qinqTransparentNum =0;
UINT8 qinqBaseNum =0;
UINT8 qinqMode =0;



OPL_STATUS dalSetLocalSwitch
(
    UINT8  lport,
    UINT8  enable
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortPVid
*
* DESCRIPTION:
*  		this function support set the port default vlan id (PVID).
*
*	INPUTS:
          lport:user logic port.
          vid  :vlan Id
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortPVid
(
    UINT8  lport,
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortPVid
*
* DESCRIPTION:
*  		this function support get the port default vlan id (PVID).
*
*	INPUTS:
          lport:user logic port.          
*		
*	OUTPUTS:
          vid  :vlan Id
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortPVid
(
    UINT8  lport,
    UINT16 *vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortBaseVlanMember
*
* DESCRIPTION:
*  		this function support set port base vlan port member.
*
*	INPUTS:
          lport:user logic port.
          memlist:member list.
          memnum:member number.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortBaseVlanMember
(
    UINT8 lport,
    UINT8 *memlist,
    UINT8 memnum
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortBaseVlanMember
*
* DESCRIPTION:
*  		this function support get port base vlan port member.
*
*	INPUTS:
          lport:user logic port.
*		
*	OUTPUTS:
          memlist:member list.
          memnum:member number.
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortBaseVlanMember
(
    UINT8 lport,
    UINT8 *memlist,
    UINT8 *memnum
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortDot1qMode
*
* DESCRIPTION:
*  		this function support set port 802.1q check mode.
*
*	INPUTS:
          lport:user logic port. 
          mode :port 802.1q check mode.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortDot1qMode
(
    UINT8 lport,
    SW_DOT1Q_MODE mode
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortDot1qMode
*
* DESCRIPTION:
*  		this function support get port 802.1q check mode.
*
*	INPUTS:
          lport:user logic port. 
*		
*	OUTPUTS:
          mode :port 802.1q check mode.
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortDot1qMode
(
    UINT8  lport,
    SW_DOT1Q_MODE *mode
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalCreateVlan
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
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalCreateVlanID
(
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalDeleteVlan
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
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalDeleteVlanID
(
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalFlushVlanAll
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
OPL_STATUS dalFlushVlanAll
(
    void
)
{
    UINT8 recordIndex;
	BRG_T_VTT_ENTRY_t oldEntry;

    OPL_MEMSET(&oldEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));
    
    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
        
        brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, &oldEntry);
    }
    return OPL_OK;
}
/*******************************************************************************
* dalAddVlanPort
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
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalAddVlanPort
(
    UINT8  lport,
    UINT16 vid,
    VLAN_EGRESS_TYPE_E type
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalMoveVlanPort
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
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMoveVlanPort
(
    UINT8  lport,
    UINT16 vid
)
{
    return OPL_OK;
}

OPL_STATUS dalGetVlanPortEGMode
(
    UINT16 vid,
    UINT32 lport,
    VLAN_EGRESS_TYPE_E *type
)
{
    return OPL_OK;
}

OPL_STATUS dalSetPortEgressMode
(
    UINT8 lport,
    PORT_EGRESS_MODE mode
)
{
    return OPL_OK;
}
OPL_STATUS dalGetPortEgressMode
(
    UINT8 lport,
    PORT_EGRESS_MODE *mode
)
{
    return OPL_OK;
}
OPL_STATUS dalShowChipVlanEntry
(
    void
)
{
    return OPL_OK;
}

int dalSetPortDefPri
(
    UINT8 lport,
    UINT8 pri
)
{
    return OPL_OK;
}

OPL_STATUS dalTrunkEntryAdd(UINT8 portId, UINT32 vlanId)
{
	UINT32 vttIndex;
	OPL_STATUS retVal;
	
	retVal = dalVttTagEntryAdd(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_UC);

    return retVal;
}

OPL_STATUS dalTrunkEntryDel(UINT8 portId, UINT32 vlanId)
{
	UINT32 vttIndex;
	OPL_STATUS retVal;

	retVal = dalVttTagEntryDel(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_UC);

    return retVal;
}

OPL_STATUS dalTrunkEntryValueGet(UINT8 portId, UINT16 entryIndex, UINT16 *pVlanId)
{
	UINT8 i;
	UINT32 cVlan;
	UINT32 sVlan;
	OPL_STATUS retVal;
	UINT16 validCount;

	validCount = 0;
	
	retVal = dalVttTagEntryGet(entryIndex, &cVlan, &sVlan, portId);

	if(retVal == OPL_OK)
	{
		*pVlanId = (UINT16)cVlan;
	}

	return retVal;
}

OPL_STATUS dalTrunkEntryNumGet(UINT8 portId, UINT32 *pNum)
{
	UINT8 recordNum;
	OPL_STATUS retVal;
	
	retVal = dalVttNumOfRecordGet(&recordNum, portId);

	if(retVal != OPL_OK)
		return retVal;

	*pNum = (UINT32)recordNum;
	
    return OPL_OK;
}

OPL_STATUS dalTrunkEntryClear(UINT8 portId)
{
	UINT8 i;
	BRG_VTT_ENTRY_t brg_vtt_entry;
	 
	OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));

	for(i = 0; i < DAL_PORT_VTT_ENTRY_NUM; i++)
	{
		brgVttEntryHwWrite(i, &brg_vtt_entry);
	}

    return OPL_OK;
}

OPL_STATUS dalVlanQinQModeSet(unsigned char mode) /* 0 - base mode, 1 - flexible mode */
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 i =0;
	/* if vtt lookup fail,then forward the packet.*/
	brgVttLookupFailActionHwWrite(1);
	
	/* if rule id memory have items, remove it */
	/*Configure base QinQ mode when mode is 0,configure flexible QinQ mode when mode is 1*/
	if(0 == mode)
	{
		for(i=0; i< QINQ_RULE_NUM_MAX; i++)
		{
			if(	vlanQinqFlexibleUsInfo[i].valid)
			{
				if (vlanQinqFlexibleUsInfo[i].sPriority != 0)
			    {
					oplDalClsAccessLock();
					retVal =dalAclListDel(vlanQinqFlexibleUsInfo[i].sPriority);
					if(OPL_OK != retVal)
					{
						printk("dalVlanQinQFlexibleDel spriority %d failed %d\n",__LINE__,vlanQinqFlexibleUsInfo[i].sPriority);
						oplDalClsAccessUnLock();
						return retVal;
					}				
					oplDalClsAccessUnLock();
			    }
			    else
			    {
					retVal = dalVoipClsDelEntry(vlanQinqFlexibleUsInfo[i].ruleID);
					if(OPL_OK != retVal)
					{
						printk("clear flexible QinQ rule %d in the upstream failed %d\n",__LINE__,vlanQinqFlexibleUsInfo[i].ruleID);
						return retVal;
					}
			    }
				
				retVal = brgVttEntryTagDel(OPL_FALSE, OPL_TRUE, vlanQinqFlexibleUsInfo[i].svlan, vlanQinqFlexibleUsInfo[i].svlan);
				if(OPL_OK != retVal)
				{
					printk("clear flexible QinQ rule in the downstream failed %d\n",__LINE__);
					return retVal;
				}				
				OPL_MEMSET(&vlanQinqFlexibleUsInfo[i], 0, sizeof(QINQ_FlEXIBLE_VLAN_US_t));
				qinqFlexibleNum--;

			}	
		}
	}
	else if(1 == mode)
	{
		for(i=0; i< QINQ_RULE_NUM_MAX; i++)
		{
			if(	vlanQinqBaseInfo[i].valid)
			{
				if (vlanQinqBaseInfo[i].insertPri != 0)
				{
					oplDalClsAccessLock();
					retVal = dalAclListDel(vlanQinqBaseInfo[i].insertPri);
					if(OPL_OK != retVal)
					{
						printk("dalVlanQinQFlexibleDel insert priority %d failed %d\n",__LINE__,vlanQinqBaseInfo[i].insertPri);
						oplDalClsAccessUnLock();
						return retVal;
					}				
					oplDalClsAccessUnLock();
				}
				else
				{
					retVal = dalVoipClsDelEntry(vlanQinqBaseInfo[i].ruleID);	
					if(OPL_OK != retVal)
					{
						printk("dalVlanQinQFlexibleDel rule id %d failed %d\n",__LINE__,vlanQinqBaseInfo[i].ruleID);
						return retVal;
					}				
				}
				
				if (vlanQinqBaseInfo[i].remarkPri != 0)
				{
					oplDalClsAccessLock();
					retVal = dalAclListDel(vlanQinqBaseInfo[i].remarkPri);
					if(OPL_OK != retVal)
					{
						printk("dalVlanQinQFlexibleDel remark priority %d failed %d\n",__LINE__,vlanQinqBaseInfo[i].remarkPri);
						oplDalClsAccessUnLock();
						return retVal;
					}				
					oplDalClsAccessUnLock();
				}
				else
				{
					retVal = dalVoipClsDelEntry(vlanQinqBaseInfo[i].ruleRemarkId);	
					if(OPL_OK != retVal)
					{
						printk("dalVlanQinQFlexibleDel rule id %d failed %d\n",__LINE__,vlanQinqBaseInfo[i].ruleID);
						return retVal;
					}				
				}
				
			
				retVal = brgVttEntryTagDel(OPL_FALSE, OPL_FALSE,  0xFFF, 0xFFF);
				OPL_MEMSET(&vlanQinqBaseInfo[i], 0, sizeof(QINQ_FlEXIBLE_VLAN_US_t));
				qinqBaseNum--;

			}	
		}

	}
	qinqMode = mode;
	return retVal;
}

OPL_STATUS dalVlanQinQBaseAdd(unsigned short vlan,unsigned short remarkPri,unsigned short insertPri )
{
	CLS_CONFIG_INFO_t ClassItemInfoSin;
	UINT16 rule=0;
	UINT16 ruleRemark =0;
	OPL_STATUS retVal = OPL_OK;
    BRG_T_VTT_ENTRY_t   brgPvidVttEntry;
	UINT16 pvid;

	
	if(0 != qinqMode)
	{
		printk("%s %d qinqMode = %d \n",__FUNCTION__,__LINE__,qinqMode);
		return OPL_ERROR;
	}
	/* upstream */

	OPL_MEMSET((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));

	ClassItemInfoSin.vlanIdFlag = 1;
	ClassItemInfoSin.vlanIdOp =1;
	ClassItemInfoSin.vlanId.lowRange = 4095;	
	ClassItemInfoSin.vlanId.highRange = 0xfff;
	ClassItemInfoSin.bingress = 0x00;
	ClassItemInfoSin.ingressmask = 0x01;
	//ClassItemInfoSin.t_act = 0;
	ClassItemInfoSin.rulePri = remarkPri;
	ClassItemInfoSin.rmkvidOp = RMK_CHANGE_VID;
	ClassItemInfoSin.rmkvid = vlan;	
    if(remarkPri !=0)
    {
		oplDalClsAccessLock();
		retVal =dalAclListAdd(&ClassItemInfoSin);
		if (retVal != OPL_OK)
        {  
			printk("dalVlanQinQBaseAdd upstream failed! %d\n",__LINE__);	
			oplDalClsAccessUnLock();			
            return retVal;
        }
		oplDalClsAccessUnLock();

    }
    else
    {
        retVal = dalVoipClsRuleCtcAdd(&ClassItemInfoSin, &ruleRemark);
        if (retVal != OPL_OK)
        {  
			printk("dalVlanQinQBaseAdd upstream failed! %d\n",__LINE__);		
            return retVal;
        }
    }

	retVal = brgVttEntryTagAdd(OPL_FALSE, OPL_FALSE,  0xFFF, 0xFFF);
	if(OPL_OK != retVal)
	{
		printk("dalVlanQinQBaseAdd upstream change 4095 to 4095 failed! %d\n",__LINE__);
		return retVal;
	}
	
	OPL_MEMSET((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
	ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x01;

	ClassItemInfoSin.rmkvidOp = RMK_INSERT_VID;
	ClassItemInfoSin.rmkvid = vlan;
	ClassItemInfoSin.t_act = 0;	
	ClassItemInfoSin.rulePri = insertPri;

	if(insertPri !=0)
	{
		oplDalClsAccessLock();
		retVal =dalAclListAdd(&ClassItemInfoSin);
		if (retVal != OPL_OK)
		{  
			printk("dalVlanQinQBaseAdd upstream failed! %d\n",__LINE__);	
			oplDalClsAccessUnLock();			
			return retVal;
		}
		oplDalClsAccessUnLock();

	}
	else
	{
		retVal = dalVoipClsRuleCtcAdd(&ClassItemInfoSin, &rule);
		if (retVal != OPL_OK)
		{  
			printk("dalVlanQinQBaseAdd upstream failed! %d\n",__LINE__);		
			return retVal;
		}
	}

	vlanQinqBaseInfo[qinqBaseNum].vlan = vlan;	
	vlanQinqBaseInfo[qinqBaseNum].insertPri = insertPri;	
	vlanQinqBaseInfo[qinqBaseNum].remarkPri = remarkPri;	
	vlanQinqBaseInfo[qinqBaseNum].ruleID = rule;
	vlanQinqBaseInfo[qinqBaseNum].ruleRemarkId = ruleRemark;
	vlanQinqBaseInfo[qinqBaseNum].valid = OPL_TRUE;		
	qinqBaseNum++;

	return retVal;
}

OPL_STATUS dalVlanQinQBaseDel(unsigned short vlan,unsigned short remarkPri,unsigned short insertPri)
{
    BRG_T_VTT_ENTRY_t   brgPvidVttEntry;	
	OPL_STATUS retVal = OPL_OK;
	UINT32 i = 0;
	
	for(i =0; i< QINQ_RULE_NUM_MAX; i++)
	{
		if(	vlanQinqBaseInfo[i].vlan == vlan)
		{

		    if (vlanQinqBaseInfo[i].insertPri!= 0)
		    {
				oplDalClsAccessLock();
				retVal = dalAclListDel(vlanQinqBaseInfo[i].insertPri);
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel insert priority %d failed! %d\n",__LINE__,vlanQinqBaseInfo[i].insertPri);
					oplDalClsAccessUnLock();
					return retVal;
				}				
				oplDalClsAccessUnLock();
		    }
		    else
		    {
		        retVal = dalVoipClsDelEntry(vlanQinqBaseInfo[i].ruleID);	
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel rule id %d failed! %d\n",__LINE__,vlanQinqBaseInfo[i].ruleID);
					return retVal;
				}				
		    }

		    if (vlanQinqBaseInfo[i].remarkPri != 0)
		    {
				oplDalClsAccessLock();
				retVal = dalAclListDel(vlanQinqBaseInfo[i].remarkPri);
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel remark priority %d failed! %d\n",__LINE__,vlanQinqBaseInfo[i].remarkPri);
					oplDalClsAccessUnLock();
					return retVal;
				}				
				oplDalClsAccessUnLock();
		    }
		    else
		    {
		        retVal = dalVoipClsDelEntry(vlanQinqBaseInfo[i].ruleRemarkId);	
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel remark rule id %d failed!\n %d\n",__LINE__,vlanQinqBaseInfo[i].ruleID);
					return retVal;
				}				
		    }
			
			retVal = brgVttEntryTagDel(OPL_FALSE, OPL_FALSE,  0xFFF, 0xFFF);
			OPL_MEMSET(&vlanQinqBaseInfo[i], 0, sizeof(QINQ_BASE_VLAN_t));
			qinqBaseNum--;
		}
	}
	
	return retVal;

}


OPL_STATUS dalVlanTransparentAdd(unsigned short vlan)
{
	CLS_CONFIG_INFO_t ClassItemInfoSin;
	UINT16 rule=0;
	OPL_STATUS retVal = OPL_OK;

	/* upstream */
	OPL_MEMSET((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
	ClassItemInfoSin.vlanIdFlag = 1;
	ClassItemInfoSin.vlanId.lowRange = vlan;
	ClassItemInfoSin.vlanId.highRange = 0xFFF;

	ClassItemInfoSin.rmkvidOp = RMK_CHANGE_VID;
	ClassItemInfoSin.rmkvid = vlan;
	ClassItemInfoSin.rulePri = 0;	
	ClassItemInfoSin.t_act = 0;	
	retVal = dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);	
	if(OPL_OK != retVal)
	{
		printk("dalVlanTransparentAdd failed!\n");
		return retVal;
	}
	vlanQinqTransparentInfo[qinqTransparentNum].vlan = vlan;		
	vlanQinqTransparentInfo[qinqTransparentNum].ruleID = rule;	
	vlanQinqTransparentInfo[qinqTransparentNum].valid = OPL_TRUE;		
	
	qinqTransparentNum++;

	return retVal;

}

OPL_STATUS dalVlanTransparentDel(unsigned short vlan)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 i = 0;
	for(i =0; i< QINQ_RULE_NUM_MAX; i++)
	{
		if(	vlanQinqTransparentInfo[i].vlan == vlan)
		{
			retVal = dalVoipClsDelEntry(vlanQinqTransparentInfo[i].ruleID);
			if(OPL_OK != retVal)
			{
				printk("dalVlanTransparentDel rule id %d failed! %d\n",__LINE__,vlanQinqTransparentInfo[i].ruleID);
				return retVal;
			}
			OPL_MEMSET(&vlanQinqTransparentInfo[i], 0, sizeof(QINQ_TRANSPARENT_VLAN_t));
			qinqTransparentNum--;
		}
	}
	
	return retVal;

}

OPL_STATUS dalVlanQinQFlexibleAdd(unsigned short svlan, unsigned short cvlan,unsigned short sPriority)
{
	CLS_CONFIG_INFO_t ClassItemInfoSin;
	UINT16 rule=0;
	OPL_STATUS retVal = OPL_OK;
	
	if(1 != qinqMode)
	{
		printk("%s %d qinqMode = %d \n",__FUNCTION__,__LINE__,qinqMode);
		return OPL_ERROR;
	}

	/* upstream */
	OPL_MEMSET((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
	ClassItemInfoSin.vlanIdFlag = 1;
	ClassItemInfoSin.vlanIdOp =1;
	ClassItemInfoSin.vlanId.lowRange = cvlan;	
	ClassItemInfoSin.vlanId.highRange = 0xfff;
	ClassItemInfoSin.bingress = 0x00;
	ClassItemInfoSin.ingressmask = 0x01;
	ClassItemInfoSin.t_act = 0;
	ClassItemInfoSin.rulePri = sPriority;
	ClassItemInfoSin.rmkvidOp = RMK_INSERT_VID;
	ClassItemInfoSin.rmkvid = svlan;

    if(sPriority !=0)
    {
		oplDalClsAccessLock();
		retVal =dalAclListAdd(&ClassItemInfoSin);
		if (retVal != OPL_OK)
        {  
			printk("dalVlanQinQFlexibleAdd upstream failed %d\n",__LINE__);	
			oplDalClsAccessUnLock();			
            return retVal;
        }
		oplDalClsAccessUnLock();

    }
    else
    {
        retVal = dalVoipClsRuleCtcAdd(&ClassItemInfoSin, &rule);
        if (retVal != OPL_OK)
        {  
			printk("dalVlanQinQFlexibleAdd upstream failed! %d\n",__LINE__);		
            return retVal;
        }
    }
	vlanQinqFlexibleUsInfo[qinqFlexibleNum].cvlan = cvlan;
	vlanQinqFlexibleUsInfo[qinqFlexibleNum].svlan = svlan;	
	vlanQinqFlexibleUsInfo[qinqFlexibleNum].sPriority = sPriority;		
	vlanQinqFlexibleUsInfo[qinqFlexibleNum].ruleID = rule;	
	vlanQinqFlexibleUsInfo[qinqFlexibleNum].valid = OPL_TRUE;		
	
	qinqFlexibleNum++;
	/* TODO keep the rule id in memory for delete */
	
	/* downstream, if svlan has been existed skip the following steps */
	brgVttEntryTagAdd(OPL_FALSE, OPL_TRUE,  svlan, svlan );
	//brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
	return retVal;	
}
OPL_STATUS dalVlanQinQFlexibleDel(unsigned short svlan, unsigned short cvlan,unsigned short sPriority)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 i = 0;
	for(i =0; i< QINQ_RULE_NUM_MAX; i++)
	{
		if(	(vlanQinqFlexibleUsInfo[i].cvlan == cvlan) &&
			(vlanQinqFlexibleUsInfo[i].svlan == svlan) &&
			(vlanQinqFlexibleUsInfo[i].sPriority == sPriority))
		{

		    if (vlanQinqFlexibleUsInfo[i].sPriority != 0)
		    {
				oplDalClsAccessLock();
				retVal = dalAclListDel(vlanQinqFlexibleUsInfo[i].sPriority);
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel spriority %d failed! %d\n",__LINE__,vlanQinqFlexibleUsInfo[i].sPriority);
					oplDalClsAccessUnLock();
					return retVal;
				}				
				oplDalClsAccessUnLock();
		    }
		    else
		    {
		        retVal = dalVoipClsDelEntry(vlanQinqFlexibleUsInfo[i].ruleID);	
				if(OPL_OK != retVal)
				{
					printk("dalVlanQinQFlexibleDel rule id %d failed! %d\n",__LINE__,vlanQinqFlexibleUsInfo[i].ruleID);
					return retVal;
				}				
		    }
			
			OPL_MEMSET(&vlanQinqFlexibleUsInfo[i], 0, sizeof(QINQ_FlEXIBLE_VLAN_US_t));
			qinqFlexibleNum--;
		}
	}
	/*downstream rull delete*/
	retVal = brgVttEntryTagDel(OPL_FALSE, OPL_TRUE, svlan, svlan);
	if(OPL_OK != retVal)
	{
		printk("dalVlanQinQFlexibleDel failed!\n");
		return retVal;
	}
	
	return retVal;
}

OPL_STATUS dalQinQInit(void)
{
	UINT8 i;
	
	for(i = 1; i < QINQ_RULE_NUM_MAX; i++)
	{
		OPL_MEMSET(&vlanQinqFlexibleUsInfo[i],0X00,sizeof(QINQ_FlEXIBLE_VLAN_US_t));
		OPL_MEMSET(&vlanQinqTransparentInfo[i],0X00,sizeof(QINQ_TRANSPARENT_VLAN_t));
		OPL_MEMSET(&vlanQinqBaseInfo[i],0X00,sizeof(QINQ_BASE_VLAN_t));		

		qinqFlexibleNum = 0;
		qinqTransparentNum =0;
		qinqBaseNum =0;
	}
	qinqMode =0;
	return OPL_OK;	
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/


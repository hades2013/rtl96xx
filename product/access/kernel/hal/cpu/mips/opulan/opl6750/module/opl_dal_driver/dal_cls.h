/*
=============================================================================
     Header Name: dal_cls.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/13		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef DAL_CLS_H
#define DAL_CLS_H

#include <linux/list.h>

#include "opconn_lib.h"

#define CLASSIFY_MAX_FIELD                   	32
#define CLASSIFY_MAX_ITEM_NUM			TAB_CLS_RULE_LEN
#define CLT_CLASSIFY_MAX_DSCP			64

#define MAX_ACL_NAME 32
#define ACL_MAC_LEN 6
#define ACL_MAX_PRI 8


enum
{
	CLASSIFY_NO_TAG 		= OPL_ZERO,
	CLASSIFY_ONE_TAG 	= 1,
	CLASSIFY_TWO_TAG
};

enum 
{
    CLASSIFY_PPPOE_IP   = 1,
    CLASSIFY_EII_IP,    
};

enum 
{
	CLASSIFY_DAL_TABLE,
	CLASSIFY_ASIC_TABLE,
	CLASSIFY_DAL_AND_ASIC_TABLE
};

enum 
{
    /*L2 address information for all type packet*/
    CLASSIFY_DESTINATIONMAC =0,   /*6Bytes*/
    CLASSIFY_SOURCEMAC,           /*6Bytes*/


    /*Tag information for single or double tag packet*/
    CLASSIFY_FIRST_TPID,       /*2Bytes*/
    CLASSIFY_FIRST_COS,        /*4Bits*/
    CLASSIFY_FIRST_VLAN,       /*12Bits*/
    #ifdef ONLY_SUPPORT_SINGLE_TAG	
    #else
    CLASSIFY_SECOND_TPID,      /*2Byts*/
    CLASSIFY_SECOND_COS,       /*4Bits  surpport Q-in-Q mode*/
    CLASSIFY_SECOND_VLAN,      /*12Bits surpport Q-in-Q mode*/
    #endif
    /*length or type field accord with ETHERNETII or 802.3 packet type*/
    CLASSIFY_TYPE_LENGTH,      /*2Bytes  This is the type or lenth field in the L2 head*/

    /*Below define IP head field*/
    CLASSIFY_VERSION,          /*4Bits*/
    /*CLASSIFY_HEAD_LENGTH,      4Bits*/
    CLASSIFY_TOS,              /*1Bytes*/
    /*CLASSIFY_PKT_LENGTH,       2Bytes*/
    /*CLASSIFY_IDENTIFIER,       2Bytes*/
    /*CLASSIFY_FLAG,             3Bits*/
    /*CLASSIFY_SLICE_OFF,        13Bits*/
    /*CLASSIFY_TTL,              1Bytes*/ 
    CLASSIFY_PRO_TYPE,         /*1Bytes This is protocol type field in the IP head*/
    /*CLASSIFY_CHECKSUM,         2Bytes */
    CLASSIFY_SOURCE_IP,        /*4Bytes*/
    CLASSIFY_DESTINATION_IP,   /*4Bytes*/ 

    /*layer4 port number*/
    CLASSIFY_SOURCE_PORT,      /*2Bytes L4 port*/
    CLASSIFY_DESTINATION_PORT, /*2Bytes L4 port*/

    /*other field define for user specify function*/
    CLASSIFY_USER_FIELD1,
    CLASSIFY_LAST_NULL_FIELD
};

enum 
{
    /*L2 address information for all type packet*/
    CLASSIFY_PPPOE_DESTINATIONMAC =0,   /*6Bytes*/
    CLASSIFY_PPPOE_SOURCEMAC,           /*6Bytes*/


    /*Tag information for single or double tag packet*/
    CLASSIFY_PPPOE_FIRST_TPID,       /*2Bytes*/
    CLASSIFY_PPPOE_FIRST_COS,        /*4Bits*/
    CLASSIFY_PPPOE_FIRST_VLAN,       /*12Bits*/
    CLASSIFY_PPPOE_SECOND_TPID,      /*2Byts*/
    CLASSIFY_PPPOE_SECOND_COS,       /*4Bits  surpport Q-in-Q mode*/
    CLASSIFY_PPPOE_SECOND_VLAN,      /*12Bits surpport Q-in-Q mode*/

    /*length or type field accord with ETHERNETII or 802.3 packet type*/
    CLASSIFY_PPPOE_TYPE_LENGTH,      /*2Bytes  This is the type or lenth field in the L2 head*/
    CLASSIFY_PPPOE_CODE,             /*1Bytes  This is the code field in the PPP head*/

    CLASSIFY_PPPOE_PPPPRO_TYPE,      /*2Bytes  This is the type or lenth field in the L2 head*/
    CLASSIFY_PPPOE_LCPIPCPCODE,      /*2Bytes  This is the type or lenth field in the L2 head*/
    CLASSIFY_PPPOE_IPVER,            /*2Bytes  This is the type or lenth field in the L2 head*/
    

    /*Below define IP head field*/
    /*CLASSIFY_PPPOE_HEAD_LENGTH,      4Bits*/
    CLASSIFY_PPPOE_TOS,              /*1Bytes*/
    /*CLASSIFY_PPPOE_PKT_LENGTH,       2Bytes*/
    /*CLASSIFY_PPPOE_IDENTIFIER,       2Bytes*/
    /*CLASSIFY_PPPOE_FLAG,             3Bits*/
    /*CLASSIFY_PPPOE_SLICE_OFF,        13Bits*/
    /*CLASSIFY_PPPOE_TTL,              1Bytes*/ 
    CLASSIFY_PPPOE_PRO_TYPE,         /*1Bytes This is protocol type field in the IP head*/
    /*CLASSIFY_PPPOE_CHECKSUM,         2Bytes */
    CLASSIFY_PPPOE_SOURCE_IP,        /*4Bytes*/
    CLASSIFY_PPPOE_DESTINATION_IP,   /*4Bytes*/ 

    /*layer4 port number*/
    CLASSIFY_PPPOE_SOURCE_PORT,      /*2Bytes L4 port*/
    CLASSIFY_PPPOE_DESTINATION_PORT, /*2Bytes L4 port*/

    /*other field define for user specify function*/
    CLASSIFY_PPPOE_USER_FIELD1,
    CLASSIFY_PPPOE_LAST_NULL_FIELD
};

typedef enum TRAF_ACTION_s
{
	COPY_PACKETS_TO_CPU = 1,
	DROP_PACKETS = 2,
	DO_NOT_COPY_TO_CPU = 4,
	DO_NOT_DROP = 8
}TRAF_ACTION_e;

typedef enum RMK_ACTION_s
{
	CHANGE_COS_FIELD = 1,
	CHANGE_ICOS_FIELD = 2,	
	DO_NOT_OP_VLAN0 = 0,
	DO_NOT_OP_VLAN1 = 4,
	INSERT_TAG_AFTER_SA = 8,
	CHANGE_VID_AS_RULE_VID = 0XC,
}RMK_ACTION_e;

typedef enum CTC_ETHER_PRI_s
{
	CTC_ETHER_PRI_0 = 0X00,
	CTC_ETHER_PRI_1 = 0X01,
	CTC_ETHER_PRI_2 = 0X02,
	CTC_ETHER_PRI_3 = 0X03,
	CTC_ETHER_PRI_4 = 0X04,
	CTC_ETHER_PRI_5 = 0X05,
	CTC_ETHER_PRI_6 = 0X06,
	CTC_ETHER_PRI_7 = 0X07,
	CTC_ETHER_PRI_END = 0XFF
}CTC_ETHER_PRI_e;

typedef enum CTC_QUEUE_MAP_s
{
	CTC_QUEUE_MAP_0 = 0X00,
	CTC_QUEUE_MAP_1 = 0X01,
	CTC_QUEUE_MAP_2 = 0X02,
	CTC_QUEUE_MAP_3 = 0X03,
	CTC_QUEUE_MAP_4 = 0X04,
	CTC_QUEUE_MAP_5 = 0X05,
	CTC_QUEUE_MAP_6 = 0X06,
	CTC_QUEUE_MAP_7 = 0X07,
	CTC_QUEUE_MAP_END = 0XFF
}CTC_QUEUE_MAP_e;

#pragma pack(1)		/* pragma pack definition */

typedef struct CLS_CTC_TO_DAL_MAP_s
{
	UINT8 							valid;
	CLS_CONFIG_INFO_t 	clsCtcConfigInfo;
	UINT32 						dalRuleMapIndex;
}CLS_CTC_TO_DAL_MAP_t;

typedef struct opl_acl_list_s{
    struct list_head list;
    UINT16 ruleid;/*rule id */
    UINT16 tcnum;/*total list number*/
    UINT8 prinum;/*priority acl number*/
    UINT8 rulepri;/*0---normal acl, 1-8---oam acl */
    UINT16 dalindex;/*dal table index*/  
	opl_cls_hwtbl_t hwshadow;
	CLS_CONFIG_INFO_t stClassCfg;
    UINT8 status;
	UINT8  name[MAX_ACL_NAME];/*rule name ,independent*/
} opl_acl_list_t;


#pragma pack()

#define DONOT_DROP					(1<<3)
#define DONOT_COPY_TO_CPU	(1<<2)
#define DROP_PKTS						(1<<1)
#define COPY_PKTS_TO_CPU		(1)
#define RMK_INSERT_VID (2)
#define RMK_CHANGE_VID (3)

/*******************************************************************************
*
* dalClsTblEntryAdd	- Add the specific rule and mask entry to the table.
*
* DESCRIPTION:
*		This function used t calculate the address, and then mallco the rule 
*		and mask entry and then write mask and rule into the mask and rule table
*		the rule and mask entry is generate by call funtion.
*
* INPUT:
*		 pstMaskEntry	-	Pointer to classification Mask entry.
*		 pstRuleEntry	-	Pointer to classification rule entry.
*
* OUTPUT:
*		 pusCruleID		-	Pointer to classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                 - If the operation is successful.
*       OPL_ERR_NULL_POINTER 	- If the input pointer is NULL.
*		CLASSIFY_RULE_ITEM_SAME - If a same rule is existed.
*		Others                 	- An appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/

OPL_STATUS  dalClsTblEntryAdd(CLASSIFY_MASK_ENTRY_t *
                 ,CLASSIFY_RULE_ENTRY_t *, UINT16 *,opl_cls_hwtbl_t * );


/*******************************************************************************
*
* dalClsTblEntryDel - delete the specific rule and mask entry from the table.
*
* DESCRIPTION:
*       this function used t calculate the address, and then locate at the rule 
*       and mask entry and then delete mask and rule into the mask and rule table
*       the rule and mask entry is generate by call funtion.
*
* INPUT:
*       pstMaskEntry - classification Mask entry.
*       pstRuleEntry - classification rule entry.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK					- If the operation is successful.
*       OPL_ERR_NULL_POINTER	- If the input pointer is NULL.
*		Others                 	- An appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsTblEntryDel(CLASSIFY_MASK_ENTRY_t *pMaskEntry
              ,CLASSIFY_RULE_ENTRY_t *pRuleEntry, UINT16 *pCruleID);



/*******************************************************************************
*
* dalClsMaskTblWr - wirte mask entry to the table.
*
* DESCRIPTION:
*       wirte mask entry to the table. the mask entry id is specific by call 
*       function, this funciton will write both local and AISC mask table..
*
* INPUT:
*       pMask - classification Mask entry.
*       maskID     - mask entry ID.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK					- If the operation is successful.
*       OPL_ERR_NULL_POINTER	- If the input pointer is NULL.
*
* SEE ALSO: 
*/
 
OPL_STATUS dalClsMaskTblWr(CLASSIFY_MASK_ENTRY_t *pMask,UINT8 maskID);

 
 
 
/*******************************************************************************
*
* dalClsRuleTblWr - wirte rule entry to the table.
*
* DESCRIPTION:
*       wirte rule entry to the table. the rule entry id is specific by call 
*       function, this funciton will write both local and AISC rule table.
*
* INPUT:
*       ruleID     - classification rule entry ID.
*       pstRuleEntry - classification rule entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK					- If the operation is successful.
*       OPL_ERR_NULL_POINTER	- If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleTblWr(CLASSIFY_RULE_ENTRY_t *pRule, UINT16 ruleID);

 
 
/*******************************************************************************
*
* dalClsAddrTblWr		-	Wirte classify rule ID to the classify address table.
*
* DESCRIPTION:
*       This function wirte classify rule ID to the classify address table. 
*			The rule entry id is specific by call function, this funciton will 
*			write both local and AISC rule table.
*
* INPUT:
*		 ruleID		-	Classification rule entry ID.
*		 address    -   Entry number in classify hash table.
*        offset     -   Indicates ADDR0~7 of current hash entry. 
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK	- If the operation is successful.
*		Others  - An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrTblWr(UINT16 address, UINT16 offset,UINT16 ruleId);




/*******************************************************************************
*
* dalClsMaskHwWr	-	Write classify mask to hardware table.
*
* DESCRIPTION:
*		 Write classify mask to hardware table.
*
* INPUT:
*       pMask		-	Classification mask entry ID.
*       maskID	-	Classification mask entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK							-	If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		Others                 			-   An appropriate specific error code defined
*											for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskHwWr(CLASSIFY_MASK_ENTRY_t *pMask, UINT8 maskID);

 
 
/*******************************************************************************
*
* dalClsRuleHwWr	-	Write classify rule to hardware table.
*
* DESCRIPTION:
*		 Write classify rule to hardware table.
*
* INPUT:
*       pstRule		-	classification rule entry ID.
*       usRuleID		-	classification rule entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK							-	If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleHwWr(CLASSIFY_RULE_ENTRY_t *pRule,UINT16 ruleID);

 
 
/*******************************************************************************
*
* dalClsAddrHwWr	-	Write classify address to hardware table.
*
* DESCRIPTION:
*		 Write classify address to hardware table.
*
* INPUT:
*		 usAddress	-	Classify address table entry.
*		 usOffset	-	Classify address table offset.
*		 usRuleId	-	Classify rule ID.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK							-	If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrHwWr(UINT16 address, UINT16 offset,UINT16 ruleId);




/*******************************************************************************
*
* dalClsRuleTblRd - read a rule entry from the rule table.
*
* DESCRIPTION:
*       read a rule entry from the rule table.
*
* INPUT:
*       ruleID  - classification Rule entry ID.
*       tblType - CLASSIFY_LOCAL_TBL               1
*                   CLASSIFY_HW_TBL                  2
*
* OUTPUT:
*        pRule  - classify rule entry.
*
* RETURNS:
*       OPL_OK		  			- If the operation is successful.
*       OPL_ERR_NULL_POINTER	- If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleTblRd(CLASSIFY_RULE_ENTRY_t *pRule, 
                                           UINT16 ruleID, UINT8 tblType);


 
/*******************************************************************************
*
* dalClsAddrTblRd		-	Read classify rule ID from table.
*
* DESCRIPTION:
*       This function read classify rule ID from table.
*
* INPUT:
*		 usAddress		-	Index of entry of classify address table.
*		 usOffset		-	Offset of classify address table.
*		 ucTblType		-	Table type, local table or hardware table.
*
* OUTPUT:
*		 pusRuleId				- Pointer to classify rule ID.
*        OPL_ERR_NULL_POINTER   - Input pointer is null.
*
* RETURNS:
*		 OPL_OK			-	If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrTblRd(UINT16 address, UINT16 offset, 
                                         UINT16 *pRuleId, UINT8 tblType);



/*******************************************************************************
*
* dalClsEntrySet - configure rule and mask into classify rule and classify mask
*                    table
*
* DESCRIPTION:
*       this function used to generate classify mask entry 
*       accordanve the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       tables
*
* INPUT:
*        pRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK						- If the operation is successful.
*       OPL_ERR_NULL_POINTER		- If the input pointer is NULL.
*		CLASSIFY_INVALID_PKT_TYPE	- If the packet type is valid.
*		Others                 		- An appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsEntrySet(CLASSIFY_PARAMETER_t *pMaskRulePar, UINT16 *pCruleID,opl_cls_hwtbl_t *pCmaskID);



/*******************************************************************************
*
* dalClsEntryMove - remove the rule and mask entry from rule and mask table
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry 
*       accordanve the input parameter, calculate the address, and then locate 
*       at the rule and mask entry and then remove mask and rule into the mask
*       and rule table
*
* INPUT:
*        pRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK						- If the operation is successful.
*       OPL_ERR_NULL_POINTER		- If the input pointer is NULL.
*		CLASSIFY_INVALID_PKT_TYPE	- If the packet type is valid.
*		Others                 		- An appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsEntryMove(CLASSIFY_PARAMETER_t *pRulePar, UINT16 *pCruleID);


/*******************************************************************************
* dalClsRst - Reset classify engine.
*
* DESCRIPTION:
*       This function clears all classify tables, but not free the tables.
*
* INPUT:
*		None.
*
* OUTPUT:
*       None.
*
* RETURNS:
*       OPL_OK.
*
* SEE ALSO: 
*/
 OPL_STATUS dalClsRst(void);
 



/*******************************************************************************
*
* dalClsInit - initialize the classification tables.
*
* DESCRIPTION:
*       malloc memory for local classification tables.
*
* INPUT:
*       NULL.
*
* OUTPUT:
*       NULL.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsInit(void);



/*******************************************************************************
* dalClsTranActSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*		 This function reads the rule entry from rule table, changes the transform action
*		 field directly and writes the rule entry to the table.
*
* INPUT:
*		 classRuleID    - Classify rule entry ID.
*		 actValue		- Classify transform action value.
*
* OUTPUT:
*		 None.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                               		-	If the classify rule ID out of range.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsTranActSet(UINT16 classRuleID, UINT8 actValue);



/*******************************************************************************
* dalClsRmkActSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*		 This function reads the rule entry from rule table, changes the transform action
*		 field directly and writes the rule entry to the table.
*
* INPUT:
*		 classRuleID    - Classify rule entry ID.
*		 cosValue		- Classify transform action value.
*
* OUTPUT:
*		 None.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                               		-	If the classify rule ID out of range.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkActSet(UINT16 classRuleID, UINT8 rmkactVal);



/*******************************************************************************
* dalClsRmkCosSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*		 This function reads the rule entry from rule table, changes the transform action
*		 field directly and writes the rule entry to the table.
*
* INPUT:
*		 classRuleID    - Classify rule entry ID.
*		 cosValue		- Classify transform action value.
*
* OUTPUT:
*		 None.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                               		-	If the classify rule ID out of range.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkCosSet(UINT16 classRuleID, UINT8 cosValue);




/*******************************************************************************
* dalClsRmkIcosSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*		 This function reads the rule entry from rule table, changes the transform action
*		 field directly and writes the rule entry to the table.
*
* INPUT:
*		 classRuleID    - Classify rule entry ID.
*		 iCosVal		- Classify transform action value.
*
* OUTPUT:
*		 None.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                               		-	If the classify rule ID out of range.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkIcosSet(UINT16 classRuleID, UINT8 iCosVal);



/*******************************************************************************
* dalClsRmkVidSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*		 This function reads the rule entry from rule table, changes the transform action
*		 field directly and writes the rule entry to the table.
*
* INPUT:
*		 classRuleID    - Classify rule entry ID.
*		 rmkVid		- Classify rmkVlan Value
*
* OUTPUT:
*		 None.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                               		-	If the classify rule ID out of range.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkVidSet(UINT16 classRuleID, UINT16 rmkVid);


/*******************************************************************************
*
* dalClsEgressPortCfg - Configure egress port and port mask field for classification
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable 
*		the egress port field mask and write egress port value into the rule entry.
*		
*		If the input parameter enable=0, this function will disable
*       the egress port field mask and clear its value from the rule entry.
*
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*       portID			- Logic port ID.
*		eMask			- egress port mask.
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsEgressPortCfg(UINT16 classItemID, UINT8 pktType, UINT8 portID, UINT8 eMask, UINT8 bEnable);


/*******************************************************************************
*
* dalClsIngressPortCfg - Configure ingress port and port mask field for classification
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable 
*		the ingress port field mask and write ingress port value into the rule entry.
*		
*		If the input parameter enable=0, this function will disable
*       the ingress port field mask and clear its value from the rule entry.
*
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*       portID			- Logic port ID.
*		iMask			- Ingress port mask.
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIngressPortCfg(UINT16 classItemID, UINT8 pktType, UINT8 portID, UINT8 iMask, UINT8 bEnable);


/*************************************************************************
* NAME: cltClsTranAct - Configure classify transform action.
*
* DESCRIPTION:
*		This function writes transformation value into the TRANF_ACT field
*		of the rule table.
*
* INPUTS:
*		classItemID		- Classify item index.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		actValue			- action value.
*
* OUTPUTS:
*		None.
*		
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsTranActCfg(UINT16 classItemID, UINT8 pktType, UINT8 actValue);


/*************************************************************************
* NAME: dalClsRmkActCfg - Configure classify transform action.
*
* DESCRIPTION:
*		This function writes transformation value into the TRANF_ACT field
*		of the rule table.
*
* INPUTS:
*		classItemID		- Classify item index.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		rmkActVal			- action value.
*
* OUTPUTS:
*		None.
*		
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkActCfg(UINT16 classItemID, UINT8 pktType, UINT8 rmkActVal);



/*************************************************************************
* NAME: dalClsRmkCosCfg - Configure classify transform action.
*
* DESCRIPTION:
*		This function writes transformation value into the TRANF_ACT field
*		of the rule table.
*
* INPUTS:
*		classItemID		- Classify item index.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		cosValue			- action value.
*
* OUTPUTS:
*		None.
*		
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkCosCfg(UINT16 classItemID, UINT8 pktType, UINT8 cosValue);


/*************************************************************************
* NAME: dalClsRmkIcosCfg - Configure classify transform action.
*
* DESCRIPTION:
*		This function writes transformation value into the TRANF_ACT field
*		of the rule table.
*
* INPUTS:
*		classItemID		- Classify item index.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		IcosValue		- icos value.
*
* OUTPUTS:
*		None.
*		
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkIcosCfg(UINT16 classItemID, UINT8 pktType, UINT8 iCosValue);


/*************************************************************************
* NAME: dalClsRmkVidCfg - Configure classify transform action.
*
* DESCRIPTION:
*		This function writes transformation value into the TRANF_ACT field
*		of the rule table.
*
* INPUTS:
*		classItemID		- Classify item index.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		rmkVid				- vid value.
*
* OUTPUTS:
*		None.
*		
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkVidCfg(UINT16 classItemID, UINT8 pktType, UINT16 rmkVid);




/*******************************************************************************
*
* dalClsMaskPrioCfg - Set classify mask priority.
*
* DESCRIPTION:
*       This function writes classify mask priority directly.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*       maskPrio		- Mask priority value.
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskPrioCfg(UINT16 classItemID, UINT8 pktType, UINT8 maskPrio);


/*******************************************************************************
* dalClsDestMacAddrCfg - Configure the destination MAC address field of current 
*							classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the destination MAC 
*		address field mask and write MAC value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the destination MAC
*		address field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		pDestMac		- Destination MAC address value. Input with string format.
*		pUsrMask		- MAC address mask. Input with string format 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsDestMacAddrCfg(UINT16 classItemID, UINT8 pktType, INT8* pDestMac, INT8* pUsrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsSrcMacAddrCfg - Configurethe source MAC address field of current 
*							classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the source MAC 
*		address field mask and write MAC value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the source MAC
*		address field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		pSrcMac			- Source MAC address value. Input with string format.
*		pUsrMask		- MAC address mask. Input with string format 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsSrcMacAddrCfg(UINT16 classItemID, UINT8 pktType, INT8* pSrcMac, INT8* pUsrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsFirstCosCfg - Configure the first cos field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the first cos 
*		field mask and write cos value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the first cos
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		cosVal			- VLAN cos value.
*		usrMask			- VLAN cos user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsFirstCosCfg(UINT16 classItemID, UINT8 pktType, UINT8 cosVal, UINT8 usrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsFirstVidCfg - Configure the first VLAN ID field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the first VLAN ID 
*		field mask and write VLAN ID value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the first VLAN ID
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		vlanID			- VLAN ID value.
*		usrMask			- VLAN ID user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsFirstVidCfg(UINT16 classItemID, UINT8 pktType, UINT16 vlanID, UINT16 usrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsTypeLenCfg - Configure the type/length field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the type/length 
*		field mask and write type/length value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the type/length
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		typeLen			- Type/Length value.
*		usrMask			- Layer 4 source port user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsTypeLenCfg(UINT16 classItemID, UINT8 pktType, UINT16 typeLen, UINT16 usrMask, UINT8 bEnable);


/*******************************************************************************
*
* dalClsDestIpCfg - Configure the destination IP address field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the destination IP 
*		address field mask and write IP value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the destination IP
*		address field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		pDestIp			- Destination IP address. Input in string format.
*		pUserMask		- Destination IP mask. Input in string format.
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsDestIpCfg(UINT16 classItemID, UINT8 pktType, INT8* pDestIp, INT8* pUsrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsSrcIpCfg - Configure the source IP address field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the source IP 
*		address field mask and write IP value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the source IP
*		address field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		pSrcIp			- Source IP address. Input in string format.
*		pUserMask		- Destination IP mask. Input in string format.
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsSrcIpCfg(UINT16 classItemID, UINT8 pktType, INT8* pSrcIp, INT8* pUsrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsIpTypeCfg - Configure the IP type field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP type
*		field mask and write IP type value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the IP type
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		ipType			- IP type value. 
*		usrMask			- IP type user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIpTypeCfg(UINT16 classItemID, UINT8 pktType, UINT8 ipType, UINT8 usrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsIpTosCfg - Configure the IP tos field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP tos
*		field mask and write IP tos value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the IP tos
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		ipTos			- IP tos value. 
*		usrMask			- IP tos user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIpTosCfg(UINT16 classItemID, UINT8 pktType, UINT8 ipTos, UINT8 usrMask, UINT8 bEnable);


/*******************************************************************************
* dalClsL4DestPortCfg - Configure the layer 4 destination port field of current 
*							classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the layer 4 destination port 
*		field mask and write port value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the layer 4 destination port
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		L4DestPortID	- Layer 4 destination port ID.
*		usrMask			- Layer 4 destination port user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsL4DestPortCfg(UINT16 classItemID, UINT8 pktType, UINT16 L4DestPortID, UINT16 usrMask, UINT8 bEnable);

/*******************************************************************************
* dalClsL4SrcPortCfg - Configure the layer 4 source port field of current 
*							classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the layer 4 source port 
*		field mask and write port value into the rule entry.
*
*		If the input parameter enable=0, this function will disable the layer 4 source port
*		field mask and clear its value from the rule entry.
*
* INPUT:
*		classItemID 	- classify item index. 1~511 is valid.
*		pktType			- Packet protocol type. 1:PPPoE, 2:EtherNet.
*		L4SrcPortID		- Layer 4 source port ID.
*		usrMask			- Layer 4 source port user mask. 
*		bEnable			- 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsL4SrcPortCfg(UINT16 classItemID, UINT8 pktType, UINT16 L4SrcPortID, UINT16 usrMask, UINT8 bEnable);



/*******************************************************************************
*
* dalClsRuleEntryShow - show rule table of classify
*
* DESCRIPTION:
*       show rule table of classify
*
* INPUT:
*       startId:
*		endId:
*		mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleEntryShow(UINT16 startId,UINT16 endId,UINT8 mode);


/*******************************************************************************
*
* dalClsMaskEntryShow - show mask table of classify
*
* DESCRIPTION:
*       show mask table of classify
*
* INPUT:
*       startId:
*		endId:
*		mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskEntryShow(UINT16 startId,UINT16 endId,UINT8 mode);

/*******************************************************************************
*
* dalClsAddrEntryShow - show addr table of classify
*
* DESCRIPTION:
*       show addr table of classify
*
* INPUT:
*       startId:
*		endId:
*		mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrEntryShow(UINT16 startId,UINT16 endId,UINT8 mode);
/*******************************************************************************
* dalClsRuleDel - this function is used to remove the specified rule
*
* DESCRIPTION:
*
* INPUT:
*
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleDel(UINT16 classItemID);

/*******************************************************************************
* dalClsConvertPara - this function is used to fix the classs parameter from ctc parameter
*
* DESCRIPTION:
*
* INPUT:
*		pClassItemInfo:	 classifyItem
*		stClassCfg:			 ctcConfig Info.
*		pktType:
*
* OUTPUT:
*       None.
*
* RETURN:
*		An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsConvertPara(CLASSIFY_ITEM_t *pClassItemInfo,CLS_CONFIG_INFO_t *stClassCfg,UINT8 pktType);

/*******************************************************************************
* dalClsCtcInit - this function is used to init ctc classification.
*
* DESCRIPTION:
*
* INPUT:
*	void
*
* OUTPUT:
*       None.
*
* RETURN:
*		
*
* SEE ALSO: 
*/
OPL_STATUS dalClsCtcInit(void);

/*******************************************************************************
* dalClsRuleCtcAdd - this function is used to add a ctc rule.
*
* DESCRIPTION:
*
* INPUT:
*		stClassCfg:
*		ruleId:
*
* OUTPUT:
*       None.
*
* RETURN:
*		
*
* SEE ALSO: 
*/

OPL_STATUS dalClsRuleCtcAdd(CLS_CONFIG_INFO_t *stClassCfg, UINT16 *ruleId);

/*******************************************************************************
* dalClsRuleCtcDel - this function is used to delete a ctc classify rule
*
* DESCRIPTION:
*
* INPUT:
*		stClassCfg: 	ctc config info
*		ruleId:			
* OUTPUT:
*       None.
*
* RETURN:
*		
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleCtcDel(CLS_CONFIG_INFO_t *stClassCfg, UINT16 *ruleId);


/*******************************************************************************
* dalClsRuleCtcClear - this function is used to clear all ctc classify rule
*
* DESCRIPTION:
*
* INPUT:
*
*
* OUTPUT:
*       None.
*
* RETURN:
*		
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleCtcClear(void);

OPL_STATUS dalExClsDelEntry(UINT32 ruleIndex);

OPL_STATUS dalClsRmkVidActSet(UINT16 classRuleID,UINT8 rmkVidAct,UINT16 rmkVid);

OPL_STATUS dalClsIpVersionCfg(UINT16 classItemID,
                           UINT8 pktType,
                           UINT8 ipVersion,
                           UINT8 usrMask,
                           UINT8 bEnable);

OPL_STATUS dalClsFirstTpidCfg(UINT16 classItemID,
                           UINT8 pktType,
                           UINT16 firsttpid,
                           UINT16 usrMask,
                           UINT8 bEnable);
OPL_STATUS dalClsRuleDel(UINT16 classItemID);

OPL_STATUS dalVoipClsRuleCtcAdd(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId);

OPL_STATUS dalClsDelEntry(UINT32 ruleid);

OPL_STATUS dalOnuClsRuleCtcAdd(opl_acl_list_t *oamcls);

OPL_STATUS dalOnuClsRuleCtcChange(opl_acl_list_t *oamcls,UINT16 newmask);

OPL_STATUS dalOnuClsRuleCtcDel(opl_acl_list_t *oamcls);

OPL_STATUS dalVoipClsDelEntry(UINT32 ruleid);

OPL_STATUS dalClaCfgCheck(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId);

OPL_STATUS dalClsRmkActGet(UINT16 classItemID,UINT8 pktType,UINT8 *rmkActVal);

OPL_STATUS dalClsCfgShow(UINT16 usFirstItem,UINT16 usLastItem);

void oplDalClsAccessLock(void);

void oplDalClsAccessUnLock(void);

int dalAclListDelLowestPre();

opl_acl_list_t * dalAclListFindPre(UINT8 precedence);

int dalAclListDel(UINT8 precedence);

int dalAclListAdd(CLS_CONFIG_INFO_t *stClassCfg);




#endif


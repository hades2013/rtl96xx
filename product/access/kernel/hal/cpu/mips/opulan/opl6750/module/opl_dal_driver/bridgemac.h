/*
=============================================================================
     Header Name: bridgemac.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/14		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef BRIDGEMAC_H
#define BRIDGEMAC_H
#include "opl_driver.h"
#include "opl_errno.h"
//#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"
//#include "opl_ethernet.h"
//#include "dal_lib.h"

#define BRG_RSV_MAC_NUM				33
#define PON_BRG_MAC_ENTRY_NUM    TAB_PON_MAC_LEN


#define GMAC_TBI_MODE			0
#define GMAC_GMII_MODE			1

/*#define DAL_NUM_OF_PORTS    1 */
#define MAX_NUM_OF_VLAN 4095


#ifndef NULL
#define NULL 0
#endif


typedef struct SOFT_LEARN_MAC_s
{
  UINT16 portId;
  UINT16 vlanId;
  UINT8 mac[6];
  UINT16 null;
  struct SOFT_LEARN_MAC_s *next;
}SOFT_LEARN_MAC_t;



typedef enum PON_MAC_STATE_s
{
	NOT_VALID,
	NOT_VALID_STATIC,
	VALID_HARDWARE_LEARN,
	VALID_STATIC
}PON_MAC_STATE_e;

typedef enum PON_MAC_AGE_s
{
	AGE_OUT,
	NOT_AGE_OUT
}PON_MAC_AGE_e;

typedef enum BRG_RSV_MAC_CTL_ACTION_s
{
	RSV_DROP = 0,
	RSV_SEND_TO_CPU = 1,
	RSV_FLOODING_ANOTHER_PORT = 2,
	RSV_RESERVED
}BRG_RSV_MAC_CTL_ACTION_e;

typedef enum BRG_ARL_AGE_CTL_s
{
	BRG_ARL_SOFTLERN_EN,
	BRG_ARL_ISOLATION,
	BRG_ARL_FULL_DROP,
	BRG_ARL_AGE_EN,
	BRG_ARL_AGE_VAL,
	BRG_ARL_CTL_END
}BRG_ARL_AGE_CTL_e;

typedef struct PON_BRG_MAC_TAB_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 state:2;
	UINT32 age:1;
	UINT32 portId:1;
	UINT32 vlanId:12;
	UINT32 common0:16;
	
	UINT32 common1:32;
#else
	UINT32 common0:16;
	UINT32 vlanId:12;
    UINT32 portId:1;
	UINT32 age:1;
	UINT32 state:2;

	UINT32 common1:32;
#endif
}PON_BRG_MAC_TAB_t;

typedef struct PON_BRG_MAC_TAB_COMMON_s
{
	union 
	{
		PON_BRG_MAC_TAB_t arl_tab;
		UINT8 common[8];
	}common;	
}PON_BRG_MAC_TAB_COMMON_t;

typedef struct RSV_MAC_CONTROL_s
{
	UINT8 bfReserved:3;
	UINT8 bfRsvCtl:2;
	UINT8 bfIcos:3;
}RSV_MAC_CONTROL_t;

/* begin added by jiangmingli for N:1 aggregation */
/* arl soft learnning callback function  */
typedef int (* BRG_SOFT_LEARN_FUNC)(UINT16 usPortid, UINT16 usVlanid, UINT8 *aucMacAddr);

OPL_STATUS brgArlSoftLearnFuncRegister(BRG_SOFT_LEARN_FUNC pfFunc);
/* end added by jiangmingli for N:1 aggregation */


OPL_STATUS brgGeParserCounterShow(void);


OPL_STATUS brgPortCounterShow(void);


OPL_STATUS brgCounterShow(void);

OPL_STATUS tmCounterShow(void);


OPL_STATUS ponMpcpCounterShow(void);


OPL_STATUS hostDmaCounterShow(void);


OPL_STATUS allCounterShow(void);

/*******************************************************************************
* brgUsrDefineRsvMacHwWrite
*
* DESCRIPTION:
*  opconn support two user define reserved mac address
* 
*  this function is used to set a user define reserved mac address.
* INPUTS: 
*		num : 0 or 1
* 		mac: user define mac address
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacHwWrite(UINT8 num,UINT8 *mac);

/*******************************************************************************
* brgUsrDefineRsvMacHwRead
*
* DESCRIPTION:
*  opconn support two user define reserved mac address
* 
*  this function is used to get a user define reserved mac address.
* INPUTS: 
*		num : 0 or 1
* 		
* OUTPUTS: 
*		mac: user define mac address
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacHwRead(UINT8 num,UINT8 *mac);

/*******************************************************************************
* brgUsrDefineRsvMacCtrlHwWrite
*
* DESCRIPTION:
*	opconn support two user define reserved mac address,and the user define mac's icos and ctrol such as 
*	drop,send to cpu,flood to another port.
* 
*  this function is used to set the user define mac control info
* INPUTS: 
*		num : 0 or 1
* 		icos: the icos
*		macCtl:the ctl
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacCtrlHwWrite(UINT8 macNum,UINT8 icos,UINT8 macCtl);

/*******************************************************************************
* brgUsrDefineRsvMacCtrlHwRead
*
* DESCRIPTION:
*	opconn support two user define reserved mac address,and the user define mac's icos and ctrol such as 
*	drop,send to cpu,flood to another port.
* 
*  this function is used to get the user define mac control info
* INPUTS: 
*		num : 0 or 1
* 		
* OUTPUTS: 
*		icos: the icos
*		macCtl:the ctl
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacCtrlHwRead(UINT8 macNum,UINT8 *icos,UINT8 *macCtl);

/*******************************************************************************
* brgRsvMacCtrlHwWrite
*
* DESCRIPTION:
*  opconn support a function of seting the 33 reserved mac address packet's icos and control action, such
*	 as drop, send to cpu,flood to another port.
* 
*  this function is used to set a reserved mac address control
* INPUTS: 
*		macNum 	: 0 to 32
*		icos			: interal cos
*		macCtl    : 
*							RSV_DROP = 0,
*							RSV_SEND_TO_CPU = 1,
*							RSV_FLOODING_ANOTHER_PORT = 2,
*							RSV_RESERVED
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgRsvMacCtrlHwWrite(UINT8 macNum,UINT8 icos,UINT8 macCtl);

/*******************************************************************************
* brgRsvMacCtrlHwRead
*
* DESCRIPTION:
*  opconn support a function of seting the 33 reserved mac address packet's icos and control action, such
*	 as drop, send to cpu,flood to another port.
* 
*  this function is used to get a reserved mac address control
* INPUTS: 
*		macNum 	: 0 to 32
* OUTPUTS: 
*		icos			: interal cos
*		macCtl    : 
*							RSV_DROP = 0,
*							RSV_SEND_TO_CPU = 1,
*							RSV_FLOODING_ANOTHER_PORT = 2,
*							RSV_RESERVED
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgRsvMacCtrlHwRead(UINT8 macNum,UINT8 *icos,UINT8 *macCtl);

/*******************************************************************************
* brgArlAgeCtrlHwWrite
* 
*  this function is used to set the bridge age control info
* INPUTS: 
*		type 	:
*				BRG_ARL_SOFTLERN_EN
*				BRG_ARL_AGE_EN,
*				BRG_ARL_AGE_VAL,
*		value:
*
* OUTPUTS: 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlAgeCtrlHwWrite(UINT8 type,UINT32 value);

/*******************************************************************************
* brgArlAgeCtrlHwRead
* 
*  this function is used to get the bridge age control info
* INPUTS: 
*		type 	:
*				BRG_ARL_SOFTLERN_EN
*				BRG_ARL_AGE_EN,
*				BRG_ARL_AGE_VAL,
*
* OUTPUTS: 
*		value:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlAgeCtrlHwRead(UINT8 type,UINT32 *value);

/*******************************************************************************
* brgArlAgeCntInSecHwWrite
* 
*  this function is used to set age time value per second
* INPUTS: 
*		value:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlAgeCntInSecHwWrite(UINT32 value);

/*******************************************************************************
* brgArlAgeCntInSecHwRead
* 
*  this function is used to get age time value per second
* INPUTS: 
*		
* OUTPUTS: 
*		value:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlAgeCntInSecHwRead(UINT32 *value);

#ifdef PON_MAC_TAB_SUPPORT
/*******************************************************************************
* brgArlEntryHwRead
*
* DESCRIPTION:
*  there is a mac table in opconn,which used to for subscriber Mac address number limiting,local MAC address 
*	 isolation and 802.1x authentication.
* 
*  this api is used to get a mac tab records from Mac table.
* INPUTS: 
* 		entryId:(0-127)
* OUTPUTS: 
*		pstEntry:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlEntryHwRead(UINT32 entryId,PON_BRG_MAC_TAB_t *pstEntry);

/*******************************************************************************
* brgArlEntryHwWrite
*
* DESCRIPTION:
*  there is a mac table in opconn,which used to for subscriber Mac address number limiting,local MAC address 
*	 isolation and 802.1x authentication.
* 
*  this api is used to set a mac tab records from Mac table.
* INPUTS: 
* 		entryId:(0-127)
*		pstEntry:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlEntryHwWrite(UINT32 entryId,PON_BRG_MAC_TAB_t *pstEntry);

/*******************************************************************************
* brgArlMacExist
*
* DESCRIPTION:
*
*	this function is used to check wether the mac exist in the arl table
*
* INPUTS: 
* 		mac: 
*
* OUTPUTS: 
*		entryId: 
*		found:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacExist(PON_BRG_MAC_TAB_t *pstEntry, UINT32 *entryId,UINT8 *found);

/*******************************************************************************
* brgArlFreeEntryFind
*
* DESCRIPTION:
*
*	this function is used to search a free entry in the arl table
*
* INPUTS: 
*
* OUTPUTS: 
*		entryId: 
*		found:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlFreeEntryFind(UINT32 *entryId,UINT8 *found);

/*******************************************************************************
* brgArlMacAdd
*
* DESCRIPTION:
*  there is a mac table in opconn,which used to for subscriber Mac address number limiting,local MAC address 
*	 isolation and 802.1x authentication.
* 
*  this function is used to add a specified mac to arl table.
* INPUTS: 
* 		mac:macAddress
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacAdd(UINT16 portId,UINT16 vlanId,UINT8 *mac);

/*******************************************************************************
* brgArlMacDel
*
* DESCRIPTION:
*  there is a mac table in opconn,which used to for subscriber Mac address number limiting,local MAC address 
*	 isolation and 802.1x authentication.
* 
*  this function is used to delete a specified mac to arl table.
* INPUTS: 
* 		mac:macAddress
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacDel(UINT16 portId,UINT16 vlanId,UINT8 *mac);

/*******************************************************************************
* brgArlInit
*
* DESCRIPTION:
*  there is a mac table in opconn,which used to for subscriber Mac address number limiting,local MAC address 
*	 isolation and 802.1x authentication.
* 
*  this function is used init the arl table.
* INPUTS: 
* 		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlInit(void);

/*******************************************************************************
* brgArlMacEntryShow
*
* DESCRIPTION:
* 
*  this function is used to show the mac table
* INPUTS: 
* 		startEntryId:
*		endEntryId:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacEntryShow(UINT32 startEntryId,UINT32 endEntryId);

/*******************************************************************************
* brgArlMacAddTest
*
* DESCRIPTION:
* 
*  this function is used to add a static mac address in arl table
* INPUTS: 
*		string: "xx:xx:xx:xx:xx:xx"
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacAddTest(UINT16 portId,UINT16 vlanId,UINT8 *string);

/*******************************************************************************
* brgArlMacDelTest
*
* DESCRIPTION:
* 
*  this function is used to remove a static mac address in arl table
* INPUTS: 
*		string: "xx:xx:xx:xx:xx:xx"
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlMacDelTest(UINT16 portId,UINT16 vlanId,UINT8 *string);

/*******************************************************************************
* brgRsvMacCtrlInit
*
* DESCRIPTION:
* 
*  this function is used init the reserved mac control function
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgRsvMacCtrlInit(void);

#endif
/*******************************************************************************
* brgUsrDefineRsvMacAdd
*
* DESCRIPTION:
*  	this function is used add a user define mac address in opconn,opconn can support two userdefine reserved 
*		mac.
*
* INPUTS: 
*		num: 0 or 1
*		macString:"xx:xx:xx:xx:xx:xx"
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacAdd(UINT8 num,UINT8 *macString);

/*******************************************************************************
* brgUsrDefineRsvMacShow
*
* DESCRIPTION:
*  	this function is used to show the userdefined reserved mac address
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgUsrDefineRsvMacShow(void);

/*******************************************************************************
* brgArlIntProcess
*
* DESCRIPTION:
*  	this function is used to show the userdefined reserved mac address
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgArlIntProcess(void);


OPL_STATUS brgArlAgeTimeHwWrite(UINT32 ageTime);


OPL_STATUS brgArlAgeTimeHwRead(UINT32 *ageTime);


OPL_STATUS brgArlLearnEnHwWrite(UINT8 enable);


OPL_STATUS brgArlLearnEnHwRead(UINT8 *enable);


OPL_STATUS brgArlSoftLearnEnHwWrite(UINT8 enable);


OPL_STATUS brgArlSoftLearnEnHwRead(UINT8 *enable);


OPL_STATUS brgArlIsoLationEnHwWrite(UINT8 enable);


OPL_STATUS brgArlIsoLationEnHwRead(UINT8 *enable);


OPL_STATUS brgArlFullDropEnHwWrite(UINT8 enable);


OPL_STATUS brgArlFullDropEnHwRead(UINT8 *enable);


OPL_STATUS brgArlAgeEnHwWrite(UINT8 enable);


OPL_STATUS brgArlAgeEnHwRead(UINT8 *enable);


OPL_STATUS brgArlConfigShow(void);

OPL_STATUS brgArlMacNumLimitSet(UINT8 enable, UINT32 numOfArlNum);

#endif

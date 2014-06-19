/*
=============================================================================
     Header Name: bridgeport.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/10		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef BRIDGEPORT_H
#define BRIDGEPORT_H

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

typedef enum BRG_PORT_NUM_s
{
	BRG_PORT_GE = 0,
	BRG_PORT_PON = 1,
	BRG_PORT_CPU = 2,
	BRG_PORT_END
}BRG_PORT_NUM_e;

typedef enum BRG_PON_PORT_LINK_CONTROL_s
{
 	PON_PORT_TRSMT_EN,
	PON_PORT_TAG,
	PON_PORT_UNTAG,
	PON_PORT_I_COS,
	PON_PORT_I_ACTION,
	PON_PORT_PVID,
	PON_PORT_COS,
	PON_PORT_RSV,
	PON_PORT_LP,
	PON_PORT_LERN_LIMIT,
	PON_PORT_LERN_DROP,
	PON_PORT_LERN,
	PON_PORT_RCV_EN,
	PON_PORT_END
}BRG_PON_PORT_LINK_CONTROL_e;

typedef enum BRG_GE_PORT_LINK_CONTROL_s
{
	GE_PORT_TRSMT_EN,
	GE_PORT_TAG,
	GE_PORT_UNTAG,
	GE_PORT_I_COS,
	GE_PORT_I_ACTION,
	GE_PORT_PVID,
	GE_PORT_COS,
	GE_PORT_RSV,
	GE_PORT_LP,
	GE_PORT_LERN_LIMIT,
	GE_PORT_LERN_DROP,
	GE_PORT_LERN,
	GE_PORT_RCV_EN,
	GE_PORT_END
}BRG_GE_PORT_LINK_CONTROL_e;

typedef struct BRG_PORT_TAB_GE_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 reserved:4;
	UINT32 rcv_en:1;
	UINT32 lern:1;
	UINT32 lern_drop:1;
	UINT32 lern_limit:1;
	UINT32 lp:1;
	UINT32 rsv_en:1;
	UINT32 cos:3;
	UINT32 pvid:12;
	UINT32 i_action:1;
	UINT32 i_cos:3;
	UINT32 untag:1;
	UINT32 tag:1;
	UINT32 trsmt_en:1;
#else
	UINT32 trsmt_en:1;
	UINT32 tag:1;
	UINT32 untag:1;
	UINT32 i_cos:3;
	UINT32 i_action:1;
	UINT32 pvid:12;
	UINT32 cos:3;
	UINT32 rsv_en:1;
	UINT32 lp:1;
	UINT32 lern_limit:1;
	UINT32 lern_drop:1;
	UINT32 lern:1;
	UINT32 rcv_en:1;
	UINT32 reserved:4;
#endif
}BRG_PORT_TAB_GE_t;

typedef struct BRG_PORT_TAB_PON_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 reserved:4;
	UINT32 rcv_en:1;
	UINT32 lern:1;
	UINT32 lern_drop:1;
	UINT32 lern_limit:1;
	UINT32 lp:1;
	UINT32 rsv_en:1;
	UINT32 cos:3;
	UINT32 pvid:12;
	UINT32 i_action:1;
	UINT32 i_cos:3;
	UINT32 untag:1;
	UINT32 tag:1;
	UINT32 trsmt_en:1;
#else
	UINT32 trsmt_en:1;
	UINT32 tag:1;
	UINT32 untag:1;
	UINT32 i_cos:3;
	UINT32 i_action:1;
	UINT32 pvid:12;
	UINT32 cos:3;
	UINT32 rsv_en:1;
	UINT32 lp:1;
	UINT32 lern_limit:1;
	UINT32 lern_drop:1;
	UINT32 lern:1;
	UINT32 rcv_en:1;
	UINT32 reserved:4;
#endif
}BRG_PORT_TAB_PON_t;

typedef struct BRG_PORT_TAB_ENTRY_s
{
	union 
	{
		BRG_PORT_TAB_GE_t 	port_entry_ge;
		BRG_PORT_TAB_PON_t  port_entry_pon;
	}common;
}BRG_PORT_TAB_ENTRY_t;

/*******************************************************************************
* brgPonPortLinkControlSet
*
* DESCRIPTION:
*  there are two port table in the asic.one is ge port while the other is pon port.
*	
*  this function is used to config the port link control.
*	INPUT:
*		type:
*			PON_PORT_TRSMT_EN
*			PON_PORT_TAG
*			PON_PORT_UNTAG
*			PON_PORT_PVID
*			PON_PORT_COS
*			PON_PORT_I_COS
*			PON_PORT_I_ACTION
*			PON_PORT_LP
*			PON_PORT_RSV
*			PON_PORT_RCV_EN
*		value:
*			
* 
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPonPortLinkControlSet(BRG_PON_PORT_LINK_CONTROL_e type,UINT32 value);
                                  
/*******************************************************************************
* brgPonPortLinkControlGet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is get the port link control parameter.
*
*	INPUT:
*		type:
*			PON_PORT_TRSMT_EN
*			PON_PORT_TAG
*			PON_PORT_UNTAG
*			PON_PORT_PVID
*			PON_PORT_COS
*			PON_PORT_I_COS
*			PON_PORT_I_ACTION
*			PON_PORT_LP
*			PON_PORT_RSV
*			PON_PORT_RCV_EN			
* OUTPUTS:
*			value   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPonPortLinkControlGet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 *value);

/*******************************************************************************
* brgGePortLinkControlSet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is config the ge port table
*
*	INPUTS:
*		type:
*			GE_PORT_TRSMT_EN
*			GE_PORT_TAG
*			GE_PORT_UNTAG
*			GE_PORT_I_COS
*			GE_PORT_I_ACTION
*			GE_PORT_PVID
*			GE_PORT_COS
*			GE_PORT_RSV
*			GE_PORT_LP
*			GE_PORT_LERN_LIMIT
*			GE_PORT_LERN_DROP
*			GE_PORT_LERN
*			GE_PORT_RCV_EN
*		vale:
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgGePortLinkControlSet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 value);

/*******************************************************************************
* brgGePortLinkControlGet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is get the ge port table
*
*	INPUTS:
*		type:
*			GE_PORT_TRSMT_EN
*			GE_PORT_TAG
*			GE_PORT_UNTAG
*			GE_PORT_I_COS
*			GE_PORT_I_ACTION
*			GE_PORT_PVID
*			GE_PORT_COS
*			GE_PORT_RSV
*			GE_PORT_LP
*			GE_PORT_LERN_LIMIT
*			GE_PORT_LERN_DROP
*			GE_PORT_LERN
*			GE_PORT_RCV_EN
*
* OUTPUTS:
*		value
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgGePortLinkControlGet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 *value);

/*******************************************************************************
* brgPortEntryHwWrite
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is used to config the the port entry
* INPUTS:
*       portNum: 0:ge port;1 : pon port,2: fe port
*       pstEntry: entry value to be configed.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryHwWrite(UINT32 portNum,UINT32 *pstEntry);
	
/*******************************************************************************
* brgPortEntryHwRead
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is used to read out the port entry value
* INPUTS:
*       portNum: 0:ge port;1 : pon port,2: fe port
*       pstEntry: entry value to be configed.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryHwRead(UINT32 portNum,UINT32 *pstEntry);

/*******************************************************************************
* brgPortInit
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is init the port entry to default value
* 
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortInit(void);

/*******************************************************************************
* brgPortEntryShow
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is init the port entry to default value
*
*	INPUT:
*		startPortNum: 	0 - 1
*		endPortNum:		0 - 1	
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryShow(UINT32 startPortNum,UINT32 endPortNum);

#endif

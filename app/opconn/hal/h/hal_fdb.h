/*
=============================================================================
     Header Name: hal_fdb.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef _HAL_FDB_H_
#define _HAL_FDB_H_


#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

/* hal api parameter struct define */
typedef struct tagHAL_API_FDB_MAC_LEARN
{
    UINT32 ulPortId;
    UINT32 ulLearnEnable;
}HAL_API_FDB_MAC_LEARN_S;

typedef struct tagHAL_API_FDB_MAC
{
    UINT8 ucPortId;
    UINT8 *aucMacAddress;
    UINT16 usVlanId;
}HAL_API_FDB_MAC_S;

typedef struct tagHAL_API_FDB_MAC_MULTIPORT
{
    UINT32 ulPortNum;
	UINT32 *aulPortlist;
	UINT8 *aucMacAddress;
	UINT16 usVlanId;
}HAL_API_FDB_MAC_MULTIPORT_S;

typedef struct tagHAL_API_FDB_MAC_SHOW
{
    INT32 lFd;
	UINT8 *aucMacAddress;
	UINT16 usType;
	UINT32 ulPortNum;
	UINT32 *aulPortlist;
}HAL_API_FDB_MAC_SHOW_S;

typedef struct tagHAL_API_FDB_MAC_LIMIT
{
    UINT32 ulPortId;
    UINT32 ulEnable;
	UINT32 ulMacNum;
}HAL_API_FDB_MAC_LIMIT_S;

typedef struct tagHAL_API_FDB_MAC_MOVE
{
    UINT32 ulPort;
    UINT32 ulToPort;
}HAL_API_FDB_MAC_MOVE_S;

#endif      /* _HAL_FDB_H_ */


 /*
  * Copyright (C) 2012 Realtek Semiconductor Corp. 
  * All Rights Reserved.
  *
  * This program is the proprietary software of Realtek Semiconductor
  * Corporation and/or its licensors, and only be used, duplicated, 
  * modified or distributed under the authorized license from Realtek. 
  *
  * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
  * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
  *
  * $Revision: 39101 $
  * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
  *
  * Purpose : Definition of SVLAN API
  *
  * Feature : The file includes the following modules and sub-modules
  * 		  (1) OAM (802.3ah) configuration
  *
  */

#ifndef __PON_OMCI_H__
#define __PON_OMCI_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <omci_driver.h>

#define PON_OMCI_MSG_PRI_NUM   (2)
#define PON_OMCI_MSG_PRI_LOW   (0)
#define PON_OMCI_MSG_PRI_HIGH  (1)


/* Based on the size of the message contents field,   */
/* the aggregate size of the attributes requested     */
/* by a single Get command should not exceed 25 bytes */
#define PON_OMCI_MSG_GET_LIMIT         (25)
#define PON_OMCI_MSG_GET_NEXT_LIMIT    (29)
#define PON_OMCI_MIB_UPLOAD_NEXT_LIMIT (26)



/* 60s for OMCI cmd: get larger attributes, mib upload, get all alarms */
#define PON_OMCI_CMD_MAX_INTERVAL    (60000)


typedef UINT16 PON_ME_ENTITY_ID;
typedef UINT16 PON_OMCI_ATTRS_SET;




typedef struct 
{
    UINT16 meClass;     /* type of PON_ME_CLASS_ID */
    UINT16 meInstance;
} PON_OMCI_MSG_ID_T;


typedef struct 
{
    UINT16 zero;     /* always be zero */
    UINT16 len;      /* always be 0x0028 */
    UINT32 crc;
} PON_OMCI_MSG_TRAILER_T;


/* Protocol byte order */
typedef struct 
{
    UINT16                 tcId;                                   /* Transaction Correlation Identifier */
    UINT8                  type;                                   /* Message Type */
    UINT8                  devId;                                  /* Device Identifier */
    PON_OMCI_MSG_ID_T      msgId;                                  /* Message Identifier */
    UINT8                  content[PON_OMCI_MSG_CONTENT_LEN];      /* Message Content */
    PON_OMCI_MSG_TRAILER_T trailer;                                /* Trailer */
} PON_OMCI_MSG_T;


/* Host byte order */
typedef struct 
{
    UINT16                 priority;
    UINT16                 tcId;
    UINT8                  db;
    UINT8                  ar;
    UINT8                  ak;
    UINT8                  type;
    UINT32                 devId;
    PON_OMCI_MSG_ID_T      msgId;                                  /* Message Identifier */
    UINT8                  content[PON_OMCI_MSG_CONTENT_LEN];      /* Message Content */
    PON_OMCI_MSG_TRAILER_T trailer;     
} PON_OMCI_MSG_H_T;

/*OMCI_CMD*/
typedef struct 
{
	PON_OMCI_CMD_TYPE 	   cmd;
	BOOL				   state;
	int					   tableId;
	UINT16				   entityId;
	char 				   sn[9];
	char 				   filename[64];
	
} PON_OMCI_CMD_T;


typedef struct omci_traff_rule_s
{
	int						servId;
	int						ingress;
	int						outgress;
	PON_GEMPORT_DIRECTION	dir;
	OMCI_VLAN_OPER_ts		vlanRule;
	LIST_ENTRY(omci_traff_rule_s) entries;	
} OMCI_TRAFF_RULE_ts;



#ifdef  __cplusplus
}
#endif

#endif /* __PON_OMCI_H__ */

 

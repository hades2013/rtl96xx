/*
=============================================================================
     Header Name: hal_rstp.h

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
#ifndef _HAL_RSTP_H_
#define _HAL_RSTP_H_

#include "opl_driver.h"

#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

#define MSTP_INSTANCE_0   0
 
#define PACKET_BUFFER_SIZE 1600

#ifndef MIN_PACKET_SIZE
#define MIN_PACKET_SIZE         60
#endif

#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         1514
#endif

/* marvell tag的偏移地址 */
#define E802_PACKETLEN_OFF (2 * MAC_ADDR_LEN) 

#define DRV_BPDU_HEADER_LEN (E802_PACKETLEN_OFF + E802_PACKETLEN_LEN + STP_LLC_LEN)

/* STP LLC head 的偏移地址 */
#define STP_LLC_OFF (E802_PACKETLEN_OFF + E802_PACKETLEN_LEN)

/* BPDU纯数据偏移 */
#define BPDU_DATA_OFF (STP_LLC_OFF + STP_LLC_LEN)


/* hal api parameter struct define */
typedef struct tagHAL_API_RSTP_PORT_STATE_SET
{
    UINT32 ulPortId;
    UINT8 ucPortState;
}HAL_API_RSTP_PORT_STATE_SET_S;

typedef struct tagHAL_API_RSTP_PORT_STATE_GET
{
    UINT32 ulPortId;
    UINT8 *pucPortState;
}HAL_API_RSTP_PORT_STATE_GET_S;

typedef struct tagHAL_API_RSTP_PORT_MAC_FLUSH
{
    UINT32 ulPortid;
    BOOL_T bOnlyThisPort;
}HAL_API_RSTP_PORT_MAC_FLUSH_S;


#endif      /* _HAL_RSTP_H_ */



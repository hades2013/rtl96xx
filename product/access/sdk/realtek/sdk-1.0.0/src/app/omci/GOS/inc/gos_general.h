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
 *           (1) OAM (802.3ah) configuration
 *
 */


#ifndef _GOS_GENERAL_H_
#define _GOS_GENERAL_H_


UINT16 GOS_Htons(UINT16 a);
UINT16 GOS_Ntohs(UINT16 b);
UINT32 GOS_Htonl(UINT32 a);
UINT32 GOS_Ntohl(UINT32 b);
UINT64 GOS_Ntohll(UINT64 c);
UINT64 GOS_Htonll(UINT64 c);
UINT64 GOS_BuffToUINT64(CHAR *pBuf, UINT32 bufSize);
void GOS_UINT64ToBuff(UINT64 value, CHAR* pBuf,  UINT32 bufSize);
UINT16 GOS_GetUINT16(UINT16* pVal);
void GOS_SetUINT16(UINT16* pDest, UINT16 src);
UINT32 GOS_GetUINT32(UINT32* pVal);
void GOS_SetUINT32(UINT32* pDest, UINT32 src);

#endif /*_GOS_GENERAL_H_*/


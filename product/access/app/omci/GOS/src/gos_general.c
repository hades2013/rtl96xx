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

#include "app_basic.h"


UINT16 GOS_Htons(UINT16 a)
{
#ifdef OMCI_X86
    UINT16 low, high;
    low = (a & 0xFF00) >> 8;
    high = a & 0x00FF;

    return (high << 8) | low;
#else
    return a;
#endif    
}


UINT16 GOS_Ntohs(UINT16 b)
{
    return GOS_Htons(b);
}


UINT32 GOS_Htonl(UINT32 a)
{
#ifdef OMCI_X86
    UINT32 byte1, byte2, byte3, byte4;
    byte1 = (a & 0xFF000000) >> 24;
    byte2 = (a & 0x00FF0000) >> 16;
    byte3 = (a & 0x0000FF00) >> 8;
    byte4 = (a & 0x000000FF) >> 0;

    return (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
#else
    return a;
#endif    
}


UINT32 GOS_Ntohl(UINT32 b)
{
    return GOS_Htonl(b);
}


UINT64 GOS_Ntohll(UINT64 c)
{
    c.low = GOS_Ntohl(c.low);
    c.high = GOS_Ntohl(c.high);
    return c;
}


UINT64 GOS_Htonll(UINT64 c)
{
    return GOS_Ntohll(c);
}


UINT64 GOS_BuffToUINT64(CHAR *pBuf, UINT32 bufSize)
{
    UINT64 val;
    UINT8 temp[8], i;
    UINT32* pLow = (UINT32*)&(temp[4]);
    UINT32* pHigh = (UINT32*)temp;

    GOS_ASSERT(bufSize > 0 && bufSize <= sizeof(UINT64));
    memset(temp, 0x00, 8);
    
    for (i = 1; i <= bufSize; i++)
    {
        temp[8 - i] = (UINT8)pBuf[bufSize - i];
    }

    val.low = GOS_Ntohl(*pLow);
    val.high = GOS_Ntohl(*pHigh);

    return val;
}


void GOS_UINT64ToBuff(UINT64 value, CHAR* pBuf,  UINT32 bufSize)
{
    value = GOS_Htonll(value);
    memcpy(pBuf, (CHAR *)&value + sizeof(UINT64) - bufSize, bufSize);
}


UINT16 GOS_GetUINT16(UINT16* pVal)
{
    GOS_ASSERT(pVal);
    
    if(0x1 & (UINT32)pVal)
    {
        UINT16 val;
        memcpy(&val, pVal, sizeof(UINT16));
        return val;
    }

    return *pVal;
}


void GOS_SetUINT16(UINT16* pDest, UINT16 src)
{
    GOS_ASSERT(pDest);
    
    if (0x1 & (UINT32)pDest)
    {
        memcpy(pDest, &src, sizeof(UINT16));
    }
    else
    {
        *pDest = src;
    }
}


UINT32 GOS_GetUINT32(UINT32* pVal)
{
    GOS_ASSERT(pVal);
    
    if (0x3 & (UINT32)pVal)
    {
        UINT32 val;
        memcpy(&val, pVal, sizeof(UINT32));
        return val;
    }

    return *pVal;
}

    
void GOS_SetUINT32(UINT32* pDest, UINT32 src)
{
    GOS_ASSERT(pDest);
    
    if (0x3 & (UINT32)pDest)
    {
        memcpy(pDest, &src, sizeof(UINT32));
    }
    else
    {
        *pDest = src;
    }
}


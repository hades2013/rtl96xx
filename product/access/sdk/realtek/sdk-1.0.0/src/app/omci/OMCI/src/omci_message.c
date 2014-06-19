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
 * Purpose : Definition of Message related APIs
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI
 *
 */

#include "app_basic.h"
#include "omci_defs.h"
#include <omci_driver.h>


#define OMCI_HIS_RSP_MSG_MAX_NUM (16)


UINT32 gOmciDupMsg         = 0;
UINT32 gOmciMsgNum         = 0;
UINT32 gOmciCrcErr         = 0;
BOOL   gOmciCrcCheckEnable = FALSE;
BOOL   gOmciOmitErrEnable  = FALSE;

PON_OMCI_MSG_H_T gOmciHistoryRspMsg[PON_OMCI_MSG_PRI_NUM][OMCI_HIS_RSP_MSG_MAX_NUM];
UINT32           gOmciLastRspMsgIndex[PON_OMCI_MSG_PRI_NUM];




/*
*  Define local function for message handler
*/

static UINT32* omci_CoeffCRC32(void)
{
    static BOOL   inited = FALSE;
    static UINT32 crctable[256];

    if (inited)
    {
        return crctable;
    }

    inited = TRUE;
         
    {
        register UINT32 i;
        register UINT32 j;
        UINT32          accum  = 0;
        UINT32          poly   = 0;
        INT32           pn32[] =
                        {
                        //  x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1       
                            0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26 
                        };
        

        for (i = 0; i < (sizeof(pn32)/sizeof(pn32[0])); i++)
        {
           poly = poly | (1UL<<pn32[i]);
        }

        for (i = 0; i < 256; i++) 
        {
           accum = i << 24;
           for (j = 0; j < 8; j++) 
           {
               if (accum & (1L << 31))
               {
                   accum = (accum << 1) ^ poly;
               }
               else
               {
                   accum = accum << 1;
               }
           }
           crctable[i] = accum;
        }
    }

    return crctable;
}


static UINT32 omci_CalcCRC32(const UINT8* pBuf, UINT32 size)
{
    UINT32  i;
    UINT32  accum = 0;
    UINT32* table = omci_CoeffCRC32();

    GOS_ASSERT(pBuf);
    
    accum = ~accum;
    
    for (i = 0; i < size; i++)
    {
        accum =  table[((accum >> 24) ^ pBuf[i]) & 0xff] ^ (accum << 8);
    }
    
    return ~accum;
}


static GOS_ERROR_CODE omci_CheckCRC(PON_OMCI_MSG_T* pOmciMsg)
{
    UINT32 crc = 0;

    crc = omci_CalcCRC32((const UINT8*)pOmciMsg, sizeof(PON_OMCI_MSG_T) - sizeof(pOmciMsg->trailer.crc));

    if (crc != GOS_Ntohl(pOmciMsg->trailer.crc))
    {
        gOmciCrcErr++;
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"CRC is incorrect (expected: 0x%x, received: 0x%x), total %d CRC incorrect", 
                  crc, pOmciMsg->trailer.crc, gOmciCrcErr);
        return GOS_ERR_CRC;
    }

    return GOS_OK;
}



static GOS_ERROR_CODE omci_CheckMsgHdr(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    if (PON_OMCI_MSG_DEVICE_ID != pNorOmciMsg->devId)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Device ID is incorrect (expected: 0x%x, received: 0x%x)",
                  PON_OMCI_MSG_DEVICE_ID, pNorOmciMsg->devId);
        return GOS_FAIL;    
    }


    // all OMCI messages should set AR bit
    if (!pNorOmciMsg->ar)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"AR bit incorrect");
        return GOS_FAIL;    
    }

    if (pNorOmciMsg->ak)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"AK bit incorrect");
        return GOS_FAIL;    
    }

    return GOS_OK;
}


static void omci_LogMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    struct timeval tv;

    
    gettimeofday(&tv, NULL);
    
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"tick: 0x%x PRI: %d, TI: 0x%x, DID: %d, DB: %d, AR: %d, AK: %d, MT: %d, Class: %d, inst: 0x%x", 
                (INT32)(tv.tv_sec * 1000000 + tv.tv_usec), pNorOmciMsg->priority, pNorOmciMsg->tcId, pNorOmciMsg->devId, pNorOmciMsg->db, 
                pNorOmciMsg->ar, pNorOmciMsg->ak, pNorOmciMsg->type, pNorOmciMsg->msgId.meClass,
                pNorOmciMsg->msgId.meInstance);
                                            
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", 
                pNorOmciMsg->content[0], pNorOmciMsg->content[1], pNorOmciMsg->content[2], pNorOmciMsg->content[3], 
                pNorOmciMsg->content[4], pNorOmciMsg->content[5], pNorOmciMsg->content[6], pNorOmciMsg->content[7], 
                pNorOmciMsg->content[8], pNorOmciMsg->content[9], pNorOmciMsg->content[10], pNorOmciMsg->content[11], 
                pNorOmciMsg->content[12], pNorOmciMsg->content[13], pNorOmciMsg->content[14], pNorOmciMsg->content[15]); 
                                            
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", 
                pNorOmciMsg->content[16], pNorOmciMsg->content[17], pNorOmciMsg->content[18], pNorOmciMsg->content[19], 
                pNorOmciMsg->content[20], pNorOmciMsg->content[21], pNorOmciMsg->content[22], pNorOmciMsg->content[23], 
                pNorOmciMsg->content[24], pNorOmciMsg->content[25], pNorOmciMsg->content[26], pNorOmciMsg->content[27], 
                pNorOmciMsg->content[28], pNorOmciMsg->content[29], pNorOmciMsg->content[30], pNorOmciMsg->content[31]); 
                                           
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"zero: 0x%x, len: 0x%x crc: 0x%x", 
                pNorOmciMsg->trailer.zero, pNorOmciMsg->trailer.len, pNorOmciMsg->trailer.crc);
}



static GOS_ERROR_CODE omci_SendOmciMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    PON_OMCI_MSG_T rawOmciMsg;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Send out OMCI msg");

    gOmciMsgNum++;

    pNorOmciMsg->trailer.len  = 0x0028;
    pNorOmciMsg->trailer.zero = 0;

    rawOmciMsg.tcId = GOS_Htons(pNorOmciMsg->tcId | (pNorOmciMsg->priority << PON_OMCI_MSG_PRI_BIT));
    rawOmciMsg.type = (pNorOmciMsg->db << PON_OMCI_MSG_TYPE_DB_BIT) | 
                      (pNorOmciMsg->ar << PON_OMCI_MSG_TYPE_AR_BIT) |
                      (pNorOmciMsg->ak << PON_OMCI_MSG_TYPE_AK_BIT) |
                      (pNorOmciMsg->type & PON_OMCI_MSG_TYPE_MT_MASK);


    rawOmciMsg.devId = (UINT8)pNorOmciMsg->devId;
    
    rawOmciMsg.msgId.meClass    = GOS_Htons(pNorOmciMsg->msgId.meClass);
    rawOmciMsg.msgId.meInstance = GOS_Htons(pNorOmciMsg->msgId.meInstance);

    memcpy(rawOmciMsg.content, pNorOmciMsg->content, PON_OMCI_MSG_CONTENT_LEN);

    rawOmciMsg.trailer.len  = GOS_Htons(pNorOmciMsg->trailer.len);
    rawOmciMsg.trailer.zero = GOS_Htons(pNorOmciMsg->trailer.zero);
    rawOmciMsg.trailer.crc  = GOS_Htonl(omci_CalcCRC32((const UINT8*)&rawOmciMsg, sizeof(PON_OMCI_MSG_T) - sizeof(rawOmciMsg.trailer.crc)));

    pNorOmciMsg->trailer.crc = GOS_Ntohl(rawOmciMsg.trailer.crc);
    
    omci_LogMsg(pNorOmciMsg);

    return OMCI_wrapper_SendOmciMsg(&rawOmciMsg);
}




static PON_OMCI_MSG_H_T* omci_GetRspMsgBuff(UINT16 pri)
{
    GOS_ASSERT(pri < PON_OMCI_MSG_PRI_NUM);
    GOS_ASSERT(gOmciLastRspMsgIndex[pri] < OMCI_HIS_RSP_MSG_MAX_NUM);
    
    if (gOmciLastRspMsgIndex[pri] == OMCI_HIS_RSP_MSG_MAX_NUM - 1)
    {
        gOmciLastRspMsgIndex[pri] = 0;
    }
    else
    {
        gOmciLastRspMsgIndex[pri]++;
    }

    memset(&gOmciHistoryRspMsg[pri][gOmciLastRspMsgIndex[pri]], 0x00, sizeof(PON_OMCI_MSG_H_T));
    
    return &gOmciHistoryRspMsg[pri][gOmciLastRspMsgIndex[pri]];
}



static GOS_ERROR_CODE omci_CheckDuplicateMsgRsp(PON_OMCI_MSG_H_T* pMsg)
{
    UINT32 i;
    GOS_ERROR_CODE ret = GOS_OK;

    GOS_ASSERT(pMsg->priority < PON_OMCI_MSG_PRI_NUM);

    for (i = 0; i < OMCI_HIS_RSP_MSG_MAX_NUM; i++)
    {
        if (gOmciHistoryRspMsg[pMsg->priority][i].tcId == pMsg->tcId)
        {
            omci_SendOmciMsg(&(gOmciHistoryRspMsg[pMsg->priority][i]));
            ret = GOS_FAIL;
        }
    }

    return ret;
}

static GOS_ERROR_CODE omci_DispatchMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    switch(pNorOmciMsg->type)
    {
        case PON_OMCI_MSG_CREATE:
        {
            return OMCI_OnCreateMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_DELETE:
        {
            return OMCI_OnDeleteMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_SET:
        {
            return OMCI_OnSetMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_GET:
        {
            return OMCI_OnGetMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_MIB_UPLOAD:
        {
            return OMCI_OnMibUploadMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_MIB_UPLOAD_NEXT:
        {
            return OMCI_OnMibUploadNextMsg(pNorOmciMsg);
        }
        case PON_OMCI_MSG_MIB_RESET:
        {
            return OMCI_OnMibReset(pNorOmciMsg);
        }
        case PON_OMCI_MSG_ACTIVATE_SW:
        {
            return OMCI_OnActivateSw(pNorOmciMsg);
        }
        case PON_OMCI_MSG_COMMIT_SW:
        {
            return OMCI_OnCommitSw(pNorOmciMsg);
        }
        case PON_OMCI_MSG_REBOOT:
        {
            return OMCI_OnReboot(pNorOmciMsg);
        }
        case PON_OMCI_MSG_GET_NEXT:
        {
            return OMCI_OnGetNextMsg(pNorOmciMsg);
        }
		case PON_OMCI_MSG_GET_ALL_ALARM:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Get all alarm\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_GET_ALL_ALARM_NEXT:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Get all alarm next\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_MIB_ALARM:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Alarm\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_TEST:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Test\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_START_SW_DOWNLOAD:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Start Software Download\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_DOWNLOAD_SECTION:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Download section\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_END_SW_DOWNLOAD:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: END Software Download\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_SYNC_TIME:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Synchronize Time\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_TEST_RESULT:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Test Result\n");
			return GOS_OK;
		}
		case PON_OMCI_MSG_GET_CURRENT_DATA:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling Action: Get current data\n");
			return GOS_OK;
		}
        default:
         break;
    }

    return GOS_FAIL;
}

static GOS_ERROR_CODE omci_HandleRxOmciMsg(PON_OMCI_MSG_T* pOmciMsg)
{
    PON_OMCI_MSG_H_T  norOmciMsg;
    BOOL meValid;
    BOOL meExist;


    GOS_ASSERT(pOmciMsg);

    gOmciMsgNum++;

    if (gOmciCrcCheckEnable)
    {
        if (GOS_OK != omci_CheckCRC(pOmciMsg))
        {
            return GOS_ERR_CRC;
        }
    }

    OMCI_NormalizeMsg(pOmciMsg, &norOmciMsg);
    omci_LogMsg(&norOmciMsg);

    if (GOS_OK != omci_CheckMsgHdr(&norOmciMsg))
    {
        return GOS_FAIL;
    }
    
    // It's a duplicate OMCI message, retransmit its response message
    if (GOS_OK != omci_CheckDuplicateMsgRsp(&norOmciMsg))
    {
        gOmciDupMsg++;
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive duplicated cmd, tcId = %d", norOmciMsg.tcId);
        
        return GOS_FAIL;
    }

    // Check if the ME is valid and exist or not
    if (GOS_OK != OMCI_CheckIsMeValid(norOmciMsg.msgId.meClass, &norOmciMsg.msgId.meInstance, &meValid, &meExist))
    {
        return GOS_FAIL;
    }

    if (meValid == FALSE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive operation on unknown me, cmd = %d, me = %d, inst = %d",
              norOmciMsg.type, norOmciMsg.msgId.meClass, norOmciMsg.msgId.meInstance);     
        //OMCI_ResponseMsg(&norOmciMsg, PON_OMCI_RSP_UNKNOWN_ME, TRUE);
        OMCI_ResponseMsg(&norOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);

        return GOS_FAIL;
    }

    if (norOmciMsg.type != PON_OMCI_MSG_CREATE && meExist == FALSE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive operation on unknown inst, cmd = %d, me = %d, inst = %d",
              norOmciMsg.type, norOmciMsg.msgId.meClass, norOmciMsg.msgId.meInstance);     
        OMCI_ResponseMsg(&norOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        
        return GOS_FAIL;
    }

    // Check if the ME action is supported or not
    if (OMCI_CheckIsActionSupported(norOmciMsg.msgId.meClass, norOmciMsg.type) == FALSE)
    {
        //OMCI_ResponseMsg(&norOmciMsg, PON_OMCI_RSP_NOT_SUPPORT, TRUE);
        OMCI_ResponseMsg(&norOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);
        
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive unsupported cmd, cmd = %d, me = %d",
                  norOmciMsg.type, norOmciMsg.msgId.meClass);        
        
        return GOS_ERR_NOTSUPPORT;
    }

    return omci_DispatchMsg(&norOmciMsg);
}


static GOS_ERROR_CODE omci_HandleAvcMsg(PON_OMCI_MSG_H_T* pOmciMsg)
{
    pOmciMsg->type = PON_OMCI_MSG_AVC;
    return OMCI_SendIndicationToOlt(pOmciMsg, 0);
}


static GOS_ERROR_CODE omci_HandleCmdMsg(PON_OMCI_CMD_T* pCmd)
{

	switch(pCmd->cmd)
	{
	case PON_OMCI_CMD_DUMPAVL:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Dump AVL Tree");
		omci_tree_dump();
	break;
	case PON_OMCI_CMD_LOG:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Set Log State to %d",pCmd->state);
		omci_log_set();
	break;
	case PON_OMCI_CMD_DUMPQUEUE:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Dump Queue TCONT map");
		omci_tqTable_dump();
	break;
	case PON_OMCI_CMD_DUMPTREECONN:		
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Dump Tree connections");
		omci_conn_dump();
	break;
	case PON_OMCI_CMD_DUMPMIB:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Dump OMCI MIB");
		omci_mib_dump(pCmd->tableId,pCmd->filename);
	break;
	case PON_OMCI_CMD_SN_SET:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Set Serial Number");
		omci_sn_set(pCmd->sn);
	break;
	case PON_OMCI_CMD_SN_GET:
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Get Serial Number");
		omci_sn_get();	
	break;
	default:
	break;
	}
	return GOS_OK;
}




/*
*  Define Global APIs for Message handler
*/


GOS_ERROR_CODE OMCI_ResetHistoryRspMsg(void)
{
    memset(gOmciHistoryRspMsg, 0x00, PON_OMCI_MSG_PRI_NUM * OMCI_HIS_RSP_MSG_MAX_NUM * sizeof(PON_OMCI_MSG_H_T));

    return GOS_OK;
}


GOS_ERROR_CODE OMCI_ResetLastRspMsgIndex(void)
{
    memset(gOmciLastRspMsgIndex, 0x00, PON_OMCI_MSG_PRI_NUM * sizeof(UINT32));

    return GOS_OK;
}



GOS_ERROR_CODE OMCI_HandleMsg(void* pData, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, OMCI_APPL_ID srcAppl)
{
    GOS_ERROR_CODE ret = GOS_FAIL;
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Rcv OMCI from MSG Queue!,type=%d\n",type);

    switch (type)
    {
        case OMCI_RX_OMCI_MSG:
        {
            ret = omci_HandleRxOmciMsg((PON_OMCI_MSG_T*)pData);    
            break;
        }
        
        case OMCI_AVC_MSG:
        {
            ret = omci_HandleAvcMsg((PON_OMCI_MSG_H_T*)pData);
            break;
        }
		case OMCI_CMD_MSG:
		{
		   ret = omci_HandleCmdMsg((PON_OMCI_CMD_T*)pData);
		   break;
		}

        default:
        {
            break;
        }
    }

    return ret;
}



GOS_ERROR_CODE OMCI_SendIndicationToOlt(PON_OMCI_MSG_H_T* pNorOmciMsg, UINT16 tr)
{
    pNorOmciMsg->priority = PON_OMCI_MSG_PRI_LOW;
    pNorOmciMsg->tcId     = tr;
    pNorOmciMsg->db       = 0;
    pNorOmciMsg->ar       = 0;
    pNorOmciMsg->ak       = 0;
    pNorOmciMsg->devId    = PON_OMCI_MSG_DEVICE_ID;

    return omci_SendOmciMsg(pNorOmciMsg);
}


GOS_ERROR_CODE OMCI_NormalizeMsg(PON_OMCI_MSG_T* pOmciMsg, PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    pNorOmciMsg->tcId     = GOS_Ntohs(pOmciMsg->tcId);
    pNorOmciMsg->priority = ((PON_OMCI_MSG_PRI_MASK & pNorOmciMsg->tcId) ? PON_OMCI_MSG_PRI_HIGH : PON_OMCI_MSG_PRI_LOW);
    pNorOmciMsg->tcId     = PON_OMCI_MSG_TID_MASK & pNorOmciMsg->tcId;

    pNorOmciMsg->db    = (pOmciMsg->type & PON_OMCI_MSG_TYPE_DB_MASK) ? 1 : 0;
    pNorOmciMsg->ar    = (pOmciMsg->type & PON_OMCI_MSG_TYPE_AR_MASK) ? 1 : 0;
    pNorOmciMsg->ak    = (pOmciMsg->type & PON_OMCI_MSG_TYPE_AK_MASK) ? 1 : 0;
    pNorOmciMsg->type  = (pOmciMsg->type & PON_OMCI_MSG_TYPE_MT_MASK);
    pNorOmciMsg->devId = (UINT32)pOmciMsg->devId;
    
    pNorOmciMsg->msgId.meClass    = GOS_Ntohs(pOmciMsg->msgId.meClass);
    pNorOmciMsg->msgId.meInstance = GOS_Ntohs(pOmciMsg->msgId.meInstance);
    
    memcpy(&(pNorOmciMsg->content[0]), &(pOmciMsg->content[0]), PON_OMCI_MSG_CONTENT_LEN);

    pNorOmciMsg->trailer.crc  = GOS_Ntohl(pOmciMsg->trailer.crc);
    pNorOmciMsg->trailer.len  = GOS_Ntohs(pOmciMsg->trailer.len);
    pNorOmciMsg->trailer.zero = GOS_Ntohs(pOmciMsg->trailer.zero);

    return GOS_OK;
}




GOS_ERROR_CODE OMCI_ResponseMsg(PON_OMCI_MSG_H_T* pNorOmciMsg, PON_OMCI_MSG_RESULT result, BOOL clear)
{
    PON_OMCI_MSG_H_T* pRspMsg = omci_GetRspMsgBuff(pNorOmciMsg->priority);

    GOS_ASSERT(pRspMsg != NULL);

    pRspMsg->priority         = pNorOmciMsg->priority;
    pRspMsg->tcId             = pNorOmciMsg->tcId;
    pRspMsg->db               = pNorOmciMsg->db;
    pRspMsg->ar               = 0;
    pRspMsg->ak               = 1;
    pRspMsg->type             = pNorOmciMsg->type;
    pRspMsg->devId            = pNorOmciMsg->devId;
    pRspMsg->msgId.meClass    = pNorOmciMsg->msgId.meClass;
    pRspMsg->msgId.meInstance = pNorOmciMsg->msgId.meInstance;

    if (clear)
    {
        memset(&(pNorOmciMsg->content[0]), 0x00, PON_OMCI_MSG_CONTENT_LEN);
    }

    if (gOmciOmitErrEnable)
    {
        pNorOmciMsg->content[0] = (UINT8)0;
    }
    else
    {
        pNorOmciMsg->content[0] = (UINT8)result;
    }
    
    memcpy(&(pRspMsg->content[0]), &(pNorOmciMsg->content[0]), PON_OMCI_MSG_CONTENT_LEN);

    return omci_SendOmciMsg(pRspMsg);
}




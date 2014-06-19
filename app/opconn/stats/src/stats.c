/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	stats.c
*
* DESCRIPTION:
*	This file implements the stats show and poll on ONU system.
*
* Date Created:
*	2008-08-27
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/

#include "stats.h"
//wfxu #include "hw_interface.h"
#include "oam.h"
#include "opconn_usr_ioctrl.h"

#if defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6097)
#include "msApi.h"
#endif

#if defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) 
OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SWH_PORT_NUM]={
{
{0, "RxGoodBytes", 0, 0, {0, 0}, "Good packets received."},
{0, "RxBadBytes", 0, 0, {0, 0}, "Packets with crc or alignment error received."},
{0, "TxFCSErr", 0, 0, {0, 0}, "Packets with crc or alignment error transmitted."},
{0, "RxUnicasts", 0, 0, {0, 0}, "Good unicast packets received."},
{0, "TxDeferred", 0, 0, {0, 0}, "Packets transmitted defered."},
{0, "RxBroadcasts", 0, 0, {0, 0}, "Good broadcast packets received."},
{0, "RxMulticasts", 0, 0, {0, 0}, "Good multicast packets received."},
{0, "64Octets", 0, 0, {0, 0}, "64-byte packets (received and transmitted)."},
{0, "127Octets", 0, 0, {0, 0}, "65-127 bytes packets (received and transmitted)."},
{0, "255Octets", 0, 0, {0, 0}, "128-255 bytes packets (received and transmitted)."},
{0, "511Octets", 0, 0, {0, 0}, "256-511 bytes packets (received and transmitted)."},
{0, "1023Octets", 0, 0, {0, 0}, "512-1023 bytes packets (received and transmitted)."},
{0, "maxOctets", 0, 0, {0, 0}, "1024-max bytes packets (received and transmitted)."},
{0, "TxGoodBytes", 0, 0, {0, 0}, "Good packets transmitted."},
{0, "TxUnicasts", 0, 0, {0, 0}, "Good unicast packets transmitted."},
{0, "TxExcessive", 0, 0, {0, 0}, "Excessive packets transmitted."},
{0, "TxMulticasts", 0, 0, {0, 0}, "Good multicast packets transmitted."},
{0, "TxBroadcasts", 0, 0, {0, 0}, "Good broadcast packets transmitted."},
{0, "TxSingle", 0, 0, {0, 0}, "Single packets transmitted."},
{0, "TxPause", 0, 0, {0, 0}, "PAUSE frames transmitted."},
{0, "RxPause", 0, 0, {0, 0}, "PAUSE frames received."},
{0, "TxMultiple", 0, 0, {0, 0}, "Multiple packets transmitted."},
{0, "RxUndersize", 0, 0, {0, 0}, "Packets less than 64 bytes but crc ok received."},
{0, "RxFragments", 0, 0, {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0, "RxOversize", 0, 0, {0, 0}, "Packets longer than MTU bytes but crc ok received."},
{0, "RxJabber", 0, 0, {0, 0}, "Packets longer than MTU and have crc error received."},
{0, "RxMACRcvErr", 0, 0, {0, 0}, "Packets have mac crc error received."},
{0, "RxFCSErr", 0, 0, {0, 0}, "Packets have fcs error received."},
{0, "TxCollisions", 0, 0, {0, 0}, "Collision Packets have transmitted."},
{0, "TxLate", 0, 0, {0, 0}, "Late Packets have transmitted."},
},
};
#else
extern OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SWH_PORT_NUM];
#endif

/*TIMER_UNIT_t g_stStatsTimer ={0,0,1};*/

#if defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) 
extern GT_QD_DEV *dev;
#endif


#define  ShowCntTitle( iWrFd,  uiMode)\
	if(CLI_MODE_SUPER == uiMode) \
	{ \
		vosPrintf(iWrFd, "\r\n%04s %-15s %-20s %s\r\n","ID","Name","Value","Help");\
	} \
	else \
	{ \
		vosPrintf(iWrFd, "\r\n%-15s %-20s %s\r\n","Name","Value","Help");\
	}


#define ShowCntValues( l_NameA, l_Cnt ) \
	for (ucCnt=0; ucCnt<l_Cnt; ucCnt++)	\
	{ \
		OPL_MEMSET(buffer, 0, 100); \
		sprintf(buffer,"%llu", l_NameA[ucCnt].stAdded); \
		if(CLI_MODE_SUPER == uiMode) \
		{	\
			vosPrintf(iWrFd, "0x%04x %-15s %-20s %s\r\n", \
							l_NameA[ucCnt].usId, l_NameA[ucCnt].pucName, \
							buffer, \
							l_NameA[ucCnt].pucHelp);\
		}\
		else\
		{\
			vosPrintf(iWrFd, "%-15s %-20s %s\r\n", \
							l_NameA[ucCnt].pucName, \
							buffer, \
							l_NameA[ucCnt].pucHelp); \
		}  \
	}


OPL_STATUS ShowCntGe(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_GE_NUM];

	dalCounterGet(DAL_CNT_POLL_GE_MASK, oplCounter, 0, DAL_CNT_GE_NUM);

	vosPrintf(iWrFd, "\r\nGe Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_GE_NUM )

	return iStatus;
}

OPL_STATUS ShowCntPon(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_PON_NUM];

	dalCounterGet(DAL_CNT_POLL_PON_MASK, oplCounter, 0, DAL_CNT_PON_NUM);

	vosPrintf(iWrFd, "\r\nPon Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_PON_NUM )

	return iStatus;
}
OPL_STATUS ShowCntFe(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_FE_NUM];

	dalCounterGet(DAL_CNT_POLL_FE_MASK, oplCounter, 0, DAL_CNT_FE_NUM);

	vosPrintf(iWrFd, "\r\nFe Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_FE_NUM )

	return iStatus;
}

OPL_STATUS ShowCntTm(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_TM_NUM];

	dalCounterGet(DAL_CNT_POLL_TM_MASK, oplCounter, 0, DAL_CNT_TM_NUM);

	vosPrintf(iWrFd, "\r\nTm Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_TM_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCle(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_CLE_NUM];

	dalCounterGet(DAL_CNT_POLL_CLE_MASK, oplCounter, 0, DAL_CNT_CLE_NUM);

	vosPrintf(iWrFd, "\r\nCle Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_CLE_NUM )

	return iStatus;
}

OPL_STATUS ShowCntBrg(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_BRG_NUM];

	dalCounterGet(DAL_CNT_POLL_BRG_MASK, oplCounter, 0, DAL_CNT_BRG_NUM);

	vosPrintf(iWrFd, "\r\nBrg Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_BRG_NUM )

	return iStatus;
}

OPL_STATUS ShowCntMpcp(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_MPCP_NUM];

	dalCounterGet(DAL_CNT_POLL_MPCP_MASK, oplCounter, 0, DAL_CNT_MPCP_NUM);

	vosPrintf(iWrFd, "\r\nMPCP Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_MPCP_NUM )

	return iStatus;
}

OPL_STATUS ShowCntMarb(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_MARB_NUM];

	dalCounterGet(DAL_CNT_POLL_MARB_MASK, oplCounter, 0, DAL_CNT_MARB_NUM);

	vosPrintf(iWrFd, "\r\nMarb Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_MARB_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCpdma(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_CPDMA_NUM];

	dalCounterGet(DAL_CNT_POLL_CPDMA_MASK, oplCounter, 0, DAL_CNT_CPDMA_NUM);

	vosPrintf(iWrFd, "\r\nCpdma Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_CPDMA_NUM )

	return iStatus;
}

OPL_STATUS ShowCntFedma(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_FEDMA_NUM];

	dalCounterGet(DAL_CNT_POLL_FEDMA_MASK, oplCounter, 0, DAL_CNT_FEDMA_NUM);

	vosPrintf(iWrFd, "\r\nFedma Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_FEDMA_NUM )

	return iStatus;
}

OPL_STATUS ShowCntGeParser(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_GEPARSER_NUM];

	dalCounterGet(DAL_CNT_POLL_GEPARSER_MASK, oplCounter, 0, DAL_CNT_GEPARSER_NUM);

	vosPrintf(iWrFd, "\r\nGeParser Counter");

	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( oplCounter, DAL_CNT_GEPARSER_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCleHit(int iWrFd, UINT8 ucStart, UINT8 ucNum)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
	UINT32 uiRegVal;
 	unsigned char buffer[100];	
	OPL_CNT_t oplCounter[DAL_CNT_CLEHIT_NUM];

	if (ucNum > DAL_CNT_CLEHIT_NUM)
		return OPL_ERROR;

	dalCounterGet(DAL_CNT_POLL_CLEHIT_MASK, oplCounter, 0, DAL_CNT_CLEHIT_NUM);

	vosPrintf(iWrFd, "\r\nCle hit Counter");
	vosPrintf(iWrFd, "\r\n%-6s %-20s\r\n","RuleID","Value");

	for (ucCnt=0; ucCnt<ucNum; ucCnt++)
	{
		OPL_MEMSET(buffer, 0, 100); 
		sprintf(buffer,"%llu", oplCounter[ucCnt+ucStart].stAdded);
		vosPrintf(iWrFd, "%-6d %-20s \r\n",
				ucCnt+ucStart, buffer); 

	}

	return iStatus;
}

OPL_STATUS ShowCntSwhPort(int iWrFd, UINT8 ucStart, UINT8 ucNum, int iType)
{
	OPL_STATUS iStatus=OPL_OK;
 	unsigned char buffer[100];	

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
	UINT8	ucCnt, ucPortId, ucCntShow;
	GT_STATS_COUNTER_SET3	stPortCounter;
	UINT32	*puiCounter=(UINT32 *)&stPortCounter;

	vosPrintf(iWrFd, "\r\nSwitch Port Counter");
	vosPrintf(iWrFd, "\r\n%-15s","Name");
	for(ucPortId=ucStart; ucPortId<ucStart+ucNum; ucPortId++)
	{
			OPL_MEMSET(buffer, 0, 100);
			sprintf(buffer,"(%1d)Value", ucPortId );
			vosPrintf(iWrFd, "%-20s", buffer);
	}
	for(ucCnt=0; ucCnt<CNT_SWH_PORT_NUM; ucCnt++)
	{
			vosPrintf(iWrFd, "\r\n%-15s ", g_astCntSwhPort[0][ucCnt].pucName);
			for(ucPortId=ucStart-1; ucPortId<ucStart-1+ucNum; ucPortId++)
			{
				OPL_MEMSET(buffer, 0, 100);
				sprintf(buffer,"%llu", g_astCntSwhPort[ucPortId][ucCnt].stAdded);
				vosPrintf(iWrFd, "%-20s", buffer);
			}
	}
	vosPrintf(iWrFd, "\r\n");
#elif defined(ONU_4PORT_AR8306) \
	|| defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
   stats_showArx8306( iWrFd, ucStart, ucNum );	
#else
	vosPrintf(iWrFd, "\r\nTHIS COMMAND SUPPORT ONLY MULTIPORT PRODUCT!\r\n");
#endif

	return iStatus;
}

OPL_STATUS CntClr(int iModule, UINT8 ucStart, UINT8 ucNum )
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt, ucPortId;

	if (iModule == DAL_CNT_POLL_SWITCH_MASK)
	{
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
		for(ucPortId= ucStart; ucPortId < ucStart + ucNum; ucPortId++)
		{
			if (ucPortId > SWITCH_PORT_NUM)
			{
				return iStatus;
			}
			
	#if defined(ONU_4PORT_88E6097)		
			gstatsFlushPort(dev,ucPortId - 1);
	#endif
			for (ucCnt=0; ucCnt<CNT_SWH_PORT_NUM; ucCnt++)
			{
				g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiLow = 0;
				g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiHigh = 0;
			}
		}
#else
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		for(ucPortId=ucStart; ucPortId<ucStart+ucNum; ucPortId++)
        {         
            if(ucPortId>SWITCH_PORT_NUM)
                return iStatus;
			stats_clearArx8306( ucPortId );
        }

#endif
#endif
	}
	else
	{
		dalCounterClear(iModule);
	}

	return iStatus;
}

UINT32 odmPONPortStatisticsGet1(ONU_PON_Port_Statistics_Get_1_t * ponStats)
{
	OPL_CNT_t oplCounter[DAL_CNT_PON_NUM];

	dalCounterGet(DAL_CNT_POLL_PON_MASK, oplCounter, 0, DAL_CNT_PON_NUM);

	ponStats->FramesTransmittedOK = (UINT64)oplCounter[2].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32);
	ponStats->OctetsTransmittedOK = (UINT64)oplCounter[13].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32);
	ponStats->MulticastFramesXmittedOK = (UINT64)oplCounter[4].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[4].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesXmittedOK = (UINT64)oplCounter[3].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[3].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesTransmittedOk = (UINT64)oplCounter[5].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[5].stAdded.uiHigh << 32);

	ponStats->FramesReceivedOK = (UINT64)oplCounter[16].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[16].stAdded.uiHigh << 32);
	ponStats->OctetsReceivedOK = (UINT64)oplCounter[37].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[37].stAdded.uiHigh << 32);
	ponStats->MulticastFramesReceivedOK = (UINT64)oplCounter[24].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[24].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesReceivedOK = (UINT64)oplCounter[23].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[23].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesReceivedOk = (UINT64)oplCounter[25].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[25].stAdded.uiHigh << 32);
	ponStats->SingleCollisionFrames = 0;
	ponStats->MultipleCollisionFrames = 0;
	ponStats->AlignmentErrors = (UINT64)oplCounter[18].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[18].stAdded.uiHigh << 32);
	ponStats->FrameTooLongErrors = (UINT64)oplCounter[21].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32);

	return OK;
}

UINT32 odmPONPortStatisticsGet2(ONU_PON_Port_Statistics_Get_2_t * ponStats)
{
	OPL_CNT_t oplCounter[DAL_CNT_MAX_NUM];

	dalCounterGet(DAL_CNT_POLL_PON_MASK, oplCounter, 0, DAL_CNT_PON_NUM);

	ponStats->CRC8Errors = (UINT64)oplCounter[18].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[18].stAdded.uiHigh << 32);
	ponStats->FECCorrectedBlocks = (UINT64)oplCounter[38].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[38].stAdded.uiHigh << 32);
	ponStats->FECUncorrectableBlocks = (UINT64)oplCounter[39].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[39].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesTransmitted = (UINT64)oplCounter[6].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[6].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesReceived = (UINT64)oplCounter[27].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[27].stAdded.uiHigh << 32);

	memset(oplCounter, 0, sizeof(oplCounter));
	dalCounterGet(DAL_CNT_POLL_MPCP_MASK, oplCounter, 0, DAL_CNT_MPCP_NUM);

	ponStats->MPCPTxGate = 0;
	ponStats->MPCPTxRegAck = (UINT64)oplCounter[13].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32);
	ponStats->MPCPTxRegister = 0; /* OLT·¢µÄ */
	ponStats->MPCPTxRegRequest = (UINT64)oplCounter[12].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[12].stAdded.uiHigh << 32);
	ponStats->MPCPTxReport = (UINT64)oplCounter[14].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[14].stAdded.uiHigh << 32);
	ponStats->MPCPRxGate = (UINT64)oplCounter[0].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[0].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegAck = 0;
	ponStats->MPCPRxRegister = (UINT64)oplCounter[1].stAdded.uiLow + 
		(UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegRequest = 0;
	ponStats->MPCPRxReport = 0;

	return OK;
}

UINT32 odmPortFluxStatisticsCounterGet(ONU_Port_Flux_Statistics_Counter_t *portStats)
{
	OPL_CNT_t oplCounter[DAL_CNT_GE_NUM];

	dalCounterGet(DAL_CNT_POLL_GE_MASK, oplCounter, 0, DAL_CNT_GE_NUM);

	portStats->ifInOctets =
	    (UINT64)oplCounter[34].stAdded.uiLow +
	    (UINT64)((UINT64)oplCounter[34].stAdded.uiHigh << 32);
	portStats->ifInUcastPkts =
        (UINT64)oplCounter[13].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[20].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[21].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32);
	portStats->ifInNUcastPkts =
        (UINT64)oplCounter[20].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) +
        (UINT64)oplCounter[21].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32);
	portStats->ifInDiscards =
        (UINT64)oplCounter[14].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[14].stAdded.uiHigh << 32);
	portStats->ifInErrors =
        (UINT64)oplCounter[19].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[19].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[13].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32);

    portStats->ifOutOctets =
        (UINT64)oplCounter[11].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[11].stAdded.uiHigh << 32);
	portStats->ifOutUcastPkts =
        (UINT64)oplCounter[0].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[0].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[1].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[2].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32);
	portStats->ifOutNUcastPkts =
        (UINT64)oplCounter[1].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) +
        (UINT64)oplCounter[2].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32);
	portStats->ifOutDiscards = 0;
	portStats->ifOutErrors = 0;

	return OK;
}



OPL_STATUS odmPonSinglePortStatsGet(UINT8 port_id, port_statistics_t *portStats)
{

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
	portStats->OctetsTx = 105;
	portStats->OctetsTxRate = 0;
	portStats->OctetsTxTopRate = 0;
	portStats->UnicastFramesTx = 106;
	portStats->NonUnicastFramesTx = 107;
	portStats->FrameTxRate = 0;
	portStats->TxPacketsError = 109;

	portStats->OctetsRx = 100;
	portStats->OctetsRxRate = 0;
	portStats->OctetsRxTopRate = 0;
	portStats->UnicastFramesRx = 101;
	portStats->NonUnicastFramesRx = 102;
	portStats->FrameRxRate = 0;
	portStats->RxPacketsError = 104;
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portStats->OctetsTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][31].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][31].stAdded.uiLow;

	/* Total good frames tranmitted with a broadcast Destination address  */
	portStats->OctetsTxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow;

	/* Total good frames transmitted with a multicast Destination address */
	portStats->OctetsTxTopRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow;

	portStats->UnicastFramesTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][23].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][23].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][24].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][24].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][25].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][25].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][26].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][26].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][27].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][27].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][28].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][28].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][29].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][29].stAdded.uiLow -
					(
						(UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow
					);

	portStats->NonUnicastFramesTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
					(UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow;

	/* Total good PAUSE frames transmitted */
	portStats->FrameTxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][20].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][20].stAdded.uiLow;

	portStats->TxPacketsError = (UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow;

	portStats->OctetsRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][15].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][15].stAdded.uiLow
					+ (UINT64)((UINT64)g_astCntSwhPort[port_id][16].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][16].stAdded.uiLow;

	/* The number of good broadcast frames received */
	portStats->OctetsRxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow;

	/* The number of good multicast frames received. */
	portStats->OctetsRxTopRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow;

	portStats->UnicastFramesRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][5].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][5].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][6].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][6].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][7].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][7].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][8].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][8].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][9].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][9].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][10].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][10].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][11].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][11].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][12].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][12].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][13].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][13].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][14].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][14].stAdded.uiLow -
					(
						(UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow
					);

	portStats->NonUnicastFramesRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow
							+ (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow;

	/* The number of PAUSE frames received */
	portStats->FrameRxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][1].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][1].stAdded.uiLow;

	portStats->RxPacketsError = (UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow
					+ (UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow;
#else
	OPL_CNT_t oplCounter[DAL_CNT_GE_NUM];

	dalCounterGet(DAL_CNT_POLL_GE_MASK, oplCounter, 0, DAL_CNT_GE_NUM);

	portStats->OctetsTx = (UINT64)((UINT64)oplCounter[11].stAdded.uiHigh << 32) + (UINT64)oplCounter[11].stAdded.uiLow;
	portStats->OctetsTxRate = 0;
	portStats->OctetsTxTopRate = 0;
	portStats->UnicastFramesTx = (UINT64)((UINT64)oplCounter[0].stAdded.uiHigh << 32) + (UINT64)oplCounter[0].stAdded.uiLow
						- (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) - (UINT64)oplCounter[1].stAdded.uiLow
						- (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32) - (UINT64)oplCounter[2].stAdded.uiLow;
	portStats->NonUnicastFramesTx = (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) + (UINT64)oplCounter[1].stAdded.uiLow
						+ (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32) + (UINT64)oplCounter[2].stAdded.uiLow;
	portStats->FrameTxRate = 0;
	portStats->TxPacketsError = 0;

	portStats->OctetsRx = (UINT64)((UINT64)oplCounter[34].stAdded.uiHigh << 32) + (UINT64)oplCounter[34].stAdded.uiLow;
	portStats->OctetsRxRate = 0;
	portStats->OctetsRxTopRate = 0;
	portStats->UnicastFramesRx = (UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32) + (UINT64)oplCounter[13].stAdded.uiLow
		- (
		(UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) + (UINT64)oplCounter[20].stAdded.uiLow +
		(UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32) + (UINT64)oplCounter[21].stAdded.uiLow
		);
	portStats->NonUnicastFramesRx = (UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) + (UINT64)oplCounter[20].stAdded.uiLow +
		(UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32) + (UINT64)oplCounter[21].stAdded.uiLow;
	portStats->FrameRxRate = 0;
	portStats->RxPacketsError = (UINT64)((UINT64)oplCounter[19].stAdded.uiHigh << 32) + (UINT64)oplCounter[19].stAdded.uiLow -
		(UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32) - (UINT64)oplCounter[13].stAdded.uiLow;
#endif

	return OK;
}

OPL_STATUS odmPonStatsGet(pon_statistics_t * ponStats)
{
	UINT8 Index;
	OPL_CNT_t oplCounter[DAL_CNT_PON_NUM];

	dalCounterGet(DAL_CNT_POLL_PON_MASK, oplCounter, 0, DAL_CNT_PON_NUM);
 
	/* get pon statistics information */
	ponStats->OctetsTx = oplCounter[13].stAdded.uiLow;

	/* Good broadcast packets transmitted.*/
	ponStats->OctetsTxRate = oplCounter[3].stAdded.uiLow;

	/* Good multicast packets transmitted */
	ponStats->OctetsTxTopRate = oplCounter[4].stAdded.uiLow;

	ponStats->FrameTx = oplCounter[2].stAdded.uiLow;

	/* OAM packets transmitted */
	ponStats->FrameTxRate = oplCounter[15].stAdded.uiLow;

	ponStats->TxPacketsError = 0;

	ponStats->OctetsRx = oplCounter[37].stAdded.uiLow;

	/* Good broadcast packets received */
	ponStats->OctetsRxRate = oplCounter[23].stAdded.uiLow;

	/* Good multicast packets received */
	ponStats->OctetsRxTopRate = oplCounter[24].stAdded.uiLow;

	ponStats->FrameRx = oplCounter[22].stAdded.uiLow;

	/* OAM packets received */
	ponStats->FrameRxRate = oplCounter[42].stAdded.uiLow;

	ponStats->RxLosePackets = oplCounter[17].stAdded.uiLow;

	if (0 != oplCounter[22].stAdded.uiLow)
	{
		ponStats->RxLosePacketsRate = oplCounter[17].stAdded.uiLow / oplCounter[22].stAdded.uiLow;
	}
	else
	{
		ponStats->RxLosePacketsRate = 0;
	}

	return OPL_OK;
}



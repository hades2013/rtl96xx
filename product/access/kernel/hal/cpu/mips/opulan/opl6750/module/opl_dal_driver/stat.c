/*
=============================================================================
     Header Name: stat.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/7		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include <opl_driver.h>
#include <opl_errno.h>
#include <opl_debug.h>
#include <opl_utils.h>
#include <hw_interface.h>
#include <oam.h>
#include <stat.h>

sem_t statTimeSem;

TIMER_UNIT_t statUnit;
UINT32 statInterval = 1;
UINT8 statEnable = OPL_DISABLE;
UINT8 statTokenEnable = OPL_DISABLE;
UINT32 numOfToken = 1;
UINT32 numOfCounter = OPL_ZERO;
UINT32 currenttokenIndex = OPL_ZERO;

UINT32 *tokenCounterArry = OPL_NULL;

OPCONN_RMON_STAT_t  rmonStats[] = 
{
{"GE_PARSER_RX_DROP_PKTCNT",	 		REG_GE_PARSER_RX_DROP_PKTCNT/4,			REG_GE_PARSER_RX_DROP_PKTCNT,			WIDTHMASK(32),	0,OPL_NULL},
{"GE_PARSER_FULL_DROP_PKTCNT",		REG_GE_PARSER_FULL_DROP_PKTCNT/4,		REG_GE_PARSER_FULL_DROP_PKTCNT,		WIDTHMASK(32),	0,OPL_NULL},
{"GE_PARSER_ERR_DROP_PKTCNT",		REG_GE_PARSER_ERR_DROP_PKTCNT/4,		REG_GE_PARSER_ERR_DROP_PKTCNT,		WIDTHMASK(32),	0,OPL_NULL},
{"GE_PARSER_TOKEN_DROP_PKTCNT",	REG_GE_PARSER_TOKEN_DROP_PKTCNT/4,	REG_GE_PARSER_TOKEN_DROP_PKTCNT,	WIDTHMASK(32),	0,OPL_NULL},
{"GE_PARSER_RX_PKTCNT",						REG_GE_PARSER_RX_PKTCNT/4,						REG_GE_PARSER_RX_PKTCNT,						WIDTHMASK(32),	0,OPL_NULL},
{"GE_MaxMinDropPktCnt",							REG_GE_MaxMinDropPktCnt/4,							REG_GE_MaxMinDropPktCnt,							WIDTHMASK(32),	0,OPL_NULL},	

{"GMAC_TxetherStatsPktsCount",	 								REG_GMAC_TxetherStatsPktsCount/4,									REG_GMAC_TxetherStatsPktsCount,									WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsBroadcastPktsCount",	 				REG_GMAC_TxetherStatsBroadcastPktsCount/4,					REG_GMAC_TxetherStatsBroadcastPktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsMulticastPktsCount",	 				REG_GMAC_TxetherStatsMulticastPktsCount/4,					REG_GMAC_TxetherStatsMulticastPktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxaPAUSEMACCtrlFramesCount",	 				REG_GMAC_TxaPAUSEMACCtrlFramesCount/4,					REG_GMAC_TxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxaMACControlFramesCount",	 					REG_GMAC_TxaMACControlFramesCount/4,						REG_GMAC_TxaMACControlFramesCount,						WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts64OctetsCount",	 				REG_GMAC_TxetherStatsPkts64OctetsCount/4,					REG_GMAC_TxetherStatsPkts64OctetsCount	,				WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts65to127OctetsCount",			REG_GMAC_TxetherStatsPkts65to127OctetsCount/4,			REG_GMAC_TxetherStatsPkts65to127OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts128to255OctetsCount",		REG_GMAC_TxetherStatsPkts128to255OctetsCount/4,		REG_GMAC_TxetherStatsPkts128to255OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxtherStatsPkts256to511OctetsCount",			REG_GMAC_TxtherStatsPkts256to511OctetsCount/4,			REG_GMAC_TxtherStatsPkts256to511OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts512to1023OctetsCount",		REG_GMAC_TxetherStatsPkts512to1023OctetsCount/4,		REG_GMAC_TxetherStatsPkts512to1023OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts1024to1518OctetsCount",	REG_GMAC_TxetherStatsPkts1024to1518OctetsCount/4,	REG_GMAC_TxetherStatsPkts1024to1518OctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsOctetsCount",	 							REG_GMAC_TxetherStatsOctetsCount/4,								REG_GMAC_TxetherStatsOctetsCount,								WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_TxetherStatsPkts1519toMAXOctetsCoun",	 	REG_GMAC_TxetherStatsPkts1519toMAXOctetsCount/4,	REG_GMAC_TxetherStatsPkts1519toMAXOctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsGoodPktsCount",	 					REG_GMAC_RxetherStatsGoodPktsCount/4,						REG_GMAC_RxetherStatsGoodPktsCount,						WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsDropEventsCount",	 					REG_GMAC_RxetherStatsDropEventsCount/4,						REG_GMAC_RxetherStatsDropEventsCount,						WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsCRCAlignErrorsCount",				REG_GMAC_RxetherStatsCRCAlignErrorsCount/4,				REG_GMAC_RxetherStatsCRCAlignErrorsCount,				WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsLenmisCount",	 						REG_GMAC_RxetherStatsLenmisCount/4,							REG_GMAC_RxetherStatsLenmisCount,							WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsShortCount",	 							REG_GMAC_RxetherStatsShortCount/4,								REG_GMAC_RxetherStatsShortCount,								WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsJabbersCount",	 						REG_GMAC_RxetherStatsJabbersCount/4,							REG_GMAC_RxetherStatsJabbersCount,							WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPktsCount",	 								REG_GMAC_RxetherStatsPktsCount/4,									REG_GMAC_RxetherStatsPktsCount,									WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsBroadcastPktsCount",	 			REG_GMAC_RxetherStatsBroadcastPktsCount/4,				REG_GMAC_RxetherStatsBroadcastPktsCount,				WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsMulticastPktsCount",	 				REG_GMAC_RxetherStatsMulticastPktsCount	/4,				REG_GMAC_RxetherStatsMulticastPktsCount	,				WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxaPAUSEMACCtrlFramesCount",	 				REG_GMAC_RxaPAUSEMACCtrlFramesCount/4,					REG_GMAC_RxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxaUnsupportedOpcodesCount",	 				REG_GMAC_RxaUnsupportedOpcodesCount/4,					REG_GMAC_RxaUnsupportedOpcodesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxaMACControlFramesCount",	 					REG_GMAC_RxaMACControlFramesCount/4,						REG_GMAC_RxaMACControlFramesCount,						WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsUndersizePktsCount",	 				REG_GMAC_RxetherStatsUndersizePktsCount/4,					REG_GMAC_RxetherStatsUndersizePktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsOversizePktsCount",	 				REG_GMAC_RxetherStatsOversizePktsCount/4,					REG_GMAC_RxetherStatsOversizePktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPkts64OctetsCount",	 				REG_GMAC_RxetherStatsPkts64OctetsCount/4,					REG_GMAC_RxetherStatsPkts64OctetsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxtherStatsPkts65to127OctetsCount",			REG_GMAC_RxtherStatsPkts65to127OctetsCount/4,			REG_GMAC_RxtherStatsPkts65to127OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPkts128to255OctetsCount",		REG_GMAC_RxetherStatsPkts128to255OctetsCount/4,		REG_GMAC_RxetherStatsPkts128to255OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxtherStatsPkts256to511OctetsCount",			REG_GMAC_RxtherStatsPkts256to511OctetsCount/4,			REG_GMAC_RxtherStatsPkts256to511OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPkts512to1023OctetsCount",		REG_GMAC_RxetherStatsPkts512to1023OctetsCount/4,		REG_GMAC_RxetherStatsPkts512to1023OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPkts1024to1518OctetsCount",	REG_GMAC_RxetherStatsPkts1024to1518OctetsCount/4,	REG_GMAC_RxetherStatsPkts1024to1518OctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsPkts1518toMAXOctetsCount",	REG_GMAC_RxetherStatsPkts1518toMAXOctetsCount/4,	REG_GMAC_RxetherStatsPkts1518toMAXOctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RxetherStatsOctetsCount",	 							REG_GMAC_RxetherStatsOctetsCount/4,								REG_GMAC_RxetherStatsOctetsCount,								WIDTHMASK(32),	0,OPL_NULL},
{"GMAC_RX_ETHL2MULTICAST_PKT_CNT",	 				REG_GMAC_RX_ETHL2MULTICAST_PKT_CNT/4,					REG_GMAC_RX_ETHL2MULTICAST_PKT_CNT,					WIDTHMASK(26),	0,OPL_NULL},

{"GMAC_RxetherRX_ERdropCount",	 							REG_GMAC_RxetherRX_ERdropCount/4,						   		REG_GMAC_RxetherRX_ERdropCount,								WIDTHMASK(16),	0,OPL_NULL},
{"GMAC_RxetherIFGdropCount",	 								REG_GMAC_RxetherIFGdropCount/4,									REG_GMAC_RxetherIFGdropCount,									WIDTHMASK(16),	0,OPL_NULL},

{"PMAC_TxetherStatsPktsCount",	 								REG_PMAC_TxetherStatsPktsCount/4,									REG_PMAC_TxetherStatsPktsCount,									WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsBroadcastPktsCount",	 				REG_PMAC_TxetherStatsBroadcastPktsCount/4,					REG_PMAC_TxetherStatsBroadcastPktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsMulticastPktsCount",	 				REG_PMAC_TxetherStatsMulticastPktsCount/4,					REG_PMAC_TxetherStatsMulticastPktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxaPAUSEMACCtrlFramesCount",	 				REG_PMAC_TxaPAUSEMACCtrlFramesCount/4,					REG_PMAC_TxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxaMACControlFramesCount",	 					REG_PMAC_TxaMACControlFramesCount/4,						REG_PMAC_TxaMACControlFramesCount,						WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts64OctetsCount",	 				REG_PMAC_TxetherStatsPkts64OctetsCount/4,					REG_PMAC_TxetherStatsPkts64OctetsCount	,				WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts65to127OctetsCount",			REG_PMAC_TxetherStatsPkts65to127OctetsCount/4,			REG_PMAC_TxetherStatsPkts65to127OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts128to255OctetsCount",		REG_PMAC_TxetherStatsPkts128to255OctetsCount/4,		REG_PMAC_TxetherStatsPkts128to255OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxtherStatsPkts256to511OctetsCount",			REG_PMAC_TxtherStatsPkts256to511OctetsCount/4,			REG_PMAC_TxtherStatsPkts256to511OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts512to1023OctetsCount",		REG_PMAC_TxetherStatsPkts512to1023OctetsCount/4,		REG_PMAC_TxetherStatsPkts512to1023OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts1024to1518OctetsCount",	REG_PMAC_TxetherStatsPkts1024to1518OctetsCount/4,	REG_PMAC_TxetherStatsPkts1024to1518OctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsOctetsCount",	 							REG_PMAC_TxetherStatsOctetsCount/4,								REG_PMAC_TxetherStatsOctetsCount,								WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_TxetherStatsPkts1519toMAXOctetsCount",	 	REG_PMAC_TxetherStatsPkts1519toMAXOctetsCount/4,		REG_PMAC_TxetherStatsPkts1519toMAXOctetsCount,		WIDTHMASK(26),	0,OPL_NULL},
{"PMAC_TX_OAM_CNT",	 											REG_PMAC_TX_OAM_CNT/4,												REG_PMAC_TX_OAM_CNT,												WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_RxetherStatsGoodPktsCount",	 						REG_PMAC_RxetherStatsGoodPktsCount/4,							REG_PMAC_RxetherStatsGoodPktsCount,							WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsDropEventsCount",	 					REG_PMAC_RxetherStatsDropEventsCount/4,						REG_PMAC_RxetherStatsDropEventsCount,						WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsCRCAlignErrorsCount",				REG_PMAC_RxetherStatsCRCAlignErrorsCount/4,				REG_PMAC_RxetherStatsCRCAlignErrorsCount,				WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsLenmisCount",	 							REG_PMAC_RxetherStatsLenmisCount/4,								REG_PMAC_RxetherStatsLenmisCount,								WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsShortCount",	 							REG_PMAC_RxetherStatsShortCount/4,								REG_PMAC_RxetherStatsShortCount,								WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsJabbersCount",	 						REG_PMAC_RxetherStatsJabbersCount/4,							REG_PMAC_RxetherStatsJabbersCount,							WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPktsCount",	 								REG_PMAC_RxetherStatsPktsCount/4,									REG_PMAC_RxetherStatsPktsCount,									WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsBroadcastPktsCount",	 				REG_PMAC_RxetherStatsBroadcastPktsCount/4,					REG_PMAC_RxetherStatsBroadcastPktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsMulticastPktsCount",	 				REG_PMAC_RxetherStatsMulticastPktsCount	/4,				REG_PMAC_RxetherStatsMulticastPktsCount	,				WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxaPAUSEMACCtrlFramesCount",	 				REG_PMAC_RxaPAUSEMACCtrlFramesCount/4,					REG_PMAC_RxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxaUnsupportedOpcodesCount",	 				REG_PMAC_RxaUnsupportedOpcodesCount/4,					REG_PMAC_RxaUnsupportedOpcodesCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxaMACControlFramesCount",	 					REG_PMAC_RxaMACControlFramesCount/4,						REG_PMAC_RxaMACControlFramesCount,						WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsUndersizePktsCount",	 				REG_PMAC_RxetherStatsUndersizePktsCount/4,					REG_PMAC_RxetherStatsUndersizePktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsOversizePktsCount",	 				REG_PMAC_RxetherStatsOversizePktsCount/4,					REG_PMAC_RxetherStatsOversizePktsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPkts64OctetsCount",	 				REG_PMAC_RxetherStatsPkts64OctetsCount/4,					REG_PMAC_RxetherStatsPkts64OctetsCount,					WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxtherStatsPkts65to127OctetsCount",			REG_PMAC_RxtherStatsPkts65to127OctetsCount/4,			REG_PMAC_RxtherStatsPkts65to127OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPkts128to255OctetsCount",		REG_PMAC_RxetherStatsPkts128to255OctetsCount/4,		REG_PMAC_RxetherStatsPkts128to255OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxtherStatsPkts256to511OctetsCount",			REG_PMAC_RxtherStatsPkts256to511OctetsCount/4,			REG_PMAC_RxtherStatsPkts256to511OctetsCount,			WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPkts512to1023OctetsCount",		REG_PMAC_RxetherStatsPkts512to1023OctetsCount/4,		REG_PMAC_RxetherStatsPkts512to1023OctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPkts1024to1518OctetsCount",	REG_PMAC_RxetherStatsPkts1024to1518OctetsCount/4,	REG_PMAC_RxetherStatsPkts1024to1518OctetsCount,	WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsPkts1518toMAXOctetsCount",		REG_PMAC_RxetherStatsPkts1518toMAXOctetsCount/4,		REG_PMAC_RxetherStatsPkts1518toMAXOctetsCount,		WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_RxetherStatsOctetsCount",	 							REG_PMAC_RxetherStatsOctetsCount/4,								REG_PMAC_RxetherStatsOctetsCount,								WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_FEC_CORRECTED_PKTCNT",	 						REG_PMAC_FEC_CORRECTED_PKTCNT/4,							REG_PMAC_FEC_CORRECTED_PKTCNT,							WIDTHMASK(32),	0,OPL_NULL},
{"PMAC_FEC_UNCORRECTED_PKTCNT",	 					REG_PMAC_FEC_UNCORRECTED_PKTCNT/4,						REG_PMAC_FEC_UNCORRECTED_PKTCNT,						WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_FEC_NOERR_PKTCNT",	 								REG_PMAC_FEC_NOERR_PKTCNT/4,									REG_PMAC_FEC_NOERR_PKTCNT,									WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_RX_ETHL2MULTICAST_PKT_CNT",	 				REG_PMAC_RX_ETHL2MULTICAST_PKT_CNT/4,					REG_PMAC_RX_ETHL2MULTICAST_PKT_CNT,					WIDTHMASK(26),	0,OPL_NULL},
{"PMAC_RX_OAM_CNT",	 											REG_PMAC_RX_OAM_CNT/4,												REG_PMAC_RX_OAM_CNT,												WIDTHMASK(16),0,OPL_NULL},
{"PMAC_RxetherRX_ERdropCount",	 							REG_PMAC_RxetherRX_ERdropCount/4,								REG_PMAC_RxetherRX_ERdropCount,								WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_RxetherIFGdropCount",	 								REG_RxetherIFGdropCount/4,												REG_RxetherIFGdropCount,												WIDTHMASK(16),	0,OPL_NULL},
{"REG_PMAC_SECU_LEN_DROP_CNT",	 						REG_PMAC_SECU_LEN_DROP_CNT/4,									REG_PMAC_SECU_LEN_DROP_CNT,									WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_RScrc8errCount",	 											REG_RScrc8errCount/4,														REG_RScrc8errCount,														WIDTHMASK(16),	0,OPL_NULL},
{"PMAC_RSlliderrCount",	 											REG_RSlliderrCount/4,															REG_RSlliderrCount,															WIDTHMASK(16),	0,OPL_NULL},
{"REG_PMAC_EBR",	 											REG_PMAC_EBR/4,														REG_PMAC_EBR,														WIDTHMASK(32),0,OPL_NULL},
{"REG_PMAC_CBR",	 											REG_PMAC_CBR/4,														REG_PMAC_CBR,														WIDTHMASK(32),0,OPL_NULL},

{"TMUS_WRED_DROP",					REG_TMUS_WRED_DROP/4,					REG_TMUS_WRED_DROP,					WIDTHMASK(16),0,OPL_NULL},
{"TMUS_ENQ_PKT_CNT",				REG_TMUS_ENQ_PKT_CNT/4,				REG_TMUS_ENQ_PKT_CNT,				WIDTHMASK(32),0,OPL_NULL},
{"TMDS_WRED_DROP",					REG_TMDS_WRED_DROP/4,					REG_TMDS_WRED_DROP,					WIDTHMASK(16),0,OPL_NULL},
{"TMDS_ENQ_PKT_CNT",				REG_TMDS_ENQ_PKT_CNT/4,				REG_TMDS_ENQ_PKT_CNT,				WIDTHMASK(32),0,OPL_NULL},
	
{"TMUS_RXED_PKT_CNT",				REG_TMUS_RXED_PKT_CNT/4,				REG_TMUS_RXED_PKT_CNT,					WIDTHMASK(32),0,OPL_NULL},
{"TMUS_CMDDRP_PKT_CNT",		REG_TMUS_CMDDRP_PKT_CNT/4,		REG_TMUS_CMDDRP_PKT_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"TMUS_QFULLDRP_PKT_CNT",		REG_TMUS_QFULLDRP_PKT_CNT/4,		REG_TMUS_QFULLDRP_PKT_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"TMUS_TXED_PKT_CNT",				REG_TMUS_TXED_PKT_CNT/4,				REG_TMUS_TXED_PKT_CNT,					WIDTHMASK(32),0,OPL_NULL},
{"TMDS_RXED_PKT_CNT",				REG_TMDS_RXED_PKT_CNT/4,				REG_TMDS_RXED_PKT_CNT,					WIDTHMASK(32),0,OPL_NULL},
{"TMDS_CMDDRP_PKT_CNT", 		REG_TMDS_CMDDRP_PKT_CNT/4,		REG_TMDS_CMDDRP_PKT_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"TMDS_QFULLDRP_PKT_CNT",		REG_TMDS_QFULLDRP_PKT_CNT/4,		REG_TMDS_QFULLDRP_PKT_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"TMDS_TXED_PKT_CNT",				REG_TMDS_TXED_PKT_CNT/4,				REG_TMDS_TXED_PKT_CNT,					WIDTHMASK(32),0,OPL_NULL},

{"BRG_CLE_RULE_MISS_COUNT",			REG_CLE_RULE_MISS_COUNT/4,			REG_CLE_RULE_MISS_COUNT,			WIDTHMASK(24), 0,OPL_NULL},
{"BRG_CLE_RULE_DROP_COUNT",			REG_CLE_RULE_DROP_COUNT/4,			REG_CLE_RULE_DROP_COUNT,			WIDTHMASK(24), 0,OPL_NULL},
{"REG_BRG_GE_RX_DROP_COUNT",		REG_BRG_GE_RX_DROP_COUNT/4,			REG_BRG_GE_RX_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_TX_DROP_COUNT",		REG_BRG_GE_TX_DROP_COUNT/4,			REG_BRG_GE_TX_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_VLAN_DROP_COUNT",		REG_BRG_GE_VLAN_DROP_COUNT/4,		REG_BRG_GE_VLAN_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_TAG_DROP_COUNT",		REG_BRG_GE_TAG_DROP_COUNT/4,		REG_BRG_GE_TAG_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_RX_DROP_COUNT",		REG_BRG_PON_RX_DROP_COUNT/4,		REG_BRG_PON_RX_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_TX_DROP_COUNT",		REG_BRG_PON_TX_DROP_COUNT/4,		REG_BRG_PON_TX_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_VLAN_DROP_COUNT",		REG_BRG_PON_VLAN_DROP_COUNT/4,		REG_BRG_PON_VLAN_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_TAG_DROP_COUNT",		REG_BRG_PON_TAG_DROP_COUNT/4,		REG_BRG_PON_TAG_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_RSV_DROP_COUNT",			REG_BRG_RSV_DROP_COUNT/4,			REG_BRG_RSV_DROP_COUNT,				WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_UC_SA_DROP_COUNT",		REG_BRG_GE_UC_SA_DROP_COUNT/4,		REG_BRG_GE_UC_SA_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_DMAC_DROP_COUNT",			REG_BRG_DMAC_DROP_COUNT/4,			REG_BRG_DMAC_DROP_COUNT,			WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_LEARN_DROP_COUNT",	REG_BRG_PON_LEARN_DROP_COUNT/4,		REG_BRG_PON_LEARN_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_LEARN_DROP_COUNT",		REG_BRG_GE_LEARN_DROP_COUNT/4,		REG_BRG_GE_LEARN_DROP_COUNT	,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_LEARN_CFL_DROP_COUNT",REG_BRG_PON_LEARN_CFL_DROP_COUNT/4, REG_BRG_PON_LEARN_CFL_DROP_COUNT,	WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_GE_LEARN_CFL_DROP_COUNT",	REG_BRG_GE_LEARN_CFL_DROP_COUNT/4,	REG_BRG_GE_LEARN_CFL_DROP_COUNT	,	WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_PON_UC_SA_DROP_COUNT",	REG_BRG_PON_UC_SA_DROP_COUNT/4,		REG_BRG_PON_UC_SA_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_DS_0_MAC_DROP_COUNT",	    REG_BRG_DS_0_MAC_DROP_COUNT/4,		REG_BRG_DS_0_MAC_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
{"REG_BRG_US_0_MAC_DROP_COUNT",	    REG_BRG_US_0_MAC_DROP_COUNT/4,		REG_BRG_US_0_MAC_DROP_COUNT,		WIDTHMASK(16), 0,OPL_NULL},
	

{"MPCP_NORM_GATE_CNT",   	REG_MPCP_NORM_GATE_CNT/4,	REG_MPCP_NORM_GATE_CNT,		WIDTHMASK(32),0,OPL_NULL}, 
{"MPCP_DISC_PKT_CNT",   		REG_MPCP_DISC_PKT_CNT/4,		REG_MPCP_DISC_PKT_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"MPCP_PKT_DROP_CNT", 		REG_MPCP_PKT_DROP_CNT/4,		REG_MPCP_PKT_DROP_CNT,			WIDTHMASK(16),0,OPL_NULL},   
{"MPCP_CLE_PKT_CNT",  			REG_MPCP_CLE_PKT_CNT/4,		REG_MPCP_CLE_PKT_CNT,			WIDTHMASK(32),0,OPL_NULL},
{"MPCP_CLE_DROP_CNT",   		REG_MPCP_CLE_DROP_CNT/4,		REG_MPCP_CLE_DROP_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"MPCP_DISCV_GATE_CNT",	REG_MPCP_DISCV_GATE_CNT/4,REG_MPCP_DISCV_GATE_CNT,	WIDTHMASK(16),0,OPL_NULL},

{"GRANT_CNT",    			REG_MPCP_GRANT_CNT/4,		REG_MPCP_GRANT_CNT,			WIDTHMASK(32),0,OPL_NULL},
{"GATE_DROP_CNT",    		REG_GATE_DROP_CNT/4,		REG_GATE_DROP_CNT,			WIDTHMASK(16),0,OPL_NULL},
{"GRANT_RTP_PKT_CNT",    	REG_GRANT_RTP_PKT_CNT/4,	REG_GRANT_RTP_PKT_CNT,		WIDTHMASK(16),0,OPL_NULL},
{"GRANT_RPT_DROP_CNT",    	REG_GRANT_RPT_DROP_CNT/4,	REG_GRANT_RPT_DROP_CNT,		WIDTHMASK(16),0,OPL_NULL},
{"GRANT_TOTAL_LEN",    		REG_GRANT_TOTAL_LEN/4,		REG_GRANT_TOTAL_LEN,		WIDTHMASK(32),0,OPL_NULL},
{"MPCP_DRIFT_DROP_CNT",    	REG_MPCP_DRIFT_DROP_CNT/4,	REG_MPCP_DRIFT_DROP_CNT,	WIDTHMASK(16),0,OPL_NULL},
{"GRANT_TOTAL_LEN_IN_SEC",  REG_GRANT_TOTAL_LEN_IN_SEC/4,REG_GRANT_TOTAL_LEN_IN_SEC,WIDTHMASK(32),0,OPL_NULL},

	
{"US_NORMPKT_TX_CNT",   			REG_US_NORMPKT_TX_CNT/4,				REG_US_NORMPKT_TX_CNT,				WIDTHMASK(16),0,OPL_NULL},
{"US_REGREQ_TX_CNT",   			REG_US_REGREQ_TX_CNT/4,				REG_US_REGREQ_TX_CNT,				WIDTHMASK(16),0,OPL_NULL},
{"US_REGACK_TX_CNT", 				REG_US_REGACK_TX_CNT/4,				REG_US_REGACK_TX_CNT,				WIDTHMASK(16),0,OPL_NULL},
{"US_RPTPKT_TX_CNT",				REG_US_RPTPKT_TX_CNT/4,				REG_US_RPTPKT_TX_CNT,				WIDTHMASK(32),0,OPL_NULL},
{"MPCP_SOP_DROP_CNT",			REG_MPCP_SOP_DROP_CNT/4,			REG_MPCP_SOP_DROP_CNT,			WIDTHMASK(16),0,OPL_NULL},  
{"MPCP_CLE_FULL_DROP_CNT",	REG_MPCP_CLE_FULL_DROP_CNT/4,	REG_MPCP_CLE_FULL_DROP_CNT,	WIDTHMASK(16),0,OPL_NULL},

{"REG_DMA0_Q0_CELL_CNT", REG_DMA0_Q0_CELL_CNT/4		 , REG_DMA0_Q0_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q1_CELL_CNT", REG_DMA0_Q1_CELL_CNT/4		 , REG_DMA0_Q1_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q2_CELL_CNT", REG_DMA0_Q2_CELL_CNT/4		 , REG_DMA0_Q2_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q3_CELL_CNT", REG_DMA0_Q3_CELL_CNT/4		 , REG_DMA0_Q3_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q4_CELL_CNT", REG_DMA0_Q4_CELL_CNT/4		 , REG_DMA0_Q4_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q5_CELL_CNT", REG_DMA0_Q5_CELL_CNT/4		 , REG_DMA0_Q5_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q6_CELL_CNT", REG_DMA0_Q6_CELL_CNT/4		 , REG_DMA0_Q6_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q7_CELL_CNT", REG_DMA0_Q7_CELL_CNT/4		 , REG_DMA0_Q7_CELL_CNT		,       WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q0_PKT_CNT", REG_DMA0_Q0_PKT_CNT/4	     , REG_DMA0_Q0_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q1_PKT_CNT", REG_DMA0_Q1_PKT_CNT/4	     , REG_DMA0_Q1_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q2_PKT_CNT", REG_DMA0_Q2_PKT_CNT/4	     , REG_DMA0_Q2_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q3_PKT_CNT", REG_DMA0_Q3_PKT_CNT/4	     , REG_DMA0_Q3_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q4_PKT_CNT", REG_DMA0_Q4_PKT_CNT/4	     , REG_DMA0_Q4_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q5_PKT_CNT", REG_DMA0_Q5_PKT_CNT/4	     , REG_DMA0_Q5_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q6_PKT_CNT", REG_DMA0_Q6_PKT_CNT/4	     , REG_DMA0_Q6_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_Q7_PKT_CNT", REG_DMA0_Q7_PKT_CNT/4	     , REG_DMA0_Q7_PKT_CNT		    ,   WIDTHMASK(13),		0,OPL_NULL},        
{"REG_DMA0_RX_CNT", REG_DMA0_RX_CNT/4	         , REG_DMA0_RX_CNT			    ,   WIDTHMASK(32),		0,OPL_NULL},        
{"REG_DMA0_TX_CNT", REG_DMA0_TX_CNT/4	         , REG_DMA0_TX_CNT			    ,   WIDTHMASK(32),		0,OPL_NULL},        
{"REG_DMA0_GE2CPU_ENQ_CNT", REG_DMA0_GE2CPU_ENQ_CNT/4	 , REG_DMA0_GE2CPU_ENQ_CNT		,   WIDTHMASK(32),		0,OPL_NULL},        
{"REG_DMA0_GE2CPU_POLIC_DRPCNT", REG_DMA0_GE2CPU_POLIC_DRPCNT/4	    ,REG_DMA0_GE2CPU_POLIC_DRPCNT	,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_DMA0_GE2CPU_QFULL_DRPCNT", REG_DMA0_GE2CPU_QFULL_DRPCNT/4	    ,REG_DMA0_GE2CPU_QFULL_DRPCNT	,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_DMA0_PON2CPU_ENQ_CNT", REG_DMA0_PON2CPU_ENQ_CNT/4		    ,REG_DMA0_PON2CPU_ENQ_CNT		,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_DMA0_PON2CPU_POLIC_DRPCNT", REG_DMA0_PON2CPU_POLIC_DRPCNT/4	    ,REG_DMA0_PON2CPU_POLIC_DRPCNT  ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_DMA0_PON2CPU_QFULL_DRPCNT", REG_DMA0_PON2CPU_QFULL_DRPCNT/4	    ,REG_DMA0_PON2CPU_QFULL_DRPCNT  ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_DMA0_DEQ_CNT", REG_DMA0_DEQ_CNT/4                  ,REG_DMA0_DEQ_CNT               ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_GE_FIFO_POST_PKT_ERR_CNT", REG_GE_FIFO_POST_PKT_ERR_CNT/4      ,REG_GE_FIFO_POST_PKT_ERR_CNT   ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_PON_FIFO_POST_PKT_ERR_CNT", REG_PON_FIFO_POST_PKT_ERR_CNT/4     ,REG_PON_FIFO_POST_PKT_ERR_CNT  ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_GE_FIFO_EARLING_PKT_DRPCNT", REG_GE_FIFO_EARLING_PKT_DRPCNT/4    ,REG_GE_FIFO_EARLING_PKT_DRPCNT ,WIDTHMASK(32),		0,OPL_NULL},    
{"REG_PON_FIFO_EARLING_PKT_DRPCNT", REG_PON_FIFO_EARLING_PKT_DRPCNT/4   ,REG_PON_FIFO_EARLING_PKT_DRPCNT,WIDTHMASK(32),		0,OPL_NULL},    
{OPL_NULL,							OPL_ZERO,							 OPL_ZERO,         				OPL_ZERO,			0,OPL_NULL},
};

/*******************************************************************************
* statInterValSet
*
* DESCRIPTION:
* 
*	this function is used to set the stat interval /us
* 
* INPUTS: 
*		interVal
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statInterValSet(UINT32 interVal)
{
	statInterval = interVal;
	statUnit.uiCnt = statInterval;
	return OPL_OK;
}
/*******************************************************************************
* statInterValGet
*
* DESCRIPTION:
* 
*	this function is used get the stat interval
* 
* INPUTS: 
*		
* OUTPUTS: 
*		interVal:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statInterValGet(UINT32 *interVal)
{
	*interVal = statInterval;
	return OPL_OK;
}
/*******************************************************************************
* statRmonEnable
*
* DESCRIPTION:
* 
*	this function is used to enable or disable rmon style stat.
* 
* INPUTS: 
*		enable:0 for disable while 1 for enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statRmonEnable(UINT8 enable)
{
	statEnable = enable;
	return OPL_OK;
}

/*******************************************************************************
* statUpdate
*
* DESCRIPTION:
* 
*	this function is used to update the counter per interval
* 
* INPUTS: 
*		mode: module such as brg,tm,mpcp and so on 
* OUTPUTS: 
*	 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statUpdate(UINT8 mode)
{
	UINT32 regVal;
	UINT32 index = OPL_ZERO;

	while(rmonStats[index].name != NULL)
	{
		oplRegRead(rmonStats[index].regAddr, &regVal);
		rmonStats[index].counter += regVal&rmonStats[index].regMask;
		if(statTokenEnable)
		{
			rmonStats[index].pTokenCounter[currenttokenIndex%numOfToken] = regVal&rmonStats[index].regMask;	
		}
		index++;
	}

	currenttokenIndex++;
	if(currenttokenIndex == numOfToken)
	{
		currenttokenIndex = 0;
	}
	
	return OPL_OK;	
}
/*******************************************************************************
* statsThread
*
* DESCRIPTION:
* 
*	statsThread
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
void statsThread(void)
{
	statUnit.iFlag = OPL_ZERO;
	statUnit.uiCnt = statInterval;
	statUnit.iStart = OPL_ZERO;
			
	sem_init (&statTimeSem, 0, 0);

	while(1)
	{
		sem_wait(&statTimeSem);
		if(statEnable)
		{
			statUpdate(0);	
		}
	}
}
/*******************************************************************************
* statsShow
*
* DESCRIPTION:
* 
*	this function is used show all counter 
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
void statsShow(void)
{
	UINT32 index = OPL_ZERO;

	OPL_DRV_PRINTF(("%-45s %6s %-18s\n","regName","regId","counter"));
	while(rmonStats[index].name != NULL)
	{
		OPL_DRV_PRINTF(("%-45s 0x%04x %16llu\n",
			rmonStats[index].name,
			rmonStats[index].regId,
			rmonStats[index].counter));
		index++;
	}
	return ;
}
/*******************************************************************************
* statsClear
*
* DESCRIPTION:
* 
*	this function is used clear all stats 
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
void statsClear(void)
{
	UINT32 index = OPL_ZERO;

	while(rmonStats[index].name != NULL)
	{
		rmonStats[index].counter = OPL_ZERO;
		if(statTokenEnable)
		{
			OPL_MEMSET(rmonStats[index].pTokenCounter,0X00,4*numOfToken);
		}
		index++;
	}
	return ;
}

/*******************************************************************************
* statsTokenCreate
*
* DESCRIPTION:
* 
*	this function is used to create token counter buff
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
void statsTokenCreate(UINT32 numOfToken)
{
	UINT32 index = OPL_ZERO;

	while(rmonStats[index].name != NULL)
	{
		index++;
	}

	tokenCounterArry = (UINT32 *)OPL_MALLOC((index+1)*(numOfToken + 1)*4);
	if(tokenCounterArry == OPL_NULL)
	{
		OPL_DRV_PRINTF(("malloc buffer for token counter failed.\n"));
	} 

	index = 0;
	
	while(rmonStats[index].name != NULL)
	{
		rmonStats[index].pTokenCounter = &tokenCounterArry[index*numOfToken];
		index++;
	}
	
	numOfCounter = index;
	return OPL_OK;
}

/*******************************************************************************
* statsTokenDestroy
*
* DESCRIPTION:
* 
*	this function is used to create token counter buff
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
void statsTokenDestroy(void)
{
	UINT32 index = OPL_ZERO;

	while(rmonStats[index].name != NULL)
	{
		rmonStats[index].pTokenCounter =  OPL_NULL;
		index++;
	}

	OPL_FREE(tokenCounterArry);	
	return;
}

/*******************************************************************************
* statsTokenSet
*
* DESCRIPTION:
* 
*	this function is used to enable and set the token num
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
void statsTokenSet(UINT8 enable,UINT32 tokenNum)
{
	if(enable)
	{
		statsTokenCreate(tokenNum);
		numOfToken = tokenNum;
		statTokenEnable = OPL_ENABLE;
		statEnable  = OPL_ENABLE;
	}else
	{
		statEnable = OPL_DISABLE;
		statTokenEnable = OPL_DISABLE;
		numOfToken = OPL_ZERO;
		statsTokenDestroy();
		return;
	}
}

/*******************************************************************************
* statsTokenCounterShow
*
* DESCRIPTION:
* 
*	this function is used to enable and set the token num
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
void statsTokenCounterShow(UINT32 startTokenId,UINT32 endTokenId)
{
	UINT32 index = OPL_ZERO;
	UINT32 tokenIndex;
	UINT32 numOfTokenToShow;

	if(startTokenId  > endTokenId )
	{
		OPL_DRV_PRINTF(("input error.\n"));
		return ;
	}

	if(endTokenId > numOfToken)
	{
		OPL_DRV_PRINTF(("input error.\n"));
		return ;
	}
	OPL_DRV_PRINTF(("%-45s ","regName"));
	for(tokenIndex = startTokenId;tokenIndex <= endTokenId;tokenIndex++ )
	{
		OPL_DRV_PRINTF(("%-10u ",tokenIndex));
	}
	OPL_DRV_PRINTF(("\n"));

	
	while(rmonStats[index].name != NULL)
	{
		OPL_DRV_PRINTF(("%-45s ",rmonStats[index].name));
		for(tokenIndex = startTokenId;tokenIndex <= endTokenId;tokenIndex++ )
		{
			OPL_DRV_PRINTF(("%-10u ",rmonStats[index].pTokenCounter[tokenIndex]));
		}
		OPL_DRV_PRINTF(("\n"));
		index++;
	}
	return ;
}

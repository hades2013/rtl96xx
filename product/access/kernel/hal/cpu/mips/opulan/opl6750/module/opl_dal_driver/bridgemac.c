/*
=============================================================================
     File Name: bridgemac.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/20		Initial Version
----------------   ------------  ----------------------------------------------
*/
#include "bridgemac.h"

#define ETH_MAC_ADDR_LEN             6       /* Ethernet MAC address length */

#if 0
#define BRG_ARL_DEBUG  1
#endif

SOFT_LEARN_MAC_t *psoftLearnMacEntry;

OPCONN_COUNTER_t brgGeParserCounter[] =
{
	{"GE_PARSER_RX_DROP_PKTCNT",	 		REG_GE_PARSER_RX_DROP_PKTCNT/4,			REG_GE_PARSER_RX_DROP_PKTCNT,			WIDTHMASK(32),	OPL_NULL},
	{"GE_PARSER_FULL_DROP_PKTCNT",		REG_GE_PARSER_FULL_DROP_PKTCNT/4,		REG_GE_PARSER_FULL_DROP_PKTCNT,		WIDTHMASK(32),	OPL_NULL},
	{"GE_PARSER_ERR_DROP_PKTCNT",		REG_GE_PARSER_ERR_DROP_PKTCNT/4,		REG_GE_PARSER_ERR_DROP_PKTCNT,		WIDTHMASK(32),	OPL_NULL},
	{"GE_PARSER_TOKEN_DROP_PKTCNT",	REG_GE_PARSER_TOKEN_DROP_PKTCNT/4,	REG_GE_PARSER_TOKEN_DROP_PKTCNT,	WIDTHMASK(32),	OPL_NULL},
	{"GE_PARSER_RX_PKTCNT",						REG_GE_PARSER_RX_PKTCNT/4,						REG_GE_PARSER_RX_PKTCNT,						WIDTHMASK(32),	OPL_NULL},
	{"GE_MaxMinDropPktCnt",							REG_GE_MaxMinDropPktCnt/4,							REG_GE_MaxMinDropPktCnt,							WIDTHMASK(32),	OPL_NULL},

	{OPL_NULL,												OPL_ZERO,														OPL_ZERO,													OPL_ZERO,	OPL_NULL}
};


OPCONN_COUNTER_t brgPortCounter[] =
{
{"GMAC_TxetherStatsPktsCount",	 								REG_GMAC_TxetherStatsPktsCount/4,								REG_GMAC_TxetherStatsPktsCount,								WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsBroadcastPktsCount",	 				REG_GMAC_TxetherStatsBroadcastPktsCount/4,				REG_GMAC_TxetherStatsBroadcastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsMulticastPktsCount",	 				REG_GMAC_TxetherStatsMulticastPktsCount/4,				REG_GMAC_TxetherStatsMulticastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxaPAUSEMACCtrlFramesCount",	 				REG_GMAC_TxaPAUSEMACCtrlFramesCount/4,					REG_GMAC_TxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxaMACControlFramesCount",	 					REG_GMAC_TxaMACControlFramesCount/4,						REG_GMAC_TxaMACControlFramesCount,						WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts64OctetsCount",	 				REG_GMAC_TxetherStatsPkts64OctetsCount/4,				REG_GMAC_TxetherStatsPkts64OctetsCount	,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts65to127OctetsCount",	 		REG_GMAC_TxetherStatsPkts65to127OctetsCount/4,		REG_GMAC_TxetherStatsPkts65to127OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts128to255OctetsCount",	 	REG_GMAC_TxetherStatsPkts128to255OctetsCount/4,		REG_GMAC_TxetherStatsPkts128to255OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxtherStatsPkts256to511OctetsCount",	 		REG_GMAC_TxtherStatsPkts256to511OctetsCount/4,		REG_GMAC_TxtherStatsPkts256to511OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts512to1023OctetsCount",	 	REG_GMAC_TxetherStatsPkts512to1023OctetsCount/4,	REG_GMAC_TxetherStatsPkts512to1023OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts1024to1518OctetsCount",	REG_GMAC_TxetherStatsPkts1024to1518OctetsCount/4,REG_GMAC_TxetherStatsPkts1024to1518OctetsCount,WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsOctetsCount",	 							REG_GMAC_TxetherStatsOctetsCount/4,							REG_GMAC_TxetherStatsOctetsCount,							WIDTHMASK(32),	OPL_NULL},
{"GMAC_TxetherStatsPkts1519toMAXOctetsCoun",	 	REG_GMAC_TxetherStatsPkts1519toMAXOctetsCount/4,	REG_GMAC_TxetherStatsPkts1519toMAXOctetsCount,	WIDTHMASK(26),	OPL_NULL},
{"GMAC_TxL2MulticastPktsCount",	 					REG_GMAC_TxL2MulticastPktsCount/4,					REG_GMAC_TxL2MulticastPktsCount,				WIDTHMASK(26),	OPL_NULL},
{"GMAC_RxetherStatsGoodPktsCount",	 						REG_GMAC_RxetherStatsGoodPktsCount/4,						REG_GMAC_RxetherStatsGoodPktsCount,						WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsDropEventsCount",	 					REG_GMAC_RxetherStatsDropEventsCount/4,					REG_GMAC_RxetherStatsDropEventsCount,					WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsCRCAlignErrorsCount",	 			REG_GMAC_RxetherStatsCRCAlignErrorsCount/4,				REG_GMAC_RxetherStatsCRCAlignErrorsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsLenmisCount",	 							REG_GMAC_RxetherStatsLenmisCount/4,							REG_GMAC_RxetherStatsLenmisCount,							WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsShortCount",	 							REG_GMAC_RxetherStatsShortCount/4,								REG_GMAC_RxetherStatsShortCount,							WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsJabbersCount",	 						REG_GMAC_RxetherStatsJabbersCount/4,							REG_GMAC_RxetherStatsJabbersCount,						WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPktsCount",	 								REG_GMAC_RxetherStatsPktsCount/4,								REG_GMAC_RxetherStatsPktsCount,								WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsBroadcastPktsCount",	 				REG_GMAC_RxetherStatsBroadcastPktsCount/4,				REG_GMAC_RxetherStatsBroadcastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsMulticastPktsCount",	 				REG_GMAC_RxetherStatsMulticastPktsCount	/4,				REG_GMAC_RxetherStatsMulticastPktsCount	,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxaPAUSEMACCtrlFramesCount",	 				REG_GMAC_RxaPAUSEMACCtrlFramesCount/4,					REG_GMAC_RxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxaUnsupportedOpcodesCount",	 					REG_GMAC_RxaUnsupportedOpcodesCount/4,					REG_GMAC_RxaUnsupportedOpcodesCount,					WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxaMACControlFramesCount",	 					REG_GMAC_RxaMACControlFramesCount/4,						REG_GMAC_RxaMACControlFramesCount,						WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsUndersizePktsCount",	 				REG_GMAC_RxetherStatsUndersizePktsCount/4,				REG_GMAC_RxetherStatsUndersizePktsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsOversizePktsCount",	 				REG_GMAC_RxetherStatsOversizePktsCount/4,				REG_GMAC_RxetherStatsOversizePktsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPkts64OctetsCount",	 				REG_GMAC_RxetherStatsPkts64OctetsCount/4,				REG_GMAC_RxetherStatsPkts64OctetsCount,				WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxtherStatsPkts65to127OctetsCount",	 		REG_GMAC_RxtherStatsPkts65to127OctetsCount/4,		REG_GMAC_RxtherStatsPkts65to127OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPkts128to255OctetsCount",	 	REG_GMAC_RxetherStatsPkts128to255OctetsCount/4,	REG_GMAC_RxetherStatsPkts128to255OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxtherStatsPkts256to511OctetsCount",	 		REG_GMAC_RxtherStatsPkts256to511OctetsCount/4,		REG_GMAC_RxtherStatsPkts256to511OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPkts512to1023OctetsCount",	 	REG_GMAC_RxetherStatsPkts512to1023OctetsCount/4,	REG_GMAC_RxetherStatsPkts512to1023OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPkts1024to1518OctetsCount",	REG_GMAC_RxetherStatsPkts1024to1518OctetsCount/4,REG_GMAC_RxetherStatsPkts1024to1518OctetsCount,WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsPkts1518toMAXOctetsCount",	REG_GMAC_RxetherStatsPkts1518toMAXOctetsCount/4,	REG_GMAC_RxetherStatsPkts1518toMAXOctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"GMAC_RxetherStatsOctetsCount",	 							REG_GMAC_RxetherStatsOctetsCount/4,							REG_GMAC_RxetherStatsOctetsCount,							WIDTHMASK(32),	OPL_NULL},
{"GMAC_RX_ETHL2MULTICAST_PKT_CNT",	 					REG_GMAC_RX_ETHL2MULTICAST_PKT_CNT/4,					REG_GMAC_RX_ETHL2MULTICAST_PKT_CNT,					WIDTHMASK(26),	OPL_NULL},

{"GMAC_RxetherRX_ERdropCount",	 							REG_GMAC_RxetherRX_ERdropCount/4,							REG_GMAC_RxetherRX_ERdropCount,							WIDTHMASK(16),	OPL_NULL},
{"GMAC_RxetherIFGdropCount",	 								REG_GMAC_RxetherIFGdropCount/4,									REG_GMAC_RxetherIFGdropCount,									WIDTHMASK(16),	OPL_NULL},

{"PMAC_TxetherStatsPktsCount",	 								REG_PMAC_TxetherStatsPktsCount/4,								REG_PMAC_TxetherStatsPktsCount,								WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsBroadcastPktsCount",	 				REG_PMAC_TxetherStatsBroadcastPktsCount/4,				REG_PMAC_TxetherStatsBroadcastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsMulticastPktsCount",	 				REG_PMAC_TxetherStatsMulticastPktsCount/4,				REG_PMAC_TxetherStatsMulticastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxaPAUSEMACCtrlFramesCount",	 				REG_PMAC_TxaPAUSEMACCtrlFramesCount/4,					REG_PMAC_TxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxaMACControlFramesCount",	 						REG_PMAC_TxaMACControlFramesCount/4,						REG_PMAC_TxaMACControlFramesCount,						WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsPkts64OctetsCount",	 				REG_PMAC_TxetherStatsPkts64OctetsCount/4,				REG_PMAC_TxetherStatsPkts64OctetsCount	,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsPkts65to127OctetsCount",	 		REG_PMAC_TxetherStatsPkts65to127OctetsCount/4,		REG_PMAC_TxetherStatsPkts65to127OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsPkts128to255OctetsCount",	 	REG_PMAC_TxetherStatsPkts128to255OctetsCount/4,		REG_PMAC_TxetherStatsPkts128to255OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxtherStatsPkts256to511OctetsCount",	 		REG_PMAC_TxtherStatsPkts256to511OctetsCount/4,		REG_PMAC_TxtherStatsPkts256to511OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsPkts512to1023OctetsCount",	 	REG_PMAC_TxetherStatsPkts512to1023OctetsCount/4,	REG_PMAC_TxetherStatsPkts512to1023OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsPkts1024to1518OctetsCount",	REG_PMAC_TxetherStatsPkts1024to1518OctetsCount/4,REG_PMAC_TxetherStatsPkts1024to1518OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"PMAC_TxetherStatsOctetsCount",	 							REG_PMAC_TxetherStatsOctetsCount/4,							REG_PMAC_TxetherStatsOctetsCount,							WIDTHMASK(32),	OPL_NULL},

{"PMAC_TxetherStatsPkts1519toMAXOctetsCount",	 	REG_PMAC_TxetherStatsPkts1519toMAXOctetsCount/4,	REG_PMAC_TxetherStatsPkts1519toMAXOctetsCount,	WIDTHMASK(26),	OPL_NULL},
{"PMAC_TX_OAM_CNT",	 												REG_PMAC_TX_OAM_CNT/4,												REG_PMAC_TX_OAM_CNT,												WIDTHMASK(16),	OPL_NULL},

{"PMAC_RxetherStatsGoodPktsCount",	 						REG_PMAC_RxetherStatsGoodPktsCount/4,						REG_PMAC_RxetherStatsGoodPktsCount,						WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsDropEventsCount",	 					REG_PMAC_RxetherStatsDropEventsCount/4,					REG_PMAC_RxetherStatsDropEventsCount,					WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsCRCAlignErrorsCount",	 			REG_PMAC_RxetherStatsCRCAlignErrorsCount/4,				REG_PMAC_RxetherStatsCRCAlignErrorsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsLenmisCount",	 							REG_PMAC_RxetherStatsLenmisCount/4,							REG_PMAC_RxetherStatsLenmisCount,							WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsShortCount",	 							REG_PMAC_RxetherStatsShortCount/4,								REG_PMAC_RxetherStatsShortCount,								WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsJabbersCount",	 						REG_PMAC_RxetherStatsJabbersCount/4,							REG_PMAC_RxetherStatsJabbersCount,							WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPktsCount",	 								REG_PMAC_RxetherStatsPktsCount/4,								REG_PMAC_RxetherStatsPktsCount,								WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsBroadcastPktsCount",	 				REG_PMAC_RxetherStatsBroadcastPktsCount/4,				REG_PMAC_RxetherStatsBroadcastPktsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsMulticastPktsCount",	 				REG_PMAC_RxetherStatsMulticastPktsCount	/4,				REG_PMAC_RxetherStatsMulticastPktsCount	,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxaPAUSEMACCtrlFramesCount",	 				REG_PMAC_RxaPAUSEMACCtrlFramesCount/4,					REG_PMAC_RxaPAUSEMACCtrlFramesCount,					WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxaUnsupportedOpcodesCount",	 					REG_PMAC_RxaUnsupportedOpcodesCount/4,					REG_PMAC_RxaUnsupportedOpcodesCount,					WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxaMACControlFramesCount",	 					REG_PMAC_RxaMACControlFramesCount/4,						REG_PMAC_RxaMACControlFramesCount,						WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsUndersizePktsCount",	 				REG_PMAC_RxetherStatsUndersizePktsCount/4,				REG_PMAC_RxetherStatsUndersizePktsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsOversizePktsCount",	 				REG_PMAC_RxetherStatsOversizePktsCount/4,				REG_PMAC_RxetherStatsOversizePktsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPkts64OctetsCount",	 				REG_PMAC_RxetherStatsPkts64OctetsCount/4,				REG_PMAC_RxetherStatsPkts64OctetsCount,				WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxtherStatsPkts65to127OctetsCount",	 		REG_PMAC_RxtherStatsPkts65to127OctetsCount/4,			REG_PMAC_RxtherStatsPkts65to127OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPkts128to255OctetsCount",	 	REG_PMAC_RxetherStatsPkts128to255OctetsCount/4,		REG_PMAC_RxetherStatsPkts128to255OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxtherStatsPkts256to511OctetsCount",	 		REG_PMAC_RxtherStatsPkts256to511OctetsCount/4,		REG_PMAC_RxtherStatsPkts256to511OctetsCount,		WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPkts512to1023OctetsCount",	 	REG_PMAC_RxetherStatsPkts512to1023OctetsCount/4,	REG_PMAC_RxetherStatsPkts512to1023OctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPkts1024to1518OctetsCount",	REG_PMAC_RxetherStatsPkts1024to1518OctetsCount/4,REG_PMAC_RxetherStatsPkts1024to1518OctetsCount,WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsPkts1518toMAXOctetsCount",	REG_PMAC_RxetherStatsPkts1518toMAXOctetsCount/4,	REG_PMAC_RxetherStatsPkts1518toMAXOctetsCount,	WIDTHMASK(32),	OPL_NULL},
{"PMAC_RxetherStatsOctetsCount",	 							REG_PMAC_RxetherStatsOctetsCount/4,							REG_PMAC_RxetherStatsOctetsCount,							WIDTHMASK(32),	OPL_NULL},
{"PMAC_FEC_CORRECTED_PKTCNT",	 							REG_PMAC_FEC_CORRECTED_PKTCNT/4,							REG_PMAC_FEC_CORRECTED_PKTCNT,							WIDTHMASK(32),	OPL_NULL},
{"PMAC_FEC_UNCORRECTED_PKTCNT",	 						REG_PMAC_FEC_UNCORRECTED_PKTCNT/4,						REG_PMAC_FEC_UNCORRECTED_PKTCNT,						WIDTHMASK(16),	OPL_NULL},
{"PMAC_FEC_NOERR_PKTCNT",	 									REG_PMAC_FEC_NOERR_PKTCNT/4,									REG_PMAC_FEC_NOERR_PKTCNT,									WIDTHMASK(16),	OPL_NULL},
{"PMAC_RX_ETHL2MULTICAST_PKT_CNT",	 					REG_PMAC_RX_ETHL2MULTICAST_PKT_CNT/4,					REG_PMAC_RX_ETHL2MULTICAST_PKT_CNT,					WIDTHMASK(26),	OPL_NULL},

{"PMAC_RX_OAM_CNT",	 												REG_PMAC_RX_OAM_CNT/4,												REG_PMAC_RX_OAM_CNT,												WIDTHMASK(16),	OPL_NULL},
{"PMAC_RxetherRX_ERdropCount",	 							REG_PMAC_RxetherRX_ERdropCount/4,							REG_PMAC_RxetherRX_ERdropCount,							WIDTHMASK(16),	OPL_NULL},
{"PMAC_RxetherIFGdropCount",	 								REG_RxetherIFGdropCount/4,											REG_RxetherIFGdropCount,											WIDTHMASK(16),	OPL_NULL},


{"REG_PMAC_SECU_LEN_DROP_CNT",	 						REG_PMAC_SECU_LEN_DROP_CNT/4,									REG_PMAC_SECU_LEN_DROP_CNT,									WIDTHMASK(16),	OPL_NULL},
{"PMAC_RScrc8errCount",	 											REG_RScrc8errCount/4,														REG_RScrc8errCount,														WIDTHMASK(16),	OPL_NULL},
{"PMAC_RSlliderrCount",	 											REG_RSlliderrCount/4,														REG_RSlliderrCount,														WIDTHMASK(16),	OPL_NULL},
{"REG_PMAC_EBR",	 											REG_PMAC_EBR/4,														REG_PMAC_EBR,														WIDTHMASK(32),	OPL_NULL},
{"REG_PMAC_CBR",	 											REG_PMAC_CBR/4,														REG_PMAC_CBR,														WIDTHMASK(32),	OPL_NULL},

{OPL_NULL,																		OPL_ZERO,																			OPL_ZERO,																		0X0000FFFF,OPL_NULL}
};

OPCONN_COUNTER_t tmCounter[] =
{
	{"TMUS_WRED_DROP",				REG_TMUS_WRED_DROP/4,					REG_TMUS_WRED_DROP,				WIDTHMASK(16),OPL_NULL},
	{"TMUS_ENQ_PKT_CNT",				REG_TMUS_ENQ_PKT_CNT/4,				REG_TMUS_ENQ_PKT_CNT,				WIDTHMASK(32),OPL_NULL},
	{"TMDS_WRED_DROP",				REG_TMDS_WRED_DROP/4,					REG_TMDS_WRED_DROP,				WIDTHMASK(16),OPL_NULL},
	{"TMDS_ENQ_PKT_CNT",				REG_TMDS_ENQ_PKT_CNT/4,				REG_TMDS_ENQ_PKT_CNT,				WIDTHMASK(32),OPL_NULL},

	{"TMUS_RXED_PKT_CNT",			REG_TMUS_RXED_PKT_CNT/4,			REG_TMUS_RXED_PKT_CNT,			WIDTHMASK(32),OPL_NULL},
	{"TMUS_CMDDRP_PKT_CNT",		REG_TMUS_CMDDRP_PKT_CNT/4,		REG_TMUS_CMDDRP_PKT_CNT,		WIDTHMASK(16),OPL_NULL},
	{"TMUS_QFULLDRP_PKT_CNT",	REG_TMUS_QFULLDRP_PKT_CNT/4,	REG_TMUS_QFULLDRP_PKT_CNT,	WIDTHMASK(16),OPL_NULL},
	{"TMUS_TXED_PKT_CNT",			REG_TMUS_TXED_PKT_CNT/4,			REG_TMUS_TXED_PKT_CNT,			WIDTHMASK(32),OPL_NULL},
	{"TMDS_RXED_PKT_CNT",			REG_TMDS_RXED_PKT_CNT/4,			REG_TMDS_RXED_PKT_CNT,			WIDTHMASK(32),OPL_NULL},
	{"TMDS_CMDDRP_PKT_CNT", 		REG_TMDS_CMDDRP_PKT_CNT/4,		REG_TMDS_CMDDRP_PKT_CNT,		WIDTHMASK(16),OPL_NULL},
	{"TMDS_QFULLDRP_PKT_CNT",	REG_TMDS_QFULLDRP_PKT_CNT/4,	REG_TMDS_QFULLDRP_PKT_CNT,	WIDTHMASK(16),OPL_NULL},
	{"TMDS_TXED_PKT_CNT",			REG_TMDS_TXED_PKT_CNT/4,			REG_TMDS_TXED_PKT_CNT,			WIDTHMASK(32),OPL_NULL},
	{OPL_NULL,									OPL_ZERO,											OPL_ZERO,										OPL_ZERO, OPL_NULL}
};

OPCONN_COUNTER_t brgCounter[] =
{
	{"BRG_CLE_RULE_MISS_COUNT",			REG_CLE_RULE_MISS_COUNT/4,			REG_CLE_RULE_MISS_COUNT,			WIDTHMASK(24), OPL_NULL},
	{"BRG_CLE_RULE_DROP_COUNT",			REG_CLE_RULE_DROP_COUNT/4,			REG_CLE_RULE_DROP_COUNT,			WIDTHMASK(24), OPL_NULL},
	{"REG_BRG_GE_RX_DROP_COUNT",		REG_BRG_GE_RX_DROP_COUNT/4,			REG_BRG_GE_RX_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_TX_DROP_COUNT",		REG_BRG_GE_TX_DROP_COUNT/4,			REG_BRG_GE_TX_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_VLAN_DROP_COUNT",		REG_BRG_GE_VLAN_DROP_COUNT/4,		REG_BRG_GE_VLAN_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_TAG_DROP_COUNT",		REG_BRG_GE_TAG_DROP_COUNT/4,		REG_BRG_GE_TAG_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_RX_DROP_COUNT",		REG_BRG_PON_RX_DROP_COUNT/4,		REG_BRG_PON_RX_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_TX_DROP_COUNT",		REG_BRG_PON_TX_DROP_COUNT/4,		REG_BRG_PON_TX_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_VLAN_DROP_COUNT",		REG_BRG_PON_VLAN_DROP_COUNT/4,		REG_BRG_PON_VLAN_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_TAG_DROP_COUNT",		REG_BRG_PON_TAG_DROP_COUNT/4,		REG_BRG_PON_TAG_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_RSV_DROP_COUNT",			REG_BRG_RSV_DROP_COUNT/4,			REG_BRG_RSV_DROP_COUNT,				WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_UC_SA_DROP_COUNT",		REG_BRG_GE_UC_SA_DROP_COUNT/4,		REG_BRG_GE_UC_SA_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_DMAC_DROP_COUNT",			REG_BRG_DMAC_DROP_COUNT/4,			REG_BRG_DMAC_DROP_COUNT,			WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_LEARN_DROP_COUNT",	REG_BRG_PON_LEARN_DROP_COUNT/4,		REG_BRG_PON_LEARN_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_LEARN_DROP_COUNT",		REG_BRG_GE_LEARN_DROP_COUNT/4,		REG_BRG_GE_LEARN_DROP_COUNT	,		WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_LEARN_CFL_DROP_COUNT",REG_BRG_PON_LEARN_CFL_DROP_COUNT/4, REG_BRG_PON_LEARN_CFL_DROP_COUNT,	WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_GE_LEARN_CFL_DROP_COUNT",	REG_BRG_GE_LEARN_CFL_DROP_COUNT/4,	REG_BRG_GE_LEARN_CFL_DROP_COUNT	,	WIDTHMASK(16), OPL_NULL},
	{"REG_BRG_PON_UC_SA_DROP_COUNT"	,	REG_BRG_PON_UC_SA_DROP_COUNT/4,		REG_BRG_PON_UC_SA_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
    {"REG_BRG_DS_0_MAC_DROP_COUNT"	,	REG_BRG_DS_0_MAC_DROP_COUNT/4,		REG_BRG_DS_0_MAC_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
    {"REG_BRG_US_0_MAC_DROP_COUNT"	,	REG_BRG_US_0_MAC_DROP_COUNT/4,		REG_BRG_US_0_MAC_DROP_COUNT,		WIDTHMASK(16), OPL_NULL},
    {OPL_NULL,									OPL_ZERO,											OPL_ZERO,	WIDTHMASK(16),OPL_NULL}
};

OPCONN_COUNTER_t ponMpcpCounter[] =
{
	{"MPCP_NORM_GATE_CNT",   		REG_MPCP_NORM_GATE_CNT/4,			REG_MPCP_NORM_GATE_CNT,		WIDTHMASK(32),OPL_NULL},
	{"MPCP_DISC_PKT_CNT",   			REG_MPCP_DISC_PKT_CNT/4,				REG_MPCP_DISC_PKT_CNT,			WIDTHMASK(16),OPL_NULL},
	{"MPCP_PKT_DROP_CNT", 			REG_MPCP_PKT_DROP_CNT/4,			REG_MPCP_PKT_DROP_CNT,			WIDTHMASK(16),OPL_NULL},
	{"MPCP_CLE_PKT_CNT",  			REG_MPCP_CLE_PKT_CNT/4,				REG_MPCP_CLE_PKT_CNT,				WIDTHMASK(32),OPL_NULL},
	{"MPCP_CLE_DROP_CNT",   		REG_MPCP_CLE_DROP_CNT/4,			REG_MPCP_CLE_DROP_CNT,			WIDTHMASK(16),OPL_NULL},
	{"MPCP_DISCV_GATE_CNT",		REG_MPCP_DISCV_GATE_CNT/4,		REG_MPCP_DISCV_GATE_CNT,		WIDTHMASK(16),OPL_NULL},


	{"GRANT_CNT",    						REG_MPCP_GRANT_CNT/4,					REG_MPCP_GRANT_CNT,					WIDTHMASK(32),OPL_NULL},
	{"GATE_DROP_CNT",    				REG_GATE_DROP_CNT/4,					REG_GATE_DROP_CNT,					WIDTHMASK(16),OPL_NULL},
	{"GRANT_RTP_PKT_CNT",    		REG_GRANT_RTP_PKT_CNT/4,				REG_GRANT_RTP_PKT_CNT,				WIDTHMASK(32),OPL_NULL},
	{"GRANT_RPT_DROP_CNT",    		REG_GRANT_RPT_DROP_CNT/4,			REG_GRANT_RPT_DROP_CNT,			WIDTHMASK(16),OPL_NULL},

	{"GRANT_TOTAL_LEN",    			REG_GRANT_TOTAL_LEN/4,					REG_GRANT_TOTAL_LEN,					WIDTHMASK(32),OPL_NULL},
	{"MPCP_DRIFT_DROP_CNT",    		REG_MPCP_DRIFT_DROP_CNT/4,				REG_MPCP_DRIFT_DROP_CNT,				WIDTHMASK(16),OPL_NULL},
	{"GRANT_TOTAL_LEN_IN_SEC",  	REG_GRANT_TOTAL_LEN_IN_SEC/4,			REG_GRANT_TOTAL_LEN_IN_SEC,				WIDTHMASK(32),OPL_NULL},

	{"US_NORMPKT_TX_CNT",   		REG_US_NORMPKT_TX_CNT/4,			REG_US_NORMPKT_TX_CNT,			WIDTHMASK(16),OPL_NULL},
	{"US_REGREQ_TX_CNT",   			REG_US_REGREQ_TX_CNT/4,				REG_US_REGREQ_TX_CNT,				WIDTHMASK(16),OPL_NULL},
	{"US_REGACK_TX_CNT", 			REG_US_REGACK_TX_CNT/4,				REG_US_REGACK_TX_CNT,				WIDTHMASK(16),OPL_NULL},
	{"US_RPTPKT_TX_CNT",				REG_US_RPTPKT_TX_CNT/4,				REG_US_RPTPKT_TX_CNT,				WIDTHMASK(32),OPL_NULL},
	{"MPCP_SOP_DROP_CNT",			REG_MPCP_SOP_DROP_CNT/4,			REG_MPCP_SOP_DROP_CNT,			WIDTHMASK(16),OPL_NULL},
	{"MPCP_CLE_FULL_DROP_CNT",	REG_MPCP_CLE_FULL_DROP_CNT/4,	REG_MPCP_CLE_FULL_DROP_CNT,	WIDTHMASK(16),OPL_NULL},
	{OPL_NULL,									OPL_ZERO,											OPL_ZERO,										WIDTHMASK(16),OPL_NULL},

};

OPCONN_COUNTER_t hostDmaCounter[] =
{
	{"REG_DMA0_Q0_CELL_CNT", REG_DMA0_Q0_CELL_CNT/4		 , REG_DMA0_Q0_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q1_CELL_CNT", REG_DMA0_Q1_CELL_CNT/4		 , REG_DMA0_Q1_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q2_CELL_CNT", REG_DMA0_Q2_CELL_CNT/4		 , REG_DMA0_Q2_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q3_CELL_CNT", REG_DMA0_Q3_CELL_CNT/4		 , REG_DMA0_Q3_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q4_CELL_CNT", REG_DMA0_Q4_CELL_CNT/4		 , REG_DMA0_Q4_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q5_CELL_CNT", REG_DMA0_Q5_CELL_CNT/4		 , REG_DMA0_Q5_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q6_CELL_CNT", REG_DMA0_Q6_CELL_CNT/4		 , REG_DMA0_Q6_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q7_CELL_CNT", REG_DMA0_Q7_CELL_CNT/4		 , REG_DMA0_Q7_CELL_CNT		,       WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q0_PKT_CNT", REG_DMA0_Q0_PKT_CNT/4	     , REG_DMA0_Q0_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q1_PKT_CNT", REG_DMA0_Q1_PKT_CNT/4	     , REG_DMA0_Q1_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q2_PKT_CNT", REG_DMA0_Q2_PKT_CNT/4	     , REG_DMA0_Q2_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q3_PKT_CNT", REG_DMA0_Q3_PKT_CNT/4	     , REG_DMA0_Q3_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q4_PKT_CNT", REG_DMA0_Q4_PKT_CNT/4	     , REG_DMA0_Q4_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q5_PKT_CNT", REG_DMA0_Q5_PKT_CNT/4	     , REG_DMA0_Q5_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q6_PKT_CNT", REG_DMA0_Q6_PKT_CNT/4	     , REG_DMA0_Q6_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_Q7_PKT_CNT", REG_DMA0_Q7_PKT_CNT/4	     , REG_DMA0_Q7_PKT_CNT		    ,   WIDTHMASK(13),		OPL_NULL},
    {"REG_DMA0_RX_CNT", REG_DMA0_RX_CNT/4	         , REG_DMA0_RX_CNT			    ,   WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_TX_CNT", REG_DMA0_TX_CNT/4	         , REG_DMA0_TX_CNT			    ,   WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_GE2CPU_ENQ_CNT", REG_DMA0_GE2CPU_ENQ_CNT/4	 , REG_DMA0_GE2CPU_ENQ_CNT		,   WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_GE2CPU_POLIC_DRPCNT", REG_DMA0_GE2CPU_POLIC_DRPCNT/4	    ,REG_DMA0_GE2CPU_POLIC_DRPCNT	,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_GE2CPU_QFULL_DRPCNT", REG_DMA0_GE2CPU_QFULL_DRPCNT/4	    ,REG_DMA0_GE2CPU_QFULL_DRPCNT	,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_PON2CPU_ENQ_CNT", REG_DMA0_PON2CPU_ENQ_CNT/4		    ,REG_DMA0_PON2CPU_ENQ_CNT		,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_PON2CPU_POLIC_DRPCNT", REG_DMA0_PON2CPU_POLIC_DRPCNT/4	    ,REG_DMA0_PON2CPU_POLIC_DRPCNT  ,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_PON2CPU_QFULL_DRPCNT", REG_DMA0_PON2CPU_QFULL_DRPCNT/4	    ,REG_DMA0_PON2CPU_QFULL_DRPCNT  ,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_DEQ_CNT", REG_DMA0_DEQ_CNT/4                  ,REG_DMA0_DEQ_CNT               ,WIDTHMASK(32),		OPL_NULL},
    {"REG_GE_FIFO_PPD_DRPCNT", REG_GE_FIFO_POST_PKT_ERR_CNT/4      ,REG_GE_FIFO_POST_PKT_ERR_CNT   ,WIDTHMASK(32),		OPL_NULL},
    {"REG_PON_FIFO_PPD_DRPCNT", REG_PON_FIFO_POST_PKT_ERR_CNT/4     ,REG_PON_FIFO_POST_PKT_ERR_CNT  ,WIDTHMASK(32),		OPL_NULL},
    {"REG_GE_FIFO_EPD_DRPCNT", REG_GE_FIFO_EARLING_PKT_DRPCNT/4    ,REG_GE_FIFO_EARLING_PKT_DRPCNT ,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_EnqueueLengthErrCnt", REG_DMA0_EnqueueLengthErrCnt/4   ,REG_DMA0_EnqueueLengthErrCnt,WIDTHMASK(32),		OPL_NULL},
    {"REG_DMA0_EnqueueCRCErrCnt", REG_DMA0_EnqueueCRCErrCnt/4   ,REG_DMA0_EnqueueCRCErrCnt,WIDTHMASK(32),		OPL_NULL},
	{OPL_NULL,							OPL_ZERO,		  				    OPL_ZERO,        	 			 WIDTHMASK(32),  	OPL_NULL}
};

BRG_SOFT_LEARN_FUNC g_pfBrgSoftLearnFunc = NULL;

#if 0
OPL_STATUS brgGeParserCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	OPL_DRV_SHOW_PRINTF(("%-28s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgGeParserCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgGeParserCounter[index].name;
		regId 		= brgGeParserCounter[index].regId;
		regAddr 	= brgGeParserCounter[index].regAddr;
		regMask 	= brgGeParserCounter[index].regMask;
		desc 		= brgGeParserCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		OPL_DRV_SHOW_PRINTF(("%-28s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS brgPortCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	OPL_DRV_SHOW_PRINTF(("%-43s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgPortCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgPortCounter[index].name;
		regId 		= brgPortCounter[index].regId;
		regAddr 	= brgPortCounter[index].regAddr;
		regMask 	= brgPortCounter[index].regMask;
		desc 		= brgPortCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		OPL_DRV_SHOW_PRINTF(("%-43s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS brgCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	OPL_DRV_SHOW_PRINTF(("%-32s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgCounter[index].name;
		regId 		= brgCounter[index].regId;
		regAddr 	= brgCounter[index].regAddr;
		regMask 	= brgCounter[index].regMask;
		desc 		= brgCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS tmCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	OPL_DRV_SHOW_PRINTF(("%-21s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; tmCounter[index].name != OPL_NULL;index++)
	{
		name 		= tmCounter[index].name;
		regId 		= tmCounter[index].regId;
		regAddr 	= tmCounter[index].regAddr;
		regMask 	= tmCounter[index].regMask;
		desc 		= tmCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		OPL_DRV_SHOW_PRINTF(("%-21s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS ponMpcpCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;
	UINT8 	*dropType[6] = {	"greater than MPCP_GATE_THR_H"
											,"smaller than MPCP_GATE_THR_L"
											,"grant_len too smaller than TAIL_GUARD"
											,"discovery grant drop after onu_registered"
											,"grant list full drop"
											,OPL_NULL};
	UINT8 dropTypeVal = OPL_ZERO;


	OPL_DRV_SHOW_PRINTF(("%-26s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; ponMpcpCounter[index].name != OPL_NULL;index++)
	{
		name 		= ponMpcpCounter[index].name;
		regId 		= ponMpcpCounter[index].regId;
		regAddr 	= ponMpcpCounter[index].regAddr;
		regMask 	= ponMpcpCounter[index].regMask;
		desc 		= ponMpcpCounter[index].desc;

		if(regAddr == REG_GATE_DROP_CNT)
		{
			continue;
		}

		oplRegRead(regAddr, &regVal);

		OPL_DRV_SHOW_PRINTF(("%-26s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
	}

	oplRegRead(REG_GATE_DROP_CNT, &regVal);
	OPL_DRV_SHOW_PRINTF(("%-26s 0x%05x 0x%08x   %-12u\n","GATE_DROP_CNT",REG_GATE_DROP_CNT/4,regVal&0xffff,regVal&0xffff));

	dropTypeVal = 0x1f&(regVal>>24);

	for(index = 0; index <=4; index++)
	{
		if(dropTypeVal)
		{
			if((1>>index )&dropTypeVal)
			{
				OPL_DRV_SHOW_PRINTF(("%s\n",dropType[index]));
			}
		}
	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS hostDmaCounterShow(void)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	OPL_DRV_SHOW_PRINTF(("%-32s %-7s %-12s %-12s %s\n","name","regId","regVal","regVal","desc"));
	for(index = 0; hostDmaCounter[index].name != OPL_NULL;index++)
	{
		name 		= hostDmaCounter[index].name;
		regId 		= hostDmaCounter[index].regId;
		regAddr 	= hostDmaCounter[index].regAddr;
		regMask 	= hostDmaCounter[index].regMask;
		desc 		= hostDmaCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		if(REG_DMA0_EnqueueLengthErrCnt/4 == hostDmaCounter[index].regId)
		{
			OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n","GE2CPU_ENQ_LEN_ERR_CNT",regId,(regVal>>16)&0XFFFF,(regVal>>16)&0XFFFF));
			OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n","PON2CPU_ENQ_LEN_ERR_CNT",regId,regVal&0XFFFF,regVal&0xFFFF));
		}else if(REG_DMA0_EnqueueCRCErrCnt/4 == hostDmaCounter[index].regId)
		{
			OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n","GE2CPU_ENQ_CRC_ERR_CNT",regId,(regVal>>16)&0XFFFF,(regVal>>16)&0XFFFF));
			OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n","PON2CPU_ENQ_CRC_ERR_CNT",regId,regVal&0XFFFF,regVal&0xFFFF));
		}else
		{
			OPL_DRV_SHOW_PRINTF(("%-32s 0x%05x 0x%08x   %-12u\n",name,regId,regVal&regMask,regVal&regMask));
		}

	}
	OPL_DRV_SHOW_PRINTF(("----------------------------------------------------------------\n"));
	return OPL_OK;
}

OPL_STATUS allCounterShow(void)
{
	brgGeParserCounterShow();

	brgPortCounterShow();

	brgCounterShow();

	tmCounterShow();

	tmInQueueCounterShow();

	hostDmaCounterShow();

	ponMpcpCounterShow();

	return OPL_OK;
}

#endif

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
OPL_STATUS brgUsrDefineRsvMacHwWrite(UINT8 num,UINT8 *mac)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 common1;
	UINT32 common2;

	if(num > 1 || OPL_NULL == mac)
	{
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("%02x:%02x:%02x:%02x:%02x:%02x\n",
	mac[0],
	mac[1],
	mac[2],
	mac[3],
	mac[4],
	mac[5]));
#endif

	common1 = (mac[2]<<24)|(mac[3]<<16)|(mac[4]<<8)|(mac[5]);
	common2 = (mac[0]<<8)|(mac[1]);
#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("common1 = %08x\n",common1));
	OPL_DRV_PRINTF(("common2 = %08x\n",common2));
#endif
	switch(num)
	{
		case 0:
			retVal = oplRegWrite(REG_BRG_USER_RSV_MAC0_0,common1);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			retVal = oplRegWrite(REG_BRG_USER_RSV_MAC0_1,common2);
			break;
		case 1:
			retVal = oplRegWrite(REG_BRG_USER_RSV_MAC1_0,common1);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			retVal = oplRegWrite(REG_BRG_USER_RSV_MAC1_1,common2);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
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
OPL_STATUS brgUsrDefineRsvMacHwRead(UINT8 num,UINT8 *mac)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 common1;
	UINT32 common2;

	if(num > 1 || OPL_NULL == mac)
	{
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(num)
	{
		case 0:
			retVal = oplRegRead(REG_BRG_USER_RSV_MAC0_0,&common1);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			retVal = oplRegRead(REG_BRG_USER_RSV_MAC0_1,&common2);
			break;
		case 1:
			retVal = oplRegRead(REG_BRG_USER_RSV_MAC1_0,&common1);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			retVal = oplRegRead(REG_BRG_USER_RSV_MAC1_1,&common2);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	if(OPL_OK == retVal)
	{
		mac[0] = (common2&0x0000ff00)>>8;
		mac[1] = (common2&0x000000ff);

		mac[2] = (common1&0xff000000)>>24;
		mac[3] = (common1&0x00ff0000)>>16;
		mac[4] = (common1&0x0000ff00)>>8;
		mac[5] = (common1&0x000000ff);
	}
	return retVal;
}

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
OPL_STATUS brgUsrDefineRsvMacCtrlHwWrite(UINT8 macNum,UINT8 icos,UINT8 macCtl)
{
	OPL_STATUS retVal = OPL_OK;
	RSV_MAC_CONTROL_t RegMacCtl;
	UINT32 regVal;

	if(macNum > 1)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	RegMacCtl.bfIcos = icos;
	RegMacCtl.bfRsvCtl = macCtl;
	regVal = *((UINT8 *)&RegMacCtl);

	switch(macNum)
	{
		case 0:
			retVal = oplRegFieldWrite(REG_BRG_USER_RSV_MAC_CTRL, 0, 8, regVal);
			break;
		case 1:
			retVal = oplRegFieldWrite(REG_BRG_USER_RSV_MAC_CTRL, 8, 8, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
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
OPL_STATUS brgUsrDefineRsvMacCtrlHwRead(UINT8 macNum,UINT8 *icos,UINT8 *macCtl)
{
	OPL_STATUS retVal = OPL_OK;
	RSV_MAC_CONTROL_t RegMacCtl;
	UINT32 regVal;

	if(macNum > 1 || OPL_NULL == icos || OPL_NULL == macCtl)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(macNum)
	{
		case 0:
			retVal = oplRegFieldRead(REG_BRG_USER_RSV_MAC_CTRL, 0, 8, &regVal);
			break;
		case 1:
			retVal = oplRegFieldRead(REG_BRG_USER_RSV_MAC_CTRL, 8, 8, &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	if(OPL_OK == retVal)
	{
		*((UINT8 *)&RegMacCtl) = regVal;
		*icos = RegMacCtl.bfIcos;
		*macCtl = RegMacCtl.bfRsvCtl;
	}
	return retVal;
}
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
OPL_STATUS brgRsvMacCtrlHwWrite(UINT8 macNum,UINT8 icos,UINT8 macCtl)
{
	OPL_STATUS retVal = OPL_OK;
	RSV_MAC_CONTROL_t regRsvMacCtl;
	UINT32 regAddr;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(macNum >= BRG_RSV_MAC_NUM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	regRsvMacCtl.bfIcos = icos;
	regRsvMacCtl.bfRsvCtl = macCtl;

	regAddr = REG_BRG_RSV_MAC_CTRL0 + (macNum/4)*4;
	fieldOffset = (macNum%4)*8;
	fieldWidth =  5;
	regVal = *((UINT8 *)&regRsvMacCtl);
#ifdef BRG_MAC_DEBUG
	OPL_DRV_PRINTF(("regVal = 0x%02x\n",regVal));
#endif
	retVal = oplRegFieldWrite(regAddr, fieldOffset, fieldWidth, regVal);
	return retVal;
}
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
OPL_STATUS brgRsvMacCtrlHwRead(UINT8 macNum,UINT8 *icos,UINT8 *macCtl)
{
	OPL_STATUS retVal = OPL_OK;
	RSV_MAC_CONTROL_t regRsvMacCtl;
	UINT32 regAddr;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(macNum >= BRG_RSV_MAC_NUM ||OPL_NULL == icos || OPL_NULL == macCtl)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	regAddr = REG_BRG_RSV_MAC_CTRL0 + (macNum/4)*4;
	fieldOffset = (macNum%4)*8;
	fieldWidth =  5;

	retVal = oplRegFieldRead(regAddr, fieldOffset, fieldWidth, &regVal);
#ifdef BRG_MAC_DEBUG
	OPL_DRV_PRINTF(("regVal = 0x%02x\n",regVal));
#endif
	if(OPL_OK == retVal)
	{
		*((UINT8 *)&regRsvMacCtl) = regVal;
		*icos = regRsvMacCtl.bfIcos;
		*macCtl = regRsvMacCtl.bfRsvCtl;
	}

	return retVal;
}
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
OPL_STATUS brgArlAgeCtrlHwWrite(UINT8 type,UINT32 value)
{
	OPL_STATUS retVal = OPL_OK;

	if(BRG_ARL_CTL_END <= type)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	switch(type)
	{
		case BRG_ARL_SOFTLERN_EN:
			retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 31, 1, value);
			break;
		case BRG_ARL_ISOLATION:
			retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 30, 1, value);
			break;
		case BRG_ARL_FULL_DROP:
			retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 29, 1, value);
			break;
		case BRG_ARL_AGE_EN:
			retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 20, 1, value);
			break;
		case BRG_ARL_AGE_VAL:
			retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 0, 20, value);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
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
OPL_STATUS brgArlAgeCtrlHwRead(UINT8 type,UINT32 *value)
{
	OPL_STATUS retVal = OPL_OK;

	if(BRG_ARL_CTL_END <= type)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	if(OPL_NULL == value)
	{
		return OPL_ERR_NULL_POINTER;
	}
	switch(type)
	{
		case BRG_ARL_SOFTLERN_EN:
			retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 31, 1, value);
			break;
		case BRG_ARL_ISOLATION:
			retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 30, 1, value);
			break;
		case BRG_ARL_FULL_DROP:
			retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 29, 1, value);
			break;
		case BRG_ARL_AGE_EN:
			retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 20, 1, value);
			break;
		case BRG_ARL_AGE_VAL:
			retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 0, 20, value);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
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
OPL_STATUS brgArlAgeCntInSecHwWrite(UINT32 value)
{
	return oplRegFieldWrite(REG_BRG_AGE_IN_SEC, 0, 28, value);
}
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
OPL_STATUS brgArlAgeCntInSecHwRead(UINT32 *value)
{
	if(OPL_NULL == value)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	return oplRegFieldRead(REG_BRG_AGE_IN_SEC, 0, 28, value);
}
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
OPL_STATUS brgArlEntryHwRead(UINT32 entryId,PON_BRG_MAC_TAB_t *pstEntry)
{
	if(entryId < 0 || entryId > PON_BRG_MAC_ENTRY_NUM || OPL_NULL == pstEntry)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	return oplTabRead( OPCONN_TAB_PON_MAC,  entryId,  1, (UINT32 *)pstEntry);
}
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
OPL_STATUS brgArlEntryHwWrite(UINT32 entryId,PON_BRG_MAC_TAB_t *pstEntry)
{
	if(entryId < 0 || entryId > PON_BRG_MAC_ENTRY_NUM || OPL_NULL == pstEntry)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	return oplTabWrite( OPCONN_TAB_PON_MAC,  entryId,  1, (UINT32 *)pstEntry);
}
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
OPL_STATUS brgArlMacExist(PON_BRG_MAC_TAB_t *pstEntry, UINT32 *entryId,UINT8 *found)
{
	OPL_STATUS retVal;
	PON_BRG_MAC_TAB_t mac_entry;
	PON_BRG_MAC_TAB_COMMON_t *pstEntryTmp1;
	PON_BRG_MAC_TAB_COMMON_t *pstEntryTmp2;
	UINT32 index;

	*found = OPL_NOT_FIND;
	pstEntryTmp1 = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;
	pstEntryTmp2 = (PON_BRG_MAC_TAB_COMMON_t *)pstEntry;
	for(index = OPL_ZERO;index < PON_BRG_MAC_ENTRY_NUM; index++)
	{
		retVal = brgArlEntryHwRead(index,&mac_entry);
		if(retVal != OPL_OK)
		{
			return retVal;
		}
#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("%08x %08x\n",*(UINT32 *)(&mac_entry),*((UINT32 *)(&mac_entry) + 1)));
	OPL_DRV_PRINTF(("%08x %08x\n",*(UINT32 *)(pstEntry),*((UINT32 *)(pstEntry) + 1)));
#endif
		if((!OPL_MEMCMP(&(pstEntryTmp1->common.common[2]),&(pstEntryTmp2->common.common[2]),MAC_LENGTH))
			&&(pstEntryTmp1->common.arl_tab.vlanId == pstEntryTmp2->common.arl_tab.vlanId))
		{
			*entryId = index;
			OPL_MEMCPY(pstEntryTmp2,pstEntryTmp1,sizeof(PON_BRG_MAC_TAB_COMMON_t));
			pstEntryTmp2->common.arl_tab.vlanId = pstEntryTmp1->common.arl_tab.vlanId;
            pstEntryTmp2->common.arl_tab.portId = pstEntryTmp1->common.arl_tab.portId;
			*found = OPL_FIND;
			break;
		}
	}
	return retVal;
}
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
OPL_STATUS brgArlFreeEntryFind(UINT32 *entryId,UINT8 *found)
{
	OPL_STATUS retVal;
	PON_BRG_MAC_TAB_t mac_entry;
	UINT32 index;

	*found = OPL_NOT_FIND;
	for(index = OPL_ZERO;index < PON_BRG_MAC_ENTRY_NUM; index++)
	{
		retVal = brgArlEntryHwRead(index,&mac_entry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}

		if(mac_entry.state == NOT_VALID ||mac_entry.state == NOT_VALID_STATIC)
		{
			*found = OPL_FIND;
			*entryId = index;
			break;
		}
	}
	return retVal;
}

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
OPL_STATUS brgArlMacAdd(UINT16 portId,UINT16 vlanId,UINT8 *mac)
{
	OPL_STATUS retVal;
	PON_BRG_MAC_TAB_t mac_entry;
	PON_BRG_MAC_TAB_COMMON_t *pstEntry;
	UINT32 entryId;
	UINT8 find = OPL_NOT_FIND;

	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	//if(vlanId > MAX_NUM_OF_VLAN || vlanId == OPL_ZERO)
	if(vlanId > MAX_NUM_OF_VLAN)
	{
		OPL_DRV_PRINTF(("vlanid out of range 1 - 4095\n"));
		return OPL_ERROR;
	}
	pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;

	OPL_MEMCPY(&(pstEntry->common.common[2]),mac,MAC_LENGTH);
	pstEntry->common.arl_tab.vlanId = vlanId;

	retVal = brgArlMacExist(&mac_entry,&entryId, &find);
	if(OPL_OK != retVal)
	{
		OPL_TRACE();
	}
#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("find = %d\n",find));
#endif
	if(OPL_FIND == find)
	{
		if(mac_entry.state != VALID_STATIC)
		{
			mac_entry.age = NOT_AGE_OUT;
			mac_entry.state = VALID_STATIC;
            if(mac_entry.portId != portId)
            {
                mac_entry.portId = portId;
            }
		}
		retVal = brgArlEntryHwWrite( entryId, &mac_entry);
	}else
	{
		retVal = brgArlFreeEntryFind(&entryId,&find);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		if(OPL_NOT_FIND == find)
		{
			return OPL_BRIDGE_ARL_TABLE_FULL;
		}
		pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;

		mac_entry.age = NOT_AGE_OUT;
		mac_entry.state = VALID_STATIC;
        mac_entry.portId = portId;
		pstEntry->common.arl_tab.vlanId = vlanId;
		OPL_MEMCPY(&(pstEntry->common.common[2]),mac,MAC_LENGTH);
		retVal = brgArlEntryHwWrite( entryId, &mac_entry);
	}

	return retVal;
}


OPL_STATUS brgArlDynamicMacAdd(UINT16 portId,UINT16 vlanId,UINT8 *mac)
{
	OPL_STATUS retVal;
	PON_BRG_MAC_TAB_t mac_entry;
	PON_BRG_MAC_TAB_COMMON_t *pstEntry;
	UINT32 entryId;
	UINT8 find = OPL_NOT_FIND;

	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	//if(vlanId > MAX_NUM_OF_VLAN || vlanId == OPL_ZERO)
	if(vlanId > MAX_NUM_OF_VLAN)
	{
		OPL_DRV_PRINTF(("vlanid out of range 1 - 4095\n"));
		return OPL_ERROR;
	}
	pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;

	OPL_MEMCPY(&(pstEntry->common.common[2]),mac,MAC_LENGTH);
	pstEntry->common.arl_tab.vlanId = vlanId;

	retVal = brgArlMacExist(&mac_entry,&entryId, &find);
	if(OPL_OK != retVal)
	{
		OPL_TRACE();
	}
#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("find = %d\n",find));
#endif
	if(OPL_FIND == find)
	{
		if(mac_entry.state == VALID_HARDWARE_LEARN)
		{
			mac_entry.age = AGE_OUT;
            if(mac_entry.portId != portId)
            {
                mac_entry.portId = portId;
            }
			retVal = brgArlEntryHwWrite( entryId, &mac_entry);
		}
	}else
	{
		retVal = brgArlFreeEntryFind(&entryId,&find);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		if(OPL_NOT_FIND == find)
		{
			return OPL_BRIDGE_ARL_TABLE_FULL;
		}
		pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;

		mac_entry.age = AGE_OUT;
		mac_entry.state = VALID_HARDWARE_LEARN;
        mac_entry.portId = portId;
		pstEntry->common.arl_tab.vlanId = vlanId;
		OPL_MEMCPY(&(pstEntry->common.common[2]),mac,MAC_LENGTH);
		retVal = brgArlEntryHwWrite( entryId, &mac_entry);
	}

	return retVal;
}


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
OPL_STATUS brgArlMacDel(UINT16 portId,UINT16 vlanId,UINT8 *mac)
{
	OPL_STATUS retVal;
	PON_BRG_MAC_TAB_t mac_entry;
	PON_BRG_MAC_TAB_COMMON_t *pstEntry;
	UINT32 entryId;
	UINT8 find = OPL_NOT_FIND;

	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(vlanId > MAX_NUM_OF_VLAN || vlanId == OPL_ZERO)
	{
		OPL_DRV_PRINTF(("vlanid out of range 1 - 4095\n"));
		return OPL_ERROR;
	}

	OPL_MEMSET(&mac_entry,0X00,sizeof(PON_BRG_MAC_TAB_t));
	pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;

	OPL_MEMCPY(&(pstEntry->common.common[2]),mac,MAC_LENGTH);
	pstEntry->common.arl_tab.vlanId = vlanId;

	retVal = brgArlMacExist(&mac_entry,&entryId, &find);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    if(mac_entry.portId != portId)
    {
        OPL_DRV_PRINTF(("No such mac entry.\n"));
        return OPL_OK;
    }

	OPL_MEMSET(&mac_entry,0X00,sizeof(PON_BRG_MAC_TAB_t));

	if(OPL_FIND == find)
	{
		retVal = brgArlEntryHwWrite( entryId, &mac_entry);
	}
	return retVal;
}
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
OPL_STATUS brgArlInit(void)
{
	PON_BRG_MAC_TAB_t brg_mac_entry;
	UINT32 entryId;
	OPL_STATUS retVal;

	OPL_MEMSET(&brg_mac_entry,0X00,sizeof(PON_BRG_MAC_TAB_t));
	for(entryId = OPL_ZERO;entryId < PON_BRG_MAC_ENTRY_NUM;entryId++)
	{
		retVal = brgArlEntryHwWrite(entryId,&brg_mac_entry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	
	psoftLearnMacEntry = NULL;
	return retVal;
}
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
OPL_STATUS brgArlMacEntryShow(UINT32 startEntryId,UINT32 endEntryId)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 entryId;
	PON_BRG_MAC_TAB_t mac_entry;
	UINT8 mac[MAC_LENGTH];
	UINT16 vlanId;
    UINT16 portId;
	PON_BRG_MAC_TAB_COMMON_t *pstEntry;
	UINT8 *statusString[] = {"ageOut","aging"};

	if(startEntryId < 0 || endEntryId >= PON_BRG_MAC_ENTRY_NUM)
	{
		OPL_DRV_SHOW_PRINTF(("out of arl table range.\n"));
		return OPL_ERROR;
	}

	if(endEntryId < startEntryId)
	{
		OPL_DRV_SHOW_PRINTF(("endEntryId should be less than startEntryId.\n"));
		return OPL_ERROR;
	}
	pstEntry = (PON_BRG_MAC_TAB_COMMON_t *)&mac_entry;
	OPL_DRV_SHOW_PRINTF(("%-10s %-10s %-10s %-10s %-10s %-18s\n","entryId","state","vlanId","portId","ageBit","macAddress"));
	for(entryId = startEntryId; entryId <= endEntryId;entryId++)
	{
		retVal = brgArlEntryHwRead( entryId, &mac_entry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		OPL_DRV_SHOW_PRINTF(("%-10d",entryId));
		OPL_MEMCPY(mac,&(pstEntry->common.common[2]),MAC_LENGTH);
		vlanId = pstEntry->common.arl_tab.vlanId;
        portId = pstEntry->common.arl_tab.portId;
		switch(mac_entry.state)
		{
			case NOT_VALID:
				OPL_DRV_SHOW_PRINTF((" %-10s","empty"));
				break;
			case NOT_VALID_STATIC:
				OPL_DRV_SHOW_PRINTF((" %-10s","occpuied"));
				break;
			case VALID_HARDWARE_LEARN:
				OPL_DRV_SHOW_PRINTF((" %-10s","hardLearn"));
				break;
			case VALID_STATIC:
				OPL_DRV_SHOW_PRINTF((" %-10s","softLearn"));
				break;
			default:
				OPL_DRV_SHOW_PRINTF(("no such type.\n"));
				break;
		}
		OPL_DRV_SHOW_PRINTF((" %-10d",vlanId));
        OPL_DRV_SHOW_PRINTF((" %-10d",portId));
		OPL_DRV_SHOW_PRINTF((" %-10s",statusString[mac_entry.age]));
		OPL_DRV_SHOW_PRINTF((" %02x:%02x:%02x:%02x:%02x:%02x\n",
				mac[0],
				mac[1],
				mac[2],
				mac[3],
				mac[4],
				mac[5]));
	}
	return retVal;
}
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
OPL_STATUS brgArlMacAddTest(UINT16 portId,UINT16 vlanId,UINT8 *string)
{
	UINT8 mac[MAC_LENGTH];
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == string)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = string2mac(string, mac);

	retVal = brgArlMacAdd(portId,vlanId,mac);

	return retVal;
}
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
OPL_STATUS brgArlMacDelTest(UINT16 portId,UINT16 vlanId,UINT8 *string)
{
	UINT8 mac[MAC_LENGTH];
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == string)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = string2mac(string, mac);

	retVal = brgArlMacDel(portId,vlanId,mac);

	return retVal;
}
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
OPL_STATUS brgRsvMacCtrlInit(void)
{
	OPL_STATUS retVal;
	UINT32 regAddr ;

	for(regAddr = 0x3034; regAddr <= 0x303c; regAddr++)
	{
		if(regAddr == 0x3034)
		{
			/* modified for BPDU */
            /*This register has been set in func ResevMacInit.*/
            retVal = OPL_OK;
            #if 0
            retVal = oplRegWrite(regAddr*4,0X100F0F09);
            #endif
		}else if(regAddr == 0x303c)
		{
			retVal = oplRegWrite(regAddr*4,0x10);
		}else
		{
			retVal = oplRegWrite(regAddr*4,0X10101010);
		}

		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	return	retVal;
}


/*******************************************************************************

* brgZeroMacLimitInit
*
* DESCRIPTION:
*
*  this function is used to limit zero mac learning
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgZeroMacDropInit(void)
{
	OPL_STATUS retVal;	
	
    retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 26, 2, 3);	
	return  retVal;
}

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
OPL_STATUS brgUsrDefineRsvMacAdd(UINT8 num,UINT8 *macString)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 mac[MAC_LENGTH];

	if(OPL_NULL == macString)
	{
		return OPL_ERR_NULL_POINTER;
	}
	if(num > 1)
	{
		OPL_DRV_PRINTF(("this chip can only support 2 user define rsv mac.\n"));
		return OPL_ERROR;
	}

	retVal = string2mac(macString, mac);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = brgUsrDefineRsvMacHwWrite(num,mac);

	return retVal;
}
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
OPL_STATUS brgUsrDefineRsvMacShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 index;
	UINT8 mac[MAC_LENGTH];

	OPL_DRV_SHOW_PRINTF(("%-4s %-17s\n","No.","Mac Address"));
	for(index = 0; index < 2; index++)
	{
		retVal =  brgUsrDefineRsvMacHwRead(index, mac);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		OPL_DRV_SHOW_PRINTF(("%-4d %02x:%02x:%02x:%02x:%02x:%02x\n",
									index,
									mac[0],
									mac[1],
									mac[2],
									mac[3],
									mac[4],
									mac[5]));
	}
	return retVal;
}

OPL_STATUS brgSoftArlMacAdd(UINT16 portId,UINT16 vlanId,UINT8 *mac)
{
  UINT8 Flg = 0;
  SOFT_LEARN_MAC_t *ptmp1;
  SOFT_LEARN_MAC_t *ptmp2;
  
  ptmp1 = psoftLearnMacEntry;
  ptmp2 = psoftLearnMacEntry;
  while (ptmp1 != NULL)
  {
    if ((!memcmp(&(ptmp1->mac[0]), mac, ETH_MAC_ADDR_LEN)) && (ptmp1->vlanId==vlanId))
    {
      Flg = 1;
      break;
    }
	ptmp2 = ptmp1;
    ptmp1 = ptmp1->next;
  }
  
  if (1 == Flg)
  {
    ptmp1->portId = portId;
  }
  else
  {
     ptmp1 = (SOFT_LEARN_MAC_t *)OPL_MALLOC(sizeof(SOFT_LEARN_MAC_t));
     if (NULL == ptmp1)
     {
       return OPL_ERROR;
     }
     ptmp1->portId = portId;
     ptmp1->vlanId = vlanId;
	 ptmp1->next = NULL;
     memcpy(&(ptmp1->mac[0]), mac, ETH_MAC_ADDR_LEN);
	 if (psoftLearnMacEntry == NULL)
	 {
	   psoftLearnMacEntry = ptmp1;
     }
	 else
	 {
	   ptmp2->next = ptmp1;
	 }
  }
  
  return OPL_OK;
}

OPL_STATUS brgSoftArlMacFlush(void)
{
  SOFT_LEARN_MAC_t *ptmp1;
  SOFT_LEARN_MAC_t *ptmp2;

  ptmp1 = psoftLearnMacEntry;
  ptmp2 = psoftLearnMacEntry;
  while (ptmp2!= NULL)
  {
    ptmp2 = ptmp2->next;
    brgArlMacDel(ptmp1->portId,ptmp1->vlanId, &(ptmp1->mac[0]));
    kfree(ptmp1);
    ptmp1 = ptmp2;
  }
  psoftLearnMacEntry = NULL;
  return OPL_OK;
}


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
OPL_STATUS brgArlIntProcess(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 arlCapReg0;
	UINT32 arlCapReg1;
	UINT8 mac[MAC_LENGTH];
	UINT16 vlanId = OPL_ZERO;
    UINT16 portId = OPL_ZERO;

	oplRegRead(REG_BRG_ARL_CAP_REG0,&arlCapReg0);
	oplRegRead(REG_BRG_ARL_CAP_REG1,&arlCapReg1);

#ifdef BRG_ARL_DEBUG
	OPL_DRV_PRINTF(("capReg0  = 0x%08x\n",arlCapReg0));
	OPL_DRV_PRINTF(("capReg1  = 0x%08x\n",arlCapReg1));
#endif

	if(arlCapReg1&0x80000000)
	{
	    portId = (arlCapReg1>>28)&0x1;
		vlanId = (arlCapReg1>>16)&0xfff;
		mac[0] = (arlCapReg1&0xff00)>>8;
		mac[1]	= 	(arlCapReg1&0xff);
		mac[2] = (arlCapReg0&0xff000000)>>24;
		mac[3] = (arlCapReg0&0x00ff0000)>>16;
		mac[4] = (arlCapReg0&0x0000ff00)>>8;
		mac[5] = (arlCapReg0&0x000000ff);

#ifdef BRG_ARL_DEBUG
        OPL_DRV_PRINTF(("portId: %d\n",portId));
		OPL_DRV_PRINTF(("vlanId: %d\n",vlanId));
		OPL_DRV_PRINTF(("%02x:%02x:%02x:%02x:%02x:%02x\n",
										mac[0],
										mac[1],
										mac[2],
										mac[3],
										mac[4],
										mac[5]));
#endif
	}

	brgSoftArlMacAdd(portId,vlanId,&mac[0]);
	retVal = brgArlMacAdd(portId,vlanId,&mac[0]);	
	//retVal = brgArlDynamicMacAdd(portId,vlanId,&mac[0]);

    /* begin added by jiangmingli for N:1 aggregation */
    if (NULL != g_pfBrgSoftLearnFunc)
    {
        g_pfBrgSoftLearnFunc(portId+1, vlanId, mac);
    }
    /* end added by jiangmingli for N:1 aggregation */

	return retVal;
}

OPL_STATUS brgArlAgeTimeHwWrite(UINT32 ageTime)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegFieldWrite(REG_BRG_AGE_CTRL, 0, 20, ageTime);

	return retVal;
}

OPL_STATUS brgArlAgeTimeHwRead(UINT32 *ageTime)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == ageTime)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_BRG_AGE_CTRL, 0, 20, ageTime);

	return retVal;
}

OPL_STATUS brgArlLearnEnHwWrite(UINT8 enable)
{
	if(OPL_ENABLE == enable)
	{
		/*enable learning in ge port table*/
		oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, 1);
		oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,25, 1, 0);
	}else
	{
		/*enable learning in ge port table*/
		oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, 1);
		oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,25, 1, 1);
	}
	return OPL_OK;
}

OPL_STATUS brgArlLearnEnHwRead(UINT8 *enable)
{
	UINT32 learnEnable;

	if(OPL_NULL == enable)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	oplRegFieldRead(REG_BRG_GE_PORT_TABLE,26, 1, &learnEnable);

	if(learnEnable == OPL_ENABLE)
	{
		*enable = OPL_ENABLE;
	}else
	{
		*enable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS brgArlSoftLearnEnHwWrite(UINT8 enable)
{
	if(OPL_ENABLE == enable)
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,31, 1, 1);
	}else
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,31, 1, 0);
	}
	return OPL_OK;
}

OPL_STATUS brgArlSoftLearnEnHwRead(UINT8 *enable)
{
	UINT32 softLearnEnable;

	if(OPL_NULL == enable)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	oplRegFieldRead(REG_BRG_AGE_CTRL,31, 1, &softLearnEnable);

	if(softLearnEnable == OPL_ENABLE)
	{
		*enable = OPL_ENABLE;
	}else
	{
		*enable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS brgArlIsoLationEnHwWrite(UINT8 enable)
{
	if(OPL_ENABLE == enable)
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,30, 1, 1);
	}else
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,30, 1, 0);
	}
	return OPL_OK;
}

OPL_STATUS brgArlIsoLationEnHwRead(UINT8 *enable)
{
	UINT32 isoLationEnable;

	if(OPL_NULL == enable)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	oplRegFieldRead(REG_BRG_AGE_CTRL,30, 1, &isoLationEnable);

	if(isoLationEnable == OPL_ENABLE)
	{
		*enable = OPL_ENABLE;
	}else
	{
		*enable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS brgArlFullDropEnHwWrite(UINT8 enable)
{
	if(OPL_ENABLE == enable)
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,29, 1, 1);
	}else
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,29, 1, 0);
	}
	return OPL_OK;
}

OPL_STATUS brgArlFullDropEnHwRead(UINT8 *enable)
{
	UINT32 fullDropEnable;

	if(OPL_NULL == enable)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	oplRegFieldRead(REG_BRG_AGE_CTRL,29, 1, &fullDropEnable);

	if(fullDropEnable == OPL_ENABLE)
	{
		*enable = OPL_ENABLE;
	}else
	{
		*enable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS brgArlAgeEnHwWrite(UINT8 enable)
{
	if(OPL_ENABLE == enable)
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,20, 1, 1);
	}else
	{
		oplRegFieldWrite(REG_BRG_AGE_CTRL,20, 1, 0);
	}
	return OPL_OK;
}

OPL_STATUS brgArlAgeEnHwRead(UINT8 *enable)
{
	UINT32 ageEnable;

	if(OPL_NULL == enable)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	oplRegFieldRead(REG_BRG_AGE_CTRL,20, 1, &ageEnable);

	if(ageEnable == OPL_ENABLE)
	{
		*enable = OPL_ENABLE;
	}else
	{
		*enable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS brgArlConfigShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 softLearnEnable;
	UINT8 learnEnable;
	UINT8 isolationEnable;
	UINT8 fullDropEnable;
	UINT8 ageEnable;
	UINT32 ageTimeVal;
	UINT8 *statusString[] = {"Disable","Enable",NULL};

	retVal = brgArlSoftLearnEnHwRead(&softLearnEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = brgArlLearnEnHwRead(&learnEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = brgArlIsoLationEnHwRead(&isolationEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = brgArlFullDropEnHwRead(&fullDropEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = brgArlAgeEnHwRead(&ageEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = brgArlAgeTimeHwRead(&ageTimeVal);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	OPL_DRV_SHOW_PRINTF(("%-20s %s\n","sotLearn",	statusString[softLearnEnable]));
	OPL_DRV_SHOW_PRINTF(("%-20s %s\n","learn",			statusString[learnEnable]));
	OPL_DRV_SHOW_PRINTF(("%-20s %s\n","isolation",	statusString[isolationEnable]));
	OPL_DRV_SHOW_PRINTF(("%-20s %s\n","fullDrop",	statusString[fullDropEnable]));
	OPL_DRV_SHOW_PRINTF(("%-20s %s\n","ageing",		statusString[ageEnable]));
	OPL_DRV_SHOW_PRINTF(("%-20s %d seconds\n","agetime",	ageTimeVal + 1));

	return OPL_OK;
}

OPL_STATUS brgArlMacNumLimitSet(UINT8 enable, UINT32 numOfArlNum)
{
	OPL_STATUS retVal = OPL_OK;
	PON_BRG_MAC_TAB_t brg_mac_entry;
	UINT32 num;
	UINT32 index;
	UINT8  learnEnable;

    if (OPL_ENABLE == enable)
    {
    	if(numOfArlNum > PON_BRG_MAC_ENTRY_NUM)
    	{
    		return OPL_ERR_INVALID_PARAMETERS;
    	}

    	/*first disable arl learning function*/

    	retVal = brgArlLearnEnHwRead(&learnEnable);
    	if(OPL_OK != retVal)
    	{
    		return retVal;
    	}

    	retVal = brgArlLearnEnHwWrite(OPL_DISABLE);
    	if(OPL_OK != retVal)
    	{
    		return retVal;
    	}

    	num = PON_BRG_MAC_ENTRY_NUM - numOfArlNum;

    	OPL_MEMSET(&brg_mac_entry,0X00,sizeof(PON_BRG_MAC_TAB_t));
    	brg_mac_entry.state = VALID_STATIC;

    	retVal = brgArlInit();

    	for(index = OPL_ZERO;index < num; index++)
    	{
    		retVal = brgArlEntryHwWrite(PON_BRG_MAC_ENTRY_NUM - 1 - index,&brg_mac_entry);
    		if(OPL_OK != retVal)
    		{
    			OPL_LOG_TRACE();
    			return retVal;
    		}
    	}

    	/*enable arl learning */
    	retVal = brgArlLearnEnHwWrite(learnEnable);
    	if(OPL_OK != retVal)
    	{
    		return retVal;
    	}

    }
    else {
    	retVal = brgArlInit();
        if(OPL_OK != retVal)
    	{
    		return retVal;
    	}
    }

    /* enable or disable arl full drop */
    retVal = brgArlFullDropEnHwWrite(enable);
    if(OPL_OK != retVal)
	{
		return retVal;
	}

	return OPL_OK;
}

#if 0
OPL_STATUS cliBrgGeParserCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	cliVosShowPrintf((fd,"%-28s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgGeParserCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgGeParserCounter[index].name;
		regId 		= brgGeParserCounter[index].regId;
		regAddr 	= brgGeParserCounter[index].regAddr;
		regMask 	= brgGeParserCounter[index].regMask;
		desc 		= brgGeParserCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		cliVosShowPrintf((fd,"%-28s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliBrgPortCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	cliVosShowPrintf((fd,"%-43s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgPortCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgPortCounter[index].name;
		regId 		= brgPortCounter[index].regId;
		regAddr 	= brgPortCounter[index].regAddr;
		regMask 	= brgPortCounter[index].regMask;
		desc 		= brgPortCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		cliVosShowPrintf((fd,"%-43s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliBrgCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	cliVosShowPrintf((fd,"%-32s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; brgCounter[index].name != OPL_NULL;index++)
	{
		name 		= brgCounter[index].name;
		regId 		= brgCounter[index].regId;
		regAddr 	= brgCounter[index].regAddr;
		regMask 	= brgCounter[index].regMask;
		desc 		= brgCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliTmCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	cliVosShowPrintf((fd,"%-21s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; tmCounter[index].name != OPL_NULL;index++)
	{
		name 		= tmCounter[index].name;
		regId 		= tmCounter[index].regId;
		regAddr 	= tmCounter[index].regAddr;
		regMask 	= tmCounter[index].regMask;
		desc 		= tmCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		cliVosShowPrintf((fd,"%-21s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliPonMpcpCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;
	UINT8 	*dropType[6] = {	"greater than MPCP_GATE_THR_H"
											,"smaller than MPCP_GATE_THR_L"
											,"grant_len too smaller than TAIL_GUARD"
											,"discovery grant drop after onu_registered"
											,"grant list full drop"
											,OPL_NULL};
	UINT8 dropTypeVal = OPL_ZERO;


	cliVosShowPrintf((fd,"%-26s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; ponMpcpCounter[index].name != OPL_NULL;index++)
	{
		name 		= ponMpcpCounter[index].name;
		regId 		= ponMpcpCounter[index].regId;
		regAddr 	= ponMpcpCounter[index].regAddr;
		regMask 	= ponMpcpCounter[index].regMask;
		desc 		= ponMpcpCounter[index].desc;

		if(regAddr == REG_GATE_DROP_CNT)
		{
			continue;
		}

		oplRegRead(regAddr, &regVal);

		cliVosShowPrintf((fd,"%-26s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
	}

	oplRegRead(REG_GATE_DROP_CNT, &regVal);
	cliVosShowPrintf((fd,"%-26s 0x%05x 0x%08x   %-12u\r\n","GATE_DROP_CNT",REG_GATE_DROP_CNT/4,regVal&0xffff,regVal&0xffff));

	dropTypeVal = 0x1f&(regVal>>24);

	for(index = 0; index <=4; index++)
	{
		if(dropTypeVal)
		{
			if((1>>index )&dropTypeVal)
			{
				cliVosShowPrintf((fd,"%s\r\n",dropType[index]));
			}
		}
	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliHostDmaCounterShow(int fd)
{
	UINT8 *name;
	UINT8 *desc;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT32 index = OPL_ZERO;
	UINT32 regVal;

	cliVosShowPrintf((fd,"%-32s %-7s %-12s %-12s %s\r\n","name","regId","regVal","regVal","desc"));
	for(index = 0; hostDmaCounter[index].name != OPL_NULL;index++)
	{
		name 		= hostDmaCounter[index].name;
		regId 		= hostDmaCounter[index].regId;
		regAddr 	= hostDmaCounter[index].regAddr;
		regMask 	= hostDmaCounter[index].regMask;
		desc 		= hostDmaCounter[index].desc;

		oplRegRead(regAddr, &regVal);

		if(REG_DMA0_EnqueueLengthErrCnt/4 == hostDmaCounter[index].regId)
		{
			cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n","GE2CPU_ENQ_LEN_ERR_CNT",regId,(regVal>>16)&0XFFFF,(regVal>>16)&0XFFFF));
			cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n","PON2CPU_ENQ_LEN_ERR_CNT",regId,regVal&0XFFFF,regVal&0xFFFF));
		}else if(REG_DMA0_EnqueueCRCErrCnt/4 == hostDmaCounter[index].regId)
		{
			cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n","GE2CPU_ENQ_CRC_ERR_CNT",regId,(regVal>>16)&0XFFFF,(regVal>>16)&0XFFFF));
			cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n","PON2CPU_ENQ_CRC_ERR_CNT",regId,regVal&0XFFFF,regVal&0xFFFF));
		}else
		{
			cliVosShowPrintf((fd,"%-32s 0x%05x 0x%08x   %-12u\r\n",name,regId,regVal&regMask,regVal&regMask));
		}

	}
	cliVosShowPrintf((fd,"----------------------------------------------------------------\r\n"));
	return OPL_OK;
}

OPL_STATUS cliAllCounterShow(int fd)
{
	cliBrgGeParserCounterShow(fd);

	cliBrgPortCounterShow(fd);

	cliBrgCounterShow(fd);

	cliTmCounterShow(fd);

	cliTmInQueueCounterShow(fd);

	cliHostDmaCounterShow(fd);

	cliPonMpcpCounterShow(fd);

	return OPL_OK;
}
#endif
/* begin added by jiangmingli for N:1 aggregation */
OPL_STATUS brgArlSoftLearnFuncRegister(BRG_SOFT_LEARN_FUNC pfFunc)
{
    if (NULL == pfFunc)
    {
        return OPL_ERROR;
    }

    g_pfBrgSoftLearnFunc = pfFunc;

    return OPL_OK;
}
/* end added by jiangmingli for N:1 aggregation */



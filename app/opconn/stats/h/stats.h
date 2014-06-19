/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	stats.h
*
* DESCRIPTION:
*	This file implements the structures and macros for stats module.
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

#ifndef __STATS_H__
#define __STATS_H__

#include "cli.h"
#include "opl_driver.h"
#include "opconn_usr_ioctrl.h"


#define CNT_SWH_PORT_NUM	30

#define SWITCH_PORT_NUM		4

#define DECIMAL_STR_LEN		21
#define LONG_HEX_NUM		8


#define STAT_PON_RCV_BITS_ERR    0
#define STAT_PON_RCV_BITS_OK     1
#define STAT_PON_TRX_PKT_OK      2
#define STAT_PON_TRX_BROD_PKT    3
#define STAT_PON_TRX_PAUSE       5
#define STAT_PON_RCV_PKT_OK      16
#define STAT_PON_RCV_FRM_DROP    17
#define STAT_PON_RCV_FRM_CRC     18
#define STAT_PON_RCV_BROD_PKT    23
#define STAT_PON_RCV_PAUSE       25
#define STAT_PON_RCV_OVERSIZE    29

#define STAT_GE_TX_TOTAL_PKT   0
#define STAT_GE_TX_PAUSE       3
#define STAT_GE_TX_BYTE        11
#define STAT_GE_RX_PKT_OK      13
#define STAT_GE_RX_FRM_DROP    14
#define STAT_GE_RX_PAUSE       22
#define STAT_GE_RX_BYTE        34


typedef struct pon_statistic_s{
    UINT64	OctetsTx;
    UINT64	OctetsTxRate;
    UINT64	OctetsTxTopRate;
    UINT64	FrameTx;
    UINT64	FrameTxRate;
    UINT64	TxPacketsError;
    UINT64	OctetsRx;
    UINT64	OctetsRxRate;
    UINT64	OctetsRxTopRate;
    UINT64	FrameRx;
    UINT64	FrameRxRate;
    UINT64	RxLosePackets;
    UINT64	RxLosePacketsRate;
}pon_statistics_t;
#ifndef __OPCONN_PORT_STAT__
#define __OPCONN_PORT_STAT__
typedef struct port_statistic_s{
    UINT64	OctetsTx;
    UINT64	OctetsTxRate;
    UINT64	OctetsTxTopRate;
    UINT64	UnicastFramesTx;
    UINT64	NonUnicastFramesTx;
    UINT64	FrameTxRate;
    UINT64	TxPacketsError;
    UINT64	OctetsRx;
    UINT64	OctetsRxRate;
    UINT64	OctetsRxTopRate;
    UINT64	UnicastFramesRx;
    UINT64	NonUnicastFramesRx;
    UINT64	FrameRxRate;
    UINT64	RxPacketsError;
}port_statistics_t;
#endif


OPL_STATUS odmPonSinglePortStatsGet(UINT8 port_id, port_statistics_t *portStats);
OPL_STATUS odmPonStatsGet(pon_statistics_t * ponStats);

OPL_STATUS ShowCntGe(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntPon(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntFe(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntTm(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntCle(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntBrg(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntMpcp(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntMarb(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntCpdma(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntFedma(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntGeParser(int iWrFd, unsigned int uiMode);
OPL_STATUS ShowCntCleHit(int iWrFd, UINT8 ucStart, UINT8 ucNum);

#endif


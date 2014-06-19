/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	hal_stats.h
*
* DESCRIPTION:
*	This file implements the hal structures and macros for stats module.
*
* Date Created:
*	2009-04-14
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/

#ifndef __HAL_STATS_H__
#define __HAL_STATS_H__

#include "hal.h"

typedef struct HAL_STATS_PAR_s
{
	int				iWrFd;
	unsigned int	uiMode;
}HAL_STATS_PAR_t;

typedef struct HAL_STATS_CLEHIT_s
{
	int             iWrFd;
	unsigned short  usStart;
	unsigned short  usNum;
}HAL_STATS_CLEHIT_t;

typedef struct HAL_STATS_POLL_s
{
	int		iModule;
	int		iEndis;
	unsigned short	usStart;
	unsigned short	usNum;
}HAL_STATS_POLL_t;

typedef struct HAL_STATS_SWHPORT_s
{
	int             iWrFd;
	unsigned short	usStart;
	unsigned short	usNum;
	int				iType;
}HAL_STATS_SWHPORT_t;

extern void HalShowCntGe(void *pPar);
extern void HalShowCntPon(void *pPar);
extern void HalShowCntFe(void *pPar);
extern void HalShowCntTm(void *pPar);
extern void HalShowCntCle(void *pPar);
extern void HalShowCntBrg(void *pPar);
extern void HalShowCntMpcp(void *pPar);
extern void HalShowCntMarb(void *pPar);
extern void HalShowCntCpdma(void *pPar);
extern void HalShowCntFedma(void *pPar);
extern void HalShowCntGeParser(void *pPar);
extern void HalShowCntCleHit(void *pPar);
extern void HalCntClr(void *pPar);
extern void HalShowCntSwhPort(void *pPar);
extern OPL_RESULT HalStatsRegister(void);

#endif


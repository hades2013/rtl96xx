/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	hal_oam.h
*
* DESCRIPTION:
*	This file implements the hal function for oam module.
*
* Date Created:
*	2009-04-15
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/

#ifndef __HAL_OAM_H__
#define __HAL_OAM_H__

#include "hal.h"

typedef struct HAL_OAM_SEND_FRAME_PAR_s
{
	unsigned char	*pucFrame;
	unsigned short	usFrmLen;
}__attribute__((packed)) HAL_OAM_SEND_FRAME_PAR_t;

extern void HalOamFrameSend(void *pPar);
extern OPL_RESULT HalOamRegister(void);
extern void Haleopl_ctc_pdu_process(void *pPar);
extern void Haleopl_h3coam_pdu_process(void *pPar);
extern void Haleopl_oui_pdu_process(void *pPar);

#endif


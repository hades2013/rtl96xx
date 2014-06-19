/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	hal_oam.c
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

#include "hal.h"
#include "hal_oam.h"
#include "oam.h"
#include "ctc.h"
extern UINT32 oam_ver_type;
extern void eopl_ctc_pdu_process_2_0(u8_t *p_byte,u16_t length);

#ifdef OAM_SUPPORT_REG_CALLBACK
extern oam_process_entry oam_private_entry;
#endif

/*******************************************************************************
* HalOamFrameSend
*
* DESCRIPTION:
*  		This function is used to send OAM frame.
*
*	INPUTS:
*		pPar  -  pointer to parameter
*
*	OUTPUTS:
*		(NULL)
*
* 	RETURNS:
* 		(NULL).
*
* 	SEE ALSO:
*
*/
void HalOamFrameSend(void *pPar)
{
	HAL_OAM_SEND_FRAME_PAR_t	*pstOamFrame=(HAL_OAM_SEND_FRAME_PAR_t *)pPar;

	OamFrameSend(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
}

/*******************************************************************************
* Haleopl_ctc_pdu_process
*
* DESCRIPTION:
*  		This function is used to process CTC related OAM packet.
*
*	INPUTS:
*		pPar  -  pointer to parameter
*
*	OUTPUTS:
*		(NULL)
*
* 	RETURNS:
* 		(NULL).
*
* 	SEE ALSO:
*
*/
void Haleopl_ctc_pdu_process(void *pPar)
{
	HAL_OAM_SEND_FRAME_PAR_t	*pstOamFrame=(HAL_OAM_SEND_FRAME_PAR_t *)pPar;

#ifndef OWNCTC
        switch(oam_ver_type)
        {
            case OAM_VER_CTC_0_1_SUPPORT:
            case OAM_VER_CTC_2_0_SUPPORT:
                eopl_ctc_pdu_process_2_0(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
                break;
            case OAM_VER_CTC_2_1_SUPPORT:
                eopl_ctc_pdu_process(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
                break;
            default:
                printf("Unknown OAM version!!\n");
        }
        /* !EricYang */
        #if 0
	eopl_ctc_pdu_process(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
	#endif
#else
#endif
}



/*******************************************************************************
* Haleopl_pmcoam_pdu_process
*
* DESCRIPTION:
*  		This function is used to process pmc EOAM related OAM packet.
*
*	INPUTS:
*		pPar  -  pointer to parameter
*
*	OUTPUTS:
*		(NULL)
*
* 	RETURNS:
* 		(NULL).
*
* 	SEE ALSO:
*
*/
void Haleopl_pmcoam_pdu_process(void *pPar)
{
	HAL_OAM_SEND_FRAME_PAR_t	*pstOamFrame=(HAL_OAM_SEND_FRAME_PAR_t *)pPar;

#ifdef OAM_H3C_SUPPORT
        POam_Pkt_Handle(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
#endif
    return;
}


/*******************************************************************************
* Haleopl_oui_pdu_process
*
* DESCRIPTION:
*  		This function is used to process OUI (besides CTC) related OAM packet.
*
*	INPUTS:
*		pPar  -  pointer to parameter
*
*	OUTPUTS:
*		(NULL)
*
* 	RETURNS:
* 		(NULL).
*
* 	SEE ALSO:
*
*/
void Haleopl_oui_pdu_process(void *pPar)
{
	HAL_OAM_SEND_FRAME_PAR_t	*pstOamFrame=(HAL_OAM_SEND_FRAME_PAR_t *)pPar;
#ifdef OAM_SUPPORT_REG_CALLBACK
        if (oam_private_entry != NULL) {
            oam_private_entry(pstOamFrame->pucFrame, pstOamFrame->usFrmLen);
        }
#endif

}

/*******************************************************************************
* HalOamRegister
*
* DESCRIPTION:
*  		This function is used to register functions at hal oam to hal.
*
*	INPUTS:
*		(NULL)
*
*	OUTPUTS:
*		(NULL)
*
* 	RETURNS:
* 		(NULL).
*
* 	SEE ALSO:
*
*/
OPL_RESULT HalOamRegister(void)
{
	OPL_RESULT		iStatus=0;
	OPL_API_UNIT_t	stApiUnit;

	stApiUnit.apiId = HAL_API_OAM_SEND_FRAME;
	stApiUnit.apiFunc = HalOamFrameSend;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_OAM_CTC_RECEIVE;
	stApiUnit.apiFunc = Haleopl_ctc_pdu_process;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_OAM_OUI_RECEIVE;
	stApiUnit.apiFunc = Haleopl_oui_pdu_process;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

  

 
    stApiUnit.apiId = HAL_API_OAM_PMC_RECEIVE;
	stApiUnit.apiFunc = Haleopl_pmcoam_pdu_process;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	return iStatus;
}


/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	hal_stats.c
*
* DESCRIPTION:
*	This file implements the hal function for stats module.
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

#include "hal_stats.h"
#include "hal.h"
#include "stats.h"

/*******************************************************************************
* HalShowCntGe
*
* DESCRIPTION:
*  		This function is used to show GE counter.
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
void HalShowCntGe(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntGe(pstPar->iWrFd, pstPar->uiMode);

	return;
}

/*******************************************************************************
* HalShowCntPon
*
* DESCRIPTION:
*  		This function is used to show PON counter.
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
void HalShowCntPon(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntPon(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntFe
*
* DESCRIPTION:
*  		This function is used to show FE counter.
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
void HalShowCntFe(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntFe(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntTm
*
* DESCRIPTION:
*  		This function is used to show TM counter.
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
void HalShowCntTm(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntTm(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntCle
*
* DESCRIPTION:
*  		This function is used to show classify counter.
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
void HalShowCntCle(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntCle(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntBrg
*
* DESCRIPTION:
*  		This function is used to show bridge counter.
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
void HalShowCntBrg(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntBrg(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntMpcp
*
* DESCRIPTION:
*  		This function is used to show MPCP counter.
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
void HalShowCntMpcp(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntMpcp(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntMarb
*
* DESCRIPTION:
*  		This function is used to show Marb counter.
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
void HalShowCntMarb(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntMarb(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntCpdma
*
* DESCRIPTION:
*  		This function is used to show DMA of CPU counter.
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
void HalShowCntCpdma(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntCpdma(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntFedma
*
* DESCRIPTION:
*  		This function is used to show DMA of FE counter.
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
void HalShowCntFedma(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntFedma(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntGeParser
*
* DESCRIPTION:
*  		This function is used to show GE parser counter.
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
void HalShowCntGeParser(void *pPar)
{
	HAL_STATS_PAR_t	*pstPar=(HAL_STATS_PAR_t *)pPar;

	ShowCntGeParser(pstPar->iWrFd, pstPar->uiMode);
}

/*******************************************************************************
* HalShowCntCleHit
*
* DESCRIPTION:
*  		This function is used to show classify hit counter.
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
void HalShowCntCleHit(void *pPar)
{
	HAL_STATS_CLEHIT_t	*pstPar=(HAL_STATS_CLEHIT_t *)pPar;

	ShowCntCleHit(pstPar->iWrFd, pstPar->usStart, pstPar->usNum);
}

/*******************************************************************************
* HalCntClr
*
* DESCRIPTION:
*  		This function is used to clear counter.
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
void HalCntClr(void *pPar)
{
	HAL_STATS_POLL_t	*pstPar=(HAL_STATS_POLL_t *)pPar;

	CntClr(pstPar->iModule,pstPar->usStart,pstPar->usNum);
}

/*******************************************************************************
* HalShowCntSwhPort
*
* DESCRIPTION:
*  		This function is used to show switch port counter.
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
void HalShowCntSwhPort(void *pPar)
{
	HAL_STATS_SWHPORT_t	*pstPar=(HAL_STATS_SWHPORT_t *)pPar;

	ShowCntSwhPort(pstPar->iWrFd, pstPar->usStart, pstPar->usNum, pstPar->iType);
}

/*******************************************************************************
* HalStatsRegister
*
* DESCRIPTION:
*  		This function is used to register functions at hal stats to hal.
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
OPL_RESULT HalStatsRegister(void)
{
	OPL_RESULT		iStatus=0;
	OPL_API_UNIT_t	stApiUnit;

	stApiUnit.apiId = HAL_API_STATS_GE;
	stApiUnit.apiFunc = HalShowCntGe;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_PON;
	stApiUnit.apiFunc = HalShowCntPon;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_FE;
	stApiUnit.apiFunc = HalShowCntFe;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_TM;
	stApiUnit.apiFunc = HalShowCntTm;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_CLE;
	stApiUnit.apiFunc = HalShowCntCle;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_BRG;
	stApiUnit.apiFunc = HalShowCntBrg;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_MPCP;
	stApiUnit.apiFunc = HalShowCntMpcp;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_MARB;
	stApiUnit.apiFunc = HalShowCntMarb;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_CPDMA;
	stApiUnit.apiFunc = HalShowCntCpdma;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_FEDMA;
	stApiUnit.apiFunc = HalShowCntFedma;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_GEPARSER;
	stApiUnit.apiFunc = HalShowCntGeParser;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_CLEHIT;
	stApiUnit.apiFunc = HalShowCntCleHit;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}


	stApiUnit.apiId = HAL_API_STATS_CLR;
	stApiUnit.apiFunc = HalCntClr;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	stApiUnit.apiId = HAL_API_STATS_SWHPORT;
	stApiUnit.apiFunc = HalShowCntSwhPort;
	iStatus = halDrvApiRegister(&stApiUnit);
	if(0 != iStatus)
	{
		return iStatus;
	}

	return iStatus;
}


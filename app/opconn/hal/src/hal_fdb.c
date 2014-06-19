/*
=============================================================================
     File Name: hal_fdb.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <vos.h>
#include "hal.h"

#include "opl_driver.h"
#include "opl_debug.h"


//wfxu #include "dal_port.h"
//wfxu #include <dal_mac.h>

#include "hal_fdb.h"
#include "opconn_usr_ioctrl.h"

/*******************************************************************************
* halFdbAgeTimeSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbAgeTimeSet(IN VOID *pPara)
{
    UINT32 ulAgingTime;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    ulAgingTime = *(UINT32 *)pPara;

    dalArlAgeTimeSet(ulAgingTime);

    return;
}

/*******************************************************************************
* halFdbAgeTimeGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbAgeTimeGet(IN VOID *pPara)
{
    UINT32 *pulAgingTime;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pulAgingTime = (UINT32 *)pPara;

    dalArlAgeTimeGet(pulAgingTime);

    return;
}

/*******************************************************************************
* halFdbMacAdd
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacAdd(IN VOID *pPara)
{
    HAL_API_FDB_MAC_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_S *)pPara;

    dalArlMacAdd(pstPara->ucPortId, pstPara->aucMacAddress, pstPara->usVlanId);

    return;
}

/*******************************************************************************
* halFdbMacDel
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacDel(IN VOID *pPara)
{
    HAL_API_FDB_MAC_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_S *)pPara;

    dalArlMacDel(pstPara->ucPortId, pstPara->aucMacAddress, pstPara->usVlanId);

    return;
}

/*******************************************************************************
* halFdbMacRemove
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacRemove(IN VOID *pPara)
{
    HAL_API_FDB_MAC_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_S *)pPara;

    dalArlMacRemove(pstPara->aucMacAddress, pstPara->usVlanId);

    return;
}

/*******************************************************************************
* halFdbDynamicFlush
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbDynamicFlush(IN VOID *pPara)
{
    dalArlFlushDynamic();

    return;
}

/*******************************************************************************
* halFdbAllFlush
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbAllFlush(IN VOID *pPara)
{
    dalArlFlushAll();

    return;
}

VOID halFdbAllFlushByPort(IN VOID *pPara)
{

#if defined(ONU_1PORT)
	dalArlFlushAll();
#else
	if (NULL != pPara){
        dalArlFlushAllByPort(*(UINT32 *)pPara);
    }
#endif
	
    return;
}

/*******************************************************************************
* halFdbLearnEnSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbLearnEnSet(IN VOID *pPara)
{
    HAL_API_FDB_MAC_LEARN_S *pstPara;
    UINT32 ulPortId;
    UINT32 ulEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_LEARN_S *)pPara;
    ulPortId = pstPara->ulPortId;
    ulEnable = pstPara->ulLearnEnable;

    if (INVALID_32 == ulPortId)
    {
        dalArlLearnEnSet((UINT8)ulEnable);
    }
    else
    {
#if defined(ONU_1PORT)
        dalArlLearnEnSet((UINT8)ulEnable);
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
        dalPortLearnEnableSet(ulPortId,ulEnable);
#elif defined(ONU_4PORT_88E6045)
    	dalPortLearnEnableSet(ulPortId,ulEnable);
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    	dalPortLearnEnableSet(ulPortId,ulEnable);
#endif
    }

    return;
}

/*******************************************************************************
* halFdbLearnEnGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbLearnEnGet(IN VOID *pPara)
{
    UINT8 *pucEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pucEnable = (UINT8 *)pPara;

    dalArlLearnEnGet(pucEnable);

    return;
}

/*******************************************************************************
* halFdbSoftLearnEnSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbSoftLearnEnSet(IN VOID *pPara)
{
    UINT8 ucEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    ucEnable = *(UINT8 *)pPara;

    dalArlSoftLearnEnSet(ucEnable);

    return;
}


/*******************************************************************************
* halFdbSoftLearnEnGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbSoftLearnEnGet(IN VOID *pPara)
{
    UINT8 *pucEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pucEnable = (UINT8 *)pPara;

    dalArlSoftLearnEnGet(pucEnable);

    return;
}


/*******************************************************************************
* halFdbAgeEnableSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbAgeEnableSet(IN VOID *pPara)
{
    UINT8 ucEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }
    
    ucEnable = *(UINT8 *)pPara;

    dalArlAgeEnableSet(ucEnable);
    
    return;
}

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbAgeEnableGet(IN VOID *pPara)
{
    UINT8 *pucEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pucEnable = (UINT8 *)pPara;

    dalArlAgeEnableGet(pucEnable);

    return;
}

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacNumLimitSet(IN VOID *pPara)
{
	HAL_API_FDB_MAC_LIMIT_S *macLimit = (HAL_API_FDB_MAC_LIMIT_S *)pPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    dalArlMacNumLimitSet(macLimit->ulPortId, macLimit->ulEnable, macLimit->ulMacNum);

    return;
}

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMultiPortMacAdd(IN VOID *pPara)
{
    HAL_API_FDB_MAC_MULTIPORT_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_MULTIPORT_S *)pPara;

    dalArlMultiPortMacAdd(pstPara->ulPortNum, pstPara->aulPortlist, pstPara->aucMacAddress, pstPara->usVlanId);

    return;
}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
/*******************************************************************************
* halFdbMove
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMove(IN VOID *pPara)
{
    HAL_API_FDB_MAC_MOVE_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_MOVE_S *)pPara;

    dalArlMacMove(pstPara->ulPort, pstPara->ulToPort);

    return;
}
#endif

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMultiPortMacDel(IN VOID *pPara)
{
    HAL_API_FDB_MAC_MULTIPORT_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_MULTIPORT_S *)pPara;

    dalArlMultiPortMacDel(pstPara->ulPortNum, pstPara->aulPortlist, pstPara->aucMacAddress, pstPara->usVlanId);

    return;
}

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacEntryShowOne(IN VOID *pPara)
{
    HAL_API_FDB_MAC_SHOW_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_SHOW_S *)pPara;

    dalArlMacEntryShowOne(pstPara->lFd,pstPara->aucMacAddress, pstPara->usType, pstPara->ulPortNum, pstPara->aulPortlist);

    return;
}

/*******************************************************************************
* halFdbAgeEnableGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halFdbMacEntryShowAll(IN VOID *pPara)
{
    HAL_API_FDB_MAC_SHOW_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_FDB_MAC_SHOW_S *)pPara;

    dalArlMacEntryShowAll(pstPara->lFd, pstPara->usType, pstPara->ulPortNum, pstPara->aulPortlist);

    return;
}

/*******************************************************************************
* halRstpInit
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
OPL_RESULT halFdbInit(VOID)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register rstp api */
    stApiUnit.apiId = HAL_API_FDB_AGETIME_SET;
    stApiUnit.apiFunc = halFdbAgeTimeSet;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_AGETIME_GET;
    stApiUnit.apiFunc = halFdbAgeTimeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_ADD;
    stApiUnit.apiFunc = halFdbMacAdd;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_DEL;
    stApiUnit.apiFunc = halFdbMacDel;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_REMOVE;
    stApiUnit.apiFunc = halFdbMacRemove;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_DYNAMIC_FLUSH;
    stApiUnit.apiFunc = halFdbDynamicFlush;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_ALL_FLUSH;
    stApiUnit.apiFunc = halFdbAllFlush;
    ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_FDB_ALL_FLUSH_BY_PORT;
    stApiUnit.apiFunc = halFdbAllFlushByPort;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_LEARN_EN_SET;
    stApiUnit.apiFunc = halFdbLearnEnSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_LEARN_EN_GET;
    stApiUnit.apiFunc = halFdbLearnEnGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_SOFT_LEARN_EN_SET;
    stApiUnit.apiFunc = halFdbSoftLearnEnSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_SOFT_LEARN_EN_GET;
    stApiUnit.apiFunc = halFdbSoftLearnEnGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_AGE_EN_SET;
    stApiUnit.apiFunc = halFdbAgeEnableSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_AGE_EN_GET;
    stApiUnit.apiFunc = halFdbAgeEnableGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_NUM_LIMIT_SET;
    stApiUnit.apiFunc = halFdbMacNumLimitSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_MULTIPORT_ADD;
    stApiUnit.apiFunc = halFdbMultiPortMacAdd;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_MAC_MULTIPORT_DEL;
    stApiUnit.apiFunc = halFdbMultiPortMacDel;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_ONE_SHOW;
    stApiUnit.apiFunc = halFdbMacEntryShowOne;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_FDB_ALL_SHOW;
    stApiUnit.apiFunc = halFdbMacEntryShowAll;
    ulRet += halDrvApiRegister(&stApiUnit);

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    stApiUnit.apiId = HAL_API_FDB_MAC_MOVE;
    stApiUnit.apiFunc = halFdbMove;
    ulRet += halDrvApiRegister(&stApiUnit);
#endif

    return ulRet;
}


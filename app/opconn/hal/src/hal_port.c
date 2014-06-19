/*
=============================================================================
     File Name: hal_port.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include "opl_driver.h"
#include <vos.h>
#include "errors.h"
#include "hal.h"
#include "hal_port.h"
#include "log.h"
#include "odm_fdb.h"

#define MODULE MOD_PORT

void halPortInit(void *pInput)
{
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;
    
    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return ;
    }
    
    dalPortInit();
    
    return ;
}

void halPortPvidSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return ;
    }
    
    dalPortPvidSet(p->portId,p->pvid);
    
    return;
}

void halPortPvidGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT16 pvid = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortPvidGet(p->portId,&pvid);

    if(OK == halStatus)
    {
        p->pvid = pvid;
    }    
    
    return;
}

void halPortVlanModeSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_VLAN_CFG_t *p = (HAL_PORT_VLAN_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortVlanModeSet(p->portId,p->vlanMode);

    return;
}

void halPortVlanModeGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 vlanMode = 0;
    HAL_PORT_VLAN_CFG_t *p = (HAL_PORT_VLAN_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortVlanModeGet((UINT8)(p->portId),(UINT32 *)&vlanMode);

    if(OK == halStatus)
    {
        p->vlanMode = vlanMode;
    }

    return;
}

void halPortDefaultPriSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortDefaultPriSet(p->portId,p->defaultPri);

    return;
}

void halPortDefaultPriGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 defaultPri = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortVlanModeGet((UINT8)(p->portId),(UINT32 *)&defaultPri);

    if(OK == halStatus)
    {
        p->defaultPri = defaultPri;
    }    
    
    return;
}

 
void halPortStateGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 linkStatus = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortStateGet(p->portId,&linkStatus);

    if(OK == halStatus)
    {
        p->linkStatus = linkStatus;
    }    
    
    return;
}

void halPortSpeedSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortSpeedSet(p->portId,p->speed);

    return;
}

void halPortSpeedGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 speed = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortSpeedGet(p->portId,&speed);

    if(OK == halStatus)
    {
        p->speed = speed;
    }    
    
    return;
}


void halPortDuplexSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortDuplexSet(p->portId,p->duplex);

    return;
}

void halPortDuplexGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 duplex = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDuplexGet(p->portId,&duplex);

    if(OK == halStatus)
    {
        p->duplex = duplex;
    }    
    
    return;
}

void halPortFlowcontrolSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortFlowcontrolSet(p->portId,p->flowCtrol);
    
    return;
}

void halPortFlowcontrolGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 flowCtrol = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortFlowcontrolGet(p->portId,&flowCtrol);

    if(OK == halStatus)
    {
        p->flowCtrol = flowCtrol;
    }    
    
    return;
}

void halPortLoopbackSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortLoopbackSet(p->portId,p->loopback);

    return;
}

void halPortAdminControlSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPhyAdminControlSet(p->portId,p->admin);

    return;
}

void halPortAdminStateGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 admin = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPhyAdminStateGet(p->portId,&admin);

    if(OK == halStatus)
    {
        p->admin = admin;
    }    
    
    return;
}

void halPortAutonegEnableSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPhyAutonegEnableSet(p->portId,p->autoAdmin);

    return;
}

void halPortAutonegAdminStateGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 autoAdmin = 0;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPhyAutonegAdminStateGet(p->portId,&autoAdmin);

    if(OK == halStatus)
    {
        p->autoAdmin = autoAdmin;
    }    
    
    return;
}

void halPortAutonegRestart(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_CFG_INFO_t *p = (HAL_PORT_CFG_INFO_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
   
    halStatus = dalPhyAutonegRestart(p->portId);

    return;
}

void halPortUsPolicingEnableSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortUsPolicingEnableSet(p->portId,p->ingressRateLimitEnable);

    return;
}

void halPortUsPolicingEnableGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 ingressRateLimitEnable = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortFlowcontrolGet(p->portId,&ingressRateLimitEnable);

    if(OK == halStatus)
    {
        p->ingressRateLimitEnable = ingressRateLimitEnable;
    }    
    
    return;
}

void halPortUsPolicingCirSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortUsPolicingCirSet(p->portId,p->ingressCir);

    return;
}

void halPortUsPolicingCirGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 ingressCir = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortUsPolicingCirGet(p->portId,&ingressCir);

    if(OK == halStatus)
    {
        p->ingressCir = ingressCir;
    }    
    
    return;
}

void halPortUsPolicingCbsSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortUsPolicingCbsSet(p->portId,p->ingressCbs);
   
    return;
}

void halPortUsPolicingCbsGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 ingressCbs = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortUsPolicingCbsGet(p->portId,&ingressCbs);

    if(OK == halStatus)
    {
        p->ingressCbs = ingressCbs;
    }    
    
    return;
}

void halPortUsPolicingEbsSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortUsPolicingEbsSet(p->portId,p->ingressEbs);

    return;
}

void halPortUsPolicingEbsGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT32 ingressEbs = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortUsPolicingEbsGet(p->portId,&ingressEbs);

    if(OK == halStatus)
    {
        p->ingressEbs = ingressEbs;
    }    
    
    return;
}

void halPortUsPolicingSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortUsPolicingSet(p->portId,
        p->ingressRateLimitEnable,
        p->ingressCir,
        p->ingressCbs,
        p->ingressEbs);

    return;
}

void halPortUsPolicingGet(void *pInput)
{
    INT32  halStatus = OK;
    UINT8  ingressRateLimitEnable = 0;
    UINT32 ingressCir = 0;
    UINT32 ingressCbs = 0;
    UINT32 ingressEbs = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortUsPolicingGet(p->portId,
        &ingressRateLimitEnable,
        &ingressCir,
        &ingressCbs,
        &ingressEbs);

    if(OK == halStatus)
    {
        p->ingressRateLimitEnable = ingressRateLimitEnable;
        p->ingressCir = ingressCir;
        p->ingressCbs = ingressCbs;
        p->ingressEbs = ingressEbs;
    }    
    
    return;
}

void halPortDsPolicingEnableSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortDsPolicingEnableSet(p->portId,p->egressRateLimitEnable);

    return;
}

void halPortDsPolicingEnableGet(void *pInput)
{
    INT32  halStatus = OK;
    UINT8  egressRateLimitEnable = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDsPolicingEnableGet(p->portId,&egressRateLimitEnable);

    if(OK == halStatus)
    {
        p->egressRateLimitEnable = egressRateLimitEnable;
    }    
    
    return;
}

void halPortDsPolicingCirSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDsPolicingCirSet(p->portId,p->egressCir);

    return;
}

void halPortDsPolicingCirGet(void *pInput)
{
    INT32  halStatus = OK;
    UINT32  egressCir = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDsPolicingCirGet(p->portId,&egressCir);

    if(OK == halStatus)
    {
        p->egressCir = egressCir;
    }    
    
    return;
}

void halPortDsPolicingPirSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortDsPolicingPirSet(p->portId,p->egressCbs);

    return;
}

void halPortDsPolicingPirGet(void *pInput)
{
    INT32  halStatus = OK;
    UINT32  egressCbs = 0;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDsPolicingPirGet(p->portId,&egressCbs);

    if(OK == halStatus)
    {
        p->egressCbs = egressCbs;
    }    
    
    return;
}

void halPortDsPolicingSet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPortDsPolicingSet(p->portId,
        p->egressRateLimitEnable,
        p->egressCir,
        p->egressCbs);
    
    return;
}

void halPortDsPolicingGet(void *pInput)
{
    INT32  halStatus = OK;
    UINT8  egressRateLimitEnable = 0;
    UINT32 egressCir = 0;
    UINT32 egressCbs = 0; 
    HAL_PORT_RATE_LIMIT_CFG_t *p = (HAL_PORT_RATE_LIMIT_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalPortDsPolicingGet(p->portId,
        &egressRateLimitEnable,
        &egressCir,
        &egressCbs);

    if(OK == halStatus)
    {
        p->egressRateLimitEnable = egressRateLimitEnable;
        p->egressCir = egressCir;
        p->egressCbs = egressCbs;
    }    
    
    return;
}

void halPortVttEntryAdd(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_VTT_TAB_t *p = (HAL_PORT_VTT_TAB_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalVttTagEntryAdd(
        0,
        0,
        p->cVlan,
        p->sVlan,
        p->portId,
        &(p->vttIndex), DAL_VTT_UC);

	p->retVal = halStatus;

    return;
}

void halPortVttEntryDel(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_VTT_TAB_t *p = (HAL_PORT_VTT_TAB_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalVttTagEntryDel(
        0,
        0,
        p->cVlan,
        p->sVlan,
        p->portId,
        &(p->vttIndex), DAL_VTT_UC);

	p->retVal = halStatus;

    return;
}

void halPortVttEntryInit(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_VTT_TAB_t *p = (HAL_PORT_VTT_TAB_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalVttInit();

	p->retVal = halStatus;

    return;
}

void halPortVttEntryGet(void *pInput)
{
    INT32 halStatus = OK;
    HAL_PORT_VTT_TAB_t *p = (HAL_PORT_VTT_TAB_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus =  dalVttTagEntryGet(
        p->vttIndex,
        &(p->cVlan),
        &(p->sVlan),
        p->portId
        );

	p->retVal = halStatus;

    return;
}

void halPortVttNumOfRecordGet(void *pInput)
{
    INT32 halStatus = OK;
    UINT8 numOfVttEntry = 0;
    HAL_PORT_VLAN_CFG_t *p = (HAL_PORT_VLAN_CFG_t *)pInput;

    HAL_PORT_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalVttNumOfRecordGet(&numOfVttEntry,p->portId);
	p->retVal = halStatus;

    if(OK == halStatus)
    {
        p->numOfVttEntry = numOfVttEntry;
    }
 
    return;
}

void halPortVlanTrunkEntryAdd(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_VLAN_TRUNK_ENTRY_t *p = (HAL_PORT_VLAN_TRUNK_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

	halStatus = dalTrunkEntryAdd(p->portId, p->vlanId);

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryAdd fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryAdd successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortVlanTrunkEntryDel(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_VLAN_TRUNK_ENTRY_t *p = (HAL_PORT_VLAN_TRUNK_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
	
	halStatus = dalTrunkEntryDel(p->portId, p->vlanId);

	p->retVal = halStatus;

	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryDel fail\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryDel successfully!\n", __FUNCTION__);
	}	
	return;
}

void halPortVlanTrunkEntryValueGet(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_VLAN_TRUNK_ENTRY_t *p = (HAL_PORT_VLAN_TRUNK_ENTRY_t *)pInput;
	UINT16 vlanId;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
	
	halStatus = dalTrunkEntryValueGet(p->portId, p->entryIndex, &vlanId);

	p->retVal = halStatus;

	if(OK == halStatus)
	{
		p->vlanId = (UINT32)vlanId;
	}
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryValueGet fail\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryValueGet successfully, vlanId = %d!\n", __FUNCTION__, vlanId);
	}	

	return;
}

void halPortVlanTrunkEntryNumGet(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_VLAN_CFG_t *p = (HAL_PORT_VLAN_CFG_t *)pInput;
    UINT32 numOfTrunkEntry = 0;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalTrunkEntryNumGet(p->portId, &numOfTrunkEntry);

	p->retVal = halStatus;

    if(OK == halStatus)
    {
        p->numOfTrunkEntry = numOfTrunkEntry;
    }	

	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryNumGet fail\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryNumGet successfully, numOfEntry = %d!\n", __FUNCTION__, numOfTrunkEntry);
	}

	return;
}

void halPortVlanTrunkEntryClear(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_VLAN_CFG_t *p = (HAL_PORT_VLAN_CFG_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
    
    halStatus = dalTrunkEntryClear(p->portId);
	
	p->retVal = halStatus;

	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryClear fail\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalTrunkEntryClear successfully!\n", __FUNCTION__);
	}

	return;
}

void halPortMacFilterEntryAdd(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_MAC_FILTER_ENTRY_t *p = (HAL_PORT_MAC_FILTER_ENTRY_t *)pInput;
	CLS_CONFIG_INFO_t stClassCfgInfo;
	UINT16 usAclRuleId = 0;
	UINT8 szAclRuleName[20];

	vosMemSet(szAclRuleName, 0, 20);
	vosMacToStr(p->mac, szAclRuleName);

	if(NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	halStatus = dalArlMacFilterAdd(p->portId, p->vlanId, p->mac);
#else
	OPL_MEMSET(&stClassCfgInfo, 0, sizeof(CLS_CONFIG_INFO_t));
	stClassCfgInfo.srcMacFlag = 1;
	OPL_MEMCPY(stClassCfgInfo.srcMac.lowRange, p->mac, MAC_LENGTH);
	OPL_MEMSET(stClassCfgInfo.srcMac.highRange, 0xFF, MAC_LENGTH);

	/* set action */
	stClassCfgInfo.t_act = DROP_PKTS;
	dalVoipClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);

	vosConfigUInt32Set(FDB_FILENAME_CFG,
 		FDB_SECTION_MAC_DISCARD,
 		szAclRuleName,
 		usAclRuleId);
      /* add DMAC filter for bug3160 */
	OPL_MEMSET(&stClassCfgInfo, 0, sizeof(CLS_CONFIG_INFO_t));
	stClassCfgInfo.dstMacFlag = 1;
	OPL_MEMCPY(stClassCfgInfo.dstMac.lowRange, p->mac, MAC_LENGTH);
	OPL_MEMSET(stClassCfgInfo.dstMac.highRange, 0xFF, MAC_LENGTH);

	/* set action */
	stClassCfgInfo.t_act = DROP_PKTS;
	dalVoipClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);
	
	szAclRuleName[17] = 'D';
	szAclRuleName[18] = '\0';
	
    vosConfigUInt32Set(FDB_FILENAME_CFG,
 		FDB_SECTION_MAC_DISCARD,
 		szAclRuleName,
 		usAclRuleId);
#endif

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacFilterAdd fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacFilterAdd successfully!\n", __FUNCTION__);
	}
	
	return;
}


void halPortMacFilterEntryDel(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_MAC_FILTER_ENTRY_t *p = (HAL_PORT_MAC_FILTER_ENTRY_t *)pInput;
	UINT32 uiAclRuleId = 0;
	UINT8 szAclRuleName[20];
	INT32 iExist;

	vosMemSet(szAclRuleName, 0, 20);
	vosMacToStr(p->mac, szAclRuleName);

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	halStatus = dalArlMacFilterDel(p->portId, p->vlanId, p->mac);
#else
	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_MAC_DISCARD,
											   szAclRuleName);

	/* the key in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}
	uiAclRuleId = vosConfigUInt32Get(FDB_FILENAME_CFG,
									     		FDB_SECTION_MAC_DISCARD,
									     		szAclRuleName,
									     		0xffffffff);
	if (0xffffffff == uiAclRuleId)
	{
		return FDB_MAC_CFG_FILE_KEY_READ_ERROR;
	}
	halStatus = dalVoipClsDelEntry(uiAclRuleId);

    vosConfigKeyDelete(FDB_FILENAME_CFG, FDB_SECTION_MAC_DISCARD, szAclRuleName);

       /* add DMAC filter for bug3160 */
	szAclRuleName[17] = 'D';
	szAclRuleName[18] = '\0';
	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_MAC_DISCARD,
											   szAclRuleName);

	/* the key in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}
	uiAclRuleId = vosConfigUInt32Get(FDB_FILENAME_CFG,
									     		FDB_SECTION_MAC_DISCARD,
									     		szAclRuleName,
									     		0xffffffff);
	if (0xffffffff == uiAclRuleId)
	{
		return FDB_MAC_CFG_FILE_KEY_READ_ERROR;
	}
	halStatus = dalVoipClsDelEntry(uiAclRuleId);

    vosConfigKeyDelete(FDB_FILENAME_CFG, FDB_SECTION_MAC_DISCARD, szAclRuleName);

#endif

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacFilterDel fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacFilterDel successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortBindMacEntryAdd(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_BIND_MAC_ENTRY_t *p = (HAL_PORT_BIND_MAC_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

	halStatus = dalArlMacAdd(p->portId, p->mac, p->vlanId);

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacAdd fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacAdd successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortBindMacEntryDel(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_BIND_MAC_ENTRY_t *p = (HAL_PORT_BIND_MAC_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

	halStatus = dalArlMacDel(p->portId, p->mac, p->vlanId);

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacDel fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacDel successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortDlfFilterEnable(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_DLF_FILTER_EN_t *p = (HAL_PORT_DLF_FILTER_EN_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	halStatus = dalArlMacDlfFilterEn(p->portId, p->enable);
#endif
	
	return;
}

/* added by lancunyi at 2010-06-08 */
void halPortCntlLockDropEnable(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_CTL_LOCK_DROP_EN_t *p = (HAL_PORT_CTL_LOCK_DROP_EN_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	halStatus = dalArlMacPortLockDropEn(p->portId, p->enable);
#endif
	
	return;
}
/* end added */


void halPortStaticMacEntryAdd(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_STATIC_MAC_ENTRY_t *p = (HAL_PORT_STATIC_MAC_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }
	halStatus = dalArlMacAdd(p->portId, p->mac, p->vlanId);

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacAdd fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacAdd successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortStaticMacEntryDel(void *pInput)
{
    INT32 halStatus = OK;
	HAL_PORT_STATIC_MAC_ENTRY_t *p = (HAL_PORT_STATIC_MAC_ENTRY_t *)pInput;

    if(NULL == p)
    {
        HAL_PORT_TRACE();
        HAL_PORT_PRINTF(("input null pointer.\n"));
        return;
    }

	halStatus = dalArlMacDel(p->portId, p->mac, p->vlanId);

	p->retVal = halStatus;
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacDel fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalArlMacDel successfully!\n", __FUNCTION__);
	}
	
	return;
}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

void halPortMirrorPortSet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_PORT_CFG_t *p = (HAL_PORT_MIRROR_PORT_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirPortSet(p->portId);
	
	return;
}

void halPortMirrorPortGet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_PORT_CFG_t *p = (HAL_PORT_MIRROR_PORT_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirPortGet(p->portId);
	
	return;
}

void halPortMirrorIngressStateSet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_INGRESS_CFG_t *p = (HAL_PORT_MIRROR_INGRESS_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirIngPortSet(p->portId, p->state);
	
	return;
}

void halPortMirrorIngressStateGet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_INGRESS_CFG_t *p = (HAL_PORT_MIRROR_INGRESS_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirIngPortGet(p->portId, p->state);
	
	return;
}

void halPortMirrorEngressStateSet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_ENGRESS_CFG_t *p = (HAL_PORT_MIRROR_ENGRESS_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirEgPortSet(p->portId, p->state);
	
	return;
}

void halPortMirrorEngressStateGet(void *pInput)
{
  INT32 halStatus = OK;
  HAL_PORT_MIRROR_ENGRESS_CFG_t *p = (HAL_PORT_MIRROR_ENGRESS_CFG_t *)pInput;
  
  if (NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}
	halStatus = dalMirEgPortGet(p->portId, p->state);
	
	return;
}
#endif

void halPortStormCtrlRateSet(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_STORM_CFG_t *p = (HAL_PORT_STORM_CFG_t *)pInput;

	if(NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

	halStatus = dalStormCtrlRateSet(p->portId, p->rateLimit);
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalStormRateSet fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalStormRateSet successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortStormCtrlRateGet(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_STORM_CFG_t *p = (HAL_PORT_STORM_CFG_t *)pInput;
    UINT32 rateLimit;
	
	if(NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

	halStatus = dalStormCtrlRateGet(p->portId, &rateLimit);
	
    if(OK == halStatus)
    {
        p->rateLimit= rateLimit;
    }	
	
	return;
}

void halPortStormCtrlFrameSet(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_STORM_CFG_t *p = (HAL_PORT_STORM_CFG_t *)pInput;

	if(NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

	halStatus = dalStormCtrlFrameSet(p->portId, p->stormType, p->limitEds);
	
	if(halStatus != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call dalStormLimitSet fail!\n", __FUNCTION__);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call v successfully!\n", __FUNCTION__);
	}
	
	return;
}

void halPortStormCtrlFrameGet(void *pInput)
{
	INT32 halStatus = OK;
	HAL_PORT_STORM_CFG_t *p = (HAL_PORT_STORM_CFG_t *)pInput;
    UINT32 limitEds;
	
	if(NULL == p)
	{
		HAL_PORT_TRACE();
		HAL_PORT_PRINTF(("input null pointer.\n"));
		return;
	}

	halStatus = dalStormCtrlFrameGet(p->portId, p->stormType, &limitEds);
	
    if(OK == halStatus)
    {
        p->limitEds= limitEds;
    }	
	
	return;
}


void halPortRegister(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet = 0;

    stApiUnit.apiId = HAL_API_PORT_INIT ;
    stApiUnit.apiFunc = halPortInit;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_PVID_SET;
    stApiUnit.apiFunc = halPortPvidSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_PVID_GET;
    stApiUnit.apiFunc = halPortPvidGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VLAN_MODE_SET;
    stApiUnit.apiFunc = halPortVlanModeSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VLAN_MODE_GET;
    stApiUnit.apiFunc = halPortVlanModeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DEFAULT_PRI_SET;
    stApiUnit.apiFunc = halPortDefaultPriSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DEFAULT_PRI_GET;
    stApiUnit.apiFunc = halPortDefaultPriGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_STATE_GET;
    stApiUnit.apiFunc = halPortStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_SPEED_SET;
    stApiUnit.apiFunc = halPortSpeedSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_SPEED_GET;
    stApiUnit.apiFunc = halPortSpeedGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DUPLEX_SET;
    stApiUnit.apiFunc = halPortDuplexSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DUPLEX_GET;
    stApiUnit.apiFunc = halPortDuplexGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_FLOWCONTROL_SET;
    stApiUnit.apiFunc = halPortFlowcontrolSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_FLOWCONTROL_GET;
    stApiUnit.apiFunc = halPortFlowcontrolGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_LOOPBACK_SET;
    stApiUnit.apiFunc = halPortLoopbackSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_ADMIN_SET;
    stApiUnit.apiFunc = halPortAdminControlSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_ADMIN_GET;
    stApiUnit.apiFunc = halPortAdminStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_AUTO_ADMIN_SET;
    stApiUnit.apiFunc = halPortAutonegEnableSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_AUTO_ADMIN_GET;
    stApiUnit.apiFunc = halPortAutonegAdminStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_AUTONEG_RESTART;
    stApiUnit.apiFunc = halPortAutonegRestart;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_USPOLICING_EN_SET;
    stApiUnit.apiFunc = halPortUsPolicingEnableSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_USPOLICING_EN_GET;
    stApiUnit.apiFunc = halPortUsPolicingEnableGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_US_CIR_SET;
    stApiUnit.apiFunc = halPortUsPolicingCirSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_Us_CIR_GET;
    stApiUnit.apiFunc = halPortUsPolicingCirGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_US_CBS_SET;
    stApiUnit.apiFunc = halPortUsPolicingCbsSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_US_CBS_GET;
    stApiUnit.apiFunc = halPortUsPolicingCbsGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_US_EBS_SET;
    stApiUnit.apiFunc = halPortUsPolicingEbsSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_US_EBS_GET;
    stApiUnit.apiFunc = halPortUsPolicingEbsGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_USPOLICING_SET;
    stApiUnit.apiFunc = halPortUsPolicingSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_USPOLICING_GET;
    stApiUnit.apiFunc = halPortUsPolicingGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_POLICING_EN_SET;
    stApiUnit.apiFunc = halPortDsPolicingEnableSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_POLICING_EN_GET;
    stApiUnit.apiFunc = halPortDsPolicingEnableGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_CIR_SET;
    stApiUnit.apiFunc = halPortDsPolicingCirSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_CIR_GET;
    stApiUnit.apiFunc = halPortDsPolicingCirGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_CBS_SET;
    stApiUnit.apiFunc = halPortDsPolicingPirSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_CBS_GET;
    stApiUnit.apiFunc = halPortDsPolicingPirGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_EBS_SET;
    stApiUnit.apiFunc = NULL;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DS_EBS_GET;
    stApiUnit.apiFunc = NULL;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_PORT_DSPOLICING_SET;
    stApiUnit.apiFunc = halPortDsPolicingSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_DSPOLICING_GET;
    stApiUnit.apiFunc = halPortDsPolicingGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VTT_ENTRY_ADD;
    stApiUnit.apiFunc = halPortVttEntryAdd;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VTT_ENTRY_DEL;
    stApiUnit.apiFunc = halPortVttEntryDel;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VTT_ENTRY_INIT;
    stApiUnit.apiFunc = halPortVttEntryInit;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VTT_ENTRY_GET;
    stApiUnit.apiFunc = halPortVttEntryGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VTT_ENTRY_COUNT_GET;
    stApiUnit.apiFunc = halPortVttNumOfRecordGet;
    ulRet += halDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_ADD;
    stApiUnit.apiFunc = halPortVlanTrunkEntryAdd;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_DEL;
    stApiUnit.apiFunc = halPortVlanTrunkEntryDel;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_VALUE_GET;
    stApiUnit.apiFunc = halPortVlanTrunkEntryValueGet;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_NUM_GET;
    stApiUnit.apiFunc = halPortVlanTrunkEntryNumGet;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_CLEAR;
    stApiUnit.apiFunc = halPortVlanTrunkEntryClear;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_ADD;
    stApiUnit.apiFunc = halPortMacFilterEntryAdd;
    ulRet += halDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_DEL;
    stApiUnit.apiFunc = halPortMacFilterEntryDel;
    ulRet += halDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = HAL_API_PORT_BIND_MAC_ENTRY_ADD;
	stApiUnit.apiFunc = halPortBindMacEntryAdd;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_PORT_BIND_MAC_ENTRY_DEL;
	stApiUnit.apiFunc = halPortBindMacEntryDel;
	ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_STORM_CTRL_RATE_SET;
    stApiUnit.apiFunc = halPortStormCtrlRateSet;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_PORT_STORM_CTRL_RATE_GET;
	stApiUnit.apiFunc = halPortStormCtrlRateGet;
	ulRet += halDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = HAL_API_PORT_STORM_CTRL_FRAME_SET;
	stApiUnit.apiFunc = halPortStormCtrlFrameSet;
	ulRet += halDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = HAL_API_PORT_STORM_CTRL_FRAME_GET;
	stApiUnit.apiFunc = halPortStormCtrlFrameGet;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_PORT_DLF_FILTER_ENABLE;
	stApiUnit.apiFunc = halPortDlfFilterEnable;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_PORT_CTL_LOCK_DROP_ENABLE;
	stApiUnit.apiFunc = halPortCntlLockDropEnable;
	ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_ADD;
    stApiUnit.apiFunc = halPortStaticMacEntryAdd;
    ulRet += halDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_DEL;
    stApiUnit.apiFunc = halPortStaticMacEntryDel;
    ulRet += halDrvApiRegister(&stApiUnit);

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

    stApiUnit.apiId = HAL_API_PORT_MIRROR_PORT_SET;
    stApiUnit.apiFunc = halPortMirrorPortSet;
    ulRet += halDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = HAL_API_PORT_MIRROR_PORT_GET;
    stApiUnit.apiFunc = halPortMirrorPortGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_MIRROR_INGRESS_STATE_SET;
    stApiUnit.apiFunc = halPortMirrorIngressStateSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_MIRROR_INGRESS_STATE_GET;
    stApiUnit.apiFunc = halPortMirrorIngressStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_PORT_MIRROR_ENGRESS_STATE_SET;
    stApiUnit.apiFunc = halPortMirrorEngressStateSet;
    ulRet += halDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = HAL_API_PORT_MIRROR_ENGRESS_STATE_GET;
    stApiUnit.apiFunc = halPortMirrorEngressStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);
	
#endif

    return ;
}


/*
=============================================================================
     File Name: hal_vlan.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
wangliang 			2008/8/19		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/
#include <hal.h>
#include <hal_vlan.h>

#define HAL_VLAN_PRINTF(x)  printf x
#define HAL_VLAN_TRACE() HAL_VLAN_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))
#define HAL_VLAN_EXIT(s) {ret=s;goto exit;}
/*******************************************************************************
*
* halVlanCreate:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void  halVlanCreate(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_CREATE_PAR_t *vlan=NULL;

    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }

    vlan = (HAL_VLAN_CREATE_PAR_t *)p1;
    ret = dalCreateVlanID(vlan->vid);
 exit:
    vlan->ret = ret;
    return ;
}
/*******************************************************************************
*
* halVlanDelete:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halVlanDelete(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_CREATE_PAR_t  *vlan=NULL;

    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }

    vlan = (HAL_VLAN_CREATE_PAR_t *)p1;
#if defined(ONU_1PORT)	
	    ret = dalVttMulticastEntryDel(0,0,vlan->vid,vlan->vid,1);
#elif defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
        ret = dalDeleteVlanID(vlan->vid);
#endif
exit:
    vlan->ret = ret;
    return ;
}
/*******************************************************************************
*
* HalVlanFlush:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halVlanFlush(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_FLUSH_PAR_t *vlan=NULL;
    
    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }
    vlan = (HAL_VLAN_FLUSH_PAR_t *)p1;
    ret = dalFlushVlanAll();
    
 exit:
    vlan->ret = ret;
    return ;
}
/*******************************************************************************
*
* HalVlanMemberAdd:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halVlanMemberAdd(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_MEMBER_ADD_PAR_t *vlan=NULL;

    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }

    vlan = (HAL_VLAN_MEMBER_ADD_PAR_t *)p1;
#if defined(ONU_1PORT)
    dalVttMulticastEntryDel(0,0,vlan->vid,vlan->vid,vlan->port);
	if (vlan->type == UNTAG)
	{
	    ret = dalVttMulticastEntryAdd(0,1,vlan->vid,vlan->vid,vlan->port);
	}
	else
	{
	    ret = dalVttMulticastEntryAdd(0,0,vlan->vid,vlan->vid,vlan->port);
	}
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6097)
    ret = dalAddVlanPort(vlan->port,vlan->vid,vlan->type);
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    ret = dalAddVlanPort(vlan->port,vlan->vid,vlan->type);
#endif
    
 exit:
    vlan->ret = ret;
    return ;
}
/*******************************************************************************
*
* HalVlanMemberDel:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
*/
void halVlanMemberDel(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_MEMBER_DEL_PAR_t *vlan=NULL;

    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }

    vlan = (HAL_VLAN_MEMBER_DEL_PAR_t *)p1;

#if defined(ONU_1PORT)	
    ret = dalVttMulticastEntryDel(0,0,vlan->vid,vlan->vid,vlan->port);
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
    ret = dalMoveVlanPort(vlan->port,vlan->vid);
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    ret = dalMoveVlanPort(vlan->port,vlan->vid);
#endif

 exit:
    vlan->ret = ret;
    return ;
}
/*******************************************************************************
*
* halVlanLocalSwitchSet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halVlanLocalSwitchSet(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t *vlan=NULL;

    if (NULL == p1)
    {
        HAL_VLAN_TRACE();
        HAL_VLAN_EXIT(OPL_ERR_NULL_POINTER);
    }

    vlan = (HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t *)p1;
    ret = dalSetLocalSwitch(vlan->port,vlan->enable);
    
 exit:
    vlan->ret = ret;
    return ;
}

#if 0
/*******************************************************************************
*
* HalVlanMemberGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
*/
OPL_STATUS HalVlanMemberGet(void *p1)
{
    OPL_STATUS ret;
    HAL_VLAN_MEMBER_GET_PAR_t *vlan;

    if (NULL == p1)
    {
        return OPL_ERR_NULL_POINTER;
    }

    vlan = (HAL_VLAN_MEMBER_GET_PAR_t *)p1;

    return ret;
}
#endif
void halVlanInit(void)
{
    OPL_API_UNIT_t clsApiUnit;
    OPL_RESULT ulRet;
    /*register vlan hal api*/
    clsApiUnit.apiId   = HAL_API_VLAN_CREATE;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanCreate;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_VLAN_DELETE;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanDelete;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_VLAN_FLUSH;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanFlush;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_VLAN_MEMBERADD;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanMemberAdd;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_VLAN_MEMBERDEL;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanMemberDel;    
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_VLAN_LOCAL_SWITCH_SET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halVlanLocalSwitchSet;    
    ulRet = halDrvApiRegister(&clsApiUnit);

    return ;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/



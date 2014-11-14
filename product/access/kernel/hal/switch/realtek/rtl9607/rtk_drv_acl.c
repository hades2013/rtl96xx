/*****************************************************************************
------------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/
#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>

#include <hal/chipdef/apollomp/apollomp_def.h>
#include <hal/common/halctrl.h>

#include <rtk/acl.h>
#include <rtk/vlan.h>
#include <rtk/rate.h>

#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/autoconf.h>
//#include <linux/sched.h>
//#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/slab.h>

/*----------------------------------------------*
 * extern function                              *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal function                              *
 *----------------------------------------------*/
 static vlanIntf_t aclVlanIntf[MAX_VLAN_INTERFACE+1];
static DRV_RET_E _Hal_CfgPortFilterModeSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, ACL_TRUST_MODE_E enMode);
static DRV_RET_E _Hal_CfgPortFilterRuleIdSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, UINT32 uiAclId);
static DRV_RET_E _Hal_CfgPortClfRmkModeSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, ACL_TRUST_MODE_E enMode);
static DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 uiAclId);

//#ifdef CTC_MULTICAST_SURPORT
#if 1
static DRV_RET_E _Hal_CfgPortMcVlanSet(UINT32 uiLPortId, PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg);
#endif
#define MAX_TEMPLATE_NUM 4
#define MAX_ACL_FEILD_NUM 8
/*----------------------------------------------*
 * global parameter                             *
 *----------------------------------------------*/

rtk_acl_template_t m_astAclTemplate[MAX_TEMPLATE_NUM] =
{
	{
		.index = 0,
		.fieldType[0] = ACL_FIELD_DMAC0,
		.fieldType[1] = ACL_FIELD_DMAC1,
		.fieldType[2] = ACL_FIELD_DMAC2,
		.fieldType[3] = ACL_FIELD_STAG,
		.fieldType[4] = ACL_FIELD_SMAC0,
		.fieldType[5] = ACL_FIELD_SMAC1,
		.fieldType[6] = ACL_FIELD_SMAC2,
		.fieldType[7] = ACL_FIELD_ETHERTYPE,
	},
	{
		.index = 1,
		.fieldType[0] = ACL_FIELD_CTAG,
		.fieldType[1] = ACL_FIELD_IPV4_SIP0,
		.fieldType[2] = ACL_FIELD_IPV4_SIP1,
		.fieldType[3] = ACL_FIELD_VID_RANGE,
		.fieldType[4] = ACL_FIELD_IP_RANGE,
		.fieldType[5] = ACL_FIELD_PORT_RANGE,
		.fieldType[6] = ACL_FIELD_IPV4_DIP0,
		.fieldType[7] = ACL_FIELD_IPV4_DIP1,
	},
	{
		.index = 2,
		.fieldType[0] = ACL_FIELD_USER_DEFINED07,
		.fieldType[1] = ACL_FIELD_USER_DEFINED08,
		.fieldType[2] = ACL_FIELD_USER_DEFINED09,
		.fieldType[3] = ACL_FIELD_USER_DEFINED10,
		.fieldType[4] = ACL_FIELD_USER_DEFINED02,
		.fieldType[5] = ACL_FIELD_USER_DEFINED03,
		.fieldType[6] = ACL_FIELD_USER_DEFINED04,
		.fieldType[7] = ACL_FIELD_USER_DEFINED05,
	},
	{
		.index = 3,
		.fieldType[0] = ACL_FIELD_DMAC0,
		.fieldType[1] = ACL_FIELD_DMAC1,
		.fieldType[2] = ACL_FIELD_DMAC2,
		.fieldType[3] = ACL_FIELD_CTAG,
		.fieldType[4] = ACL_FIELD_ETHERTYPE,
		.fieldType[5] = ACL_FIELD_USER_DEFINED07,/*ACL_FIELD_TCP_SPORT*//*ACL_FIELD_UDP_SPORT*/
		.fieldType[6] = ACL_FIELD_USER_DEFINED08,/*ACL_FIELD_TCP_DPORT*//*ACL_FIELD_UDP_DPORT*/
		//.fieldType[7] = ACL_FIELD_USER_DEFINED10,		
		.fieldType[7] = ACL_FIELD_STAG,
	}	
};

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stAclMutex;

rtk_acl_ingress_entry_t m_astAclCfg[ACL_RULE_NUM_MAX];
//rtk_acl_template_t m_astAclTemplate[4];
//rtk_enable_t m_astAclTemplateEn[4][RTK_MAX_NUM_OF_ACL_RULE_FIELD];
PORT_CLF_REMAERK_CFG_S m_astUniClfMarkMode[LOGIC_PORT_NO+1];
/*include uplink port*/
PORT_FILTER_CFG_S m_astUniFilterMode[LOGIC_PORT_NO+2];
PORT_QINQ_S g_astUniQinqMode[LOGIC_PORT_NO+1];
STATIC UINT32 g_uiDropLoopdPPMask = 0;

//#ifdef CTC_MULTICAST_SURPORT
#if 1
PORT_MC_VLAN_CFG_S m_astUniMcVlanCfg[LOGIC_PORT_NO+1];
#endif

/*----------------------------------------------*
 * macro                                        *
 *----------------------------------------------*/
#define ACL_MUTEX_INIT     mutex_init(&m_stAclMutex)
#define ACL_MUTEX_LOCK     mutex_lock(&m_stAclMutex)
#define ACL_MUTEX_UNLOCK   mutex_unlock(&m_stAclMutex)

/*****************************************************************************
    Func Name: Hal_AclInit
  Description: Initiate acl module
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void Hal_AclInit(void)
{
    UINT32 lport;
    UINT32 uiPrec;
	int index=0;
//#ifdef CTC_MULTICAST_SURPORT
#if 1
    PORT_MC_VLAN_CFG_S stCtcMcVlanCfg;
#endif
    
    ACL_MUTEX_INIT;

	ACL_MUTEX_LOCK;	
	memset(m_astAclCfg, 0, sizeof(m_astAclCfg));  
	memset(aclVlanIntf, 0, sizeof(aclVlanIntf));
	ACL_MUTEX_UNLOCK;	
	for(index=0;index<MAX_TEMPLATE_NUM;index++)
	rtk_acl_template_set(&m_astAclTemplate[index]);
	
    memset(&stCtcMcVlanCfg, 0, sizeof(stCtcMcVlanCfg));
    for (uiPrec = 0; uiPrec < CTC_MC_MAX_GROUP_NUM; uiPrec++)
    {
        stCtcMcVlanCfg.auiAclList[uiPrec] = ACL_RULE_ID_IVALLID;
        stCtcMcVlanCfg.auiVlanList[uiPrec] = CTC_MC_VLAN_ID_IVALID;
    }     
    LgcPortFor(lport)
    {
        for (uiPrec = 0; uiPrec < EOAM_FILTER_RULE_NUM_MAX; uiPrec++)
        {
            (void)_Hal_CfgPortFilterModeSet(lport, uiPrec, ACL_DIRECTION_BOTH, ACL_TRUST_END);
            (void)_Hal_CfgPortFilterRuleIdSet(lport, uiPrec, ACL_DIRECTION_BOTH, ACL_RULE_ID_IVALLID);
        }

        for (uiPrec = 0; uiPrec < CTC_CLF_REMARK_RULE_NUM_MAX*2; uiPrec++)
        {
            (void)_Hal_CfgPortClfRmkModeSet(lport, uiPrec, ACL_TRUST_END);
            (void)_Hal_CfgPortClfRmkAclRuleIdSet(lport, uiPrec, ACL_RULE_ID_IVALLID);
        }
//#ifdef CTC_MULTICAST_SURPORT
#if 1
        (void)_Hal_CfgPortMcVlanSet(lport, &stCtcMcVlanCfg);
#endif
    }

    for (uiPrec = 0; uiPrec < EOAM_FILTER_RULE_NUM_MAX; uiPrec++)
    {
        (void)_Hal_CfgPortFilterModeSet(LOGIC_UPPON_PORT, uiPrec, ACL_DIRECTION_BOTH, ACL_TRUST_END);
        (void)_Hal_CfgPortFilterRuleIdSet(LOGIC_UPPON_PORT, uiPrec, ACL_DIRECTION_BOTH, ACL_RULE_ID_IVALLID);
    }
    
    return;
}

/*****************************************************************************
    Func Name: _Hal_AclCfgGet
  Description: Get softcfg of acl.
        Input: UINT32 uiAclId               
                void *pstAclCfg  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclCfgGet(UINT32 uiAclId, void *pstAclCfg)
{
	rtk_acl_ingress_entry_t *pstRtlAclCfg = NULL;

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclCfg))
    {
        return DRV_ERR_PARA;
    }

	pstRtlAclCfg = (rtk_acl_ingress_entry_t *)pstAclCfg;

    ACL_MUTEX_LOCK;
    memcpy(pstRtlAclCfg, &m_astAclCfg[uiAclId], sizeof(rtk_acl_ingress_entry_t));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclCfgSet
  Description: Save softcfg of acl.
        Input: UINT32 uiAclId               
                void *pstAclCfg  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclCfgSet(UINT32 uiAclId, void *pstAclCfg)
{
    rtk_acl_ingress_entry_t *pstRtlAclCfg = NULL;

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclCfg))
    {
        return DRV_ERR_PARA;
    }

    pstRtlAclCfg = (rtk_acl_ingress_entry_t *)pstAclCfg;

    ACL_MUTEX_LOCK;
    memcpy(&m_astAclCfg[uiAclId],pstRtlAclCfg,sizeof(rtk_acl_ingress_entry_t));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclActionCfgGet
  Description: Get softcfg of acl action.
        Input: UINT32 uiAclId                     
                void *pstAclAction  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclActionCfgGet(UINT32 uiAclId, void *pstAclAction)
{
    rtk_acl_igr_act_t *pstRtlAclAct = NULL;

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclAction))
    {
        return DRV_ERR_PARA;
    }

    pstRtlAclAct = (rtk_acl_igr_act_t *)pstAclAction;
 
    ACL_MUTEX_LOCK;
    memcpy(pstRtlAclAct, &m_astAclCfg[uiAclId].act, sizeof(rtk_acl_igr_act_t));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclActionCfgSet
  Description: Save softcfg of acl action.
        Input: UINT32 uiAclId                     
                void *pstAclAction  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclActionCfgSet(UINT32 uiAclId, void *pstAclAction)
{
    rtk_acl_igr_act_t *pstRtlAclAct = NULL;

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclAction))
    {
        return DRV_ERR_PARA;
    }

    pstRtlAclAct = (rtk_acl_igr_act_t *)pstAclAction;
     
    ACL_MUTEX_LOCK;
    memcpy(&m_astAclCfg[uiAclId].act, pstRtlAclAct, sizeof(rtk_acl_igr_act_t));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleFieldFill
  Description: Fill acl rule field.
        Input: ACL_TRUST_MODE_E enTrustMode          
                void *pValue                          
                void *pAclTypeAndValue  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleFieldFill(ACL_TRUST_MODE_E enTrustMode, void *pValue, void *pAclTypeAndValue) 
{
    rtk_acl_field_t *pstRtlAclTypeAndValue = NULL;

    if ((ACL_TRUST_END <= enTrustMode) ||
        (NULL == pValue)             ||
        (NULL == pAclTypeAndValue))
    {
        return DRV_ERR_PARA;
    }   

    pstRtlAclTypeAndValue = (rtk_acl_field_t *)pAclTypeAndValue;

    switch (enTrustMode)
    {
        case ACL_TRUST_PORT://trust port
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_DMAC;
            memset(pstRtlAclTypeAndValue->fieldUnion.mac.value.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->fieldUnion.mac.mask.octet,0,ETHER_ADDR_LEN);
            break;
        case ACL_TRUST_SMAC:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_SMAC;
            memcpy(pstRtlAclTypeAndValue->fieldUnion.mac.value.octet,(UINT8 *)pValue,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->fieldUnion.mac.mask.octet,0xFF,ETHER_ADDR_LEN);
            break;
        case ACL_TRUST_DMAC:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_DMAC;
            memcpy(pstRtlAclTypeAndValue->fieldUnion.mac.value.octet,(UINT8 *)pValue,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->fieldUnion.mac.mask.octet,0xFF,ETHER_ADDR_LEN);
            break;
        case ACL_TRUST_CTAG_PRIO:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_CTAG;
            pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.mask = RTK_MAX_NUM_OF_PRIORITY - 1;
            break;
        case ACL_TRUST_ETHTYPE:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_ETHERTYPE;
            pstRtlAclTypeAndValue->fieldUnion.data.value = *((UINT32 *)pValue);
    		pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFFFF;
            break;
        case ACL_TRUST_CTAG_VID:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_CTAG;
            pstRtlAclTypeAndValue->fieldUnion.l2tag.vid.value = *((UINT32 *)pValue);      
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.value = 0;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.cfi_dei.value = 0;
			pstRtlAclTypeAndValue->fieldUnion.l2tag.vid.mask = RTK_VLAN_ID_MAX;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.mask = 0;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.cfi_dei.mask = 0;
            break;
        case ACL_TRUST_IPV4_SIP:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_IPV4_SIP;
            pstRtlAclTypeAndValue->fieldUnion.ip.value = *(UINT32 *)pValue;
            pstRtlAclTypeAndValue->fieldUnion.ip.mask = 0xFFFFFFFF;
            break;
        case ACL_TRUST_IPV4_DIP:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_IPV4_DIP;
            pstRtlAclTypeAndValue->fieldUnion.ip.value = *(UINT32 *)pValue;
            pstRtlAclTypeAndValue->fieldUnion.ip.mask = 0xFFFFFFFF;
            break;
        case ACL_TRUST_IPV4_PRENCEDENCE:
            pstRtlAclTypeAndValue->fieldType = ACL_FIELD_END;
            break;
        case ACL_TRUST_IPV4_TOS:
            pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED09;
            pstRtlAclTypeAndValue->fieldUnion.data.value = (*((UINT32 *)pValue))*256;
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFF00;
			break;
        case ACL_TRUST_IPV4_PROTOCOL:
            pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED10;
            pstRtlAclTypeAndValue->fieldUnion.data.value = (*((UINT32 *)pValue))*256;
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFF00;
            break;
        case ACL_TRUST_TCP_SPORT:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED07;/*ACL_FIELD_TCP_SPORT*/
            pstRtlAclTypeAndValue->fieldUnion.data.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFFFF;
            break;
        case ACL_TRUST_TCP_DPORT:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED08;/*ACL_FIELD_TCP_DPORT*/
            pstRtlAclTypeAndValue->fieldUnion.data.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFFFF;
            break;
        case ACL_TRUST_UDP_SPORT:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED07;/*ACL_FIELD_UDP_SPORT*/
            pstRtlAclTypeAndValue->fieldUnion.data.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFFFF;
            break;
        case ACL_TRUST_UDP_DPORT:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_USER_DEFINED08;/*ACL_FIELD_UDP_DPORT*/
            pstRtlAclTypeAndValue->fieldUnion.data.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->fieldUnion.data.mask = 0xFFFF;
            break;
        case ACL_TRUST_STAG_VID:
			pstRtlAclTypeAndValue->fieldType = ACL_FIELD_STAG;
			pstRtlAclTypeAndValue->fieldUnion.l2tag.vid.value = *((UINT32 *)pValue);      
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.value = 0;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.cfi_dei.value = 0;
			pstRtlAclTypeAndValue->fieldUnion.l2tag.vid.mask = RTK_VLAN_ID_MAX;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.pri.mask = 0;
    		pstRtlAclTypeAndValue->fieldUnion.l2tag.cfi_dei.mask = 0;
            break;
        default:
            pstRtlAclTypeAndValue->fieldType = ACL_FIELD_END;
            break;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleCfgCreate
  Description: Create acl rule cfg.
        Input: UINT32 uiAclId                  
                ACL_TRUST_MODE_E uiAclRuleType  
                ACL_ACTION_E enAclAct           
                void *pAclRuleTypeValue         
                void *pAclRuleData              
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleCfgCreate(UINT32 uiAclId, ACL_TRUST_MODE_E uiAclRuleType, ACL_ACTION_E enAclAct, void *pAclRuleTypeValue, void *pAclRuleData)
{
    DRV_RET_E RetVal = DRV_OK;
    rtk_acl_field_t *pstAclRuleField = NULL;
	rtk_acl_ingress_entry_t stAclCfg;
  
    if ((ACL_RULE_NUM_MAX <= uiAclId)    ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        (ACL_ACTION_END <= enAclAct)     ||
        (NULL == pAclRuleTypeValue)      ||
        (NULL == pAclRuleData))
    {
        return DRV_ERR_PARA;
    }

    pstAclRuleField = (rtk_acl_field_t *)kmalloc(sizeof(rtk_acl_field_t), GFP_KERNEL);
    if(NULL == pstAclRuleField)
    {
        return DRV_ERR_NO_MEM;
    }
    
    memset(pstAclRuleField, 0, sizeof(rtk_acl_field_t));    

    RetVal = _Hal_AclRuleFieldFill(uiAclRuleType, pAclRuleTypeValue, (void *)pstAclRuleField);
    if (DRV_OK != RetVal)
    {
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    if (ACL_FIELD_END <= pstAclRuleField->fieldType)
    {
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    pstAclRuleField->next = NULL;

    (void)_Hal_AclCfgGet(uiAclId,(void *)&stAclCfg);

    if (RT_ERR_OK != rtk_acl_igrRuleField_add(&stAclCfg, pstAclRuleField))
    {
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    switch (enAclAct)
    {
        case ACL_ACTION_REMATK_PRIORITY:
            /*remark internal priority*/
            stAclCfg.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
            stAclCfg.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
    		stAclCfg.act.priAct.aclPri = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_MIRROR:
			stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_IGR_MIRROR_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_CVLAN_REMARK:
			stAclCfg.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
			stAclCfg.act.cvlanAct.act = ACL_IGR_CVLAN_EGR_CVLAN_ACT;
            stAclCfg.act.cvlanAct.cvid = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_CVLAN_ASSIGN:
            stAclCfg.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    		stAclCfg.act.cvlanAct.act = ACL_IGR_CVLAN_IGR_CVLAN_ACT;
    		stAclCfg.act.cvlanAct.cvid= *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_SVLAN_REMARK:
            stAclCfg.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
			stAclCfg.act.svlanAct.act = ACL_IGR_SVLAN_EGR_SVLAN_ACT;
            stAclCfg.act.svlanAct.svid = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_COPY_TO_PORTS:
			stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_REDIRECT_TO_PORTS:
			stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_COPY_TO_CPU:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = 0x1<<SWITCH_CPU_PHY_PORT;
            break;
        case ACL_ACTION_POLICING_0:
            stAclCfg.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    		stAclCfg.act.cvlanAct.act = ACL_IGR_LOG_POLICING_ACT;
    		stAclCfg.act.cvlanAct.meter = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_TRAP_TO_CPU:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
            break;
        case ACL_ACTION_DROP:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = 0;
            break;
        default:
            break;
    }
	
    (void)_Hal_AclCfgSet(uiAclId, (void *)&stAclCfg);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleCfgCreate
  Description: Create acl rule cfg.
        Input: UINT32 uiAclId                  
                ACL_TRUST_MODE_E uiAclRuleType  
                ACL_ACTION_E enAclAct           
                void *pAclRuleTypeValue         
                void *pAclRuleData              
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleMacCfgCreate(UINT32 uiAclId, ACL_TRUST_MODE_E uiAclRuleType, ACL_ACTION_E enAclAct, void *pMac, void * pMacMask, void *pAclRuleData)
{
	rtk_acl_field_t *pstAclRuleField = NULL;
	rtk_acl_ingress_entry_t stAclCfg;
  
    if ((ACL_RULE_NUM_MAX <= uiAclId)    ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        (ACL_ACTION_END <= enAclAct)     ||
        (NULL == pMac)                   ||
        (NULL == pMacMask)               ||
        (NULL == pAclRuleData))
    {
        return DRV_ERR_PARA;
    }

    pstAclRuleField = (rtk_acl_field_t *)kmalloc(sizeof(rtk_acl_field_t), GFP_KERNEL);
    if(NULL == pstAclRuleField)
    {
        return DRV_ERR_NO_MEM;
    }
    
    memset(pstAclRuleField, 0, sizeof(rtk_acl_field_t));    

	switch (uiAclRuleType)
    {
        case ACL_TRUST_SMAC:
			pstAclRuleField->fieldType = ACL_FIELD_SMAC;
            memcpy(pstAclRuleField->fieldUnion.mac.value.octet,(UINT8 *)pMac,ETHER_ADDR_LEN);
            memcpy(pstAclRuleField->fieldUnion.mac.mask.octet,(UINT8 *)pMacMask,ETHER_ADDR_LEN);
            break;
        case ACL_TRUST_DMAC:
			pstAclRuleField->fieldType = ACL_FIELD_DMAC;
            memcpy(pstAclRuleField->fieldUnion.mac.value.octet,(UINT8 *)pMac,ETHER_ADDR_LEN);
            memcpy(pstAclRuleField->fieldUnion.mac.mask.octet,(UINT8 *)pMacMask,ETHER_ADDR_LEN);
            break;
		default:
            pstAclRuleField->fieldType = ACL_FIELD_END;
            break;
	}

    if (ACL_FIELD_END <= pstAclRuleField->fieldType)
    {
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    pstAclRuleField->next = NULL;

    (void)_Hal_AclCfgGet(uiAclId,(void *)&stAclCfg);

    if (RT_ERR_OK != rtk_acl_igrRuleField_add(&stAclCfg, pstAclRuleField))
    {
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    switch (enAclAct)
    {
        case ACL_ACTION_REMATK_PRIORITY:
            stAclCfg.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;           
            stAclCfg.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
    		stAclCfg.act.priAct.aclPri = *((UINT8 *)pAclRuleData);;
            break;
        case ACL_ACTION_MIRROR:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_IGR_MIRROR_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_CVLAN_REMARK:
            stAclCfg.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    		stAclCfg.act.cvlanAct.act = ACL_IGR_CVLAN_EGR_CVLAN_ACT;
    		stAclCfg.act.cvlanAct.cvid = *((UINT16 *)pAclRuleData);
            break;
        case ACL_ACTION_CVLAN_ASSIGN:
            stAclCfg.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    		stAclCfg.act.cvlanAct.act = ACL_IGR_CVLAN_IGR_CVLAN_ACT;
    		stAclCfg.act.cvlanAct.cvid = *((UINT16 *)pAclRuleData);
            break;
        case ACL_ACTION_SVLAN_REMARK:
            stAclCfg.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
			stAclCfg.act.svlanAct.act = ACL_IGR_SVLAN_EGR_SVLAN_ACT;
            stAclCfg.act.svlanAct.svid = *((UINT16 *)pAclRuleData);
            break;
        case ACL_ACTION_COPY_TO_PORTS:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_REDIRECT_TO_PORTS:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_COPY_TO_CPU:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = *((UINT32 *)pAclRuleData);
            break;
        case ACL_ACTION_POLICING_0:
            stAclCfg.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    		stAclCfg.act.cvlanAct.act = ACL_IGR_LOG_POLICING_ACT;
    		stAclCfg.act.cvlanAct.meter = *((UINT8 *)pAclRuleData);
            break;
        case ACL_ACTION_TRAP_TO_CPU:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
            break;
        case ACL_ACTION_DROP:
            stAclCfg.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
			stAclCfg.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
			stAclCfg.act.forwardAct.portMask.bits[0] = 0;
            break;
        default:
            break;
    }
	
    (void)_Hal_AclCfgSet(uiAclId, (void *)&stAclCfg);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleCfgClear
  Description: Clear acl rule cfg.
        Input: UINT32 uiAclRuleId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleCfgClear(UINT32 uiAclRuleId)
{
    rtk_acl_field_t *pstAclRuleField = NULL;
    rtk_acl_field_t *pstAclRuleFieldNext = NULL;
    rtk_acl_ingress_entry_t stAclCfg;

    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

    pstAclRuleField = stAclCfg.pFieldHead;
    while (NULL != pstAclRuleField)
    {
        pstAclRuleFieldNext = pstAclRuleField->next;
        kfree(pstAclRuleField);
        pstAclRuleField = pstAclRuleFieldNext;
    }

    ACL_MUTEX_LOCK;
    memset((m_astAclCfg + uiAclRuleId), 0, sizeof(m_astAclCfg[0]));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleEmptyIdGet
  Description: Get ampty rule id.
        Input: UINT32 *puiAclRuleId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
//static DRV_RET_E _Hal_AclRuleEmptyIdGet(UINT32 *puiAclRuleId)
DRV_RET_E _Hal_AclRuleEmptyIdGet(UINT32 *puiAclRuleId)
{
    UINT32 uiAclruleIndex = 0;
    rtk_acl_ingress_entry_t stAclCfg;

    if (NULL == puiAclRuleId)
    {
        return DRV_ERR_PARA;
    }
    
    for (uiAclruleIndex = 0; uiAclruleIndex < ACL_RULE_NUM_MAX; uiAclruleIndex++)
    {
        (VOID)_Hal_AclCfgGet(uiAclruleIndex, (void *)&stAclCfg);

        if (NULL == stAclCfg.pFieldHead)
        {
            #if 0
            *(puiAclRuleId + uiAclIdNum) = uiAclruleIndex;
            uiAclIdNum++;
 
            if (uiAclIdNum >= uiIdNum)
            {
                break;
            }
            #else
            *puiAclRuleId = uiAclruleIndex;
            break;
            #endif
        }
    }

    if (ACL_RULE_NUM_MAX == uiAclruleIndex)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleIsEmpty
  Description: Check if the acl rule is empty.
        Input: UINT32 uiAclRuleId  
                BOOL *pbState      
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleIsEmpty(UINT32 uiAclRuleId, BOOL *pbState)
{
	rtk_acl_ingress_entry_t stAclCfg;

    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (NULL == pbState))
    {
        return DRV_ERR_PARA;
    }
    
    (void)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

    if (NULL == stAclCfg.pFieldHead)
    {
        *pbState = TRUE;
    }
    else
    {
        *pbState = FALSE;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleActivePortsGet
  Description: Get ports which the rule was bind to.
        Input: UINT32 uiAclRuleId   
                UINT32 *puiPortMask  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleActivePortsGet(UINT32 uiAclRuleId, UINT32 *puiPortMask)
{
    rtk_acl_ingress_entry_t stAclCfg;
    
    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (NULL == puiPortMask))
    {
        return DRV_ERR_PARA;
    }
    
    (VOID)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

    *puiPortMask = stAclCfg.activePorts.bits[0];

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleCfgPortsClear
  Description: Clear the acl rule cfg's ports which the acl rule was bind to.
        Input: UINT32 uiAclRuleId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleCfgPortsClear(UINT32 uiAclRuleId)
{
    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    m_astAclCfg[uiAclRuleId].activePorts.bits[0] = 0;
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_ACLRuleCfgTagCare
  Description: Set if the rule cares vlan tag of packet.
        Input: UINT32 uiAclRuleId  
                INT32 iCareTagType  
                BOOL bEnable      
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_ACLRuleCfgTagCare(UINT32 uiAclRuleId, INT32 iCareTagType, BOOL bEnable)
{
    rtk_acl_care_tag_index_t enRtkCareTagType;

    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (ACL_CARE_TAG_END <= iCareTagType)    
        )
    {
        return DRV_ERR_PARA;
    }

    enRtkCareTagType = (rtk_acl_care_tag_index_t)iCareTagType;

    ACL_MUTEX_LOCK;
    if (TRUE == bEnable)
    {
        m_astAclCfg[uiAclRuleId].careTag.tags[enRtkCareTagType].value = TRUE;
        m_astAclCfg[uiAclRuleId].careTag.tags[enRtkCareTagType].mask = TRUE;
    }
    else
    {
        m_astAclCfg[uiAclRuleId].careTag.tags[enRtkCareTagType].value = FALSE;
        m_astAclCfg[uiAclRuleId].careTag.tags[enRtkCareTagType].mask = FALSE;
    }
    ACL_MUTEX_LOCK;
    
    return DRV_OK;
}
static int _Hal_AclTemplatefind(rtk_acl_field_type_t * fieldType,int num)
{
	int tempindex=0,fieldindex=0,tempfeildindex=0;
	int matchnum=0;
	//rtk_acl_ingress_entry_t *pstRtlAclCfg = NULL;
	if(fieldType == NULL||num>MAX_ACL_FEILD_NUM||num<=1)
	{
		return -1;
	}
	
	for(tempindex = 0; tempindex < MAX_TEMPLATE_NUM; tempindex++)
	{		
		matchnum=0;
		for(fieldindex = 0; fieldindex < num; fieldindex++)
		{		
			for(tempfeildindex = 0; tempfeildindex < MAX_ACL_FEILD_NUM; tempfeildindex++)
			{
				if((fieldType[fieldindex]==m_astAclTemplate[tempindex].fieldType[tempfeildindex])||
					((fieldType[fieldindex]==ACL_FIELD_DMAC) && (m_astAclTemplate[tempindex].fieldType[tempfeildindex]==ACL_FIELD_DMAC0))
					|| ((fieldType[fieldindex]==ACL_FIELD_SMAC) && (m_astAclTemplate[tempindex].fieldType[tempfeildindex]==ACL_FIELD_SMAC0)))
				{
					matchnum++;
					break;
				}				
			}	
			if(matchnum==num)
			{
				return tempindex;
			}
			if(tempfeildindex==MAX_ACL_FEILD_NUM)
			{
				
				break;		
			}
		}
	}
	return tempindex;
}

static DRV_RET_E _Hal_AclTemplateBind(VOID *pstAclCfg)
{	
	unsigned char index = 0;
	int tempindex=0;
	unsigned char fieldindex;
	rtk_acl_field_type_t fieldType[RTK_MAX_NUM_OF_ACL_RULE_FIELD];
	rtk_acl_field_t *tmpPtr = NULL;
	rtk_acl_ingress_entry_t *pstRtlAclCfg = NULL;

	if (NULL == pstAclCfg)
    {
        return DRV_ERR_PARA;
    }
	
	pstRtlAclCfg = (rtk_acl_ingress_entry_t *)pstAclCfg;

	fieldType[index++] = pstRtlAclCfg->pFieldHead->fieldType;
	tmpPtr = pstRtlAclCfg->pFieldHead->next;
	while(tmpPtr != NULL)
	{
		fieldType[index++] = tmpPtr->fieldType;
		tmpPtr = tmpPtr->next;
	}
	
	if((index > 1))
	{
		/*Reserve*/
		tempindex=_Hal_AclTemplatefind(fieldType,index);
		if(tempindex>=MAX_TEMPLATE_NUM)
		{
			printk("\n fatal error func:%s,line:%d.\n",__FUNCTION__,__LINE__);
			return DRV_ERR_UNKNOW;
		}
		pstRtlAclCfg->templateIdx=tempindex;
		return DRV_OK;
	}
	else
	{
		if((ACL_FIELD_DMAC == pstRtlAclCfg->pFieldHead->fieldType) ||
			(ACL_FIELD_SMAC == pstRtlAclCfg->pFieldHead->fieldType))
		{
			pstRtlAclCfg->templateIdx = 0;
	    	return DRV_OK;
		}
		else if((ACL_FIELD_IPV4_SIP == pstRtlAclCfg->pFieldHead->fieldType) ||
				(ACL_FIELD_IPV4_DIP == pstRtlAclCfg->pFieldHead->fieldType))
		{
			pstRtlAclCfg->templateIdx = 1;
	    	return DRV_OK;
		}
		else
		{
			for(index = 0; index < MAX_TEMPLATE_NUM; index++)
			{
				for(fieldindex = 0; fieldindex < MAX_ACL_FEILD_NUM; fieldindex++)
				{						
					if(pstRtlAclCfg->pFieldHead->fieldType == m_astAclTemplate[index].fieldType[fieldindex])
					{
						pstRtlAclCfg->templateIdx = index;
		    			return DRV_OK;
					}
				}
			}
		}
		
	}

	return DRV_ERR_UNKNOW;
};

/*****************************************************************************
    Func Name: _Hal_AclRuleBind
  Description: Bind acl rule to ports.
        Input: UINT32 uiPortMask  
                UINT32 uiAclId     
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleBind(UINT32 uiPortMask, UINT32 uiAclId)
{
    rtk_acl_ingress_entry_t stAclCfg;
    rtk_api_ret_t RetVal = RT_ERR_OK;	

    if (ACL_RULE_NUM_MAX <= uiAclId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_AclCfgGet(uiAclId, (void *)&stAclCfg);

	/*begin add by shipeng 2013-05-12*/
	stAclCfg.valid = ENABLED;
	stAclCfg.index = uiAclId;

	_Hal_AclTemplateBind((void *)&stAclCfg);
	/*end add by shipeng 2013-05-12*/

    stAclCfg.activePorts.bits[0] = uiPortMask;
    stAclCfg.invert = ACL_INVERT_DISABLE;

    RetVal = rtk_acl_igrRuleEntry_add(&stAclCfg);
    if (RT_ERR_OK != RetVal)
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    (void)_Hal_AclCfgSet(uiAclId, (void *)&stAclCfg);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_AclRuleUnbind
  Description: Unbind acl rule.
        Input: UINT32 uiAclId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_AclRuleUnbind(UINT32 uiAclId)
{
    rtk_api_ret_t RetVal = RT_ERR_OK;

    if (ACL_RULE_NUM_MAX <= uiAclId)
    {
        return DRV_ERR_PARA;
    }
   
    (void)_Hal_AclRuleCfgClear(uiAclId);
    
 	RetVal = rtk_acl_igrRuleEntry_del(uiAclId);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterGet
  Description: Get filter cfg.
        Input: UINT32 uiLPortId                  
                PORT_FILTER_CFG_S *pstFilterMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterGet(UINT32 uiLPortId, PORT_FILTER_CFG_S *pstFilterMode)
{
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (NULL == pstFilterMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(pstFilterMode, &m_astUniFilterMode[uiLPortId], sizeof(PORT_FILTER_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterSet
  Description: Save filter cfg.
        Input: UINT32 uiLPortId                  
                PORT_FILTER_CFG_S *pstFilterMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterSet(UINT32 uiLPortId, PORT_FILTER_CFG_S *pstFilterMode)
{
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (NULL == pstFilterMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(&m_astUniFilterMode[uiLPortId], pstFilterMode, sizeof(PORT_FILTER_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterModeSet
  Description: Get filter mode.
        Input: UINT32 uiLPortId         
                UINT32 uiFilterIndex     
                ACL_TRUST_MODE_E enMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterModeSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, ACL_TRUST_MODE_E enMode)
{
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (ACL_TRUST_END < enMode)    ||
        (ACL_DIRECTION_END <= enAclDir) ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiFilterIndex))
    {
        return DRV_ERR_PARA;
    }
    
    if (ACL_DIRECTION_UPLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex] = enMode;
        ACL_MUTEX_UNLOCK;
    }
    else if (ACL_DIRECTION_DOWNLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX] = enMode;
        ACL_MUTEX_UNLOCK;
    }
    else
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex] = enMode;
        m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX] = enMode;
        ACL_MUTEX_UNLOCK;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterModeGet
  Description: Save filter mode,
        Input: UINT32 uiLPortId         
                UINT32 uiFilterIndex     
                ACL_TRUST_MODE_E *penMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterModeGet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, ACL_TRUST_MODE_E *penMode)
{
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (NULL == penMode)    ||
        (ACL_DIRECTION_END <= enAclDir) ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiFilterIndex))
    {
        return DRV_ERR_PARA;
    }

    if (ACL_DIRECTION_UPLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        *penMode = m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex];
        ACL_MUTEX_UNLOCK;
    }
    else 
    {
        ACL_MUTEX_LOCK;
        *penMode = m_astUniFilterMode[uiLPortId].aenFiletrkMode[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX];
        ACL_MUTEX_UNLOCK;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterRuleIdSet
  Description: Save rule id used bu filter.
        Input: UINT32 uiLPortId      
                UINT32 uiFilterIndex  
                UINT32 uiAclId        
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterRuleIdSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, UINT32 uiAclId)
{   
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (ACL_DIRECTION_END <= enAclDir) ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiFilterIndex))
    {
        return DRV_ERR_PARA;
    }

    if (ACL_DIRECTION_UPLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex] = uiAclId;
        ACL_MUTEX_UNLOCK;
    }
    else if (ACL_DIRECTION_DOWNLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX] = uiAclId;
        ACL_MUTEX_UNLOCK;
    }
    else
    {
        ACL_MUTEX_LOCK;
        m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex] = uiAclId;
        m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX] = uiAclId;
        ACL_MUTEX_UNLOCK;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortFilterRuleIdGet
  Description: Get rule id used for filter.
        Input: UINT32 uiLPortId      
                UINT32 uiFilterIndex  
                UINT32 *puiAclId      
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortFilterRuleIdGet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, UINT32 *puiAclId)
{
    if (((LOGIC_UPPON_PORT != uiLPortId) && (!IsValidLgcPort(uiLPortId))) ||
        (NULL == puiAclId)           ||
        (ACL_DIRECTION_END <= enAclDir) ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiFilterIndex))
    {
        return DRV_ERR_PARA;
    }

    if (ACL_DIRECTION_UPLINK == enAclDir)
    {
        ACL_MUTEX_LOCK;
        *puiAclId = m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex];
        ACL_MUTEX_UNLOCK;
    }
    else
    {
        ACL_MUTEX_LOCK;
        *puiAclId = m_astUniFilterMode[uiLPortId].auiAclList[uiFilterIndex+EOAM_FILTER_RULE_NUM_MAX];
        ACL_MUTEX_UNLOCK;
    }
    

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkGet
  Description: Get port's ctc classification mode.
        Input: UINT32 uiLPortId                        
                PORT_CLF_REMAERK_CFG_S *pstClfMarkMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkGet(UINT32 uiLPortId, PORT_CLF_REMAERK_CFG_S *pstClfMarkMode)
{
    if ((!IsValidLgcPort(uiLPortId))     ||
        (NULL == pstClfMarkMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(pstClfMarkMode, &m_astUniClfMarkMode[uiLPortId], sizeof(PORT_CLF_REMAERK_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkSet
  Description: Set port's ctc claccification mode.
        Input: UINT32 uiLPortId                        
                PORT_CLF_REMAERK_CFG_S *pstClfMarkMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkSet(UINT32 uiLPortId, PORT_CLF_REMAERK_CFG_S *pstClfMarkMode)
{
    if ((!IsValidLgcPort(uiLPortId))     ||
        (NULL == pstClfMarkMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(&m_astUniClfMarkMode[uiLPortId], pstClfMarkMode, sizeof(PORT_CLF_REMAERK_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkNumInc
  Description: Increase port's classification rule number.
        Input: UINT32 uiLPortId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkNumInc(UINT32 uiLPortId)
{
    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    if ((CTC_CLF_REMARK_RULE_NUM_MAX*2) > m_astUniClfMarkMode[uiLPortId].uiClfRmkRuleNum)
    {
        m_astUniClfMarkMode[uiLPortId].uiClfRmkRuleNum++;
    }
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkNumDec
  Description: Decrease port 's classification rule number.
        Input: UINT32 uiLPortId  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkNumDec(UINT32 uiLPortId)
{
    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    if (0 < m_astUniClfMarkMode[uiLPortId].uiClfRmkRuleNum)
    {
        m_astUniClfMarkMode[uiLPortId].uiClfRmkRuleNum--;
    }
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkNumGet
  Description: Get port's classification rule number.
        Input: UINT32 uiLPortId  
                UINT32 *puiNum    
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkNumGet(UINT32 uiLPortId, UINT32 *puiNum)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == puiNum))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    *puiNum = m_astUniClfMarkMode[uiLPortId].uiClfRmkRuleNum;
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkAclRuleIdSet
  Description: Save acl rule id used for classification.
        Input: UINT32 uiLPortId      
                UINT32 uiClfRmkIndex  
                UINT32 uiAclId        
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 uiAclId)
{   
    if ((!IsValidLgcPort(uiLPortId)) ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    m_astUniClfMarkMode[uiLPortId].auiAclList[uiClfRmkIndex] = uiAclId;
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkAclRuleIdGet
  Description: Get acl rule id used for classification.
        Input: UINT32 uiLPortId      
                UINT32 uiClfRmkIndex  
                UINT32 *puiAclId      
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
//static DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 *puiAclId)
DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 *puiAclId)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == puiAclId)           ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    *puiAclId = m_astUniClfMarkMode[uiLPortId].auiAclList[uiClfRmkIndex];
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkModeSet
  Description: Set port classification mode.
        Input: UINT32 uiLPortId         
                UINT32 uiClfRmkIndex     
                ACL_TRUST_MODE_E enMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkModeSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, ACL_TRUST_MODE_E enMode)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (ACL_TRUST_END < enMode)    ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    m_astUniClfMarkMode[uiLPortId].aenClfRemarkMode[uiClfRmkIndex] = enMode;
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortClfRmkModeGet
  Description: Get port classification mode.
        Input: UINT32 uiLPortId           
                UINT32 uiClfRmkIndex       
                ACL_TRUST_MODE_E *penMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortClfRmkModeGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, ACL_TRUST_MODE_E *penMode)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == penMode)    ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;
    *penMode = m_astUniClfMarkMode[uiLPortId].aenClfRemarkMode[uiClfRmkIndex];
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgAclCareTag
  Description: Set caring tag.
        Input: UINT32 uiAclRuleId                         
                rtk_filter_care_tag_index_t enCareTagType  
                BOOL bEnable                               
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
//static DRV_RET_E _Hal_CfgAclCareTag(UINT32 uiAclRuleId, rtk_filter_care_tag_index_t enCareTagType, BOOL bEnable)
static DRV_RET_E _Hal_CfgAclCareTag(UINT32 uiAclRuleId, rtk_acl_care_tag_index_t enCareTagType, BOOL bEnable)
{
    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (ACL_CARE_TAG_END <= enCareTagType)   ||
        ((TRUE != bEnable) && (FALSE != bEnable)))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;

    if (TRUE == bEnable)
    {
		m_astAclCfg[uiAclRuleId].careTag.tags[enCareTagType].value = TRUE;
        m_astAclCfg[uiAclRuleId].careTag.tags[enCareTagType].mask = TRUE;
    }
    else
    {
        m_astAclCfg[uiAclRuleId].careTag.tags[enCareTagType].value = FALSE;
        m_astAclCfg[uiAclRuleId].careTag.tags[enCareTagType].mask = FALSE;
    }

    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_CtcVlanEnoughVlanIdxEntryCheck
  Description: Check if there is enough vlan index entries for ctc tag and translation vlan mode.
        Input: CTC_VLAN_CFG_S *pstVlanMode  
                BOOL_T *pbEnough             
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CtcVlanEnoughVlanIdxEntryCheck(CTC_VLAN_CFG_S *pstVlanMode, UINT32 *puiEnough)
{
    ret_t tdRet;
    UINT32 i;
    UINT32 uiPvid;
    UINT32 uiOldvid;
    UINT32 uiNewVid;
    UINT32 uiEntryIdx;
    UINT32 uiNeedEntryNum = 0;
    UINT32 uiEmptyEntryNum = 0;
    UINT32 uiExistEntryNum = 0;
    UINT32 auiExistEntry[APOLLOMP_CVIDXNO];
    rtk_vlan_mbrcfg_t stVlanIdxEntry;

    if ((NULL == pstVlanMode) ||
        (NULL == puiEnough))
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    memset(auiExistEntry, 0xFF, sizeof(auiExistEntry));

    /*Get empty vlan entry number and existed vlan entries.*/
	for(uiEntryIdx = 0; uiEntryIdx <= APOLLOMP_CVIDXMAX; uiEntryIdx++)
	{		
		extern rtk_api_ret_t rtk_vlan_mbrCfg_get(rtk_uint32 idx,rtk_vlan_mbrcfg_t * pMbrcfg);
        tdRet = rtk_vlan_mbrCfg_get(uiEntryIdx, &stVlanIdxEntry);
	    if(RT_ERR_OK != tdRet)
	    {
            printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
	        return DRV_ERR_UNKNOW;
	    }

		if (0 == stVlanIdxEntry.mbr && 0 == stVlanIdxEntry.evid)
		{
			uiEmptyEntryNum++;
		}
        else
        {
            auiExistEntry[uiExistEntryNum++] = stVlanIdxEntry.evid;
        }
	}

    /*Get entry number needed pvid.*/
    uiPvid = pstVlanMode->default_vlan;
    for (i = 0; i < uiExistEntryNum; i++)
    {
        if (uiPvid == auiExistEntry[i])
        {
            break;
        }
    }
    if (i >= uiExistEntryNum)
    {
        uiNeedEntryNum++;
    }

    /*Get entry number needed for ctc translation.*/
    if (CTC_VLAN_MODE_TRANSLATION == pstVlanMode->mode) 
    {
        UINT32 j;
        
        for (i = 0; i < pstVlanMode->number_of_entries; i++)
        {
            /*Needed by old vlan.*/
            uiOldvid = pstVlanMode->vlan_list[i*2];
            for (j = 0; j < uiExistEntryNum; j++)
            {
                if (uiOldvid == auiExistEntry[j])
                {
                    break;
                }
            }
            if (j >= uiExistEntryNum)
            {
                uiNeedEntryNum++;
            }

            /*Needed by new vlan.*/
            uiNewVid = pstVlanMode->vlan_list[(i*2)+1];
            if (uiOldvid == uiNewVid)
            {
                continue;
            }
            
            for (j = 0; j < uiExistEntryNum; j++)
            {
                if (uiNewVid == auiExistEntry[j])
                {
                    break;
                }
            }
            if (j >= uiExistEntryNum)
            {
                uiNeedEntryNum++;
            }
        }
    }
    
    if (uiNeedEntryNum > uiEmptyEntryNum)
    {
        *puiEnough = FALSE;
    }
    else
    {
        *puiEnough = TRUE;
    }

    //printk("\nfunc:%s,line:%d,need:%d,free:%d,enough:%d.\n",__FUNCTION__,__LINE__,
    //         uiNeedEntryNum,uiEmptyEntryNum,*puiEnough);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleEmptyNumGet
  Description: Get empty acl rule num.
        Input: UINT32 *pAclEmptyNum  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleEmptyNumGet(UINT32 *pAclEmptyNum)
{
    UINT32 uiAclruleIndex;
    UINT32 uiAclruleEmptyNum = 0;
    rtk_acl_ingress_entry_t stRtlAclCfg;

    if (NULL == pAclEmptyNum)
    {
        return DRV_ERR_PARA;
    }
    
    for (uiAclruleIndex = 0; uiAclruleIndex < ACL_RULE_NUM_MAX; uiAclruleIndex++)
    {
        memset(&stRtlAclCfg, 0, sizeof(stRtlAclCfg));
        (void)_Hal_AclCfgGet(uiAclruleIndex, &stRtlAclCfg);
        if (NULL == stRtlAclCfg.pFieldHead)
        {
            uiAclruleEmptyNum++;
        }
    }

    *pAclEmptyNum = uiAclruleEmptyNum;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForTrapCpuCreate
  Description: Create acl rule for trapping frames to cpu.
        Input: BOOL bBroadEn
                ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForTrapRaisecomCpuCreate(BOOL bBroadEn, ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
	DRV_RET_E enRet;
	UINT32 uiPPort;
	UINT32 uiPPortMask = 0;
	UINT32 uiAclRuleId;
	UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	if ((NULL == pRuleValue)			 ||
		(ACL_TRUST_END <= uiAclRuleType) ||
		((TRUE != bBroadEn) && (FALSE != bBroadEn)))
	{
		return DRV_ERR_PARA;
	}
   
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
	if (DRV_OK != enRet)
	{
		return DRV_ERR_UNKNOW;
	}

	if (TRUE == bBroadEn)
	{
		enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, aucMacAdd, aucMacAdd);
		if (DRV_OK != enRet)
		{
			return DRV_ERR_UNKNOW;
		}
	}
	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_TRAP_TO_CPU, pRuleValue, pRuleValue);
	if (DRV_OK != enRet) 
	{
		return DRV_ERR_UNKNOW;
	}
	
	/*only pon port available*/
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
	
	enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
	if (DRV_OK != enRet) 
	{
		return DRV_ERR_UNKNOW;
	}

	return DRV_OK;
}
DRV_RET_E Hal_AclRuleForTrapCpuCreate(BOOL bBroadEn, ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
    DRV_RET_E enRet;
    UINT32 uiLPortId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
    UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    if ((NULL == pRuleValue)             ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        ((TRUE != bBroadEn) && (FALSE != bBroadEn)))
    {
        return DRV_ERR_PARA;
    }
   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    if (TRUE == bBroadEn)
    {
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, aucMacAdd, aucMacAdd);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_TRAP_TO_CPU, pRuleValue, pRuleValue);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

#if defined(CONFIG_ZBL_SNMP)

/*****************************************************************************
  Func Name: Hal_AclRuleForTrapSnmpBroatcast2Cpu
  Description: Create acl rule for Trap Snmp Broatcast  to cpu.
        Input: BOOL bBroadEn
                ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE:2014-05-12
  NAME:            
  DESCRIPTION:for bug 583                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForTrapSnmpBroatcast2Cpu(BOOL bBroadEn, ACL_TRUST_MODE_E uiAclRuleType, void *pRuleValue)
{
    DRV_RET_E enRet;
    UINT32 uiLPortId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
    UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    if ((NULL == pRuleValue) || ((TRUE != bBroadEn) && (FALSE != bBroadEn)))
    {
        return DRV_ERR_PARA;
    }
   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    if (TRUE == bBroadEn)
    {
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_COPY_TO_CPU, aucMacAdd, aucMacAdd);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType,  ACL_ACTION_COPY_TO_CPU, pRuleValue, pRuleValue);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);

    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}
#endif


/*****************************************************************************
    Func Name: Hal_AclRuleForCopyCpuCreate
  Description: Create acl rule for copy frames to cpu.
        Input: BOOL bBroadEn
                ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCopyCpuCreate(BOOL bBroadEn, ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
    DRV_RET_E enRet;
    UINT32 uiLPortId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
    UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    if ((NULL == pRuleValue)             ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        ((TRUE != bBroadEn) && (FALSE != bBroadEn)))
    {
        return DRV_ERR_PARA;
    }
   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    if (TRUE == bBroadEn)
    {
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_COPY_TO_CPU, aucMacAdd, aucMacAdd);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_COPY_TO_CPU, pRuleValue, pRuleValue);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForTrapCpuDelete
  Description: Delete acl rule for trapping frames to cpu.
        Input: ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForTrapCpuDelete(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    rtk_acl_ingress_entry_t stRtlAclCfg;
	rtk_acl_igr_act_t stRtlAclAct;

    if ((NULL == pRuleValue) ||
        (ACL_TRUST_END <= uiAclRuleType))
    {
        return DRV_ERR_PARA;
    }
    /* For lint. */
    uiAclRuleType = uiAclRuleType;

    /* Get acl rule id. */
    for (uiAclRuleId = 0; ACL_RULE_NUM_MAX > uiAclRuleId; uiAclRuleId++)
    {
        if (DRV_OK == _Hal_AclActionCfgGet(uiAclRuleId, &stRtlAclAct))
        {
            if ((ENABLED == stRtlAclAct.enableAct[ACL_IGR_FORWARD_ACT]) 
				&& (ACL_IGR_FORWARD_TRAP_ACT == stRtlAclAct.forwardAct.act))
            {
                if (DRV_OK == _Hal_AclCfgGet(uiAclRuleId, &stRtlAclCfg))
                {
                    if ((ACL_FIELD_ETHERTYPE == stRtlAclCfg.pFieldHead->fieldType) &&
                        ((*(UINT32 *)pRuleValue) == stRtlAclCfg.pFieldHead->fieldUnion.data.value))
                    {
                        break;
                    }
                }
            }
        }
    }

    /* Do not find the rule. */
    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_OK;
    }
    
    /*Delete acl rule.*/

    enRet = _Hal_AclRuleUnbind(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    
    enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForTrapCpuCreate
  Description: Create acl rule for trapping frames to cpu.
        Input: BOOL bBroadEn
                ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
added by liaohongjun 2012/11/30 of EPN104QID0084                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForTrapLoopd2CpuCreate(BOOL bBroadEn, ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
    DRV_RET_E enRet;
    UINT32 uiLPortId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
    UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    UINT32 uiEthTypePri = 5;/*Modified by linguobin for Bug 262*/

    if ((NULL == pRuleValue)             ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        ((TRUE != bBroadEn) && (FALSE != bBroadEn)))
    {
        return DRV_ERR_PARA;
    }

    #if 1   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    #else
    uiAclRuleId = ACL_RULE_ID_LOOPD_TRAPCPU;
    #endif
    
    if (TRUE == bBroadEn)
    {
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, aucMacAdd, aucMacAdd);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_TRAP_TO_CPU, pRuleValue, pRuleValue);
    /*Begin modified by linguobin for Bug 262*/
	if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_REMATK_PRIORITY, pRuleValue, &uiEthTypePri);
    /*End modified by linguobin for Bug 262*/
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}
/*****************************************************************************
    Func Name: _Hal_AclRuleForDropLoopdDelete
  Description: Delete acl rule for drop loopback detection frames.
        Input: ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
  added by liaohongjun 2012/11/30 of EPN104QID0084                                                                          
*****************************************************************************/
DRV_RET_E _Hal_AclRuleForDropLoopdDelete(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    rtk_acl_ingress_entry_t stRtlAclCfg;
	rtk_acl_igr_act_t stRtlAclAct;

    if ((NULL == pRuleValue) ||
        (ACL_TRUST_END <= uiAclRuleType))
    {
        return DRV_ERR_PARA;
    }
    /* For lint. */
    uiAclRuleType = uiAclRuleType;

    /* Get acl rule id. */
    for (uiAclRuleId = 0; ACL_RULE_NUM_MAX > uiAclRuleId; uiAclRuleId++)
    {
        if (DRV_OK == _Hal_AclActionCfgGet(uiAclRuleId, &stRtlAclAct))
        {
            if ((ENABLED == stRtlAclAct.enableAct[ACL_IGR_FORWARD_ACT]) 
				&& (ACL_IGR_FORWARD_REDIRECT_ACT == stRtlAclAct.forwardAct.act) 
				&& (0 == stRtlAclAct.forwardAct.portMask.bits[0]))
            {
                if (DRV_OK == _Hal_AclCfgGet(uiAclRuleId, &stRtlAclCfg))
                {
                    if ((ACL_FIELD_ETHERTYPE == stRtlAclCfg.pFieldHead->fieldType) &&
                        ((*(UINT32 *)pRuleValue) == stRtlAclCfg.pFieldHead->fieldUnion.data.value))
                    {
                        break;
                    }
                }
            }
        }
    }

    /* Do not find the rule. */
    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_OK;
    }
    
    /*Delete acl rule.*/

    enRet = _Hal_AclRuleUnbind(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    
    enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************
    Function:_Hal_AclRuleForDropLoopdCreate
    Description: 创建丢弃loopd进程发送的环路检测报文ACL
                      规则
    Author:liaohongjun
    Date:2012/11/29
    Input:  UINT32 lgcPort
                ACL_TRUST_MODE_E uiAclRuleType  
                void *pRuleValue
                DROP_LOOPD_OP_E uiDropLoopdOp    
    Output:         
    Return:
        DRV_ERR_PARA
        DRV_ERR_UNKNOW
        DRV_OK
=================================================
    Note:  added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************/
DRV_RET_E _Hal_AclRuleForDropLoopdCreate(UINT32 lgcPort, ACL_TRUST_MODE_E uiAclRuleType, void *pRuleValue, DROP_LOOPD_OP_E uiDropLoopdOp)

{
    DRV_RET_E enRet;
    UINT32 uiPPort;
    UINT32 uiAclRuleId;
    
    if ((NULL == pRuleValue) || (ACL_TRUST_END <= uiAclRuleType) || 
        (!IsValidLgcPort(lgcPort)) || (ACL_DROPLOOPD_END <= uiDropLoopdOp))
    {
        return DRV_ERR_PARA;
    }

    enRet = _Hal_AclRuleForDropLoopdDelete(uiAclRuleType, pRuleValue);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    } 

    //printk("[%s,%d]:locPorrt = %d,operation = %d\n",__FUNCTION__,__LINE__,lgcPort,uiDropLoopdOp);
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    } 
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_DROP, pRuleValue, pRuleValue);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }


    uiPPort = PortLogic2PhyPortId(lgcPort);
    switch(uiDropLoopdOp)
    {
        case ACL_DROPLOOPD_ADD_PORT:
            ACL_MUTEX_LOCK;
            g_uiDropLoopdPPMask |= (1U << uiPPort); 
            ACL_MUTEX_UNLOCK;  
            break;
        case ACL_DROPLOOPD_REMOVE_PORT:
            ACL_MUTEX_LOCK;
            g_uiDropLoopdPPMask &= ~(1U << uiPPort); 
            ACL_MUTEX_UNLOCK;
            break;
        default:
            return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_AclRuleBind(g_uiDropLoopdPPMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}
/*****************************************************************
    Function:Hal_AclRuleForDropLoopdOperation
    Description: 添加或删除丢弃loopd进程发送的环路检测
                     报文ACL 规则端口
    Author:liaohongjun
    Date:2012/11/29
    Input:  UINT32 lgcPort
                DROP_LOOPD_OP_E uiDropLoopdOp    
    Output:         
    Return:
        DRV_ERR_PARA
        DRV_ERR_UNKNOW
        DRV_OK
=================================================
    Note: added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************/
DRV_RET_E Hal_AclRuleForDropLoopdOperation(UINT32 lgcPort, DROP_LOOPD_OP_E uiDropLoopdOp)
{      
    DRV_RET_E enRet;
	UINT32 uiEthTypeLoopd = 0x9001;
    
    if ( (!IsValidLgcPort(lgcPort)) || (ACL_DROPLOOPD_END <= uiDropLoopdOp))
    {
        return DRV_ERR_PARA;
    }

    enRet = _Hal_AclRuleForDropLoopdCreate(lgcPort, ACL_TRUST_ETHTYPE, &uiEthTypeLoopd, uiDropLoopdOp);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
    
    return DRV_OK;
}
/*****************************************************************
    Function:Hal_AclRuleForDropLoopdRemove
    Description: 删除丢弃loopd进程发送的环路检测报文ACL
                      规则
    Author:liaohongjun
    Date:2012/11/29
    Input:  UINT32 lgcPort
                DROP_LOOPD_OP_E uiDropLoopdOp    
    Output:         
    Return:
        DRV_ERR_PARA
        DRV_ERR_UNKNOW
        DRV_OK
=================================================
    Note: added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************/
DRV_RET_E Hal_AclRuleForDropLoopdRemove(VOID)
{      
    DRV_RET_E enRet;
	UINT32 uiEthTypeLoopd = 0x9001;

    enRet = _Hal_AclRuleForDropLoopdDelete(ACL_TRUST_ETHTYPE, &uiEthTypeLoopd);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
    
    ACL_MUTEX_LOCK;
    g_uiDropLoopdPPMask = 0; 
    ACL_MUTEX_UNLOCK; 
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcClfRmkCreate
  Description: Create acl rule for ctc classification.
        Input: UINT32 uiLPortId                
                UINT32 uiRulePrecedence         
                ACL_TRUST_MODE_E uiAclRuleType  
                VOID *pRuleValue                
                VOID *pRemarkPri                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcClfRmkCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   VOID *pRuleValue,
                                   VOID *pRemarkPri)
{
    DRV_RET_E enRet;
    UINT32 uiPPort;
    UINT32 uiPortMask;
    UINT32 uiAclRuleId;
	rtk_acl_ingress_entry_t stAclCfg;

    if ((!IsValidLgcPort(uiLPortId))     ||
        (NULL == pRemarkPri)             ||
        (NULL == pRuleValue)             ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiRulePrecedence))
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_CfgPortClfRmkAclRuleIdGet(uiLPortId, uiRulePrecedence, &uiAclRuleId);
    if (ACL_RULE_ID_IVALLID != uiAclRuleId)
    {
        if (DRV_OK != _Hal_AclRuleActivePortsGet(uiAclRuleId, &uiPortMask))
        {
            return DRV_ERR_UNKNOW;
        }

        if (0 != uiPortMask)
        {
            enRet = _Hal_AclRuleUnbind(uiAclRuleId);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }
    
            enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }

            (void)_Hal_CfgPortClfRmkNumDec(uiLPortId);
            (void)_Hal_CfgPortClfRmkModeSet(uiLPortId, uiRulePrecedence, ACL_TRUST_END);
            (void)_Hal_CfgPortClfRmkAclRuleIdSet(uiLPortId, uiRulePrecedence, ACL_RULE_ID_IVALLID);
        }
    }
    
    
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPortMask = (1U << uiPPort);
    
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_REMATK_PRIORITY, pRuleValue, pRemarkPri);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
#if 1
	(void)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);
	stAclCfg.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    stAclCfg.act.aclLatch = ENABLED;
	(void)_Hal_AclCfgSet(uiAclRuleId, (void *)&stAclCfg);
#endif	
    enRet = _Hal_AclRuleBind(uiPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    (void)_Hal_CfgPortClfRmkNumInc(uiLPortId);
    (void)_Hal_CfgPortClfRmkModeSet(uiLPortId, uiRulePrecedence, uiAclRuleType);
    (void)_Hal_CfgPortClfRmkAclRuleIdSet(uiLPortId, uiRulePrecedence, uiAclRuleId);

    return DRV_OK;
}

/*****************************************************************************
 函 数 名  : Hal_AclRuleForCtcClfRmkDelete
 输入参数  : UINT32 uiLPortId         
             UINT32 uiRulePrecedence  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcClfRmkDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    ACL_TRUST_MODE_E enAclMode;

    if ((!IsValidLgcPort(uiLPortId))     ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiRulePrecedence))
    {
        return DRV_ERR_PARA;
    }

    enRet = _Hal_CfgPortClfRmkModeGet(uiLPortId, uiRulePrecedence, &enAclMode);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW; 
    }

    if (ACL_TRUST_END == enAclMode)
    {
        return DRV_OK;
    }

    enRet = _Hal_CfgPortClfRmkAclRuleIdGet(uiLPortId, uiRulePrecedence, &uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }

    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_ERR_UNKNOW;
    }

    /*Delete acl rule of this port.*/
    if (ACL_RULE_ID_IVALLID != uiAclRuleId)
    {
        enRet = _Hal_AclRuleUnbind(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    
        enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        (void)_Hal_CfgPortClfRmkNumDec(uiLPortId);
        (void)_Hal_CfgPortClfRmkModeSet(uiLPortId, uiRulePrecedence, ACL_TRUST_END);
        (void)_Hal_CfgPortClfRmkAclRuleIdSet(uiLPortId, uiRulePrecedence, ACL_RULE_ID_IVALLID);
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcClfRmkClear
  Description: Delete acl rule for classification.
        Input: UINT32 uiLPortId          
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcClfRmkClear(UINT32 uiLPortId)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    UINT32 uiRulePrecedence;
    ACL_TRUST_MODE_E enAclMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    for (uiRulePrecedence = 0; uiRulePrecedence < CTC_CLF_REMARK_RULE_NUM_MAX*2; uiRulePrecedence++)
    {
        enRet = _Hal_CfgPortClfRmkModeGet(uiLPortId, uiRulePrecedence, &enAclMode);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW; 
        }

        if (ACL_TRUST_END <= enAclMode)
        {
            //printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            continue;
        }

        enRet = _Hal_CfgPortClfRmkAclRuleIdGet(uiLPortId, uiRulePrecedence, &uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;  
        }

        if (ACL_RULE_NUM_MAX <= uiAclRuleId)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
    
        /*Delete acl rule of this port.*/
        if (ACL_RULE_ID_IVALLID != uiAclRuleId)
        {
            enRet = _Hal_AclRuleUnbind(uiAclRuleId);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }
        
            enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }

            (void)_Hal_CfgPortClfRmkNumDec(uiLPortId);
            (void)_Hal_CfgPortClfRmkModeSet(uiLPortId, uiRulePrecedence, ACL_TRUST_END);
            (void)_Hal_CfgPortClfRmkAclRuleIdSet(uiLPortId, uiRulePrecedence, ACL_RULE_ID_IVALLID);
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForEoamFilterCreate
  Description: Create acl rule for eoam filter.
        Input: UINT32 uiLPortId                
                UINT32 uiRulePrecedence    
                ACL_TRUST_MODE_E uiAclRuleType  
                ACL_DIRECTION_E enAclDir
                VOID *pRuleValue                
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION 

  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForEoamFilterCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   ACL_DIRECTION_E enAclDir,
                                   VOID *pRuleValue)
{
    DRV_RET_E enRet;
    //logic_pmask_t uiPortMask;
    UINT32 uiAclRuleId;
    UINT32 lport, pport;
    UINT32 uiLPortTmp;
    UINT32 uiPPortMask = 0;
    ACL_TRUST_MODE_E enTrustMode;

    if (((!IsValidLgcPort(uiLPortId)) && (INVALID_PORT != uiLPortId) && (LOGIC_UPPON_PORT != uiLPortId)) ||
        (NULL == pRuleValue)                             ||
        (ACL_TRUST_END <= uiAclRuleType)                 ||
        (ACL_DIRECTION_END <= enAclDir)                  ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiRulePrecedence))
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    switch (enAclDir)
    {
        case ACL_DIRECTION_UPLINK:
            if (INVALID_PORT == uiLPortId)
            {
                /*Just for eoam filter.*/
                uiLPortTmp = MAX_PORT_NUM;

                /*all unis*/
                LgcPortFor(lport)
                {
                    pport = PortLogic2PhyPortId(lport);
                    uiPPortMask |= (1U << pport);
                }
            }
            else
            {
                uiLPortTmp = uiLPortId;
                /*Specific user interface or upon-port*/
                pport = PortLogic2PhyPortId(uiLPortId);
                uiPPortMask |= (1U << pport);
            }
            break;
        case ACL_DIRECTION_DOWNLINK:
            /*Just for eoam filter.*/
            uiLPortTmp = LOGIC_UPPON_PORT;

            /*upon-port*/
            pport = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
            uiPPortMask |= (1U << pport);
            break;
        case ACL_DIRECTION_BOTH:
            /*Just for eoam filter.*/
            uiLPortTmp = LOGIC_UPPON_PORT;
            
            if (INVALID_PORT == uiLPortId)
            {
                /*all unis*/
                LgcPortFor(lport)
                {
                    pport = PortLogic2PhyPortId(lport);
                    uiPPortMask |= (1U << pport);
                }
            }
            else
            {
                /*Specific user interface or upon-port*/
                pport = PortLogic2PhyPortId(uiLPortId);
                uiPPortMask |= (1U << pport);
            }

            /*upon-port*/
            pport = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
            uiPPortMask |= (1U << pport);
            break;
        default :
            return DRV_ERR_UNKNOW;
    }
    
    (void)_Hal_CfgPortFilterModeGet(uiLPortTmp, uiRulePrecedence, enAclDir, &enTrustMode);
    (void)_Hal_CfgPortFilterRuleIdGet(uiLPortTmp, uiRulePrecedence, enAclDir, &uiAclRuleId);
    if ((ACL_TRUST_END != enTrustMode) && 
        (ACL_RULE_ID_IVALLID != uiAclRuleId))
    {
        enRet = _Hal_AclRuleUnbind(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        } 
    }

#if 0
    if (INVALID_PORT == uiLPortId)
    {
        /*all unis*/
        LgcPortFor(lport)
        {
            pport = PortLogic2PhyPortId(uiLPortId);
            uiPPortMask |= (1U << pport);
        }
    }
    else
    {
        /*Specific user interface or upon-port*/
        pport = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << pport);
    }
#endif

    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_DROP, pRuleValue, pRuleValue);
    if (DRV_OK != enRet) 
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    
    /*tcp/udp src-port or des-port is saved in the same acl rule field, 
      so the tcp or udp tag must be cared.*/
    switch (uiAclRuleType)
    {
        case ACL_TRUST_TCP_SPORT:
        case ACL_TRUST_TCP_DPORT:
            enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_TCP, TRUE);
            break;
        case ACL_TRUST_UDP_SPORT:
        case ACL_TRUST_UDP_DPORT:
            enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_UDP, TRUE);
            break;
        default :
            enRet = DRV_OK;
            break;
    }
    if (DRV_OK != enRet) 
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    /*014063 */
    
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
 
    switch (enAclDir)
    {
        case ACL_DIRECTION_UPLINK:
            if (INVALID_PORT == uiLPortId)
            {
                /*all unis*/
                LgcPortFor(lport)
                {
                    (void)_Hal_CfgPortFilterModeSet(lport, uiRulePrecedence, enAclDir, uiAclRuleType);
                    (void)_Hal_CfgPortFilterRuleIdSet(lport, uiRulePrecedence, enAclDir, uiAclRuleId); 
                }
            }
            else
            {
                /*Specific user interface or upon-port*/
               (void)_Hal_CfgPortFilterModeSet(uiLPortId, uiRulePrecedence, enAclDir, uiAclRuleType);
               (void)_Hal_CfgPortFilterRuleIdSet(uiLPortId, uiRulePrecedence, enAclDir, uiAclRuleId);  
            }
            break;
        case ACL_DIRECTION_DOWNLINK:
            /*upon-port*/
            (void)_Hal_CfgPortFilterModeSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, uiAclRuleType);
            (void)_Hal_CfgPortFilterRuleIdSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, uiAclRuleId); 
            break;
        case ACL_DIRECTION_BOTH:
            if (INVALID_PORT == uiLPortId)
            {
                /*all unis*/
                LgcPortFor(lport)
                {
                    (void)_Hal_CfgPortFilterModeSet(lport, uiRulePrecedence, enAclDir, uiAclRuleType);
                    (void)_Hal_CfgPortFilterRuleIdSet(lport, uiRulePrecedence, enAclDir, uiAclRuleId); 
                }
            }
            else
            {
                /*Specific user interface or upon-port*/
               (void)_Hal_CfgPortFilterModeSet(uiLPortId, uiRulePrecedence, enAclDir, uiAclRuleType);
               (void)_Hal_CfgPortFilterRuleIdSet(uiLPortId, uiRulePrecedence, enAclDir, uiAclRuleId);  
            }

            /*upon-port*/
            (void)_Hal_CfgPortFilterModeSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, uiAclRuleType);
            (void)_Hal_CfgPortFilterRuleIdSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, uiAclRuleId); 
            break;
        default :
            printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForEoamFilterDelete
  Description: Delete acl rule used by eoam filter.
        Input: UINT32 uiLPortId         
                UINT32 uiRulePrecedence  
                ACL_DIRECTION_E enAclDir
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
 
*****************************************************************************/
DRV_RET_E Hal_AclRuleForEoamFilterDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence, ACL_DIRECTION_E enAclDir)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    //logic_pmask_t uiPortMask;
    UINT32 lport;
    UINT32 uiLPortTmp;
    ACL_TRUST_MODE_E enTrustMode;

    if (((!IsValidLgcPort(uiLPortId)) && (INVALID_PORT != uiLPortId) && (LOGIC_UPPON_PORT != uiLPortId)) ||
        (EOAM_FILTER_RULE_NUM_MAX <= uiRulePrecedence) ||
        (ACL_DIRECTION_END <= enAclDir))
    {
        return DRV_ERR_PARA;
    }

    switch (enAclDir)
    {
        case ACL_DIRECTION_UPLINK:
            if (INVALID_PORT == uiLPortId)
            {
                /*Just for eoam filter.*/
                uiLPortTmp = MAX_PORT_NUM;
            }
            else
            {
                uiLPortTmp = uiLPortId;
            }
            break;
        case ACL_DIRECTION_DOWNLINK:
            /*Just for eoam filter.*/
            uiLPortTmp = LOGIC_UPPON_PORT;
            break;
        case ACL_DIRECTION_BOTH:
            /*Just for eoam filter.*/
            uiLPortTmp = LOGIC_UPPON_PORT;
            break;
        default :
            return DRV_ERR_UNKNOW;
    }
    
    (void)_Hal_CfgPortFilterModeGet(uiLPortTmp, uiRulePrecedence, enAclDir, &enTrustMode);
    (void)_Hal_CfgPortFilterRuleIdGet(uiLPortTmp, uiRulePrecedence, enAclDir, &uiAclRuleId);
    //printk("\nfunc:%s,line:%d,port:%d,mode:%d,aclid:%d\n",__FUNCTION__,__LINE__,uiLPortTmp,
    //          enTrustMode,uiAclRuleId);
    if ((ACL_TRUST_END != enTrustMode) && 
        (ACL_RULE_ID_IVALLID != uiAclRuleId))
    {
        enRet = _Hal_AclRuleUnbind(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        } 

        switch (enAclDir)
        {
            case ACL_DIRECTION_UPLINK:
                if (INVALID_PORT == uiLPortId)
                {
                    LgcPortFor(lport)
                    {
                        (void)_Hal_CfgPortFilterModeSet(lport, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                        (void)_Hal_CfgPortFilterRuleIdSet(lport, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                    }
                }
                else
                {
                    (void)_Hal_CfgPortFilterModeSet(uiLPortId, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                    (void)_Hal_CfgPortFilterRuleIdSet(uiLPortId, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                }
                break;
            case ACL_DIRECTION_DOWNLINK:
                (void)_Hal_CfgPortFilterModeSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                (void)_Hal_CfgPortFilterRuleIdSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                break;
            case ACL_DIRECTION_BOTH:
                if (INVALID_PORT == uiLPortId)
                {
                    LgcPortFor(lport)
                    {
                        (void)_Hal_CfgPortFilterModeSet(lport, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                        (void)_Hal_CfgPortFilterRuleIdSet(lport, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                    }
                }
                else
                {
                    (void)_Hal_CfgPortFilterModeSet(uiLPortId, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                    (void)_Hal_CfgPortFilterRuleIdSet(uiLPortId, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                }

                (void)_Hal_CfgPortFilterModeSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, ACL_TRUST_END);
                (void)_Hal_CfgPortFilterRuleIdSet(LOGIC_UPPON_PORT, uiRulePrecedence, enAclDir, ACL_RULE_ID_IVALLID);
                break;
            default :
                return DRV_ERR_UNKNOW;
        }
    }

    return DRV_OK;
}

DRV_RET_E __HAL_CreateCPUPacketPRI(ACL_TRUST_MODE_E uiAclRuleType, VOID *pRuleValue, VOID *pRemarkPri)
{
	UINT32 lport,pport;
    UINT32 uiPortMask;
    UINT32 uiAclRuleId;
	DRV_RET_E enRet;

	LgcPortFor(lport)
    {
        pport = PortLogic2PhyPortId(lport);
        uiPortMask |= (1U << pport);
    }
	
     pport = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
     uiPortMask |= (1U << pport);
    
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, uiAclRuleType, ACL_ACTION_REMATK_PRIORITY, pRuleValue, pRemarkPri);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_AclRuleBind(uiPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

 	return DRV_OK;
}

/*****************************************************************************
  函 数 名: _HAL_GetValidRateShareMeterIndex
  函数描述: 获取有效的rate share meter index
  输入参数: UINT32 uiRate
            BOOL *pbFind
            UINT32 *pIndex  
  输出参数: 无
    返回值: STATIC
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
STATIC INT32 _HAL_GetValidRateShareMeterIndex(UINT32 uiRate, BOOL *pbFind, UINT32 *pIndex)
{
    UINT32 i;
    UINT32 uiEmptyIndex = HAL_MAX_NUM_OF_METERING();
    rtk_rate_t Rate = 0;
    rtk_enable_t IncludeIfg;
    rtk_api_ret_t Ret;
    
    *pbFind = FALSE;
    
    for (i = 0; i < HAL_MAX_NUM_OF_METERING(); i++)
    {
        Rate = 0;
        Ret = rtk_rate_shareMeter_get(i, &Rate, &IncludeIfg);
        if (RT_ERR_OK != Ret)
        {
            printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if ((0 == Rate) &&
            (HAL_MAX_NUM_OF_METERING() == uiEmptyIndex))
        {
            uiEmptyIndex = i;
        }
        else if (uiRate == (UINT32)Rate)
        {
            uiEmptyIndex = i;
            *pbFind = TRUE;
        }
    }

    *pIndex = uiEmptyIndex;

    return DRV_OK;
}


/*****************************************************************************
  函 数 名: ACL_CreateRuleForArpRateLimit
  函数描述: 创建入口限速的acl规则
  输入参数: UINT32 uiPPortMask  
               UINT32 uiRateLimit  
  输出参数: 无
    返回值: STATIC
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
STATIC INT32 ACL_CreateRuleForArpRateLimit(UINT32 uiPPortMask, UINT32 uiRateLimit)
{
    BOOL bFind = FALSE;
    INT32 iRet = DRV_OK;
    UINT32 Index;
    UINT32 uiAclRuleId;
    UINT32 uiEthType = 0x0806;
    rtk_api_ret_t Ret;

    /* Set rate share meter. */
    iRet = _HAL_GetValidRateShareMeterIndex(uiRateLimit, &bFind, &Index);
    if (DRV_OK != iRet)  
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (HAL_MAX_NUM_OF_METERING() <= Index)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (FALSE == bFind)
    {
        Ret = rtk_rate_shareMeter_set(Index, uiRateLimit, ENABLED);
        if (RT_ERR_OK != Ret)
        {
            printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
    }
    
    /* Add acl rule. */
    iRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_POLICING_0, (VOID *)&uiEthType, (VOID *)&Index);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
  函 数 名: ACL_CreateRuleForBpduRateLimit
  函数描述: 创建BPDU报文入口限速的acl规则
  输入参数: UINT32 uiPPortMask  
            UINT32 uiRateLimit  
  输出参数: 无
    返回值: STATIC
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
STATIC INT32 ACL_CreateRuleForBpduRateLimit(UINT32 uiPPortMask, UINT32 uiRateLimit)
{
    BOOL bFind = FALSE;
    INT32 iRet = DRV_OK;
    UINT32 Index;
    UINT32 uiAclRuleId;
    UINT8  aucBpduMac[MAC_ADDR_LEN] = {0x01,0x80,0xC2,0x00,0x00,0x00};
    UINT8  aucMask[MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0x00};
    rtk_api_ret_t Ret;

    /* Set rate share meter. */
    iRet = _HAL_GetValidRateShareMeterIndex(uiRateLimit, &bFind, &Index);
    if (DRV_OK != iRet)  
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (HAL_MAX_NUM_OF_METERING() <= Index)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (FALSE == bFind)
    {
        Ret = rtk_rate_shareMeter_set(Index, uiRateLimit, ENABLED);
        if (RT_ERR_OK != Ret)
        {
            printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
    }
    
    /* Add acl rule. */
    iRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleMacCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_POLICING_0, 
                               (VOID *)aucBpduMac, (VOID *)aucMask, (VOID *)&Index);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

DRV_RET_E ACL_CreateRuleForDropVid(UINT32 uiLPortId, UINT32 uiVid, UINT32 *puiAclId)
{
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
    DRV_RET_E enRet = DRV_OK;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (!VALID_VLAN_ID(uiVid))      ||
        (NULL == puiAclId))
    {
        return DRV_ERR_PARA;
    }

    *puiAclId = ACL_RULE_NUM_MAX;
    
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    if (INVALID_PORT == uiPPort)  
    {
        return DRV_ERR_UNKNOW; 
    }

    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW; 
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_DROP, (VOID *)&uiVid, (VOID *)&uiVid);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW; 
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    uiPPortMask = 1U << uiPPort;
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    *puiAclId = uiAclRuleId;

    return DRV_OK;
}

DRV_RET_E ACL_DeleteRuleByAclid(UINT32 uiAclRuleId)
{
    DRV_RET_E enRet = DRV_OK;
    
    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_AclRuleUnbind(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }

    enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }

    return DRV_OK;
}

DRV_RET_E ACL_CreateRuleForStagToCpu()
{
	UINT32 uiAclRuleId=0;
	UINT32 uiPPort=0;
	int enRet=0;
	UINT32 uiPPortMask;
	UINT32 uiEthTypeArp = 0x0806;
	UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	extern unsigned char fe_addr[6];
	/*dst mac==cpu mac*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, fe_addr, fe_addr);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	uiPPortMask = 0;
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	/*arp*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_COPY_TO_CPU, aucMacAdd, &aucMacAdd);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_COPY_TO_CPU, &uiEthTypeArp, &uiEthTypeArp);
	if (DRV_OK != enRet) 
	{
		return DRV_ERR_UNKNOW;
	}	
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

	return DRV_OK;
}

#if 0
/*****************************************************************************
    Func Name: ACL_CreateRuleForDhcpRateLimit
  Description: Create acl rule for dhcp rate limit.
        Input: UINT32 uiPPortMask  
                UINT32 uiRateLimit  
       Output: 
       Return: STATIC
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
STATIC INT32 ACL_CreateRuleForDhcpRateLimit(UINT32 uiPPortMask, UINT32 uiRateLimit)
{
    BOOL bFind = FALSE;
    INT32 iRet = DRV_OK;
    UINT32 Index;
    UINT32 uiAclRuleId;
    UINT32 uiUdpuSPort;
    rtk_api_ret_t Ret;

    /* Set rate share meter. */
    iRet = _HAL_GetValidRateShareMeterIndex(uiRateLimit, &bFind, &Index);
    if (DRV_OK != iRet)  
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (RTK_MAX_NUM_OF_METER <= Index)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    if (FALSE == bFind)
    {
        Ret = rtk_rate_shareMeter_set(Index, uiRateLimit, ENABLED);
        if (RT_ERR_OK != Ret)
        {
            printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
    }

    /*For dhcp req*/
    /* Add acl rule. */
    iRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    
    uiUdpuSPort = 68;
    iRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_POLICING_0, (VOID *)&uiUdpuSPort, (VOID *)&Index);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    /*For dhcp ack*/
    /* Add acl rule. */
    iRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    
    uiUdpuSPort = 67;
    iRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_POLICING_0, (VOID *)&uiUdpuSPort, (VOID *)&Index);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    iRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != iRet)
    {
        printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}
#endif

static void Hal_Acl_FieldSelect_init(void)
{
	rtk_acl_field_entry_t fieldSel;
	
	fieldSel.index = 7;
	fieldSel.format = ACL_FORMAT_IPPAYLOAD;
	fieldSel.offset = 0;
	rtk_acl_fieldSelect_set(&fieldSel);/*ACL_FIELD_TCP_SPORT*//*ACL_FIELD_UDP_SPORT*/

	fieldSel.index = 8;
	fieldSel.format = ACL_FORMAT_IPPAYLOAD;
	fieldSel.offset = 2;
	rtk_acl_fieldSelect_set(&fieldSel);/*ACL_FIELD_TCP_DPORT*//*ACL_FIELD_UDP_DPORT*/

	fieldSel.index = 9;
	fieldSel.format = ACL_FORMAT_IPV4;
	fieldSel.offset = 1;
	rtk_acl_fieldSelect_set(&fieldSel);/*ACL_TRUST_IPV4_TOS*/

	fieldSel.index = 10;
	fieldSel.format = ACL_FORMAT_IPV4;
	fieldSel.offset = 9;
	rtk_acl_fieldSelect_set(&fieldSel);/*ACL_TRUST_IPV4_PROTOCOL*/
}
DRV_RET_E __Hal_CreateRuleForCpuMac(void);

#ifdef CONFIG_EOC_EXTEND
    void Hal_EocLowLevelFunctionInit(void);   
#endif /* EOC_PORTING */

/*****************************************************************************
    Func Name: Hal_AclRuleInit
  Description: Initiate default acl rules.
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleInit(void)
{
	UINT32 uiLPortId;
    UINT32 uiPPort;
    UINT32 uiPPortMask;
    /*begin modified by liaohongjun 2012/11/30 of EPN104QID0084*/
	UINT32 uiEthTypeLoopd = 0x9001;
	UINT32 uiRaisecomPkt = 0xfefe;
	
#if 1
    #define DHCP_SERVER_PORT 67
    #define DHCP_CLIENT_PORT 68

    UINT32 uiEthTypeArp = 0x0806;	
    UINT32 uiUdpPort;
	UINT32 uiEthTypeOAM = 0x8809;	
	UINT32 uiEthTypeOAMPri= 7;	//added by wanghuanyu for 181
#else
//    UINT8 aucBpduMacAdd[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
//    UINT8 aucIgmpMacAdd[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
#endif
    printk("\n#######call Hal_AclRuleInit ... ");

    Hal_AclInit();

	/*begin add by shipeng 2013-05-14*/
	LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
		/*enable all the uiport*/
		rtk_acl_igrState_set(uiPPort, ENABLED);
	}
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	rtk_acl_igrState_set(uiPPort, ENABLED);
	/*end add by shipeng 2013-05-14*/	

	Hal_Acl_FieldSelect_init();

	/*broadcast */
	/*added by wanghuanyu for 181*/
	__HAL_CreateCPUPacketPRI(ACL_TRUST_ETHTYPE,&uiEthTypeOAM,&uiEthTypeOAMPri);
	/*added by wanghuanyu for 181*/
    (void)Hal_AclRuleForTrapLoopd2CpuCreate(FALSE, ACL_TRUST_ETHTYPE, &uiEthTypeLoopd);
    (void)Hal_AclRuleForTrapRaisecomCpuCreate(FALSE, ACL_TRUST_ETHTYPE, &uiRaisecomPkt);
    /*end modified by liaohongjun 2012/11/30 of EPN104QID0084*/

	/* trap dst mac == cpu mac frame to cpu. */
	//__Hal_CreateRuleForCpuMac();----add by an
	
    /* Copy broadcast arp frame to cpu. */
    (void)Hal_AclRuleForCopyCpuCreate(TRUE, ACL_TRUST_ETHTYPE, &uiEthTypeArp);
#if 1

    /* Copy broadcast dhcp req frame to cpu. */
    uiUdpPort = DHCP_CLIENT_PORT;
    (void)Hal_AclRuleForCopyCpuCreate(TRUE, ACL_TRUST_UDP_SPORT, &uiUdpPort);
    uiUdpPort = DHCP_SERVER_PORT; 
    (void)Hal_AclRuleForCopyCpuCreate(TRUE, ACL_TRUST_UDP_DPORT, &uiUdpPort);

    /* Copy broadcast dhcp ack frame to cpu. */
    uiUdpPort = DHCP_SERVER_PORT;
    (void)Hal_AclRuleForCopyCpuCreate(TRUE, ACL_TRUST_UDP_SPORT, &uiUdpPort);
    uiUdpPort = DHCP_CLIENT_PORT; 
    (void)Hal_AclRuleForCopyCpuCreate(TRUE, ACL_TRUST_UDP_DPORT, &uiUdpPort);
#else
    /* Trap BPDU frame to cpu. */
    //(void)Hal_AclRuleForTrapCpuCreate(FALSE, ACL_TRUST_DMAC, aucBpduMacAdd);

    /* Trap igmp frame to cpu. */
    //(void)Hal_AclRuleForTrapCpuCreate(FALSE, ACL_TRUST_DMAC, aucIgmpMacAdd);

	#if 0
	uiPPortMask = 0;
    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);

    (VOID)ACL_CreateRuleForArpRateLimit(uiPPortMask, 64);
   
    uiPPortMask = 0;
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
    (VOID)ACL_CreateRuleForBpduRateLimit(uiPPortMask, 32);
	#endif


#endif
	#ifdef ONU_STYLE
	//(VOID)ACL_CreateRuleForStagToCpu();
	#endif
/*Begin add by huangmingjian 2014-05-12 for Bug 583*/	
#if defined(CONFIG_ZBL_SNMP)
	int ruleValue = 161;
	Hal_AclRuleForTrapSnmpBroatcast2Cpu(TRUE, ACL_TRUST_UDP_DPORT, &ruleValue);
#endif
/*End add by huangmingjian 2014-05-12  for Bug 583*/	
#ifdef CONFIG_EOC_EXTEND
        Hal_EocLowLevelFunctionInit();   
#endif /* EOC_PORTING */

    printk("done.##############\n");
    return DRV_OK;
}

//#ifdef CTC_MULTICAST_SURPORT
#if 1
/*****************************************************************************
    Func Name: _Hal_CfgPortMcVlanSet
  Description: Set port mc vlan cfg.
        Input: UINT32 uiLPortId                     
                PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortMcVlanSet(UINT32 uiLPortId, PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg)
{

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcMcVlanCfg))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(&m_astUniMcVlanCfg[uiLPortId], pstCtcMcVlanCfg, sizeof(PORT_MC_VLAN_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortMcVlanGet
  Description: Get port mc vlan cfg.
        Input: UINT32 uiLPortId                     
                PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortMcVlanGet(UINT32 uiLPortId, PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg)
{

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcMcVlanCfg))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(pstCtcMcVlanCfg, &m_astUniMcVlanCfg[uiLPortId], sizeof(PORT_MC_VLAN_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcMcVlanCreate
  Description: Create acl rule for mc vlan.
        Input: UINT32 uiLPortId  
                UINT32 uiMcVlan   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcMcVlanCreate(UINT32 uiLPortId, UINT32 uiMcVlan)
{
    UINT32 i = 0;
    UINT32 uiVlanId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId = 0;
    DRV_RET_E enRet = DRV_OK;
    PORT_MC_VLAN_CFG_S stCtcMcVlanCfg;

    if ((!IsValidLgcPort(uiLPortId)) || (!VALID_VLAN_ID(uiMcVlan)))
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    (void)_Hal_CfgPortMcVlanGet(uiLPortId, &stCtcMcVlanCfg);
    for (i = 0; i < stCtcMcVlanCfg.uiMcVlanNum; i++)
    {
        if (uiMcVlan == stCtcMcVlanCfg.auiVlanList[i])
        {
            return DRV_OK;
        }
    }
    
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    uiVlanId = uiMcVlan;
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiVlanId, (VOID *)&uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    uiPPortMask = 1U << SWITCH_UPLINK_PHY_PORT;
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPPortMask |= (1U << uiPPort);
    //printk("\nfunc:%s,bind mask:0x%04X; rule id:%d.\n",__FUNCTION__,uiPPortMask,uiAclRuleId);
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW; 
    }

    stCtcMcVlanCfg.auiAclList[stCtcMcVlanCfg.uiMcVlanNum] = uiAclRuleId;
    stCtcMcVlanCfg.auiVlanList[stCtcMcVlanCfg.uiMcVlanNum] = uiMcVlan;
    stCtcMcVlanCfg.uiMcVlanNum++;
//printk("\nfunc:%s,port:%d,num:%d, vid:%d,aclid:%d.\n",__FUNCTION__,
//          uiLPortId,stCtcMcVlanCfg.uiMcVlanNum,uiMcVlan,uiAclRuleId);
    (void)_Hal_CfgPortMcVlanSet(uiLPortId, &stCtcMcVlanCfg);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcMcVlanDelete
  Description: Delete acl rule used by mc vlan.
        Input: UINT32 uiLPortId  
                UINT32 uiMcVlan   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcMcVlanDelete(UINT32 uiLPortId, UINT32 uiMcVlan)
{
    UINT32 i;
    UINT32 uiAclRuleId = ACL_RULE_ID_IVALLID;
    DRV_RET_E enRet = DRV_OK;
    PORT_MC_VLAN_CFG_S stCtcMcVlanCfg;

    if ((!IsValidLgcPort(uiLPortId)) || (!VALID_VLAN_ID(uiMcVlan)))
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }
    
    (void)_Hal_CfgPortMcVlanGet(uiLPortId, &stCtcMcVlanCfg);
    if (0 == stCtcMcVlanCfg.uiMcVlanNum)
    {
        return DRV_OK;  
    }

    for (i = 0; i < stCtcMcVlanCfg.uiMcVlanNum; i++)
    {
        if (uiMcVlan == stCtcMcVlanCfg.auiVlanList[i])
        {
            uiAclRuleId = stCtcMcVlanCfg.auiAclList[i];
        }
    }
//printk("\nfunc:%s,port:%d,num:%d, vid:%d,aclid:%d.\n",__FUNCTION__,
//          uiLPortId,stCtcMcVlanCfg.uiMcVlanNum,uiMcVlan,uiAclRuleId);
    if (ACL_RULE_ID_IVALLID == uiAclRuleId)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;  
    }

    /*Delete acl rule of this port.*/
    enRet = _Hal_AclRuleUnbind(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;  
    }

    enRet = _Hal_AclRuleCfgPortsClear(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;  
    }

    for (i = 0; i < stCtcMcVlanCfg.uiMcVlanNum; i++)
    {
        if (uiMcVlan == stCtcMcVlanCfg.uiMcVlanNum)
        {
            break;
        }
    }

    for (; i < (stCtcMcVlanCfg.uiMcVlanNum-1); i++)
    {
        stCtcMcVlanCfg.auiAclList[i] = stCtcMcVlanCfg.auiAclList[i+1];
        stCtcMcVlanCfg.auiVlanList[i] = stCtcMcVlanCfg.auiAclList[i+1];
    }
    stCtcMcVlanCfg.uiMcVlanNum--;
    (void)_Hal_CfgPortMcVlanSet(uiLPortId, &stCtcMcVlanCfg);

    return DRV_OK;
}

int _Hal_VlanifRuleGetFreeIndex(VOID)
{
	int index=0;
	ACL_MUTEX_LOCK;
	for(index=0;index <MAX_VLAN_INTERFACE;index++)
		if(aclVlanIntf[index].valid==0)
		{
			aclVlanIntf[index].valid=1;
			break;
		}
	ACL_MUTEX_UNLOCK;
	return index;
}
DRV_RET_E __Hal_CreateRuleForCpuMac(void)
{
	UINT32 uiAclRuleId=0;
	UINT32 uiLPortId=0;
	UINT32 uiPPort=0;
	int enRet=0;
	UINT32 uiPPortMask;	
	extern unsigned char fe_addr[6];
	int index=0;
	UINT32 uiCpuPacketPri= 6;
	
	index=_Hal_VlanifRuleGetFreeIndex();
	if(index==-1||index==MAX_VLAN_INTERFACE)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_PARA;
	}

	uiPPortMask = 0;
	LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);

	/*dst mac==cpu mac*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
	ACL_MUTEX_LOCK;
	aclVlanIntf[index].vid=0;
	aclVlanIntf[index].uimacRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;	
   
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, fe_addr, fe_addr);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_REMATK_PRIORITY, fe_addr, &uiCpuPacketPri);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	#if 0
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, FALSE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	#endif
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	return DRV_OK;
}
DRV_RET_E Hal_VlanInterfaceAdd(UINT32 vid, mac_address_t mac_address)
{
	UINT32 uiAclRuleId=0;
	UINT32 uiVlanId=0;
	UINT32 uiLPortId=0;
	UINT32 uiPPort=0;
	int enRet=0;
	UINT32 uiPPortMask;
	UINT32 uiEthTypeArp = 0x0806;
	UINT8  aucMacAdd[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	#define DHCP_SERVER_PORT 67
	#define DHCP_CLIENT_PORT 68
	UINT32 uiUdpPort;
	int index=0;

	#if 0
	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
	if(!VALID_VLAN_ID(vid))
	{
		   return DRV_ERR_PARA;
	}
	index=_Hal_VlanifRuleGetFreeIndex();
	if(index==-1||index==MAX_VLAN_INTERFACE)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_PARA;
	}

	uiPPortMask = 0;
	LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }

	/*ctag==mvlan && dst mac==cpu mac*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
	uiVlanId = vid;
	ACL_MUTEX_LOCK;
	aclVlanIntf[index].vid=uiVlanId;
	aclVlanIntf[index].uimacRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;	
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_TRAP_TO_CPU, (VOID *)&uiVlanId, (VOID *)&uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, mac_address, mac_address);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}		
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	/*ctag==mvlan && arp*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uiarpRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
	
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_COPY_TO_CPU, aucMacAdd, &aucMacAdd);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_COPY_TO_CPU, &uiEthTypeArp, &uiEthTypeArp);
	if (DRV_OK != enRet) 
	{
		return DRV_ERR_UNKNOW;
	}
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }	
	
	/*dhcp*/
	/* 1.Trap broadcast dhcp req frame to cpu.  ctag==mvlan && udp-sport==dhcp client port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdcreqRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }	 
    uiUdpPort = DHCP_CLIENT_PORT;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	/* 2.ctag==mvlan && udp-dport==dhcp server port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdsreqRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
    uiUdpPort = DHCP_SERVER_PORT; 
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_DPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	/* 3.Trap broadcast dhcp ack frame to cpu. ctag==mvlan && udp-sport==dhcp server port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdsackRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
	uiUdpPort = DHCP_SERVER_PORT;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	/* 4.ctag==mvlan && udp-dport==dhcp client port*/
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdcackRuleid_ctag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
    uiUdpPort = DHCP_CLIENT_PORT; 
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_DPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_CTAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	uiPPortMask = 0;
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
	
	/* 5.Trap broadcast dhcp req frame to cpu.  stag==mvlan && udp-sport==dhcp client port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdcreqRuleid_stag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_STAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }	 
    uiUdpPort = DHCP_CLIENT_PORT;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_STAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}	
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

	/* 6.stag==mvlan && udp-dport==dhcp server port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_STAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdsreqRuleid_stag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
    uiUdpPort = DHCP_SERVER_PORT; 
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_DPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
    enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_STAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	/* 7.Trap broadcast dhcp ack frame to cpu. stag==mvlan && udp-sport==dhcp server port*/
	enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_STAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdsackRuleid_stag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
	uiUdpPort = DHCP_SERVER_PORT;
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_SPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_STAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	
	/* 8.stag==mvlan && udp-dport==dhcp client port*/
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);	
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_STAG_VID, ACL_ACTION_COPY_TO_CPU, (VOID *)&uiVlanId, &uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	ACL_MUTEX_LOCK;	
	aclVlanIntf[index].uidhcpbrdcackRuleid_stag=uiAclRuleId;
	ACL_MUTEX_UNLOCK;
    uiUdpPort = DHCP_CLIENT_PORT; 
	enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_UDP_DPORT, ACL_ACTION_COPY_TO_CPU, &uiUdpPort, &uiUdpPort);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }
	enRet = _Hal_CfgAclCareTag(uiAclRuleId, ACL_CARE_TAG_STAG, TRUE);
	if (DRV_OK != enRet)
	{
		printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return DRV_ERR_UNKNOW;
	}
	
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
    	printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
	printk("add vid %d ok\n",uiVlanId);
	#endif
	return DRV_OK;
}

DRV_RET_E Hal_VlanInterfaceDelete(UINT32 vid)
{
	UINT32 uiInterfaceIndex=0;
	
	
	for(uiInterfaceIndex = 0; uiInterfaceIndex < MAX_VLAN_INTERFACE; uiInterfaceIndex++)
	{
	  if(aclVlanIntf[uiInterfaceIndex].vid == vid&&aclVlanIntf[uiInterfaceIndex].valid==1)
	  {
		  break;
	  }
	}
	if(uiInterfaceIndex>=MAX_VLAN_INTERFACE)
	{
		return DRV_ERR_UNKNOW;
	}
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uimacRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uiarpRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdsackRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdcackRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdcreqRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdsreqRuleid_ctag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdsackRuleid_stag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdcackRuleid_stag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdcreqRuleid_stag);
	ACL_DeleteRuleByAclid(aclVlanIntf[uiInterfaceIndex].uidhcpbrdsreqRuleid_stag);
	ACL_MUTEX_LOCK;	
	aclVlanIntf[uiInterfaceIndex].valid=0;
	ACL_MUTEX_UNLOCK;
	printk("del vid %d ok\n",vid);
	return DRV_OK;
}

#endif

#ifdef CONFIG_EOC_EXTEND

static UINT32 eocBcastMmeTrapToCpuAclId;
static UINT32 eocCpuMmeTrapToCpuAclId;
static UINT32 eocDropMmeAclId;
static UINT32 eocDropIpFromCableToCpuAclId;


static DRV_RET_E Hal_AclForMmeTrapToCpu(logic_pmask_t *pstLgcMask, UINT8 *aucMac, UINT32 *puiAclRuleId)
{
    DRV_RET_E enRet;    
    UINT32 uiAclRuleId;
    UINT32 uiMmeEthType = 0x88E1;
    UINT32 uiPortMask;
    UINT32 lgcPort, uiPhyId;


    if (!LgcMaskNotNull(pstLgcMask))
    {
        return DRV_ERR_PARA;
    }
   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    if (NULL != aucMac)
    {
        printk("%s %d: _Hal_AclRuleCfgCreate(%d, %02X:%02X:%02X:%02X:%02X:%02X);\n", __FUNCTION__, __LINE__, uiAclRuleId, aucMac[0], aucMac[1], aucMac[2], aucMac[3], aucMac[4], aucMac[5]);    
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_TRAP_TO_CPU, aucMac, aucMac);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_TRAP_TO_CPU, &uiMmeEthType, &uiMmeEthType);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    uiPortMask = 0;
    LgcPortFor(lgcPort)
    {
        if(TstLgcMaskBit(lgcPort, pstLgcMask))
        {
            uiPhyId = PortLogic2PhyPortId(lgcPort);
            uiPortMask |= (1U << uiPhyId);
        }
    }    

    printk("%s %d: _Hal_AclRuleBind(%08X, %d);\n", __FUNCTION__, __LINE__, uiPortMask, uiAclRuleId);
    enRet = _Hal_AclRuleBind(uiPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    if (puiAclRuleId){
        *puiAclRuleId = uiAclRuleId;
    }

    return DRV_OK;
}


static DRV_RET_E Hal_AclForIpFromCableToCpuDrop(logic_pmask_t *pstLgcMask, UINT8 *aucCpuMac, UINT32 *puiAclRuleId)
{
    DRV_RET_E enRet;  
    UINT32 uiAclRuleId;
    UINT32 uiIpEthType = 0x0800;
    UINT32 uiPortMask;
    UINT32 lgcPort, uiPhyId;
    

    if (!LgcMaskNotNull(pstLgcMask))
    {
        return DRV_ERR_PARA;
    }

   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    if (NULL != aucCpuMac)
    {
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_DMAC, ACL_ACTION_DROP, aucCpuMac, aucCpuMac);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_DROP, &uiIpEthType, &uiIpEthType);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    uiPortMask = 0;
    LgcPortFor(lgcPort)
    {
        if(TstLgcMaskBit(lgcPort, pstLgcMask))
        {
            uiPhyId = PortLogic2PhyPortId(lgcPort);
            uiPortMask |= (1U << uiPhyId);
        }
    }   

    printk("%s %d: _Hal_AclRuleBind(%08X, %d);\n", __FUNCTION__, __LINE__, uiPortMask, uiAclRuleId);
    enRet = _Hal_AclRuleBind(uiPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    if (puiAclRuleId){
        *puiAclRuleId = uiAclRuleId;
    }

    return DRV_OK;
}


static DRV_RET_E Hal_AclForMmeDrop(logic_pmask_t *pstLgcMask, UINT32 *puiAclRuleId)
{
    DRV_RET_E enRet;   
    UINT32 uiAclRuleId;
    UINT32 uiMmeEthType = 0x88E1;
    UINT32 uiPortMask;
    UINT32 lgcPort, uiPhyId;

    if (!LgcMaskNotNull(pstLgcMask))
    {
        return DRV_ERR_PARA;
    }

   
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_ETHTYPE, ACL_ACTION_DROP, &uiMmeEthType, &uiMmeEthType);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    uiPortMask = 0;
    LgcPortFor(lgcPort)
    {
        if(TstLgcMaskBit(lgcPort, pstLgcMask))
        {
            uiPhyId = PortLogic2PhyPortId(lgcPort);
            uiPortMask |= (1U << uiPhyId);
        }
    }       

    printk("%s %d: _Hal_AclRuleBind(%08X, %d);\n", __FUNCTION__, __LINE__, uiPortMask, uiAclRuleId);
    
    enRet = _Hal_AclRuleBind(uiPortMask, uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    if (puiAclRuleId){
        *puiAclRuleId = uiAclRuleId;
    }

    return DRV_OK;
}


DRV_RET_E Hal_SetEocLowLevelFunction(eoc_low_level_t *peocLowLevel)
{
    DRV_RET_E enRet = DRV_OK;
    UINT8  aucMacBcast[ETHER_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    UINT8  aucMacZero[ETHER_ADDR_LEN] = {0x00,0x00,0x00,0x00,0x00,0x00};        

    if (NULL == peocLowLevel){
        return DRV_ERR_PARA;
    }

    if (eocBcastMmeTrapToCpuAclId != ACL_RULE_ID_IVALLID){
        enRet = ACL_DeleteRuleByAclid(eocBcastMmeTrapToCpuAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }       
        eocBcastMmeTrapToCpuAclId = ACL_RULE_ID_IVALLID;
    }
    
    if (eocCpuMmeTrapToCpuAclId != ACL_RULE_ID_IVALLID){
        enRet = ACL_DeleteRuleByAclid(eocCpuMmeTrapToCpuAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }  
        eocCpuMmeTrapToCpuAclId = ACL_RULE_ID_IVALLID;
    } 
    
    if (eocDropMmeAclId != ACL_RULE_ID_IVALLID){
        enRet = ACL_DeleteRuleByAclid(eocDropMmeAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }   
        eocDropMmeAclId = ACL_RULE_ID_IVALLID;
    }  
   
    if (eocDropIpFromCableToCpuAclId != ACL_RULE_ID_IVALLID){
        enRet = ACL_DeleteRuleByAclid(eocDropIpFromCableToCpuAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }
        eocDropIpFromCableToCpuAclId = ACL_RULE_ID_IVALLID;
    }          
    if (LgcMaskNotNull(&peocLowLevel->cable_ports)){
        enRet = Hal_AclForMmeTrapToCpu(&peocLowLevel->cable_ports, aucMacBcast, &eocBcastMmeTrapToCpuAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }        
        if (memcmp(peocLowLevel->cpu_mac, aucMacZero, sizeof(peocLowLevel->cpu_mac))){
            enRet = Hal_AclForMmeTrapToCpu(&peocLowLevel->cable_ports, peocLowLevel->cpu_mac, &eocBcastMmeTrapToCpuAclId);            
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }            
        }
        if (peocLowLevel->drop_ip_from_cable_to_cpu){
            enRet = Hal_AclForIpFromCableToCpuDrop(&peocLowLevel->cable_ports, peocLowLevel->cpu_mac, &eocDropIpFromCableToCpuAclId);            
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }            
        }        
    }
    
    if (LgcMaskNotNull(&peocLowLevel->drop_mme_ports)){
        enRet = Hal_AclForMmeDrop(&peocLowLevel->drop_mme_ports, &eocDropMmeAclId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }        
    }

    return enRet;    
}


void Hal_EocLowLevelFunctionInit(void)
{
    eocBcastMmeTrapToCpuAclId = ACL_RULE_ID_IVALLID;
    eocCpuMmeTrapToCpuAclId = ACL_RULE_ID_IVALLID;
    eocDropMmeAclId = ACL_RULE_ID_IVALLID;
    eocDropIpFromCableToCpuAclId = ACL_RULE_ID_IVALLID; 
}



#endif /* CONFIG_EOC_EXTEND */






#ifdef  __cplusplus
}
#endif



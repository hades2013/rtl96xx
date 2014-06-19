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
#ifdef CHIPSET_RTL8328
#include <common/error.h>
#include <rtk/pie.h>
#include <rtk/vlan.h>
#include <rtk/l2.h>
#include <rtk/switch.h>
#include <rtk/mirror.h>
#include <hal/common/halctrl.h>
#include <osal/sem.h>
#elif defined(CHIPSET_RTL8305)
#include "rtk_api.h"
#include "rtk_api_ext.h"
#include "rtl8367b_asicdrv.h"
#endif
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
static DRV_RET_E _Hal_CfgPortFilterModeSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, ACL_TRUST_MODE_E enMode);
static DRV_RET_E _Hal_CfgPortFilterRuleIdSet(UINT32 uiLPortId, UINT32 uiFilterIndex, ACL_DIRECTION_E enAclDir, UINT32 uiAclId);
static DRV_RET_E _Hal_CfgPortClfRmkModeSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, ACL_TRUST_MODE_E enMode);
static DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 uiAclId);

//#ifdef CTC_MULTICAST_SURPORT
#if 1
static DRV_RET_E _Hal_CfgPortMcVlanSet(UINT32 uiLPortId, PORT_MC_VLAN_CFG_S *pstCtcMcVlanCfg);
#endif

/*----------------------------------------------*
 * global parameter                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stAclMutex;
#ifdef CHIPSET_RTL8305
rtk_filter_cfg_t m_astAclCfg[ACL_RULE_NUM_MAX];
rtk_filter_action_t m_astAclAction[ACL_RULE_NUM_MAX];
CTC_VLAN_CFG_S m_astUniVlanMode[LOGIC_PORT_NO+1];
PORT_CLF_REMAERK_CFG_S m_astUniClfMarkMode[LOGIC_PORT_NO+1];
/*include uplink port*/
PORT_FILTER_CFG_S m_astUniFilterMode[LOGIC_PORT_NO+2];
PORT_QINQ_S g_astUniQinqMode[LOGIC_PORT_NO+1];
STATIC UINT32 g_uiDropLoopdPPMask = 0;
#endif

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
//#ifdef CTC_MULTICAST_SURPORT
#if 1
    PORT_MC_VLAN_CFG_S stCtcMcVlanCfg;
#endif
    
    ACL_MUTEX_INIT;
#ifdef CHIPSET_RTL8305
    ACL_MUTEX_LOCK;
    memset(m_astAclCfg, 0, sizeof(m_astAclCfg));
    memset(m_astAclAction, 0, sizeof(m_astAclAction));
    ACL_MUTEX_UNLOCK;
#endif
//#ifdef CTC_MULTICAST_SURPORT
#if 1
    memset(&stCtcMcVlanCfg, 0, sizeof(stCtcMcVlanCfg));
    for (uiPrec = 0; uiPrec < CTC_MC_MAX_GROUP_NUM; uiPrec++)
    {
        stCtcMcVlanCfg.auiAclList[uiPrec] = ACL_RULE_ID_IVALLID;
        stCtcMcVlanCfg.auiVlanList[uiPrec] = CTC_MC_VLAN_ID_IVALID;
    }
#endif
     
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t *pstRtlAclCfg = NULL;
#endif 

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclCfg))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    pstRtlAclCfg = (rtk_filter_cfg_t *)pstAclCfg;

    ACL_MUTEX_LOCK;
    memcpy(pstRtlAclCfg, &m_astAclCfg[uiAclId], sizeof(rtk_filter_cfg_t));
    ACL_MUTEX_UNLOCK;
#endif 

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t *pstRtlAclCfg = NULL;
#endif

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclCfg))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    pstRtlAclCfg = (rtk_filter_cfg_t *)pstAclCfg;

    ACL_MUTEX_LOCK;
    memcpy(&m_astAclCfg[uiAclId],pstRtlAclCfg,sizeof(rtk_filter_cfg_t));
    ACL_MUTEX_UNLOCK;
#endif

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_action_t *pstRtlAclAct = NULL;
#endif    

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclAction))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    pstRtlAclAct = (rtk_filter_action_t *)pstAclAction;
 
    ACL_MUTEX_LOCK;
    memcpy(pstRtlAclAct, &m_astAclAction[uiAclId], sizeof(rtk_filter_action_t));
    ACL_MUTEX_UNLOCK;
#endif

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_action_t *pstRtlAclAct = NULL;
#endif    

    if ((ACL_RULE_NUM_MAX <= uiAclId) ||
        (NULL == pstAclAction))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    pstRtlAclAct = (rtk_filter_action_t *)pstAclAction;
     
    ACL_MUTEX_LOCK;
    memcpy(&m_astAclAction[uiAclId], pstRtlAclAct, sizeof(rtk_filter_action_t));
    ACL_MUTEX_UNLOCK;
#endif

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_field_t *pstRtlAclTypeAndValue = NULL;
#endif    

    if ((ACL_TRUST_END <= enTrustMode) ||
        (NULL == pValue)             ||
        (NULL == pAclTypeAndValue))
    {
        return DRV_ERR_PARA;
    }   

#if defined(CHIPSET_RTL8305)
    pstRtlAclTypeAndValue = (rtk_filter_field_t *)pAclTypeAndValue;
#endif

    switch (enTrustMode)
    {
        case ACL_TRUST_PORT://trust port
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_DMAC;
            pstRtlAclTypeAndValue->filter_pattern_union.dmac.dataType = FILTER_FIELD_DATA_MASK;
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.value.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.mask.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.rangeStart.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.rangeEnd.octet,0,ETHER_ADDR_LEN);
            #endif
            break;
        case ACL_TRUST_SMAC:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_SMAC;
            pstRtlAclTypeAndValue->filter_pattern_union.smac.dataType = FILTER_FIELD_DATA_MASK;
            memcpy(pstRtlAclTypeAndValue->filter_pattern_union.smac.value.octet,(UINT8 *)pValue,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.smac.mask.octet,0xFF,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.smac.rangeStart.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.smac.rangeEnd.octet,0,ETHER_ADDR_LEN);
            #endif
            break;
        case ACL_TRUST_DMAC:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_DMAC;
            pstRtlAclTypeAndValue->filter_pattern_union.dmac.dataType = FILTER_FIELD_DATA_MASK;
            memcpy(pstRtlAclTypeAndValue->filter_pattern_union.dmac.value.octet,(UINT8 *)pValue,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.mask.octet,0xFF,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.rangeStart.octet,0,ETHER_ADDR_LEN);
            memset(pstRtlAclTypeAndValue->filter_pattern_union.dmac.rangeEnd.octet,0,ETHER_ADDR_LEN);
            #endif
            break;
        case ACL_TRUST_CTAG_PRIO:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_CTAG;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.pri.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.pri.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.pri.mask = RTK_MAX_NUM_OF_PRIORITY - 1;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.pri.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.pri.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_ETHTYPE:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_ETHERTYPE;
            pstRtlAclTypeAndValue->filter_pattern_union.etherType.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.etherType.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.etherType.mask = 0xFFFF;
            pstRtlAclTypeAndValue->filter_pattern_union.etherType.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.etherType.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_CTAG_VID:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_CTAG;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.vid.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.vid.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.vid.mask = 0x0FFF;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.vid.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.ctag.vid.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_IPV4_SIP:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_IPV4_SIP;
            pstRtlAclTypeAndValue->filter_pattern_union.sip.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.sip.value = *(UINT32 *)pValue;
            pstRtlAclTypeAndValue->filter_pattern_union.sip.mask = 0xFFFFFFFF;
            #endif
            break;
        case ACL_TRUST_IPV4_DIP:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_IPV4_DIP;
            pstRtlAclTypeAndValue->filter_pattern_union.dip.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.dip.value = *(UINT32 *)pValue;
            pstRtlAclTypeAndValue->filter_pattern_union.dip.mask = 0xFFFFFFFF;
            #endif
            break;
        case ACL_TRUST_IPV4_PRENCEDENCE:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_END;
            #endif
            break;
        case ACL_TRUST_IPV4_TOS:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_IPV4_TOS;
            pstRtlAclTypeAndValue->filter_pattern_union.ipTos.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.ipTos.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.ipTos.mask = 0xFF;
            pstRtlAclTypeAndValue->filter_pattern_union.ipTos.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.ipTos.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_IPV4_PROTOCOL:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_IPV4_PROTOCOL;
            pstRtlAclTypeAndValue->filter_pattern_union.protocol.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.protocol.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.protocol.mask = 0xFF;
            pstRtlAclTypeAndValue->filter_pattern_union.protocol.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.protocol.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_TCP_SPORT:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_TCP_SPORT;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpSrcPort.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpSrcPort.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.tcpSrcPort.mask = 0xFFFF;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpSrcPort.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpSrcPort.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_TCP_DPORT:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_TCP_DPORT;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpDstPort.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpDstPort.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.tcpDstPort.mask = 0xFFFF;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpDstPort.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.tcpDstPort.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_UDP_SPORT:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_UDP_SPORT;
            pstRtlAclTypeAndValue->filter_pattern_union.udpSrcPort.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.udpSrcPort.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.udpSrcPort.mask = 0xFFFF;
            pstRtlAclTypeAndValue->filter_pattern_union.udpSrcPort.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.udpSrcPort.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_UDP_DPORT:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_UDP_DPORT;
            pstRtlAclTypeAndValue->filter_pattern_union.udpDstPort.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.udpDstPort.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.udpDstPort.mask = 0xFFFF;
            pstRtlAclTypeAndValue->filter_pattern_union.udpDstPort.rangeStart = 0;
            pstRtlAclTypeAndValue->filter_pattern_union.udpDstPort.rangeEnd = 0;
            #endif
            break;
        case ACL_TRUST_STAG_VID:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_STAG;
            pstRtlAclTypeAndValue->filter_pattern_union.stag.vid.dataType = FILTER_FIELD_DATA_MASK;
            pstRtlAclTypeAndValue->filter_pattern_union.stag.vid.value = *((UINT32 *)pValue);
            pstRtlAclTypeAndValue->filter_pattern_union.stag.vid.mask= 0xFFF;
            #endif
            break;
        default:
            #if defined(CHIPSET_RTL8305)
            pstRtlAclTypeAndValue->fieldType = FILTER_FIELD_END;
            #endif
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
#if defined(CHIPSET_RTL8305)
    //rtk_filter_field_type_t enAclRuleType;
    rtk_filter_field_t *pstAclRuleField = NULL;
    rtk_filter_cfg_t stAclCfg;
#endif    
  
    if ((ACL_RULE_NUM_MAX <= uiAclId)    ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        (ACL_ACTION_END <= enAclAct)     ||
        (NULL == pAclRuleTypeValue)      ||
        (NULL == pAclRuleData))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    //pstAclRuleField = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    pstAclRuleField = (rtk_filter_field_t *)kmalloc(sizeof(rtk_filter_field_t), GFP_KERNEL);
    if(NULL == pstAclRuleField)
    {
        return DRV_ERR_NO_MEM;
    }
    
    memset(pstAclRuleField, 0, sizeof(rtk_filter_field_t));    
#endif

    RetVal = _Hal_AclRuleFieldFill(uiAclRuleType, pAclRuleTypeValue, (void *)pstAclRuleField);
    if (DRV_OK != RetVal)
    {
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

#if defined(CHIPSET_RTL8305)    
    if (FILTER_FIELD_END <= pstAclRuleField->fieldType)
    {
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    pstAclRuleField->next = NULL;
#endif

    (void)_Hal_AclCfgGet(uiAclId,(void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)
    if (RT_ERR_OK != rtk_filter_igrAcl_field_add(&stAclCfg, pstAclRuleField))
    {
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }
#endif

    (void)_Hal_AclCfgSet(uiAclId, (void *)&stAclCfg);


    ACL_MUTEX_LOCK;
    switch (enAclAct)
    {
        case ACL_ACTION_REMATK_PRIORITY:
            #if defined(CHIPSET_RTL8305)
            /* */
            #if 0
            /*remark 1p riority in vlan tag*/
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_1P_REMARK] = TRUE;
            #else
            /*remark internal priority*/
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_PRIORITY] = TRUE;
            #endif
            /* 014453 */
            m_astAclAction[uiAclId].filterPriority = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_MIRROR:
            #if defined(CHIPSET_RTL8305)
            /*Set Action to CVLAN Member Configuration Index 30*/
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanIdx = 30;

            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_MIRROR] = TRUE;
            #endif
            break;
        case ACL_ACTION_CVLAN_REMARK:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_VID] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanVid = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_CVLAN_ASSIGN:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanIdx = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_SVLAN_REMARK:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_EGRESS_SVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterSvlanVid = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_COPY_TO_PORTS:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;
            m_astAclAction[uiAclId].filterAddDstPortmask = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_REDIRECT_TO_PORTS:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_REDIRECT] = TRUE;
            m_astAclAction[uiAclId].filterRedirectPortmask = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_COPY_TO_CPU:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_COPY_CPU] = TRUE;
            #endif
            break;
        case ACL_ACTION_POLICING_0:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_POLICING_0] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_TRAP_TO_CPU:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_TRAP_CPU] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_DROP:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_DROP] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        default:
            break;
    }
    ACL_MUTEX_UNLOCK;

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
    //DRV_RET_E RetVal = DRV_OK;
#if defined(CHIPSET_RTL8305)
    //rtk_filter_field_type_t enAclRuleType;
    rtk_filter_field_t *pstAclRuleField = NULL;
    rtk_filter_cfg_t stAclCfg;
#endif    
  
    if ((ACL_RULE_NUM_MAX <= uiAclId)    ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        (ACL_ACTION_END <= enAclAct)     ||
        (NULL == pMac)                   ||
        (NULL == pMacMask)               ||
        (NULL == pAclRuleData))
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    //pstAclRuleField = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    pstAclRuleField = (rtk_filter_field_t *)kmalloc(sizeof(rtk_filter_field_t), GFP_KERNEL);
    if(NULL == pstAclRuleField)
    {
        return DRV_ERR_NO_MEM;
    }
    
    memset(pstAclRuleField, 0, sizeof(rtk_filter_field_t));    
#endif

	switch (uiAclRuleType)
    {
        case ACL_TRUST_SMAC:
            #if defined(CHIPSET_RTL8305)
            pstAclRuleField->fieldType = FILTER_FIELD_SMAC;
            pstAclRuleField->filter_pattern_union.smac.dataType = FILTER_FIELD_DATA_MASK;
            memcpy(pstAclRuleField->filter_pattern_union.smac.value.octet,(UINT8 *)pMac,ETHER_ADDR_LEN);
            memcpy(pstAclRuleField->filter_pattern_union.smac.mask.octet,(UINT8 *)pMacMask,ETHER_ADDR_LEN);
            memset(pstAclRuleField->filter_pattern_union.smac.rangeStart.octet,0,ETHER_ADDR_LEN);
            memset(pstAclRuleField->filter_pattern_union.smac.rangeEnd.octet,0,ETHER_ADDR_LEN);
            #endif
            break;
        case ACL_TRUST_DMAC:
            #if defined(CHIPSET_RTL8305)
            pstAclRuleField->fieldType = FILTER_FIELD_DMAC;
            pstAclRuleField->filter_pattern_union.dmac.dataType = FILTER_FIELD_DATA_MASK;
            memcpy(pstAclRuleField->filter_pattern_union.dmac.value.octet,(UINT8 *)pMac,ETHER_ADDR_LEN);
            memcpy(pstAclRuleField->filter_pattern_union.dmac.mask.octet,(UINT8 *)pMacMask,ETHER_ADDR_LEN);
            memset(pstAclRuleField->filter_pattern_union.dmac.rangeStart.octet,0,ETHER_ADDR_LEN);
            memset(pstAclRuleField->filter_pattern_union.dmac.rangeEnd.octet,0,ETHER_ADDR_LEN);
            #endif
            break;
		default:
            #if defined(CHIPSET_RTL8305)
            pstAclRuleField->fieldType = FILTER_FIELD_END;
            #endif
            break;
	}

#if defined(CHIPSET_RTL8305)    
    if (FILTER_FIELD_END <= pstAclRuleField->fieldType)
    {
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }

    pstAclRuleField->next = NULL;
#endif

    (void)_Hal_AclCfgGet(uiAclId,(void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)
    if (RT_ERR_OK != rtk_filter_igrAcl_field_add(&stAclCfg, pstAclRuleField))
    {
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        return DRV_ERR_UNKNOW;
    }
#endif

    (void)_Hal_AclCfgSet(uiAclId, (void *)&stAclCfg);


    ACL_MUTEX_LOCK;
    switch (enAclAct)
    {
        case ACL_ACTION_REMATK_PRIORITY:
            #if defined(CHIPSET_RTL8305)
            //m_astAclAction[uiAclId].actEnable[FILTER_ENACT_PRIORITY] = TRUE;
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_1P_REMARK] = TRUE;
            m_astAclAction[uiAclId].filterPriority = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_MIRROR:
            #if defined(CHIPSET_RTL8305)
            /*Set Action to CVLAN Member Configuration Index 30*/
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanIdx = 30;

            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_MIRROR] = TRUE;
            #endif
            break;
        case ACL_ACTION_CVLAN_REMARK:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_VID] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanVid = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_CVLAN_ASSIGN:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_INGRESS_CVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterIngressCvlanIdx = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_SVLAN_REMARK:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_EGRESS_SVLAN_INDEX] = TRUE;
            m_astAclAction[uiAclId].filterSvlanVid = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_COPY_TO_PORTS:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;
            m_astAclAction[uiAclId].filterAddDstPortmask = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_REDIRECT_TO_PORTS:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_REDIRECT] = TRUE;
            m_astAclAction[uiAclId].filterRedirectPortmask = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_COPY_TO_CPU:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_COPY_CPU] = TRUE;
            #endif
            break;
        case ACL_ACTION_POLICING_0:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_POLICING_0] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_TRAP_TO_CPU:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_TRAP_CPU] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        case ACL_ACTION_DROP:
            #if defined(CHIPSET_RTL8305)
            m_astAclAction[uiAclId].actEnable[FILTER_ENACT_DROP] = TRUE;
            m_astAclAction[uiAclId].filterPolicingIdx[0] = *((UINT32 *)pAclRuleData);
            #endif
            break;
        default:
            break;
    }
    ACL_MUTEX_UNLOCK;

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_field_t *pstAclRuleField = NULL;
    rtk_filter_field_t *pstAclRuleFieldNext = NULL;
    rtk_filter_cfg_t stAclCfg;
#endif

    if (ACL_RULE_NUM_MAX <= uiAclRuleId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)    
    pstAclRuleField = stAclCfg.fieldHead;
    while (NULL != pstAclRuleField)
    {
        pstAclRuleFieldNext = pstAclRuleField->next;
        //free(pstAclRuleField);
        kfree(pstAclRuleField);
        pstAclRuleField = pstAclRuleFieldNext;
    }
#endif

    ACL_MUTEX_LOCK;
    memset((m_astAclCfg + uiAclRuleId), 0, sizeof(m_astAclCfg[0]));
    memset((m_astAclAction + uiAclRuleId), 0, sizeof(m_astAclAction[0]));
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
DRV_RET_E _Hal_AclRuleEmptyIdGet(UINT32 *puiAclRuleId)
{
    UINT32 uiAclruleIndex = 0;
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stAclCfg;
#endif

    if (NULL == puiAclRuleId)
    {
        return DRV_ERR_PARA;
    }
    
    for (uiAclruleIndex = 0; uiAclruleIndex < ACL_RULE_NUM_MAX; uiAclruleIndex++)
    {
        (VOID)_Hal_AclCfgGet(uiAclruleIndex, (void *)&stAclCfg);

        #if defined(CHIPSET_RTL8305)
        if (NULL == stAclCfg.fieldHead)
        #endif
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stAclCfg;
#endif

    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (NULL == pbState))
    {
        return DRV_ERR_PARA;
    }
    
    (void)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)
    if (NULL == stAclCfg.fieldHead)
#endif
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stAclCfg;
#endif
    
    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (NULL == puiPortMask))
    {
        return DRV_ERR_PARA;
    }
    
    (VOID)_Hal_AclCfgGet(uiAclRuleId, (void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)
    *puiPortMask = stAclCfg.activeport.value;
#endif

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
    m_astAclCfg[uiAclRuleId].activeport.value = 0;
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_care_tag_index_t enRtkCareTagType;
#endif

    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
#if defined(CHIPSET_RTL8305)
        (CARE_TAG_END <= iCareTagType)    
#endif
        )
    {
        return DRV_ERR_PARA;
    }

#if defined(CHIPSET_RTL8305)
    enRtkCareTagType = (rtk_filter_care_tag_index_t)iCareTagType;
#endif

    ACL_MUTEX_LOCK;
    if (TRUE == bEnable)
    {
        #if defined(CHIPSET_RTL8305)
        m_astAclCfg[uiAclRuleId].careTag.tagType[enRtkCareTagType].value = TRUE;
        m_astAclCfg[uiAclRuleId].careTag.tagType[enRtkCareTagType].mask = TRUE;
        #endif
    }
    else
    {
        #if defined(CHIPSET_RTL8305)
        m_astAclCfg[uiAclRuleId].careTag.tagType[enRtkCareTagType].value = FALSE;
        m_astAclCfg[uiAclRuleId].careTag.tagType[enRtkCareTagType].mask = FALSE;
        #endif
    }
    ACL_MUTEX_LOCK;
    
    return DRV_OK;
}

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
#if defined(CHIPSET_RTL8305)
    rtk_filter_number_t rule_num = 1;
    rtk_filter_cfg_t stAclCfg;
    rtk_filter_action_t stAclAction;
    rtk_api_ret_t RetVal = RT_ERR_OK;
#endif

    if (ACL_RULE_NUM_MAX <= uiAclId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_AclCfgGet(uiAclId, (void *)&stAclCfg);

#if defined(CHIPSET_RTL8305)
    stAclCfg.activeport.dataType = FILTER_FIELD_DATA_MASK;
    stAclCfg.activeport.value = uiPortMask;
    stAclCfg.activeport.mask = RTK_MAX_PORT_MASK;
    stAclCfg.activeport.rangeStart = 0;
    stAclCfg.activeport.rangeEnd = 0;
    stAclCfg.invert = FILTER_INVERT_DISABLE;
#endif

    (void)_Hal_AclActionCfgGet(uiAclId, (void *)&stAclAction);

#if defined(CHIPSET_RTL8305)
    RetVal = rtk_filter_igrAcl_cfg_add(uiAclId, &stAclCfg, &stAclAction, &rule_num);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_SDK_GEN_ERROR;
    }
#endif

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
#if defined(CHIPSET_RTL8305)
    rtk_api_ret_t RetVal = RT_ERR_OK;
#endif

    if (ACL_RULE_NUM_MAX <= uiAclId)
    {
        return DRV_ERR_PARA;
    }
   
    (void)_Hal_AclRuleCfgClear(uiAclId);
    
 #if defined(CHIPSET_RTL8305)   
    RetVal = rtk_filter_igrAcl_cfg_del(uiAclId);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_SDK_GEN_ERROR;
    }
#endif

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
static DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 *puiAclId)
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
    Func Name: _Hal_CfgCtcTranspAclRuleIdGet
  Description: Get acl rule id used by ctc transparent vlan and transparent portmask.
        Input: UINT32 *puiAclRuleId  
                UINT32 *puiLPortMask  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgCtcTranspAclRuleIdGet(UINT32 *puiAclRuleId, UINT32 *puiLPortMask)
{
    UINT32 uiLPortId;

    if ((NULL == puiAclRuleId) ||
        (NULL == puiLPortMask))
    {
        return DRV_ERR_PARA;
    }

    *puiAclRuleId = ACL_RULE_NUM_MAX;
    *puiLPortMask = 0;
    
    ACL_MUTEX_LOCK;
    LgcPortFor(uiLPortId)
    {
        if (CTC_VLAN_MODE_TRANSPARENT == m_astUniVlanMode[uiLPortId].mode)
        {
            *puiAclRuleId = m_astUniVlanMode[uiLPortId].acl_list[0];
            *puiLPortMask |= (1U) << uiLPortId;
        }
    }
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortCtcVlanSet
  Description: Save port's ctc vlan cfg.
        Input: UINT32 uiLPortId                
                CTC_VLAN_CFG_S *pstCtcVlanMode  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CfgPortCtcVlanSet(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode)
{

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcVlanMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(&m_astUniVlanMode[uiLPortId], pstCtcVlanMode, sizeof(CTC_VLAN_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_CfgPortCtcVlanGet
  Description: Get port'a ctc vlan cfg.
        Input: UINT32 uiLPortId                
                CTC_VLAN_CFG_S *pstCtcVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CfgPortCtcVlanGet(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcVlanMode))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    memcpy(pstCtcVlanMode, &m_astUniVlanMode[uiLPortId], sizeof(CTC_VLAN_CFG_S));
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CfgPortCtcVlanRuleIdSet
  Description: Set acl rule id used by port's ctc vlan mode.
        Input: UINT32 uiLPortId  
                UINT32 uiListId   
                UINT32 uiAclId    
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortCtcVlanRuleIdSet(UINT32 uiLPortId, UINT32 uiListId, UINT32 uiAclId)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (ACL_RULE_NUM_MAX <= uiListId))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    m_astUniVlanMode[uiLPortId].acl_list[uiListId] = uiAclId;
    ACL_MUTEX_UNLOCK;

    return DRV_OK;
}


/*****************************************************************************
    Func Name: _Hal_CfgPortCtcVlanRuleIdGet
  Description: Get acl rule id used by port's ctc vlan mode.
        Input: UINT32 uiLPortId  
                UINT32 uiListId   
                UINT32 uiAclId    
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_CfgPortCtcVlanRuleIdGet(UINT32 uiLPortId, UINT32 uiListId, UINT32 *puiAclId)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == puiAclId)           ||
        (ACL_RULE_NUM_MAX <= uiListId))
    {
        return DRV_ERR_PARA;
    }

    ACL_MUTEX_LOCK;
    *puiAclId = m_astUniVlanMode[uiLPortId].acl_list[uiListId];
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
static DRV_RET_E _Hal_CfgAclCareTag(UINT32 uiAclRuleId, rtk_filter_care_tag_index_t enCareTagType, BOOL bEnable)
{
    if ((ACL_RULE_NUM_MAX <= uiAclRuleId) ||
        (CARE_TAG_END <= enCareTagType)   ||
        ((TRUE != bEnable) && (FALSE != bEnable)))
    {
        return DRV_ERR_PARA;
    }
    
    ACL_MUTEX_LOCK;

    if (TRUE == bEnable)
    {
        m_astAclCfg[uiAclRuleId].careTag.tagType[enCareTagType].value = TRUE;
        m_astAclCfg[uiAclRuleId].careTag.tagType[enCareTagType].mask = TRUE;
    }
    else
    {
        m_astAclCfg[uiAclRuleId].careTag.tagType[enCareTagType].value = FALSE;
        m_astAclCfg[uiAclRuleId].careTag.tagType[enCareTagType].mask = FALSE;
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
    UINT32 auiExistEntry[RTL8367B_CVIDXNO];
    rtk_vlan_mbrcfg_t stVlanIdxEntry;

    if ((NULL == pstVlanMode) ||
        (NULL == puiEnough))
    {
        printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    memset(auiExistEntry, 0xFF, sizeof(auiExistEntry));

    /*Get empty vlan entry number and existed vlan entries.*/
	for(uiEntryIdx = 0; uiEntryIdx <= RTL8367B_CVIDXMAX; uiEntryIdx++)
	{
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stRtlAclCfg;
#endif 

    if (NULL == pAclEmptyNum)
    {
        return DRV_ERR_PARA;
    }
    
    for (uiAclruleIndex = 0; uiAclruleIndex < ACL_RULE_NUM_MAX; uiAclruleIndex++)
    {
        memset(&stRtlAclCfg, 0, sizeof(stRtlAclCfg));
        (void)_Hal_AclCfgGet(uiAclruleIndex, &stRtlAclCfg);
        if (NULL == stRtlAclCfg.fieldHead)
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
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stRtlAclCfg;
    rtk_filter_action_t stRtlAclAct;
#endif

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
            #if defined(CHIPSET_RTL8305)
            if (TRUE == stRtlAclAct.actEnable[FILTER_ENACT_TRAP_CPU])
            #endif
            {
                if (DRV_OK == _Hal_AclCfgGet(uiAclRuleId, &stRtlAclCfg))
                {
                    #if defined(CHIPSET_RTL8305)
                    if ((FILTER_FIELD_ETHERTYPE == stRtlAclCfg.fieldHead->fieldType) &&
                        ((*(UINT32 *)pRuleValue) == stRtlAclCfg.fieldHead->filter_pattern_union.etherType.value))
                    #endif
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

    if ((NULL == pRuleValue)             ||
        (ACL_TRUST_END <= uiAclRuleType) ||
        ((TRUE != bBroadEn) && (FALSE != bBroadEn)))
    {
        return DRV_ERR_PARA;
    }

    #if 0   
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
    UINT32 uiPPort;  
    UINT32 uiPPortMask = 0;
    UINT32 uiAclRuleId;
#if defined(CHIPSET_RTL8305)
    rtk_filter_cfg_t stRtlAclCfg;
    rtk_filter_action_t stRtlAclAct;
#endif

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
            #if defined(CHIPSET_RTL8305)
            if (TRUE == stRtlAclAct.actEnable[FILTER_ENACT_DROP])
            #endif
            {
                if (DRV_OK == _Hal_AclCfgGet(uiAclRuleId, &stRtlAclCfg))
                {
                    #if defined(CHIPSET_RTL8305)
                    if ((FILTER_FIELD_ETHERTYPE == stRtlAclCfg.fieldHead->fieldType) &&
                        ((*(UINT32 *)pRuleValue) == stRtlAclCfg.fieldHead->filter_pattern_union.etherType.value))
                    #endif
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
            enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_TCP, TRUE);
            break;
        case ACL_TRUST_UDP_SPORT:
        case ACL_TRUST_UDP_DPORT:
            enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_UDP, TRUE);
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

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTranspCreate
  Description: Create acl rule for ctc transparent vlan.
        Input: UINT32 uiLPortId             
                CTC_VLAN_CFG_S *pstVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTranspCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanMode)
{
    DRV_RET_E enRet = DRV_OK;
    static BOOL bFirstTimes = TRUE;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiVlanIndex = 0;
    UINT32 uiAclRuleId = 0;
    //UINT32 uiLPortMask = 0;
    UINT32 uiLPortTmp;
    CTC_VLAN_CFG_S stVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstVlanMode))
    {
        return DRV_ERR_PARA;
    }
   
    memset(&stVlanMode, 0, sizeof(stVlanMode));
 #if 0 
    /* Get acl rule id used by transparent vlan and delete it first. */
    enRet = _Hal_CfgCtcTranspAclRuleIdGet(&uiAclRuleId, &uiLPortMask);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
#endif
    /*The acl rule used by tag and translation must be 
      priorer than rule used by transparent.*/
    uiAclRuleId = ACL_RULE_ID_VLAN_TRANSP;
    if (DRV_OK != _Hal_AclRuleActivePortsGet(uiAclRuleId, &uiPPortMask))
    {
        return DRV_ERR_UNKNOW;
    }
    
    if (TRUE != bFirstTimes)
    {
        if (0 != uiPPortMask)
        {        
            enRet = _Hal_AclRuleUnbind(uiAclRuleId);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }
        }
    }
    else
    {
        bFirstTimes = FALSE;
    }

    uiVlanIndex = RTL8367B_CVIDXMAX;
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_PORT, ACL_ACTION_CVLAN_ASSIGN, (void *)&uiVlanIndex, (void *)&uiVlanIndex);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, FALSE);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
#if 0
    uiLPortMask |= (1U << uiLPortId);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortMask & (1U << uiLPortTmp))
        {
            uiPPort = PortLogic2PhyPortId(uiLPortTmp);
            uiPPortMask |= (1U << uiPPort);
        }
    }
#endif
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPPortMask |= (1U << uiPPort);
    uiPPort = PortLogic2PhyPortId(LOGIC_CPU_PORT);
    uiPPortMask |= (1U << uiPPort);
    uiPPort = PortLogic2PhyPortId(LOGIC_PON_PORT);
    uiPPortMask |= (1U << uiPPort);
    
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    pstVlanMode->number_of_acl_rules = 1;
    pstVlanMode->acl_list[0] = uiAclRuleId;
#if 0
    /* Renew acl rule id used by transparent port. */
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortMask & (1U << uiLPortTmp))
        {
            (void)_Hal_CfgPortCtcVlanRuleIdSet(uiLPortTmp, 0, uiAclRuleId);
        }
    }
#else
    (void)_Hal_CfgPortCtcVlanRuleIdSet(uiLPortId, 0, uiAclRuleId);
#endif   
    /* This port will send out frames with vlan tag belonging to other untransparent port unmodified. */
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPortId)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT != stVlanMode.mode) &&
            (CTC_VLAN_MODE_QINQ != stVlanMode.mode))
        {
            enRet = _Hal_AclRuleActivePortsGet(stVlanMode.acl_list[0], &uiPPortMask);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }

            if (uiPPortMask & (1U << uiPPort))
            {
                continue;
            }

            enRet = _Hal_AclRuleUnbind(stVlanMode.acl_list[0]);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }
            
            uiPPortMask |= (1U << uiPPort);

            enRet = _Hal_AclRuleCfgCreate(stVlanMode.acl_list[0], ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&stVlanMode.default_vlan, (VOID *)&stVlanMode.default_vlan);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }

            enRet = _Hal_CfgAclCareTag(stVlanMode.acl_list[0], CARE_TAG_CTAG, TRUE);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }

            enRet = _Hal_AclRuleBind(uiPPortMask, stVlanMode.acl_list[0]);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;
            }
        }
    }

    /*014453 :for new ctc vlan implementation.*/
    (void)Hal_CfgPortCtcVlanSet(uiLPortId, pstVlanMode);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTranspDelete
  Description: Delete acl rule used by ctc transparent vlan mode.
        Input: UINT32 uiLPortId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTranspDelete(UINT32 uiLPortId)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiAclRuleId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiVlanIndex = 0;
    UINT32 uiPortIndex = 0;
    CTC_VLAN_CFG_S stVlanMode;

    /*Get acl rule used by ctc transparent vlan.*/
    enRet = Hal_CfgPortCtcVlanGet(uiLPortId, &stVlanMode);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }

    uiAclRuleId = stVlanMode.acl_list[0];
    if (DRV_OK != _Hal_AclRuleActivePortsGet(uiAclRuleId, &uiPPortMask))
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_AclRuleUnbind(uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW; 
    }

    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPPortMask &= ~(1U << uiPPort);
    
    if (0 != uiPPortMask)
    {
        uiVlanIndex = RTL8367B_CVIDXMAX;
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_PORT, ACL_ACTION_CVLAN_ASSIGN, (VOID *)&uiVlanIndex, (VOID *)&uiVlanIndex);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW; 
        }
   
        enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW; 
        }
    }

    /* Renew acl rules used by other untransparent port. */
    uiPPort = PortLogic2PhyPortId(uiLPortId);
    for (uiPortIndex = 1; uiPortIndex <= LOGIC_PORT_NO; uiPortIndex++)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }
        
        enRet = Hal_CfgPortCtcVlanGet(uiPortIndex, &stVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;  
        }

        if ((CTC_VLAN_MODE_TRANSPARENT != stVlanMode.mode) &&
            (CTC_VLAN_MODE_QINQ != stVlanMode.mode))
        {
            enRet = _Hal_AclRuleActivePortsGet(stVlanMode.acl_list[0], &uiPPortMask);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;  
            }

            if (!(uiPPortMask & (1U << uiPPort)))
            {
                continue;
            }

            enRet = _Hal_AclRuleUnbind(stVlanMode.acl_list[0]);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;  
            }

            uiPPortMask &= ~(1U << uiPPort);
            if ((uiPPortMask & ((1U << LOGIC_CPU_PORT) | (1U << LOGIC_PON_PORT))) == ((1U << LOGIC_CPU_PORT) | (1U << LOGIC_PON_PORT)))
            {
                continue;
            }

            enRet = _Hal_AclRuleCfgCreate(stVlanMode.acl_list[0], ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&stVlanMode.default_vlan, (VOID *)&stVlanMode.default_vlan);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;  
            }

            enRet = _Hal_CfgAclCareTag(stVlanMode.acl_list[0], CARE_TAG_CTAG, TRUE);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;  
            }

            enRet = _Hal_AclRuleBind(uiPPortMask, stVlanMode.acl_list[0]);
            if (DRV_OK != enRet)
            {
                return DRV_ERR_UNKNOW;  
            }
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTagCreate
  Description: Add acl rule for ctc tag vlan mode.
        Input: UINT32 uiLPortId                        
                CTC_VLAN_CFG_S *pstVlanCfg  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTagCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanCfg)
{
    UINT32 uiVlanId;
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiPortIndex = 0;
    UINT32 uiAclRuleId = 0;
    DRV_RET_E enRet = DRV_OK;
    CTC_VLAN_CFG_S stVlanCfg;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstVlanCfg))
    {
        return DRV_ERR_PARA;
    }
    
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    uiVlanId = pstVlanCfg->default_vlan;
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiVlanId, (VOID *)&uiVlanId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }

    uiPPortMask = (1U << SWITCH_UPLINK_PHY_PORT) | (1U << SWITCH_CPU_PHY_PORT);

    for (uiPortIndex = 1; uiPortIndex <= LOGIC_PORT_NO; uiPortIndex++)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiPortIndex, &stVlanCfg);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW; 
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stVlanCfg.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanCfg.mode))
        {
            uiPPort = PortLogic2PhyPortId(uiPortIndex);
            if (INVALID_PORT == uiPPort)  
            {
                return DRV_ERR_UNKNOW; 
            }
            uiPPortMask |= (1U << uiPPort);
        }
    }

    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPPortMask |= (1U << uiPPort);
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW; 
    }

    pstVlanCfg->number_of_acl_rules = 1;
    pstVlanCfg->acl_list[0] = uiAclRuleId;

    /* Add acl rule for this tag port to drop packets with pvid tag. 
    Use this rule to fix the bug of ingress vlan tagged filter.
    */
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW; 
    }
    
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_DROP, (VOID *)&uiVlanId, (VOID *)&uiVlanId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW; 
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    uiPPort = PortLogic2PhyPortId(uiLPortId);
    uiPPortMask = 1U << uiPPort;
    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    pstVlanCfg->acl_list[pstVlanCfg->number_of_acl_rules] = uiAclRuleId;
    pstVlanCfg->number_of_acl_rules++;

    /*014453 :for new ctc vlan implementation.*/
    (void)Hal_CfgPortCtcVlanSet(uiLPortId, pstVlanCfg);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTagDelete
  Description: Delete acl rule used by ctc tag vlan mode.
        Input: UINT32 uiLPortId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTagDelete(UINT32 uiLPortId)
{
    UINT32 i;
    UINT32 uiAclRuleId;
    DRV_RET_E enRet = DRV_OK;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }
    
    enRet = Hal_CfgPortCtcVlanGet(uiLPortId, &stVlanMode);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }
    
    uiAclRuleId = stVlanMode.acl_list[0];

#if 0
    /*Delete acl rule of this port.*/
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
#else
    for (i = 0; i < stVlanMode.number_of_acl_rules; i++)
    {
        uiAclRuleId = stVlanMode.acl_list[i];
        
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
#endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTranslCreate
  Description: Create acl rule for ctc translation vlan mode.
        Input: UINT32 uiLPortId             
                CTC_VLAN_CFG_S *pstVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTranslCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanMode)
{
    UINT32 uiPPort;
    UINT32 uiPPortMask = 0;
    UINT32 uiPortIndex = 0;
    UINT32 uiPvid = 0;
    UINT32 uiOldVlanId = 0;
    UINT32 uiNewVlanId = 0;
    //UINT32 uiVidTmp = 0;
    UINT32 uiAclRuleId;
    UINT32 i = 0;
    DRV_RET_E enRet = DRV_OK;
    BOOL bAccessMode = FALSE;
    CTC_VLAN_CFG_S stVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstVlanMode))
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }
    
    uiPvid = pstVlanMode->default_vlan;
    
    enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    
    /* Add pvid to pvid changing acl rule for uplink port and other transparent port. */
    enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiPvid, (VOID *)&uiPvid);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    uiPPortMask = (1U << SWITCH_UPLINK_PHY_PORT) | (1U << SWITCH_CPU_PHY_PORT);

    for (uiPortIndex = 1; uiPortIndex <= LOGIC_PORT_NO; uiPortIndex++)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiPortIndex, &stVlanMode);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        {
            uiPPort = PortLogic2PhyPortId(uiPortIndex);
            if (INVALID_PORT == uiPPort)  
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW; 
            }
            
            uiPPortMask |= (1U << uiPPort);
        }
    }

    enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
    if (DRV_OK != enRet)
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW; 
    }

    pstVlanMode->number_of_acl_rules = 1;
    pstVlanMode->acl_list[0] = uiAclRuleId;

    uiPPort = PortLogic2PhyPortId(uiLPortId);
    if (INVALID_PORT == uiPPort)  
    {
        printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW; 
    }
    
    /* For access vlan mode, packets with pvid tag should not be dropped. */
    for (i = 0; i < pstVlanMode->number_of_entries; i++)
    {
        uiOldVlanId = pstVlanMode->vlan_list[i*2];
        uiNewVlanId = pstVlanMode->vlan_list[(i*2)+1];

        if ((uiOldVlanId == uiNewVlanId) &&
            (uiPvid == uiNewVlanId))
        {
            bAccessMode = TRUE;
            break;
        }
    }

    if (FALSE == bAccessMode)
    {
        /* Add acl rule for this tag port to drop packets with pvid tag. */
        enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW; 
        }
        
        //uiVidTmp = 4096;
        //enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiPvid, (VOID *)&uiVidTmp);
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_DROP, (VOID *)&uiPvid, (VOID *)&uiPvid);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW; 
        }

        enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        uiPPortMask = 1U << uiPPort;
        enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        pstVlanMode->acl_list[pstVlanMode->number_of_acl_rules] = uiAclRuleId;
        pstVlanMode->number_of_acl_rules++;
    }

    /* Add acl rule for vlan translation. */
    for (i = 0; i < pstVlanMode->number_of_entries; i++)
    {
        enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        uiOldVlanId = pstVlanMode->vlan_list[i*2];
        uiNewVlanId = pstVlanMode->vlan_list[(i*2)+1];
        
        /* Add old to new vlan changing acl rule for this port. */
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiOldVlanId, (VOID *)&uiNewVlanId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        uiPPortMask = 1U << uiPPort;
        enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        pstVlanMode->acl_list[pstVlanMode->number_of_acl_rules] = uiAclRuleId;
        pstVlanMode->number_of_acl_rules++;

        /* We have add acl rule for access vlan mode. */
        if ((uiOldVlanId == uiNewVlanId) &&
            (uiPvid == uiNewVlanId))
        {
            continue;
        }
        
        /* Add new to old vlan changing acl rule for uplinkport. */
        enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
        
        enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiNewVlanId, (VOID *)&uiOldVlanId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        uiPPortMask = (1U << SWITCH_UPLINK_PHY_PORT);
        enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
        if (DRV_OK != enRet)
        {
            printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }
        
        pstVlanMode->acl_list[pstVlanMode->number_of_acl_rules] = uiAclRuleId;
        pstVlanMode->number_of_acl_rules++;

        if (uiOldVlanId != uiNewVlanId)
        {
            /* Add new vid to 4096 vlan changing acl rule for this port, to drop packets with new vlan tag. */
            enRet = _Hal_AclRuleEmptyIdGet(&uiAclRuleId);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }
            
            //uiVidTmp = 4096;
            //enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_CVLAN_REMARK, (VOID *)&uiNewVlanId, (VOID *)&uiVidTmp);
            enRet = _Hal_AclRuleCfgCreate(uiAclRuleId, ACL_TRUST_CTAG_VID, ACL_ACTION_DROP, (VOID *)&uiNewVlanId, (VOID *)&uiNewVlanId);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }

            enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }
            
            uiPPortMask = 1U << uiPPort;
            enRet = _Hal_AclRuleBind(uiPPortMask, uiAclRuleId);
            if (DRV_OK != enRet)
            {
                printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_ERR_UNKNOW;
            }

            pstVlanMode->acl_list[pstVlanMode->number_of_acl_rules] = uiAclRuleId;
            pstVlanMode->number_of_acl_rules++;
        }
    }

    /*014453 :for new ctc vlan implementation.*/
    (void)Hal_CfgPortCtcVlanSet(uiLPortId, pstVlanMode);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AclRuleForCtcTranslDelete
  Description: Delete acl rule used by ctc translation vlan mode.
        Input: UINT32 uiLPortId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AclRuleForCtcTranslDelete(UINT32 uiLPortId)
{
    UINT32 i = 0;
    UINT32 uiAclRuleId;
    DRV_RET_E enRet = DRV_OK;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }

    memset(&stVlanMode, 0, sizeof(stVlanMode));
    
    enRet = Hal_CfgPortCtcVlanGet(uiLPortId, &stVlanMode);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }
    
    for (i = 0; i < stVlanMode.number_of_acl_rules; i++)
    {
        uiAclRuleId = stVlanMode.acl_list[i];
        
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
    UINT32 uiEmptyIndex = RTK_MAX_NUM_OF_METER;
    rtk_rate_t Rate = 0;
    rtk_enable_t IncludeIfg;
    rtk_api_ret_t Ret;
    
    *pbFind = FALSE;
    
    for (i = 0; i < RTK_MAX_NUM_OF_METER; i++)
    {
        Rate = 0;
        Ret = rtk_rate_shareMeter_get(i, &Rate, &IncludeIfg);
        if (RT_ERR_OK != Ret)
        {
            printk("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if ((0 == Rate) &&
            (RTK_MAX_NUM_OF_METER == uiEmptyIndex))
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


/* */
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

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
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
/* 014453 */

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
    UINT32 uiPPortMask = 0;
    /*begin modified by liaohongjun 2012/11/30 of EPN104QID0084*/
	UINT32 uiEthTypeLoopd = 0x9001;
#if 0
    #define DHCP_SERVER_PORT 67
    #define DHCP_CLIENT_PORT 68

    UINT32 uiEthTypeArp = 0x0806;
    UINT32 uiUdpPort;
#else
//    UINT8 aucBpduMacAdd[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
//    UINT8 aucIgmpMacAdd[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
#endif
    printk("\n#######call Hal_AclRuleInit ... ");

    Hal_AclInit();   

    (void)Hal_AclRuleForTrapLoopd2CpuCreate(FALSE, ACL_TRUST_ETHTYPE, &uiEthTypeLoopd);
    /*end modified by liaohongjun 2012/11/30 of EPN104QID0084*/
#if 0
    /* Trap broadcast arp frame to cpu. */
    (void)Hal_AclRuleForTrapCpuCreate(TRUE, ACL_TRUST_ETHTYPE, &uiEthTypeArp);

    /* Trap broadcast dhcp req frame to cpu. */
    uiUdpPort = DHCP_CLIENT_PORT;
    (void)Hal_AclRuleForTrapCpuCreate(TRUE, ACL_TRUST_UDP_SPORT, &uiUdpPort);
    uiUdpPort = DHCP_SERVER_PORT; 
    (void)Hal_AclRuleForTrapCpuCreate(TRUE, ACL_TRUST_UDP_DPORT, &uiUdpPort);

    /* Trap broadcast dhcp ack frame to cpu. */
    uiUdpPort = DHCP_SERVER_PORT;
    (void)Hal_AclRuleForTrapCpuCreate(TRUE, ACL_TRUST_UDP_SPORT, &uiUdpPort);
    uiUdpPort = DHCP_CLIENT_PORT; 
    (void)Hal_AclRuleForTrapCpuCreate(TRUE, ACL_TRUST_UDP_DPORT, &uiUdpPort);
#else
    /* Trap BPDU frame to cpu. */
    //(void)Hal_AclRuleForTrapCpuCreate(FALSE, ACL_TRUST_DMAC, aucBpduMacAdd);

    /* Trap igmp frame to cpu. */
    //(void)Hal_AclRuleForTrapCpuCreate(FALSE, ACL_TRUST_DMAC, aucIgmpMacAdd);

    LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }

    (VOID)ACL_CreateRuleForArpRateLimit(uiPPortMask, 64);
   
    /* 014052 */
    (VOID)ACL_CreateRuleForBpduRateLimit(uiPPortMask, 32);   
#endif
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

    enRet = _Hal_CfgAclCareTag(uiAclRuleId, CARE_TAG_CTAG, TRUE);
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
#endif

#ifdef  __cplusplus
}
#endif



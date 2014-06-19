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

#include <rtk/classify.h>

#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/autoconf.h>
//#include <linux/sched.h>
//#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/slab.h>


rtk_classify_cfg_t m_astClfCfg[CLF_RULE_NUM_MAX];

PORT_CLF_PRI_TO_QUEUE_CFG_S m_astUniClfPriToQueueMode[LOGIC_PORT_NO+1];

struct mutex m_stClfMutex;

#define CLF_MUTEX_INIT     mutex_init(&m_stClfMutex)
#define CLF_MUTEX_LOCK     mutex_lock(&m_stClfMutex)
#define CLF_MUTEX_UNLOCK   mutex_unlock(&m_stClfMutex)

static DRV_RET_E _Hal_CfgPortClfPriToQueueRuleIdSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 uiClfId);

extern DRV_RET_E _Hal_CfgPortClfRmkAclRuleIdGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 *puiAclId);

void Hal_ClfInit(void)
{
	UINT32 lport;
	UINT32 uiPrec;

    CLF_MUTEX_INIT;

	CLF_MUTEX_LOCK;
	memset(m_astClfCfg, 0, sizeof(m_astClfCfg));	
	CLF_MUTEX_UNLOCK;	

	LgcPortFor(lport)
    {        
        for (uiPrec = 0; uiPrec < CTC_CLF_REMARK_RULE_NUM_MAX*2; uiPrec++)
        {
            (void)_Hal_CfgPortClfPriToQueueRuleIdSet(lport, uiPrec, ACL_RULE_ID_IVALLID);
        }
    }
    
    return;
}

static DRV_RET_E _Hal_ClfCfgGet(UINT32 uiClfId, void *pstClfCfg)
{
	rtk_classify_cfg_t *pstRtlClfCfg = NULL;

    if ((CLF_RULE_NUM_MAX <= uiClfId) ||
        (NULL == pstClfCfg))
    {
        return DRV_ERR_PARA;
    }

	pstRtlClfCfg = (rtk_classify_cfg_t *)pstClfCfg;

    CLF_MUTEX_LOCK;
    memcpy(pstRtlClfCfg, &m_astClfCfg[uiClfId], sizeof(rtk_classify_cfg_t));
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

static DRV_RET_E _Hal_ClfCfgSet(UINT32 uiClfId, void *pstClfCfg)
{
    rtk_classify_cfg_t *pstRtlClfCfg = NULL;

    if ((CLF_RULE_NUM_MAX <= uiClfId) ||
        (NULL == pstClfCfg))
    {
        return DRV_ERR_PARA;
    }

    pstRtlClfCfg = (rtk_classify_cfg_t *)pstClfCfg;

    CLF_MUTEX_LOCK;
    memcpy(&m_astClfCfg[uiClfId],pstRtlClfCfg,sizeof(rtk_classify_cfg_t));
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

static DRV_RET_E _Hal_ClfRuleCfgClear(UINT32 uiClfRuleId)
{
    rtk_classify_field_t *pstClfRuleField = NULL;
    rtk_classify_field_t *pstClfRuleFieldNext = NULL;
    rtk_classify_cfg_t stClfCfg;

    if (ACL_RULE_NUM_MAX <= uiClfRuleId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_ClfCfgGet(uiClfRuleId, (void *)&stClfCfg);

    pstClfRuleField = stClfCfg.field.pFieldHead;
    while (NULL != pstClfRuleField)
    {
        pstClfRuleFieldNext = pstClfRuleField->next;
        kfree(pstClfRuleField);
        pstClfRuleField = pstClfRuleFieldNext;
    }

    CLF_MUTEX_LOCK;
    memset((m_astClfCfg + uiClfRuleId), 0, sizeof(m_astClfCfg[0]));
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

DRV_RET_E _Hal_ClfRuleEmptyIdGet(UINT32 *puiClfRuleId)
{
    UINT32 uiClfruleIndex = 0;
    rtk_classify_cfg_t stAclCfg;

    if (NULL == puiClfRuleId)
    {
        return DRV_ERR_PARA;
    }
    
    for (uiClfruleIndex = 0; uiClfruleIndex < CLF_RULE_NUM_MAX; uiClfruleIndex++)
    {
        (VOID)_Hal_ClfCfgGet(uiClfruleIndex, (void *)&stAclCfg);

        if (NULL == stAclCfg.field.pFieldHead)
        {
            *puiClfRuleId = uiClfruleIndex;
            break;
        }
    }

    if (CLF_RULE_NUM_MAX == uiClfruleIndex)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

static DRV_RET_E _Hal_ClfRuleFieldFill(rtk_classify_field_type_t enTrustMode, 
											void *pValue, 
											void *pClfTypeAndValue) 
{
    rtk_classify_field_t *pstRtlClfTypeAndValue = NULL;

    if ((CLASSIFY_FIELD_END <= enTrustMode) ||
        (NULL == pValue)             ||
        (NULL == pClfTypeAndValue))
    {
        return DRV_ERR_PARA;
    }   

    pstRtlClfTypeAndValue = (rtk_classify_field_t *)pClfTypeAndValue;

    switch (enTrustMode)
    {        
        case CLASSIFY_FIELD_IS_STAG:
			pstRtlClfTypeAndValue->fieldType = CLASSIFY_FIELD_IS_STAG;
			pstRtlClfTypeAndValue->classify_pattern.isStag.value = *((UINT32 *)pValue);
			pstRtlClfTypeAndValue->classify_pattern.isStag.mask = 0x1;
            break;
		case CLASSIFY_FIELD_ACL_HIT:
			pstRtlClfTypeAndValue->fieldType = CLASSIFY_FIELD_ACL_HIT;
			pstRtlClfTypeAndValue->classify_pattern.aclHit.value = *((UINT32 *)pValue);
			pstRtlClfTypeAndValue->classify_pattern.aclHit.mask = 0x7f;
			break;
        default:
            pstRtlClfTypeAndValue->fieldType = CLASSIFY_FIELD_END;
            break;
    }

    return DRV_OK;
}

static DRV_RET_E _Hal_ClfRuleCfgCreate(UINT32 uiClfId, 
											rtk_classify_field_type_t uiClfRuleType, 
											void *pClfRuleTypeValue)
{
    DRV_RET_E RetVal = DRV_OK;
    rtk_classify_field_t *pstClfRuleField = NULL;
	rtk_classify_cfg_t stClfCfg;
  
    if ((CLF_RULE_NUM_MAX <= uiClfId)    ||
        (CLASSIFY_FIELD_END <= uiClfRuleType) ||        
        (NULL == pClfRuleTypeValue))
    {
        return DRV_ERR_PARA;
    }

    pstClfRuleField = (rtk_classify_field_t *)kmalloc(sizeof(rtk_classify_field_t), GFP_KERNEL);
    if(NULL == pstClfRuleField)
    {
        return DRV_ERR_NO_MEM;
    }
    
    memset(pstClfRuleField, 0, sizeof(rtk_classify_field_t));    

    RetVal = _Hal_ClfRuleFieldFill(uiClfRuleType, pClfRuleTypeValue, (void *)pstClfRuleField);
    if (DRV_OK != RetVal)
    {
        kfree(pstClfRuleField);
        return DRV_ERR_UNKNOW;
    }

    if (CLASSIFY_FIELD_END <= pstClfRuleField->fieldType)
    {
        kfree(pstClfRuleField);
        return DRV_ERR_UNKNOW;
    }

    pstClfRuleField->next = NULL;

    (void)_Hal_ClfCfgGet(uiClfId,(void *)&stClfCfg);

    if (RT_ERR_OK != rtk_classify_field_add(&stClfCfg, pstClfRuleField))
    {
        kfree(pstClfRuleField);
        return DRV_ERR_UNKNOW;
    }	
	
    (void)_Hal_ClfCfgSet(uiClfId, (void *)&stClfCfg);

    return DRV_OK;
}

static DRV_RET_E _Hal_ClfRuleBind(UINT32 uiClfId)
{
    rtk_classify_cfg_t stClfCfg;
    rtk_api_ret_t RetVal = RT_ERR_OK;	

    if (CLF_RULE_NUM_MAX <= uiClfId)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_ClfCfgGet(uiClfId, (void *)&stClfCfg);

	stClfCfg.valid = ENABLED;
	stClfCfg.index = uiClfId;

    RetVal = rtk_classify_cfgEntry_add(&stClfCfg);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_SDK_GEN_ERROR;
    }

    (void)_Hal_ClfCfgSet(uiClfId, (void *)&stClfCfg);
    
    return DRV_OK;
}

static DRV_RET_E _Hal_ClfRuleUnbind(UINT32 uiAclId)
{
    rtk_api_ret_t RetVal = RT_ERR_OK;

    if (CLF_RULE_NUM_MAX <= uiAclId)
    {
        return DRV_ERR_PARA;
    }
   
    (void)_Hal_ClfRuleCfgClear(uiAclId);
    
 	RetVal = rtk_classify_cfgEntry_del(uiAclId);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

static DRV_RET_E _Hal_CfgPortClfPriToQueueNumInc(UINT32 uiLPortId)
{
    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }
    
    CLF_MUTEX_LOCK;
    if ((CTC_CLF_REMARK_RULE_NUM_MAX*2) > m_astUniClfPriToQueueMode[uiLPortId].uiClfRmkRuleNum)
    {
        m_astUniClfPriToQueueMode[uiLPortId].uiClfRmkRuleNum++;
    }
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

static DRV_RET_E _Hal_CfgPortClfPriToQueueNumDec(UINT32 uiLPortId)
{
    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }
    
    CLF_MUTEX_LOCK;
    if (0 < m_astUniClfPriToQueueMode[uiLPortId].uiClfRmkRuleNum)
    {
        m_astUniClfPriToQueueMode[uiLPortId].uiClfRmkRuleNum--;
    }
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

static DRV_RET_E _Hal_CfgPortClfPriToQueueRuleIdSet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 uiClfId)
{   
    if ((!IsValidLgcPort(uiLPortId)) ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    CLF_MUTEX_LOCK;
    m_astUniClfPriToQueueMode[uiLPortId].auiClfList[uiClfRmkIndex] = uiClfId;
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

static DRV_RET_E _Hal_CfgPortClfPriToQueueRuleIdGet(UINT32 uiLPortId, UINT32 uiClfRmkIndex, UINT32 *puiClfId)
{
	if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == puiClfId)           ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiClfRmkIndex))
    {
        return DRV_ERR_PARA;
    }
    
    CLF_MUTEX_LOCK;
    *puiClfId = m_astUniClfPriToQueueMode[uiLPortId].auiClfList[uiClfRmkIndex];
    CLF_MUTEX_UNLOCK;

    return DRV_OK;
}

DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   VOID *pRemarkPri)
{
    DRV_RET_E enRet;
    UINT32 uiClfRuleId;
	UINT32 uiAclRuleId;
	rtk_classify_cfg_t stClfCfg;

    if ((!IsValidLgcPort(uiLPortId))     ||
        (NULL == pRemarkPri)             ||                
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiRulePrecedence))
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_CfgPortClfPriToQueueRuleIdGet(uiLPortId, uiRulePrecedence, &uiClfRuleId);
    if (ACL_RULE_ID_IVALLID != uiClfRuleId)
    {        
        enRet = _Hal_ClfRuleUnbind(uiClfRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        (void)_Hal_CfgPortClfPriToQueueNumDec(uiLPortId);
        (void)_Hal_CfgPortClfPriToQueueRuleIdSet(uiLPortId, uiRulePrecedence, ACL_RULE_ID_IVALLID);
    }
    
    enRet = _Hal_ClfRuleEmptyIdGet(&uiClfRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
	
	(void)_Hal_CfgPortClfRmkAclRuleIdGet(uiLPortId, uiRulePrecedence, &uiAclRuleId);
	
    enRet = _Hal_ClfRuleCfgCreate(uiClfRuleId, CLASSIFY_FIELD_ACL_HIT, (void *)&uiAclRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

	(void)_Hal_ClfCfgGet(uiClfRuleId, (void *)&stClfCfg);	
	stClfCfg.direction = CLASSIFY_DIRECTION_US;
	stClfCfg.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
	stClfCfg.act.usAct.cfPri = *(UINT32 *)pRemarkPri;
	(void)_Hal_ClfCfgSet(uiClfRuleId, (void *)&stClfCfg);
	
    enRet = _Hal_ClfRuleBind(uiClfRuleId);
    if (DRV_OK != enRet) 
    {
        return DRV_ERR_UNKNOW;
    }

    (void)_Hal_CfgPortClfPriToQueueNumInc(uiLPortId);
    (void)_Hal_CfgPortClfPriToQueueRuleIdSet(uiLPortId, uiRulePrecedence, uiClfRuleId);

    return DRV_OK;
}

DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiClfRuleId;

    if ((!IsValidLgcPort(uiLPortId))     ||
        (CTC_CLF_REMARK_RULE_NUM_MAX*2 <= uiRulePrecedence))
    {
        return DRV_ERR_PARA;
    }

    enRet = _Hal_CfgPortClfPriToQueueRuleIdGet(uiLPortId, uiRulePrecedence, &uiClfRuleId);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;  
    }

    if ((CLF_RULE_NUM_MAX <= uiClfRuleId) && (ACL_RULE_ID_IVALLID != uiClfRuleId))
    {
        return DRV_ERR_UNKNOW;
    }

    /*Delete clf rule of this port.*/
    if (ACL_RULE_ID_IVALLID != uiClfRuleId)
    {
        enRet = _Hal_ClfRuleUnbind(uiClfRuleId);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        (void)_Hal_CfgPortClfPriToQueueNumDec(uiLPortId);
        (void)_Hal_CfgPortClfPriToQueueRuleIdSet(uiLPortId, uiRulePrecedence, ACL_RULE_ID_IVALLID);
    }

    return DRV_OK;
}

DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueClear(UINT32 uiLPortId)
{
    DRV_RET_E enRet = DRV_OK;
    UINT32 uiRulePrecedence;

    if (!IsValidLgcPort(uiLPortId))
    {
        return DRV_ERR_PARA;
    }

    for (uiRulePrecedence = 0; uiRulePrecedence < CTC_CLF_REMARK_RULE_NUM_MAX*2; uiRulePrecedence++)
    {	
		enRet = Hal_ClfRuleForCtcClfPriToQueueDelete(uiLPortId, uiRulePrecedence);
		if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;  
        }
    }

    return DRV_OK;
}

static DRV_RET_E _Hal_Clf_CreateRuleForDelStag0()
{
	DRV_RET_E enRet=0;
	UINT32 uiClfRuleId=0;
	UINT32 value=0;
	rtk_classify_cfg_t stClfCfg;

    enRet = _Hal_ClfRuleEmptyIdGet(&uiClfRuleId);
    if (DRV_OK != enRet)
    {    	
        return DRV_ERR_UNKNOW; 
    }
    
    enRet = _Hal_ClfRuleCfgCreate(uiClfRuleId, CLASSIFY_FIELD_IS_STAG, (void *)&value);
    if (DRV_OK != enRet)
    {    	
        return DRV_ERR_UNKNOW; 
    }

	(void)_Hal_ClfCfgGet(uiClfRuleId, (void *)&stClfCfg);	
	stClfCfg.direction = CLASSIFY_DIRECTION_DS;
	stClfCfg.act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
	(void)_Hal_ClfCfgSet(uiClfRuleId, (void *)&stClfCfg);

    enRet = _Hal_ClfRuleBind(uiClfRuleId);
    if (DRV_OK != enRet)
    {    	
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

DRV_RET_E Hal_ClfRuleInit(void)
{
	Hal_ClfInit();

	(void)rtk_classify_cfSel_set(PortLogic2PhyID(LOGIC_PON_PORT), CLASSIFY_CF_SEL_ENABLE);

	(void)_Hal_Clf_CreateRuleForDelStag0();

	return DRV_OK;
}

#ifdef  __cplusplus
}
#endif


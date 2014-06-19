/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-06-24 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : OMCI driver layer module defination
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI (G.984.4)
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <module/gpon/gpon.h>
#include <module/gpon/gpon_defs.h>
#include <OMCI/src/omci_mib.h>
#include <OMCI/inc/omci_driver.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <pkt_redirect/pkt_redirect.h>

#include "omci_drv.h"
/*rtk api*/
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <rtk/classify.h>

static int tcontIdState[TXC_TCONT_TCONT_NUM];

static omci_work_t *omci_work;



#if 0
static int omci_cf_usActSet(OMCI_TRAFF_RULE_ts data,rtk_classify_us_act_t *pUsAct)
{
	
	UINT16   flowId;
	flowId = omci_GetUsFlowIdByPortId(data.outgress);
	pUsAct->sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
	pUsAct->sidQid    = flowId;

	if(data.operRule.ena)
	{
		switch(data.operRule.tagOper)
		{
		case 0:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Transparent",__FUNCTION__);
			pUsAct->cAct = CLASSIFY_US_CACT_TRANSPARENT;
			pUsAct->csAct = CLASSIFY_US_CSACT_TRANSPARENT;
		}
		break;
		case 1:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Assign",__FUNCTION__);
			pUsAct->cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
			pUsAct->csAct = CLASSIFY_US_CSACT_DEL_STAG;
			pUsAct->cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
			pUsAct->csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
			pUsAct->cVidAct  = CLASSIFY_US_VID_ACT_ASSIGN;
			pUsAct->cPriAct  = CLASSIFY_US_PRI_ACT_ASSIGN;
			pUsAct->cTagVid  = data.operRule.sVlanId & 0xfff;
			pUsAct->cTagPri  = data.operRule.sVlanId >> 13 & 0x7;
		}
		break;
		case 2:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Assign",__FUNCTION__);
			pUsAct->cAct = CLASSIFY_US_CACT_TRANSPARENT;
			pUsAct->csAct = CLASSIFY_US_CSACT_ADD_TAG_8100;
			pUsAct->csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
			pUsAct->csVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
			pUsAct->sTagVid  = data.operRule.sVlanId & 0xfff;
			pUsAct->sTagPri  = data.operRule.sVlanId >> 13 & 0x7;
		}
		break;
		default:
		break;
		}
	}
	return OMCI_ERR_OK;
}

static int omci_cf_usRuleSet(int ingress,rtk_classify_cfg_t *pCfg)
{
	static rtk_classify_field_t sfield;

	sfield.fieldType = CLASSIFY_FIELD_UNI;
	sfield.classify_pattern.uni.value = ingress & 0x7;
	sfield.classify_pattern.uni.mask  = 0x7;

	hal_classify_field_add(pCfg,&sfield);

	return OMCI_ERR_OK;
}


static int omci_cf_dsActSet(OMCI_TRAFF_RULE_ts data,rtk_classify_ds_act_t *pDsAct)
{
	
	int portNum = omci_GetUNIPortByID(data.outgress);
	pDsAct->uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
	pDsAct->uniMask.bits[0] = 1 << portNum;

	if(data.operRule.ena)
	{
		switch(data.operRule.tagOper)
		{
		case 0:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Transparent",__FUNCTION__);
			pDsAct->cAct = CLASSIFY_DS_CACT_TRANSPARENT;
			pDsAct->csAct = CLASSIFY_DS_CSACT_TRANSPARENT;
		}
		break;
		case 1:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Assign",__FUNCTION__);
			pDsAct->cAct = CLASSIFY_US_CACT_DEL_CTAG;
			pDsAct->csAct = CLASSIFY_US_CSACT_DEL_STAG;
		}
		break;
		default:
		break;
		}
	}	
	return OMCI_ERR_OK;
}


static int omci_cf_dsRuleSet(int ingress,rtk_classify_cfg_t *pCfg)
{	
	UINT16   flowId;
	int ret;
	static rtk_classify_field_t sfield;
	
	flowId = omci_GetDsFlowIdByPortId(ingress);
	
	if(TXC_ONT2G_GEM_PORT_NUM==flowId)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: DS rule set fail flowId=%d, ret=%d",__FUNCTION__,flowId,ret);
		return OMCI_ERR_FAILED;
	}

	sfield.fieldType = CLASSIFY_FIELD_TOS_DSIDX;
	sfield.classify_pattern.tosDsidx.value = flowId;
	sfield.classify_pattern.tosDsidx.mask  = 0x7f;
	if((ret=hal_classify_field_add(pCfg,&sfield))!=RT_ERR_OK)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: DS rule set fail flowId=%d, ret=%d",__FUNCTION__,flowId,ret);
		return OMCI_ERR_FAILED;
	}

	return OMCI_ERR_OK;
}


static int omci_cf_ruleAdd(PON_GEMPORT_DIRECTION dir,UINT16 servId,rtk_classify_cfg_t *pCfg)
{
	int32 ret;
	omci_cf_rule_t *pCfRule;
	
	pCfg->direction = (dir==PON_GEMPORT_DIRECTION_US) ?   CLASSIFY_DIRECTION_US : CLASSIFY_DIRECTION_DS;
	pCfg->valid = ENABLED;

	pCfg->index = omci_GetAvailCFId();

	if(pCfg->index==TXC_HW_CF_RULE_NUM)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: CF rule is full",__FUNCTION__);
		return OMCI_ERR_FAILED;
	}

	pCfg->invert = CLASSIFY_INVERT_DISABLE;
	

	ret = hal_classify_cfgEntry_add(pCfg);

	if(ret!=RT_ERR_OK)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s:  CF rule add failed,dir=%d,index =%d,ret=%d",__FUNCTION__,pCfg->direction,pCfg->index,ret);
		return OMCI_ERR_FAILED;
	}else{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s:  CF rule add ok,dir=%d, index =%d",__FUNCTION__,pCfg->direction,pCfg->index);
	}

	
	pCfRule = (omci_cf_rule_t*)malloc(sizeof(omci_cf_rule_t));	
	pCfRule->cfRuleId = pCfg->index;
	
	if(dir == PON_GEMPORT_DIRECTION_US){
		LIST_INSERT_HEAD(&gOmciUsServInfo[servId].cfHead,pCfRule,entries);
	}else
	{
		LIST_INSERT_HEAD(&gOmciDsServInfo[servId].cfHead,pCfRule,entries);
	}

	return OMCI_ERR_OK;
}


static int omci_cf_vlanFilterRuleSet(UINT16 servId,PON_GEMPORT_DIRECTION dir,
OMCI_VLAN_TAG_FLT_ts rule,rtk_classify_cfg_t *pCfg)
{
	static rtk_classify_field_t isCtag,isStag;
	static rtk_classify_field_t vidfield;
	static rtk_classify_field_t prifield;
	rtk_classify_field_t *plastfield = NULL;
	int i;
	
	if(rule.ena!=TRUE)
	{
		return GOS_OK;
	}
	

	switch(rule.fwdOper)
	{
	case PON_VLANFILTER_FWDOP_A_A:
	case PON_VLANFILTER_FWDOP_FV_A:	
	case PON_VLANFILTER_FWDOP_GV_A:	
	case PON_VLANFILTER_FWDOP_FP_A:	
	case PON_VLANFILTER_FWDOP_GP_A:	
	case PON_VLANFILTER_FWDOP_FT_A:	
	case PON_VLANFILTER_FWDOP_GT_A:
	{
		MIB_LOG("%s: filter > forward all",__FUNCTION__);
		omci_cf_ruleAdd(dir,servId,pCfg);
	}
	break;
	case PON_VLANFILTER_FWDOP_C_A:
	{		
		MIB_LOG("%s: filter > drop tag, forward untag",__FUNCTION__);	

		isCtag.fieldType = CLASSIFY_FIELD_IS_CTAG;
		isCtag.classify_pattern.isCtag.mask = 0x1;
		isCtag.classify_pattern.isCtag.value= 0x0;		
		isCtag.fieldType = CLASSIFY_FIELD_IS_STAG;
		isStag.classify_pattern.isStag.mask = 0x1;
		isStag.classify_pattern.isStag.value= 0x0;

		hal_classify_field_add(pCfg,&isCtag);
		hal_classify_field_add(pCfg,&isStag);

		omci_cf_ruleAdd(dir,servId,pCfg);

	}
	break;
	case PON_VLANFILTER_FWDOP_A_E:
	case PON_VLANFILTER_FWDOP_FV_E:	
	case PON_VLANFILTER_FWDOP_GV_E:
	case PON_VLANFILTER_FWDOP_FP_E:	
	case PON_VLANFILTER_FWDOP_GP_E:	
	case PON_VLANFILTER_FWDOP_FT_E:	
	case PON_VLANFILTER_FWDOP_GT_E:	
	{			
		MIB_LOG("%s: filter > forward tag, drop untag",__FUNCTION__);	

		isCtag.fieldType = CLASSIFY_FIELD_IS_CTAG;
		isCtag.classify_pattern.isCtag.mask = 0x1;
		isCtag.classify_pattern.isCtag.value= 0x1;
		isStag.fieldType = CLASSIFY_FIELD_IS_STAG;
		isStag.classify_pattern.isStag.mask = 0x1;
		isStag.classify_pattern.isStag.value= 0x1;

		hal_classify_field_add(pCfg,&isCtag);
		hal_classify_field_add(pCfg,&isStag);
		omci_cf_ruleAdd(dir,servId,pCfg);

	}
	break;
	case PON_VLANFILTER_FWDOP_HV_A:	
	case PON_VLANFILTER_FWDOP_HP_A:	
	case PON_VLANFILTER_FWDOP_HT_A:
	{
		MIB_LOG("%s: filter > forward list VID,others drop, forward untag",__FUNCTION__);

		vidfield.fieldType = CLASSIFY_FIELD_TAG_VID;		
		prifield.fieldType = CLASSIFY_FIELD_TAG_PRI;
		plastfield = pCfg->field.pFieldHead;
		while( plastfield->next !=NULL) { plastfield = plastfield->next ; }
		
		for(i=0;i<12;i++)
		{
			if(rule.cTci[i]!=0)
			{			
				vidfield.classify_pattern.tagVid.value = rule.cTci[i] & 0xfff;
				vidfield.classify_pattern.tagVid.mask  = 0xfff;
				prifield.classify_pattern.tagPri.value = (rule.cTci[i] >> 13) & 0x7;					
				prifield.classify_pattern.tagPri.mask  = 0x7;
				
				if(rule.fwdOper==PON_VLANFILTER_FWDOP_HV_A)
				{		
					
					hal_classify_field_add(pCfg,&vidfield);
					omci_cf_ruleAdd(dir,servId,pCfg);
					
				}else
				if(rule.fwdOper==PON_VLANFILTER_FWDOP_HP_A)
				{
					
					hal_classify_field_add(pCfg,&prifield);					
					omci_cf_ruleAdd(dir,servId,pCfg);
					
				}else
				{					
					hal_classify_field_add(pCfg,&vidfield);
					hal_classify_field_add(pCfg,&prifield);					
					omci_cf_ruleAdd(dir,servId,pCfg);
				}				
			}
		}
		/*remove vlan part*/
		plastfield->next = NULL;
		/*assign forward untag*/
		isCtag.fieldType = CLASSIFY_FIELD_IS_CTAG;
		isCtag.classify_pattern.isCtag.mask = 0x1;
		isCtag.classify_pattern.isCtag.value= 0x0;		
		isCtag.fieldType = CLASSIFY_FIELD_IS_STAG;
		isStag.classify_pattern.isStag.mask = 0x1;
		isStag.classify_pattern.isStag.value= 0x0;

		hal_classify_field_add(pCfg,&isCtag);
		hal_classify_field_add(pCfg,&isStag);

		omci_cf_ruleAdd(dir,servId,pCfg);
	}
	break;
	case PON_VLANFILTER_FWDOP_HV_E:
	case PON_VLANFILTER_FWDOP_HP_E:
	case PON_VLANFILTER_FWDOP_HT_E:
	{
		MIB_LOG("%s: filter > forward list VID,others drop, drop untag",__FUNCTION__);
		
		vidfield.fieldType = CLASSIFY_FIELD_TAG_VID;		
		prifield.fieldType = CLASSIFY_FIELD_TAG_PRI;
		
		for(i=0;i<12;i++)
		{
			if(rule.cTci[i]!=0)
			{			
				vidfield.classify_pattern.tagVid.value = rule.cTci[i] & 0xfff;
				vidfield.classify_pattern.tagVid.mask  = 0xfff;
				prifield.classify_pattern.tagPri.value = (rule.cTci[i] >> 13) & 0x7;					
				prifield.classify_pattern.tagPri.mask  = 0x7;
				
				if(rule.fwdOper==PON_VLANFILTER_FWDOP_HV_A)
				{		
					
					hal_classify_field_add(pCfg,&vidfield);
					omci_cf_ruleAdd(dir,servId,pCfg);
					
				}else
				if(rule.fwdOper==PON_VLANFILTER_FWDOP_HP_A)
				{
					
					hal_classify_field_add(pCfg,&prifield);					
					omci_cf_ruleAdd(dir,servId,pCfg);
					
				}else
				{					
					hal_classify_field_add(pCfg,&vidfield);
					hal_classify_field_add(pCfg,&prifield);					
					omci_cf_ruleAdd(dir,servId,pCfg);
				}				
			}
		}
	}
	break;
	default:
	break;
	}
	
	return GOS_OK;
}


static int omci_serviceRule_set(OMCI_TRAFF_RULE_ts traffRule, int servId)
{

	rtk_classify_cfg_t cfg;

	memset(&cfg,0,sizeof(rtk_classify_cfg_t));

	if(traffRule.dir == PON_GEMPORT_DIRECTION_US)
	{
		omci_cf_usActSet(traffRule,&cfg.act.usAct);
		omci_cf_usRuleSet(traffRule.ingress,&cfg);		
		omci_cf_vlanFilterRuleSet(servId,traffRule.dir,traffRule.fltRule,&cfg);		
	}
	else
	{
		omci_cf_dsActSet(traffRule,&cfg.act.dsAct);		
		omci_cf_dsRuleSet(traffRule.ingress,&cfg);		
		omci_cf_vlanFilterRuleSet(servId,traffRule.dir,traffRule.fltRule,&cfg);		
	}
	return OMCI_ERR_OK;
}

static UINT16 omci_GetAvailCFId(void)
{
    UINT16 cfId;
 	rtk_classify_cfg_t cfg;   
    for(cfId = CF_RULE_BASE; cfId < TXC_HW_CF_RULE_NUM; cfId++)
    {
    	cfg.index = cfId;
    	hal_classify_cfgEntry_get(&cfg);

		if(cfg.valid == DISABLED)
		{
			break;
		}
    }

    return (cfId == TXC_HW_CF_RULE_NUM ? TXC_HW_CF_RULE_NUM : cfId);
}

#endif


static int assignNonUsedTcontId(int allocId)
{
	int i;
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
	{
		if(tcontIdState[i]==0xff)
		{
			tcontIdState[i]=allocId;
			return i*2;
		}
	}
	return -1;
}

static int removeUsedTcontId(int tcontId)
{
	int i = tcontId/2;
	
	if(i > 0)
	tcontIdState[i]=0xff;

	return 0;
}

static int initUsedTcontId(void)
{
	int i;	
	rtk_gpon_tcont_ind_t ind;
	
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
	{
		if(tcontIdState[i]!=0xff){
			ind.alloc_id = tcontIdState[i];
			printk("init tcont allocId %d\n",ind.alloc_id);
		 	rtk_gponapp_tcont_destroy(&ind);
			tcontIdState[i]=0xff;
		}
	}
	return 0;
}


static void omci_allocateTcont(rtk_gpon_ploam_t *ploam)
{
	int ret;
	int alloc;
	rtk_gpon_tcont_ind_t ind;
	rtk_gpon_tcont_attr_t attr;
	
    alloc = (ploam->msg[0]<<4)|(ploam->msg[1]>>4);

	if(alloc==ploam->onuid)
		return;
	
	memset(&ind,0,sizeof(rtk_gpon_tcont_ind_t));
	memset(&attr,0,sizeof(rtk_gpon_tcont_attr_t));
	
	ind.alloc_id = alloc;
	ind.type  =RTK_GPON_TCONT_TYPE_1;
	
	ret = rtk_gponapp_tcont_get(&ind,&attr);

    /* de-allocate this alloc-id */
    if(ploam->msg[2]==0xFF && ret == RT_ERR_OK)
    {
	   removeUsedTcontId(attr.tcont_id);
       rtk_gponapp_tcont_destroy(&ind);
    }
    /* allocate this GEM alloc-id */
    else if(ploam->msg[2]==1 && ret !=RT_ERR_OK)
    {
       attr.tcont_id = assignNonUsedTcontId(alloc);	   
	   if(attr.tcont_id > 0)
	   {	   	
	   		ret = rtk_gponapp_tcont_create(&ind,&attr);
	   }
    }
	return;
}


static void omci_to_nic_work(struct work_struct *ws)
{
	int ret ;
	omci_work_t *work = (omci_work_t*) container_of(ws,omci_work_t, work) ;

	if((ret = rtk_gponapp_omci_tx(&work->omci))!=RT_ERR_OK)
	{
		return;
	}
	return ;
}

static void omci_send_to_user(rtk_gpon_omci_msg_t* omci)
{
	int ret;
	rtk_gpon_pkt_t  data;

	memset(&data,0,sizeof(rtk_gpon_pkt_t));
	data.type = RTK_GPON_MSG_OMCI;
	memcpy(&data.msg.omci,omci,sizeof(rtk_gpon_omci_msg_t));
	if((ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_GPONOMCI,1, RTK_GPON_OMCI_MSG_LEN, (unsigned char *)&data))!=RT_ERR_OK)
	{
		return ;
	}
	return;
}


static void omci_send_to_nic(unsigned short len,unsigned char *omci)
{
	int ret ;
	omci_work = kzalloc(sizeof(typeof(*omci_work)),GFP_KERNEL);
	memcpy(&omci_work->omci.msg[0],&omci[0],RTK_GPON_OMCI_MSG_LEN);
	INIT_WORK(&omci_work->work,omci_to_nic_work);
	schedule_work(&omci_work->work);
	return ;
}


static int omcc_create(void)
{
	int ret;
	rtk_ponmac_queue_t queue;
	rtk_ponmac_queueCfg_t queueCfg;
	/*queue assign*/
	queue.schedulerId = GPON_OMCI_TCONT_ID;
	queue.queueId = GPON_OMCI_QUEUE_ID;
	/*queue configure assign*/
	memset(&queueCfg,0,sizeof(rtk_ponmac_queueCfg_t));
	queueCfg.cir = 0;
	queueCfg.pir = 0x1ffff;
	queueCfg.type = STRICT_PRIORITY;
	/*add omcc tcont & queue*/
	if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
	{
		return ret;
	}
	/*assign strem id to tcont & queue*/
	if((ret = rtk_ponmac_flow2Queue_set(GPON_OMCI_FLOW_ID, &queue))!=RT_ERR_OK)
	{
		return ret;
	}
	return RT_ERR_OK;
}


static int omcc_del(void)
{
	int ret;
	rtk_ponmac_queue_t queue;

	/*queue assign*/
	queue.schedulerId = GPON_OMCI_TCONT_ID;
	queue.queueId = GPON_OMCI_QUEUE_ID;
	/*add omcc tcont & queue*/
	if((ret = rtk_ponmac_queue_del(&queue))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	/*assign strem id to tcont & queue*/
	queue.queueId = 31;
	queue.schedulerId = 24;
	if((ret = rtk_ponmac_flow2Queue_set(GPON_OMCI_FLOW_ID, &queue))!=RT_ERR_OK)
	{
		return ret;
	}
	return RT_ERR_OK;
}

static int multicast_flow_create(void)
{
	int ret;
    uint32 flowId = GPON_MULTICAST_FLOW_ID;
    rtk_gpon_dsFlow_attr_t attr;

	/*queue configure assign*/
	memset(&attr,0,sizeof(rtk_gpon_dsFlow_attr_t));
	attr.gem_port_id = GPON_MULTICAST_GEM_PORT;
	attr.type = RTK_GPON_FLOW_TYPE_ETH;
	/*add omcc tcont & queue*/
	if((ret = rtk_gponapp_dsFlow_set(flowId, &attr))!=RT_ERR_OK)
	{
		return ret;
	}

	return RT_ERR_OK;
}

static int omci_init(void)
{
	int32 ret;
	/*register omci callback function, register function for send packet to NIC to PON*/
	if((ret = pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GPONOMCI,omci_send_to_nic))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	/*register receive function from PON*/
	if((ret = rtk_gponapp_evtHdlOmci_reg(omci_send_to_user))!=RT_ERR_OK)
	{
		return ret;
	}
	/*create OMCI channel*/
	if((ret = omcc_create())!=RT_ERR_OK)
	{
		return ret;
	}
	/*create multicast gem port*/
    if((ret = multicast_flow_create())!=RT_ERR_OK)
	{
		return ret;
	}
}


static int omci_exit(void)
{
	int32 ret;
	/*remove receive GPON OMCI callback for send packet to NIC*/
	if((ret = pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_GPONOMCI))!=RT_ERR_OK)
	{
		return ret;
	}
	/*remove OMCI channel*/
	if((ret = omcc_del())!=RT_ERR_OK)
	{
		return ret;
	}
}

static rtk_gpon_eventHandleFunc_ploam_t omci_ploam_callback(rtk_gpon_ploam_t *ploam)
{
	if (ploam->type == GPON_PLOAM_DS_ASSIGNEDALLOCID)
	{		
		omci_allocateTcont(ploam);
		return GPON_PLOAM_STOP_WITH_ACK;
	}	
	return GPON_PLOAM_CONTINUE;
}

static int OMCI_wrapper_CreateTcont(OMCI_TCONT_ts *pTcont)
{
	int ret;
	rtk_gpon_tcont_ind_t ind;
	rtk_gpon_tcont_attr_t attr;

	memset(&ind,0,sizeof(rtk_gpon_tcont_ind_t));
	memset(&attr,0,sizeof(rtk_gpon_tcont_attr_t));

	ind.alloc_id = pTcont->allocId;
	ind.type = RTK_GPON_TCONT_TYPE_1;
	
	ret = rtk_gponapp_tcont_get(&ind,&attr);

	pTcont->tcontId = attr.tcont_id;
	
	return ret;
}


static int OMCI_wrapper_CfgGemFlow(OMCI_GEM_FLOW_ts *gemFlow)
{
    int ret,flowId;
    rtk_gpon_dsFlow_attr_t dsFlow;
	rtk_gpon_usFlow_attr_t usFlow;
	rtk_ponmac_queue_t queue;

	if(!gemFlow)
		return OMCI_ERR_FAILED;


	flowId = gemFlow->flowId;
    printk("Create/Delete Flow for Gem Port 0x%x in Direction[%d]\n",gemFlow->portId, gemFlow->dir);

	if(gemFlow->dir == PON_GEMPORT_DIRECTION_US || gemFlow->dir == PON_GEMPORT_DIRECTION_BI)
    {
      
        if (gemFlow->ena == TRUE)
        {
		    
		    memset(&usFlow, 0, sizeof(rtk_gpon_usFlow_attr_t));
		    usFlow.type = gemFlow->isOmcc ? RTK_GPON_FLOW_TYPE_OMCI : RTK_GPON_FLOW_TYPE_ETH;
		    usFlow.gem_port_id = gemFlow->portId;
		    usFlow.tcont_id = gemFlow->tcontPId;
	            
	        ret = rtk_gponapp_usFlow_set(gemFlow->flowId, &usFlow);

	        if (ret != RT_ERR_OK)
	        {
	            printk("could not create u/s gem flow [0x%x]\n", (int)ret);
	            return OMCI_ERR_FAILED;
	        }

		    queue.queueId = gemFlow->queuePId;
		    queue.schedulerId = gemFlow->tcontPId;
		
		    if(queue.queueId!=TXC_MAX_QUEUE_ID)
		    {
				ret = rtk_ponmac_flow2Queue_set(flowId,&queue);
		    }   

	        printk("create a u/s gem flow [%d]\n", flowId);

        }
        else 
        { 
            /* delete a gem u/s flow */
		    memset(&usFlow, 0, sizeof(rtk_gpon_usFlow_attr_t));
            ret = rtk_gponapp_usFlow_set(flowId, &usFlow);

            if (ret != RT_ERR_OK)
            {
                return OMCI_ERR_FAILED;
            }
            
            printk("delete a u/s gem flow [%d]\n", flowId);
        }
    }
    if (gemFlow->dir == PON_GEMPORT_DIRECTION_DS || gemFlow->dir == PON_GEMPORT_DIRECTION_BI)
    {        
        if (gemFlow->ena == TRUE)
        {   
            dsFlow.gem_port_id = gemFlow->portId;
            dsFlow.type        = gemFlow->isOmcc == TRUE ? RTK_GPON_FLOW_TYPE_OMCI : RTK_GPON_FLOW_TYPE_ETH;
            dsFlow.multicast   = DISABLED;
            dsFlow.aes_en      = DISABLED;
            
            ret = rtk_gponapp_dsFlow_set(flowId, &dsFlow);

            if (ret != RT_ERR_OK)
            {
                printk("could not create d/s gem flow [0x%x]\n", (int)ret);
                
                return OMCI_ERR_FAILED;
            }
            printk("create a d/s gem flow [%d]\n", flowId);

        }
        else
        {
            /* delete a gem d/s flow */
			dsFlow.gem_port_id 	= 0;
			dsFlow.type		 	= 0;
			dsFlow.multicast	= DISABLED;
			dsFlow.aes_en 	 	= DISABLED;
            ret = rtk_gponapp_dsFlow_set(flowId, &dsFlow);

            if (ret != RT_ERR_OK)
            {
                return OMCI_ERR_FAILED;
            }
            
            printk("delete a d/s gem flow [%d\n]", flowId);
        }
    }
    return OMCI_ERR_OK;
}


static int OMCI_wrapper_SetPriQueue(OMCI_PRIQ_ts *pPriQ)
{
	int ret;
	rtk_ponmac_queue_t  queue;
	rtk_ponmac_queueCfg_t queueCfg;

	memset(&queue,0,sizeof(rtk_ponmac_queue_t));
	memset(&queueCfg,0,sizeof(rtk_ponmac_queueCfg_t));

	queue.queueId = pPriQ->queueId;
	queue.schedulerId = pPriQ->tcontId;
	queueCfg.type = (pPriQ->scheduleType==PON_US_TRAFF_PRI_SCHE) ? STRICT_PRIORITY : WFQ_WRR_PRIORITY;
	queueCfg.cir = pPriQ->cir;
	queueCfg.pir = pPriQ->pir;
	queueCfg.egrssDrop = DISABLED;
	queueCfg.weight = pPriQ->weight;
	
	if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
	{
		printk("PriQ add failed\n");
	}

	return OMCI_ERR_OK;
	
}


static int OMCI_wrapper_DelClassifyRule(uint32 *ruleId)
{
	int ret;
	if(ruleId!=NULL)
	{
		if((ret = rtk_classify_cfgEntry_del(*ruleId))!=RT_ERR_OK)
		{
			printk("del cf rule %d failed\n",ruleId);
			return OMCI_ERR_FAILED;
		}
		
	}

	return OMCI_ERR_OK;
}

static int OMCI_wrapper_SetSerialNum(char *serial)
{
	int ret;
	rtk_gpon_serialNumber_t sn;
	sn.vendor[0] = serial[0];
	sn.vendor[1] = serial[1];
	sn.vendor[2] = serial[2];
	sn.vendor[3] = serial[3];
	sn.specific[0] = serial[4];
	sn.specific[1] = serial[5];
	sn.specific[2] = serial[6];
	sn.specific[3] = serial[7];
	
	ret = rtk_gponapp_serialNumber_set(&sn); 
	return ret;
}

static int OMCI_wrapper_GetSerialNum(char *serial)
{
	int ret;
	rtk_gpon_serialNumber_t sn;
	
	ret = rtk_gponapp_serialNumber_get(&sn);
	
	serial[0] = sn.vendor[0];
	serial[1] = sn.vendor[1]; 
	serial[2] = sn.vendor[2];
	serial[3] = sn.vendor[3];
	serial[4] = sn.specific[0];
	serial[5] = sn.specific[1];
	serial[6] = sn.specific[2]; 
	serial[7] = sn.specific[3];
	
	return ret;
}

static int OMCI_wrapper_ResetMib(void)
{
	initUsedTcontId();
}


static rtk_gpon_extMsgGetHandleFunc_t omci_ioctl_callback(rtk_gpon_extMsg_t *pExtMsg)
{
	int ret;
	switch(pExtMsg->optId){
	case OMCI_IOCTL_TCONT_GET:
		ret = OMCI_wrapper_CreateTcont((OMCI_TCONT_ts*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_GEMPORT_SET:
		ret = OMCI_wrapper_CfgGemFlow((OMCI_GEM_FLOW_ts*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_PRIQ_SET:
		ret = OMCI_wrapper_SetPriQueue((OMCI_PRIQ_ts*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_CF_DEL:
		ret = OMCI_wrapper_DelClassifyRule((uint32*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_CF_ADD:
		ret = OMCI_ERR_OK;
	break;
	case OMCI_IOCTL_SN_SET:
		ret = OMCI_wrapper_SetSerialNum((char*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_SN_GET:		
		ret = OMCI_wrapper_SetSerialNum((char*)pExtMsg->extValue);
	break;
	case OMCI_IOCTL_MIB_RESET:
		ret = OMCI_wrapper_ResetMib();
	break;
	default:
	break;
	}	
	return ret;
}



int __init omci_drv_init(void)
{
	
	int ret;
	ret = rtk_gponapp_initial(1);
	ret = rtk_classify_init();	
	ret = rtk_classify_unmatchAction_set(CLASSIFY_UNMATCH_DROP);
	omci_init();
	memset(&tcontIdState[0],0,TXC_TCONT_TCONT_NUM);
	rtk_gponapp_evtHdlPloam_reg(omci_ploam_callback);
	rtk_gponapp_callbackExtMsgGetHandle_reg(omci_ioctl_callback);
	
	return 0;
}


void __exit omci_drv_exit(void)
{
	omci_exit();
   	rtk_gponapp_evtHdlPloam_dreg();
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek OMCI kernel module");
MODULE_AUTHOR("Star Chang <starchang@realtek.com>");


module_init(omci_drv_init);
module_exit(omci_drv_exit);



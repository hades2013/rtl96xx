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
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Define the standard OAM callback and CTC extended OAM callback
 *
 * Feature : Provide standard and CTC related callback functions
 *
 */

/*
 * Include Files
 */
/* Standard include */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <linux/reboot.h>
/* EPON OAM include */
#include "epon_oam_config.h"
#include "epon_oam_err.h"
#include "epon_oam_db.h"
#include "epon_oam_dbg.h"
#include "epon_oam_rx.h"
/* User specific include */
#include "ctc_oam.h"
#include "ctc_oam_var.h"

#include "lw_type.h"/*add by shipeng*/
#include "lw_drv_pub.h"
#include "oam.h"
#include "oam_lw.h"
#include "mc_control.h"
#include <rtk/port.h>
#include <rtk/vlan.h>
#include <rtk/epon.h>
#include <rtk/rldp.h>
#include <hal/common/halctrl.h>
#include <ipc_protocol.h>
#include <lw_drv_req.h>
#include "oam_alarm.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "ufile_process.h"
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
/* MODIFY ME - use actual database instead */
ctc_dbaThreshold_t dbaThresholdDb = {
    .numQSet = 1,
    .reportMap[0] = 0xff,
    .queueSet[0].queueThreshold[0] = 0xffff,
    .queueSet[0].queueThreshold[1] = 0xffff,
    .queueSet[0].queueThreshold[2] = 0xffff,
    .queueSet[0].queueThreshold[3] = 0xffff,
    .queueSet[0].queueThreshold[4] = 0xffff,
    .queueSet[0].queueThreshold[5] = 0xffff,
    .queueSet[0].queueThreshold[6] = 0xffff,
    .queueSet[0].queueThreshold[7] = 0xffff,
};

unsigned char uni_port_auto_negotiation = 0x0f;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

extern BOOL CTC_CheckValidVidForMcTranslation(UINT32 vlanId);/*Begin add by huangmingjian 2013-10-17 for Bug 203*/


/* Dummy variable set function, always return OK.
 * This function is for debug usage only
 */

static int ctc_oam_get_portid(
	ctc_varInstant_t *pVarInstant,
	ctc_varDescriptor_t varDesc,
	ctc_varContainer_t *pVarContainer, 
	unsigned char *ucPortS, 
	unsigned char *ucPortE)
{
	if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH20)
	{
		if(0xFF == pVarInstant->varData[0])
		{
			*ucPortS = 1;
			*ucPortE = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			*ucPortS = pVarInstant->varData[0];
			*ucPortE = pVarInstant->varData[0];
		}
	}
	else if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH21)
	{
		if((pVarInstant->parse.uniPort.portType != CTC_VAR_INSTANT_PORTTYPE_ETHERNET) 
			|| ((pVarInstant->parse.uniPort.portNo > CTC_MAX_SUBPORT_NUM)
			&&(pVarInstant->parse.uniPort.portNo != 0xFF)/*add this condition for set classification about bug id 149 by shipeng 2013-08-01*/
			&&(pVarInstant->parse.uniPort.portNo != 0xFFFF)))
		{
			/*	回复报文
				-----------
				1 Branch
				-----------
				1 Leaf
				-----------
				1 Indication
				-----------
			*/
			pVarContainer->varDesc = varDesc;/*对于set操作:此信息已经在外层添加，但是我们此处重新添加也没有影响*/
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARBADPARAM;

			/*
			注意:对于回复报文我们需要打上标签,
			但是set操作会在外层自动打上，我们此处再
			打上也没有影响
			*/
			pVarContainer->varWidth |= CTC_VAR_CONTAINER_INDICATOR;
			
			return EPON_OAM_ERR_PARAM;
		}

		if((0xFF == pVarInstant->parse.uniPort.portNo)/*add this condition for set classification about bug id 149 by shipeng 2013-08-01*/
			||(0xFFFF == pVarInstant->parse.uniPort.portNo))
		{
			*ucPortS = 1;
			*ucPortE = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			*ucPortS = pVarInstant->parse.uniPort.portNo;
			*ucPortE = pVarInstant->parse.uniPort.portNo;
		}
	}	

	return EPON_OAM_ERR_OK;
}

static int ctc_oam_add_tlv(
	ctc_varInstant_t *pVarInstant,
	unsigned char *p_byte,
	unsigned char ucPortS)
{	
	oam_instance_tlv_t_2_0	*p_inst_2_0;
	oam_instance_tlv_t	*p_inst_2_1;
	unsigned char *tmp_ptr;	
	/*
			-----------------Index TLV
			branch
			-----------
			leaf
			-----------
			width
			-----------
			value
			-----------------
	*/

	if(NULL == p_byte)
	{
		return EPON_OAM_ERR_PARAM;
	}
	
	if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH20)
	{			
		p_inst_2_0 = (oam_instance_tlv_t_2_0 *)p_byte;
		p_inst_2_0->branch = pVarInstant->varDesc.varBranch;
		p_inst_2_0->leaf = pVarInstant->varDesc.varLeaf;
		p_inst_2_0->width = pVarInstant->varWidth;
		/*
			---------     ---------
			1	value   = 1 port ID
			---------     ---------
		*/
		p_inst_2_0->value = ucPortS;

		return sizeof(oam_instance_tlv_t_2_0);
	}
	else if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH21)
	{			
		p_inst_2_1 = (oam_instance_tlv_t *)p_byte;
		p_inst_2_1->branch = pVarInstant->varDesc.varBranch;
		p_inst_2_1->leaf = pVarInstant->varDesc.varLeaf;
		p_inst_2_1->width = pVarInstant->varWidth;
		/*
			---------     ---------
				4	value   = 1 portType
			---------     ---------
							1 chassisNo
							---------
							1 slotNo
							---------
							1 portNo
							---------
		*/
		tmp_ptr = (unsigned char *)&(p_inst_2_1->value);
		tmp_ptr[0] = pVarInstant->parse.uniPort.portType;
		tmp_ptr[1] = pVarInstant->parse.uniPort.chassisNo;
		tmp_ptr[2] = 0x00;
		tmp_ptr[3] = ucPortS;

		return sizeof(oam_instance_tlv_t);
	}
}

int ctc_oam_varCb_dummy_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	CTC_DEBUG("%s %d\n", __FUNCTION__, __LINE__);
    /* Pretend set operation complete */
    pVarContainer->varWidth = 0x80;
    return EPON_OAM_ERR_OK;
}

/*0xc7/0x0081 */
int ctc_oam_varCb_alarmAdmin_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=ERROR, ucPortS, ucPortE;
	oam_alarm_admin_entry_t* p_alarm_admin_entry;
	u8_t action;
	int ret_value =0;

	if(6 != pVarContainer->varWidth)
	{
		return EPON_OAM_ERR_UNKNOWN;
	}

	p_alarm_admin_entry = (oam_alarm_admin_entry_t*)pVarContainer->pVarData;

	/*get config*/
	if(CTC_OAM_VAR_ACPHYADMINCONTROL_DEACTIVE == p_alarm_admin_entry->alarm_config)
	{
		action = DISABLED;
	}
	else if(CTC_OAM_VAR_ACPHYADMINCONTROL_ACTIVE == p_alarm_admin_entry->alarm_config)
	{
		action = ENABLED;
	}
	else
	{
		CTC_DEBUG("\r\nunknown alarmAdmin action\r\n");
	}

	
	if(NULL != pVarInstant)
	{
		/*only eth action with instance*/
		printf("%s %d\n", __FILE__, __LINE__);
		ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
		if(EPON_OAM_ERR_PARAM == ret_value)
		{
			return EPON_OAM_ERR_OK;
		}
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			rlt = CTC_STACK_alarm_set_state(p_alarm_admin_entry->alarm_id, ucPortS, action);
			if(NO_ERROR != rlt)
			{
				goto send_rsp;
			}
		}
	}
	else
	{
		printf("%s %d\n", __FILE__, __LINE__);
		rlt = CTC_STACK_alarm_set_state(p_alarm_admin_entry->alarm_id, 0, action);
		if(NO_ERROR != rlt)
		{
			goto send_rsp;
		}
	}
	CTC_DEBUG("alarm_id %x  config %d\n", 
		p_alarm_admin_entry->alarm_id,
		p_alarm_admin_entry->alarm_config);
send_rsp:
	if(rlt != NO_ERROR)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
	else
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	return EPON_OAM_ERR_OK;
}
/*0xc7/0x0082 */
int ctc_oam_varCb_ponif_alarmThreshold_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=ERROR;
	oam_alarm_threshold_entry_t *p_alarm_threshold_entry;
	u8_t action;
	int ret_value =0;

	p_alarm_threshold_entry = (oam_alarm_threshold_entry_t *)pVarContainer->pVarData;
	rlt = CTC_STACK_alarm_set_thr(p_alarm_threshold_entry->alarm_id,
								 0,	
								 p_alarm_threshold_entry->raising_threshold,
								 p_alarm_threshold_entry->falling_threshold);

	CTC_DEBUG("unsigned alarm_id %d raising_thr %d falling_thr %d\n", 
		p_alarm_threshold_entry->alarm_id,
		p_alarm_threshold_entry->raising_threshold,
		p_alarm_threshold_entry->falling_threshold);
	

	CTC_DEBUG("signed alarm_id %d raising_thr %d falling_thr %d\n", 
		p_alarm_threshold_entry->alarm_id,
		(int)p_alarm_threshold_entry->raising_threshold,
		(int)p_alarm_threshold_entry->falling_threshold);
	if(NO_ERROR != rlt)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
	else
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	return EPON_OAM_ERR_OK;
}

/* 0x07/0x0025 - aPhyAdminState */
int ctc_oam_varCb_aPhyAdminState_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_phy_adminstate_t *p_admin;
    BOOL bPortStatus = TRUE;
	int ret_value =0;
	int tlv_len = 0;
	u32_t *ptmp_value;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		------------------------------------------------
		pVarContainer->varWidth		=		4
		------------------------------------------------
		pVarContainer->pVarData--------------	4 value
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = CTC_OAM_VAR_APHYADMINSTATE_LEN;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
	ptmp_value = (u32_t *)p_byte;
		
	if ( NO_ERROR == DRV_GetPortLinkState((UINT32)ucPortS, &bPortStatus))
	{
		if ( TRUE == bPortStatus )
		{
			*ptmp_value = CTC_OAM_VAR_APHYADMINSTATE_ENABLE;
		}
		else
		{
			*ptmp_value = CTC_OAM_VAR_APHYADMINSTATE_DISABLE;
		}
	}			
				
	p_byte += pVarContainer->varWidth;/*point TLV 2*/								
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{		
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width = 4
			-----------
			4 value
			-----------
		*/
		p_admin = (oam_phy_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_PHY_ADMIN;
		p_admin->width = CTC_OAM_VAR_APHYADMINSTATE_LEN;

        if ( NO_ERROR == DRV_GetPortLinkState((UINT32)ucPortS, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                p_admin->value = CTC_OAM_VAR_APHYADMINSTATE_ENABLE;
            }
            else
            {
                p_admin->value = CTC_OAM_VAR_APHYADMINSTATE_DISABLE;
            }
        }
        else
        {
            continue;
        }

		p_byte += sizeof(oam_phy_adminstate_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0x07/0x004F - aAutoNegAdminState */
int ctc_oam_varCb_aAutoNegAdminState_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_autoneg_adminstate_t *p_admin;
    BOOL bPortStatus = TRUE;
	int ret_value =0;
	int tlv_len = 0;
	u32_t *ptmp_value;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		------------------------------------------------
		pVarContainer->varWidth		=		4
		------------------------------------------------
		pVarContainer->pVarData--------------	4 value
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = CTC_OAM_VAR_AAUTONEGADMINSTATE_LEN;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
	ptmp_value = (u32_t *)p_byte;
		
	if ( NO_ERROR == DRV_GetPortAutoNegAdmin((UINT32)ucPortS, &bPortStatus))
	{
		if ( TRUE == bPortStatus )
		{
			*ptmp_value = CTC_OAM_VAR_AAUTONEGADMINSTATE_ENABLE;
		}
		else
		{
			*ptmp_value = CTC_OAM_VAR_AAUTONEGADMINSTATE_DISABLE;
		}
	}			
		
	p_byte += pVarContainer->varWidth;/*point TLV 2*/								
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{		
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width = 4
			-----------
			4 value
			-----------
		*/
		p_admin = (oam_autoneg_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_AUTONEG_ADMIN_STATE;
		p_admin->width = CTC_OAM_VAR_AAUTONEGADMINSTATE_LEN;

        if ( NO_ERROR == DRV_GetPortAutoNegAdmin((UINT32)ucPortS, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                p_admin->value = CTC_OAM_VAR_ETHPORTPAUSE_ENABLE;
            }
            else
            {
                p_admin->value = CTC_OAM_VAR_ETHPORTPAUSE_DISABLE;
            }
        }
        else
        {
            continue;
        }
		
		p_byte += sizeof(oam_autoneg_adminstate_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0x07/0x0052 - aAutoNegLocalTechnologyAbility */
int ctc_oam_varCb_aAutoNegLocalTechnologyAbility_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_autoneg_localability_t	*p_local;
	int ret_value =0;
	int tlv_len = 0;
	u32_t *ptmp_value;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	4 number
											------------
											4 first
											------------
											4 second
											------------
											4 third
											------------
											......
											-----------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = sizeof(oam_autoneg_localability_t) - 4;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;

	/* Value List extract from 802.3 30A
     * global (0),          --reserved for future use.
     * other (1),           --undefined
     * unknown (2),         --initializing, true ability not yet known.
     * 10BASE-T (14),       --10BASE-T as defined in Clause 14
     * 10BASE-TFD (142),    --Full duplex 10BASE-T as defined in Clauses
                              14 and 31
     * 100BASE-T4 (23),     --100BASE-T4 as defined in Clause 23
     * 100BASE-TX (25),     --100BASE-TX as defined in Clause 25
     * 100BASE-TXFD (252),  --Full duplex 100BASE-TX as defined in Clauses
                              25 and 31
     * 10GBASE-KX4 (483),   --10GBASE-KX4 PHY as defined in Clause 71
     * 10GBASE-KR (495),    --10GBASE-KR PHY as defined in Clause 72
     * 10GBASE-T (55),      --10GBASE-T PHY as defined in Clause 55
     * FDX PAUSE (312),     --PAUSE operation for full duplex links as 
                              defined in Annex 31B
     * FDX APAUSE (313),    --Asymmetric PAUSE operation for full duplex
                              links as defined in Clause 37 and Annex 31B
     * FDX SPAUSE (314),    --Symmetric PAUSE operation for full duplex
                              links as defined in Clause 37 and Annex 31B
     * FDX BPAUSE (315),    --Asymmetric and Symmetric PAUSE operation for
                              full duplex links as defined in Clause 37
                              and Annex 31B
     * 100BASE-T2 (32),     --100BASE-T2 as defined in Clause 32
     * 100BASE-T2FD (322),  --Full duplex 100BASE-T2 as defined in Clauses
                              31 and 32
     * 1000BASE-X (36),     --1000BASE-X as defined in Clause 36
     * 1000BASE-XFD (362),  --Full duplex 1000BASE-X as defined in Clause 36
     * 1000BASE-KX (393),   --1000BASE-KX PHY as defined in Clause 70
     * 1000BASE-T (40),     --1000BASE-T UTP PHY as defined in Clause 40
     * 1000BASE-TFD (402),  --Full duplex 1000BASE-T UTP PHY to be defined
                              in Clause 40
     * Rem Fault1 (37),     --Remote fault bit 1 (RF1) as specified in Clause 37
     * Rem Fault2 (372),    --Remote fault bit 2 (RF1) as specified in Clause 37
     * isoethernet (8029)   --802.9 ISLAN-16T
     */

    /* Number of the Enumerated - 8 enumerated */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = 8;
    p_byte += 4;

    /* 1st Enumerated - 1000BASE-T */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
    p_byte += 4;

	/* 2nd Enumerated - 1000BASE-TFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
    p_byte += 4;

    /* 3rd Enumerated - 100BASE-TX */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
    p_byte += 4;

	/* 4th Enumerated - 100BASE-TXFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
    p_byte += 4;

    /* 5th Enumerated - 10BASE-T */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
    p_byte += 4;

	/* 6th Enumerated - 10BASE-TFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
    p_byte += 4;

	/* 7th Enumerated - FDX SPAUSE */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
    p_byte += 4;

	/* 8th Enumerated - FDX PAUSE */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
    p_byte += 4;/*point TLV 2*/

	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}
	
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			4 number
			-----------
			4 first
			-----------
			4 second
			-----------
			4 third
			-----------
			......
			-----------
		*/
		p_local = (oam_autoneg_localability_t*)p_byte;
		p_local->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_local->leaf = OAM_ATTR_AUTONEG_LOCAL_ABILITY;
		p_local->width = sizeof(oam_autoneg_localability_t) - 4;
		p_local->number = 8;
		p_local->first = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
		p_local->second = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
		p_local->third = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
		p_local->fourth = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
		p_local->fifth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
		p_local->sixth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
		p_local->seventh = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
		p_local->eighth = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
		p_byte += sizeof(oam_autoneg_localability_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

    return EPON_OAM_ERR_OK;
}

/* 0x07/0x0053 - aAutoNegAdvertisedTechnologyAbility */
int ctc_oam_varCb_aAutoNegAdvertisedTechnologyAbility_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_autoneg_advability_t	*p_adv;
	int ret_value =0;
	int tlv_len = 0;
	u32_t *ptmp_value;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	4 number
											------------
											4 first
											------------
											4 second
											------------
											4 third
											------------
											......
											-----------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = sizeof(oam_autoneg_advability_t) - 4;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
	
    /* Value List extract from 802.3 30A
     * global (0),          --reserved for future use.
     * other (1),           --undefined
     * unknown (2),         --initializing, true ability not yet known.
     * 10BASE-T (14),       --10BASE-T as defined in Clause 14
     * 10BASE-TFD (142),    --Full duplex 10BASE-T as defined in Clauses
                              14 and 31
     * 100BASE-T4 (23),     --100BASE-T4 as defined in Clause 23
     * 100BASE-TX (25),     --100BASE-TX as defined in Clause 25
     * 100BASE-TXFD (252),  --Full duplex 100BASE-TX as defined in Clauses
                              25 and 31
     * 10GBASE-KX4 (483),   --10GBASE-KX4 PHY as defined in Clause 71
     * 10GBASE-KR (495),    --10GBASE-KR PHY as defined in Clause 72
     * 10GBASE-T (55),      --10GBASE-T PHY as defined in Clause 55
     * FDX PAUSE (312),     --PAUSE operation for full duplex links as 
                              defined in Annex 31B
     * FDX APAUSE (313),    --Asymmetric PAUSE operation for full duplex
                              links as defined in Clause 37 and Annex 31B
     * FDX SPAUSE (314),    --Symmetric PAUSE operation for full duplex
                              links as defined in Clause 37 and Annex 31B
     * FDX BPAUSE (315),    --Asymmetric and Symmetric PAUSE operation for
                              full duplex links as defined in Clause 37
                              and Annex 31B
     * 100BASE-T2 (32),     --100BASE-T2 as defined in Clause 32
     * 100BASE-T2FD (322),  --Full duplex 100BASE-T2 as defined in Clauses
                              31 and 32
     * 1000BASE-X (36),     --1000BASE-X as defined in Clause 36
     * 1000BASE-XFD (362),  --Full duplex 1000BASE-X as defined in Clause 36
     * 1000BASE-KX (393),   --1000BASE-KX PHY as defined in Clause 70
     * 1000BASE-T (40),     --1000BASE-T UTP PHY as defined in Clause 40
     * 1000BASE-TFD (402),  --Full duplex 1000BASE-T UTP PHY to be defined
                              in Clause 40
     * Rem Fault1 (37),     --Remote fault bit 1 (RF1) as specified in Clause 37
     * Rem Fault2 (372),    --Remote fault bit 2 (RF1) as specified in Clause 37
     * isoethernet (8029)   --802.9 ISLAN-16T
     */

    /* Number of the Enumerated - 8 enumerated */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = 8;
    p_byte += 4;

    /* 1st Enumerated - 1000BASE-T */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
    p_byte += 4;

	/* 2nd Enumerated - 1000BASE-TFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
    p_byte += 4;

    /* 3rd Enumerated - 100BASE-TX */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
    p_byte += 4;

	/* 4th Enumerated - 100BASE-TXFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
    p_byte += 4;

    /* 5th Enumerated - 10BASE-T */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
    p_byte += 4;

	/* 6th Enumerated - 10BASE-TFD */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
    p_byte += 4;

	/* 7th Enumerated - FDX SPAUSE */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
    p_byte += 4;

	/* 8th Enumerated - FDX PAUSE */
    ptmp_value = (u32_t *)p_byte;
	*ptmp_value = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
    p_byte += 4;/*point TLV 2*/

    if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}
	
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			4 number
			-----------
			4 first
			-----------
			4 second
			-----------
			4 third
			-----------
			......
			-----------
		*/
		p_adv = (oam_autoneg_advability_t*)p_byte;
		p_adv->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_adv->leaf = OAM_ATTR_AUTONEG_ADV_ABILITY;
		p_adv->width = sizeof(oam_autoneg_advability_t) - 4;
		p_adv->number = 8;
		p_adv->first = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
		p_adv->second = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
		p_adv->third = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
		p_adv->fourth = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
		p_adv->fifth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
		p_adv->sixth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
		p_adv->seventh = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
		p_adv->eighth = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
		p_byte += sizeof(oam_autoneg_advability_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

    return EPON_OAM_ERR_OK;
}

/* 0x07/0x0139 - aFECAbility */
int ctc_oam_varCb_aFECAbility_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
    unsigned int getValue;

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_AFECABILITY_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Value - always return FEC is supported */
    getValue = CTC_OAM_VAR_AFECABILITY_SUPPORTED;
    pVarContainer->pVarData[0] = ((unsigned char *)&getValue)[0];
    pVarContainer->pVarData[1] = ((unsigned char *)&getValue)[1];
    pVarContainer->pVarData[2] = ((unsigned char *)&getValue)[2];
    pVarContainer->pVarData[3] = ((unsigned char *)&getValue)[3];

    return EPON_OAM_ERR_OK;
}

/* 0x07/0x013A - aFECmode */
int ctc_oam_varCb_aFecMode_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler allocate data field in container, 
                                          * caller will release it
                                          */
{
    int ret;
	unsigned int state;
    unsigned int getValue;

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_AFECMODE_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    ret = DRV_GetFecMode(&state);
    if(NO_ERROR != ret)
    {
    	CTC_DEBUG("aFecMode_get fail\n");
        return EPON_OAM_ERR_UNKNOWN;
    }

    if((state != ENABLED) && (state != DISABLED))
    {
        getValue = CTC_OAM_VAR_AFECMODE_UNKNOWN;
    }
    else
    {
        if(ENABLED == state)
        {
            getValue = CTC_OAM_VAR_AFECMODE_ENABLED;
        }
        else
        {
            getValue = CTC_OAM_VAR_AFECMODE_DISABLED;
        }
    }

    /* Construct return value */
    CTC_BUF_ENCODE32(pVarContainer->pVarData, &getValue);

    return EPON_OAM_ERR_OK;
}

/* 0x07/0x013A - aFECmode */
int ctc_oam_varCb_aFecMode_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    int ret;
    unsigned int setValue;

    if(CTC_OAM_VAR_AFECMODE_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }
    CTC_BUF_PARSE32(pVarContainer->pVarData, &setValue);

    /* Configure both US and DS FEC */
    switch(setValue)
    {
    case CTC_OAM_VAR_AFECMODE_ENABLED:
        do {
            ret = DRV_SetFecMode(ENABLED);
            if(NO_ERROR != ret)
            {
            	CTC_DEBUG("aFecMode_set ENABLED fail\n");
                pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                break;
            }
            pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
        } while(0);
        break;
    case CTC_OAM_VAR_AFECMODE_DISABLED:
        do {
            ret = DRV_SetFecMode(DISABLED);
            if(NO_ERROR != ret)
            {
            	CTC_DEBUG("aFecMode_set DISABLED fail\n");
                pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                break;
            }
            pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
        } while(0);
        break;
    case CTC_OAM_VAR_AFECMODE_UNKNOWN:
    default:
        pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARBADPARAM;
        break;
    }

    return EPON_OAM_ERR_OK;
}

/* 0x09/0x0005 - acPhyAdminControl */
int ctc_oam_varCb_acPhyAdminControl_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE;
	oam_phy_admincontrol_t p_phy_admin;
	u32_t action;
	int ret_value =0;

	if(CTC_OAM_VAR_ACPHYADMINCONTROL_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the phy_admin config*/
	p_phy_admin.branch 	= pVarContainer->varDesc.varBranch;
	p_phy_admin.leaf 	= pVarContainer->varDesc.varLeaf;
	p_phy_admin.width 	= pVarContainer->varWidth;
	memcpy((unsigned char *)(&p_phy_admin.value), pVarContainer->pVarData, 4);
		
	if(CTC_OAM_VAR_ACPHYADMINCONTROL_DEACTIVE == p_phy_admin.value)
	{
		action = DISABLED;
	}
	else if(CTC_OAM_VAR_ACPHYADMINCONTROL_ACTIVE == p_phy_admin.value)
	{
		action = ENABLED;
	}
	else
	{
		CTC_DEBUG("\r\nunknown PhyAdmin action\r\n");
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's PhyAdmin set enable: %d\n", ucPortS, action);
        rlt = DRV_SetPortLinkState (ucPortS, action);
		if(rlt == OPL_OK)
		{
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
		}
		else
		{
			CTC_DEBUG("PhyAdminControl_set fail\n");
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
		}
	}

	return EPON_OAM_ERR_OK;
}

/* 0x09/0x000B - acAutoNegRestartAutoConfig */
int ctc_oam_varCb_acAutoNegRestartAutoConfig_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE;
	int ret_value =0;

	if(CTC_OAM_VAR_ACAUTONEGRESTARTAUTOCONFIG_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's AutoNeg restart\n", ucPortS);
        rlt = DRV_PortRestartNeg(ucPortS);
		if(rlt == OPL_OK)
		{
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
		}
		else
		{
			CTC_DEBUG("AutoNegRestartAutoConfig_set fail\n");
		    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
		}
	}

	return EPON_OAM_ERR_OK;
}

/* 0x09/0x000C - acAutoNegAdminControl */
int ctc_oam_varCb_acAutoNegAdminControl_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE;
	oam_autoneg_admincontrol_t p_auto_admin;
	u32_t action;
	int ret_value =0;

	if(CTC_OAM_VAR_ACAUTONEGADMINCONTROL_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the auto_admin config*/
	p_auto_admin.branch 	= pVarContainer->varDesc.varBranch;
	p_auto_admin.leaf 	= pVarContainer->varDesc.varLeaf;
	p_auto_admin.width 	= pVarContainer->varWidth;
	memcpy((unsigned char *)(&p_auto_admin.indication), pVarContainer->pVarData, 4);

	if(CTC_OAM_VAR_ACAUTONEGADMINCONTROL_DEACTIVE == p_auto_admin.indication)
	{
		action = DISABLED;
	}
	else if(CTC_OAM_VAR_ACAUTONEGADMINCONTROL_ACTIVE == p_auto_admin.indication)
	{
		action = ENABLED;
	}
	else
	{
		CTC_DEBUG("\r\nunknown AutoNegAdmin action\r\n");
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's AutoNegAdmin set enable: %d\n", ucPortS, action);
        rlt = DRV_SetPortAutoNeg((u32_t)ucPortS, action);
		if(rlt == OPL_OK)
		{
			if(1 == action)
			{
				/*set bit*/
				CTC_DEBUG("auto nego\n");
				uni_port_auto_negotiation |= 1<<(ucPortS-1);
			}
			else
			{
				/*clear bit*/
				CTC_DEBUG("no auto nego\n");
				uni_port_auto_negotiation &= ~(1<<(ucPortS-1));
			}
	
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
		}
		else
		{
			CTC_DEBUG("AutoNegAdminControl_set fail\n");
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
		}
	}	

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0001 - ONU SN */
int ctc_oam_varCb_onuSn_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	extern unsigned char oam_onu_vendor[4];
    unsigned char *pPtr;
	unsigned char modelStr[4] = {0x0,0x0,0x02,0x54};
    unsigned char hwVerStr[8] = {0x41,0x00};
	unsigned char swVerStr[16] = {0x31,0x31,0x38,0x0,0x0,0x0};
    oam_config_t oamConfig;
	OLT_FLAG_E		olt_flag;
	oam_onu_sn_t	*p_sn;
#define u8_t 	unsigned char

	/*YOTC olt*/
	u8_t	yotc_onu_model[4] = {0X00,0X00,0X01,0X41};
	u8_t	yotc_chip_vendor[2] = {0x45,0x36};
	u8_t	yotc_chip_model[2] = {0x63,0x01};
	u8_t	yotc_chip_revision = 0x00;
	u8_t	yotc_chip_version[3] = {0x06, 0x09, 0x1B};
	u8_t	yotc_firm_ver[FIRM_LENGTH]={0x01, 0x04};
	u8_t	yotc_onu_hwarever[8] = "T_Ver.A";
	u8_t	yotc_onu_swarever[16] = {'V','4','0','0','R','0','0','1','B','0','1','2',0};

	u8_t	zte_onu_model[4] = {'D','4','0','0'};

	u8_t	oam_onu_model[4] = {0x0,0x0,0x07,0x04};
	u8_t	oam_chip_vendor[2] = {0x45,0x30};
	u8_t	oam_chip_model[2] = {0x67,0x50};
	u8_t	oam_chip_revision = 0xA0;
	u8_t	oam_chip_version[3] = {0x08, 0x03, 0x1B};
	u8_t	oam_firm_ver[FIRM_LENGTH]={0x01, 0x00};
	u8_t	oam_onu_hwarever[MAX_BUILDTIME_SIZE] = {0};
	u8_t	oam_onu_hwarever_default[8] = {'A', '0'};
//	u8_t	oam_onu_swarever[16] = {'E','P','N','1','0','4','N','V','1','0','0','R','0','0','1'};
	u8_t	oam_onu_swarever[16] = CONFIG_RELEASE_VER;

	/*raisecom olt*/
	u8_t	raisecom_onu_model[4] = {0X00,0X07,0Xca,0X2a};
	u8_t	raisecom_chip_vendor[2] = {0x45,0x36};
	u8_t	raisecom_chip_model[2] = {0x63,0x01};
	u8_t	raisecom_chip_revision = 0x00;
	u8_t	raisecom_chip_version[3] = {0x06, 0x09, 0x1B};
	u8_t	raisecom_firm_ver[FIRM_LENGTH]={0x01, 0x04};
	u8_t	raisecom_onu_hwarever[8] = {0,0,0,0,0,0x41,0x2e,0x30};
	u8_t	raisecom_onu_swarever[16] = {0x00,0x31,0x2E,0x34,0x32,0x2E,0x39,0x5F,
	0x32,0x30,0x31,0x32,0x30,0x39,0x32,0x35};

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = 38; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    pPtr = pVarContainer->pVarData;

	
	#if 0
    /* Vendor ID - RTK */
    memcpy(pPtr, oam_onu_vendor,
        sizeof(oam_onu_vendor) > 4 ? 4 : sizeof(oam_onu_vendor));
    pPtr += sizeof(oam_onu_vendor);
    
    /* ONU model - blank model */
    memcpy(pPtr, modelStr,
        sizeof(modelStr) > 4 ? 4 : sizeof(modelStr));
    pPtr += sizeof(modelStr);

    /* ONU ID - Local device MAC address */
    /* Get MAC address from EPON OAM stack configuration */
    epon_oam_config_get(llidIdx, &oamConfig);
    memcpy(pPtr, oamConfig.macAddr, sizeof(oamConfig.macAddr));
    pPtr += sizeof(oamConfig.macAddr);

    /* MODIFY ME - Get actual HW version */
    /* Hardware Version - dummy HW version */
    memcpy(pPtr, hwVerStr, sizeof(hwVerStr) > 8 ? 8 : sizeof(hwVerStr));
    pPtr += sizeof(hwVerStr);

    /* MODIFY ME - Get actual SW version */
    /* Software Version - dummy SW version */
    memcpy(pPtr, swVerStr, sizeof(swVerStr) > 16 ? 16 : sizeof(swVerStr));
    pPtr += sizeof(swVerStr);
	#endif
	#if 1
	p_sn = (oam_onu_sn_t*)pPtr;
	olt_flag = OamGetOltFlag();

	
	if(0 != GetHardVersion(oam_onu_hwarever, sizeof(oam_onu_hwarever)))
	{
		memcpy(oam_onu_hwarever, oam_onu_hwarever_default, sizeof(oam_onu_hwarever_default));
	}
	switch(olt_flag)
	{
        /*Begin modified by linguobin 2013-12-3 for bug 290*/
        /*Reporting capabilities need not match model except RAISECOM*/
#if 0
		case OLT_FLAG_YOTC:
//			oam_onu_vendor[0] = 'Y';
//			oam_onu_vendor[1] = 'O';
//			oam_onu_vendor[2] = 'T';
//			oam_onu_vendor[3] = 'U';
			vosMemCpy(p_sn->model, yotc_onu_model, 4);
			vosMemCpy(p_sn->hware_ver,yotc_onu_hwarever, 8);
			vosMemCpy(p_sn->sware_ver,yotc_onu_swarever, 16);
			vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
			break;
#endif
		case OLT_FLAG_RAISECOM:
//			oam_onu_vendor[0] = 'R';
//			oam_onu_vendor[1] = 'C';
//			oam_onu_vendor[2] = 0;
//			oam_onu_vendor[3] = 0;
			vosMemCpy(p_sn->model, raisecom_onu_model, 4);
			vosMemCpy(p_sn->hware_ver,raisecom_onu_hwarever, 8);
			vosMemCpy(p_sn->sware_ver,raisecom_onu_swarever, 16);
			vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
			break;
		case OLT_FLAG_ZTE:
			vosMemCpy(p_sn->model, zte_onu_model, 4);
			vosMemCpy(p_sn->hware_ver,oam_onu_hwarever, 8);
			vosMemCpy(p_sn->sware_ver,oam_onu_swarever, 16);
			vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
			break;
#if 0
        /*add by linguobin 2013-11-28*/
        case OLT_FLAG_FIBERHOME:
			vosMemCpy(p_sn->model, oam_onu_model, 4);
			vosMemCpy(p_sn->hware_ver,oam_onu_hwarever, 8);
			vosMemCpy(p_sn->sware_ver,oam_onu_swarever, 16);
			vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
			break;
        /*end add by linguobin 2013-11-28*/
#endif
        /*End modified by linguobin 2013-12-3 for bug 290*/
		default:
			vosMemCpy(p_sn->model, oam_onu_model, 4);
			vosMemCpy(p_sn->hware_ver,oam_onu_hwarever, 8);
			vosMemCpy(p_sn->sware_ver,oam_onu_swarever, 16);
			vosMemCpy(p_sn->vendor_id, modelStr, 4);
			break;
	}
	
	epon_oam_config_get(llidIdx, &oamConfig);
	vosMemCpy(p_sn->onu_id, oamConfig.macAddr, 6);
	vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
	pPtr += sizeof(oam_onu_sn_t);
	#endif
    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0002 - FirmwareVer */
int ctc_oam_varCb_firmwareVer_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	unsigned char oam_firm_ver[2]={0x01, 0x04};

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = sizeof(oam_firm_ver);
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }
	
	memcpy(pVarContainer->pVarData, oam_firm_ver, pVarContainer->varWidth);

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0003 - Chipset ID */
int ctc_oam_varCb_chipsetId_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = 0x08; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Vender ID - JEDEC ID (dummy ID here) */
    pVarContainer->pVarData[0] = 0x45;
    pVarContainer->pVarData[1] = 0x36;

    /* Chip Model - Dummy model here */
    pVarContainer->pVarData[2] = 0x63;
    pVarContainer->pVarData[3] = 0x01;

    /* Revision - Dummy revision here */
    pVarContainer->pVarData[4] = 0x00;

    /* IC_Version/Date - data */
    pVarContainer->pVarData[5] = 0x08;
    pVarContainer->pVarData[6] = 0x03;
    pVarContainer->pVarData[7] = 0x1B;
    
    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0004 - ONU capabilities-1 */
int ctc_oam_varCb_onuCapabilities1_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	/*for yotc olt bug, product-type and capbility dismatch*/
	OLT_FLAG_E		olt_flag;
	
    pVarContainer->varDesc = varDesc;
    /* For sample system, pretend it is a 4 ports GE SFU w/o multiple LLID */
    pVarContainer->varWidth = 0x1A; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

	
	olt_flag = OamGetOltFlag();

    /* ServiceSupported - Support GE ports */
    pVarContainer->pVarData[0] = 0x01;

    /* Number of GE Ports - 4 GE ports */
#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
/*for yotc olt bug, product-type and capbility dismatch*/
/*Begin modified by linguobin 2013-12-3 for bug 290*/
/*Ability to report the actual*/
	pVarContainer->pVarData[1] = 0x04;
	pVarContainer->pVarData[9] = 0x0f;
/*End modified by linguobin 2013-12-3 for bug 290*/
    /* Bitmap of GE Ports - port 1~4 */
    pVarContainer->pVarData[2] = 0x00;
    pVarContainer->pVarData[3] = 0x00;
    pVarContainer->pVarData[4] = 0x00;
    pVarContainer->pVarData[5] = 0x00;
    pVarContainer->pVarData[6] = 0x00;
    pVarContainer->pVarData[7] = 0x00;
    pVarContainer->pVarData[8] = 0x00;
//   pVarContainer->pVarData[9] = 0x0f;
#elif defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG)
	pVarContainer->pVarData[1] = 0x01;

    /* Bitmap of GE Ports - port 1~2 */
    pVarContainer->pVarData[2] = 0x00;
    pVarContainer->pVarData[3] = 0x00;
    pVarContainer->pVarData[4] = 0x00;
    pVarContainer->pVarData[5] = 0x00;
    pVarContainer->pVarData[6] = 0x00;
    pVarContainer->pVarData[7] = 0x00;
    pVarContainer->pVarData[8] = 0x00;
    pVarContainer->pVarData[9] = 0x01;
#elif defined(CONFIG_PRODUCT_5500)
	pVarContainer->pVarData[1] = 0x03;

    /* Bitmap of GE Ports - port 1~2 */
    pVarContainer->pVarData[2] = 0x00;
    pVarContainer->pVarData[3] = 0x00;
    pVarContainer->pVarData[4] = 0x00;
    pVarContainer->pVarData[5] = 0x00;
    pVarContainer->pVarData[6] = 0x00;
    pVarContainer->pVarData[7] = 0x00;
    pVarContainer->pVarData[8] = 0x00;
    pVarContainer->pVarData[9] = 0x07;
#endif
/*for yotc olt bug, product-type and capbility dismatch*/
    /*Begin modified by linguobin 2013-12-3 for bug 290*/
    /*Ability to report the actual*/
	pVarContainer->pVarData[10] = 0x00;
	pVarContainer->pVarData[18] = 0x00;
    /*End modified by linguobin 2013-12-3 for bug 290*/
    /* Number of FE Ports - No FE ports */
//    pVarContainer->pVarData[10] = 0x04;

    /* Bitmap of FE Ports - None */
    pVarContainer->pVarData[11] = 0x00;
    pVarContainer->pVarData[12] = 0x00;
    pVarContainer->pVarData[13] = 0x00;
    pVarContainer->pVarData[14] = 0x00;
    pVarContainer->pVarData[15] = 0x00;
    pVarContainer->pVarData[16] = 0x00;
    pVarContainer->pVarData[17] = 0x00;
//    pVarContainer->pVarData[18] = 0x00;

    /* Number of POTS ports - No POTS ports */
    pVarContainer->pVarData[19] = 0x00;

    /* Number of E1 ports - No E1 ports */
    pVarContainer->pVarData[20] = 0x00;

    /* Number of US Queues - No US queue */
    pVarContainer->pVarData[21] = 0x08;

    /* QueueMax per Port US - No queue per port */
    pVarContainer->pVarData[22] = 0x08;

    /* Number of DS Queues - No DS queue */
    pVarContainer->pVarData[23] = 0x08;

    /* QueueMax per Port DS - No queue per port */
    pVarContainer->pVarData[24] = 0x08;

    /* Battery Backup - No battery backup */
    pVarContainer->pVarData[25] = 0x00;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0005 - OpticalTransceiverDiagnosis */
int ctc_oam_varCb_opticalTransceiverDiagnosis_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    /* For sample system, pretend it is a 4 ports GE SFU w/o multiple LLID */
    pVarContainer->varWidth = 0x0A; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

	/*Begin add by huangmingjian 2013-08-27*/
	rtk_transceiver_data_app_t para;
    /* TransceiverTemperature - dummy temperature */
	memset(&para, 0, sizeof(para));
	if(0 != Ioctl_GetPonmacTransceiver(RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE_, &para))
	{
		printf("Get pon TEMPERATURE fail\n");
		return EPON_OAM_ERR_PARAM;
	}
    pVarContainer->pVarData[0] = para.buf[0];
    pVarContainer->pVarData[1] = para.buf[1];

    /* Supply Voltage */
	memset(&para, 0, sizeof(para));
	if(0 != Ioctl_GetPonmacTransceiver(RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE_, &para))
	{
		printf("Get pon VOLTAGE fail\n");
		return EPON_OAM_ERR_PARAM;
	}	
    pVarContainer->pVarData[2] = para.buf[0];
    pVarContainer->pVarData[3] = para.buf[1];

    /* Tx bias current */
    pVarContainer->pVarData[4] = 0x55;
    pVarContainer->pVarData[5] = 0x66;

    /* Tx power (output) */
	memset(&para, 0, sizeof(para));
	if(0 != Ioctl_GetPonmacTransceiver(RTK_TRANSCEIVER_PARA_TYPE_TX_POWER_, &para))
	{
		printf("Get pon TX_POWER fail\n");
		return EPON_OAM_ERR_PARAM;
	}	
    pVarContainer->pVarData[6] = para.buf[0];
    pVarContainer->pVarData[7] = para.buf[1];

    /* Rx power (input) */
	memset(&para, 0, sizeof(para));
	if(0 != Ioctl_GetPonmacTransceiver(RTK_TRANSCEIVER_PARA_TYPE_RX_POWER_, &para))
	{
		printf("Get pon RX_POWER fail\n");
		return EPON_OAM_ERR_PARAM;
	}		
    pVarContainer->pVarData[8] = para.buf[0];
    pVarContainer->pVarData[9] = para.buf[1];
	
	/*End add by huangmingjian 2013-08-27*/

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0006 - Service SLA */
int ctc_oam_varCb_serviceSla_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)	/* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t operation;
	u8_t queue_num;
	u8_t *p_byte;
	oam_service_sla_body_hdr_t *p_sla_body_hdr;
	oam_service_sla_body_entry_t *p_sla_body_entry;
	u8_t i;

	pVarContainer->varDesc = varDesc;    
    pVarContainer->pVarData = (unsigned char *) malloc(1500);
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }
	p_byte = pVarContainer->pVarData;

	operation = OAM_CTC_SERVICE_DBA_ACTIVE;

	if(operation == OAM_CTC_SERVICE_DBA_DEACTIVE)
	{
		pVarContainer->varWidth = 1;
		pVarContainer->pVarData[0] = operation;
		p_byte += pVarContainer->varWidth;		
	}
	else if(operation == OAM_CTC_SERVICE_DBA_ACTIVE)
	{
		queue_num = 8;
		pVarContainer->varWidth = 8 + 10 * queue_num;
		pVarContainer->pVarData[0] = operation;		
		p_byte += 1;

		p_sla_body_hdr = (oam_service_sla_body_hdr_t *)p_byte;
		p_sla_body_hdr->schedule_scheme = 0;
		p_sla_body_hdr->high_pri_boundry = 7;
		p_sla_body_hdr->cycle_len = 0;
		p_sla_body_hdr->service_num = 8;
		p_byte += sizeof(oam_service_sla_body_hdr_t);

		for(i = 0; i < queue_num; i++)
		{
			p_sla_body_entry = (oam_service_sla_body_entry_t *)p_byte;
			p_sla_body_entry->queue_id = i;
			p_sla_body_entry->fix_size = 0;
			p_sla_body_entry->fix_bw = 0;
			p_sla_body_entry->guaranteed_bw = 0;
			p_sla_body_entry->best_effor_bw = 0;
			p_sla_body_entry->wrr_weight = 0;
			p_byte += sizeof(oam_service_sla_body_entry_t);
		}		
	}

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0006 - Service SLA */
int ctc_oam_varCb_serviceSla_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)	/* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	oam_service_sla_t p_sla;
	oam_service_sla_body_hdr_t   *p_body_hdr;
	oam_service_sla_body_entry_t *p_body_entry;
	u8_t schedule_scheme;
	u8_t high_pri_boundry;
	u32_t cycle_len;
	u8_t queue_num;
	u8_t i;
	u32_t ret;

	/*get the sla config*/
	p_sla.branch 	= pVarContainer->varDesc.varBranch;
	p_sla.leaf 	= pVarContainer->varDesc.varLeaf;
	p_sla.width 	= pVarContainer->varWidth;
	p_sla.operation = pVarContainer->pVarData[0];

	if(p_sla.operation == OAM_CTC_SERVICE_DBA_DEACTIVE)
	{
		/*
		 * set service sla parameters
		 */

		ret = 0;
	}
	else if(p_sla.operation == OAM_CTC_SERVICE_DBA_ACTIVE)
	{
		p_body_hdr = (oam_service_sla_body_hdr_t *)(&pVarContainer->pVarData[1]);

		schedule_scheme = p_body_hdr->schedule_scheme;
		high_pri_boundry = p_body_hdr->high_pri_boundry;
		cycle_len = p_body_hdr->cycle_len;
		queue_num = p_body_hdr->service_num;
		/*
		 * set service sla parameters
		 */

		p_body_entry = (oam_service_sla_body_entry_t *)(&pVarContainer->pVarData[1 + sizeof(oam_service_sla_body_hdr_t)]);

		for(i = 0; i < queue_num; i++)
		{
			/*
			 * set queue parameters
			 */

			ret = 0;
			p_body_entry++;
		}
	}
	pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0007 - ONU capabilities-2 */
int ctc_oam_varCb_onuCapabilities2_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    /* For sample system, pretend it is a 4 ports GE SFU w/o multiple LLID */
    pVarContainer->varWidth = 10 + 6;
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* ONU Type - SFU */
    pVarContainer->pVarData[0] = 0x00;
    pVarContainer->pVarData[1] = 0x00;
    pVarContainer->pVarData[2] = 0x00;
    pVarContainer->pVarData[3] = 0x00;

    /* MultiLLID - single LLID */
    pVarContainer->pVarData[4] = 0x01;

    /* ProtectionType - Unsupport */
    pVarContainer->pVarData[5] = 0x00;

    /* Num of PON IF - 1 PON IF */
    pVarContainer->pVarData[6] = 0x01;

    /* Num of Slot - SFU */
    pVarContainer->pVarData[7] = 0x00;

    /* Num of Interface type - 1 IF Type */
    pVarContainer->pVarData[8] = 0x01;

    /* Interface Type - GE */
    pVarContainer->pVarData[9] = 0x00;
    pVarContainer->pVarData[10] = 0x00;
    pVarContainer->pVarData[11] = 0x00;
    pVarContainer->pVarData[12] = 0x00;

    /* Num of Port - 4 ports */
#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
		pVarContainer->pVarData[13] = 0x00;
		pVarContainer->pVarData[14] = 0x04;
#elif defined(CONFIG_PRODUCT_5500)
		pVarContainer->pVarData[13] = 0x00;
		pVarContainer->pVarData[14] = 0x02;
#elif defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG)
		pVarContainer->pVarData[13] = 0x00;
		pVarContainer->pVarData[14] = 0x01;
#endif


    /* Battery Backup - No battery backup */
    pVarContainer->pVarData[15] = 0x00;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0008 - HoldoverConfig */
int ctc_oam_varCb_holdoverConfig_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    int ret = RT_ERR_OK;
    unsigned int getState, getTime;

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_HOLDOVERCONFIG_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* MODIFY ME - get from OAM module */

    /* ret = rtk_epon_dsFecState_get(&dsState); */
    if(RT_ERR_OK != ret)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    /* Construct return value */
    CTC_BUF_ENCODE32(pVarContainer->pVarData, &getState);
    CTC_BUF_ENCODE32(pVarContainer->pVarData + 4, &getTime);

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0008 - HoldoverConfig */
int ctc_oam_varCb_holdoverConfig_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    int ret;
    unsigned int setState, setTime;

    if(CTC_OAM_VAR_HOLDOVERCONFIG_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }
    CTC_BUF_PARSE32(pVarContainer->pVarData, &setState);
    CTC_BUF_PARSE32(pVarContainer->pVarData + 4, &setTime);

    /* MODIFY ME - set to OAM module */

    pVarContainer->varWidth = 0x80;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0009 - MxUMngGlobalParameter */
int ctc_oam_varCb_mxUMngGlobalParameter_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
#if 0
	u8_t *p_out;
	oam_mxu_mng_global_para_t *p_mng_global_para;
	oam_set_response_t *p_rsp;
	ODM_SYS_IF_CONFIG_t stIpConfig;
	u32_t ret = NO_ERROR;
	p_mng_global_para  = (oam_mxu_mng_global_para_t *)pVarContainer->pVarData;
	vosMemSet(&stIpConfig, 0, sizeof(ODM_SYS_IF_CONFIG_t));
	u32_t *ip_val = &(p_mng_global_para->mng_ip_addr);
	
#if 1
	if(*ip_val != 0)
	{
		ret = eopl_ip_processing(p_mng_global_para->mng_ip_addr,
							p_mng_global_para->mng_ip_mask,
							p_mng_global_para->mng_gw);
		if(NO_ERROR != ret)
		{
			goto send_rsp;
		}
	
	}
	if(0 != p_mng_global_para->mng_data_cvlan)
	{
		ret = eopl_mgmtvlan_processing(p_mng_global_para->mng_data_cvlan);
		if(NO_ERROR != ret)
		{
			goto send_rsp;
		}
	}
send_rsp:
	if(NO_ERROR != ret)
	{
		//p_rsp->indication = OAM_SET_NO_RESOURCE;
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
	else
	{
		//p_rsp->indication = OAM_SET_OK;
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
#endif
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
#endif
}

/* 0xC7/0x0009 - MxUMngGlobalParameter */
int ctc_oam_varCb_mxUMngGlobalParameter_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    /* Pretend set operation complete */
    u8_t *p_out;
	oam_mxu_mng_global_para_t *p_mng_global_para;
	u32_t ret = NO_ERROR;
	p_mng_global_para  = (oam_mxu_mng_global_para_t *)pVarContainer->pVarData;
	u32_t *ip_val = &(p_mng_global_para->mng_ip_addr);
	
#if 1
	if(*ip_val != 0)
	{
	    //luoruncai@hexicomtech.com delete 2016-6.20
		//ret = eopl_ip_processing(p_mng_global_para->mng_ip_addr,
		//					p_mng_global_para->mng_ip_mask,
		//					p_mng_global_para->mng_gw);
		ret = NO_ERROR;
		if(NO_ERROR != ret)
		{
			goto send_rsp;
		}
	
	}
	if(0 != p_mng_global_para->mng_data_cvlan)
	{
	    //luoruncai@hexicomtech.com delete 2016-6.20
		//ret = eopl_mgmtvlan_processing(p_mng_global_para->mng_data_cvlan);
        ret = NO_ERROR;
		if(NO_ERROR != ret)
		{
			goto send_rsp;
		}
	}
send_rsp:
	if(NO_ERROR != ret)
	{
		//p_rsp->indication = OAM_SET_NO_RESOURCE;
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
	else
	{
		//p_rsp->indication = OAM_SET_OK;
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
#endif

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000A - MxUMngSNMPParameter */
int ctc_oam_varCb_mxUMngSNMPParameter_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
}

/* 0xC7/0x000A - MxUMngSNMPParameter */
int ctc_oam_varCb_mxUMngSNMPParameter_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    /* Pretend set operation complete */
    pVarContainer->varWidth = 0x80;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000B - Active PON_IFAdminstate */
int ctc_oam_varCb_activePonIfAdminstate_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_ACTIVEPONIFADMINSTATE_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Fix value for the PON number */
    pVarContainer->pVarData[0] = CTC_OAM_VAR_ACTIVEPONIFADMINSTATE_PONNUM;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000B - Active PON_IFAdminstate */
int ctc_oam_varCb_activePonIfAdminstate_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    /* Not support type C/D protection */
    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000C - ONU capabilities-3 */
int ctc_oam_varCb_onuCapabilities3_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    /* For sample system, pretend it doesn't support IPv6 aware and power control */
    pVarContainer->varWidth = 0x02; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* IPv6 Supported - not supported */
    pVarContainer->pVarData[0] = 0x00;

    /* ONUPowerSupplyControl - not supported */
    pVarContainer->pVarData[1] = 0x00;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000D - ONU power saving capabilities */
int ctc_oam_varCb_onuPowerSavingCapabilities_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_ONUPOWERSAVINGCAPABILITES_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Sleep mode capability - not supported */
    pVarContainer->pVarData[0] = 0x00;

    /* Early wake-up capability - not supported */
    pVarContainer->pVarData[0] = 0x00;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000E - ONU power saving config */
int ctc_oam_varCb_onuPowerSavingConfig_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_ONUPOWERSAVINGCONFIG_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Early wake-up - unsupport */
    pVarContainer->pVarData[0] = CTC_OAM_VAR_ONUPOWERSAVINGCONFIG_UNSUPPORT;

    /* Sleep duration max - not support so fill in all zeros */
    memset((pVarContainer->pVarData+1), 0, CTC_OAM_VAR_ONUPOWERSAVINGCONFIG_LEN - 1);

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000E - ONU power saving config */
int ctc_oam_varCb_onuPowerSavingConfig_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    /* Not support power saving */
    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000F - ONU Protection Parameters */
int ctc_oam_varCb_onuProtectionParameters_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_ONUPROTECTIONPARAMETERS_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Due to the actual LOS time setting is on the tranceiver side
     * And the tranciver control should be different from vender to vender
     * So here always return the default value.
     */

    /* T LOS_Optical - 2 ms */
    CTC_BUF_ENCODE16(pVarContainer->pVarData, CTC_OAM_VAR_ONUPROTECTIONPARAMETERS_LOSOPT);

    /* MODIFY ME - add MPCP timeout settings */
    /* T LOS_MAC - 50 ms */
    CTC_BUF_ENCODE16(pVarContainer->pVarData + 2, CTC_OAM_VAR_ONUPROTECTIONPARAMETERS_LOSMAC);

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x000F - ONU Protection Parameters */
int ctc_oam_varCb_onuProtectionParameters_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    /* Due to the actual LOS time setting is on the tranceiver side
     * And the tranciver control should be different from vender to vender
     * So here always return the default value.
     */

    /* Pretend set operation complete */
    pVarContainer->varWidth = 0x80;

    return EPON_OAM_ERR_OK;
}
/* 0xC7/0x0011 - EthLinkState */
int ctc_oam_varCb_ethLinkState_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_eth_linkstate_t	*p_link;	
    u32_t state=0;
    BOOL bPortStatus = TRUE;	
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 linkstate
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = CTC_OAM_VAR_ETHLINKSTATE_LEN;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;		
		
	if ( NO_ERROR == DRV_GetPortLinkStatus((UINT32)ucPortS, &bPortStatus))
	{
		if ( TRUE == bPortStatus )
		{
			state = CTC_OAM_VAR_ETHLINKSTATE_UP;
		}
		else
		{
			state = CTC_OAM_VAR_ETHLINKSTATE_DOWN;
		}
	}			
		
	pVarContainer->pVarData[0] = (u8_t)state;
		
	p_byte += pVarContainer->varWidth;/*point TLV 2*/								
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			linkstate
			-----------
		*/
		p_link = (oam_eth_linkstate_t*)p_byte;
		p_link->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_link->leaf = OAM_ATTR_ETH_LINKSTATE;
		p_link->width = CTC_OAM_VAR_ETHLINKSTATE_LEN;

        if ( NO_ERROR == DRV_GetPortLinkStatus((UINT32)ucPortS, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                state = CTC_OAM_VAR_ETHLINKSTATE_UP;
            }
            else
            {
                state = CTC_OAM_VAR_ETHLINKSTATE_DOWN;
            }
        }
        else
        {
            continue;
        }
		
		p_link->linkstate = (u8_t)state;
		p_byte += sizeof(oam_eth_linkstate_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0012 - EthPortPause */
int ctc_oam_varCb_ethPortPause_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_eth_portpause_t *p_pause;
    u32_t state=0;
    BOOL bPortStatus = TRUE;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 back_oper
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = CTC_OAM_VAR_ETHPORTPAUSE_LEN;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;		
		
	if ( NO_ERROR == DRV_GetPortFlowCtrl((UINT32)ucPortS, &bPortStatus))
	{
		if ( TRUE == bPortStatus )
		{
			state = CTC_OAM_VAR_ETHPORTPAUSE_ENABLE;
		}
		else
		{
			state = CTC_OAM_VAR_ETHPORTPAUSE_DISABLE;
		}
	}			
		
	pVarContainer->pVarData[0] = (u8_t)state;
		
	p_byte += pVarContainer->varWidth;/*point TLV 2*/								
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{		
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			back_oper
			-----------
		*/
		p_pause = (oam_eth_portpause_t*)p_byte;
		p_pause->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_pause->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_pause->width = CTC_OAM_VAR_ETHPORTPAUSE_LEN;

        if ( NO_ERROR == DRV_GetPortFlowCtrl((UINT32)ucPortS, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                state = CTC_OAM_VAR_ETHPORTPAUSE_ENABLE;
            }
            else
            {
                state = CTC_OAM_VAR_ETHPORTPAUSE_DISABLE;
            }
        }
        else
        {
            continue;
        }
		
		p_pause->back_oper = (u8_t)state;
		p_byte += sizeof(oam_eth_portpause_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0012 - ethPort Pause */
int ctc_oam_varCb_ethPortPause_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=OPL_OK, ucPortS, ucPortE;
	oam_eth_portpause_t p_pause;
	u32_t pause_state;
	int ret_value =0;

	if(CTC_OAM_VAR_ETHPORTPAUSE_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the pause config*/
	p_pause.branch 	= pVarContainer->varDesc.varBranch;
	p_pause.leaf 	= pVarContainer->varDesc.varLeaf;
	p_pause.width 	= pVarContainer->varWidth;
	p_pause.back_oper = pVarContainer->pVarData[0];
	pause_state = (u32_t)p_pause.back_oper;

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's PortPause set enable: %d\n", ucPortS, pause_state);
		rlt = DRV_SetPortFlowCtrl(ucPortS, (TRUE == pause_state)? TRUE : FALSE);
	}

	if(rlt == OPL_OK)
	{
		/*	回复报文
			-----------
			1 Branch
			-----------
			1 Leaf
			-----------以上信息已经在外层添加，我们只需要从此处添加相关信息
			1 Indication
			-----------
		*/
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
		/*
		注意:对于回复报文我们需要打上标签,
		但是set操作会在外层自动打上
		*/
	}
	else
	{
		CTC_DEBUG("ethPortPause_set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0013 - ethPortUs Policing */
int ctc_oam_varCb_ethPortUsPolicing_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t enable=1, rlt=0;
	u8_t ucPortS, ucPortE, *p_byte, *p_temp;
	u32_t	cir=0xFF,cbs=0xFF,ebs=0xFF;
	unsigned long ratevalue = 0;
	oam_eth_portpolicing_t	*p_policing;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 operation
											------------
											3 cir
											------------
											3 cbs
											------------
											3 ebs
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
		
	_CTC_GetUniLineRateInCfg(ucPortS, &ratevalue);
	cir = ratevalue;
	
	pVarContainer->pVarData[0] = enable;
	if(enable == 0)
	{
		pVarContainer->varWidth = 1;
	}
	else
	{
		pVarContainer->varWidth = 10;
		p_temp = (u8_t*)&cir;
		memcpy(&pVarContainer->pVarData[1], &p_temp[1], 3);
		p_temp = (u8_t*)&cbs;
		memcpy(&pVarContainer->pVarData[4], &p_temp[1], 3);
		p_temp = (u8_t*)&ebs;
		memcpy(&pVarContainer->pVarData[7], &p_temp[1], 3);
	}
	
	p_byte += pVarContainer->varWidth;/*point TLV 2*/		
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			1 operation
			------------
			3 cir
			------------
			3 cbs
			------------
			3 ebs
			------------
		*/
		p_policing = (oam_eth_portpolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_US_POLICING;
		
		_CTC_GetUniLineRateInCfg(ucPortS, &ratevalue);
		cir = ratevalue;
		
		p_policing->operation = enable;
		if(enable == 0)
		{
			p_policing->width = 1;
		}
		else
		{
			p_policing->width = 10;
			p_temp = (u8_t*)&cir;
			memcpy(p_policing->cir, &p_temp[1], 3);
			p_temp = (u8_t*)&cbs;
			memcpy(p_policing->cbs, &p_temp[1], 3);
			p_temp = (u8_t*)&ebs;
			memcpy(p_policing->ebs, &p_temp[1], 3);
		}
		
		p_byte += sizeof(oam_eth_portpolicing_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0013 - ethPortUs Policing */
int ctc_oam_varCb_ethPortUsPolicing_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE;
	u32_t	cir=0, cbs=0, ebs=0;
	oam_eth_portpolicing_t p_policing;
	int ret_value =0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the pause config*/
	p_policing.branch 	= pVarContainer->varDesc.varBranch;
	p_policing.leaf 	= pVarContainer->varDesc.varLeaf;
	p_policing.width 	= pVarContainer->varWidth;
	p_policing.operation = pVarContainer->pVarData[0];
	memcpy(p_policing.cir, &pVarContainer->pVarData[1], 3);
	memcpy(p_policing.cbs, &pVarContainer->pVarData[4], 3);
	memcpy(p_policing.ebs, &pVarContainer->pVarData[7], 3);

	if(p_policing.operation == 1)
	{
 		memcpy((u8_t*)&cir+1, p_policing.cir, 3);
		memcpy((u8_t*)&cbs+1, p_policing.cbs, 3);
		memcpy((u8_t*)&ebs+1, p_policing.ebs, 3);
	}
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's PortUsPolicing\n", ucPortS);
        rlt = DRV_SetPortLineRate(ucPortS, DIRECT_IN, cir);
	}

	if(rlt == OPL_OK)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("ethPortUsPolicing_set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0016 - ethPortDs Policing */
int ctc_oam_varCb_ethPortDsPolicing_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t enable=1,rlt=0;
	u8_t ucPortS, ucPortE, *p_byte, *p_temp;
	u32_t	cir=0xFF,pir=0xFF;
	unsigned long ratevalue = 0;
	oam_eth_portdspolicing_t	*p_policing;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 operation
											------------
											3 cir
											------------
											3 pir
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;

	_CTC_GetUniLineRateOutCfg(ucPortS, &ratevalue);
	cir = ratevalue;
	
	pVarContainer->pVarData[0] = enable;
	if(enable == 0)
	{
		pVarContainer->varWidth = 1;
	}
	else
	{
		pVarContainer->varWidth = 7;
		p_temp = (u8_t*)&cir;
		memcpy(&pVarContainer->pVarData[1], &p_temp[1], 3);
		p_temp = (u8_t*)&pir;
		memcpy(&pVarContainer->pVarData[4], &p_temp[1], 3);
	}

	p_byte += pVarContainer->varWidth;/*point TLV 2*/		
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			1 operation
			------------
			3 cir
			------------
			3 pir
			------------
		*/
		p_policing = (oam_eth_portdspolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;

		_CTC_GetUniLineRateOutCfg(ucPortS, &ratevalue);
		cir = ratevalue;
		
		p_policing->operation = enable;
		if(enable == 0)
		{
			p_policing->width = 1;
		}
		else
		{
			p_policing->width = 7;
			p_temp = (u8_t*)&cir;
			memcpy(p_policing->cir, &p_temp[1], 3);
			p_temp = (u8_t*)&pir;
			memcpy(p_policing->pir, &p_temp[1], 3);
		}
		
		p_byte += sizeof(oam_eth_portpolicing_t);
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0016 - ethPortDs Policing */
int ctc_oam_varCb_ethPortDsPolicing_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE;
	u32_t	cir=0, pir=0;
	oam_eth_portdspolicing_t p_dspolicing;
	int ret_value =0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the pause config*/
	p_dspolicing.branch 	= pVarContainer->varDesc.varBranch;
	p_dspolicing.leaf 	= pVarContainer->varDesc.varLeaf;
	p_dspolicing.width 	= pVarContainer->varWidth;
	p_dspolicing.operation = pVarContainer->pVarData[0];
	memcpy(p_dspolicing.cir, &pVarContainer->pVarData[1], 3);
	memcpy(p_dspolicing.pir, &pVarContainer->pVarData[4], 3);

	if(p_dspolicing.operation == 1)
	{
 		memcpy((u8_t*)&cir+1, p_dspolicing.cir, 3);
		memcpy((u8_t*)&pir+1, p_dspolicing.pir, 3);
	}
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("Port%d's PortDsPolicing\n", ucPortS);
        rlt = DRV_SetPortLineRate(ucPortS, DIRECT_OUT, cir);
	}

	if(rlt == OPL_OK)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("ethPortDsPolicing_set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}

extern int oam_ipc_get_sys_loop(sys_loop_t *info);
extern int oam_ipc_set_sys_loop(sys_loop_t *info);
/* 0xC7/0x0017 - PortLoopDetect */
int ctc_oam_varCb_portLoopDetect_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    int ret;
    unsigned int port;
    unsigned int setValue;
    rtk_enable_t enable;
	sys_loop_t loop_info;
    rtk_rldp_config_t config;
    rtk_rldp_portConfig_t portConfig;

    if(CTC_OAM_VAR_PORTLOOPDETECT_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

    if(CTC_INSTANT_IS_ETHERNET(pVarInstant))
    {
        /* Make sure the global RLDP is enabled */
		#if 0
        ret = rtk_rldp_config_get(&config);
        if(RT_ERR_OK != ret)
        {
            pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
            return EPON_OAM_ERR_OK;
        }
        config.rldp_enable = ENABLED;
        ret = rtk_rldp_config_set(&config);
        if(RT_ERR_OK != ret)
        {
            pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
            return EPON_OAM_ERR_OK;
        }
		#endif
		memset(&loop_info, 0, sizeof(sys_loop_t));
		ret = oam_ipc_get_sys_loop(&loop_info);
		
	    if(0 != ret)
	    {
			CTC_DEBUG("OAM ipc get failed\n");
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	        return EPON_OAM_ERR_OK;        
	    } 
        CTC_BUF_PARSE32(pVarContainer->pVarData, &setValue);
        if(CTC_OAM_VAR_PORTLOOPDETECT_DEACTIVE == setValue)
        {
            enable = DISABLED;
        }
        else if(CTC_OAM_VAR_PORTLOOPDETECT_ACTIVE == setValue)
        {
            enable = ENABLED;
        }
        else
        {
            pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARBADPARAM;
            return EPON_OAM_ERR_OK;
        }

		if(enable == loop_info.loopback_enable)
		{
			pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
    		return EPON_OAM_ERR_OK;
		}

		loop_info.loopback_enable = enable;
		ret = oam_ipc_set_sys_loop(&loop_info);
	    if(0 != ret)    
	    {
	        pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARBADPARAM;
            return EPON_OAM_ERR_OK;       
	    }
		#if 0
        if(CTC_INSTANT_IS_ALLPORTS(pVarInstant))
        {
            HAL_SCAN_ALL_PORT(port)
            {
                ret = rtk_rldp_portConfig_get(port, &portConfig);
                if(RT_ERR_OK != ret)
                {
                    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                    return EPON_OAM_ERR_OK;
                }
                portConfig.tx_enable = enable;
                ret = rtk_rldp_portConfig_set(port, &portConfig);
                if(RT_ERR_OK != ret)
                {
                    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                    return EPON_OAM_ERR_OK;
                }
            }
        }
        else
        {
            ret = rtk_rldp_portConfig_get(pVarInstant->parse.uniPort.portNo - 1, &portConfig);
            if(RT_ERR_OK != ret)
            {
                pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                return EPON_OAM_ERR_OK;
            }
            portConfig.tx_enable = enable;
            ret = rtk_rldp_portConfig_set(port, &portConfig);
            if(RT_ERR_OK != ret)
            {
                pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                return EPON_OAM_ERR_OK;
            }
        }
		#endif
    }
    pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0019 - PortMACAgingTime */
int ctc_oam_varCb_portMacAgingTime_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    int ret;
    unsigned int getValue;
    rtk_enable_t enable;

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_PORTMACAGINGTIME_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    if(!CTC_INSTANT_IS_ALLPORTS(pVarInstant) && CTC_INSTANT_IS_ETHERNET(pVarInstant))
    {
        /* TODO - Apollo does not support per-port aging time
         * It only support per-system aging time
         */
        ret = rtk_l2_portAgingEnable_get(pVarInstant->parse.uniPort.portNo - 1, &enable);
        if(RT_ERR_OK == ret)
        {
            if(ENABLED == enable)
            {
                ret = rtk_l2_aging_get(&getValue);
                if(RT_ERR_OK != ret)
                {
                    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                    return EPON_OAM_ERR_OK;
                }
            }
            else
            {
                getValue = CTC_OAM_VAR_PORTMACAGINGTIME_DEACTIVE;
            }
            CTC_BUF_ENCODE32(pVarContainer->pVarData, &getValue);
        }
        else
        {
            return EPON_OAM_ERR_UNKNOWN;
        }
    }

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0019 - PortMACAgingTime */
int ctc_oam_varCb_portMacAgingTime_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    int ret;
    unsigned int port;
    unsigned int setValue;
    rtk_enable_t enable;

    if(CTC_OAM_VAR_PORTMACAGINGTIME_LEN != pVarContainer->varWidth)
    {
        return EPON_OAM_ERR_UNKNOWN;
    }

    if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

    /* TODO - Apollo does not support per-port aging time
     * It only support per-system aging time
     */
    CTC_BUF_PARSE32(pVarContainer->pVarData, &setValue);
    if(CTC_OAM_VAR_PORTMACAGINGTIME_DEACTIVE == setValue)
    {
        setValue = 0;
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    ret = rtk_l2_aging_set(setValue);
    if(RT_ERR_OK != ret)
    {
        pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
        return EPON_OAM_ERR_OK;
    }

    /* Turn on/off each ports' aging out mechanism */
    if(CTC_INSTANT_IS_ETHERNET(pVarInstant))
    {
        if(CTC_INSTANT_IS_ALLPORTS(pVarInstant))
        {
            HAL_SCAN_ALL_PORT(port)
            {
                ret = rtk_l2_portAgingEnable_set(port, &enable);
                if(RT_ERR_OK != ret)
                {
                    pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                    return EPON_OAM_ERR_OK;
                }
            }
        }
        else
        {
            ret = rtk_l2_portAgingEnable_set(pVarInstant->parse.uniPort.portNo - 1, &enable);
            if(RT_ERR_OK != ret)
            {
                pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
                return EPON_OAM_ERR_OK;
            }
        }
    }
    pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0021 - VLAN */
int ctc_oam_varCb_vlan_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_vlan_config_t	*p_vlan_config;		
	u16_t length = 0;
	u8_t vlanmode;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 mode
											------------
											X vlan config
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
		
	(void)DRV_GetPortVlanCfg(ucPortS, &vlanmode, &pVarContainer->pVarData[1]/*vlan config*/, &length);				
	pVarContainer->varWidth = length + 1;
	switch(vlanmode)/*mode*/
	{
		case CTC_VLAN_MODE_TRANSPARENT:
			pVarContainer->pVarData[0] = CTC_OAM_VAR_VLAN_MODE_TRANSPARENT;
		break;
		case CTC_VLAN_MODE_TAG:
			pVarContainer->pVarData[0] = CTC_OAM_VAR_VLAN_MODE_TAG;
		break;
		case CTC_VLAN_MODE_TRANSLATION:
			pVarContainer->pVarData[0] = CTC_OAM_VAR_VLAN_MODE_TRANSLATION;
		break;
		default:
			pVarContainer->pVarData[0] = 0xff;
		break;
	}
		
	p_byte += pVarContainer->varWidth;/*point TLV 2*/				
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}
	
	for(; ucPortS<=ucPortE; ucPortS++)
	{		
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			mode
			-----------
		*/
		p_vlan_config = (oam_vlan_config_t*)p_byte;
		p_vlan_config->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_vlan_config->leaf = OAM_ATTR_VLAN;

		p_byte += sizeof(struct  oam_vlan_config);/*point vlan config*/
		(void)DRV_GetPortVlanCfg(ucPortS, &vlanmode, p_byte, &length);
		p_vlan_config->width = length + 1;
		switch(vlanmode)
		{
			case CTC_VLAN_MODE_TRANSPARENT:
				p_vlan_config->mode = CTC_OAM_VAR_VLAN_MODE_TRANSPARENT;
			break;
			case CTC_VLAN_MODE_TAG:
				p_vlan_config->mode = CTC_OAM_VAR_VLAN_MODE_TAG;
			break;
			case CTC_VLAN_MODE_TRANSLATION:
				p_vlan_config->mode = CTC_OAM_VAR_VLAN_MODE_TRANSLATION;
			break;
			default:
				p_vlan_config->mode = 0xff;
			break;
		}
		p_byte += length;
	}

	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}
    
/* 0xC7/0x0021 - VLAN */
int ctc_oam_varCb_vlan_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	i, j, ucPortS, ucPortE, *p_byte;
	u32_t   rlt = 0;
	u32_t	defaultTag, oldTag, newTag;
	oam_vlan_config_t	p_vlan_t;
	oam_vlan_config_t	*p_vlan = &p_vlan_t;
	u32_t oldMode;
	u32_t vlanId;
	u32_t vlanlist[32];
	u32_t vlanIndex;	
	u8_t * p_vlan_check;
	u8_t*  p_in;
	int ret_value =0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the vlan config*/
	p_vlan->branch 	= pVarContainer->varDesc.varBranch;
	p_vlan->leaf 	= pVarContainer->varDesc.varLeaf;
	p_vlan->width 	= pVarContainer->varWidth;
	p_vlan->mode 	= pVarContainer->pVarData[0];
	p_in			= &pVarContainer->pVarData[1];
	
	CTC_DEBUG("Port%d's vlan_set mode:%d\n", ucPortS, p_vlan->mode);
		
	if(CTC_OAM_VAR_VLAN_MODE_TRANSPARENT == p_vlan->mode)
	{   
		int iRet = NO_ERROR;

		if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH20)
		{
			if(0xFF == pVarInstant->varData[0])
			{
				CTC_DEBUG("\nDo not set transparent vlan for all ports.\n");
				rlt = OPL_OK;
				goto send_rsp;
			}
		}
		else if(pVarInstant->varWidth == CTC_ORGSPEC_VARINSTANT_WIDTH21)
		{
			if(0xFFFF == pVarInstant->parse.uniPort.portNo)
			{
				CTC_DEBUG("\nDo not set transparent vlan for all ports.\n");
				rlt = OPL_OK;
				goto send_rsp;
			}
		}
	        
		iRet = CTC_DRV_SetTransparentVlan(ucPortS);
		if (NO_ERROR != iRet)
		{
			CTC_DEBUG("\nFailed to set transparent vlan for port %d.\n",ucPortS);
			rlt = OPL_ERROR;
			goto send_rsp;
		}
	}
	else if(CTC_OAM_VAR_VLAN_MODE_TAG == p_vlan->mode)
	{
		memcpy(&defaultTag, p_in, 4);

		int iRet = NO_ERROR;
		u32_t priority;
		u32_t default_vlan;

		rlt = OPL_OK;

		iRet = CTC_DRV_GetVlanIdAndPri(defaultTag, &default_vlan, &priority);
		if (NO_ERROR != iRet)
		{
			CTC_DEBUG("\nFailed to set tag vlan for port %d.\n",ucPortS);
			rlt = OPL_ERROR;
			goto send_rsp;
		}

		iRet = CTC_DRV_SetTagVlan(ucPortS, default_vlan, priority);
		if (NO_ERROR != iRet)
		{
			CTC_DEBUG("\nFailed to set tag vlan for port %d.\n",ucPortS);
			rlt = OPL_ERROR;
			goto send_rsp;
		}
	}
	else if(CTC_OAM_VAR_VLAN_MODE_TRANSLATION == p_vlan->mode)
	{
		int iRet = NO_ERROR;
		u32_t uiVlanPair;
		u32_t priority;
			
		rlt = OPL_OK;

		memcpy(&defaultTag, p_in, 4);
		p_in += 4;

		memset(vlanlist, 0, sizeof(vlanlist));
		vlanIndex = 0;		
		p_vlan_check = p_in;
		uiVlanPair = (p_vlan->width - 5)/8;
	          
		for(i = 0; i < uiVlanPair; i++)
		{
			memcpy(&oldTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;
			memcpy(&newTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;

			oldTag = oldTag&0xFFF;
			newTag = newTag&0xFFF;

			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			CTC_DEBUG("%s %d %d %d %d\n", __FUNCTION__, __LINE__, vlanIndex, oldTag, newTag);

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
				CTC_DEBUG("\n Onu only supports 8 vlan translation pairs for every port. \n");
				rlt = CTC_OAM_VAR_RESP_VARBADPARAM;
				goto send_rsp;
			}
		}

		iRet = CTC_DRV_SetTranslationVlan(ucPortS, defaultTag, vlanlist, (vlanIndex/2), CTC_VLAN_MODE_TRANSLATION);

		if (NO_ERROR != iRet)
		{
			CTC_DEBUG("\nFailed to set translation vlan for port %d.\n",ucPortS);
			rlt = OPL_ERROR;
			goto send_rsp;
		}
	}
	else if(CTC_OAM_VAR_VLAN_MODE_AGGREGATION == p_vlan->mode)
	{
		/*CTC 2.1 新增,但是现有OLT尚无命令配置*/
		rlt = CTC_OAM_VAR_RESP_VARNORESOURCE;
		goto send_rsp;
	}
	else if(CTC_OAM_VAR_VLAN_MODE_TRUNK == p_vlan->mode)
	{
		/*CTC 2.1 新增,但是现有OLT尚无命令配置*/
		//rlt = CTC_OAM_VAR_RESP_VARNORESOURCE;
		//goto send_rsp;
		int iRet = NO_ERROR;
		u32_t uiVlanPair;
		u32_t priority;
			
		rlt = OPL_OK;

		memcpy(&defaultTag, p_in, 4);
		p_in += 4;

		memset(vlanlist, 0, sizeof(vlanlist));
		vlanIndex = 0;		
		p_vlan_check = p_in;
		uiVlanPair = (p_vlan->width - 5)/4;
	          
		for(i = 0; i < uiVlanPair; i++)
		{
			memcpy(&oldTag, (u8_t*)p_vlan_check, 4);
			memcpy(&newTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;
			//memcpy(&newTag, (u8_t*)p_vlan_check, 4);
			//p_vlan_check += 4;

			oldTag = oldTag&0xFFF;
			newTag = newTag&0xFFF;

			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			CTC_DEBUG("%s %d %d %d %d\n", __FUNCTION__, __LINE__, vlanIndex, oldTag, newTag);

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
				CTC_DEBUG("\n Onu only supports 8 vlan translation pairs for every port. \n");
				rlt = CTC_OAM_VAR_RESP_VARBADPARAM;
				goto send_rsp;
			}
		}

		iRet = CTC_DRV_SetTranslationVlan(ucPortS, defaultTag, vlanlist, (vlanIndex/2), CTC_VLAN_MODE_TRUNK);

		if (NO_ERROR != iRet)
		{
			CTC_DEBUG("\nFailed to set translation vlan for port %d.\n",ucPortS);
			rlt = OPL_ERROR;
			goto send_rsp;
		}
	}
	else {
		rlt = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

send_rsp:
	if(rlt == OPL_OK)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("vlan set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0031 - Classification&Marking */
int ctc_oam_varCb_calssificationMarking_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	oam_clasmark_t				*pClsHeader;
	oam_clasmark_rulebody_t		*pClsRule;
	oam_clasmark_fieldbody_t	*pClsField;
	oam_clasmark_rulebody_t  clsRule;
	oam_clasmark_fieldbody_t clsFields[12];
	u8_t precedenceOfRule;
	u8_t numOfRule = 0;
	u8_t numOfField;
	u8_t curPortId;
	u32_t ret;
	u8_t ucPortS, ucPortE, *p_byte;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 action
											------------
											1 numOfRule
											------------
											x rulebody
											------------
											x fieldbody
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = 2;/*action + numOfRule*/
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
	p_byte += 2;

	for(precedenceOfRule = 1; precedenceOfRule <= 8; precedenceOfRule++)
	{
		memset(&clsRule, 0, sizeof(oam_clasmark_rulebody_t));
		memset(clsFields, 0, sizeof(oam_clasmark_fieldbody_t)*12);
		
		Ctc_GetClfRmk2Local((u8_t)curPortId, precedenceOfRule - 1, &clsRule, &clsFields[0]);
		
		numOfRule++;

		/* add cls rule entry */
		pClsRule = (oam_clasmark_rulebody_t *)p_byte;
		memcpy(pClsRule, &clsRule, sizeof(oam_clasmark_rulebody_t));
		p_byte += sizeof(oam_clasmark_rulebody_t);

		/* add fileds entry */
		pClsField = (oam_clasmark_fieldbody_t *)p_byte;
		numOfField = pClsRule->numOfField;
		memcpy(pClsField, & clsFields, numOfField * sizeof(oam_clasmark_fieldbody_t));
		p_byte += numOfField * sizeof(oam_clasmark_fieldbody_t);

		pVarContainer->varWidth += pClsRule->lenOfRule + 2;
	}

	pVarContainer->pVarData[0] = OAM_CLASSIFY_ACTION_SHOW;
	pVarContainer->pVarData[1] = numOfRule;

	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			action
			-----------
			numOfRule
			------------
			rulebody
			------------
			fieldbody
			------------
		*/
		pClsHeader = (oam_clasmark_t *)p_byte;
		pClsHeader->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		pClsHeader->leaf = OAM_ATTR_CLAMARK;
		pClsHeader->width = 2; /* action(1Byte) +  numOfRule(1Byte) */
		p_byte += sizeof(oam_clasmark_t);

		for(precedenceOfRule = 1; precedenceOfRule <= 8; precedenceOfRule++)
		{
			memset(&clsRule, 0, sizeof(oam_clasmark_rulebody_t));
			memset(clsFields, 0, sizeof(oam_clasmark_fieldbody_t)*12);

			Ctc_GetClfRmk2Local((u8_t)curPortId, precedenceOfRule - 1, &clsRule, &clsFields[0]);

			numOfRule++;

			/* add cls rule entry */
			pClsRule = (oam_clasmark_rulebody_t *)p_byte;
			memcpy(pClsRule, &clsRule, sizeof(oam_clasmark_rulebody_t));
			p_byte += sizeof(oam_clasmark_rulebody_t);

			/* add fileds entry */
			pClsField = (oam_clasmark_fieldbody_t *)p_byte;
			numOfField = pClsRule->numOfField;
			memcpy(pClsField, & clsFields, numOfField * sizeof(oam_clasmark_fieldbody_t));
			p_byte += numOfField * sizeof(oam_clasmark_fieldbody_t);

			pClsHeader->width += pClsRule->lenOfRule + 2;
		}

		pClsHeader->action = OAM_CLASSIFY_ACTION_SHOW;
		pClsHeader->numOfRule = numOfRule;

	}

	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0031 - Classification&Marking */
int ctc_oam_varCb_calssificationMarking_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	ucPortS, ucPortE;
	oam_clasmark_t		pClsHeader_t;
	oam_clasmark_t		*pClsHeader = &pClsHeader_t;
	oam_clasmark_rulebody_t	*pClsRule;
	oam_clasmark_fieldbody_t *pClsField;
	u8_t ret;
	u8_t precedenceOfRule;
	u8_t numOfRule;
	u8_t numOfField;
	u8_t ruleId;
	u16_t curPortId;
	u8_t*  p_in;
	int ret_value =0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the classification config*/
	pClsHeader->branch 	= pVarContainer->varDesc.varBranch;
	pClsHeader->leaf 	= pVarContainer->varDesc.varLeaf;
	pClsHeader->width 	= pVarContainer->varWidth;
	pClsHeader->action = pVarContainer->pVarData[0];
	pClsHeader->numOfRule = pVarContainer->pVarData[1];

	p_in = &pVarContainer->pVarData[2];
	
	if(OAM_CLASSIFY_ACTION_CLR == pClsHeader->action)
	{		
        ret = OAM_SET_OK;

        for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
        {
            CTC_DEBUG("Clear Port%d's Classification.\n",curPortId);
            if (NO_ERROR != DRV_CtcClearClfRmkByPort(curPortId))
            {
                CTC_DEBUG("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }

			Ctc_ClearClfRmk2LocalByPort(curPortId);
        }			
	}
	else if(OAM_CLASSIFY_ACTION_DEL == pClsHeader->action)
	{
		BOOL bBatchCfg = FALSE;
		numOfRule = pClsHeader->numOfRule;
		
		for(ruleId = 0; ruleId < numOfRule; ruleId++)
		{
			precedenceOfRule = *((u8_t *)p_in);
           
            ret = OAM_SET_OK;
            /* The precedence of rule begins with 0, when OLT sets batch configuration.
            Otherwise the precedence begins with 1. */
            if (0 == precedenceOfRule)
            { 
                bBatchCfg = TRUE;
            }

            if (TRUE != bBatchCfg)
            {
                precedenceOfRule -= 1;
            }
            
            if (NO_ERROR != DRV_CtcDelClfRmkByPrec(ucPortS, precedenceOfRule))
            {
            	CTC_DEBUG("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }
			
            Ctc_ClearClfRmk2LocalByPrec(ucPortS, precedenceOfRule);
		}
	}
	else if((OAM_CLASSIFY_ACTION_ADD == pClsHeader->action))
	{
		BOOL bBatchCfg = FALSE;
		numOfRule = pClsHeader->numOfRule;
		
		for(ruleId = 0; ruleId < numOfRule; ruleId++)
		{
			ret = OAM_SET_OK;
		
			pClsRule = (oam_clasmark_rulebody_t *)p_in;
			p_in += sizeof(oam_clasmark_rulebody_t);
			precedenceOfRule = pClsRule->precedenceOfRule;
			numOfField = pClsRule->numOfField;

			pClsField = (oam_clasmark_fieldbody_t *)p_in;
			p_in += numOfField * sizeof(oam_clasmark_fieldbody_t);

            /* 014090 */
            /* only support equal and always-match */
            if((OAM_CLF_EQUAL != pClsField->operator) && (OAM_CLF_ALWAYS_MATCH != pClsField->operator))
            {
               /*Not support*/
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }
            /* 014090 */ 
            /* The precedence of rule begins with 0, when OLT sets batch configuration.
            Otherwise the precedence begins with 1. */
            if (0 == precedenceOfRule)
            { 
                bBatchCfg = TRUE;
            }

            if (TRUE != bBatchCfg)
            {
                precedenceOfRule -= 1;
            }			
            
            CTC_DEBUG("\nctc: Add clf for port%d. prece:%d, num:%d\n",ucPortS,precedenceOfRule,numOfRule);
            if (NO_ERROR != DRV_CtcAddClfRmkByPrec(ucPortS, precedenceOfRule, pClsRule, pClsField))
            {
            	CTC_DEBUG("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }

			Ctc_AddClfRmk2Local(ucPortS, precedenceOfRule, pClsRule, pClsField);
		}		
	}
	else
	{
		CTC_DEBUG("classify unknown\n");
		ret = OAM_SET_BAD_PARAMETERS;
	}

send_rsp:
	
	pVarContainer->varWidth = ret;
	
	return EPON_OAM_ERR_OK;

}

/* 0xC7/0x0041 - Add/Del Multicast VLAN */
int ctc_oam_varCb_addDelMulticastVlan_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, ucPortS, ucPortE, number, *p_byte;
	u16_t	*p_vid;
	oam_mcast_vlan_t	*p_mcast_vlan;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 operation
											------------
											x pvid
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;
	pVarContainer->pVarData[0] = OAM_CTC_MCAST_VLAN_LIST;
	p_byte += 1;
	p_vid = (u16_t*)p_byte;
	rlt = odmMulticastVlanGet(ucPortS, p_vid, &number);
	if(0 != rlt)
	{
		CTC_DEBUG("[ %s ]: odmMulticastVlanGet return error!\n", __FUNCTION__);
		number = 0;
	}
	CTC_DEBUG("[ %s ]: mcalst vlan num = %d\n", __FUNCTION__, number);
	pVarContainer->varWidth = 1+2*number;	
	
	p_byte += 2*number;/*point TLV 2*/	
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			operation
			-----------
			pvid
			-----------
		*/
		p_mcast_vlan = (oam_mcast_vlan_t*)p_byte;
		p_mcast_vlan->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_mcast_vlan->leaf = OAM_ATTR_VLAN;
		p_mcast_vlan->operation = OAM_CTC_MCAST_VLAN_LIST;
		p_byte += sizeof(struct  oam_mcast_vlan);
		p_vid = (u16_t*)p_byte;
		rlt = odmMulticastVlanGet(ucPortS, p_vid, &number);
		if(0 != rlt)
		{
			CTC_DEBUG("[ %s ]: odmMulticastVlanGet return error!\n", __FUNCTION__);
			number = 0;
		}
		CTC_DEBUG("[ %s ]: mcalst vlan num = %d\n", __FUNCTION__, number);
		p_mcast_vlan->width = 1+2*number;
		p_byte += 2*number;		
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0041 - Add/Del Multicast VLAN */
int ctc_oam_varCb_addDelMulticastVlan_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	i, number, ucPortS, ucPortE, curPortId;
	u8_t *p_in;
	u16_t	*p_vid;
	oam_mcast_vlan_t	p_mcast_vlan_t;
	oam_mcast_vlan_t	*p_mcast_vlan = &p_mcast_vlan_t;
	u32_t   rlt = 0;
	int ret_value =0;

	/*Begin add by huangmingjian 2013-12-10 for Bug 305*/
	/*no need for these commands in MC_MODE_CTC*/
	if(mcControlBlockModeGet()==MC_MODE_CTC) 
	{
	  CTC_DEBUG("Not in MC_MODE_SNOOPING mode.\r\n");
	  pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	  return EPON_OAM_ERR_OK;
	}
	/*End add by huangmingjian 2013-12-10 for Bug 305*/

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}	

	/*get the MulticastVlan config*/
	p_mcast_vlan->branch = pVarContainer->varDesc.varBranch;
	p_mcast_vlan->leaf = pVarContainer->varDesc.varLeaf;
	p_mcast_vlan->width = pVarContainer->varWidth;
	p_mcast_vlan->operation = pVarContainer->pVarData[0];	

	if(p_mcast_vlan->width == 1)
	{
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			odmMulticastVlanClear(ucPortS);
		}
	}
	else
	{
        extern u32_t mcVlanNumGet(void);
        extern int mcVlanExist(u32_t vlanId);
        u16_t *p_vid_tmp = NULL;
        u32_t ui_mc_vlan_num = 0;

		p_in = &pVarContainer->pVarData[1];
		p_vid = (u16_t*)p_in;
		number = p_mcast_vlan->width-1;
        
        /*every uni supports max 4 mc vlans.*/
        if ((number%2 !=0) || (15 < number/2))
        {
            rlt = OPL_ERROR;
	    	goto send_rsp;
        }

        p_vid_tmp = p_vid;
        for (i = 0; i < (number/2); i++)
        {
            if (0 == mcVlanExist(*(p_vid_tmp + i)))
            {
                ui_mc_vlan_num++;
            }

            /*Mc vlan can not be same with ctc vlan.*/
            {
                extern BOOL CTC_CheckValidVidForMcvlan(u32_t uiMcVid);
                if (FALSE == CTC_CheckValidVidForMcvlan(*(p_vid_tmp + i)))
                {
                    rlt = OPL_ERROR;
	    	        goto send_rsp;
                }
            }
			
			/*Begin add by huangmingjian 2013-10-17 for Bug 203*/
            /*Mc vlan can not be same with Mc translation vlan.*/
            if (FALSE == CTC_CheckValidVidForMcTranslation(*(p_vid_tmp + i)))
            {
                rlt = OPL_ERROR;
    	        goto send_rsp;
            }
			/*End add by huangmingjian 2013-10-17 for Bug 203*/
        }

        ui_mc_vlan_num = ui_mc_vlan_num + mcVlanNumGet();
        if (15 < ui_mc_vlan_num)
        {
            rlt = OPL_ERROR;
	    	goto send_rsp;
        }

		for(i=0;i<number/2;i++,p_vid++)
		{
			CTC_DEBUG("mc vlan: %d\n", *p_vid);
	 		if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_DEL)
	 		{
				for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
				{
					odmMulticastVlanDelete(curPortId, *p_vid);
	 			}
	 		}

			if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_ADD)
			{
				for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
				{
					odmMulticastVlanAdd(curPortId, *p_vid);
				}
			}
		}
	}

send_rsp:
	if(rlt == OPL_OK)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("Multicast vlan set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
  
    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0042 - MulticastTagOper */
int ctc_oam_varCb_multicastTagOper_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_mcast_tagstripe_t	*p_tagstripe;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 operation
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = 1;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;	
	odmMulticastTagstripGet(ucPortS, &(pVarContainer->pVarData[0]));

	p_byte += pVarContainer->varWidth;/*point TLV 2*/	
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			tagstriped
			-----------
		*/
		p_tagstripe = (oam_mcast_tagstripe_t*)p_byte;
		p_tagstripe->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_tagstripe->leaf = OAM_ATTR_MCAST_TAG_STRIP;
		p_tagstripe->width = 1;
		odmMulticastTagstripGet(ucPortS, &(p_tagstripe->tagstriped));
		p_byte += p_tagstripe->width;
	}
	
	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0042 - MulticastTagOper */
int ctc_oam_varCb_multicastTagOper_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    u8_t	rlt=0, ucPortS, ucPortE;
	u16_t portId;
	oam_mcast_tagstripe_t p_mcast_tag_strip_hdr_t;
	oam_mcast_tagstripe_t *p_mcast_tag_strip_hdr = &p_mcast_tag_strip_hdr_t;
	int ret_value =0;
	int vlanAddNum,reusableVttNum,num_of_mcast_vlan=0;
	int mvlan;
	u8_t tagOper;
	int dal_vtt_num;
	extern int mcTagTranslationTableNum(void);
	dal_vtt_num=mcTagTranslationTableNum();
	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the MulticastTag config*/
	p_mcast_tag_strip_hdr->branch = pVarContainer->varDesc.varBranch;
	p_mcast_tag_strip_hdr->leaf = pVarContainer->varDesc.varLeaf;
	p_mcast_tag_strip_hdr->width = pVarContainer->varWidth;
	p_mcast_tag_strip_hdr->tagstriped = pVarContainer->pVarData[0];
	//printf(" %s %d\n",__FUNCTION__,__LINE__);
	/*Begin add by huangmingjian 2013-10-26:Bug 203,257*/
 	int i=0;
  	oam_mcast_vlan_translation_entry_t *p_translation_entry=NULL;
  	u8_t *p_in = (u8_t *)&(pVarContainer->pVarData[2]);
  	u8_t *p_in_tmp = p_in;
	/* if translation, must check first */
	if(p_mcast_tag_strip_hdr->tagstriped == OAM_CTC_MCAST_VLAN_TAG_TRANLATE)
	{
		num_of_mcast_vlan = pVarContainer->pVarData[1];
		if(ucPortS != ucPortE) 
		{
			vlanAddNum = ODM_NUM_OF_PORTS*num_of_mcast_vlan;
		}
		else 
		{
			vlanAddNum = 1*num_of_mcast_vlan;
		}
		 
		if(dal_vtt_num+vlanAddNum>15) 
		{
			rlt = OPL_ERROR;
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_tag_strip: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
			goto send_rsp;
		}

		for(portId = ucPortS; portId <= ucPortE; portId++)
		{
			p_translation_entry = (oam_mcast_vlan_translation_entry_t *)p_in_tmp;
			p_in_tmp += sizeof(oam_mcast_vlan_translation_entry_t);
			if(OPL_OK != mctag_translation_check(portId, p_translation_entry->mcast_vlan, p_translation_entry->iptv_vlan))
			{
				DBG_PRINTF("FIALED ... ");
				rlt = OPL_ERROR;
				goto send_rsp;
			}
		}
		  
	}
	/*End add by huangmingjian 2013-10-26:Bug 203,257*/
	
	for(portId = ucPortS; portId <= ucPortE; portId++)
	{
		CTC_DEBUG("call odmMulticastTagstripSet(%d, %d)\n", portId, p_mcast_tag_strip_hdr->tagstriped);
		rlt = odmMulticastTagstripSet(portId,p_mcast_tag_strip_hdr->tagstriped);

		if(rlt != OPL_OK)
		{
			CTC_DEBUG("call odmMulticastTagstripSet() fail!\n");
			goto send_rsp;
		}
	}
	//printf(" %s %d\n",__FUNCTION__,__LINE__);
	/* if tagstriped == 0x02, add mcast vlan translation entry */
	if(p_mcast_tag_strip_hdr->tagstriped == OAM_CTC_MCAST_VLAN_TAG_TRANLATE)
	{   

 	   for(i = 0; i < num_of_mcast_vlan; i++)
 	   {
		   for(portId = ucPortS; portId <= ucPortE; portId++)
		   {

				   p_translation_entry = (oam_mcast_vlan_translation_entry_t *)p_in;
				   p_in += sizeof(oam_mcast_vlan_translation_entry_t);

				   OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationAdd(%ld, %ld, %ld)\n",
																				   portId,
																				   p_translation_entry->mcast_vlan,
																				   p_translation_entry->iptv_vlan);

				   rlt = odmMulticastTagTranslationAdd(portId,
														   p_translation_entry->mcast_vlan,
														   p_translation_entry->iptv_vlan);

				   if(rlt != OPL_OK)
				   {
				//   	printf(" %s %d\n",__FUNCTION__,__LINE__);
						   OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationAdd() fail!\n");
						   goto send_rsp;
				   }
		   }
 	   }
       
	}
	
send_rsp:
	if(rlt == OPL_OK)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("Multicast tag oper set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}
  
    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0043 - MulticastSwitch */
int ctc_oam_varCb_multicastSwitch_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	/*	-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 action
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = 1;
	pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	pVarContainer->pVarData[0] = odmMulticastSwitchGet();

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0043 - MulticastSwitch */
int ctc_oam_varCb_multicastSwitch_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    u8_t	rlt=0;
	oam_mcast_switch_t p_mcast_switch;

	/*get the MulticastSwitch config*/
	p_mcast_switch.branch = pVarContainer->varDesc.varBranch;
	p_mcast_switch.leaf = pVarContainer->varDesc.varLeaf;
	p_mcast_switch.width = pVarContainer->varWidth;
	p_mcast_switch.action = pVarContainer->pVarData[0];

	CTC_DEBUG("mc vlan switch: %d\n", p_mcast_switch.action);

	if((p_mcast_switch.action == 0) || (p_mcast_switch.action == 1))
	{
		rlt = odmMulticastSwitchSet(p_mcast_switch.action);
	}
	else
	{
		rlt = 1;
	}

	if(!rlt) /*for multicast, true means success */
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("Multicast switch set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0044 - MulticastControl */
int ctc_oam_varCb_multicastControl_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t *p_byte;
	u16_t number;
	oam_mcast_control_entry_t	*p_mcast_entry_src=0;

	/*	-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 action
											------------
											1 control_type
											------------
											1 number
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;		
	pVarContainer->pVarData[0] = 3;
	pVarContainer->pVarData[1] = (u8_t)odmMulticastControlTypeGet();		
	p_byte += 3;	
	p_mcast_entry_src = (oam_mcast_control_entry_t*)p_byte;
	odmMulticastControlEntryGet((multicast_control_entry_t *)p_mcast_entry_src, &number);
	pVarContainer->varWidth = 3+10*number;
	pVarContainer->pVarData[2] = number;
	p_byte += 10*number;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0044 - MulticastControl */
int ctc_oam_varCb_multicastControl_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	i;
	oam_mcast_control_t 		p_mcast_control_t;
	oam_mcast_control_t			*p_mcast_control = &p_mcast_control_t;
	oam_mcast_control_entry_t	*p_mcast_entry;
    u32_t retVal = 0;
	u8_t *p_in = pVarContainer->pVarData;

	/*get the MulticastControl config*/
	p_mcast_control->branch = pVarContainer->varDesc.varBranch;
	p_mcast_control->leaf = pVarContainer->varDesc.varLeaf;
	p_mcast_control->width = pVarContainer->varWidth;
	p_mcast_control->action = pVarContainer->pVarData[0];
	p_mcast_control->control_type = pVarContainer->pVarData[1];
	p_mcast_control->number = pVarContainer->pVarData[2];
	p_in += 3;

	if(p_mcast_control->width == 1)
	{
		retVal = odmMulticastControlEntryClear();
	}
	else
	{
		if (p_mcast_control->action == 1)
		{
          multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];         
		  
		  UINT16 ctl_entry_num = 0;
		  UINT16 ctl_entry_index;
		  oam_mcast_control_entry_t	*p_mcast_entry_check;
		  UINT8 ctl_entry_num_per_port[ODM_NUM_OF_PORTS+1] = {0};
		  
		  odmMulticastControlEntryGet(&ctl_entry_list, &ctl_entry_num);

		  for(ctl_entry_index=0; ctl_entry_index<ctl_entry_num; ctl_entry_index++)
		  {
    		    ctl_entry_num_per_port[ctl_entry_list[ctl_entry_index].port]++;
		  }
          
		  p_mcast_entry_check = (oam_mcast_control_entry_t*)p_in;
		  for(i=0; i<p_mcast_control->number; i++)
		  {
    		    ctl_entry_num_per_port[p_mcast_entry_check->port_id]++;
    		    p_mcast_entry_check++;
		  }
		  
		  for(i=ODM_START_PORT_NUN; i<=ODM_NUM_OF_PORTS; i++)
		  {
    		    if(ctl_entry_num_per_port[i]>64)
    		    {
        		    retVal = -1;
        		    CTC_DEBUG("eopl_set_onu_mcast_control: port%d entry>64\n", i);
        		    goto send_rsp;
    		    }
		  }
		}
		/*Begin modified by huangmingjian 2013-12-10 for Bug 306*/
		if((p_mcast_control->control_type>=MC_CTL_GDA_MAC) && (p_mcast_control->control_type < MC_CTL_END))
		{	
			/*we only support MC_CTL_GDA_MAC_VID and MC_CTL_GDA_GDA_IP_VID*/
			if(p_mcast_control->control_type==MC_CTL_GDA_MAC_VID || p_mcast_control->control_type==MC_CTL_GDA_GDA_IP_VID)
			{
				odmMulticastControlTypeSet(p_mcast_control->control_type);		
			}
		}
		/*End modified by huangmingjian 2013-12-10 for Bug 306*/
		
		p_mcast_entry = (oam_mcast_control_entry_t*)p_in;
		for(i=0;i<p_mcast_control->number;i++,p_mcast_entry++)
		{
			if(p_mcast_control->action == 0)
			{
				CTC_DEBUG("mc vlan ctrl del, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);

				retVal = odmMulticastControlEntryDelete(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}
			if(p_mcast_control->action == 1)
			{
				CTC_DEBUG("mc vlan ctrl add, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);
				retVal = odmMulticastControlEntryAdd(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}
		}
	}

send_rsp:
	if(retVal == 0)
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("Multicast Control set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0045 - Group Num Max */
int ctc_oam_varCb_groupNumMax_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_group_num_max_t	*p_group;
	int ret_value =0;
	int tlv_len = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 num_max
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = 1;
	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	if(NULL == pVarContainer->pVarData)
	{
		return EPON_OAM_ERR_MEM;
	}
	p_byte = pVarContainer->pVarData;	
	pVarContainer->pVarData[0] = odmMulticastGroupMaxNumGet(ucPortS);

	p_byte += pVarContainer->varWidth;/*point TLV 2*/	
	if(ucPortS == ucPortE)
	{			
		return EPON_OAM_ERR_OK;/*如果只取一份配置，成功返回*/
	}
	else
	{
		ucPortS += 1;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		/*we must add tlv before the var*/
		tlv_len = ctc_oam_add_tlv(pVarInstant, p_byte, ucPortS);
		if (tlv_len < EPON_OAM_ERR_OK)
		{
			return EPON_OAM_ERR_PARAM;
		}
		p_byte += tlv_len;

		/*
			-----------
			branch
			-----------
			leaf
			-----------
			width
			-----------
			num_max
			-----------
		*/
		p_group = (oam_group_num_max_t*)p_byte;
		p_group->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_group->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
		p_group->width = 1;
		p_group->num_max = odmMulticastGroupMaxNumGet(ucPortS);
		p_byte += p_group->width;
	}

	*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0045 - Group Num Max */
int ctc_oam_varCb_groupNumMax_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
    u8_t	rlt=0, ucPortS, ucPortE;
	oam_group_num_max_t p_group;
	int ret_value =0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the groupNumMax config*/
	p_group.branch = pVarContainer->varDesc.varBranch;
	p_group.leaf = pVarContainer->varDesc.varLeaf;
	p_group.width = pVarContainer->varWidth;
	p_group.num_max = pVarContainer->pVarData[0];
	
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		CTC_DEBUG("mc vlan grp max: %d\n", p_group.num_max);
		rlt = odmMulticastGroupMaxNumSet(ucPortS, p_group.num_max);
	}

	if(!rlt) /*for multicast, true means success */
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("groupNumMax set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0046 - aFastLeaveAbility */
int ctc_oam_varCb_aFastLeaveAbility_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	/*yotc cant bind */
    unsigned int enumCnt = 4, enumValue;

    pVarContainer->varDesc = varDesc;
    /* Length varies according to the enum number
     * Here reply all possible enum values defined in CTC standard
     */
    pVarContainer->varWidth = 
        CTC_OAM_VAR_AFASTLEAVEABILITY_HDR_LEN +
        (CTC_OAM_VAR_AFASTLEAVEABILITY_ENUM_LEN * enumCnt);
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* Fill in the header */
    CTC_BUF_ENCODE32(pVarContainer->pVarData, &enumCnt);

    /* 1st enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_SNOOPING_NONFASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 4), &enumValue);

    /* 2nd enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_SNOOPING_FASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 8), &enumValue);

    /* 3rd enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_IGMP_NONFASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 12), &enumValue);

    /* 4th enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_IGMP_FASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 16), &enumValue);
	#if 0
    /* 5th enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_MLD_NONFASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 20), &enumValue);

    /* 6th enum */
    enumValue = CTC_OAM_VAR_AFASTLEAVEABILITY_MLD_FASTLEAVE;
    CTC_BUF_ENCODE32((pVarContainer->pVarData + 24), &enumValue);
	#endif 
    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x0047 - aFastLeaveAdminState */
int ctc_oam_varCb_aFastLeaveAdminState_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
    unsigned int setValue;
    pVarContainer->varDesc = varDesc;
    pVarContainer->varWidth = CTC_OAM_VAR_AFASTLEAVEADMINSTATE_LEN; /* Fix value from CTC standard */
    pVarContainer->pVarData = (unsigned char *) malloc(pVarContainer->varWidth * sizeof(unsigned char));
    if(NULL == pVarContainer->pVarData)
    {
        return EPON_OAM_ERR_MEM;
    }

    /* MODIFY ME - change to actual get function and fill the value */
    setValue = CTC_OAM_VAR_AFASTLEAVEADMINSTATE_DISABLED;
    CTC_BUF_ENCODE32(pVarContainer->pVarData, &setValue);

    return EPON_OAM_ERR_OK;
}
/* 0xC9/0x0001 - Reset ONU */
int ctc_oam_varCb_resetOnu_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	
	int reboot_fd = -1;
    pVarContainer->varWidth = 0x80;

    /* OLT issue reset ONU command */
    CTC_DEBUG("%s:%d OLU issue reset ONU command\n", __FILE__, __LINE__);
    /* Sleep 3 seconds to make sure the print complete */
	if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
//	if((reboot_fd = open("/tmp/reboot.lock", O_RDWR|O_CREAT|O_TRUNC)) < 0)
	{
		/*should not go here*/
		printf("File open error\n");
		return EPON_OAM_ERR_OK;
	}
	
	if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
	{
		/*system is going to reboot, so no need to release lock*/
		printf("System is performing upgrade\n");
		return EPON_OAM_ERR_OK;
	}
    sleep(3);
    reboot(LINUX_REBOOT_CMD_RESTART);

    return EPON_OAM_ERR_OK;
}

/* 0xC9/0x0048 - acFastLeaveAdminControl */
int ctc_oam_varCb_acFastLeaveAdminControl_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=0, *p_byte;
	oam_fastleave_adm_t p_fast;
	
    p_fast.branch = pVarContainer->varDesc.varBranch;
	p_fast.leaf = pVarContainer->varDesc.varLeaf;
	p_fast.width = pVarContainer->varWidth;
	CTC_BUF_PARSE32(pVarContainer->pVarData, &p_fast.value);

	CTC_DEBUG("mc fast leave set: %d\n", p_fast.value);

	if (1 == p_fast.value)
	{
		rlt = odmMulticastFastleaveModeSet(0);
	}
	else if (2 == p_fast.value)
	{
		rlt = odmMulticastFastleaveModeSet(1);
	}

	if(!rlt) /*for multicast, true means success */
	{
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
	}
	else
	{
		CTC_DEBUG("FastLeaveAdminControl set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

    return EPON_OAM_ERR_OK;
}

int ctc_oam_churningKey_set(
    unsigned char llidIdx,
    unsigned char keyIdx,
    unsigned char key[])
{
    rtk_epon_churningKeyEntry_t entry;

    entry.llidIdx = llidIdx;
    entry.keyIdx = keyIdx;
    memcpy(entry.churningKey, key, RTK_EPON_KEY_SIZE);

    /* Call RTK API to set key to HW */
    rtk_epon_churningKey_set(&entry);

    return EPON_OAM_ERR_OK;
}

int ctc_oam_dbaConfig_get(
    ctc_dbaThreshold_t *pDbaThreshold)
{
    /* MODIFY ME - replace with acturl RTK API
     * Fill in the dummy database
     */
    *pDbaThreshold = dbaThresholdDb;

    return EPON_OAM_ERR_OK;
}

int ctc_oam_dbaConfig_set(
    ctc_dbaThreshold_t *pDbaSetResp)
{
    /* MODIFY ME - replace with acturl RTK API
     * pretend to store the data
     */
    dbaThresholdDb = *pDbaSetResp;

    return EPON_OAM_ERR_OK;
}

/* 0xC7/0x001d - EthPortPause */
int ctc_oam_varCb_ethPortmode_set(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    ctc_varContainer_t *pVarContainer)   /* caller prepare all the resource needed, 
                                          * handler won't need to allocate anything
                                          */
{
	u8_t	rlt=OPL_OK, ucPortS, ucPortE;
	oam_eth_portpause_t p_pause;
	int ret_value =0;
	u32_t port_mode_duplex = 0;
	u32_t port_mode_speed = 0;
	u32_t port = 0;

	if(CTC_OAM_VAR_ETHPORTPAUSE_LEN != pVarContainer->varWidth)
	{
		return EPON_OAM_ERR_UNKNOWN;
	}

	if(NULL == pVarInstant)
	{
		return EPON_OAM_ERR_NOT_FOUND;
	}

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*get the pause config*/
	p_pause.branch	= pVarContainer->varDesc.varBranch;
	p_pause.leaf	= pVarContainer->varDesc.varLeaf;
	p_pause.width	= pVarContainer->varWidth;
	p_pause.back_oper = pVarContainer->pVarData[0];

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		switch(p_pause.back_oper)
		{
			case OAM_PORT_MODE_10H:
				port_mode_duplex = DUPLEX_HALF;
				port_mode_speed = SPEED_10M;
				break;
			case OAM_PORT_MODE_10F:
				port_mode_duplex = DUPLEX_FULL;
				port_mode_speed = SPEED_10M;
				break;
			case OAM_PORT_MODE_100H:
				port_mode_duplex = DUPLEX_HALF;
				port_mode_speed = SPEED_100M;
				break;
			case OAM_PORT_MODE_100F:
				port_mode_duplex = DUPLEX_FULL;
				port_mode_speed = SPEED_100M;
				break;
			case OAM_PORT_MODE_1000F:
				port_mode_speed = SPEED_1000M;
				break;
			default:
				rlt = OPL_ERROR;
				break;
		}
		port = (u32_t)ucPortS;
		rlt = DRV_SetPortSpeed(port, port_mode_speed);
		if(OPL_OK != rlt)
		{
			break;
		}
		
		rlt = DRV_SetPortDuplex(ucPortS, port_mode_duplex);
	}

	if(rlt == OPL_OK)
	{
		/*	回复报文
			-----------
			1 Branch
			-----------
			1 Leaf
			-----------以上信息已经在外层添加，我们只需要从此处添加相关信息
			1 Indication
			-----------
		*/
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_SETOK;
		/*
		注意:对于回复报文我们需要打上标签,
		但是set操作会在外层自动打上
		*/
	}
	else
	{
		CTC_DEBUG("ethPortPause_set fail\n");
		pVarContainer->varWidth = CTC_OAM_VAR_RESP_VARNORESOURCE;
	}

	return EPON_OAM_ERR_OK;
}
/* 0xC7/0x001f - EthPortStatistic */
int ctc_oam_varCb_ethPortStatistic_get(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer)   /* handler should allocate resource, 
                                          * caller will release it
                                          */
{
	u8_t ucPortS, ucPortE, *p_byte;
	oam_eth_portpause_t *p_pause;
    u32_t state=0;
    BOOL bPortStatus = TRUE;
	int ret_value =0;
	int tlv_len = 0;
	int ret = 0;

	if(NULL == pVarInstant)
    {
        return EPON_OAM_ERR_NOT_FOUND;
    }

	ret_value = ctc_oam_get_portid(pVarInstant, varDesc, pVarContainer, &ucPortS, &ucPortE);
	if(EPON_OAM_ERR_PARAM == ret_value)
	{
		return EPON_OAM_ERR_OK;
	}

	/*	第一份配置的TLV已经在外层实现
		--------------Index TLV 1
		branch
		-----------
		leaf
		-----------
		width
		-----------
		value			
		-------------------------------
		pVarContainer->varDesc.varBranch
		-------------------------------
		pVarContainer->varDesc.varLeaf
		-------------------------------
		pVarContainer->varWidth
		------------------------------------------------
		pVarContainer->pVarData--------------	1 back_oper
											------------
	*/
	pVarContainer->varDesc = varDesc;
	pVarContainer->varWidth = 78;

	pVarContainer->pVarData = (unsigned char *) malloc(1500);
	UNI_COUNTER_S mibCounters;

	ret = DRV_GetPortMibCounter(ucPortS, &mibCounters);
	if(ret != NO_ERROR)
	{
		CTC_DEBUG(("[FILE] %s [LINE] %d\n", __FILE__, __LINE__));
	}
	*(pVarContainer->pVarData) = mibCounters.uiRecOctets; 
	pVarContainer->pVarData+=8;
	*(pVarContainer->pVarData) = mibCounters.InBroadcast+mibCounters.InMulticast+mibCounters.InUnicast;
	pVarContainer->pVarData+=8;
	*(pVarContainer->pVarData) = mibCounters.InUnicast;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.InMulticast;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.InBroadcast;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = 0;
	pVarContainer->pVarData+=4;
//	p_stats->ifInErrorPkts = mibCounters.InError;
	*(pVarContainer->pVarData) = mibCounters.InUnderflow;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = 0;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.InOverflow;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = 0;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.InCrcError;
	pVarContainer->pVarData+=4;
	
	*(pVarContainer->pVarData) = mibCounters.uiTransOctets;
	pVarContainer->pVarData+=8;
	*(pVarContainer->pVarData) = mibCounters.OutBroadcast+mibCounters.OutMulticast+mibCounters.OutUnicast;
	pVarContainer->pVarData+=8;
	*(pVarContainer->pVarData) = mibCounters.OutUnicast;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.OutMulticast;
	pVarContainer->pVarData+=4;
	*(pVarContainer->pVarData) = mibCounters.OutBroadcast;
	//*pVarContainer_Total_Len = p_byte - pVarContainer->pVarData;

	return EPON_OAM_ERR_OK;
}




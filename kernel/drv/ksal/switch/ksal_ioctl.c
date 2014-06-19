#ifdef  __cplusplus
extern "C"{
#endif

#include <linux/kernel.h>
#include <bspchip.h>
#include "hal_common.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

extern unsigned int g_uiIfPonStateErr;
extern unsigned int g_uiIfPonModeOff;

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
LW_DRV_OP_S g_LW_Drv_Ops;
unsigned int g_uiIfPonIntChangeErr = TRUE;
unsigned int g_oam_discovery_state = 0;
unsigned int g_mac_write_flag = 0;
unsigned int g_uiLaserOnTest_Flag = LASEON_TEST_DISABLE; /*Add by huangmingjian 2013/03/19 for EPN204QID0033*/ 

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#define REG_INTERRUPT_BASEADDR        (0x0B80)
//#define REG_INTERRUPT_LEVEL_STATUS    ((REG_INTERRUPT_BASEADDR + 0)*4)
#define REG_INTERRUPT_LEVEL_ENABLE    ((REG_INTERRUPT_BASEADDR + 2)*4)
//#define REG_INTERRUPT_CONFIGURE       ((REG_INTERRUPT_BASEADDR + 6)*4)

/*begin added by liaohongjun 2012/11/19 of EPN204QID0008*/
#define REG_OPCONN_BASE_ADDR		0x00000000
#define REG_GB_BA_B                  (REG_OPCONN_BASE_ADDR + 0x0000)
#define REG_LEVEL_CP_LED_CTRL    ((REG_GB_BA_B + 0x0025)*4)
/*end added by liaohongjun 2012/11/19 of EPN204QID0008*/

DRV_RET_E Drv_L2SwtichIoctl(DRV_REQ_S *pstDrvReq)
{
	UINT32 uiRegValTmp;

    if(NULL == pstDrvReq)
    {
        return DRV_ERR_PARA;
    }

    switch (pstDrvReq->cmd)
    {
        #if 0
        /*drv_port*/
        case DRV_CMD_FIBER_CFG_INIT:
            return Hal_FiberPortCfgInit(pstDrvReq->p_lport, &pstDrvReq->para2_u.stComboCfg);
        case DRV_CMD_COPPER_CFG_INIT:
            return Hal_CopperPortCfgInit(pstDrvReq->p_lport, &pstDrvReq->para2_u.stComboCfg);
        #endif
        /*Single port*/
        case DRV_CMD_SET_MEDIUM:
			if(g_LW_Drv_Ops.p_Hal_SetPortMedium!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortMedium(pstDrvReq->p_lport, pstDrvReq->p_medium);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_PRIORITY:
			if(g_LW_Drv_Ops.p_Hal_SetPortPriority!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortPriority(pstDrvReq->p_lport, pstDrvReq->p_prio);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MDIX:
			if(g_LW_Drv_Ops.p_Hal_SetPortMdix!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortMdix(pstDrvReq->p_lport, pstDrvReq->p_mdix);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_PORTENABLE:
			if(g_LW_Drv_Ops.p_Hal_SetPortEnable!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortEnable(pstDrvReq->p_lport, pstDrvReq->p_enable);
			else
				return DRV_NULL_POINTER;
        /*begin added by liaohongjun 2012/12/10 of EPN104QID0086*/    
        case DRV_CMD_GET_PORTENABLE:
			if(g_LW_Drv_Ops.p_Hal_GetPortEnable!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortEnable(pstDrvReq->p_lport, &(pstDrvReq->p_enable));
			else
				return DRV_NULL_POINTER;       
        /*end added by liaohongjun 2012/12/10 of EPN104QID0086*/    
        case DRV_CMD_SET_JUMBO:
			if(g_LW_Drv_Ops.p_Hal_SetPortJumbo!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortJumbo(pstDrvReq->p_lport, &(pstDrvReq->p_jumbo));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_TXRX:
			if(g_LW_Drv_Ops.p_Hal_SetPortTxRx!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortTxRx(pstDrvReq->p_lport, &(pstDrvReq->p_txrx));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_ABILITY:
			if(g_LW_Drv_Ops.p_Hal_SetPortAbility!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortAbility(pstDrvReq->p_lport, pstDrvReq->p_ability, pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;
			
        case DRV_CMD_SET_AUTONEG:
			if(g_LW_Drv_Ops.p_Hal_SetPortAutoneg!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortAutoneg(pstDrvReq->p_lport, pstDrvReq->p_autoneg, pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;
		/*Begin add by shipeng 2013/05/27 for CTC*/ 
		case DRV_CMD_GET_AUTONEG:
			if(g_LW_Drv_Ops.p_Hal_GetPortAutoneg!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortAutoneg(pstDrvReq->p_lport, &(pstDrvReq->p_autoneg));
			else
				return DRV_NULL_POINTER;
		/*End add by shipeng 2013/05/27 for CTC*/ 
        case DRV_CMD_SET_SPEED:
			if(g_LW_Drv_Ops.p_Hal_SetPortSpeed!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortSpeed(pstDrvReq->p_lport, pstDrvReq->p_speed, pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_DUPLEX:	
			if(g_LW_Drv_Ops.p_Hal_SetPortDuplex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortDuplex(pstDrvReq->p_lport, pstDrvReq->p_duplex, pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;	
        case DRV_CMD_SET_SPDLX:
			if(g_LW_Drv_Ops.p_Hal_SetPortSpdlx!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortSpdlx(pstDrvReq->p_lport, pstDrvReq->p_spdlx);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_PHYREG:
			if(g_LW_Drv_Ops.p_Hal_SetPhyReg!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPhyReg(pstDrvReq->p_lport, pstDrvReq->para2_u.uiValue, pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PHYREG:
			if(g_LW_Drv_Ops.p_Hal_GetPhyReg!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPhyReg(pstDrvReq->p_lport, &(pstDrvReq->para2_u.uiValue), pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_ETHDBG:
			if(g_LW_Drv_Ops.p_Hal_SetEthdbg!=NULL)
			{
            	g_LW_Drv_Ops.p_Hal_SetEthdbg(pstDrvReq->para1_u.uiValue);
				return DRV_OK;
			}	
			else
			{
            	return DRV_OK;
			}
        case DRV_CMD_GET_SPDLX_SET:
			if(g_LW_Drv_Ops.p_Hal_GetSpdlxSet!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetSpdlxSet(pstDrvReq->p_lport, &(pstDrvReq->para2_u.uiValue));   
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_ISOLATE_SET:
			if(g_LW_Drv_Ops.p_Hal_GetIsolateSet!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetIsolateSet(&(pstDrvReq->p_mask));
        	else
				return DRV_NULL_POINTER;
        case DRV_CMD_ADD_ACL_FILTER:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterCreate!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterCreate(pstDrvReq->p_lport, \
                                                  pstDrvReq->para2_u.uiValue, \
                                                  (ACL_TRUST_MODE_E)pstDrvReq->para4_u.uiValue, \
                                                  (ACL_DIRECTION_E)pstDrvReq->para5_u.uiValue, \
                                                  (void *)&pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_DEL_ACL_FILTER_BYPORT:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterDelete!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterDelete(pstDrvReq->p_lport, \
                                                  pstDrvReq->para2_u.uiValue, \
                                                  (ACL_DIRECTION_E)pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_ADD_CTC_CLF_BYPORT:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkCreate!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkCreate(pstDrvReq->p_lport, \
                                                  pstDrvReq->para4_u.uiValue, \
                                                  (ACL_TRUST_MODE_E)pstDrvReq->para5_u.uiValue, \
                                                  (void *)&pstDrvReq->para2_u.uiValue, \
                                                  (void *)&pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_DEL_CTC_CLF_BYPREC:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkDelete!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkDelete(pstDrvReq->p_lport, \
                                                  pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CLEAR_CTC_CLF_BYPORT:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkClear!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkClear(pstDrvReq->p_lport);  
       		else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_PAUSE:
			if(g_LW_Drv_Ops.p_Hal_SetPortFlowctrl!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortFlowctrl(pstDrvReq->p_lport, pstDrvReq->p_pause_tx, pstDrvReq->p_pause_rx);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTMEDIUM:
			if(g_LW_Drv_Ops.p_Hal_GetPortMedium!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortMedium(pstDrvReq->p_lport, &(pstDrvReq->p_medium));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_CUR_LINK:
			if(g_LW_Drv_Ops.p_Hal_GetPortCurrentLink!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortCurrentLink(pstDrvReq->p_lport, &(pstDrvReq->p_cur_link));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTABILITY:
			if(g_LW_Drv_Ops.p_Hal_GetPortAbility!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortAbility(pstDrvReq->p_lport, &(pstDrvReq->p_ability), pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTSPEED:
			if(g_LW_Drv_Ops.p_Hal_GetPortSpeed!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortSpeed(pstDrvReq->p_lport, &(pstDrvReq->p_speed));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTDUPLEX:
			if(g_LW_Drv_Ops.p_Hal_GetPortDuplex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortDuplex(pstDrvReq->p_lport, &(pstDrvReq->p_duplex));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_LINK_MASK:
			if(g_LW_Drv_Ops.p_Hal_GetPortLinkMask!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortLinkMask(&(pstDrvReq->p_mask));
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GET_MEDIA_FIBER_MASK:
			if(g_LW_Drv_Ops.p_Hal_GetPortMediaFiberMask!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortMediaFiberMask(&(pstDrvReq->p_mask));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTMDIX:
			if(g_LW_Drv_Ops.p_Hal_GetPortMdix!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortMdix(pstDrvReq->p_lport, &(pstDrvReq->p_mdix));
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GET_PORTPRIO:
			if(g_LW_Drv_Ops.p_Hal_GetPortPriority!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortPriority(pstDrvReq->p_lport, &(pstDrvReq->p_prio));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTFLOWCTRL:
			if(g_LW_Drv_Ops.p_Hal_GetPortFlowctrl!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortFlowctrl(pstDrvReq->p_lport, &(pstDrvReq->p_flowctrl));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTJUMBO:
			if(g_LW_Drv_Ops.p_Hal_GetPortJumbo!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortJumbo(pstDrvReq->p_lport, &(pstDrvReq->p_jumbo));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORTFRAMELEN:
			if(g_LW_Drv_Ops.p_Hal_GetPortMaxFrameLen!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortMaxFrameLen(pstDrvReq->p_lport, &(pstDrvReq->p_frame_len));
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_PORTFRAMELEN:
			if(g_LW_Drv_Ops.p_Hal_SetPortMaxFrameLen!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortMaxFrameLen(pstDrvReq->p_lport, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        #if 0
        case DRV_CMD_GET_FIBERPORTSPEED:
            return Hal_GetFiberSpeed(pstDrvReq->p_lport, &(pstDrvReq->p_speed));
        #endif
        /*led*/
        case DRV_CMD_PORT_LED_INIT:
			if(g_LW_Drv_Ops.p_Hal_PortLedInit!=NULL)
            	return g_LW_Drv_Ops.p_Hal_PortLedInit();
			else
				return DRV_NULL_POINTER;	
        case DRV_CMD_SET_PORT_LED:
			if(g_LW_Drv_Ops.p_Hal_SetPortLed!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortLed(pstDrvReq->p_lport, pstDrvReq->p_ledstate, pstDrvReq->p_porttype);
			else
				return DRV_NULL_POINTER;	
        /*drv_fiber*/
        case DRV_CMD_I2C_INIT:
			if(g_LW_Drv_Ops.p_Hal_I2cFiberInit!=NULL)
            	return g_LW_Drv_Ops.p_Hal_I2cFiberInit();
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_I2C_PORT_READ:
			if(g_LW_Drv_Ops.p_Hal_I2cFiberPortRead!=NULL)
            	return g_LW_Drv_Ops.p_Hal_I2cFiberPortRead(pstDrvReq->p_lport, (UINT16)(pstDrvReq->para3_u.uiValue), (UINT8*)(pstDrvReq->para2_u.ulValue), pstDrvReq->para4_u.uiValue, ADDRESS_IN_USER_SPACE);        
			else
				return DRV_NULL_POINTER;
        /*drv_aggr*/
        case DRV_CMD_SET_AGGRMODE:
			if(g_LW_Drv_Ops.p_Hal_SetAggrMode!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetAggrMode(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;	
        case DRV_CMD_SET_AGGRGROUP:
			if(g_LW_Drv_Ops.p_Hal_SetAggrGroup!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetAggrGroup(pstDrvReq->para1_u.uiValue, 
                pstDrvReq->para2_u.lgcMask);
			}
     		else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_VLAN_NUM:
			if(g_LW_Drv_Ops.p_Hal_GetVlanExistNum!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_GetVlanExistNum(&(pstDrvReq->para2_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLANMODE:
			if(g_LW_Drv_Ops.p_Hal_SetVlanMode!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetVlanMode(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLAN_ENTRY_CREATE:
			if(g_LW_Drv_Ops.p_Hal_SetVlanEntryCreate!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetVlanEntryCreate(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_VLAN_ENTRY_DELETE:
			if(g_LW_Drv_Ops.p_Hal_SetVlanEntryDelete!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetVlanEntryDelete(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLAN_MEMBER_REMOVE:
			if(g_LW_Drv_Ops.p_Hal_SetVlanMemberRemove!=NULL)
			{
	            return g_LW_Drv_Ops.p_Hal_SetVlanMemberRemove(pstDrvReq->para1_u.uiValue, 
	                pstDrvReq->para2_u.lgcMask);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLAN_MEMBER_ADD:
			if(g_LW_Drv_Ops.p_Hal_SetVlanMemberAdd!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetVlanMemberAdd(pstDrvReq->para1_u.uiValue, 
                	pstDrvReq->para2_u.lgcMask, pstDrvReq->para3_u.lgcMask);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLAN_PVID:
			if(g_LW_Drv_Ops.p_Hal_SetVlanPvid!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetVlanPvid(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_VLAN_PORT_VLAN_MEMBER:
			if(g_LW_Drv_Ops.p_Hal_SetVlanPortVlanMember!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetVlanPortVlanMember(pstDrvReq->para1_u.uiValue, 
                	pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_VLAN_MEMBER:
			if(g_LW_Drv_Ops.p_Hal_GetVlanMember!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_GetVlanMember(pstDrvReq->para1_u.uiValue, 
                	&(pstDrvReq->para2_u.lgcMask), &(pstDrvReq->para3_u.lgcMask));
			}
			else
				return DRV_NULL_POINTER;
        /*drv_mvlan*/
        case DRV_CMD_SET_CPU_IN_VLAN:
			if(g_LW_Drv_Ops.p_Hal_AddCpuToVlanMember!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AddCpuToVlanMember(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_CPU_OUT_VLAN:
			if(g_LW_Drv_Ops.p_Hal_RemoveCpuFromVlanMember!=NULL)
            	return g_LW_Drv_Ops.p_Hal_RemoveCpuFromVlanMember(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        /*drv_mac*/
        case DRV_CMD_SET_MAC_CPU_PORT_LEARM_ENABLE:
			if(g_LW_Drv_Ops.p_Hal_SetCpuPortMacLearnEnable!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetCpuPortMacLearnEnable(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_LEARN_ENABLE:
			if(g_LW_Drv_Ops.p_Hal_SetMacLearnEnable!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacLearnEnable(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GET_MAC_LEARN_ENABLE:
			if(g_LW_Drv_Ops.p_Hal_GetMacLearnEnable!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacLearnEnable(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_MAC_CPU_MAC_ADD:
			if(g_LW_Drv_Ops.p_Hal_SetMacCpuMacAdd!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacCpuMacAdd((vlan_id_t)pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.mac_address);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_FLUSH_UCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacFlushUcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacFlushUcastMac(pstDrvReq->para2_u.stMacDelete);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_ADD_UCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacAddUcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacAddUcastMac(pstDrvReq->para2_u.stMacUcast);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_DEL_UCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacDelUcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacDelUcastMac(pstDrvReq->para2_u.stMacUcast);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_ADD_MCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacAddMcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacAddMcastMac(pstDrvReq->para2_u.stMacMcast);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_DEL_MCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacDelMcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacDelMcastMac(pstDrvReq->para2_u.stMacMcast);
			else
				return DRV_NULL_POINTER;
	/*Begin add by huangmingjian 2013-09-07 for mac aging time*/
        case DRV_CMD_GET_MAC_AGE_TIME:
			if(g_LW_Drv_Ops.p_Hal_GetMacAgeTime!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacAgeTime(&(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;		
	/*End add by huangmingjian 2013-09-07 for mac aging time*/
        case DRV_CMD_SET_MAC_AGE_TIME:
			if(g_LW_Drv_Ops.p_Hal_SetMacAgeTime!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacAgeTime(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_MAC_LEARN_LIMIT:
			if(g_LW_Drv_Ops.p_Hal_SetMacLearnLimit!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacLearnLimit(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue, pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GET_MAC_UCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_GetMacUcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacUcastMac((vlan_id_t)pstDrvReq->para1_u.uiValue, pstDrvReq->para3_u.mac_address, 
                	&(pstDrvReq->para2_u.stMacUcast));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_MCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_GetMacMcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacMcastMac((vlan_id_t)pstDrvReq->para1_u.uiValue, pstDrvReq->para3_u.mac_address, 
                	&(pstDrvReq->para2_u.stMacMcast));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_SET_MCAST_MAC:
			if(g_LW_Drv_Ops.p_Hal_SetMacSetMcastMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacSetMcastMac(pstDrvReq->para2_u.stMacMcast);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_MAC_UCAST_LOOK_FAIL_FRD:
			if(g_LW_Drv_Ops.p_Hal_SetMacLookFailFrd!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacLookFailFrd(pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_LEARN_COUNT:
			if(g_LW_Drv_Ops.p_Hal_GetMacLearnCount!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacLearnCount(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_REFRESH_UCAST:
			if(g_LW_Drv_Ops.p_Hal_RefreshUCast!=NULL)
            	return g_LW_Drv_Ops.p_Hal_RefreshUCast(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;            
        case DRV_CMD_SET_RSV_MAC_TRAP:
			if(g_LW_Drv_Ops.p_Hal_SetReservedMacTrap!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetReservedMacTrap(pstDrvReq->para1_u.uiValue, pstDrvReq->p_prio, pstDrvReq->para3_u.uiAction);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_DUMP_UCAST:
			if(g_LW_Drv_Ops.p_Hal_GetMacUcastDump!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacUcastDump(pstDrvReq->para2_u.ulValue, &(pstDrvReq->para3_u.uiValue), ADDRESS_IN_USER_SPACE);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_DUMP_MCAST:
			if(g_LW_Drv_Ops.p_Hal_GetMacMcastDump!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacMcastDump(pstDrvReq->para2_u.ulValue, &(pstDrvReq->para3_u.uiValue), ADDRESS_IN_USER_SPACE);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_DUMP_UCAST_SORTED:
			if(g_LW_Drv_Ops.p_Hal_GetMacUcastDumpSorted!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacUcastDumpSorted(pstDrvReq->para2_u.ulValue, &(pstDrvReq->para3_u.uiValue), ADDRESS_IN_USER_SPACE);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MAC_TRAP_CPU:
			if(g_LW_Drv_Ops.p_Hal_SetMacTrapToCpu!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetMacTrapToCpu(pstDrvReq->para1_u.lgcPort, pstDrvReq->para2_u.stTrapSet, pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_CONFLICT:
			if(g_LW_Drv_Ops.p_Hal_GetMacFindConflictMac!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacFindConflictMac(pstDrvReq->para2_u.stMacOp, &(pstDrvReq->para3_u.stMacOp));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_BYINDEX:
			if(g_LW_Drv_Ops.p_Hal_GetMacByIndex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacByIndex(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.stMacCommon));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_MORE_BYINDEX:
			if(g_LW_Drv_Ops.p_Hal_GetMacMoreByIndex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacMoreByIndex(pstDrvReq->para1_u.uiValue, pstDrvReq->para3_u.uiValue, 
                	pstDrvReq->para2_u.ulValue, &(pstDrvReq->para4_u.uiValue), &(pstDrvReq->para3_u.uiValue), ADDRESS_IN_USER_SPACE);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MAC_EXACT:
			if(g_LW_Drv_Ops.p_Hal_GetMacExact!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetMacExact((vlan_id_t)pstDrvReq->para1_u.uiValue,
                	pstDrvReq->para3_u.mac_address, 
                	&(pstDrvReq->para2_u.stMacCommon));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_MCASTCOUNT:
			if(g_LW_Drv_Ops.p_Hal_GetMcastCount!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GetMcastCount(&(pstDrvReq->para1_u.uiValue));        
        	else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_MAC_DLF_MCAST_DROP:
            if(g_LW_Drv_Ops.p_Hal_SetMcastLookupMissDrop!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetMcastLookupMissDrop(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_IGMP_TRAP:
			if(g_LW_Drv_Ops.p_Hal_SetIgmpPktAction!=NULL)
			{
				return g_LW_Drv_Ops.p_Hal_SetIgmpPktAction(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
        /*drv_isolate*/
        case DRV_CMD_SET_PORT_ISOLATE_MASK:
			if(g_LW_Drv_Ops.p_Hal_SetPortIsolateMask!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetPortIsolateMask(&(pstDrvReq->para2_u.lgcMask));
			else
				return DRV_NULL_POINTER;
        /*drv_mirror*/
        case DRV_CMD_SET_MIRROR_GROUP:
			if(g_LW_Drv_Ops.p_Hal_SetMirrorGroup!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetMirrorGroup(pstDrvReq->para1_u.lgcPort, pstDrvReq->para2_u.lgcMask, pstDrvReq->para3_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        /*drv_statistics*/
        case DRV_CMD_GET_MIB_COUNT:
			if(g_LW_Drv_Ops.p_Hal_GetMibPortStatistics!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_GetMibPortStatistics(pstDrvReq->para1_u.lgcPort, pstDrvReq->para2_u.mibCountType, &pstDrvReq->para3_u.ui64Value);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MIB_COUNT_SYNC:
			if(g_LW_Drv_Ops.p_Hal_SetMibCountSync!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetMibCountSync();
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_MIB_COUNT_RESET:
			if(g_LW_Drv_Ops.p_Hal_SetMibCountReset!=NULL)
    	        return g_LW_Drv_Ops.p_Hal_SetMibCountReset((port_num_t)(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GET_PORT_COUNTER:
			if(g_LW_Drv_Ops.p_Hal_GetPortCounter!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetPortCounter(pstDrvReq->para1_u.lgcPort, pstDrvReq->para2_u.portCounterType, &pstDrvReq->para3_u.ui64Value);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GET_PORT_STATISTICS:
			if(g_LW_Drv_Ops.p_Hal_GetPortStatistics!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GetPortStatistics(pstDrvReq->para1_u.lgcPort, pstDrvReq->para2_u.portStatisticsType, &pstDrvReq->para3_u.ui64Value);
			else
				return DRV_NULL_POINTER;
		/*drv_ratelimit*/
        case DRV_CMD_SET_RATELIMIT_INIT:
			if(g_LW_Drv_Ops.p_Hal_SetRatelimitInit!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetRatelimitInit();
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_RATELIMIT_PORT:
			if(g_LW_Drv_Ops.p_Hal_SetRatelimitByPort!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_SetRatelimitByPort(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask, pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_RATELIMIT_STORM:
			if(g_LW_Drv_Ops.p_Hal_SetRatelimitStormCtl!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetRatelimitStormCtl(pstDrvReq->para1_u.stormType, pstDrvReq->para2_u.lgcMask, pstDrvReq->para3_u.stStorm);
			else
				return DRV_NULL_POINTER;
        /*drv_qos*/
        case DRV_CMD_SET_QOS_INIT:
			if(g_LW_Drv_Ops.p_Hal_SetQosInit!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_SetQosInit();
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_QOS_1P_REMAP:
			if(g_LW_Drv_Ops.p_Hal_Set1pIngressPriorityRemap!=NULL)
	            return g_LW_Drv_Ops.p_Hal_Set1pIngressPriorityRemap(pstDrvReq->para2_u.st1pToIntPri);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_SET_QOS_DSCP_REMAP:
			if(g_LW_Drv_Ops.p_Hal_SetDscpIngressPriorityRemap!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetDscpIngressPriorityRemap(pstDrvReq->para2_u.stDscpToIntPri);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_QOS_TRUST_MODE:
			if(g_LW_Drv_Ops.p_Hal_SetQosTrustMode!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetQosTrustMode(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SET_QOS_SCHEDULE:
			if(g_LW_Drv_Ops.p_Hal_SetQosQueueSchedul!=NULL)
			{
            	return g_LW_Drv_Ops.p_Hal_SetQosQueueSchedul(pstDrvReq->para2_u.stQueueSchedule);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_QOS_2_QID:
			if(g_LW_Drv_Ops.p_Hal_SetCos2QidRemap!=NULL)
				return g_LW_Drv_Ops.p_Hal_SetCos2QidRemap(pstDrvReq->para2_u.st1pToIntPri);
			else
				return DRV_NULL_POINTER;
		/* read/write register */  
        case DRV_CMD_REG_READ:
			if(g_LW_Drv_Ops.p_Hal_soc_reg_read!=NULL)
    	        return g_LW_Drv_Ops.p_Hal_soc_reg_read(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue, &(pstDrvReq->para3_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_REG_WRITE:
			if(g_LW_Drv_Ops.p_Hal_soc_reg_write!=NULL)
            	return g_LW_Drv_Ops.p_Hal_soc_reg_write(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue, pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_CPU_GET:
            if(g_LW_Drv_Ops.p_Hal_CpuRegRead!=NULL)
            	return g_LW_Drv_Ops.p_Hal_CpuRegRead(pstDrvReq->para1_u.uiValue*4, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CPU_SET:
            if(g_LW_Drv_Ops.p_Hal_CpuRegWrite!=NULL)
            	return g_LW_Drv_Ops.p_Hal_CpuRegWrite(pstDrvReq->para1_u.uiValue*4, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        #if 0
        case DRV_CMD_SET_ACL_INIT:
            return Hal_SetAclInit();
        case DRV_CMD_ADD_ACL_VLAN_INTF:
            return Hal_SetVlanInterfaceAdd((vlan_id_t)(pstDrvReq->para1_u.uiValue), pstDrvReq->para2_u.mac_address);
        case DRV_CMD_DEL_ACL_VLAN_INTF:
            return Hal_SetVlanInterfaceDel((vlan_id_t)(pstDrvReq->para1_u.uiValue));
        #endif
        /*drv_eee*/
        case DRV_CMD_EEE_ENABLE:
            if(g_LW_Drv_Ops.p_Hal_SetEEEByPort!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetEEEByPort(&(pstDrvReq->para2_u.lgcMask), (BOOL)(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_AUTO_PWR_DWN_ENABLE:
            if(g_LW_Drv_Ops.p_Hal_SetAutoPwrDwnByPort!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetAutoPwrDwnByPort(&(pstDrvReq->para2_u.lgcMask), (BOOL)(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
        /*drv cable diag*/
        case DRV_CMD_GET_CABLE_DIAG:
			if(g_LW_Drv_Ops.p_Hal_GetCableDiag!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GetCableDiag(pstDrvReq->para1_u.lgcPort, &(pstDrvReq->para2_u.stCableDiag));
			else
				return DRV_NULL_POINTER;
		/*drv stp*/
		case DRV_CMD_GET_STP_PORTSTATE:
			if(g_LW_Drv_Ops.p_Hal_GetStpPortState!=NULL)
				return g_LW_Drv_Ops.p_Hal_GetStpPortState(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_STP_PORTSTATE:
			if(g_LW_Drv_Ops.p_Hal_SetStpPortState!=NULL)
				return g_LW_Drv_Ops.p_Hal_SetStpPortState(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        #ifdef CONFIG_L2_HANDLE      
        case DRV_CMD_L2_SEND:
			if(g_LW_Drv_Ops.p_Hal_L2send!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_L2send((unsigned char*)pstDrvReq->pmsg,pstDrvReq->p_msg_len,&(pstDrvReq->l2_op));
			else
				return DRV_NULL_POINTER;
        #endif
        case DRV_CMD_SET_INTER_LOOPBACK:
			if(g_LW_Drv_Ops.p_Hal_SetInternalLoopback!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetInternalLoopback(pstDrvReq->p_lport,pstDrvReq->p_enable);
			else
				return DRV_NULL_POINTER;
        /* gpio */
        case DRV_CMD_GPIO_GET:
			if(g_LW_Drv_Ops.p_Hal_GpioValGet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioValGet(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioOutputSet!=NULL)
    	        return g_LW_Drv_Ops.p_Hal_GpioOutputSet(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GPIO_MULTIPLEX_GET:
			if(g_LW_Drv_Ops.p_Hal_GpioMultiplexGet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioMultiplexGet(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_MULTIPLEX_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioMultiplexSet!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_GpioMultiplexSet(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GPIO_DIR_IN_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioDirInSet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioDirInSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GPIO_DIR_OUT_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioDirOutSet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioDirOutSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_MODE_LEVEL_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioTrigModeLevelSet!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_GpioTrigModeLevelSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_GPIO_MODE_EDGE_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioTrigModeEdgeSet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioTrigModeEdgeSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_INT_LEVEL_HIGH_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioIntLevelHighSet!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_GpioIntLevelHighSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_INT_LEVEL_LOW_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioIntLevelLowSet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GpioIntLevelLowSet(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_GPIO_INT_EDGE_SET:
			if(g_LW_Drv_Ops.p_Hal_GpioIntEdgeSet!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_GpioIntEdgeSet(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
			
        case DRV_CMD_GET_PON_STATUS_ERR_FLAG:
            pstDrvReq->para1_u.uiValue = g_uiIfPonStateErr;
            return DRV_OK;
        case DRV_CMD_SET_PON_STATUS_ERR_FLAG:
            if ((TRUE != pstDrvReq->para1_u.uiValue) && (FALSE != pstDrvReq->para1_u.uiValue))
            {
                return DRV_ERR_PARA;
            }
            g_uiIfPonStateErr = pstDrvReq->para1_u.uiValue;
            return DRV_OK;
        case DRV_CMD_GET_PON_MODE_OFF_FLAG:
            pstDrvReq->para1_u.uiValue = g_uiIfPonModeOff;
            return DRV_OK;
        case DRV_CMD_SET_PON_MODE_OFF_FLAG:
            if ((TRUE != pstDrvReq->para1_u.uiValue) && (FALSE != pstDrvReq->para1_u.uiValue))
            {
                return DRV_ERR_PARA;
            }
            g_uiIfPonModeOff = pstDrvReq->para1_u.uiValue;
            return DRV_OK;
		/*Begin add by huangmingjian 2014/02/23*/ 
        case DRV_CMD_GET_OAM_DISCOVERY_STATE:
            pstDrvReq->para1_u.uiValue = g_oam_discovery_state;			
            return DRV_OK;
        case DRV_CMD_SET_OAM_DISCOVERY_STATE:
            g_oam_discovery_state = pstDrvReq->para1_u.uiValue;
            return DRV_OK;   
		/*End add by huangmingjian 2014/02/23*/ 
		/*Begin add by huangmingjian 2014/04/04 for Bug 500*/ 
        case DRV_CMD_GET_MAC_WRITE_FLAG:
            pstDrvReq->para1_u.uiValue = g_mac_write_flag;	
            return DRV_OK;
        case DRV_CMD_SET_MAC_WRITE_FLAG:
            g_mac_write_flag = pstDrvReq->para1_u.uiValue;
            return DRV_OK;   
		/*End add by huangmingjian 2014/04/04 for Bug 500*/ 
        case DRV_CMD_GET_PON_INT_CHANGE_FLAG:
            pstDrvReq->para1_u.uiValue = g_uiIfPonIntChangeErr;			
            return DRV_OK;
        case DRV_CMD_SET_PON_INT_CHANGE_FLAG:
            if ((TRUE != pstDrvReq->para1_u.uiValue) && (FALSE != pstDrvReq->para1_u.uiValue))
            {
                return DRV_ERR_PARA;
            }
            g_uiIfPonIntChangeErr = (unsigned char)(pstDrvReq->para1_u.uiValue);
			
			extern DRV_RET_E Hal_EnablePonGpio(void);
			if(g_uiIfPonIntChangeErr==FALSE)
			{
				Hal_EnablePonGpio();
			}
            return DRV_OK;        
        case DRV_CMD_SET_SYS_LED:
            
            return DRV_OK; 
        case DRV_CMD_OPL_INTREG_FIELD_READ:
			if(g_LW_Drv_Ops.p_Hal_CpuRegRead!=NULL)
			{
            	(void)g_LW_Drv_Ops.p_Hal_CpuRegRead(BSP_GIMR0_0, &uiRegValTmp);
            	//pstDrvReq->para2_u.uiValue = (uiRegValTmp & (1U << (pstDrvReq->para1_u.uiValue))) >> (pstDrvReq->para1_u.uiValue);
            	pstDrvReq->para2_u.uiValue = (uiRegValTmp & (1U << BSP_CUP1_T_CPU0_IRQ)) >> BSP_CUP1_T_CPU0_IRQ;
			}
            return DRV_OK;
        case DRV_CMD_OPL_INTREG_FIELD_WRITE:
            if ((TRUE != pstDrvReq->para2_u.uiValue) && (FALSE != pstDrvReq->para2_u.uiValue))
            {
                return DRV_ERR_PARA;
            }
			
			if((g_LW_Drv_Ops.p_Hal_CpuRegRead!=NULL) && (g_LW_Drv_Ops.p_Hal_CpuRegWrite!=NULL))
			{
	            (void)g_LW_Drv_Ops.p_Hal_CpuRegRead(BSP_GIMR0_0, &uiRegValTmp);

	        	if (pstDrvReq->para2_u.uiValue)
	        	{
	        		//uiRegValTmp |= (1U << (pstDrvReq->para1_u.uiValue));
	        		uiRegValTmp |= (1U << BSP_CUP1_T_CPU0_IRQ);
	                (void)g_LW_Drv_Ops.p_Hal_CpuRegWrite(BSP_GIMR0_0, uiRegValTmp);
	        	}
	        	else
	        	{
	        		//uiRegValTmp &= ~(1U << (pstDrvReq->para1_u.uiValue));
	        		uiRegValTmp &= ~(1U << BSP_CUP1_T_CPU0_IRQ);
	                (void)g_LW_Drv_Ops.p_Hal_CpuRegWrite(BSP_GIMR0_0, uiRegValTmp);
	        	}
			}
            return DRV_OK;
        case DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD_BY_ID:
			if(g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemByIndx!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemByIndx(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL_BY_ID:
			if(g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemByIndx!=NULL)
            	return g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemByIndx(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD:
			if(g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemb!=NULL)
	            return g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemb(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL:
			if(g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemb!=NULL)
	            return g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemb(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_BASE_VLAN_ENTY_CLR:
			if(g_LW_Drv_Ops.p_Hal_ClrPortBaseVlanEntryByVid!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_ClrPortBaseVlanEntryByVid(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_CTC_MC_VLAN_ADD:
			if(g_LW_Drv_Ops.p_Hal_SetMcVlanMemberAdd!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetMcVlanMemberAdd(pstDrvReq->para1_u.uiValue, 
                pstDrvReq->para2_u.lgcMask, pstDrvReq->para3_u.lgcMask);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_MC_MAC_GET:
            if(g_LW_Drv_Ops.p_Hal_GetMcMacByMacAndFid!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GetMcMacByMacAndFid(pstDrvReq->para1_u.uiValue, \
                                           pstDrvReq->para3_u.mac_address, \
                                           &(pstDrvReq->para2_u.stMacMcast));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_MC_MAC_SET:
            if(g_LW_Drv_Ops.p_Hal_SetMcMacByMacAndFid!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetMcMacByMacAndFid(pstDrvReq->para2_u.stMacMcast);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_MC_MAC_DEL:
            if(g_LW_Drv_Ops.p_Hal_DelMcMacByMacAndFid!=NULL)
	            return g_LW_Drv_Ops.p_Hal_DelMcMacByMacAndFid(pstDrvReq->para2_u.stMacMcast);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_MC_VLAN_MEM_RMV:
			if(g_LW_Drv_Ops.p_Hal_SetMcVlanMemberRmv!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetMcVlanMemberRmv(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.lgcMask);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_CTC_MC_VLAN_MEM_GET:
			if(g_LW_Drv_Ops.p_Hal_GetMcVlanMem!=NULL)
				return g_LW_Drv_Ops.p_Hal_GetMcVlanMem(pstDrvReq->para1_u.uiValue, \
	                                       &(pstDrvReq->para2_u.lgcMask), \
	                                       &(pstDrvReq->para3_u.lgcMask));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_CREATE:            
			if(g_LW_Drv_Ops.p_Hal_SetPortTransparentMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_SetPortTransparentMode(pstDrvReq->p_lport, pstDrvReq->para2_u.pstCtcVlanCfg);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_DELETE:
			if(g_LW_Drv_Ops.p_Hal_ResetPortTransparentMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_ResetPortTransparentMode(pstDrvReq->p_lport);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TAG_VLAN_CREATE:
			if(g_LW_Drv_Ops.p_Hal_SetPortTagMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_SetPortTagMode(pstDrvReq->p_lport, pstDrvReq->para2_u.pstCtcVlanCfg);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TAG_VLAN_DELETE:
			if(g_LW_Drv_Ops.p_Hal_ResetPortTagMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_ResetPortTagMode(pstDrvReq->p_lport);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_CREATE:
			if(g_LW_Drv_Ops.p_Hal_SetPortTranslationMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_SetPortTranslationMode(pstDrvReq->p_lport, pstDrvReq->para2_u.pstCtcVlanCfg);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_DELETE:
			if(g_LW_Drv_Ops.p_Hal_ResetPortTranslationMode!=NULL)
				return g_LW_Drv_Ops.p_Hal_ResetPortTranslationMode(pstDrvReq->p_lport);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_MC_VLAN_CREATE:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanCreate!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanCreate(pstDrvReq->para1_u.uiValue,pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_ACL_FOR_MC_VLAN_DELETE:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanDelete!=NULL)
    	        return g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanDelete(pstDrvReq->para1_u.uiValue,pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;        
        case DRV_CMD_CTC_PORT_INGRESS_VLAN_FILTER:
			if(g_LW_Drv_Ops.p_Hal_SetPortVlanIngressFilter!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetPortVlanIngressFilter(pstDrvReq->p_lport, (BOOL)(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_INGRESS_VLAN_RECIEVE:
			if(g_LW_Drv_Ops.p_Hal_SetPortVlanIngressMode!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_SetPortVlanIngressMode(pstDrvReq->p_lport, (PORT_INGRESS_MODE_E)(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_PORT_EGRESS_VLAN_FILTER:
			if(g_LW_Drv_Ops.p_Hal_SetPortVlanEgressMode!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetPortVlanEgressMode(pstDrvReq->para1_u.uiValue, (PORT_EGRESS_MODE_E)(pstDrvReq->para2_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_GET_EMPTY_ACL_RULE_NUM:
			if(g_LW_Drv_Ops.p_Hal_AclRuleEmptyNumGet!=NULL)
        	    return g_LW_Drv_Ops.p_Hal_AclRuleEmptyNumGet(&(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_GET_PORT_VLAN_CFG:
			if(g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanGet!=NULL)
	            return g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanGet(pstDrvReq->p_lport, pstDrvReq->para2_u.pstCtcVlanCfg);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CTC_SET_PORT_VLAN_CFG:
			if(g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanSet!=NULL)
				return g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanSet(pstDrvReq->p_lport, pstDrvReq->para2_u.pstCtcVlanCfg);
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_CTC_RESTART_AUTONEG:
			if(g_LW_Drv_Ops.p_Hal_RestartPortNeg!=NULL)
				return g_LW_Drv_Ops.p_Hal_RestartPortNeg(pstDrvReq->p_lport);
			else
				return DRV_NULL_POINTER;			
        case DRV_CMD_CTC_ENOUGH_VLAN_INDEX_CHECK:
			if(g_LW_Drv_Ops.p_Hal_CtcVlanEnoughVlanIdxEntryCheck!=NULL)
	            return g_LW_Drv_Ops.p_Hal_CtcVlanEnoughVlanIdxEntryCheck(pstDrvReq->para2_u.pstCtcVlanCfg, &(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_IGMP_UNKNOW_IP4_ACT:
			if(g_LW_Drv_Ops.p_Hal_SetUnKnowIp4McastAct!=NULL)
	            return g_LW_Drv_Ops.p_Hal_SetUnKnowIp4McastAct((MCAST_UNKNOW_ACT_E)pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_FDB_ENTRY_GET:
            if(g_LW_Drv_Ops.p_Hal_GetFdbEntryByIndex!=NULL)
	            return g_LW_Drv_Ops.p_Hal_GetFdbEntryByIndex(pstDrvReq->para1_u.uiValue, \
                                          &(pstDrvReq->para3_u.uiValue), \
                                          &(pstDrvReq->para2_u.stMacCommon));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_CVLAN_ENTRY_GET:
			if(g_LW_Drv_Ops.p_Hal_GetCvlanEntry!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetCvlanEntry(pstDrvReq->para1_u.uiValue, \
                                     &(pstDrvReq->para2_u.uiValue), \
                                     &(pstDrvReq->para3_u.uiValue), \
                                     &(pstDrvReq->para4_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_SVLAN_ENTRY_GET:
			if(g_LW_Drv_Ops.p_Hal_GetSvlanEntry!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetSvlanEntry(pstDrvReq->para1_u.uiValue, \
                                     &(pstDrvReq->para2_u.uiValue), \
                                     &(pstDrvReq->para3_u.uiValue), \
                                     &(pstDrvReq->para4_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_C2S_ENTRY_GET:
			if(g_LW_Drv_Ops.p_Hal_GetC2sEntryByIndex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetC2sEntryByIndex(pstDrvReq->para1_u.uiValue, \
                                          &(pstDrvReq->para2_u.uiValue), \
                                          &(pstDrvReq->para3_u.uiValue), \
                                          &(pstDrvReq->para4_u.uiValue));
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_S2C_ENTRY_GET:
			if(g_LW_Drv_Ops.p_Hal_GetSp2cEntryByIndex!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetSp2cEntryByIndex(pstDrvReq->para1_u.uiValue, \
                                          &(pstDrvReq->para2_u.uiValue), \
                                          &(pstDrvReq->para3_u.uiValue), \
                                          &(pstDrvReq->para4_u.uiValue));
			else
				return DRV_NULL_POINTER;
        #if 0
        case DRV_CMD_SINGLE_DBG_REG_GET:
            return (DRV_RET_E)single_getDbgReg(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.uiValue));
        case DRV_CMD_SINGLE_DBG_REG_SET:
            return (DRV_RET_E)single_setDbgReg(pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.uiValue);
        #endif     
		case DRV_CMD_PORTS_INIT:
			if(g_LW_Drv_Ops.p_Hal_PortInit!=NULL)
            	return g_LW_Drv_Ops.p_Hal_PortInit();
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_MANAGE_VLAN:
            if(g_LW_Drv_Ops.p_Hal_SetManageVlan!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetManageVlan(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
		/*Begin add by shipeng 2013-11-11*/
		case DRV_CMD_SET_WIRELESS_UP_SERVICE_VLAN:
            if(g_LW_Drv_Ops.p_Hal_SetWirelessUpServiceVlan!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetWirelessUpServiceVlan(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
		/*End add by shipeng 2013-11-11*/
        /*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/            
		case DRV_CMD_ACL_DROP_LOOPD_CREATE_ADD_PORT:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation!=NULL)
    	        return g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation(pstDrvReq->p_lport, ACL_DROPLOOPD_ADD_PORT); 
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_ACL_DROP_LOOPD_CREATE_REMOVE_PORT:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation(pstDrvReq->p_lport, ACL_DROPLOOPD_REMOVE_PORT);
			else
				return DRV_NULL_POINTER;
        case DRV_CMD_ACL_DROP_LOOPD_DELETE:
			if(g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdRemove!=NULL)
            	return g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdRemove();  
			else
				return DRV_NULL_POINTER;
        /*end added by liaohongjun 2012/11/30 of EPN104QID0084*/
		/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
		case DRV_CMD_SET_LASERON_TEST_FLAG:
            if ((LASEON_TEST_ENABLE != pstDrvReq->para1_u.uiValue) && (LASEON_TEST_DISABLE != pstDrvReq->para1_u.uiValue))
            {
                return DRV_ERR_PARA;
            }
            g_uiLaserOnTest_Flag = pstDrvReq->para1_u.uiValue;
            return DRV_OK;
        case DRV_CMD_GET_LASERON_TEST_FLAG: 
            pstDrvReq->para1_u.uiValue = g_uiLaserOnTest_Flag;
            return DRV_OK;
		/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
		/*Begin add by shipeng 2013/05/27 for CTC*/ 
		case DRV_CMD_SET_FECMODE:
			if(g_LW_Drv_Ops.p_Hal_SetFecMode!=NULL)
            	return g_LW_Drv_Ops.p_Hal_SetFecMode(pstDrvReq->para1_u.uiValue);
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_GET_FECMODE:
			if(g_LW_Drv_Ops.p_Hal_GetFecMode!=NULL)
            	return g_LW_Drv_Ops.p_Hal_GetFecMode(&(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
		/*End add by shipeng 2013/05/27 for CTC*/ 
		/*begin add by sunmingliang for packet droped*/
		case DRV_CMD_ACL_GET_EMPTY_ENTRY:
			if(g_LW_Drv_Ops.p_Hal_AclRuleEmptyIdGet!=NULL)
				return g_LW_Drv_Ops.p_Hal_AclRuleEmptyIdGet(&(pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_ACL_DELETE_BY_ID:
			if(g_LW_Drv_Ops.p_ACL_DeleteRuleByAclid!=NULL)
				return g_LW_Drv_Ops.p_ACL_DeleteRuleByAclid((pstDrvReq->para1_u.uiValue));
			else
				return DRV_NULL_POINTER;
		/*end add by sunmingliang for packet droped*/	
		case DRV_CMD_ADD_ACL_VLAN_INTF:
			if(g_LW_Drv_Ops.p_Hal_VlanInterfaceAdd!=NULL)
			{
				return g_LW_Drv_Ops.p_Hal_VlanInterfaceAdd((vlan_id_t)(pstDrvReq->para1_u.uiValue), pstDrvReq->para2_u.mac_address);
			}
			else
				return DRV_NULL_POINTER;
	    case DRV_CMD_DEL_ACL_VLAN_INTF:
			if(g_LW_Drv_Ops.p_Hal_VlanInterfaceDelete!=NULL)
			{
				return g_LW_Drv_Ops.p_Hal_VlanInterfaceDelete((vlan_id_t)(pstDrvReq->para1_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_ADDS2C_BY_PORT:
			if(g_LW_Drv_Ops.p_Hal_AddSp2cEntry2Port)
			{
				return g_LW_Drv_Ops.p_Hal_AddSp2cEntry2Port(pstDrvReq->para1_u.lgcPort,pstDrvReq->para2_u.uiValue,pstDrvReq->para3_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_DELS2C_BY_PORT:
			if(g_LW_Drv_Ops.p_Hal_DelSp2cEntryFromPort)
			{
				return g_LW_Drv_Ops.p_Hal_DelSp2cEntryFromPort(pstDrvReq->para1_u.lgcPort,pstDrvReq->para2_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_ADDC2S_BY_PORT:
			if(g_LW_Drv_Ops.p_Hal_AddC2sEntry2Port)
			{
				return g_LW_Drv_Ops.p_Hal_AddC2sEntry2Port(pstDrvReq->para1_u.lgcPort,pstDrvReq->para2_u.uiValue,pstDrvReq->para3_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_DELC2S_BY_PORT:
			if(g_LW_Drv_Ops.p_Hal_DelC2sEntryFromPort)
			{
				return g_LW_Drv_Ops.p_Hal_DelC2sEntryFromPort(pstDrvReq->para1_u.lgcPort,pstDrvReq->para2_u.uiValue,pstDrvReq->para3_u.uiValue);
			}
			else
			{
				return DRV_NULL_POINTER;
			}
		case DRV_CMD_ADDTRSLMC_MBR:
			if(g_LW_Drv_Ops.p_Hal_SetMcTransltVlanMemberAdd)
			{
				return g_LW_Drv_Ops.p_Hal_SetMcTransltVlanMemberAdd(pstDrvReq->para1_u.uiValue,pstDrvReq->para4_u.uiValue,pstDrvReq->para2_u.lgcMask,pstDrvReq->para3_u.lgcMask,pstDrvReq->para5_u.lgcMask);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_DELTRSLMC_MBR:
			if(g_LW_Drv_Ops.p_Hal_SetMctransltVlanMemberRmv)
			{
				return g_LW_Drv_Ops.p_Hal_SetMctransltVlanMemberRmv(pstDrvReq->para1_u.uiValue,pstDrvReq->para2_u.uiValue,pstDrvReq->para3_u.lgcMask);
			}
			else
				return DRV_NULL_POINTER;
		/*Begin add by huangmingjian 2013-08-27*/
	    case DRV_CMD_GET_EPON_MIB_COUNTER:
			if(g_LW_Drv_Ops.p_Hal_GetEponMibCounter!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetEponMibCounter(pstDrvReq->para2_u.Counter);
			}
			else
				return DRV_NULL_POINTER;
	    case DRV_CMD_EPON_MIB_GLOBAL_RESET:
			if(g_LW_Drv_Ops.p_Hal_EponMibGlobal_Reset!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_EponMibGlobal_Reset();
			}
			else
				return DRV_NULL_POINTER;
	    case DRV_CMD_GET_PONMAC_TRANSCEIVER:
			if(g_LW_Drv_Ops.p_Hal_GetPonmacTransceiver!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetPonmacTransceiver((rtk_transceiver_parameter_type_app_t)pstDrvReq->para1_u.uiValue, pstDrvReq->para2_u.pData);
			}
			else
				return DRV_NULL_POINTER;			
		/*End add by huangmingjian 2013-08-27*/
		/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
	case DRV_CMD_GET_PONFTOVERFLAG:
			if(g_LW_Drv_Ops.p_Hal_GetPonftoverflag!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetPonftoverflag(&(pstDrvReq->para1_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_PONFTOVERFLAG:
			if(g_LW_Drv_Ops.p_Hal_SetPonftoverflag!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_SetPonftoverflag(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_SET_LASER:
			if(g_LW_Drv_Ops.p_Hal_SetLaser!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_SetLaser(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_GET_LASER:
			if(g_LW_Drv_Ops.p_Hal_GetLaser!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetLaser(&(pstDrvReq->para1_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_GET_APCSET:
			if(g_LW_Drv_Ops.p_Hal_GetApcset!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetApcset(&(pstDrvReq->para1_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;	
		case DRV_CMD_SET_APCSET:
			if(g_LW_Drv_Ops.p_Hal_SetApcset!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_SetApcset(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_GET_IMODSET:
			if(g_LW_Drv_Ops.p_Hal_GetImodset!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_GetImodset(&(pstDrvReq->para1_u.uiValue));
			}
			else
				return DRV_NULL_POINTER;	
		case DRV_CMD_SET_IMODSET:
			if(g_LW_Drv_Ops.p_Hal_SetImodset!=NULL)
			{	
				return g_LW_Drv_Ops.p_Hal_SetImodset(pstDrvReq->para1_u.uiValue);
			}
			else
				return DRV_NULL_POINTER;				
#endif
		/*Begin add by huangmingjian 2014-01-13*/

		
		case DRV_CMD_ADD_CTC_CLF_PRI_TO_QUEUE_BYPORT:
			if(g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueCreate!=NULL)
            	return g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueCreate(pstDrvReq->p_lport, \
                                                  pstDrvReq->para2_u.uiValue, \
                                                  (void *)&pstDrvReq->para3_u.uiValue);
			else
				return DRV_NULL_POINTER;
		case DRV_CMD_DEL_CTC_CLF_PRI_TO_QUEUE_BYPREC:
			if(g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueDelete!=NULL)
            	return g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueDelete(pstDrvReq->p_lport, \
                                                  pstDrvReq->para2_u.uiValue);
			else
				return DRV_NULL_POINTER;
    	case DRV_CMD_CLEAR_CTC_CLF_PRI_TO_QUEUE_BYPORT:
			if(g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueClear!=NULL)
            	return g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueClear(pstDrvReq->p_lport);  
       		else
				return DRV_NULL_POINTER;
			
		case DRV_CMD_LOOKUP_MISS_FLOOD_SET:
			if(g_LW_Drv_Ops.p_Hal_LookupMissFloodPortMaskSet!=NULL)
				return g_LW_Drv_Ops.p_Hal_LookupMissFloodPortMaskSet(pstDrvReq->para1_u.uiValue, &(pstDrvReq->para2_u.lgcMask));	
			else
				return DRV_NULL_POINTER;
        default:
            break;
    }

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif


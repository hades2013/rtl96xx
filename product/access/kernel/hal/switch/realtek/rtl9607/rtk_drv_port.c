/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <linux/kernel.h>
#include <linux/delay.h>
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>
//#include <rtk/port.h>
//#include <rtk/switch.h>
//#include <rtk/qos.h>
//#include <rtk/flowctrl.h>
#include "hal_common.h"
//#include <common/rt_error.h>
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#include <common/error.h>
#include <common/rt_error.h>
#include <hal/phy/phydef.h>
#include <hal/common/halctrl.h>
#include <rtk/port.h>
#include <rtk/qos.h> 
#include <rtk/switch.h> 
#include <rtk/led.h>
#include <linux/kthread.h>




//#include <common/debug/rt_log.h>
//typedef unsigned int   uint32;
//typedef int             int32;

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

uint32 spdlx_save[LOGIC_PORT_NO+1];
logic_pmask_t isolate_save;
static rtk_enable_t flowcontrol_save[LOGIC_PORT_NO+1];

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
static spinlock_t _spin_hal_port_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
#define RTK_MAX_PACKET_LEN      16000
#define RTK_NORMAL_PACKET_LEN   1522

#define SPIN_HAL_PORT_LOCK       spin_lock(&_spin_hal_port_lock)
#define SPIN_HAL_PORT_UNLOCK     spin_unlock(&_spin_hal_port_lock)

//#define RTK_PORT_DEBUG_SUPPORT
#ifdef RTK_PORT_DEBUG_SUPPORT
#define RTK_PORT_DEBUG rt_log_printf
#else
#define RTK_PORT_DEBUG(fmt,arg...)
#endif




/*****************************************************************************
    Func Name: Hal_GetPortLinkupType
  Description: 获取当前端口linkup的类型
        Input: phyid_t phyid  
                UINT32 *pulLinkupType      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
*****************************************************************************/
DRV_RET_E Hal_GetPortLinkupType(phyid_t phyid, UINT32 *pulLinkupType)
{
    int32 iRv = RT_ERR_OK;     
    rtk_port_linkStatus_t enStatus;    
    rtk_port_media_t enMedia;  
 
    UINT32 uiChipId = 0;    
    UINT32 uiPortId = 0;
    
    
    if(NULL == pulLinkupType)
    {
        return DRV_ERR_PARA;
    }    
    
    uiChipId = CHIPID(phyid);
    uiPortId = PORTID(phyid);
    iRv=rtk_port_link_get(uiPortId, &enStatus);   
    if((RT_ERR_OK != iRv) || (PORT_LINKUP!=enStatus))
    {
        *pulLinkupType = HAL_PORT_LINKUP_TYPE_DOWN;
        return DRV_OK;
    }
	*pulLinkupType = HAL_PORT_LINKUP_TYPE_COPPER_UP;
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortPriority
  Description: 设置端口优先级,范围1-7
        Input: port_num_t lgcPort  
                UINT32 uiPrio       
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortPriority(port_num_t lgcPort, UINT32 uiPrio)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if (!ISVALID_PORT_PRIORITY(uiPrio))
    {
        return DRV_INVALID_PRIORITY_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
   
    iRv = rtk_qos_portPri_set(PORTID(phyid), uiPrio);
   
    return ErrorSdk2Drv(iRv);
}


/*****************************************************************************
    Func Name: Hal_SetPortMdix
  Description: 设置端口的MDI
        Input: port_num_t lgcPort  
                UINT32 uiMdix       
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortMdix(port_num_t lgcPort, UINT32 uiMdix)
{
   
    phyid_t phyid = 0;    
    rtk_port_crossOver_mode_t rtkMdix = PORT_CROSSOVER_MODE_AUTO;
   
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    (void)uiMdix;
    
    switch (uiMdix)
    {
        case PORT_MDI_AUTO:
            rtkMdix = PORT_CROSSOVER_MODE_AUTO;
            break;
        case PORT_MDI_ACROSS:
            rtkMdix = PORT_CROSSOVER_MODE_MDIX;
            break;
        case PORT_MDI_NORMAL:
            rtkMdix = PORT_CROSSOVER_MODE_MDI;
            break;
        default:
            return DRV_INVALID_MDI_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
    
    iRv = rtk_port_phyCrossOverMode_set(PORTID(phyid), rtkMdix);
       
    return ErrorSdk2Drv(iRv);

}


/*****************************************************************************
    Func Name: Hal_SetPortEnable
  Description: 设置端口的admin,
        Input: port_num_t lgcPort  
                UINT32 uiEnable     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortEnable(port_num_t lgcPort, UINT32 uiEnable)
{
    phyid_t phyid = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    if(!ISVALID_ADMIN_VALUE(uiEnable))
    {
        return DRV_INVALID_ADMIN_VALUE;
    }

    switch (uiEnable)
    {
        case PORT_ADMIN_DISABLE:
            enable = DISABLED;
            break;
        case PORT_ADMIN_ENABLE:
            enable = ENABLED;
            break;
        default:
            return DRV_INVALID_ADMIN_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
   
    iRv = rtk_port_adminEnable_set(PORTID(phyid), enable);
   
    return ErrorSdk2Drv(iRv);

}


/*****************************************************************************
    Func Name: Hal_GetPortEnable
  Description: 获取端口的admin
        Author:liaohongjun
        Input: port_num_t lgcPort  
                UINT32 *uiEnable     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
   added by liaohongjun 2012/12/10 of EPN104QID0086                                                                         
*****************************************************************************/
DRV_RET_E Hal_GetPortEnable(port_num_t lgcPort, UINT32 *uiEnable)
{
    phyid_t phyid = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
   
   if(NULL == uiEnable)
   {
       return DRV_ERR_PARA;
   }


    phyid = PortLogic2PhyID(lgcPort);
   
   
    iRv = rtk_port_adminEnable_get(PORTID(phyid), &enable);
   
    *uiEnable = enable;
    return ErrorSdk2Drv(iRv);
}







/*****************************************************************************
    Func Name: Hal_SetPortAbility
  Description: 设置协商时端口的能力位
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAbility    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortAbility(port_num_t lgcPort, UINT32 uiAbility, UINT32 uiPortType)
{
    phyid_t phyid = 0;
   
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;    
    rtk_port_phy_ability_t rtkAbility;    
   
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }   
    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
   
    memset(&rtkAbility, 0x00, sizeof(rtk_port_phy_ability_t));  
   
    phyid = PortLogic2PhyID(lgcPort);    
    uiPortId = PORTID(phyid);   
    if(uiAbility & PORT_ABIL_10MB_HD){
        rtkAbility.Half_10 = 1;
    }
    else{
        rtkAbility.Half_10 = 0;
    }
    
    if(uiAbility & PORT_ABIL_10MB_FD){
        rtkAbility.Full_10 = 1;
    }
    else{
        rtkAbility.Full_10 = 0; 
    }
    
    if(uiAbility & PORT_ABIL_100MB_HD){
        rtkAbility.Half_100 = 1;
    }
    else{
        rtkAbility.Half_100 = 0; 
    }
    
    if(uiAbility & PORT_ABIL_100MB_FD){
        rtkAbility.Full_100 = 1; 
    }
    else{
        rtkAbility.Full_100 = 0;  
    }
    
    if(uiAbility & PORT_ABIL_1000MB_HD){
        rtkAbility.Half_1000 = 1;
    }
    else{
        rtkAbility.Half_1000 = 0;
    }
    
    if(uiAbility & PORT_ABIL_1000MB_FD){
        rtkAbility.Full_1000 = 1;
    }
    else{
        rtkAbility.Full_1000 = 0;
    }

    /*PHY Reg 4 bit 10*/
    if(uiAbility & PORT_ABIL_PAUSE_TX){
        rtkAbility.FC = 1;
    }
    else{
        rtkAbility.FC = 0;
    }

    /*PHY Reg 4 bit 11*/
    if(uiAbility & PORT_ABIL_PAUSE_RX){
        rtkAbility.AsyFC = 1;
    }
    else{
        rtkAbility.AsyFC = 0; 
    }
    
    iRv = rtk_port_phyAutoNegoAbility_set(uiPortId, &rtkAbility);
 
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortAutoneg
  Description: 设置开启/关闭端口的自协商
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAuton      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortAutoneg(port_num_t lgcPort, UINT32 uiAuton, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_enable_t enabled = ENABLED;
        
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
   
    switch (uiAuton)
    {
        case TRUE:
            enabled = ENABLED;
            break;
        case FALSE:
            enabled = DISABLED;
            break;
        default:
            return DRV_INVALID_STATE;
    }
    
    
    phyid = PortLogic2PhyID(lgcPort);
   
    iRv = rtk_port_phyAutoNegoEnable_set(PORTID(phyid), enabled);   
    
    return ErrorSdk2Drv(iRv);
}

/*Begin add by shipeng 2013/05/27 for CTC*/ 
/*****************************************************************************
    Func Name: Hal_GetPortAutoneg
  Description: 获取端口的自协商状态
        Input: port_num_t lgcPort   
                UINT32 *uiAuton  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortAutoneg(port_num_t lgcPort, UINT32 *uiAuton)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
	rtk_enable_t Status;	
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(uiAuton == NULL)
    {
        return DRV_ERR_PARA;
    }
	
    phyid = PortLogic2PhyID(lgcPort);
   
	iRv=rtk_port_phyAutoNegoEnable_get(PORTID(phyid),&Status);
    if(iRv == RT_ERR_OK)
    {        
        if (Status)
        *uiAuton = ENABLE;
        else
        *uiAuton = DISABLE;
    }
    
    return ErrorSdk2Drv(iRv);
}
/*End add by shipeng 2013/05/27 for CTC*/ 

/*****************************************************************************
    Func Name: Hal_SetPortSpeed
  Description: 设置端口的速率
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiSpeed      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortSpeed(port_num_t lgcPort, UINT32 uiSpeed, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    rtk_port_t uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    
    rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
	rtk_port_duplex_t   rtkDuplex;
    rtk_enable_t        rtkFlowControl;
    
    rtk_port_phy_ability_t Ability;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
    
   
    
    phyid = PortLogic2PhyID(lgcPort);
  
    uiPortId = PORTID(phyid);
   
    memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
    iRv = rtk_port_phyForceModeAbility_get(uiPortId,&rtkSpeed,&rtkDuplex,&rtkFlowControl);
	if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }   
    switch (uiSpeed)
    {
        case PORT_SPEED_10:
            rtkSpeed = PORT_SPEED_10M;
            break;
        case PORT_SPEED_100:
            rtkSpeed = PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000:
            rtkSpeed = PORT_SPEED_1000M;
            break;
        default:
            return DRV_INVALID_SPEED_VALUE;
    }
    
   
     
    iRv = rtk_port_phyForceModeAbility_set(uiPortId,rtkSpeed,rtkDuplex,rtkFlowControl);
   
    return ErrorSdk2Drv(iRv);
}



/*****************************************************************************
 函 数 名  : Hal_SetPortSpdlx
 功能描述  : 设置端口的速率双工
 输入参数  : port_num_t lgcPort  
             UINT32 uiDuplex     
             UINT32 uiPortType   
 输出参数  : 无
 返 回 值  : 

 Author: 
*****************************************************************************/
/*begin modified by wanghuanyu for 198*/
DRV_RET_E Hal_SetPortSpdlx(port_num_t lgcPort, UINT32 uiSpdlx)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_port_phy_ability_t Ability;
	rtk_port_macAbility_t portability;
    int powerdown = 0;
    uint32_t val32=0;
    rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
	rtk_port_duplex_t   rtkDuplex;
    rtk_enable_t        rtkFlowControl;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    spdlx_save[lgcPort] = uiSpdlx; /*for Hal_GetSpdlxSet*/

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

	if(uiPortId == 5)
	{
		/*Cable Port , So Do Nothing*/
		return DRV_OK;
	}
	
    /*set auto type*/
    if((PORT_10AUTO == uiSpdlx) || (PORT_100AUTO == uiSpdlx) || (PORT_1000AUTO == uiSpdlx) \
       || (PORT_AUTOHALF == uiSpdlx) || (PORT_AUTOFULL == uiSpdlx) \
       || (PORT_AUTOAUTO == uiSpdlx)){      
        memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
        Ability.FC = flowcontrol_save[lgcPort];
		Ability.AsyFC = flowcontrol_save[lgcPort];
        switch (uiSpdlx)
        {
            case PORT_10AUTO :
                Ability.Half_10 = 1;
                Ability.Full_10 = 1;
                break;
            case PORT_100AUTO :
                Ability.Half_100 = 1;
                Ability.Full_100 = 1;
                break;
            case PORT_1000AUTO :
                 Ability.Full_1000 = 1;
                 Ability.Half_1000 = 1;
                 break;
            case PORT_AUTOHALF :
                Ability.Half_10 = 1;
                Ability.Half_100 = 1;
				Ability.Half_1000 = 1;
                break;
            case PORT_AUTOFULL :
                Ability.Full_10 = 1;
                Ability.Full_100 = 1;        
                Ability.Full_1000 = 1;
                break;
            case PORT_AUTOAUTO:
                Ability.Full_100 = 1;            
                Ability.Half_100 = 1;
                Ability.Full_10 = 1;
                Ability.Half_10 = 1;
                Ability.Full_1000 = 1;
                Ability.Half_1000 = 1;
                break;
            default:
                break;
        }

        iRv = rtk_port_phyAutoNegoAbility_set(uiPortId,&Ability);
		iRv = rtk_port_macForceAbilityState_set(uiPortId, DISABLED);
		iRv = rtk_port_phyAutoNegoEnable_set(uiPortId,ENABLED);   
        return ErrorSdk2Drv(iRv); 
    }
    
    /*set force type*/
	rtkFlowControl = flowcontrol_save[lgcPort];
    switch (uiSpdlx)
    {
        case PORT_10HALF :            
			rtkSpeed=PORT_SPEED_10M;
			rtkDuplex=PORT_HALF_DUPLEX;
			rtkFlowControl = DISABLED;/*if the port duplex = half and flowctl enable ,it should make epon offline*/
            break;
        case PORT_10FULL :           
			rtkDuplex=PORT_FULL_DUPLEX;
			rtkSpeed=PORT_SPEED_10M;
            break;
        case PORT_100HALF :       
			rtkSpeed=PORT_SPEED_100M;
			rtkDuplex=PORT_HALF_DUPLEX;
			rtkFlowControl = DISABLED;/*if the port duplex = half and flowctl enable ,it should make epon offline*/
            break;
        case PORT_100FULL :			
    		rtkDuplex=PORT_FULL_DUPLEX;
			rtkSpeed=PORT_SPEED_100M;
            break;
        case PORT_1000FULL:
       		rtkDuplex=PORT_FULL_DUPLEX;
			rtkSpeed=PORT_SPEED_1000M;
            break;
        default:
            return DRV_ERR_PARA;
    }	

	if(HAL_IS_PHY_EXIST(uiPortId))	
	{
		iRv = rtk_port_phyForceModeAbility_set(uiPortId,rtkSpeed,rtkDuplex,rtkFlowControl);
	}

	memset(&portability, 0x00, sizeof(rtk_port_macAbility_t));
	iRv = rtk_port_macForceAbility_get(uiPortId, &portability);	
	portability.speed = rtkSpeed;
	portability.duplex = rtkDuplex;
	portability.txFc = rtkFlowControl;
	portability.rxFc = rtkFlowControl;
	iRv = rtk_port_macForceAbility_set(uiPortId, portability);
	iRv = rtk_port_macForceAbilityState_set(uiPortId, ENABLED);

	if(HAL_IS_PHY_EXIST(uiPortId))	
	{
		iRv = rtk_port_phyAutoNegoEnable_set(uiPortId,DISABLED);
		iRv = rtk_port_phyReg_get(uiPortId, 0, PHY_REG_CONTROL, &val32);
		if(val32 & PowerDown_MASK)
	    {
	        powerdown = 1;
	    }
	    if(powerdown)
	    {
	        rtk_port_adminEnable_set(uiPortId, DISABLED);
	    }
	    else{
	        /*lint --e{155,62}  z03340 add*/
	        /*mdelay(500);*/
	        rtk_port_adminEnable_set(uiPortId, DISABLED);
	        mdelay(100);
	        rtk_port_adminEnable_set(uiPortId, ENABLED);
	    }
	}

    return ErrorSdk2Drv(iRv);
}
/*end modified by wanghuanyu for 198*/


/*****************************************************************************
 函 数 名  : Hal_SetPhyReg
 功能描述  : 设置phy 寄存器
 输入参数  : port_num_t lgcPort  
             UINT32 reg_no       
             UINT32 reg_val      
 输出参数  : 无
 返 回 值  : 
 
 *****************************************************************************/
DRV_RET_E Hal_SetPhyReg(port_num_t lgcPort, UINT32 reg_no, UINT32 reg_val)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_port_phyReg_set(uiPortId,0, (rtk_port_phy_reg_t)reg_no, reg_val);


    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_GetPhyReg
 功能描述  : 获取phy 寄存器
 输入参数  : port_num_t lgcPort  
             UINT32* pRegVal     
             UINT32 reg_no       
 输出参数  : 无
 返 回 值  : 

 *****************************************************************************/
DRV_RET_E Hal_GetPhyReg(port_num_t lgcPort, UINT32* pRegVal, UINT32 reg_no)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    UINT32 reg_val=0;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_port_phyReg_get(uiPortId,0, (rtk_port_phy_reg_t)reg_no, &reg_val);

    *pRegVal = reg_val;

    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_GetSpdlxSet
 功能描述  : 获取前期对spdlx的配置
 输入参数  : port_num_t lgcPort  
             UINT32* uiSpdlx     
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :

*****************************************************************************/
DRV_RET_E Hal_GetSpdlxSet(port_num_t lgcPort, UINT32* uiSpdlx)
{
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    *uiSpdlx = spdlx_save[lgcPort];

    return DRV_OK;
}


/*****************************************************************************
 函 数 名  : Hal_GetIsolateSet
 功能描述  : 获取前期对isolate的配置
 输入参数  : logic_pmask_t *pstPortMask  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :

*****************************************************************************/
DRV_RET_E Hal_GetIsolateSet(logic_pmask_t *pstPortMask)
{
    LgcMaskCopy(pstPortMask, &isolate_save);
    
    return DRV_OK;
}



/*****************************************************************************
    Func Name: Hal_SetPortDuplex
  Description: 设置端口的双工状态
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiDuplex     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

DRV_RET_E Hal_SetPortDuplex(port_num_t lgcPort, UINT32 uiDuplex, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_port_duplex_t rtkDuplex = PORT_HALF_DUPLEX;    
	rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
	//rtk_port_duplex_t   rtkDuplex;
    rtk_enable_t        rtkFlowControl;
    
    rtk_port_phy_ability_t Ability;
   
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }

   
    
    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }
    // memset(&Ability,0,sizeof(rtk_port_phy_ability_t));   
   iRv = rtk_port_phyForceModeAbility_get(uiPortId,&rtkSpeed,&rtkDuplex,&rtkFlowControl);
   
    switch (uiDuplex)
    {
        case PORT_DUPLEX_FULL:
            rtkDuplex = PORT_FULL_DUPLEX;
            break;
        case PORT_DUPLEX_HALF:
            rtkDuplex = PORT_HALF_DUPLEX;
            break;
        default:
            return DRV_INVALID_DUPLEX_VALUE;
    } 
 
    
	iRv = rtk_port_phyForceModeAbility_set(uiPortId,rtkSpeed,rtkDuplex,rtkFlowControl);
   
   
    return ErrorSdk2Drv(iRv);

}


/*****************************************************************************
    Func Name: Hal_SetPortPause
  Description: 设置端口的流控
        Input: port_num_t lgcPort  
                UINT32 uiPauseTx    
                UINT32 uiPauseRx    
       Output: 
       Return: 
      Caution: 流控设置时两个方向需要设置为相同值
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortFlowctrl(port_num_t lgcPort, UINT32 uiPauseTx, UINT32 uiPauseRx)
{
    phyid_t phyid = 0;
	UINT32 uiPortId = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
	rtk_enable_t auto_enable;
	int powerdown = 0;
    uint32_t val32=0;   
    rtk_port_phy_ability_t Ability;
	rtk_port_macAbility_t portability;
	rtk_port_duplex_t rtkDuplex = PORT_HALF_DUPLEX;    
	rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
    rtk_enable_t        rtkFlowControl;
	
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if((TRUE == uiPauseTx) && (TRUE == uiPauseRx))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

	flowcontrol_save[lgcPort] = enable;
	
    phyid = PortLogic2PhyID(lgcPort);
	uiPortId = PORTID(phyid);

	if(HAL_IS_PHY_EXIST(uiPortId))	
	{		
		iRv = rtk_port_phyAutoNegoEnable_get(uiPortId, &auto_enable);
	}
	else
	{
		/*there is no phy, so the autoneg is disable, example port 5*/
		auto_enable = DISABLED;
	}

	if(auto_enable == ENABLED)
	{
	    if((iRv = rtk_port_phyAutoNegoAbility_get(uiPortId, &Ability)) != RT_ERR_OK)
	    {      
	       return ErrorSdk2Drv(iRv);
	    }
		
	    if (ENABLED == enable)
	    {
	       Ability.FC = 1;
	       Ability.AsyFC = 1;
	    }
	    else
	    {
	       Ability.FC = 0;
	       Ability.AsyFC = 0;
	    }
	   
	    if ((iRv = rtk_port_phyAutoNegoAbility_set(uiPortId, &Ability)) != RT_ERR_OK)
	    {
	       return ErrorSdk2Drv(iRv);
	    }

		iRv = rtk_port_phyAutoNegoEnable_set(uiPortId, ENABLED);
	}
	else if(auto_enable == DISABLED)
	{
		if(HAL_IS_PHY_EXIST(uiPortId))	
		{
			iRv = rtk_port_phyForceModeAbility_get(uiPortId, &rtkSpeed, &rtkDuplex, &rtkFlowControl);
			if(rtkDuplex == PORT_HALF_DUPLEX)/*if the port duplex = half and flowctl enable ,it should make epon offline*/
		    	rtkFlowControl = DISABLED;
			else
				rtkFlowControl = enable;
			iRv = rtk_port_phyForceModeAbility_set(uiPortId, rtkSpeed, rtkDuplex, rtkFlowControl);
		}
		else
		{
			rtkFlowControl = enable;
		}
		
		memset(&portability, 0x00, sizeof(rtk_port_macAbility_t));
		iRv = rtk_port_macForceAbility_get(uiPortId, &portability);	
		if(portability.duplex == PORT_HALF_DUPLEX)/*if the port duplex = half and flowctl enable ,it should make epon offline*/
		{
			portability.txFc = DISABLED;
			portability.rxFc = DISABLED;
		}
		else
		{
			portability.txFc = rtkFlowControl;
			portability.rxFc = rtkFlowControl;
		}
		iRv = rtk_port_macForceAbility_set(uiPortId, portability);

		if(HAL_IS_PHY_EXIST(uiPortId))	
		{
			iRv = rtk_port_phyReg_get(uiPortId, 0, PHY_REG_CONTROL, &val32);			
		    if(val32 & PowerDown_MASK)
		    {
		        powerdown = 1;
		    }
			if(powerdown)
		    {
		        rtk_port_adminEnable_set(uiPortId, DISABLED);
		    }
		    else{
		        /*lint --e{155,62}  z03340 add*/
		        /*mdelay(500);*/
		        rtk_port_adminEnable_set(uiPortId, DISABLED);
		        mdelay(100);
		        rtk_port_adminEnable_set(uiPortId, ENABLED);
		    }
		}
	}	
	
    return ErrorSdk2Drv(iRv);    
}

/*****************************************************************************
    Func Name: Hal_SetPortIsolateMask
  Description: 按端口掩码设置开启/关闭端口隔离
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortIsolateMask(logic_pmask_t *pstPortMask)
{ 
    port_num_t lgcPort = 0;
    phyid_t phyid = 0;
    logic_pmask_t stTmpLogicMask;
    phy_pmask_t stPhyMask;
    phy_pmask_t stextPhyMask;
    phy_pmask_t stAllPhyMask;
    int32 iRv = RT_ERR_OK;
	unsigned char i;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }
	
    ClrLgcMaskAll(&stTmpLogicMask);
    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stextPhyMask);
    SetPhyMaskAll(&stAllPhyMask);
	for(i = 0; i < EXT_LGCPORT0; i++)
	{
		SetPhyMaskBit(i, &stextPhyMask);
	}
    LgcMaskCopy(&isolate_save, pstPortMask);

    /*set port mask*/
    LgcPortFor(lgcPort)
    {
        if(!TstLgcMaskBit(lgcPort, pstPortMask))
        {
            SetLgcMaskBit(lgcPort, &stTmpLogicMask);
        }
    }

	/*must add PON port for port mask for RTL9607*/
	SetLgcMaskBit(LOGIC_UPPON_PORT, &stTmpLogicMask);	
    /*must add CPU port for port mask*/
    MaskLogic2Phy(&stTmpLogicMask, &stPhyMask);
    PhyMaskAddCpuPort(&stPhyMask);	
    PhyMaskAddStackPort(&stPhyMask);
    
    LgcPortFor(lgcPort)
    {
        phyid = PortLogic2PhyID(lgcPort); 				
        /*isolate port*/
        if(TstLgcMaskBit(lgcPort, pstPortMask))
        {            
            iRv = rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0,PORTID(phyid),&(CHIPNMASK(CHIPID(phyid), &stPhyMask)),&(CHIPNMASK(CHIPID(phyid), &stextPhyMask)));             
            if(iRv != RT_ERR_OK)
            {
                return ErrorSdk2Drv(iRv);
            }
        }
        else
        {            
            iRv = rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, PORTID(phyid),&(CHIPNMASK(CHIPID(phyid), &stAllPhyMask)),&(CHIPNMASK(CHIPID(phyid), &stextPhyMask)));           
            if(iRv != RT_ERR_OK)
            {
                return ErrorSdk2Drv(iRv);
            }
        }
    }

    return DRV_OK;
}


/*****************************************************************************
    Func Name: Hal_SetPortMedium
        Input: port_num_t lgcPort  
                UINT32 uiMedium   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortMedium(port_num_t lgcPort, UINT32 uiMedium)
{
    phyid_t phyid = 0;  
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
   
    
    return ErrorSdk2Drv(iRv);
}


/*****************************************************************************
    Func Name: Hal_GetPortType
  Description: 获取COMBO口当前使能的媒介
        Input: port_num_t lgcPort  
       Output: 
       Return: 当前使能的媒介
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
UINT32 Hal_GetPortType(port_num_t lgcPort)
{
    UINT32 ulPortType = PORT_TYPE_IS_COPPER;
    if(!IsValidLgcPort(lgcPort))
    {
        return PORT_TYPE_IS_COPPER;
    }
    return ulPortType;
}


/*****************************************************************************
    Func Name: Hal_GetPortMedium

                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMedium(port_num_t lgcPort, UINT32 *puiMedium)
{
    phyid_t phyid = 0;  
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiMedium == NULL)
    {
        return DRV_ERR_PARA;
    }
    /*default value*/
    *puiMedium = PORT_TYPE_NOT_FIBER;
  
    return ErrorSdk2Drv(iRv);
}


/*****************************************************************************
    Func Name: Hal_GetPortCurrentLink
  Description: 获取当前端口的连接状态
        Input: port_num_t lgcPort  
                UINT32 *puiLink     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortCurrentLink(port_num_t lgcPort, UINT32 *puiLink)
{
    phyid_t phyid = 0;
   
    rtk_port_linkStatus_t enStatus;
   
   
    int32 iRv = RT_ERR_OK;
    //printk("%s,%s,%d lgcPort=%d\n",__FILE__,__FUNCTION__,__LINE__,lgcPort);
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiLink == NULL)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort); 

    iRv=rtk_port_link_get(PORTID(phyid),&enStatus);    

    if(iRv == RT_ERR_OK)
    {
        
        if(PORT_LINKUP==enStatus)
        {
            *puiLink=PORT_LINK_UP;
        }
        else
        {
            *puiLink=PORT_LINK_DOWN;
        }
        
    }
    //printk("%s,%s,%d phyid=%d,iRv=%d,enStatus=%d\n",__FILE__,__FUNCTION__,__LINE__,phyid,iRv,enStatus);
    return ErrorSdk2Drv(iRv);

}


/*****************************************************************************
    Func Name: Hal_GetPortLinkMask
  Description: 获取已连接端口的端口掩码
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortLinkMask(logic_pmask_t *pstPortMask)
{
    port_num_t uiPort;  
    phyid_t phyid = 0;

    rtk_port_linkStatus_t enStatus;


    int32 iRv = RT_ERR_OK;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }


    for (phyid = 0; phyid < 7; phyid ++)
	{
		int speed, duplex;
		enStatus = 0;
        speed = 0;
        duplex = 0;
		iRv= rtk_port_link_get(phyid, &enStatus);
        printk("call rtk_port_link_get(%d) return %d, link=%d\n", phyid, iRv, enStatus);

        iRv = rtk_port_speedDuplex_get(phyid, &speed, &duplex);
        printk("call rtk_port_speedDuplex_get(%d) return %d, speed=%d, duplex=%d\n", phyid, iRv, speed, duplex);
	}        

   
    LgcPortFor(uiPort)
    {
        phyid = PortLogic2PhyID(uiPort);  

        iRv = rtk_port_link_get(PORTID(phyid),&enStatus);

        if(iRv == RT_ERR_OK)
        {
            

            if(PORT_LINKUP==enStatus)
            {
                SetLgcMaskBit(uiPort,pstPortMask);
            }
            else
            {
                ClrLgcMaskBit(uiPort,pstPortMask);
            }
        }
        else
        {
            return ErrorSdk2Drv(iRv);
        }
    }
    return DRV_OK;
}


/*****************************************************************************
    Func Name: Hal_GetPortMediaFiberMask
  Description: 获取当前media为fiber的端口mask
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMediaFiberMask(logic_pmask_t *pstPortMask)
{
    port_num_t uiPort;  
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }
    LgcPortFor(uiPort)
    {
       
        ClrLgcMaskBit(uiPort,pstPortMask);
              
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetPortAbility
  Description: 获取端口的自协商能力位
        Input: port_num_t lgcPort  
               UINT32 *puiAbility  
               UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortAbility(port_num_t lgcPort, UINT32 *puiAbility, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_port_phy_ability_t rtkAbility;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiAbility == NULL)
    {
        return DRV_ERR_PARA;
    }
    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort); 
    
    iRv=rtk_port_phyAutoNegoAbility_get(PORTID(phyid), &rtkAbility);
    
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }

    *puiAbility = 0;

    if(1 == rtkAbility.Half_10){
        *puiAbility |= PORT_ABIL_10MB_HD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_10MB_HD);
    }
    
    if(1 == rtkAbility.Full_10){
        *puiAbility |= PORT_ABIL_10MB_FD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_10MB_FD);
    }
    
    if(1 == rtkAbility.Half_100){
        *puiAbility |= PORT_ABIL_100MB_HD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_100MB_HD);
    }
    
    if(1 == rtkAbility.Full_100){
        *puiAbility |= PORT_ABIL_100MB_FD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_100MB_FD);
    }
    
    if(1 == rtkAbility.Half_1000){
        *puiAbility |= PORT_ABIL_1000MB_HD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_1000MB_HD);
    }
    
    if(1 == rtkAbility.Full_1000){
        *puiAbility |= PORT_ABIL_1000MB_FD;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_1000MB_FD);
    }
    
    if(1 == rtkAbility.FC){
        *puiAbility |= PORT_ABIL_PAUSE_TX;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_PAUSE_TX);
    }
    
    if(1 == rtkAbility.AsyFC){
        *puiAbility |= PORT_ABIL_PAUSE_RX;
    }
    else{
        *puiAbility &= ~((UINT32)PORT_ABIL_PAUSE_RX);
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetPortSpeed
  Description: 获取端口的速率
     
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortSpeed(port_num_t lgcPort, UINT32 *puiSpeed)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
   
    rtk_port_speed_t enSpeed;
    rtk_port_duplex_t enDuplex;
   
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(NULL == puiSpeed)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort);
   
    iRv = rtk_port_speedDuplex_get(PORTID(phyid), &enSpeed, &enDuplex);
    if(RT_ERR_OK == iRv)
    {
        switch(enSpeed)
        {
            case PORT_SPEED_10M:
            {
                *puiSpeed=PORT_SPEED_10;
                break;
            }
            case PORT_SPEED_100M:
            {
                *puiSpeed=PORT_SPEED_100;
                break;
            }
            case PORT_SPEED_1000M:
            {
                *puiSpeed=PORT_SPEED_1000;
                break;
            }
            default:
                return DRV_SDK_GEN_ERROR;                           
        }
    }  
    //printk("%s,%s,%d  phyid=%d,iRv=%d,enSpeed=%d\n",__FILE__,__FUNCTION__,__LINE__,phyid,iRv,enSpeed);
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortDuplex
  Description: 获取端口的双工状态
     
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortDuplex(port_num_t lgcPort, UINT32 *puiDuplex)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
 
    rtk_port_speed_t enSpeed;
    rtk_port_duplex_t enDuplex;
   
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiDuplex == NULL)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort);    
   
    iRv = rtk_port_speedDuplex_get(PORTID(phyid), &enSpeed, &enDuplex);
    
    if(RT_ERR_OK == iRv)
    {
        switch(enDuplex)
        {
            case PORT_HALF_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_HALF;
                break;
            }
            case PORT_FULL_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_FULL;
                break;
            }
            default:
                 return DRV_SDK_GEN_ERROR;                           
        }
    }
    //printk("%s,%s,%d  phyid=%d,iRv=%d,enDuplex=%d\n",__FILE__,__FUNCTION__,__LINE__,phyid,iRv,enDuplex);
    
    return ErrorSdk2Drv(iRv);
}


/*****************************************************************************
    Func Name: Hal_GetPortMdix
  Description: 获取端口的MDI
        Input: port_num_t lgcPort  
                UINT32 *puiMdix     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMdix(port_num_t lgcPort, UINT32 *puiMdix)
{
    
    phyid_t phyid = 0;    
    rtk_port_crossOver_mode_t rtkMdix = PORT_CROSSOVER_MODE_AUTO;
     
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    if(puiMdix == NULL)
    {
        return DRV_ERR_PARA;
    }
   
    phyid = PortLogic2PhyID(lgcPort);    
    iRv = rtk_port_phyCrossOverMode_get(PORTID(phyid), &rtkMdix);

    if(RT_ERR_OK == iRv)
    {
        switch (rtkMdix)
        {
            case PORT_CROSSOVER_MODE_AUTO:
                *puiMdix = PORT_MDI_AUTO;
                break;
            case PORT_CROSSOVER_MODE_MDIX:
                *puiMdix = PORT_MDI_ACROSS;
                break;
            case PORT_CROSSOVER_MODE_MDI:
                *puiMdix = PORT_MDI_NORMAL;
                break;
            default:
                return DRV_INVALID_MDI_VALUE;
        }
    }
    
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortPriority
  Description: 获取端口优先级
        Input: port_num_t lgcPort  
                UINT32 *puiPrio     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortPriority(port_num_t lgcPort, UINT32 *puiPrio)
{
    phyid_t phyid = 0;
    rtk_pri_t rtkPrio = 0;
    int32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiPrio == NULL)
    {
        return DRV_ERR_PARA;
    }

    phyid = PortLogic2PhyID(lgcPort);


    iRv = rtk_qos_portPri_get(PORTID(phyid),&rtkPrio);

    if(iRv == RT_ERR_OK)
    {
        *puiPrio = rtkPrio;
    }
    
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortFlowctrl
  Description: 获取端口的流控状态
        Input: port_num_t lgcPort   
                UINT32 *puiFlowctrl  
       Output: 
       Return: 
      Caution: 发送和接收方向设置时都是统一设置,所以这里判断时只用tx判断
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortFlowctrl(port_num_t lgcPort, UINT32 *puiFlowctrl)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
	uint32 txStatus,rxStatus=0;
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiFlowctrl == NULL)
    {
        return DRV_ERR_PARA;
    }

    phyid = PortLogic2PhyID(lgcPort);
   
	iRv=rtk_port_flowctrl_get(PORTID(phyid),&txStatus,&rxStatus);
    if(iRv == RT_ERR_OK)
    {
        
        if (txStatus && rxStatus)
        *puiFlowctrl = PORT_FLOWCTRL_ENABLE;
        else
        *puiFlowctrl = PORT_FLOWCTRL_DISABLE;
    }
   
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortJumbo
  Description: 获取端口是否使能JUMBO功能
        Input: port_num_t lgcPort  
                UINT32 *puiJumbo    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortJumbo(port_num_t lgcPort, UINT32 *puiJumbo)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    uint32 ulLen = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiJumbo == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
   
  
    iRv = rtk_switch_maxPktLenLinkSpeed_get(MAXPKTLEN_LINK_SPEED_GE, &ulLen);
    
    if(RT_ERR_OK == iRv)
    {      
        if(RTK_NORMAL_PACKET_LEN != ulLen)
        {
            *puiJumbo = PORT_JUMBO_ENABLE;
        }
        else
        {
            *puiJumbo = PORT_JUMBO_DISABLE;
        }
    }
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortJumbo
  Description: 获取端口支持的最大报文长度
        Input: port_num_t lgcPort  
                UINT32 *puiLen    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMaxFrameLen(port_num_t lgcPort, UINT32 *puiLen)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    uint32 ulLen = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiLen == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
   
    iRv = rtk_switch_maxPktLenLinkSpeed_get(MAXPKTLEN_LINK_SPEED_GE, &ulLen);
    
    if(RT_ERR_OK == iRv)
    {      
        *puiLen = ulLen;
    }
    else
    {
        *puiLen = 0;
    }
    
    return ErrorSdk2Drv(iRv);
}

/* 014538 */
/*****************************************************************************
  函 数 名: Hal_SetPortMaxFrameLen

  函数描述: 设置端口支持的最大报文长度
  输入参数: UINT32 uiLen
  输出参数: 无
    返回值:
    注意点:
------------------------------------------------------------------------------
    修改历史
    日期        姓名             描述
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetPortMaxFrameLen(port_num_t lgcPort, UINT32 uiLen)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    
    iRv = rtk_switch_maxPktLenLinkSpeed_set(MAXPKTLEN_LINK_SPEED_GE, uiLen);
  
    if(RT_ERR_OK == iRv)
    {
        return DRV_OK;
    }
    else
    {
        return ErrorSdk2Drv(iRv);
    }

}



/*****************************************************************************
    Func Name: Hal_PortLedInit
  Description: 设置端口的led灯
        Input:      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_PortLedInit(void)
{
   
   
    rtk_led_modeForce_set(0, LED_FORCE_ON);
    msleep(500);
    rtk_led_modeForce_set(0, LED_FORCE_BLINK);
    
    return DRV_OK;
}



/*****************************************************************************
    Func Name: Hal_SetPortLed
  Description: 设置端口的led灯
   
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortLed(port_num_t lgcPort, UINT32 uiLinkState, UINT32 uiPortType)
{   
    INT32 iRv = RT_ERR_OK;

    return ErrorSdk2Drv(iRv);
}
/*****************************************************************************
    Func Name: Hal_SetInternalLoopback
  Description: 设置内环测试
        Input: port_num_t lgcPort   
                UINT32 uiEnable 开启/关闭
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

DRV_RET_E Hal_SetInternalLoopback(port_num_t lgcPort, UINT32 uiEnable)
{
    INT32 iRv = RT_ERR_OK;
    phyid_t phyid = 0;
    //rtk_enable_t enable;    
    UINT32 data;
    
   return DRV_OK;
}



void  rtk_port_parm_init(void)
{    
    int ii;
    
    printk("##### Hi call Hal_CopperPortCfgInit\n");
    ClrLgcMaskAll(&isolate_save);
    for ( ii = 0 ; ii <  (LOGIC_PORT_NO+1); ii++ )
    {
        spdlx_save[ii] = (uint32)PORT_AUTOAUTO;
		flowcontrol_save[ii] = DISABLED;
    }
}




/*****************************************************************************
    Func Name: Hal_SetPortAbility
  Description: 设置协商时端口的能力位
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAbility    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_RestartPortNeg(port_num_t lgcPort)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;      
    uint32 phyData = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);

   
    iRv = rtk_port_phyReg_get(phyid,0,PHY_CONTROL_REG,&phyData);
    if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }       
	#define PHY_CONTROL_REG_AUTO_NEGO_OFFSET            12
    /* 不支持自协商，返回失败 */
    if (!(phyData & (1U << PHY_CONTROL_REG_AUTO_NEGO_OFFSET)))
    {
        return DRV_INVALID_VALUE;
    }
	#define PHY_CONTROL_REG_RESTART_AUTO_NEGO_OFFSET   9
    phyData |= (1U << PHY_CONTROL_REG_RESTART_AUTO_NEGO_OFFSET); /* Restart neg bit */
    iRv = rtk_port_phyReg_set(phyid,0,PHY_CONTROL_REG,phyData);
    if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }    
    
    return ErrorSdk2Drv(iRv);
}
static struct task_struct *kport_monitor_task;
static void gphy_status_handler(void)
{
    int ret;
	rtk_portmask_t state;
	port_num_t i;
	int macdata;
	int phydata;
	rtk_port_macAbility_t portability;
	rtk_port_macAbility_t newportability;

	
	LgcPortFor(i)
    {		
		if(PortLogic2PhyID(i) == 5)
		{
			/*Cable Port , So Do Nothing*/
		}
		else
		{
			rtk_port_macForceAbilityState_get(PortLogic2PhyID(i),&macdata);
			
			if(macdata==1)
			{
				
				rtk_port_macForceAbility_get(PortLogic2PhyID(i), &portability);
				rtk_port_phyReg_get(PortLogic2PhyID(i),0,1,&phydata);
				
				if(phydata&LinkStatus_MASK)
				{
					newportability.linkStatus=1;
					//printk("port %d is up\n",i);
				}
				else
				{
					newportability.linkStatus=0;
					//printk("port %d is down %02x \n",i,phydata);
				}
				
				if(newportability.linkStatus!=portability.linkStatus)
				{
					portability.linkStatus=newportability.linkStatus;
					rtk_port_macForceAbility_set(PortLogic2PhyID(i),portability);
				}
			}
		}
	}	
	
    //osal_printf("\n\rDyingGasp isr done!!!\n\r");
}

extern int32 __RestoreIntr_Handler(UINT32 status);

static int portminitor_thread(void)
{
	UINT8 readByte = 0;
    UINT32 flag;
    
    while(1)
    {       
        msleep(1000);
        Hal_GpioValGet(22, &flag);
        __RestoreIntr_Handler(flag);
		gphy_status_handler();
    }
    return 0;
}

/*  */
/*****************************************************************************
    Func Name: Hal_PortInit
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_PortInit(void)
{
    rtk_port_t tdPhyId;
    rtk_api_ret_t retVal;
    rtk_port_phy_data_t tdPhyData = 0;
    port_num_t uiPort;  
	
    /*Enable all of the ports.*/
     LgcPortFor(uiPort)
    {
    	if(PortLogic2PhyID(uiPort) == 5)
    	{
    		/*Cable Port , So Do Nothing*/
    	}
		else
		{
	        retVal = rtk_port_phyReg_get(PortLogic2PhyID(uiPort),0, PHY_REG_CONTROL, &tdPhyData);
	        if(RT_ERR_OK != retVal)
	        {
	            return DRV_SDK_GEN_ERROR;
	        }
	    
	        tdPhyData &= 0xF7FF;
	        tdPhyData |= 0x0200;
	        retVal = rtk_port_phyReg_set(PortLogic2PhyID(uiPort),0, PHY_REG_CONTROL, tdPhyData);
	        if(RT_ERR_OK != retVal)
	        {
	            return DRV_SDK_GEN_ERROR;
	        }
    	}
    }
	
    return DRV_OK;
}
/* 014288 */

void _Hal_PortMonitorInit(void)
{
	kport_monitor_task = kthread_run(portminitor_thread, NULL, "portmonitor");
	if (kport_monitor_task==NULL)
	{
		return ;  
	}
	return ;
}

#ifdef  __cplusplus
}
#endif


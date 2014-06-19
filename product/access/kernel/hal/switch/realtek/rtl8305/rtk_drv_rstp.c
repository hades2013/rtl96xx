/*****************************************************************************

                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
#include "rtk_api.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
 

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/


/*****************************************************************************
    Func Name: Hal_GetStpPortState
  Description: 获取当前端口STP的状态
        Input: port_num_t lport                
       Output: UINT32 * pulState
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                         
*****************************************************************************/
DRV_RET_E Hal_GetStpPortState(port_num_t lgcPort, UINT32 * puiState)
{
	phyid_t phyid = 0;
	UINT32 uiPortId = 0;
	INT32 iRv = RT_ERR_OK;
	rtk_stp_state_t enStpState = 0;

	if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

	phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_stp_mstpState_get(0, uiPortId, &enStpState);
	if(RT_ERR_OK != iRv)
	{
	    return ErrorSdk2Drv(iRv);
	}

	switch (enStpState)
	{
        case STP_STATE_DISABLED:
            *puiState = PORT_STATE_DISABLED;
            break;
        case STP_STATE_BLOCKING:
            *puiState = PORT_STATE_BLOCKING;
            break;
        case STP_STATE_LEARNING:
            *puiState = PORT_STATE_LEARNING;
            break;
        case STP_STATE_FORWARDING:
            *puiState = PORT_STATE_FORWARDING;
            break;
        default:
            return DRV_ERR_UNKNOW;
    }  

	return DRV_OK;
}


/*****************************************************************************
    Func Name: Hal_SetStpPortState
  Description: 设置当前端口STP的状态
        Input: port_num_t lport    
        	     UINT32 ulState
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                         
*****************************************************************************/
DRV_RET_E Hal_SetStpPortState(port_num_t lgcPort, UINT32 uiState)
{
	phyid_t phyid = 0;
	UINT32 uiPortId = 0;
	INT32 iRv = RT_ERR_OK;
	rtk_stp_state_t enStpState = 0;

	if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

	switch (uiState)
	{
        case PORT_STATE_DISABLED:
            enStpState = STP_STATE_DISABLED;
            break;
        case PORT_STATE_BLOCKING:
            enStpState = STP_STATE_BLOCKING;
            break;
        case PORT_STATE_LEARNING:
            enStpState = STP_STATE_LEARNING;
            break;
        case PORT_STATE_FORWARDING:
            enStpState = STP_STATE_FORWARDING;
            break;
        default:
            return DRV_ERR_PARA;
    }  

	phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_stp_mstpState_set(0, uiPortId, enStpState);
	
	return ErrorSdk2Drv(iRv);
}




#ifdef  __cplusplus
}
#endif


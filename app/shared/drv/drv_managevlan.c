/*****************************************************************************

      File name:drv_managevlan.c
      Description:provide funcs of management vlan
      Author:liaohongjun
      Date:
              2012/11/2
==========================================================
      Note:
*****************************************************************************/

#ifdef  __cplusplus
extern "C"{
#endif

#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"

/*****************************************************************
    Function:Drv_SetManageVlan
    Description:func to set management vlan
    Author:liaohongjun
    Date:2012/11/2
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/   
DRV_RET_E Drv_SetManageVlan(UINT32 uiVlanId)
{
    if((uiVlanId < MIN_VLAN_INDEX) || (uiVlanId > MAX_VLAN_INDEX))
    {
        return DRV_ERR_PARA;
    }
    
    if(DRV_OK != Ioctl_SetManageVlan(uiVlanId))
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}






#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */



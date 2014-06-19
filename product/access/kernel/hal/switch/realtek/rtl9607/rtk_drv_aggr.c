/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/

/*****************************************************************************
    Func Name:  Hal_SetAggrMode
  Description:  set global aggr mode
        Input:  UINT32 ulMode     
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetAggrMode(UINT32 ulMode)
{
    (void)ulMode;     

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetAggrGroup
  Description:  set aggr group
        Input:  grpNum
                stLgcMask
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetAggrGroup(agg_grp_num_t grpNum, logic_pmask_t stLgcMask)
{
    (void)grpNum;
    (void)stLgcMask;  

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif


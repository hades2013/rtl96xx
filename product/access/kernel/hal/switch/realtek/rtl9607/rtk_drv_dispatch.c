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
#include <ioal/mem32.h>

/*****************************************************************************
    Func Name: Hal_soc_reg_read
  Description: 读取指定寄存器值的接口
        Input: UINT32 ulChipNum
                UINT32 ulRegIndex  
                UINT32 *pulData    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_soc_reg_read(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 *pulData)
{
    if (ulChipNum >= MAX_CHIP_NUM)
    {
        return DRV_ERR_PARA;
    }
    if (NULL == pulData)
    {
        return DRV_ERR_PARA;
    }

    /*For lint*/
    ulChipNum = ulChipNum;

    if (RT_ERR_OK != ioal_mem32_read(ulRegIndex, pulData))
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_soc_reg_write
  Description: 提供按寄存器地址设置寄存器值的接口
        Input: UINT32 ulChipNum
                UINT32 ulRegIndex  
                UINT32 ulData      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_soc_reg_write(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 ulData)
{
    if (ulChipNum >= MAX_CHIP_NUM)
    {
        return DRV_ERR_PARA;
    }

    /*For lint*/
    ulChipNum = ulChipNum;
    
    if (RT_ERR_OK != ioal_mem32_write(ulRegIndex, ulData))
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}
/*****************************************************************************
    Func Name: Hal_soc_mem_read
  Description: Read one specified table entry by table index.
        Input: UINT32 ulChipNum      
                UINT32 ulTable        
                UINT32 ulAddr         
                UINT32* pulEntryData  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_soc_mem_read(UINT32 ulChipNum, UINT32 ulTable,  UINT32 ulAddr, UINT32* pulEntryData)
{
    /*For lint*/
    ulChipNum = ulChipNum;
    ulTable = ulTable;
    ulAddr = ulAddr;
    pulEntryData = pulEntryData;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_soc_mem_write
  Description: Write one specified table entry by table index.
        Input: UINT32 ulChipNum      
                UINT32 ulTable        
                UINT32 ulAddr         
                UINT32* pulEntryData  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_soc_mem_write(UINT32 ulChipNum, UINT32 ulTable,  UINT32 ulAddr, UINT32* pulEntryData)
{
    /*For lint*/
    ulChipNum = ulChipNum;
    ulTable = ulTable;
    ulAddr = ulAddr;
    pulEntryData = pulEntryData;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetCpuAddr
  Description: 提供读取CPU地址的值的接口
        Input: UINT32 ulCpuAddr  
                UINT32 *pulData   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetCpuAddr(UINT32 ulCpuAddr, UINT32 *pulData)
{   
    /*For lint*/
    ulCpuAddr = ulCpuAddr;
    pulData = pulData;
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetCpuAddr
  Description: 提供设置CPU地址的值的接口
        Input: UINT32 ulCpuAddr  
                UINT32 ulData     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetCpuAddr(UINT32 ulCpuAddr, UINT32 ulData)
{
    /*For lint*/
    ulCpuAddr = ulCpuAddr;
    ulData = ulData;
    
    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif



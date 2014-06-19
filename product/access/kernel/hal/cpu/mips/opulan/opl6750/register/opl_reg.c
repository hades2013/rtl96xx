/*****************************************************************************
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <linux/mutex.h>
#include <lw_type.h>
#include "opl_drv.h"
#include "lw_drv_pub.h"
#include "hal_common.h"
#include <linux/init.h>
#include <linux/module.h>

/*----------------------------------------------*
 * extern function                              *
 *----------------------------------------------*/
 //extern DRV_RET_E mdioRegisterInit(void);

/*----------------------------------------------*
 * internal function                              *
 *----------------------------------------------*/

/*----------------------------------------------*
 * global parameter                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stRegMutex;

 
/*----------------------------------------------*
 * macro                                        *
 *----------------------------------------------*/
//#define REG_MUTEX_CREATE()   DEFINE_MUTEX(m_stRegMutex)
#define REG_MUTEX_INIT     mutex_init(&m_stRegMutex)
#define REG_MUTEX_LOCK     mutex_lock(&m_stRegMutex)
#define REG_MUTEX_UNLOCK   mutex_unlock(&m_stRegMutex)

#define OPL_REG_BASE_ADDR  (0xbf000000)

/*****************************************************************************
    Func Name: Hal_CpuRegAccessInit
  Description: Init mutext for register accessing.
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void Hal_CpuRegAccessInit(void)
{
    REG_MUTEX_INIT; 

    return ;
}

/*****************************************************************************
    Func Name: Hal_CpuRegRead
  Description: Read value of register
        Input: UINT32 regAddr  
                UINT32 *regVal  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal)
{
	if((OPL_NULL) == regVal)
	{
		return DRV_ERR_PARA;
	}
	if(regAddr%4)
	{
		return DRV_ERR_PARA;
	}

    REG_MUTEX_LOCK;
	*regVal = *(volatile UINT32 *)((UINT32)OPL_REG_BASE_ADDR + regAddr);	
    REG_MUTEX_UNLOCK;

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_CpuRegWrite
  Description: Write value to register
        Input: UINT32 regAddr  
                UINT32 regVal   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal)
{
	if(regAddr%4)
	{
		return DRV_ERR_PARA;
	}

    REG_MUTEX_LOCK;
	*(volatile UINT32 *)((UINT32)OPL_REG_BASE_ADDR + regAddr) = regVal;
    REG_MUTEX_UNLOCK;
	return DRV_OK;
}


/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 

int (* pFunc_oplRegFieldWrite)(unsigned int regAddr,unsigned short fieldOffset,unsigned short fieldWidth,unsigned int data0) = NULL;
int (* pFunc_oplRegFieldRead)(unsigned int  regAddr,unsigned short fieldOffset,unsigned short fieldWidth,unsigned int *data0) = NULL;

EXPORT_SYMBOL(pFunc_oplRegFieldRead);

EXPORT_SYMBOL(pFunc_oplRegFieldWrite);
/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/ 


/*****************************************************************
Function:Hal_LaserSetReg
Description:Hal_LaserSetReg
Author:huangmingjian
Date:2013/03/15
INPUT: pstDrvReq
OUTPUT:         
Return: DRV_OK or DRV_CFG_WRITE_ERROR or DRV_NULL_POINTER

Others:	       
*****************************************************************/

DRV_RET_E Hal_LaserSetReg(DRV_REQ_S *pstDrvReq)
{
	INT ret = 0;

	if(pFunc_oplRegFieldWrite)
	{
		ret = pFunc_oplRegFieldWrite(pstDrvReq->para1_u.uiValue, 
				pstDrvReq->para2_u.uiValue,
				pstDrvReq->para3_u.uiValue,
				pstDrvReq->para4_u.uiValue);
		if(ret != 0)
		{
			printk("ret error\n");
			return DRV_CFG_WRITE_ERROR;
		}	
	}
	else
	{	
		printk("pFunc_oplRegFieldWrite isn't exist\n");	
		return DRV_NULL_POINTER;
	}
	return DRV_OK;
}


/*****************************************************************
Function:Hal_LaserGetReg
Description:Hal_LaserGetReg
Author:huangmingjian
Date:2013/03/15
INPUT: pstDrvReq
OUTPUT:         
Return: DRV_OK or DRV_CFG_WRITE_ERROR or DRV_NULL_POINTER

Others:	       
*****************************************************************/

DRV_RET_E Hal_LaserGetReg(DRV_REQ_S *pstDrvReq)
{
	UINT32 *read_data;
	INT ret = 0;

	*read_data = 0;
	if(pFunc_oplRegFieldRead)
	{
		ret = pFunc_oplRegFieldRead(pstDrvReq->para1_u.uiValue, 
				pstDrvReq->para2_u.uiValue,
				pstDrvReq->para3_u.uiValue,
				read_data);
		if(ret != 0)
		{
			printk("ret error\n");
			return DRV_CFG_READ_ERROR;
		}
		pstDrvReq->para4_u.uiValue = *read_data;
	}
	else
	{	
		printk("pFunc_oplRegFieldRead isn't exist\n");	
		return DRV_NULL_POINTER;
	}
	return DRV_OK;
}

#ifdef  __cplusplus
}
#endif


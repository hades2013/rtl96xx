#ifdef  __cplusplus
extern "C"{
#endif

#include <linux/mutex.h>
#include "opl_drv.h"
#include <lw_type.h>
#include "lw_drv_pub.h"

/*----------------------------------------------*
 * extern function                              *
 *----------------------------------------------*/
extern DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal);
extern DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal);

/*----------------------------------------------*
 * internal function                              *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * global parameter                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stMidoMutex;
/*----------------------------------------------*
 * macro                                        *
 *----------------------------------------------*/
#define MDIO_READ				0X02
#define MDIO_WRITE				0X01

#define MDIO_BUSY				0X1
#define MDIO_TIMEOUT			1000

#define MDIO_BA_B				 (0x0500)
//#define MDIO_BA_E				 (0x0506)
//#define REG_MDIO_DIV_FACTOR     		((MDIO_BA_B + 0)*4)
#define REG_MDIO_OP_PULSE				((MDIO_BA_B + 1)*4)
#define REG_MDIO_PHYAD  				((MDIO_BA_B + 2)*4)
#define REG_MDIO_REGAD					((MDIO_BA_B + 3)*4)
#define REG_MDIO_WRITE_DATA   			((MDIO_BA_B + 4)*4)
#define REG_MDIO_READ_DATA     			((MDIO_BA_B + 5)*4)
#define REG_MDIO_BUSY     				((MDIO_BA_B + 6)*4)
#if 0
#define MDIO_REG_FIELD_READ(regAddr,fieldOffset,fieldWidth,data0) { \
	Hal_CpuRegRead(regAddr, &(data0)); \
	data0 = ((data0)&((~(0XFFFFFFFF<<(fieldWidth)))<<(fieldOffset)))>>(fieldOffset); \
}

#define MDIO_REG_FIELD_WRITE(regAddr,fieldOffset,fieldWidth,data0) { \
	UINT32 oldVal,fieldMask; \
	fieldMask = (~(0XFFFFFFFF<<(fieldWidth)))<<(fieldOffset); \
	Hal_CpuRegRead(regAddr,&oldVal); \
	Hal_CpuRegWrite(regAddr, (((data0)<<(fieldOffset))&fieldMask)|(oldVal&(~fieldMask))); \
}
#else
#define MDIO_REG_FIELD_READ(regAddr,fieldOffset,fieldWidth,data0) { \
	Hal_CpuRegRead(regAddr, &(data0)); \
	data0 = ((data0)&(((1U<<(fieldWidth))-1)<<(fieldOffset)))>>(fieldOffset); \
}

#define MDIO_REG_FIELD_WRITE(regAddr,fieldOffset,fieldWidth,data0) { \
	UINT32 oldVal,fieldMask; \
	fieldMask = ((1U<<(fieldWidth))-1)<<(fieldOffset); \
	Hal_CpuRegRead(regAddr,&oldVal); \
	Hal_CpuRegWrite(regAddr, (((data0)<<(fieldOffset))&fieldMask)|(oldVal&(~fieldMask))); \
}
#endif

//#define MDIO_MUTEX_CREATE()   DEFINE_MUTEX(m_stMidoMutex)
#define MDIO_MUTEX_INIT     mutex_init(&m_stMidoMutex)
#define MDIO_MUTEX_LOCK     mutex_lock(&m_stMidoMutex)
#define MDIO_MUTEX_UNLOCK   mutex_unlock(&m_stMidoMutex)



//#define MDIO_LOCK_DEBUG 

void Hal_MdioRegisterInit(void)
{
	MDIO_MUTEX_INIT; 

	return ;
}

/*****************************************************************************
    Func Name: Hal_MdioRegisterRead
  Description: Read register value via mdio interface
        Input: UINT8 deviceAddr  
                UINT8 regAddr     
                UINT16 *data0     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_MdioRegisterRead(UINT8 deviceAddr,UINT8 regAddr,UINT16 *data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	UINT32 regVal = 0;
	DRV_RET_E retVal = DRV_OK;

    MDIO_MUTEX_LOCK;

    /* first check that it is not busy */
	MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	while(mdioBusy&MDIO_BUSY){
		if(!timeOut--){
			//OPL_TRACE();
			retVal = DRV_ERR_UNKNOW;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	}
	
	MDIO_REG_FIELD_WRITE(REG_MDIO_PHYAD,0,5,deviceAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_OP_PULSE, 0, 2, MDIO_READ);
	MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	timeOut = MDIO_TIMEOUT;
	while(mdioBusy&MDIO_BUSY){
		if(!timeOut--){
			//OPL_TRACE();
			retVal = DRV_ERR_UNKNOW;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	}

	MDIO_REG_FIELD_READ(REG_MDIO_READ_DATA, 0, 16, regVal);
	
exit_label:
	*data0 = (UINT16)regVal;
    MDIO_MUTEX_UNLOCK;
	return retVal;
}

/*****************************************************************************
    Func Name: Hal_MdioRegisterWrite
  Description: Write value to register via mdio interface
        Input: UINT8 deviceAddr  
                UINT8 regAddr     
                UINT16 data0      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_MdioRegisterWrite(UINT8 deviceAddr,UINT8 regAddr,UINT16 data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	DRV_RET_E retVal = DRV_OK;

    MDIO_MUTEX_LOCK;
	MDIO_REG_FIELD_READ(REG_MDIO_BUSY,0, 1, mdioBusy);
	while(MDIO_BUSY&mdioBusy){
		if(!timeOut--){
			//OPL_TRACE();
			retVal = DRV_ERR_UNKNOW;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY,0, 1, mdioBusy);
	}

	MDIO_REG_FIELD_WRITE(REG_MDIO_PHYAD, 0, 5, deviceAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_WRITE_DATA, 0, 16, data0);
	MDIO_REG_FIELD_WRITE(REG_MDIO_OP_PULSE, 0, 2, MDIO_WRITE);

exit_label:	
    MDIO_MUTEX_UNLOCK;
	return retVal;	
}

/*****************************************************************************
    Func Name: Hal_MdioRegisterFieldRead
  Description: Read value from specific field of register via mdio interface
        Input: UINT8 deviceAddr  
                UINT8 regAddr     
                UINT8 offset      
                UINT8 width       
                UINT16 *data0     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_MdioRegisterFieldRead(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 *data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	DRV_RET_E retVal;

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		//OPL_TRACE();
		return DRV_ERR_PARA;
	}

	if(((void *) 0) == data0)
	{
		//OPL_TRACE();
		return DRV_ERR_PARA;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		//fieldMask = (~(0xffff<<width))<<offset;	
		fieldMask = (UINT16)(((1U<<width) - 1)<<offset);
	}

	retVal = Hal_MdioRegisterRead( deviceAddr,  regAddr, &regVal);

	if(DRV_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&fieldMask;
	*data0 = regVal>>offset;
	return retVal;
}

/*****************************************************************************
    Func Name: Hal_MdioRegisterFieldWrite
  Description: Write value to specific field of register via mdio interface
        Input: UINT8 deviceAddr  
                UINT8 regAddr     
                UINT8 offset      
                UINT8 width       
                UINT16 data0      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_MdioRegisterFieldWrite(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	DRV_RET_E retVal;

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		//OPL_TRACE();
		return DRV_ERR_PARA;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		//fieldMask = (~(0xffff<<width))<<offset;		
        fieldMask = (UINT16)(((1U<<width) - 1)<<offset);
	}

	retVal = Hal_MdioRegisterRead( deviceAddr,  regAddr, &regVal);

	if(DRV_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&~fieldMask;
	regVal = regVal|((data0<<offset)&fieldMask);

	retVal = Hal_MdioRegisterWrite( deviceAddr,  regAddr,  regVal);
	return retVal;
}

#ifdef  __cplusplus
}
#endif


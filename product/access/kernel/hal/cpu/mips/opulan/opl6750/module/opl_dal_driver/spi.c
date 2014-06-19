/**
*
*  COPYRIGHT (C) 2003-2030 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
* 
* @file spi.c Driver source file for SPI module
*
* DESCRIPTION: 
*
* Date Created:
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
*
*/
/**
 * Source file changeLog:
 *   - 2010-01-20:add initial code according to ONU2 register document v1.0
 */
/**
 * @defgroup SPI SPI module
 * @{
 */

/* FPGA MAX : set spi  clock 1/6 of sysclk  */

/***** INCLUDE FILES *****/
#include <opl_driver.h>
#include <opl_errno.h>
#include <opl_debug.h>
#include <opl_utils.h>
#include <opconn_api.h>

#include <stdio.h>
#include "vos.h"

#include "spi.h"


#define O_RDONLY 0
#define NO_ERROR OPL_OK

/***** LOCAL DEFINES and ENUM *****/
#define PRINT_HELP printk("1. param filesize must < actually size");\
		printk("2. param filesize must be 12*N");

/***** LOCAL STRUCTURES and UNIONS *****/
SPI_INT_STATUS P_int_status;

/***** LOCAL TYPEDEFS ****/

/***** LOCAL (PRIVATE) PROTOTYPES *****/

/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/**
*@par Description
* This function init spi master.
*
 * @retval OPL_OK  Spi init success.
*/
/* div : 16bit */
OPL_STATUS spiInit(int div)
{
    OPL_STATUS retVal;
//	retVal = oplRegFieldWrite(0x28025, 0, 1, 0 );	
//	retVal = oplRegFieldWrite(0xc25, 0, 1, 0 );	

//	retVal = gpioMppSet(2,1);

/* tbd:?
	retVal = oplRegField. b29, 2, 1, 1 ); 
*/	

	OPL_PRINTF("set gpio 2 spi_select...\n");
//	retVal = gpioMppSet(4,1);  
//   OPL_PRINTF("set gpio 4 spi_select...\n");
	owr(REG_SPI_INT_ENABLE,0x0);
	retVal = oplRegFieldWrite(REG_SPI_CTL, 2, 1, 1 );
	OPL_PRINTF("set spi to change at falling edge...\n");
	retVal = spiIntEnable();
	if(OPL_OK != retVal)
	{
		DBG_LINE;
		return retVal;
	}
	retVal = spiAutoSelectChose(1);
	if(OPL_OK != retVal)
	{
		DBG_LINE;	
		return retVal;
	}
	retVal = spiFreqDivSet(div);
	OPL_PRINTF("set spi  clock 1/%d of sysclk\n", div + 1);
	if(OPL_OK != retVal)
	{
		DBG_LINE;	
		return retVal;
	}
	retVal = spiSlaveSelect(0);
	retVal = spiTransLenSet(0x8080);
	DBG_LINE;  
	return retVal;
	
}


OPL_STATUS spiFreqDivSet(OPL_UINT32_t div)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_FRQ_DIV, 0, 16, div );
    return retVal;
}
OPL_STATUS  spiTransLenSet (OPL_UINT32_t len) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldWrite(REG_SPI_LEN, 0, 16, len );
     return retVal;
}
OPL_STATUS  spiTransLenGet (OPL_UINT32_t *len) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldRead(REG_SPI_LEN, 0,16, len );
     return retVal;
}
OPL_STATUS spiIntEnable(void)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_CTL, 4, 1, 1 );
	OPL_PRINTF("enable spi interrupt...\n");
    return retVal;
}
OPL_STATUS spiIntDisable(void)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_CTL, 4, 1, 0 );
	OPL_PRINTF("disable spi interrupt...\n");
    return retVal;
}
OPL_STATUS  spiBufReadySet (OPL_UINT32_t buf_num) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldWrite(REG_SPI_BUF_READY, buf_num, 1, 1 );
     return retVal;
}
OPL_STATUS	spiIntStatusGet (void)
{
    OPL_STATUS retVal;
	OPL_STATUS tmpVal;
	retVal = oplRegRead(REG_SPI_INT,&tmpVal);
	OPL_PRINTF("get spi int status : 0x%08x \n",tmpVal);
	P_int_status.ready_buff_id = (tmpVal&0x10)>>4 ;
	P_int_status.spi_trans_error_int = (tmpVal&0x08)>>3 ;
	P_int_status.spi_trans_done_int = (tmpVal&0x04)>>2 ;
	P_int_status.spi_config_error_int = (tmpVal&0x02)>>1 ;
	P_int_status.spi_complete_int = tmpVal&0x01 ;
	return retVal;
}
OPL_STATUS  spiReadyBufIdGet (OPL_UINT32_t *id) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldRead(REG_SPI_INT, 4, 1, id );
     return retVal;
}
OPL_STATUS spiAutoSelectChose(OPL_UINT32_t option)
{
    OPL_STATUS retVal;
    switch (option){
		case 0:
			OPL_PRINTF("disable auto select... \n");
			break;
		case 1: 
			OPL_PRINTF("enable auto select... \n");
			break;			
		default:
           OPL_PRINTF("chu  cuo  la......... : valid option\n");
           return OPL_ERROR;		
		}
    retVal = oplRegFieldWrite(REG_SPI_CTL, 5, 1, option );
    return retVal;
}
OPL_STATUS spiSlaveSelect(OPL_UINT32_t cs)
{
    OPL_STATUS retVal;
	OPL_UINT32_t cs_val;
    switch (cs){
		case 0:
			OPL_PRINTF("select slave 0 ... \n");
			cs_val = 0x1;
			break;
		case 1: 
			OPL_PRINTF("select slave 1 ... \n");
		    cs_val = 0x2;
			break;	
		case 2: 
			OPL_PRINTF("select slave 2 ... \n");
			cs_val = 0x4;
			break;	
		default:
           OPL_PRINTF("chu  cuo  la......... : valid CS\n");
           return OPL_ERROR;		
		}
    retVal = oplRegFieldWrite(REG_SPI_SS, 0, 3, cs_val );
    return retVal;
}
OPL_STATUS  waitTransDone( void )
{
	OPL_STATUS retVal;
	UINT32 timeout=50000;
	UINT32 transdone=0;
    while (timeout--)
    {
        retVal = oplRegFieldRead(REG_SPI_INT, 2, 1, &transdone );
        if (transdone )
        {
           goto OUT1;
        }
    }
	OPL_PRINTF("chu  cuo  la......... : wait spi trans done timeout\n");
	return OPL_ERROR;
OUT1:
    return retVal;
}
OPL_STATUS  waitBusy( void )
{
	OPL_STATUS retVal;
	UINT32 timeout=500000;
	UINT32 busy=1;
	UINT32 transdone=1;
    while (timeout--)
    {
        retVal = oplRegFieldRead(REG_SPI_BUSY, 1, 1, &busy );
        if (!busy )
        {
           goto OUT1;
        }
    }
	OPL_PRINTF("chu  cuo  la......... : wait spi busy timeout\n");
	return OPL_ERROR;
OUT1:
    return retVal;
}
OPL_STATUS spiStartTrans(void)
{
    OPL_STATUS retVal;
	if(0 != waitBusy()){
	   OPL_PRINTF("spi bus is busy...\n");
	   return OPL_ERROR;
	  }
	retVal = oplRegFieldWrite(REG_SPI_BUSY, 1, 1, 1);
	return retVal;		
}
OPL_STATUS spiTransDataFill( int buff_num, OPL_UINT32_t len, void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     OPL_PRINTF("invaild buffer number\n");
	     return OPL_ERROR;
    	}	
	switch (((len>>8)-1)/32){
		case 0:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, *(DATA32 *)data);
			break;
		case 1:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA64 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA96 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA96 *)data)->data1);
			retVal = oplRegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA128 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA128 *)data)->data1);
			retVal = oplRegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA128 *)data)->data2);
			retVal = oplRegFieldWrite(REG_SPI_TX_(3+4*buff_num), 0, 32, ((DATA128 *)data)->data3);
			break;
		default:
			OPL_PRINTF("invalid length %d ...\n", len);
		}
#endif
	return retVal;
}
OPL_STATUS spiRcvDataGet( int buff_num, OPL_UINT32_t len,void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     OPL_PRINTF("invaild buffer number\n");
	     return OPL_ERROR;
    	}
	switch (((len>>8)-1)/32){
		case 0:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, (DATA32 *)data);
			break;
		case 1:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA64 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA96 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA96 *)data)->data1);
			retVal = oplRegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA128 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA128 *)data)->data1);
			retVal = oplRegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA128 *)data)->data2);
			retVal = oplRegFieldRead(REG_SPI_RX_(3+4*buff_num), 0, 32, &((DATA128 *)data)->data3);
			break;
		}
#endif
	return retVal;
}
void spiRegShow(void)
{
	int i= 0;
	OPL_PRINTF("  show all registers need by spi-----------\n");
	orr(0x28025);
	orr(0x2602a);
	for (i=0;i<0x24;i++)
	  {
	  if((i>0x18)&&(i<0x20)){
	    continue;
	  	}
	  orr(0x2000+i);
	  }
}
OPL_STATUS spiReadMan(void)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd ,tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;

	retVal = spiTransLenSet(0x8080);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x9f000000 ;
	
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();

	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);
	
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
	
	OPL_PRINTF("read spi flash manufacture ID = 0x%x\n",(tmp_data.data3<<8));
	
	return retVal;

}

/**
*@par Description
* This function read spi flash status.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiReadStatus(OPL_UINT32_t *status)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;

	retVal = spiTransLenSet(0x1010);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
		
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x0500 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x05000000 ;
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();

	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);
	
	/*just for debug*/
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));

	*status = (tmp_data.data0<<8)>>8 ;
//	OPL_PRINTF("read spi flash status  = 0x%x\n",tmp_data.data0);
	return retVal;

}

/**
*@par Description
* This function write spi flash status.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWriteStatus(OPL_UINT32_t status)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t delay = 0x100000;

	retVal = spiTransLenSet(0x8080);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0x01000000|(( status & 0xf )<<16)) ;
	
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
   
    while(delay--);
	/*just for debug*/
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
	spiStatusShow();	
	
	return retVal;

}

/**
*@par Description
* This function enable spi flash writing.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWriteEnable(void)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x0808);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x06 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x06000000 ;
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer  ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();

	/*just for debug*/	
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
//    spiWriteStatus(0x9c);
//	spiStatusShow();

//    OPL_PRINTF("enable spi flash write...\n");
	return retVal;

}

/**
*@par Description
* This function disable spi flash writing.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWriteDisable(void)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x0808);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x04 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x04000000 ;
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer  ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();
	
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	
	/*just for debug*/
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
	spiStatusShow();

    OPL_PRINTF("disable spi flash write...\n");
	return retVal;

}

/**
*@par Description
* This function erase a block of spi flash .
*
* @param [in] addr_ofs
*	   address offset of spi flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiEraseBlock(OPL_UINT32_t addr_ofs)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t flash_status;
	OPL_UINT32_t timeout=5000000;
	
    /*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x2020);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
		
	/*set cmd for spi flash*/
	(&cmd)->data0 =(0x20000000 | (addr_ofs*0x1000)) ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0x20000000 | (addr_ofs*0x10000)) ;
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	
	OPL_PRINTF("start erase spi flash block %d...",addr_ofs);
	
    while (timeout--)
    {
        retVal = spiReadStatus(&flash_status);
//		OPL_PRINTF("flash_status = 0x%x \n",flash_status);
		if(!(timeout%10))
		{
			OPL_PRINTF(".");
		}
        if (!(flash_status&0x3))
        {
           goto OUT1;
        }
    }
	
	OPL_PRINTF("\n chu  cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;
	
OUT1:
	OPL_PRINTF("done\n");
//	retVal = spiStatusShow();
	return retVal;

}

/**
*@par Description
* This function program one word to spi flash .
*
* @param [in] *value
*	   pointer  of the word to be programed in flash.
* @param [in] addr_ofs
*	   address offset of spi flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWordProgram(OPL_UINT32_t *value,OPL_UINT32_t addr_ofs)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t flash_status;
	OPL_UINT32_t timeout=5000000;
	
	/*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x4040);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
		
	/*set cmd for spi flash*/
	(&cmd)->data0 = *value;
	(&cmd)->data1 = (0x02000000 | (addr_ofs & 0x00ffffff)) ;
	(&cmd)->data2 = 0 ;
	(&cmd)->data3 = 0 ;
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
		
	while (timeout--)
	{
		retVal = spiReadStatus(&flash_status);

        if (!(flash_status&0x3))
        {
           goto OUT1;
        }

	}
	OPL_PRINTF("\n chu	cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;
	OUT1:
//		OPL_PRINTF("done\n");
		
	return retVal;

}



OPL_STATUS spiTestW(char * filename, int filesize)
{
	PRINT_HELP;
	spiProgramUb(filename, filesize);
}

/**
*@par Description
* This function program u-boot to spi flash .
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiProgramUb(char * filename, int filesize)
{
	OPL_STATUS retVal;
	OPL_UINT8_t *tmp_buf = NULL;
	int fd;
	int count;
	
	OPL_UINT32_t header1 = 0x030b0108 ;
	OPL_UINT32_t header2 = 0x10000000 ;
	OPL_UINT32_t header3 = 0x00000000 ;
	OPL_UINT32_t header4 = 0x00000000 ;
	
	/*open file*/
	fd = open(filename,O_RDONLY);

	if(fd < 0)
	{
	     OPL_PRINTF("open file failed\n");
	     return OPL_ERROR;
	}else{
		  OPL_PRINTF("open file ,fd = %d\n",fd);
		 }
	
	/*malloc a buffer */
	tmp_buf = kmalloc(filesize, GFP_KERNEL);
	
	if(tmp_buf == NULL)
	{
	     OPL_PRINTF("malloc buffer failed\n");
	     return OPL_ERROR;	
	}
	
	/*read file to buffer*/
	read(fd,tmp_buf,filesize);
	
	/*erase spi flash*/
	for(count=0;count<0x40;count++)
	{
	    spiEraseBlock(count);
	}

	/*program header to flash*/ 

	 retVal = spiWordProgram(&header1,0x0);
	 retVal = spiWordProgram(&header2,0x4);
	 retVal = spiWordProgram(&header3,0x8);
	 retVal = spiWordProgram(&header4,0xc);

	/*program file to flash*/
	for(count=0;count<(filesize/4);count++)
	{
		if (0 == (count%100)) OPL_PRINTF(".");
	    retVal = spiWordProgram((OPL_UINT32_t*)(tmp_buf+4*count),4*count+0x10);
	}

	OPL_PRINTF("\nprigram file finished.\n");

	close(fd);

	kfree(tmp_buf);
	
	return retVal;

}

/**
*@par Description
* This function show spi flash status .
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiStatusShow()
{
	OPL_STATUS retVal;
	OPL_UINT32_t status;
	OPL_UINT32_t timeout=5000000;
	
	retVal = spiReadStatus(&status);
	OPL_PRINTF("debug :read back status  = 0x%x\n",status);

	return retVal;
}

/**
*@par Description
* This function read spi flash .
*
* @param [in] addr
*	   address  to be read in flash.
* @param [in] size
*	   size  to be read in flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiRead(OPL_UINT32_t addr,OPL_UINT32_t size)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x8080);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0x03000000 | (addr & 0x00ffffff));
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(0);
	
	/*set spi buffer 1 ready*/
	retVal = spiBufReadySet(1);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();
	
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	
	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);
	
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));

	while(size--)
	{
	     retVal = spiReadyBufIdGet(&ready_buf_id);
	 
		 if(ready_buf_id == 0)
		 {
		      retVal = spiBufReadySet(1);
		 }else{
			   retVal = spiBufReadySet(0);
			  }					

	/*wait spi trans(rx)  finished*/
	 retVal = waitTransDone();

	 /*get data from rx buffer*/
	 retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);
	 
//     OPL_PRINTF("debug :read back data3	= 0x%x\n",(tmp_data.data3));
//	  OPL_PRINTF("debug :read back data2	= 0x%x\n",(tmp_data.data2));
//	  OPL_PRINTF("debug :read back data1	= 0x%x\n",(tmp_data.data1));
//	  OPL_PRINTF("debug :read back data0	= 0x%x\n",(tmp_data.data0));

			/*copy data  to the dest*/
//			memcpy()
	}
		
		return retVal;

}

/**
*@par Description
* This function read  data to a buffer.
*
* @param [in] addr
*	   address  to be read in flash.
* @param [in] *tmp_data
*	   pointer of the buffer .
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiReadtoBuf(OPL_UINT32_t addr,DATA128 *tmp_data)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x8080);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
		
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0x03000000 | (addr & 0x00ffffff));
	
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(0);
	
	/*set spi buffer 1 ready*/
	retVal = spiBufReadySet(1);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();
	
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	
	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,tmp_data);
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data->data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data->data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data->data0));

// 	 memcpy()	
	return retVal;

}

#if 1
OPL_STATUS spiTestRW(char * filename,int filesize)
{
	OPL_STATUS retVal;
	OPL_UINT8_t *tmp_buf = NULL;
	OPL_UINT8_t *read_buf = NULL;
	DATA128 rd_data;
	int fd;
	int i;

	spiTestW(filename, filesize);
	
	spiTestR(filename, filesize);
}


OPL_STATUS spiTestR(char * filename,int filesize)
{
	OPL_STATUS retVal;
	OPL_UINT8_t *tmp_buf = NULL;
	OPL_UINT8_t *read_buf = NULL;
	DATA128 rd_data;
	int fd;
	int i;

	PRINT_HELP;
	/*open file*/
	fd = open(filename,O_RDONLY);
	
	if(fd < 0)
	{
	     OPL_PRINTF("open file failed\n");
	     return OPL_ERROR;
	}
	else{
		OPL_PRINTF("open file ,fd = %d\n",fd);
		}
	
	/*malloc a buffer */
	tmp_buf = kmalloc(filesize, GFP_KERNEL);

	OPL_PRINTF("tmp_buff = 0x%8x\n",tmp_buf);

	if(tmp_buf == NULL)
	{
	     OPL_PRINTF("malloc buffer failed\n");
	     return OPL_ERROR;	
	}
	
	/*read file to buffer*/
	read(fd,tmp_buf,filesize);

     /*malloc a buffer */
	read_buf = kmalloc(filesize, GFP_KERNEL); 

	OPL_PRINTF("read_buff = 0x%8x\n",read_buf);

	if(read_buf == NULL)
	{
	     OPL_PRINTF("malloc read buffer failed\n");
	     return OPL_ERROR;	
	}

	for(i=0;i<filesize/12;i++)
	{  
        retVal = spiReadtoBuf(i*4*3 + 0x10, &rd_data);
		*(OPL_UINT32_t*)(read_buf +12*i) = rd_data.data2;
		*(OPL_UINT32_t*)((read_buf+4)+12*i) = rd_data.data1;
		*(OPL_UINT32_t*)((read_buf+8)+12*i) = rd_data.data0;
	}   

#if 0	
   *(OPL_UINT32_t*)(read_buf+8) = 0x12345678;
   *(OPL_UINT32_t*)(read_buf+0x10) = 0x12345678;
   *(OPL_UINT32_t*)(read_buf+0x1600) = 0x12345678;
#endif

	for(i=0;i<filesize/4;i++)
	{
	  if((*(OPL_UINT32_t*)(read_buf+i*4)) != (*(OPL_UINT32_t*)(tmp_buf+4*i)))
	  	{
		  	OPL_PRINTF("error:addrs 0x%x different....!\n",4*i);
			OPL_PRINTF("	  data in source = 0x%x\n",*(OPL_UINT32_t*)(tmp_buf+4*i));
	        OPL_PRINTF("      data read back = 0x%x\n",*(OPL_UINT32_t*)(read_buf+4*i));
	    }
	}

    close(fd);

	kfree(tmp_buf);
	kfree(read_buf);
	return retVal;

}

OPL_STATUS sp(void)
{
	OPL_STATUS retVal;
	OPL_UINT32_t tmp_val;
	int i;
	for(i=0;i<0x40;i++)
	{  
	    tmp_val = i;
        retVal = spiWordProgram(&tmp_val, 4*i);
	}
    return retVal;
}

OPL_STATUS sr48(OPL_UINT32_t flash_addr)
{
	OPL_STATUS retVal;
	int i;
	for(i=0;i<0x10;i++)
	{  
        retVal = spiRead(i*4*3+flash_addr, 0);
	}
    return retVal;
}

OPL_STATUS spi2ioDualAddrWrite(OPL_UINT32_t addr,OPL_UINT32_t data)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t delay = 0x100000;
	
//	OPL_UINT32_t tmp_data[3];
//	tmp_data=data;

	retVal = spiTransLenSet(0x4040);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 = data;
	(&cmd)->data1 =((DUALADDR_2IO_WRITE<<24| (addr * 4) & 0xffffff));
	(&cmd)->data2 = 0;
	(&cmd)->data3 = 0;
	
	printk("debug : set normal write command: 0x%x\n",(&cmd)->data1);
	printk("debug : write word: 0x%x\n",(&cmd)->data0);
	
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
   
	return retVal;

}


OPL_STATUS spi2ioSingleAddrWrite(OPL_UINT32_t addr,OPL_UINT32_t data)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t delay = 0x100000;
	
//	OPL_UINT32_t tmp_data[3];
//	tmp_data=data;

	retVal = spiTransLenSet(0x4040);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 = data;
	(&cmd)->data1 =((SINGADDR_2IO_WRITE<<24| (addr * 4) & 0xffffff));
	(&cmd)->data2 = 0;
	(&cmd)->data3 = 0;
	
	printk("debug : set normal write command: 0x%x\n",(&cmd)->data1);
	printk("debug : write word: 0x%x\n",(&cmd)->data0);
	
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
   
	return retVal;

}
#endif

/**
*@par Description
* This function write  one word to spi slave in normal write mode.
*
* @param [in] addr
*	   address  to be write in flash.
* @param [in] data
*	   data to be write .
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiNwriteOnewordToSlave(OPL_UINT32_t addr,OPL_UINT32_t word)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd ;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t delay = 0x100000;

	retVal = spiTransLenSet(0x4040);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 = word;
	(&cmd)->data1 =((NORMAL_WRITE<<24) | (addr * 4) & 0xffffff) ;
	(&cmd)->data2 = 0;
	(&cmd)->data3 = 0 ;
	
	printk("debug : set normal write command: 0x%x\n",(&cmd)->data1);
	printk("debug : write word: 0x%x\n",(&cmd)->data0);
	
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
   
	return retVal;

}

/**
*@par Description
* This function read  one word from spi slave in normal write mode.
*
* @param [in] addr
*	   address  to be write in slave.
* @param [in] *data
*	   pointer of the data read from spi salve .
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiNreadOnewordFromSlave(OPL_UINT32_t addr,OPL_UINT32_t *data)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
    DATA128 cmd;
	DATA64 tmp_data;
	OPL_UINT32_t len;
	OPL_UINT32_t ready_buf_id = 0;
	OPL_UINT32_t delay = 0x100000;

	retVal = spiTransLenSet(0x4848);
	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	
	/*set cmd for spi flash*/
	(&cmd)->data0 = 0;
	(&cmd)->data1 = ((addr * 4) << 8);
	(&cmd)->data2 = NORMAL_READ ;
	(&cmd)->data3 = 0 ;
	
	printk("debug : set normal read command: 0x%x\n",(&cmd)->data1);

    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	
	/*start spi trans*/
	retVal = spiStartTrans();
	
	/*wait for spi tx done*/
	retVal = waitBusy();

	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();

 	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);

	*data = tmp_data.data0;
	
	printk("debug :read back data0  = 0x%x\n",tmp_data.data0);
	
	return retVal;

}

/**
*@par Description
* This function write  data to spi slave in normal write mode.
*
* @param [in] regAddr
*	   register  to be write in slave.
* @param [in] *pDataBuf
*	   source address  of the data write to spi salve .
* @param [in] dataLen
*	  length of data to be write to slave
* @param [in] *pTxLen
*	  length of data  have been write to slave
*
 * @retval OPL_OK  This operation success.
*/


OPL_STATUS spiBytesWrite(OPL_UINT32_t regAddr, OPL_UINT8_t *pDataBuf, OPL_UINT32_t dataLen, OPL_UINT32_t *pTxLen)
{
	OPL_STATUS retVal;
	int count = 0;
	
    /**set ptxlen 0 before writing**/
	*pTxLen = 0;
	
	if( dataLen < 4 )
	{
	    OPL_PRINTF("ERROR: the data you want to write is less than 4 bytes. \n");
		return OPL_ERROR;
	}

	/*begain to write data word by word*/
	while( *pTxLen < dataLen )
	{
	    retVal = spiNwriteOnewordToSlave(regAddr,*(OPL_UINT32_t*)(pDataBuf + 4*count));
		
		if(retVal != OPL_OK)
		{
         OPL_PRINTF("ERROR: write one byte to slave failed. ptxlen = 0x%x\n",*pTxLen);
		 return OPL_ERROR;
		}
		
        /*when the last word less rhan 4 bytes*/
		if(((dataLen - *pTxLen)< 4))
		{
		    retVal = spiNwriteOnewordToSlave(regAddr,*(OPL_UINT32_t*)(pDataBuf + *pTxLen));
			
			if(retVal != OPL_OK)
		    {
                OPL_PRINTF("ERROR: write the last bytes failed. ptxlen = 0x%x\n",*pTxLen);
		        return OPL_ERROR;
		    }
			
			/* write  finished */
		    *pTxLen = dataLen;	
			continue;
		}
		
		*pTxLen += 4 ;
		count++;		
	}
	
    //OPL_PRINTF("write 0x%x bytes to reg : 0x%x OK.",*pTxLen,regAddr);
	return OPL_OK;

}

/**
*@par Description
* This function read  data from spi slave in normal write mode.
*
* @param [in] regAddr
*	   register  to be read in slave.
* @param [in] *pDataBuf
*	   source address  of the data read from spi salve .
* @param [in] dataLen
*	  length of data to be read from slave
* @param [in] *pRxLen
*	  length of data  have been read from slave
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiBytesRead(OPL_UINT32_t regAddr, OPL_UINT8_t *pDataBuf, OPL_UINT32_t dataLen, OPL_UINT32_t *pRxLen)
{
	OPL_STATUS retVal;
	int count = 0;
	
    /**set pRxlen 0 before writing**/
	*pRxLen = 0;
	
	if( dataLen < 4 )
	{
	    OPL_PRINTF("ERROR: the data you want to read is less than 4 bytes. \n");
		return OPL_ERROR;
	}

	if( dataLen % 4 )
	{
	    OPL_PRINTF("ERROR:  read must be 4 bytes align. \n");
		return OPL_ERROR;
	}

	/*begain to Read data word by word*/
	while( *pRxLen < dataLen )
	{
	    retVal = spiNreadOnewordFromSlave(regAddr,(OPL_UINT32_t*)(pDataBuf + 4*count));
		
		if(retVal != OPL_OK)
		{
         OPL_PRINTF("ERROR: write one bytes to slave failed. pRxLen = 0x%x\n",*pRxLen);
		 return OPL_ERROR;
		}
		
		*pRxLen += 4 ;
		count++;		
	}
	
    //OPL_PRINTF("Read 0x%x bytes from reg : 0x%x OK.",*pRxLen,regAddr);
	return OPL_OK;

}


/**
*@par Description
* This function transfer ddr training code to slave.
*
* @param [in] ramAddr
*	   address used by training code.
* @param [in] *pDataBuf
*	   source address of training code .
* @param [in] dataLen
*	   length of training code .
 *
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiSlaveTxDdrImage(OPL_UINT32_t ramAddr, OPL_UINT8_t *pDataBuf, OPL_UINT32_t dataLen)
{
	OPL_STATUS ret;
	OPL_UINT32_t totalTxLen;
	OPL_UINT32_t curTxLen;
	OPL_UINT32_t data;
	OPL_UINT32_t destAddr;

	totalTxLen = 0;
	
	while(totalTxLen < dataLen)
	{
	
		/* write spi slave address register */
		destAddr = ramAddr + totalTxLen;
		ret = spiBytesWrite(REG_SPI_SLAVE_ADDR, (OPL_UINT8_t *)&destAddr, sizeof(OPL_UINT32_t), &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_ADDR fail!\n");
		}

		/* write spi slave control register */
		//data = 3 << SPI_CTRL_CMD_OFFSET;
		data = 0x58000000;
		
		ret = spiBytesWrite(REG_SPI_SLAVE_CTRL, (OPL_UINT8_t *)&data, sizeof(data), &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_CTRL fail!\n");
		}
	
		/* write spi slave data register */
		
		ret = spiBytesWrite(REG_SPI_SLAVE_DATA, (pDataBuf + totalTxLen), 4, &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_DATA fail!\n");		
		}
		
		totalTxLen += curTxLen;
	}
}

/**
*@par Description
* This function transfer ddr booting code to slave.
*
* @param [in] ramAddr
*	   address used by booting code.
* @param [in] *pDataBuf
*	   source address of booting code .
* @param [in] dataLen
*	   length of booting code .
 *
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiSlaveTxBootImage(OPL_UINT32_t ramAddr, OPL_UINT8_t *pDataBuf, OPL_UINT32_t dataLen)
{
	OPL_STATUS ret;
	OPL_UINT32_t totalTxLen;
	OPL_UINT32_t curTxLen;
	OPL_UINT32_t data;
	OPL_UINT32_t destAddr;


	while(totalTxLen < dataLen)
	{
	
		/* write spi slave address register */
		destAddr = ramAddr + totalTxLen;
		ret = spiBytesWrite(REG_SPI_SLAVE_ADDR, (OPL_UINT8_t *)&destAddr, sizeof(OPL_UINT32_t), &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_ADDR fail!\n");
		}

		/* write spi slave control register */
		//data = 3 << SPI_CTRL_CMD_OFFSET;
		data = 0x98000000;
		
		ret = spiBytesWrite(REG_SPI_SLAVE_CTRL, (OPL_UINT8_t *)&data, sizeof(data), &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_CTRL fail!\n");
		}

		/* write spi slave data register */
		//ret = spiBytesWrite(REG_SPI_SLAVE_DATA, (pDataBuf + totalTxLen), 4, &curTxLen);
		ret = spiBytesWrite(REG_SPI_SLAVE_DATA, (pDataBuf + totalTxLen), (dataLen - totalTxLen), &curTxLen);
		if(ret != NO_ERROR)
		{
			printk("write REG_SPI_SLAVE_DATA fail!\n");		
		}
		
		totalTxLen += curTxLen;
	}
}

/**
*@par Description
* This function run ddr training code .
*
 *
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiSlaveRunDdrImage(void)
{
	OPL_UINT32_t data;
	OPL_UINT32_t txLen;
	OPL_UINT32_t rxLen;
	OPL_STATUS retVal;

	retVal = spiBytesRead(REG_SPI_SLAVE_CTRL, (OPL_UINT8_t *)&data, sizeof(data), &rxLen);
	if(retVal != NO_ERROR)
	{
		printk("read REG_SPI_SLAVE_CTRL fail!\n");
		return retVal;
	}

	/* prepare data */
	data = data | (1 << SPI_CTRL_RESET_OFFSET);

	/* write spi slave control reg */
	retVal = spiBytesWrite(REG_SPI_SLAVE_CTRL, (OPL_UINT8_t *)&data, sizeof(data), &txLen);
	if(retVal != NO_ERROR)
	{
		printk("write REG_SPI_SLAVE_CTRL fail !\n");
		return retVal;
	}

	return retVal;
}

/**
*@par Description
* This function set sram buffer start.
*
* @param [in] addr
*	   address of sram buffer.
 *
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiSlaveSramBufStartSet(OPL_UINT32_t addr)
{
	OPL_UINT32_t data;
	OPL_UINT32_t txLen;
	OPL_STATUS retVal;

	/* prepare data */
	data = addr;

	/* write spi slave control reg */
	retVal = spiBytesWrite(REG_SPI_SLAVE_SRAM_BUF_START, (OPL_UINT8_t *)&data, sizeof(data), &txLen);
	if(retVal != NO_ERROR)
	{
		printk("write REG_SPI_SLAVE_CTRL fail !\n");
		return retVal;
	}

	return retVal;
}

/**
*@par Description
* This function set sram page size.
*
* @param [in] pageSize
*	   pageSize of sram page.
 *
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiSlaveSramPageSizeSet(OPL_UINT32_t pageSize)
{
	OPL_UINT32_t data;
	OPL_UINT32_t txLen;
	OPL_STATUS retVal;

	/* prepare data */
	data = pageSize;

	/* write spi slave control reg */
	retVal = spiBytesWrite(REG_SPI_SLAVE_SRAM_PAGE_SIZE, (OPL_UINT8_t *)&data, sizeof(data), &txLen);
	if(retVal != NO_ERROR)
	{
		printk("write REG_SPI_SLAVE_CTRL fail !\n");
		return retVal;
	}

	return retVal;	
}


#if 1
void spiTestRegRead(OPL_UINT32_t regAddr)
{
	OPL_UINT8_t dataBuf[4];
	OPL_UINT32_t dataLen;
	OPL_UINT32_t rxLen;
	OPL_UINT8_t i;

	dataLen = 4;
	spiBytesRead(regAddr, &dataBuf, dataLen, &rxLen);

	printk("\nReg(0x%x) = ", regAddr);
	for(i = 0; i < 4; i++)
	{
		printk("%.2x ", dataBuf[i]);
	}
	printk("\n");
	
}

void spiTestRegWrite(OPL_UINT32_t regAddr, OPL_UINT32_t regValue)
{
	OPL_UINT32_t rxLen;
	spiBytesWrite(regAddr, &regValue, 4, &rxLen);
}

void spiSramClear()
{
	OPL_UINT8_t i;
	OPL_UINT8_t data[4] = {0};
	OPL_UINT32_t txLen;

	for(i = 0; i < 16; i++)
	{
		spiBytesWrite(0x3a000 + i, &data[0], 4, &txLen);
	}

	for(i = 0; i < 16; i++)
	{
		spiBytesWrite(0xa1e000 + i, &data[0], 4, &txLen);
	}

	return;
}

void spiSramRead()
{
	OPL_UINT8_t i;
	OPL_UINT32_t rxLen;
	OPL_UINT8_t data[4];

	for(i = 0; i < 16; i++)
	{
		spiBytesRead(0x3a000 + i, &data[0], 4, &rxLen);
		printk("%.2x %.2x %.2x %.2x ", data[0], data[1], data[2], data[3]);
		if((i % 4) == 3)
			printk("\n");
	}
}


void spiSramWrite(OPL_UINT8_t value)
{
	OPL_UINT8_t i;
	OPL_UINT32_t rxLen;
	OPL_UINT8_t data[4];

	for(i = 0; i < 4; i++)
	{
		data[i] = value;
	}

	for(i = 0; i < 16; i++)
	{
		spiBytesWrite(0x3a000 + i, &data[0], 4, &rxLen);
	}
}

/* slave mode : outer ddr parameter */
void spiDDRRead(OPL_UINT32_t baseAddr, OPL_UINT32_t length)
{
	OPL_UINT32_t i;
	OPL_UINT32_t txLen;
	OPL_UINT32_t rxLen;
	OPL_UINT32_t readCfg;
	OPL_UINT8_t data[4];

	readCfg = 0x88000000;

	spiTestRegWrite(0x80, 0);
	
	for(i = 0; i < length ; (i += 4))
	{
		if((i % 16) == 0)
		{
			printk("\n0x%.8x:  ", baseAddr);
		}
		
		spiBytesWrite(0x2050, &baseAddr, 4, &txLen);

		spiBytesWrite(0x2051, &readCfg, 4, &txLen);

		spiBytesRead(0x2052, &data[0], 4, &rxLen);
	
		baseAddr += 4;
		
		printk("%.2x %.2x %.2x %.2x ", data[0], data[1], data[2], data[3]);

	}
	
	spiTestRegWrite(0x80, 2);
}

void spiDDRWrite(OPL_UINT32_t baseAddr, OPL_UINT32_t value, OPL_UINT32_t length)
{
	OPL_UINT32_t i;
	OPL_UINT32_t txLen;
	OPL_UINT32_t writeCfg;
	OPL_UINT8_t data[4];

	writeCfg = 0x98000000;

	for(i = 0; i < 4; i++)
	{
		data[i] = value;
	}

	spiTestRegWrite(0x80, 0);

	for(i = 0; i < length; (i += 4))
	{
		//printf("write bytes %d - %d\n", i, i+3);
		
		spiBytesWrite(0x2050, &baseAddr, 4, &txLen);

		spiBytesWrite(0x2051, &writeCfg, 4, &txLen);

		spiBytesWrite(0x2052, &data[0], 4, &txLen);

		baseAddr += 4;
	}

	spiTestRegWrite(0x80, 2);
}
#endif
/** 
 *@} 
 */
 /* END FILE */

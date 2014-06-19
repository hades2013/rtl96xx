/*
=============================================================================
Header file Name:i2c.h

  General Description:
  
	This file define all opcomm chip regisger and table address.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		zzhu			2007/11/1		Initial Version
		zgan            2009/06/05      Implement codes
		---------------	---------------	----------------------------------------------
*/
#ifndef I2C_H
#define I2C_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

#define REG_BIT_I2CTOTE_OFF			    7
#define REG_BIT_I2CTOTE					(0x1 << REG_BIT_I2CTOTE_OFF)

#define REG_BIT_I2CADR_ADR_OFF		    1
#define REG_BIT_I2CADR_ADR				(0x7f << REG_BIT_I2CADR_ADR_OFF)
#define REG_BIT_I2CADR_RW_OFF			0
#define REG_BIT_I2CADR_RW				(0x1 << REG_BIT_I2CADR_RW_OFF)

#define REG_BIT_I2CCON_AA_OFF			7
#define REG_BIT_I2CCON_AA				(0x1 << REG_BIT_I2CCON_AA_OFF)
#define REG_BIT_I2CCON_ENSIO_OFF		6
#define REG_BIT_I2CCON_ENSIO			(0x1 << REG_BIT_I2CCON_ENSIO_OFF)
#define REG_BIT_I2CCON_STA_OFF		    5
#define REG_BIT_I2CCON_STA				(0x1 << REG_BIT_I2CCON_STA_OFF)
#define REG_BIT_I2CCON_STO_OFF		    4
#define REG_BIT_I2CCON_STO				(0x1 << REG_BIT_I2CCON_STO_OFF)
#define REG_BIT_I2CCON_SI_OFF			3
#define REG_BIT_I2CCON_SI				(0x1 << REG_BIT_I2CCON_SI_OFF)
#define REG_BIT_I2CCON_CR_OFF			0
#define REG_BIT_I2CCON_CR				(0x7 << REG_BIT_I2CCON_CR_OFF)

#define REG_BIT_I2CEN_RD_OFF			3
#define REG_BIT_I2CEN_RD				(FIELD_BITS(1) << REG_BIT_I2CEN_RD_OFF)
#define REG_BIT_I2CEN_WR_OFF			2
#define REG_BIT_I2CEN_WR				(FIELD_BITS(1) << REG_BIT_I2CEN_WR_OFF)
#define REG_BIT_I2CEN_INT_OFF			1
#define REG_BIT_I2CEN_INT				(FIELD_BITS(1) << REG_BIT_I2CEN_INT_OFF)
#define REG_BIT_I2CEN_MS_OFF			0
#define REG_BIT_I2CEN_MS				(FIELD_BITS(1) << REG_BIT_I2CEN_MS_OFF)


#define REG_BIT_I2CSR_TIP_OFF			1
#define REG_BIT_I2CSR_TIP				(FIELD_BITS(1) << REG_BIT_I2CSR_TIP_OFF)
#define REG_BIT_I2CSR_BUSY_OFF			6
#define REG_BIT_I2CSR_BUSY				(FIELD_BITS(1) << REG_BIT_I2CSR_BUSY_OFF)


/* only important in master mode, Clock Rate values: Serial Clock Frequncy(SCF) */
typedef enum
	{
	I2C_SCF_330,	/* 0 */
	I2C_SCF_288,	/* 1 */
	I2C_SCF_217,	/* 2 */
	I2C_SCF_146,	/* 3 */
	I2C_SCF_88,		/* 4 */
	I2C_SCF_59,		/* 5 */
	I2C_SCF_44,		/* 6 */
	I2C_SCF_36,		/* 7 */
	I2C_SCF_END
}I2C_SPEED_e;

/* Status Errors */
#define OPL_I2C_BUSY					    -1 /* transfer busy */
#define OPL_I2C_NO_DATA				        -3 /* err: No data in block */
#define OPL_I2C_NACK_ON_DATA			    -4 /* err: No ack on data */
#define OPL_I2C_NACK_ON_ADDRESS		        -5 /* err: No ack on address */
#define OPL_I2C_DEVICE_NOT_PRESENT	        -6 /* err: Device not present */

#define OPL_I2C_ARBITRATION_LOST		    -7 /* err: Arbitration lost */
#define OPL_I2C_TIME_OUT				    -8 /* err: Time out occurred */
#define OPL_I2C_SLV_WRITE_MORE_DATA_NEED	-9
#define OPL_I2C_SLV_READ_BUFFER_TOO_SMALL	-10

/* typedef  */
#define 	I2C_READ		1
#define 	I2C_WRITE		0

#define OT_REG_I2C_READ_FLAG_SET 		bNoStopIssue = OPL_TRUE
#define OT_REG_I2C_READ_FLAG_CLEAR	    bNoStopIssue = OPL_FALSE

/* Optical Tansceiver EPROM on ONU board' salve address, which is transfer by I2C*/
#define OT_EPROM_I2C_ADDR          0x50
#define OT_I2C_ADDR                0x00

#pragma pack(1)		/* pragma pack definition */

/* I2C device mode configuration */
typedef struct I2C_SYS_CONFIG_s
{
	UINT8 masterMode	:1;		/* 1:master , 0:slave */
	UINT8 interrupt		:1;		/* 1:interrupt, 0:polling */
	UINT8 status;
	UINT8 lastStatus;
} I2C_SYS_CONFIG_t;

#pragma pack()

int i2cInit (int i2cMode, int intMode, UINT8 slvAddr, int nRate);
int i2cWriteRegByteData (UINT8 slvAddr, UINT8 regAddr, UINT8 byteData );
int i2cReadRandom (UINT8 slvAddr, UINT8 staRegAddr, UINT32 nByte, UINT8 *pDataBuf );

#endif


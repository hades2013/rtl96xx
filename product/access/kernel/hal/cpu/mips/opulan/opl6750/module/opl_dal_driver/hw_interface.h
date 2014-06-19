/*
=============================================================================
     Header Name: hw_interface.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/10		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef HW_INTERFACE_H
#define HW_INTERFACE_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "opconn_hw_reg.h"

#if 0
#define VOQ_SUPPORT								1
#endif
#define PON_MAC_TAB_SUPPORT				1

#define OPCONN_SIZE   32
#define FIELD_BITS(n)				(0xffffffff >> (OPCONN_SIZE - (n)))
#define OPCONN_BIT(n)				(1<<(n))
#define DBG_LINE 

typedef struct OPCONN_REG_INFO_s
{
	UINT32	regAddr;
	UINT32 	rwMask;
	UINT32 	rsvMask;
	UINT32 	rMask;
	UINT32 	wMask;
	UINT32  	rcMask;
	UINT32 	defaultVal;
	UINT8 		*desc;
}OPCONN_REG_INFO_t;

typedef struct OPCONN_TAB_INFO_s
{
	UINT32 	tabId;
	UINT32  	numOfRec;
	UINT32 	widthOfRec;
	UINT8 		*desc;
}OPCONN_TAB_INFO_t;

typedef enum OPCONN_TAB_s
{
	OPCONN_TAB_CLS_MASK = OPL_ZERO,
	OPCONN_TAB_CLS_ADDR,
	OPCONN_TAB_CLS_RULE,
	OPCONN_TAB_CLS_COUNTER,
	OPCONN_TAB_PON_MAC,
	OPCONN_TAB_US_SHPBD,
	OPCONN_TAB_DS_SHPBD,
	OPCONN_TAB_US_WRED,
	OPCONN_TAB_DS_WRED,
	OPCONN_TAB_Q_CONF_STA,
	OPCONN_TAB_DMA0_RX,
	OPCONN_TAB_DMA0_TX,
	OPCONN_TAB_DMA1_RX,
	OPCONN_TAB_DMA1_TX,
	OPCONN_TAB_END
}OPCONN_TAB_e;

typedef struct OPCONN_COUNTER_s
{
	UINT8 *name;
	UINT32 regId;
	UINT32 regAddr;
	UINT32 regMask;
	UINT8 *desc;
}OPCONN_COUNTER_t;

typedef struct OPCONN_TAB_CTL_REG_MAP_s
{
	UINT32 tabId;
	UINT32 regCtlAddr;
	UINT32 dataAddr;
}OPCONN_TAB_CTL_REG_MAP_t;

#define BITS_PER_BYTE               8

/*	all table records length	*/
#define TAB_CLS_MASK_LEN    					24
#define TAB_CLS_ADDR_LEN 	  					128	
#define TAB_CLS_RULE_LEN 	  					128
#define TAB_CLS_COUNTER_LEN				128
#define TAB_BRG_VTT_LEN  						16
#define TAB_PON_MAC_LEN						64

#define TAB_US_SHPBD_LEN						16 //respin
#define TAB_US_SHPBD_PIR_START						8
#define TAB_DS_SHPBD_LEN						9

#define TAB_US_WRED_LEN						8
#define TAB_DS_WRED_LEN						8

#define TAB_Q_CONF_STA_LEN					16
#define TAB_DMA0_RX_LEN						64
#define TAB_DMA0_TX_LEN						64
#define TAB_DMA1_RX_LEN						64
#define TAB_DMA1_TX_LEN						64

/*	all table records width 	*/
#define TAB_CLS_MASK_WIDTH    				7
#define TAB_CLS_ADDR_WIDTH	  				1
#define TAB_CLS_RULE_WIDTH   				8/*27*//*28*/
#define TAB_CLS_COUNTER_WIDTH			1
#define TAB_BRG_VTT_WIDTH 					1	
#define TAB_PON_MAC_WIDTH					2/*7*//*8*/
#define TAB_US_SHPBD_WIDTH					1
#define TAB_DS_SHPBD_WIDTH					1

#define TAB_US_WRED_WIDTH					2
#define TAB_DS_WRED_WIDTH					2

#define TAB_Q_CONF_STA_WIDTH				2
#define TAB_DMA0_RX_WIDTH					1
#define TAB_DMA0_TX_WIDTH					1
#define TAB_DMA1_RX_WIDTH					1
#define TAB_DMA1_TX_WIDTH					1

#define TAB_MAX_WIDTH							(10)

/* opconn Table dimension:legth,in records */

/* opconn Table dimension:width,in records */
#define MDIO_READ				0X02
#define MDIO_WRITE				0X01

#define MDIO_BUSY				0X1
#define MDIO_TIMEOUT			1000

#define BRG_MIA_BUSY_OFFSET		(31)
#define BRG_MIA_TYPE_OFFSET		(20)
#define BRG_MIA_TSEL_OFFSET			(16)
#define MIA_TABLE_INDEX					(0)

#define SHPBD_MIA_BUSY_OFFSET		(31)
#define SHPBD_MIA_TYPE_OFFSET		(30)

#define DMA_MIA_BUSY_OFFSET		(31)
#define DMA_MIA_TYPE_OFFSET		(30)

#define Q_MIA_BUSY_OFFSET			(31)
#define Q_MIA_TYPE_OFFSET			(30)

#define MIA_OP_EXEC						(1)
#define MIA_READ_OP						(0)
#define MIA_WRITE_OP						(1)

#ifdef OPCONN_SIM_MIPS
OPL_STATUS oplSimInit(void);

OPL_STATUS oplSimFree(void);

OPL_STATUS oplRegSimDbInit(void);

OPL_STATUS oplTabSimDbInit(void);

OPL_STATUS oplRegSimDbFree(void);

OPL_STATUS oplTabSimDbFree(void);

OPL_STATUS oplRegSimRead(UINT32 regId, UINT32 *regVal);

OPL_STATUS oplRegSimWrite(UINT32 regId, UINT32 regVal);

OPL_STATUS oplTabSimRead(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal);

OPL_STATUS oplTabSimWrite(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal);

#endif


OPL_STATUS oplDelay(UINT32 count);

OPL_STATUS string2mac(UINT8 *macString, UINT8 *mac);

OPL_STATUS oplRegRead(UINT32 regAddr,UINT32 *regVal);

OPL_STATUS oplRegWrite(UINT32 regAddr,UINT32 regVal);

OPL_STATUS oplRegFieldRead(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 *data0);

OPL_STATUS oplRegFieldWrite(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 data0);
	
OPL_STATUS oplTabRead(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal);

OPL_STATUS oplTabWrite(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal);

OPL_STATUS mrr(UINT32 regAddr,UINT32 num);

OPL_STATUS mwr(UINT32 regAddr,UINT32 num,UINT32 newVal,UINT8 mode);

OPL_STATUS ort(OPCONN_TAB_e region,UINT32 startId,UINT32 num);

OPL_STATUS owt(UINT32 tabId,UINT32 startRecId,UINT32 numOfRec,UINT32 offset,UINT32 value);

INT32 convert20xValue(UINT8 *p,UINT32 len,UINT32 *pR);

void oplRegAccessLock(void);

void oplRegAccessUnLock(void);

OPL_STATUS ost(void);

static inline int oplRegBitTest (UINT32 regId, UINT32 bits)
{
    UINT32 nTemp = 0;
	if (regId % 4 != 0)
		return OPL_ERROR;

        oplRegRead (regId, &nTemp);
        return (nTemp & bits);
}

static inline void oplRegBitSet(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp |= (bits);
	oplRegWrite (regId, nTemp);
}

static inline void oplRegBitClear(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp &= (~(bits));
	oplRegWrite (regId, nTemp);
}

static inline void oplRegBitToggle(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	UINT32 nTemp1 = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp1 = nTemp;
	nTemp &= (bits);
	nTemp1 &= (~(bits));
	nTemp1 |= (~nTemp);
	oplRegWrite (regId, nTemp1);
}

#endif

/*
=============================================================================
     Header Name: hw_interface.c

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

#include <linux/mutex.h>

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"
#include "peripheral.h"
#include "opconn_lib.h"

#if 0
#define NOT_USING_FIELDMASK  1
#define HW_DEBUG 1
#define OPCONN_SIM_MIPS  1
#endif

struct mutex		g_pstRegAccessLock;
struct mutex		g_pstTabAccessLock;

#define OPL_REG_ACCESS_LOCK mutex_lock(&g_pstRegAccessLock)
#define OPL_REG_ACCESS_UNLOCK mutex_unlock(&g_pstRegAccessLock)

#define OPL_TAB_ACCESS_LOCK mutex_lock(&g_pstTabAccessLock)
#define OPL_TAB_ACCESS_UNLOCK mutex_unlock(&g_pstTabAccessLock)


OPCONN_TAB_INFO_t opconn_tab_info[OPCONN_TAB_END+1] = 
{
		{OPCONN_TAB_CLS_MASK,					TAB_CLS_MASK_LEN,			TAB_CLS_MASK_WIDTH,				"clsMask"},
		{OPCONN_TAB_CLS_ADDR,					TAB_CLS_ADDR_LEN,			TAB_CLS_ADDR_WIDTH,				"clsAddr"},
		{OPCONN_TAB_CLS_RULE,					TAB_CLS_RULE_LEN,			TAB_CLS_RULE_WIDTH,				"clsRule"},
		{OPCONN_TAB_CLS_COUNTER,			TAB_CLS_COUNTER_LEN,	TAB_CLS_COUNTER_WIDTH,		"clsCounter"},
		{OPCONN_TAB_PON_MAC,					TAB_PON_MAC_LEN,			TAB_PON_MAC_WIDTH,				"ponMac"},
		{OPCONN_TAB_US_SHPBD,				TAB_US_SHPBD_LEN,			TAB_US_SHPBD_WIDTH,				"upStream shaper bd tab"},
		{OPCONN_TAB_DS_SHPBD,				TAB_DS_SHPBD_LEN,			TAB_DS_SHPBD_WIDTH,				"downStream shaper bd tab"},
		{OPCONN_TAB_US_WRED,					TAB_US_WRED_LEN,			TAB_US_WRED_WIDTH,				"upStream wred bd tab"},
		{OPCONN_TAB_DS_WRED,					TAB_DS_WRED_LEN,			TAB_DS_WRED_WIDTH,				"downStream wred bd tab"},
		{OPCONN_TAB_Q_CONF_STA,			TAB_Q_CONF_STA_LEN,		TAB_Q_CONF_STA_WIDTH,			"up and down stream queue set info and status"},
		{OPCONN_TAB_DMA0_RX,					TAB_DMA0_RX_LEN,			TAB_DMA0_RX_WIDTH,				"dma0 rx"},
		{OPCONN_TAB_DMA0_TX,					TAB_DMA0_TX_LEN,			TAB_DMA0_TX_WIDTH,				"dma0 tx"},
		{OPCONN_TAB_DMA1_RX,					TAB_DMA1_RX_LEN,			TAB_DMA1_RX_WIDTH,				"dma1 rx"},
		{OPCONN_TAB_DMA1_TX,					TAB_DMA1_RX_LEN,			TAB_DMA1_TX_WIDTH,				"dma1 tx"},
		{OPCONN_TAB_END,							OPL_ZERO,							OPL_ZERO,									OPL_NULL}
};

static OPCONN_TAB_CTL_REG_MAP_t opconn_tab_control_map[OPCONN_TAB_END+1] = 
{
		{OPCONN_TAB_CLS_MASK,			REG_CLE_MEM_INDIR_ACC_CTRL				,	REG_CLE_SHADOW_SET0},
		{OPCONN_TAB_CLS_ADDR,			REG_CLE_MEM_INDIR_ACC_CTRL				,	REG_CLE_SHADOW_SET0},
		{OPCONN_TAB_CLS_RULE,			REG_CLE_MEM_INDIR_ACC_CTRL				,	REG_CLE_SHADOW_SET0},
		{OPCONN_TAB_CLS_COUNTER,	REG_CLE_MEM_INDIR_ACC_CTRL				,	REG_CLE_SHADOW_SET0},
		{OPCONN_TAB_PON_MAC,			REG_CLE_MEM_INDIR_ACC_CTRL				,	REG_CLE_SHADOW_SET0},
		{OPCONN_TAB_US_SHPBD,		REG_TMUS_MEM_INDIR_ACC_CTRL_SHPBD,	REG_TMUS_SHADOW_SET_SHPBD},
		{OPCONN_TAB_DS_SHPBD,		REG_TMDS_MEM_INDIR_ACC_CTRL_SHPBD,	REG_TMDS_SHADOW_SET_SHPBD},
		
		{OPCONN_TAB_US_WRED,			REG_TMUS_MEM_INDIR_ACC_CTRL_WRED,	REG_TMUS_SHADOW_SET_WRED_H},
		{OPCONN_TAB_DS_WRED,			REG_TMDS_MEM_INDIR_ACC_CTRL_WRED,	REG_TMDS_SHADOW_SET_WRED_H},
		
		{OPCONN_TAB_Q_CONF_STA,	REG_TM_MEM_INDIR_ACC_CTRL_QSTAB,		REG_TM_SHADOW_SET_QSTAB},
		{OPCONN_TAB_DMA0_RX,			REG_DMA0_MEM_INDIR_ACC_CTRL_RXBD	,	REG_DMA0_SHADOW_SET_RXBD},
		{OPCONN_TAB_DMA0_TX,			REG_DMA0_MEM_INDIR_ACC_CTRL_TXBD	,	REG_DMA0_SHADOW_SET_TXBD},
		{OPCONN_TAB_DMA1_RX,			REG_DMA1_MEM_INDIR_ACC_CTRL_RXBD	,	REG_DMA1_SHADOW_SET_RXBD},
		{OPCONN_TAB_DMA1_TX,			REG_DMA1_MEM_INDIR_ACC_CTRL_TXBD	,	REG_DMA1_SHADOW_SET_TXBD},
		{OPCONN_TAB_END,					OPL_ZERO,														OPL_ZERO}
};

OPCONN_TAB_INFO_t *oplTabAttrInfoGet(void)
{
    return &opconn_tab_info[0];
}

OPL_STATUS oplHwAcessInit(void)
{
	mutex_init(&g_pstRegAccessLock);
	mutex_init(&g_pstTabAccessLock);
	mdioRegisterInit();

    return OPL_OK;
}

void oplRegAccessLock(void)
{
	mutex_lock(&g_pstRegAccessLock);
}

void oplRegAccessUnLock(void)
{
	mutex_unlock(&g_pstRegAccessLock);
}

#ifdef OPCONN_SIM_MIPS
UINT32 *pRegSimDb = OPL_NULL;

UINT32 clsMaskTabSimDb[TAB_CLS_MASK_LEN][TAB_CLS_MASK_WIDTH];
UINT32 clsAddrTabSimDb[TAB_CLS_ADDR_LEN][TAB_CLS_ADDR_WIDTH];
UINT32 clsRuleTabSimDb[TAB_CLS_RULE_LEN][TAB_CLS_RULE_WIDTH];
UINT32 clsCounterTabSimDb[TAB_CLS_COUNTER_LEN][TAB_CLS_COUNTER_WIDTH];
UINT32 brgVttTabSimDb[TAB_BRG_VTT_LEN][TAB_BRG_VTT_WIDTH];
UINT32 ponMacTabSimDb[TAB_PON_MAC_LEN][TAB_PON_MAC_WIDTH];
UINT32 usShpbdTabSimDb[TAB_US_SHPBD_LEN][TAB_US_SHPBD_WIDTH];
UINT32 dsShpbdTabSimDb[TAB_DS_SHPBD_LEN][TAB_DS_SHPBD_WIDTH];
UINT32 dma0RxTabSimDb[TAB_DMA0_RX_LEN][TAB_DMA0_RX_WIDTH];
UINT32 dma0TxTabSimDb[TAB_DMA0_TX_LEN][TAB_DMA0_TX_WIDTH];
UINT32 dma1RxTabSimDb[TAB_DMA1_RX_LEN][TAB_DMA1_RX_WIDTH];
UINT32 dma1TxTabSimDb[TAB_DMA1_TX_LEN][TAB_DMA1_TX_WIDTH];

OPL_STATUS oplSimInit(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegSimDbInit();
	if(OPL_OK != retVal)
	{
		return OPL_ERROR;
	}
	retVal = oplTabSimDbInit();
	if(OPL_OK != retVal)
	{
		return OPL_ERROR;
	}
	return retVal;
}
OPL_STATUS oplSimFree(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegSimDbFree();
	if(OPL_OK != retVal)
	{
		return OPL_ERROR;
	}
	retVal = oplTabSimDbFree();
	if(OPL_OK != retVal)
	{
		return OPL_ERROR;
	}
	return retVal;
}

OPL_STATUS oplRegSimDbInit(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 *pBuff = OPL_NULL;

	pBuff = OPL_MALLOC(0X9000*4);
	if(OPL_NULL == pBuff)
	{
		OPL_DRV_PRINTF(("reg sim init failed.\n"));
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}
	pRegSimDb = (UINT32 *)pBuff;

	OPL_BZERO(pRegSimDb,0X9000*4);
	return retVal;
}

OPL_STATUS oplTabSimDbInit(void)
{
	OPL_STATUS retVal = OPL_OK;

	OPL_MEMSET(clsMaskTabSimDb,0X00,TAB_CLS_MASK_LEN*TAB_CLS_MASK_WIDTH);
	OPL_MEMSET(clsAddrTabSimDb,0X00,TAB_CLS_ADDR_LEN*TAB_CLS_ADDR_WIDTH);
	OPL_MEMSET(clsRuleTabSimDb,0X00,TAB_CLS_RULE_LEN*TAB_CLS_RULE_WIDTH);
	OPL_MEMSET(clsCounterTabSimDb,0X00,TAB_CLS_COUNTER_LEN*TAB_CLS_COUNTER_WIDTH);
	OPL_MEMSET(brgVttTabSimDb,0X00,TAB_BRG_VTT_LEN*TAB_BRG_VTT_WIDTH);
	OPL_MEMSET(ponMacTabSimDb,0X00,TAB_PON_MAC_LEN*TAB_PON_MAC_WIDTH);
	OPL_MEMSET(usShpbdTabSimDb,0x00,TAB_US_SHPBD_LEN*TAB_US_SHPBD_WIDTH);
	OPL_MEMSET(dsShpbdTabSimDb,0x00,TAB_DS_SHPBD_LEN*TAB_DS_SHPBD_WIDTH);
	OPL_MEMSET(dma0RxTabSimDb,0X00,TAB_DMA0_RX_LEN*TAB_DMA0_RX_WIDTH);
	OPL_MEMSET(dma0TxTabSimDb,0X00,TAB_DMA0_TX_LEN*TAB_DMA0_TX_WIDTH);
	OPL_MEMSET(dma0RxTabSimDb,0X00,TAB_DMA1_RX_LEN*TAB_DMA1_RX_WIDTH);
	OPL_MEMSET(dma0TxTabSimDb,0X00,TAB_DMA1_TX_LEN*TAB_DMA1_TX_WIDTH);
	return retVal;
}

OPL_STATUS oplRegSimDbFree(void)
{
	OPL_FREE(pRegSimDb);
	return OPL_OK;
}

OPL_STATUS oplTabSimDbFree(void)
{
	return OPL_OK;
}

OPL_STATUS oplRegSimRead(UINT32 regId, UINT32 *regVal)
{
	*regVal = pRegSimDb[regId/4];
	return OPL_OK;
}

OPL_STATUS oplRegSimWrite(UINT32 regId, UINT32 regVal)
{
	pRegSimDb[regId/4] = regVal;
	return OPL_OK;
}

OPL_STATUS oplTabSimRead(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 *pSrc = OPL_NULL;
	UINT32 *pDst = OPL_NULL;
	UINT32 tabId;
	UINT32 numOfRec;
	UINT32 widthOfRec;
	UINT32 recIndex;
	UINT32 count;

	if(OPL_NULL == pVal)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}	

	if(region>=OPCONN_TAB_END)
	{
		OPL_DRV_PRINTF(("no such module.\n"));
		return OPL_ERROR;
	}
	
	tabId 			= opconn_tab_info[region].tabId;
	numOfRec 	= opconn_tab_info[region].numOfRec;
	widthOfRec 	= opconn_tab_info[region].widthOfRec;

	if((startId+num)>numOfRec)
	{
		OPL_DRV_PRINTF(("out of range.\n"));
		return OPL_ERROR;
	}

	switch(region)
	{
		case OPCONN_TAB_CLS_MASK:
			pSrc = &clsMaskTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_ADDR:
			pSrc = &clsAddrTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_RULE:
			pSrc = &clsRuleTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_COUNTER:
			pSrc = &clsCounterTabSimDb[0];
			break;
		case OPCONN_TAB_PON_MAC:
			pSrc = &ponMacTabSimDb[0];
			break;
		case OPCONN_TAB_US_SHPBD:
			pSrc = &usShpbdTabSimDb[0];
			break;
		case OPCONN_TAB_DS_SHPBD:
			pSrc = &dsShpbdTabSimDb[0];
			break;		
		case OPCONN_TAB_DMA0_RX:
			pSrc = &dma0RxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA0_TX:
			pSrc = &dma0TxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA1_RX:
			pSrc = &dma1RxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA1_TX:
			pSrc = &dma1TxTabSimDb[0];
			break;	
		default:
			break;
	}
	
	pDst = pVal;
	for(recIndex = startId; recIndex < startId + num; recIndex++)
	{
		for(count = widthOfRec; count > 0; count--)
		{
			pDst[0] = pSrc[widthOfRec*recIndex  + count - 1];
			pDst++;
		}
	}
	return retVal;
}
OPL_STATUS oplTabSimWrite(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 *pSrc = OPL_NULL;
	UINT32 *pDst = OPL_NULL;
	UINT32 tabId;
	UINT32 numOfRec;
	UINT32 widthOfRec;
	UINT32 recIndex;
	UINT32 count;

	if(OPL_NULL == pVal)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}	

	if(region>=OPCONN_TAB_END)
	{
		OPL_DRV_PRINTF(("no such module.\n"));
		return OPL_ERROR;
	}
	
	tabId 			= opconn_tab_info[region].tabId;
	numOfRec 	= opconn_tab_info[region].numOfRec;
	widthOfRec = opconn_tab_info[region].widthOfRec;

	if((startId+num)>numOfRec)
	{
		OPL_DRV_PRINTF(("out of range.\n"));
		return OPL_ERROR;
	}

	switch(region)
	{
		case OPCONN_TAB_CLS_MASK:
			pDst = &clsMaskTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_ADDR:
			pDst = &clsAddrTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_RULE:
			pDst = &clsRuleTabSimDb[0];
			break;
		case OPCONN_TAB_CLS_COUNTER:
			pDst = &clsCounterTabSimDb[0];
			break;
		case OPCONN_TAB_PON_MAC:
			pDst = &ponMacTabSimDb[0];
			break;
		case OPCONN_TAB_US_SHPBD:
			pSrc = &usShpbdTabSimDb[0];
			break;
		case OPCONN_TAB_DS_SHPBD:
			pSrc = &dsShpbdTabSimDb[0];
			break;
		case OPCONN_TAB_DMA0_RX:
			pDst = &dma0RxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA0_TX:
			pDst = &dma0TxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA1_RX:
			pDst = &dma1RxTabSimDb[0];
			break;
		case OPCONN_TAB_DMA1_TX:
			pDst = &dma1TxTabSimDb[0];
			break;		
		default:
			break;
	}
	
	pSrc = pVal;
	for(recIndex = startId; recIndex < startId + num; recIndex++)
	{
		for(count = widthOfRec; count > 0; count--)
		{
			pDst[widthOfRec*recIndex  + count - 1] = pSrc[0];
			pSrc++;
		}
	}
	
	return retVal;
}
#endif
OPL_STATUS oplDelay(UINT32 count)
{
	while(count--)//hujian modified for no delay bug
	{
		count = count;
	}
	return OPL_OK;
}

#ifdef OPCONN_SIM_MIPS
OPL_STATUS oplRegRead(UINT32 regAddr,UINT32 *regVal)
{
	return oplRegSimRead(regAddr,regVal);
}

OPL_STATUS oplRegWrite(UINT32 regAddr,UINT32 regVal)
{
	return oplRegSimWrite(regAddr,regVal);
}
OPL_STATUS oplTabRead(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	return oplTabSimRead( region, startId, num, pVal);
}
OPL_STATUS oplTabWrite(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	return oplTabSimWrite( region,  startId,  num,  pVal);
}
#else
OPL_STATUS oplRegRead(UINT32 regAddr,UINT32 *regVal)
{
	if(OPL_NULL == regVal)
	{
		return OPL_ERR_NULL_POINTER;
	}
	if(regAddr%4)
	{
		OPL_DRV_PRINTF_ERR(("invalid regiser addr.\n"));
		return OPL_ERROR;
	}
    OPL_REG_ACCESS_LOCK;
	*regVal = *(volatile UINT32 *)(ONU_REGBASE + regAddr);	
    OPL_REG_ACCESS_UNLOCK;
	return OPL_OK;
}

OPL_STATUS oplRegWrite(UINT32 regAddr,UINT32 regVal)
{
	OPL_STATUS retVal = OPL_OK;
	
	if(regAddr%4)
	{
		OPL_DRV_PRINTF_ERR(("invalid regiser addr.\n"));
		return OPL_ERROR;
	}
    OPL_REG_ACCESS_LOCK;
	*(UINT32 *)(ONU_REGBASE + regAddr) = regVal;
    OPL_REG_ACCESS_UNLOCK;
	return retVal;
}
OPL_STATUS oplTabRead(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 tabId;
	UINT32 numOfRec;
	UINT32 widthOfRec;
	UINT32 psrc;
	UINT32 *pdst;
	UINT32 recIndex;
	UINT32 count;
	UINT32 regCtrl = OPL_ZERO;
	UINT32 regCtrlPerRec;
	UINT32 timeOut = 1000;
	UINT32 opBusy = 1;
	UINT32 regCtlAddr;
	UINT32 shadowDataAddr;
	
    OPL_TAB_ACCESS_LOCK;

	if(OPCONN_TAB_CLS_MASK > region || OPCONN_TAB_END <= region || OPL_NULL == pVal)
	{
		OPL_TRACE();
		//OPL_DRV_PRINTF_ERR(("region = %x startId = %d num = %d pval = %08x\n",region,startId,num,(UINT32)pVal));
		retVal = OPL_ERR_INVALID_PARAMETERS;
        goto exit_label;
	}

	tabId 			= opconn_tab_info[region].tabId;
	numOfRec 	= opconn_tab_info[region].numOfRec;
	widthOfRec	= opconn_tab_info[region].widthOfRec;

	if(numOfRec < (startId + num))
	{
		//OPL_DRV_PRINTF_ERR();
		retVal = OPL_ERR_INVALID_PARAMETERS;
        goto exit_label;
	}

	switch(tabId)
	{
		case OPCONN_TAB_CLS_MASK:
		case OPCONN_TAB_CLS_ADDR:
		case OPCONN_TAB_CLS_RULE:
		case OPCONN_TAB_CLS_COUNTER:
			regCtrl = (MIA_OP_EXEC<<BRG_MIA_BUSY_OFFSET)|(MIA_READ_OP<<BRG_MIA_TYPE_OFFSET)|((tabId&0x0f)<<BRG_MIA_TSEL_OFFSET);
			break;
		case OPCONN_TAB_PON_MAC:
			regCtrl = (MIA_OP_EXEC<<BRG_MIA_BUSY_OFFSET)|(MIA_READ_OP<<BRG_MIA_TYPE_OFFSET)|((tabId+1)<<BRG_MIA_TSEL_OFFSET);
			break;
		case OPCONN_TAB_US_SHPBD:
		case OPCONN_TAB_DS_SHPBD:
			regCtrl = (MIA_OP_EXEC<<SHPBD_MIA_BUSY_OFFSET)|(MIA_READ_OP<<SHPBD_MIA_TYPE_OFFSET);
			break;
		case OPCONN_TAB_US_WRED:
		case OPCONN_TAB_DS_WRED:
			regCtrl = (MIA_OP_EXEC<<SHPBD_MIA_BUSY_OFFSET)|(MIA_READ_OP<<SHPBD_MIA_TYPE_OFFSET);
			break;
		case OPCONN_TAB_DMA0_RX:
		case OPCONN_TAB_DMA0_TX:
		case OPCONN_TAB_DMA1_RX:
		case OPCONN_TAB_DMA1_TX:
			regCtrl = (MIA_OP_EXEC<<DMA_MIA_BUSY_OFFSET)|(MIA_READ_OP<<DMA_MIA_TYPE_OFFSET);
			break;
		case OPCONN_TAB_Q_CONF_STA:
			regCtrl = (MIA_OP_EXEC<<Q_MIA_BUSY_OFFSET)|(MIA_READ_OP<<Q_MIA_TYPE_OFFSET);
			break;
		default:
			//OPL_DRV_PRINTF(("no such table."));
			break;
	}
	
	regCtlAddr 			= opconn_tab_control_map[tabId].regCtlAddr;
	shadowDataAddr	= opconn_tab_control_map[tabId].dataAddr;
	pdst = pVal;

	/*first we need check if the table access is busy*/
	while(opBusy)
	{
#ifdef NOT_USING_FIELDMASK
		retVal = oplRegFieldRead(regCtlAddr, 31, 1, &opBusy);
#else
		retVal = oplRegRead(regCtlAddr,&opBusy);
		opBusy = opBusy&0x80000000;
#endif
		if(OPL_OK != retVal)
		{
			OPL_TRACE();
            goto exit_label;
		}
		if(!timeOut--)
		{
			OPL_TRACE();
			retVal = OPL_ERROR;
            goto exit_label;
		}
	}
	opBusy = 1;
	
	for(recIndex = startId; recIndex < (startId + num);recIndex++)
	{
		regCtrlPerRec = regCtrl|((recIndex&0xff)<<MIA_TABLE_INDEX);
		psrc = (shadowDataAddr + (widthOfRec - 1)*4);
		
		retVal = oplRegWrite(regCtlAddr,regCtrlPerRec);

		while(opBusy)
		{
#ifdef NOT_USING_FIELDMASK		
			retVal = oplRegFieldRead(regCtlAddr, 31, 1, &opBusy);
#else
			retVal = oplRegRead(regCtlAddr, &opBusy);
			opBusy = opBusy&0x80000000;
#endif
			if(OPL_OK != retVal)
			{
				OPL_TRACE();
                goto exit_label;
			}
			if(!timeOut--)
			{
				OPL_TRACE();
				retVal = OPL_ERROR;
                goto exit_label;
			}
		}
				
		for(count = OPL_ZERO; count < widthOfRec; count++,pdst++)
		{
			oplRegRead(psrc,&pdst[0]);	
			psrc = psrc - 4;
		}
		/*first we need check if the table access is busy*/

		opBusy = 1;
	}

exit_label:
    OPL_TAB_ACCESS_UNLOCK;
	return retVal;
}

OPL_STATUS oplTabWrite(OPCONN_TAB_e region,UINT32 startId,UINT32 num,UINT32 *pVal)
{
	UINT32 tabId;
	UINT32 numOfRec;
	UINT32 widthOfRec;
	UINT32 *psrc;
	UINT32 pdst;
	UINT32 recIndex;
	UINT32 count;
	UINT32 regCtrl = OPL_ZERO;
	UINT32 regCtrlPerRec;
	UINT32 timeOut = 1000;
	UINT32 opBusy = 1;
	OPL_STATUS retVal = OPL_OK;
	UINT32 regCtlAddr;
	UINT32 shadowDataAddr;

    OPL_TAB_ACCESS_LOCK;

	if(OPCONN_TAB_CLS_MASK > region || OPCONN_TAB_END <= region || OPL_NULL == pVal)
	{
		OPL_TRACE();
		//OPL_DRV_PRINTF(("region = %x startId = %d num = %d pval = %08x\n",region,startId,num,(UINT32)pVal));
		retVal = OPL_ERR_INVALID_PARAMETERS;
        goto exit_label;
	}

	tabId 		= opconn_tab_info[region].tabId;
	numOfRec 	= opconn_tab_info[region].numOfRec;
	widthOfRec	= opconn_tab_info[region].widthOfRec;

	if(numOfRec < (startId + num))
	{
		OPL_TRACE();
		retVal = OPL_ERR_INVALID_PARAMETERS;
        goto exit_label;
	}

	switch(tabId)
	{
		case OPCONN_TAB_CLS_MASK:
		case OPCONN_TAB_CLS_ADDR:
		case OPCONN_TAB_CLS_RULE:
		case OPCONN_TAB_CLS_COUNTER:
			regCtrl = (MIA_OP_EXEC<<BRG_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<BRG_MIA_TYPE_OFFSET)|((tabId&0x0f)<<BRG_MIA_TSEL_OFFSET);
			break;	
		case OPCONN_TAB_PON_MAC:
			regCtrl = (MIA_OP_EXEC<<BRG_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<BRG_MIA_TYPE_OFFSET)|((tabId+1)<<BRG_MIA_TSEL_OFFSET);
			break;
		case OPCONN_TAB_US_SHPBD:
			if (!IS_ONU_RESPIN && (startId > TAB_US_SHPBD_PIR_START))
			{
				retVal = OPL_ERROR;
	            goto exit_label;
			}
		case OPCONN_TAB_DS_SHPBD:
			regCtrl = (MIA_OP_EXEC<<SHPBD_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<SHPBD_MIA_TYPE_OFFSET);
			break;
		case OPCONN_TAB_US_WRED:
		case OPCONN_TAB_DS_WRED:
			regCtrl = (MIA_OP_EXEC<<SHPBD_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<SHPBD_MIA_TYPE_OFFSET);
			break;
	
		case OPCONN_TAB_DMA0_RX:
		case OPCONN_TAB_DMA0_TX:
		case OPCONN_TAB_DMA1_RX:
		case OPCONN_TAB_DMA1_TX:
			regCtrl = (MIA_OP_EXEC<<DMA_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<DMA_MIA_TYPE_OFFSET);
			break;
		case OPCONN_TAB_Q_CONF_STA:
			regCtrl = (MIA_OP_EXEC<<Q_MIA_BUSY_OFFSET)|(MIA_WRITE_OP<<Q_MIA_TYPE_OFFSET);
			break;
		default:
			//OPL_DRV_PRINTF(("no such table."));
			break;
	}
	
	regCtlAddr 			= opconn_tab_control_map[tabId].regCtlAddr;
	shadowDataAddr	= opconn_tab_control_map[tabId].dataAddr;
	psrc = pVal;
	/*first we need check if the table access is busy*/
	while(opBusy)
	{
#ifdef NOT_USING_FIELDMASK	
		retVal = oplRegFieldRead(regCtlAddr, 31, 1, &opBusy);
#else
		retVal = oplRegRead(regCtlAddr, &opBusy);
		opBusy = opBusy&0x80000000;
#endif
		if(OPL_OK != retVal)
		{
			OPL_TRACE();
            goto exit_label;
		}
		if(!timeOut--)
		{
			OPL_TRACE();
    		retVal = OPL_ERROR;
            goto exit_label;
		}
	}
	opBusy = 1;
		
	for(recIndex = startId; recIndex < (startId + num); recIndex++)
	{
		pdst = (shadowDataAddr + (widthOfRec-1)*4);
		regCtrlPerRec = regCtrl|((recIndex&0xff)<<MIA_TABLE_INDEX);
		
		for(count = OPL_ZERO; count < widthOfRec;count++,psrc++)
		{
			oplRegWrite(pdst,psrc[0]);
			pdst = pdst - 4;
		}
		
		oplRegWrite(regCtlAddr,regCtrlPerRec);
		/*check the busy bit if the operation is completed.*/
		while(opBusy)
		{
#ifdef NOT_USING_FIELDMASK		
			retVal = oplRegFieldRead(regCtlAddr, 31, 1, &opBusy);
#else
			retVal = oplRegRead(regCtlAddr, &opBusy);
			opBusy = opBusy&0x80000000;
#endif
			if(OPL_OK != retVal)
			{
				OPL_TRACE();
                goto exit_label;
			}
			if(!timeOut--)
			{
				OPL_TRACE();
        		retVal = OPL_ERROR;
                goto exit_label;
			}
		}
		opBusy = 1;
	}
	
exit_label:
    OPL_TAB_ACCESS_UNLOCK;
	return retVal;
}
#endif

OPL_STATUS oplRegFieldRead(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 *data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 fieldMask;
	
	if(OPL_NULL == data0)
	{
		OPL_TRACE();
		return OPL_ERR_NULL_POINTER;
	}

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		OPL_DRV_PRINTF(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("fieldMask = %08x\n",fieldMask));
#endif


	retVal = oplRegRead(regAddr,&regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif

	regVal = regVal&fieldMask;

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif

	*data0 = regVal>>fieldOffset;
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("data0 = %08x\n",*data0));
#endif
	return OPL_OK;	
}
OPL_STATUS oplRegFieldWrite(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 oldVal;
	UINT32 fieldMask;

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		OPL_DRV_PRINTF_ERR(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("fieldMask = %08x\n",fieldMask));
#endif


	retVal = oplRegRead(regAddr,&oldVal);
	if(OPL_OK != retVal)
	{
		DBG_LINE;
		return retVal;
	}

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("oldVal = %08x\n",oldVal));
#endif


	oldVal = oldVal&(~fieldMask);

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("oldVal = %08x\n",oldVal));
#endif


	regVal = (data0<<fieldOffset)&fieldMask;

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif


	regVal = regVal|oldVal;
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif
	retVal = oplRegWrite(regAddr,regVal);
	
	return retVal;	
}	

OPL_STATUS oplRegFieldRd(UINT32 reg,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 *data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 fieldMask;
	UINT32 regAddr = reg*4;
    
	if(OPL_NULL == data0)
	{
		OPL_TRACE();
		return OPL_ERR_NULL_POINTER;
	}

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		OPL_DRV_PRINTF(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("fieldMask = %08x\n",fieldMask));
#endif


	retVal = oplRegRead(regAddr,&regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif

	regVal = regVal&fieldMask;

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif

	*data0 = regVal>>fieldOffset;
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("data0 = %08x\n",*data0));
#endif
	return OPL_OK;	
}
OPL_STATUS oplRegFieldWr(UINT32 reg,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 oldVal;
	UINT32 fieldMask;
    UINT32 regAddr = reg*4;

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		OPL_DRV_PRINTF(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("fieldMask = %08x\n",fieldMask));
#endif


	retVal = oplRegRead(regAddr,&oldVal);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("oldVal = %08x\n",oldVal));
#endif


	oldVal = oldVal&(~fieldMask);

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("oldVal = %08x\n",oldVal));
#endif


	regVal = (data0<<fieldOffset)&fieldMask;

#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif


	regVal = regVal|oldVal;
#ifdef HW_DEBUG
	OPL_DRV_PRINTF(("regVal = %08x\n",regVal));
#endif
	
	retVal = oplRegWrite(regAddr,regVal);
	
	return retVal;	
}

OPL_STATUS oplRegFieldReadTest(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	
	retVal =  oplRegFieldRead( regAddr,  fieldOffset,  fieldWidth, &regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
	OPL_DRV_PRINTF(("bitsVal = %08x\n",regVal));
	return retVal;
}
OPL_STATUS orr(UINT32 regAddr)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	retVal = oplRegRead(regAddr*4,&regVal);

	if(OPL_OK == retVal)
	{
		OPL_DRV_SHOW_PRINTF(("reg 0x%08x  value:0x%08x\n",regAddr,regVal));
	}
	return retVal;
}

OPL_STATUS owr(UINT32 regAddr,UINT32 regVal)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegWrite(regAddr*4,regVal);

	if(OPL_OK == retVal)
	{
		OPL_DRV_SHOW_PRINTF(("reg 0x%08x  value:0x%08x\n",regAddr,regVal));
	}
	return retVal;
}

OPL_STATUS mrr(UINT32 regAddr,UINT32 num)
{
	UINT32 regVal;
	UINT32 index;
		
	OPL_DRV_SHOW_PRINTF((" addr    regVal\n"));
	for(index = OPL_ZERO; index < num; index++,regVal = OPL_ZERO)
	{
		oplRegRead((regAddr+index)*4,&regVal);
		OPL_DRV_SHOW_PRINTF(("0x%08x  0x%08x\n",(regAddr+index),regVal));
	}
	return OPL_OK;
}

OPL_STATUS mwr(UINT32 regAddr,UINT32 num,UINT32 newVal,UINT8 mode)
{
	UINT32 regVal;
	UINT32 index;

	OPL_DRV_SHOW_PRINTF((" addr    regVal\n"));
	regVal = newVal;
	for(index = OPL_ZERO; index < num; index++,regVal++)
	{
		if(mode)
		{
			oplRegWrite((regAddr+index)*4, regVal);
			OPL_DRV_SHOW_PRINTF(("0x%08x  0x%08x\n",regAddr+index,regVal));
		}else
		{
			oplRegWrite(regAddr*4, regVal+index);
			OPL_DRV_SHOW_PRINTF(("0x%08x  0x%08x\n",regAddr,regVal+index));
		}
	}
	return OPL_OK;
}

OPL_STATUS ort(OPCONN_TAB_e region,UINT32 startId,UINT32 num)
{
	UINT32 val[TAB_MAX_WIDTH];
	OPL_STATUS retVal;
	UINT32 index;
	UINT32 cnt;

	for(index = OPL_ZERO;index < num;index++)
	{
		retVal = oplTabRead(region, startId+index, 1,&val[0]);
		if(OPL_OK != retVal)
		{
			OPL_DRV_SHOW_PRINTF(("read table  %d ,record %d failed.\n ",(UINT16)region,(startId + index)));
			return OPL_ERROR;
		}
		OPL_DRV_SHOW_PRINTF(("recordId %d\n",(startId+index)));
		for(cnt = 0; cnt< opconn_tab_info[region].widthOfRec;cnt++)
		{
			if((cnt+1)%4 == 0)
			{
				OPL_DRV_SHOW_PRINTF(("%08x\n",val[cnt]));
			}else
			{
				OPL_DRV_SHOW_PRINTF(("%08x ",val[cnt]));
			}
		}
		OPL_DRV_SHOW_PRINTF(("\n"));
	}
	return OPL_OK;
}

OPL_STATUS owt(UINT32 tabId,UINT32 startRecId,UINT32 numOfRec,UINT32 offset,UINT32 value)
{
	UINT32 tmpRecVal[TAB_MAX_WIDTH];
	UINT32 index = OPL_ZERO;
	OPL_STATUS retVal;

	for(index = startRecId; index < (startRecId + numOfRec); index++)
	{
		retVal = oplTabRead(tabId,index,1,tmpRecVal);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		tmpRecVal[offset%TAB_MAX_WIDTH] = value;

		retVal = oplTabWrite(tabId,index,1,tmpRecVal);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	return OPL_OK;
}
#if 0
OPL_STATUS owt2(UINT32 tabId,UINT32 startRecId,UINT32 numOfRec,UINT32 value,...)
{
	UINT32 width = opconn_tab_info[tabId].widthOfRec;
    va_list x;
    UINT32 numOfP = 0;
    UINT32 pVal[10] = {0x0};
    UINT32 index = 0;
    OPL_STATUS retVal = OPL_OK;

    va_start(x,value);
    pVal[0] = value;
       
    for(index = 0; index<width-1; index++)
    {
        pVal[index+1]=va_arg(x,int);
    }
    
    va_end(x);

    for(index = startRecId; index < (startRecId + numOfRec); index++)
    {
		retVal = oplTabWrite(tabId,index,1,pVal);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	} 
}
#endif
OPL_STATUS ost()
{
	UINT32 tabIndex;
	
	OPL_DRV_SHOW_PRINTF(("%-12s %-12s %-12s\n","tabId", "numOfRecords","description"));
	for(tabIndex = OPCONN_TAB_CLS_MASK; tabIndex < OPCONN_TAB_END; tabIndex++)
	{
		OPL_DRV_SHOW_PRINTF(("%-12d %-12d %-12s\n",	opconn_tab_info[tabIndex].tabId,
																					opconn_tab_info[tabIndex].numOfRec,
																					opconn_tab_info[tabIndex].desc));
	}
	return OPL_OK;
}


/*register define 2.24*/
OPCONN_REG_INFO_t opconn_reg_info[] = 
{
/*gb */
{0x0000,0x80000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0002,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0003,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0004,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0005,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0006,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0007,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0008,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x000A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x000B,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x000E,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x000F,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0010,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0011,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0012,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0013,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0014,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0017,0x0000000f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0018,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0020,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0021,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0022,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0023,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0024,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0025,0x00000303,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0026,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0030,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*spi register */
{0x0300,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0301,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0302,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0303,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0304,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0305,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0306,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0307,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0308,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0309,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x030a,0x0000003f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x030b,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x030c,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*mdio register*/
{0x0500,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0501,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0502,0x0000001f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0503,0x0000001f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0504,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0505,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0506,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
/*uart0 register*/
{0x0600,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0600,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0601,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0602,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0602,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0603,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0604,0x0000000a,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0605,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0606,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0607,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
/*uart1 register*/
{0x0700,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0700,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0701,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0702,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0702,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0703,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0704,0x0000000a,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0705,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0706,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0707,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
/*i2c register*/
{0x0800,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0800,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0801,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0802,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0803,0x0000008b,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0804,0x0000000f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0805,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*gmac register*/

{0x0900,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0901,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0902,0x000007ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0904,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0905,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0906,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0907,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0908,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0909,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x090A,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x090B,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0918,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0920,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0921,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0922,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0923,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0924,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0925,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0926,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0927,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0928,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0929,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x092a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x092b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0940,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0941,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0942,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0943,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0944,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0945,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0946,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0947,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0948,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0949,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x094f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0950,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0951,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0952,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0953,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0954,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0955,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*pmac register*/
{0x0A00,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A01,0x00000003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A02,0x000007ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A04,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A05,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A06,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A07,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A08,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A10,0x000007ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A15,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A16,0x0007ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A18,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A19,0x00000001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A1A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A20,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A21,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A22,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A23,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A24,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A25,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A26,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A27,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A28,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A29,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A2a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A2b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A40,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A41,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A42,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A43,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A44,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A45,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A46,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A47,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A48,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A49,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A4f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A50,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A51,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A52,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A53,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A54,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0A55,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*watch dog register*/
{0x0B00,0xffff0003,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B01,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B02,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B03,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*gpio register*/
{0x0B20,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B21,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B22,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B23,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B26,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B27,0x000000ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B28,0x0000ffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*four timer register*/
{0x0B40,0x0000ff8f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B41,0x0000ff8f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B42,0x0000ff8f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B43,0x0000ff8f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B44,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B45,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B46,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B47,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B48,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B49,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B4A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B4B,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*real timer register*/
{0x0B60,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B61,0x40ffc0ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B62,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B63,0x070f3f1f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B64,0x0fff071f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B65,0x000001ff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B66,0x1f1f1f1f,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B67,0xffff0fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B70,0x0fffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*interrupt register*/
{0x0B80,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B82,0xffffffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B86,0x00008001,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B87,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B88,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B89,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B8A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B8B,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B8C,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B8D,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0B8E,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*host register*/

{0x0C00,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C01,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C02,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C03,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C04,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C05,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C06,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C07,0x807f7fff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C08,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C09,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0a,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0b,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0c,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0d,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0e,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C0f,0x000fffff,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C10,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C11,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C12,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C13,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C14,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C15,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C16,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C17,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C18,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0C19,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*fe mac register*/
{0x0D00,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D01,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D02,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D03,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D04,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D05,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D06,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D07,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D08,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D09,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D0A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D0B,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D0C,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x0D0D,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*report register*/
{0x1000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x100f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1010,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1011,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1018,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x1019,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x101a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*tm register */
{0x2000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2005,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2006,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2010,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2011,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2018,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2019,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x201f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2020,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2021,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2022,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2023,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2024,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2025,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2026,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2027,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2028,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2029,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x202f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2050,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2051,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2052,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2053,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2054,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2055,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2056,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2057,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2058,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2059,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x205f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2060,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2061,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2062,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2063,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2064,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2065,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2066,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2067,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2068,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2069,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x206f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2070,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2071,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2072,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2073,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2074,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2075,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2076,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2077,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2078,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2079,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x207f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2080,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2081,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2083,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2084,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2090,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2091,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2092,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2093,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2094,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2095,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2096,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2097,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2098,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x2099,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x209f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a0,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a1,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a2,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a3,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a4,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a5,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a6,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a7,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a8,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20a9,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20aa,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20ab,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20ac,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20ad,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20ae,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20af,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b0,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b1,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b2,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b3,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b4,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b5,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b6,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b7,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b8,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20b9,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20ba,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x20bb,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*cle register*/
{0x3000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x300a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3010,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3011,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3020,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3021,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3022,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3023,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3024,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3030,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3031,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3032,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3033,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3034,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3035,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3036,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3037,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3038,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3039,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x303a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x303b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x303c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x303d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3040,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3041,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3044,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3045,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3048,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3049,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x304f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3050,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3051,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3052,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3053,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3060,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3061,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3064,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3065,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3066,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x3067,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*mpcp register */
{0x4000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4005,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4006,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4007,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x400a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x400b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x400c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x400d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x400e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4019,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4020,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4021,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4022,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4023,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4024,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4025,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4030,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4031,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4032,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4033,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4034,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4035,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4036,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4037,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x4038,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*marb register*/

{0x5020,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5021,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5023,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5024,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5025,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5030,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5031,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5032,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5033,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5034,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5035,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5036,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5040,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5041,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5042,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5043,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5048,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5049,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x504a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x504b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x504c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5050,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5051,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5058,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x5059,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x505a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x505b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*cp dma register*/
{0x6000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6005,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6006,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6007,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6010,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6011,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6018,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6019,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x601f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6028,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6029,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x602f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6030,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6031,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6032,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6033,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6034,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6035,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6036,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6037,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6038,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6039,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6040,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6041,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6042,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6043,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6044,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6045,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6046,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6047,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6048,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6049,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x604f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6050,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6051,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6052,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6053,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6054,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6055,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6056,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6057,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6058,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6070,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6071,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6072,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6073,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6074,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6075,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6076,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6077,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6078,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6079,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x607f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6080,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x6081,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*fe dma register*/
{0x7000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7005,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7006,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7007,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700a,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700b,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700c,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700d,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700e,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x700f,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7010,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7011,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7012,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7013,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7014,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7015,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7016,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7017,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7018,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7019,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7020,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7021,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7022,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7023,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7024,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7025,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7026,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7027,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7028,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7030,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x7031,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

/*ge paser register*/
{0x8000,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8001,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8002,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8003,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8004,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8005,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8006,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8007,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8008,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x8009,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x800A,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x800B,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x800C,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x800D,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},
{0x800E,0x00000000,0x00000000,0X00000000,0X00000000,0X00000000,0x00000000,OPL_NULL},

{0XFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,OPL_NULL}
};

#define testPatternNum				6

UINT32 testValue[] = 
{
	0X00000000,0XFFFFFFFF,0X5A5A5A5A,0XA5A5A5A5,0X55555555,0XAAAAAAAA
};

UINT32 testTable[6][10] = 
{
		{	
			0X00000000,0X00000000,0X00000000,0X00000000,0X00000000,
			0X00000000,0X00000000,0X00000000,	0X00000000,0X00000000
		},
		{	
			0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,
			0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF
		},
		{	
			0X55555555,0X55555555,0X55555555,0X55555555,0X55555555,
			0X55555555,0X55555555,0X55555555,0X55555555,0X55555555
		},
		{	
			0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA,
			0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA,0XAAAAAAAA
		},
		{	
			0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A,
			0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A,0X5A5A5A5A
		},
		{	
			0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5,
			0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5,0XA5A5A5A5
		}	
};

OPL_STATUS oplRegTest(void)
{
	UINT32 index = OPL_ZERO;
	UINT32 count;
	UINT32 regAddr;
	UINT32 inputVal;
	UINT32 outputVal;
	UINT32 rwMask;
	
	while(opconn_reg_info[index].regAddr != 0xffff)
	{
		regAddr = opconn_reg_info[index].regAddr;
		
		for(count = 0; count < testPatternNum; count++)
		{
			rwMask = opconn_reg_info[index].rwMask;
			outputVal = OPL_ZERO;
			inputVal = testValue[count]&rwMask;
			oplRegWrite(regAddr*4,inputVal);
			oplDelay(100);
			oplRegRead(regAddr*4,&outputVal);
			outputVal = outputVal&rwMask;
			if(outputVal != inputVal)
			{
				OPL_DRV_PRINTF(("regAddr::0x%08x    input %08x output %08x\n",regAddr,inputVal,outputVal));
			}
		}
		index++;
	}
	return OPL_OK;
}

OPL_STATUS oneTabOnePattern(UINT32 tabId,UINT32 recordIndex,UINT32 *inputVal)
{
	UINT32 recVal[TAB_MAX_WIDTH];
	UINT32 testVal[TAB_MAX_WIDTH];
	UINT32 timeOut = 100;
	UINT32 count;
	
	OPL_MEMSET(&recVal[0],0X00,TAB_MAX_WIDTH*4);
	OPL_MEMCPY(&testVal[0],inputVal,TAB_MAX_WIDTH*4);
	switch(tabId)
	{
		case OPCONN_TAB_CLS_MASK:
			testVal[0] = testVal[0]&0x00000007;
			break;
		case OPCONN_TAB_CLS_ADDR:
			testVal[0] = testVal[0]&0x00000fff;
			break;
		case OPCONN_TAB_CLS_RULE:
			testVal[0] = testVal[0]&0x000001ff;
			break;
		case OPCONN_TAB_CLS_COUNTER:
			
			break;
		case OPCONN_TAB_PON_MAC:
			testVal[0] = 0;
			testVal[1] = testVal[1]&0x0007ffff;
			break;
		case OPCONN_TAB_DMA0_RX:
		case OPCONN_TAB_DMA0_TX:
		case OPCONN_TAB_DMA1_RX:
		case OPCONN_TAB_DMA1_TX:
			testVal[0] = testVal[0]&0x07ffffff;
			default:
			OPL_DRV_PRINTF(("tabId error.\n"));
	}
	if(OPCONN_TAB_CLS_COUNTER == tabId)
	{
		return OPL_OK;
	}
	
	oplTabWrite(tabId, recordIndex, 1, &testVal[0]);
	while(timeOut--)
	{
		
	}
	
	oplTabRead(tabId, recordIndex, 1, &recVal[0]);

	if(!OPL_MEMCMP((&recVal[0]),&testVal[0],opconn_tab_info[tabId].widthOfRec*4))
	{
		return OPL_OK;
	}else
	{
		OPL_DRV_PRINTF(("\ntabId %d failed.\n",tabId));
		OPL_DRV_PRINTF(("input value:\n"));
		for(count = OPL_ZERO; count < opconn_tab_info[tabId].widthOfRec;count++)
		{
			OPL_DRV_PRINTF(("%08x  ",testVal[count]));
		}
		OPL_DRV_PRINTF(("\noutput value:\n"));
		for(count = OPL_ZERO; count < opconn_tab_info[tabId].widthOfRec;count++)
		{
			OPL_DRV_PRINTF(("%08x  ",recVal[count]));
		}
	}
	OPL_DRV_PRINTF(("\n"));
	return OPL_OK;
}
OPL_STATUS tabTest(void)
{
	UINT32 tabId;
	UINT32 count;
	UINT32 recIndex;
	UINT32 pattenIndex;

	for(count = OPCONN_TAB_CLS_MASK; count < OPCONN_TAB_PON_MAC; count++)
	{
		tabId = opconn_tab_info[count].tabId;
		for(recIndex = OPL_ZERO;recIndex < opconn_tab_info[count].numOfRec;recIndex++)
		{
			for(pattenIndex = OPL_ZERO; pattenIndex < 6; pattenIndex++)
			{
				oneTabOnePattern(tabId,recIndex,(UINT32 *)(&testTable[0]+pattenIndex*TAB_MAX_WIDTH));
			}
		}
	}
	
	return OPL_OK;
}

#if 0
OPL_STATUS oplRegTestLog(UINT8 *fileName)
{
	UINT32 index = OPL_ZERO;
	UINT32 count;
	UINT32 regAddr;
	UINT32 inputVal;
	UINT32 outputVal;
	UINT32 rwMask;
	FILE *fp;

	fp = fopen(fileName,"w");
	
	if(OPL_NULL == fp)
	{
		OPL_DRV_PRINTF(("cannot create %s file.\n",fileName));
		return OPL_ERROR;
	}

	fprintf(fp,"%s","follow are onu register test record.\n");
	
	while(opconn_reg_info[index].regAddr != 0xffff)
	{
		regAddr = opconn_reg_info[index].regAddr;
		
		for(count = 0; count < testPatternNum; count++)
		{
			rwMask = opconn_reg_info[index].rwMask;
			outputVal = OPL_ZERO;
			inputVal = testValue[count]&rwMask;
			oplRegWrite(regAddr*4,inputVal);
			oplDelay(100);
			oplRegRead(regAddr*4,&outputVal);
			outputVal = outputVal&rwMask;
			if(outputVal != inputVal)
			{
				fprintf(fp,"regAddr::0x%08x    input %08x output %08x\n",regAddr,inputVal,outputVal);
			}
		}
		index++;
	}
	fclose(fp);
	return OPL_OK;
}

OPL_STATUS oneTabOnePatternLog(UINT32 tabId,UINT32 recordIndex,UINT32 *inputVal,FILE *fp)
{
	UINT32 recVal[TAB_MAX_WIDTH];
	UINT32 testVal[TAB_MAX_WIDTH];
	UINT32 timeOut = 100;
	UINT32 count;
	
	OPL_MEMSET(&recVal[0],0X00,TAB_MAX_WIDTH*4);
	OPL_MEMCPY(&testVal[0],inputVal,TAB_MAX_WIDTH*4);
	switch(tabId)
	{
		case OPCONN_TAB_CLS_MASK:
			testVal[0] = testVal[0]&0x1f;
			break;
		case OPCONN_TAB_CLS_ADDR:
			testVal[0] = testVal[0]&0x0f;
			break;
		case OPCONN_TAB_CLS_RULE:
			testVal[0] = 0;
			testVal[1] = testVal[1]&0X3f;
			break;
		case OPCONN_TAB_CLS_COUNTER:
			
			break;
		case OPCONN_TAB_PON_MAC:
			testVal[0] = 0;
			testVal[1] = testVal[1]&0x07;
			break;
		default:
			OPL_DRV_PRINTF(("tabId error.\n"));
	}
	if(OPCONN_TAB_CLS_COUNTER == tabId)
	{
		return OPL_OK;
	}
	oplTabWrite(tabId, recordIndex, 1, &testVal[0]);
	while(timeOut--)
	{
		
	}
	oplTabRead(tabId, recordIndex, 1, &recVal[0]);

	if(!OPL_MEMCMP((&recVal[0]),&testVal[0],opconn_tab_info[tabId].widthOfRec*4))
	{
		return OPL_OK;
	}else
	{
		fprintf(fp,"\ntabId %d failed.\n",tabId);
		fprintf(fp,"input value:\n");
		for(count = OPL_ZERO; count < opconn_tab_info[tabId].widthOfRec;count++)
		{
			fprintf(fp,"%08x  ",testVal[count]);
		}
		fprintf(fp,"\noutput value:\n");
		for(count = OPL_ZERO; count < opconn_tab_info[tabId].widthOfRec;count++)
		{
			fprintf(fp,"%08x  ",recVal[count]);
		}
	}
	fprintf(fp,"\n");
	return OPL_OK;
}
OPL_STATUS tabTestLog(UINT8 *fileName)
{
	UINT32 tabId;
	UINT32 count;
	UINT32 recIndex;
	UINT32 pattenIndex;
	FILE *fp;

	fp = fopen(fileName,"w");
	if(OPL_NULL == fp)
	{
		OPL_DRV_PRINTF(("can not create file %s.\n",fileName));
		return OPL_ERROR;
	}
	
	for(count = OPCONN_TAB_CLS_MASK; count <= OPCONN_TAB_PON_MAC; count++)
	{
		tabId = opconn_tab_info[count].tabId;
		if(tabId == OPCONN_TAB_CLS_COUNTER)
		{
			continue;
		}
		
		for(recIndex = OPL_ZERO;recIndex < opconn_tab_info[count].numOfRec;recIndex++)
		{
			for(pattenIndex = OPL_ZERO; pattenIndex < 6; pattenIndex++)
			{
				oneTabOnePatternLog(tabId,recIndex,(UINT32 *)(&testTable[0]+pattenIndex*TAB_MAX_WIDTH),fp);
			}
		}
	}
	fclose(fp);
	return OPL_OK;
}

#endif
OPL_STATUS mdioRegWriteTest(UINT16 devAddr,UINT16 regAddr,UINT16 regVal)
{
	OPL_STATUS retVal = OPL_OK;
	retVal = mdioRegisterWrite(devAddr, regAddr, regVal);
	return retVal;
}

OPL_STATUS mdioRegReadTest(UINT16 devAddr,UINT16 regAddr)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;
	retVal = mdioRegisterRead(devAddr, regAddr, &regVal);
	OPL_DRV_SHOW_PRINTF(("reg = %d val = %08x\n",regAddr,regVal));
	return retVal;
}

OPL_STATUS mdioRegShow(UINT16 devAddr,UINT16 startRegId,UINT16 endRegId)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;
	UINT32 regId;

	for(regId = startRegId; regId <= endRegId;regId++)
	{
		mdioRegisterRead(devAddr, regId, &regVal);
		OPL_DRV_SHOW_PRINTF(("regId = %04d val = %04x\n",regId,regVal));
	}
	return retVal;
}

OPL_STATUS mdioRegFieldReadTest(UINT16 devAddr,UINT16 regAddr,UINT8 offset,UINT8 width)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	retVal = mdioRegisterFieldRead(devAddr, regAddr, offset, width,&regVal);
	OPL_DRV_SHOW_PRINTF(("bitVal = %08x\n",regVal));
	return retVal;
}

OPL_STATUS mdioRegFieldWriteTest(UINT16 devAddr,UINT16 regAddr,UINT8 offset,UINT8 width,UINT16 regVal)
{
	OPL_STATUS retVal = OPL_OK;
	
	retVal = mdioRegisterFieldWrite(devAddr, regAddr, offset, width,regVal);
	
	return retVal;
}

INT32 convert20xValue(UINT8 *p,UINT32 len,UINT32 *pR)
{
	UINT32 value = 0;
	UINT32 cnt = 0;

	if(NULL == p || NULL == pR || 3 > len)
	{
		return -1;
	}
	
	if(!(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')))
	{
		return -1;
	}

	for(cnt = 0; cnt < len - 2; cnt++)
	{
		value = value<<4;
		
		switch(p[cnt+2])
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				/*
				value |= (p[cnt+2] - '0')<<((7-cnt)*4);
				*/
				value |= (p[cnt+2] - '0');
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				/*
				value |= (p[cnt+2] - 'a' + 10)<<((7-cnt)*4);
				*/
				value |= (p[cnt+2] - 'a' + 10);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				/*
				value |= (p[cnt+2] - 'A' + 10)<<((7-cnt)*4);
				*/
				value |= (p[cnt+2] - 'A' + 10);
				break;
			default:
				return -1;
		}
	}
	
	*pR = value;
	
	return 0;
}

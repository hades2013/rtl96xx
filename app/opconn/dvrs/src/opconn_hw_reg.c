/* 
 * Copyright (c) 2011 Qualcomm Atheros, Inc.. 
 * All Rights Reserved. 
 * Qualcomm Atheros Confidential and Proprietary. 
 *
 *
 * Source Name: opconn_hw_reg.c
 *
 *
 * Revision History:
 * Author        Date         Description of Changes
 * ---------   ------------  -------------------------
 * wfxu        2012/04/3      Initial Version	
 * ---------   ------------  -------------------------
 */

#include "opl_driver.h"
#include "opl_debug.h"
#include "opconn_hw_reg.h"
#include "opconn_usr_ioctrl.h"


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




int oplRegBitTest (unsigned int regId, unsigned int bits)
{
	unsigned int nTemp = 0;
	if (regId % 4 != 0)
		return OPL_ERROR;
	
	oplRegRead (regId, &nTemp);
	return (nTemp & bits);
}

void oplRegBitSet(unsigned int regId, unsigned int bits)	
{
	unsigned int nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp |= (bits);
	oplRegWrite (regId, nTemp);
}

void oplRegBitClear(unsigned int regId, unsigned int bits)	
{
	unsigned int nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp &= (~(bits));
	oplRegWrite (regId, nTemp);
}

void oplRegBitToggle(unsigned int regId, unsigned int bits)	
{
	unsigned int nTemp = OPL_ZERO;
	unsigned int nTemp1 = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	oplRegRead (regId, &nTemp);
	nTemp1 = nTemp;
	nTemp &= (bits);
	nTemp1 &= (~(bits));
	nTemp1 |= (~nTemp);
	oplRegWrite (regId, nTemp1);
}

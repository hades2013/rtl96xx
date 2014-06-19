/* 
 * Copyright (c) 2011 Qualcomm Atheros, Inc.. 
 * All Rights Reserved. 
 * Qualcomm Atheros Confidential and Proprietary. 
 *
 * Source Name: stats.c
 *
 * Revision History:
 * wfxu        2012/04/6      Initial Version
 *
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/mutex.h>

#include "dal_stats.h"
#include "hw_interface.h"


OPL_CNT_t g_astCntGe[DAL_CNT_GE_NUM]={
{0x0920, "TxTotalPkt", {0, 0}, "Packets (good and error) transmitted."},
{0x0921, "TxBroad", {0, 0}, "Good broadcast packets transmitted."},
{0x0922, "TxMulti", {0, 0}, "Good multicast packets transmitted."},
{0x0923, "TxPause", {0, 0}, "PAUSE frames transmitted."},
{0x0924, "TxMacCtrl", {0, 0}, "MAC Control frames transmitted."},
{0x0925, "Tx64byte", {0, 0}, "64-byte packets (good and error) transmitted."},
{0x0926, "Tx65to127", {0, 0}, "65-127 bytes packets (good and error) transmitted."},
{0x0927, "Tx128to255", {0, 0}, "128-255 bytes packets (good and error) transmitted."},
{0x0928, "Tx256to511", {0, 0}, "256-511 bytes packets (good and error) transmitted."},
{0x0929, "Tx512to1023", {0, 0}, "512-1023 bytes packets (good and error) transmitted."},
{0x092A, "Tx1024to1518", {0, 0}, "1024-1518 bytes packets (good and error) transmitted."},
{0x092B, "TxByte", {0, 0}, "Good bytes transmitted."},
{0x092C, "Tx1519toMTU", {0, 0}, "1519-MTU bytes packets (good and error) transmitted."},
{0x0940, "RxGood", {0, 0}, "Good packets received."},
{0x0941, "RxDrop", {0, 0}, "Packets received and droped."},
{0x0942, "RxCRCAlignErr", {0, 0}, "Packets with crc or alignment error received."},
{0x0943, "RxLenErr", {0, 0}, "Packets with type/len field error received."},
{0x0944, "RxFragments", {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0945, "RxJabbers", {0, 0}, "Packets longer than MTU and have crc error received."},
{0x0946, "RxTotalPkt", {0, 0}, "Packets (good and error) received."},
{0x0947, "RxBroad", {0, 0}, "Good broadcast packets received."},
{0x0948, "RxMulti", {0, 0}, "Good multicast packets received."},
{0x0949, "RxPause", {0, 0}, "PAUSE frames received."},
{0x094A, "RxErrMacCtrl", {0, 0}, "Unsupported MAC Control frames received."},
{0x094B, "RxMACCtrl", {0, 0}, "MAC Control frames received."},
{0x094C, "RxUndersize", {0, 0}, "Packets less than 64 bytes in length received."},
{0x094D, "RxOversize", {0, 0}, "Packets longer than MTU in length received."},
{0x094E, "Rx64byte", {0, 0}, "64-byte packets (good and error) received."},
{0x094F, "Rx65to127", {0, 0}, "65-127 bytes packets (good and error) received."},
{0x0950, "Rx128to255", {0, 0}, "128-255 bytes packets (good and error) received."},
{0x0951, "Rx256to511", {0, 0}, "256-511 bytes packets (good and error) received."},
{0x0952, "Rx512to1023", {0, 0}, "512-1023 bytes packets (good and error) received."},
{0x0953, "Rx1024to1518", {0, 0}, "1024-1518 bytes packets (good and error) received."},
{0x0954, "Rx1519toMTU", {0, 0}, "1519-MTU bytes packets (good and error) received."},
{0x0955, "RxByte", {0, 0}, "Good bytes received."},
{0x0959, "RxL2Multi", {0, 0}, "Good L2 multicast packets received."},
{0x0960, "RxRXERPkts", {0, 0}, "Packets received error with other reasons."},
{0x0961, "RxIPGErr", {0, 0}, "Packets received for which the gap is less than the minimum."},
};

OPL_CNT_t g_astCntPon[DAL_CNT_PON_NUM]={
{0x0A1C, "ErrBits", {0, 0}, "PMAC interface received error coded bits counter."},
{0x0A1D, "CorrectBits", {0, 0}, "PMAC interface received correct coded bits counter."},
{0x0A20, "TxTotalPkt", {0, 0}, "Packets (good and error) transmitted."},
{0x0A21, "TxBroad", {0, 0}, "Good broadcast packets transmitted."},
{0x0A22, "TxMulti", {0, 0}, "Good multicast packets transmitted."},
{0x0A23, "TxPause", {0, 0}, "PAUSE frames transmitted."},
{0x0A24, "TxMacCtrl", {0, 0}, "MAC Control frames transmitted."},
{0x0A25, "Tx64byte", {0, 0}, "64-byte packets (good and error) transmitted."},
{0x0A26, "Tx65to127", {0, 0}, "65-127 bytes packets (good and error) transmitted."},
{0x0A27, "Tx128to255", {0, 0}, "128-255 bytes packets (good and error) transmitted."},
{0x0A28, "Tx256to511", {0, 0}, "256-511 bytes packets (good and error) transmitted."},
{0x0A29, "Tx512to1023", {0, 0}, "512-1023 bytes packets (good and error) transmitted."},
{0x0A2A, "Tx1024to1518", {0, 0}, "1024-1518 bytes packets (good and error) transmitted."},
{0x0A2B, "TxByte", {0, 0}, "Good bytes transmitted."},
{0x0A2C, "Tx1519toMTU", {0, 0}, "1519-MTU bytes packets (good and error) transmitted."},
{0x0A2D, "TxOAM", {0, 0}, "OAM packets transmitted."},
{0x0A40, "RxGood", {0, 0}, "Good packets received."},
{0x0A41, "RxDrop", {0, 0}, "Packets received and droped."},
{0x0A42, "RxCRCAlignErr", {0, 0}, "Packets with crc or alignment error received."},
{0x0A43, "RxLenErr", {0, 0}, "Packets with type/len field error received."},
{0x0A44, "RxFragments", {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0A45, "RxJabbers", {0, 0}, "Packets longer than MTU and have crc error received."},
{0x0A46, "RxTotalPkt", {0, 0}, "Packets (good and error) received."},
{0x0A47, "RxBroad", {0, 0}, "Good broadcast packets received."},
{0x0A48, "RxMulti", {0, 0}, "Good multicast packets received."},
{0x0A49, "RxPause", {0, 0}, "PAUSE frames received."},
{0x0A4A, "RxErrMacCtrl", {0, 0}, "Unsupported MAC Control frames received."},
{0x0A4B, "RxMACCtrl", {0, 0}, "MAC Control frames received."},
{0x0A4C, "RxUndersize", {0, 0}, "Packets less than 64 bytes in length received."},
{0x0A4D, "RxOversize", {0, 0}, "Packets longer than MTU in length received."},
{0x0A4E, "Rx64byte", {0, 0}, "64-byte packets (good and error) received."},
{0x0A4F, "Rx65to127", {0, 0}, "65-127 bytes packets (good and error) received."},
{0x0A50, "Rx128to255", {0, 0}, "128-255 bytes packets (good and error) received."},
{0x0A51, "Rx256to511", {0, 0}, "256-511 bytes packets (good and error) received."},
{0x0A52, "Rx512to1023", {0, 0}, "512-1023 bytes packets (good and error) received."},
{0x0A53, "Rx1024to1518", {0, 0}, "1024-1518 bytes packets (good and error) received."},
{0x0A54, "Rx1519toMTU", {0, 0}, "1519-MTU bytes packets (good and error) received."},
{0x0A55, "RxByte", {0, 0}, "Good bytes received."},
{0x0A56, "FECCORRECT", {0, 0}, "Packet that FEC has corrected."},
{0x0A57, "FECUNCOR", {0, 0}, "Packet that FEC has not corrected."},
{0x0A58, "FECNOERR", {0, 0}, "Packet without any error."},
{0x0A59, "RxL2Multi", {0, 0}, "Good L2 multicast packets received."},
{0x0A5a, "RxOAM", {0, 0}, "OAM packets received."},
{0x0A60, "RxRXERPkts", {0, 0}, "Packets received error with other reasons."},
{0x0A61, "RxIPGErr", {0, 0}, "Packets received for which the gap is less than the minimum."},
};

OPL_CNT_t g_astCntFe[DAL_CNT_FE_NUM]={
{0x0D10, "TxTotalPkt", {0, 0}, "Packets (good and error) transmitted."},
{0x0D11, "TxPause", {0, 0}, "PAUSE frames transmitted."},
{0x0D12, "RxGood", {0, 0}, "Good packets received."},
{0x0D13, "RxDrop", {0, 0}, "Packets received and droped."},
{0x0D14, "RxPause", {0, 0}, "PAUSE frames received."},
{0x0D15, "RxCRCAlignErr", {0, 0}, "Packets with crc or alignment error received."},
{0x0D16, "RxLenErr", {0, 0}, "Packets with type/len field error received by FE port."},
{0x0D17, "RxFragments", {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0D18, "RxJabbers", {0, 0}, "Packets longer than MTU and have crc error received."},
};

OPL_CNT_t g_astCntTm[DAL_CNT_TM_NUM]={
{0x20AC, "UpWREDDrop", {0, 0}, "Upstream packets dropped due to WRED."},
{0x20AD, "UpEnQ", {0, 0}, "Upstream packets written into DDR queue."},
{0x20AE, "DnWREDDrop", {0, 0}, "Downstream packets dropped due to WRED."},
{0x20AF, "DnEnQ", {0, 0}, "Downstream packets written into DDR queue."},
{0x20B0, "UpInput", {0, 0}, "Upstream packets entered TM module."},
{0x20B1, "UpIndiDrop", {0, 0}, "Upstream packets dropped due to the indication from CLE module."},
{0x20B2, "UpQFullDrop", {0, 0}, "Upstream packets dropped due to DDR queue full."},
{0x20B3, "UpOutput", {0, 0}, "Upstream packets quitted TM module."},
{0x20B4, "DnInput", {0, 0}, "Downstream packets entered TM module."},
{0x20B5, "DnIndiDrop", {0, 0}, "Downstream packets dropped due to the indication from CLE module."},
{0x20B6, "DnQFullDrop", {0, 0}, "Downstream packets dropped due to DDR queue full."},
{0x20B7, "DnOutput", {0, 0}, "Downstream packets quitted TM module."},
};

OPL_CNT_t g_astCntCle[DAL_CNT_CLE_NUM]={
{0x3009, "RuleMiss", {0, 0}, "Packets which have rule mismatch for all srule tables."},
{0x300A, "RuleDrop", {0, 0}, "Packets which match the rule where the transfer action is drop."},
};

OPL_CNT_t g_astCntBrg[DAL_CNT_BRG_NUM]={
{0x3048, "GERxDrop", {0, 0}, "Received packets dropped due to Rx disabled at GE port."},
{0x3049, "GETxDrop", {0, 0}, "Transmited packets dropped due to Tx disabled at GE port."},
{0x304A, "UpVTTMis", {0, 0}, "Upstream packets dropped due to mismatch."},
{0x304B, "UpTagDrop", {0, 0}, "Upstream packets dropped not complying with the tag config."},
{0x304C, "PONRxDrop", {0, 0}, "Received packets dropped due to Rx disabled at PON port."},
{0x304D, "PONTxDrop", {0, 0}, "Transmited packets dropped due to Tx disabled at PON port."},
{0x304E, "DnVTTMis", {0, 0}, "Downstream packets dropped due to  mismatch."},
{0x304F, "DnTagDrop", {0, 0}, "Downstream packets dropped not complying with the tag config."},
{0x3050, "RsvMacDrop", {0, 0}, "Reserved mac packets dropped due to config in related registers."},
{0x3051, "NonUniDrop", {0, 0}, "Upstream packets dropped due to non-unicast SA ."},
{0x3052, "UserMACDrop", {0, 0}, "Upstream packets dropped due to DA+VID match in ARL table."},
{0x3053, "PonLearnDrop", {0, 0}, "Downstream packets dropped for ARL table is full or SA+VID mismatch."},
{0x3054, "GeLearnDrop", {0, 0}, "Upstream packets dropped for ARL table is full or SA+VID mismatch."},
{0x3055, "PonLernColDrop", {0, 0}, "Ds packets dropped for ARL learn conflict."},
{0x3056, "GeLernColDrop", {0, 0}, "Us packets dropped for ARL learn conflict."},
{0x3057, "DsNUniDrop", {0, 0}, "Ds packets dropped for non-unicast SA."},
{0x3058, "Ds0DADrop", {0, 0}, "Ds packets dropped caused by DA is 0 and learn conflict disabled."},
{0x3059, "Us0DADrop", {0, 0}, "Us packets dropped caused by DA is 0 and learn conflict disabled."},
};

OPL_CNT_t g_astCntMpcp[DAL_CNT_MPCP_NUM]={
{0x4012, "NormGate", {0, 0}, "Normal GATE packet counter rx."},
{0x4013, "DiscPkt", {0, 0}, "Register packet counter rx."},
{0x4014, "DROP", {0, 0}, "MPCP packet drop counter."},
{0x4015, "CLEPkt", {0, 0}, "CLE(traffic) packet counter."},
{0x4016, "CLEDrop", {0, 0}, "CLE(traffic) packet drop counter."},
{0x4017, "DiscGate", {0, 0}, "Discovery gate(broadcast and unicast) rx."},
{0x4018, "Grant", {0, 0}, "Grant rx."},
{0x4019, "GntDrop", {0, 0}, "Grant drop."},
{0x401A, "ForceRpt", {0, 0}, "Force report grant rx."},
{0x401B, "RptDrop", {0, 0}, "Force report grant dropped."},
{0x401C, "GntTotal", {0, 0}, "Total grant rx (tq)."},
{0x4020, "USNORPKT", {0, 0}, "Packet of upstream counter."},
{0x4021, "USREGREQ", {0, 0}, "Transmit register request message."},
{0x4022, "USREGACK", {0, 0}, "Transmit acknowledgement message."},
{0x4023, "USRPTPKT", {0, 0}, "Transmit report message."},
{0x4024, "SOPDROP", {0, 0}, "Packets dropped by MPCP_PARSER when packet haven't SOP flag."},
{0x4025, "CLEFullDrop", {0, 0}, "Packet dropped when traffic buffer is FULL."},
};

OPL_CNT_t g_astCntMarb[DAL_CNT_MARB_NUM]={
{0x5030, "USUniPage", {0, 0}, "Page number in DDR occupied by upstream unicast packets."},
{0x5031, "DSUniPage", {0, 0}, "Page number in DDR occupied by downstream unicast packets."},
{0x5032, "CPUTxPage", {0, 0}, "Page number in DDR occupied by unicast packets sending from CPU."},
{0x5034, "FETxPage", {0, 0}, "Page number in DDR occupied by the packets received on FE port."},
{0x5035, "MIPSWord", {0, 0}, "Words operated by MIPS Write on DDR interface."},
{0x5040, "FreePage", {0, 0}, "Free pages of data space in DDR RAM."},
};

OPL_CNT_t g_astCntCPDma[DAL_CNT_CPDMA_NUM]={
{0x6050, "Rx", {0, 0}, "DMA0 Packets rx."},
{0x6051, "Tx", {0, 0}, "DMA0 Packets tx."},
{0x6052, "GEtoCPU", {0, 0}, "Packets from GE port, rx."},
{0x6053, "GEPolDrop", {0, 0}, "Packets from GE port, dropped due to policy."},
{0x6054, "GEQFullDrop", {0, 0}, "Packets from GE port, dropped due to queue full."},
{0x6055, "PONtoCPU", {0, 0}, "Packets from PON port, rx."},
{0x6056, "PONPolDrop", {0, 0}, "Packets from PON port, dropped due to PON policing."},
{0x6057, "PONQFullDrop", {0, 0}, "Packets from PON port, dropped due to queue full."},
{0x6058, "Dequeue", {0, 0}, "Packets read out from queues."},
{0x6059, "GEFIFOFD", {0, 0}, "Packets dropped due to internal GE FIFO full."},
{0x605A, "PONFIFOFD", {0, 0}, "Packets dropped due to internal PON FIFO full."},
};

OPL_CNT_t g_astCntFEDma[DAL_CNT_FEDMA_NUM]={
{0x7008, "Tx", {0, 0}, "DMA1 Packets rx."},
{0x7009, "FEtoCPU", {0, 0}, "DMA1 Packets tx."},
{0x700A, "ErrDrop", {0, 0}, "Packets from FE port, dropped due to errors in the packet."},
{0x700B, "FIFOFD", {0, 0}, "Packets from FE port, dropped due to internal FIFO full."},
{0x700C, "QFullDrop", {0, 0}, "Packets from FE port, dropped due to queue full."},
{0x700D, "Dequeue", {0, 0}, "Packets read out from queue in DDR data space."},
{0x700E, "Rx", {0, 0}, "Packets written into (DMA1 Write) FEDMA_RXBD table."},
};

OPL_CNT_t g_astCntGeParser[DAL_CNT_GEPARSER_NUM]={
{0x8005, "RxDrop", {0, 0}, "Received packets dropped due to one or more reasons."},
{0x8006, "FIFOFD", {0, 0}, "Received packets dropped due to internal FIFO full."},
{0x8007, "ErrDrop", {0, 0}, "Received packets dropped due to errors in the packet."},
{0x8008, "TokenDrop", {0, 0}, "Received packets dropped due to token in GE_PARSER not enough."},
{0x800E, "Rx", {0, 0}, "All received packets including error packets."},
{0x800F, "MaxMinDrop ", {0, 0}, "Rreceived packets dropped due to too long or too short."},
};

OPL_CNT_t g_astCntCleHit[DAL_CNT_CLEHIT_NUM];

UINT32 g_uiCntPollFlag = 0xFFFFFFFF;

struct mutex		g_pstStatsMutex;

struct task_struct *g_pstStatsThreadId = NULL;



void dalStatsLock()
{
    mutex_lock(&g_pstStatsMutex);
}

void dalStatsUnLock()
{
    mutex_unlock(&g_pstStatsMutex);
}

OPL_STATUS dalCounterGet(unsigned int moduleBit, OPL_CNT_t *oplCounter, unsigned int start, unsigned int num)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;

	dalStatsLock();

	if (moduleBit == DAL_CNT_POLL_GE_MASK)
	{
		if ((start >= DAL_CNT_GE_NUM) || (start+num > DAL_CNT_GE_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntGe[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_PON_MASK)
	{
		if ((start >= DAL_CNT_PON_NUM) || (start+num > DAL_CNT_PON_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntPon[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_FE_MASK)
	{
		if ((start >= DAL_CNT_FE_NUM) || (start+num > DAL_CNT_FE_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntFe[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_TM_MASK)
	{
		if ((start >= DAL_CNT_TM_NUM) || (start+num > DAL_CNT_TM_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntTm[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_CLE_MASK)
	{
		if ((start >= DAL_CNT_CLE_NUM) || (start+num > DAL_CNT_CLE_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntCle[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_BRG_MASK)
	{
		if ((start >= DAL_CNT_BRG_NUM) || (start+num > DAL_CNT_BRG_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntBrg[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_MPCP_MASK)
	{
		if ((start >= DAL_CNT_MPCP_NUM) || (start+num > DAL_CNT_MPCP_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntMpcp[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_MARB_MASK)
	{
		if ((start >= DAL_CNT_MARB_NUM) || (start+num > DAL_CNT_MARB_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntMarb[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_CPDMA_MASK)
	{
		if ((start >= DAL_CNT_CPDMA_NUM) || (start+num > DAL_CNT_CPDMA_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntCPDma[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_FEDMA_MASK)
	{
		if ((start >= DAL_CNT_FEDMA_NUM) || (start+num > DAL_CNT_FEDMA_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntFEDma[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_GEPARSER_MASK)
	{
		if ((start >= DAL_CNT_GEPARSER_NUM) || (start+num > DAL_CNT_GEPARSER_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntGeParser[start], sizeof(OPL_CNT_t)*num);
	}
	else if (moduleBit == DAL_CNT_POLL_CLEHIT_MASK)
	{
		if ((start >= DAL_CNT_CLEHIT_NUM) || (start+num > DAL_CNT_CLEHIT_NUM)) 
		{
			dalStatsUnLock();
			return OPL_ERR_INVALID_PARAMETERS;
		}
		OPL_MEMCPY(oplCounter, &g_astCntCleHit[start], sizeof(OPL_CNT_t)*num);
	}
	else
	{
		iStatus = OPL_ERROR;
	}

	dalStatsUnLock();

	return iStatus;
}


OPL_STATUS dalCounterEnable(unsigned int moduleBitmap)
{
	g_uiCntPollFlag = moduleBitmap;
	return OPL_OK;
}

OPL_STATUS dalCounterEnableGet(unsigned int *moduleBitmap)
{
	*moduleBitmap = g_uiCntPollFlag;
	return OPL_OK;
}

OPL_STATUS dalCounterClear(unsigned int moduleBitmap)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;

    dalStatsLock();

	if (moduleBitmap & DAL_CNT_POLL_GE_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_GE_NUM; ucCnt++)
		{
			g_astCntGe[ucCnt].stAdded.uiLow = 0;
			g_astCntGe[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_PON_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_PON_NUM; ucCnt++)
		{
			g_astCntPon[ucCnt].stAdded.uiLow = 0;
			g_astCntPon[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_FE_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_FE_NUM; ucCnt++)
		{
			g_astCntFe[ucCnt].stAdded.uiLow = 0;
			g_astCntFe[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_TM_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_TM_NUM; ucCnt++)
		{
			g_astCntTm[ucCnt].stAdded.uiLow = 0;
			g_astCntTm[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_CLE_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_CLE_NUM; ucCnt++)
		{
			g_astCntCle[ucCnt].stAdded.uiLow = 0;
			g_astCntCle[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_BRG_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_BRG_NUM; ucCnt++)
		{
			g_astCntBrg[ucCnt].stAdded.uiLow = 0;
			g_astCntBrg[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_MPCP_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_MPCP_NUM; ucCnt++)
		{
			g_astCntMpcp[ucCnt].stAdded.uiLow = 0;
			g_astCntMpcp[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_MARB_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_MARB_NUM; ucCnt++)
		{
			g_astCntMarb[ucCnt].stAdded.uiLow = 0;
			g_astCntMarb[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_CPDMA_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_CPDMA_NUM; ucCnt++)
		{
			g_astCntCPDma[ucCnt].stAdded.uiLow = 0;
			g_astCntCPDma[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_FEDMA_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_FEDMA_NUM; ucCnt++)
		{
			g_astCntFEDma[ucCnt].stAdded.uiLow = 0;
			g_astCntFEDma[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_GEPARSER_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_GEPARSER_NUM; ucCnt++)
		{
			g_astCntGeParser[ucCnt].stAdded.uiLow = 0;
			g_astCntGeParser[ucCnt].stAdded.uiHigh = 0;
		}
	}
	if (moduleBitmap & DAL_CNT_POLL_CLEHIT_MASK)
	{
		for (ucCnt=0; ucCnt<DAL_CNT_CLEHIT_NUM; ucCnt++)
		{
			g_astCntCleHit[ucCnt].stAdded.uiLow = 0;
			g_astCntCleHit[ucCnt].stAdded.uiHigh = 0;
		}
	}

    dalStatsUnLock();

	return iStatus;
}

static void PollingHandler(unsigned long unused)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
	UINT32 uiRegVal;

    while (1)
    {
        dalStatsLock();

    	if (g_uiCntPollFlag & DAL_CNT_POLL_GE_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_GE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntGe[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntGe[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntGe[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntGe[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntGe[ucCnt].stAdded.uiHigh += 1;
    				g_astCntGe[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntGe[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntGe[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_PON_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_PON_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntPon[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntPon[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntPon[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntPon[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntPon[ucCnt].stAdded.uiHigh += 1;
    				g_astCntPon[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntPon[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntPon[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_FE_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_FE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntFe[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntFe[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntFe[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntFe[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntFe[ucCnt].stAdded.uiHigh += 1;
    				g_astCntFe[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntFe[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntFe[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_TM_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_TM_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntTm[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntTm[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntTm[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntTm[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntTm[ucCnt].stAdded.uiHigh += 1;
    				g_astCntTm[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntTm[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntTm[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_CLE_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_CLE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntCle[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCle[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCle[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCle[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCle[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCle[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCle[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCle[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_BRG_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_BRG_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntBrg[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntBrg[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntBrg[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntBrg[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntBrg[ucCnt].stAdded.uiHigh += 1;
    				g_astCntBrg[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntBrg[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntBrg[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_MPCP_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_MPCP_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntMpcp[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntMpcp[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntMpcp[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiHigh += 1;
    				g_astCntMpcp[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntMpcp[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_MARB_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_MARB_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntMarb[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntMarb[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntMarb[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntMarb[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntMarb[ucCnt].stAdded.uiHigh += 1;
    				g_astCntMarb[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntMarb[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntMarb[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_CPDMA_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_CPDMA_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntCPDma[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCPDma[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCPDma[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCPDma[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCPDma[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_FEDMA_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_FEDMA_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntFEDma[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntFEDma[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntFEDma[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiHigh += 1;
    				g_astCntFEDma[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntFEDma[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_GEPARSER_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_GEPARSER_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntGeParser[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntGeParser[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntGeParser[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiHigh += 1;
    				g_astCntGeParser[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntGeParser[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & DAL_CNT_POLL_CLEHIT_MASK)
    	{
    		for (ucCnt=0; ucCnt<DAL_CNT_CLEHIT_NUM; ucCnt++)
    		{
    			iStatus = oplTabRead(OPCONN_TAB_CLS_COUNTER, ucCnt, 1, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCleHit[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCleHit[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCleHit[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCleHit[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}

        dalStatsUnLock();
        msleep_interruptible(1000); 
    }

	return;
}

OPL_STATUS dalStatsThreadInit(void)
{
    if (g_pstStatsThreadId) {
        return OPL_ERROR;
    }
	mutex_init(&g_pstStatsMutex);
	
    g_pstStatsThreadId = kthread_run(PollingHandler, NULL, "oplStatsTask");

    if (g_pstStatsThreadId == NULL) {
        return OPL_ERROR;
    }

    return OPL_OK;
}

OPL_STATUS dalStatsThreadShutdown(void)
{
    if (!g_pstStatsThreadId) {
        return OPL_ERROR;
    }

    if (g_pstStatsThreadId != NULL) {
        kthread_stop(g_pstStatsThreadId);
        g_pstStatsThreadId = NULL;
    }

    return OPL_OK;
}

OPL_STATUS dalPONPortStatisticsGet1(ONU_PON_Port_Statistics_Get_1_t * ponStats)
{
	dalStatsLock();

	ponStats->FramesTransmittedOK = (UINT64)g_astCntPon[2].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[2].stAdded.uiHigh << 32);
	ponStats->OctetsTransmittedOK = (UINT64)g_astCntPon[13].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[13].stAdded.uiHigh << 32);
	ponStats->MulticastFramesXmittedOK = (UINT64)g_astCntPon[4].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[4].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesXmittedOK = (UINT64)g_astCntPon[3].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[3].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesTransmittedOk = (UINT64)g_astCntPon[5].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[5].stAdded.uiHigh << 32);

	ponStats->FramesReceivedOK = (UINT64)g_astCntPon[16].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[16].stAdded.uiHigh << 32);
	ponStats->OctetsReceivedOK = (UINT64)g_astCntPon[37].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[37].stAdded.uiHigh << 32);
	ponStats->MulticastFramesReceivedOK = (UINT64)g_astCntPon[24].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[24].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesReceivedOK = (UINT64)g_astCntPon[23].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[23].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesReceivedOk = (UINT64)g_astCntPon[25].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[25].stAdded.uiHigh << 32);
	ponStats->SingleCollisionFrames = 0;
	ponStats->MultipleCollisionFrames = 0;
	ponStats->AlignmentErrors = (UINT64)g_astCntPon[18].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[18].stAdded.uiHigh << 32);
	ponStats->FrameTooLongErrors = (UINT64)g_astCntPon[21].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[21].stAdded.uiHigh << 32);

	dalStatsUnLock();

	return OPL_OK;
}

OPL_STATUS dalPONPortStatisticsGet2(ONU_PON_Port_Statistics_Get_2_t * ponStats)
{
	dalStatsLock();

	ponStats->CRC8Errors = (UINT64)g_astCntPon[18].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[18].stAdded.uiHigh << 32);
	ponStats->FECCorrectedBlocks = (UINT64)g_astCntPon[38].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[38].stAdded.uiHigh << 32);
	ponStats->FECUncorrectableBlocks = (UINT64)g_astCntPon[39].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[39].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesTransmitted = (UINT64)g_astCntPon[6].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[6].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesReceived = (UINT64)g_astCntPon[27].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[27].stAdded.uiHigh << 32);

	ponStats->MPCPTxGate = 0;
	ponStats->MPCPTxRegAck = (UINT64)g_astCntMpcp[13].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[13].stAdded.uiHigh << 32);
	ponStats->MPCPTxRegister = 0; /* OLT·¢µÄ */
	ponStats->MPCPTxRegRequest = (UINT64)g_astCntMpcp[12].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[12].stAdded.uiHigh << 32);
	ponStats->MPCPTxReport = (UINT64)g_astCntMpcp[14].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[14].stAdded.uiHigh << 32);
	ponStats->MPCPRxGate = (UINT64)g_astCntMpcp[0].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[0].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegAck = 0;
	ponStats->MPCPRxRegister = (UINT64)g_astCntMpcp[1].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[1].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegRequest = 0;
	ponStats->MPCPRxReport = 0;

	dalStatsUnLock();

	return OPL_OK;
}

OPL_STATUS dalPortFluxStatisticsCounterGet(ONU_Port_Flux_Statistics_Counter_t *portStats)
{
	dalStatsLock();

	portStats->ifInOctets =
	    (UINT64)g_astCntGe[34].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntGe[34].stAdded.uiHigh << 32);
	portStats->ifInUcastPkts =
        (UINT64)g_astCntGe[13].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[20].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[21].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32);
	portStats->ifInNUcastPkts =
        (UINT64)g_astCntGe[20].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) +
        (UINT64)g_astCntGe[21].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32);
	portStats->ifInDiscards =
        (UINT64)g_astCntGe[14].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[14].stAdded.uiHigh << 32);
	portStats->ifInErrors =
        (UINT64)g_astCntGe[19].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[19].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[13].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32);

    portStats->ifOutOctets =
        (UINT64)g_astCntGe[11].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[11].stAdded.uiHigh << 32);
	portStats->ifOutUcastPkts =
        (UINT64)g_astCntGe[0].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[0].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[1].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[2].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32);
	portStats->ifOutNUcastPkts =
        (UINT64)g_astCntGe[1].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) +
        (UINT64)g_astCntGe[2].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32);
	portStats->ifOutDiscards = 0;
	portStats->ifOutErrors = 0;

	dalStatsUnLock();

	return OPL_OK;
}



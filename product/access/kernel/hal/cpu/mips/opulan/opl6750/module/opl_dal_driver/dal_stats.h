/* 
 * Copyright (c) 2011 Qualcomm Atheros, Inc.. 
 * All Rights Reserved. 
 * Qualcomm Atheros Confidential and Proprietary. 
 *
 * Header Name: stats.h
 *
 * Revision History:
 * wfxu        2012/04/6      Initial Version
 *
 */

#ifndef __STATS_H__
#define __STATS_H__

#include "opl_driver.h"

#define DAL_CNT_POLL_GE_MASK		0x00000001
#define DAL_CNT_POLL_PON_MASK		0x00000002
#define DAL_CNT_POLL_FE_MASK		0x00000004
#define DAL_CNT_POLL_TM_MASK		0x00000008
#define DAL_CNT_POLL_CLE_MASK		0x00000010
#define DAL_CNT_POLL_BRG_MASK		0x00000020
#define DAL_CNT_POLL_MPCP_MASK		0x00000040
#define DAL_CNT_POLL_MARB_MASK		0x00000080
#define DAL_CNT_POLL_CPDMA_MASK		0x00000100
#define DAL_CNT_POLL_FEDMA_MASK		0x00000200
#define DAL_CNT_POLL_GEPARSER_MASK	0x00000400
#define DAL_CNT_POLL_CLEHIT_MASK	0x00000800
#define DAL_CNT_POLL_SWITCH_MASK	0x00001000

#define DAL_CNT_GE_NUM			38
#define DAL_CNT_PON_NUM			45
#define DAL_CNT_FE_NUM			9
#define DAL_CNT_TM_NUM			12
#define DAL_CNT_CLE_NUM			2
#define DAL_CNT_BRG_NUM			18
#define DAL_CNT_MPCP_NUM		17
#define DAL_CNT_MARB_NUM		6
#define DAL_CNT_CPDMA_NUM		11
#define DAL_CNT_FEDMA_NUM		7
#define DAL_CNT_GEPARSER_NUM	6
#define DAL_CNT_CLEHIT_NUM		128
#define DAL_CNT_MAX_NUM			128

#pragma pack(1)	

typedef struct OPL_CNT_ADD_s
{
	UINT32 uiHigh;
	UINT32 uiLow;
}OPL_CNT_ADD_t;

typedef struct OPL_CNT_s
{
	UINT16			usId;
	UINT8			pucName[32];
	OPL_CNT_ADD_t	stAdded;
	UINT8			pucHelp[80];
}OPL_CNT_t;


/* PON Counter Special Treatment Index */
typedef enum OPL_PON_CNT_ST_IDX_e
{
    OPL_PON_CNT_ST_IDX_TxTotalPkts = 2,
    OPL_PON_CNT_ST_IDX_TxMACControlPkts = 6,
    OPL_PON_CNT_ST_IDX_TxOAMPkts = 15,
    OPL_PON_CNT_ST_IDX_RxTotalPkts = 22,
    OPL_PON_CNT_ST_IDX_RxMACControlPkts = 27,
    OPL_PON_CNT_ST_IDX_RxOAMPkts = 42
}OPL_PON_CNT_ST_IDX_t;


/* MPCP Counter Special Treatment Index */
typedef enum OPL_MPCP_CNT_ST_IDX_e
{
    OPL_MPCP_CNT_ST_IDX_REGREQ = 12,
    OPL_MPCP_CNT_ST_IDX_REGACK = 13
}OPL_MPCP_CNT_ST_IDX_t;

typedef struct ONU_PON_Port_Statistics_Get_1_s{
	UINT64 FramesTransmittedOK;			/* LLID端口发送帧计数*/
	UINT64 OctetsTransmittedOK;			/* LLID端口发送字节计数*/
	UINT64 MulticastFramesXmittedOK;	/* LLID端口发送组播帧计数*/
	UINT64 BroadcastFramesXmittedOK;	/* LLID端口发送广播包计数 */
	UINT64 PONPauseFramesTransmittedOk;	/* PON端口发送PAUSE帧计数*/
	UINT64 FramesReceivedOK;			/*LLID端口接收帧计数*/
	UINT64 OctetsReceivedOK;			/* LLID端口接收字节计数 */
	UINT64 MulticastFramesReceivedOK;	/* LLID端口接收组播帧计数*/
	UINT64 BroadcastFramesReceivedOK;	/* LLID端口接收广播包计数 */
	UINT64 PONPauseFramesReceivedOk;	/* PON端口接收PAUSE帧计数*/
	UINT64 SingleCollisionFrames;		/*单次帧碰撞计数*/
	UINT64 MultipleCollisionFrames;		/*多次帧碰撞计数*/ 	
	UINT64 AlignmentErrors;				/*对齐错误计数*/
	UINT64 FrameTooLongErrors;			/*帧超长计数*/
}ONU_PON_Port_Statistics_Get_1_t;

typedef struct ONU_PON_Port_Statistics_Get_2_s{
	UINT64 CRC8Errors;						/* CRC错误计数 */
	UINT64 FECCorrectedBlocks;				/* FEC正确块计数 */
	UINT64 FECUncorrectableBlocks;			/* FEC错误块计数 */
	UINT64 MPCPMACCtrlFramesTransmitted;	/* MPCP控制帧发送计数*/
	UINT64 MPCPMACCtrlFramesReceived;		/* MPCP控制帧接收计数*/
	UINT64 MPCPTxGate;						/* MPCP Tx Gate计数*/
	UINT64 MPCPTxRegAck;					/* MPCP Tx Reg Ack计数*/
	UINT64 MPCPTxRegister;					/* MPCP Tx Register计数*/
	UINT64 MPCPTxRegRequest;				/* MPCP Tx Registe Requestr计数  */
	UINT64 MPCPTxReport;					/* MPCP Tx Report计数 */
	UINT64 MPCPRxGate;						/*MPCP Rx Gate计数*/
	UINT64 MPCPRxRegAck;					/*MPCP Rx Reg Ack计数*/
	UINT64 MPCPRxRegister;					/*MPCP Rx Register计数*/
	UINT64 MPCPRxRegRequest;				/*MPCP Rx RegRequest计数*/
	UINT64 MPCPRxReport;					/*MPCP Rx Report计数*/	
}ONU_PON_Port_Statistics_Get_2_t;

typedef struct ONU_Port_Flux_Statistics_Counter_s{
	UINT64 ifInOctets;       /* 收到的上行包字节数 */
	UINT64 ifInUcastPkts;    /* 收到的上行单播包数目 */
	UINT64 ifInNUcastPkts;   /* 收到的上行组播包数目 */
	UINT64 ifInDiscards;     /* 丢弃的上行包数目 */
	UINT64 ifInErrors;       /* 收到的上行错误包数目  */
	UINT64 ifOutOctets;      /* 收到的下行包字节数  */
	UINT64 ifOutUcastPkts;   /* 收到的下行单播包数目  */
	UINT64 ifOutNUcastPkts;  /* 收到的下行组播包数目  */
	UINT64 ifOutDiscards;    /* 丢弃的下行包数目  */
	UINT64 ifOutErrors;      /* 收到的下行包数目  */
}ONU_Port_Flux_Statistics_Counter_t;

#pragma pack()		/* end of pragma pack definition */

extern OPL_STATUS dalPONPortStatisticsGet1(ONU_PON_Port_Statistics_Get_1_t * ponStats);
extern OPL_STATUS dalPONPortStatisticsGet2(ONU_PON_Port_Statistics_Get_2_t * ponStats);
extern OPL_STATUS dalPortFluxStatisticsCounterGet(ONU_Port_Flux_Statistics_Counter_t * portStats);

extern OPL_STATUS dalCounterGet( unsigned int moduleBit, OPL_CNT_t * oplCounter, unsigned int start, unsigned int num );

extern OPL_STATUS dalCounterEnable(unsigned int moduleBitmap);
extern OPL_STATUS dalCounterEnableGet(unsigned int *moduleBitmap);

extern OPL_STATUS dalCounterClear(unsigned int moduleBitmap);

extern void dalStatsLock();
extern void dalStatsUnLock();

extern OPL_STATUS dalStatsThreadInit(void);
extern OPL_STATUS dalStatsThreadShutdown(void);

#endif


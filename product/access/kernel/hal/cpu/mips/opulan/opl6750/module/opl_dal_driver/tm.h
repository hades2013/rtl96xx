/*
=============================================================================
     Header Name: tm.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/11		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef TM_H
#define TM_H


#define TM_UP_STREAM_NUM    				8

#define TM_DOWN_STREAM_NUM			8
#define TM_UP_STREAM_NUM0				8
#define TM_WRED_C1_MAX_VAL          	16
#define TM_WRED_MAXP_FACTOR         	10
#define TM_US_Q_MAX_PKT_NUM   		8000

typedef enum UP_DOWN_STREAM_s
{
	UP_STREAM = 0,
	DOWN_STREAM = 1,
	BOTH_STREAM = 2
}UP_DOWN_STREAM_e;

typedef enum UP_QUEUE_PRI_s
{
	UP_Q_PRI0 = 0,
	UP_Q_PRI1 = 1,
	UP_Q_PRI2 = 2,
	UP_Q_PRI3 = 3,
	UP_Q_PRI4 = 4,
	UP_Q_PRI5 = 5,
	UP_Q_PRI6 = 6,
	UP_Q_PRI7 = 7	
}UP_QUEUE_PRI_e;

typedef enum DOWN_QUEUE_PRI_s
{
	DOWN_Q_PRI0 = 0,
	DOWN_Q_PRI1 = 1,
	DOWN_Q_PRI2 = 2,
	DOWN_Q_PRI3 = 3,
	DOWN_Q_PRI4 = 4,
	DOWN_Q_PRI5 = 5,
	DOWN_Q_PRI6 = 6,
	DOWN_Q_PRI7 = 7	
}DOWN_QUEUE_PRI_e;

typedef enum TM_US_WRED_TYPE_s
{
	US_WRED_MINTH,
	US_WRED_MAXTH,
	US_WRED_WEIGHT,
	US_WRED_C1,
	US_WRED_END,
}TM_US_WRED_TYPE_e;

typedef enum TM_DS_WRED_TYPE_s{
	DS_WRED_MINTH,
	DS_WRED_MAXTH,
	DS_WRED_WEIGHT,
	DS_WRED_C1,
	DS_WRED_END,
}TM_DS_WRED_TYPE_e;

typedef struct TM_US_Q_CONF_STA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:5;
	UINT32 bfNumOfCell:14;
	UINT32 bfNumOfPkt:13;

	UINT32 bfReserved1:5;
	UINT32 bfMaxNumOfCell:14;
	UINT32 bfMaxNumOfPkt:13;
#else
	UINT32 bfNumOfPkt:13;
	UINT32 bfNumOfCell:14;
	UINT32 bfReserved0:5;

	UINT32 bfMaxNumOfPkt:13;
	UINT32 bfMaxNumOfCell:14;
	UINT32 bfReserved1:5;
#endif
}TM_US_Q_CONF_STA_t;

typedef struct TM_DS_Q_CONF_STA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:5;
	UINT32 bfNumOfCell:14;
	UINT32 bfNumOfPkt:13;

	UINT32 bfReserved2:5;
	UINT32 bfMaxNumOfCell:14;
	UINT32 bfMaxNumOfPkt:13;
#else
	UINT32 bfNumOfPkt:13;
	UINT32 bfNumOfCell:14;
	UINT32 bfReserved0:5;

	UINT32 bfMaxNumOfPkt:13;
	UINT32 bfMaxNumOfCell:14;
	UINT32 bfReserved2:5;
#endif
}TM_DS_Q_CONF_STA_t;

typedef struct TM_US_Q_SHAPER_PA1_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:7;
	UINT32 bfPir:15;
	UINT32 bfPbs:10;
#else
	UINT32 bfPbs:10;
	UINT32 bfPir:15;
	UINT32 bfReserved0:7;
#endif
}TM_US_Q_SHAPER_PA1_t;

typedef struct TM_DS_Q_SHAPER_PA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:7;
	UINT32 bfCir:15;
	UINT32 bfCbs:10;
#else
	UINT32 bfCbs:10;
	UINT32 bfCir:15;
	UINT32 bfReserved0:7;
#endif
}TM_DS_Q_SHAPER_PA_t;

typedef struct TM_US_Q_SHAPER_PA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:7;
	UINT32 bfCir:15;
	UINT32 bfCbs:10;
#else
	UINT32 bfCbs:10;
	UINT32 bfCir:15;
	UINT32 bfReserved0:7;
#endif
}TM_US_Q_SHAPER_PA_t;

typedef struct TM_DS_WRED_PA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:16;
	UINT32 bfC1:4;
	UINT32 bfWeight:4;
	UINT32 bfMaxthH:8;

	UINT32 bfMaxthL:12;
	UINT32 bfMinth:20;
#else
	UINT32 bfMaxthH:8;
	UINT32 bfWeight:4;
	UINT32 bfC1:4;
	UINT32 bfReserved0:16;

	UINT32 bfMinth:20;
	UINT32 bfMaxthL:12;
#endif
}TM_DS_WRED_PA_t;

typedef struct TM_US_WRED_PA_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:16;
	UINT32 bfC1:4;
	UINT32 bfWeight:4;
	UINT32 bfMaxthH:8;

	UINT32 bfMaxthL:12;
	UINT32 bfMinth:20;
#else
	UINT32 bfMaxthH:8;
	UINT32 bfWeight:4;
	UINT32 bfC1:4;
	UINT32 bfReserved0:16;

	UINT32 bfMinth:20;
	UINT32 bfMaxthL:12;	
#endif
}TM_US_WRED_PA_t;


OPL_STATUS tmInit(void);


OPL_STATUS tmGeShaperEnableHwWrite(UINT8 stream,UINT8 enable);


OPL_STATUS tmGeShaperEnableHwRead(UINT8 stream,UINT8 *enable);

OPL_STATUS tmQueueShaperEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable);


OPL_STATUS tmQueueShaperCirEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable);


OPL_STATUS tmQueueShaperPirEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable);


OPL_STATUS tmQueueShaperEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable);


OPL_STATUS tmQueueShaperCirEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable);


OPL_STATUS tmQueueShaperPirEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable);


OPL_STATUS tmQueuePriHwWrite(UINT8 stream,UINT8 Qid,UINT8 priority);


OPL_STATUS tmQueuePriHwRead(UINT8 stream,UINT8 Qid,UINT8 *priority);


OPL_STATUS tmQueueWeightHwWrite(UINT8 stream,UINT8 Qid,UINT32 weight);


OPL_STATUS tmQueueWeightHwRead(UINT8 stream,UINT8 Qid,UINT32 *weight);


OPL_STATUS tmGeCirHwWrite(UINT8 stream,UINT32 geCir);


OPL_STATUS tmGeCirHwRead(UINT8 stream,UINT32 *geCir);


OPL_STATUS tmGeCbsHwWrite(UINT8 stream,UINT32 geCbs);


OPL_STATUS tmGeCbsHwRead(UINT8 stream,UINT32 *geCbs);


OPL_STATUS tmQueueIcosMapHwWrite(UINT8 stream,UINT8 cosVal,UINT8 icosVal);

OPL_STATUS tmQueueIcosMapHwRead(UINT8 stream,UINT8 cosVal,UINT8 *icosVal);

OPL_STATUS tmCosIcosMapHwWrite(UINT8 stream,UINT8 cos,UINT8 icos);

OPL_STATUS tmCosIcosMapHwRead(UINT8 stream,UINT8 cos,UINT8 *icos);



/**************************************************************************************************
 *		The clk number should be added in every period, for cir and cbs of tmds shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		Then TMDS_CLKNUM_PERIOD = 520000 - 15*32768 = 8480 (0x2120)
 *
 *************************************************************************************************/
OPL_STATUS tmQueuePeriodsOfClkNumHwWrite(UINT8 stream,UINT32 clkNum);


/**************************************************************************************************
 *		The clk number should be added in every period, for cir and cbs of tmds shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		Then TMDS_CLKNUM_PERIOD = 520000 - 15*32768 = 8480 (0x2120)
 *
 *************************************************************************************************/
OPL_STATUS tmQueuePeriodsOfClkNumHwRead(UINT8 stream,UINT32 *clkNum);



/**********************************************************************************************
 *		The clk number in every part, for cir and cbs of tmus shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		hen TMUS_STEP_CNT = 15-1
 *
 **********************************************************************************************/
OPL_STATUS tmQueueClkCountPerStepHwWrite(UINT8 stream,UINT32 clkCntPerStep);

/**********************************************************************************************
 *		The clk number in every part, for cir and cbs of tmus shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		hen TMUS_STEP_CNT = 15-1
 *
 **********************************************************************************************/
OPL_STATUS tmQueueClkCountPerStepHwRead(UINT8 stream,UINT32 *clkCntPerStep);



OPL_STATUS tmQueueMaxCellNumHwWrite(UINT8 stream,UINT8 Qid,UINT16 maxCellNum);

OPL_STATUS tmQueueMaxCellNumHwRead(UINT8 stream,UINT8 Qid,UINT16 *maxCellNum);


OPL_STATUS tmQueueMaxPktNumHwWrite(UINT8 stream,UINT8 Qid,UINT16 maxPktNum);


OPL_STATUS tmQueueMaxPktNumHwRead(UINT8 stream,UINT8 Qid,UINT16 *maxPktNum);


OPL_STATUS tmPktNumInQueueHwRead(UINT8 stream,UINT8  Qid,UINT32 *numOfPkt);


OPL_STATUS tmCellNumInQueueHwRead(UINT8 stream,UINT8  Qid,UINT32 *numOfCell);


OPL_STATUS tmWredEntryHwWrite(UINT8 stream,UINT8 queueId,UINT32 *pEntry);


OPL_STATUS tmWredEntryHwRead(UINT8 stream,UINT8 queueId,UINT32 *pEntry);


OPL_STATUS tmQueueCbsHwWrite(UINT8 stream,UINT8 Qid,UINT32 cbs);


OPL_STATUS tmQueueCbsHwRead(UINT8 stream,UINT8 Qid,UINT32 *cbs);

OPL_STATUS tmQueueCirHwWrite(UINT8 stream,UINT8 Qid,UINT32 cir);

OPL_STATUS tmQueueCirHwRead(UINT8 stream,UINT8 Qid,UINT32 *cir);

OPL_STATUS tmWredCfgSet(UINT8 stream,UINT8 queueId, UINT8 type,UINT32 value);


OPL_STATUS tmWredCfgGet(UINT8 stream,UINT8 queueId,UINT8 type,UINT32 *value);

OPL_STATUS tmWredEnableHwWrite(UINT8 stream,UINT8 queueId,UINT32 enable);

OPL_STATUS tmWredEnableHwRead(UINT8 stream,UINT8 queueId,UINT32 *enable);
OPL_STATUS tmWredConfigAll(UINT8 stream,UINT8 queueId,UINT8 weight,UINT32 maxTh,UINT32 minTh);


OPL_STATUS tmUsShaperTabCirDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs);
OPL_STATUS tmUsShaperTabPirDirectHwWrite(UINT32 entryId,UINT32 pir,UINT32 pbs);
OPL_STATUS tmUsShaperTabDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs);
OPL_STATUS tmDsShaperTabDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs);
OPL_STATUS tmQueueTabDirectHwWrite(UINT32 entryId,UINT32 maxCellNum,UINT32 maxPktNum);
OPL_STATUS tmUsWredTabDirectHwWrite(UINT32 entryId,UINT32 weight,UINT32 minth,UINT32 maxth);
OPL_STATUS tmDsWredTabDirectHwWrite(UINT32 entryId,UINT32 weight,UINT32 minth,UINT32 maxth);
OPL_STATUS tmMaxpUsWredTabDirectHwWrite(UINT32 entryId,UINT8 c1,UINT32 weight,UINT32 minth,UINT32 maxth);

OPL_STATUS tmMaxpDsWredTabDirectHwWrite(UINT32 entryId,UINT8 c1,UINT32 weight,UINT32 minth,UINT32 maxth);


#endif

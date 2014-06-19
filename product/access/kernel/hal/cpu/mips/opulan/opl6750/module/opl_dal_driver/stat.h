/*
=============================================================================
     Header Name: stat.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/7		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef  STAT_H
#define  STAT_H
typedef enum OPCONN_INTERFACE_s
{
	RX_etherStatsDropEvents_Count = 0,
	RX_etherStatsOctets_Count,
	RX_etherStatsPkts_Count,
	RX_etherStatsBroadcastPkts_Count,
	RX_etherStatsMulticastPkts_Count,
	RX_etherStatsCRCAlignErrors_Count,
	RX_etherStatsUndersizePkts_Count,
	RX_etherStatsOversizePkts_Count,
	RX_etherStatsJabbers_Count,
	RX_etherStatsPkts64Ocets_Count,
	RX_etherStatsPkts65to127Ocets_Count,
	RX_etherStatsPkts128to255Ocets_Count,
	RX_etherStatsPkts256to511Ocets_Count,
	RX_etherStatsPkts512to1023Ocets_Count,
	RX_etherStatsPkts1024to1518Ocets_Count,
	RX_aMACControlFrames_Count,
	RX_aPAUSEMACCtrlFrames_Count,
	RX_aUnsupportOpcodes_Count,


	Tx_etherStatsOcets_Count,
	Tx_etherStatsPkts_Count,
	Tx_etherStatsBroadcastPkts_Count,
	Tx_etherStatsMulticastPkts_Count,
	Tx_etherStatsPkts64Ocets_Count,
	Tx_etherStatsPkts65to127Ocets_Count,
	Tx_etherStatsPkts128to255Ocets_Count,
	Tx_etherStatsPkts256to511Ocets_Count,
	Tx_etherStatsPkts512to1023Ocets_Count,
	Tx_etherStatsPkts1024to1518Ocets_Count,
	Tx_aPAUSEMACCtrlFrames_Count,
	Tx_aMACControlFrames_Count,
	
	RX_TX_INTERFACE_COUNT_END	
} OPCONN_INTERFACE_e;

typedef enum OPCONN_BRG_s
{
	Rx_GE_PortDisabledDropPkts_Count = 0,
	Rx_PON_PortDisabledDropPkts_Count,
	
	Tx_GE_PortDisableDropPkts_Count,
	Tx_PON_PortDisableDropPkts_Count,

	VTT_UP_ERROR_Count,
	VTT_DN_ERROR_Count,

	VLAN_GE_PORT_ERROR_DROP_Pkts_Count,
	VLAN_PON_PORT_ERROR_DROP_Pkts_Count,

	OPCONN_BRG_COUNT_END
} OPCONN_BRG_e;

typedef enum OPCONN_TM_s
{
	HOST_BufferOverflowDropPkts_Count = 0,
	UP_FlowQueueLengthFullDropPkts_Count,
	DN_FlowQueueLengthFullDropPkts_Count,
	OPCONN_TM_COUNT_END
} OPCONN_TM_e;

typedef enum OPCONN_CLS_s
{
	CLS_MISS_Count = 0,
	CLS_HIT_Count,
	OPCONN_CLS_COUNT_END
} OPCONN_CLS_e;

typedef struct OPCONN_RMON_STAT_s{
	UINT8 		*name;
	UINT32 	regId;
	UINT32 	regAddr;
	UINT32 	regMask;
	UINT64 	counter;
	UINT32  *pTokenCounter;
}OPCONN_RMON_STAT_t;


/*******************************************************************************
* statInterValSet
*
* DESCRIPTION:
* 
*	this function is used to set the stat interval /us
* 
* INPUTS: 
*		interVal
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statInterValSet(UINT32 interVal);

/*******************************************************************************
* statInterValGet
*
* DESCRIPTION:
* 
*	this function is used get the stat interval
* 
* INPUTS: 
*		
* OUTPUTS: 
*		interVal:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statInterValGet(UINT32 *interVal);

/*******************************************************************************
* statRmonEnable
*
* DESCRIPTION:
* 
*	this function is used to enable or disable rmon style stat.
* 
* INPUTS: 
*		enable:0 for disable while 1 for enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statRmonEnable(UINT8 enable);


/*******************************************************************************
* statUpdate
*
* DESCRIPTION:
* 
*	this function is used to update the counter per interval
* 
* INPUTS: 
*		mode: module such as brg,tm,mpcp and so on 
* OUTPUTS: 
*	 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS statUpdate(UINT8 mode);

/*******************************************************************************
* statsThread
*
* DESCRIPTION:
* 
*	statsThread
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsThread(void);

/*******************************************************************************
* statsShow
*
* DESCRIPTION:
* 
*	this function is used show all counter 
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsShow(void);

/*******************************************************************************
* statsClear
*
* DESCRIPTION:
* 
*	this function is used clear all stats 
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsClear(void);


/*******************************************************************************
* statsTokenCreate
*
* DESCRIPTION:
* 
*	this function is used to create token counter buff
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsTokenCreate(UINT32 numOfToken);


/*******************************************************************************
* statsTokenDestroy
*
* DESCRIPTION:
* 
*	this function is used to create token counter buff
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsTokenDestroy(void);


/*******************************************************************************
* statsTokenSet
*
* DESCRIPTION:
* 
*	this function is used to enable and set the token num
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsTokenSet(UINT8 enable,UINT32 tokenNum);


/*******************************************************************************
* statsTokenCounterShow
*
* DESCRIPTION:
* 
*	this function is used to enable and set the token num
* 
* INPUTS: 
*		
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void statsTokenCounterShow(UINT32 startTokenId,UINT32 endTokenId);
#endif

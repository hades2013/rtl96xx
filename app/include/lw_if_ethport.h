/*****************************************************************************
 Copyright (c) 2011, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
------------------------------------------------------------------------------
                            if_ethport.h
  Project Code: Miniware2
   Module Name:
  Date Created: 2011-4-22 
        Author: tangxiaohu
   Description: if_eth_port.c header file 

------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifndef _IF_ETHPORT_H_
#define _IF_ETHPORT_H_

#ifdef  __cplusplus
extern "C"{
#endif

typedef enum tagPORTSTATE_E
{
    ETHPORT_DOWN,
    ETHPORT_UP,
    ETHPORT_STATE_ALL
}PORTSTATE_E;

typedef enum tagDUPLEX_E
{
    DUPLEX_AUTO,    
    DUPLEX_HALF,
    DUPLEX_FULL        
}DUPLEX_E;

typedef enum tagSPEED_E
{
    SPEED_AUTO,    
    SPEED_10M,
    SPEED_100M,
    SPEED_1000M  
}SPEED_E;
#define MACLEN 18

typedef enum tagFIBERINFOFLAG_E
{
    FIBER_INFO_VALID,
    FIBER_INFO_INVALID
}FIBERINFOFLAG_E;
#define MAX_FIBER_INFO_LEN 512

typedef struct tagETHPORT_S
{
    PORTSTATE_E enState;
    UCHAR     ucMac[MACLEN];
    DUPLEX_E  enDuplex; 
    SPEED_E   enSpeed;
    UINT      uiPortid;
    FIBERINFOFLAG_E enFiberInfoFlag;
    UCHAR    *pucFiberInfo;
    USHORT   usPortStatus;
    ULONG    ulBelongToPA;
    ULONG    ulMasterPort;
    ULONG    ulAggGroupID;
    ULONG    ulUseResource;
    ULONG    ulBlockStatus;
    ULONG    ulPortModeInAgg;
    ULONG    ulPAmode;
    USHORT   usLinkStatus;
    USHORT   usDuplexSet;
    USHORT   usActualDuplex;
    USHORT   usStpState;
    USHORT   usSpeedSet;
    USHORT   usActualSpeed;
    ULONG    ulPortDefaultMdi;
    ULONG    ulPortDefaultDuplex;
    ULONG    ulPortDefaultSpeed;
    ULONG    uiIsFiber;
}ETHPORT_S;

typedef struct tagIf_EthFunc
{
    IF_RET_E (*Init)(ifindex_t);   
    IF_RET_E (*SetState)(ifindex_t, UINT32);
    IF_RET_E (*GetState)(ifindex_t, UINT32 *);
    IF_RET_E (*SetSpeed)(ifindex_t, UINT32);
    IF_RET_E (*GetSpeed)(ifindex_t, UINT32 *);
    IF_RET_E (*SetDuplex)(ifindex_t, UINT32);
    IF_RET_E (*GetDuplex)(ifindex_t, UINT32 *);
    IF_RET_E (*SetMdix)(ifindex_t, UINT32);
    IF_RET_E (*GetMdix)(ifindex_t, UINT32 *);
   
}IF_ETH_FUNC_S;

extern IF_ETH_FUNC_S g_stEthPortFunc;

extern IF_RET_E IF_EthPortGetDuplex(ifindex_t uiIfindex,UINT32 *puiDuplex);
extern IF_RET_E IF_EthPortGetMdix(ifindex_t uiIfindex,UINT32 *puiMdix);
extern IF_RET_E IF_EthPortGetSpeed(ifindex_t uiIfindex,UINT32 *puiSpeed);
extern IF_RET_E IF_EthPortGetState(ifindex_t uiIfindex,UINT32 *puiState);
extern IF_RET_E IF_EthPortInit(ifindex_t uiIfindex);
extern IF_RET_E IF_EthPortSetDuplex(ifindex_t uiIfindex,UINT32 uiDuplex);
extern IF_RET_E IF_EthPortSetMdix(ifindex_t uiIfindex,UINT32 uiMdix);
extern IF_RET_E IF_EthPortSetSpeed(ifindex_t uiIfindex,UINT32 uiSpeed);
extern IF_RET_E IF_EthPortSetState(ifindex_t uiIfindex,UINT32 uiState);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_ETHPORT_H_ */

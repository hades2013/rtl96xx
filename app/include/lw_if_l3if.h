/*****************************************************************************
 Copyright (c) 2011, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
------------------------------------------------------------------------------
                            if_l3if.h
  Project Code: Miniware2
   Module Name:
  Date Created: 2011-6-15
        Author: wanghuanyu
   Description: if_l3if.c header file 

------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifndef _IF_L3IF_H_
#define _IF_L3IF_H_

#ifdef  __cplusplus
extern "C"{
#endif
///#ifdef

typedef struct tagL3IF_S
{  
    #ifdef CONFIG_IPV6
    #endif     
    UINT   uiIpAddr;
    UINT   uiMask;
    UINT   uiGateWay;
    UINT   uiMtu;
    UINT   uiPhyid;
    CHAR   szLinkName[IFNAMESIZE];/*netdevice name */
    CHAR   szNetName[IFNAMESIZE];/*for ppp*/
    CHAR   szMacaddr[MACADDRSIZE];
    UINT   uiDnsMst;
    UINT   uiDnsSlv;
    UINT   uiWorkMode;
    ULONG uiLinkState;
    VOID * pPri;
}L3IF_S;
//#if defined(__KERNEL__)
IF_RET_E IF_L3IfInit(IN ifindex_t uiIfindex,INOUT VOID *pvPara);
VOID * IF_L3IfAlloc(IF_SUB_TYPE_E enIftype);
VOID  IF_L3IfFree(CONST VOID * pData);
IF_RET_E  IF_L3IfGetAttr(ifindex_t uiIfindex,INT iAttrId, VOID * pValue ,UINT iLen);
IF_RET_E IF_L3IfSetAttr(ifindex_t uiIfindex, INT iAttrId,VOID * pValue,UINT uiLen );
VOID IF_L3vlanRegister(VOID);
VOID IF_L3vlanUnRegister(VOID);
//#endif







#ifdef  __cplusplus
}
#endif
#endif


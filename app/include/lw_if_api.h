/*****************************************************************************
 
*****************************************************************************/
#ifndef _IF_API_H_
#define _IF_API_H_

#ifdef  __cplusplus
extern "C"{
#endif
#include "lw_if_ethport.h"
/*和主类型对应，每个主类型对应一个操作集*/
typedef struct tagIF_FUNC
{
    IF_ETH_FUNC_S *pstEthPortFuncs;      
/*  IF_VLAN_FUNC_S *pstVlanFuncs; */
/*  IF_ATM_FUNC_S *pstAtmFuncs; */
/*  IF_WIRELESS_FUNC_S *pstWirelessFuncs; */
    
}IF_FUNC_S;


//#define DEFINE_IFINIT(_fun)     do{g_szIfFunc[IF_MAIN_TYPE].IF_Init = _fun;}while(0);

IF_RET_E IF_InitInterface(IN ifindex_t uiIfindex);
IF_RET_E IF_GetPseudoName(IN ifindex_t uiIfindex, OUT CHAR * pcName);
IF_RET_E IF_GetPhysicalId(IN ifindex_t uiIfindex, OUT UINT32 *puiPid);
IF_RET_E IF_GetState(IN ifindex_t uiIfindex, OUT UINT32 *puiState);
IF_RET_E IF_GetSpeed(IN ifindex_t uiIfindex, OUT UINT32 *puiSpeed);
IF_RET_E IF_GetDuplex(IN ifindex_t uiIfindex, OUT UINT32 *puiDuplex);
IF_RET_E IF_GetMdix(IN ifindex_t uiIfindex, OUT UINT32 *puiMdix);
IF_RET_E IF_SetIpMask(IN ifindex_t uiIfindex,IN CHAR *pIpstr,IN CHAR *pMskstr);
IF_RET_E IF_SetGateWay(IN ifindex_t uiIfindex,IN CHAR *pGwstr);
IF_RET_E IF_SetMtu(IN ifindex_t uiIfindex,IN UINT uiMtu);
IF_RET_E IF_Destroy(IN ifindex_t uiIfindex);
IF_RET_E IF_Enable(IN ifindex_t uiIfindex);
IF_RET_E IF_Disable(IN ifindex_t uiIfindex);
//IF_RET_E IF_GetWebName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize);
//IF_RET_E IF_GetCliName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize);
//IF_RET_E IF_Userstr2ifame(INOUT CHAR *str,INOUT CHAR *pszName,IN UINT uiSize);





#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_API_H_ */

/*****************************************************************************
  --------------------------------------------------------------------------
  wanghuanyu                                                                           
*****************************************************************************/
#ifndef _IF_TYPE_H_
#define _IF_TYPE_H_
    
#ifdef  __cplusplus
    extern "C"{
#endif


typedef enum tagIf_Main_type {
    IF_MAIN_TYPE_ROOT = 0,
    IF_MAIN_TYPE_ETH, 
    IF_MAIN_TYPE_ETHPORT, 
    IF_MAIN_TYPE_L3VLAN, 
    IF_MAIN_TYPE_ATM,         
    IF_MAIN_TYPE_WIRELESS,   
    IF_MAIN_TYPE_ALL,   
    IF_MAIN_TYPE_COUNT
}IF_MAIN_TYPE_E;

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_TYPE_H_ */



/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include <lw_if_pub.h>
#include <lw_if_l3if.h>
#include <linux/slab.h>
#include <lw_if_obj.h>
#ifdef UT_TEST_X86
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define kmalloc(a,b) malloc(a)
#define kfree(a)   free((void *)a) 
#define printk printf
#endif
DEFINE_IF_REGISTER(stEth);

/*****************************************************************************
    Func Name: IF_EthRegister()
                                                                            
*****************************************************************************/
VOID IF_EthRegister(VOID)
{
    memset(&stEth,0,sizeof(IF_REGISTER_BLOCK));
    stEth.init=IF_L3IfInit;
    stEth.alloc=IF_L3IfAlloc;
    stEth.free =IF_L3IfFree;
    stEth.attribute=IF_L3IfGetAttr;
    stEth.setattr=IF_L3IfSetAttr;
    K_IF_Register(IF_SUB_ETH,&stEth);     
    return;
}
/*****************************************************************************
    Func Name: IF_EthUnRegister()
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
VOID IF_EthUnRegister(VOID)
{
    K_IF_UnRegister(IF_SUB_ETH);     
    return;
}
#ifdef  __cplusplus
}
#endif


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
DEFINE_IF_REGISTER(stL3vlan);

/*****************************************************************************
    Func Name: IF_L3vlanRegister()
                                                                            
*****************************************************************************/
VOID IF_L3vlanRegister(VOID)
{
    memset(&stL3vlan,0,sizeof(IF_REGISTER_BLOCK));
    stL3vlan.init=IF_L3IfInit;
    stL3vlan.alloc=IF_L3IfAlloc;
    stL3vlan.free =IF_L3IfFree;
    stL3vlan.attribute=IF_L3IfGetAttr;
    stL3vlan.setattr=IF_L3IfSetAttr;
    K_IF_Register(IF_SUB_L3VLAN,&stL3vlan);     
    return;
}
/*****************************************************************************
    Func Name: IF_L3vlanUnRegister()
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
VOID IF_L3vlanUnRegister(VOID)
{
    K_IF_UnRegister(IF_SUB_L3VLAN);     
    return;
}

#ifdef  __cplusplus
}
#endif



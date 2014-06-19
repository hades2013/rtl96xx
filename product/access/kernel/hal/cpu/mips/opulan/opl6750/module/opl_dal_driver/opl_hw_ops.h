#ifndef _OPL_HW_H_
#define _OPL_HW_H_
/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

*  @defgroup 
*  @copyright 
*  @author 
*  @version 
*  @date 
*  @histroy 
*  author		 date		 description
*  -------------------------------------------------------------------

*  @{
*/
#ifndef __KERNEL__
#include <opl_typedef.h>
#endif

/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define IPMUX_REG_BASE			0xBF000000	/* default IPMux register address space */




/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */





/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */





/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*-------------------local  function declaration---------------------- */





/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */





/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */





/** @}*/


/** @defgroup

*  @{
*/
/*----------------------local  function definition--------------------- */

#define IPMUX_REG_ID2ADDR(regId) 	((u32)IPMUX_REG_BASE + (u32)(regId))
#define IPMUX_REG_ADDR2ID(addr) 	((u32)(addr) - (u32)IPMUX_REG_BASE)
#ifdef __KERNEL__
static inline int ipMuxRegRead(u32 regID, volatile u32 *pval)
{
  if (regID % 4 != 0)
    return -1;

  *(u32*)(pval) = *(volatile u32 *)IPMUX_REG_ID2ADDR(regID);

  return 0;
}

static inline int ipMuxRegWrite(u32 regID, u32 val)
{
  if (regID % 4 != 0)
    return -1;

  *((volatile u32 *)IPMUX_REG_ID2ADDR(regID)) = (u32)(val);

  return 0;
}
#else
extern char *g_pbuf;
static inline int ipMuxRegRead(u32 regID, volatile u32 *pval)
{
	if (regID % 4 != 0)
		return -1;

        *(u32*)(pval) = *(volatile u32 *)(g_pbuf + regID);
        return 0;
}

static inline int ipMuxRegWrite(u32 regID, u32 val)
{
	if (regID % 4 != 0)
		return -1;

        *((volatile u32 *)(g_pbuf + regID)) = (u32)(val);
        return 0;
}

#endif



/** @}*/
/** @}*/
#endif/* _OPL_HW_H_ */

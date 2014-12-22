#ifndef RLX5281_H
#define RLX5281_H

#include "regdef.h"
#include "soc.h"

#define BPCTL_REG $4
#define CCTL_REG $20
#define CACHE_OP_D_INV		(0x11)
#define CACHE_OP_D_WB_INV	(0x15)
#define CACHE_OP_D_WB		(0x19)
#define CACHE_OP_I_INV		(0x10)
#define DCACHE_LINE_SIZE	(CACHELINE_SIZE)
#define ICACHE_LINE_SIZE	(CACHELINE_SIZE)

#define __TO_STR(x) #x
#define TO_STR(x) __TO_STR(x)

#define __asm_mfc0(mfc_reg, mfc_sel) ({\
    unsigned int __ret;\
	__asm__ __volatile__ (\
	    "mfc0 %0," TO_STR(mfc_reg) "," TO_STR(mfc_sel) ";\n\t"\
	    : "=r" (__ret));\
	    __ret;})

#define __asm_mtc0(value, mtc_reg, mtc_sel) ({\
    unsigned int __value=(value);\
    __asm__ __volatile__ (\
        "mtc0 %0, " TO_STR(mtc_reg) "," TO_STR(mtc_sel) ";\n\t"\
        : : "r" (__value)); })

#define asm_mfc0(mfc_reg) __asm_mfc0(mfc_reg, 0)
#define asm_mfc0_3(mfc_reg) __asm_mfc0(mfc_reg, 3)
#define asm_mtc0(value, mtc_reg) __asm_mtc0(value, mtc_reg, 0)

#define asm_mflxc0(mflxc0_reg) ({\
    unsigned int __ret;\
	__asm__ __volatile__ (\
	    "mflxc0 %0," TO_STR(mflxc0_reg) ";\n\t"\
	    : "=r" (__ret));\
	    __ret;})
	    
#define asm_mtlxc0(value, mtlxc_reg) ({\
    unsigned int __value=(value);\
    __asm__ __volatile__ (\
        "mtlxc0 %0, " TO_STR(mtlxc_reg) ";\n\t"\
        : : "r" (__value)); })

#endif //RLX5281_H

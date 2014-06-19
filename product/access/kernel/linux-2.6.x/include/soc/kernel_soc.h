#ifndef KERNEL_SOC_H
#define KERNEL_SOC_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <soc.h>

#define printf printk
#undef parameters
extern parameter_to_bootloader_t kernel_soc_parameters;
#define parameters kernel_soc_parameters
#define parameter_soc_rwp   ((soc_t*)(&(parameters.soc)))
#define para_flash_info     (parameters.soc.flash_info)
extern u32 otto_get_flash_size(void);

#ifdef USE_SOC_SPARE
extern flash_info_t *get_flash_spare(u32_t *num_instances);
#endif //USE_SOC_SPARE

#ifdef pblr_puts
#undef pblr_puts
#endif /*pblr_pus*/
#define pblr_puts printk

#endif //#ifndef KERNEL_SOC_H


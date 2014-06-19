#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#if 0
#include "common/type.h"


extern int rtk_flash_direct_erase(uint32 addr, uint32 len);
extern int rtk_flash_direct_write(uint32 to, uint32 len, const uint8 *buf);
extern int rtk_flash_direct_read(uint32 from, uint32 len, uint8 *buf);

#define PHY_ADDR_TO_FLASH_ADDR(phy_addr)   ((uint32)phy_addr - FLASH_BASE)
#define FLASH_ADDR_TO_PHY_ADDR(flash_addr) ((uint32)flash_addr + FLASH_BASE)

#endif
#include <linux/autoconf.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>

#define uint8 unsigned char
#define uint32 unsigned int

#define FLASH_DEV_NAME "bootenv" //modify by dengjian for QID0013
extern struct mtd_info *get_mtd_device_nm(const char *name);

/* addr: 0~8M*/
int drv_flash_sect_erase (uint32 addr_first, uint32 addr_last);
int drv_flash_read(uint8 * src, uint32 addr, uint32 cnt);
int drv_flash_write (uint8 *src, uint32 addr, uint32 cnt);


#endif //DRV_FLASH_H

/**
 *  SPI Flash common control code.
 *  (C) 2006 Atmark Techno, Inc.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/gen_probe.h>

#include <bspchip.h>
#include <kernel_soc.h>
#include "spi_flash.h"

/*begin add by shipeng for watchdog*/
void feed_once_watchdog(void)
{
	extern unsigned int watchdog_counter;
	watchdog_counter++;
	//*((volatile unsigned int *)(0xb8003260)) |= (0x1 << 31);
}
/*end add by shipeng for watchdog*/

extern s32_t flash_unit_erase(const u32_t, u32_t);
extern s32_t flash_unit_write(const u32_t, u32_t, const u32_t, const void*);
extern s32_t flash_read(const u32_t, u32_t, const u32_t, void*);

spinlock_t spi_lock = SPIN_LOCK_UNLOCKED;

int mtd_spi_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	unsigned long adr, len;
	unsigned long cs, spc, cadr;
	int ret = 0;
	unsigned long  flags;
	//if (!chip_info->erase)
	//	return -EOPNOTSUPP;

	// skip 1st block erase
	if (instr->addr < (mtd->erasesize)) {
		instr->state = MTD_ERASE_DONE;
		return 0;
	}
	if (instr->addr & (mtd->erasesize - 1))
		return -EINVAL;

/*
	if (instr->len & (mtd->erasesize -1))
		return -EINVAL;
*/
	if ((instr->len + instr->addr) > mtd->size)
		return -EINVAL;
	adr = instr->addr;
	len = instr->len;
	KDEBUG("mtd_spi_erase():: adr: 0x%08lx, len: 0x%08lx\n", adr, len);
	if (len & (mtd->erasesize - 1)) {
		len = len - (len & (mtd->erasesize - 1)) + mtd->erasesize;
	}
	if (len < mtd->erasesize)
		len = mtd->erasesize;

	spc = (0x1 << para_flash_info.size_per_chip);

	spin_lock_irqsave(&spi_lock, flags);
	while (len) {
		KDEBUG("%s:: spi_erase:addr=0x%08lx\n", __FUNCTION__, adr);
		cs = adr/spc;
		cadr = adr%spc;
		feed_once_watchdog();/*add by shipeng for watchdog*/
		ret = flash_unit_erase(cs, cadr);
		feed_once_watchdog();/*add by shipeng for watchdog*/
		if (ret) {
			spin_unlock_irqrestore(&spi_lock, flags);
			return ret;
		}
		adr += mtd->erasesize;
		len -= mtd->erasesize;
	}
	spin_unlock_irqrestore(&spi_lock, flags);

	instr->state = MTD_ERASE_DONE;
	if (instr->callback)
		instr->callback(instr);
	return 0;
}

static u32 spi_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	memcpy((void *)to, (void *)(from + SPI_NOR_FLASH_START_ADDR), size);

	return 0;
}

int mtd_spi_read(struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
	int ret = 0;
	unsigned long flags;
	u32 cs, spc, pio_from, pio_len, pio_iter_len;

	KDEBUG("mtd_spi_read():: adr: 0x%08x, len: %08x\n", (u32) from, len);
	spin_lock_irqsave(&spi_lock, flags);
	if((from + len) <= SPI_NOR_FLASH_MMIO_SIZE) {
		/* Use MMIO read */
		feed_once_watchdog();/*add by shipeng for watchdog*/
		ret = spi_copy_to_dram(from, (u32)buf, len);
		feed_once_watchdog();/*add by shipeng for watchdog*/
	} else {
		/* Use PIO read */
		pio_from = from;
		pio_len = len;
		pio_iter_len = 0;
		spc = (0x1 << para_flash_info.size_per_chip);
		cs = pio_from/spc;
		pio_from = pio_from%spc;
		while(pio_len) {
			if( (pio_from + pio_len) > spc) {
				pio_iter_len = spc - pio_from;	
				KDEBUG("flash_read: cs=%u pio_from=0x%08x pio_iter_len=%u buf=0x%p\n", cs, pio_from, pio_iter_len, buf);
				feed_once_watchdog();/*add by shipeng for watchdog*/
				ret = flash_read(cs, pio_from, pio_iter_len, buf);
				feed_once_watchdog();/*add by shipeng for watchdog*/
				cs++;
				pio_from = 0;
				buf += pio_iter_len;
				pio_len -= pio_iter_len; 
			} else {
				KDEBUG("flash_read: cs=%u pio_from=0x%08x pio_len=%u buf=0x%p\n", cs, pio_from, pio_len, buf);
				feed_once_watchdog();/*add by shipeng for watchdog*/
				ret = flash_read(cs, pio_from, pio_len, buf);
				feed_once_watchdog();/*add by shipeng for watchdog*/
				break;
			}
		}
	}
	spin_unlock_irqrestore(&spi_lock, flags);

	if (ret)
		return ret;
	if (retlen)
		(*retlen) = len;
	return 0;
}

#define IS_OVERLAP(a, b, c, d) ((b>c) && (a<d))
extern u32_t wr_boundary;
int mtd_spi_write(struct mtd_info *mtd, loff_t to, size_t len,
		  size_t * retlen, const u_char * buf)
{
	int ret = 0;
	unsigned long flags;
	unsigned int remaining_size;
	unsigned int cur_addr;
	void *dp;/* Current data pointer */

	KDEBUG("mtd_spi_write():: adr: 0x%08x, len: 0x%08x, buf: %p\n", (u32) to, len, buf);
	remaining_size = len;
	cur_addr = to;
	dp = (void *)buf;
	if(IS_OVERLAP(CADDR(buf), CADDR(buf)+len, SPI_NOR_FLASH_MMIO_BASE0, SPI_NOR_FLASH_MMIO_BASE0+SPI_NOR_FLASH_MMIO_SIZE0) ||
	   IS_OVERLAP(CADDR(buf), CADDR(buf)+len, SPI_NOR_FLASH_MMIO_BASE1, SPI_NOR_FLASH_MMIO_BASE1+SPI_NOR_FLASH_MMIO_SIZE1) ) { 
		printk(KERN_ERR "ERROR: SPI Write from MMIO range(0x%p).\n", (CADDR(buf) + len)); 
		return -EINVAL;
	}
	spin_lock_irqsave(&spi_lock, flags);
	while(remaining_size>0) {
		unsigned int iter_size;
		unsigned int cs, spc;
		
		iter_size = (wr_boundary-1);
		iter_size &= cur_addr;
		iter_size = wr_boundary - iter_size;
		if(iter_size > remaining_size) {
			iter_size = remaining_size;
		}
		KDEBUG("%s:: flash_unit_write: cur_addr=0x%08x, iter_size=0x%08x, dp=%p\n", __FUNCTION__, cur_addr, iter_size, dp);
		spc = (0x1 << para_flash_info.size_per_chip);
		cs = cur_addr/spc;
		feed_once_watchdog();/*add by shipeng for watchdog*/
		ret = flash_unit_write(cs, (cur_addr%spc), iter_size, dp); 
		feed_once_watchdog();/*add by shipeng for watchdog*/
		if(ret) {
			printk("Warning: failed to write flash at 0x%08x with error code=%d\n", cur_addr, ret);
			spin_unlock_irqrestore(&spi_lock, flags);
			return ret;
		} else {
			cur_addr += iter_size;
			remaining_size -= iter_size;
			dp += iter_size;
		}
	}
	spin_unlock_irqrestore(&spi_lock, flags);
	if (ret)
		return ret;
	if (retlen)
		(*retlen) = len;
	return 0;
}

void mtd_spi_sync(struct mtd_info *mtd)
{
	/* Operation not supported on transport endpoint */
} 
int mtd_spi_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	/* Operation not supported on transport endpoint */
	return -EOPNOTSUPP;
}

int mtd_spi_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	/* Operation not supported on transport endpoint */
	return -EOPNOTSUPP;
}

int mtd_spi_suspend(struct mtd_info *mtd)
{
	/* Operation not supported on transport endpoint */
	return -EOPNOTSUPP;
}

void mtd_spi_resume(struct mtd_info *mtd)
{
	/* Operation not supported on transport endpoint */
} 

EXPORT_SYMBOL(mtd_spi_erase);
EXPORT_SYMBOL(mtd_spi_read);
EXPORT_SYMBOL(mtd_spi_write);
EXPORT_SYMBOL(mtd_spi_sync);
EXPORT_SYMBOL(mtd_spi_lock);
EXPORT_SYMBOL(mtd_spi_unlock);
EXPORT_SYMBOL(mtd_spi_suspend);
EXPORT_SYMBOL(mtd_spi_resume);

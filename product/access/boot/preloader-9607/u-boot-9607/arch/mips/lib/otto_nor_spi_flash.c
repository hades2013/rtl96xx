#include <common.h>
#include <config.h>
#include <soc.h>
#include <pblr.h>
#include <spi_flash.h>

#define FLASHI parameters.soc.flash_info

int otto_spi_flash_read(struct spi_flash *flash, u32 offset,
                        size_t len, void *buf);
int otto_spi_flash_write(struct spi_flash *flash, u32 offset,
                         size_t len, const void *buf);
int otto_spi_flash_erase(struct spi_flash *flash, u32 offset,
                         size_t len);

static struct spi_slave otto_slave;
static struct spi_flash otto_sf;

static uint32_t enable_message = 1;

#ifdef CONFIG_CMD_JFFS2
#include <flash.h>
flash_info_t flash_info[2];		//FIXME if more than 2 flash supported
#endif

#if (CONFIG_STANDALONE_UBOOT == 1)
extern void flash_init(void);
#endif

void spi_disable_message(void) {
	enable_message = 0;
	return;
}

void spi_enable_message(void) {
	enable_message = 1;
	return;
}

void spi_flash_init(void) {
#if (CONFIG_STANDALONE_UBOOT == 1)
	/* When there is no Preloader, flash driver is bundled with
	   U-Boot flow, as a result, flash initialization should start
	   after U-Boot resets bss to zero. */
	flash_init();
#endif
	printf("%dx%d MB\n",
	       FLASHI.num_chips, (1<<FLASHI.size_per_chip)>>20);

	otto_sf.spi = &otto_slave;
	otto_sf.name = NULL;
	otto_sf.size = 1 << FLASHI.size_per_chip;
	otto_sf.page_size = (FLASHI.wr_boundary==0) ? (1<<FLASHI.size_per_chip) :
	                                              (1<<FLASHI.wr_boundary);
	otto_sf.sector_size = (1 << FLASHI.erase_unit);
	otto_sf.read  = otto_spi_flash_read;
	otto_sf.write = otto_spi_flash_write;
	otto_sf.erase = otto_spi_flash_erase;

#ifdef CONFIG_CMD_JFFS2
	unsigned int i, j, offset;
	unsigned int unit_size = (1 << FLASHI.erase_unit);
	unsigned int num_chips = FLASHI.num_chips;
    printf("[debug] start to init flash_info (%d)\n", num_chips);
	for (i=0; i<num_chips; i++) {
        flash_info[i].size = otto_sf.size;
        flash_info[i].sector_count = (flash_info[i].size / unit_size);
        flash_info[i].flash_id = FLASHI.id;
        for(j=0, offset=0;offset<(otto_sf.size);j++, offset+=unit_size){
            flash_info[i].start[j] = CONFIG_SYS_FLASH_BASE+offset;
        }
        printf("[debug] <%d> unit size=0x%x, unit count=%d\n", i, unit_size, flash_info[i].sector_count);
    }    
#endif
	return;
}

struct spi_flash *
spi_flash_probe(unsigned int bus, unsigned int cs,
                unsigned int max_hz __attribute__((unused)),
                unsigned int spi_mode __attribute__((unused))) {
	if (cs >= FLASHI.num_chips) {
		return NULL;
	}

	otto_slave.bus = bus;
	otto_slave.cs = cs;

	return &otto_sf;
}

void spi_flash_free(struct spi_flash *flash __attribute__((unused))) {
	return;
};

int otto_spi_flash_read(struct spi_flash *flash, u32 offset,
                        size_t len, void *buf) {
	uint32_t cs_backup = flash->spi->cs;
	uint32_t temp_len = 0;

	flash->spi->cs = offset / flash->size;
	offset = offset % flash->size;

	while (len) {
		if ((offset + len) > flash->size) {
			temp_len = flash->size - offset;
			parameters._nor_spi_read(flash->spi->cs, offset, temp_len, buf);
			flash->spi->cs++;
			offset = 0;
			buf += temp_len;
			len -= temp_len;
		} else {
			parameters._nor_spi_read(flash->spi->cs, offset, len, buf);
			break;
		}
	}

	flash->spi->cs = cs_backup;

	return 0;
}

static inline uint8_t
is_addr_in_range(const uint32_t range_start, const uint32_t range_end,
                 const uint32_t caddr_start, const uint32_t caddr_end) {
	if (((caddr_end > range_start) && (caddr_end <= range_end)) ||
	    ((range_end > caddr_start) && (range_end <= caddr_end))) {
		return 1;
	} else {
		return 0;
	}
}

int otto_spi_flash_write(struct spi_flash *flash, u32 offset,
                         size_t len, const void *buf) {
	const uint32_t wr_unit = flash->page_size;
	uint32_t end_of_dst, next_dst;
	void *next_src = (void *)buf;
	uint32_t temp_len, orig_len = len;
	uint32_t cs_backup = flash->spi->cs;

	const uint32_t cached_buf = ((uint32_t)buf) & 0xDFFFFFFF;
	if ((is_addr_in_range(SPI_NOR_FLASH_MMIO_BASE0, (SPI_NOR_FLASH_MMIO_BASE0 + SPI_NOR_FLASH_MMIO_SIZE0),
	                      cached_buf, (cached_buf + len)) == 1) ||
	    (is_addr_in_range(SPI_NOR_FLASH_MMIO_BASE1, (SPI_NOR_FLASH_MMIO_BASE1 + SPI_NOR_FLASH_MMIO_SIZE1),
	                      cached_buf, (cached_buf + len)) == 1)) {
		printf("EE: source address(0x%p) on flash is not allowed.\n", buf);
		return 1;
	}

	flash->spi->cs = offset / flash->size;
	offset = offset % flash->size;
	end_of_dst = offset + len;
	next_dst = offset;

	if (enable_message) {
		printf("II: Writting %d bytes to %08x... ",
		       (end_of_dst - next_dst), next_dst);
	}

	/* Deal with the situation that offset is NOT page-aligned. */
	if (((next_dst % wr_unit) != 0) &&
	    (len > (wr_unit - (next_dst % wr_unit)))) {

		temp_len = (wr_unit - (next_dst % wr_unit));

		if (enable_message) {
			printf("%3d%%\b\b\b\b", (100 - len*100/orig_len));
		}
		parameters._nor_spi_write(flash->spi->cs, next_dst, temp_len, next_src);

		next_dst += temp_len;
		next_src += temp_len;
		len -= temp_len;

		if (next_dst == flash->size) {
			next_dst = 0;
			flash->spi->cs++;
		}
	}

	while (len > 0) {
		if (len > wr_unit) {
			temp_len = wr_unit;
		} else {
			temp_len = len;
		}

		if (enable_message) {
			printf("%3d%%\b\b\b\b", (100 - len*100/orig_len));
		}
		parameters._nor_spi_write(flash->spi->cs, next_dst, temp_len, next_src);

		next_dst += temp_len;
		next_src += temp_len;
		len -= temp_len;

		if (next_dst == flash->size) {
			next_dst = 0;
			flash->spi->cs++;
		}
	}

	if (enable_message) {
		puts("100%\n");
	}

	flash->spi->cs = cs_backup;

	return 0;
}

int otto_spi_flash_erase(struct spi_flash *flash, u32 offset,
                         size_t len) {
	const uint32_t erase_unit = flash->sector_size;
	uint32_t end_of_erase, next, iter, i;
	uint32_t cs_backup = flash->spi->cs;

	flash->spi->cs = offset / flash->size;
	offset = offset % flash->size;
	end_of_erase = ((offset + len + erase_unit - 1) &
	                (~(erase_unit - 1)));
	next = offset & (~(erase_unit - 1));
	iter = (end_of_erase - next) / erase_unit;

	if (enable_message) {
		printf("II: Erasing %d bytes from %08x... ",
		       (end_of_erase - next), next);
	}
	for (i=0; i<iter; i++) {
		if (enable_message) {
			printf("%3d%%\b\b\b\b", i*100/iter);
		}
		parameters._nor_spi_erase(flash->spi->cs, next);
		next += erase_unit;
		if (next == flash->size) {
			next = 0;
			flash->spi->cs++;
		}
	}

	if (enable_message) {
		puts("100%\n");
	}

	flash->spi->cs = cs_backup;

	return 0;
}

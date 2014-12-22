#ifndef C600_PBLR_H
#define C600_PBLR_H

extern parameter_to_bootloader_t  parameters;

#define parameter_soc_rwp   ((soc_t*)(&(parameters.soc)))
#define para_flash_info     (parameters.soc.flash_info)
#define para_ddr_info       (parameters.soc.dram_info)
#define para_pll_info       (parameters.soc.pll_info)
#define pblr_printf         (parameters._pblr_printf)
#define pblr_nor_spi_erase  (parameters._nor_spi_erase)
#define pblr_nor_spi_read   (parameters._nor_spi_read)
#define pblr_nor_spi_write  (parameters._nor_spi_write)
#define pblr_dc_flushall    (parameters._dcache_writeback_invalidate_all)

inline static void
pblr_bset(void *buf,u8_t value, u32_t nbyte) {
	u8_t *b=(u8_t *)buf;
	u8_t *e=b+nbyte;
    while (b!=e) *(b++)=value;
}

inline static void
pblr_bzero(void *buf, u32_t nbyte) {
    pblr_bset(buf,0,nbyte);
}
#endif

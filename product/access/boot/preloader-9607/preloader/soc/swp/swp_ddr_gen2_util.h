#ifndef SWP_DDR_GEN2_UTIL_H
#define SWP_DDR_GEN2_UTIL_H

int get_ddr_gen2_info(const dram_gen2_info_t *info, const char *pname, char *buf);
void dump_ddr_gen2_info(const dram_gen2_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask);
int set_ddr_gen2_para(dram_gen2_info_t *info, const char *pname, const char *value);
void cli_dg2_memctl_set_ODT_OCD(dram_gen2_info_t *info, unsigned int odt, unsigned int ocd);
void board_dram_write_pattern_generation(const dram_gen2_info_t *info,
    unsigned int test_times, unsigned int string_pattern);
void board_dram_read_pattern_generation(const dram_gen2_info_t *info,unsigned int test_times);

#endif //SWP_DDR_GEN2_UTIL_H


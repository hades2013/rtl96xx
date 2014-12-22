#ifndef SWP_CLI_UTIL_H
#define SWP_CLI_UTIL_H
#include <soc.h>

// command parsers result
typedef enum {
    CPR_NEXT,
    CPR_EXIT,
    CPR_INCOMPLETE_CMD,
    CPR_UNSUPPORT_PARAMETER,
    CPR_UNKNOWN_CMD    
} cpr_t;



typedef cpr_t(line_parser_t)(char *cmd_buf);
extern cpr_t cli_main_loop(line_parser_t *line_parser);

extern char *get_token(char **cmd_buf);
extern void test_udelay(u32_t unit, u32_t min);
extern void test_get_timer(u32_t min);


// command parsers
#define CMD_INP_BUF_SZ  128
#define MAX_ARGV        7
typedef cpr_t (cli_cmd_t)(int argc, char *argv[]);
typedef struct {
    const char *cmd_name;
    cli_cmd_t *cli_cmd;
} cli_cmd_entry_t;
extern cpr_t cli_line_parser(const cli_cmd_entry_t *list, char *cmd_buf);

// common used commands
extern cpr_t cli_std_exit(int argc, char *argv[]);
extern cpr_t cli_std_store_word(int argc, char *argv[]);
extern cpr_t cli_std_load_word(int argc, char *argv[]);
extern cpr_t cli_std_timer(int argc, char *argv[]);
extern cpr_t cli_std_mdump(int argc, char *argv[]);
extern cpr_t cli_std_save_para(int argc, char *argv[]);
extern cpr_t cli_update_image_by_xmodem(int argc, char *argv[]);
extern cpr_t cli_std_mt(int argc, char *argv[]);


/***************************
  * Macros for DRAM memory test
  ***************************/ 
#define CLI_MT_CASE_ADR_ROT_TEST (1<<0)
#define CLI_MT_CASE_ULS_TEST     (1<<1)
#define CLI_MT_CASE_MDRAM_TEST   (1<<2)
#define CLI_MT_PASS              (0) 
#define CLI_MT_FAIL              (-1)
#define HANDLE_FAIL \
({ \
	printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
	return CLI_MT_FAIL;\
})


/***********************************
  * Extern Functions for DRAM memory test
  ***********************************/ 
extern int dram_adr_rotate_test(u32_t startAddr, u32_t size);
extern int unaligned_test(u32_t startAddr, u32_t area_size);
extern int mdram_test(u32_t start_addr, u32_t size);

#endif //SWP_CLI_UTIL_H



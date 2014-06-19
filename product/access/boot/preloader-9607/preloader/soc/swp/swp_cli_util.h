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
#define MAX_ARGV 7
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
extern cpr_t cli_std_mt(int argc, char *argv[]);

#endif //SWP_CLI_UTIL_H



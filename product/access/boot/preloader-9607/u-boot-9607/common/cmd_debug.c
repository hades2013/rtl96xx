#include <common.h>
#include <command.h>

#ifdef CONFIG_CMD_DEBUG

int debug_flag = 0;

int do_debug ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if (2 != argc)
    {
        printf("Usage: debug on/off.\n");
        return 0;
    }
    
    if (0 == strcmp("on",argv[1]))
    {
        debug_flag = 1;
    }
    else if (0 == strcmp("off",argv[1]))
    {
        debug_flag = 0;
    }
    else
    {
        printf("Usage: debug on/off.\n");
    }
    
	return 0;
}


/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	debug,	2,	1,	do_debug,
	"switch on/off debug flag.",
	NULL
);

#endif	/* CONFIG_CMD_DEBUG */


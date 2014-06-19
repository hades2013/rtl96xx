
#if 0
#include <linux/init.h>
#include <linux/string.h>

#include <asm/bootinfo.h>

extern int prom_argc;
extern char **prom_argv, **prom_envp;


char * prom_getcmdline(void)
{
	return &(arcs_cmdline[0]);
}

void  prom_init_cmdline(void)
{
	char *cp;
	int actr;

	actr = 1; /* Always ignore argv[0] */

	cp = &(arcs_cmdline[0]);
	prom_printf("prom_argc=%d\n", prom_argc);
	while(actr < prom_argc) {
		strcpy(cp, prom_argv[actr]);
		cp += strlen(prom_argv[actr]);
		*cp++ = ' ';
		actr++;
	}
	if (cp != &(arcs_cmdline[0])) /* get rid of trailing space */
		--cp;
	if (prom_argc > 1)
		*cp = '\0';

}


char *prom_getenv(char *envname)
{
	/*
	 * Return a pointer to the given environment variable.
	 * YAMON uses "name", "value" pairs, while U-Boot uses "name=value".
	 */

	char **env = prom_envp;
	int i = strlen(envname);
	int yamon = (*env && strchr(*env, '=') == NULL);

	while (*env) {
		if (yamon) {
			if (strcmp(envname, *env++) == 0)
				return *env;
		} else {
			if (strncmp(envname, *env, i) == 0 && (*env)[i] == '=')
				return *env + i + 1;
		}
		env++;
	}
	return NULL;
}

#else

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/bootinfo.h>


extern char arcs_cmdline[CL_SIZE];
static char *ignored[] = {
	"ConsoleIn=",
	"ConsoleOut=",
	"SystemPartition=",
	"OSLoader=",
	"OSLoadPartition=",
	"OSLoadFilename=",
	"OSLoadOptions="
};

static char *ifNotExitsAdded[] = {
	"rdinit=",
};
static char *ifNotExitsAddedValue[] = {
	"/init",
};

#define NENTS(foo) ((sizeof((foo)) / (sizeof((foo[0])))))

static char *used_arc[][2] = {
	{ "OSLoadPartition=", "root=" },
	{ "OSLoadOptions=", "" }
};

extern unsigned long *_prom_argv, *_prom_envp;
extern int prom_argc;
/* A 32-bit ARC PROM pass arguments and environment as 32-bit pointer.
   These macros take care of sign extension.  */
#define prom_argv(index) ((char *) (long) (_prom_argv[(index)] | 0x80000000))
#define prom_argc(index) ((char *) (long) (_prom_argc[(index)] | 0X80000000))


#define prom_envp(index) ((char *)(long)arcs_cmdline[(index)])

/*begin add code for bootargs 2013-05-08*/
extern void prom_printf(char *fmt, ...);
/*begin add code for bootargs 2013-05-08*/

char *prom_getenv(char *envname)
{
	/*
	 * Return a pointer to the given environment variable.
	 * YAMON uses "name", "value" pairs, while U-Boot uses "name=value".
	 */

	char *env = arcs_cmdline;
	int i = strlen(envname);
	int yamon = (env && strchr(env, '=') == NULL);

	while (env) {
		if (yamon) {
			if (strcmp(envname, env++) == 0)
				return env;
		} else {
			if (strncmp(envname, env, i) == 0 && (env)[i] == '=')
				return env + i + 1;
		}
		env++;
	}
	return NULL;
}

static char * __init move_firmware_args(char* cp)
{
	char *s;
	int actr, i;

	actr = 1; /* Always ignore argv[0] */


	while (actr < prom_argc) {

		for(i = 0; i < NENTS(used_arc); i++) {

			int len = strlen(used_arc[i][0]);

			//prom_printf("_prom_argv=0x%x.\n", _prom_argv);
			//prom_printf("actr=0x%x, prom_argv(actr)=0x%x, used_arc[i][0]=0x%x.\n", actr, prom_argv(actr), used_arc[i][0]);

			if (!strncmp(prom_argv(actr), used_arc[i][0], len)) {
				//prom_printf("Ok, we want it\n");
				/* Ok, we want it. First append the replacement... */
				strcat(cp, used_arc[i][1]);
				cp += strlen(used_arc[i][1]);
				/* ... and now the argument */
				//prom_printf("and now the argument\n");
				s = strstr(prom_argv(actr), "=");
				if (s) {
					s++;
					//prom_printf("strcpy %x to %x\n", cp, s);
					strcpy(cp, s);
					cp += strlen(s);
				}
				*cp++ = ' ';
				break;
			}
		}
		actr++;
	}

	return cp;
}

void __init prom_init_cmdline(void)
{
	char *cp;
	int actr, i;

	actr = 1; /* Always ignore argv[0] */

	cp = &(arcs_cmdline[0]);

	/*
	 * Move ARC variables to the beginning to make sure they can be
	 * overridden by later arguments.
	 */
	cp = move_firmware_args(cp);

	while (actr < prom_argc) {
		for (i = 0; i < NENTS(ignored); i++) {
			int len = strlen(ignored[i]);

			if (!strncmp(prom_argv(actr), ignored[i], len))
				goto pic_cont;
		}
		//prom_printf("Ok, we want it\n");
		/* Ok, we want it. */
		strcpy(cp, prom_argv(actr));
		cp += strlen(prom_argv(actr));
		*cp++ = ' ';

pic_cont:
		actr++;
	}
	if (cp != &(arcs_cmdline[0])) /* get rid of trailing space */
		--cp;
	*cp = '\0';

	/*------------------------------------------------*
	 *    apend default value if not found            *
	 *------------------------------------------------*/
	for (i = 0; i < NENTS(ifNotExitsAdded); i++) {
		if (NULL == strstr(arcs_cmdline, ifNotExitsAdded[i]))
		{
			char tmpBuf[20];
			//prom_printf("add tmpBuf %s\n",tmpBuf);
			sprintf(tmpBuf, " %s%s", ifNotExitsAdded[i], ifNotExitsAddedValue[i]);
			strcat(arcs_cmdline, tmpBuf);
		}

	}


	//prom_printf("prom_init_cmdline: %s\n", &(arcs_cmdline[0]));
}




#endif

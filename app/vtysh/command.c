/*
 $Id$
 
 Command interpreter routine for virtual terminal [aka TeletYpe]
 Copyright (C) 1997, 98, 99 Kunihiro Ishiguro

 This file is part of GNU Zebra.
 
 GNU Zebra is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by the Free Software Foundation; either version 2, or (at your
 option) any later version.

 GNU Zebra is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GNU Zebra; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.  */

#include <zebra.h>

#include "memory.h"
#include "log.h"
#include <version.h>
#include "thread.h"
#include "vector.h"
#include "vty.h"
#include "command.h"
#include "workqueue.h"
#include "debug.h"
#include "user.h"
#include "eoc_utils.h"
#include "memshare.h"
#include <ipc_protocol.h>
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>

//#include "cmd_eoc.h"
//#include "eoc_utils.h"


host_init_t ht_init;

/* Command vector which includes some level of command lists. Normally
 each daemon maintains each own cmdvec. */
vector cmdvec = NULL;

char DEBUG_FLAG = 0;

struct desc desc_cr;
char *command_cr = NULL;

/* Host information structure. */
struct host_t host;

/* Standard command node structures. */
/*begin added by liaohongjun 2012/9/5 of QID0015*/
static struct cmd_node enter_node = { ENTER_NODE, "Please press Enter to continue.", };
/*end added by liaohongjun 2012/9/5 of QID0015*/

static struct cmd_node auth_node = { AUTH_NODE, "Username: ", };
static struct cmd_node auth_passwd_node = { AUTH_PASSWD_NODE, "Password: ", };

static struct cmd_node view_node = { VIEW_NODE, "%s> ", };

static struct cmd_node restricted_node = { RESTRICTED_NODE, "%s$ ", };

static struct cmd_node auth_enable_node = { AUTH_ENABLE_NODE, "Password: ", };

static struct cmd_node enable_node = { ENABLE_NODE, "%s# ", };

static struct cmd_node config_node = { CONFIG_NODE, "%s(config)# ", NULL, 1 };

/* Default motd string. */
static const char *default_motd = VTY_MOTD;
/*				"\r\n\
Hello, this is " QUAGGA_PROGNAME " (version " QUAGGA_VERSION ").\r\n\
" QUAGGA_COPYRIGHT "\r\n\
\r\n";
*/

static const struct facility_map {
	int facility;
	const char *name;
	size_t match;
} syslog_facilities[] = { { LOG_KERN, "kern", 1 }, { LOG_USER, "user", 2 }, {
		LOG_MAIL, "mail", 1 }, { LOG_DAEMON, "daemon", 1 }, { LOG_AUTH, "auth",
		1 }, { LOG_SYSLOG, "syslog", 1 }, { LOG_LPR, "lpr", 2 }, { LOG_NEWS,
		"news", 1 }, { LOG_UUCP, "uucp", 2 }, { LOG_CRON, "cron", 1 },
#ifdef LOG_FTP
		{ LOG_FTP, "ftp", 1 },
#endif
		{ LOG_LOCAL0, "local0", 6 }, { LOG_LOCAL1, "local1", 6 }, { LOG_LOCAL2,
				"local2", 6 }, { LOG_LOCAL3, "local3", 6 }, { LOG_LOCAL4,
				"local4", 6 }, { LOG_LOCAL5, "local5", 6 }, { LOG_LOCAL6,
				"local6", 6 }, { LOG_LOCAL7, "local7", 6 }, { 0, NULL, 0 }, };

static const char *
facility_name(int facility) {
	const struct facility_map *fm;

	for (fm = syslog_facilities; fm->name; fm++)
		if (fm->facility == facility)
			return fm->name;
	return "";
}

static int facility_match(const char *str) {
	const struct facility_map *fm;

	for (fm = syslog_facilities; fm->name; fm++)
		if (!strncmp(str, fm->name, fm->match))
			return fm->facility;
	return -1;
}

static int level_match(const char *s) {
	int level;

	for (level = 0; zlog_priority[level] != NULL; level++)
		if (!strncmp(s, zlog_priority[level], 2))
			return level;
	return ZLOG_DISABLED;
}

/* This is called from main when a daemon is invoked with -v or --version. */
void print_version(const char *progname) {
	printf("%s version %s\n", progname, QUAGGA_VERSION);
	printf("%s\n", QUAGGA_COPYRIGHT);
}


/* Utility function to concatenate argv argument into a single string
 with inserting ' ' character between each argument.  */
char *
argv_concat(const char **argv, int argc, int shift) {
	int i;
	size_t len;
	char *str;
	char *p;

	len = 0;
	for (i = shift; i < argc; i++)
		len += strlen(argv[i]) + 1;
	if (!len)
		return NULL;
	p = str = XMALLOC(MTYPE_TMP, len);
	for (i = shift; i < argc; i++) {
		size_t arglen;
		memcpy(p, argv[i], (arglen = strlen(argv[i])));
		p += arglen;
		*p++ = ' ';
	}
	*(p - 1) = '\0';
	return str;
}

/* Install top node of command vector. */
void install_node(struct cmd_node *node, int(*func)(struct vty *)) {
	vector_set_index(cmdvec, node->node, node);
	node->func = func;
	node->cmd_vector = vector_init(VECTOR_MIN_SIZE);
}

/* Compare two command's string.  Used in sort_node (). */
static int cmp_node(const void *p, const void *q) {
	const struct cmd_element *a = *(struct cmd_element * const *) p;
	const struct cmd_element *b = *(struct cmd_element * const *) q;

	return strcmp(a->string, b->string);
}

static int cmp_desc(const void *p, const void *q) {
	const struct desc *a = *(struct desc * const *) p;
	const struct desc *b = *(struct desc * const *) q;

	return strcmp(a->cmd, b->cmd);
}

/* Sort each node's command element according to command string. */
void sort_node() {
	unsigned int i, j;
	struct cmd_node *cnode;
	vector descvec;
	struct cmd_element *cmd_element;

	for (i = 0; i < vector_active (cmdvec); i++)
		if ((cnode = vector_slot (cmdvec, i)) != NULL) {
			vector cmd_vector = cnode->cmd_vector;
			qsort(cmd_vector->index, vector_active (cmd_vector),
					sizeof(void *), cmp_node);

			for (j = 0; j < vector_active (cmd_vector); j++)
				if ((cmd_element = vector_slot (cmd_vector, j)) != NULL
						&& vector_active (cmd_element->strvec)) {
					descvec = vector_slot (cmd_element->strvec,
							vector_active (cmd_element->strvec) - 1);
					qsort(descvec->index, vector_active (descvec),
							sizeof(void *), cmp_desc);
				}
		}
}

/* Breaking up string into each command piece. I assume given
 character is separated by a space character. Return value is a
 vector which includes char ** data element. */
vector cmd_make_strvec(const char *string) {
	const char *cp, *start;
	char *token;
	int strlen;
	vector strvec;

	if (string == NULL)
		return NULL;

	cp = string;

	/* Skip white spaces. */
	while (isspace ((int) *cp) && *cp != '\0')
		cp++;

	/* Return if there is only white spaces */
	if (*cp == '\0')
		return NULL;

	if (*cp == '!' || *cp == '#')
		return NULL;

	/* Prepare return vector. */
	strvec = vector_init(VECTOR_MIN_SIZE);

	/* Copy each command piece and set into vector. */
	while (1) {
		start = cp;
		while (!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n') && *cp
				!= '\0')
			cp++;
		strlen = cp - start;
		token = XMALLOC (MTYPE_STRVEC, strlen + 1);
		memcpy(token, start, strlen);
		*(token + strlen) = '\0';
		vector_set(strvec, token);

		while ((isspace ((int) *cp) || *cp == '\n' || *cp == '\r') && *cp
				!= '\0')
			cp++;

		if (*cp == '\0')
			return strvec;
	}
}

/* Free allocated string vector. */
void cmd_free_strvec(vector v) {
	unsigned int i;
	char *cp;

	if (!v)
		return;

	for (i = 0; i < vector_active (v); i++)
		if ((cp = vector_slot (v, i)) != NULL)
			XFREE (MTYPE_STRVEC, cp);

	vector_free(v);
}

/* Fetch next description.  Used in cmd_make_descvec(). */
static char *
cmd_desc_str(const char **string) {
	const char *cp, *start;
	char *token;
	int strlen;

	cp = *string;

	if (cp == NULL)
		return NULL;

	/* Skip white spaces. */
	while (isspace ((int) *cp) && *cp != '\0')
		cp++;

	/* Return if there is only white spaces */
	if (*cp == '\0')
		return NULL;

	start = cp;

	while (!(*cp == '\r' || *cp == '\n') && *cp != '\0')
		cp++;

	strlen = cp - start;
	token = XMALLOC (MTYPE_STRVEC, strlen + 1);
	memcpy(token, start, strlen);
	*(token + strlen) = '\0';

	*string = cp;

	return token;
}

/* New string vector. */
static vector cmd_make_descvec(const char *string, const char *descstr) {
	int multiple = 0;
	const char *sp;
	char *token;
	int len;
	const char *cp;
	const char *dp;
	vector allvec;
	vector strvec = NULL;
	struct desc *desc;

	cp = string;
	dp = descstr;

	if (cp == NULL)
		return NULL;

	allvec = vector_init(VECTOR_MIN_SIZE);

	while (1) {
		while (isspace ((int) *cp) && *cp != '\0')
			cp++;

		if (*cp == '(') {
			multiple = 1;
			cp++;
		}
		if (*cp == ')') {
			multiple = 0;
			cp++;
		}
		if (*cp == '|') {
			if (!multiple) {
				fprintf(stderr, "Command parse error!: %s\n", string);
				exit(1);
			}
			cp++;
		}

		while (isspace ((int) *cp) && *cp != '\0')
			cp++;

		if (*cp == '(') {
			multiple = 1;
			cp++;
		}

		if (*cp == '\0')
			return allvec;

		sp = cp;

		while (!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n' || *cp
				== ')' || *cp == '|') && *cp != '\0')
			cp++;

		len = cp - sp;

		token = XMALLOC (MTYPE_STRVEC, len + 1);
		memcpy(token, sp, len);
		*(token + len) = '\0';

		desc = XCALLOC (MTYPE_DESC, sizeof (struct desc));
		desc->cmd = token;
		desc->str = cmd_desc_str(&dp);

		if (multiple) {
			if (multiple == 1) {
				strvec = vector_init(VECTOR_MIN_SIZE);
				vector_set(allvec, strvec);
			}
			multiple++;
		} else {
			strvec = vector_init(VECTOR_MIN_SIZE);
			vector_set(allvec, strvec);
		}
		vector_set(strvec, desc);
	}
}

/* Count mandantory string vector size.  This is to determine inputed
 command has enough command length. */
static int cmd_cmdsize(vector strvec) {
	unsigned int i;
	int size = 0;
	vector descvec;
	struct desc *desc;

	for (i = 0; i < vector_active (strvec); i++)
		if ((descvec = vector_slot (strvec, i)) != NULL) {
			if ((vector_active (descvec)) == 1 && (desc
					= vector_slot (descvec, 0)) != NULL) {
				if (desc->cmd == NULL || CMD_OPTION (desc->cmd))
					return size;
				else
					size++;
			} else
				size++;
		}
	return size;
}

/* Return prompt character of specified node. */
const char *
cmd_prompt(struct vty *vty) {
	struct cmd_node *cnode;
	enum node_type node = vty->node;

	cnode = vector_slot (cmdvec, node);
	if(cnode->prompt) return cnode->prompt;
	if(cnode->prompt_func)
		return cnode->prompt_func(vty);
	else
		return "%s ";
}

/* Install a command into a node. */
void install_element(enum node_type ntype, struct cmd_element *cmd) {
	struct cmd_node *cnode;

	/* cmd_init hasn't been called */
	if (!cmdvec)
		return;

	cnode = vector_slot (cmdvec, ntype);

	if (cnode == NULL) {
		fprintf(stderr, "Command node %d doesn't exist, please check it\n",
				ntype);
		exit(1);
	}

	vector_set(cnode->cmd_vector, cmd);

	if (cmd->strvec == NULL)
		cmd->strvec = cmd_make_descvec(cmd->string, cmd->doc);

	cmd->cmdsize = cmd_cmdsize(cmd->strvec);
}

static const unsigned char itoa64[] =
		"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void to64(char *s, long v, int n) {
	while (--n >= 0) {
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

static char *
zencrypt(const char *passwd) {
	char salt[6];
	struct timeval tv;
	char *crypt(const char *, const char *);

	gettimeofday(&tv, 0);

	to64(&salt[0], random(), 3);
	to64(&salt[3], tv.tv_usec, 3);
	salt[5] = '\0';

	return crypt(passwd, salt);
}
/* This function write configuration of this host. */
static int config_write_host(struct vty *vty) 
{
	if (host.name)
		vty_out(vty, "hostname %s%s", host.name, VTY_NEWLINE);

	if (host.username) {
		vty_out(vty, "username %s%s", host.username, VTY_NEWLINE);
	}

	if (host.encrypt) {
		if (host.password_encrypt)
			vty_out(vty, "password 8 %s%s", host.password_encrypt, VTY_NEWLINE);
		if (host.enable_encrypt)
			vty_out(vty, "enable password 8 %s%s", host.enable_encrypt,
					VTY_NEWLINE);
	} else {
		if (host.password)
			vty_out(vty, "password %s%s", host.password, VTY_NEWLINE);
		if (host.enable)
			vty_out(vty, "enable password %s%s", host.enable, VTY_NEWLINE);
	}

	if (zlog_default->default_lvl != LOG_DEBUG) {
		vty_out(vty, "! N.B. The 'log trap' command is deprecated.%s",
				VTY_NEWLINE);
		vty_out(vty, "log trap %s%s", zlog_priority[zlog_default->default_lvl],
				VTY_NEWLINE);
	}

	if (host.logfile && (zlog_default->maxlvl[ZLOG_DEST_FILE] != ZLOG_DISABLED)) {
		vty_out(vty, "log file %s", host.logfile);
		if (zlog_default->maxlvl[ZLOG_DEST_FILE] != zlog_default->default_lvl)
			vty_out(vty, " %s",
					zlog_priority[zlog_default->maxlvl[ZLOG_DEST_FILE]]);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	if (zlog_default->maxlvl[ZLOG_DEST_STDOUT] != ZLOG_DISABLED) {
		vty_out(vty, "log stdout");
		if (zlog_default->maxlvl[ZLOG_DEST_STDOUT] != zlog_default->default_lvl)
			vty_out(vty, " %s",
					zlog_priority[zlog_default->maxlvl[ZLOG_DEST_STDOUT]]);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	if (zlog_default->maxlvl[ZLOG_DEST_MONITOR] == ZLOG_DISABLED)
		vty_out(vty, "no log monitor%s", VTY_NEWLINE);
	else if (zlog_default->maxlvl[ZLOG_DEST_MONITOR]
			!= zlog_default->default_lvl)
		vty_out(vty, "log monitor %s%s",
				zlog_priority[zlog_default->maxlvl[ZLOG_DEST_MONITOR]],
				VTY_NEWLINE);

	if (zlog_default->maxlvl[ZLOG_DEST_SYSLOG] != ZLOG_DISABLED) {
		vty_out(vty, "log syslog");
		if (zlog_default->maxlvl[ZLOG_DEST_SYSLOG] != zlog_default->default_lvl)
			vty_out(vty, " %s",
					zlog_priority[zlog_default->maxlvl[ZLOG_DEST_SYSLOG]]);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	if (zlog_default->facility != LOG_DAEMON)
		vty_out(vty, "log facility %s%s",
				facility_name(zlog_default->facility), VTY_NEWLINE);

	if (zlog_default->record_priority == 1)
		vty_out(vty, "log record-priority%s", VTY_NEWLINE);

	if (zlog_default->timestamp_precision > 0)
		vty_out(vty, "log timestamp precision %d%s",
				zlog_default->timestamp_precision, VTY_NEWLINE);

	if (host.advanced)
		vty_out(vty, "service advanced-vty%s", VTY_NEWLINE);

	if (host.encrypt)
		vty_out(vty, "service password-encryption%s", VTY_NEWLINE);

	if (host.lines >= 0)
		vty_out(vty, "service terminal-length %d%s", host.lines, VTY_NEWLINE);

	if (host.motdfile)
		vty_out(vty, "banner motd file %s%s", host.motdfile, VTY_NEWLINE);
	else if (!host.motd)
		vty_out(vty, "no banner motd%s", VTY_NEWLINE);
	return 1;
}

/* Utility function for getting command vector. */
static vector cmd_node_vector(vector v, enum node_type ntype) {
	struct cmd_node *cnode = vector_slot (v, ntype);
	return cnode->cmd_vector;
}

#if 0
/* Filter command vector by symbol.  This function is not actually used;
 * should it be deleted? */
static int
cmd_filter_by_symbol (char *command, char *symbol)
{
	int i, lim;

	if (strcmp (symbol, "IPV4_ADDRESS") == 0)
	{
		i = 0;
		lim = strlen (command);
		while (i < lim)
		{
			if (! (isdigit ((int) command[i]) || command[i] == '.' || command[i] == '/'))
			return 1;
			i++;
		}
		return 0;
	}
	if (strcmp (symbol, "STRING") == 0)
	{
		i = 0;
		lim = strlen (command);
		while (i < lim)
		{
			if (! (isalpha ((int) command[i]) || command[i] == '_' || command[i] == '-'))
			return 1;
			i++;
		}
		return 0;
	}
	if (strcmp (symbol, "IFNAME") == 0)
	{
		i = 0;
		lim = strlen (command);
		while (i < lim)
		{
			if (! isalnum ((int) command[i]))
			return 1;
			i++;
		}
		return 0;
	}
	return 0;
}
#endif

/* Completion match types. */
enum match_type {
	no_match,
	extend_match,
	ipv4_prefix_match,
	ipv4_match,
	ipv6_prefix_match,
	ipv6_match,
	range_match,
	vararg_match,
	partly_match,
	exact_match
};

static enum match_type cmd_ipv4_match(const char *str) {
	const char *sp;
	int dots = 0, nums = 0;
	char buf[4];

	if (str == NULL)
		return partly_match;

	for (;;) {
		memset(buf, 0, sizeof(buf));
		sp = str;
		while (*str != '\0') {
			if (*str == '.') {
				if (dots >= 3)
					return no_match;

				if (*(str + 1) == '.')
					return no_match;

				if (*(str + 1) == '\0')
					return partly_match;

				dots++;
				break;
			}
			if (!isdigit ((int) *str))
				return no_match;

			str++;
		}

		if (str - sp > 3)
			return no_match;

		strncpy(buf, sp, str - sp);
		if (atoi(buf) > 255)
			return no_match;

		nums++;

		if (*str == '\0')
			break;

		str++;
	}

	if (nums < 4)
		return partly_match;

	return exact_match;
}

static enum match_type cmd_ipv4_prefix_match(const char *str) {
	const char *sp;
	int dots = 0;
	char buf[4];

	if (str == NULL)
		return partly_match;

	for (;;) {
		memset(buf, 0, sizeof(buf));
		sp = str;
		while (*str != '\0' && *str != '/') {
			if (*str == '.') {
				if (dots == 3)
					return no_match;

				if (*(str + 1) == '.' || *(str + 1) == '/')
					return no_match;

				if (*(str + 1) == '\0')
					return partly_match;

				dots++;
				break;
			}

			if (!isdigit ((int) *str))
				return no_match;

			str++;
		}

		if (str - sp > 3)
			return no_match;

		strncpy(buf, sp, str - sp);
		if (atoi(buf) > 255)
			return no_match;

		if (dots == 3) {
			if (*str == '/') {
				if (*(str + 1) == '\0')
					return partly_match;

				str++;
				break;
			} else if (*str == '\0')
				return partly_match;
		}

		if (*str == '\0')
			return partly_match;

		str++;
	}

	sp = str;
	while (*str != '\0') {
		if (!isdigit ((int) *str))
			return no_match;

		str++;
	}

	if (atoi(sp) > 32)
		return no_match;

	return exact_match;
}

#define IPV6_ADDR_STR		"0123456789abcdefABCDEF:.%"
#define IPV6_PREFIX_STR		"0123456789abcdefABCDEF:.%/"
#define STATE_START		1
#define STATE_COLON		2
#define STATE_DOUBLE		3
#define STATE_ADDR		4
#define STATE_DOT               5
#define STATE_SLASH		6
#define STATE_MASK		7

#ifdef HAVE_IPV6

static enum match_type
cmd_ipv6_match (const char *str)
{
	int state = STATE_START;
	int colons = 0, nums = 0, double_colon = 0;
	const char *sp = NULL;
	struct sockaddr_in6 sin6_dummy;
	int ret;

	if (str == NULL)
	return partly_match;

	if (strspn (str, IPV6_ADDR_STR) != strlen (str))
	return no_match;

	/* use inet_pton that has a better support,
	 * for example inet_pton can support the automatic addresses:
	 *  ::1.2.3.4
	 */
	ret = inet_pton(AF_INET6, str, &sin6_dummy.sin6_addr);

	if (ret == 1)
	return exact_match;

	while (*str != '\0')
	{
		switch (state)
		{
			case STATE_START:
			if (*str == ':')
			{
				if (*(str + 1) != ':' && *(str + 1) != '\0')
				return no_match;
				colons--;
				state = STATE_COLON;
			}
			else
			{
				sp = str;
				state = STATE_ADDR;
			}

			continue;
			case STATE_COLON:
			colons++;
			if (*(str + 1) == ':')
			state = STATE_DOUBLE;
			else
			{
				sp = str + 1;
				state = STATE_ADDR;
			}
			break;
			case STATE_DOUBLE:
			if (double_colon)
			return no_match;

			if (*(str + 1) == ':')
			return no_match;
			else
			{
				if (*(str + 1) != '\0')
				colons++;
				sp = str + 1;
				state = STATE_ADDR;
			}

			double_colon++;
			nums++;
			break;
			case STATE_ADDR:
			if (*(str + 1) == ':' || *(str + 1) == '\0')
			{
				if (str - sp > 3)
				return no_match;

				nums++;
				state = STATE_COLON;
			}
			if (*(str + 1) == '.')
			state = STATE_DOT;
			break;
			case STATE_DOT:
			state = STATE_ADDR;
			break;
			default:
			break;
		}

		if (nums > 8)
		return no_match;

		if (colons > 7)
		return no_match;

		str++;
	}

#if 0
	if (nums < 11)
	return partly_match;
#endif /* 0 */

	return exact_match;
}

static enum match_type
cmd_ipv6_prefix_match (const char *str)
{
	int state = STATE_START;
	int colons = 0, nums = 0, double_colon = 0;
	int mask;
	const char *sp = NULL;
	char *endptr = NULL;

	if (str == NULL)
	return partly_match;

	if (strspn (str, IPV6_PREFIX_STR) != strlen (str))
	return no_match;

	while (*str != '\0' && state != STATE_MASK)
	{
		switch (state)
		{
			case STATE_START:
			if (*str == ':')
			{
				if (*(str + 1) != ':' && *(str + 1) != '\0')
				return no_match;
				colons--;
				state = STATE_COLON;
			}
			else
			{
				sp = str;
				state = STATE_ADDR;
			}

			continue;
			case STATE_COLON:
			colons++;
			if (*(str + 1) == '/')
			return no_match;
			else if (*(str + 1) == ':')
			state = STATE_DOUBLE;
			else
			{
				sp = str + 1;
				state = STATE_ADDR;
			}
			break;
			case STATE_DOUBLE:
			if (double_colon)
			return no_match;

			if (*(str + 1) == ':')
			return no_match;
			else
			{
				if (*(str + 1) != '\0' && *(str + 1) != '/')
				colons++;
				sp = str + 1;

				if (*(str + 1) == '/')
				state = STATE_SLASH;
				else
				state = STATE_ADDR;
			}

			double_colon++;
			nums += 1;
			break;
			case STATE_ADDR:
			if (*(str + 1) == ':' || *(str + 1) == '.'
					|| *(str + 1) == '\0' || *(str + 1) == '/')
			{
				if (str - sp > 3)
				return no_match;

				for (; sp <= str; sp++)
				if (*sp == '/')
				return no_match;

				nums++;

				if (*(str + 1) == ':')
				state = STATE_COLON;
				else if (*(str + 1) == '.')
				state = STATE_DOT;
				else if (*(str + 1) == '/')
				state = STATE_SLASH;
			}
			break;
			case STATE_DOT:
			state = STATE_ADDR;
			break;
			case STATE_SLASH:
			if (*(str + 1) == '\0')
			return partly_match;

			state = STATE_MASK;
			break;
			default:
			break;
		}

		if (nums > 11)
		return no_match;

		if (colons > 7)
		return no_match;

		str++;
	}

	if (state < STATE_MASK)
	return partly_match;

	mask = strtol (str, &endptr, 10);
	if (*endptr != '\0')
	return no_match;

	if (mask < 0 || mask > 128)
	return no_match;

	/* I don't know why mask < 13 makes command match partly.
	 Forgive me to make this comments. I Want to set static default route
	 because of lack of function to originate default in ospf6d; sorry
	 yasu
	 if (mask < 13)
	 return partly_match;
	 */

	return exact_match;
}

#endif /* HAVE_IPV6  */

#define DECIMAL_STRLEN_MAX 10

static int cmd_range_match(const char *range, const char *str) {
	char *p;
	char buf[DECIMAL_STRLEN_MAX + 1];
	char *endptr = NULL;
	unsigned long min, max, val;

	if (str == NULL)
		return 1;

	val = strtoul(str, &endptr, 10);
	if (*endptr != '\0')
		return 0;

	range++;
	p = strchr(range, '-');
	if (p == NULL)
		return 0;
	if (p - range > DECIMAL_STRLEN_MAX)
		return 0;
	strncpy(buf, range, p - range);
	buf[p - range] = '\0';
	min = strtoul(buf, &endptr, 10);
	if (*endptr != '\0')
		return 0;

	range = p + 1;
	p = strchr(range, '>');
	if (p == NULL)
		return 0;
	if (p - range > DECIMAL_STRLEN_MAX)
		return 0;
	strncpy(buf, range, p - range);
	buf[p - range] = '\0';
	max = strtoul(buf, &endptr, 10);
	if (*endptr != '\0')
		return 0;

	if (val < min || val > max)
		return 0;

	return 1;
}

/* Make completion match and return match type flag. */
static enum match_type cmd_filter_by_completion(char *command, vector v,
		unsigned int index) {
	unsigned int i;
	const char *str;
	struct cmd_element *cmd_element;
	enum match_type match_type;
	vector descvec;
	struct desc *desc;

	match_type = no_match;

	/* If command and cmd_element string does not match set NULL to vector */
	for (i = 0; i < vector_active (v); i++)
		if ((cmd_element = vector_slot (v, i)) != NULL) {
			if (index >= vector_active (cmd_element->strvec))
				vector_slot (v, i) = NULL;
			else {
				unsigned int j;
				int matched = 0;

				descvec = vector_slot (cmd_element->strvec, index);

				for (j = 0; j < vector_active (descvec); j++)
					if ((desc = vector_slot (descvec, j))) {
						str = desc->cmd;

						if (CMD_VARARG (str)) {
							if (match_type < vararg_match)
								match_type = vararg_match;
							matched++;
						} else if (CMD_RANGE (str)) {
							if (cmd_range_match(str, command)) {
								if (match_type < range_match)
									match_type = range_match;

								matched++;
							}
						}
#ifdef HAVE_IPV6
						else if (CMD_IPV6 (str))
						{
							if (cmd_ipv6_match (command))
							{
								if (match_type < ipv6_match)
								match_type = ipv6_match;

								matched++;
							}
						}
						else if (CMD_IPV6_PREFIX (str))
						{
							if (cmd_ipv6_prefix_match (command))
							{
								if (match_type < ipv6_prefix_match)
								match_type = ipv6_prefix_match;

								matched++;
							}
						}
#endif /* HAVE_IPV6  */
						else if (CMD_IPV4 (str)) {
							if (cmd_ipv4_match(command)) {
								if (match_type < ipv4_match)
									match_type = ipv4_match;

								matched++;
							}
						} else if (CMD_IPV4_PREFIX (str)) {
							if (cmd_ipv4_prefix_match(command)) {
								if (match_type < ipv4_prefix_match)
									match_type = ipv4_prefix_match;
								matched++;
							}
						} else
						/* Check is this point's argument optional ? */
						if (CMD_OPTION (str) || CMD_VARIABLE (str)) {
							if (match_type < extend_match)
								match_type = extend_match;
							matched++;
						} else if (strncmp(command, str, strlen(command)) == 0) {
							if (strcmp(command, str) == 0)
								match_type = exact_match;
							else {
								if (match_type < partly_match)
									match_type = partly_match;
							}
							matched++;
						}
					}
				if (!matched)
					vector_slot (v, i) = NULL;
			}
		}
	return match_type;
}

/* Filter vector by command character with index. */
static enum match_type cmd_filter_by_string(char *command, vector v,
		unsigned int index) {
	unsigned int i;
	const char *str;
	struct cmd_element *cmd_element;
	enum match_type match_type;
	vector descvec;
	struct desc *desc;

	match_type = no_match;

	/* If command and cmd_element string does not match set NULL to vector */
	for (i = 0; i < vector_active (v); i++)
		if ((cmd_element = vector_slot (v, i)) != NULL) {
			/* If given index is bigger than max string vector of command,
			 set NULL */
			if (index >= vector_active (cmd_element->strvec))
				vector_slot (v, i) = NULL;
			else {
				unsigned int j;
				int matched = 0;

				descvec = vector_slot (cmd_element->strvec, index);

				for (j = 0; j < vector_active (descvec); j++)
					if ((desc = vector_slot (descvec, j))) {
						str = desc->cmd;

						if (CMD_VARARG (str)) {
							if (match_type < vararg_match)
								match_type = vararg_match;
							matched++;
						} else if (CMD_RANGE (str)) {
							if (cmd_range_match(str, command)) {
								if (match_type < range_match)
									match_type = range_match;
								matched++;
							}
						}
#ifdef HAVE_IPV6
						else if (CMD_IPV6 (str))
						{
							if (cmd_ipv6_match (command) == exact_match)
							{
								if (match_type < ipv6_match)
								match_type = ipv6_match;
								matched++;
							}
						}
						else if (CMD_IPV6_PREFIX (str))
						{
							if (cmd_ipv6_prefix_match (command) == exact_match)
							{
								if (match_type < ipv6_prefix_match)
								match_type = ipv6_prefix_match;
								matched++;
							}
						}
#endif /* HAVE_IPV6  */
						else if (CMD_IPV4 (str)) {
							if (cmd_ipv4_match(command) == exact_match) {
								if (match_type < ipv4_match)
									match_type = ipv4_match;
								matched++;
							}
						} else if (CMD_IPV4_PREFIX (str)) {
							if (cmd_ipv4_prefix_match(command) == exact_match) {
								if (match_type < ipv4_prefix_match)
									match_type = ipv4_prefix_match;
								matched++;
							}
						} else if (CMD_OPTION (str) || CMD_VARIABLE (str)) {
							if (match_type < extend_match)
								match_type = extend_match;
							matched++;
						} else {
							if (strcmp(command, str) == 0) {
								match_type = exact_match;
								matched++;
							}
						}
					}
				if (!matched)
					vector_slot (v, i) = NULL;
			}
		}
	return match_type;
}

/* Check ambiguous match */
static int is_cmd_ambiguous(char *command, vector v, int index,
		enum match_type type) {
	unsigned int i;
	unsigned int j;
	const char *str = NULL;
	struct cmd_element *cmd_element;
	const char *matched = NULL;
	vector descvec;
	struct desc *desc;

	for (i = 0; i < vector_active (v); i++)
		if ((cmd_element = vector_slot (v, i)) != NULL) {
			int match = 0;

			descvec = vector_slot (cmd_element->strvec, index);

			for (j = 0; j < vector_active (descvec); j++)
				if ((desc = vector_slot (descvec, j))) {
					enum match_type ret;

					str = desc->cmd;

					switch (type) {
					case exact_match:
						if (!(CMD_OPTION (str) || CMD_VARIABLE (str))
								&& strcmp(command, str) == 0)
							match++;
						break;
					case partly_match:
						if (!(CMD_OPTION (str) || CMD_VARIABLE (str))
								&& strncmp(command, str, strlen(command)) == 0) {
							if (matched && strcmp(matched, str) != 0)
								return 1; /* There is ambiguous match. */
							else
								matched = str;
							match++;
						}
						break;
					case range_match:
						if (cmd_range_match(str, command)) {
							if (matched && strcmp(matched, str) != 0)
								return 1;
							else
								matched = str;
							match++;
						}
						break;
#ifdef HAVE_IPV6
						case ipv6_match:
						if (CMD_IPV6 (str))
						match++;
						break;
						case ipv6_prefix_match:
						if ((ret = cmd_ipv6_prefix_match (command)) != no_match)
						{
							if (ret == partly_match)
							return 2; /* There is incomplete match. */

							match++;
						}
						break;
#endif /* HAVE_IPV6 */
					case ipv4_match:
						if (CMD_IPV4 (str))
							match++;
						break;
					case ipv4_prefix_match:
						if ((ret = cmd_ipv4_prefix_match(command)) != no_match) {
							if (ret == partly_match)
								return 2; /* There is incomplete match. */

							match++;
						}
						break;
					case extend_match:
						if (CMD_OPTION (str) || CMD_VARIABLE (str))
							match++;
						break;
					case no_match:
					default:
						break;
					}
				}
			if (!match)
				vector_slot (v, i) = NULL;
		}
	return 0;
}

/* If src matches dst return dst string, otherwise return NULL */
static const char *
cmd_entry_function(const char *src, const char *dst) {
	/* Skip variable arguments. */
	if (CMD_OPTION (dst) || CMD_VARIABLE (dst) || CMD_VARARG (dst)
			|| CMD_IPV4 (dst) || CMD_IPV4_PREFIX (dst) || CMD_RANGE (dst))
		return NULL;

	/* In case of 'command \t', given src is NULL string. */
	if (src == NULL)
		return dst;

	/* Matched with input string. */
	if (strncmp(src, dst, strlen(src)) == 0)
		return dst;

	return NULL;
}

/* If src matches dst return dst string, otherwise return NULL */
/* This version will return the dst string always if it is
 CMD_VARIABLE for '?' key processing */
static const char *
cmd_entry_function_desc(const char *src, const char *dst) {
	if (CMD_VARARG (dst))
		return dst;

	if (CMD_RANGE (dst)) {
		if (cmd_range_match(dst, src))
			return dst;
		else
			return NULL;
	}

#ifdef HAVE_IPV6
	if (CMD_IPV6 (dst))
	{
		if (cmd_ipv6_match (src))
		return dst;
		else
		return NULL;
	}

	if (CMD_IPV6_PREFIX (dst))
	{
		if (cmd_ipv6_prefix_match (src))
		return dst;
		else
		return NULL;
	}
#endif /* HAVE_IPV6 */

	if (CMD_IPV4 (dst)) {
		if (cmd_ipv4_match(src))
			return dst;
		else
			return NULL;
	}

	if (CMD_IPV4_PREFIX (dst)) {
		if (cmd_ipv4_prefix_match(src))
			return dst;
		else
			return NULL;
	}

	/* Optional or variable commands always match on '?' */
	if (CMD_OPTION (dst) || CMD_VARIABLE (dst))
		return dst;

	/* In case of 'command \t', given src is NULL string. */
	if (src == NULL)
		return dst;

	if (strncmp(src, dst, strlen(src)) == 0)
		return dst;
	else
		return NULL;
}

/* Check same string element existence.  If it isn't there return
 1. */
static int cmd_unique_string(vector v, const char *str) {
	unsigned int i;
	char *match;

	for (i = 0; i < vector_active (v); i++)
		if ((match = vector_slot (v, i)) != NULL)
			if (strcmp(match, str) == 0)
				return 0;
	return 1;
}

/* Compare string to description vector.  If there is same string
 return 1 else return 0. */
static int desc_unique_string(vector v, const char *str) {
	unsigned int i;
	struct desc *desc;

	for (i = 0; i < vector_active (v); i++)
		if ((desc = vector_slot (v, i)) != NULL)
			if (strcmp(desc->cmd, str) == 0)
				return 1;
	return 0;
}

static int cmd_try_do_shortcut(enum node_type node, char* first_word) {
	if (first_word != NULL && node != AUTH_NODE && node != AUTH_PASSWD_NODE && node != VIEW_NODE && node
			!= AUTH_ENABLE_NODE && node != ENABLE_NODE && node
			!= RESTRICTED_NODE && 0 == strcmp("do", first_word))
		return 1;
	return 0;
}

/* '?' describe command support. */
static vector cmd_describe_command_real(vector vline, struct vty *vty,
		int *status) {
	unsigned int i;
	vector cmd_vector;
#define INIT_MATCHVEC_SIZE 10
	vector matchvec;
	struct cmd_element *cmd_element;
	unsigned int index;
	int ret;
	enum match_type match;
	char *command;

	/* Set index. */
	if (vector_active (vline) == 0) {
		*status = CMD_ERR_NO_MATCH;
		return NULL;
	} else
		index = vector_active (vline) - 1;

	/* Make copy vector of current node's command vector. */
	cmd_vector = vector_copy(cmd_node_vector(cmdvec, vty->node));

	/* Prepare match vector */
	matchvec = vector_init(INIT_MATCHVEC_SIZE);

	/* Filter commands. */
	/* Only words precedes current word will be checked in this loop. */
	for (i = 0; i < index; i++)
		if ((command = vector_slot (vline, i))) {
			match = cmd_filter_by_completion(command, cmd_vector, i);

			if (match == vararg_match) {
				struct cmd_element *cmd_element;
				vector descvec;
				unsigned int j, k;

				for (j = 0; j < vector_active (cmd_vector); j++)
					if ((cmd_element = vector_slot (cmd_vector, j)) != NULL
							&& (vector_active (cmd_element->strvec))) {
						descvec = vector_slot (cmd_element->strvec,
								vector_active (cmd_element->strvec) - 1);
						for (k = 0; k < vector_active (descvec); k++) {
							struct desc *desc = vector_slot (descvec, k);
							vector_set(matchvec, desc);
						}
					}

				vector_set(matchvec, &desc_cr);
				vector_free(cmd_vector);

				return matchvec;
			}

			if ((ret = is_cmd_ambiguous(command, cmd_vector, i, match)) == 1) {
				vector_free(cmd_vector);
				vector_free(matchvec);
				*status = CMD_ERR_AMBIGUOUS;
				return NULL;
			} else if (ret == 2) {
				vector_free(cmd_vector);
				vector_free(matchvec);
				*status = CMD_ERR_NO_MATCH;
				return NULL;
			}
		}

	/* Prepare match vector */
	/*  matchvec = vector_init (INIT_MATCHVEC_SIZE); */

	/* Make sure that cmd_vector is filtered based on current word */
	command = vector_slot (vline, index);
	if (command)
		match = cmd_filter_by_completion(command, cmd_vector, index);

	/* Make description vector. */
	for (i = 0; i < vector_active (cmd_vector); i++)
		if ((cmd_element = vector_slot (cmd_vector, i)) != NULL) {
			vector strvec = cmd_element->strvec;

			/* if command is NULL, index may be equal to vector_active */
			if (command && index >= vector_active (strvec))
				vector_slot (cmd_vector, i) = NULL;
			else {
				/* Check if command is completed. */
				if (command == NULL && index == vector_active (strvec)) {
					if (!desc_unique_string(matchvec, command_cr))
						vector_set(matchvec, &desc_cr);
				} else {
					unsigned int j;
					vector descvec = vector_slot (strvec, index);
					struct desc *desc;

					for (j = 0; j < vector_active (descvec); j++)
						if ((desc = vector_slot (descvec, j))) {
							const char *string;

							string
									= cmd_entry_function_desc(command,
											desc->cmd);
							if (string) {
								/* Uniqueness check */
								if (!desc_unique_string(matchvec, string))
									vector_set(matchvec, desc);
							}
						}
				}
			}
		}
	vector_free(cmd_vector);

	if (vector_slot (matchvec, 0) == NULL) {
		vector_free(matchvec);
		*status = CMD_ERR_NO_MATCH;
		return NULL;
	}

	*status = CMD_SUCCESS;
	return matchvec;
}

vector cmd_describe_command(vector vline, struct vty *vty, int *status) {
	vector ret;

	if (cmd_try_do_shortcut(vty->node, vector_slot(vline, 0))) {
		enum node_type onode;
		vector shifted_vline;
		unsigned int index;

		onode = vty->node;
		vty->node = ENABLE_NODE;
		/* We can try it on enable node, cos' the vty is authenticated */

		shifted_vline = vector_init(vector_count(vline));
		/* use memcpy? */
		for (index = 1; index < vector_active (vline); index++) {
			vector_set_index(shifted_vline, index - 1, vector_lookup(vline,
					index));
		}

		ret = cmd_describe_command_real(shifted_vline, vty, status);

		vector_free(shifted_vline);
		vty->node = onode;
		return ret;
	}

	return cmd_describe_command_real(vline, vty, status);
}

/* Check LCD of matched command. */
static int cmd_lcd(char **matched) {
	int i;
	int j;
	int lcd = -1;
	char *s1, *s2;
	char c1, c2;

	if (matched[0] == NULL || matched[1] == NULL)
		return 0;

	for (i = 1; matched[i] != NULL; i++) {
		s1 = matched[i - 1];
		s2 = matched[i];

		for (j = 0; (c1 = s1[j]) && (c2 = s2[j]); j++)
			if (c1 != c2)
				break;

		if (lcd < 0)
			lcd = j;
		else {
			if (lcd > j)
				lcd = j;
		}
	}
	return lcd;
}

/* Command line completion support. */
static char **
cmd_complete_command_real(vector vline, struct vty *vty, int *status) {
	unsigned int i;
	vector cmd_vector = vector_copy(cmd_node_vector(cmdvec, vty->node));
#define INIT_MATCHVEC_SIZE 10
	vector matchvec;
	struct cmd_element *cmd_element;
	unsigned int index;
	char **match_str;
	struct desc *desc;
	vector descvec;
	char *command;
	int lcd;

	if (vector_active (vline) == 0) {
		vector_free(cmd_vector);
		*status = CMD_ERR_NO_MATCH;
		return NULL;
	} else
		index = vector_active (vline) - 1;

	/* First, filter by preceeding command string */
	for (i = 0; i < index; i++)
		if ((command = vector_slot (vline, i))) {
			enum match_type match;
			int ret;

			/* First try completion match, if there is exactly match return 1 */
			match = cmd_filter_by_completion(command, cmd_vector, i);

			/* If there is exact match then filter ambiguous match else check
			 ambiguousness. */
			if ((ret = is_cmd_ambiguous(command, cmd_vector, i, match)) == 1) {
				vector_free(cmd_vector);
				*status = CMD_ERR_AMBIGUOUS;
				return NULL;
			}
			/*
			 else if (ret == 2)
			 {
			 vector_free (cmd_vector);
			 *status = CMD_ERR_NO_MATCH;
			 return NULL;
			 }
			 */
		}

	/* Prepare match vector. */
	matchvec = vector_init(INIT_MATCHVEC_SIZE);

	/* Now we got into completion */
	for (i = 0; i < vector_active (cmd_vector); i++)
		if ((cmd_element = vector_slot (cmd_vector, i))) {
			const char *string;
			vector strvec = cmd_element->strvec;

			/* Check field length */
			if (index >= vector_active (strvec))
				vector_slot (cmd_vector, i) = NULL;
			else {
				unsigned int j;

				descvec = vector_slot (strvec, index);
				for (j = 0; j < vector_active (descvec); j++)
					if ((desc = vector_slot (descvec, j))) {
						if ((string = cmd_entry_function(
								vector_slot (vline, index), desc->cmd)))
							if (cmd_unique_string(matchvec, string))
								vector_set(matchvec,
										XSTRDUP (MTYPE_TMP, string));
					}
			}
		}

	/* We don't need cmd_vector any more. */
	vector_free(cmd_vector);

	/* No matched command */
	if (vector_slot (matchvec, 0) == NULL) {
		vector_free(matchvec);

		/* In case of 'command \t' pattern.  Do you need '?' command at
		 the end of the line. */
		if (vector_slot (vline, index) == '\0')
			*status = CMD_ERR_NOTHING_TODO;
		else
			*status = CMD_ERR_NO_MATCH;
		return NULL;
	}

	/* Only one matched */
	if (vector_slot (matchvec, 1) == NULL) {
		match_str = (char **) matchvec->index;
		vector_only_wrapper_free(matchvec);
		*status = CMD_COMPLETE_FULL_MATCH;
		return match_str;
	}
	/* Make it sure last element is NULL. */
	vector_set(matchvec, NULL);

	/* Check LCD of matched strings. */
	if (vector_slot (vline, index) != NULL) {
		lcd = cmd_lcd((char **) matchvec->index);

		if (lcd) {
			int len = strlen(vector_slot (vline, index));

			if (len < lcd) {
				char *lcdstr;

				lcdstr = XMALLOC (MTYPE_STRVEC, lcd + 1);
				memcpy(lcdstr, matchvec->index[0], lcd);
				lcdstr[lcd] = '\0';

				/* match_str = (char **) &lcdstr; */

				/* Free matchvec. */
				for (i = 0; i < vector_active (matchvec); i++) {
					if (vector_slot (matchvec, i))
						XFREE (MTYPE_STRVEC, vector_slot (matchvec, i));
				}
				vector_free(matchvec);

				/* Make new matchvec. */
				matchvec = vector_init(INIT_MATCHVEC_SIZE);
				vector_set(matchvec, lcdstr);
				match_str = (char **) matchvec->index;
				vector_only_wrapper_free(matchvec);

				*status = CMD_COMPLETE_MATCH;
				return match_str;
			}
		}
	}

	match_str = (char **) matchvec->index;
	vector_only_wrapper_free(matchvec);
	*status = CMD_COMPLETE_LIST_MATCH;
	return match_str;
}

char **
cmd_complete_command(vector vline, struct vty *vty, int *status) {
	char **ret;

	if (cmd_try_do_shortcut(vty->node, vector_slot(vline, 0))) {
		enum node_type onode;
		vector shifted_vline;
		unsigned int index;

		onode = vty->node;
		vty->node = ENABLE_NODE;
		/* We can try it on enable node, cos' the vty is authenticated */

		shifted_vline = vector_init(vector_count(vline));
		/* use memcpy? */
		for (index = 1; index < vector_active (vline); index++) {
			vector_set_index(shifted_vline, index - 1, vector_lookup(vline,
					index));
		}

		ret = cmd_complete_command_real(shifted_vline, vty, status);

		vector_free(shifted_vline);
		vty->node = onode;
		return ret;
	}

	return cmd_complete_command_real(vline, vty, status);
}

/* return parent node */
/* MUST eventually converge on CONFIG_NODE */
enum node_type node_parent(enum node_type node) {
	enum node_type ret;

    /*begin modified by liaohongjun 2012/8/22*/
	//assert (node > CONFIG_NODE);
	assert (node > ENABLE_NODE);

	switch (node) {
	case BGP_VPNV4_NODE:
	case BGP_IPV4_NODE:
	case BGP_IPV4M_NODE:
	case BGP_IPV6_NODE:
	case BGP_IPV6M_NODE:
		ret = BGP_NODE;
		break;
	case KEYCHAIN_KEY_NODE:
		ret = KEYCHAIN_NODE;
		break;
	default:
		//ret = CONFIG_NODE;
		ret = ENABLE_NODE;
        /*end modified by liaohongjun 2012/8/22*/
	}

	return ret;
}

/* Execute command by argument vline vector. */
static int cmd_execute_command_real(vector vline, struct vty *vty,
		struct cmd_element **cmd) {
	unsigned int i;
	unsigned int index;
	vector cmd_vector;
	struct cmd_element *cmd_element;
	struct cmd_element *matched_element;
	unsigned int matched_count, incomplete_count;
	int argc;
	const char *argv[CMD_ARGC_MAX];
	enum match_type match = 0;
	int varflag;
	char *command;

	/* Make copy of command elements. */
	cmd_vector = vector_copy(cmd_node_vector(cmdvec, vty->node));

	for (index = 0; index < vector_active (vline); index++)
		if ((command = vector_slot (vline, index))) {
			int ret;

			match = cmd_filter_by_completion(command, cmd_vector, index);

			if (match == vararg_match)
				break;

			ret = is_cmd_ambiguous(command, cmd_vector, index, match);

			if (ret == 1) {
				vector_free(cmd_vector);
				return CMD_ERR_AMBIGUOUS;
			} else if (ret == 2) {
				vector_free(cmd_vector);
				return CMD_ERR_NO_MATCH;
			}
		}

	/* Check matched count. */
	matched_element = NULL;
	matched_count = 0;
	incomplete_count = 0;

	for (i = 0; i < vector_active (cmd_vector); i++)
		if ((cmd_element = vector_slot (cmd_vector, i))) {
			if (match == vararg_match || index >= cmd_element->cmdsize) {
				matched_element = cmd_element;
#if 0
				printf ("DEBUG: %s\n", cmd_element->string);
#endif          
				/*Begin add by huangmingjian 2012/09/03 for EPN104QID0018*/ 
				if(matched_element->attr == CMD_ATTR_HIDDEN)
				{  	
					if(!memchr(matched_element->string, ' ', strlen(matched_element->string)))
					{
						if(0 != strcmp(matched_element->string, vty->buf))
						{
                            /*begin added by liaohongjun 2012/9/26 of QID0059*/
                            vector_free(cmd_vector);
                            /*end added by liaohongjun 2012/9/26 of QID0059*/
							return CMD_ERR_NO_MATCH;
						}
					}
				}
				/*End add by huangmingjian 2012/09/03 for EPN104QID0018 */
				matched_count++;
			} else {
				incomplete_count++;
			}
		}

	/* Finish of using cmd_vector. */
	vector_free(cmd_vector);

	/* To execute command, matched_count must be 1. */
	if (matched_count == 0) {
		if (incomplete_count)
			return CMD_ERR_INCOMPLETE;
		else
			return CMD_ERR_NO_MATCH;
	}

	if (matched_count > 1)
		return CMD_ERR_AMBIGUOUS;

	/* Argument treatment */
	varflag = 0;
	argc = 0;

	for (i = 0; i < vector_active (vline); i++) {
		if (varflag)
			argv[argc++] = vector_slot (vline, i);
		else {
			vector descvec = vector_slot (matched_element->strvec, i);

			if (vector_active (descvec) == 1) {
				struct desc *desc = vector_slot (descvec, 0);

				if (CMD_VARARG (desc->cmd))
					varflag = 1;

				if (varflag || CMD_VARIABLE (desc->cmd)
						|| CMD_OPTION (desc->cmd))
					argv[argc++] = vector_slot (vline, i);
			} else
				argv[argc++] = vector_slot (vline, i);
		}

		if (argc >= CMD_ARGC_MAX)
			return CMD_ERR_EXEED_ARGC_MAX;
	}

	/* For vtysh execution. */
	if (cmd)
		*cmd = matched_element;

	if (matched_element->daemon)
		return CMD_SUCCESS_DAEMON;

	/* Execute matched command. */
	return (*matched_element->func)(matched_element, vty, argc, argv);
}

int cmd_execute_command(vector vline, struct vty *vty,
		struct cmd_element **cmd, int vtysh) {
	int ret, saved_ret, tried = 0;
	enum node_type onode, try_node;

	onode = try_node = vty->node;

	if (cmd_try_do_shortcut(vty->node, vector_slot(vline, 0))) {
		vector shifted_vline;
		unsigned int index;

		vty->node = ENABLE_NODE;
		/* We can try it on enable node, cos' the vty is authenticated */

		shifted_vline = vector_init(vector_count(vline));
		/* use memcpy? */
		for (index = 1; index < vector_active (vline); index++) {
			vector_set_index(shifted_vline, index - 1, vector_lookup(vline,
					index));
		}

		ret = cmd_execute_command_real(shifted_vline, vty, cmd);

		vector_free(shifted_vline);
		vty->node = onode;
		return ret;
	}

	saved_ret = ret = cmd_execute_command_real(vline, vty, cmd);

	if (vtysh)
		return saved_ret;
    /*begin modified by liaohongjun 2012/8/22*/
#if 0  
	/* This assumes all nodes above CONFIG_NODE are childs of CONFIG_NODE */  
	while (ret != CMD_SUCCESS && ret != CMD_WARNING && vty->node > CONFIG_NODE) {
#endif
        /* This assumes all nodes above ENABLE_NODE are childs of ENABLE_NODE */ 
        while (ret != CMD_SUCCESS && ret != CMD_WARNING && vty->node > ENABLE_NODE) {
            /*end modified by liaohongjun 2012/8/22*/
		try_node = node_parent(try_node);
		vty->node = try_node;
		ret = cmd_execute_command_real(vline, vty, cmd);
		tried = 1;
		if (ret == CMD_SUCCESS || ret == CMD_WARNING) {
			/* succesfull command, leave the node as is */
			return ret;
		}
	}
	/* no command succeeded, reset the vty to the original node and
	 return the error for this node */
	if (tried)
		vty->node = onode;
	return saved_ret;
}

/* Execute command by argument readline. */
int cmd_execute_command_strict(vector vline, struct vty *vty,
		struct cmd_element **cmd) {
	unsigned int i;
	unsigned int index;
	vector cmd_vector;
	struct cmd_element *cmd_element;
	struct cmd_element *matched_element;
	unsigned int matched_count, incomplete_count;
	int argc;
	const char *argv[CMD_ARGC_MAX];
	int varflag;
	enum match_type match = 0;
	char *command;

	/* Make copy of command element */
	cmd_vector = vector_copy(cmd_node_vector(cmdvec, vty->node));

	for (index = 0; index < vector_active (vline); index++)
		if ((command = vector_slot (vline, index))) {
			int ret;

			match = cmd_filter_by_string(vector_slot (vline, index),
					cmd_vector, index);

			/* If command meets '.VARARG' then finish matching. */
			if (match == vararg_match)
				break;

			ret = is_cmd_ambiguous(command, cmd_vector, index, match);
			if (ret == 1) {
				vector_free(cmd_vector);
				return CMD_ERR_AMBIGUOUS;
			}
			if (ret == 2) {
				vector_free(cmd_vector);
				return CMD_ERR_NO_MATCH;
			}
		}

	/* Check matched count. */
	matched_element = NULL;
	matched_count = 0;
	incomplete_count = 0;
	for (i = 0; i < vector_active (cmd_vector); i++)
		if (vector_slot (cmd_vector, i) != NULL) {
			cmd_element = vector_slot (cmd_vector, i);

			if (match == vararg_match || index >= cmd_element->cmdsize) {
				matched_element = cmd_element;
				matched_count++;
			} else
				incomplete_count++;
		}

	/* Finish of using cmd_vector. */
	vector_free(cmd_vector);

	/* To execute command, matched_count must be 1. */
	if (matched_count == 0) {
		if (incomplete_count)
			return CMD_ERR_INCOMPLETE;
		else
			return CMD_ERR_NO_MATCH;
	}

	if (matched_count > 1)
		return CMD_ERR_AMBIGUOUS;

	/* Argument treatment */
	varflag = 0;
	argc = 0;

	for (i = 0; i < vector_active (vline); i++) {
		if (varflag)
			argv[argc++] = vector_slot (vline, i);
		else {
			vector descvec = vector_slot (matched_element->strvec, i);

			if (vector_active (descvec) == 1) {
				struct desc *desc = vector_slot (descvec, 0);

				if (CMD_VARARG (desc->cmd))
					varflag = 1;

				if (varflag || CMD_VARIABLE (desc->cmd)
						|| CMD_OPTION (desc->cmd))
					argv[argc++] = vector_slot (vline, i);
			} else
				argv[argc++] = vector_slot (vline, i);
		}

		if (argc >= CMD_ARGC_MAX)
			return CMD_ERR_EXEED_ARGC_MAX;
	}

	/* For vtysh execution. */
	if (cmd)
		*cmd = matched_element;

	if (matched_element->daemon)
		return CMD_SUCCESS_DAEMON;

	/* Now execute matched command */
	return (*matched_element->func)(matched_element, vty, argc, argv);
}

/* Configration make from file. */
int config_from_file(struct vty *vty, FILE *fp) {
	int ret;
	vector vline;

	while (fgets(vty->buf, VTY_BUFSIZ, fp)) {
		vline = cmd_make_strvec(vty->buf);

		/* In case of comment line */
		if (vline == NULL)
			continue;
		/* Execute configuration command : this is strict match */
		ret = cmd_execute_command_strict(vline, vty, NULL);

		/* Try again with setting node to CONFIG_NODE */
		while (ret != CMD_SUCCESS && ret != CMD_WARNING && ret
				!= CMD_ERR_NOTHING_TODO && vty->node != CONFIG_NODE) {
			vty->node = node_parent(vty->node);
			ret = cmd_execute_command_strict(vline, vty, NULL);
		}

		cmd_free_strvec(vline);

		if (ret != CMD_SUCCESS && ret != CMD_WARNING && ret
				!= CMD_ERR_NOTHING_TODO)
			return ret;
	}
	return CMD_SUCCESS;
}

/*begin added by liaohongjun 2012/9/7*/
extern void enter_shell(void);

DEFUN_HIDDEN (debug_shell,
		debug_shell_cmd,
		"_debugshell",
		"Enter shell debug mode\n")
{

	if (vty->address&&(strcmp("Console",vty->address)==0))
	{
	    enter_shell();	
		
		if (vty->address)
	     XFREE (MTYPE_TMP, vty->address);
		vty->status = VTY_CLOSE;
		vty->node = VIEW_NODE;
	}
//	vty_close(vty);
       //             /* cannot call vty_close, beca
	return CMD_SUCCESS;
}

/*end added by liaohongjun 2012/9/7*/

#ifdef CONFIG_CATVCOM
/*Begin add by huangmingjian 2013/03/13 for EPN204QID0033*/ 


DEFUN_HIDDEN (ontest,
					ontest_cmd,
					"ontest",
					"Enable catv serial and laser on test\n")
{
   	vty_out(vty, "enable catv and laser on test%s", VTY_NEWLINE);
   	gst_CATVInfo->catv_laser_test = 1;
	
   	return CMD_SUCCESS;
}

DEFUN_HIDDEN(offtest,
					offtest_cmd,
					"offtest",
					"Disable catv serial and laser on test\n")
{
	vty_out(vty, "disable catv and laser on test%s", VTY_NEWLINE);
   	gst_CATVInfo->catv_laser_test = 0;
	
   	return CMD_SUCCESS;
}


DEFUN_HIDDEN (onrevfirst,
					onrevfirst_cmd,
					"onrevfirst",
					"Set the board as receiver first in catv serial test\n")
{
    vty_out(vty, "set the onrevfirst flag%s", VTY_NEWLINE);
    gst_CATVInfo->catv_rev_first = 1;

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (offrevfirst,
					offrevfirst_cmd,
					"offrevfirst",
					"Set the board as receiver first in catv serial test\n")
{

    vty_out(vty, "set the offrevfirst flag%s", VTY_NEWLINE);
    gst_CATVInfo->catv_rev_first = 0;

	return CMD_SUCCESS;
}

/*End add by huangmingjian 2013/03/13 for EPN204QID0033*/ 
#endif

/* Configration from terminal */
DEFUN (config_terminal,
		config_terminal_cmd,
		"configure terminal",
		"Configuration from vty interface\n"
		"Configuration terminal\n")
{
    #if 0
	if (vty_config_lock(vty))
		vty->node = CONFIG_NODE;
	else {
		vty_out(vty, "VTY configuration is locked by other VTY%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    #endif
    vty->node = CONFIG_NODE;
	return CMD_SUCCESS;
}

/* Enable command */
DEFUN (enable,
		config_enable_cmd,
		"enable",
		"Turn on privileged mode command\n")
{
	/* If enable password is NULL, change to ENABLE_NODE */
    /*begin modified by liaohongjun 2012/9/5 of QID0015*/
    #if 0
	if ((host.enable == NULL && host.enable_encrypt == NULL) || vty->type
			== VTY_SHELL_SERV)
		vty->node = ENABLE_NODE;
    #endif
    if(!strcmp(host.enable, DEFAULT_VAL_SUPER) || VTY_SHELL_SERV == vty->type)
    {
        vty->node = ENABLE_NODE;
    }
	else
	{
		vty->node = AUTH_ENABLE_NODE;
        /*end modified by liaohongjun 2012/9/5 of QID0015*/
	}

	return CMD_SUCCESS;
}


/* Disable command */
DEFUN (disable,
		config_disable_cmd,
		"disable",
		"Turn off privileged mode command\n")
{
	if (vty->node == ENABLE_NODE)
		vty->node = VIEW_NODE;
	return CMD_SUCCESS;
}

extern void restore_terminal_and_exit(void);
/* Down vty node level. */
DEFUN (config_exit,
		config_exit_cmd,
		"exit",
		"Exit current mode and down to previous mode\n")
{
	switch (vty->node) {
/*	case AUTH_NODE:
//	case AUTH_PASSWD_NODE:
#if 0
		if (vty_shell (vty))
		exit (0);
		else
		vty->status = VTY_CLOSE;
#endif

		// By Ryan
		if (!strcmp(vty->address, "Console")) {
			// For debug only, if exit from restricted node, vtysh should not exit.
			restore_terminal_and_exit();
		} else {
			vty->status = VTY_CLOSE;
		}
		break;*/
	case VIEW_NODE:
		if (!strcmp(vty->address, "Console")) {
			// For debug only, if exit from restricted node, vtysh should not exit.
			//restore_terminal_and_exit();
			vty_out(vty,"%s", VTY_NEWLINE);
            vty->status = VTY_CLOSE;
		} else {
				if(strcmp(vty->address,"ssh")==0)
				{
					exit(0);
				}
			vty->status = VTY_CLOSE;
			vty_user[vty->current_no].login_flag = 0;
		}
//	case RESTRICTED_NODE:
//		vty->node = AUTH_NODE;
		break;
	case ENABLE_NODE:
		vty->node = VIEW_NODE;
		break;
	case SU_NODE:
		vty->node = ENABLE_NODE;
		break;
	case CONFIG_NODE:
		vty->node = ENABLE_NODE;
		vty_config_unlock(vty);
		break;
	case INTERFACE_NODE:
	case ZEBRA_NODE:
	case BGP_NODE:
	case RIP_NODE:
	case RIPNG_NODE:
	case OSPF_NODE:
	case OSPF6_NODE:
	case ISIS_NODE:
	case KEYCHAIN_NODE:
	case MASC_NODE:
	case RMAP_NODE:
		vty->node = CONFIG_NODE;
		break;
	case VTY_NODE:
		/*Begin add by huangmingjian 2013-11-18 for Bug 276*/
		vty->node = ENABLE_NODE;
		break;
		/*End add by huangmingjian 2013-11-18 for Bug 276*/
    /*begin added by liaohongjun 2012/9/18 of QID0019*/
    case VLAN_INTERFACE_NODE:
    /*end added by liaohongjun 2012/9/18 of QID0019*/   
	/*begin added by leijinbao 2013/11/1of QID237*/ 
		vty->node = CONFIG_NODE;
	/*begin added by leijinbao 2012/11/1 of QID237*/
		break;
	case BGP_VPNV4_NODE:
	case BGP_IPV4_NODE:
	case BGP_IPV4M_NODE:
	case BGP_IPV6_NODE:
	case BGP_IPV6M_NODE:
		vty->node = BGP_NODE;
		break;
	case KEYCHAIN_KEY_NODE:
		vty->node = KEYCHAIN_NODE;
		break;
	case SYSCONFIG_NODE:
		vty->node = ENABLE_NODE;
		break;
	default:
		break;
	}
	return CMD_SUCCESS;
}
/* quit is alias of exit. */
ALIAS (config_exit,
		config_quit_cmd,
		"quit",
		"Exit current mode and down to previous mode\n")

/* End of configuration. */
DEFUN (config_end,
		config_end_cmd,
		"home",
		"End current mode and change to home mode.")
{
	switch (vty->node) {
	case VIEW_NODE:
	case ENABLE_NODE:
	case RESTRICTED_NODE:
		/* Nothing to do. */
		//break;
	case CONFIG_NODE:
    /*begin added by liaohongjun 2012/9/18 of QID0019*/    
    case VLAN_INTERFACE_NODE:
    /*end added by liaohongjun 2012/9/18 of QID0019*/    
	case INTERFACE_NODE:
	case ZEBRA_NODE:
	case RIP_NODE:
	case RIPNG_NODE:
	case BGP_NODE:
	case BGP_VPNV4_NODE:
	case BGP_IPV4_NODE:
	case BGP_IPV4M_NODE:
	case BGP_IPV6_NODE:
	case BGP_IPV6M_NODE:
	case RMAP_NODE:
	case OSPF_NODE:
	case OSPF6_NODE:
	case ISIS_NODE:
	case KEYCHAIN_NODE:
	case KEYCHAIN_KEY_NODE:
	case MASC_NODE:
	case VTY_NODE:
	case SYSCONFIG_NODE:
		vty_config_unlock(vty);
		//vty->node = ENABLE_NODE;
		vty->node = VIEW_NODE;
		break;
	default:
		break;
	}
	return CMD_SUCCESS;
}
/*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
/* Show version. */
DEFUN (show_version,
        show_version_cmd,
        "show version",
        SHOW_STR
        "Device hardware and software version information\n")
{
    int ret;
    sys_info_t info;

    memset(&info, 0x0, sizeof(sys_info_t));
    //ret = ipc_get_sys_info(&info);//disable by zhouguanhua 2013/6/5
    ret = get_info_val(&info);
    if(IPC_STATUS_OK != ret)
    {
        vty_out(vty, " ERROR: get system info failed!%s", PRODUCT_NAME, VTY_NEWLINE);
        return CMD_WARNING;
    }
	
#if defined(PRIV_PRODUCT_NAME_SUPPORT)	
	vty_out(vty, "Product Model    : %s%s", CONFIG_PRODUCT_NAME_PRIV, VTY_NEWLINE);
#else
    vty_out(vty, "Product Model    : %s%s", PRODUCT_NAME, VTY_NEWLINE);
#endif
    vty_out(vty, "MAC Address      : %s%s", mac2str(&info.sysmac), VTY_NEWLINE);    
    vty_out(vty, "Hardware Version : %s%s", info.hw_version, VTY_NEWLINE);	
    vty_out(vty, "Software Version : %s%s", info.sw_version, VTY_NEWLINE);
    vty_out(vty, "Built Time       : %s%s", info.buildtime, VTY_NEWLINE);
    vty_out(vty, "Runtime          : %s%s", runtime_str(info.uptime), VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    
    return CMD_SUCCESS;
}
/*end added by liaohongjun 2012/9/18 of EPN104QID0019*/

/* Help display function for all node. */
DEFUN (config_help,
		config_help_cmd,
		"help",
		"Description of the interactive help system\n")
{
	vty_out(
			vty,
			"Quagga VTY provides advanced help feature.  When you need help,%s\
anytime at the command line please press '?'.%s\
%s\
If nothing matches, the help list will be empty and you must backup%s\
 until entering a '?' shows the available options.%s\
Two styles of help are provided:%s\
1. Full help is available when you are ready to enter a%s\
command argument (e.g. 'show ?') and describes each possible%s\
argument.%s\
2. Partial help is provided when an abbreviated argument is entered%s\
   and you want to know what arguments match the input%s\
   (e.g. 'show me?'.)%s%s",
			VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE,
			VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE,
			VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);
	return CMD_SUCCESS;
}

/* Help display function for all node. */
DEFUN (config_list,
		config_list_cmd,
		"list",
		"Print command list\n")
{
	unsigned int i;
	struct cmd_node *cnode = vector_slot (cmdvec, vty->node);
	struct cmd_element *cmd;
	
    if(1 == DEBUG_FLAG)
    {
    	for (i = 0; i < vector_active (cnode->cmd_vector); i++)
			if ((cmd = vector_slot (cnode->cmd_vector, i)) != NULL && !(cmd->attr
				== CMD_ATTR_DEPRECATED )) 
				vty_out(vty, "  %s%s", cmd->string, VTY_NEWLINE);
	}
   else
   {
		for (i = 0; i < vector_active (cnode->cmd_vector); i++)
			if ((cmd = vector_slot (cnode->cmd_vector, i)) != NULL && !(cmd->attr
					== CMD_ATTR_DEPRECATED || cmd->attr == CMD_ATTR_HIDDEN))
				vty_out(vty, "  %s%s", cmd->string, VTY_NEWLINE);

   }
	return CMD_SUCCESS;

}




DEFUN_HIDDEN (debug,
					debug_cmd,
					"debug",
					"show hidden command\n")
{
	if(strstr(self->string, "no"))
		DEBUG_FLAG = 0;
	else 
		DEBUG_FLAG = 1;
	return CMD_SUCCESS;
}

ALIAS_HIDDEN(debug,
	               no_debug_cmd,
	               "nodebug",
	               "don't show hidden command\n"
					);




/* Write current configuration into file. */
DEFUN (config_write_file,
		config_write_file_cmd,
		"write file",
		"Write running configuration to memory, network, or terminal\n"
		"Write to configuration file\n")
{
	unsigned int i;
	int fd;
	struct cmd_node *node;
	char *config_file;
	char *config_file_tmp = NULL;
	char *config_file_sav = NULL;
	int ret = CMD_WARNING;
	struct vty *file_vty;

	/* Check and see if we are operating under vtysh configuration */
	if (host.config == NULL) {
		vty_out(vty, "Can't save to configuration file, using vtysh.%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get filename. */
	config_file = host.config;

	config_file_sav
			= XMALLOC (MTYPE_TMP, strlen (config_file) + strlen (CONF_BACKUP_EXT) + 1);
	strcpy(config_file_sav, config_file);
	strcat(config_file_sav, CONF_BACKUP_EXT);

	config_file_tmp = XMALLOC (MTYPE_TMP, strlen (config_file) + 8);
	sprintf(config_file_tmp, "%s.XXXXXX", config_file);

	/* Open file to configuration write. */
	fd = mkstemp(config_file_tmp);
	if (fd < 0) {
		vty_out(vty, "Can't open configuration file %s.%s", config_file_tmp,
				VTY_NEWLINE);
		goto finished;
	}

	/* Make vty for configuration file. */
	file_vty = vty_new();
	file_vty->fd = fd;
	file_vty->type = VTY_FILE;

	/* Config file header print. */
	vty_out(file_vty, "!\n! Zebra configuration saved from vty\n!   ");
	vty_time_print(file_vty, 1);
	vty_out(file_vty, "!\n");

	for (i = 0; i < vector_active (cmdvec); i++)
		if ((node = vector_slot (cmdvec, i)) && node->func) {
			if ((*node->func)(file_vty))
				vty_out(file_vty, "!\n");
		}
	vty_close(file_vty);

	if (unlink(config_file_sav) != 0)
		if (errno != ENOENT) {
			vty_out(vty, "Can't unlink backup configuration file %s.%s",
					config_file_sav, VTY_NEWLINE);
			goto finished;
		}
	if (link(config_file, config_file_sav) != 0) {
		vty_out(vty, "Can't backup old configuration file %s.%s",
				config_file_sav, VTY_NEWLINE);
		goto finished;
	}
	sync();
	if (unlink(config_file) != 0) {
		vty_out(vty, "Can't unlink configuration file %s.%s", config_file,
				VTY_NEWLINE);
		goto finished;
	}
	if (link(config_file_tmp, config_file) != 0) {
		vty_out(vty, "Can't save configuration file %s.%s", config_file,
				VTY_NEWLINE);
		goto finished;
	}
	sync();

	if (chmod(config_file, CONFIGFILE_MASK) != 0) {
		vty_out(vty, "Can't chmod configuration file %s: %s (%d).%s",
				config_file, safe_strerror(errno), errno, VTY_NEWLINE);
		goto finished;
	}

	vty_out(vty, "Configuration saved to %s%s", config_file, VTY_NEWLINE);
	ret = CMD_SUCCESS;

	finished: unlink(config_file_tmp);
	XFREE (MTYPE_TMP, config_file_tmp);
	XFREE (MTYPE_TMP, config_file_sav);
	return ret;
}
ALIAS (config_write_file,
		config_write_cmd,
		"write",
		"Write running configuration to memory, network, or terminal\n")
ALIAS (config_write_file,
		config_write_memory_cmd,
		"write memory",
		"Write running configuration to memory, network, or terminal\n"
		"Write configuration to the file (same as write file)\n")
ALIAS (config_write_file,
		copy_runningconfig_startupconfig_cmd,
		"copy running-config startup-config",
		"Copy configuration\n"
		"Copy running config to... \n"
		"Copy running config to startup config (same as write file)\n")

/* Write current configuration into the terminal. */

DEFUN (config_write_terminal,
		config_write_terminal_cmd,
		"write terminal",
		"Write running configuration to memory, network, or terminal\n"
		"Write to terminal\n")
{
	unsigned int i;
	struct cmd_node *node;

	if (vty->type == VTY_SHELL_SERV) {
		for (i = 0; i < vector_active (cmdvec); i++)
			if ((node = vector_slot (cmdvec, i)) && node->func && node->vtysh) {
				if ((*node->func)(vty))
					vty_out(vty, "!%s", VTY_NEWLINE);
			}
	} else {
		vty_out(vty, "%sCurrent configuration:%s", VTY_NEWLINE, VTY_NEWLINE);
		vty_out(vty, "start%s", VTY_NEWLINE);

		for (i = 0; i < vector_active (cmdvec); i++)
			if ((node = vector_slot (cmdvec, i)) && node->func) 
			{/*Begin modified by huangmingjian 2012/08/20 for EPN104QID0007*/  
				(*node->func)(vty);
			 /*End modified by huangmingjian 2012/08/20 for EPN104QID0007 */
			}
		vty_out(vty, "end%s", VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}

/*Begin add by huangmingjian 2012/08/23 for EPN104QID0008*/ 
/* Hostname configuration */
DEFUN (config_hostname,
		hostname_cmd,
		"hostname WORD<1-16>",
		"Set system's network name\n"
		"This system's network name\n")
{   
    int ret;
	if(strlen(argv[0]) > 16)
   {	
   	   vty_out(vty, "Hostname is too long, make sure that it is between 1-16 characters%s",VTY_NEWLINE);
	   return CMD_WARNING;
   }
	 
	if (!isalpha((int) *argv[0])) {
		vty_out(vty, "Please specify string starting with alphabet%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*Begin add by huangmingjian 2012/10/11 for EPN104QID0070*/ 
		/*Begin add by huangmingjian 2012/12/3 for EPN204QID0012*/ 
	if( (NULL != strchr(argv[0], ';')) || (NULL != strchr(argv[0], '\''))
	  ||(NULL != strchr(argv[0], '\\'))||(NULL != strchr(argv[0], '|'))
	  ||(NULL != strchr(argv[0], ')'))||(NULL != strchr(argv[0], '(')) 
	  ||(NULL != strchr(argv[0], '`'))||(NULL != strchr(argv[0], '~'))
	  ||(NULL != strchr(argv[0], '&'))||(NULL != strchr(argv[0], '<'))
	  ||(NULL != strchr(argv[0], '>')) )
	{
		vty_out(vty, "Symbols ; '( ) | ` ~ & < > and \\ are not allowed in hostname%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*Begin add by huangmingjian 2012/10/11 for EPN104QID0070*/ 
	
	/*Begin add by huangmingjian 2012/09/25 for EPN104QID0058*/ 
	if(0 == strcmp(host.name, argv[0]))
	{	
		return CMD_SUCCESS;
	}
	/*End add by huangmingjian 2012/09/25 for EPN104QID0058 */
	if(host.name)
		XFREE (MTYPE_HOST, host.name);
    host.name = NULL;
	host.name = XSTRDUP (MTYPE_HOST, argv[0]);
	memset(ht_init.ht_name, 0x0, sizeof(ht_init.ht_name));
	strncpy(ht_init.ht_name, host.name, sizeof(ht_init.ht_name) - 1); 
	ret = ipc_set_host_info(&ht_init, HOSTNAME);
	VTY_IPC_ASSERT(ret);
	return CMD_SUCCESS;
}



DEFUN (config_no_hostname,
		no_hostname_cmd,
		"no hostname",
		NO_STR
		"Reset system's network name\n"
		"Host name of this router\n")
{
	int ret;
	if (host.name)
	{
		XFREE (MTYPE_HOST, host.name);
	}
	host.name = NULL;
	host.name = XSTRDUP (MTYPE_HOST, DEFAULT_VAL_HOSTNAME);
	memset(ht_init.ht_name, 0x0, sizeof(ht_init.ht_name));
	strncpy(ht_init.ht_name, host.name, sizeof(ht_init.ht_name) - 1); 
	ret = ipc_set_host_info(&ht_init, HOSTNAME);
	VTY_IPC_ASSERT(ret);
	return CMD_SUCCESS;
}


DEFUN(config_no_super,
	    no_super_cmd,
	    "no super password",
	    NO_STR
	    "Modify  super password parameters\n"
	    "Super password of this device\n")
{   
	int ret;
	if (host.enable)
	{
		XFREE (MTYPE_HOST, host.enable);
	}
	host.enable = NULL;
	host.enable = XSTRDUP (MTYPE_HOST, DEFAULT_VAL_SUPER);
	memset(ht_init.ht_super, 0x0, sizeof(ht_init.ht_super));
	strncpy(ht_init.ht_super, host.enable, sizeof(ht_init.ht_super) - 1); 
	ret = ipc_set_host_info(&ht_init, SUPER_PASSWD);
	VTY_IPC_ASSERT(ret);
	return CMD_SUCCESS;
}

/*End add by huangmingjian 2012/08/23 for EPN104QID0008 */

/*

DEFUN (show_password, show_password_cmd,
		"show password",
		"Display ..."
		"Display  password and password_encrypt\n")
{   

	
	int ret;
	host_init_t ht_init;
	memset(&ht_init, '\0', sizeof(host_init_t));
   DBG_PRINTF("go in show password\n");
	ret = ipc_get_host_info(&ht_init);
	host.username = ht_init.ht_usrname;
	host.password = ht_init.ht_passwd;
	host.enable = ht_init.ht_super;
	
//	VTY_IPC_ASSERT(ret);
    if(strstr(self->string, "en") || strstr(self->string, "enable"))
	{
		vty_out(vty, "enable:%s   enable_encrypt:%s%s",host.enable ,host.enable_encrypt, VTY_NEWLINE);
	    return CMD_SUCCESS;
	}
	vty_out(vty, "password:%s   password_encrypt:%s%s",host.password ,host.password_encrypt, VTY_NEWLINE);
	return CMD_SUCCESS;
}

ALIAS (show_password, show_enable_password_cmd,
		"show enable password",
		"Display ..."
		"Display  password and password_encrypt\n");

*/


///* VTY enable password set. */
/*
DEFUN (config_enable_password, enable_password_cmd,
		"enable password (8|) WORD",
		"Modify enable password parameters\n"
		"Assign the privileged level password\n"
		"Specifies a HIDDEN password will follow\n"
		"dummy string \n"
		"The HIDDEN 'enable' password string\n")
{
	//Argument check. 
	if (argc == 0) {
		vty_out(vty, "Please specify password.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	// Crypt type is specified. 
	if (argc == 2) {
		if (*argv[0] == '8') {

			if (host.enable)
				XFREE (MTYPE_HOST, host.enable);
			host.enable = NULL;

			if (host.enable_encrypt)
				XFREE (MTYPE_HOST, host.enable_encrypt);
			host.enable_encrypt = XSTRDUP (MTYPE_HOST, argv[1]);

			return CMD_SUCCESS;
		} else {
			vty_out(vty, "Unknown encryption type.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	if (!isalnum ((int) *argv[0])) {
		vty_out(vty, "Please specify string starting with alphanumeric%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (host.enable)
		XFREE (MTYPE_HOST, host.enable);
	host.enable = NULL;

	// Plain password input. 
	if (host.encrypt) {
		if (host.enable_encrypt)
			XFREE (MTYPE_HOST, host.enable_encrypt);
		host.enable_encrypt = XSTRDUP (MTYPE_HOST, zencrypt (argv[0]));
	} else
		host.enable = XSTRDUP (MTYPE_HOST, argv[0]);
	return CMD_SUCCESS;
}
ALIAS (config_enable_password,
		super_password_text_cmd,
		"enable password LINE",
		"Modify enable password parameters\n"
		"Assign the privileged level password\n"
		"The UNENCRYPTED (cleartext) 'enable' password\n");

*/

/*Begin add by huangmingjian 2012/08/23 for EPN104QID0008*/ 
DEFUN (config_super_password,
		super_password_cmd,
		"super password LINE<1-16>",
		"Modify enable password parameters\n"
		"Assign the privileged level password\n"
		"The UNENCRYPTED (cleartext) 'enable' password\n")
{
   int ret;

   if(strlen(argv[0]) > 16)
   {	
   	   vty_out(vty, "Password is too long, make sure that it is between 1-16 characters%s",VTY_NEWLINE);
	   return CMD_WARNING;
   }
   if(!isalpha((int) *argv[0])) 
   {
	   vty_out(vty, "Please specify string starting with alphabet%s",VTY_NEWLINE);
	   return CMD_WARNING;
   }
   if(host.enable)
	   XFREE (MTYPE_HOST, host.enable);
   host.enable = NULL;
   host.enable = XSTRDUP(MTYPE_HOST, argv[0]);
   
   memset(ht_init.ht_super, 0x0, sizeof(ht_init.ht_super));
   strncpy(ht_init.ht_super, host.enable, sizeof(ht_init.ht_super) - 1);
   ret = ipc_set_host_info(&ht_init, SUPER_PASSWD);
   VTY_IPC_ASSERT(ret);
   return CMD_SUCCESS;
}

/*End add by huangmingjian 2012/08/23 for EPN104QID0008 */

/* VTY enable password delete. */

/*
DEFUN (no_config_enable_password, no_enable_password_cmd,
		"no enable password",
		NO_STR
		"Modify enable password parameters\n"
		"Assign the privileged level password\n")
{
	if (host.enable)
		XFREE (MTYPE_HOST, host.enable);
	host.enable = NULL;

	if (host.enable_encrypt)
		XFREE (MTYPE_HOST, host.enable_encrypt);
	host.enable_encrypt = NULL;

	return CMD_SUCCESS;
}


*/



DEFUN (service_password_encrypt,
		service_password_encrypt_cmd,
		"service password-encryption",
		"Set up miscellaneous service\n"
		"Enable encrypted passwords\n")
{
	if (host.encrypt)
		return CMD_SUCCESS;

	host.encrypt = 1;

	if (host.password) {
		if (host.password_encrypt)
			XFREE (MTYPE_HOST, host.password_encrypt);
		host.password_encrypt = XSTRDUP (MTYPE_HOST, zencrypt (host.password));
	}
	if (host.enable) {
		if (host.enable_encrypt)
			XFREE (MTYPE_HOST, host.enable_encrypt);
		host.enable_encrypt = XSTRDUP (MTYPE_HOST, zencrypt (host.enable));
	}

	return CMD_SUCCESS;
}

DEFUN (no_service_password_encrypt,
		no_service_password_encrypt_cmd,
		"no service password-encryption",
		NO_STR
		"Set up miscellaneous service\n"
		"Enable encrypted passwords\n")
{
	if (!host.encrypt)
		return CMD_SUCCESS;

	host.encrypt = 0;

	if (host.password_encrypt)
		XFREE (MTYPE_HOST, host.password_encrypt);
	host.password_encrypt = NULL;

	if (host.enable_encrypt)
		XFREE (MTYPE_HOST, host.enable_encrypt);
	host.enable_encrypt = NULL;

	return CMD_SUCCESS;
}

DEFUN (config_terminal_length, config_terminal_length_cmd,
		"terminal length <0-512>",
		"Set terminal line parameters\n"
		"Set number of lines on a screen\n"
		"Number of lines on screen (0 for no pausing)\n")
{
	int lines;
	char *endptr = NULL;

	lines = strtol(argv[0], &endptr, 10);
	if (lines < 0 || lines > 512 || *endptr != '\0') {
		vty_out(vty, "length is malformed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty->lines = lines;

	return CMD_SUCCESS;
}

DEFUN (config_terminal_no_length, config_terminal_no_length_cmd,
		"terminal no length",
		"Set terminal line parameters\n"
		NO_STR
		"Set number of lines on a screen\n")
{
	vty->lines = -1;
	return CMD_SUCCESS;
}

DEFUN (service_terminal_length, service_terminal_length_cmd,
		"service terminal-length <0-512>",
		"Set up miscellaneous service\n"
		"System wide terminal length configuration\n"
		"Number of lines of VTY (0 means no line control)\n")
{
	int lines;
	char *endptr = NULL;

	lines = strtol(argv[0], &endptr, 10);
	if (lines < 0 || lines > 512 || *endptr != '\0') {
		vty_out(vty, "length is malformed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	host.lines = lines;

	return CMD_SUCCESS;
}

DEFUN (no_service_terminal_length, no_service_terminal_length_cmd,
		"no service terminal-length [<0-512>]",
		NO_STR
		"Set up miscellaneous service\n"
		"System wide terminal length configuration\n"
		"Number of lines of VTY (0 means no line control)\n")
{
	host.lines = -1;
	return CMD_SUCCESS;
}

DEFUN_HIDDEN (do_echo,
		echo_cmd,
		"echo .MESSAGE",
		"Echo a message back to the vty\n"
		"The message to echo\n")
{
	char *message;

	vty_out(vty, "%s%s",
			((message = argv_concat(argv, argc, 0)) ? message : ""),
			VTY_NEWLINE);
	if (message)
		XFREE(MTYPE_TMP, message);
	return CMD_SUCCESS;
}

DEFUN (config_logmsg,
		config_logmsg_cmd,
		"logmsg "LOG_LEVELS" .MESSAGE",
		"Send a message to enabled logging destinations\n"
		LOG_LEVEL_DESC
		"The message to send\n")
{
	int level;
	char *message;

	if ((level = level_match(argv[0])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;

	zlog(NULL, level, ((message = argv_concat(argv, argc, 1)) ? message : ""));
	if (message)
		XFREE(MTYPE_TMP, message);
	return CMD_SUCCESS;
}

DEFUN (show_logging,
		show_logging_cmd,
		"show logging",
		SHOW_STR
		"Show current logging configuration\n")
{
	struct zlog *zl = zlog_default;

	vty_out(vty, "Syslog logging: ");
	if (zl->maxlvl[ZLOG_DEST_SYSLOG] == ZLOG_DISABLED)
		vty_out(vty, "disabled");
	else
		vty_out(vty, "level %s, facility %s, ident %s",
				zlog_priority[zl->maxlvl[ZLOG_DEST_SYSLOG]], facility_name(
						zl->facility), zl->ident);
	vty_out(vty, "%s", VTY_NEWLINE);

	vty_out(vty, "Stdout logging: ");
	if (zl->maxlvl[ZLOG_DEST_STDOUT] == ZLOG_DISABLED)
		vty_out(vty, "disabled");
	else
		vty_out(vty, "level %s", zlog_priority[zl->maxlvl[ZLOG_DEST_STDOUT]]);
	vty_out(vty, "%s", VTY_NEWLINE);

	vty_out(vty, "Monitor logging: ");
	if (zl->maxlvl[ZLOG_DEST_MONITOR] == ZLOG_DISABLED)
		vty_out(vty, "disabled");
	else
		vty_out(vty, "level %s", zlog_priority[zl->maxlvl[ZLOG_DEST_MONITOR]]);
	vty_out(vty, "%s", VTY_NEWLINE);

	vty_out(vty, "File logging: ");
	if ((zl->maxlvl[ZLOG_DEST_FILE] == ZLOG_DISABLED) || !zl->fp)
		vty_out(vty, "disabled");
	else
		vty_out(vty, "level %s, filename %s",
				zlog_priority[zl->maxlvl[ZLOG_DEST_FILE]], zl->filename);
	vty_out(vty, "%s", VTY_NEWLINE);

	vty_out(vty, "Protocol name: %s%s", zlog_proto_names[zl->protocol],
			VTY_NEWLINE);
	vty_out(vty, "Record priority: %s%s", (zl->record_priority ? "enabled"
			: "disabled"), VTY_NEWLINE);
	vty_out(vty, "Timestamp precision: %d%s", zl->timestamp_precision,
			VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (config_log_stdout,
		config_log_stdout_cmd,
		"log stdout",
		"Logging control\n"
		"Set stdout logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_STDOUT, zlog_default->default_lvl);
	return CMD_SUCCESS;
}

DEFUN (config_log_stdout_level,
		config_log_stdout_level_cmd,
		"log stdout "LOG_LEVELS,
		"Logging control\n"
		"Set stdout logging level\n"
		LOG_LEVEL_DESC)
{
	int level;

	if ((level = level_match(argv[0])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;
	zlog_set_level(NULL, ZLOG_DEST_STDOUT, level);
	return CMD_SUCCESS;
}

DEFUN (no_config_log_stdout,
		no_config_log_stdout_cmd,
		"no log stdout [LEVEL]",
		NO_STR
		"Logging control\n"
		"Cancel logging to stdout\n"
		"Logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_STDOUT, ZLOG_DISABLED);
	return CMD_SUCCESS;
}

DEFUN (config_log_monitor,
		config_log_monitor_cmd,
		"log monitor",
		"Logging control\n"
		"Set terminal line (monitor) logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_MONITOR, zlog_default->default_lvl);
	return CMD_SUCCESS;
}

DEFUN (config_log_monitor_level,
		config_log_monitor_level_cmd,
		"log monitor "LOG_LEVELS,
		"Logging control\n"
		"Set terminal line (monitor) logging level\n"
		LOG_LEVEL_DESC)
{
	int level;

	if ((level = level_match(argv[0])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;
	zlog_set_level(NULL, ZLOG_DEST_MONITOR, level);
	return CMD_SUCCESS;
}

DEFUN (no_config_log_monitor,
		no_config_log_monitor_cmd,
		"no log monitor [LEVEL]",
		NO_STR
		"Logging control\n"
		"Disable terminal line (monitor) logging\n"
		"Logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_MONITOR, ZLOG_DISABLED);
	return CMD_SUCCESS;
}

static int set_log_file(struct vty *vty, const char *fname, int loglevel) {
	int ret;
	char *p = NULL;
	const char *fullpath;

	/* Path detection. */
	if (!IS_DIRECTORY_SEP (*fname)) {
		char cwd[MAXPATHLEN + 1];
		cwd[MAXPATHLEN] = '\0';

		if (getcwd(cwd, MAXPATHLEN) == NULL) {
			zlog_err("config_log_file: Unable to alloc mem!");
			return CMD_WARNING;
		}

		if ((p = XMALLOC (MTYPE_TMP, strlen (cwd) + strlen (fname) + 2))
				== NULL) {
			zlog_err("config_log_file: Unable to alloc mem!");
			return CMD_WARNING;
		}
		sprintf(p, "%s/%s", cwd, fname);
		fullpath = p;
	} else
		fullpath = fname;

	ret = zlog_set_file(NULL, fullpath, loglevel);

	if (p)
		XFREE (MTYPE_TMP, p);

	if (!ret) {
		vty_out(vty, "can't open logfile %s\n", fname);
		return CMD_WARNING;
	}

	if (host.logfile)
		XFREE (MTYPE_HOST, host.logfile);

	host.logfile = XSTRDUP (MTYPE_HOST, fname);

	return CMD_SUCCESS;
}

DEFUN (config_log_file,
		config_log_file_cmd,
		"log file FILENAME",
		"Logging control\n"
		"Logging to file\n"
		"Logging filename\n")
{
	return set_log_file(vty, argv[0], zlog_default->default_lvl);
}

DEFUN (config_log_file_level,
		config_log_file_level_cmd,
		"log file FILENAME "LOG_LEVELS,
		"Logging control\n"
		"Logging to file\n"
		"Logging filename\n"
		LOG_LEVEL_DESC)
{
	int level;

	if ((level = level_match(argv[1])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;
	return set_log_file(vty, argv[0], level);
}

DEFUN (no_config_log_file,
		no_config_log_file_cmd,
		"no log file [FILENAME]",
		NO_STR
		"Logging control\n"
		"Cancel logging to file\n"
		"Logging file name\n")
{
	zlog_reset_file(NULL);

	if (host.logfile)
		XFREE (MTYPE_HOST, host.logfile);

	host.logfile = NULL;

	return CMD_SUCCESS;
}
ALIAS (no_config_log_file,
		no_config_log_file_level_cmd,
		"no log file FILENAME LEVEL",
		NO_STR
		"Logging control\n"
		"Cancel logging to file\n"
		"Logging file name\n"
		"Logging level\n")

DEFUN (config_log_syslog,
		config_log_syslog_cmd,
		"log syslog",
		"Logging control\n"
		"Set syslog logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_SYSLOG, zlog_default->default_lvl);
	return CMD_SUCCESS;
}

DEFUN (config_log_syslog_level,
		config_log_syslog_level_cmd,
		"log syslog "LOG_LEVELS,
		"Logging control\n"
		"Set syslog logging level\n"
		LOG_LEVEL_DESC)
{
	int level;

	if ((level = level_match(argv[0])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;
	zlog_set_level(NULL, ZLOG_DEST_SYSLOG, level);
	return CMD_SUCCESS;
}

DEFUN_DEPRECATED (config_log_syslog_facility,
		config_log_syslog_facility_cmd,
		"log syslog facility "LOG_FACILITIES,
		"Logging control\n"
		"Logging goes to syslog\n"
		"(Deprecated) Facility parameter for syslog messages\n"
		LOG_FACILITY_DESC)
{
	int facility;

	if ((facility = facility_match(argv[0])) < 0)
		return CMD_ERR_NO_MATCH;

	zlog_set_level(NULL, ZLOG_DEST_SYSLOG, zlog_default->default_lvl);
	zlog_default->facility = facility;
	return CMD_SUCCESS;
}

DEFUN (no_config_log_syslog,
		no_config_log_syslog_cmd,
		"no log syslog [LEVEL]",
		NO_STR
		"Logging control\n"
		"Cancel logging to syslog\n"
		"Logging level\n")
{
	zlog_set_level(NULL, ZLOG_DEST_SYSLOG, ZLOG_DISABLED);
	return CMD_SUCCESS;
}
ALIAS (no_config_log_syslog,
		no_config_log_syslog_facility_cmd,
		"no log syslog facility "LOG_FACILITIES,
		NO_STR
		"Logging control\n"
		"Logging goes to syslog\n"
		"Facility parameter for syslog messages\n"
		LOG_FACILITY_DESC)

DEFUN (config_log_facility,
		config_log_facility_cmd,
		"log facility "LOG_FACILITIES,
		"Logging control\n"
		"Facility parameter for syslog messages\n"
		LOG_FACILITY_DESC)
{
	int facility;

	if ((facility = facility_match(argv[0])) < 0)
		return CMD_ERR_NO_MATCH;
	zlog_default->facility = facility;
	return CMD_SUCCESS;
}

DEFUN (no_config_log_facility,
		no_config_log_facility_cmd,
		"no log facility [FACILITY]",
		NO_STR
		"Logging control\n"
		"Reset syslog facility to default (daemon)\n"
		"Syslog facility\n")
{
	zlog_default->facility = LOG_DAEMON;
	return CMD_SUCCESS;
}

DEFUN_DEPRECATED (config_log_trap,
		config_log_trap_cmd,
		"log trap "LOG_LEVELS,
		"Logging control\n"
		"(Deprecated) Set logging level and default for all destinations\n"
		LOG_LEVEL_DESC)
{
	int new_level;
	int i;

	if ((new_level = level_match(argv[0])) == ZLOG_DISABLED)
		return CMD_ERR_NO_MATCH;

	zlog_default->default_lvl = new_level;
	for (i = 0; i < ZLOG_NUM_DESTS; i++)
		if (zlog_default->maxlvl[i] != ZLOG_DISABLED)
			zlog_default->maxlvl[i] = new_level;
	return CMD_SUCCESS;
}

DEFUN_DEPRECATED (no_config_log_trap,
		no_config_log_trap_cmd,
		"no log trap [LEVEL]",
		NO_STR
		"Logging control\n"
		"Permit all logging information\n"
		"Logging level\n")
{
	zlog_default->default_lvl = LOG_DEBUG;
	return CMD_SUCCESS;
}

DEFUN (config_log_record_priority,
		config_log_record_priority_cmd,
		"log record-priority",
		"Logging control\n"
		"Log the priority of the message within the message\n")
{
	zlog_default->record_priority = 1;
	return CMD_SUCCESS;
}

DEFUN (no_config_log_record_priority,
		no_config_log_record_priority_cmd,
		"no log record-priority",
		NO_STR
		"Logging control\n"
		"Do not log the priority of the message within the message\n")
{
	zlog_default->record_priority = 0;
	return CMD_SUCCESS;
}

DEFUN (config_log_timestamp_precision,
		config_log_timestamp_precision_cmd,
		"log timestamp precision <0-6>",
		"Logging control\n"
		"Timestamp configuration\n"
		"Set the timestamp precision\n"
		"Number of subsecond digits\n")
{
	if (argc != 1) {
		vty_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER_RANGE("Timestamp Precision",
			zlog_default->timestamp_precision, argv[0], 0, 6);
	return CMD_SUCCESS;
}

DEFUN (no_config_log_timestamp_precision,
		no_config_log_timestamp_precision_cmd,
		"no log timestamp precision",
		NO_STR
		"Logging control\n"
		"Timestamp configuration\n"
		"Reset the timestamp precision to the default value of 0\n")
{
	zlog_default->timestamp_precision = 0;
	return CMD_SUCCESS;
}

DEFUN (banner_motd_file,
		banner_motd_file_cmd,
		"banner motd file [FILE]",
		"Set banner\n"
		"Banner for motd\n"
		"Banner from a file\n"
		"Filename\n")
{
	if (host.motdfile)
		XFREE (MTYPE_HOST, host.motdfile);
	host.motdfile = XSTRDUP (MTYPE_HOST, argv[0]);

	return CMD_SUCCESS;
}

DEFUN (banner_motd_default,
		banner_motd_default_cmd,
		"banner motd default",
		"Set banner string\n"
		"Strings for motd\n"
		"Default string\n")
{
	host.motd = default_motd;
	return CMD_SUCCESS;
}

DEFUN (no_banner_motd,
		no_banner_motd_cmd,
		"no banner motd",
		NO_STR
		"Set banner string\n"
		"Strings for motd\n")
{
	host.motd = NULL;
	if (host.motdfile)
		XFREE (MTYPE_HOST, host.motdfile);
	host.motdfile = NULL;
	return CMD_SUCCESS;
}


/*****************************************************************
    Function:func_host_info_show_current
    Description:show vty mode enable config 
    Author:huangmingjian
    Date:2012/8/23
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_host_info_show_current(struct vty *vty)
{
    host_init_t ht_info;
	int ret = IPC_STATUS_OK;
	ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
	char val[MAX_CONFIG_VAL_SIZE] = {0};
	
	memset(&ht_info, 0x0, sizeof(host_init_t));
	if(vty->cfgdomain==0)
	{
		ret = ipc_get_host_info(&ht_info);
		if (IPC_STATUS_OK != ret)
	    {
	        vty_out(vty, " ERROR: get vty host info failed! %s",VTY_NEWLINE);
	        return CMD_WARNING;        
	    } 
		
		if(0 != strcmp(ht_info.ht_name, DEFAULT_VAL_HOSTNAME))
		{	
			vty_out(vty, " hostname %s%s",ht_info.ht_name, VTY_NEWLINE);
			vty_out(vty, "!%s", VTY_NEWLINE);
		}
		
		if(0 != strcmp(ht_info.ht_super, DEFAULT_VAL_SUPER))
		{	
			/*Begin add by huangmingjian 2012/09/25 for EPN104QID0046*/ 
			if(vty->node != VIEW_NODE)
			{	vty_out(vty, " super password %s%s",ht_info.ht_super, VTY_NEWLINE);
				vty_out(vty, "!%s", VTY_NEWLINE);
			}
			/*End add by huangmingjian 2012/09/25 for EPN104QID0046 */
		}
	}
	else 
	{
		
		if((cfg_getval(ifindex, CONFIG_SYSTEM_NAME, (void *)val, "", sizeof(val))) < 0)
		{
		 
		  return CMD_WARNING;
		}	
		if(0 != strcmp(val, DEFAULT_VAL_HOSTNAME))
		{	
			vty_out(vty, " hostname %s%s",val, VTY_NEWLINE);
			vty_out(vty, "!%s", VTY_NEWLINE);
		}
		if((cfg_getval(ifindex, CONFIG_SUPER_PASSWORD, (void *)val, "", sizeof(val))) < 0)
		{		 
		  return CMD_WARNING;
		}
		if(0 != strcmp(val, DEFAULT_VAL_SUPER))
		{			
			if(vty->node != VIEW_NODE)
			{	vty_out(vty, " super password %s%s",val, VTY_NEWLINE);
				vty_out(vty, "!%s", VTY_NEWLINE);
			}			
		}
	}
	return CMD_SUCCESS;
}


/*****************************************************************
    Function:func_telnet_service_show_current
    Description: func to show current info about telnet service
    Author:liaohongjun
    Date:2012/8/20
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_telnet_service_show_current(struct vty *vty)
{
    char service[MAX_CONFIG_VAL_SIZE] = {0};
	ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
	if(vty->cfgdomain==0)
	{
    	ipc_get_telnet_service (service);
	}
	else 
	{
		 
		 cfg_getval(ifindex,CONFIG_TELNET_SERVICE_ENABLE,service,"",sizeof(service));
		 
	}
    if(strcmp(DEFAULT_TELNET_SERVICE_STR, service))
    {
        vty_out(vty, " telnet service %s %s",service, VTY_NEWLINE);
        vty_out(vty, "!%s", VTY_NEWLINE);
    }
    return CMD_SUCCESS;
}
/*****************************************************************
    Function:func_manage_vlan_show_current
    Description: func to show current info about management vlan
    Author:liaohongjun
    Date:2012/8/20
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_manage_vlan_show_current(struct vty *vty)
{
    sys_mvlan_t info;
    int iRet = IPC_STATUS_FAIL;    
	ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
	char val[MAX_CONFIG_VAL_SIZE] = {0};
	
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    
    memset(&info, 0, sizeof(sys_mvlan_t));
    //iRet = ipc_get_sys_networking(&info);//disable by zhouguanhua 2013/6/3
    if(vty->cfgdomain==0)
	{
	    iRet = get_sys_mvlan_value(&info);
	    if(IPC_STATUS_OK != iRet)    
	    {
	        vty_out(vty, " ERROR: get system mvlan failed!%s", VTY_NEWLINE);
	        return CMD_WARNING;        
	    }
	    
	    if(DEFAULT_MANAGEMENT_VLAN != info.m_vlan)
	    {
	        vty_out(vty, " management-vlan %d %s", info.m_vlan, VTY_NEWLINE);
	        vty_out(vty, "!%s", VTY_NEWLINE);
	    }
	}
	else
	{
		if((cfg_getval(ifindex, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val)))!=0)
		{
			return CMD_WARNING;  
		}
		if(strcmp(DEFAULT_MANAGEMENT_VLAN_STR,val))
	    {
	        vty_out(vty, " management-vlan %s %s", val, VTY_NEWLINE);
	        vty_out(vty, "!%s", VTY_NEWLINE);
	    }
	}
    
    return CMD_SUCCESS;
}

#ifdef CONFIG_LOOPD
/*****************************************************************
    Function:func_loopback_show_current
    Description: func to show current info about management vlan
    Author:huangmingjian
    Date:2012/11/28
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_loopback_show_current(struct vty *vty)
{	
    sys_loop_t info;
    int iRet = IPC_STATUS_FAIL;    	
	ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
	char val[MAX_CONFIG_VAL_SIZE] = {0};
	
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    if(vty->cfgdomain==0)
	{
	    memset(&info, 0, sizeof(sys_loop_t));
	    //iRet = ipc_get_sys_networking(&info);//disable by zhouguanhua 2013/6/5
	    iRet = ipc_get_sys_loop(&info);
	    
	    if(IPC_STATUS_OK != iRet)    
	    {
	        vty_out(vty, " ERROR: get system loop failed!%s", VTY_NEWLINE);
	        return CMD_WARNING;        
	    }
	    
	    if(DEFAULT_LOOPBACK_ENABLE != info.loopback_enable)
	    {
	        vty_out(vty, " loopback-detection enable %s", VTY_NEWLINE);
	        vty_out(vty, "!%s", VTY_NEWLINE);
	    }
	}
	else
	{
		if((cfg_getval(ifindex, CONFIG_LOOP_ENABLED, (void *)val, "", sizeof(val)))!=0)
		{
			return CMD_WARNING;  
		}
		if(ENABLE == strtoul(val, NULL, 0))
	    {
	        vty_out(vty, " loopback-detection enable %s", VTY_NEWLINE);
	        vty_out(vty, "!%s", VTY_NEWLINE);
	    }
	}
    return CMD_SUCCESS;
}

#endif
/*****************************************************************
    Function:func_syslog_show_current
    Description: func to show current info about syslog
    Author:zhouguanhua
    Date:2013/6/28
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_syslog_show_current(struct vty *vty)
{	
    sys_syslog_t info;
    int iRet = IPC_STATUS_FAIL;    	
	
	char val[MAX_CONFIG_VAL_SIZE] = {0};
	struct in_addr  ip;
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    
    memset(&info, 0, sizeof(sys_syslog_t));
    iRet = get_syslog_value(&info,vty->cfgdomain);   
 

    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: get system log failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    
    if(strtol(DEFAULT_SYSLOG_ENABLE_STR,NULL,0) != info.syslog_en)
    {
        vty_out(vty, " logging enable %s", VTY_NEWLINE);
    }
    
    if(strtol(DEFAULT_SYSLOG_DEGREE_STR,NULL,0) != info.syslog_degree)
    {
        vty_out(vty, " logging send messages severity is %d %s", info.syslog_degree,VTY_NEWLINE);
    }
    
    if(strtol(DEFAULT_SYSLOG_SAVEDEGREE_STR,NULL,0) != info.syslog_save_degree)
    {
        vty_out(vty, " logging buffered severity is %d %s",info.syslog_save_degree, VTY_NEWLINE);
    }

    if(strcmp(DEFAULT_SYSLOG_IP_STR,info.syslog_ip))
    {
        vty_out(vty, " logging host ip %s %s", info.syslog_ip,VTY_NEWLINE);
    }
     vty_out(vty, "!%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}
/*****************************************************************
    Function:func_enable_node_show_current
    Description: 显示enable node 下所有当前配置，各模块
                      需要自己实现接口供本接口调用
    Author:liaohongjun
    Date:2012/8/20
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_enable_node_show_current(struct vty *vty)
{
    int iRet = CMD_WARNING;

    if(NULL == vty)
    {
        return CMD_WARNING;
    }

    /*current config of telnet service */
    iRet = func_telnet_service_show_current(vty);
	
    return iRet;
}

/*****************************************************************
    Function:func_config_node_show_current
    Description: 显示configure node 下所有当前配置，各模块
                      需要自己实现接口供本接口调用
    Author:linguobin
    Date:2013/10/22
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_config_node_show_current(struct vty *vty)
{
    int iRet = CMD_WARNING;

    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    iRet = func_manage_vlan_show_current(vty);
#ifdef CONFIG_LOOPD	
    iRet |= func_loopback_show_current(vty); 
#endif
	iRet |= func_syslog_show_current(vty);
    return iRet;
}


/*****************************************************************
    Function:func_sysconfig_node_show_current
    Description: display the current configuration of  sysconfig node
    Author:huangmingjian
    Date:2012/09/21
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/

int func_sysconfig_node_show_current(struct vty *vty)
{
	int iRet = CMD_WARNING;

    if(NULL == vty)
    {
        return CMD_WARNING;
    }
	 /*current config of host info */
	iRet = func_host_info_show_current(vty);
    return iRet;
}


/*****************************************************************
    Function:func_vlan_interface_node_show_current
    Description: 显示vlan interface node 下当前配置
    Author:liaohongjun
    Date:2012/8/20
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:add of EPN104QID0019
*****************************************************************/
int func_vlan_interface_node_show_current(struct vty *vty)
{
    int iRet = IPC_STATUS_OK;
    sys_network_t network_info;
    sys_mvlan_t mvlan_info;
	char mvlan[64];
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    
    memset(&network_info, 0x0, sizeof(network_info));
	//iRet = ipc_get_sys_networking(&network_info);//disable by zhouguanhua 2013/6/3
    iRet =get_sys_networking_value(&network_info,vty->cfgdomain);
    
	if(IPC_STATUS_OK != iRet)    
	{
		vty_out(vty, " ERROR: get system networking failed!%s", VTY_NEWLINE);
		return CMD_WARNING;        
    }	
	/*begin modifed by wanghuanyu for 228*/
    #if 0
    /*begin modify by zhouguanhuya 2013/6/4*/   
    if(IPC_STATUS_OK != get_sys_mvlan_value(&mvlan_info))
    {
		vty_out(vty, " ERROR: get system vlan_interface failed!%s", VTY_NEWLINE);
		return CMD_WARNING;          
    }
    vty_out(vty, "interface vlan-interface %d %s",mvlan_info.vlan_interface, VTY_NEWLINE);
    /*end begin modify by zhouguanhuya 2013/6/4*/
    #endif
	if(vty->cfgdomain==0)
	{
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)mvlan, "", sizeof(mvlan))) < 0)
	    {
	        DBG_ASSERT(0, "Get mvlan id  faild");
	       // invalid ++;
	    }
	}
	else
	{
		if((cfg_getval(IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0), CONFIG_MVLAN_VID, (void *)mvlan, "", sizeof(mvlan))) < 0)
	    {
	        DBG_ASSERT(0, "Get mvlan id  faild");
	       // invalid ++;
	    }
	}
	/*end modifed by wanghuanyu for 228*/
	vty_out(vty, "interface vlan-interface %s %s",mvlan, VTY_NEWLINE);	
	   
	
   	
    if(!strcmp(network_info.interface_state, INTERFACE_STATE_DOWN))
    {
        vty_out(vty, " shutdown %s", VTY_NEWLINE);
    }
   
    if(IP_PROTO_DYNAMIC == network_info.ip_proto)
    {
        vty_out(vty, " ip dynamic %s", VTY_NEWLINE);
    }
    else if(IP_PROTO_STATIC == network_info.ip_proto)
    {
        if((strcmp(DEFAULT_MANAGEIP, inet_ntoa(network_info.netif.ip)) || strcmp(DEFAULT_MANAGEMASK, inet_ntoa(network_info.netif.subnet))) && \
            (!strcmp(DEFAULT_MANAGEGW, inet_ntoa(network_info.netif.gateway)) || (0 == network_info.netif.gateway.s_addr)))
        {
            vty_out(vty, " ip address %s",inet_ntoa(network_info.netif.ip));            
            vty_out(vty, " %s %s", inet_ntoa(network_info.netif.subnet), VTY_NEWLINE);
        }
        else 
        {
            if((0 != network_info.netif.gateway.s_addr)&& (strcmp(DEFAULT_MANAGEGW, inet_ntoa(network_info.netif.gateway))))
            {
                vty_out(vty, " ip address %s",inet_ntoa(network_info.netif.ip));  
                vty_out(vty, " %s",inet_ntoa(network_info.netif.subnet));
                vty_out(vty, " %s %s",inet_ntoa(network_info.netif.gateway),VTY_NEWLINE);
            }
        }   
    }
    
    vty_out(vty, "!%s", VTY_NEWLINE);       
    return CMD_SUCCESS;
}

/*****************************************************************
    Function:func_snmp_node_show_current
    Description: 显示snmp node 下当前配置
    Author:linguobin
    Date:2013/10/23
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:add of EPN104QID0019
*****************************************************************/
int func_snmp_node_show_current(struct vty *vty)
{
    int iRet = IPC_STATUS_OK;
    sys_snmp_t sys;
    int i = 0;
	char val[1024]; 
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    iRet = ipc_get_sys_snmp_value(&sys,vty->cfgdomain);
    if(IPC_STATUS_OK != iRet)    
    {
        vty_out(vty, " ERROR: get snmp info failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    if(ENABLE == sys.snmp_en)
    {
	    vty_out(vty, "Snmp enable %s", VTY_NEWLINE);
    }
    if(strcmp(sys.snmp_engineID,DEFAULT_SNMP_ENGINE_ID))
    {
        vty_out(vty, "Snmp engine ID: %s %s",sys.snmp_engineID, VTY_NEWLINE);
    }
    if(DEFAULT_SNMP_MAX_PACKET_LEN != sys.snmp_max_packet_len)
    {
        vty_out(vty, "Snmp max packet length: %d %s",sys.snmp_max_packet_len, VTY_NEWLINE);	
    }
    if(strcmp(sys.snmp_location,DEFAULT_SNMP_LOCATION))
    {
        vty_out(vty, "Snmp location: %s %s",sys.snmp_location, VTY_NEWLINE);	
    }
    if(strcmp(sys.snmp_contact,DEFAULT_SNMP_CONTACT))
    {
        vty_out(vty, "Snmp contact: %s %s",sys.snmp_contact, VTY_NEWLINE);
    }
    if(SNMP_V2c_VAL != sys.snmp_version)
    {
        if(SNMP_V1_VAL == sys.snmp_version)
        {
            vty_out(vty, "Snmp version: %s %s","v1", VTY_NEWLINE);	
        }
        else if(SNMP_V3_VAL == sys.snmp_version)
        {
            vty_out(vty, "Snmp version: %s %s","v3", VTY_NEWLINE);	
        }
    }
    if(strcmp(sys.snmp_trust_host,"default"))
    {
        vty_out(vty, "Snmp trust host: %s %s",sys.snmp_trust_host, VTY_NEWLINE);
    }
    for(i=0; i< MAX_SNMP_COMM_COUNT; i++)
    {
        if(strcmp(sys.snmp_comm[i].snmp_comm_name,""))
        {
            vty_out(vty, "Snmp community %d : %s",i+1, VTY_NEWLINE);

            vty_out(vty, "name %s %s",sys.snmp_comm[i].snmp_comm_name, VTY_NEWLINE);
            if(SNMP_ACESS_MODE_RW == sys.snmp_comm[i].snmp_access_mode)
    		{
    			vty_out(vty, "AccessMode %s %s","read-write", VTY_NEWLINE);
    		}
    		else if(SNMP_ACESS_MODE_RO == sys.snmp_comm[i].snmp_access_mode)
    		{
    			vty_out(vty, "AccessMode %s %s","read-only", VTY_NEWLINE);
    		}
        }
    }
    vty_out(vty, "!%s", VTY_NEWLINE);  
    if(ENABLE != sys.snmp_trap_en)
    {
        vty_out(vty, "Snmp trap disable%s", VTY_NEWLINE);
    }
    if(strcmp(inet_ntoa(sys.snmp_trap_ip),DEFAULT_TRAP_IP))
    {
        vty_out(vty, "Snmp trap ip: %s %s",inet_ntoa(sys.snmp_trap_ip), VTY_NEWLINE);
    }
    if(DEFAULT_UDP_PORT != sys.snmp_trap_com)
    {
        vty_out(vty, "Snmp trap udpport: %d %s",sys.snmp_trap_com, VTY_NEWLINE);
    }
    if(2 != sys.snmp_trap_version)
    {
        if(1 == sys.snmp_trap_version)
        {
            vty_out(vty, "Snmp trap version: %s %s","v1", VTY_NEWLINE);	
        }
        else if(3 == sys.snmp_trap_version)
        {
            vty_out(vty, "Snmp trap version: %s %s","v3", VTY_NEWLINE);	
        }
    }
    if(strcmp(sys.snmp_trap_community,"public"))
    {
        vty_out(vty, "Snmp trap Security Name: %s %s",sys.snmp_trap_community, VTY_NEWLINE);
    }
    if(3 == sys.snmp_trap_version)
    {
        if(0 == sys.snmp_trap_safe)
        {
            vty_out(vty, "Snmp trap Security level: noauthnopriv %s", VTY_NEWLINE);
        }
        else if(1 == sys.snmp_trap_safe)
        {
            vty_out(vty, "Snmp trap Security level: authnopriv %s", VTY_NEWLINE);
        }
        else if(2 == sys.snmp_trap_safe)
        {
            vty_out(vty, "Snmp trap Security level: authpriv %s", VTY_NEWLINE);
        }
    }
    vty_out(vty, "!%s", VTY_NEWLINE); 

     
    if(SNMP_V3_VAL == sys.snmp_version)
    {       
        for (i = 0; i < MAX_SNMP_GROUP_COUNT; i++)
        {
            if(strcmp(sys.snmp_group[i].snmp_group_name,""))
            {
                vty_out(vty, "Group (%d) for v3%s",i+1, VTY_NEWLINE);
                if(0 == sys.snmp_group[i].snmp_group_security)
                {
                    vty_out(vty, "Name %s  | SecurityLevel %s  | AccessMode %s%s", sys.snmp_group[i].snmp_group_name,"noauthnopriv",sys.snmp_group[i].snmp_group_mode == 0?"read-only":"read-write", VTY_NEWLINE); 
                }
                else if(1 == sys.snmp_group[i].snmp_group_security)
                {
                    vty_out(vty, "Name %s  | SecurityLevel %s  | AccessMode %s%s", sys.snmp_group[i].snmp_group_name,"authnopriv",sys.snmp_group[i].snmp_group_mode == 0?"read-only":"read-write", VTY_NEWLINE); 
                }
                else if(2 == sys.snmp_group[i].snmp_group_security)
                {
                    vty_out(vty, "Name %s  | SecurityLevel %s  | AccessMode %s%s", sys.snmp_group[i].snmp_group_name,"authpriv",sys.snmp_group[i].snmp_group_mode == 0?"read-only":"read-write", VTY_NEWLINE); 
                }
            }
        } 
        vty_out(vty, "!%s", VTY_NEWLINE); 
        for (i = 0; i < MAX_SNMP_USER_COUNT; i++)
        {
            if(strcmp(sys.snmp_user[i].snmp_user_name,""))
            {
                vty_out(vty, "user (%d) for v3%s",i+1, VTY_NEWLINE);
                if(0 == sys.snmp_user[i].snmp_user_security)
                {
                    vty_out(vty, "GroupName %s  | UserName %s | SecurityLevel %s | AuthMode %s | PrivMode %s%s", sys.snmp_user[i].snmp_user_group,sys.snmp_user[i].snmp_user_name,"noauthnopriv","MD5","DE856", VTY_NEWLINE); 
                }
                else if(1 == sys.snmp_user[i].snmp_user_security)
                {
                    vty_out(vty, "GroupName %s  | UserName %s | SecurityLevel %s | AuthMode %s | PrivMode %s%s", sys.snmp_user[i].snmp_user_group,sys.snmp_user[i].snmp_user_name,"authnopriv","MD5","DE856", VTY_NEWLINE);  
                }
                else if(2 == sys.snmp_user[i].snmp_user_security)
                {
                    vty_out(vty, "GroupName %s  | UserName %s | SecurityLevel %s | AuthMode %s | PrivMode %s%s", sys.snmp_user[i].snmp_user_group,sys.snmp_user[i].snmp_user_name,"authpriv","MD5","DE856", VTY_NEWLINE); 
                }
            }
        } 
        vty_out(vty, "!%s", VTY_NEWLINE); 
    }
    return CMD_SUCCESS;
}

#if 1
/*****************************************************************
    Function:show_running_config
    Description: 显示系统所有当前配置信息统一接口,
                    其余各模块需要自己实现显示当前配置
                    信息接口
    Author:liaohongjun
    Date:2012/8/20
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/

DEFUN (show_running_config,
        show_running_config_cmd,
        "show running-config",
        SHOW_STR
        "Running configuration\n")
{
    int iRet = CMD_WARNING;
    #if 1
    vty_out(vty, "!%s", VTY_NEWLINE);
	vty->cfgdomain=0;
    /*enable node show current*/
    iRet = func_enable_node_show_current(vty);
    /*begin added by linguobin 2013/10/23*/
    iRet |= func_config_node_show_current(vty);
    /*end added by linguobin 2013/10/23*/

	/*Begin add by huangmingjian 2012/08/21 for EPN104QID0046*/
	/*sysconfig node show current*/
	iRet |= func_sysconfig_node_show_current(vty);
	/*End add by huangmingjian 2012/08/21 for EPN104QID0046*/
	
    /*vty node show current*/
    iRet |= func_vty_node_show_current(vty);

    /*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
    /*vlan interface node show current*/
    iRet |= func_vlan_interface_node_show_current(vty); 
    /*end added by liaohongjun 2012/9/18 of EPN104QID0019*/

    /*begin added by linguobin 2013/10/23*/
    /*snmp node show current*/
    iRet |= func_snmp_node_show_current(vty); 
    /*end added by linguobin 2013/10/23*/
    vty_out(vty, "end%s", VTY_NEWLINE);
	#endif
    return iRet;
}
#endif
#if 1
DEFUN (show_startup_config,
        show_startup_config_cmd,
        "show startup-config",
        SHOW_DESC_STR
        "Contentes of startup configuration\n")
{
    int iRet = CMD_WARNING;
	
//	int iRet = CMD_WARNING;
	#if 1
    vty_out(vty, "!%s", VTY_NEWLINE);
	ipc_show_startupstart();
    vty->cfgdomain=1;
    /*enable node show current*/
    iRet = func_enable_node_show_current(vty);
    
    /*begin added by linguobin 2013/10/23*/
    iRet |= func_config_node_show_current(vty);
    /*end added by linguobin 2013/10/23*/
    
    /*Begin add by huangmingjian 2012/08/21 for EPN104QID0046*/
    /*sysconfig node show current*/
    iRet |= func_sysconfig_node_show_current(vty);
    /*End add by huangmingjian 2012/08/21 for EPN104QID0046*/
    
    /*vty node show current*/
    iRet |= func_vty_node_show_current(vty);
    /*startup config of vlan interface node*/
    iRet |= func_vlan_interface_node_show_current(vty);
	/*begin added by linguobin 2013/10/23*/
    /*snmp node show current*/
    iRet |= func_snmp_node_show_current(vty); 
    /*end added by linguobin 2013/10/23*/
	ipc_show_startupend();
	vty_out(vty, "end%s", VTY_NEWLINE);
	
	vty->cfgdomain=0;
	#endif
    return iRet;
}
#endif
/*****************************************************************
    Function:show_current
    Description: 各模式下的show current命令在此接口下统一
                       调用
    Author:liaohongjun
    Date:2012/8/20
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/

DEFUN (show_current,
		show_current_cmd,
		"show current",
		SHOW_DESC_STR
		CURRENT_DESC_STR)
{
    int iRet = CMD_WARNING;
    
    vty_out(vty, "!%s", VTY_NEWLINE);
    switch(vty->node)
    {
        /*each function remember to output a ' !' before return*/
        case VIEW_NODE:
            break;   
        case ENABLE_NODE:
            iRet = func_enable_node_show_current(vty);
            break;
         /*begin added by linguobin 2013/10/22*/ 
        case CONFIG_NODE:
            iRet = func_config_node_show_current(vty);
            break; 
         /*end added by linguobin 2013/10/22*/  
        case SYSCONFIG_NODE:
			iRet = func_sysconfig_node_show_current(vty);/*Add by huangmingjian 2012/08/21 for EPN104QID0046*/
            break;             
        case VTY_NODE:
            iRet = func_vty_node_show_current(vty);
            break;
        /*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/    
        case VLAN_INTERFACE_NODE:
            iRet = func_vlan_interface_node_show_current(vty);
            break;
        /*end added by liaohongjun 2012/9/18 of EPN104QID0019*/    
        default:
            vty_out(vty, " Unknow vty node!%s", VTY_NEWLINE);
            break;
    }
    
    vty_out(vty, "end%s", VTY_NEWLINE);
    return iRet;
}

/*****************************************************************
    Function:telnet_service_enable
    Description: func to enable or disable telnet service 
    Author:liaohongjun
    Date:2012/8/20
    Input:   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (telnet_service_enable,
		telnet_service_enable_cmd,
		"telnet service (enable|disable)",
		"Set telnet service function\n"
		"Telnet service\n"
		"Enable function\n"
		"Disable function\n")
{
    int i =0;
    struct vty *vty_tmp = NULL;
    /*begin modified by liaohongjun 2012/9/3 of QID0015*/
    int iRet = IPC_STATUS_OK;
    char val[BUF_SIZE_64] = {0};

    if(strstr(ENABLE_STR, (char *)argv[0]))
    {
        sprintf(val, "%s", ENABLE_STR);
    }
    else if(strstr(DISABLE_STR, (char *)argv[0])) 
    {
        sprintf(val, "%s", DISABLE_STR);
    }
    else
    {
        vty_out(vty, " ERROR: set telnet service paras error!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    iRet = ipc_cfg_set_telnetService("telnet_service_enabled", val);
    if (IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: set telnet service failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    
    if(strstr(DISABLE_STR, (char *)argv[0])) 
    { 
        for (i = 0; i < vector_active (vtyvec); i++)
        {
            if ((vty_tmp = vector_slot (vtyvec, i)) != NULL)
            {
                if(strcmp(vty_tmp->address, "Console"))
                {
                    vty_tmp->status = VTY_CLOSE;
                    /* cannot call vty_close, because a parent routine may still try to 
                                access the vty struct */                    
                    shutdown(vty_tmp->fd, SHUT_RDWR);
                    /*end modified by liaohongjun 2012/9/3 of QID0015*/
                }
            }
        }
    } 

    return CMD_SUCCESS;
}
/*****************************************************************
    Function:config_qos_trustmode
    Description:config_qos_trustmode_cmd
    Author:feihuaxin
    Date:2013/7/15
    Input:     
    Output:         
    Return:
    Note:
*****************************************************************/

DEFUN (config_qos_trustmode,
		config_qos_trustmode_cmd,
		"qos-trustmode (cos|dscp)",
		"Configure system trusting the packet priority\n"
		"Trust 802.1p mode\n"
		"Trust DSCP mode\n")
{
    int iRet = IPC_STATUS_OK;
    int req = 0;
    char val[BUF_SIZE_64] = {0};
	ipc_sys_qos_ack_t *pack = NULL;


    if(strstr(QOS_COS_STR, (char *)argv[0]))
    {
        req = 0;
    }
    else if(strstr(QOS_DSCP_STR, (char *)argv[0])) 
    {
        req = 1;
    }
    else
    {
        vty_out(vty, " ERROR: set qos trustmode paras error!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	pack = ipc_qos_trustmode(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, req);
    if (pack->hdr.status != IPC_STATUS_OK)
    {
    	if(pack)
        {
    	    free(pack);
    	}
        vty_out(vty, " ERROR: set set qos trustmode failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

	if(pack)
    {
	    free(pack);
	}
    return CMD_SUCCESS;
}
/*****************************************************************
    Function:config_qos_queuemode
    Description:config_qos_queuemode_cmd
    Author:feihuaxin
    Date:2013/7/15
    Input:     
    Output:         
    Return:
    Note:
*****************************************************************/
    DEFUN (config_qos_queuemode,
            config_qos_queuemode_cmd,
            "qos-queuemode (hq-wrr|wrr) <1-31> <1-31> <1-31> <1-31> ",
            "Specify queue scheduling mode and parameters\n"
            "Specify scheduling mode as HQ_WRR\n"
            "Specify scheduling mode as weighted round robin\n"
            "Weight of 1st queue\n"
            "Weight of 2nd queue\n"
            "Weight of 3rd queue\n"
            "Weight of 4th queue\n"
            )

{
    QueueMode_S req;
	ipc_sys_qosqueuemod_ack_t *pack = NULL;
    memset(&req, 0, sizeof(req));

    if(strstr((char *)argv[0], QOS_HQ_WRR_STR))
    {
        req.QueueMode = 1;
    }
    else if(strstr((char *)argv[0], QOS_WRR_STR))
    {
        req.QueueMode = 0;
    }
    else
    {
        vty_out(vty, " ERROR: set qos queuemode paras error!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    req.Weight[0] = strtoul(argv[1], NULL, 0);
    req.Weight[1] = strtoul(argv[2], NULL, 0);
    req.Weight[2] = strtoul(argv[3], NULL, 0);
    req.Weight[3] = strtoul(argv[4], NULL, 0);
    
	pack = ipc_qos_queuemode(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &req);
    if (pack->hdr.status != IPC_STATUS_OK)
    {
    	if(pack)
        {
    	    free(pack);
    	}
        vty_out(vty, " ERROR: set set qos trustmode failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

	if(pack)
    {
	    free(pack);
	}

    return CMD_SUCCESS;
}

DEFUN (show_qos_trustmode,
          show_qos_trustmode_cmd,
          "show qos-trustmode",
          SHOW_STR
          "Priority trust mode\n")
{
    char qosmode = 0;
	int ret = 0;
	ipc_sys_qos_ack_t *pack = NULL;

    
	pack = ipc_qos_trustmode(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        qosmode = pack->uiQosTrustmode;
		ret = IPC_STATUS_OK;
	}
    else if (pack)
    {
		ret = pack->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(pack) 
    {   
        free(pack);     
    }
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get QOS mode faild");
        return CMD_WARNING;
    }
    
    vty_out(vty, "  qos trustmode:%s%s",qosmode ? QOS_DSCP_STR : QOS_COS_STR ,VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN (show_qos_queuemode,
          show_qos_queuemode_cmd,
          "show qos-queuemode",
          SHOW_STR
          "Queue scheduling configuration information\n")
{
	int ret = 0;
    QueueMode_S qos_queuemode;
    ipc_sys_qosqueuemod_ack_t *rvpkt = NULL;

    memset(&qos_queuemode, 0, sizeof(QueueMode_S));

  	rvpkt = ipc_qos_queuemode(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (rvpkt && rvpkt->hdr.status == IPC_STATUS_OK) 
    {
        qos_queuemode.QueueMode = rvpkt->Qosqueuemode.QueueMode;
        qos_queuemode.Weight[0] = rvpkt->Qosqueuemode.Weight[0];
        qos_queuemode.Weight[1] = rvpkt->Qosqueuemode.Weight[1];
        qos_queuemode.Weight[2] = rvpkt->Qosqueuemode.Weight[2];
        qos_queuemode.Weight[3] = rvpkt->Qosqueuemode.Weight[3];
		ret = IPC_STATUS_OK;
	}
    else if (rvpkt)
    {
		ret = rvpkt->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(rvpkt) 
    {   
        free(rvpkt);     
    }
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get QOS queuemod faild");
        return CMD_SUCCESS;
    }  

    vty_out(vty, "  qos queue mode: %s%s",qos_queuemode.QueueMode ? QOS_HQ_WRR_STR : QOS_WRR_STR,VTY_NEWLINE);
    vty_out(vty, "  weight of queue 1: %d%s",qos_queuemode.Weight[0] ,VTY_NEWLINE);    
    vty_out(vty, "  weight of queue 2: %d%s",qos_queuemode.Weight[1] ,VTY_NEWLINE);
    vty_out(vty, "  weight of queue 3: %d%s",qos_queuemode.Weight[2] ,VTY_NEWLINE);
    vty_out(vty, "  weight of queue 4: %d%s",qos_queuemode.Weight[3] ,VTY_NEWLINE);
	return CMD_SUCCESS;
}

DEFUN (no_qos_queuemode,
		  no_qos_queuemode_cmd,
		  "no qos-queuemode",
		  NO_STR
		  "Specify queue scheduling mode and parameters\n")
{
    QueueMode_S req;
	ipc_sys_qosqueuemod_ack_t *pack = NULL;
    memset(&req, 0, sizeof(req));

    req.QueueMode = 0;
    req.Weight[0] = 1;
    req.Weight[1] = 2;
    req.Weight[2] = 4;
    req.Weight[3] = 8;
    
	pack = ipc_qos_queuemode(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &req);
    if (pack->hdr.status != IPC_STATUS_OK)
    {
    	if(pack)
        {
    	    free(pack);
    	}
        vty_out(vty, " ERROR: set set qos trustmode failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

	if(pack)
    {
	    free(pack);
	}

    return CMD_SUCCESS;
 
}


/* Set config filename.  Called from vty.c */
void host_config_set(char *filename) {
	if (host.config)
		XFREE (MTYPE_HOST, host.config);
	host.config = XSTRDUP (MTYPE_HOST, filename);
}

void install_default(enum node_type node) {
	install_element(node, &config_quit_cmd);
	install_element(node, &config_list_cmd);
    install_element(node, &config_who_cmd); 
	install_element(node, &show_history_cmd);
    install_element(node, &show_version_cmd);
	install_element(node, &show_startup_config_cmd);
	install_element(node, &show_running_config_cmd);	
	install_element(node, &show_current_cmd);
    install_element(node, &save_cmd);
#ifdef CONFIG_LOOPD	
	install_element(node, &show_loopback_detection_cmd);
#endif
    /*begin modified by liaohongjun 2012/11/19 of EPN204QID0004*/
    if(VIEW_NODE != node)
    {
        install_element(node, &config_end_cmd);
    }
    /*end modified by liaohongjun 2012/11/19 of EPN204QID0004*/
    /*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
    install_element(node, &show_ip_interface_cmd);
    install_element(node, &show_vlan_interface_cmd); 
    /*end added by liaohongjun 2012/9/18 of EPN104QID0019*/
#ifdef SWITCH_STYLE	
    install_element(node, &show_qos_trustmode_cmd); 
    install_element(node, &show_qos_queuemode_cmd); 
#endif	
	//add for loid
	extern struct cmd_element show_epon_loid_cmd;
	install_element(node, &show_epon_loid_cmd);
    #if (CMD_CURRENT_SUPPORT)
    install_element(node, &config_help_cmd);//unsupport of EPN204QID0003 by liaohongjun 2012/11/19
	install_element(node, &show_system_info_cmd);
	install_element(node, &show_system_networking_cmd);    
	install_element(node, &show_system_security_cmd);    
	install_element(node, &config_exit_cmd);    
    install_element(node, &show_address_cmd);
   // install_element(node, &show_thread_cpu_cmd); /*Modified by huangmingjian 2012/09/26 for EPN104QID0046*/    
	install_element(node, &config_write_terminal_cmd);
	install_element(node, &config_write_file_cmd);
	install_element(node, &config_write_memory_cmd);
	install_element(node, &config_write_cmd);
	#endif
}

/* Initialize command interface. Install basic nodes and commands. */
void cmd_init(int terminal) {
	command_cr = XSTRDUP(MTYPE_STRVEC, "<cr>");
	desc_cr.cmd = command_cr;
	desc_cr.str = XSTRDUP(MTYPE_STRVEC, "");

	/* Allocate initial top vector of commands. */
	cmdvec = vector_init(VECTOR_MIN_SIZE);
	
    /* Default host value settings. */
	host_init();
	/* Install top nodes. */
	install_node(&view_node, NULL);
    /*begin added by liaohongjun 2012/9/5 of QID0015*/
    install_node(&enter_node, NULL);
    /*end added by liaohongjun 2012/9/5 of QID0015*/
	install_node(&enable_node, NULL);
	install_node(&auth_node, NULL);
	install_node(&auth_passwd_node, NULL);
	install_node(&auth_enable_node, NULL);
	install_node(&restricted_node, NULL);
	install_node(&config_node, config_write_host);

	/* Each node's basic commands. */

    /*install VIEW_NODE commands*/
	if (terminal) {
        install_default(VIEW_NODE);
		install_element(VIEW_NODE, &config_enable_cmd);
       // install_element(VIEW_NODE, &show_thread_cpu_cmd);/*Modified by huangmingjian 2012/09/26 for EPN104QID0046*/
        install_element(VIEW_NODE, &reboot_cmd);
        install_element(VIEW_NODE, &debug_shell_cmd);
		
		/*Begin add by huangmingjian 2013/03/14 for EPN204QID0033*/
		#ifdef CONFIG_CATVCOM
		install_element(VIEW_NODE, &ontest_cmd);
		install_element(VIEW_NODE, &offtest_cmd);
	   	install_element(VIEW_NODE, &onrevfirst_cmd);
	   	install_element(VIEW_NODE, &offrevfirst_cmd);
		#endif
		/*End add by huangmingjian 2013/03/14 for EPN204QID0033*/ 
		
        #if (CMD_CURRENT_SUPPORT)
    	install_element(VIEW_NODE, &debug_cmd);
    	install_element(VIEW_NODE, &no_debug_cmd);        
        install_element(VIEW_NODE, &echo_cmd);
		install_element(VIEW_NODE, &config_terminal_length_cmd);
		install_element(VIEW_NODE, &config_terminal_no_length_cmd);
		install_element(VIEW_NODE, &show_logging_cmd);	
        install_element(VIEW_NODE, &show_work_queues_cmd);
	    #endif
	}

    /*install RESTRICTED_NODE commands*/
	if (terminal) {    
            /* -- disabled by Einsn*/
        #if (CMD_CURRENT_SUPPORT)
        install_element(RESTRICTED_NODE, &config_list_cmd);
        install_element(RESTRICTED_NODE, &config_exit_cmd);
        install_element(RESTRICTED_NODE, &config_quit_cmd);
        install_element(RESTRICTED_NODE, &config_help_cmd);
        install_element(RESTRICTED_NODE, &config_enable_cmd);
        install_element(RESTRICTED_NODE, &config_terminal_length_cmd);
        install_element(RESTRICTED_NODE, &config_terminal_no_length_cmd);
        install_element(RESTRICTED_NODE, &echo_cmd);
        //install_element(RESTRICTED_NODE, &show_thread_cpu_cmd);/*Modified by huangmingjian 2012/09/26 for EPN104QID0046*/
        #endif
	}

    /*install ENABLE_NODE commands*/
	if (terminal) {
		install_default(ENABLE_NODE);      
        install_element(ENABLE_NODE, &telnet_service_enable_cmd);
		//install_element(ENABLE_NODE, &show_thread_cpu_cmd);  /*Modified by huangmingjian 2012/09/26 for EPN104QID0046*/      
       // install_element(ENABLE_NODE, &config_qos_queuemode_cmd);
     //   install_element(ENABLE_NODE, &config_qos_trustmode_cmd);
        
		install_element(ENABLE_NODE, &config_disable_cmd);
		install_element(ENABLE_NODE, &config_terminal_cmd);   
        /* -- disabled by Einsn*/
        #if (CMD_CURRENT_SUPPORT)

		     
        install_element(ENABLE_NODE, &echo_cmd);
		install_element(ENABLE_NODE, &copy_runningconfig_startupconfig_cmd);
		install_element(ENABLE_NODE, &config_terminal_length_cmd);
		install_element(ENABLE_NODE, &config_terminal_no_length_cmd);
		install_element(ENABLE_NODE, &show_logging_cmd);  
		install_element(ENABLE_NODE, &config_logmsg_cmd);
        install_element(ENABLE_NODE, &show_work_queues_cmd);
		#endif
	}

    /*install CONFIG_NODE commands*/        
	if (terminal) {
        /* -- disabled by liaohongjun*/
		install_default(CONFIG_NODE);
	#ifdef SWITCH_STYLE	
		install_element(CONFIG_NODE, &config_qos_queuemode_cmd);
        install_element(CONFIG_NODE, &config_qos_trustmode_cmd);
		install_element(CONFIG_NODE, &no_qos_queuemode_cmd);
	#endif	
        #if (CMD_CURRENT_SUPPORT)
      
    	install_element(CONFIG_NODE, &password_text_cmd);
		install_element(CONFIG_NODE, &config_log_stdout_cmd);
		install_element(CONFIG_NODE, &config_log_stdout_level_cmd);
		install_element(CONFIG_NODE, &no_config_log_stdout_cmd);
		install_element(CONFIG_NODE, &config_log_monitor_cmd);
		install_element(CONFIG_NODE, &config_log_monitor_level_cmd);
		install_element(CONFIG_NODE, &no_config_log_monitor_cmd);
		install_element(CONFIG_NODE, &config_log_file_cmd);
		install_element(CONFIG_NODE, &config_log_file_level_cmd);
		install_element(CONFIG_NODE, &no_config_log_file_cmd);
		install_element(CONFIG_NODE, &no_config_log_file_level_cmd);
		install_element(CONFIG_NODE, &config_log_syslog_cmd);
		install_element(CONFIG_NODE, &config_log_syslog_level_cmd);
		install_element(CONFIG_NODE, &config_log_syslog_facility_cmd);
		install_element(CONFIG_NODE, &no_config_log_syslog_cmd);
		install_element(CONFIG_NODE, &no_config_log_syslog_facility_cmd);
		install_element(CONFIG_NODE, &config_log_facility_cmd);
		install_element(CONFIG_NODE, &no_config_log_facility_cmd);
		install_element(CONFIG_NODE, &config_log_trap_cmd);
		install_element(CONFIG_NODE, &no_config_log_trap_cmd);
		install_element(CONFIG_NODE, &config_log_record_priority_cmd);
		install_element(CONFIG_NODE, &no_config_log_record_priority_cmd);
		install_element(CONFIG_NODE, &config_log_timestamp_precision_cmd);
		install_element(CONFIG_NODE, &no_config_log_timestamp_precision_cmd);
		install_element(CONFIG_NODE, &service_password_encrypt_cmd);
		install_element(CONFIG_NODE, &no_service_password_encrypt_cmd);
		install_element(CONFIG_NODE, &banner_motd_default_cmd);
		install_element(CONFIG_NODE, &banner_motd_file_cmd);
		install_element(CONFIG_NODE, &no_banner_motd_cmd);
		install_element(CONFIG_NODE, &service_terminal_length_cmd);
		install_element(CONFIG_NODE, &no_service_terminal_length_cmd);        
        #endif
	}
    
	srand(time(NULL));
}


/**********************************************************
**  Copyright (C) 2012-7-11, LOSOTECH Co. Ltd.
**  All rights reserved.
**  
**  FileName:       command.c
** Function:         host_init
**  Description:    initialize the host,include username,password and enable password
**  Author:           huangmingjian
**  Date:             2012/8/23
**  Others:          
**********************************************************/

void host_init (void)
{
	int ret;
	host.logfile = NULL;
	host.config = NULL;
	host.lines = -1;
	host.motd = default_motd;
	host.motdfile = NULL;

	ret = ipc_get_host_info(&ht_init);
	host.username = NULL;
	host.password = NULL;
	host.name = XSTRDUP(MTYPE_HOST, ht_init.ht_name);
	host.enable = XSTRDUP(MTYPE_HOST, ht_init.ht_super);	
	return;
	
}


void cmd_terminate() {
	unsigned int i, j, k, l;
	struct cmd_node *cmd_node;
	struct cmd_element *cmd_element;
	struct desc *desc;
	vector cmd_node_v, cmd_element_v, desc_v;

	if (cmdvec) {
		for (i = 0; i < vector_active (cmdvec); i++)
			if ((cmd_node = vector_slot (cmdvec, i)) != NULL) {
				cmd_node_v = cmd_node->cmd_vector;

				for (j = 0; j < vector_active (cmd_node_v); j++)
					if ((cmd_element = vector_slot (cmd_node_v, j)) != NULL
							&& cmd_element->strvec != NULL) {
						cmd_element_v = cmd_element->strvec;

						for (k = 0; k < vector_active (cmd_element_v); k++)
							if ((desc_v = vector_slot (cmd_element_v, k))
									!= NULL) {
								for (l = 0; l < vector_active (desc_v); l++)
									if ((desc = vector_slot (desc_v, l))
											!= NULL) {
										if (desc->cmd)
											XFREE (MTYPE_STRVEC, desc->cmd);
										if (desc->str)
											XFREE (MTYPE_STRVEC, desc->str);

										XFREE (MTYPE_DESC, desc);
									}
								vector_free(desc_v);
							}

						cmd_element->strvec = NULL;
						vector_free(cmd_element_v);
					}

				vector_free(cmd_node_v);
			}

		vector_free(cmdvec);
		cmdvec = NULL;
	}

	if (command_cr)
		XFREE(MTYPE_STRVEC, command_cr);
	if (desc_cr.str)
		XFREE(MTYPE_STRVEC, desc_cr.str);
	if (host.name)
		XFREE (MTYPE_HOST, host.name);
//	if (host.password)
//		XFREE (MTYPE_HOST, host.password);
	if (host.password_encrypt)
		XFREE (MTYPE_HOST, host.password_encrypt);
//	if (host.enable)
//		XFREE (MTYPE_HOST, host.enable);
	if (host.enable_encrypt)
		XFREE (MTYPE_HOST, host.enable_encrypt);
	if (host.logfile)
		XFREE (MTYPE_HOST, host.logfile);
	if (host.motdfile)
		XFREE (MTYPE_HOST, host.motdfile);
	if (host.config)
		XFREE (MTYPE_HOST, host.config);
}
/*
int if_arg_decode2(char *arg, int *s1, int *s2)
{
	char *cp, *p1, *p2;
	int i;

	p1 = strdup(arg);
	cp = strchr(p1, '/');
	if(!cp) {free(p1);return -1;}
	*cp = 0;
	p2 = cp + 1;
	if( (strlen(p1) <= 0) || (strlen(p2) <= 0) ) { free(p1); return -1; };
	for(i = 0; i < strlen(p1); i ++) if(!isdigit(p1[i])) { free(p1); return -1; }
	for(i = 0; i < strlen(p2); i ++) if(!isdigit(p2[i])) { free(p1); return -1; }
	*s1 = strtoul(p1, NULL, 0);
	*s2 = strtoul(p2, NULL, 0);
	free(p1);
	return 0;
}

int if_arg_decode3(char *arg, int *s1, int *s2, int *s3)
{
	char *cp, *p1, *p2, *p3;
	int i;

	p1 = arg;
	cp = strchr(arg, '/');
	if(!cp) return -1;
	*cp = 0;
	p2 = cp + 1;
	cp = strchr(p2, '/');
	if(!cp) return -1;
	*cp = 0;
	p3 = cp + 1;
	if( (strlen(p1) <= 0) || (strlen(p2) <= 0) || (strlen(p3) <= 0)) return -1;
	for(i = 0; i < strlen(p1); i ++) if(!isdigit(p1[i])) return -1;
	for(i = 0; i < strlen(p2); i ++) if(!isdigit(p2[i])) return -1;
	for(i = 0; i < strlen(p3); i ++) if(!isdigit(p3[i])) return -1;
	*s1 = strtoul(p1, NULL, 0);
	*s2 = strtoul(p2, NULL, 0);
	*s3 = strtoul(p3, NULL, 0);
	return 0;
}
*/

/*
 * shutils.h
 *
 *  Created on: Feb 18, 2011
 *      Author: root
 */
#include <lw_type.h>
#include <systemlog.h>

#ifndef SHUTILS_H_
#define SHUTILS_H_

#define  READ_BUF_SIZE 256

extern int _eval(char *const argv[], char *path, int timeout, pid_t *ppid);
extern pid_t get_pid_by_name(char *pname);
extern UINT32 sys_os_get_uptime(void);
extern CHAR *version_string(void);
extern UINT32 version_count(void);
extern CHAR *version_date(void);
extern int replace_char(char *src, int old, int new); //add by leijinbao 2013/8/26

/* Simple version of _eval() (no timeout and wait for child termination) */
#define eval(cmd, args...) ({ \
	char *argv[] = { cmd , ## args, NULL }; \
	_eval(argv, ">/dev/console", 0, NULL); \
})

/* Copy each token in wordlist delimited by space into word */
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

#endif /* SHUTILS_H_ */

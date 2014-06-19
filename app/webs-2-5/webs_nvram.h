/*
 * asp_nvram.h
 *
 *  Created on: Jan 10, 2011
 *      Author: root
 */

#ifndef ASP_NVRAM_H_
#define ASP_NVRAM_H_

int webs_nvram_init(void);

char *webs_nvram_get(char *name);

#define webs_nvram_safe_get(name) (webs_nvram_get(name) ? : "")

int webs_nvram_set(char *name, char *value);

int webs_nvram_unset(char *name);

int webs_nvram_commit(void);

#endif /* ASP_NVRAM_H_ */

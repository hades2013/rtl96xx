/*
 * NVRAM variable manipulation
 *
 * Copyright 2007, Broadcom Corporation
 * Copyright 2009, OpenWrt.org
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef _nvram_h_
#define _nvram_h_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "sdinitvals.h"

#define NVRAM_DEBUG_ON 0
#if (NVRAM_DEBUG_ON)
#define	NVRAM_DBG(fmt, ...) do { fprintf(stderr, "NVRAM_DBG [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#else
#define NVRAM_DBG(fmt, ...)
#endif

struct nvram_header {
	uint32_t magic;
	uint32_t len;
	uint32_t crc_ver_init;	/* 0:7 crc, 8:15 ver, 16:31 sdram_init */
	uint32_t config_refresh;	/* 0:15 sdram_config, 16:31 sdram_refresh */
	uint32_t config_ncdl;	/* ncdl values for memc */
} __attribute__((__packed__));

struct nvram_tuple {
	char *name;
	char *value;
	struct nvram_tuple *next;
};

struct nvram_handle {
	int fd;
	char *mmap;
	int has_change; // 1 means some not committed updates 
	unsigned int length;
	unsigned int offset;
	struct nvram_tuple *nvram_hash[257];
	struct nvram_tuple *nvram_dead;
};

typedef struct nvram_handle nvram_handle_t;
typedef struct nvram_header nvram_header_t;
typedef struct nvram_tuple  nvram_tuple_t;


/* Get nvram header. */
nvram_header_t * nvram_header(nvram_handle_t *h);

/* Set the value of an NVRAM variable */
int nvram_set(nvram_handle_t *h, const char *name, const char *value);

/* Get the value of an NVRAM variable. */
char * nvram_get(nvram_handle_t *h, const char *name);

/* Unset the value of an NVRAM variable. */
int nvram_unset(nvram_handle_t *h, const char *name);

/* Get all NVRAM variables. */
nvram_tuple_t * nvram_getall(nvram_handle_t *h);

/* Regenerate NVRAM. */
int nvram_commit(nvram_handle_t *h);

/* Open NVRAM and obtain a handle. */
nvram_handle_t * nvram_open(const char *file, int rdonly);
/* remalloc nvram_handle. */
nvram_handle_t *nvram_handle_remalloc(const char *file, int rdonly);

/* Close NVRAM and free memory. */
int nvram_close(nvram_handle_t *h);

/* Get the value of an NVRAM variable in a safe way, use "" instead of NULL. */
#define nvram_safe_get(h, name) (nvram_get(h, name) ? : "")

/* Computes a crc8 over the input data. */
uint8_t hndcrc8 (uint8_t * pdata, uint32_t nbytes, uint8_t crc);

/* Returns the crc value of the nvram. */
uint8_t nvram_calc_crc(nvram_header_t * nvh);

/* Determine NVRAM device node. */
char * nvram_find_mtd(void);

/* Copy NVRAM contents to staging file. */
/*begin modified by huangmingjian 2012-08-15 for EPN104QID0005*/
int nvram_to_staging(const char *file);
/*end modified by huangmingjian 2012-08-15 for EPN104QID0005*/


/* Copy staging file to NVRAM device. */
int staging_to_nvram(void);

/* Check NVRAM staging file. */
/*begin modified by huangmingjian 2012-08-15 for EPN104QID0005*/
char * nvram_find_staging(const char *file);
/*end modified by huangmingjian 2012-08-15 for EPN104QID0005*/

/*begin modified by huangmingjian 2012-08-17 for EPN104QID0006*/
nvram_handle_t * nvram_open_rdonly(const char *paths);
/*end modified by huangmingjian 2012-08-17 for EPN104QID0006*/

/*begin modified by huangmingjian 2012-08-15 for EPN104QID0005*/
nvram_handle_t * nvram_open_staging(const char *file);
/*end modified by huangmingjian 2012-08-15 for EPN104QID0005*/


/* Staging file for NVRAM */
#define NVRAM_STAGING		"/tmp/.nvram"

/*begin modified by huangmingjian 2012-08-15 for EPN104QID0005*/
#define NVRAM_STAGING_TMP		"/tmp/.nvram_tmp"
/*end modified by huangmingjian 2012-08-15 for EPN104QID0005*/


#define NVRAM_RO			1
#define NVRAM_RW			0

/* Helper macros */
#define NVRAM_ARRAYSIZE(a)	sizeof(a)/sizeof(a[0])
#define	NVRAM_ROUNDUP(x, y)	((((x)+((y)-1))/(y))*(y))

/* NVRAM constants */
/*begin modified by liaohongjun 2012/6/25*/
//#define NVRAM_SPACE			(0x20000-sizeof(nvram_header_t))
#define NVRAM_SPACE			0x20000
/*end modified by liaohongjun 2012/6/25*/

#define NVRAM_MAGIC			0x48534C46	/* 'FLSH' */
#define NVRAM_VERSION		1

#define NVRAM_CRC_START_POSITION	9 /* magic, len, crc8 to be skipped */

#define	NVRAM_NAME_SIZE		80
#define	NVRAM_VALUE_SIZE	256
#define	NVRAM_LVALUE_SIZE	(3 * 4096)

#endif /* _nvram_h_ */

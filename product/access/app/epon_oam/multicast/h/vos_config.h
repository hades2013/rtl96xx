/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_config.h
*
* DESCRIPTION: 
*	
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_config.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_CONFIG_H_
#define __VOS_CONFIG_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_types.h>
/*--------------------------Macro definition------------------------- */

#define OP_VOS_CONFIG_STR_MAX       128 /* Max len of "NAME=VALUE\0" */

#define FLASH_DEV_NAME_CFG          "/dev/mtdcfg"
#define FLASH_DEV_NAME_OS1          "/dev/mtdk0"
#define FLASH_DEV_NAME_OS2          "/dev/mtdk1"

enum {
    FLASH_BOOT_FLAG_NORMAL,
    FLASH_BOOT_FLAG_UPGRADE,
    FLASH_BOOT_FLAG_MAX
};

enum {
    FLASH_BOOT_FLAG_NORMAL_OFFSET,
    FLASH_BOOT_FLAG_UPGRADE_OFFSET,
    FLASH_CONFIG_FILE_LEN_OFFSET,
    FLASH_CONFIG_FILE_CHSUM_OFFSET = 10
};

/* for FLASH_BOOT_FLAG_UPGRADE, we use FLASH_BOOT_OS2_FLAG to indicate 
   that the os upgrade by OAM
*/

#define FLASH_BOOT_OS1_FLAG         '1'
#define FLASH_BOOT_OS2_FLAG         '2'

/* Configuration File Format Definition
 * 
 *   |-+-+--------+--------+---....-|
 *
 *      1. NORMAL BOOT FLAG (1 byte)
 *      2. UPGRADE BOOT FLAG (1 byte)
 *      3. Config File Length (8 bytes)
 *      4. Config File Checksum (8 Bytes)
 *      5. Config File Data
 */

int vosConfigGetFromFlash(char *toFileName);

int  vosConfigInit(void);
int  vosConfigDestroy(void);

int  vosConfigBootFlagSet(uint8 offset, uint8 value);
uint8  vosConfigBootFlagGet(uint8 offset);

int  vosConfigFileLengthSet(uint32 value);
uint32  vosConfigFileLengthGet();

int  vosConfigFileCheckSumSet(uint32 value);
uint32  vosConfigFileCheckSumGet();

int vosConfigSave(char *fname);
int vosConfigExport(char *fname);
int vosConfigErase();

int  vosConfigRefresh(void);
int  vosConfigRefreshByModule(char *fname);

int  vosConfigFlush(void);
int  vosConfigFlushByModule(char *fname);

void vosConfigShowByKey(char *fname, char *section, char*key, int fd);
void vosConfigShowBySection(char *fname, char *section, int fd);
void vosConfigShowByModule(char *fname, int fd);
void vosConfigShow(int fd);

char *vosConfigValueGet(
    char *fname, 
    const char *section,
    char *key, 
    char *default_value);
int vosConfigIntegerGet(
    char *fname, 
    char *section,
    char *key, 
    int default_value);
uint32 vosConfigUInt32Get(
    char *fname, 
    char *section,
    char *key,
    uint32 default_value);

int vosConfigValueSet(
    char *fname, 
    const char *section,
    char *key, 
    const char *value);
int vosConfigIntegerSet(
    char *fname, 
    char *section,
    char *key, 
    int value);
int vosConfigUInt32Set(
    char *fname, 
    char *section,
    char *key,
    uint32 value);

int vosConfigModuleDelete(char *fname);
int vosConfigSectionDelete(char *fname, const char *section);
int vosConfigKeyDelete(
    char *fname, 
    char *section,
    char *key);

int vosConfigModuleCount();
int vosConfigSectionCount(char *fname);
int vosConfigKeyCount(char *fname, 
                                       char *section);

int vosConfigSectionCreate(
    char *fname, 
    const char *section);
int vosConfigModuleIsExisted(char *fname);
int vosConfigSectionIsExisted(
    char *fname, 
    const char *section);
int vosConfigKeyIsExisted(
    char *fname, 
    const char *section,
    char *key);

int vosConfigSectionGetByIndex(char *fname, int index, char **section);
int vosConfigKeyGetByIndex(
    char *fname, 
    char *section,
    int index,
    char **key,
    char **value);
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_CONFIG_H_ */
 

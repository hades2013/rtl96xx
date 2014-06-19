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
* FILENAME:  vos_config.c
*
* DESCRIPTION: 
*	
*
* Date Created: Nov 03, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_config.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>

#include <vos_alloc.h>
#include <vos_types.h>
#include <vos_config.h>
#include <vos_io.h>
#include <vos_hash.h>
#include <vos_macros.h>
#include <vos_thread.h>
#include <vos_sync.h>
#include <vos.h>


/*--------------------------Macro definition------------------------- */
#define CFG_NOT_FLUSHED   0
#define CFG_FLUSHED       1

#define CONFIG_TMP_FILE1 "/tmp/tmpConfig1%08X"
#define CONFIG_TMP_FILE2 "/tmp/tmpConfig2%08X"
#define CONFIG_TMP_FILE3 "/tmp/tmpConfig3%08X"
#define CONFIG_TMP_FILE4 "/tmp/tmpConfig4%08X"
#define CONFIG_TMP_FILE5 "/tmp/tmpConfig5%08X"


/*-----------------global varible/function declaration--------------- */

typedef struct CFG_KEY_DATA_s {
    uint32      flag; /* Used for duplicate checking and flushing */
    char        *data;
} CFG_KEY_DATA_t;

typedef struct CFG_SECTION_DATA_s {
    uint32      flag; /* Used for duplicate checking and flushing */
    HASH_TABLE_t   *keys;
} CFG_SECTION_DATA_t;

typedef struct CFG_MODULE_DATA_s {
    HASH_TABLE_t   *sections;
} CFG_MODULE_DATA_t;

HASH_TABLE_t * g_pstConfigDatabase;
char * g_pcConfigDefaultPath = "/cfg";
char * g_pcConfigFileExt = "*.conf";

static VOS_MUTEX_t g_pstConfigLock;
static VOS_MUTEX_t g_pstSaveEraseLock;



/*--------------local  variable declaration and definition------------- */

static void vosConfigModuleDestroy(void *key, void *value);
static void vosConfigSectionDestroy(void *key, void *value);
static void vosConfigKeyDestroy(void *key, void *value);
static char *trim(const char *str);
static int vosConfigFlushKeyReset(void * key,void * value);
static int vosConfigFlushSectionReset(void * key,void * value);
static uint32 vosConfigFlushKeyAppend(void * key,void * value, FILE *stream);
static uint32 vosConfigFlushSectionAppend(void * key,void * value, FILE *stream);
static int vosConfigFlushForEachModule(void * key,void * value);
static int vosConfigShowForEachKey(void * key,void * value, int fd);
static int vosConfigShowForEachSection(void * key,void * value, int fd);
static int vosConfigShowForEachModule(void * key,void * value, int fd);


/*----------------------local  function definition--------------------- */


/*******************************************************************************
*
* vosConfigReadFromFlash:
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	toFileName - the file name to write to
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigReadFromFlash(char *toFileName)
{
    FILE *pFile;
    char *pcConfigData = NULL;
    long nReadBytes,nWroteBytes;
    long ulFileLen, ulFileChsum;
    char acString[128], acCommand[128];

    vosMutexTake(&g_pstSaveEraseLock);

    nReadBytes = vosFileRead(FLASH_DEV_NAME_CFG, &pcConfigData);

    ulFileLen = 0;
    if (nReadBytes > (FLASH_CONFIG_FILE_LEN_OFFSET + 8))
    {
        vosMemCpy(&acString[2], &pcConfigData[FLASH_CONFIG_FILE_LEN_OFFSET], 8); 
        acString[0] = '0';
        acString[1] = 'x';
        acString[10] = 0;
        ulFileLen = vosStrToUInt(acString, NULL);
    }

    ulFileChsum = 0;
    if (nReadBytes > (FLASH_CONFIG_FILE_CHSUM_OFFSET + 8))
    {
        vosMemCpy(&acString[2], &pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET], 8); 
        acString[0] = '0';
        acString[1] = 'x';
        acString[10] = 0;
        ulFileChsum = vosStrToUInt(acString, NULL);
    }

    if (((ulFileLen + FLASH_CONFIG_FILE_CHSUM_OFFSET + 8) <= nReadBytes) && 
        (OK == vosCheckCRC(&pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8], ulFileLen, ulFileChsum)))
    {
        if (toFileName)
        {
            nWroteBytes = vosFileWrite(toFileName, &pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8], ulFileLen);
            if (ulFileLen != nWroteBytes)
            {
                /* remove temp file */
                vosSprintf(acCommand, "rm -f %s", toFileName);
                vosSystem(acCommand);
                goto exit_label;
            }
        }
        else {
            nWroteBytes = vosFileWrite("/cfg.tar.gz", &pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8], ulFileLen);
            if (ulFileLen != nWroteBytes)
            {
                /* remove temp file */
                vosSystem("rm -f /cfg.tar.gz");
                goto exit_label;
            }

            vosSystem("cd /");
            vosSystem("tar zxf cfg.tar.gz");
            vosSystem("rm -f /cfg.tar.gz");
        }
    }
    else {
        printf("vosConfigReadFromFlash: invalid file length or CRC error\r\n");
    }

exit_label:
    vosFree(pcConfigData);
    vosMutexGive(&g_pstSaveEraseLock);
    return 0;
}

/*******************************************************************************
*
* vosConfigInit:	 If not already initialized, call refresh.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigInit(void)
{
    VOS_DIR_t * pDir;
    FILE *pFile;
    char *pcConfigData = NULL;
    long nReadBytes,nWroteBytes;
    long ulFileLen, ulFileChsum;
    char acString[128], acCommand[128];

    if (VOS_MUTEX_OK != vosMutexCreate(&g_pstConfigLock))
    { 
        printf("vosConfigInit: create config mutex failed.\r\n");
        return -1;
    } 

    if (VOS_MUTEX_OK != vosMutexCreate(&g_pstSaveEraseLock))
    { 
        printf("vosConfigInit: create save and erase mutex failed.\r\n");
        return -1;
    } 

    pDir = vosOpenDir(g_pcConfigDefaultPath);
    if (pDir == NULL)
    {
        vosMakeDir(g_pcConfigDefaultPath);
    }
    else
    {
        vosCloseDir(pDir);
    }

    nReadBytes = vosFileRead(FLASH_DEV_NAME_CFG, &pcConfigData);

    /* check upgrade option */
    if (nReadBytes > FLASH_BOOT_FLAG_MAX)
    {
        if (FLASH_BOOT_OS2_FLAG == pcConfigData[FLASH_BOOT_FLAG_UPGRADE])
        {
            /* restore the NORMAL BOOT option */
            if (FLASH_BOOT_OS2_FLAG == pcConfigData[FLASH_BOOT_FLAG_NORMAL])
            {
        		pcConfigData[FLASH_BOOT_FLAG_NORMAL] = FLASH_BOOT_OS1_FLAG;
            }
            else {
        		pcConfigData[FLASH_BOOT_FLAG_NORMAL] = FLASH_BOOT_OS2_FLAG;
            }
            /* cleanup the UPGRADE BOOT option */
            pcConfigData[FLASH_BOOT_FLAG_UPGRADE] = FLASH_BOOT_OS1_FLAG;
            
            vosSprintf(acString, CONFIG_TMP_FILE1, (int)vosThreadSelf());
            nWroteBytes = vosFileWrite(acString, pcConfigData, nReadBytes);
            if (nReadBytes != nWroteBytes)
            {
                /* remove temp file */
                vosSprintf(acCommand, "rm -f %s", acString);
                vosSystem(acCommand);
                goto exit_label;
            }

            /* write file to MTD */
            vosSprintf(acCommand, "mtd write %s %s", acString, FLASH_DEV_NAME_CFG);
            vosSystem(acCommand);

            /* remove temp file */
            vosSprintf(acCommand, "rm -f %s", acString);
            vosSystem(acCommand);
        }
    }

    /* if boot flag not be init or boot flag value is invalid, initialize boot flag  value */
    if (nReadBytes <= FLASH_BOOT_FLAG_MAX || FLASH_BOOT_OS1_FLAG != pcConfigData[FLASH_BOOT_FLAG_UPGRADE])
    {
        /* set default boot flag and upgrade value */
        vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
        vosConfigBootFlagSet(FLASH_BOOT_FLAG_UPGRADE, FLASH_BOOT_OS1_FLAG);
    }

    ulFileLen = 0;
    if (nReadBytes > (FLASH_CONFIG_FILE_LEN_OFFSET + 8))
    {
        vosMemCpy(&acString[2], &pcConfigData[FLASH_CONFIG_FILE_LEN_OFFSET], 8); 
        acString[0] = '0';
        acString[1] = 'x';
        acString[10] = 0;
        ulFileLen = vosStrToUInt(acString, NULL);
    }

    ulFileChsum = 0;
    if (nReadBytes > (FLASH_CONFIG_FILE_CHSUM_OFFSET + 8))
    {
        vosMemCpy(&acString[2], &pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET], 8); 
        acString[0] = '0';
        acString[1] = 'x';
        acString[10] = 0;
        ulFileChsum = vosStrToUInt(acString, NULL);
    }

    if (((ulFileLen + FLASH_CONFIG_FILE_CHSUM_OFFSET + 8) <= nReadBytes) && 
        (OK == vosCheckCRC(&pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8], ulFileLen, ulFileChsum)))
    {
        /* begin modifieded by liaohongjun of QID0017*/
    	VOS_DBG("Load config data from flash ... \n");
        /* end modifieded by liaohongjun of QID0017*/
        nWroteBytes = vosFileWrite("/cfg.tar.gz", 
            &pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8], ulFileLen);
        if (ulFileLen != nWroteBytes)
        {
            /* remove temp file */
            vosSystem("rm -f /cfg.tar.gz");
            goto exit_label;
        }

        vosSystem("cd /");
        vosSystem("tar zxf cfg.tar.gz");
        vosSystem("rm -f /cfg.tar.gz");
    }
    else { /* load default configuration */
		/* begin modifieded by liaohongjun of QID0017*/
    	VOS_DBG("Load config data from default configuration ... \n");
        /* PON */
        //vosSystem("cd /");
        //vosSystem("cp /etc/cfg.tar.gz /");
        //vosSystem("tar zxf cfg.tar.gz");
        //vosSystem("rm -f /cfg.tar.gz");
        /* end modifieded by liaohongjun of QID0017*/
    	}

exit_label:
    if (!g_pstConfigDatabase) {
        g_pstConfigDatabase = vosHashCreate(
            vosStringHash, 
            vosEqualString, 
            vosConfigModuleDestroy, 
            0);
        vosFree(pcConfigData);
        return vosConfigRefresh();
    }

    vosFree(pcConfigData);
    return 0;
}

static void vosConfigModuleDestroy(void *key, void *value)
{
    char *k = key;
    CFG_MODULE_DATA_t *v = value;
    vosFree(k);
    vosHashFree(v->sections);
    vosFree(v);
}

static void vosConfigSectionDestroy(void *key, void *value)
{
    char *k = key;
    CFG_SECTION_DATA_t *v = value;
    vosFree(k);
    vosHashFree(v->keys);
    vosFree(v);
}

static void vosConfigKeyDestroy(void *key, void *value)
{
    char *k = key;
    CFG_KEY_DATA_t *v = value;
    vosFree(k);
    vosFree(v->data);
    vosFree(v);
}

/*******************************************************************************
*
* vosConfigDestroy:	 destroy config database, but dont delete config files.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigDestroy(void)
{
    if (g_pstConfigDatabase) {
        vosHashFree(g_pstConfigDatabase);
    }

    return 0;
}

int  vosConfigBootFlagSet(uint8 offset, uint8 value)
{
    FILE *pFile;
    char *pcConfigData = NULL;
    long nReadBytes,nWroteBytes;
    char acString[128], acCommand[128];
    int  rv = 0;

    vosMutexTake(&g_pstSaveEraseLock);

    nReadBytes = vosFileRead(FLASH_DEV_NAME_CFG, &pcConfigData);

    if (nReadBytes > offset)
    {
        pcConfigData[offset] = value;
    }
    else {
        rv = -1;
        goto exit_label;
    }

    vosSprintf(acString, CONFIG_TMP_FILE1, (int)vosThreadSelf());
    nWroteBytes = vosFileWrite(acString, pcConfigData, nReadBytes);
    if (nReadBytes != nWroteBytes)
    {
        /* remove temp file */
        vosSprintf(acCommand, "rm -f %s", acString);
        vosSystem(acCommand);
        rv = -1;
        goto exit_label;
    }

    /* write file to MTD */
	#if 0
    vosSprintf(acCommand, "mtd write %s %s", acString, FLASH_DEV_NAME_CFG);
    rv = vosSystem(acCommand);
	#else
	write_flash(acString, FLASH_DEV_NAME_CFG);
	#endif

    /* remove temp file */
    vosSprintf(acCommand, "rm -f %s", acString);
    vosSystem(acCommand);

exit_label:
    vosFree(pcConfigData);
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;    
}

uint8  vosConfigBootFlagGet(uint8 offset)
{
    FILE *pFile;
    int ch, count = 0;
    uint8 rv = INVALID_8;

    vosMutexTake(&g_pstSaveEraseLock);

    pFile = fopen(FLASH_DEV_NAME_CFG, "rb");
    if (pFile == NULL)
    {
        goto exit_label;
    }
    
    while ((ch = fgetc(pFile)) != EOF)
    {
        count++;
        if (count > offset)
        {
            fclose(pFile);
            rv = (uint8)ch;
            goto exit_label;
        }
    }

    fclose(pFile);

exit_label:
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;
}

int  vosConfigFileLengthSet(uint32 value)
{
    FILE *pFile;
    char *pcConfigData = NULL;
    long nReadBytes,nWroteBytes;
    char acString[128], acCommand[128];
    int  rv = 0;

    vosMutexTake(&g_pstSaveEraseLock);

    nReadBytes = vosFileRead(FLASH_DEV_NAME_CFG, &pcConfigData);

    if (nReadBytes > (FLASH_CONFIG_FILE_LEN_OFFSET + 8))
    {
        vosSprintf(&pcConfigData[FLASH_CONFIG_FILE_LEN_OFFSET], "%08X", value);
    }
    else {
        rv = -1;
        goto exit_label;
    }

    vosSprintf(acString, CONFIG_TMP_FILE1, (int)vosThreadSelf());
    nWroteBytes = vosFileWrite(acString, pcConfigData, nReadBytes);
    if (nReadBytes != nWroteBytes)
    {
        /* remove temp file */
        vosSprintf(acCommand, "rm -f %s", acString);
        vosSystem(acCommand);
        rv = -1;
        goto exit_label;
    }

    /* write file to MTD */
    vosSprintf(acCommand, "mtd write %s %s", acString, FLASH_DEV_NAME_CFG);
    rv = vosSystem(acCommand);

    /* remove temp file */
    vosSprintf(acCommand, "rm -f %s", acString);
    vosSystem(acCommand);

exit_label:
    vosFree(pcConfigData);
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;    
}

uint32 vosConfigFileLengthGet()
{
    FILE *pFile;
    int ch, count = 0;
    char buf[FLASH_CONFIG_FILE_LEN_OFFSET + 8 + 1] = {0};
    uint32 rv = 0;

    vosMutexTake(&g_pstSaveEraseLock);

    pFile = fopen(FLASH_DEV_NAME_CFG, "rb");
    if (pFile == NULL)
    {
        goto exit_label;
    }
    
    while ((ch = fgetc(pFile)) != EOF)
    {
        count++;
        if (count > (FLASH_CONFIG_FILE_LEN_OFFSET + 8))
        {
            buf[count - 1] = 0;
            buf[FLASH_CONFIG_FILE_LEN_OFFSET-2] = '0';
            buf[FLASH_CONFIG_FILE_LEN_OFFSET-1] = 'x';
            fclose(pFile);
            rv = vosStrToUInt(&buf[FLASH_CONFIG_FILE_LEN_OFFSET-2], NULL);
            goto exit_label;
        }
        buf[count - 1] = (char)ch;
    }

    fclose(pFile);

exit_label:
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;
}


int  vosConfigFileCheckSumSet(uint32 value)
{
    FILE *pFile;
    char *pcConfigData = NULL;
    long nReadBytes,nWroteBytes;
    char acString[128], acCommand[128];
    int  rv = 0;

    vosMutexTake(&g_pstSaveEraseLock);

    nReadBytes = vosFileRead(FLASH_DEV_NAME_CFG, &pcConfigData);

    if (nReadBytes > (FLASH_CONFIG_FILE_CHSUM_OFFSET + 8))
    {
        vosSprintf(&pcConfigData[FLASH_CONFIG_FILE_CHSUM_OFFSET], "%08X", value);
    }
    else {
        rv = -1;
        goto exit_label;
    }

    vosSprintf(acString, CONFIG_TMP_FILE1, (int)vosThreadSelf());
    nWroteBytes = vosFileWrite(acString, pcConfigData, nReadBytes);
    if (nReadBytes != nWroteBytes)
    {
        /* remove temp file */
        vosSprintf(acCommand, "rm -f %s", acString);
        vosSystem(acCommand);
        rv = -1;
        goto exit_label;
    }

    /* write file to MTD */
    vosSprintf(acCommand, "mtd write %s %s", acString, FLASH_DEV_NAME_CFG);
    rv = vosSystem(acCommand);

    /* remove temp file */
    vosSprintf(acCommand, "rm -f %s", acString);
    vosSystem(acCommand);

exit_label:
    vosFree(pcConfigData);
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;    
}

uint32 vosConfigFileCheckSumGet()
{
    FILE *pFile;
    int ch, count = 0;
    char buf[FLASH_CONFIG_FILE_CHSUM_OFFSET + 8 + 1] = {0};
    uint32 rv = 0;

    vosMutexTake(&g_pstSaveEraseLock);

    pFile = fopen(FLASH_DEV_NAME_CFG, "rb");
    if (pFile == NULL)
    {
        goto exit_label;
    }
    
    while ((ch = fgetc(pFile)) != EOF)
    {
        count++;
        if (count > (FLASH_CONFIG_FILE_CHSUM_OFFSET + 8))
        {
            buf[count - 1] = 0;
            buf[FLASH_CONFIG_FILE_CHSUM_OFFSET-2] = '0';
            buf[FLASH_CONFIG_FILE_CHSUM_OFFSET-1] = 'x';
            fclose(pFile);
            rv = vosStrToUInt(&buf[FLASH_CONFIG_FILE_CHSUM_OFFSET-2], NULL);
            goto exit_label;
        }
        buf[count - 1] = (char)ch;
    }

    fclose(pFile);

exit_label:
    vosMutexGive(&g_pstSaveEraseLock);
    return rv;
}

/** description: save configruation.
 *  @param 
 *      fname - the config compress file name that need to save
 *  @return less than zero represent error.

 *  @see
 *  @deprecated
 */
int vosConfigSave(char *fname)
{
    char acString[128], acCommand[128];
    uint8 bootFlag, upgradeFlag;
    char *pcConfigData = NULL;
    uint32 fileSize = 0;
    uint32 crc = 0;

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    upgradeFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_UPGRADE);

    vosMutexTake(&g_pstSaveEraseLock);

#if 0  /* modified by Gan Zhiheng - 2009/11/19 */
    vosSystem("mtd erase "FLASH_DEV_NAME_CFG);
#else
    erase_partition(FLASH_DEV_NAME_CFG);
#endif /* #if 0 */

    if (fname)
    {
        fileSize = vosFileRead(fname, &pcConfigData);
        if (fileSize > 0)
        {
            crc = vosCRC32(pcConfigData, fileSize);
        }
        vosFree(pcConfigData);

        vosSprintf(acCommand, "awk 'BEGIN{ printf(\"%c%c%08X%08X\") > \""CONFIG_TMP_FILE2"\";}'", 
            bootFlag, upgradeFlag, fileSize, crc, (int)vosThreadSelf());
        vosSystem(acCommand);

        vosSprintf(acCommand, "cat "CONFIG_TMP_FILE2" %s > "CONFIG_TMP_FILE4, 
            (int)vosThreadSelf(),fname,(int)vosThreadSelf());
        vosSystem(acCommand);
    }
    else {
        vosConfigFlush();

        vosSprintf(acCommand, "tar czf "CONFIG_TMP_FILE3" -C / cfg", (int)vosThreadSelf());
        vosSystem(acCommand);

        vosSprintf(acString, CONFIG_TMP_FILE3, (int)vosThreadSelf());
        fileSize = vosFileRead(acString, &pcConfigData);
        if (fileSize > 0)
        {
            crc = vosCRC32(pcConfigData, fileSize);
        }
        vosFree(pcConfigData);

        vosSprintf(acCommand, "awk 'BEGIN{ printf(\"%c%c%08X%08X\") > \""CONFIG_TMP_FILE2"\";}'", 
            bootFlag, upgradeFlag, fileSize, crc, (int)vosThreadSelf());
        vosSystem(acCommand);

        vosSprintf(acCommand, "cat "CONFIG_TMP_FILE2" "CONFIG_TMP_FILE3" > "CONFIG_TMP_FILE4, 
            (int)vosThreadSelf(),(int)vosThreadSelf(),(int)vosThreadSelf());
        vosSystem(acCommand);
    }

    vosSprintf(acCommand, "mtd write "CONFIG_TMP_FILE4" %s", (int)vosThreadSelf(), FLASH_DEV_NAME_CFG);
    vosSystem(acCommand);

    /* remove temp file */
    vosSprintf(acCommand, "rm -f "CONFIG_TMP_FILE2, (int)vosThreadSelf());
    vosSystem(acCommand);
    vosSprintf(acCommand, "rm -f "CONFIG_TMP_FILE3, (int)vosThreadSelf());
    vosSystem(acCommand);
    vosSprintf(acCommand, "rm -f "CONFIG_TMP_FILE4, (int)vosThreadSelf());
    vosSystem(acCommand);

    vosMutexGive(&g_pstSaveEraseLock);
    return 0;
}

int vosConfigExport(char *fname)
{
    char acCommand[128];

    vosConfigFlush();

    vosSprintf(acCommand, "tar czf %s -C / cfg", fname);
    vosSystem(acCommand);

    return 0;
}


static int vosConfigDeleteForEachModule(void *key, void *value)
{
    if (key) {
        return vosConfigModuleDelete((char*)key);
    }
    else {
        vosPrintk("Input a null pointer to delete file\r\n");
        return -1;
    }
}


/** description: erase all modules' configruation.
 *  @param void
 *  @return less than zero represent error.

 *  @see
 *  @deprecated
 */
int vosConfigErase()
{
    char acString[128], acCommand[128];
    uint8 bootFlag, upgradeFlag;
    char *pcConfigData = NULL;
    uint32 fileSize = 0;
    uint32 crc = 0;

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    upgradeFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_UPGRADE);

    vosMutexTake(&g_pstSaveEraseLock);

    /* backup loid.conf file */
    vosSystem("mv /cfg/loid.conf /tmp/loid.conf");

    /* delete cfg/* files */
#if 0  /* modified by Gan Zhiheng - 2009/11/15 */
    if (g_pstConfigDatabase)
    {
        vosHashForEachDo(g_pstConfigDatabase, vosConfigDeleteForEachModule);
    }
#else
    vosSystem("rm -rf /cfg");
#endif /* #if 0 */

    /* resore loid.conf file */
    vosSystem("mkdir /cfg");
    vosSystem("mv /tmp/loid.conf /cfg/");
#if 0  /* deleted by Gan Zhiheng - 2009/11/16 */
    vosSystem("cp /etc/config.xml /cfg/");
#endif /* #if 0 */

#if 0  /* modified by Gan Zhiheng - 2009/11/19 */
    vosSystem("mtd erase "FLASH_DEV_NAME_CFG);
#else
    erase_partition(FLASH_DEV_NAME_CFG);
#endif /* #if 0 */

    vosSprintf(acCommand, "tar czf "CONFIG_TMP_FILE3" -C / cfg", (int)vosThreadSelf());
    vosSystem(acCommand);

    vosSprintf(acString, CONFIG_TMP_FILE3, (int)vosThreadSelf());
    fileSize = vosFileRead(acString, &pcConfigData);
    if (fileSize > 0)
    {
        crc = vosCRC32(pcConfigData, fileSize);
    }
    vosFree(pcConfigData);

    vosSprintf(acCommand, "awk 'BEGIN{ printf(\"%c%c%08X%08X\") > \""CONFIG_TMP_FILE2"\";}'", 
        bootFlag, upgradeFlag, fileSize, crc, (int)vosThreadSelf());
    vosSystem(acCommand);

    vosSprintf(acCommand, "cat "CONFIG_TMP_FILE2" "CONFIG_TMP_FILE3" > "CONFIG_TMP_FILE4, 
        (int)vosThreadSelf(),(int)vosThreadSelf(),(int)vosThreadSelf());
    vosSystem(acCommand);

    vosSprintf(acCommand, "mtd write "CONFIG_TMP_FILE4" %s", (int)vosThreadSelf(), FLASH_DEV_NAME_CFG);
    vosSystem(acCommand);

    vosMutexGive(&g_pstSaveEraseLock);

    return 0;
}


static char * trim(const char *str)
{
    const char *p = str;
    char *result;
    size_t len;

    assert(p != NULL);

    while (*p == ' ' || *p == '\t')
        p++;

    len = vosStrLen(p) + 1;
    result = (char *)vosAlloc(len);
    result[0] = 0;
    vosSafeStrNCpy(result, p, len);

    while (result[vosStrLen(result) - 1] == ' ' ||
        result[vosStrLen(result) - 1] == '\t' ||
        result[vosStrLen(result) - 1] == '\r') {
        result[vosStrLen(result) - 1] = '\0';
    }

  return result;
}

/*******************************************************************************
*
* vosConfigRefreshByModule:	 refresh memory image from the configuration
*   file for specified module.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fname - the file name of the module, which include full path
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigRefreshByModule(char *fname)
{
    FILE *fp;
    char *line, *p, *section = NULL, *key, *value;
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection = NULL;
    CFG_KEY_DATA_t *tkey;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL) {
        rv = -1;
        goto exit_label;
    }

    if (fname == NULL) {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL) {
        tmodule = (CFG_MODULE_DATA_t *)vosAlloc(sizeof(CFG_MODULE_DATA_t));
        ASSERT(tmodule != NULL, "module is NULL");
        tmodule->sections = vosHashCreate(vosStringHash, vosEqualString, vosConfigSectionDestroy, 0);
        vosHashInsert(vosStrDup(fname), tmodule, g_pstConfigDatabase);
    }

    fp = vosFOpen(fname, "r");
    if (fp == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    while (!feof(fp)) {
        line = vosFGetWholeLine(fp);
        if (line == NULL)
            continue;

        p = line;
        while (*p == ' ' || *p == '\t')
            p++;

        /* Comment line */
        if (*p == '#' || *p == ';') {
            vosFree(line);
            continue;
        }

        /* Section line */
        if (*p == '[') {
            char *quoted = strchr(++p, ']');
            if (quoted != NULL) {
            	char *old_section = section;
            	*quoted = '\0';
            	section = trim(p);
            	if (strlen(section) == 0) {
            	    vosFree(section);
            	    section = old_section;
            	}
            	else {
            	    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
            	    if (tsection == NULL) {
            	        tsection = (CFG_SECTION_DATA_t *)vosAlloc(sizeof(CFG_SECTION_DATA_t));
                        tsection->keys = vosHashCreate(vosStringHash, vosEqualString, vosConfigKeyDestroy, 0);
                        tsection->flag = CFG_NOT_FLUSHED;
            	        vosHashInsert(section, tsection, tmodule->sections);
            	    }
            	}
            }
            vosFree(line);
            continue;
        }

        /* Configuration line */
        if (tsection == NULL) {
            vosFree(line);
            continue;
        }

        value = strchr(p, '=');
        if (value == NULL) {
            vosFree(line);
            continue;
        }

        *value++ = '\0';
        key = trim(p);

        p = value;
        value = trim(p);

        tkey = (CFG_KEY_DATA_t *)vosAlloc(sizeof(CFG_KEY_DATA_t));
        tkey->data = value;
        tkey->flag = CFG_NOT_FLUSHED;

        vosHashInsert(key, tkey, tsection->keys);
        vosFree(line);
    }

    vosFClose(fp);

#ifdef DEBUG
    vosPrintk("file: %s\r\n", fname);
    vosHashForEachDoCb(tmodule->sections,
        vosConfigShowForEachSection, (uint32)STDOUT_FILENO, 4, 5, 6, 7, 8);
#endif

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}


/*******************************************************************************
*
* vosConfigRefresh:	 refresh all configuration image from configuration file
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigRefresh(void)
{

    vosScanDirFiles(g_pcConfigDefaultPath, g_pcConfigFileExt, vosConfigRefreshByModule);

    return 0;
}

static int vosConfigFlushKeyReset(void * key,void * value)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey) {
        tkey->flag = CFG_NOT_FLUSHED;
    }
}

static int vosConfigFlushSectionReset(void * key,void * value)
{
    CFG_SECTION_DATA_t *tsection = (CFG_SECTION_DATA_t *)value;
    if (tsection) {
        tsection->flag = CFG_NOT_FLUSHED;
        vosHashForEachDo(tsection->keys, vosConfigFlushKeyReset);
    }
}

static uint32 vosConfigFlushKeyAppend(void * key,void * value, FILE *stream)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey && tkey->flag == CFG_NOT_FLUSHED) {
        fprintf(stream, "%s=%s\n", (char *)key, (char *)tkey->data);
        tkey->flag = CFG_FLUSHED;
    }
}

static uint32 vosConfigFlushSectionAppend(void * key,void * value, FILE *stream)
{
    CFG_SECTION_DATA_t *data = (CFG_SECTION_DATA_t *)value;
    if (data && data->flag == CFG_NOT_FLUSHED) {
        data->flag = CFG_FLUSHED;
        fprintf(stream, "[%s]\n", (char *)key);
        vosHashForEachDoCb(data->keys,
            (FUNCPTR)vosConfigFlushKeyAppend, (uint32)stream, 4, 5, 6, 7, 8);
    }
}


/*******************************************************************************
*
* vosConfigFlushByModule:	 Flush the memory image to the configuration file 
*   for specified module. for now, we dont keep the comment info, just write out 
*   the sections.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fname - the file name of module, which include full path
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigFlushByModule(char *fname)
{
    int		rv = 0;
    FILE	*fp;
    CFG_MODULE_DATA_t *tmodule = NULL;

    vosMutexTake(&g_pstConfigLock);

    if (fname == NULL ||
        (fname != NULL && fname[0] == 0)) {
        vosPrintk("vosConfigFlush: config file name is null\r\n");
        rv = -1;
        goto exit_label;
    }

    if (g_pstConfigDatabase == NULL)
    {
        vosPrintk("vosConfigFlush: config database is null\r\n");
        rv = -2;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        vosPrintk("vosConfigFlush: module is null\r\n");
        rv = -3;
        goto exit_label;
    }

    vosHashForEachDo(tmodule->sections, vosConfigFlushSectionReset);

    /* fp can be NULL if creating config file for first time */

    fp = vosFOpen(fname, "w");
    if (fp == NULL) {
        vosPrintk("vosConfigFlush: cannot write file: %s\r\n", fname);
        rv = -4;
        goto exit_label;
    }

    /*
     * write out the sections
     */
    vosHashForEachDoCb(tmodule->sections,
        (FUNCPTR)vosConfigFlushSectionAppend, (uint32)fp, 4, 5, 6, 7, 8);

    vosFClose(fp);
    
exit_label:
    if (rv < 0) {
        vosPrintk("vosConfigFlush: variables not saved\r\n");
    }
    vosMutexGive(&g_pstConfigLock);
    return rv;
}


static int vosConfigFlushForEachModule(void *key, void *value)
{
    if (key) {
        return vosConfigFlushByModule((char*)key);
    }
    else {
        vosPrintk("Input a null pointer to flush file\r\n");
        return -1;
    }
}

/*******************************************************************************
*
* vosConfigFlush:	 flush all modules' configruation into config file.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int vosConfigFlush()
{
    if (g_pstConfigDatabase)
    {
        vosHashForEachDo(g_pstConfigDatabase, vosConfigFlushForEachModule);
    }

    return 0;
}

static int vosConfigShowForEachKey(void *key, void *value, int fd)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey && tkey->data)
    {
        vosPrintf(fd, "    %-20s : %s\r\n", (char *)key, (char *)tkey->data);
    }
}


static int vosConfigShowForEachSection(void *key, void *value, int fd)
{
    CFG_SECTION_DATA_t *tsection = (CFG_SECTION_DATA_t *)value;
    vosPrintf(fd, "  [%s]\r\n", (char *)key);
    if (tsection && tsection->keys) {
        vosHashForEachDoCb(tsection->keys,
            (FUNCPTR)vosConfigShowForEachKey, (uint32)fd, 4, 5, 6, 7, 8);
    }
}

static int vosConfigShowForEachModule(void *key, void *value, int fd)
{
    CFG_MODULE_DATA_t *tmodule = (CFG_MODULE_DATA_t *)value;

    /* strip the default path */
    vosPrintf(fd, "%s\r\n", ((char *)key)+vosStrLen(g_pcConfigDefaultPath)+1);

    if (tmodule && tmodule->sections) {
        vosHashForEachDoCb(tmodule->sections,
            (FUNCPTR)vosConfigShowForEachSection, (uint32)fd, 4, 5, 6, 7, 8);
    }
}

/*******************************************************************************
*
* vosConfigShowByKey:	show current configuration for a specified key 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fname - specified module file which to show
*   section - specified section which to show
*   key - specified key which to show
*   fd - file descriptor on which to write
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosConfigShowByKey(char *fname, char *section, char *key, int fd)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL) {
        vosPrintf(fd, "Database have not been initialized.\r\n\r\n");
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL) {
        vosPrintf(fd, "Specified module is not existent.\r\n\r\n");
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
    if (tsection == NULL) {
        vosPrintf(fd, "Specified section is not existent.\r\n\r\n");
        goto exit_label;
    }

    tkey = (CFG_KEY_DATA_t *)vosHashValue(key, tsection->keys);
    if (tkey == NULL) {
        vosPrintf(fd, "Specified key is not existent.\r\n\r\n");
        goto exit_label;
    }

    if (tkey->data == NULL) {
        vosPrintf(fd, "The value of specified key is not existent.\r\n\r\n");
        goto exit_label;
    }

    vosPrintf(fd, "%s\r\n\r\n", (char *)tkey->data);

exit_label:
    vosMutexGive(&g_pstConfigLock);
}

/*******************************************************************************
*
* vosConfigShowBySection:   show current configuration for a specified section 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fname - specified module file which to show
*   section - specified section which to show
*   fd - file descriptor on which to write
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosConfigShowBySection(char *fname, char *section, int fd)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL) {
        vosPrintf(fd, "Database have not been initialized.\r\n\r\n");
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL) {
        vosPrintf(fd, "Specified module is not existent.\r\n\r\n");
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
    if (tsection == NULL) {
        vosPrintf(fd, "Specified section is not existent.\r\n\r\n");
        goto exit_label;
    }

    vosHashForEachDoCb(tsection->keys,
            (FUNCPTR)vosConfigShowForEachKey, (uint32)fd, 4, 5, 6, 7, 8);

exit_label:
    vosMutexGive(&g_pstConfigLock);
}

/*******************************************************************************
*
* vosConfigShowByModule:   show current configuration for a specified module 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fname - specified module file which to show
*   fd - file descriptor on which to write
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosConfigShowByModule(char *fname, int fd)
{
    CFG_MODULE_DATA_t *tmodule;

    vosMutexTake(&g_pstConfigLock);

    /* strip the default path */
    vosPrintf(fd, "%s\r\n", fname+vosStrLen(g_pcConfigDefaultPath)+1);

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule && tmodule->sections)
    {
        vosHashForEachDoCb(tmodule->sections,
            (FUNCPTR)vosConfigShowForEachSection, (uint32)fd, 4, 5, 6, 7, 8);
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
}

/*******************************************************************************
*
* vosConfigShow:	 Display current configuration variables for all modules.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fd - file descriptor on which to write
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosConfigShow(int fd)
{
    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase)
    {
        vosHashForEachDoCb(g_pstConfigDatabase,
            (FUNCPTR)vosConfigShowForEachModule, (uint32)fd, 4, 5, 6, 7, 8);
    }
    vosPrintf(fd, "\r\n");

exit_label:
    vosMutexGive(&g_pstConfigLock);
}

char *vosConfigValueGet(
    char *fname,
    const char *section,
    char *key,
    char *default_value)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    char *value = default_value;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        goto exit_label;
    }

    tkey = (CFG_KEY_DATA_t *)vosHashValue(key, tsection->keys);
    if (tkey == NULL)
    {
        goto exit_label;
    }

    if (tkey->data == NULL)
    {
        goto exit_label;
    }

    value = tkey->data;

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return value;
}

int vosConfigIntegerGet(
    char *fname,
    char *section,
    char *key,
    int default_value)
{
    char *value = vosConfigValueGet(fname, section, key, NULL);
    if (value == NULL)
      return default_value;

    return strtol(value, NULL, 0);
}

uint32 vosConfigUInt32Get(
    char *fname,
    char *section,
    char *key,
    uint32 default_value)
{
    char *value = vosConfigValueGet(fname, section, key, NULL);
    if (value == NULL)
      return default_value;

    return strtoul(value, NULL, 0);
}

int vosConfigValueSet(
    char *fname,
    const char *section,
    char *key,
    const char *value)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL) {
        tmodule = vosAlloc(sizeof(CFG_MODULE_DATA_t));
        ASSERT(tmodule != NULL, "Out of memory!!!");
        tmodule->sections = vosHashCreate(vosStringHash, vosEqualString, vosConfigSectionDestroy, 0);
        vosHashInsert(vosStrDup(fname), tmodule, g_pstConfigDatabase);
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        tsection = vosAlloc(sizeof(CFG_SECTION_DATA_t));
        ASSERT(tsection != NULL, "Out of memory!!!");
        tsection->keys = vosHashCreate(vosStringHash, vosEqualString, vosConfigKeyDestroy, 0);
        tsection->flag = CFG_NOT_FLUSHED;
        vosHashInsert(vosStrDup(section), tsection, tmodule->sections);
    }

    tkey = vosAlloc(sizeof(CFG_KEY_DATA_t));
    ASSERT(tsection != NULL, "Out of memory!!!");
    tkey->data = vosStrDup(value);
    tkey->flag = CFG_NOT_FLUSHED;

    vosHashInsert(vosStrDup(key), tkey, tsection->keys);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigIntegerSet(
    char *fname,
    char *section,
    char *key,
    int value)
{
    char acValue[32];
    vosSprintf(acValue, "%d", value);
    return vosConfigValueSet(fname, section, key, acValue);
}

int vosConfigUInt32Set(
    char *fname,
    char *section,
    char *key,
    uint32 value)
{
    char acValue[32];
    vosSprintf(acValue, "%u", value);
    return vosConfigValueSet(fname, section, key, acValue);
}



/** description: delete config module.
 *  @param fname: the file name of the module, which include full path
 *  @return success: 0
 *          fail:    -1

 *  @see
 *  @deprecated
 */
int vosConfigModuleDelete(char *fname)
{
    CFG_MODULE_DATA_t *tmodule;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    if (fname != NULL && fname[0] != '\0') {
        tmodule = (CFG_MODULE_DATA_t *)vosHashValue(
            (void *)fname,
            g_pstConfigDatabase);
        vosHashRemove((void *)fname, g_pstConfigDatabase);
        vosRemove(fname);
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigSectionDelete(char *fname, const char *section)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    vosHashRemove((void *)section, tmodule->sections);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigKeyDelete(char *fname, char *section, char *key)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
    if (tsection == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    vosHashRemove((void *)key, tsection->keys);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigModuleCount()
{
    int cnt = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    cnt = vosHashCount(g_pstConfigDatabase);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return cnt;
}

int vosConfigSectionCount(char *fname)
{
    CFG_MODULE_DATA_t *tmodule;
    int cnt = 0;
    
    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    cnt = vosHashCount(tmodule->sections);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return cnt;
}

int vosConfigKeyCount(char *fname, char *section)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    int cnt = 0;
    
    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
    if (tsection == NULL)
    {
        goto exit_label;
    }

    cnt = vosHashCount(tsection->keys);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return cnt;
}

int vosConfigSectionCreate(
    char *fname,
    const char *section)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL) {
        tmodule = vosAlloc(sizeof(CFG_MODULE_DATA_t));
        ASSERT(tmodule != NULL, "Out of memory!!!");

        if (tmodule == NULL) {
            rv = -1;
            goto exit_label;
        }
        tmodule->sections = vosHashCreate(vosStringHash, vosEqualString, vosConfigSectionDestroy, 0);
        vosHashInsert(vosStrDup(fname), tmodule, g_pstConfigDatabase);
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        tsection = vosAlloc(sizeof(CFG_SECTION_DATA_t));
        ASSERT(tsection != NULL, "Out of memory!!!");

        if (tsection == NULL) {
            rv = -1;
            goto exit_label;
        }
        tsection->keys = vosHashCreate(vosStringHash, vosEqualString, vosConfigKeyDestroy, 0);
        tsection->flag = CFG_NOT_FLUSHED;
        vosHashInsert(vosStrDup(section), tsection, tmodule->sections);
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}


int vosConfigModuleIsExisted(char *fname)
{
    CFG_MODULE_DATA_t *tmodule;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigSectionIsExisted(
    char *fname,
    const char *section)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        rv = -1;
        goto exit_label;
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigKeyIsExisted(
    char *fname,
    const char *section,
    char *key)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    int rv = 0;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tkey = (CFG_KEY_DATA_t *)vosHashValue(key, tsection->keys);
    if (tkey == NULL)
    {
        rv = -1;
        goto exit_label;
    }

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigSectionGetByIndex(char *fname, int index, char **section)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection = NULL;
    int rv = -1;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    rv = vosHashValueGetByIndex(index, tmodule->sections, (void **)section, (void **)&tsection);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}

int vosConfigKeyGetByIndex(
    char *fname,
    char *section,
    int index,
    char **key,
    char **value)
{
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    int rv = -1;

    vosMutexTake(&g_pstConfigLock);

    if (g_pstConfigDatabase == NULL)
    {
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)vosHashValue(fname, g_pstConfigDatabase);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)vosHashValue(section, tmodule->sections);
    if (tsection == NULL)
    {
        goto exit_label;
    }

    rv = vosHashValueGetByIndex(index, tsection->keys, (void **)key, (void **)value);

exit_label:
    vosMutexGive(&g_pstConfigLock);
    return rv;
}


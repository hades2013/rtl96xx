
#ifndef __UFILE_PROCESS_H__
#define __UFILE_PROCESS_H__

#include "ufile.h"
#include <lw_config.h>
#define UPGRADE_APP_0 0
#define UPGRADE_APP_1 1

typedef enum{
	UP_OK = 0,
	UP_FAILED, 
	UP_ERR_LENGTH,
	UP_ERR_IDENTIFY,
	UP_ERR_CHKSUM,
	UP_ERR_IO,
	UP_ERR_NOMEM,
	UP_ERR_NONAME,
	UP_ERR_IPC,
	UP_ERR_EXT,
	UP_ERR_UNKNOWN
}
upgrade_error_t;

typedef enum{
	APP_CHECK_OK = 0,
	APP_CHECK_ERROR, 
    APP_CHECK_ERROR_PARA
}APP_CHECK_E;

typedef enum{
	READ_CONTENT_OF_KERNEL = 0,
    READ_CONTENT_OF_ROOTFS
}READ_CONTENT_E;

#define UFILE_TMP_PATH "/tmp/"
#define UFILE_TMP_BOOT "/tmp/boot.bin"
#define UFILE_TMP_KERNEL "/tmp/kernel.bin"
#define UFILE_TMP_ROOTFS "/tmp/rootfs.bin"
//#define UFILE_TMP_CONFIG "/tmp/config.bin"

#define UFILE_IDENTIFY "EPN104"
#define UFILE_BOOTFLAG "/proc/manufactory/bootflag"
#define UFILE_RESTOREFLAG "/proc/manufactory/restoreflag" //add by dengjian 2012-08-29 for QID0013
#define UFILE_BOOTVERSION "/proc/manufactory/bootversion" //add by zhouguanhua 2013-05-6 for bug id 36
#define UFILE_HARD_VERSION "/proc/manufactory/hardversion" //add by zhouguanhua 2013-05-6 for bug id 36

/*Begin add by zhouguanhua 2013-5-15 for bug id 52*/
#define UFILE_SN        "/proc/manufactory/SN" 
#define UFILE_ETHADDR   "/proc/manufactory/ethaddr" 

//#define MAX_MAC  32
#define MAX_MAC  89			//edit by leijinbao for mac Encryption 2013/12/20
#define MAX_SN_SIZE 64
/*End add by zhouguanhua 2013-5-15 for bug id 52*/

/*Add by huangmingjian 2014-01-08 */
#define MANUFACTORY_DIR			   "/proc/manufactory/"	

/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
#define ENV_FTOVERFLAG "FToverflag"
#define ENV_APCSETVAL  "apcsetval"
#define ENV_MODSETVAL  "modsetval"
#define UFILE_FTOVERFLAG (MANUFACTORY_DIR ENV_FTOVERFLAG)  
#define UFILE_APCSETVAL  (MANUFACTORY_DIR ENV_APCSETVAL)  
#define UFILE_MODSETVAL  (MANUFACTORY_DIR ENV_MODSETVAL)  
#endif
/*Begin add by huangmingjian 2014-01-13*/



typedef enum{
	UFILE_STATE_IDLE = 0,	
	UFILE_STATE_BOOT_ERASE,
	UFILE_STATE_BOOT_WRITE,
	UFILE_STATE_BOOT_VERIFY,
	UFILE_STATE_ROOTFS_ERASE,
	UFILE_STATE_ROOTFS_WRITE,
	UFILE_STATE_ROOTFS_VERIFY,
	UFILE_STATE_KERNEL_ERASE,
	UFILE_STATE_KERNEL_WRITE,
	UFILE_STATE_KERNEL_VERIFY,
	UFILE_STATE_APP_ERASE,
	UFILE_STATE_APP_WRITE,
	UFILE_STATE_APP_VERIFY,	
	UFILE_STATE_FAILED,
	UFILE_STATE_DONE
}
ufile_state_t;


#define MAX_VERSION_LEN             48
#define MAX_RELEASE_LEN             32
#define MAX_CFGHEAD_FILENAME_LEN        16

typedef struct tagLoadFileHeaderS
{
    unsigned int usFileCRC;           /* 对文件的校验 */
    unsigned int ulFileLength;         /* 文件长度信息 */
    unsigned short usReserve[3];        /* 保留*/
    unsigned short fstype;
    char    szVersion[MAX_VERSION_LEN];     /* 保存文件名信息（文件名信息代表了板子类型信息)*/
    char    szRelease[MAX_RELEASE_LEN];     /* 文件发行、编译日期 */
}APP_HEADER_S;

#define PRODUCT_VER_STR_LEN_MAX 36
#define FMW_HEADLEN 0x60

#pragma pack(1)
typedef struct tagFmwHdrS
{
	unsigned int headlen;   // 文件头的长度
	unsigned int imgsize;   // 整个文件大小，包含头
	unsigned int checksum;  // 校验值，包含头，算法：4字节相加，结果为0。
	unsigned int timestamp; // 升级文件制作的时间
	unsigned int kernellen; // 内核的长度
	unsigned int fslen;     // 文件系统的长度
			        // 因此文件必须4字节对齐，如果不对齐，则用0填充。
	
	unsigned char _model; // 产品类型 
	unsigned char _major; // 产品V版本号 V100
	unsigned char _minor; // 产品R版本号 R001
	unsigned char _patch; // 产品D版本号 D001
	unsigned char _limit; // 产品受限发布的版本号 L00
	
	
	unsigned char kernel_type;   // 软件类型：1：内核； 0xff:空
	unsigned char _kernel_major; // 内核V版本号 V100，  无相关信息时为0xff
	unsigned char _kernel_minor; // 内核R版本号 R001，  无相关信息时为0xff
	unsigned char _kernel_patch; // 内核D版本号 D001，  无相关信息时为0xff
	unsigned char _kernel_limit; // 内核受限发布的版本号 L00，  无相关信息时为0xff
	
	unsigned char fs_type;   // 软件类型：1：文件系统； 0xff:空
	unsigned char product_version_str[PRODUCT_VER_STR_LEN_MAX]; 
	
	unsigned char addition[FMW_HEADLEN-0x47]; // 保留，并填充0到固定长度
	
}APP_FMW_HEADER_S;
#pragma pack()

#pragma pack(1)
typedef struct tagImportFileHeaderS
{
    unsigned int usFileCRC;           /* 对文件的校验 */
    unsigned int ulFileLength;         /* 文件长度信息 */
    unsigned short usReserve[3];        /* 保留*/
    char    szCfgFileName[MAX_CFGHEAD_FILENAME_LEN];     /* 保存配置文件文件名信息*/
    char tail;                          /*以换行符结束*/
}CONFIG_HEADER_S;
#pragma pack()

extern uint16_t _ufile_content;

int ufile_check(const char *path);
int app_ufile_check(const char *path);
uint8_t *ufile_read(const char *path, uint32_t *read_size);
uint8_t *upgrade_content_read_from_app(const char *path, uint32_t *read_size, int read_type);
int ufile_validate(const uint8_t *buffer, uint32_t length);
int fw_head_check(const uint8_t *buffer, uint32_t length);

int ufile_process(const uint8_t *buffer, uint32_t length);
int ufile_output_config(const uint8_t *buffer, uint32_t length);
uint16_t ufile_content(void);
unsigned short crc_calc_direct(unsigned short usCrc,short * pusBuf,unsigned long ulSize);
extern int cfgFileAddHeadAndSave(const char * path);
int cfgFileValidate(const uint8_t *buffer, uint32_t length);
int get_bootflag(void);
int GetEthaddr(char * ethaddr,int ethaddr_size);
int GetSN(char *manucode,int manucode_size);
int GetHardVersion(char *hr_version, int hr_version_size);
int GetBootVersion(char *boot_version,int boot_version_size);
int read_kernel_version(char *kernel_version,char *kernel_build_time);
int getFactoryParasByName(char *name, char *buffer, int length);
size_t write_date_file(const char *path, const  char *buffer, size_t size);

#endif /*#ifndef __UFILE_PROCESS_H__*/


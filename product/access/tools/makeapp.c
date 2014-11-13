#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include "include/lw_config.h"


// TODO
/*Begin modify by dengjian 2012-11-06 for QID0080*/
#if defined(CONFIG_PRODUCT_EPN104) || defined(CONFIG_PRODUCT_EPN204)
#ifdef CONFIG_BOOT_MULTI_APP
#define FLASH_SIZE              (8 * 1024 * 1024)
#else
#define FLASH_SIZE              (4 * 1024 * 1024)
#endif
#endif

#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
#define FLASH_SIZE              (8 * 1024 * 1024)
#endif

#if defined(CONFIG_PRODUCT_5500)
#define FLASH_SIZE              (16 * 1024 * 1024)
#endif

#if defined(CONFIG_PRODUCT_EPN101R)
#define FLASH_SIZE              (32 * 1024 * 1024)
#endif
/*End modify by dengjian 2012-11-06 for QID0080*/

// TODO
#define RECORD_INFO_SIZE        (1024 * 64)
#define SWAP_SEC_SIZE           (1024 * 64)
#define SEC_SIZE                (1024 * 64)


#define HEADER_ONU_ID_EPN104       0x01




#define VERSION_LEN             48
#define RELEASE_LEN             32


static const unsigned short m_ausCrcTableDirect[] = {
    0x0000,  0xC0C1,  0xC181,  0x0140,  0xC301,  0x03C0,  0x0280,  0xC241,
    0xC601,  0x06C0,  0x0780,  0xC741,  0x0500,  0xC5C1,  0xC481,  0x0440,
    0xCC01,  0x0CC0,  0x0D80,  0xCD41,  0x0F00,  0xCFC1,  0xCE81,  0x0E40,
    0x0A00,  0xCAC1,  0xCB81,  0x0B40,  0xC901,  0x09C0,  0x0880,  0xC841,
    0xD801,  0x18C0,  0x1980,  0xD941,  0x1B00,  0xDBC1,  0xDA81,  0x1A40,
    0x1E00,  0xDEC1,  0xDF81,  0x1F40,  0xDD01,  0x1DC0,  0x1C80,  0xDC41,
    0x1400,  0xD4C1,  0xD581,  0x1540,  0xD701,  0x17C0,  0x1680,  0xD641,
    0xD201,  0x12C0,  0x1380,  0xD341,  0x1100,  0xD1C1,  0xD081,  0x1040,
    0xF001,  0x30C0,  0x3180,  0xF141,  0x3300,  0xF3C1,  0xF281,  0x3240,
    0x3600,  0xF6C1,  0xF781,  0x3740,  0xF501,  0x35C0,  0x3480,  0xF441,
    0x3C00,  0xFCC1,  0xFD81,  0x3D40,  0xFF01,  0x3FC0,  0x3E80,  0xFE41,
    0xFA01,  0x3AC0,  0x3B80,  0xFB41,  0x3900,  0xF9C1,  0xF881,  0x3840,
    0x2800,  0xE8C1,  0xE981,  0x2940,  0xEB01,  0x2BC0,  0x2A80,  0xEA41,
    0xEE01,  0x2EC0,  0x2F80,  0xEF41,  0x2D00,  0xEDC1,  0xEC81,  0x2C40,
    0xE401,  0x24C0,  0x2580,  0xE541,  0x2700,  0xE7C1,  0xE681,  0x2640,
    0x2200,  0xE2C1,  0xE381,  0x2340,  0xE101,  0x21C0,  0x2080,  0xE041,
    0xA001,  0x60C0,  0x6180,  0xA141,  0x6300,  0xA3C1,  0xA281,  0x6240,
    0x6600,  0xA6C1,  0xA781,  0x6740,  0xA501,  0x65C0,  0x6480,  0xA441,
    0x6C00,  0xACC1,  0xAD81,  0x6D40,  0xAF01,  0x6FC0,  0x6E80,  0xAE41,
    0xAA01,  0x6AC0,  0x6B80,  0xAB41,  0x6900,  0xA9C1,  0xA881,  0x6840,
    0x7800,  0xB8C1,  0xB981,  0x7940,  0xBB01,  0x7BC0,  0x7A80,  0xBA41,
    0xBE01,  0x7EC0,  0x7F80,  0xBF41,  0x7D00,  0xBDC1,  0xBC81,  0x7C40,
    0xB401,  0x74C0,  0x7580,  0xB541,  0x7700,  0xB7C1,  0xB681,  0x7640,
    0x7200,  0xB2C1,  0xB381,  0x7340,  0xB101,  0x71C0,  0x7080,  0xB041,
    0x5000,  0x90C1,  0x9181,  0x5140,  0x9301,  0x53C0,  0x5280,  0x9241,
    0x9601,  0x56C0,  0x5780,  0x9741,  0x5500,  0x95C1,  0x9481,  0x5440,
    0x9C01,  0x5CC0,  0x5D80,  0x9D41,  0x5F00,  0x9FC1,  0x9E81,  0x5E40,
    0x5A00,  0x9AC1,  0x9B81,  0x5B40,  0x9901,  0x59C0,  0x5880,  0x9841,
    0x8801,  0x48C0,  0x4980,  0x8941,  0x4B00,  0x8BC1,  0x8A81,  0x4A40,
    0x4E00,  0x8EC1,  0x8F81,  0x4F40,  0x8D01,  0x4DC0,  0x4C80,  0x8C41,
    0x4400,  0x84C1,  0x8581,  0x4540,  0x8701,  0x47C0,  0x4680,  0x8641,
    0x8201,  0x42C0,  0x4380,  0x8341,  0x4100,  0x81C1,  0x8081,  0x4040
};

/* BOOT MANAGEMENT DATA的格式请参考《Passave Bootloader User Manual 1.0.pdf》 */
/* 第一份BOOT MANAGEMENT DATA的内容 */
static unsigned char m_aucDefaultBootManagementData0[] =
{
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* 第二份BOOT MANAGEMENT DATA的内容 */
static unsigned char m_aucDefaultBootManagementData1[] =
{
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#if 0
/* 长度为16字节 */
typedef struct SOFTWARE_TAIL
{
    unsigned int ulYear     :16;
    unsigned int ulMonth    :8;
    unsigned int ulDay      :8;
    unsigned int ulHour     :8;
    unsigned int ulMinute   :8;
    unsigned int ulSecond   :8;
    unsigned int ulReserved :8;
    unsigned int ulUueLen   :32;
    unsigned int ucVersion  :8;
    unsigned int ucObject   :8;
    unsigned int ulCrc      :16;
}TAIL_S;
#endif


/* 使用小字节序， 96个字节 */
typedef struct tagLoadFileHeaderS
{
    unsigned int usFileCRC;           /* 对文件的校验 */
    unsigned int ulFileLength;         /* 文件长度信息 */
    unsigned short usReserve[3];        /* 保留*/
    unsigned short fstype;
    char    szVersion[VERSION_LEN];     /* 保存文件名信息（文件名信息代表了板子类型信息)*/
    char    szRelease[RELEASE_LEN];     /* 文件发行、编译日期 */
}APP_HEADER_S;

#define TAIL_SIZE       sizeof(TAIL_S)
#define APP_HEAD_SIZE   sizeof(APP_HEADER_S)

unsigned char *m_pucFilePtr = NULL;


#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_5500) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
#define FLASH_OFFSET	0x0
#else
#define FLASH_OFFSET	0x10
#endif

/* app文件中各部分的位置 */
#define APP_HEADER_OFF      m_pucFilePtr
#define SOFT_OFF            (APP_HEADER_OFF+APP_HEAD_SIZE)
/* bin文件中各部分的位置 */
#define BIN_BOOT_SIZE      (256 * 1024)
#define BIN_BOOT_ENV_SIZE  (64 * 1024)
#define BIN_EXP_SIZE       (64 * 1024)
//#define BIN_RSD_SIZE       (512 * 1024)
#if defined(CONFIG_PRODUCT_EPN104)
#define BIN_OS_SIZE        (3584 * 1024)
#endif

#if defined(CONFIG_PRODUCT_EPN105)
//#define BIN_OS_SIZE        (1536 * 1024)
#define BIN_OS_SIZE        (0x160000) //kernel1 , 0x160000 
#define BIN_FS_SIZE        (0x230000) //rootfs1, 0x230000 
#define BIN_NV1_SIZE        (0x40000) //nvram1,0x40000 
#define BIN_NV2_SIZE        (0x40000) //nvram2,0x40000 
#endif

#if defined(CONFIG_PRODUCT_5500)
#define BIN_OS_SIZE        (4608 * 1024)
#endif

#if defined(CONFIG_PRODUCT_EPN101R)
#define BIN_OS_SIZE        (15872 * 1024)
#endif

#ifdef FLASH_OFFSET
#define BIN_BOOT_OFF        (m_pucFilePtr + FLASH_OFFSET)
#else
#define BIN_BOOT_OFF        m_pucFilePtr
#endif

#define BIN_BOOT_ENV_OFF    (BIN_BOOT_OFF+BIN_BOOT_SIZE)
#define BIN_EXP_OFF         (BIN_BOOT_ENV_OFF+BIN_BOOT_ENV_SIZE)
#define BIN_OS1_OFF         (BIN_EXP_OFF+BIN_EXP_SIZE)
#define BIN_OS2_OFF         (BIN_OS1_OFF+BIN_OS_SIZE+BIN_FS_SIZE+BIN_NV1_SIZE+BIN_NV2_SIZE)
#define BIN_CFG_OFF         (BIN_OS2_OFF+BIN_OS_SIZE)
///////////////////////////////////////////////////////////////////
#define write_4(ptr, value)  (*((int *)(ptr)) = (value))
#define panic(_m...)  \
    do { \
        fprintf(stderr, ##_m); \
        if (NULL != m_pucFilePtr) \
            free(m_pucFilePtr); \
            m_pucFilePtr = NULL;\
        exit(1); \
    }while(0)

/*****************************************************************************
 函 数 名  : crc_calc_direct
 返 回 值  : static
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned short crc_calc_direct( unsigned short usCrc,
                                short *pusBuf,
                                unsigned long ulSize)
{
    unsigned short *pusTmp = NULL;
    unsigned short *pusLim = NULL;

    pusTmp = (unsigned short *)pusBuf;
    pusLim = pusTmp + ( ulSize >> 1 );
    while (pusTmp < pusLim)
    {
		/*Begin modify by jiangzhenhua 2012/7/23*/
        #if (__BYTE_ORDER == __LITTLE_ENDIAN)
       
     
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( *pusTmp & 0xFF )];
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( ( *pusTmp++ & 0xFF00 ) >> 8 )];
        #else
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( ( *pusTmp & 0xFF00 ) >> 8 )];
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( *pusTmp++ & 0xFF )];
        #endif
		/*End modify by jiangzhenhua 2012/7/23*/
    }

    return usCrc;
}

int AddFile(unsigned char *pucBuf, const char *pucFile)
{
    char szDir[128];
    int fd = -1;
    struct stat stFileStat;

    strcpy(szDir, pucFile);

    fd = open(pucFile, O_RDONLY);
    if (-1 == fd)
        panic("File [%s] does not exist!\n", szDir);
    stat(szDir, &stFileStat);
    read(fd, pucBuf, stFileStat.st_size);
    close(fd);

    return stFileStat.st_size;
}


void AddAppHead(unsigned char *pucBuf, int len )
{
    unsigned short usCrc = 0;
    
    APP_HEADER_S *pstHead = (APP_HEADER_S *)pucBuf;
    memset(pstHead, 0, sizeof(APP_HEADER_S));
    pstHead->ulFileLength = htonl(len);
    strcpy(pstHead->szVersion, CONFIG_DEVELOP_VER);
    strcpy(pstHead->szRelease, CONFIG_RELEASE_VER);
    usCrc = crc_calc_direct(0, (short *)(pucBuf+4), len+sizeof(APP_HEADER_S)- 4);
    pstHead->usFileCRC = htonl(usCrc);
}


void SaveFile(char *FileName, unsigned char *pucBuf, int len)
{
    char szDir[128];
    int fd = -1;

    if (getcwd(szDir, 128) == NULL)
        panic("Can't get current directory!\n");

    strcat(szDir, FileName);

    printf("==> file:%s len=0x%08x\n", szDir,len);

    fd = open(szDir, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == fd)
        panic("Can't create file .%s\n", szDir);

    /* 写文件 */
    write(fd, pucBuf, len);
    close(fd);
}

void CheckPara(unsigned long BootVer, unsigned long SoftVer, unsigned long EepromVer, char *soft_name, char *boot_name)
{
    int fd = -1;

    if (BootVer<100 || BootVer >256)
        panic("Bootloder version is illegal!\n");



    fd = open(soft_name, O_RDONLY);
    if (-1 == fd)
        panic("File [%s] does not exist!\n", soft_name);

    fd = open(boot_name, O_RDONLY);
    if (-1 == fd)
        panic("File [%s] does not exist!\n", boot_name);
}

// todo_list:
//  软件长度的对齐处理

int main(int argc, char* argv[])
{
    int len = 0, master_len, slave_len;
    int file_len = FLASH_SIZE;
    unsigned long BootVer, SoftVer, EepromVer;
    char *soft_name = NULL;
    char *boot_name = NULL;
    char *tmp_file = NULL;
    char *product_name = NULL;
    char szapp_file_name[30] = {0};
    char szimg_file_name[30] = {0};
    char szDir[128];
    unsigned char aucMagic[16] = {0x01, 0x03 ,0x02 ,0x00, 0x10 ,0x00 ,0x00 ,0x00, 
    				  0x00 ,0x00 ,0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00};


    if (7 != argc)
        panic("Usage: makeapp bootver softver softname bootname tmpfile productname\n" );
    /*------------------------------------------------------------------------
     * 提取参数信息
     *-----------------------------------------------------------------------*/
    BootVer = atol(argv[1]);
    SoftVer = atol(argv[2]);
    soft_name = argv[3];
    boot_name = argv[4];
    tmp_file = argv[5];
    product_name = argv[6];

    CheckPara(BootVer, SoftVer, EepromVer, soft_name, boot_name);
    /* 判断产品名称 */
    if (0 == strcmp(product_name, CONFIG_PRODUCT_NAME))
    {
        strcpy(szapp_file_name, "/"CONFIG_RELEASE_VER".bin");
        strcpy(szimg_file_name, "/"CONFIG_RELEASE_VER".img");
    }
    else
    {
        panic("Unknown product name!\n");
    }

    
    printf("FLASH_SIZE=0x%08x\n",(unsigned int)FLASH_SIZE);
    printf("BIN_BOOT_OFF=0x%08x\n",(unsigned int)BIN_BOOT_OFF);
    printf("BIN_BOOT_ENV_OFF=0x%08x\n",(unsigned int)BIN_BOOT_ENV_OFF);
    printf("BIN_EXP_OFF=0x%08x\n",(unsigned int)BIN_EXP_OFF);
    printf("BIN_OS1_OFF=0x%08x\n",(unsigned int)BIN_OS1_OFF);
    printf("BIN_OS2_OFF=0x%08x\n",(unsigned int)BIN_OS2_OFF);
    printf("BIN_CFG_OFF=0x%08x\n",(unsigned int)BIN_CFG_OFF);


    /* 申请一块足够大的内存 */
    m_pucFilePtr = (unsigned char *)malloc(file_len);
    if (NULL == m_pucFilePtr)
        panic("Memory malloc failed!\n");

    memset(m_pucFilePtr, 0xff, file_len);

	#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_5500) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
		printf("EPN104N is not use add APP_HEADER\n");
	#else
  		/*------------------------------------------------------------------------
     		* 制作APP升级文件，APP升级文件包含kernel升级文件和应用软件升级文件
     		*-----------------------------------------------------------------------*/

    	/* 2. 处理SOFTWARE文件 */
    	len = AddFile(SOFT_OFF, soft_name);

    	/* 3. 生成app头部 */
    	AddAppHead(APP_HEADER_OFF, len);

    	/* 4. 生成app文件 */
    	SaveFile(szapp_file_name, m_pucFilePtr, len+APP_HEAD_SIZE);
	#endif
	
  	/*------------------------------------------------------------------------
     	* 制作BIN生产烧片文件
     	*-----------------------------------------------------------------------*/
    memset(m_pucFilePtr, 0xff, file_len);

	#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_5500) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
		printf("EPN104N is not use add aucMagic\n");
	#else
    	/*add magic for opl boot*/
    	memcpy(m_pucFilePtr, (unsigned char *)aucMagic, sizeof(aucMagic));
	#endif
  
	len = AddFile(BIN_BOOT_OFF, boot_name);
	/*Begin modify by dengjian 2012-11-01 for QID0077*/
    #if 0
    /*Begin add by dengjian 2012-07-17 for make 8M image*/
    if (getcwd(szDir, 128) == NULL)
        panic("Can't get current directory!\n");

    strcat(szDir, szapp_file_name);
    /*End add by dengjian 2012-07-17 for make 8M image*/
    #endif
   
    len = AddFile(BIN_OS1_OFF, soft_name);
    
    /*Begin modify by dengjian 2012-11-06 for QID0080*/
#ifdef CONFIG_BOOT_MULTI_APP
    //memcpy(BIN_OS2_OFF,BIN_OS1_OFF, len);
    memmove(BIN_OS2_OFF,BIN_OS1_OFF, len);

#endif
    /*End modify by dengjian 2012-11-06 for QID0080*/
    /*End modify by dengjian 2012-11-01 for QID0077*/
    /* 生成bin文件 */
    SaveFile(szimg_file_name, m_pucFilePtr, FLASH_SIZE);  
    free(m_pucFilePtr);
    m_pucFilePtr = NULL;

    return 0;
}


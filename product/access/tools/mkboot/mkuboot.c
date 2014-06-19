/*****************************************************************************

      File name:makeboot.c
      Description:add tail with secret key to boot binary file
      Author:liaohongjun
      Date:
              2013/3/28
==========================================================
      Note:add by liaohongjun for EPN104QID0094
*****************************************************************************/
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
#include "pdt_config.h"
#include "md5.h"

#define DB_EN 0
#if (DB_EN)
#define db_printf(args...) fprintf(stderr, ##args)
#else
#define db_printf(args...)
#endif
#define R_VERSION_LEN             32
#define BOOT_SECRET_KEY_LEN             32
#define BOOT_SIZE (1*1024*1024)  /*1M*/

unsigned char *pucFilePtr = NULL;

#define panic(_m...)  \
    do { \
        fprintf(stderr, ##_m); \
        if (NULL != pucFilePtr) \
            free(pucFilePtr); \
            pucFilePtr = NULL;\
        exit(1); \
    }while(0)

/* 使用小字节序， 80个字节 ,
更改此结构体请同步更改uboot中common.h中内容*/
typedef struct tagLoadFileTailS
{
    unsigned int ulFileLength;         /* 文件长度信息 */
    unsigned int usReserve[3];        /* 保留*/
    char    szRelease[R_VERSION_LEN];  /*版本号*/
    char    szSecretKey[BOOT_SECRET_KEY_LEN];     /* 密钥*/
}BOOT_TAIL_S;

#define BOOT_TAIL_SIZE   sizeof(BOOT_TAIL_S)

int ReadFile(unsigned char *pucBuf, const char *pucFile)
{
    char szDir[128];
    int fd = -1;
    struct stat stFileStat;

    strcpy(szDir, pucFile);

    fd = open(pucFile, O_RDONLY);
    if (-1 == fd)
    {
        panic("File [%s] does not exist!\n", szDir);
    }
    stat(szDir, &stFileStat);
    read(fd, pucBuf, stFileStat.st_size);
    close(fd);

    return stFileStat.st_size;
}
void SimpleCheckPara(char *boot_name)
{
    int fd = -1;
    
    fd = open(boot_name, O_RDONLY);
    if (-1 == fd)
    {
        panic("File [%s] does not exist!\n", boot_name);
    }
}
void BootAddTail(unsigned char *pucBuf, int len )
{   
    UCHAR ucSecretKey[16] = {0};
    BOOT_TAIL_S *pstTail = (BOOT_TAIL_S *)(pucBuf+len);
    int i = 0;
    int j = 0;
    int num = 0;
    char str[32] = {0};
    memset(pstTail, 0, sizeof(BOOT_TAIL_S));
    pstTail->ulFileLength = htonl(len);
    strcpy(pstTail->szRelease, CONFIG_RELEASE_VER);
    MD5Calc((UCHAR *)ucSecretKey,(UCHAR *)BOOT_SECRET_KEY, strlen(BOOT_SECRET_KEY));    
    for(i=0;i<16;i++)
    {
        num=ucSecretKey[i]/16;
        switch(num)
        {
            case 15:pstTail->szSecretKey[j++]='f';break;
            case 14:pstTail->szSecretKey[j++]='e';break;
            case 13:pstTail->szSecretKey[j++]='d';break;
            case 12:pstTail->szSecretKey[j++]='c';break;
            case 11:pstTail->szSecretKey[j++]='b';break;
            case 10:pstTail->szSecretKey[j++]='a';break;
            default:pstTail->szSecretKey[j++]=num+48;break;
        }
        num = ucSecretKey[i]%16;
        switch(num)
        {
            case 15:pstTail->szSecretKey[j++]='f';break;
            case 14:pstTail->szSecretKey[j++]='e';break;
            case 13:pstTail->szSecretKey[j++]='d';break;
            case 12:pstTail->szSecretKey[j++]='c';break;
            case 11:pstTail->szSecretKey[j++]='b';break;
            case 10:pstTail->szSecretKey[j++]='a';break;
            default:pstTail->szSecretKey[j++]=num+48;break;
        }
    }    
    db_printf("SecretKey is %s \n", pstTail->szSecretKey);
}
void SaveFile(char *FileName, unsigned char *pucBuf, int len)
{
    char szDir[128];
    int fd = -1;

    if (getcwd(szDir, 128) == NULL)
        panic("Can't get current directory!\n");

    strcat(szDir, FileName);

    printf("==> file:%s \n", szDir);

    fd = open(szDir, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == fd)
        panic("Can't create file .%s\n", szDir);

    /* 写文件 */
    write(fd, pucBuf, len);
    close(fd);
}

int main(int argc, char* argv[])
{
    int len = 0;
    int file_len = BOOT_SIZE;
    char *boot_name = NULL;
    char szboot_file_name[20] = {0};
    
    if (2 != argc)
    {
        panic("Usage: mkboot bootname\n" );
    }
    /*------------------------------------------------------------------------
     * 提取参数信息
     *-----------------------------------------------------------------------*/
    boot_name = argv[1];
    SimpleCheckPara(boot_name);
    /* 申请一块足够大的内存 */
    pucFilePtr = (unsigned char *)malloc(file_len);
    if (NULL == pucFilePtr)
    {
        panic("Memory malloc failed!\n");
    }

    memset(pucFilePtr, 0xff, file_len);
    len = ReadFile(pucFilePtr, boot_name);
    printf("Read %d bytes from %s\n",len,boot_name);
    BootAddTail(pucFilePtr, len);
    strcpy(szboot_file_name, "/");
    strcat(szboot_file_name, boot_name);
    SaveFile(szboot_file_name, pucFilePtr, len + BOOT_TAIL_SIZE);

    free(pucFilePtr);
    pucFilePtr = NULL;

    return 0;
}



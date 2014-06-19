
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "debug.h"
#include "ufile_process.h"
#include <pdt_config.h>
#include "lw_config.h" //add by dengjian 2012-11-06 for QID0080
#include <basetype.h>
#include <ipc_protocol.h>

uint16_t _ufile_content = 0;

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
        #if (__BYTE_ORDER == __LITTLE_ENDIAN)
       
     
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( *pusTmp & 0xFF )];
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( ( *pusTmp++ & 0xFF00 ) >> 8 )];
        #else
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( ( *pusTmp & 0xFF00 ) >> 8 )];
        usCrc = (usCrc >> 8 ) ^ m_ausCrcTableDirect[(usCrc & 0xFF) ^ ( *pusTmp++ & 0xFF )];
        #endif
    }

    return usCrc;
}

/*
 checksum

 valid firmware?
 
*/
static uint32_t checksum_32 (register const void * memory, register size_t extent, register uint32_t checksum)

{
	while (extent >= sizeof (checksum))
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}
	return (~checksum);
}

int ufile_check(const char *path)
{
	struct stat buf;
	if (stat(path, &buf) < 0){
		return 0;
	}else if (buf.st_size > 0){
		return 1;
	}
	return 0;
}
/*****************************************************************
Function:app_ufile_check
Description:check app file and it's header 
Author:liaohongjun
Date:2012/7/10
INPUT:path        
OUTPUT:         
Return:
    APP_CHECK_ERROR_PARA   //paras error
    APP_CHECK_ERROR          //error
    APP_CHECK_OK    //app file and it's header OK!
Others:	       
*****************************************************************/
#if 1
int app_ufile_check(const char *path)
{
    struct stat buf;
    int fd = -1;
	unsigned char *date_buf = NULL;
	unsigned int readcount = 0;

    if(NULL == path)
    {
        DBG_PRINTF("Invalid path!");
        return APP_CHECK_ERROR_PARA;
    }		
	
    if (stat(path, &buf) < 0)
    {
        DBG_PRINTF("Get file %s property error!",path);		
        return APP_CHECK_ERROR;
    }
    else if (buf.st_size > 0)
    {
        if((fd = open(path, O_RDONLY)) < 0)
        {
            DBG_PRINTF("Could not open file : %s", path);
            return APP_CHECK_ERROR;
        }
		date_buf = malloc(buf.st_size);
	    if(NULL == date_buf)
	    {
			DBG_PRINTF("can not malloc for upgrade_file!");
			close(fd);
	        return APP_CHECK_ERROR;
		}
    	memset(date_buf, 0x0, buf.st_size);
		readcount = read(fd, date_buf, buf.st_size);
        if(readcount < 0)
        {
			free(date_buf);
			close(fd);
			return APP_CHECK_ERROR;
		}
		if(UP_OK != fw_head_check(date_buf, readcount))
	    {			
			DBG_PRINTF("Image check: image file invalid!\r\n");	        
			free(date_buf);
			close(fd);
			return APP_CHECK_ERROR;
	    }        
		free(date_buf);
		close(fd);
        return APP_CHECK_OK;
    }
    return APP_CHECK_ERROR;
}
#else
int app_ufile_check(const char *path)
{
    struct stat buf;
    int fd = -1;
    APP_HEADER_S pstHead;
    
    if(NULL == path)
    {
        DBG_PRINTF("Invalid path!");
        return APP_CHECK_ERROR_PARA;
    }
    
    memset(&pstHead, 0x0, sizeof(APP_HEADER_S));
    if (stat(path, &buf) < 0)
    {
        DBG_PRINTF("Get file %s property error!",path);
        return APP_CHECK_ERROR;
    }
    else if (buf.st_size > 0)
    {
        if((fd = open(path, O_RDONLY)) < 0)
        {
            DBG_PRINTF("Could not open file : %s", path);
            return APP_CHECK_ERROR;
        }

        (void)read(fd, &pstHead, sizeof(APP_HEADER_S));
        if(!strstr(pstHead.szRelease, PRODUCT_NAME))
        {
            DBG_PRINTF("App release version check error!");
            close(fd);
            return APP_CHECK_ERROR;
        }
        close(fd);
        return APP_CHECK_OK;
    }
    return APP_CHECK_ERROR;
}
#endif

uint8_t *ufile_read(const char *path, uint32_t *read_size)
{
	FILE* fp;
	uint8_t *pbuf;
	struct stat fs;
	if (stat(path, &fs) < 0){
		return NULL;
	}else if (fs.st_size == 0){
		return NULL;
	}

	fp = fopen(path, "r");
	if (fp == NULL){
		return NULL;
	}

	pbuf = (uint8_t *)malloc(fs.st_size);
	if (pbuf == NULL){
		fclose(fp);
		return NULL;
	}

	if (fread(pbuf, sizeof(uint8_t), fs.st_size, fp) !=  fs.st_size){
		free(pbuf);
		fclose(fp);
		return NULL;
	}

	if (read_size){
		*read_size = fs.st_size;
	}

    fclose(fp);
	return pbuf;
}
/*****************************************************************
Function:upgrade_content_read_from_app
Description:process app upgrade   
Author:liaohongjun
Date:2012/10/30
INPUT:const char *path,  int read_type       
OUTPUT:uint32_t *read_size     
Return:
    NULL
    pbuf 
Others:        
*****************************************************************/
uint8_t *upgrade_content_read_from_app(const char *path, uint32_t *read_size, int read_type)
{
	FILE* fp;
	uint8_t *pbuf;
	struct stat fs;
    APP_FMW_HEADER_S stFmwHeader;
    
    if((NULL == path) || (NULL == read_size))
    {
        return NULL;
    }
    
	if (stat(path, &fs) < 0)
    {
		return NULL;
	}
    else if (fs.st_size == 0)
	{
		return NULL;
	}

	fp = fopen(path, "r");
	if (fp == NULL)
    {
		return NULL;
	}
    
    if(-1 == fseek(fp,0,SEEK_SET))
    {
        fclose(fp);
        return NULL;
    }
    
    /*Begin modify by zhouguanhua 2013/7/2*/
    #if 1
        DBG_PRINTF("without APP_HEADER\n");
    #else
    if(-1 == fseek(fp, sizeof(APP_HEADER_S),SEEK_SET))
    {
        fclose(fp);
        return NULL;
    }
    #endif

    memset(&stFmwHeader, 0x0, sizeof(APP_FMW_HEADER_S));
	if (fread(&stFmwHeader, sizeof(uint8_t), sizeof(APP_FMW_HEADER_S), fp) !=  sizeof(APP_FMW_HEADER_S))
    {
		fclose(fp);
		return NULL;
	}     
    if(-1 == fseek(fp,0,SEEK_SET))
    {
        fclose(fp);
        return NULL;
    }
    
    switch(read_type)
    {
        case READ_CONTENT_OF_KERNEL:
            *read_size = ntohl(stFmwHeader.kernellen) + sizeof(APP_FMW_HEADER_S);
            pbuf = (uint8_t *)malloc(*read_size); 
        	if (pbuf == NULL)
            {
        		fclose(fp);
        		return NULL;
        	}             
            #if 1
                DBG_PRINTF("without APP_HEADER\n");     
            #else
            if(-1 == fseek(fp, sizeof(APP_HEADER_S),SEEK_SET))
            {
                free(pbuf);
                fclose(fp);
                return NULL;
            }            
            #endif
            if (fread(pbuf, sizeof(uint8_t), *read_size, fp) !=  *read_size)
            {
                free(pbuf);
                fclose(fp);
                return NULL;
            } 
            break;
        case READ_CONTENT_OF_ROOTFS:
            *read_size = ntohl(stFmwHeader.fslen); 
            pbuf = (uint8_t *)malloc(*read_size); 
        	if (pbuf == NULL)
            {
        		fclose(fp);
        		return NULL;
        	}             
        	#if 1
                if(-1 == fseek(fp,(sizeof(APP_FMW_HEADER_S) + ntohl(stFmwHeader.kernellen)),SEEK_SET))
                {
                    free(pbuf);                
                    fclose(fp);
                    return NULL;
                }  
            #else
            if(-1 == fseek(fp,(sizeof(APP_HEADER_S) + sizeof(APP_FMW_HEADER_S) + ntohl(stFmwHeader.kernellen)),SEEK_SET))
            {
                free(pbuf);                
                fclose(fp);
                return NULL;
            }  
            #endif
            if (fread(pbuf, sizeof(uint8_t), *read_size, fp) !=  *read_size)
            {
                free(pbuf);
                fclose(fp);
                return NULL;
            }    
            break;                
        default:
            fclose(fp);                
            return NULL;
    }

    fclose(fp);
    return pbuf;
}



static int ufile_output(const char *path, const uint8_t *buffer, uint32_t size)
{
	FILE * fp;
	int ret = UP_OK;
    size_t locWrite, numLeft, numWrite = 0;
	char pathtmp[128];

	sprintf(pathtmp, "%s.tmp", path);
		

    if ((fp = fopen(pathtmp, "w+b")) == NULL) {
		DBG_PRINTF("File[%s] open failed!", pathtmp);
		return UP_ERR_IO;
    } 
	DBG_PRINTF("File[%s] open success!", pathtmp);

    locWrite = 0;
    numLeft = size;
    while (numLeft > 0) {
        numWrite = fwrite(&buffer[locWrite], sizeof(*buffer), numLeft, fp);
        if (numWrite < numLeft) {
        	DBG_PRINTF("File write failed. ferror=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes", ferror(fp), locWrite, numLeft, numWrite, size);
			ret = UP_ERR_IO;
       		break;
        }
        locWrite += numWrite;
        numLeft -= numWrite;
    }

    if (numLeft == 0) {
        if (fclose(fp) != 0) {
            DBG_PRINTF("File close failed.errno=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes<br>", errno, locWrite, numLeft, numWrite, size);			
			ret = UP_ERR_IO;	
        } else {
            DBG_PRINTF("File Size Written = %d bytes", size);
			ret = UP_OK;
        }
    } else {
        DBG_PRINTF("numLeft=%d locWrite=%d Size=%d bytes", numLeft, locWrite, size);
    }
	if (rename(pathtmp, path) < 0){
		perror("rename");
		DBG_ASSERT(0, "Failed to rename %s to %s", pathtmp, path);
	}	

	if (chmod(path, 0755) < 0){
		perror("chmod");
		DBG_ASSERT(0, "Failed to chmod %s ", path);
	}	

	return ret;
}



static int ufile_process_boot(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process boot (len=%u,to=%s)", length, to ? to : "");	
	return ufile_output(UFILE_TMP_BOOT, buffer, length);	
}


static int ufile_process_rootfs(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process rootfs (len=%u,to=%s)", length, to ? to : "");	

	return ufile_output(UFILE_TMP_ROOTFS, buffer, length);		
}


static int ufile_process_kernel(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process kernel (len=%u,to=%s)", length, to ? to : "");	

	return ufile_output(UFILE_TMP_KERNEL, buffer, length);	
}


static int ufile_process_file(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process file (len=%u,to=%s)", length, to ? to : "");	
	if (to == NULL){
		return UP_ERR_NONAME;
	}
	return ufile_output(to, buffer, length);
}


static int ufile_process_config(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process config file (len=%u,to=%s)", length, to ? to : "");

	return ufile_output("/tmp/config.bin", buffer, length);

}



/*Begin modify by jiangzhenhua 2012/7/23*/
#if 0
int ufile_validate(const uint8_t *buffer, uint32_t length)
{
	uint32_t chksum, chksum_c;
	ufile_header_t *hdr;
	if (length < sizeof (ufile_header_t)){
		DBG_PRINTF("Invalid length :%u", length);
		return UP_ERR_LENGTH;
	}

	hdr = (ufile_header_t *)buffer;

	if (strcmp((char *)hdr->identify, UFILE_IDENTIFY)){
		DBG_PRINTF("Invalid Identify!");
		return UP_ERR_IDENTIFY;
	}

	if (ntohl(hdr->total_length) != length)
	{
		DBG_PRINTF("Invalid length : (in %u :hdr %u)", length, ntohl(hdr->total_length));
		return UP_ERR_LENGTH;
	}

	if (hdr->file_num == 0)
	{
		DBG_PRINTF("Invalid file num : %d", hdr->file_num);
		return UP_ERR_LENGTH;
	}
	
	chksum = ntohl(hdr->checksum);
	hdr->checksum = 0;
	chksum_c = checksum_32(buffer, length, 0);
	if (chksum_c !=  chksum){
		DBG_PRINTF("Invalid CHKSUM[%08X:%08X]", chksum, chksum_c);
		return UP_ERR_CHKSUM;
	}

	return UP_OK;
}
#endif

int ufile_validate(const uint8_t *buffer, uint32_t length)
{
	//uint32_t chksum, chksum_c;
	APP_HEADER_S *hdr;
	unsigned short usCrc;
	
	if (length < sizeof(APP_HEADER_S)){
		DBG_PRINTF("Invalid length :%u", length);
		return UP_ERR_LENGTH;
	}
	/*Begin add by jiangzhenhua for EPN104QID0056 2012/9/25*/
	if(length*10 > MAX_UPLOAD_FILE_LEN)
	{
		DBG_PRINTF("spuer length :%u", length);
		return UP_ERR_LENGTH;
	}
	/*End add by jiangzhenhua for EPN104QID0056  2012/9/25*/
	hdr = (APP_HEADER_S *)buffer;
    /*Begin modified by liaohongjun for EPN204QID0001 2012/11/19*/
	if(!strstr(hdr->szRelease, PRODUCT_NAME))
	{
        /*End modified by liaohongjun for EPN204QID0001  2012/11/19*/
		DBG_PRINTF("Invalid Identify!");
		return UP_ERR_IDENTIFY;
	}
	usCrc = crc_calc_direct(0, (short *)(buffer+4), length - 4);
	if(usCrc != hdr->usFileCRC)
	{
		DBG_PRINTF("Invalid CHKSUM[%08X:%08X]", usCrc, hdr->usFileCRC);
		return UP_ERR_CHKSUM;
	}
		
	return UP_OK;
	
}
/*Added by sunmingliang for EPN104N & 5500 upfile check*/
int fw_head_check(const uint8_t *buffer, uint32_t length)
{     
    APP_FMW_HEADER_S *pstHead = (APP_FMW_HEADER_S *)buffer;
	unsigned int count = 0;
	unsigned int tmp_addr = buffer + FMW_HEADLEN;
	unsigned int checksum = 0;
	unsigned int checksum_tmp = 0;
	unsigned int checksum_tmp_i = 0;
	unsigned int *tmp_ptr;

	
	if((length != pstHead->imgsize) || (FMW_HEADLEN != pstHead->headlen))
	{
		DBG_PRINTF("Invalid length :%u\n", length);
		return UP_ERR_LENGTH;
	}

	if(0 != strcmp(pstHead->product_version_str, CONFIG_PRODUCT_NAME))
	{
		DBG_PRINTF("check product name error:%s\n", pstHead->product_version_str);
		return UP_ERR_NONAME;
	}
	
	if(length*10 > MAX_UPLOAD_FILE_LEN)
	{
		DBG_PRINTF("super length :%u\n", length);
		return UP_ERR_LENGTH;
	}

	/*计算数据的checksum*/
	while(count < (pstHead->imgsize - FMW_HEADLEN))
	{		
		checksum_tmp = (checksum_tmp<<8)|((unsigned int)(*(unsigned char *)tmp_addr++));
		checksum_tmp_i++;
		if (checksum_tmp_i >=4)
		{
    		checksum += checksum_tmp;
    		checksum_tmp_i = 0;
	    }
		count++;		
	}

	/*计算头的checksum, 除了成员pstHead->checksum*/
	count = FMW_HEADLEN;
	tmp_ptr=(unsigned int *)pstHead;		
	while(count)
	{
		checksum += *tmp_ptr;
		tmp_ptr++;
		count -=4;
	}
	checksum -= pstHead->checksum;
	
	if(pstHead->checksum != (~checksum))
    {
        DBG_PRINTF("check FileCRC error!");
        return UP_ERR_CHKSUM;
    }
	
    return UP_OK;    
}



/*Begin add by zhouguanhua 2013-05-6 for bug id 36*/
int GetHardVersion(char *hr_version, int hr_version_size)
{
	int fp;
	char hard_version_tmp[MAX_BUILDTIME_SIZE];
    char *delim=NULL;
    if(hr_version ==NULL || hr_version_size < sizeof(hard_version_tmp))
    {
        return -1;    
    }
	memset(hard_version_tmp,0,sizeof(hard_version_tmp));
	if((fp = open(UFILE_HARD_VERSION, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", UFILE_HARD_VERSION);
		return -1;
	}
    if(read(fp, hard_version_tmp, MAX_BUILDTIME_SIZE) < 0)
    {
    	DBG_PRINTF("File [%s] read failed!", UFILE_HARD_VERSION);
    	close(fp);
		return -1;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", UFILE_HARD_VERSION);
		return -1;
    } 
    delim="\n";
    strtok(hard_version_tmp,delim);//if has  \n,instead of \0       
    strcpy(hr_version,hard_version_tmp);
    return 0;    
}
int GetBootVersion(char *boot_version,int boot_version_size)
{
	int fp;
	char boot_version_tmp[MAX_BUILDTIME_SIZE];
    char *delim=NULL;
    if(boot_version ==NULL || boot_version_size < sizeof(boot_version_tmp))
    {
        return -1;    
    }
	memset(boot_version_tmp,0,sizeof(boot_version_tmp));
	if((fp = open(UFILE_BOOTVERSION, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", UFILE_BOOTVERSION);
		return -1;
	}
    if(read(fp, boot_version_tmp, MAX_BUILDTIME_SIZE) < 0)
    {
    	DBG_PRINTF("File [%s] read failed!", UFILE_BOOTVERSION);
    	close(fp);
		return -1;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", UFILE_BOOTVERSION);
		return -1;
    } 
    delim="\n";
    strtok(boot_version_tmp,delim);//if has  \n,instead of \0    
    strcpy(boot_version,boot_version_tmp);
    return 0;    
}
/*End add by zhouguanhua 2013-05-6 for bug id 36*/
/*Add by zhouguanhua 2013/5/3 for bug id 36*/
int read_kernel_version(char *kernel_version,char *kernel_build_time)
{
    #define BUFFER_SIZE 300
    FILE *fd;   
   // pid_t pid = 0;
    char buffer[BUFFER_SIZE]={0};

    char *p=NULL,*version_temp=NULL,*build_time_temp=NULL;
	char *delim=NULL;
    int num1=0,num2=0;
    
    if(kernel_version ==NULL || kernel_build_time ==NULL)
    {
        DBG_PRINTF(" error: read_kernel_version error !");
        return -1;
    }
    memset(buffer,0,sizeof(buffer));
    fd = fopen("/proc/version", "r");
    if (fd != NULL)
    {
        if(fgets(buffer,BUFFER_SIZE-1, fd))
        {
            p=buffer;
            version_temp=kernel_version;
            build_time_temp=strstr(buffer,"#");
            if(build_time_temp !=NULL)
            {                  
			delim="\n";
            strtok(build_time_temp,delim);//if has  \n,instead of \0
                if(strlen(build_time_temp) > MAX_BUILDTIME_SIZE)
                {
                    DBG_PRINTF(" error: kernel_build_time buffer error !");
                    fclose(fd);
                    return -1; 
                }
            strcpy(kernel_build_time,build_time_temp);          			
            }
            else
            {
                DBG_PRINTF(" error: kernel_build_time buffer error !");
                fclose(fd);
                return -1;                
            }
            num2=strcspn(buffer,"(");
            if(num2 > MAX_BUILDTIME_SIZE || num2 <= 0)
            {
                DBG_PRINTF(" error: kernel_version buffer error !");
                fclose(fd);
                return -1;                 
            }
            num1=strlen("Linux version ");
            for(;*p !='\0' ,num1 < num2;num1++,version_temp++)
            {
               *version_temp=*(p+num1);           
            }
        }         
         fclose(fd);
    }
    return 0;
}
/*END add by zhouguanhua 2013/5/3 for bug id 36*/

int get_bootflag(void)
{
	int fp;
//	int  ucount = 1;
	char bootflag[4];
	
	if((fp = open(UFILE_BOOTFLAG, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
	}
    if(2 != read(fp, bootflag, 2))
    {
    	DBG_PRINTF("File [%s] read failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
    }
    /*Begin modify by dengjian 2012-08-29 for QID0013*/
    if('1' == bootflag[0])
    {
    	_ufile_content = UPGRADE_APP_0;
    }
    else
    {
    	_ufile_content = UPGRADE_APP_1;
    }
	/*End modify by dengjian 2012-08-29 for QID0013*/
    return UP_OK;
	
}

/*Begin add by zhouguanhua 2013-5-15 for bug id 52*/
//get factory serial number
int GetSN(char *manucode,int manucode_size)
{
	int fp;
	char manucode_tmp[MAX_SN_SIZE];
    char *delim=NULL;
    if(manucode ==NULL || manucode_size < sizeof(manucode_tmp))
    {
        return -1;    
    }
	memset(manucode_tmp,0,sizeof(manucode_tmp));
	if((fp = open(UFILE_SN, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", UFILE_SN);
		return -1;
	}
    if(read(fp, manucode_tmp, MAX_SN_SIZE) < 0)
    {
    	DBG_PRINTF("File [%s] read failed!", UFILE_SN);
    	close(fp);
		return -1;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", UFILE_SN);
		return -1;
    } 
    delim="\n";
    strtok(manucode_tmp,delim);//if has  \n,instead of \0    
    strcpy(manucode,manucode_tmp);
    return 0;    
}
int GetEthaddr(char * ethaddr,int ethaddr_size)
{
	int fp;
	char ethaddr_tmp[MAX_MAC];
    char *delim=NULL;
    if(ethaddr ==NULL || ethaddr_size < sizeof(ethaddr_tmp))
    {
        return -1;    
    }
	memset(ethaddr_tmp,0,sizeof(ethaddr_tmp));
	if((fp = open(UFILE_ETHADDR, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", UFILE_ETHADDR);
		return -1;
	}
    if(read(fp, ethaddr_tmp, MAX_MAC) < 0)
    {
    	DBG_PRINTF("File [%s] read failed!", UFILE_ETHADDR);
    	close(fp);
		return -1;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", UFILE_ETHADDR);
		return -1;
    } 
    delim="\n";
    strtok(ethaddr_tmp,delim);//if has  \n,instead of \0   
   
    strcpy(ethaddr,ethaddr_tmp);
    return 0;    
}
/*End add by zhouguanhua 2013-5-15 for bug id 52*/


/*****************************************************************
    Function:getFactoryParasByName
    Description:Get Factory Paras By Name
    Author:huangmingjian
    Date:2014-01-08
    Input:char *name, int length   
    Output:   char *buffer,       
    Return:
		0 :OK
		-1:Fail
=================================================
*****************************************************************/

int getFactoryParasByName(char *name, char *buffer, int length)
{
	int fp = 0;
	char tmp[MAX_CONFIG_VAL_SIZE] = {0};
    char *delim=NULL;
	
    if(name == NULL || buffer ==NULL || length <=0)
    {
        return -1;    
    }
	if((fp = open(name, O_RDONLY)) < 0)
	{
		DBG_PRINTF("File [%s] open failed!", name);
		return -1;
	}
    if(read(fp, tmp, length) < 0)
    {
    	DBG_PRINTF("File [%s] read failed!", name);
    	close(fp);
		return -1;
    }
    if(0 != close(fp))
    {
    	DBG_PRINTF("File [%s] close failed!", name);
		return -1;
    } 
    delim="\n";
    strtok(tmp, delim);//if has  \n,instead of \0    
    strcpy(buffer, tmp);
	
    return 0;    
}


/*Add by zhouguanhua 2013/5/15 for bug id 52*/
size_t write_date_file(const char *path, const  char *buffer, size_t size)
{	
	FILE *fp;
	size_t wlen;

	if(!path || !buffer)
	{
		fprintf(stderr, "Paras error!\n");		
		return -1;
	}
	
	fp = fopen(path, "w+");
	if (fp == NULL){
		fprintf(stderr, "Conuld not open file %s\n", path);		
		return -1;
	}
	
	wlen = fwrite(buffer, sizeof( char), size,  fp);
	if (wlen != size){
		fprintf(stderr, "File write failed! (wlen=%u:size=%u)\n", wlen, size);
	}
	
	fclose(fp);
	
	return wlen;
}


/*****************************************************************
    Function:cfgFileValidate
    Description: check config file to be imported
    Author:liaohongjun
    Date:2012/8/1
    Input:const uint8_t *buffer, uint32_t length   
    Output:         
    Return:
        UP_ERR_LENGTH   
        UP_ERR_IDENTIFY         
        UP_ERR_CHKSUM    
        UP_OK 
=================================================
    Note: added by liaohongjun 2012/8/1 of QID0003
*****************************************************************/
int cfgFileValidate(const uint8_t *buffer, uint32_t length)
{
    CONFIG_HEADER_S *hdr;
    unsigned short usCrc = 0;
    
    if (length < sizeof(CONFIG_HEADER_S))
    {
        DBG_PRINTF("Invalid length :%u", length);
        return UP_ERR_LENGTH;
    }

    hdr = (CONFIG_HEADER_S *)(buffer);
    DBG_PRINTF("Identify[hdr->szCfgFileName=%s]!",hdr->szCfgFileName);
    if(!strstr(hdr->szCfgFileName, PRODUCT_NAME))
    {
        DBG_PRINTF("Invalid Identify[hdr->szCfgFileName=%s]!",hdr->szCfgFileName);
        return UP_ERR_IDENTIFY;
    }
    
    usCrc = crc_calc_direct(0, (short *)(buffer + 4), length - 4);
    if(usCrc != hdr->usFileCRC)
    {
        DBG_PRINTF("Invalid CHKSUM[%08X:%08X]", usCrc, hdr->usFileCRC);
        return UP_ERR_CHKSUM;
    }

    return UP_OK;
}

/*****************************************************************
    Function:cfgFileAddHeadAndSave
    Description: add header to the config file and save it
    Author:liaohongjun
    Date:2012/8/1
    Input:const char * path   
    Output:         
    Return:
        UP_FAILED
        UP_ERR_LENGTH   
        UP_ERR_IO   
        UP_OK
=================================================
    Note: added by liaohongjun 2012/8/1 of QID0003
*****************************************************************/
int cfgFileAddHeadAndSave(const char * path)
{
    CONFIG_HEADER_S *pstHead = NULL;
    unsigned char *pFilePtr = NULL; 
    char cfgName[MAX_CFGHEAD_FILENAME_LEN] = {0};
    unsigned short usCrc;
    FILE* fp = NULL;
    FILE* fp1 = NULL;
    size_t numWrite = 0;
    struct stat fs;

    if (stat(path, &fs) < 0)
    {
        DBG_PRINTF("Get file %s property error!",path);
        return UP_FAILED;
    }
    else if(0 == fs.st_size)
    {
        DBG_PRINTF("File %s is NULL!",path);
        return UP_ERR_LENGTH;
    }
    
    fp = fopen(path, "r");
    if (fp == NULL)
    {
        DBG_ASSERT(fp, "Could not open file : %s", path);
        return UP_ERR_IO;
    }

    pFilePtr = (unsigned char *)malloc((fs.st_size+sizeof(CONFIG_HEADER_S)));
    if(fs.st_size != fread((pFilePtr+sizeof(CONFIG_HEADER_S)), sizeof(unsigned char), fs.st_size, fp))
    {
        DBG_PRINTF("Read File %s error!",path);
        fclose(fp);
        free(pFilePtr);
        return UP_ERR_IO;
    }
    fclose(fp);
    
    /*add header to the file*/
    pstHead = (CONFIG_HEADER_S *)(pFilePtr);
    memset(pstHead ,0x0, sizeof(CONFIG_HEADER_S));
    pstHead->tail = 0x0A;
    pstHead->ulFileLength = (unsigned int)(fs.st_size+sizeof(CONFIG_HEADER_S));
    sprintf(cfgName, "%s.config", PRODUCT_NAME);
    strcpy(pstHead->szCfgFileName, cfgName);
    usCrc = crc_calc_direct(0, (short *)(pFilePtr+4), pstHead->ulFileLength - 4);
    pstHead->usFileCRC = (unsigned int)usCrc;
    DBG_PRINTF("pstHead->ulFileLength = %d, pstHead->usFileCRC = %x!",pstHead->ulFileLength,pstHead->usFileCRC);
    
    /*reopen the file and delect all content*/
    fp1 = fopen(path, "w");
    if (fp1 == NULL)
    {
        DBG_ASSERT(fp, "Could not open file : %s", path);
        free(pFilePtr);
        return UP_ERR_IO;
    }    

    /*rewrite the file with header*/
    numWrite = fwrite(pFilePtr, sizeof(unsigned char), pstHead->ulFileLength, fp1);
    DBG_PRINTF("Write %d byte to File %s!",numWrite,path);
    if(pstHead->ulFileLength != (unsigned int)numWrite)
    {
        DBG_PRINTF("Should write %d bytes,but write %d bytes",pstHead->ulFileLength,numWrite);
        fclose(fp1);
        free(pFilePtr);
        return UP_ERR_IO;
    }
    fclose(fp1);
    free(pFilePtr);
    return UP_OK;
}

int ufile_process(const uint8_t *buffer, uint32_t length)
{
	int ret = UP_OK;
	//APP_HEADER_S *hdr;
	uint8_t *p;
	uint32_t flen;
	char to[25];

	if(NULL == buffer)
	{
		return UP_ERR_UNKNOWN;
	}
	
	/*Begin modify by dengjian 2012-11-06 for QID0080*/
#ifdef CONFIG_BOOT_MULTI_APP
	/*Begin modify by dengjian 2012-08-29 for QID0013*/
	if(UP_OK != get_bootflag())
	{
		return UP_ERR_IO;
	}
#else
    _ufile_content = UPGRADE_APP_0;
#endif
	/*End modify by dengjian 2012-11-06 for QID0080*/
	//_ufile_content = 1;
	/*End modify by dengjian 2012-08-29 for QID0013*/
	//hdr = (APP_HEADER_S *)buffer;
	p = buffer;
	
	/*Begin modify by zhouguanhua 2013-4-16 for BugID0005*/
	sprintf(to, "/tmp/%s", APP_UPGRADE_FILE);
	/*End modify by zhouguanhua 2013-4-16 for   BugID0005*/
	flen = length;
	ret = ufile_process_file(p, flen, to);
	
	if (ret != UP_OK)
	{
		return ret;
	}
	
	return UP_OK;
}

#if 0
int ufile_process(const uint8_t *buffer, uint32_t length)
{
	int i, num, ret = UP_OK;
	ufile_header_t *hdr;
	uint32_t flen;
	ufile_data_hdr_t *fhdr;
	uint8_t *p;
	char *to;
	
	hdr = (ufile_header_t *)buffer;
	num = hdr->file_num;

	_ufile_content = 0;

	fhdr = (ufile_data_hdr_t *)(buffer + ntohs(hdr->data_offset));
	for (i = 0; i < num; i ++){
		p  = (uint8_t *)fhdr;
		flen = ntohl(fhdr->length) - ntohs(fhdr->data_offset);
		p += ntohs(fhdr->data_offset);
		if (fhdr->has_file_name){
			to = fhdr->file_name;
		}else {
			to = NULL;
		}
		_ufile_content |= ntohs(fhdr->content);
		
		switch(ntohs(fhdr->content)){
			case UFILE_CONTENT_BOOT:
				ret = ufile_process_boot(p, flen, to);
				break;
			case UFILE_CONTENT_ROOTFS:
				ret = ufile_process_rootfs(p, flen, to);
				break;
			case UFILE_CONTENT_KERNEL:
				ret = ufile_process_kernel(p, flen, to);
				break;
			case UFILE_CONTENT_FILE:
				ret = ufile_process_file(p, flen, to);
				break;
			case UFILE_CONTENT_CONFIG:
				ret = ufile_process_config(p, flen, to);
				break;
			default:
				ret = UP_ERR_UNKNOWN;
		}
		if (ret != UP_OK){
			return ret;
		}
		fhdr =(ufile_data_hdr_t *)(p + flen);
	}

	return ret;
}
#endif
/*End modify by jiangzhenhua 2012/7/23*/

int ufile_output_config(const uint8_t *buffer, uint32_t length)
{
    char cfgPath[64] = {0};
	DBG_PRINTF("Upgrade process config file (len=%u)", length);
    
   // sprintf(cfgPath, "%s%s.config", UFILE_TMP_PATH, PRODUCT_NAME);
    sprintf(cfgPath, "%sconfig.bin", UFILE_TMP_PATH);
	return ufile_output(cfgPath, buffer, length);

}


uint16_t ufile_content(void)
{
	return _ufile_content;
}





#ifndef APP_HEAD_H
#define APP_HEAD_H


#define ADDI_HEADLEN 64

struct addi_hdr_t
{
	unsigned char str_mark[ADDI_HEADLEN-4];
	unsigned int imgsize;
} __attribute__ ((aligned(1), packed));



#define PRODUCT_VER_STR_LEN_MAX 36
#define FMW_HEADLEN 0x60

struct fmw_hdr_t
{
	unsigned int headlen;   // 文件头的长度
	unsigned int imgsize;   // 整个文件大小，包含头
	unsigned int checksum;  // 校验值，包含头，算法：4字节相加，结果为0。
	unsigned int timestamp; // 升级文件制作的时间
	unsigned int kernellen; // 内核的长度
	unsigned int fslen;     // 文件系统的长度
			        // 因此文件必须4字节对齐，如果不对齐，则用0填充。
	
	unsigned char _model; // 产品类型 ER5200
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
	
} __attribute__ ((aligned(1), packed));


#define KERNEL_TYPE (0x01)
#define FS_TYPE     (0x02)
#define NULL_TYPE   (0xff)

#endif


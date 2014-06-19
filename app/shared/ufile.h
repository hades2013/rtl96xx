
#ifndef __UFILE_H__
#define  __UFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


#ifndef __packed
#define	__packed	__attribute__((__packed__))
#endif



#define TARGET_TYPE_EOC       1
#define TARGET_MODEL_CLT502   1




#define UFILE_IDENTIFY_LEN 16 
#define MAX_UFILE_NAME_SIZE  128

#define UFILE_CONTENT_BOOT 	0x01 
#define UFILE_CONTENT_KERNEL 	0x02
#define UFILE_CONTENT_ROOTFS 	0x04
#define UFILE_CONTENT_FILE 	0x08
#define UFILE_CONTENT_CONFIG  0x10



typedef struct __packed {
	uint8_t identify[UFILE_IDENTIFY_LEN];// 16byte
	uint32_t total_length;// little endian 4
	uint32_t checksum;  // 4
	uint16_t data_offset;	
	uint16_t content;  
	uint8_t flags;
	uint8_t file_num;	
	uint8_t target_type;
	uint8_t target_model;
	uint8_t reserved[32];
}
ufile_header_t;


typedef struct __packed{
	uint16_t content;
	uint16_t data_offset;
	uint32_t length;// include this header
	uint8_t flags;	
	uint8_t reserved;	
	uint8_t file_type;		
	uint8_t has_file_name;
	char file_name[MAX_UFILE_NAME_SIZE];	
}
ufile_data_hdr_t;



#endif /* __UFILE_H__*/


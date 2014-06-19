
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "debug.h"
#include "upgrade.h"


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



static int ufile_output(const char *path, const uint8_t *buffer, uint32_t size)
{
	FILE * fp;
	int ret = UP_OK;
    size_t locWrite, numLeft, numWrite;
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
	DBG_PRINTF("Upgrade process kernel (len=%lu,to=%s)", length, to ? to : "");	
	return ufile_output("/tmp/boot.bin", buffer, length);	
}


static int ufile_process_rootfs(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process kernel (len=%lu,to=%s)", length, to ? to : "");	

	return ufile_output("/tmp/rootfs.bin", buffer, length);		
}


static int ufile_process_kernel(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process kernel (len=%lu,to=%s)", length, to ? to : "");	

	return ufile_output("/tmp/kernel.bin", buffer, length);	
}


static int ufile_process_file(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process file (len=%lu,to=%s)", length, to ? to : "");	
	if (to == NULL){
		return UP_ERR_NONAME;
	}
	return ufile_output(to, buffer, length);
}


static int ufile_process_config(const uint8_t *buffer, uint32_t length, const char *to)
{
	DBG_PRINTF("Upgrade process config file (len=%lu,to=%s)", length, to ? to : "");

	return ufile_output("/tmp/config.bin", buffer, length);

}





int ufile_validate(const uint8_t *buffer, uint32_t length)
{
	uint32_t chksum, chksum_c;
	ufile_header_t *hdr;
	if (length < sizeof (ufile_header_t)){
		DBG_PRINTF("Invalid length :%u", length);
		return UP_ERR_LENGTH;
	}

	hdr = (ufile_header_t *)buffer;

	if (strcmp(hdr->identify, UFILE_IDENTIFY)){
		DBG_PRINTF("Invalid Identify!");
		return UP_ERR_IDENTIFY;
	}

	if (ntohl(hdr->total_length) != length)
	{
		DBG_PRINTF("Invalid length : (in %lu :hdr %lu)", length, ntohl(hdr->total_length));
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


int ufile_process(const uint8_t *buffer, uint32_t length)
{
	int i, num, ret;
	ufile_header_t *hdr;
	uint32_t flen;
	ufile_data_hdr_t *fhdr;
	uint8_t *p;
	char *to;

	ipc_system_ack_t *pack;
	
	hdr = (ufile_header_t *)buffer;
	num = hdr->file_num;

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

	// tell master upgrade files are ready

	pack = ipc_system_req(ipc_fd, IPC_SYS_UPGRADE_READY);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)){
		ret = UP_OK;
	}else {
		ret = UP_ERR_IPC;
	}

	if (pack) free(pack);

	return ret;
}





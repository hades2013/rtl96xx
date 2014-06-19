
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <string.h>
#include <errno.h>
#include <mtd/mtd-user.h>

#include "utils.h"
#include "timer.h"
#include "master.h"
#include "mtd_utils.h"


#if 0

#ifndef SIZE_BUFFER 
#define SIZE_BUFFER 4096
#endif 


int erase_mtd(char* path_mtd, int* size)
{
	int fd;
	mtd_info_t meminfo;
	erase_info_t erase;

	if ((fd = open(path_mtd, O_RDWR)) < 0) {
		printf("Error opening mtd file %s\n", path_mtd);
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		printf("Unable to get MTD device info %s\n", path_mtd);
		return -1;
	}

	erase.length = meminfo.erasesize;
	*size = meminfo.size;

	for (erase.start = 0; erase.start < meminfo.size; erase.start += meminfo.erasesize)
	{
		if (ioctl(fd, MEMERASE, &erase) != 0) {
			printf("MTD Erase failure: %s\n", path_mtd);
			return -1;
		}
		printf("\rErasing %d Kibyte @ %x -- %2llu %% complete.",
		     meminfo.erasesize / 1024, erase.start,
		     (unsigned long long)
		     erase.start * 100 / meminfo.size);	
		fflush(stdout);		

		#if 0
		if (lseek (fd, erase.start, SEEK_SET) < 0) {
			printf("MTD lseek failure: %s\n", path_mtd);
			return -1;
		}
		if (write (fd , &cleanmarker, sizeof (cleanmarker)) != sizeof (cleanmarker)) {
			printf("MTD write failure: %s\n", path_mtd);
			return -1;
		}
		#endif
	}

	return 0;
}

int write_mtd(char* path_mtd, int fd, int offset, int size)
{
	int written_total = 0, written = 0, readd = 1;
	int fmtd = 0;
	char buffer[SIZE_BUFFER];

	/* Open mtd */
	if ((fmtd = open(path_mtd, O_WRONLY)) < 0)
	{
		printf("Error opening mtd file %s\n", path_mtd);
		return -1;
	}

	/* Position offset in the image file */
	lseek(fd, offset, SEEK_SET);

	/* Write to the mtd file blocks of SIZE_BUFFER */
	while (written_total < size)
	{
		memset(buffer, 0xFF, SIZE_BUFFER);

		/* Read image file */
		if ((readd = read(fd, buffer, SIZE_BUFFER)) < 0)
		{
			printf("Error reading the image file\n");
			return -1;
		}

		/* Write! */
		if ((written = write(fmtd, buffer, SIZE_BUFFER)) != SIZE_BUFFER)
		{
			printf("Error writing the image to the mtd %s, written a total of %d bytes\n", path_mtd, written_total);
			return -1;
		}

		written_total += SIZE_BUFFER;

		if (readd != SIZE_BUFFER)
		{
			printf("End of input file while writing to the mtd %s, written a total of %d bytes\n", path_mtd, written_total);
			return 0;
		}
	}

	printf("Erasing and writting to the mtd %s successfull, written %d bytes, offset used %d\n", path_mtd, written_total, offset);

	return 0;
}




int main(void)
{
	uint32_t size;
	erase_mtd("/dev/mtd2", &size);
	exit(0);
}
#endif 


typedef struct {
	char path[128];
	int fd;
	unsigned char *buffer;
	uint32_t buffer_size;
	uint32_t buffer_wrote;
	uint32_t erase_size;
	uint32_t mtd_size;
	erase_info_t erase;
	int error;
	int state;
	uint32_t chksum_in;
	uint32_t chksum_out;
	mtd_on_state_change_t on_state_change;
}
mtd_op_t;
/*begin modified by liaohongjun 2012/7/11*/
char * mtd_get_mtdblock(char *mtd)
{
    #if 0
    if(NULL == mtd)
    {
        return "";
    }

    if (!strcmp(mtd, MTD_APP_0))
    {
        return MTDBLOCK_APP_0;
    }
    else if (!strcmp(mtd, MTD_APP_1))
    {
        return MTDBLOCK_APP_1;
    } 
    #endif
    return ""; 
}

#if 0
char * mtd_get_mtdblock(char *mtd)
{
    if (!strcmp(mtd, MTD_BOOT))
    {
        return MTDBLOCK_BOOT;
    }
    else if (!strcmp(mtd, MTD_KERNEL))
    {
        return MTDBLOCK_KERNEL;
    }
    else if (!strcmp(mtd, MTD_ROOTFS))
    {
        return MTDBLOCK_ROOTFS;
    }
    else if (!strcmp(mtd, MTD_APP_0))
    {
        return MTDBLOCK_APP_0;
    }
    else if (!strcmp(mtd, MTD_APP_1))
    {
        return MTDBLOCK_APP_1;
    } 

    return "";
}
#endif
/*end modified by liaohongjun 2012/7/10*/

static int timer_handle_mtd_erase_write(timer_element_t *timer, mtd_op_t *op)
{
	int fd = op->fd;
	int erase_block_per_timer = 8; // 0 means all
	int write_block_per_timer = 8; // 0 means all
	int write_block_size = 64*1024;
	int written;
	int read_size;
	int cnt, top;
	unsigned long process;

	cnt = 0;
	if (op->state == MTD_ERASING){ // flash erase
		top = erase_block_per_timer;
		for ( ; ((top == 0) || (cnt < top)) && (op->erase.start < op->mtd_size); op->erase.start += op->erase_size){
			if (ioctl(fd, MEMERASE, &op->erase) != 0) {
			//if (0) {				
				fprintf(stderr,"MTD Erase failure: %s\n", op->path);
				op->error = MTD_OP_IOERR;
				return TIMER_RUN_ONCE;
			}

			if (op->on_state_change){
				process = (unsigned long)((unsigned long long)op->erase.start * 100 / op->mtd_size);
				op->on_state_change(op->state, op->error, process);
			}	
			
			fprintf(stderr,"\rErasing %d Kibyte @ %x -- %2llu %% complete.",
			     op->erase_size / 1024, op->erase.start, 
			     (unsigned long long)op->erase.start * 100 / op->mtd_size);				
			
			fflush(stderr);	
			cnt ++;
		}
		if (op->erase.start >= op->mtd_size){
			
			fprintf(stderr,"\rErasing %d Kibyte @ %x -- %2llu %% complete.",
			     op->erase_size / 1024, op->erase.start, 
			     (unsigned long long)op->erase.start * 100 / op->mtd_size);	

			
			fprintf(stderr,"MTD Erase done: %s\n", op->path);
			op->state = MTD_WRITING;
			op->buffer_wrote = 0;

			// update path to mtdblock
			#if 0
			strcpy(op->path, mtd_get_mtdblock(op->path));
			
			if ((op->fd = open(op->path, O_WRONLY | O_SYNC)) < 0)
			{
				fprintf(stderr,"Error opening mtd file %s\n", op->path);
				op->error = MTD_OP_IOERR;
				return TIMER_RUN_ONCE;
			}
			#endif
			if (op->on_state_change){
				op->on_state_change(op->state, op->error, 0);
			}			
		}
		return TIMER_RUN_FOREVER;
	}else if(op->state == MTD_WRITING){ // flash write

		top = write_block_per_timer;
		for ( ; ((top == 0) || (cnt < top)) && (op->buffer_wrote < op->buffer_size); op->buffer_wrote += written){
			if ((op->buffer_size - op->buffer_wrote) < write_block_size){
				write_block_size = op->buffer_size - op->buffer_wrote;
			}			
			if ((written = write(op->fd, &op->buffer[op->buffer_wrote], write_block_size)) != write_block_size)
			{
				fprintf(stderr, "Error writing the image to the mtd %s, written a total of %d bytes\n", op->path, op->buffer_wrote);
				op->error = MTD_OP_IOERR;				
				return TIMER_RUN_ONCE;
			}
			if (op->on_state_change){
				process = (unsigned long)((unsigned long long)op->buffer_wrote * 100 / op->buffer_size);				
				op->on_state_change(op->state, op->error, process);
			}
			fprintf(stderr, "\rWriting %d byte @ %x -- %2llu %% complete.",
				 write_block_size, op->buffer_wrote,
				 (unsigned long long)op->buffer_wrote * 100 / op->buffer_size); 
			fflush(stderr); 
			cnt ++;
		}
		if (op->buffer_wrote >= op->buffer_size ){
			
			fprintf(stderr, "\rWriting %d byte @ %x -- %2llu %% complete.",
				 write_block_size, op->buffer_wrote,
				 (unsigned long long)op->buffer_wrote * 100 / op->buffer_size); 
			fflush(stderr); 
			
			fprintf(stderr, "MTD Write done: %s\n", op->path);
			close(op->fd); 
			op->state = MTD_VERIFING;	
			if (op->on_state_change){
				op->on_state_change(op->state, op->error, 0);
			}				
		}
		return TIMER_RUN_FOREVER;	
	}else if (op->state == MTD_VERIFING){
	
			if ((op->fd = open(op->path, O_RDONLY)) < 0)
			{
				fprintf(stderr,"Error opening mtd file %s\n", op->path);
				op->error = MTD_OP_IOERR;
				return TIMER_RUN_ONCE;
			}	
			if ((read_size = read(op->fd, op->buffer, op->buffer_size)) < 0){
				fprintf(stderr,"Error reading mtd file %s\n", op->path);
				op->error = MTD_OP_IOERR;
				return TIMER_RUN_ONCE;				
			}		
			
			close(op->fd);
			
			if (read_size != op->buffer_size){
				fprintf(stderr,"Verify %s : length unmatch[%d:%d]\n", op->path, op->buffer_size, read_size);
				op->error = MTD_OP_VERIFY_LEN;
				return TIMER_RUN_ONCE;					
			}
			op->chksum_out = checksum_32(op->buffer, read_size, 0);
			if (op->chksum_in != op->chksum_out){
				fprintf(stderr,"Verify %s : CRC unmatch[%08X:%08X]\n", op->path, op->chksum_in, op->chksum_out);
				op->error = MTD_OP_VERIFY_CRC;
				return TIMER_RUN_ONCE;									
			}
			op->error = MTD_OP_OK;
			return TIMER_RUN_ONCE;
	}else {
		op->error = MTD_OP_UNKNOWN;				
		return TIMER_RUN_ONCE;
	}
}

static void timer_handle_on_exit(timer_element_t *timer)
{
	mtd_op_t *op = timer->data;
	
//	DBG_PRINTF("timer exit");
	if (op){
		if (op->on_state_change){
			op->on_state_change(MTD_DONE, op->error, 0);
		}
		free(op);
	}	
}



int mtd_write_start(const char *mtd_path, unsigned char *buffer, uint32_t size, mtd_on_state_change_t on_state_change)
{
	int fd;
	mtd_info_t meminfo;

	mtd_op_t *op = (mtd_op_t*)malloc(sizeof(mtd_op_t));
	if (op == NULL){
		DBG_ASSERT(op, "Malloc");
		return MTD_OP_NOMEM;
	}
	memset(op, 0, sizeof(mtd_op_t));

	strcpy(op->path, mtd_path);
	op->buffer = buffer;
	op->buffer_size = size;

	if ((fd = open(op->path, O_RDWR)) < 0) {
		DBG_PRINTF("Error opening mtd file %s\n", op->path);
		return MTD_OP_IOERR;
	}

	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		DBG_PRINTF("Unable to get MTD device info %s\n", op->path);
		return MTD_OP_IOERR;
	}

//	strcpy(op->path, "/tmp/rootfs.js");

	op->fd = fd;
	op->erase_size = meminfo.erasesize;
	op->mtd_size = meminfo.size;
	op->erase.length = meminfo.erasesize;
	op->erase.start = 0;
	op->state = MTD_ERASING;
	op->error = 0;
	op->on_state_change = on_state_change;
	op->chksum_in = checksum_32(buffer, size, 0);
	// enter erasing state
	if (op->on_state_change){
		op->on_state_change(op->state, op->error, 0);
	}	
	timer_register(100, 0, (timer_func_t)timer_handle_mtd_erase_write,
		(void *)op, (timer_exit_t)timer_handle_on_exit, "MTDWRITE");
	
	return MTD_OP_OK;
}






